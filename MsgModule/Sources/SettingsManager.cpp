#include "stdafx.h"
#include "SettingsManager.h"
#include "RegistryHelpers.h"
#include "AdaptersAddressManager.h"
#include <algorithm>
#include "MsgModuleSettings.h"

using namespace settings::settingsmanager;

SettingsManager* SettingsManager::Instance()
{
	static SettingsManager manager;
	return &manager;
}

SettingsManager::SettingsManager()
	: m_stateMsgPort(0)
	, m_chatMsgPort(0)
{
	Initialize();
}

SettingsManager::~SettingsManager()
{
	SaveSettings();
}


std::wstring SettingsManager::GetCurrentNetAddres() const
{
	return m_currentNetAddress;
}

unsigned long SettingsManager::GetCurrentStatesPort() const
{
	return m_stateMsgPort;
}

unsigned long SettingsManager::GetCurrentMessagesPort() const
{
	return m_chatMsgPort;
}

void SettingsManager::Initialize()
{
	CRegKey key;
	if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, SETTINGS_KEYS_PATH, KEY_READ))
	{
		SetCurrentAddress(reghlp::GetStringValue(key, CURRENT_NET_ADDRESS));
		SetCurrentStatesPort(reghlp::GetDWORDValue(key, CURRENT_STATE_MSG_PORT));
		SetCurrentMessagesPort(reghlp::GetDWORDValue(key, CURRENT_CHAT_MSG_PORT));
	}
	else
	{
		key.Create(HKEY_CURRENT_USER, SETTINGS_KEYS_PATH);
		SetDefaultValue();
	}
}

void SettingsManager::SetCurrentAddress(const std::wstring& addr)
{
	m_currentNetAddress = addr;
	std::vector<std::wstring> addresses(net::AdaptersAddressManager::Instance()->GetLocalAddresses());
	auto iter = std::find(addresses.begin(), addresses.end(), m_currentNetAddress);
	if (iter == addresses.end())
	{
		m_currentNetAddress = net::AdaptersAddressManager::Instance()->GetAppropriateAddress();
	}
}

void SettingsManager::SetCurrentStatesPort(unsigned long statePort)
{
	m_stateMsgPort = statePort;
}

void SettingsManager::SetCurrentMessagesPort(unsigned long chatPort)
{
	m_chatMsgPort = chatPort;
}

void SettingsManager::SetDefaultValue()
{
	m_currentNetAddress = net::AdaptersAddressManager::Instance()->GetAppropriateAddress();
	m_stateMsgPort = STATE_MSG_PORT;
	m_chatMsgPort = CHAT_MSG_PORT;
}

void SettingsManager::SaveSettings()
{
	CRegKey key;
	if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, SETTINGS_KEYS_PATH, KEY_WRITE))
	{
		key.SetStringValue(CURRENT_NET_ADDRESS, m_currentNetAddress.c_str());
		key.SetDWORDValue(CURRENT_STATE_MSG_PORT, m_stateMsgPort);
		key.SetDWORDValue(CURRENT_CHAT_MSG_PORT, m_chatMsgPort);
	}
}