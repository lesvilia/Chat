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

  std::wstring StateServerSettingsHolder::GetAddress() const
  {
    return m_manager->GetCurrentNetAddres();
  }

  unsigned short StateServerSettingsHolder::GetPort() const
  {
    return m_manager->GetCurrentStatesPort();
  }
}