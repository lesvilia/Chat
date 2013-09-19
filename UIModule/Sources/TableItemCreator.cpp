#include "TableItemCreator.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QWidget>

#include "QtHelpers.h"
#include "UISettings.h"

using namespace settings::MessageView;

namespace ui
{
  ItemCreator::ItemCreator()
  {
  }

  ItemCreator::~ItemCreator()
  {
  }

  QWidget* ItemCreator::CreateNameItem(const std::wstring& text, const QColor& color) const
  {
    return CreateTextCellItem(text, color);
  }

  QWidget* ItemCreator::CreateMessageItem(const std::wstring& text) const
  {
    return CreateTextCellItem(text, Qt::black);
  }

  QWidget* ItemCreator::CreateTimeItem(const std::wstring& text) const
  {
    return CreateTextCellItem(text, Qt::gray, Qt::AlignTop | Qt::AlignHCenter);
  }

  QWidget* ItemCreator::CreateTextCellItem(const std::wstring& text, const QColor& color,
                                           QFlags<Qt::AlignmentFlag> alignment)
  {
    QLabel* item = new QLabel(WStrToQStr(text));
    item->setStyleSheet(TABLE_ITEM_STYLE_TEMPLATE.arg(color.name()));
    item->setAlignment(alignment);
    return item;
  }

  FileItemCreator::FileItemCreator()
    : m_observer(nullptr)
  {
  }

  FileItemCreator::~FileItemCreator()
  {
  }

  QWidget* FileItemCreator::CreateMessageItem(const std::wstring& text) const
  {
    m_observer = new QProgressBar();

    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* message = new QLabel(WStrToQStr(text));
    infoLayout->addWidget(message);
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

  QProgressBar* FileItemCreator::GetProgressObserver() const
  {
    return m_observer;
  }

  DBFileItemCreator::DBFileItemCreator()
  {
  }
  
  DBFileItemCreator::~DBFileItemCreator()
  {
  }

  QWidget* DBFileItemCreator::CreateMessageItem(const std::wstring& text) const
  {
    QLabel* message = new QLabel(WStrToQStr(text));
    QLabel* picture = new QLabel();
    picture->setPixmap(QPixmap(FILE_ICON_PATH));

    QHBoxLayout* cellLayout = new QHBoxLayout();
    cellLayout->addWidget(picture);
    cellLayout->addWidget(message);
    
    QWidget* item = new QWidget();
    item->setLayout(cellLayout);
    item->setMaximumHeight(50);
    return item;
  }
}