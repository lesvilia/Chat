#pragma once
#include <string>

class SettingsManager
{
public:
	static SettingsManager* Instance();
	std::wstring GetCurrentNetAddres() const;
	unsigned long GetCurrentStatesPort() const;
	unsigned long GetCurrentMessagesPort() const;
	void SetCurrentAddress(const std::wstring& addr);
	void SetCurrentStatesPort(unsigned long statePort);
	void SetCurrentMessagesPort(unsigned long chatPort);
private:
	SettingsManager();
	~SettingsManager();
	void Initialize();
	void SetDefaultValue();
	void SaveSettings();
private:
	std::wstring m_currentNetAddress;
	unsigned long m_stateMsgPort;
	unsigned long m_chatMsgPort;
};