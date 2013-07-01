#pragma once

#include <memory>
#include <Windows.h>
#include "boost\noncopyable.hpp"


namespace msg
{
	class MessageReciever;

	class MessageWindow 
		: private boost::noncopyable
	{
	public:
		MessageWindow(MessageReciever* msgReceiver);
		~MessageWindow();
		void OnStateMessageReceived();
		void OnChatMessageReceived();

	private:
		void CreateMainWindow();
		void RegisterMainWindow();

	private:
		HWND m_window;
		MessageReciever* m_msgReceiver;
	};

	typedef std::unique_ptr<MessageWindow> MessageWindowPtr;
}