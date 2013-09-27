#pragma once
#include <string>
#include "boost/noncopyable.hpp"
#include "INetUsersObserver.h"
#include "DataBaseUIHandler.h"

namespace db
{
  class DataBaseService;

  class DataBaseManager
    : private boost::noncopyable
    , private net::INetUsersObserver
  {
  public:
    static DataBaseManager* Instance();
    virtual void OnNetUserConnected(const std::wstring& uuid);
    virtual void OnNetUserDisconnected(const std::wstring& uuid);
    void SaveMessageToDB(const std::wstring& uuid, MessageType type, const ui::MessageInfo& msgInfo);
    void GetLastConversation(const std::wstring& uuid, DataBaseUIHandler* handler);
  private:
    DataBaseManager();
    ~DataBaseManager();
  private:
    std::unique_ptr<DataBaseService> m_service;
  };
}
