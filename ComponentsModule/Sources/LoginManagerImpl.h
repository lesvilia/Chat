#pragma once
#include <atlbase.h>
#include <memory>
#include <map>
#include <vector>
#include "boost\noncopyable.hpp"
#include "boost\thread\shared_mutex.hpp"
#include "boost\thread\locks.hpp"
#include "LoginHandlers.h"

namespace login
{
  namespace impl
  {
    class LoginManagerImpl
      : private boost::noncopyable
    {
      typedef boost::shared_mutex SharedMutex;
      typedef boost::shared_lock<SharedMutex> ReadLock;
      typedef boost::unique_lock<SharedMutex> WriteLock;

    public:
      explicit LoginManagerImpl();
      ~LoginManagerImpl();
      void Login(ILoginUIHandler* uiHandler);
      void Logout();
      bool IsOnline() const;
      void Subscribe(ILoginStateObserver* observer);
      UserDataPtr GetCurrentUser() const;

      void AddNewUserData(const UserDataPtr& data);
      void SetCurrentUser(const std::wstring& name);
      void SetLoginState(bool online);
      std::vector<UserDataPtr> GetUsersData() const;
      bool IsValidRegistrationData(const UserDataPtr& data);
      bool IsValidLoginData(const UserDataPtr& data);
      unsigned GetUserDataError(const UserDataPtr& data);

    private:
      void Init();
      void GetUserData(CRegKey& userKey);
      void SaveUsersData();
      void NotifyObservers();

    private:
      std::map<std::wstring, UserDataPtr> m_users;
      std::vector<ILoginStateObserver*> m_obsrevers;
      mutable SharedMutex m_mutex;
      bool m_isOnline;
      UserDataPtr m_currentUser;
    };
  }
}