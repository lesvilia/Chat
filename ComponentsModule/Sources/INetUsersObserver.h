#pragma once
#include <string>

namespace net
{
  class INetUsersObserver
  {
  public:
    virtual void OnNetUserConnected(const std::wstring& uuid) = 0;
    virtual void OnNetUserDisconnected(const std::wstring& uuid) = 0;
  protected:
    ~INetUsersObserver() { }
  };
}