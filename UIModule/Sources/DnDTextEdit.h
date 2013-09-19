#pragma once
#include <QTextEdit>

class QDragEnterEvent;
class QDropEvent;

namespace ui
{
  class IDropResultHandler;

  namespace controls
  {
    class DnDTextEdit
      : public QTextEdit
    {
    public:
      explicit DnDTextEdit(IDropResultHandler* handler);
    protected:
      virtual void dragEnterEvent(QDragEnterEvent* event);
      virtual void dropEvent(QDropEvent* event);
    private:
      IDropResultHandler* m_handler;
    };
  }
}
