#pragma once
#include <string>
#include<vector>

#include "IMessagesHandler.h"
#include "FileInfo.h"

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class UIMessageHandler;

  class FileMessagesHandler
    : public IMessagesHandler
  {
  public:
    FileMessagesHandler(UIMessageHandler* msgHandler);
    virtual ~FileMessagesHandler();
    virtual void HandleConnect(const SocketStream& sockStream);

  private:
    UIMessageHandler* m_msgHandler;
  };
}
