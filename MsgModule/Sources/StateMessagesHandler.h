#pragma once
#include "IMessagesHandler.h"
#include "MessagesQueue.h"

namespace msg
{
  class MessagesReceiver;

  class StateMessagesHandler
    : public IMessagesHandler
  {
  public:
    StateMessagesHandler(MessagesReceiver* msgReceiver, StateMessagesQueue* msgQueue);
    virtual ~StateMessagesHandler();
    virtual void HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr);
  private:
    StateMessagePtr GetMsgDataFromXml(const std::wstring& xmlMsg);
    bool IsMyBroadcastMessage(const std::wstring& uuid);
  private:
    MessagesReceiver* m_msgReceiver;
    StateMessagesQueue* m_msgQueue;
  };
}