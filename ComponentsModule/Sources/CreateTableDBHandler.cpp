#include "CreateTableDBHandler.h"
#include "StringHelpers.h"
#include "boost/format.hpp"

namespace
{
  const char CREATE_TABLE_PATTERN[] = {"CREATE TABLE IF NOT EXISTS %1% "
                                       "(username TEXT, user INTEGER, message TEXT, time TEXT, type INTEGER)"};
}

namespace db
{
  int CreateTableDBHandler::Response(void*, int, char**, char**)
  {
    return 0;
  }

  CreateTableDBHandler::CreateTableDBHandler(const std::wstring& uuid)
    : m_uuid(uuid)
  {
  }

  CreateTableDBHandler::~CreateTableDBHandler()
  {
  }

  void CreateTableDBHandler::RequestFinished(bool succeeded)
  {
  }

  std::string CreateTableDBHandler::SqlRequest() const
  {
    return boost::str(boost::format(CREATE_TABLE_PATTERN) % strhlp::WstrToStr(m_uuid));
  }

  ResponseCallback CreateTableDBHandler::Callback() const
  {
    return &CreateTableDBHandler::Response;
  }
}
