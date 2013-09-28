#pragma once
#include <QEvent>

namespace ui
{
  class ErrorProgressEvent
    : public QEvent
  {
  public:
    ErrorProgressEvent();
    virtual ~ErrorProgressEvent();
    static const QEvent::Type type;
  };
}
