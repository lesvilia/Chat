#pragma once
#include "DBRequest.h"
#include "DataBaseUIHandler.h"

namespace db
{
  class GetTableContentsRequest
    : public DBRequest
  {
  public:
    GetTableContentsRequest(const std::wstring& uuid, DataBaseUIHandler* handler);
    virtual ~GetTableContentsRequest();
    virtual void RequestFinished(bool succeeded);
    virtual std::string SqlRequest() const;
    virtual ResponseCallback Callback() const;
    void AddMessage(const std::pair<MessageType, ui::MessageInfo>& message);
  private:
    static int Response(void*, int, char**, char**);
  private:
    std::wstring m_uuid;
    DataBaseUIHandler* m_handler;
    MessageListPtr m_messages;
  };
}
