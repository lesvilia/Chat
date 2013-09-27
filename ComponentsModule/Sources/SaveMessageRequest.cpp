#include "SaveMessageRequest.h"
#include "StringHelpers.h"
#include "boost/format.hpp"

namespace
{
  const char SAVE_MESSAGE_PATTERN[] = {"INSERT INTO '%1%'(username, user, message, time, type) "
    "VALUES('%2%', %3%, '%4%', '%5%', %6%);"};
}

namespace db
{
  int SaveMessageRequest::Response(void*, int, char**, char**)
  {
    return 0;
  }

  SaveMessageRequest::SaveMessageRequest(const std::wstring& uuid, MessageType type, const ui::MessageInfo& msgInfo)
    : m_uuid(uuid)
    , m_type(type)
    , m_msgInfo(msgInfo)
  {
  }

  SaveMessageRequest::~SaveMessageRequest()
  {
  }

  void SaveMessageRequest::RequestFinished(bool succeeded)
  {
  }

  std::string SaveMessageRequest::SqlRequest() const
  {
    using namespace strhlp;
    return boost::str(boost::format(SAVE_MESSAGE_PATTERN) % WstrToStr(m_uuid)
      % WstrToStr(m_msgInfo.m_username)
      % (m_msgInfo.m_isNetUser ? 1 : 0)
      % WstrToStr(m_msgInfo.m_message)
      % WstrToStr(m_msgInfo.m_time)
      % m_type);
  }

  ResponseCallback SaveMessageRequest::Callback() const
  {
    return &SaveMessageRequest::Response;
  }
}
