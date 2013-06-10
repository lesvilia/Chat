#include "LoginManagerImpl.h"
#include "LoginManager.h"
#include <string>
#include <vector>

namespace login
{
	namespace impl
	{
		namespace
		{
			const std::wstring USERS_KEYS_PATH(L"Software\\LChat\\Users");
			const std::wstring USER_NAME(L"Name");
			const std::wstring USER_PASSWORD(L"Password");
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
		{
			Init();
		}

		LoginManagerImpl::~LoginManagerImpl()
		{
		}

		void LoginManagerImpl::Init()
		{
			CRegKey key;
			if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, USERS_KEYS_PATH.c_str(), KEY_READ))
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

		void LoginManagerImpl::Login(ILoginUIHandler* handler)
		{
			if (handler)
			{
				if (m_users.empty())	//no registration users
				{
					handler->ShowRegistrationDlg();
				}
				else
				{
					handler->ShowLoginDlg();
				}
			}
		}

		void LoginManagerImpl::AddNewUserData(const UserDataPtr& data)
		{
			m_users[data->name] = data;
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


