#include "stdafx.h"
#include "FileTransferHelpers.h"

#include "LoginManager.h"
#include "NetUsersManager.h"
#include "IProgressUIObserver.h"

namespace msg
{

  Interrupter::Error::Error(const std::string& error)
    : m_error(error)
  {
  }

  const char* Interrupter::Error::what() const
  {
    return m_error.c_str();
  }

  Interrupter::Interrupter(const std::wstring& uuid)
    : m_uuid(uuid)
    , m_errorID(-1)
  {
    login::LoginManager::Instance()->Subscribe(this);
    net::NetUsersManager::Instance()->Subscribe(this);
  }

  Interrupter::~Interrupter()
  {
    login::LoginManager::Instance()->Unsubscribe(this);
    net::NetUsersManager::Instance()->Unsubscribe(this);
  }

  void Interrupter::Check()
  {
    LockGuard lock(m_mutex);
    switch (m_errorID)
    {
    case TRANSFER_ERROR:
      throw Error("Transfer error");
      break;

    case LOGOUT_ERROR:
      throw Error("Logout error");
      break;

    case DISCONNECT_ERROR:
      throw Error("Disconnect user error");
      break;
    }
  }

  void Interrupter::OnError(int errorID)
  {
    LockGuard lock(m_mutex);
    m_errorID = errorID;
  }

  void Interrupter::OnNetUserConnected(const std::wstring& uuid)
  {
  }

  void Interrupter::OnNetUserDisconnected(const std::wstring& uuid)
  {
    if (m_uuid == uuid)
    {
      OnError(DISCONNECT_ERROR);
    }
  }
  void Interrupter::OnlineStateChanged()
  {
    if (!login::LoginManager::Instance()->IsOnline())
    {
      OnError(LOGOUT_ERROR);
    }
  }

  ObserverIniter::ObserverIniter()
    : m_observer(nullptr)
  {
  }

  ObserverIniter::~ObserverIniter()
  {
  }

  void ObserverIniter::SetObserver(ui::IProgressUIObserver* observer)
  {
    {
      Lock lock(m_mutex);
      m_observer = observer;
    }
    m_cond.notify_one();
  }

  ui::IProgressUIObserver* ObserverIniter::GetObserver()
  {
    Lock lock(m_mutex);
    m_cond.timed_wait(lock, boost::posix_time::milliseconds(3000));
    return m_observer;
  }

  ProgressUpdater::ProgressUpdater(ui::IProgressUIObserver* observer, size_t fileSize,
                                   const std::wstring& uuid)
    : m_fileSize(fileSize)
    , m_transferedSize(0)
    , m_progressObserver(observer)
    , m_interrupter(uuid)
  {
    m_interrupter.Check();
  }

  ProgressUpdater::~ProgressUpdater()
  {
  }

  void ProgressUpdater::Update(size_t size)
  {
    m_interrupter.Check();
    if (m_progressObserver)
    {
      m_transferedSize += size;
      float result = ((float)m_transferedSize / m_fileSize) * 100;
      m_progressObserver->UpdateProgress(static_cast<int>(result));
    }
  }

  void ProgressUpdater::Finished()
  {
    m_interrupter.Check();
    if (m_progressObserver)
    {
      m_progressObserver->UpdateProgress(100);
      m_progressObserver->OnFinished();
    }
  }

  void ProgressUpdater::TransferError()
  {
    m_interrupter.OnError(Interrupter::TRANSFER_ERROR);
    m_interrupter.Check();
  }
}
