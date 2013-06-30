#pragma once
#include <string>
#include <vector>
#include <memory>

namespace sm
{
	namespace impl
	{
		class AdaptersAddressHolder;
	}

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
		std::vector<std::wstring> GetActiveAddresses() const;

	private:
		SettingsManager();
		~SettingsManager();
		void Initialize();
		void SetDefaultValue();
		void SaveSettings();

	private:
		std::unique_ptr<impl::AdaptersAddressHolder> m_addressHolder;
		std::wstring m_currentNetAddress;
		unsigned long m_stateMsgPort;
		unsigned long m_chatMsgPort;
	};
}

