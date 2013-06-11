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
			std::wstring QStrToWStr(const QString& str)
			{
				return std::wstring((wchar_t*)str.unicode(), str.length());
			}
		}

		LoginDialog::LoginDialog(QWidget* parent, login::ILoginHandler* handler)
			: QDialog(parent)
			, m_handler(handler)
			, m_user(nullptr)
			, m_nameEdit(nullptr)
			, m_passEdit(nullptr)
			, m_needRegistration(false)
		{
			InitDialog();
		}

		LoginDialog::~LoginDialog()
		{
		}

		void LoginDialog::InitDialog()
		{
			QPushButton* buttonOk = new QPushButton("Ok");
			QPushButton* buttonCancel = new QPushButton("Cancel");
			connect(buttonOk, SIGNAL(clicked()), SLOT(DoLogin()));
			connect(buttonCancel, SIGNAL(clicked()), SLOT(reject()));
			
			QLabel* labelName = new QLabel("Name:"); 
			QLabel* labelPassword = new QLabel("Password:");
			
			StaticLink* link = new StaticLink();
			link->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			link->setText("<a href=\"registration\">Registration</a>");
			connect(link, SIGNAL(clicked()), SLOT(SetNeedRegistartion()));

			m_nameEdit = new QLineEdit();
			m_passEdit = new QLineEdit();
			QGridLayout* mainLayot = new QGridLayout();
			
			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(m_nameEdit, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(m_passEdit, 1, 2, 1, 3);
			mainLayot->addWidget(link, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			setLayout(mainLayot);
			setWindowTitle("Login");
			resize(300, 150);
		}

		login::UserDataPtr LoginDialog::GetUserData() const
		{
			return m_user;
		}

		void LoginDialog::DoLogin()
		{
			std::wstring name(QStrToWStr(m_nameEdit->text()));
			std::wstring password(QStrToWStr(m_passEdit->text()));
			login::UserDataPtr data(std::make_shared<login::UserData>(name, password));
			
			if (m_handler->IsValidLoginData(data))
			{
				m_user.swap(data);
				accept();
			}
			else
			{
				HandleErrorData(data);
			}
		}

		void LoginDialog::HandleErrorData(const login::UserDataPtr& data)
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

		bool LoginDialog::NeedRegistration() const
		{
			return m_needRegistration;
		}

		void LoginDialog::SetNeedRegistartion()
		{
			m_needRegistration = true;
			reject();
		}
	}
}