#pragma once
#include <map>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QStackedWidget>
#include <QPushButton>
#include "LoginHandlers.h"

namespace ui
{
	class MainFrame 
		: public QMainWindow
		, public login::ILoginUIHandler
	{
		Q_OBJECT

	public:
		MainFrame(QWidget* parent = 0);
		~MainFrame();
		virtual void ShowRegistrationDlg();
		virtual void ShowLoginDlg();

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
		void AddMessageToView(const QString& msg, QTextEdit* view);

	private slots:
		void ListItemChanged(QListWidgetItem* currentItem, QListWidgetItem* prevItem);
		void ResizeMessagesView();
		void SendMessage();

	private:
		QListWidget*		m_userListWidget;
		QStackedWidget*		m_msgBoxStack;
		QListWidgetItem*	m_currentItem;
		std::map<QListWidgetItem*, int> m_userListItems;
	};
}
