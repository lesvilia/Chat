#include "stdafx.h"
#include "FileMessagesHandler.h"

#include <vector>

#include "ace/SOCK_Stream.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_IO.h"
#include "ace/FILE_Connector.h"
#include "ace/Message_Block.h"
#include "ace/Time_Value.h"
#include "FileAsyncReceiver.h"
#include "UIMessageHandler.h"
#include "IProgressUIObserver.h"
#include "LoginManager.h"
#include "MessagesTemplates.h"
#include "FileTransferHelpers.h"
#include "pugixml.hpp"
#include "StringHelpers.h"
#include "Typedefs.h"

namespace
{
  const size_t MTU_SIZE = 1400;
  const ACE_Time_Value TIMEOUT(2);

  void MessageDeleter(ACE_Message_Block* block)
  {
    block->release();
  }
}

namespace msg
{
  FileMessagesHandler::FileMessagesHandler(UIMessageHandler* msgHandler)
    : m_msgHandler(msgHandler)
  {
    assert(m_msgHandler != nullptr);
  }

  FileMessagesHandler::~FileMessagesHandler()
  {
  }

  void FileMessagesHandler::HandleConnect(const SocketStream& sockStream)
  {
    AsyncFileReceiver fileReceiver(sockStream, m_msgHandler);
  }

}
