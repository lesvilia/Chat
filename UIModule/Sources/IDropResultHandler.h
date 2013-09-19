#pragma once

#include <string>

namespace ui
{
  class IDropResultHandler
  {
  public:
    virtual void HandleDropFileResult(const std::wstring& path) = 0;
  protected:
    ~IDropResultHandler() {}
  };
}
