#pragma once
#include <string>
#include <QSplitter>
#include "DataBaseUIHandler.h"

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

  class UsersMessageView
    : public QSplitter
    , public db::DataBaseUIHandler
  {
    Q_OBJECT
  public:
    explicit UsersMessageView(IDropResultHandler* dropHandler);
    virtual ~UsersMessageView();
    bool GetTextFromEdit(std::wstring* msg);
    void AppendTxtMessage(const MessageInfo& msg);
    IProgressUIObserver* AppendFileMessage(const MessageInfo& msg);

    //DataBaseUIHandler interface
    virtual void AddLastConversations(const db::MessageListPtr& messages);
    virtual bool event(QEvent* ev);

  private:
    void InsertTxtMessageFromDB(const MessageInfo& msg, int rowNum);
    void InsertFileMessageFromDB(const MessageInfo& msg, int rowNum);
    void InsertMessageImpl(const MessageInfo& msg, int rowNum, MessageItemCreator& creator);
    void CreateSubControls(IDropResultHandler* dropHandler);
    void ClearMessageEdit();
    void CreateStaticLink();
    void SaveLastConversations(const db::MessageListPtr& messages);
  
  private slots:
     void ShowLastConversations();

  private:
    QTableWidget* m_msgView;
    controls::DnDTextEdit* m_msgEdit;
    db::MessageListPtr m_lastMsg;
  };
}
