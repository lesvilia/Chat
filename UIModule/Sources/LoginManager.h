#pragma once
#include <memory>
#include <QWidget>
#include "boost/noncopyable.hpp"
#include "LoginHandlers.h"

namespace login
{
	namespace impl
	{
		class LoginManagerImpl;
	}

	struct UserData
	{
		UserData();
		UserData(const std::wstring& n, const std::wstring& p); 
		bool Empty() const;

		std::wstring name;
		std::wstring password;
	};

	class LoginManager
		: public ILoginHandler
		, private boost::noncopyable
	{
	public:
		static LoginManager* Instance();
		void Login(QWidget* parent);
		void Logout();
		bool IsOnline() const;
		void Subscrabe(ILoginStateObserver* observer);
		UserDataPtr GetCurrentUser() const;

		//ILoginHandler interface
		virtual void AddNewUserData(const UserDataPtr& data);
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