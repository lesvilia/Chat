#include "stdafx.h"
#include "LoginManager.h"
#include "LoginManagerImpl.h"

namespace login
{
  UserData::UserData()
  {
  }

  UserData::UserData(const std::wstring& userName, const std::wstring& userPass, const std::wstring& userID) 
    : name(userName)
    , password(userPass)
    , uuid(userID)
  {
  }

  bool UserData::Empty() const
  {
    return name.empty() || password.empty() || uuid.empty();  
  }


  ILoginManager* LoginManager::Instance()
  {
    static LoginManager manager;
    return &manager;
  }

  
  LoginManager::LoginManager()
    : m_impl(new impl::LoginManagerImpl())
  {
  }

  LoginManager::~LoginManager()
  {
  }

  void LoginManager::LogIn(ILoginUIHandler* uiHandler)
  {
    m_impl->Login(uiHandler);
  }

  void LoginManager::LogOut()
  {
    m_impl->Logout();
  }

  bool LoginManager::IsOnline() const
  {
    return m_impl->IsOnline();
  }
  
  void LoginManager::Subscribe(ILoginStateObserver* observer)
  {
    m_impl->Subscribe(observer);
  }

  void LoginManager::Unsubscribe(ILoginStateObserver* observer)
  {
    m_impl->Unsubscribe(observer);
  }

  ILoginHandler* LoginManager::GetLoginHandler()
  {
    return this;
  }

  void LoginManager::SetLoginState(bool online)
  {
    m_impl->SetLoginState(online);
  }

  UserDataPtr LoginManager::GetCurrentUser() const
  {
    return m_impl->GetCurrentUser();
  }

  void LoginManager::AddNewUserData(const UserDataPtr& data)
  {
    m_impl->AddNewUserData(data);
  }

  void LoginManager::SetCurrentUser(const std::wstring& name)
  {
    m_impl->SetCurrentUser(name);
  }

  std::vector<UserDataPtr> LoginManager::GetUsersData() const
  {
    return m_impl->GetUsersData();
  }

  bool LoginManager::IsValidRegistrationData(const UserDataPtr& data)
  {
    return m_impl->IsValidRegistrationData(data);
  }

  bool LoginManager::IsValidLoginData(const UserDataPtr& data)
  {
    return m_impl->IsValidLoginData(data);
  }

  unsigned LoginManager::GetUserDataError(const UserDataPtr& data)
  {
    return m_impl->GetUserDataError(data);
  }
}
