#pragma once
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/thread.hpp"

namespace msg
{
  typedef boost::mutex Mutex;
  typedef boost::condition_variable ConditionVariable;
  typedef boost::unique_lock<Mutex> Lock;
  typedef boost::thread Thread;
}