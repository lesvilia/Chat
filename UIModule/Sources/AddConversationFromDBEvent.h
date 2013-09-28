#pragma once
#include <QEvent>
#include "DataBaseUIHandler.h"

namespace ui
{
  class AddConversationFromDBEvent
    : public QEvent
  {
  public:
    AddConversationFromDBEvent(const db::MessageListPtr& messages);
    virtual ~AddConversationFromDBEvent();
    const db::MessageListPtr m_messages;
    static const QEvent::Type type;
  };
}
