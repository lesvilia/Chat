#include "LoginDialog.h"
#include "LoginManager.h"
#include "StaticLink.h"
#include "UISettings.h"
#include "QtHelpers.h"
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>


namespace ui
{
	namespace controls
	{
		using namespace settings::logindialog;
		namespace
		{
			login::UserDataPtr CreateUserData(QLineEdit* nameEdit, QLineEdit* passwordEdit)
			{
				std::wstring name(hlp::QStrToWStr(nameEdit->text()));
				std::wstring password(hlp::QStrToWStr(passwordEdit->text()));
				return std::make_shared<login::UserData>(name, password);
			}
		}

		LoginDialog::LoginDialog(QWidget* parent, login::ILoginHandler* handler)
			: QDialog(parent)
			, m_handler(handler)
			, m_mainLayout(new QStackedLayout())
			, m_loginWidget(new QWidget())
			, m_logNameEdit(nullptr)
			, m_logPassEdit(nullptr)
			, m_registrationWidget(new QWidget())
			, m_regNameEdit(nullptr)
			, m_regPassEdit(nullptr)
		{
			InitDialog();
		}

		LoginDialog::~LoginDialog()
		{
		}

		void LoginDialog::InitDialog()
		{
			SetupUI();

			if (m_handler->GetUsersData().empty())
			{
				SwitchToRegistrationMode();
			}
			else
			{
				SwitchToLoginMode();
			}
		}

		void LoginDialog::SetupUI()
		{
			SetUILogin();
			SetUIRegistration();
			m_mainLayout->addWidget(m_loginWidget);
			m_mainLayout->addWidget(m_registrationWidget);
			setLayout(m_mainLayout);
			resize(DIALOG_WIDTH, DIALOG_HEIGHT);
		}

		void LoginDialog::SwitchToRegistrationMode()
		{
			m_mainLayout->setCurrentWidget(m_registrationWidget);
			setWindowTitle(REGISTRATION_TITLE);
		}

		void LoginDialog::SwitchToLoginMode()
		{
			m_mainLayout->setCurrentWidget(m_loginWidget);
			setWindowTitle(LOGIN_TITLE);
		}

		void LoginDialog::SetUILogin()
		{
			QPushButton* buttonOk = new QPushButton("Ok");
			QPushButton* buttonCancel = new QPushButton("Cancel");
			connect(buttonOk, SIGNAL(clicked()), SLOT(DoLogin()));
			connect(buttonCancel, SIGNAL(clicked()), SLOT(reject()));
			
			buttonOk->setAutoDefault(true);
			buttonCancel->setAutoDefault(false);

			QLabel* labelName = new QLabel(LABEL_NAME); 
			QLabel* labelPassword = new QLabel(LABEL_PASSWORD);

			StaticLink* link = new StaticLink();
			link->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			link->setText(hlp::SetLinkStyle("Registration"));
			connect(link, SIGNAL(clicked()), SLOT(SwitchToRegistrationMode()));

			m_logNameEdit = new QLineEdit();
			m_logPassEdit = new QLineEdit();
			m_logPassEdit->setEchoMode(QLineEdit::Password);
			QGridLayout* mainLayot = new QGridLayout();

			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(m_logNameEdit, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(m_logPassEdit, 1, 2, 1, 3);
			mainLayot->addWidget(link, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			m_loginWidget->setLayout(mainLayot);
		}

		void LoginDialog::SetUIRegistration()
		{
			QPushButton* buttonOk = new QPushButton("Registration");
			QPushButton* buttonCancel = new QPushButton("Cancel");
			connect(buttonOk, SIGNAL(clicked()), SLOT(DoRegistration()));
			connect(buttonCancel, SIGNAL(clicked()), SLOT(reject()));
			
			buttonOk->setAutoDefault(true);
			buttonCancel->setAutoDefault(false);

			QLabel* labelName = new QLabel(LABEL_NAME); 
			QLabel* labelPassword = new QLabel(LABEL_PASSWORD);

			QLabel* loginLink = new StaticLink();
			loginLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			loginLink->setText(hlp::SetLinkStyle("Log In"));
			connect(loginLink, SIGNAL(clicked()), SLOT(SwitchToLoginMode()));

			m_regNameEdit = new QLineEdit();
			m_regPassEdit = new QLineEdit();
			m_regPassEdit->setEchoMode(QLineEdit::Password);
			QGridLayout* mainLayot = new QGridLayout();

			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(m_regNameEdit, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(m_regPassEdit, 1, 2, 1, 3);
			mainLayot->addWidget(loginLink, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			m_registrationWidget->setLayout(mainLayot);
		}

		void LoginDialog::DoLogin()
		{
			login::UserDataPtr data(CreateUserData(m_logNameEdit, m_logPassEdit));
			if (m_handler->IsValidLoginData(data))
			{
				m_handler->SetCurrentUser(data);
				m_handler->SetLoginState(true);
				accept();
			}
			else
			{
				HandleLoginError(data);
			}
		}

		void LoginDialog::DoRegistration()
		{
			login::UserDataPtr data(CreateUserData(m_regNameEdit, m_regPassEdit));
			if (m_handler->IsValidRegistrationData(data))
			{
				m_handler->AddNewUserData(data);
				m_handler->SetCurrentUser(data);
				m_handler->SetLoginState(true);
				accept();
			}
			else
			{
				HandleRegistrationError(data);
			}
		}

		void LoginDialog::HandleLoginError(const login::UserDataPtr& data)
		{
			if (!data->name.empty() && !data->password.empty())
			{	
				unsigned error = m_handler->GetUserDataError(data);
				if (error & login::USER_NOT_FOUND)
				{
					QMessageBox::warning(this, MSGBOX_LOGIN_TITLE, MSGBOX_USER_NOT_FOUND);
				}
				else if (error & login::WRONG_USER_PASSWORD)
				{
					QMessageBox::warning(this, MSGBOX_LOGIN_TITLE, MSGBOX_WRONG_PASS);
				}
			}
			else
			{
				QMessageBox::warning(this, MSGBOX_LOGIN_TITLE, MSGBOX_DATA_EMPTY);
			}
		}

		void LoginDialog::HandleRegistrationError(const login::UserDataPtr& data)
		{
			if (!data->name.empty() && !data->password.empty())
			{
				if (m_handler->GetUserDataError(data) & login::USER_EXISTS)
				{
					QMessageBox::warning(this, MSGBOX_REG_TITLE, MSGBOX_USER_EXIST);
				}
			}
			else
			{
				QMessageBox::warning(this, MSGBOX_REG_TITLE, MSGBOX_DATA_EMPTY);
			}
		}
	}
}