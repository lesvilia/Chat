#include "LoginManager.h"
#include "LoginManagerImpl.h"

namespace login
{
	UserData::UserData()
	{
	}

	UserData::UserData(const std::wstring& n, const std::wstring& p) 
		: name(n)
		, password(p)
	{
	}

	bool UserData::Empty() const
	{
		return name.empty() && password.empty();  
	}


	LoginManager* LoginManager::Instance()
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

	void LoginManager::Login(ILoginUIHandler* handler)
	{
		m_impl->Login(handler);
	}

	void LoginManager::AddNewUserData(const UserDataPtr& data)
	{
		m_impl->AddNewUserData(data);
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