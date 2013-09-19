#pragma once
#include <string>

#include <QSplitter>

class QTableWidget;
class QWidget;
class QProgressBar;

namespace ui
{
  class IDropResultHandler;
  class IProgressUIObserver;
  class ItemCreator;

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
    void AppendTxtMessage(const MessageInfo& msg);
    QProgressBar* AppendFileMessage(const MessageInfo& msg);
    void InsertTxtMessageFromDB(const MessageInfo& msg, int rowNum);
    void InsertFileMessageFromDB(const MessageInfo& msg, int rowNum);

  private:
    void InsertMessageImpl(const MessageInfo& msg, int rowNum, const ItemCreator& creator);
    void CreateSubControls(IDropResultHandler* dropHandler);
    void ClearMessageEdit();

  private:
    QTableWidget* m_msgView;
    controls::DnDTextEdit* m_msgEdit;
  };
}
