#pragma once

namespace settings
{
	namespace settingsmanager
	{
		const unsigned long TEMP_MSG_PORT = 0;
		const unsigned long STATE_MSG_PORT = 50150;
		const unsigned long CHAT_MSG_PORT = 50152;
		const wchar_t SETTINGS_KEYS_PATH[] = L"Software\\LChat\\Settings";
		const wchar_t CURRENT_NET_ADDRESS[] = L"CurrentIPAddress";
		const wchar_t CURRENT_STATE_MSG_PORT[] = L"CurrentStateMsgPort";
		const wchar_t CURRENT_CHAT_MSG_PORT[] = L"CurrentChatMsgPort";
	}
}
