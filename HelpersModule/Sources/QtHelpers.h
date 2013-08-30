#pragma once
#include <string>
#include <QString>

QString SetLinkStyle(const QString& str);
QString WStrToQStr(const std::wstring& str);
std::wstring QStrToWStr(const QString& str);
QString SetBoldStyle(const QString& str);
QString SetFontColor(const QString& str, const QString& color);
