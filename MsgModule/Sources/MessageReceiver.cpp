#include "stdafx.h"
#include "MessageReceiver.h"

namespace msg
{
	MessageReciever::MessageReciever()
		: m_window(nullptr)
	{
		m_window.reset(new MessageWindow(this));
	}

	MessageReciever::~MessageReciever()
	{
	}

	void MessageReciever::OnStateMessageReceived()
	{
		m_window->OnStateMessageReceived();
	}

	void MessageReciever::OnChatMessageReceived()
	{
		m_window->OnChatMessageReceived();
	}
}