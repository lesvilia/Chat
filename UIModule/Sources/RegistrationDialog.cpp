#include "RegistrationDialog.h"
#include "StaticLink.h"
#include "LoginManager.h"
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QMessageBox>

namespace ui
{
	namespace controls
	{
		namespace
		{
			std::wstring QStrToWStr(const QString& str)
			{
				return std::wstring((wchar_t*)str.unicode(), str.length());
			}
		}

		RegistrationDialog::RegistrationDialog(QWidget* parent, login::ILoginHandler* handler)
			: QDialog(parent)
			, m_handler(handler)
			, m_user(nullptr)
			, m_nameEdit(nullptr)
			, m_passEdit(nullptr)
			, m_needLogin(false)
		{
			InitDialog();
		}

		RegistrationDialog::~RegistrationDialog()
		{
		}

		void RegistrationDialog::InitDialog()
		{
			QPushButton* buttonOk = new QPushButton("Add new user");
			QPushButton* buttonCancel = new QPushButton("Cancel");
			connect(buttonOk, SIGNAL(clicked()), SLOT(DoRegistration()));
			connect(buttonCancel, SIGNAL(clicked()), SLOT(reject()));

			QLabel* labelName = new QLabel("Name:"); 
			QLabel* labelPassword = new QLabel("Password:");

			QLabel* loginLink = new StaticLink();
			loginLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			loginLink->setText("<a href=\"login\">Log_In</a>");
			connect(loginLink, SIGNAL(clicked()), SLOT(SetNeedLogin()));

			m_nameEdit = new QLineEdit();
			m_passEdit = new QLineEdit();
			QGridLayout* mainLayot = new QGridLayout();

			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(m_nameEdit, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(m_passEdit, 1, 2, 1, 3);
			mainLayot->addWidget(loginLink, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			setLayout(mainLayot);
			setWindowTitle("Registration");
			resize(300, 150);
		}

		void RegistrationDialog::HandleErrorData(const login::UserDataPtr& data)
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

		login::UserDataPtr RegistrationDialog::GetUserData() const
		{
			return m_user;
		}

		void RegistrationDialog::DoRegistration()
		{
			std::wstring name(QStrToWStr(m_nameEdit->text()));
			std::wstring password(QStrToWStr(m_passEdit->text()));
			login::UserDataPtr data(std::make_shared<login::UserData>(name, password));
			if (m_handler->IsValidRegistrationData(data))
			{
				m_handler->AddNewUserData(data);
				m_user.swap(data);
				accept();
			}
			else
			{
				HandleErrorData(data);
			}
		}

		void RegistrationDialog::SetNeedLogin()
		{
			m_needLogin = true;
			reject();
		}

		bool RegistrationDialog::NeedLogin() const
		{
			return m_needLogin;
		}
	}
}