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
#include "pugixml.hpp"
#include "StringHelpers.h"
#include "Typedefs.h"

namespace
{
  const size_t MTU_SIZE = 1400;
  const ACE_Time_Value TIMEOUT(2);

  class ObserverIniter
  {
  public:
    ObserverIniter()
    : m_progressObserver(nullptr) { }

    void SetObserver(ui::IProgressUIObserver* observer) 
    {
      m_progressObserver = observer;
      m_cond.notify_one();
    }

    ui::IProgressUIObserver* GetObserver() const 
    {
      return m_progressObserver;
    }

    void WaitForInit()
    {
      msg::Lock lock(m_mutex);
      m_cond.timed_wait(lock, boost::posix_time::milliseconds(5000));
    }

  private:
    ui::IProgressUIObserver* m_progressObserver;
    msg::Mutex m_mutex;
    msg::ConditionVariable m_cond;
  };

  class ProgressUpdater
  {
  public:
    ProgressUpdater(ui::IProgressUIObserver* observer, size_t fileSize)
      : m_fileSize(fileSize)
      , m_transferedSize(0)
      , m_progressObserver(observer) { }
    
    void Update(size_t size)
    {
      if (m_progressObserver)
      {
        m_transferedSize += size;
        float result = ((float)m_transferedSize / m_fileSize) * 100;
        m_progressObserver->UpdateProgress(static_cast<int>(result));
      }
    }

    void Finished()
    {
      if (m_progressObserver)
      {
        m_progressObserver->UpdateProgress(100);
        m_progressObserver->OnFinished();
      }
    }

    void Error()
    {
      if (m_progressObserver)
      {
        m_progressObserver->OnError();
      }
    }

  private:
    size_t m_fileSize;
    size_t m_transferedSize;
    ui::IProgressUIObserver* m_progressObserver;
  };
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

  void FileMessagesHandler::HandleConnect(SocketStream sockStream)
  {
    RunAsyncHandler(sockStream);
  }

  void FileMessagesHandler::HandleConnectImpl(SocketStream sockStream)
  {
    FileMessagesHandler::FileInfoPtr fileInfo(GetFileInfo(sockStream));
    if (fileInfo)
    {
      ObserverIniter initer;
      m_msgHandler->OnFileMessageReceived(fileInfo->m_uuid, fileInfo->m_name,
        [&initer](ui::IProgressUIObserver* observer) { initer.SetObserver(observer); });
      initer.WaitForInit();
      ui::IProgressUIObserver* observer = initer.GetObserver();
      if (observer)
      {
        ACE_Message_Block* head = nullptr;
        RecvMessageBlocks(sockStream, fileInfo->m_size, head, observer);
        if (head)
        {
          SaveMessageBlockToFile(fileInfo->m_name, head);
          head->release();
        }
      }
    }
    sockStream->close();
  }

  void FileMessagesHandler::RunAsyncHandler(SocketStream sockStream)
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


  void FileMessagesHandler::RecvMessageBlocks(const SocketStream& sockStream, size_t fileSize,
                                              ACE_Message_Block*& head, ui::IProgressUIObserver* observer)
  {
    if (fileSize > 0)
    {
      const size_t MTUChunkCount = fileSize / MTU_SIZE;
      const size_t residualSize = fileSize % MTU_SIZE;

      bool onError = false;
      std::vector<ACE_Message_Block*> blocks;
      ProgressUpdater updater(observer, fileSize);

      for (size_t count = 0; (count != MTUChunkCount) && !onError; ++count)
      {
        ACE_Message_Block* MTUBlock = new ACE_Message_Block(MTU_SIZE);
        if (sockStream->recv_n(MTUBlock->wr_ptr(), MTU_SIZE, &TIMEOUT) == MTU_SIZE)
        {
          MTUBlock->wr_ptr(MTU_SIZE);
          updater.Update(MTU_SIZE);
        }
        else
        {
          onError = true;
          updater.Error();
        }
        blocks.push_back(MTUBlock);
      }

      if (residualSize > 0 && !onError)
      {
        ACE_Message_Block* residualBlock = new ACE_Message_Block(residualSize);
        if (sockStream->recv_n(residualBlock->wr_ptr(), residualSize, &TIMEOUT) == residualSize)
        {
          residualBlock->wr_ptr(residualSize);
          updater.Update(residualSize);
          updater.Finished();
        }
        else
        {
          onError = true;
          updater.Error();
        }
        blocks.push_back(residualBlock);
      }

      if (!blocks.empty())
      {
        ACE_Message_Block* message = *blocks.begin();
        head = message;
        for (auto it = blocks.begin() + 1; it != blocks.end(); ++it)
        {
          message->cont(*it);
          message = message->cont();
        }

        if (onError)
        {
          head->release();
          head = nullptr;
        }
      }
    }
  }

  void FileMessagesHandler::SaveMessageBlockToFile(const std::wstring& fileName, ACE_Message_Block* message)
  {
    ACE_FILE_IO file;
    ACE_FILE_Connector connector;
    connector.connect(file, ACE_FILE_Addr(ACE_TEXT_WCHAR_TO_TCHAR(fileName.c_str())), 0, ACE_Addr::sap_any, 0,
                      O_RDWR | O_CREAT | ACE_DEFAULT_FILE_PERMS);
    file.send_n(message);
    file.close();
  }
}
