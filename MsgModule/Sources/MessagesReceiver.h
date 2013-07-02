#pragma once
#include "MessagesWindow.h"

namespace msg
{
	class MessagesReceiver
		: private boost::noncopyable
	{
	public:
		MessagesReceiver();
		~MessagesReceiver();
		void OnStateMessageReceived();
		void OnChatMessageReceived();

	private:
		MessagesWindowPtr m_window;
	};
}