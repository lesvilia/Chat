#pragma once
#include "IMessagesHandler.h"
#include "MessagesQueue.h"

namespace msg
{
  class MessagesReceiver;

  class ChatMessagesHandler
    : public IMessagesHandler
  {
  public:
    ChatMessagesHandler(MessagesReceiver* msgReciever, ChatMessagesQueue* msgQueue);
    virtual ~ChatMessagesHandler();
    virtual void HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr);
  private:
    ChatMessagePtr GetMsgDataFromXml(const std::wstring& xmlMsg);
  private:
    MessagesReceiver* m_msgReceiver;
    ChatMessagesQueue* m_msgQueue;
  };
}