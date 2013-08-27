#pragma once
#include "boost/noncopyable.hpp"
#include "LoginHandlers.h"
#include "MessagesQueue.h"

namespace msg
{
	class MessagesReceiver;
	class ChatMessagesHandler;
	class UDPMessageServer;
	class ChatServerSettingsHolder;

	class ChatMessagesManager
		: private boost::noncopyable
		, public login::ILoginStateObserver
	{
	public:
		static ChatMessagesManager* Instance();
		void Send(const std::wstring& uuid, const std::wstring& txtMessage);
		void Activate(MessagesReceiver* receiver);
    void Deactivate();
		ChatMessagesQueue* GetMessagesQueue();
		void ResetServer();
		virtual void OnlineStateChanged();

	private:
		ChatMessagesManager();
		~ChatMessagesManager();
		std::wstring CreateMessage(const std::wstring& txtMessage);

	private:
		std::unique_ptr<ChatMessagesQueue> m_msgQueue;
		std::unique_ptr<ChatServerSettingsHolder> m_settingsHolder;
    bool m_activated;
		std::unique_ptr<ChatMessagesHandler> m_msgHandler;
		std::unique_ptr<UDPMessageServer> m_server;
	};
}
