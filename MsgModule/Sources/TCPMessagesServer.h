#pragma once
#include <memory>
#include "MessagesServer.h"
#include "Typedefs.h"

class ACE_SOCK_Acceptor;

namespace msg
{
  class IMessagesHandler;
  class IServerSettingsHolder;

  class TCPMessageServer
    : public MessageServer
  {
    typedef void (*SocketDeleter)(ACE_SOCK_Acceptor*);
    typedef std::unique_ptr<ACE_SOCK_Acceptor, SocketDeleter> SocketAcceptor;
    typedef std::unique_ptr<Thread> ThreadPtr;

  public:
    TCPMessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder);
    virtual ~TCPMessageServer();
    virtual void Shutdown();
    virtual void Reset(const ResetCompletionCallback& callback = ResetCompletionCallback());

  private:
    virtual void Run();
    void Initialize();
    void InitSocket();
    bool ShouldShutdown();
    void ResetIfNeeded();

  private:
    bool m_shouldShutdown;
    bool m_needReset;
    SocketAcceptor m_acceptor;
    ThreadPtr m_thread;
    Mutex m_mutex;
    ResetCompletionCallback m_callback;
  };
}
