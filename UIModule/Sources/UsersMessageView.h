#pragma once
#include <string>

#include <QSplitter>

class QTableWidget;
class IDropResultHandler;

namespace ui
{
  namespace controls
  {
    class DnDTextEdit;
  }
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

  class UsersMessageView
    : public QSplitter
  {
    enum
    {
      NAME_COLUMN,
      MESSAGE_COLUMN,
      TIME_COLUMN,
      COLUMN_COUNT //always last
    };

  public:
    explicit UsersMessageView(IDropResultHandler* dropHandler);
    virtual ~UsersMessageView();
    bool GetTextFromEdit(std::wstring* msg);
    void AppendMessage(const MessageInfo& msg);
    void InsertMessage(const MessageInfo& msg, int rowNum);

  private:
    void CreateSubControls(IDropResultHandler* dropHandler);
    void ClearMessageEdit();
  private:
    QTableWidget* m_msgView;
    controls::DnDTextEdit* m_msgEdit;
  };
}
