#include "ErrorProgressEvent.h"

namespace ui
{
  const QEvent::Type ErrorProgressEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

  ErrorProgressEvent::ErrorProgressEvent()
    : QEvent(ErrorProgressEvent::type)
  {
  }

  ErrorProgressEvent::~ErrorProgressEvent()
  {
  }
}
