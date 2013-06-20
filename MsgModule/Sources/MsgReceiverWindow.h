#pragma once

#include <Windows.h>
#include "MessageHandlers.h"
#include "boost\noncopyable.hpp"


namespace msg
{
	class StateMsgQueue;
	class ChatMsgQueue;

	class MsgReceiverWindow 
		: private boost::noncopyable
	{
	public:
		MsgReceiverWindow(IStateMsgHandler* stateHandler, IChatMsgHandler* chatHandler);
		~MsgReceiverWindow();
		void OnStateMessageReceived(StateMsgQueue* queue);
		void OnChatMessageReceived(ChatMsgQueue* queue);

	private:
		void CreateMainWindow();
		void RegisterMainWindow();

	private:
		HWND m_window;
		IStateMsgHandler* m_stateHandler;
		IChatMsgHandler* m_chatHandler;
	};
}