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
#include "UserListItem.h"

namespace ui
{
	class MainFrame 
		: public QMainWindow
		, public msg::UIMessageHandler
		, public login::ILoginUIHandler
		, public login::ILoginStateObserver
		, public net::INetUsersObserver
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

		//UIMessageHandler interface
		virtual void AddNewMessage(const std::wstring& uuid, const std::wstring& message);

	protected:
		void closeEvent(QCloseEvent* event);

	private:
		int AddUserMsgView();
		controls::UserListItem* AddUserListItem(const std::wstring& userName, const std::wstring& uuid);
		void AddMessageToView(const QString& userName, const QString& msg, QTextEdit* view);
		void AddNewUser(const std::wstring& uuid);
		void RemoveUser(const std::wstring& uuid);
		void ClearMsgWidgets();
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
		void LogOut();
		void About();
		void OpenSettingsDlg();

	private:
		QListWidget*			m_userListWidget;
		QStackedWidget*			m_msgBoxStackedWidget;
		controls::UserListItem*	m_currentItem;
		QLabel*					m_stateLabel;
		QString					m_currentUser;
		std::map<std::wstring, UserItem> m_userItems;
	};
}
