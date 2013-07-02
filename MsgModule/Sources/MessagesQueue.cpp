#include "stdafx.h"
#include "MessagesQueue.h"

namespace msg
{
	StateMessageData::StateMessageData()
	{
	}

	StateMessageData::StateMessageData(const ACE_INET_Addr& userAddr, const StateMessagePtr& userMessage)
		: m_userAddr(userAddr)
		, m_userMessage(userMessage)
	{
	}

	ChatMessageData::ChatMessageData()
	{
	}

	ChatMessageData::ChatMessageData(const ACE_INET_Addr& userAddr, const ChatMessagePtr& userMessage)
		: m_userAddr(userAddr)
		, m_userMessage(userMessage)
	{
	}
}