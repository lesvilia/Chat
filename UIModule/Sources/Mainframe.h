#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <map>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QPushButton>
#include "LoginManager.h"

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget* parent = 0);
	~MainFrame();

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
	
	login::LoginManager m_loginmanager;
};

#endif // MAINFRAME_H
