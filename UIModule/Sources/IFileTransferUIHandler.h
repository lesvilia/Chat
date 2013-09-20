#pragma once
#include <string>

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
}
