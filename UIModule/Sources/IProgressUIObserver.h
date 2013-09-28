#pragma once

namespace ui
{
  class IProgressUIObserver
  {
  public:
    virtual void UpdateProgress(int count) = 0;
    virtual void OnFinished() = 0;
    virtual void OnError() = 0;
  protected:
    ~IProgressUIObserver() {}
  };
}
