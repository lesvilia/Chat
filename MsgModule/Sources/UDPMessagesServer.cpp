#include "stdafx.h"
#include "UDPMessagesServer.h"

#include <vector>
#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"
#include "SettingsManager.h"
#include "LoginManager.h"
#include "IMessagesHandler.h"
#include "IServerSettingsHolder.h"

namespace msg
{
  namespace 
  {
    const ACE_Time_Value TIMEOUT(1); // 1 second
    
    void DeleteSocket(ACE_SOCK_Dgram* udpSocket)
    {
      udpSocket->close();
      delete udpSocket;
    }
  }

  UDPMessageServer::UDPMessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder)
    : MessageServer(handler, settingsHolder)
    , m_shouldShutdown(false)
    , m_needReset(false)
    , m_udpSocket(nullptr, DeleteSocket)
    , m_thread(nullptr)
  {
      Initialize();
  }

  UDPMessageServer::~UDPMessageServer()
  {
    if (m_thread->joinable())
    {
      Shutdown();
      m_thread->join();
    }
  }

  void UDPMessageServer::Run()
  {
    while (!ShouldShutdown())
    {
      if (login::LoginManager::Instance()->IsOnline())
      {
        ACE_INET_Addr userAddr;
        iovec buffer = {0};
        size_t result = m_udpSocket->recv(&buffer, userAddr, 0, &TIMEOUT);
        if ((result > 0) && buffer.iov_base)
        {
          std::wstring message((wchar_t*)buffer.iov_base, (wchar_t*)(buffer.iov_base + buffer.iov_len));
          if (!message.empty())
          {
            Handler()->HandleMessage(message, userAddr);
          }
          delete[] buffer.iov_base;
        }
      }

      if (NeedReset())
      {
        InitSocket();
      }
    }

    m_udpSocket.reset(); // if server should shutdown need close socket
  }

  void UDPMessageServer::Shutdown()
  {
      Lock lock(m_mutex);
      m_shouldShutdown = true;
  }

  void UDPMessageServer::Reset()
  {
    Lock lock(m_mutex);
    m_needReset = true;
  }

  void UDPMessageServer::Initialize()
  {
    InitSocket();
    m_thread.reset(new Thread(&UDPMessageServer::Run, this)); //start thread
  }

  void UDPMessageServer::InitSocket()
  {
    ACE_INET_Addr serverAddr(Port(), IPAddress().c_str());
    m_udpSocket.reset(new ACE_SOCK_Dgram(serverAddr));
  }

  bool UDPMessageServer::ShouldShutdown()
  {
    Lock lock(m_mutex);
    return m_shouldShutdown;
  }

  bool UDPMessageServer::NeedReset()
  {
    Lock lock(m_mutex);
    return m_needReset;
  }
}
