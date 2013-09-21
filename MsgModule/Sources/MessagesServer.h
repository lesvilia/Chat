#pragma once
#include <string>
#include "WSAStartupHolder.h"
#include "boost\noncopyable.hpp"

namespace msg
{
  class IServerSettingsHolder;
  class IMessagesHandler;

  class MessageServer
    : private boost::noncopyable
  {
  public:
    virtual void Shutdown();
    virtual void Reset();
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