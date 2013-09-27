
#include "DataBaseService.h"
#include "DataBaseHolder.h"
#include "SQLite/sqlite3.h"

namespace 
{
  const char DB_FILE_NAME[] = {"chat.db"};
}

namespace db
{
  DataBaseService::DataBaseService()
    : m_shouldShutdown(false)
  {
  }

  DataBaseService::~DataBaseService()
  {
    if (m_thread->joinable())
    {
      Stop();
      m_thread->join();
    }
  }

  void DataBaseService::Start()
  {
    m_thread.reset(new boost::thread(&DataBaseService::ProcessRequest, this));
  }

  void DataBaseService::Stop()
  {
    Lock lock;
    m_shouldShutdown = true;
  }

  void DataBaseService::PostRequest(const DBRequestPtr& holder)
  {
    m_requestQueue.Enqueue(holder);
  }

  void DataBaseService::ProcessRequest()
  {
    DataBaseHolder dbHolder(DB_FILE_NAME);
    if (dbHolder.Open())
    {
      while (!ShouldShutdown())
      {
        DBRequestPtr reqHolder(m_requestQueue.DequeueWait(boost::posix_time::milliseconds(500)));
        if (reqHolder)
        {
          char* errMsg = nullptr;
          int result = sqlite3_exec(dbHolder.GetHandle(), reqHolder->SqlRequest().c_str(),
                                    reqHolder->Callback(), reqHolder.get(), &errMsg);
          reqHolder->RequestFinished(result == SQLITE_OK);
          sqlite3_free(errMsg);
        }
      }
    }
  }

  bool DataBaseService::ShouldShutdown()
  {
    Lock lock;
    return m_shouldShutdown;
  }
}
