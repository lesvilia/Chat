#pragma once
#include <memory>
#include "ace/SOCK_Dgram.h"
#include "MessagesServer.h"
#include "Typedefs.h"


namespace msg
{
  class IMessagesHandler;
  class IServerSettingsHolder;

  class UDPMessageServer
    : public MessageServer
  {
    typedef void (*SocketDeleter)(ACE_SOCK_Dgram*);
    typedef std::unique_ptr<ACE_SOCK_Dgram, SocketDeleter> SocketPtr;
    typedef std::unique_ptr<Thread> ThreadPtr;

  public:
    UDPMessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder);
    virtual ~UDPMessageServer();
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
    SocketPtr m_udpSocket;
    ThreadPtr m_thread;
    Mutex m_mutex;
    ResetCompletionCallback m_callback;
  };
}
