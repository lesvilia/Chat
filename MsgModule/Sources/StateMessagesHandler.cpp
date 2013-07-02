#include "stdafx.h"
#include "StateMessagesHandler.h"
#include "MessagesReceiver.h"
#include "MessagesTemplates.h"
#include "pugixml.hpp"

namespace msg
{
	namespace 
	{

	}

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
		StateMessagePtr stateMsg(GetMsgDataFromXml(message));
		if (stateMsg)
		{
			StateMessageDataPtr newMessage(std::make_shared<StateMessageData>(addr, stateMsg)); //initialize newMessage (PugiXML)
			m_msgQueue->Enqueue(newMessage);
			m_msgReceiver->OnStateMessageReceived();
		}
	}

	StateMessagePtr GetMsgDataFromXml(const std::wstring& xmlMsg)
	{
		StateMessagePtr stateMsg(std::make_shared<StateMessage>());
		pugi::xml_document messageDoc;
		pugi::xml_parse_result result = messageDoc.load(xmlMsg.c_str());
		if (result)
		{
			pugi::xml_node messageNode = messageDoc.first_child();
			
			pugi::xml_node userNode = messageNode.child(USER_NODE);
			stateMsg->m_uuid = userNode.child_value(USER_UUID_NODE);
			stateMsg->m_username = userNode.child_value(USER_NAME_NODE);

			pugi::xml_node dataNode = messageNode.child(DATA_NODE);
			stateMsg->m_state = dataNode.text().as_uint();
			return stateMsg;
		}
		return nullptr;
	}
}