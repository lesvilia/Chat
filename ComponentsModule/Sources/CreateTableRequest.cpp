#include "CreateTableRequest.h"
#include "StringHelpers.h"
#include "boost/format.hpp"

namespace
{
  const char CREATE_TABLE_PATTERN[] = {"CREATE TABLE IF NOT EXISTS '%1%'"
                                       "(username TEXT, user INTEGER, message TEXT, time TEXT, type INTEGER);"};
}

namespace db
{
  int CreateTableRequest::Response(void*, int, char**, char**)
  {
    return 0;
  }

  CreateTableRequest::CreateTableRequest(const std::wstring& uuid)
    : m_uuid(uuid)
  {
  }

  CreateTableRequest::~CreateTableRequest()
  {
  }

  void CreateTableRequest::RequestFinished(bool succeeded)
  {
  }

  std::string CreateTableRequest::SqlRequest() const
  {
    std::string request(boost::str(boost::format(CREATE_TABLE_PATTERN) % strhlp::WstrToStr(m_uuid)));
    return boost::str(boost::format(CREATE_TABLE_PATTERN) % strhlp::WstrToStr(m_uuid));
  }

  ResponseCallback CreateTableRequest::Callback() const
  {
    return &CreateTableRequest::Response;
  }
}
