#include "stdafx.h"
#include "QtHelpers.h"
#include <QObject>

QString SetLinkStyle(const QString& str)
{
  const char LINK_FORMAT[] = "<a href=\" \">%1</a>";
  return QString(QObject::tr(LINK_FORMAT).arg(str));
}

QString WStrToQStr(const std::wstring& str)
{
  return (QString((const QChar*)str.c_str(), str.length()));
}

std::wstring QStrToWStr(const QString& str)
{
  return std::wstring((wchar_t*)str.unicode(), str.length());
}

QString SetBoldStyle(const QString& str)
{
  return QString("<font style='font-weight: bold;'>%1</font>").arg(str);
}

QString SetFontColor(const QString& str, const QString& color)
{
  return QString("<font style='color: %1;'>%2</font>").arg(color, str);
}
