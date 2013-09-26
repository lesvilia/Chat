#pragma once
#include <string>

namespace strhlp
{
  std::wstring StrToWstr(const std::string& str);
  std::string WstrToStr(const std::wstring& str);
}
