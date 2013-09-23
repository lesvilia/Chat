#include "stdafx.h"
#include "FileMessagesHandler.h"

#include <vector>

#include "ace/SOCK_Stream.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_IO.h"
#include "ace/FILE_Connector.h"
#include "ace/Message_Block.h"
#include "ace/Time_Value.h"
#include "UIMessageHandler.h"
#include "IProgressUIObserver.h"
#include "LoginManager.h"
#include "MessagesTemplates.h"
#include "FileTransferHelpers.h"
#include "pugixml.hpp"
#include "StringHelpers.h"
#include "Typedefs.h"

namespace
{
  const size_t MTU_SIZE = 1400;
  const ACE_Time_Value TIMEOUT(2);
  ACE_Message_Block* LinksMessageBlocks(std::vector<std::unique_ptr<ACE_Message_Block> >& blocks)
  {
    ACE_Message_Block *head = nullptr, *message = nullptr;
    if (!blocks.empty())
    {
      head = (*blocks.begin()).release();
      message = head;
      for (auto it = blocks.begin() + 1; it != blocks.end(); ++it)
      {
        message->cont((*it).release());
        message = message->cont();
      }
    }
    return head;
  }
}

namespace msg
{
  FileMessagesHandler::FileInfo::FileInfo(size_t size, const std::wstring& name, const std::wstring& uuid)
    : m_size(size)
    , m_name(name)
  {
  }

  FileMessagesHandler::FileMessagesHandler(UIMessageHandler* msgHandler)
    : m_msgHandler(msgHandler)
  {
    assert(m_msgHandler != nullptr);
  }

  FileMessagesHandler::~FileMessagesHandler()
  {
  }

  void FileMessagesHandler::HandleConnect(const SocketStream& sockStream)
  {
    RunAsyncHandler(sockStream);
  }

  void FileMessagesHandler::HandleConnectImpl(const SocketStream& sockStream)
  {
    if (login::LoginManager::Instance()->IsOnline())
    {
      FileMessagesHandler::FileInfoPtr fileInfo(GetFileInfo(sockStream));
      if (fileInfo && fileInfo->m_size > 0)
      {
        ObserverIniter initer;
        m_msgHandler->OnFileMessageReceived(fileInfo->m_uuid, fileInfo->m_name,
                                            boost::bind(&ObserverIniter::SetObserver, boost::ref(initer), _1));
        ui::IProgressUIObserver* observer = initer.GetObserver();
        if (observer)
        {
          try
          {
            ACE_Message_Block* head = RecvMessageBlocks(sockStream, fileInfo->m_size, observer);
            if (head)
            {
              SaveMessageBlocksToFile(fileInfo->m_name, head);
              head->release();
            }
          }
          catch (const TransferringError&)
          {
          }
          catch (const LogOutError&)
          {
          }
        }
      }
    }
  }

  void FileMessagesHandler::RunAsyncHandler(const SocketStream& sockStream)
  {
    Thread newThread(&FileMessagesHandler::HandleConnectImpl, this, sockStream);
    newThread.detach();
  }

  FileMessagesHandler::FileInfoPtr FileMessagesHandler::GetFileInfo(SocketStream sockStream) const
  {
    std::vector<char> buff(MTU_SIZE + 1, '\0');
    if (sockStream->recv_n(&buff[0], MTU_SIZE, &TIMEOUT) > 0)
    {
      std::wstring header(buff.begin(), buff.end());
      if (!header.empty())
      {
        return ParseMessageHeader(header);
      }
    }
    return nullptr;
  }

  FileMessagesHandler::FileInfoPtr FileMessagesHandler::ParseMessageHeader(const std::wstring& msgHeader) const
  {
    FileMessagesHandler::FileInfoPtr fileInfo(nullptr);
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
      fileInfo.reset(new  FileMessagesHandler::FileInfo(std::stoi(fileSize), fileName, uuid));
    }
    return fileInfo;
  }


  ACE_Message_Block* FileMessagesHandler::RecvMessageBlocks(const SocketStream& sockStream, size_t fileSize,
                                                            ui::IProgressUIObserver* observer)
  {
    const size_t MTUChunkCount = fileSize / MTU_SIZE;
    const size_t residualSize = fileSize % MTU_SIZE;

    std::vector<MessageBlockPtr> blocks;
    ProgressUpdater updater(observer, fileSize);

    for (size_t count = 0; count != MTUChunkCount; ++count)
    {
      MessageBlockPtr MTUBlock(new ACE_Message_Block(MTU_SIZE));
      if (sockStream->recv_n(MTUBlock->wr_ptr(), MTU_SIZE, &TIMEOUT) == MTU_SIZE)
      {
        MTUBlock->wr_ptr(MTU_SIZE);
        updater.Update(MTU_SIZE);
      }
      else
      {
        updater.Error();
      }
      blocks.push_back(std::move(MTUBlock));
    }

    if (residualSize > 0)
    {
      MessageBlockPtr residualBlock(new ACE_Message_Block(residualSize));
      if (sockStream->recv_n(residualBlock->wr_ptr(), residualSize, &TIMEOUT) == residualSize)
      {
        residualBlock->wr_ptr(residualSize);
        updater.Update(residualSize);
        updater.Finished();
      }
      else
      {
        updater.Error();
      }
      blocks.push_back(std::move(residualBlock));
    }
    return LinksMessageBlocks(blocks);
  }

  void FileMessagesHandler::SaveMessageBlocksToFile(const std::wstring& fileName, ACE_Message_Block* message)
  {
    ACE_FILE_IO file;
    ACE_FILE_Connector connector;
    connector.connect(file, ACE_FILE_Addr(ACE_TEXT_WCHAR_TO_TCHAR(fileName.c_str())), 0, ACE_Addr::sap_any, 0,
                      O_RDWR | O_CREAT | ACE_DEFAULT_FILE_PERMS);
    file.send_n(message);
    file.close();
  }
}
