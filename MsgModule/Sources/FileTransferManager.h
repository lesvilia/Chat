#pragma once
#include <memory>
#include <string>
#include "boost/noncopyable.hpp"
#include "LoginHandlers.h"

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

  class FileTransferManager
    : private boost::noncopyable
    , public login::ILoginStateObserver
  {
  public:
    static FileTransferManager* Instance();
    void SendFile(const std::wstring& filePath, ui::IProgressUIObserver* observer);
    virtual void OnlineStateChanged();
    void Activate(UIMessageHandler* handler);
    void Deactivate();
    void ResetServer();

  private:
    FileTransferManager();
    ~FileTransferManager();

  private:
    std::unique_ptr<IServerSettingsHolder> m_settingsHolder;
    bool m_activated;
    std::unique_ptr<IMessagesHandler> m_msgHandler;
    std::unique_ptr<MessageServer> m_server;
  };
}