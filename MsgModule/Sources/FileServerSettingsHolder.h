#pragma once
#include "IServerSettingsHolder.h"

namespace sm
{
  class SettingsManager;
}

namespace msg
{
  class sm::SettingsManager;

  class FileServerSettingsHolder
    : public IServerSettingsHolder
  {
  public:
    explicit FileServerSettingsHolder(sm::SettingsManager* manager);
    virtual ~FileServerSettingsHolder();
    virtual std::wstring GetAddress() const;
    virtual unsigned short GetPort() const;

  private:
    sm::SettingsManager* m_manager;
  };
}