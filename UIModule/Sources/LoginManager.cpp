#include "LoginManager.h"
#include <string>
#include <vector>


namespace login
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

	LoginManager::LoginManager()
	{
		Init();
	}

	void LoginManager::Init()
	{
		CRegKey key;
		if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, USERS_KEYS_PATH.c_str(), KEY_READ))
		{
			DWORD index = 0;
			DWORD nameLength = MAX_STRING_LENGTH;
			LONG nRet = 0;
			std::vector<wchar_t> buff(MAX_STRING_LENGTH, '\0');
			while ((nRet = key.EnumKey(index, &buff[0], &nameLength)) != ERROR_NO_MORE_ITEMS)
			{
				if(nRet == ERROR_SUCCESS)
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

	bool LoginManager::ValidUserData(UserDataPtr userdata)
	{
		return !userdata->name.empty() && !userdata->password.empty();
	}

	void LoginManager::GetUserData(CRegKey& userKey)
	{
		UserDataPtr user(std::make_shared<UserData>
			(GetStringValue(userKey, USER_NAME)
			, GetStringValue(userKey, USER_PASSWORD)));

		if (ValidUserData(user))
		{
			m_users.insert(std::make_pair(user->name, user));
		}
	}

	void LoginManager::ShowLoginDlg(QWidget* parent)
	{

	}

	void LoginManager::ShowRegistrationDlg(QWidget* parent)
	{

	}
}