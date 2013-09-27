#pragma once
#include <memory>
#include <queue>
#include "boost/noncopyable.hpp"
#include "boost/thread/mutex.hpp"
#include "Messages.h"
#include "Typedefs.h"

namespace msg
{
  struct StateMessageData
  {
    StateMessageData();
    StateMessageData(const std::wstring& userAddr, const StateMessagePtr& userMessage);
    std::wstring m_userAddr;
    StateMessagePtr m_userMessage;
  };

  struct ChatMessageData
  {
    ChatMessageData();
    ChatMessageData(const std::wstring& userAddr, const ChatMessagePtr& userMessage);
    std::wstring m_userAddr;
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
      m_cond.notify_all();
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

    template<typename Duration>
    DataType DequeueWait(const Duration& duration)
    {
      Lock lock(m_mutex);
      const boost::system_time timeout = boost::get_system_time() + duration;
      if (m_cond.timed_wait(lock, timeout,
          [this]()->bool { return !m_messages.empty(); }))
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
    ConditionVariable m_cond;
  };

  typedef std::shared_ptr<StateMessageData> StateMessageDataPtr;
  typedef std::shared_ptr<ChatMessageData> ChatMessageDataPtr;
  
  typedef MessagesQueue<StateMessageDataPtr> StateMessagesQueue;
  typedef MessagesQueue<ChatMessageDataPtr> ChatMessagesQueue;
}
