#pragma once

namespace msg
{
	class StateMessage;
	class ChatMessage;

	class IStateMsgHandler
	{
	public:
		virtual void HandleMessage(const StateMessage& msg) = 0;
	protected:
		~IStateMsgHandler();
	};

	class IChatMsgHandler
	{
	public:
		virtual void HandleMessage(const ChatMessage& msg) = 0;
	protected:
		~IChatMsgHandler();
	};

}