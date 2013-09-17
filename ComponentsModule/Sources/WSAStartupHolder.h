#pragma once
#include "boost/noncopyable.hpp"
#include <winsock2.h>
#include <iphlpapi.h>

namespace net
{
  class WSAStartupHolder
    : private boost::noncopyable
  {
  public:
    WSAStartupHolder(unsigned short version);
    ~WSAStartupHolder();
    int GetErrorCode() const;

  private:
    int m_error;
  };
}