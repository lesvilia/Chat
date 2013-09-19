#pragma once

#include <string>

class IDropResultHandler
{
public:
  virtual void HandleDropFileResult(const std::wstring& path) = 0;
protected:
  virtual ~IDropResultHandler() {}
};
