#pragma once
#include <string>
#include "WSAStartupHolder.h"
#include "boost/noncopyable.hpp"
#include "boost/function.hpp"

namespace msg
{
  class IServerSettingsHolder;
  class IMessagesHandler;

  typedef boost::function<void ()> ResetCompletionCallback;
  class MessageServer
    : private boost::noncopyable
  {
  public:
    virtual void Shutdown();
    virtual void Reset(const ResetCompletionCallback& callback = ResetCompletionCallback());
    virtual ~MessageServer();

  private:
    virtual void Run();

  protected:
    MessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder);
    IMessagesHandler* Handler() const;
    unsigned short Port() const;
    std::wstring IPAddress() const;

  private:
    net::WSAStartupHolder m_startupHolder;
    IMessagesHandler* m_msgHandler;
    IServerSettingsHolder* m_settingsHolder;
  };
}
