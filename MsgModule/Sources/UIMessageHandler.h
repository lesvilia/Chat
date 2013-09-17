#pragma once
#include <string>

namespace msg
{
  class UIMessageHandler
  {
  public:
    virtual void AddNewMessage(const std::wstring& uuid, const std::wstring& message) = 0;
  protected:
    ~UIMessageHandler(){}
  };
}