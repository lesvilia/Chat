#pragma once
#include <QEvent>
#include "UIMessageHandler.h"

namespace ui
{
  class FileMessageEvent
    : public QEvent
  {
  public:
    FileMessageEvent(const std::wstring& uuid, const std::wstring& fileName,
                     const msg::CompletionCallback& callback);
    virtual ~FileMessageEvent();
    const std::wstring m_uuid;
    const std::wstring m_fileName;
    const msg::CompletionCallback m_callback;
    static const QEvent::Type type;
  };
}
