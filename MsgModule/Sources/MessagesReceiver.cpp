#include "stdafx.h"
#include "MessagesReceiver.h"
#include "StateMessagesManager.h"
#include "ChatMessagesManager.h"
#include "NetUsersManager.h"

namespace msg
{
  MessagesReceiver::MessagesReceiver(UIMessageHandler* uiHandler)
    : m_uiHandler(uiHandler)
    , m_window(nullptr)
  {
    m_window.reset(new MessagesWindow(this));
    StateMessagesManager::Instance()->Activate(this);
    ChatMessagesManager::Instance()->Activate(this);
  }

  MessagesReceiver::~MessagesReceiver()
  {
    StateMessagesManager::Instance()->Deactivate();
    ChatMessagesManager::Instance()->Deactivate();
  }

  void MessagesReceiver::ProcessStateMessage()
  {
    StateMessageDataPtr stateMsg(StateMessagesManager::Instance()->GetMessagesQueue()->Dequeue());
    if (stateMsg)
    {
      StateMessagePtr msg(stateMsg->m_userMessage);
      switch (msg->m_state)
      {
      case CONNECT_REQUEST_STATE:
        net::NetUsersManager::Instance()->AddNewUser(msg->m_uuid, msg->m_username, stateMsg->m_userAddr);
        StateMessagesManager::Instance()->SendResponseToConnect(stateMsg->m_userAddr);
        break;

      case CONNECT_RESPONSE_STATE:
        net::NetUsersManager::Instance()->AddNewUser(msg->m_uuid, msg->m_username, stateMsg->m_userAddr);
        break;

      case DISCONNECT_STATE:
        net::NetUsersManager::Instance()->RemoveUser(msg->m_uuid);
        break;
      }
    }
  }

  void MessagesReceiver::ProcessChatMessage()
  {
    ChatMessageDataPtr chatMsg(ChatMessagesManager::Instance()->GetMessagesQueue()->Dequeue());
    if (chatMsg)
    {
      ChatMessagePtr msg(chatMsg->m_userMessage);
      m_uiHandler->OnTextMessageReceived(msg->m_uuid, msg->m_message);
    }
  }

  void MessagesReceiver::OnStateMessageReceived()
  {
    m_window->OnStateMessageReceived();
  }

  void MessagesReceiver::OnChatMessageReceived()
  {
    m_window->OnChatMessageReceived();
  }
}
