#include "FileAsyncReceiver.h"

#include <fstream>
#include "ace/Message_Block.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "IProgressUIObserver.h"
#include "MessagesTemplates.h"
#include "SettingsManager.h"
#include "FileTransferHelpers.h"
#include "pugixml.hpp"
#include "UIMessageHandler.h"
#include "StringHelpers.h"

namespace
{
  const size_t MTU_SIZE = 1400;

  typedef void (*FileCloser)(std::ofstream*);
  typedef std::unique_ptr<std::ofstream, FileCloser> FileHolder;

  void CloseFile(std::ofstream* file)
  {
    if (*file)
      file->close();

    delete file;
  }

  void MessageDeleter(ACE_Message_Block* block)
  {
    block->release();
  }
}

namespace msg
{
  AsyncFileReceiver::AsyncFileReceiver(const SocketStream& socket, UIMessageHandler* handler)
    : m_thread(&AsyncFileReceiver::RecvFileImpl, socket, handler)
  {
  }

  AsyncFileReceiver::~AsyncFileReceiver()
  {
    if (m_thread.joinable())
    {
      m_thread.detach();
    }
  }

  void AsyncFileReceiver::RecvFileImpl(const SocketStream& socket, UIMessageHandler* handler)
  {
    FileInfoPtr fileInfo(GetFileInfo(socket));
    if (!fileInfo)
      return;

    ui::IProgressUIObserver* observer = GetObserver(fileInfo, handler);
    if (!observer)
      return;

    try
    {
      ProgressUpdater updater(observer, fileInfo->m_size, fileInfo->m_uuid);
      MessageBlockHolder message(RecvMessageBlocks(socket, fileInfo, updater), MessageDeleter);
      SaveMessageBlocksToFile(fileInfo->m_name, message.get());
    }
    catch (const Interrupter::Error&)
    {
      if (observer)
      {
        observer->OnError();
      }
    }
  }

  ui::IProgressUIObserver* AsyncFileReceiver::GetObserver(const FileInfoPtr& fileInfo, UIMessageHandler* handler)
  {
    ObserverIniter initer;
    handler->OnFileMessageReceived(fileInfo->m_uuid, fileInfo->m_name,
      boost::bind(&ObserverIniter::SetObserver, boost::ref(initer), _1));
    return initer.GetObserver();
  }

  ACE_Message_Block* AsyncFileReceiver::RecvMessageBlocks(const SocketStream& sockStream, const FileInfoPtr& fileInfo,
                                                          ProgressUpdater& updater)
  {
    const size_t MTUChunkCount = fileInfo->m_size / MTU_SIZE;
    const size_t residualSize = fileInfo->m_size % MTU_SIZE;

    std::vector<MessageBlockPtr> blocks;

    for (size_t count = 0; count != MTUChunkCount; ++count)
    {
      MessageBlockPtr MTUBlock(new ACE_Message_Block(MTU_SIZE));
      size_t isTransfered = sockStream->recv_n(MTUBlock->wr_ptr(), MTU_SIZE);
      int error = errno;
      if (isTransfered == MTU_SIZE)
      {
        MTUBlock->wr_ptr(MTU_SIZE);
        updater.Update(MTU_SIZE);
      }
      else
      {
        updater.TransferError();
      }
      blocks.push_back(std::move(MTUBlock));
    }

    if (residualSize > 0)
    {
      MessageBlockPtr residualBlock(new ACE_Message_Block(residualSize));
      if (sockStream->recv_n(residualBlock->wr_ptr(), residualSize) == residualSize)
      {
        residualBlock->wr_ptr(residualSize);
        updater.Update(residualSize);
        updater.Finished();
      }
      else
      {
        updater.TransferError();
      }
      blocks.push_back(std::move(residualBlock));
    }
    return LinksMessageBlocks(blocks);
  }

  FileInfoPtr AsyncFileReceiver::GetFileInfo(const SocketStream& sockStream)
  {
    std::vector<char> buff(MTU_SIZE + 1, '\0');
    if (sockStream->recv_n(&buff[0], MTU_SIZE) > 0)
    {
      std::string header(buff.begin(), buff.end());
      if (!header.empty())
      {
        return ParseMessageHeader(strhlp::StrToWstr(header));
      }
    }
    return nullptr;
  }

  FileInfoPtr AsyncFileReceiver::ParseMessageHeader(const std::wstring& msgHeader)
  {
    FileInfoPtr fileInfo(nullptr);
    pugi::xml_document messageDoc;
    pugi::xml_parse_result result = messageDoc.load(msgHeader.c_str());
    if (result)
    {
      pugi::xml_node messageNode = messageDoc.first_child();

      pugi::xml_node userNode = messageNode.child(USER_NODE);
      std::wstring uuid = userNode.child_value(USER_UUID_NODE);

      pugi::xml_node dataNode = messageNode.child(DATA_NODE);
      std::wstring fileSize(dataNode.child_value(FILE_SIZE_NODE));
      std::wstring fileName(dataNode.child_value(FILE_NAME_NODE));
      fileInfo.reset(new  FileInfo(std::stoi(fileSize), fileName, uuid));
    }
    return fileInfo;
  }

  void AsyncFileReceiver::SaveMessageBlocksToFile(const std::wstring& fileName, ACE_Message_Block* message)
  {
    std::wstring saveDir(sm::SettingsManager::Instance()->GetCurrentSaveDir());
    std::wstring filePath(saveDir + L"\\" + fileName);
    FileHolder outputFile(new std::ofstream(filePath.c_str(), std::ios::binary), CloseFile);
    if (!(*outputFile.get()))
      return;

    for (auto block = message; block != nullptr; block = block->cont())
    {
      outputFile->write(block->rd_ptr(), block->length());
    }
  }
}
