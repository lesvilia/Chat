#include "stdafx.h"
#include "DataBaseManager.h"

namespace db
{
  DataBaseManager* DataBaseManager::Instance()
  {
    static DataBaseManager manager;
    return &manager;
  }

  void DataBaseManager::OnNetUserConnected(const std::wstring& uuid)
  {

  }

  void DataBaseManager::OnNetUserDisconnected(const std::wstring& uuid)
  {

  }
}
