#include "stdafx.h"
#include "DataBaseService.h"

namespace db
{
  DataBaseService::DataBaseService()
    : m_shouldShutdown(false)
  {

  }
  DataBaseService::~DataBaseService()
  {

  }

  void DataBaseService::Start()
  {

  }

  void DataBaseService::Stop()
  {
    Lock lock;
    m_shouldShutdown = true;
  }

  void DataBaseService::PostRequest()
  {

  }

  bool DataBaseService::ShouldShutdown()
  {
    Lock lock;
    return m_shouldShutdown;
  }
}
