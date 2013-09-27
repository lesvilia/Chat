#pragma once
#include "DBRequest.h"


namespace db
{
  class CreateTableRequest
    : public DBRequest
  {
  public:
    CreateTableRequest(const std::wstring& m_uuid);
    virtual ~CreateTableRequest();
    virtual void RequestFinished(bool succeeded);
    virtual std::string SqlRequest() const;
    virtual ResponseCallback Callback() const;
  private:
    static int Response(void*, int, char**, char**);
  private:
    std::wstring m_uuid;
  };
}
