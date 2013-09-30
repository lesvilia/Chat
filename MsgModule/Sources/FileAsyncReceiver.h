#pragma once
#include <string>

#include "FileInfo.h"
#include "IMessagesHandler.h"
#include "Typedefs.h"

class ACE_INET_Addr;

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class UIMessageHandler;
  class ProgressUpdater;

  class AsyncFileReceiver
    : private boost::noncopyable
  {
  public:
    AsyncFileReceiver(const SocketStream& socket, UIMessageHandler* handler);
    ~AsyncFileReceiver();

  private:
    static void RecvFileImpl(const SocketStream& socket, UIMessageHandler* handler);
    static ACE_Message_Block* RecvMessageBlocks(const SocketStream& sockStream, const FileInfoPtr& fileInfo,
                                                ProgressUpdater& updater);;
    static FileInfoPtr GetFileInfo(const SocketStream& sockStream);
    static FileInfoPtr ParseMessageHeader(const std::wstring& msgHeader);
    static ui::IProgressUIObserver* GetObserver(const FileInfoPtr& fileInfo, UIMessageHandler* handler);
    static void SaveMessageBlocksToFile(const std::wstring& fileName, ACE_Message_Block* message);
  private:
    Thread m_thread;
  };
}
