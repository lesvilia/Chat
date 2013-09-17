#pragma once
#include "MessagesWindow.h"
#include "UIMessageHandler.h"

namespace msg
{
  class MessagesReceiver
    : private boost::noncopyable
  {
  public:
    explicit MessagesReceiver(UIMessageHandler* uiHandler);
    ~MessagesReceiver();
    void ProcessStateMessage();
    void ProcessChatMessage();
    void OnStateMessageReceived();
    void OnChatMessageReceived();

  private:
    UIMessageHandler* m_uiHandler;
    MessagesWindowPtr m_window;
  };
}
