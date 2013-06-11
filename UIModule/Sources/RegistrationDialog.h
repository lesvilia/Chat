#pragma once
#include <QDialog>
#include <QLineEdit>
#include "LoginHandlers.h"

namespace ui
{
	namespace controls
	{
		class RegistrationDialog
			: public QDialog
		{
			Q_OBJECT
		public:
			RegistrationDialog(QWidget* parent, login::ILoginHandler* handler);
			~RegistrationDialog();
			bool NeedLogin() const;
			login::UserDataPtr GetUserData() const;

		private:
			void InitDialog();
			void HandleErrorData(const login::UserDataPtr& data);

		private slots:
			void DoRegistration();
			void SetNeedLogin();

		private:
			login::ILoginHandler* m_handler;
			login::UserDataPtr m_user;
			QLineEdit* m_nameEdit;
			QLineEdit* m_passEdit;
			bool m_needLogin;
		};
	}
}