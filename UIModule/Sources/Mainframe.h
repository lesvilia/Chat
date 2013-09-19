#pragma once
#include <map>

#include <QMainWindow>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>

#include "UIMessageHandler.h"
#include "LoginHandlers.h"
#include "INetUsersObserver.h"
#include "IDropResultHandler.h"

class QTableWidget;

namespace ui
{
  namespace controls
  {
    class UserListItem;
  }

  class MainFrame 
    : public QMainWindow
    , public msg::UIMessageHandler
    , public login::ILoginUIHandler
    , public login::ILoginStateObserver
    , public net::INetUsersObserver
    , public IDropResultHandler
  {
    Q_OBJECT

    struct UserItem
    {
      UserItem();
      UserItem(controls::UserListItem* item, int id);
      controls::UserListItem* userlistItem;
      int msgWidgetID;
    };

  public:
    MainFrame(QWidget* parent = 0);
    ~MainFrame();
    void Show();

    //ILoginUIHandler interface
    virtual void EnableLoginUI();

    //ILoginStateObserver interface
    virtual void OnlineStateChanged();

    //INetUsersObserver interface
    virtual void OnNetUserConnected(const std::wstring& uuid);
    virtual void OnNetUserDisconnected(const std::wstring& uuid);

    //IDropResultHandler interface
    virtual void HandleDropFileResult(const std::wstring& path);

    //UIMessageHandler interface
    virtual void AddNewMessage(const std::wstring& uuid, const std::wstring& message);

  protected:
    void closeEvent(QCloseEvent* event);

  private:
    int CreateUserMsgView();
    QList<int> GetMsgViewSizes();
    controls::UserListItem* AddUserListItem(const std::wstring& userName, const std::wstring& uuid);
    void AddNewUser(const std::wstring& uuid);
    void RemoveUser(const std::wstring& uuid);
    void SetupUI();
    void CreateMenuBar();
    QWidget* CreateMessagesWidget();
    QWidget* CreateUsersWidget();
    QHBoxLayout* CreateMainLayout(QWidget* leftWidget, QWidget* rightWidget);
  
  private slots:
    void ListItemChanged(QListWidgetItem* currentItem, QListWidgetItem* prevItem);
    void ResizeMessagesView();
    void SendMessageToUser();
    void LogIn();
    void LogInAs();
    void LogOut();
    void About();
    void OpenSettingsDlg();

  private:
    QListWidget*        m_userListWidget;
    QStackedWidget*      m_msgBoxStackedWidget;
    QLabel*              m_stateLabel;
    std::map<std::wstring, UserItem> m_userItems;
  };
}
