#pragma once
#include <memory>
#include <string>
#include "boost/noncopyable.hpp"
#include "FileMessagesHandler.h"
#include "LoginHandlers.h"

class ACE_Message_Block;

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class UIMessageHandler;
  class IServerSettingsHolder;
  class IMessagesHandler;
  class MessageServer;
  class ProgressUpdater;

  class FileTransferManager
    : private boost::noncopyable
    , public login::ILoginStateObserver
  {
  public:
    static FileTransferManager* Instance();
    void SendFile(const std::wstring& uuid, const std::wstring& filePath,
                  ui::IProgressUIObserver* observer);
    virtual void OnlineStateChanged();
    void Activate(UIMessageHandler* handler);
    void Deactivate();
    void ResetServer();

  private:
    void SendFileImpl(const ACE_INET_Addr& userAddr, const std::wstring& filePath, size_t size,
                      const std::wstring& header, ui::IProgressUIObserver* observer);
    void RunAsyncSender(const ACE_INET_Addr& userAddr, const std::wstring& filePath, size_t size,
                        const std::wstring& header, ui::IProgressUIObserver* observer);
    void SendMessageBlock(const SocketStream& socket, ACE_Message_Block* message,
                          ProgressUpdater& updater);
    FileInfoPtr GetFileInfo(const std::wstring& filePath);
    ACE_Message_Block* MakeMessageBlock(const std::wstring& header, const std::wstring& filePath,
                                        size_t fileSize);
    std::wstring MakeMessageHeader(const FileInfoPtr& fileInfo);
    FileTransferManager();
    ~FileTransferManager();

  private:
    std::unique_ptr<IServerSettingsHolder> m_settingsHolder;
    bool m_activated;
    std::unique_ptr<IMessagesHandler> m_msgHandler;
    std::unique_ptr<MessageServer> m_server;
  };
}
