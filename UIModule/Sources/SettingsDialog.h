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
      bool NeedReset() const;

    private slots:
      void SaveSettings();
      void RestoreDefaultSettings();
      void SetNeedReset();
      void EnableOkButton();
      void ShowFolderOpenDlg();

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
      QLineEdit* m_saveDirEdit;
      QPushButton* m_buttonOK;
      bool m_needReset;
    };
  }
}
