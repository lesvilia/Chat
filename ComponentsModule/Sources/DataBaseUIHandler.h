#pragma once
#include <memory>
#include <vector>


namespace ui
{
  struct MessageInfo
  {
    MessageInfo(const std::wstring& username, const std::wstring& message,
                const std::wstring& time, bool isNetUser)
      : m_username(username)
      , m_message(message)
      , m_time(time)
      , m_isNetUser(isNetUser)
    {
    }
    std::wstring m_username;
    std::wstring m_message;
    std::wstring m_time;
    bool m_isNetUser;
  };
}

namespace db
{
  enum MessageType
  {
    TEXT_MSG,
    FILE_MSG
  };

  typedef std::vector<std::pair<MessageType, ui::MessageInfo> > MessageList;
  typedef std::shared_ptr<MessageList> MessageListPtr;

  class DataBaseUIHandler
  {
  public:
    virtual void AddLastConversations(const MessageListPtr& messages) = 0;
  protected:
    ~DataBaseUIHandler() {}
  };
}
