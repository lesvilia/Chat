#pragma once
#include <map>
#include <vector>
#include <memory>
#include <string>
#include "ace\INET_Addr.h"
#include "boost\noncopyable.hpp"
#include "boost\thread\mutex.hpp"
#include "boost\thread\locks.hpp"

namespace net
{
  class INetUsersObserver;

  class NetUsersManager
    : private boost::noncopyable
  {
    struct NetUserData
    {
      NetUserData();
      NetUserData(const std::wstring& userName, const std::wstring& userAddress);
      std::wstring name;
      std::wstring address;
    };

    typedef std::shared_ptr<NetUserData> NetUserDataPtr;
    typedef boost::mutex Mutex;
    typedef boost::unique_lock<Mutex> Lock;

  public:
    static NetUsersManager* Instance();
    void Subscribe(INetUsersObserver* observer);
    void Unsubscribe(INetUsersObserver* observer);
    void AddNewUser(const std::wstring& uuid, const std::wstring& name, const std::wstring& addr);
    void RemoveUser(const std::wstring& uuid);
    void ClearOnlineUsers();
    std::wstring GetNetUserAddress(const std::wstring& uuid);
    std::vector<std::wstring> GetNetUserAddresses() const;
    std::wstring GetNetUserName(const std::wstring& uuid);
    bool IsUserExist(const std::wstring& uuid);

  private:
    NetUsersManager();
    ~NetUsersManager();
    void UserConnectedNotify(const std::wstring& uuid);
    void UserDisconnectedNotify(const std::wstring& uuid);
    std::vector<INetUsersObserver*> CopyObservers() const;
  private:
    std::map<std::wstring, NetUserDataPtr> m_netUsers;
    std::vector<INetUsersObserver*> m_observers;
    mutable Mutex m_observerMutex;
  };
}
