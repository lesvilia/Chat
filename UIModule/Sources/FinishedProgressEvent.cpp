#include "FinishedProgressEvent.h"

namespace ui
{
  const QEvent::Type FinishedProgressEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

  FinishedProgressEvent::FinishedProgressEvent()
    : QEvent(FinishedProgressEvent::type)
  {
  }

  FinishedProgressEvent::~FinishedProgressEvent()
  {
  }
}
