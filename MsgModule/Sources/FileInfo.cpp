#include "stdafx.h"
#include "FileInfo.h"

#include <vector>
#include "ace/Message_Block.h"

namespace msg
{
  FileInfo::FileInfo(size_t size, const std::wstring& name,
                     const std::wstring& uuid, const std::wstring& path)
    : m_size(size)
    , m_name(name)
    , m_uuid(uuid)
    , m_path(path)
  {
  }

  ACE_Message_Block* LinksMessageBlocks(std::vector<MessageBlockPtr>& blocks)
  {
    ACE_Message_Block *head = nullptr, *message = nullptr;
    if (!blocks.empty())
    {
      head = (*blocks.begin()).release();
      message = head;
      for (auto it = blocks.begin() + 1; it != blocks.end(); ++it)
      {
        message->cont((*it).release());
        message = message->cont();
      }
    }
    return head;
  }
}
