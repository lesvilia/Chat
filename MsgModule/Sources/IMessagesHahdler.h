#pragma once
#include <string>
#include "ace/INET_Addr.h"

namespace msg
{
  class IMessagesHandler
  {
  public:
    virtual void HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr) = 0;
  protected:
    ~IMessagesHandler() { }
  };
}