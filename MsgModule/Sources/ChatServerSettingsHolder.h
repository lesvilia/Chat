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
    virtual ~ChatServerSettingsHolder();
    virtual std::wstring GetAddress() const;
    virtual unsigned short GetPort() const;

  private:
    sm::SettingsManager* m_manager;
  };
}