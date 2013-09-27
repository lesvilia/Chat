
#include "GetTableContentsRequest.h"
#include "StringHelpers.h"
#include "boost/format.hpp"

namespace
{
  const char GET_CONTENT_PATTERN[] = {"SELECT * FROM '%1%' ORDER BY time;"};
}

namespace db
{
  GetTableContentsRequest::GetTableContentsRequest(const std::wstring& uuid, DataBaseUIHandler* handler)
    : m_uuid(uuid)
    , m_handler(handler)
    , m_messages(new MessageList())
  {
  }

  GetTableContentsRequest::~GetTableContentsRequest()
  {
  }

  void GetTableContentsRequest::RequestFinished(bool succeeded)
  {
    if (m_handler)
    {
      m_handler->AddLastConversations(m_messages);
    }
  }

  void GetTableContentsRequest::AddMessage(const std::pair<MessageType, ui::MessageInfo>& message)
  {
    m_messages->push_back(message);
  }

  std::string GetTableContentsRequest::SqlRequest() const
  {
    return boost::str(boost::format(GET_CONTENT_PATTERN) % strhlp::WstrToStr(m_uuid));
  }
  
  ResponseCallback GetTableContentsRequest::Callback() const
  {
    return &GetTableContentsRequest::Response;
  }

  int GetTableContentsRequest::Response(void* handler, int colCount, char** colContent, char** colName)
  {
    using namespace strhlp;
    GetTableContentsRequest* contentHandler = static_cast<GetTableContentsRequest*>(handler);
    if (contentHandler)
    {
      bool isNetUser = std::stoi(colContent[1]) ? true : false;
      ui::MessageInfo messageInfo(StrToWstr(colContent[0]), StrToWstr(colContent[2]),
        StrToWstr(colContent[3]), isNetUser);
      MessageType type = std::stoi(colContent[4]) ? FILE_MSG : TEXT_MSG;
      contentHandler->AddMessage(std::make_pair(type, messageInfo));
    }
    return 0;
  }
}
