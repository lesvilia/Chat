#include "UsersMessageView.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTextEdit>

#include "UISettings.h"
#include "QtHelpers.h"

namespace
{
  QTableWidgetItem* CreateTableItem(const std::wstring& text, const QColor& color, int alignment = Qt::AlignTop)
  {
    QTableWidgetItem* item = new QTableWidgetItem(WStrToQStr(text));
    item->setTextColor(color);
    item->setTextAlignment(alignment);
    item->setFlags(item->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable));
    return item;
  }

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

  UsersMessageView::UsersMessageView()
    : QSplitter(Qt::Vertical)
    , m_msgView(nullptr)
    , m_msgEdit(nullptr)
  {
    CreateSubControls();
  }

  UsersMessageView::~UsersMessageView()
  {
  }

  void UsersMessageView::CreateSubControls()
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
    
    m_msgEdit = new QTextEdit();
    
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

  void UsersMessageView::AppendMessage(const MessageInfo& msg)
  {
    const int rowNum = m_msgView->rowCount();
    InsertMessage(msg, rowNum);
  }

  void UsersMessageView::InsertMessage(const MessageInfo& msg, int rowNum)
  {
    m_msgView->insertRow(rowNum);
    QTableWidgetItem* nameItem = CreateTableItem(msg.m_username, msg.m_isNetUser ? Qt::blue : Qt::gray);
    QTableWidgetItem* messageItem = CreateTableItem(msg.m_message, Qt::black);
    QTableWidgetItem* timeItem = CreateTableItem(msg.m_time, Qt::gray, Qt::AlignTop | Qt::AlignHCenter);

    m_msgView->setItem(rowNum, NAME_COLUMN, nameItem);
    m_msgView->setItem(rowNum, MESSAGE_COLUMN, messageItem);
    m_msgView->setItem(rowNum, TIME_COLUMN, timeItem);
  }
}
