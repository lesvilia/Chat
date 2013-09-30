
#include <algorithm>
#include "StateMessagesManager.h"
#include "StateMessagesHandler.h"
#include "UDPMessagesServer.h"
#include "SettingsManager.h"
#include "StateServerSettingsHolder.h"
#include "NetUsersManager.h"
#include "LoginManager.h"
#include "MessagesTemplates.h"
#include "ace/SOCK_Dgram_Bcast.h"
#include "boost/format.hpp"

namespace msg
{
  namespace
  {
    const unsigned short TEMP_PORT = 0;
  }

  StateMessagesManager* StateMessagesManager::Instance()
  {
    static StateMessagesManager manager;
    return &manager;
  }

  StateMessagesManager::StateMessagesManager()
    : m_msgQueue(new StateMessagesQueue())
    , m_settingsHolder(new StateServerSettingsHolder(sm::SettingsManager::Instance()))
    , m_activated(false)
  {
    login::LoginManager::Instance()->Subscribe(this);
  }

  StateMessagesManager::~StateMessagesManager()
  {
  }

  void StateMessagesManager::SendResponseToConnect(const std::wstring& addr)
  {
    net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
    if (wsaHolder.GetErrorCode() == 0)
    {
      std::wstring message(CreateMessage(CONNECT_RESPONSE_STATE));
      ACE_INET_Addr userAddr(m_settingsHolder->GetPort(), addr.c_str());
      ACE_INET_Addr currentAddr(TEMP_PORT, m_settingsHolder->GetAddress().c_str());
      ACE_SOCK_Dgram udpSocket(currentAddr);
      size_t res = udpSocket.send(message.c_str(), message.size() * sizeof(wchar_t), userAddr);
      udpSocket.close();
    }
  }

  void StateMessagesManager::SendMessageToUsers(State currentState)
  {
    net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
    if (wsaHolder.GetErrorCode() == 0)
    {
      std::wstring message(CreateMessage(currentState));
      ACE_INET_Addr currentAddr(TEMP_PORT, m_settingsHolder->GetAddress().c_str());
      ACE_SOCK_Dgram udpSocket(currentAddr);
      
      std::vector<std::wstring> usersAddresses(net::NetUsersManager::Instance()->GetNetUserAddresses());
      std::for_each(usersAddresses.begin(), usersAddresses.end(),
      [&](const std::wstring& addr)
      {
        ACE_INET_Addr userAddr(m_settingsHolder->GetPort(), addr.c_str());
        udpSocket.send(message.c_str(), message.size() * sizeof(wchar_t), userAddr);
      });
      udpSocket.close();
    }
  }

  void StateMessagesManager::SendBroadcastMessage(State currentState)
  {
    net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
    if (wsaHolder.GetErrorCode() == 0)
    {
      std::wstring message(CreateMessage(currentState));
      ACE_INET_Addr currentAddr(TEMP_PORT, m_settingsHolder->GetAddress().c_str());
      ACE_SOCK_Dgram_Bcast udpBroadcastSocket(currentAddr);
      size_t result = udpBroadcastSocket.send(message.c_str(), message.size() * sizeof(wchar_t), m_settingsHolder->GetPort());
      udpBroadcastSocket.close();
    }
  }

  void StateMessagesManager::Activate(MessagesReceiver* receiver)
  {
    if (!m_activated)
    {
      m_msgHandler.reset(new StateMessagesHandler(receiver, m_msgQueue.get()));
      m_server.reset(new UDPMessageServer(m_msgHandler.get(), m_settingsHolder.get())); //init socket and run server
      m_activated = true;
    }
  }


  void StateMessagesManager::Deactivate()
  {
    m_server->Shutdown();
    m_activated = false;
  }

  StateMessagesQueue* StateMessagesManager::GetMessagesQueue() const
  {
    return m_msgQueue.get();
  }

  void StateMessagesManager::ResetServer()
  {
    SendMessageToUsers(DISCONNECT_STATE);
    m_server->Reset();
    net::NetUsersManager::Instance()->ClearOnlineUsers();
    SendBroadcastMessage(CONNECT_REQUEST_STATE);
  }

  std::wstring StateMessagesManager::CreateMessage(State currentState)
  {
    try
    {
      login::UserDataPtr user(login::LoginManager::Instance()->GetCurrentUser());
      return boost::str(boost::wformat(STATE_MESSAGE_FORMAT) % user->uuid % user->name % currentState);
    }
    catch (const std::exception&) 
    {
      return std::wstring();
    }
  }

  void StateMessagesManager::OnlineStateChanged()
  {
    if (login::LoginManager::Instance()->IsOnline())
    {
      SendBroadcastMessage(CONNECT_REQUEST_STATE);
    }
    else
    {
      SendMessageToUsers(DISCONNECT_STATE);
      net::NetUsersManager::Instance()->ClearOnlineUsers();
    }
  }
}
