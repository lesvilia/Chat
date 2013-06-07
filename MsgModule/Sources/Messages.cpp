#include "stdafx.h"
#include "Messages.h"
#include <vector>

namespace msg
{
	StateMessage::StateMessage()
	{
	}

	StateMessage::StateMessage(State state)
		: m_state(state)
	{
	}

	void StateMessage::SetState(int state)
	{
		m_state = state;
	}

	int StateMessage::GetState()const
	{
		return m_state;
	}
	
	int operator<<(ACE_OutputCDR& cdr, const StateMessage& message)
	{
		cdr << ACE_CDR::ULong(message.GetState());
		return cdr.good_bit();
	}

	int operator>>(ACE_InputCDR& cdr, StateMessage& message)
	{
		ACE_CDR::ULong state = 0;
		cdr >> state;
		message.SetState(state);
		return cdr.good_bit();
	}
	

	ChatMessage::ChatMessage()
	{
	}

	ChatMessage::ChatMessage(const std::wstring& message)
		: m_message(message)
	{
	}

	const wchar_t* ChatMessage::GetData() const
	{
		return m_message.c_str();
	}

	void ChatMessage::SetData(const std::wstring& data)
	{
		m_message = data;
	}

	size_t ChatMessage::MsgLength() const
	{
		return m_message.size();
	}
	

	int operator<<(ACE_OutputCDR& cdr, const ChatMessage& message)
	{
		size_t msgLen = message.MsgLength();
		cdr << ACE_CDR::ULong(msgLen);
		cdr.write_wchar_array(message.GetData(), msgLen);
		return cdr.good_bit();
	}

	int operator>>(ACE_InputCDR& cdr, ChatMessage& message)
	{
		ACE_CDR::ULong bufferLen = 0;
		if (cdr >> bufferLen)
		{
			std::vector<wchar_t> tempBuffer(bufferLen + 1, '\0');
			cdr.read_wchar_array(&tempBuffer[0], bufferLen);
			message.SetData(&tempBuffer[0]);
		}
		return cdr.good_bit();
	}

}