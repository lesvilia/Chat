#include "stdafx.h"
#include "FileTransferManager.h"

#include <fstream>
#include "ace/Message_Block.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "FileServerSettingsHolder.h"
#include "IProgressUIObserver.h"
#include "MessagesServer.h"
#include "SettingsManager.h"
#include "MessagesTemplates.h"
#include "LoginManager.h"
#include "NetUsersManager.h"
#include "TCPMessagesServer.h"
#include "FileTransferHelpers.h"
#include "boost/format.hpp"

namespace
{
  const size_t MTU_SIZE = 1400;
  const ACE_Time_Value TIMEOUT(2);
  const unsigned short TEMP_PORT = 0;

  size_t GetFileLength(std::ifstream& file)
  {
    file.seekg (0, file.end);
    size_t length = static_cast<size_t>(file.tellg());
    file.seekg (0, file.beg);
    return length;
  }

  std::wstring GetFileName(const std::wstring& filePath)
  {
    size_t position = filePath.rfind(L"\\");
    if (position != std::wstring::npos)
    {
      return filePath.substr(position, filePath.size() - position);
    }
    return std::wstring();
  }

  void MessageDeleter(ACE_Message_Block* block)
  {
    block->release();
  }

  void DeleteStream(ACE_SOCK_Stream* stream)
  {
    stream->close();
    delete stream;
  }
}

namespace msg
{
  FileTransferManager* FileTransferManager::Instance()
  {
    static FileTransferManager manager;
    return &manager;
  }

  FileTransferManager::FileTransferManager()
    : m_settingsHolder(new FileServerSettingsHolder(sm::SettingsManager::Instance()))
    , m_activated(false)
  {
  }

  FileTransferManager::~FileTransferManager()
  {
  }

  void FileTransferManager::SendFile(const std::wstring& uuid, const std::wstring& filePath,
                                     ui::IProgressUIObserver* observer)
  {
    if (net::NetUsersManager::Instance()->IsUserExist(uuid))
    {
      FileInfoPtr fileInfo(GetFileInfo(filePath));
      if (fileInfo)
      {
        std::wstring header(MakeMessageHeader(fileInfo));
        std::wstring addr(net::NetUsersManager::Instance()->GetNetUserAddress(uuid));
        ACE_INET_Addr userAddr(m_settingsHolder->GetPort(), addr.c_str());
        RunAsyncSender(userAddr, filePath, fileInfo->m_size, header, observer);
      }
    }
  }

  void FileTransferManager::SendFileImpl(const ACE_INET_Addr& userAddr, const std::wstring& filePath, size_t size,
                                         const std::wstring& header, ui::IProgressUIObserver* observer)
  {
    MessageBlockHolder message(MakeMessageBlock(header, filePath, size), MessageDeleter);
    net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
    if ((wsaHolder.GetErrorCode() == 0))
    {
      ProgressUpdater updater(observer, size);
      SocketStream streamSocket(new ACE_SOCK_Stream());
      ACE_SOCK_Connector connector;
      if (connector.connect(*streamSocket, userAddr, &TIMEOUT) == 0)
      {
        try
        {
          SendMessageBlock(streamSocket, message.get(), updater);
        }
        catch (const TransferringError&)
        {
          if (observer)
          {
            observer->OnError();
          }
        }
        catch (const LogOutError&)
        {
        }
      }
    }
  }

  void FileTransferManager::SendMessageBlock(const SocketStream& socket, ACE_Message_Block* message,
                                             ProgressUpdater& updater)
  {
    for (auto block = message; block != nullptr; block = block->cont())
    {
      size_t messageSize = block->length();
      if (socket->send_n(block->rd_ptr(), messageSize, &TIMEOUT) == messageSize)
      {
        updater.Update(messageSize);
      }
      else
      {
        updater.Error();
      }
    }
    updater.Finished();
  }

  void FileTransferManager::RunAsyncSender(const ACE_INET_Addr& userAddr, const std::wstring& filePath, size_t size,
                                           const std::wstring& header, ui::IProgressUIObserver* observer)
  {
    Thread thread(&FileTransferManager::SendFileImpl, this, userAddr, filePath, size, header, observer);
    thread.detach();
  }

  FileInfoPtr FileTransferManager::GetFileInfo(const std::wstring& filePath)
  {
    FileInfoPtr fileInfo = nullptr;
    std::ifstream file(filePath);
    if (file)
    {
      std::wstring uuid(login::LoginManager::Instance()->GetCurrentUser()->uuid);
      fileInfo.reset(new FileInfo(GetFileLength(file), GetFileName(filePath), uuid));
      file.close();
    }
    return fileInfo;
  }

  ACE_Message_Block* FileTransferManager::MakeMessageBlock(const std::wstring& header, const std::wstring& filePath,
                                                            size_t fileSize)
  {
    std::vector<MessageBlockPtr> blocks;
    
    MessageBlockPtr headerBlock(new ACE_Message_Block(MTU_SIZE));
    size_t headerSize = header.size() * sizeof(wchar_t);
    int copyResult = headerBlock->copy((const char*)header.c_str(), headerSize);
    headerBlock->wr_ptr(copyResult);
    blocks.push_back(std::move(headerBlock));

    const size_t MTUChunkCount = fileSize / MTU_SIZE;
    const size_t residualSize = fileSize % MTU_SIZE;

    std::ifstream file(filePath);
    if (file)
    {
      for (size_t count = 0; count != MTUChunkCount; ++count)
      {
        MessageBlockPtr block(new ACE_Message_Block(MTU_SIZE));
        file.read(block->wr_ptr(), MTU_SIZE);
        block->wr_ptr(MTU_SIZE);
        blocks.push_back(std::move(block));
      }
      
      if (residualSize > 0)
      {
        MessageBlockPtr block(new ACE_Message_Block(residualSize));
        file.read(block->wr_ptr(), residualSize);
         block->wr_ptr(residualSize);
        blocks.push_back(std::move(block));
      }
    }

    if (file)
    {
      file.close();
      return LinksMessageBlocks(blocks);
    }

    return nullptr;
  }

  std::wstring FileTransferManager::MakeMessageHeader(const FileInfoPtr& fileInfo)
  {
    return boost::str(boost::wformat(FILE_MESSAGE_HEADER)
                      % fileInfo->m_uuid % fileInfo->m_size % fileInfo->m_name);
  }

  void FileTransferManager::OnlineStateChanged()
  {
  }

  void FileTransferManager::Activate(UIMessageHandler* handler)
  {
    if (!m_activated)
    {
      m_msgHandler.reset(new FileMessagesHandler(handler));
      m_server.reset(new TCPMessageServer(m_msgHandler.get(), m_settingsHolder.get()));
      m_activated = true;
    }
  }

  void FileTransferManager::Deactivate()
  {
    m_server->Shutdown();
    m_activated = false;
  }

  void FileTransferManager::ResetServer()
  {
    m_server->Reset();
  }
}
