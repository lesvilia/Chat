#include "DataBaseManager.h"

#include "DataBaseService.h"
#include "CreateTableRequest.h"
#include "GetTableContentsRequest.h"
#include "LoginManager.h"
#include "SaveMessageRequest.h"
#include "NetUsersManager.h"

namespace db
{
  DataBaseManager* DataBaseManager::Instance()
  {
    static DataBaseManager manager;
    return &manager;
  }

  DataBaseManager::DataBaseManager()
    : m_service(new DataBaseService())
  {
    net::NetUsersManager::Instance()->Subscribe(this);
    login::LoginManager::Instance()->Subscribe(this);
  }

  DataBaseManager::~DataBaseManager()
  {
  }

  void DataBaseManager::OnNetUserConnected(const std::wstring& uuid)
  {
    m_service->PostRequest(std::make_shared<CreateTableRequest>(uuid));
  }

  void DataBaseManager::OnNetUserDisconnected(const std::wstring& uuid)
  {
  }

  void DataBaseManager::OnlineStateChanged()
  {
    if (login::LoginManager::Instance()->IsOnline())
    {
      m_service->Start();
    }
    else
    {
      m_service->Stop();
    }
    
  }

  void DataBaseManager::SaveMessageToDB(const std::wstring& uuid, MessageType type, const ui::MessageInfo& msgInfo)
  {
    m_service->PostRequest(std::make_shared<SaveMessageRequest>(uuid, type, msgInfo));
  }

  void DataBaseManager::GetLastConversation(const std::wstring& uuid, DataBaseUIHandler* handler)
  {
    m_service->PostRequest(std::make_shared<GetTableContentsRequest>(uuid, handler));
  }
}
