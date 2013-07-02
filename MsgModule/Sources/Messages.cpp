#include "stdafx.h"
#include "Messages.h"
#include <vector>

namespace msg
{
	StateMessage::StateMessage()
		:m_state(UNDEFINED_STATE)
	{
	}

	StateMessage::StateMessage(int state, const std::wstring& uuid, const std::wstring& username)
		: m_state(state)
		, m_uuid(uuid)
		, m_username(username)
	{
	}

	ChatMessage::ChatMessage()
	{
	}

	ChatMessage::ChatMessage(const std::wstring& uuid, const std::wstring& message)
		: m_uuid(uuid)
		, m_message(message)
	{
	}
}