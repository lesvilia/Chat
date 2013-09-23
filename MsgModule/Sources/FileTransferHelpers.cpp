#include "stdafx.h"
#include "FileTransferHelpers.h"

#include "LoginManager.h"
#include "IProgressUIObserver.h"

namespace msg
{
  ObserverIniter::ObserverIniter()
    : m_observer(nullptr)
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

  ProgressUpdater::ProgressUpdater(ui::IProgressUIObserver* observer, size_t fileSize)
    : m_fileSize(fileSize)
    , m_transferedSize(0)
    , m_progressObserver(observer)
  {
  }

  void ProgressUpdater::Update(size_t size)
  {
    if (login::LoginManager::Instance()->IsOnline())
    {
      m_transferedSize += size;
      float result = ((float)m_transferedSize / m_fileSize) * 100;
      m_progressObserver->UpdateProgress(static_cast<int>(result));
    }
    else
    {
      throw LogOutError();
    }
  }

  void ProgressUpdater::Finished()
  {
    m_progressObserver->UpdateProgress(100);
    m_progressObserver->OnFinished();
  }

  void ProgressUpdater::Error()
  {
    m_progressObserver->OnError();
    throw TransferringError();
  }
}