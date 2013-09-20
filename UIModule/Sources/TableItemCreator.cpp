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

  const QString IMAGE_TEMPLATE("QWidget {background-image: url(%1); }");
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

    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* message = new QLabel(WStrToQStr(msg.m_message));
    handler->setFixedHeight(10);
    infoLayout->setContentsMargins(0, 0, 0, 5);
    infoLayout->addWidget(message, 1);
    infoLayout->addWidget(handler, 1);
    QWidget* infoWidget = new QWidget();
    infoWidget->setLayout(infoLayout);

    QHBoxLayout* cellLayout = new QHBoxLayout();
    QWidget* picture = new QWidget();
    picture->setFixedHeight(24);
    picture->setFixedWidth(24);
    picture->setStyleSheet(IMAGE_TEMPLATE.arg(FILE_ICON_PATH));
    cellLayout->setContentsMargins(0, 0, 0, 5);
    cellLayout->addWidget(picture);
    cellLayout->addWidget(infoWidget, 3);

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
    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* message = new QLabel(WStrToQStr(msg.m_message));
    infoLayout->setContentsMargins(0, 0, 0, 5);
    infoLayout->addWidget(message, 1);
    infoLayout->addStretch(1);
    QWidget* infoWidget = new QWidget();
    infoWidget->setLayout(infoLayout);

    QHBoxLayout* cellLayout = new QHBoxLayout();
    QWidget* picture = new QWidget();
    picture->setFixedHeight(24);
    picture->setFixedWidth(24);
    picture->setStyleSheet(IMAGE_TEMPLATE.arg(FILE_ICON_PATH));
    cellLayout->setContentsMargins(0, 0, 0, 0);
    cellLayout->addWidget(picture);
    cellLayout->addWidget(infoWidget, 3);

    QWidget* item = new QWidget();
    item->setLayout(cellLayout);
    m_msgView->setCellWidget(m_rowNum, column, item);
  }
}
