#include "stdafx.h"
#include "MessagesQueue.h"

namespace msg
{
	StateMessageData::StateMessageData()
	{
	}

	StateMessageData::StateMessageData(const ACE_INET_Addr& userAddr, const StateMessage& userMessage)
		: m_userAddr(userAddr)
		, m_userMessage(userMessage)
	{
	}

	ChatMessageData::ChatMessageData()
	{
	}

	ChatMessageData::ChatMessageData(const ACE_INET_Addr& userAddr, const ChatMessage& userMessage)
		: m_userAddr(userAddr)
		, m_userMessage(userMessage)
	{
	}
}