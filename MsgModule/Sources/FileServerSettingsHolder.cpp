#include "stdafx.h"
#include "FileServerSettingsHolder.h"

#include "SettingsManager.h"

namespace msg
{
  FileServerSettingsHolder::FileServerSettingsHolder(sm::SettingsManager* manager)
    : m_manager(manager)
  {
  }

  FileServerSettingsHolder::~FileServerSettingsHolder()
  {
  }

  std::wstring FileServerSettingsHolder::GetAddress() const
  {
    return m_manager->GetCurrentNetAddres();
  }

  unsigned short FileServerSettingsHolder::GetPort() const
  {
    return m_manager->GetCurrentFileMessagesPort();
  }
}