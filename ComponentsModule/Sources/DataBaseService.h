#pragma once
#include <string>
#include "boost/noncopyable.hpp"
#include "boost/thread.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/mutex.hpp"
#include "MessagesQueue.h"
#include "DBResponseHandler.h"


namespace db
{
  class DataBaseService
    : private boost::noncopyable
  {
    typedef boost::unique_lock<boost::mutex> Lock;
    typedef std::unique_ptr<boost::thread> ThreadPtr;
  public:
    DataBaseService();
    ~DataBaseService();
    void Start();
    void Stop();
    void PostRequest(const DBRequestHolder& holder);

  private:
    void ProcessRequest();
    bool ShouldShutdown();

  private:
    bool m_shouldShutdown;
    ThreadPtr m_thread;
    mutable boost::mutex m_mutex;
    msg::MessagesQueue<DBRequestHolder> m_requestQueue;
  };
}
