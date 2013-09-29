#pragma once
#include <string>
#include "boost/noncopyable.hpp"
#include "INetUsersObserver.h"
#include "LoginHandlers.h"
#include "DataBaseUIHandler.h"

namespace db
{
  class DataBaseService;

  class DataBaseManager
    : private boost::noncopyable
    , public net::INetUsersObserver
    , public login::ILoginStateObserver
  {
  public:
    static DataBaseManager* Instance();
    void SaveMessageToDB(const std::wstring& uuid, MessageType type, const ui::MessageInfo& msgInfo);
    void GetLastConversation(const std::wstring& uuid, DataBaseUIHandler* handler);

  private:
    DataBaseManager();
    ~DataBaseManager();
    virtual void OnNetUserConnected(const std::wstring& uuid);
    virtual void OnNetUserDisconnected(const std::wstring& uuid);
    virtual void OnlineStateChanged();

  private:
    std::unique_ptr<DataBaseService> m_service;
  };
}
