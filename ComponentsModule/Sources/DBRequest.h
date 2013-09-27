#pragma once
#include <memory>
#include <string>

namespace db
{
  typedef int (*ResponseCallback) (void*, int, char**, char**);

  class DBRequest
  {
  public:
    virtual void RequestFinished(bool succeeded) = 0;
    virtual std::string SqlRequest() const = 0;
    virtual ResponseCallback Callback() const = 0;
    virtual ~DBRequest() { }
  };

  typedef std::shared_ptr<DBRequest> DBRequestPtr;
}
