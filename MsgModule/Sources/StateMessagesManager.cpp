#include "stdafx.h"
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

	StateMessageManager* StateMessageManager::Instance()
	{
		static StateMessageManager manager;
		return &manager;
	}

	StateMessageManager::StateMessageManager()
		: m_msgQueue(new StateMessagesQueue())
		, m_settingsHolder(new StateServerSettingsHolder(sm::SettingsManager::Instance()))
	{
		login::LoginManager::Instance()->Subscribe(this);
	}

	StateMessageManager::~StateMessageManager()
	{
		m_server->Shutdown();
	}

	void StateMessageManager::SendMessage(State currentState)
	{
		net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
		if (wsaHolder.GetErrorCode() == 0)
		{
			std::wstring message(CreateMessage(currentState));
			ACE_INET_Addr currentAddr(TEMP_PORT, sm::SettingsManager::Instance()->GetCurrentNetAddres().c_str());
			ACE_SOCK_Dgram udpSocket(currentAddr);
			
			std::vector<ACE_INET_Addr> usersAddresses(net::NetUsersManager::Instance()->GetNetUserAddresses());
			std::for_each(usersAddresses.begin(), usersAddresses.end(),
			[&](const ACE_INET_Addr& addr)
			{
				udpSocket.send(message.c_str(), message.size() * sizeof(wchar_t), addr);
			});
			udpSocket.close();
		}
	}

	void StateMessageManager::SendBroadcastMessage(State currentState)
	{
		net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
		if (wsaHolder.GetErrorCode() == 0)
		{
			std::wstring message(CreateMessage(currentState));
			ACE_INET_Addr currentAddr(TEMP_PORT, sm::SettingsManager::Instance()->GetCurrentNetAddres().c_str());
			ACE_SOCK_Dgram_Bcast udpBroadcastSocket(currentAddr);
			udpBroadcastSocket.send(message.c_str(), message.size() * sizeof(wchar_t), sm::SettingsManager::Instance()->GetCurrentStatesPort());
			udpBroadcastSocket.close();
		}
	}

	void StateMessageManager::Activate(MessagesReceiver* receiver)
	{
		if (!m_server)
		{
			m_msgHandler.reset(new StateMessagesHandler(receiver, m_msgQueue.get()));
			m_server.reset(new UDPMessageServer(m_msgHandler.get(), m_settingsHolder.get())); //init socket and run server
		}
	}

	StateMessagesQueue* StateMessageManager::GetMessagesQueue()
	{
		return m_msgQueue.get();
	}

	void StateMessageManager::ResetServer()
	{
		m_server->Reset();
	}

	std::wstring StateMessageManager::CreateMessage(State currentState)
	{
		using namespace login;
		UserDataPtr user(LoginManager::Instance()->GetCurrentUser());
		return std::wstring((boost::wformat(STATE_MESSAGE) % user->uuid % user->name % currentState).str());
	}

	void StateMessageManager::OnlineStateChanged()
	{
		if (login::LoginManager::Instance()->IsOnline())
		{
			SendBroadcastMessage(CONNECT_REQUEST_STATE);
			m_server->Notify();
		}
		else
		{
			SendMessage(DISCONNECT_STATE);
		}
	}
}