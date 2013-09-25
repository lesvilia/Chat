#pragma once
#include <memory>
#include <string>
#include <vector>

class ACE_Message_Block;

namespace msg
{
  struct FileInfo
  {
    FileInfo(size_t size, const std::wstring& name,
             const std::wstring& uuid, const std::wstring& path = std::wstring());
    size_t m_size;
    std::wstring m_name;
    std::wstring m_uuid;
    std::wstring m_path;
  };

  typedef std::shared_ptr<FileInfo> FileInfoPtr;

  typedef void (*MessageBlockDeleter)(ACE_Message_Block*);
  typedef std::unique_ptr<ACE_Message_Block> MessageBlockPtr;
  typedef std::unique_ptr<ACE_Message_Block, MessageBlockDeleter> MessageBlockHolder;
  ACE_Message_Block* LinksMessageBlocks(std::vector<MessageBlockPtr>& blocks);
}
