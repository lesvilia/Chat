#pragma once
#include <QEvent>

namespace ui
{
  class FinishedProgressEvent
    : public QEvent
  {
  public:
    FinishedProgressEvent();
    virtual ~FinishedProgressEvent();
    static const QEvent::Type type;
  };
}
