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
#include "UISettings.h"
#include "QtHelpers.h"

using namespace settings::MessageView;

namespace
{
  QWidget* CreateTextCellItem(const std::wstring& text, const QColor& color,
                               QFlags<Qt::AlignmentFlag> alignment = Qt::AlignTop)
  {
    QLabel* item = new QLabel(WStrToQStr(text));
    item->setStyleSheet(TABLE_ITEM_STYLE_TEMPLATE.arg(color.name()));
    item->setAlignment(alignment);
    return item;
  }

  QWidget* CreateFileCellItem(const std::wstring& text, QProgressBar* progressBar)
  {
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* message = new QLabel(WStrToQStr(text));
    infoLayout->addWidget(message);
    infoLayout->addWidget(progressBar);
    infoWidget->setLayout(infoLayout);
    infoWidget->setMaximumHeight(50);

    QHBoxLayout* cellLayout = new QHBoxLayout();
    QLabel* picture = new QLabel();
    picture->setPixmap(QPixmap(FILE_ICON_PATH));
    cellLayout->addWidget(picture);
    cellLayout->addWidget(infoWidget);
    
    QWidget* item = new QWidget();
    item->setLayout(cellLayout);
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
  MessageInfo::MessageInfo(const std::wstring& username, const std::wstring& message,
                           const std::wstring& time, bool isNetUser)
    : m_username(username)
    , m_message(message)
    , m_time(time)
    , m_isNetUser(isNetUser)
  {
  }

  internal::TableItemCreator::TableItemCreator()
  {
  }

  QWidget* internal::TableItemCreator::CreateNameItem(const std::wstring& text, const QColor& color)
  {
    return CreateTextCellItem(text, color);
  }

  QWidget* internal::TableItemCreator::CreateMessageItem(const std::wstring& text)
  {
    return CreateTextCellItem(text, Qt::black);
  }

  QWidget* internal::TableItemCreator::CreateTimeItem(const std::wstring& text)
  {
    return CreateTextCellItem(text, Qt::gray, Qt::AlignTop | Qt::AlignHCenter);
  }

  QWidget* internal::TableItemCreator::CreateTextCellItem(const std::wstring& text, const QColor& color,
                                                             QFlags<Qt::AlignmentFlag> alignment)
  {
    QLabel* item = new QLabel(WStrToQStr(text));
    item->setStyleSheet(TABLE_ITEM_STYLE_TEMPLATE.arg(color.name()));
    item->setAlignment(alignment);
    return item;
  }

  internal::TableFileItemCreator::TableFileItemCreator()
    : m_observer(nullptr)
  {
  }

  QWidget* internal::TableFileItemCreator::CreateMessageItem(const std::wstring& text)
  {
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* message = new QLabel(WStrToQStr(text));
    infoLayout->addWidget(message);
    m_observer = new QProgressBar();
    infoLayout->addWidget(m_observer);
    infoWidget->setLayout(infoLayout);
    infoWidget->setMaximumHeight(50);

    QHBoxLayout* cellLayout = new QHBoxLayout();
    QLabel* picture = new QLabel();
    picture->setPixmap(QPixmap(FILE_ICON_PATH));
    cellLayout->addWidget(picture);
    cellLayout->addWidget(infoWidget);

    QWidget* item = new QWidget();
    item->setLayout(cellLayout);
    return item;
  }

  QProgressBar* internal::TableFileItemCreator::GetProgressObserver() const
  {
    return m_observer;
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
    InsertTxtMessage(msg, m_msgView->rowCount());
  }

  void UsersMessageView::InsertTxtMessage(const MessageInfo& msg, int rowNum)
  {
    InsertMessageImpl(msg, rowNum, internal::TableItemCreator());
  }

  QProgressBar* UsersMessageView::AppendFileMessage(const MessageInfo& msg)
  {
    internal::TableFileItemCreator creator;
    InsertMessageImpl(msg, m_msgView->rowCount(), creator);
    return creator.GetProgressObserver();
  }

  void UsersMessageView::InsertMessageImpl(const MessageInfo& msg, int rowNum,
                                           internal::TableItemCreator& creator)
  {
    m_msgView->insertRow(rowNum);
    QWidget* nameItem = creator.CreateNameItem(msg.m_username, msg.m_isNetUser ? Qt::blue : Qt::gray);
    QWidget* timeItem = creator.CreateMessageItem(msg.m_message);
    QWidget* messageItem = creator.CreateTimeItem(msg.m_time);

    m_msgView->setCellWidget(rowNum, NAME_COLUMN, nameItem);
    m_msgView->setCellWidget(rowNum, MESSAGE_COLUMN, messageItem);
    m_msgView->setCellWidget(rowNum, TIME_COLUMN, timeItem);
  }
}
