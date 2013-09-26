#include "RegistryHelpers.h"
#include <vector>

namespace reghlp
{
  std::wstring GetStringValue(CRegKey& key, const std::wstring& valueName)
  {
    ULONG nChars = 0;
    if (ERROR_SUCCESS == key.QueryStringValue(valueName.c_str(), NULL, &nChars))
    {
      std::vector<wchar_t> buff(nChars, '\0');
      if (ERROR_SUCCESS == key.QueryStringValue(valueName.c_str(), &buff[0], &nChars))
      {
        return std::wstring(&buff[0]);
      }
    }
    return std::wstring();
  }

  DWORD GetDWORDValue(CRegKey& key, const std::wstring& valueName)
  {
    DWORD resultValue = 0;
    if (ERROR_SUCCESS == key.QueryDWORDValue(valueName.c_str(), resultValue))
    {
      return resultValue;
    }
    else
    {
      return 0;
    }
  }
}