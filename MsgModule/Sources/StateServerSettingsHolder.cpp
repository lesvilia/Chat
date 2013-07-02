#include "stdafx.h"
#include "StateServerSettingsHolder.h"
#include "SettingsManager.h"

namespace msg
{
	StateServerSettingsHolder::StateServerSettingsHolder(sm::SettingsManager* manager)
		: m_manager(manager)
	{
	}

	StateServerSettingsHolder::~StateServerSettingsHolder()
	{
	}

	std::wstring StateServerSettingsHolder::GetAddress()
	{
		return m_manager->GetCurrentNetAddres();
	}

	unsigned short StateServerSettingsHolder::GetPort()
	{
		return m_manager->GetCurrentStatesPort();
	}
}