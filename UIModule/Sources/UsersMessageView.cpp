#include "UsersMessageView.h"

#include <cassert>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QVBoxLayout>

#include "DnDTextEdit.h"
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

  MessageInfo::MessageInfo(const std::wstring& username, const std::wstring& message,
                           const std::wstring& time, bool isNetUser)
    : m_username(username)
    , m_message(message)
    , m_time(time)
    , m_isNetUser(isNetUser)
  {
  }

  UsersMessageView::UsersMessageView(IDropResultHandler* dropHandler)
    : QSplitter(Qt::Vertical)
    , m_msgView(nullptr)
    , m_msgEdit(nullptr)
  {
    CreateSubControls(dropHandler);
  }

  UsersMessageView::~UsersMessageView()
  {
  }

  void UsersMessageView::CreateSubControls(IDropResultHandler* dropHandler)
  {
    m_msgView = new QTableWidget();
    m_msgView->setColumnCount(COLUMN_COUNT);
    m_msgView->setShowGrid(false);
    m_msgView->setStyleSheet(MESSAGE_WIDGET_STYLE);

    QHeaderView* verticalHeader = m_msgView->verticalHeader();
    verticalHeader->setVisible(false);
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);

    QHeaderView* horizontalHeader = m_msgView->horizontalHeader();
    horizontalHeader->setVisible(false);
    horizontalHeader->setSectionResizeMode(NAME_COLUMN, QHeaderView::Fixed);
    horizontalHeader->setSectionResizeMode(MESSAGE_COLUMN, QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(TIME_COLUMN, QHeaderView::Fixed);
    horizontalHeader->setDefaultSectionSize(DEFAULT_COLUMN_WIDTH);
    
    m_msgEdit = new controls::DnDTextEdit(dropHandler);
    
    addWidget(m_msgView);
    addWidget(m_msgEdit);
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
    InsertMessageImpl(msg, m_msgView->rowCount(), ItemCreator());
  }

  QProgressBar* UsersMessageView::AppendFileMessage(const MessageInfo& msg)
  {
    FileItemCreator creator;
    InsertMessageImpl(msg, m_msgView->rowCount(), creator);
    return creator.GetProgressObserver();
  }

  void UsersMessageView::InsertTxtMessageFromDB(const MessageInfo& msg, int rowNum)
  {
    InsertMessageImpl(msg, rowNum, ItemCreator());
  }

  void UsersMessageView::InsertFileMessageFromDB(const MessageInfo& msg, int rowNum)
  {
    InsertMessageImpl(msg, rowNum, DBFileItemCreator());
  }

  void UsersMessageView::InsertMessageImpl(const MessageInfo& msg, int rowNum,
                                           const ItemCreator& creator)
  {
    m_msgView->insertRow(rowNum);
    QWidget* nameItem = creator.CreateNameItem(msg.m_username, msg.m_isNetUser ? Qt::blue : Qt::gray);
    QWidget* messageItem = creator.CreateMessageItem(msg.m_message);
    QWidget* timeItem = creator.CreateTimeItem(msg.m_time);

    m_msgView->setCellWidget(rowNum, NAME_COLUMN, nameItem);
    m_msgView->setCellWidget(rowNum, MESSAGE_COLUMN, messageItem);
    m_msgView->setCellWidget(rowNum, TIME_COLUMN, timeItem);
  }
}
