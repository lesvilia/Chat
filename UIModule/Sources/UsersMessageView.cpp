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

namespace controls
{
    UsersMessageView::UsersMessageView()
      : QSplitter(Qt::Vertical)
    {
      CreateSubControls();
    }

    UsersMessageView::~UsersMessageView()
    {
    }

    void UsersMessageView::CreateSubControls()
    {
      QTableWidget* msgView = new QTableWidget();
      msgView->setColumnCount(COLUMN_COUNT);
      msgView->setShowGrid(false);
      msgView->setStyleSheet(MESSAGE_WIDGET_STYLE);

      QHeaderView* verticalHeader = msgView->verticalHeader();
      verticalHeader->setVisible(false);
      verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);

      QHeaderView* horizontalHeader = msgView->horizontalHeader();
      horizontalHeader->setVisible(false);
      horizontalHeader->setSectionResizeMode(NAME_COLUMN, QHeaderView::Fixed);
      horizontalHeader->setSectionResizeMode(MESSAGE_COLUMN, QHeaderView::Stretch);
      horizontalHeader->setSectionResizeMode(TIME_COLUMN, QHeaderView::Fixed);
      horizontalHeader->setDefaultSectionSize(DEFAULT_COLUMN_WIDTH);

      QTextEdit* msgEdit = new QTextEdit();
      addWidget(msgView);
      addWidget(msgEdit);
    }

    bool UsersMessageView::GetTextFromEdit(std::wstring* msg)
    {
      QTextEdit* msgEdit = static_cast<QTextEdit*>(widget(EDIT_SECTION_ID));
      if (msgEdit)
      {
        QString message(msgEdit->toPlainText());
        PrepareMessage(message);

        msgEdit->clear();
        msgEdit->document()->clear();

        if (!message.isEmpty())
        {
          msg->assign(message.toStdWString());
          return true;
        }
      }
      return false;
    }

    void UsersMessageView::AppendMessage(const MessageInfo& msg)
    {
      QTableWidget* msgView = static_cast<QTableWidget*>(widget(VIEW_SECTION_ID));
      const int rowNum = msgView->rowCount();
      msgView->insertRow(rowNum);
      QTableWidgetItem* nameItem = CreateTableItem(msg.m_username, msg.m_isNetUser ? Qt::blue : Qt::gray);
      QTableWidgetItem* messageItem = CreateTableItem(msg.m_message, Qt::black);
      QTableWidgetItem* timeItem = CreateTableItem(msg.m_time, Qt::gray, Qt::AlignTop | Qt::AlignHCenter);

      msgView->setItem(rowNum, NAME_COLUMN, nameItem);
      msgView->setItem(rowNum, MESSAGE_COLUMN, messageItem);
      msgView->setItem(rowNum, TIME_COLUMN, timeItem);
    }

    void UsersMessageView::InsertMessage(const MessageInfo& msg, int row)
    {

    }
  }
}
