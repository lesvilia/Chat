#pragma once
#include <QEvent>

namespace ui
{
  class UpdateProgressEvent
    : public QEvent
  {
  public:
    UpdateProgressEvent(int value);
    virtual ~UpdateProgressEvent();
    const int m_value;
    static const QEvent::Type type;
  };
}
