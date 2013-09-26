#pragma once
#include "DBResponseHandler.h"

namespace db
{
  class CreateTableDBHandler
    : public DBResponseHandler
  {
  public:
    CreateTableDBHandler(const std::wstring& uuid);
    virtual ~CreateTableDBHandler();
    virtual void RequestFinished(bool succeeded);
    virtual std::string SqlRequest() const;
    virtual ResponseCallback Callback() const;
  private:
    static int Response(void*, int, char**, char**);
  private:
    std::wstring m_uuid;
  };
}
