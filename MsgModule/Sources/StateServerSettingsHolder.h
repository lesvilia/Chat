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
    virtual ~StateServerSettingsHolder();
    virtual std::wstring GetAddress() const;
    virtual unsigned short GetPort() const;

  private:
    sm::SettingsManager* m_manager;
  };
}