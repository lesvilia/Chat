#include "stdafx.h"
#include "StateMessagesHandler.h"
#include "MessagesReceiver.h"
#include "MessagesTemplates.h"
#include "LoginManager.h"
#include "StringHelpers.h"
#include "pugixml.hpp"

namespace msg
{
  StateMessagesHandler::StateMessagesHandler(MessagesReceiver* msgReceiver, StateMessagesQueue* msgQueue)
    : m_msgReceiver(msgReceiver)
    , m_msgQueue(msgQueue)
  {
  }

  StateMessagesHandler::~StateMessagesHandler()
  {
  }

  void StateMessagesHandler::HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr)
  {
    if (m_msgReceiver)
    {
      StateMessagePtr stateMsg(GetMsgDataFromXml(message));
      if (stateMsg && !IsMyBroadcastMessage(stateMsg->m_uuid))
      {
        std::wstring userAddr(strhlp::StrToWstr(addr.get_host_addr()));
        StateMessageDataPtr newMessage(std::make_shared<StateMessageData>(userAddr, stateMsg));
        m_msgQueue->Enqueue(newMessage);
        m_msgReceiver->OnStateMessageReceived();
      }
    }
  }

  bool StateMessagesHandler::IsMyBroadcastMessage(const std::wstring& uuid)
  {
    return login::LoginManager::Instance()->GetCurrentUser()->uuid == uuid;
  }

  StateMessagePtr StateMessagesHandler::GetMsgDataFromXml(const std::wstring& xmlMsg)
  {
    StateMessagePtr stateMsg(nullptr);
    pugi::xml_document messageDoc;
    pugi::xml_parse_result result = messageDoc.load(xmlMsg.c_str());
    if (result)
    {
      pugi::xml_node messageNode = messageDoc.first_child();
      
      pugi::xml_node userNode = messageNode.child(USER_NODE);
      std::wstring uuid = userNode.child_value(USER_UUID_NODE);
      std::wstring username = userNode.child_value(USER_NAME_NODE);

      pugi::xml_node dataNode = messageNode.child(DATA_NODE);
      int state = dataNode.text().as_uint();
      stateMsg.reset(new StateMessage(state, uuid, username));
    }
    return stateMsg;
  }
}