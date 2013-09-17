#include "UserListItem.h"

namespace ui
{
  namespace 
  {
    const QColor NORMAL_COLOR(255, 255, 255);
    const QColor NOTIFY_COLOR(255, 255, 0);
  }

  namespace controls
  {
    UserListItem::UserListItem()
    {
    }

    UserListItem::UserListItem(const QIcon& icon, const QString& name, const std::wstring& uuid)
      : QListWidgetItem(icon, name)
      , m_uuid(uuid)
    {
      EnableNormalStyle();
    }

    UserListItem::~UserListItem()
    {
    }

    void UserListItem::EnableNotifyStyle()
    {
      setBackgroundColor(NOTIFY_COLOR);
    }

    void UserListItem::EnableNormalStyle()
    {
      setBackgroundColor(NORMAL_COLOR);
    }

    std::wstring UserListItem::GetUserID() const
    {
      return m_uuid;
    }
  }
}