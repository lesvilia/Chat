#include "LoginManagerImpl.h"
#include "LoginManager.h"
#include "LoginDialog.h"
#include <string>
#include <vector>

namespace login
{
	namespace impl
	{
		namespace
		{
			const wchar_t USERS_KEYS_PATH[] = L"Software\\LChat\\Users";
			const wchar_t USER_NAME[] = L"Name";
			const wchar_t USER_PASSWORD[] = L"Password";
			const DWORD MAX_STRING_LENGTH = 128;

			std::wstring GetStringValue(CRegKey& key, const std::wstring& valueName)
			{
				ULONG nChars = 0;
				if (ERROR_SUCCESS == key.QueryStringValue(valueName.c_str(), NULL, &nChars))
				{
					std::vector<wchar_t> buff(nChars, '\0');
					if (ERROR_SUCCESS == key.QueryStringValue(valueName.c_str(), &buff[0], &nChars))
					{
						return std::wstring(&buff[0]);
					}
				}
				return std::wstring();
			}
		}

		LoginManagerImpl::LoginManagerImpl()
			: m_isOnline(false)
			, m_currentUser(nullptr)
		{
			Init();
		}

		LoginManagerImpl::~LoginManagerImpl()
		{
			SaveUsersData();
		}

		void LoginManagerImpl::Init()
		{
			CRegKey key;
			if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, USERS_KEYS_PATH, KEY_READ))
			{
				DWORD index = 0;
				DWORD nameLength = MAX_STRING_LENGTH;
				LONG result = 0;
				std::vector<wchar_t> buff(MAX_STRING_LENGTH, '\0');
				while ((result = key.EnumKey(index, &buff[0], &nameLength)) != ERROR_NO_MORE_ITEMS)
				{
					if(result == ERROR_SUCCESS)
					{
						CRegKey userKey;
						std::wstring userPath(&buff[0]);
						if (ERROR_SUCCESS == userKey.Open(key, userPath.c_str(), KEY_READ))
						{
							GetUserData(userKey);
						}
					}
					++index;
					nameLength = MAX_STRING_LENGTH;
				}
			}
		}

		void LoginManagerImpl::GetUserData(CRegKey& userKey)
		{
			UserDataPtr user(std::make_shared<UserData>
				(GetStringValue(userKey, USER_NAME)
				, GetStringValue(userKey, USER_PASSWORD)));

			if (!user->Empty())
			{
				m_users.insert(std::make_pair(user->name, user));
			}
		}

		void LoginManagerImpl::SaveUsersData()
		{
			CRegKey key;
			if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, USERS_KEYS_PATH, KEY_WRITE))
			{
				std::for_each(m_users.begin(), m_users.end(),
				[&key](const std::pair<std::wstring, UserDataPtr>& it)
				{
					CRegKey newKey;
					UserDataPtr data(it.second);
					if (ERROR_SUCCESS == newKey.Create(key, data->name.c_str()))
					{
						newKey.SetStringValue(USER_NAME, data->name.c_str());
						newKey.SetStringValue(USER_PASSWORD, data->password.c_str());
					}
				});				
			}
		}

		void LoginManagerImpl::Login(ILoginUIHandler* uiHandler)
		{
			uiHandler->EnableLoginUI();
		}

		void LoginManagerImpl::Logout()
		{
			SetLoginState(false);
		}

		bool LoginManagerImpl::IsOnline() const
		{
			boost::mutex::scoped_lock lock(m_mutex);
			return m_isOnline;
		}

		void LoginManagerImpl::SetLoginState(bool online)
		{
			{
				boost::mutex::scoped_lock lock(m_mutex);
				m_isOnline = online;
			}

			NotifyObservers();
		}

		void LoginManagerImpl::NotifyObservers()
		{
			std::for_each(m_obsrevers.begin(), m_obsrevers.end(),
			[](ILoginStateObserver* observer)
			{
				if (observer)
				{
					observer->OnlineStateChanged();
				}
			});
		}

		void LoginManagerImpl::Subscribe(ILoginStateObserver* observer)
		{
			m_obsrevers.push_back(observer);
		}

		void LoginManagerImpl::AddNewUserData(const UserDataPtr& data)
		{
			m_users[data->name] = data;
		}

		UserDataPtr LoginManagerImpl::GetCurrentUser() const
		{
			return m_currentUser;
		}

		void LoginManagerImpl::SetCurrentUser(const UserDataPtr& data)
		{
			m_currentUser = data;  
		}

		std::vector<UserDataPtr> LoginManagerImpl::GetUsersData() const
		{
			std::vector<UserDataPtr> users;
			std::for_each(m_users.begin(), m_users.end(),
			[&users](const std::pair<std::wstring, UserDataPtr>& it)
			{
				users.push_back(it.second);
			});
			return users;
		}

		bool LoginManagerImpl::IsValidRegistrationData(const UserDataPtr& data)
		{
			return !data->Empty() && m_users.find(data->name) == m_users.end();
		}

		bool LoginManagerImpl::IsValidLoginData(const UserDataPtr& data)
		{
			auto iter = m_users.find(data->name);
			return !data->Empty() && iter != m_users.end() && iter->second->password == data->password;
		}

		unsigned LoginManagerImpl::GetUserDataError(const UserDataPtr& data)
		{
			unsigned result = NO_LOGIN_ERROR;
			auto iter = m_users.find(data->name);
			if (iter != m_users.end())
			{
				result |= USER_EXISTS;
			
				if (iter->second->password != data->password)
				{
					result |= WRONG_USER_PASSWORD;
				}
			}
			else
			{
				result |= USER_NOT_FOUND;
			}

			return result;
		}

	}//impl
}//login


