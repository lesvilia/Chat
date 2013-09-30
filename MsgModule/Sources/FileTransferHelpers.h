#pragma once
#include "string"
#include "LoginHandlers.h"
#include "INetUsersObserver.h"
#include "Typedefs.h"

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class Interrupter
    : private login::ILoginStateObserver
    , private net::INetUsersObserver
  {
  public:
    enum
    {
      TRANSFER_ERROR,
      LOGOUT_ERROR,
      DISCONNECT_ERROR
    };

    class Error 
      : public std::exception
    {
    public:
      Error(const std::string& error);
      virtual const char* what() const;
    private:
      std::string m_error;
    };

    explicit Interrupter(const std::wstring& uuid);
    ~Interrupter();
    void Check();
    void OnError(int errorID);
  private:
    virtual void OnNetUserConnected(const std::wstring& uuid);
    virtual void OnNetUserDisconnected(const std::wstring& uuid);
    virtual void OnlineStateChanged();
  private:
    std::wstring m_uuid;
    int m_errorID;
    Mutex m_mutex;
  };

  class ObserverIniter
  {
  public:
    ObserverIniter();
    ~ObserverIniter();
    void SetObserver(ui::IProgressUIObserver* observer);
    ui::IProgressUIObserver* GetObserver(); 

  private:
    ui::IProgressUIObserver* m_observer;
    Mutex m_mutex;
    ConditionVariable m_cond;
  };

  class ProgressUpdater
  {
  public:
    ProgressUpdater(ui::IProgressUIObserver* observer, size_t fileSize, const std::wstring& uuid);
    ~ProgressUpdater();
    void Update(size_t size, bool isHeader = false);
    void Finished();
    void TransferError();
  private:
    size_t m_fileSize;
    size_t m_transferedSize;
    ui::IProgressUIObserver* m_progressObserver;
    Interrupter m_interrupter;
  };
}
