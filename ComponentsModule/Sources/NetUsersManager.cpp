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

	NetUsersManager::NetUserData::NetUserData(const std::wstring& userName, const std::wstring& userAddress)
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

	void NetUsersManager::AddNewUser(const std::wstring& uuid, const std::wstring& name, const std::wstring& addr)
	{
		m_netUsers.insert(std::make_pair(uuid, std::make_shared<NetUserData>(name, addr)));
		UserConnectedNotify(uuid);
	}

	void NetUsersManager::RemoveUser(const std::wstring& uuid)
	{
		m_netUsers.erase(uuid);
		UserDisconnectedNotify(uuid);
	}

	std::wstring NetUsersManager::GetNetUserAddress(const std::wstring& uuid)
	{
		auto iter = m_netUsers.find(uuid);
		if (iter != m_netUsers.end())
		{
			return iter->second->address;
		}
		else
		{
			return std::wstring();
		}
	}

	std::vector<std::wstring> NetUsersManager::GetNetUserAddresses() const
	{
		std::vector<std::wstring> usersAddresses;
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