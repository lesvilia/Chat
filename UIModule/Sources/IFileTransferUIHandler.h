#pragma once
#include <string>
#include "boost/function.hpp"

namespace ui
{
  class IProgressUIObserver
  {
  public:
    virtual void UpdateProgress(int count) = 0;
    virtual void OnFinished() = 0;
  protected:
    ~IProgressUIObserver() {}
  };

  typedef boost::function<void (IProgressUIObserver* observer)> CompletionCallback;

  class IFileTransferUIHandler
  {
  public:
    virtual void OnFileMessageReceived(const std::wstring& uuid, const std::wstring& fileName,
                                       const CompletionCallback& callback) = 0;
  protected:
    ~IFileTransferUIHandler() {}
  };
}
