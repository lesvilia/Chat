#pragma once
#include <string>

namespace ui
{
  class IProgressUIObserver
  {
  public:
    virtual void UpdateProgress(int count) = 0;
  protected:
    ~IProgressUIObserver() {}
  };

  class IFileTransferUIHandler
  {
  public:
    virtual IProgressUIObserver* AddFileMessage(const std::wstring& uuid,
                                                const std::wstring& fileName) = 0;
  protected:
    ~IFileTransferUIHandler() {}
  };
}
