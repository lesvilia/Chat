#pragma once
#include <memory>
#include <string>

class ACE_INET_Addr;
class ACE_SOCK_Stream;

namespace msg
{
  typedef std::shared_ptr<ACE_SOCK_Stream> SocketStream;

  class IMessagesHandler
  {
  public:
    virtual void HandleMessage(const std::wstring& message, const ACE_INET_Addr& addr) { }
    virtual void HandleConnect(const SocketStream& sockStream) { }
    virtual ~IMessagesHandler() { }
  };
}
