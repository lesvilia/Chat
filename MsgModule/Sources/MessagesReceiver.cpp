#include "stdafx.h"
#include "MessagesReceiver.h"

namespace msg
{
	MessagesReceiver::MessagesReceiver()
		: m_window(nullptr)
	{
		m_window.reset(new MessagesWindow(this));
	}

	MessagesReceiver::~MessagesReceiver()
	{
	}

	void MessagesReceiver::OnStateMessageReceived()
	{
		m_window->OnStateMessageReceived();
	}

	void MessagesReceiver::OnChatMessageReceived()
	{
		m_window->OnChatMessageReceived();
	}
}