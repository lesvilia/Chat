#include "LoginDialog.h"
#include "LoginManager.h"
#include "StaticLink.h"
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>


namespace ui
{
	namespace controls
	{
		namespace
		{
			const unsigned DIALOG_WIDTH = 300;
			const unsigned DIALOG_HEIGHT = 150;
			const QString LOGIN_TITLE("Log In");
			const QString REGISTRATION_TITLE("Registration");

			std::wstring QStrToWStr(const QString& str)
			{
				return std::wstring((wchar_t*)str.unicode(), str.length());
			}
		}

		LoginDialog::LoginDialog(QWidget* parent, login::ILoginHandler* handler)
			: QDialog(parent)
			, m_handler(handler)
			, m_mainLayout(new QStackedLayout())
			, m_loginWidget(nullptr)
			, m_logNameEdit(nullptr)
			, m_logPassEdit(nullptr)
			, m_registrationWidget(nullptr)
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
			SetUIRegistration();
			SetUILogin();
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

			QLabel* labelName = new QLabel("Name:"); 
			QLabel* labelPassword = new QLabel("Password:");

			StaticLink* link = new StaticLink();
			link->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			link->setText("<a href=\"registration\">Registration</a>");
			connect(link, SIGNAL(clicked()), SLOT(SwitchToRegistrationMode()));

			m_logNameEdit = new QLineEdit();
			m_logPassEdit = new QLineEdit();
			QGridLayout* mainLayot = new QGridLayout();

			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(m_logNameEdit, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(m_logPassEdit, 1, 2, 1, 3);
			mainLayot->addWidget(link, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			m_loginWidget = new QWidget();
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

			QLabel* labelName = new QLabel("Name:"); 
			QLabel* labelPassword = new QLabel("Password:");

			QLabel* loginLink = new StaticLink();
			loginLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			loginLink->setText("<a href=\"login\">Log In</a>");
			connect(loginLink, SIGNAL(clicked()), SLOT(SwitchToLoginMode()));

			m_regNameEdit = new QLineEdit();
			m_regPassEdit = new QLineEdit();
			QGridLayout* mainLayot = new QGridLayout();

			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(m_regNameEdit, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(m_regPassEdit, 1, 2, 1, 3);
			mainLayot->addWidget(loginLink, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			m_registrationWidget = new QWidget();
			m_registrationWidget->setLayout(mainLayot);
		}

		void LoginDialog::DoLogin()
		{
			std::wstring name(QStrToWStr(m_logNameEdit->text()));
			std::wstring password(QStrToWStr(m_logPassEdit->text()));
			login::UserDataPtr data(std::make_shared<login::UserData>(name, password));
			
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
			std::wstring name(QStrToWStr(m_regNameEdit->text()));
			std::wstring password(QStrToWStr(m_regPassEdit->text()));
			login::UserDataPtr data(std::make_shared<login::UserData>(name, password));
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
			if (!data->Empty())
			{	
				unsigned error = m_handler->GetUserDataError(data);
				if (error & login::USER_NOT_FOUND)
				{
					QMessageBox::warning(this, tr("Login Error!"), tr("User not found! Please check entered name."));
				}
				else if (error & login::WRONG_USER_PASSWORD)
				{
					QMessageBox::warning(this, tr("Login Error!"), tr("Wrong user password!"));
				}
			}
			else
			{
				QMessageBox::warning(this, tr("Login Error!"), tr("You not entered Name or Password."));
			}
		}

		void LoginDialog::HandleRegistrationError(const login::UserDataPtr& data)
		{
			if (!data->Empty())
			{
				if (m_handler->GetUserDataError(data) & login::USER_EXISTS)
				{
					QMessageBox::warning(this, tr("Registration Error!"), tr("User already exists! Please enter other name."));
				}
			}
			else
			{
				QMessageBox::warning(this, tr("Registration Error!"), tr("You not entered Name or Password."));
			}
		}
	}
}