#pragma once
#include "IServerSettingsHolder.h"

namespace sm
{
	class SettingsManager;
}

namespace msg
{
	class sm::SettingsManager;

	class ChatServerSettingsHolder
		: public IServerSettingsHolder
	{
	public:
		explicit ChatServerSettingsHolder(sm::SettingsManager* manager);
		~ChatServerSettingsHolder();
		virtual std::wstring GetAddress();
		virtual unsigned short GetPort();

	private:
		sm::SettingsManager* m_manager;
	};
}