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
#include "UserListItem.h"

namespace ui
{
	class MainFrame 
		: public QMainWindow
		, public UIMessageHandler
		, public login::ILoginUIHandler
		, public login::ILoginStateObserver
	{
		Q_OBJECT

		typedef std::pair<controls::UserListItem*, int> UserItem;

	public:
		MainFrame(QWidget* parent = 0);
		~MainFrame();
		void show();
		virtual void EnableLoginUI();
		virtual void OnlineStateChanged();

		virtual void AddNewUser(const std::wstring& uuid, const std::wstring& name);
		virtual void RemoveUser(const std::wstring& uuid);
		virtual void AddNewMessage(const std::wstring& uuid, const std::wstring& message);

	private:
		void SetupUI();
		void CreateMenuBar();
		QWidget* CreateMessagesWidget();
		QWidget* CreateUsersWidget();
		QHBoxLayout* CreateMainLayout(QWidget* leftWidget, QWidget* rightWidget);
	
		int AddUserMsgView();
		controls::UserListItem* AddUserListItem(const std::wstring& userName, const std::wstring& uuid);
		void AddMessageToView(const QString& userName, const QString& msg, QTextEdit* view);
		void Reset();

	private slots:
		void ListItemChanged(QListWidgetItem* currentItem, QListWidgetItem* prevItem);
		void ResizeMessagesView();
		void SendMessage();
		void LogIn();
		void LogOut();
		void About();

	private:
		QListWidget*			m_userListWidget;
		QStackedWidget*			m_msgBoxStack;
		controls::UserListItem*	m_currentItem;
		QLabel*					m_stateLabel;
		QString					m_currentUserName;
		std::map<std::wstring, UserItem> m_userItems;
	};
}
