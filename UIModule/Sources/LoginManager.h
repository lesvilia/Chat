#pragma once
#include <memory>
#include <QWidget>
#include "boost/noncopyable.hpp"
#include "ILoginManager.h"

namespace login
{
	namespace impl
	{
		class LoginManagerImpl;
	}

	struct UserData
	{
		UserData();
		UserData(const std::wstring& userName, const std::wstring& userPass, const std::wstring& userID = std::wstring()); 
		bool Empty() const;

		std::wstring name;
		std::wstring password;
		std::wstring uuid;
	};

	class LoginManager
		: public ILoginManager
		, public ILoginHandler
		, private boost::noncopyable
	{
	public:
		static ILoginManager* Instance();

		//ILoginManager interface
		virtual void LogIn(ILoginUIHandler* uiHandler);
		virtual void LogOut();
		virtual bool IsOnline() const;
		virtual UserDataPtr GetCurrentUser() const;
		virtual void Subscribe(ILoginStateObserver* observer);
		virtual ILoginHandler* GetLoginHandler();

		//ILoginHandler interface
		virtual void AddNewUserData(const UserDataPtr& data);
		virtual void SetCurrentUser(const UserDataPtr& data);
		virtual void SetLoginState(bool online);
		virtual std::vector<UserDataPtr> GetUsersData() const;
		virtual bool IsValidRegistrationData(const UserDataPtr& data);
		virtual bool IsValidLoginData(const UserDataPtr& data);
		virtual unsigned GetUserDataError(const UserDataPtr& data);

	private:
		LoginManager();
		~LoginManager();
		
	private:
		std::unique_ptr<impl::LoginManagerImpl> m_impl;
	};
}