
#include "MessagesQueue.h"

namespace msg
{
  StateMessageData::StateMessageData()
  {
  }

  StateMessageData::StateMessageData(const std::wstring& userAddr, const StateMessagePtr& userMessage)
    : m_userAddr(userAddr)
    , m_userMessage(userMessage)
  {
  }

  ChatMessageData::ChatMessageData()
  {
  }

  ChatMessageData::ChatMessageData(const std::wstring& userAddr, const ChatMessagePtr& userMessage)
    : m_userAddr(userAddr)
    , m_userMessage(userMessage)
  {
  }
}