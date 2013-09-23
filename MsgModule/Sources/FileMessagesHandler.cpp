#include "stdafx.h"
#include "FileMessagesHandler.h"

#include <vector>

#include "ace/SOCK_Stream.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_IO.h"
#include "ace/FILE_Connector.h"
#include "ace/Message_Block.h"
#include "UIMessageHandler.h"
#include "IProgressUIObserver.h"
#include "MessagesTemplates.h"
#include "pugixml.hpp"
#include "StringHelpers.h"
#include "Typedefs.h"

namespace
{
  const size_t MTU_SIZE = 1400;

  class ObserverIniter
  {
  public:
    ObserverIniter() { }
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
      m_transferedSize += size;
      float result = ((float)m_transferedSize / m_fileSize) * 100;
      m_progressObserver->UpdateProgress(static_cast<int>(result));
    }
    void Finished()
    {
      m_progressObserver->UpdateProgress(100);
      m_progressObserver->OnFinished();
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
    std::vector<char> buff(MTU_SIZE + 1, '\0');
    if (sockStream->recv(&buff[0], MTU_SIZE) > 0)
    {
      std::wstring header(buff.begin(), buff.end());
      if (!header.empty())
      {
        FileMessagesHandler::FileInfoPtr fileInfo(GetFileInfo(header));
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
          }
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

  FileMessagesHandler::FileInfoPtr FileMessagesHandler::GetFileInfo(const std::wstring& msgHeader) const
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
       ACE_Message_Block* message = nullptr;
       head = message;

       ProgressUpdater updater(observer, fileSize);
       for (size_t count = 0; count != MTUChunkCount; ++count)
       {
         ACE_Message_Block* MTUBlock = new ACE_Message_Block(MTU_SIZE);
         if (sockStream->recv_n(MTUBlock->wr_ptr(), MTU_SIZE) == MTU_SIZE)
         {
           MTUBlock->wr_ptr(MTU_SIZE);
           updater.Update(MTU_SIZE);
         }
         message->cont(MTUBlock);
         message = message->cont();
       }

       if (residualSize > 0)
       {
         ACE_Message_Block* residualBlock = new ACE_Message_Block(residualSize);
         if (sockStream->recv_n(residualBlock->wr_ptr(), residualSize) == residualSize)
         {
           residualBlock->wr_ptr(residualSize);
           updater.Update(residualSize);
         }
         message->cont(residualBlock);
       }
       updater.Finished();
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
