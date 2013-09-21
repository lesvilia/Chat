#pragma once
#include <memory>
#include <string>

class ACE_INET_Addr;
class ACE_SOCK_Stream;

namespace msg
{
  class IMessagesHandler
  {
  public:
    virtual void HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr) { }
    virtual void HandleConnect(std::unique_ptr<ACE_SOCK_Stream>&& sockStream) { }
    //virtual void HandleStop() { }
    virtual ~IMessagesHandler() { }
  };
}