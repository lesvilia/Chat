#include "UsersMessageView.h"

#include <cassert>

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "DnDTextEdit.h"
#include "StaticLink.h"
#include "IDropResultHandler.h"
#include "TableItemCreator.h"
#include "AddConversationFromDBEvent.h"
#include "UISettings.h"
#include "QtHelpers.h"

namespace
{
  void PrepareMessage(QString& msg)
  {
    QString temp(msg);
    temp.remove("\n");
    if (temp.isEmpty())
    {
      msg.clear();
    }
  }
}

namespace ui
{
  using namespace settings::MessageView;

  UsersMessageView::UsersMessageView(IDropResultHandler* dropHandler)
    : QSplitter(Qt::Vertical)
    , m_msgView(nullptr)
    , m_msgEdit(nullptr)
    , m_lastMsg(new db::MessageList())
  {
    CreateSubControls(dropHandler);
    CreateStaticLink();
  }

  UsersMessageView::~UsersMessageView()
  {
  }

  void UsersMessageView::CreateSubControls(IDropResultHandler* dropHandler)
  {
    m_msgView = new QTableWidget();
    m_msgView->setColumnCount(MessageItemCreator::COLUMN_COUNT);
    m_msgView->setShowGrid(false);
    m_msgView->setStyleSheet(MESSAGE_WIDGET_STYLE);

    QHeaderView* verticalHeader = m_msgView->verticalHeader();
    verticalHeader->setVisible(false);
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);

    QHeaderView* horizontalHeader = m_msgView->horizontalHeader();
    horizontalHeader->setVisible(false);
    horizontalHeader->setSectionResizeMode(MessageItemCreator::NAME_COLUMN, QHeaderView::Fixed);
    horizontalHeader->setSectionResizeMode(MessageItemCreator::MESSAGE_COLUMN, QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(MessageItemCreator::TIME_COLUMN, QHeaderView::Fixed);
    horizontalHeader->setDefaultSectionSize(DEFAULT_COLUMN_WIDTH);

    m_msgEdit = new controls::DnDTextEdit(dropHandler);
    
    addWidget(m_msgView);
    addWidget(m_msgEdit);
  }

   void UsersMessageView::CreateStaticLink()
   {
     controls::StaticLink* link = new controls::StaticLink();
     link->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
     link->setText(SetLinkStyle("Last Conversation"));
     link->setAlignment(Qt::AlignHCenter);
     connect(link, SIGNAL(clicked()), SLOT(ShowLastConversations()));

     m_msgView->insertRow(0);
     m_msgView->setCellWidget(0, MessageItemCreator::MESSAGE_COLUMN, link);
   }

  void UsersMessageView::ClearMessageEdit()
  {
    m_msgEdit->clear();
    m_msgEdit->document()->clear();
  }

  bool UsersMessageView::GetTextFromEdit(std::wstring* msg)
  {
    QString text(m_msgEdit->toPlainText());
    PrepareMessage(text);
    ClearMessageEdit();
    if (!text.isEmpty())
    {
      msg->assign(text.toStdWString());
      return true;
    }
    return false;
  }

  void UsersMessageView::AppendTxtMessage(const MessageInfo& msg)
  {
    const int rowNum = m_msgView->rowCount();
    InsertMessageImpl(msg, rowNum, MessageItemCreator(m_msgView, rowNum));
  }

  IProgressUIObserver* UsersMessageView::AppendFileMessage(const MessageInfo& msg)
  {
    const int rowNum = m_msgView->rowCount();
    FileMessageItemCreator creator(m_msgView, rowNum);
    InsertMessageImpl(msg, rowNum, creator);
    return creator.GetProgressObserver();
  }

  void UsersMessageView::InsertTxtMessageFromDB(const MessageInfo& msg, int rowNum)
  {
    InsertMessageImpl(msg, rowNum, MessageItemCreator(m_msgView, rowNum));
  }

  void UsersMessageView::InsertFileMessageFromDB(const MessageInfo& msg, int rowNum)
  {
    InsertMessageImpl(msg, rowNum, DBFileMessageItemCreator(m_msgView, rowNum));
  }

  void UsersMessageView::InsertMessageImpl(const MessageInfo& msg, int rowNum,
                                           MessageItemCreator& creator)
  {
    m_msgView->insertRow(rowNum);
    creator.CreateItems(msg);
    m_msgView->update();
    QTimer::singleShot(200, m_msgView, SLOT(scrollToBottom()));
  }

  void UsersMessageView::AddLastConversations(const db::MessageListPtr& messages)
  {
    QCoreApplication::postEvent(this, new AddConversationFromDBEvent(messages));
  }

  bool UsersMessageView::event(QEvent* ev)
  {
    if (ev->type() == AddConversationFromDBEvent::type)
    {
      AddConversationFromDBEvent* event = static_cast<AddConversationFromDBEvent*>(ev);
      SaveLastConversations(event->m_messages);
      return true;
    }
    return QWidget::event(ev);
  }

  void UsersMessageView::SaveLastConversations(const db::MessageListPtr& messages)
  {
    m_lastMsg = messages;
  }

  void UsersMessageView::ShowLastConversations()
  {
    m_msgView->removeRow(0);

    for (auto iter = m_lastMsg->rbegin(); iter != m_lastMsg->rend(); ++iter)
    {
      switch (iter->first)
      {
      case db::TEXT_MSG:
        InsertTxtMessageFromDB(iter->second, 0);
        break;

      case db::FILE_MSG:
        InsertFileMessageFromDB(iter->second, 0);
        break;
      }
    }
  }
}
