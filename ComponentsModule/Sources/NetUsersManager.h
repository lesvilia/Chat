#pragma once
#include <map>
#include <vector>
#include <memory>
#include <string>
#include "ace/INET_Addr.h"
#include "boost/noncopyable.hpp"

namespace net
{
	class INetUsersObserver;
	class NetUsersManager
		: private boost::noncopyable
	{
		struct NetUserData
		{
			NetUserData();
			NetUserData(const std::wstring& userName, const ACE_INET_Addr& userAddress);
			std::wstring name;
			ACE_INET_Addr address;
		};
		typedef std::shared_ptr<NetUserData> NetUserDataPtr;

	public:
		static NetUsersManager* Instance();
		void Subscribe(INetUsersObserver* observer);
		void AddNewUser(const std::wstring& uuid, const std::wstring& name, const ACE_INET_Addr& addr);
		void RemoveUser(const std::wstring& uuid);
		ACE_INET_Addr GetNetUserAddres(const std::wstring& uuid);
		std::wstring GetNetUserName(const std::wstring& uuid);
		bool IsUserExist(const std::wstring& uuid);

	private:
		NetUsersManager();
		~NetUsersManager();
		void UserConnectedNotify(const std::wstring& uuid);
		void UserDisconnectedNotify(const std::wstring& uuid);

	private:
		std::map<std::wstring, NetUserDataPtr> m_netUsers;
		std::vector<INetUsersObserver*> m_observers;
	};
}