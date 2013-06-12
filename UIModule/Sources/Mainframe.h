#pragma once
#include <map>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include "LoginHandlers.h"

namespace ui
{
	class MainFrame 
		: public QMainWindow
		, public login::ILoginUIHandler
		, public login::ILoginStateObserver
	{
		Q_OBJECT

	public:
		MainFrame(QWidget* parent = 0);
		~MainFrame();
		void show();
		virtual void EnableLoginUI();
		virtual void OnlineStateChanged();

	private:
		void SetupUI();
		void CreateMenuBar();
		QWidget* CreateMessagesWidget();
		QWidget* CreateUsersWidget();
		QHBoxLayout* CreateMainLayout(QWidget* leftWidget, QWidget* rightWidget);
	
		QListWidgetItem* AddNewUser(const std::string& name);
		int AddUserMsgView();
		QListWidgetItem* AddUserListItem(const std::string& userName);
		QWidget* SearchMsgView(QListWidgetItem* item) const;
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
		QListWidget*		m_userListWidget;
		QStackedWidget*		m_msgBoxStack;
		QListWidgetItem*	m_currentItem;
		QLabel*				m_stateLabel;
		QString				m_currentUserName;
		std::map<QListWidgetItem*, int> m_userListItems;
	};
}
