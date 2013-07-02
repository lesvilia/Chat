#pragma once
#include <memory>
#include <queue>
#include "boost/noncopyable.hpp"
#include "boost/thread/mutex.hpp"
#include "Messages.h"
#include "Typedefs.h"
#include "ace/INET_Addr.h"

namespace msg
{
	struct StateMessageData
	{
		StateMessageData();
		StateMessageData(const ACE_INET_Addr& userAddr, const StateMessagePtr& userMessage);
		ACE_INET_Addr m_userAddr;
		StateMessagePtr m_userMessage;
	};

	struct ChatMessageData
	{
		ChatMessageData();
		ChatMessageData(const ACE_INET_Addr& userAddr, const ChatMessagePtr& userMessage);
		ACE_INET_Addr m_userAddr;
		ChatMessagePtr m_userMessage;
	};

	template<typename DataType>
	class MessagesQueue
		: private boost::noncopyable
	{
	public:
		MessagesQueue()
		{
		}

		void Enqueue(const DataType& msgData)
		{
			Lock lock(m_mutex);
			m_messages.push(msgData);
		}

		DataType Dequeue()
		{
			Lock lock(m_mutex);
			if (!m_messages.empty())
			{
				DataType data(m_messages.front());
				m_messages.pop();
				return data;
			}
			else
			{
				return DataType();
			}
		}

	private:
		std::queue<DataType> m_messages;
		Mutex m_mutex;
	};

	typedef std::shared_ptr<StateMessageData> StateMessageDataPtr;
	typedef std::shared_ptr<ChatMessageData> ChatMessageDataPtr;
	
	typedef MessagesQueue<StateMessageDataPtr> StateMessagesQueue;
	typedef MessagesQueue<ChatMessageDataPtr> ChatMessagesQueue;
}