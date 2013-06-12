#pragma once
#include <atlbase.h>
#include <memory>
#include <map>
#include <vector>
#include "boost\noncopyable.hpp"
#include "boost\thread\mutex.hpp"
#include "LoginHandlers.h"

class QWidget;
namespace login
{
	namespace impl
	{
		class LoginManagerImpl
			: private boost::noncopyable
		{
		public:
			explicit LoginManagerImpl();
			~LoginManagerImpl();
			void Login(ILoginUIHandler* uiHandler);
			void Logout();
			bool IsOnline() const;
			void Subscribe(ILoginStateObserver* observer);
			UserDataPtr GetCurrentUser() const;

			void AddNewUserData(const UserDataPtr& data);
			void SetCurrentUser(const UserDataPtr& data);
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
			mutable boost::mutex m_mutex;
			bool m_isOnline;
			UserDataPtr m_currentUser;
		};
	}
}