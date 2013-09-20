#pragma once
#include <string>

#include <QColor>
#include <QFlags>

class QWidget;
class QTableWidget;
class QTableWidgetItem;

namespace ui
{
  class IProgressUIObserver;
  struct MessageInfo;

  class MessageItemCreator
  {
  public:
    enum Columns
    {
      NAME_COLUMN,
      MESSAGE_COLUMN,
      TIME_COLUMN,
      COLUMN_COUNT //always last
    };

    MessageItemCreator(QTableWidget* msgView, int rowNum);
    ~MessageItemCreator();
    void CreateItems(const MessageInfo& msg);

  private:
    virtual void CreateNameItem(const MessageInfo& msg, Columns column);
    virtual void CreateMessageItem(const MessageInfo& msg, Columns column);
    virtual void CreateTimeItem(const MessageInfo& msg, Columns column);

  protected:
    QTableWidget* m_msgView;
    const int m_rowNum;
  };

  class FileMessageItemCreator
    : public MessageItemCreator
  {
  public:
    FileMessageItemCreator(QTableWidget* msgView, int rowNum);
    ~FileMessageItemCreator();
    IProgressUIObserver* GetProgressObserver() const;

  private:
    virtual void CreateMessageItem(const MessageInfo& msg, Columns column);

  private:
    IProgressUIObserver* m_observer; //TODO: need to IProgressUIObserver replace
  };                                  //after implement progress bar control

  class DBFileMessageItemCreator
    : public MessageItemCreator
  {
  public:
    DBFileMessageItemCreator(QTableWidget* msgView, int rowNum);
    ~DBFileMessageItemCreator();
  private:
    virtual void CreateMessageItem(const MessageInfo& msg, Columns column);
  };
}
