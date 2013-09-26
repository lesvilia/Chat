#include "WSAStartupHolder.h"

namespace net
{
  WSAStartupHolder::WSAStartupHolder(unsigned short version)
    : m_error(0)
  {
    WSADATA wsaData;
    m_error = ::WSAStartup(version, &wsaData);
  }

  int WSAStartupHolder::GetErrorCode() const
  {
    return m_error;
  }

  WSAStartupHolder::~WSAStartupHolder()
  {
    ::WSACleanup();
  }
}