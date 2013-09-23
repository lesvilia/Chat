#include "stdafx.h"
#include "TCPMessagesServer.h"

#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "SettingsManager.h"
#include "LoginManager.h"
#include "IMessagesHandler.h"
#include "IServerSettingsHolder.h"

namespace
{
  ACE_Time_Value TIMEOUT(1); // 1 second

  void DeleteAcceptor(ACE_SOCK_Acceptor* acceptor)
  {
    acceptor->close();
    delete acceptor;
  }

  void DeleteStream(ACE_SOCK_Stream* stream)
  {
    stream->close();
    delete stream;
  }
}

namespace msg
{
  TCPMessageServer::TCPMessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder)
    : MessageServer(handler, settingsHolder)
    , m_shouldShutdown(false)
    , m_needReset(false)
    , m_acceptor(nullptr, DeleteAcceptor)
  {
    Initialize();
  }

  TCPMessageServer::~TCPMessageServer()
  {
    if (m_thread->joinable())
    {
      Shutdown();
      m_thread->join();
    }
  }
  
  void TCPMessageServer::Run()
  {
    while (!ShouldShutdown())
    {
      if (login::LoginManager::Instance()->IsOnline())
      {
        ACE_Time_Value timeOut(1); // 1 second
        ACE_INET_Addr userAddr;
        SocketStream socketStream(new ACE_SOCK_Stream(), DeleteStream);
        if (m_acceptor->accept(*socketStream, &userAddr, &timeOut) != -1)
        {
          Handler()->HandleConnect(socketStream);
        }
      }

      if (NeedReset())
      {
        InitSocket();
      }
    }
    m_acceptor.reset();
  }

  void TCPMessageServer::Shutdown()
  {
    LockGuard lock(m_mutex);
    m_shouldShutdown = true;
  }

  void TCPMessageServer::Reset()
  {
    LockGuard lock(m_mutex);
    m_needReset = true;
  }

  void TCPMessageServer::Initialize()
  {
    InitSocket();
    m_thread.reset(new Thread(&TCPMessageServer::Run, this));
  }

  void TCPMessageServer::InitSocket()
  {
    ACE_INET_Addr addr(Port(), IPAddress().c_str());
    m_acceptor.reset(new ACE_SOCK_Acceptor(addr, 1));
  }

  bool TCPMessageServer::ShouldShutdown()
  {
    LockGuard lock(m_mutex);
    return m_shouldShutdown;
  }

  bool TCPMessageServer::NeedReset()
  {
    LockGuard lock(m_mutex);
    return m_needReset;
  }
}
