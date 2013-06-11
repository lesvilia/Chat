#pragma once
#include <QDialog>
#include <QLineEdit>
#include "LoginHandlers.h"

namespace ui
{
	namespace controls
	{
		class LoginDialog
			: public QDialog
		{
			Q_OBJECT
		public:
			LoginDialog(QWidget* parent, login::ILoginHandler* handler);
			~LoginDialog();
			bool NeedRegistration() const;
			login::UserDataPtr GetUserData() const;

		private:
			void InitDialog();
			void HandleErrorData(const login::UserDataPtr& data);

		private slots:
			void SetNeedRegistartion();
			void DoLogin();

		private:
			login::ILoginHandler* m_handler;
			login::UserDataPtr m_user;
			QLineEdit* m_nameEdit;
			QLineEdit* m_passEdit;
			bool m_needRegistration;
		};
	}
}
