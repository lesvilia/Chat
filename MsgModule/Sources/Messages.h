#pragma once
#include <string>
#include "ace/OS.h"
#include "ace/CDR_Stream.h"

namespace msg
{
	enum State
	{
		UNDEFINED_STATE,
		CONNECT_REQUEST_STATE,
		CONNECT_RESPONSE_STATE,
		DISCONNECT_STATE
	};

	class StateMessage
	{
	public:
		StateMessage();
		StateMessage(int state, const std::wstring& uuid, const std::wstring& username);
		int m_state;
		std::wstring m_uuid;
		std::wstring m_username;
	};

	int operator<<(ACE_OutputCDR& cdr, const StateMessage& message);
	int operator>>(ACE_InputCDR& cdr, StateMessage& message);

	class ChatMessage
	{
	public:
		ChatMessage();
		ChatMessage(const std::wstring& uuid, const std::wstring& message);
		std::wstring m_uuid;
		std::wstring m_message;
	};

	int operator<<(ACE_OutputCDR& cdr, const ChatMessage& message);
	int operator>>(ACE_InputCDR& cdr, ChatMessage& message);
}