#pragma once
#include <string>
#include<vector>

#include "IMessagesHandler.h"

class ACE_Message_Block;

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class UIMessageHandler;

  struct FileInfo
  {
    FileInfo(size_t size, const std::wstring& name, const std::wstring& uuid);
    size_t m_size;
    std::wstring m_name;
    std::wstring m_uuid;
  };

  typedef std::unique_ptr<FileInfo> FileInfoPtr;
  typedef void (*MessageBlockDeleter)(ACE_Message_Block*);
  typedef std::unique_ptr<ACE_Message_Block> MessageBlockPtr;
  typedef std::unique_ptr<ACE_Message_Block, MessageBlockDeleter> MessageBlockHolder;
  ACE_Message_Block* LinksMessageBlocks(std::vector<MessageBlockPtr>& blocks);

  class FileMessagesHandler
    : public IMessagesHandler
  {
  public:
    FileMessagesHandler(UIMessageHandler* msgHandler);
    virtual ~FileMessagesHandler();
    virtual void HandleConnect(const SocketStream& sockStream);

  private:
    void HandleConnectImpl(const SocketStream& sockStream);
    void RunAsyncHandler(const SocketStream& sockStream);
    FileInfoPtr GetFileInfo(SocketStream sockStream) const;
    FileInfoPtr ParseMessageHeader(const std::wstring& msgHeader) const;
    ACE_Message_Block* RecvMessageBlocks(const SocketStream& sockStream, size_t fileSize,
                                         ui::IProgressUIObserver* observer);
    void SaveMessageBlocksToFile(const std::wstring& fileName, ACE_Message_Block* message);
  private:
    UIMessageHandler* m_msgHandler;
  };
}
