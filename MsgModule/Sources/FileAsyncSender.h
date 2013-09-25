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
  class ProgressUpdater;
  class AsyncFileSender
    : private boost::noncopyable
  {
  public:
    AsyncFileSender(const ACE_INET_Addr& userAddr, const FileInfoPtr& file,
                    const std::wstring& uuid, ui::IProgressUIObserver* observer);
    ~AsyncFileSender();

  private:
    static void SendFileImpl(const ACE_INET_Addr& userAddr, const FileInfoPtr& file,
                             const std::wstring& uuid, ui::IProgressUIObserver* observer);
    static void SendMessageBlock(const SocketStream& socket, ACE_Message_Block* message,
      ProgressUpdater& updater);
    static ACE_Message_Block* MakeMessageBlocks(const FileInfoPtr& fileInfo);
    static std::wstring MakeMessageHeader(const FileInfoPtr& fileInfo);

  private:
    Thread m_thread;
  };
}
