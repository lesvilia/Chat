#pragma once
#include "LoginHandlers.h"

namespace login
{
  class ILoginManager
  {
  public:
    virtual void LogIn(ILoginUIHandler* uiHandler) = 0;
    virtual void LogOut() = 0;
    virtual UserDataPtr GetCurrentUser() const = 0;
    virtual bool IsOnline() const = 0;
    virtual void Subscribe(ILoginStateObserver* observer) = 0;
    virtual void Unsubscribe(ILoginStateObserver* observer) = 0;
    virtual ILoginHandler* GetLoginHandler() = 0;
  protected:
    ~ILoginManager() {}
  };
}
