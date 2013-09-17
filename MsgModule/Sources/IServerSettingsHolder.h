#pragma once
#include <string>

namespace msg
{
  class IServerSettingsHolder
  {
  public:
    virtual std::wstring GetAddress() = 0;
    virtual unsigned short GetPort() = 0;
  protected:
    ~IServerSettingsHolder() { }
  };
}