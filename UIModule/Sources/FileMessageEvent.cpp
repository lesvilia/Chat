#include "FileMessageEvent.h"

namespace ui
{
  const QEvent::Type FileMessageEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

  FileMessageEvent::FileMessageEvent(const std::wstring& uuid, const std::wstring& fileName,
                                     const msg::CompletionCallback& callback)
  : QEvent(FileMessageEvent::type)
  , m_uuid(uuid)
  , m_fileName(fileName)
  , m_callback(callback)
  {
  }

  FileMessageEvent::~FileMessageEvent()
  {
  }
}
