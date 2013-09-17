#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QStackedLayout>
#include "LoginHandlers.h"

namespace ui
{
  namespace controls
  {
    class LoginDialog
      : public QDialog
    {
      Q_OBJECT
    public:
      LoginDialog(QWidget* parent, login::ILoginHandler* handler);
      ~LoginDialog();

    private:
      void InitDialog();
      void SetupUI();
      void HandleLoginError(const login::UserDataPtr& data);
      void HandleRegistrationError(const login::UserDataPtr& data);
      void SetUILogin();
      void SetUIRegistration();
      void InitUsersList();

    private slots:
      void DoLogin();
      void DoRegistration();
      void SwitchToRegistrationMode();
      void SwitchToLoginMode();

    private:
      login::ILoginHandler* m_handler;
      QStackedLayout* m_mainLayout;
      
      QWidget* m_loginWidget;
      QComboBox* m_logNameEdit;
      QLineEdit* m_logPassEdit;
      
      QWidget* m_registrationWidget;
      QLineEdit* m_regNameEdit;
      QLineEdit* m_regPassEdit;
      QStringList m_usersList;
    };
  }
}
