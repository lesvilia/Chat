#include "FileAsyncSender.h"

#include <fstream>
#include "ace/Message_Block.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "boost/format.hpp"
#include "FileTransferHelpers.h"
#include "IProgressUIObserver.h"
#include "LoginManager.h"
#include "MessagesTemplates.h"
#include "WSAStartupHolder.h"
#include "IProgressUIObserver.h"

namespace
{
  const size_t MTU_SIZE = 1400;
  const ACE_Time_Value TIMEOUT(2);

  void MessageDeleter(ACE_Message_Block* block)
  {
    block->release();
    if (block)
      delete block;
  }

  void StreamDeleter(ACE_SOCK_Stream* stream)
  {
    stream->close();
    delete stream;
  }

  typedef void (*FileCloser)(std::ifstream*);
  typedef std::unique_ptr<std::ifstream, FileCloser> FileHolder;

  void CloseFile(std::ifstream* file)
  {
    if (*file)
      file->close();

    delete file;
  }
}

namespace msg
{
  AsyncFileSender::AsyncFileSender(const ACE_INET_Addr& userAddr, const FileInfoPtr& file,
                                   const std::wstring& uuid, ui::IProgressUIObserver* observer)
    : m_thread(&AsyncFileSender::SendFileImpl, userAddr, file, uuid, observer)
  {
  }

  AsyncFileSender::~AsyncFileSender()
  {
    if (m_thread.joinable())
    {
      m_thread.detach();
    }
  }

  void AsyncFileSender::SendFileImpl(const ACE_INET_Addr& userAddr, const FileInfoPtr& fileInfo,
                                     const std::wstring& uuid, ui::IProgressUIObserver* observer)
  {
    ProgressUpdater updater(observer, fileInfo->m_size, uuid);
    MessageBlockHolder message(MakeMessageBlocks(fileInfo), MessageDeleter);
    
    net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
    if (!(wsaHolder.GetErrorCode() == 0))
      return;

    SocketStream streamSocket(new ACE_SOCK_Stream(), StreamDeleter);
    ACE_SOCK_Connector connector;
    if (connector.connect(*streamSocket, userAddr, &TIMEOUT) == -1)
      return;

    try
    {
      SendMessageBlock(streamSocket, message.get(), updater);
    }
    catch (const Interrupter::Error&)
    {
      if (observer)
      {
        observer->OnError();
      }
    }
  }

  void AsyncFileSender::SendMessageBlock(const SocketStream& socket, ACE_Message_Block* message,
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
        updater.TransferError();
      }
    }
    updater.Finished();
  }

  ACE_Message_Block* AsyncFileSender::MakeMessageBlocks(const FileInfoPtr& fileInfo)
  {
    std::wstring header(MakeMessageHeader(fileInfo));
    std::vector<MessageBlockPtr> blocks;

    MessageBlockPtr headerBlock(new ACE_Message_Block(MTU_SIZE));
    size_t headerSize = header.size() * sizeof(wchar_t);
    int copyResult = headerBlock->copy((const char*)header.c_str(), headerSize);
    headerBlock->wr_ptr(copyResult);
    blocks.push_back(std::move(headerBlock));

    const size_t MTUChunkCount = fileInfo->m_size / MTU_SIZE;
    const size_t residualSize = fileInfo->m_size % MTU_SIZE;

    FileHolder file(new std::ifstream(fileInfo->m_path, std::ios::binary), CloseFile);
    if (!(*file.get()))
      return nullptr;

    for (size_t count = 0; count != MTUChunkCount; ++count)
    {
      MessageBlockPtr block(new ACE_Message_Block(MTU_SIZE));
      file->read(block->wr_ptr(), MTU_SIZE);
      block->wr_ptr(MTU_SIZE);
      blocks.push_back(std::move(block));
    }

    if (residualSize > 0)
    {
      MessageBlockPtr block(new ACE_Message_Block(residualSize));
      file->read(block->wr_ptr(), residualSize);
      block->wr_ptr(residualSize);
      blocks.push_back(std::move(block));
    }

    if (*file.get())
      return LinksMessageBlocks(blocks);

    return nullptr;
  }

  std::wstring AsyncFileSender::MakeMessageHeader(const FileInfoPtr& fileInfo)
  {
    return boost::str(boost::wformat(FILE_MESSAGE_HEADER) 
                      % fileInfo->m_uuid 
                      % fileInfo->m_size 
                      % fileInfo->m_name);
  }
}
