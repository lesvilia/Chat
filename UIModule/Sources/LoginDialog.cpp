#include "LoginDialog.h"
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>


namespace ui
{
	namespace controls
	{
		LoginDialog::LoginDialog(QWidget* parent, login::ILoginHandler* handler)
			: QDialog(parent)
			, m_parent(parent)
			, m_handler(handler)
		{
			InitDialog();
		}

		void LoginDialog::InitDialog()
		{
			QPushButton* buttonOk = new QPushButton("Ok");
			QPushButton* buttonCancel = new QPushButton("Cancel");
			QLabel* labelName = new QLabel("Name:"); 
			QLabel* labelPassword = new QLabel("Password:");
			
			QLabel* labelRegistration = new QLabel("<a href=\"google.com\">Registration</a>");
			labelRegistration->setTextFormat(Qt::RichText);
			labelRegistration->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

			QLineEdit* txtEditName = new QLineEdit();
			QLineEdit* txtEditPassword = new QLineEdit();
			QGridLayout* mainLayot = new QGridLayout();
			
			mainLayot->addWidget(labelName, 0, 0, 1, 2);
			mainLayot->addWidget(txtEditName, 0, 2, 1, 3);
			mainLayot->addWidget(labelPassword, 1, 0, 1, 2);
			mainLayot->addWidget(txtEditPassword, 1, 2, 1, 3);
			mainLayot->addWidget(labelRegistration, 2, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 2, 2, 1, 1);
			mainLayot->addWidget(buttonOk, 2, 4, 1, 1);
			setLayout(mainLayot);
			setWindowTitle("Login");
			resize(300, 150);
		}
	}
}