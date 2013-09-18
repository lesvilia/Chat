#pragma once
#include <string>

#include <QSplitter>

namespace ui
{
  struct MessageInfo
  {
    MessageInfo(const std::wstring& username, const std::wstring& message,
      const std::wstring& time, bool isNetUser = false);
    std::wstring m_username;
    std::wstring m_message;
    std::wstring m_time;
    bool m_isNetUser;
  };

  struct FileMessageInfo
  {
  };

  namespace controls
  {
    class UsersMessageView
      : public QSplitter
    {
      enum
      {
        VIEW_SECTION_ID,
        EDIT_SECTION_ID,
      };

      enum
      {
        NAME_COLUMN,
        MESSAGE_COLUMN,
        TIME_COLUMN,

        COLUMN_COUNT //always last
      };

    public:
      UsersMessageView();
      virtual ~UsersMessageView();
      bool GetTextFromEdit(std::wstring* msg);
      void AppendMessage(const MessageInfo& msg);
      void InsertMessage(const MessageInfo& msg, int row);

    private:
      void CreateSubControls();
    };
  }
}
