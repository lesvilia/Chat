#pragma once
#include <string>
#include <QString>

namespace hlp
{
	QString SetLinkStyle(const QString& str);
	QString WStrToQStr(const std::wstring& str);
	std::wstring QStrToWStr(const QString& str);
	QString SetBoldStyle(const QString& str);
}