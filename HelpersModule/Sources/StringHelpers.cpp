
#include "StringHelpers.h"
#include <vector>
#include <Windows.h>

namespace strhlp
{
  std::wstring StrToWstr(const std::string& str)
  {
    int size = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::vector<wchar_t> buff(size + 1, '\0');
    ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &buff[0], size);
    return &buff[0];
  }

  std::string WstrToStr(const std::wstring& str)
  {
    int size = ::WideCharToMultiByte(CP_UTF8, 0, &str[0], -1, NULL, 0, NULL, NULL);
    std::vector<char> buff(size + 1, '\0');
    ::WideCharToMultiByte(CP_UTF8, 0, &str[0], -1, &buff[0], size, NULL, NULL);
    return &buff[0];
  }
}
