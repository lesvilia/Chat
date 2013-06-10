#pragma once
#include <QDialog>
#include "LoginHandlers.h"

namespace ui
{
	namespace controls
	{
		class LoginDialog
			: public QDialog
		{
		public:
			LoginDialog(QWidget* parent, login::ILoginHandler* handler);
			
		private:
			void InitDialog();

		private slots:
			void OpenRegistrationDlg();

		private:
			QWidget* m_parent;
			login::ILoginHandler* m_handler;
		};
	}
}
