#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

namespace ui
{
	namespace controls
	{
		class SettingsDialog
			: public QDialog
		{
			Q_OBJECT

		public:
			SettingsDialog(QWidget* parent);
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
			QStringList m_addresses;
			QString m_appropriateAddress;
			QComboBox* m_addressWidget;
			QLineEdit* m_statePortEdit;
			QLineEdit* m_chatPortEdit;
			QPushButton* m_buttonOK;
		};
	}
}