#include "stdafx.h"
#include "ChatMessagesManager.h"
#include "ChatMessagesHandler.h"
#include "UDPMessagesServer.h"
#include "SettingsManager.h"
#include "ChatServerSettingsHolder.h"
#include "NetUsersManager.h"
#include "LoginManager.h"
#include "MessagesTemplates.h"
#include "ace/SOCK_Dgram.h"
#include "boost/format.hpp"

namespace msg
{
	namespace
	{
		const unsigned short TEMP_PORT = 0;
	}

	ChatMessagesManager* ChatMessagesManager::Instance()
	{
		static ChatMessagesManager manager;
		return &manager;
	}

	ChatMessagesManager::ChatMessagesManager()
		: m_msgQueue(new ChatMessagesQueue())
		, m_settingsHolder(new ChatServerSettingsHolder(sm::SettingsManager::Instance()))
	{
		login::LoginManager::Instance()->Subscribe(this);
	}

	ChatMessagesManager::~ChatMessagesManager()
	{
		m_server->Shutdown();
	}

	void ChatMessagesManager::Send(const std::wstring& uuid, const std::wstring& txtMessage)
	{
		net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
		if (wsaHolder.GetErrorCode() == 0)
		{
			std::wstring message(CreateMessage(txtMessage));
			ACE_INET_Addr currentAddr(TEMP_PORT, m_settingsHolder->GetAddress().c_str());
			ACE_SOCK_Dgram udpSocket(currentAddr);
			std::wstring addr(net::NetUsersManager::Instance()->GetNetUserAddress(uuid));
			ACE_INET_Addr userAddr(m_settingsHolder->GetPort(), addr.c_str());

			size_t res = udpSocket.send(message.c_str(), message.size() * sizeof(wchar_t), userAddr);
			udpSocket.close();
		}
	}

	void ChatMessagesManager::Activate(MessagesReceiver* receiver)
	{
		if (!m_server)
		{
			m_msgHandler.reset(new ChatMessagesHandler(receiver, m_msgQueue.get()));
			m_server.reset(new UDPMessageServer(m_msgHandler.get(), m_settingsHolder.get())); //init socket and run server
		}
	}

	ChatMessagesQueue* ChatMessagesManager::GetMessagesQueue()
	{
		return m_msgQueue.get();
	}

	void ChatMessagesManager::ResetServer()
	{
		m_server->Reset();
	}

	std::wstring ChatMessagesManager::CreateMessage(const std::wstring& txtMessage)
	{
		try
		{
			login::UserDataPtr user(login::LoginManager::Instance()->GetCurrentUser());
			return boost::str(boost::wformat(CHAT_MESSAGE_FORMAT) % user->uuid % txtMessage);
		}
		catch (const std::exception&) 
		{
			return std::wstring();
		}
	}

	void ChatMessagesManager::OnlineStateChanged()
	{
		if (login::LoginManager::Instance()->IsOnline())
		{
			m_server->Notify();
		}
	}
}