#include "stdafx.h"
#include <algorithm>
#include "SettingsManager.h"
#include "RegistryHelpers.h"
#include "AdaptersAddressHolder.h"
#include "DefaultSettings.h"

namespace sm
{
	namespace 
	{
		const wchar_t SETTINGS_KEYS_PATH[] = L"Software\\LChat\\Settings";
		const wchar_t CURRENT_NET_ADDRESS[] = L"IPAddress";
		const wchar_t CURRENT_STATE_MSG_PORT[] = L"SPort";
		const wchar_t CURRENT_CHAT_MSG_PORT[] = L"MPort";
	}

	SettingsManager* SettingsManager::Instance()
	{
		static SettingsManager manager;
		return &manager;
	}

	SettingsManager::SettingsManager()
		: m_addressHolder(new impl::AdaptersAddressHolder())
		, m_stateMsgPort(0)
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
		std::vector<std::wstring> addresses(m_addressHolder->GetLocalAddresses());
		auto iter = std::find(addresses.begin(), addresses.end(), addr);
		if (iter != addresses.end())
		{
			m_currentNetAddress = addr;
		}
		else
		{
			m_currentNetAddress = m_addressHolder->GetAppropriateAddress();
		}
	}

	void SettingsManager::SetCurrentStatesPort(unsigned long statePort)
	{
		m_stateMsgPort = statePort ? statePort : DEFAULT_STATE_MSG_PORT;
	}

	void SettingsManager::SetCurrentMessagesPort(unsigned long chatPort)
	{
		m_chatMsgPort = chatPort ? chatPort : DEFAULT_CHAT_MSG_PORT;
	}

	std::vector<std::wstring> SettingsManager::GetActiveAddresses() const
	{
		return m_addressHolder->GetLocalAddresses();
	}

	void SettingsManager::SetDefaultValue()
	{
		m_currentNetAddress = m_addressHolder->GetAppropriateAddress();
		m_stateMsgPort = DEFAULT_STATE_MSG_PORT;
		m_chatMsgPort =  DEFAULT_CHAT_MSG_PORT;
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
}