#pragma once
#include <memory>
#include <string>

namespace db
{
  typedef int (*ResponseCallback) (void*, int, char**, char**);

  class DBResponseHandler
  {
  public:
    virtual void RequestFinished(bool succeeded) = 0;
    virtual std::string SqlRequest() const = 0;
    virtual ResponseCallback Callback() const = 0;
    virtual ~DBResponseHandler() { }
  };

  typedef std::shared_ptr<DBResponseHandler> DBRequestHolder;
}
