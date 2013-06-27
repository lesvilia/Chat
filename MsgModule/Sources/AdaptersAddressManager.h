#pragma once
#include <vector>
#include <string>
#include "boost/noncopyable.hpp"

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

	class AdaptersAddressManager
		: private boost::noncopyable
	{
		typedef void (*AddressesDeleter)(void*);
	public:
		static AdaptersAddressManager* Instance();
		std::vector<std::wstring> GetLocalAddresses() const;
		std::wstring GetAppropriateAddress() const;

	private:
		AdaptersAddressManager();
		~AdaptersAddressManager();
		void Initialization();

	private:
		std::vector<std::wstring> m_localAddress;
	};

}