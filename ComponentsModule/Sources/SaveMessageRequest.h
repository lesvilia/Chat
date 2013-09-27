#pragma once
#include "DBRequest.h"
#include "DataBaseUIHandler.h"

namespace db
{
  class SaveMessageRequest
    : public DBRequest
  {
  public:
    SaveMessageRequest(const std::wstring& uuid, MessageType type, const ui::MessageInfo& msgInfo);
    virtual ~SaveMessageRequest();
    virtual void RequestFinished(bool succeeded);
    virtual std::string SqlRequest() const;
    virtual ResponseCallback Callback() const;
  private:
    static int Response(void*, int, char**, char**);
  private:
    std::wstring m_uuid;
    MessageType m_type;
    ui::MessageInfo m_msgInfo;
  };
}
