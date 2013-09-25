#include "stdafx.h"
#include "FileTransferManager.h"

#include <fstream>
#include "ace/Message_Block.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "FileServerSettingsHolder.h"
#include "FileAsyncSender.h"
#include "FileMessagesHandler.h"
#include "IProgressUIObserver.h"
#include "SettingsManager.h"
#include "LoginManager.h"
#include "NetUsersManager.h"
#include "TCPMessagesServer.h"

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
        std::wstring addr(net::NetUsersManager::Instance()->GetNetUserAddress(uuid));
        ACE_INET_Addr userAddr(m_settingsHolder->GetPort(), addr.c_str());
        AsyncFileSender sender(userAddr, fileInfo, uuid, observer); //runs and detaches thread
      }
    }
  }

  FileInfoPtr FileTransferManager::GetFileInfo(const std::wstring& filePath)
  {
    FileInfoPtr fileInfo = nullptr;
    std::ifstream file(filePath, std::ios::binary);
    if (file)
    {
      std::wstring uuid(login::LoginManager::Instance()->GetCurrentUser()->uuid);
      fileInfo.reset(new FileInfo(GetFileLength(file), GetFileName(filePath), uuid, filePath));
      file.close();
    }
    return fileInfo;
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
