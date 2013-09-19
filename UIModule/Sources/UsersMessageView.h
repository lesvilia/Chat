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

  namespace internal
  {
    class TableItemCreator
    {
    public:
      TableItemCreator();
      virtual QWidget* CreateNameItem(const std::wstring& text, const QColor& color);
      virtual QWidget* CreateMessageItem(const std::wstring& text);
      virtual QWidget* CreateTimeItem(const std::wstring& text);
    private:
      QWidget* CreateTextCellItem(const std::wstring& text, const QColor& color,
        QFlags<Qt::AlignmentFlag> alignment = Qt::AlignTop);
    };

    class TableFileItemCreator
      : public TableItemCreator
    {
    public:
      TableFileItemCreator();
      virtual QWidget* CreateMessageItem(const std::wstring& text);
      QProgressBar* GetProgressObserver() const;
    private:
      QProgressBar* m_observer; //TODO: need to IProgressUIObserver replace
    };                          //after implement progress bar control
  }

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
    void InsertTxtMessage(const MessageInfo& msg, int rowNum);
    QProgressBar* AppendFileMessage(const MessageInfo& msg);

    void InsertMessageFromDB(const MessageInfo& msg);

  private:
    void InsertMessageImpl(const MessageInfo& msg, int rowNum, internal::TableItemCreator& creator);
    void CreateSubControls(IDropResultHandler* dropHandler);
    void ClearMessageEdit();

  private:
    QTableWidget* m_msgView;
    controls::DnDTextEdit* m_msgEdit;
  };
}
