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
  class MessageItemCreator;

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
  public:
    explicit UsersMessageView(IDropResultHandler* dropHandler);
    virtual ~UsersMessageView();
    bool GetTextFromEdit(std::wstring* msg);
    void AppendTxtMessage(const MessageInfo& msg);
    IProgressUIObserver* AppendFileMessage(const MessageInfo& msg);
    void InsertTxtMessageFromDB(const MessageInfo& msg, int rowNum);
    void InsertFileMessageFromDB(const MessageInfo& msg, int rowNum);

  private:
    void InsertMessageImpl(const MessageInfo& msg, int rowNum, MessageItemCreator& creator);
    void CreateSubControls(IDropResultHandler* dropHandler);
    void ClearMessageEdit();

  private:
    QTableWidget* m_msgView;
    controls::DnDTextEdit* m_msgEdit;
  };
}
