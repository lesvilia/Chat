
#include "GetTableContentsDBHandler.h"
#include "StringHelpers.h"
#include "boost/format.hpp"

namespace
{
  const char GET_CONTENT_PATTERN[] = {"SELECT * FROM %1% ORDER BY time"};
}

namespace db
{
  GetTableContentsDBHandler::GetTableContentsDBHandler(const std::wstring& uuid, DataBaseUIHandler* handler)
    : m_uuid(uuid)
    , m_handler(handler)
  {
  }

  GetTableContentsDBHandler::~GetTableContentsDBHandler()
  {
  }

  void GetTableContentsDBHandler::RequestFinished(bool succeeded)
  {
    if (m_handler)
    {
      m_handler->AddLastConversations(m_messages);
    }
  }

  void GetTableContentsDBHandler::AddMessage(const std::pair<MessageType, ui::MessageInfo>& message)
  {
    m_messages->push_back(message);
  }

  std::string GetTableContentsDBHandler::SqlRequest() const
  {
    return boost::str(boost::format(GET_CONTENT_PATTERN) % strhlp::WstrToStr(m_uuid));
  }
  
  ResponseCallback GetTableContentsDBHandler::Callback() const
  {
    return &GetTableContentsDBHandler::Response;
  }

  int GetTableContentsDBHandler::Response(void* handler, int colCount, char** colContent, char** colName)
  {
    using namespace strhlp;
    GetTableContentsDBHandler* contentHandler = static_cast<GetTableContentsDBHandler*>(handler);
    if (contentHandler)
    {
      bool isNetUser = std::stoi(colContent[0]) ? true : false;
      ui::MessageInfo messageInfo(StrToWstr(colContent[0]), StrToWstr(colContent[2]),
        StrToWstr(colContent[3]), isNetUser);
      MessageType type = std::stoi(colContent[0]) ? FILE_MSG : TEXT_MSG;
      contentHandler->AddMessage(std::make_pair(type, messageInfo));
    }
    return 0;
  }
}
