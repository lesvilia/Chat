#include "stdafx.h"
#include "ChatMessagesHandler.h"
#include "MessagesReceiver.h"
#include "MessagesTemplates.h"
#include "StringHelpers.h"
#include "pugixml.hpp"

namespace msg
{
	ChatMessagesHandler::ChatMessagesHandler(MessagesReceiver* msgReceiver, ChatMessagesQueue* msgQueue)
		: m_msgReceiver(msgReceiver)
		, m_msgQueue(msgQueue)
	{
	}

	ChatMessagesHandler::~ChatMessagesHandler()
	{
	}

	void ChatMessagesHandler::HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr)
	{
		if (m_msgReceiver)
		{
			ChatMessagePtr chatMsg(GetMsgDataFromXml(message));
			if (chatMsg)
			{
				std::wstring userAddr(strhlp::StrToWstr(addr.get_host_addr()));
				ChatMessageDataPtr newMessage(std::make_shared<ChatMessageData>(userAddr, chatMsg));
				m_msgQueue->Enqueue(newMessage);
				m_msgReceiver->OnChatMessageReceived();
			}
		}
	}

	ChatMessagePtr ChatMessagesHandler::GetMsgDataFromXml(const std::wstring& xmlMsg)
	{
		ChatMessagePtr chatMsg(nullptr);
		pugi::xml_document messageDoc;
		pugi::xml_parse_result result = messageDoc.load(xmlMsg.c_str());
		if (result)
		{
			pugi::xml_node messageNode = messageDoc.first_child();

			pugi::xml_node userNode = messageNode.child(USER_NODE);
			std::wstring uuid = userNode.child_value(USER_UUID_NODE);
			pugi::xml_node dataNode = messageNode.child(DATA_NODE);
			std::wstring message = dataNode.text().as_string();
			chatMsg.reset(new ChatMessage(uuid, message));
		}
		return chatMsg;
	}
}