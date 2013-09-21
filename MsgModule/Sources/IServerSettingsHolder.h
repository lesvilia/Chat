#pragma once
#include <string>

namespace msg
{
  class IServerSettingsHolder
  {
  public:
    virtual std::wstring GetAddress() const = 0;
    virtual unsigned short GetPort() const = 0;
    virtual ~IServerSettingsHolder() { }
  };
}