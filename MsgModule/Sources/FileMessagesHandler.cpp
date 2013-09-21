#include "stdafx.h"
#include "FileMessagesHandler.h"

#include "ace/SOCK_Stream.h"
#include "UIMessageHandler.h"
#include "IProgressUIObserver.h"
#include "Typedefs.h"

namespace msg
{
  FileMessagesHandler::FileMessagesHandler(UIMessageHandler* msgHandler)
    : m_msgHandler(msgHandler)
  {
  }

  FileMessagesHandler::~FileMessagesHandler()
  {
  }

  void FileMessagesHandler::HandleConnect(std::unique_ptr<ACE_SOCK_Stream>&& sockStream)
  {

  }
}