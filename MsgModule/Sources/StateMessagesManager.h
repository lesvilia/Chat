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

	class StateMessageManager
		: private boost::noncopyable
		, public login::ILoginStateObserver
	{
	public:
		static StateMessageManager* Instance();
		void SendMessage(State currentState);
		void SendBroadcastMessage(State currentState);
		void Activate(MessagesReceiver* receiver);
		StateMessagesQueue* GetMessagesQueue();
		void ResetServer();
		virtual void OnlineStateChanged();

	private:
		StateMessageManager();
		~StateMessageManager();
		std::wstring CreateMessage(State currentState);

	private:
		std::unique_ptr<StateMessagesQueue> m_msgQueue;
		std::unique_ptr<StateServerSettingsHolder> m_settingsHolder;
		std::unique_ptr<StateMessagesHandler> m_msgHandler;
		std::unique_ptr<UDPMessageServer> m_server;
	};
}