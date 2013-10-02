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

  class StateMessagesManager
    : private boost::noncopyable
    , public login::ILoginStateObserver
    , public sm::SettingsChangeObserver
  {
  public:
    static StateMessagesManager* Instance();
    void SendResponseToConnect(const std::wstring& addr);
    void SendMessageToUsers(State currentState);
    void SendBroadcastMessage(State currentState);
    void Activate(MessagesReceiver* receiver);
    void Deactivate();
    StateMessagesQueue* GetMessagesQueue() const;
    virtual void OnlineStateChanged();

  private:
    StateMessagesManager();
    ~StateMessagesManager();
    void ResetServer();
    std::wstring CreateMessage(State currentState);
    virtual void SettingsWillBeChanged(int type);
    virtual void SettingsChanged(int type);

  private:
    std::unique_ptr<StateMessagesQueue> m_msgQueue;
    std::unique_ptr<IServerSettingsHolder> m_settingsHolder;
    bool m_activated;
    std::unique_ptr<IMessagesHandler> m_msgHandler;
    std::unique_ptr<MessageServer> m_server;
  };
}
