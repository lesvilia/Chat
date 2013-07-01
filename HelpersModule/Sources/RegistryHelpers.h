#pragma once
#include <atlbase.h>
#include <string>

namespace reghlp
{
	std::wstring GetStringValue(CRegKey& key, const std::wstring& valueName);
	DWORD GetDWORDValue(CRegKey& key, const std::wstring& valueName);
}