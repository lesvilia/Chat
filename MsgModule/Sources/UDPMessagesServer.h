#pragma once
#include <memory>
#include "ace/SOCK_Dgram.h"
#include "Typedefs.h"
#include "WSAStartupHolder.h"

namespace msg
{
	class IMessagesHandler;
	class IServerSettingsHolder;

	class UDPMessageServer
	{
		typedef void (*SocketDeleter)(ACE_SOCK_Dgram*);
		typedef std::unique_ptr<ACE_SOCK_Dgram, SocketDeleter> SocketPtr;
		typedef std::unique_ptr<Thread> ThreadPtr;

	public:
		UDPMessageServer(IMessagesHandler* handler, IServerSettingsHolder* settingsHolder);
		~UDPMessageServer();
		void Run();
		void Shutdown();
		void Reset();
		void Notify();

	private:
		void Initialize();
		void InitSocket();
		void ResetSocket();
		bool ShouldShutdown();
		bool NeedReset();
		void WaitForEvents();

	private:
		net::WSAStartupHolder m_startupHolder;
		IMessagesHandler* m_msgHandler;
		IServerSettingsHolder* m_settingsHolder;
		bool m_shouldShutdown;
		bool m_needReset;
		SocketPtr m_udpSocket;
		ThreadPtr m_thread;
		Mutex m_mutex;
		ConditionVariable m_condVariable;
	};
}