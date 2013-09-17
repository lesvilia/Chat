#pragma once
#include <vector>
#include <string>
#include "boost/noncopyable.hpp"

namespace sm
{
  namespace impl
  {
    class AdaptersAddressHolder
      : private boost::noncopyable
    {
      typedef void (*AddressesDeleter)(void*);

    public:
      AdaptersAddressHolder();
      ~AdaptersAddressHolder();
      std::vector<std::wstring> GetLocalAddresses() const;
      std::wstring GetAppropriateAddress() const;

    private:
      void Initialization();

    private:
      std::vector<std::wstring> m_localAddress;
    };
  }
}