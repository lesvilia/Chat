#pragma once
#include "boost/noncopyable.hpp"
#include "LoginHandlers.h"
#include "MessagesQueue.h"

namespace msg
{
	class MessagesReceiver;
	class StateMessagesHandler;
	class UDPMessageServer;
	class StateServerSettingsHolder;

	class StateMessagesManager
		: private boost::noncopyable
		, public login::ILoginStateObserver
	{
	public:
		static StateMessagesManager* Instance();
		void SendResponseToConnect(const ACE_INET_Addr& userAddr);
		void Activate(MessagesReceiver* receiver);
		StateMessagesQueue* GetMessagesQueue();
		void ResetServer();
		virtual void OnlineStateChanged();

	private:
		StateMessagesManager();
		~StateMessagesManager();
		std::wstring CreateMessage(State currentState);
		void SendMessageToUsers(State currentState);
		void SendBroadcastMessage(State currentState);

	private:
		std::unique_ptr<StateMessagesQueue> m_msgQueue;
		std::unique_ptr<StateServerSettingsHolder> m_settingsHolder;
		std::unique_ptr<StateMessagesHandler> m_msgHandler;
		std::unique_ptr<UDPMessageServer> m_server;
	};
}