#include "stdafx.h"
#include "ChatServerSettingsHolder.h"
#include "SettingsManager.h"

namespace msg
{
  ChatServerSettingsHolder::ChatServerSettingsHolder(sm::SettingsManager* manager)
    : m_manager(manager)
  {
  }

  ChatServerSettingsHolder::~ChatServerSettingsHolder()
  {
  }

  std::wstring ChatServerSettingsHolder::GetAddress()
  {
    return m_manager->GetCurrentNetAddres();
  }

  unsigned short ChatServerSettingsHolder::GetPort()
  {
    return m_manager->GetCurrentMessagesPort();
  }
}