#pragma once
#include <string>

#include "IMessagesHandler.h"

class ACE_Message_Block;

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class UIMessageHandler;

  class FileMessagesHandler
    : public IMessagesHandler
  {
    struct FileInfo
    {
      FileInfo(size_t size, const std::wstring& name, const std::wstring& uuid);
      size_t m_size;
      std::wstring m_name;
      std::wstring m_uuid;
    };
    typedef std::shared_ptr<FileInfo> FileInfoPtr;

  public:
    FileMessagesHandler(UIMessageHandler* msgHandler);
    virtual ~FileMessagesHandler();
    virtual void HandleConnect(SocketStream sockStream);

  private:
    void HandleConnectImpl(SocketStream sockStream);
    void RunAsyncHandler(SocketStream sockStream);
    FileInfoPtr GetFileInfo(const std::wstring& msgHeader) const;
    void RecvMessageBlocks(const SocketStream& sockStream, size_t fileSize,
                           ACE_Message_Block*& head, ui::IProgressUIObserver* observer);
    void SaveMessageBlockToFile(const std::wstring& fileName, ACE_Message_Block* message);
  private:
    UIMessageHandler* m_msgHandler;
  };
}
