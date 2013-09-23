#pragma once
#include <string>
#include "boost/function.hpp"

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  typedef boost::function<void (ui::IProgressUIObserver* observer)> CompletionCallback;

  class UIMessageHandler
  {
  public:
    virtual void OnTextMessageReceived(const std::wstring& uuid, const std::wstring& message) = 0;
    virtual void OnFileMessageReceived(const std::wstring& uuid, const std::wstring& fileName,
                                       const CompletionCallback& callback) = 0; //thread safe operation
  protected:
    ~UIMessageHandler(){}
  };
}
