#pragma once
#include <QListWidgetItem>

namespace ui
{
  namespace controls
  {
    class UserListItem
      : public QListWidgetItem
    {
      
    public:
      UserListItem();
      UserListItem(const QIcon& icon, const QString& name, const std::wstring& uuid);
      virtual ~UserListItem();
      void EnableNotifyStyle();
      void EnableNormalStyle();
      std::wstring GetUserID() const;

    private:
      std::wstring m_uuid;
    };
  }
}
