#include "UpdateProgressEvent.h"

namespace ui
{
  const QEvent::Type UpdateProgressEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

  UpdateProgressEvent::UpdateProgressEvent(int value)
    : QEvent(UpdateProgressEvent::type)
    , m_value(value)
  {
  }

  UpdateProgressEvent::~UpdateProgressEvent()
  {
  }
}
