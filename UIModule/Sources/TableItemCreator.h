#pragma once
#include <string>

#include <QColor>
#include <QFlags>

class QWidget;
class QProgressBar;

namespace ui
{
  class ItemCreator
  {
  public:
    ItemCreator();
    ~ItemCreator();
    virtual QWidget* CreateNameItem(const std::wstring& text, const QColor& color) const;
    virtual QWidget* CreateMessageItem(const std::wstring& text) const;
    virtual QWidget* CreateTimeItem(const std::wstring& text) const;
  private:
    static QWidget* CreateTextCellItem(const std::wstring& text, const QColor& color,
                                QFlags<Qt::AlignmentFlag> alignment = Qt::AlignTop);
  };

  class FileItemCreator
    : public ItemCreator
  {
  public:
    FileItemCreator();
    ~FileItemCreator();
    virtual QWidget* CreateMessageItem(const std::wstring& text) const;
    QProgressBar* GetProgressObserver() const;

  private:
    mutable QProgressBar* m_observer; //TODO: need to IProgressUIObserver replace
  };                                  //after implement progress bar control

  class DBFileItemCreator
    : public ItemCreator
  {
  public:
    DBFileItemCreator();
    ~DBFileItemCreator();
    virtual QWidget* CreateMessageItem(const std::wstring& text) const;
  };
}