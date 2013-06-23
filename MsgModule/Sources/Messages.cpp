#include "stdafx.h"
#include "Messages.h"
#include <vector>

namespace msg
{
	namespace
	{
		void ReadIntFromCDR(ACE_InputCDR& cdr, int& outInt)
		{
			ACE_CDR::ULong value = 0;
			if (cdr >> value)
			{
				outInt = value;
			}
		}

		void ReadWstringFromCDR(ACE_InputCDR& cdr, std::wstring& outStr)
		{
			ACE_CDR::ULong length = 0;
			if (cdr >> length)
			{
				std::vector<wchar_t> tempBuffer(length + 1, '\0');
				cdr.read_wchar_array(&tempBuffer[0], length);
				outStr.swap(std::wstring(&tempBuffer[0]));
			}
		}
	}

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

	int operator<<(ACE_OutputCDR& cdr, const StateMessage& message)
	{
		cdr << ACE_CDR::ULong(message.m_state);
		
		cdr << ACE_CDR::ULong(message.m_uuid.size());
		cdr.write_wchar_array(message.m_uuid.c_str(), message.m_uuid.size());
		
		cdr << ACE_CDR::ULong(message.m_username.size());
		cdr.write_wchar_array(message.m_username.c_str(), message.m_username.size());
		
		return cdr.good_bit();
	}

	int operator>>(ACE_InputCDR& cdr, StateMessage& message)
	{
		ReadIntFromCDR(cdr, message.m_state);
		ReadWstringFromCDR(cdr, message.m_uuid);
		ReadWstringFromCDR(cdr, message.m_username);

		return cdr.good_bit();
	}
	

	ChatMessage::ChatMessage()
	{
	}

	ChatMessage::ChatMessage(const std::wstring& uuid, const std::wstring& message)
		: m_uuid(uuid)
		, m_message(message)
	{
	}

	

	int operator<<(ACE_OutputCDR& cdr, const ChatMessage& message)
	{
		cdr << ACE_CDR::ULong(message.m_uuid.size());
		cdr.write_wchar_array(message.m_uuid.c_str(), message.m_uuid.size());

		cdr << ACE_CDR::ULong(message.m_message.size());
		cdr.write_wchar_array(message.m_message.c_str(), message.m_message.size());

		return cdr.good_bit();
	}

	int operator>>(ACE_InputCDR& cdr, ChatMessage& message)
	{
		ReadWstringFromCDR(cdr, message.m_uuid);
		ReadWstringFromCDR(cdr, message.m_message);
		return cdr.good_bit();
	}

}