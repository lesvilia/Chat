#pragma once
#include <atlbase.h>
#include <memory>
#include <map>
#include "boost/noncopyable.hpp"
#include "LoginHandlers.h"

namespace login
{
	namespace impl
	{
		class LoginManagerImpl
			: private boost::noncopyable
		{
		public:
			LoginManagerImpl();
			~LoginManagerImpl();
			void Login(ILoginUIHandler* handler);
			void AddNewUserData(const UserDataPtr& data);
			bool IsValidRegistrationData(const UserDataPtr& data);
			bool IsValidLoginData(const UserDataPtr& data);
			unsigned GetUserDataError(const UserDataPtr& data);

		private:
			void Init();
			void GetUserData(CRegKey& userKey);

		private:
			std::map<std::wstring, UserDataPtr> m_users;
		};
	}
}