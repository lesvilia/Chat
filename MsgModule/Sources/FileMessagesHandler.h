#pragma once
#include "IMessagesHandler.h"

namespace msg
{
  class UIMessageHandler;

  class FileMessagesHandler
    : public IMessagesHandler
  {
  public:
    FileMessagesHandler(UIMessageHandler* msgHandler);
    virtual ~FileMessagesHandler();
    virtual void HandleConnect(std::unique_ptr<ACE_SOCK_Stream>&& sockStream);
  private:
    UIMessageHandler* m_msgHandler;
  };
}