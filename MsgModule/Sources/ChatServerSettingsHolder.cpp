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

  std::wstring ChatServerSettingsHolder::GetAddress() const
  {
    return m_manager->GetCurrentNetAddres();
  }

  unsigned short ChatServerSettingsHolder::GetPort() const
  {
    return m_manager->GetCurrentMessagesPort();
  }
}