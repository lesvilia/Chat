#pragma once
#include <string>
#include "ace/OS.h"
#include "ace/CDR_Stream.h"

namespace msg
{
	enum State
	{
		CONNECT_STATE,
		DISCONNECT_STATE
	};

	class StateMessage
	{
	public:
		StateMessage();
		explicit StateMessage(State state);
		void SetState(int state);
		int GetState() const;
	private:
	  int m_state;
	};

	int operator<<(ACE_OutputCDR& cdr, const StateMessage& message);
	int operator>>(ACE_InputCDR& cdr, StateMessage& message);

	class ChatMessage
	{
	public:
		ChatMessage();
		explicit ChatMessage(const std::wstring& message);
		void SetData(const std::wstring& data);
		const wchar_t* GetData() const;
		size_t MsgLength() const;
	private:
		std::wstring m_message;
	};

	int operator<<(ACE_OutputCDR& cdr, const ChatMessage& message);
	int operator>>(ACE_InputCDR& cdr, ChatMessage& message);
}