#include "QtHelpers.h"
#include <QObject>

namespace hlp
{
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
		return QString("<font style='font-weight: bold;'>" + str + "</font>");
	}
}