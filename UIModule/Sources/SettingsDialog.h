#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

namespace sm
{
  class SettingsManager;
}

namespace ui
{
  namespace controls
  {
    class SettingsDialog
      : public QDialog
    {
      Q_OBJECT

    public:
      SettingsDialog(QWidget* parent, sm::SettingsManager* settingsManager);
      ~SettingsDialog();

    private slots:
      void SaveSettings();
      void RestoreDefaultSettings();
      void EnableOkButton();

    private:
      void InitDialog();
      void InitAddressesValues();
      void SetupUI();

    private:
      sm::SettingsManager* m_settingsMngr;
      QStringList m_addresses;
      QString m_appropriateAddress;
      QComboBox* m_addressWidget;
      QLineEdit* m_statePortEdit;
      QLineEdit* m_chatPortEdit;
      QLineEdit* m_filePortEdit;
      QPushButton* m_buttonOK;
    };
  }
}