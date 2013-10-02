#pragma once
#include "boost/noncopyable.hpp"
#include "LoginHandlers.h"
#include "MessagesQueue.h"
#include "SettingsChangeObserver.h"

namespace msg
{
  class MessagesReceiver;
  class IMessagesHandler;
  class MessageServer;
  class IServerSettingsHolder;

  class ChatMessagesManager
    : private boost::noncopyable
    , public sm::SettingsChangeObserver
    , public login::ILoginStateObserver
  {
  public:
    static ChatMessagesManager* Instance();
    void Send(const std::wstring& uuid, const std::wstring& txtMessage);
    void Activate(MessagesReceiver* receiver);
    void Deactivate();
    ChatMessagesQueue* GetMessagesQueue();
    virtual void OnlineStateChanged();

  private:
    ChatMessagesManager();
    ~ChatMessagesManager();
    void ResetServer();
    virtual void SettingsWillBeChanged(int type);
    virtual void SettingsChanged(int type);
    std::wstring CreateMessage(const std::wstring& txtMessage);

  private:
    std::unique_ptr<ChatMessagesQueue> m_msgQueue;
    std::unique_ptr<IServerSettingsHolder> m_settingsHolder;
    bool m_activated;
    std::unique_ptr<IMessagesHandler> m_msgHandler;
    std::unique_ptr<MessageServer> m_server;
  };
}
