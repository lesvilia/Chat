#include "UsersMessageView.h"

#include <cassert>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QVBoxLayout>

#include "DnDTextEdit.h"
#include "StaticLink.h"
#include "IDropResultHandler.h"
#include "TableItemCreator.h"
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
  {
    CreateSubControls(dropHandler);
    CreateStaticLink();
    auto res = connect(this, SIGNAL(ConversationsRecieved(db::MessageListPtr)),
            SLOT(SaveLastConversations(db::MessageListPtr)));
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
     auto res = connect(link, SIGNAL(clicked()), SLOT(ShowLastConversations()));

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
  }

  void UsersMessageView::AddLastConversations(db::MessageListPtr messages)
  {
    emit ConversationsRecieved(messages);
  }

  void UsersMessageView::SaveLastConversations(db::MessageListPtr messages)
  {
    m_lastMsg.swap(messages);
  }

  void UsersMessageView::ShowLastConversations()
  {
    m_msgView->removeRow(0);
    
    if (!m_lastMsg)
      return;

    for (auto iter = m_lastMsg->begin(); iter != m_lastMsg->end(); ++iter)
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
