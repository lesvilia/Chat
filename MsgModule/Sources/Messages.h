#pragma once
#include <memory>
#include <string>

namespace msg
{
  enum State
  {
    UNDEFINED_STATE,
    CONNECT_REQUEST_STATE,
    CONNECT_RESPONSE_STATE,
    DISCONNECT_STATE
  };

  struct StateMessage
  {
    StateMessage();
    StateMessage(int state, const std::wstring& uuid, const std::wstring& username);
    int m_state;
    std::wstring m_uuid;
    std::wstring m_username;
  };

  struct ChatMessage
  {
    ChatMessage();
    ChatMessage(const std::wstring& uuid, const std::wstring& message);
    std::wstring m_uuid;
    std::wstring m_message;
  };

  typedef std::shared_ptr<StateMessage> StateMessagePtr;
  typedef std::shared_ptr<ChatMessage> ChatMessagePtr;
}