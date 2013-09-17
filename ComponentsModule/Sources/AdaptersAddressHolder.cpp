#include "stdafx.h"
#include "AdaptersAddressHolder.h"
#include "WSAStartupHolder.h"
#include <memory>
#include <algorithm>

namespace sm
{
  namespace impl
  {
    namespace
    {
      const ULONG WORKING_BUFFER_SIZE = 15000;
      const unsigned MAX_TRIES = 3;
      const wchar_t* IANA_PRIVATE_ADDRESSES[] = { L"10.", L"172.", L"192.168." };
      const size_t PRIVATE_ADDRESSES_SIZE = sizeof(IANA_PRIVATE_ADDRESSES) / sizeof(*IANA_PRIVATE_ADDRESSES);

      bool IsIANAPrivateAddress(const std::wstring& address)
      {
        bool findResult = false;
        for (int id = 0; id != PRIVATE_ADDRESSES_SIZE; ++id)
        {
          size_t position = -1;
          if(((position = address.find(IANA_PRIVATE_ADDRESSES[id])) != std::wstring::npos)
            && position == 0)
          {
            findResult = true;
          }
        }
        return findResult;
      }
    }

    AdaptersAddressHolder::AdaptersAddressHolder()
    {
      Initialization();
    }

    AdaptersAddressHolder::~AdaptersAddressHolder()
    {

    }

    void AdaptersAddressHolder::Initialization()
    {
      net::WSAStartupHolder wsaHolder(MAKEWORD(2, 2));
      if (wsaHolder.GetErrorCode() == 0)
      {
        ULONG outBufLen = WORKING_BUFFER_SIZE;
        ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST 
          | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

        std::unique_ptr<IP_ADAPTER_ADDRESSES, AddressesDeleter> pAddresses(nullptr, ::free);
        unsigned iterations = 0;
        DWORD dwRetVal = 0;

        do 
        {
          pAddresses.reset((PIP_ADAPTER_ADDRESSES)::malloc(outBufLen));
          dwRetVal = GetAdaptersAddresses(AF_INET, flags, NULL, pAddresses.get(), &outBufLen);
          ++iterations;
        } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (iterations < MAX_TRIES));

        if (dwRetVal == NO_ERROR)
        {
          PIP_ADAPTER_ADDRESSES currentAddress = pAddresses.get();
          while (currentAddress)
          {
            if (currentAddress->IfType & (IF_TYPE_ETHERNET_CSMACD | IF_TYPE_IEEE80211))
            {
              PIP_ADAPTER_UNICAST_ADDRESS pUnicast = currentAddress->FirstUnicastAddress;
              while (pUnicast)
              {
                DWORD length = pUnicast->Address.iSockaddrLength;
                std::vector<wchar_t> buff(length, '\0');
                if (::WSAAddressToString(pUnicast->Address.lpSockaddr, length, NULL, &buff[0], &length) == 0)
                {
                  m_localAddress.push_back(&buff[0]);
                }
                pUnicast = pUnicast->Next;
              }
            }
            currentAddress = currentAddress->Next;
          }
        }
      }
    }

    std::vector<std::wstring> AdaptersAddressHolder::GetLocalAddresses() const
    {
      return m_localAddress;
    }

    std::wstring AdaptersAddressHolder::GetAppropriateAddress() const
    {
      std::wstring appropriateAddress;
      std::for_each(m_localAddress.begin(), m_localAddress.end(),
        [&appropriateAddress](const std::wstring& addr)
      {
        if (IsIANAPrivateAddress(addr) && appropriateAddress.empty())
        {
          appropriateAddress = addr;
        }

      });
      if (appropriateAddress.empty() && !m_localAddress.empty())
      {
        appropriateAddress = *m_localAddress.begin();
      }
      return appropriateAddress;
    }
  }//namespace impl
}//namespace sm 