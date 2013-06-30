#include "UserListItem.h"

namespace ui
{
	namespace controls
	{
		UserListItem::UserListItem()
		{
		}

		UserListItem::UserListItem(const QIcon& icon, const QString& name, const std::wstring& uuid)
			: QListWidgetItem(icon, name)
			, m_uuid(uuid)
		{
		}

		UserListItem::~UserListItem()
		{
		}

		std::wstring UserListItem::GetUserID() const
		{
			return m_uuid;
		}
	}
}