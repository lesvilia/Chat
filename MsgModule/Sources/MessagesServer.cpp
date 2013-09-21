#include "stdafx.h"
#include "MessagesServer.h"

#include "IServerSettingsHolder.h"
#include "IMessagesHandler.h"

namespace msg
{
  MessageServer::MessageServer(IMessagesHandler* handler,
                                 IServerSettingsHolder* settingsHolder)
    : m_startupHolder(MAKEWORD(2, 2))
    , m_msgHandler(handler)
    , m_settingsHolder(settingsHolder)
  {
    assert(m_startupHolder.GetErrorCode() == 0);
  }

  MessageServer::~MessageServer()
  {
  }

  void MessageServer::Run()
  {
  }

  void MessageServer::Shutdown()
  {
  }

  void MessageServer::Reset()
  {
  }

  IMessagesHandler* MessageServer::Handler() const
  {
    return m_msgHandler;
  }

  unsigned short MessageServer::Port() const
  {
    return m_settingsHolder->GetPort();
  }
  std::wstring MessageServer::IPAddress() const
  {
    return m_settingsHolder->GetAddress();
  }
}