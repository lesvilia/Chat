#pragma once
#include "MessageWindow.h"

namespace msg
{
	class MessageReciever
		: private boost::noncopyable
	{
	public:
		MessageReciever();
		~MessageReciever();
		void OnStateMessageReceived();
		void OnChatMessageReceived();

	private:
		MessageWindowPtr m_window;
	};
}