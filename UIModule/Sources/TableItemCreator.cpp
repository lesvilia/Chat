#include "TableItemCreator.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSizePolicy>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QWidget>

#include "ProgressHandler.h"
#include "UsersMessageView.h"
#include "QtHelpers.h"
#include "UISettings.h"

using namespace settings::MessageView;

namespace
{
  QTableWidgetItem* CreateTableItem(const std::wstring& text, const QColor& color,
                                    QFlags<Qt::AlignmentFlag> alignment = Qt::AlignTop)
  {
    QTableWidgetItem* item = new QTableWidgetItem(WStrToQStr(text));
    item->setTextAlignment(alignment);
    item ->setTextColor(color);
    item->setFlags(item->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable));
    return item;
  }
}

namespace ui
{
  MessageItemCreator::MessageItemCreator(QTableWidget* msgView, int rowNum)
    : m_msgView(msgView)
    , m_rowNum(rowNum)
  {
  }

  MessageItemCreator::~MessageItemCreator()
  {
  }

  void MessageItemCreator::CreateItems(const MessageInfo& msg)
  {
    CreateNameItem(msg, NAME_COLUMN);
    CreateMessageItem(msg, MESSAGE_COLUMN);
    CreateTimeItem(msg, TIME_COLUMN);
  }

  void MessageItemCreator::CreateNameItem(const MessageInfo& msg, Columns column)
  {
    m_msgView->setItem(m_rowNum, column,
      CreateTableItem(msg.m_username, msg.m_isNetUser ? Qt::blue : Qt::gray));
  }

  void MessageItemCreator::CreateMessageItem(const MessageInfo& msg, Columns column)
  {
    m_msgView->setItem(m_rowNum, column,
      CreateTableItem(msg.m_message, Qt::black));
  }

  void MessageItemCreator::CreateTimeItem(const MessageInfo& msg, Columns column)
  {
    m_msgView->setItem(m_rowNum, column,
      CreateTableItem(msg.m_time, Qt::gray, Qt::AlignTop | Qt::AlignHCenter));
  }

  FileMessageItemCreator::FileMessageItemCreator(QTableWidget* msgView, int rowNum)
    : MessageItemCreator(msgView, rowNum)
  {
  }

  FileMessageItemCreator::~FileMessageItemCreator()
  {
  }

  void FileMessageItemCreator::CreateMessageItem(const MessageInfo& msg, Columns column)
  {
    controls::ProgressUIHandler* handler = new controls::ProgressUIHandler();
    m_observer = handler;

    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* message = new QLabel(WStrToQStr(msg.m_message));
    infoLayout->addWidget(message);
    infoLayout->addWidget(handler);
    infoWidget->setLayout(infoLayout);
    infoWidget->setMaximumHeight(50);

    QHBoxLayout* cellLayout = new QHBoxLayout();
    QLabel* picture = new QLabel();
    picture->setPixmap(QPixmap(FILE_ICON_PATH));
    cellLayout->addWidget(picture);
    cellLayout->addWidget(infoWidget);

    QWidget* item = new QWidget();
    item->setLayout(cellLayout);
    m_msgView->setCellWidget(m_rowNum, column, item);
  }

  IProgressUIObserver* FileMessageItemCreator::GetProgressObserver() const
  {
    return m_observer;
  }

  DBFileMessageItemCreator::DBFileMessageItemCreator(QTableWidget* msgView, int rowNum)
    : MessageItemCreator(msgView, rowNum)
  {
  }

  DBFileMessageItemCreator::~DBFileMessageItemCreator()
  {
  }

  void DBFileMessageItemCreator::CreateMessageItem(const MessageInfo& msg, Columns column)
  {
    QLabel* message = new QLabel(WStrToQStr(msg.m_message));
    QLabel* picture = new QLabel();
    picture->setPixmap(QPixmap(FILE_ICON_PATH));

    QHBoxLayout* cellLayout = new QHBoxLayout();
    cellLayout->addWidget(picture);
    cellLayout->addWidget(message);

    QWidget* item = new QWidget();
    item->setLayout(cellLayout);
    item->setMaximumHeight(50);
    m_msgView->setCellWidget(m_rowNum, column, item);
  }
}
