#pragma once
#include "IServerSettingsHolder.h"

namespace sm
{
	class SettingsManager;
}

namespace msg
{
	class sm::SettingsManager;

	class StateServerSettingsHolder
		: public IServerSettingsHolder
	{
	public:
		explicit StateServerSettingsHolder(sm::SettingsManager* manager);
		~StateServerSettingsHolder();
		virtual std::wstring GetAddress();
		virtual unsigned short GetPort();

	private:
		sm::SettingsManager* m_manager;
	};
}