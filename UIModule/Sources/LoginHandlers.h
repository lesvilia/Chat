#pragma once
#include <string>
#include <memory>


namespace login
{
	class UserData;
	typedef std::shared_ptr<UserData> UserDataPtr;

	enum LoginErr
	{
		NO_LOGIN_ERROR		= 0, // always is NULL
		USER_EXISTS			= 1,
		WRONG_USER_PASSWORD	= 2,
		USER_NOT_FOUND		= 4
	};

	class ILoginHandler
	{
	public:
		virtual ~ILoginHandler() {}
		virtual void AddNewUserData(const UserDataPtr& data) = 0;
		virtual bool IsValidRegistrationData(const UserDataPtr& data) = 0;
		virtual bool IsValidLoginData(const UserDataPtr& data) = 0;
		virtual unsigned GetUserDataError(const UserDataPtr& data) = 0;
	};

	class ILoginStateObserver
	{
	public:
		virtual ~ILoginStateObserver() {}
		virtual void OnlineStateChanged() = 0;
	};
}