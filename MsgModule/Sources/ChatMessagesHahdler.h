#pragma once
#include "IMessagesHahdler.h"
#include "MessagesQueue.h"

namespace msg
{
	class MessagesReceiver;

	class ChatMessagesHandler
		: public IMessagesHandler
	{
	public:
		ChatMessagesHandler(MessagesReceiver* msgReciever, ChatMessagesQueue* msgQueue);
		~ChatMessagesHandler();
		virtual void HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr);
	private:
		MessagesReceiver* m_msgReceiver;
		ChatMessagesQueue* m_msgQueue;
	};
}