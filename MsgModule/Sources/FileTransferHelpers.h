#pragma once
#include "LoginHandlers.h"
#include "Typedefs.h"

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class TransferringError
    : public std::exception
  {
  };

  class LogOutError
    : public std::exception
  {
  };

  class ObserverIniter
  {
  public:
    ObserverIniter();
    void SetObserver(ui::IProgressUIObserver* observer);
    ui::IProgressUIObserver* GetObserver(); 

  private:
    ui::IProgressUIObserver* m_observer;
    Mutex m_mutex;
    ConditionVariable m_cond;
  };

  class ProgressUpdater
    : private login::ILoginStateObserver
  {
  public:
    ProgressUpdater(ui::IProgressUIObserver* observer, size_t fileSize);
    void Update(size_t size);
    void Finished();
    void Error();
    virtual void OnlineStateChanged();

  private:
    size_t m_fileSize;
    size_t m_transferedSize;
    ui::IProgressUIObserver* m_progressObserver;
  };
}