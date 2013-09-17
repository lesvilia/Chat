#include "SettingsDialog.h"
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <vector>
#include <algorithm>
#include "StaticLink.h"
#include "UISettings.h"
#include "QtHelpers.h"
#include "SettingsManager.h"
#include "DefaultSettings.h"

namespace ui
{
  namespace controls
  {
    using namespace settings::settingsdialog;

    SettingsDialog::SettingsDialog(QWidget* parent, sm::SettingsManager* settingsManager)
      : QDialog(parent)
      , m_settingsMngr(settingsManager)
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
      link->setText(SetLinkStyle("Restore default settings"));
      connect(link, SIGNAL(clicked()), SLOT(RestoreDefaultSettings()));

      m_addressWidget = new QComboBox();
      m_addressWidget->setEditable(false);
      m_addressWidget->insertItems(0, m_addresses);
      m_addressWidget->setCurrentText(m_appropriateAddress);
      connect(m_addressWidget, SIGNAL(activated(const QString&)), SLOT(EnableOkButton()));

      m_statePortEdit = new QLineEdit();
      m_statePortEdit->setText(QString::number(m_settingsMngr->GetCurrentStatesPort()));
      m_chatPortEdit = new QLineEdit();
      m_chatPortEdit->setText(QString::number(m_settingsMngr->GetCurrentMessagesPort()));
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
      m_settingsMngr->SetCurrentAddress(QStrToWStr(m_addressWidget->currentText()));
      m_settingsMngr->SetCurrentStatesPort(m_statePortEdit->text().toULong());
      m_settingsMngr->SetCurrentMessagesPort(m_chatPortEdit->text().toULong());
      accept();
    }

    void SettingsDialog::RestoreDefaultSettings()
    {
      m_addressWidget->setCurrentText(m_appropriateAddress);
      m_statePortEdit->setText(QString::number(sm::DEFAULT_STATE_MSG_PORT));
      m_chatPortEdit->setText(QString::number(sm::DEFAULT_CHAT_MSG_PORT));
    }

    void SettingsDialog::InitAddressesValues()
    { 
      m_appropriateAddress = WStrToQStr(m_settingsMngr->GetCurrentNetAddres());
      std::vector<std::wstring> addresses(m_settingsMngr->GetActiveAddresses());
      std::for_each(addresses.cbegin(), addresses.cend(),
      [this](const std::wstring& addr)
      {
        m_addresses << WStrToQStr(addr);
      });
    }

    void SettingsDialog::EnableOkButton()
    {
      if (!m_statePortEdit->text().isEmpty() && !m_chatPortEdit->text().isEmpty())
      {
        m_buttonOK->setEnabled(true);
      }
      else
      {
        m_buttonOK->setEnabled(false);
      }
    }
  }
}
