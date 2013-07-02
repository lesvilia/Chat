#include "stdafx.h"
#include "ChatMessagesHahdler.h"
#include "MessagesReceiver.h"

namespace msg
{
	ChatMessagesHandler::ChatMessagesHandler(MessagesReceiver* msgReceiver, ChatMessagesQueue* msgQueue)
		: m_msgReceiver(msgReceiver)
		, m_msgQueue(msgQueue)
	{
	}

	ChatMessagesHandler::~ChatMessagesHandler()
	{
	}

	void ChatMessagesHandler::HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr)
	{
		ChatMessageDataPtr newMessage; //initialize newMessage (PugiXML)
		m_msgQueue->Enqueue(newMessage);
		m_msgReceiver->OnChatMessageReceived();
	}
}