
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
    m_thread.reset(new boost::thread());
  }

  void DataBaseService::Stop()
  {
    Lock lock;
    m_shouldShutdown = true;
  }

  void DataBaseService::PostRequest(const DBRequestHolder& holder)
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
        DBRequestHolder reqHolder(m_requestQueue.DequeueWait());
        if (reqHolder)
        {
          int result = sqlite3_exec(dbHolder.GetHandle(), reqHolder->SqlRequest().c_str(),
                                    reqHolder->Callback(), reqHolder.get(), nullptr);
          reqHolder->RequestFinished(result == SQLITE_OK);
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
