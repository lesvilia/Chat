#include "AddConversationFromDBEvent.h"

namespace ui
{
  const QEvent::Type AddConversationFromDBEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

  AddConversationFromDBEvent::AddConversationFromDBEvent(const db::MessageListPtr& messages)
    : QEvent(AddConversationFromDBEvent::type)
    , m_messages(messages)
  {
  }

  AddConversationFromDBEvent::~AddConversationFromDBEvent()
  {
  }
}
