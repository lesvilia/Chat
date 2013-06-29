#include "SettingsDialog.h"
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <vector>
#include <algorithm>
#include "StaticLink.h"
#include "Settings.h"
#include "QtHelpers.h"
#include "MsgModuleSettings.h"
#include "AdaptersAddressManager.h"
#include "SettingsManager.h"

namespace ui
{
	namespace controls
	{
		using namespace settings::settingsdialog;
		using namespace settings::settingsmanager;

		SettingsDialog::SettingsDialog(QWidget* parent)
			: QDialog(parent)
			, m_addressWidget(nullptr)
			, m_statePortEdit(nullptr)
			, m_chatPortEdit(nullptr)
		{
			InitDialog();
		}

		SettingsDialog::~SettingsDialog()
		{
		}

		void SettingsDialog::InitDialog()
		{
			InitAddressesValues();
			SetupUI();
		}

		void SettingsDialog::SetupUI()
		{
			m_buttonOK = new QPushButton("Ok");
			QPushButton* buttonCancel = new QPushButton("Cancel");
			m_buttonOK->setEnabled(false);
			connect(m_buttonOK, SIGNAL(clicked()), SLOT(SaveSettings()));
			connect(buttonCancel, SIGNAL(clicked()), SLOT(reject()));

			QLabel* labelAddres = new QLabel(LABEL_ADDRESS); 
			QLabel* labelStatePort = new QLabel(LABEL_STATE_PORT);
			QLabel* labelChatPort = new QLabel(LABEL_CHAT_PORT);

			StaticLink* link = new StaticLink();
			link->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			link->setText(hlp::SetLinkStyle("Restore default settings"));
			connect(link, SIGNAL(clicked()), SLOT(RestoreDefaultSettings()));

			m_addressWidget = new QComboBox();
			m_addressWidget->setEditable(false);
			m_addressWidget->insertItems(0, m_addresses);
			m_addressWidget->setCurrentText(m_appropriateAddress);
			connect(m_addressWidget, SIGNAL(activated(const QString&)), SLOT(EnableOkButton()));

			m_statePortEdit = new QLineEdit();
			m_chatPortEdit = new QLineEdit();
			connect(m_statePortEdit, SIGNAL(textEdited(const QString&)), SLOT(EnableOkButton()));
			connect(m_chatPortEdit, SIGNAL(textEdited(const QString&)), SLOT(EnableOkButton()));

			QGridLayout* mainLayot = new QGridLayout();
			mainLayot->addWidget(labelAddres, 0, 0, 1, 2);
			mainLayot->addWidget(m_addressWidget, 0, 2, 1, 3);
			mainLayot->addWidget(labelStatePort, 1, 0, 1, 2);
			mainLayot->addWidget(m_statePortEdit, 1, 2, 1, 3);
			mainLayot->addWidget(labelChatPort, 2, 0, 1, 2);
			mainLayot->addWidget(m_chatPortEdit, 2, 2, 1, 3);
			mainLayot->addWidget(link, 3, 0, 1, 1);
			mainLayot->addWidget(buttonCancel, 3, 2, 1, 1);
			mainLayot->addWidget(m_buttonOK, 3, 4, 1, 1);
			setLayout(mainLayot);
		}

		void SettingsDialog::SaveSettings()
		{
			SettingsManager::Instance()->SetCurrentAddress(hlp::QStrToWStr(m_addressWidget->currentText()));
			SettingsManager::Instance()->SetCurrentStatesPort(m_statePortEdit->text().toULong());
			SettingsManager::Instance()->SetCurrentMessagesPort(m_chatPortEdit->text().toULong());
		}

		void SettingsDialog::RestoreDefaultSettings()
		{
			m_addressWidget->setCurrentText(m_appropriateAddress);
			m_statePortEdit->setText(QString::number(STATE_MSG_PORT));
			m_chatPortEdit->setText(QString::number(CHAT_MSG_PORT));
		}

		void SettingsDialog::InitAddressesValues()
		{ 
			m_appropriateAddress = hlp::WStrToQStr(net::AdaptersAddressManager::Instance()->GetAppropriateAddress());
			std::vector<std::wstring> addresses(net::AdaptersAddressManager::Instance()->GetLocalAddresses());
			std::for_each(addresses.cbegin(), addresses.cend(),
			[this](const std::wstring& addr)
			{
				m_addresses << hlp::WStrToQStr(addr);
			});
		}

		void SettingsDialog::EnableOkButton()
		{
			m_buttonOK->setEnabled(true);
		}
	}
}