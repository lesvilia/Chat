#pragma once
#include <QtWidgets/QWidget>
#include <atlbase.h>
#include <memory>
#include "boost/noncopyable.hpp"
#include <map>

namespace login
{
	struct UserData
	{
		UserData() { }
		UserData(const std::wstring& n, const std::wstring& p) 
			: name(n)
			, password(p) { }
		std::wstring name;
		std::wstring password;
	};

	typedef std::shared_ptr<UserData> UserDataPtr;

	class LoginManager
		: private boost::noncopyable
	{
	public:
		LoginManager();
		void ShowLoginDlg(QWidget* parent = nullptr);
		void ShowRegistrationDlg(QWidget* parent = nullptr);

	private:
		void Init();
		void GetUserData(CRegKey& userKey);
		bool ValidUserData(UserDataPtr userdata);
	private:
		std::map<std::wstring, UserDataPtr> m_users;
	};
}