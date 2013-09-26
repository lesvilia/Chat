#pragma once
#include <string>
#include "boost/noncopyable.hpp"
#include "INetUsersObserver.h"

namespace db
{
  class DataBaseService;

  class DataBaseManager
    : private boost::noncopyable
    , public net::INetUsersObserver
  {
  public:
    static DataBaseManager* Instance();
    virtual void OnNetUserConnected(const std::wstring& uuid);
    virtual void OnNetUserDisconnected(const std::wstring& uuid);
  private:
    DataBaseManager();
    ~DataBaseManager();
  };
}
