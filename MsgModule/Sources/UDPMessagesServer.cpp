#include "stdafx.h"
#include <vector>
#include "UDPMessagesServer.h"
#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"
#include "SettingsManager.h"
#include "LoginManager.h"
#include "IMessagesHahdler.h"
#include "IServerSettingsHolder.h"

namespace msg
{
	namespace 
	{
		const ACE_Time_Value TIMEOUT(1); // 1 second
		const size_t BUFFER_SISE = 512;

		void DeleteSocket(ACE_SOCK_Dgram* udpSocket)
		{
			udpSocket->close();
			delete udpSocket;
		}
	}

	UDPMessageServer::UDPMessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder)
		: m_startupHolder(MAKEWORD(2, 2))
		, m_msgHandler(handler)
		, m_settingsHolder(settingsHolder)
		, m_shouldShutdown(false)
		, m_needReset(false)
		, m_udpSocket(nullptr, DeleteSocket)
		, m_thread(nullptr)
	{
		if (m_startupHolder.GetErrorCode() == 0)
		{
			Initialize();
		}
	}

	UDPMessageServer::~UDPMessageServer()
	{
		m_thread->join();
	}

	void UDPMessageServer::Run()
	{
		while (!ShouldShutdown())
		{
			if (login::LoginManager::Instance()->IsOnline())
			{
				ACE_INET_Addr userAddr;
				std::vector<wchar_t> buff(BUFFER_SISE, '\0');
				size_t result = m_udpSocket->recv(&buff[0], BUFFER_SISE * sizeof(wchar_t), userAddr, 0, &TIMEOUT);
				if (result > 0)
				{
					m_msgHandler->HandleMessage(&buff[0], userAddr);
				}
			}
			else
			{
				WaitForEvents();
			}

			if (NeedReset())
			{
				InitSocket();
			}
		}
	}

	void UDPMessageServer::Shutdown()
	{
		Lock lock(m_mutex);
		m_shouldShutdown = true;
		m_condVariable.notify_one();
	}

	void UDPMessageServer::Reset()
	{
		Lock lock(m_mutex);
		m_needReset = true;
	}

	void UDPMessageServer::Notify()
	{
		m_condVariable.notify_one();
	}

	void UDPMessageServer::Initialize()
	{
		InitSocket();
		m_thread.reset(new Thread(&UDPMessageServer::Run, this)); //start thread
	}

	void UDPMessageServer::InitSocket()
	{
		ACE_INET_Addr serverAddr(m_settingsHolder->GetPort(), m_settingsHolder->GetAddress().c_str());
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
	
	void UDPMessageServer::WaitForEvents()
	{
		Lock lock(m_mutex);
		m_condVariable.wait(lock);
	}
}