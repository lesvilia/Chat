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
			explicit LoginManagerImpl(ILoginHandler* handler);
			~LoginManagerImpl();
			void Login(QWidget* parent);
			void Logout();
			bool IsOnline() const;
			void Subscrabe(ILoginStateObserver* observer);
			UserDataPtr GetCurrentUser() const;

			void AddNewUserData(const UserDataPtr& data);
			bool IsValidRegistrationData(const UserDataPtr& data);
			bool IsValidLoginData(const UserDataPtr& data);
			unsigned GetUserDataError(const UserDataPtr& data);

		private:
			void Init();
			void ShowRegistrationDlg(QWidget* parent);
			void ShowLoginDlg(QWidget* parent);
			void GetUserData(CRegKey& userKey);
			void SaveUsersData();
			void SetLoginState(bool online);
			void NotifyObservers();

		private:
			std::map<std::wstring, UserDataPtr> m_users;
			std::vector<ILoginStateObserver*> m_obsrevers;
			mutable boost::mutex m_mutex;
			ILoginHandler* m_handler;
			bool m_isOnline;
			UserDataPtr m_currentUser;
		};
	}
}