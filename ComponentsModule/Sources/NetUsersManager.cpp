#include "stdafx.h"
#include <algorithm>
#include "NetUsersManager.h"
#include "INetUsersObserver.h"
#include "LoginManager.h"

namespace net
{
	NetUsersManager::NetUserData::NetUserData()
	{
	}

	NetUsersManager::NetUserData::NetUserData(const std::wstring& userName, const ACE_INET_Addr& userAddress)
		: name(userName)
		, address(userAddress)
	{
	}

	NetUsersManager* NetUsersManager::Instance()
	{
		static NetUsersManager manager;
		return &manager;
	}

	NetUsersManager::NetUsersManager()
	{
		login::LoginManager::Instance()->Subscribe(this);
	}

	NetUsersManager::~NetUsersManager()
	{
	}

	void NetUsersManager::Subscribe(INetUsersObserver* observer)
	{
		m_observers.push_back(observer);
	}

	void NetUsersManager::AddNewUser(const std::wstring& uuid, const std::wstring& name, const ACE_INET_Addr& addr)
	{
		m_netUsers.insert(std::make_pair(uuid, std::make_shared<NetUserData>(name, addr)));
		UserConnectedNotify(uuid);
	}

	void NetUsersManager::RemoveUser(const std::wstring& uuid)
	{
		m_netUsers.erase(uuid);
		UserDisconnectedNotify(uuid);
	}

	ACE_INET_Addr NetUsersManager::GetNetUserAddress(const std::wstring& uuid)
	{
		auto iter = m_netUsers.find(uuid);
		if (iter != m_netUsers.end())
		{
			return iter->second->address;
		}
		else
		{
			return ACE_INET_Addr();
		}
	}

	std::vector<ACE_INET_Addr> NetUsersManager::GetNetUserAddresses() const
	{
		std::vector<ACE_INET_Addr> usersAddresses;
		for (auto it = m_netUsers.begin(); it != m_netUsers.end(); ++it)
		{
			usersAddresses.push_back(it->second->address);
		}
		return usersAddresses;
	}

	std::wstring NetUsersManager::GetNetUserName(const std::wstring& uuid)
	{
		auto iter = m_netUsers.find(uuid);
		if (iter != m_netUsers.end())
		{
			return iter->second->name;
		}
		else
		{
			return std::wstring();
		}
	}

	bool NetUsersManager::IsUserExist(const std::wstring& uuid)
	{
		return m_netUsers.find(uuid) != m_netUsers.end();
	}

	void NetUsersManager::OnlineStateChanged()
	{
		if (!login::LoginManager::Instance()->IsOnline())
		{
			for (auto it = m_netUsers.begin(); it != m_netUsers.end(); ++it)
			{
				UserDisconnectedNotify(it->first);
			}
			m_netUsers.clear();
		}
	}

	void NetUsersManager::UserConnectedNotify(const std::wstring& uuid)
	{
		std::for_each(m_observers.cbegin(), m_observers.cend(),
			[&uuid](INetUsersObserver* observer)
		{
			observer->OnNetUserConnected(uuid);
		});
	}

	void NetUsersManager::UserDisconnectedNotify(const std::wstring& uuid)
	{
		std::for_each(m_observers.cbegin(), m_observers.cend(),
			[&uuid](INetUsersObserver* observer)
		{
			observer->OnNetUserDisconnected(uuid);
		});
	}
}