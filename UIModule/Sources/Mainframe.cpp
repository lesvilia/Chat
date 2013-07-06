#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>
#include "Mainframe.h"
#include "NetUsersManager.h"
#include "LoginDialog.h"
#include "SettingsDialog.h"
#include "SettingsManager.h"
#include "LoginManager.h"
#include "ChatMessagesManager.h"
#include "StateMessagesManager.h"
#include "UISettings.h"
#include "QtHelpers.h"

namespace
{
	void PrepareMessage(QString& msg)
	{
		QString temp(msg);
		temp.remove("\n");
		if (temp.isEmpty())
		{
			msg.clear();
		}
	}
}

namespace ui
{
	using namespace settings::mainframe::sizes;
	using namespace settings::mainframe::strings;
	using namespace settings::mainframe::colors;

	MainFrame::UserItem::UserItem()
	{
	}

	MainFrame::UserItem::UserItem(controls::UserListItem* item, int id)
		: userlistItem(item)
		, msgWidgetID(id)
	{
	}

	MainFrame::MainFrame(QWidget *parent)
		: QMainWindow(parent)
		, m_userListWidget(nullptr)
		, m_msgBoxStackedWidget(nullptr)
		, m_stateLabel(nullptr)
	{
		SetupUI();
		login::LoginManager::Instance()->Subscribe(this);
		net::NetUsersManager::Instance()->Subscribe(this);
	}

	MainFrame::~MainFrame()
	{
	}

	void MainFrame::Show()
	{
		QMainWindow::show();
		LogIn();
	}

	void MainFrame::EnableLoginUI()
	{
		controls::LoginDialog dlg(this, login::LoginManager::Instance()->GetLoginHandler());
		dlg.exec();
	}

	void MainFrame::OnlineStateChanged()
	{
		if (login::LoginManager::Instance()->IsOnline())
		{
			login::UserDataPtr user(login::LoginManager::Instance()->GetCurrentUser());
			QString newUser(qthlp::WStrToQStr(user->name));
			m_currentUser = newUser;
			QString stateMessage(tr(STATE_LABEL_FORMAT).arg(qthlp::SetBoldStyle(m_currentUser), ONLINE_STATE));
			m_stateLabel->setText(stateMessage);
		}
		else
		{
			m_stateLabel->setText(OFLINE_STATE);
		}
	}

	void MainFrame::OnNetUserConnected(const std::wstring& uuid)
	{
		AddNewUser(uuid);
	}

	void MainFrame::OnNetUserDisconnected(const std::wstring& uuid)
	{
		RemoveUser(uuid);
	}

	void MainFrame::AddNewUser(const std::wstring& uuid)
	{
		controls::UserListItem* userItem = AddUserListItem(net::NetUsersManager::Instance()->GetNetUserName(uuid), uuid);
		int viewID = AddUserMsgView();
		m_userItems.insert(std::make_pair(uuid, UserItem(userItem, viewID)));
		m_userListWidget->setCurrentItem(userItem);
	}

	void MainFrame::RemoveUser(const std::wstring& uuid)
	{
		auto iter = m_userItems.find(uuid);
		if (iter != m_userItems.end())
		{
			std::unique_ptr<QListWidgetItem> listItem(m_userListWidget->takeItem(m_userListWidget->row(iter->second.userlistItem)));
			m_msgBoxStackedWidget->removeWidget(m_msgBoxStackedWidget->widget(iter->second.msgWidgetID));
		}
	}

	controls::UserListItem* MainFrame::AddUserListItem(const std::wstring& userName, const std::wstring& uuid)
	{
		controls::UserListItem* userItem = new controls::UserListItem(QIcon(USER_ICON_PATH), qthlp::WStrToQStr(userName), uuid);
		userItem->setSizeHint(QSize(25, 25));
		m_userListWidget->addItem(userItem);
		return userItem;
	}

	int MainFrame::AddUserMsgView()
	{
		QSplitter* msgViewWidget = new QSplitter(Qt::Vertical); 
		QTextEdit* msgView = new QTextEdit();
		QTextEdit* msgEdit = new QTextEdit();
		msgView->setReadOnly(true);

		msgViewWidget->addWidget(msgView);
		msgViewWidget->addWidget(msgEdit);

		QList<int> listSizes;
		listSizes << TOP_PART_HEIGH << BOTTOM_PART_HEIGH;
		msgViewWidget->setSizes(listSizes);

		connect(msgViewWidget, SIGNAL(splitterMoved(int, int)), SLOT(ResizeMessagesView()));
		return m_msgBoxStackedWidget->addWidget(msgViewWidget);
	}

	void MainFrame::AddNewMessage(const std::wstring& uuid, const std::wstring& message)
	{
		using namespace qthlp;
		
		auto iter = m_userItems.find(uuid);
		if (iter != m_userItems.end())
		{
			UserItem item(iter->second);
			
			controls::UserListItem* curentItem = static_cast<controls::UserListItem*>(m_userListWidget->currentItem());
			if (uuid != curentItem->GetUserID())
			{
				item.userlistItem->EnableNotifyStyle();
			}

			QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStackedWidget->widget(item.msgWidgetID));
			if (wdgSplitter)
			{
				QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
				QString userName(WStrToQStr(net::NetUsersManager::Instance()->GetNetUserName(uuid)));
				msgView->setTextColor(USERS_MSG_COLOR);
				AddMessageToView(userName, WStrToQStr(message), msgView);
			}
		}
	}

	void MainFrame::closeEvent(QCloseEvent* event)
	{
		if (login::LoginManager::Instance()->IsOnline())
		{
			LogOut();
		}
		event->accept();
	}

	void MainFrame::SendMessageToUser()
	{
		if (login::LoginManager::Instance()->IsOnline())
		{
			QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStackedWidget->currentWidget());
			if (wdgSplitter)
			{
				QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
				QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
				QString message(msgEdit->toPlainText());
				PrepareMessage(message);
				if (!message.isEmpty())
				{
					msgView->setTextColor(MY_MSG_COLOR);
					AddMessageToView(m_currentUser, message, msgView);
					controls::UserListItem* currentItem = static_cast<controls::UserListItem*>(m_userListWidget->currentItem());
					if (currentItem)
					{
						msg::ChatMessagesManager::Instance()->Send(currentItem->GetUserID(), qthlp::QStrToWStr(message));
					}
				}
				msgEdit->clear();
			}
		}
	}

	void MainFrame::AddMessageToView(const QString& userName, const QString& msg, QTextEdit* view)
	{
		view->setFontWeight(QFont::Bold);
		view->append(userName);
		view->setFontWeight(QFont::Normal);
		view->append(msg);
		view->append("");
	}

	void MainFrame::SetupUI()
	{
		QWidget* mainWidget = new QWidget();
		mainWidget->setObjectName(MAIN_WIDGET_NAME);
		mainWidget->setStyleSheet(MAIN_WIDGET_STYLE);
		
		CreateMenuBar();
		
		QHBoxLayout* mainLayout = CreateMainLayout(CreateUsersWidget(), CreateMessagesWidget());
		mainWidget->setLayout(mainLayout);
		
		setCentralWidget(mainWidget);
		setWindowTitle(MAIN_TITLE);
		setWindowIcon(QIcon(MAIN_ICON_PATH));
		
		resize(MAINFRAME_WIDTH, MAINFRAME_HEIGH);
	}

	void MainFrame::CreateMenuBar()
	{
		menuBar()->setStyleSheet(MAIN_MENU_BAR_STYLE);
		QMenu* menu = menuBar()->addMenu("Sign_In");
		menu->setStyleSheet(MENU_STYLE);
		menu->addAction("Sign_In", this, SLOT(LogIn()));
		menu->addAction("Sign_In_As", this, SLOT(LogInAs()));
		menu->addAction("Sign_Out", this, SLOT(LogOut()));
		menu->addAction("Exit", this, SLOT(close()));
		menuBar()->addAction("Settings", this, SLOT(OpenSettingsDlg()));
		menuBar()->addAction("About", this, SLOT(About()));
	}

	QWidget* MainFrame::CreateMessagesWidget()
	{
		QWidget* verticalWidget = new QWidget();
		QVBoxLayout* verticalLayout = new QVBoxLayout();
	
		QLabel* label = new QLabel(MESSAGES_LABEL_TEXT);
		label->setAlignment(Qt::AlignCenter);
		label->setMaximumHeight(20);
		label->setMinimumHeight(20);

		m_msgBoxStackedWidget = new QStackedWidget();

		QBoxLayout* msgBotomWidget = new QBoxLayout(QBoxLayout::RightToLeft);
		QPushButton* sendMsgButton = new QPushButton(BUTTON_TEXT);
		connect(sendMsgButton, SIGNAL(clicked(bool)), SLOT(SendMessageToUser()));
		sendMsgButton->setMinimumWidth(MIN_BUTTON_WIDTH);

		m_stateLabel = new QLabel();
		m_stateLabel->setText(OFLINE_STATE);
		m_stateLabel->setAlignment(Qt::AlignCenter);
		m_stateLabel->setStyleSheet(STATE_LABEL_STYLE);

		msgBotomWidget->addWidget(sendMsgButton);
		msgBotomWidget->addWidget(m_stateLabel, 1);

		verticalLayout->setMargin(0);
		verticalLayout->setSpacing(5);
		verticalLayout->addWidget(label);
		verticalLayout->addWidget(m_msgBoxStackedWidget);
		verticalLayout->addLayout(msgBotomWidget);
		verticalWidget->setLayout(verticalLayout);
	
		return verticalWidget;
	}

	QWidget* MainFrame::CreateUsersWidget()
	{
		QWidget* verticalWidget = new QWidget();
		QVBoxLayout* verticalLayout = new QVBoxLayout();
		m_userListWidget = new QListWidget();
		connect(m_userListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
			this, SLOT(ListItemChanged(QListWidgetItem*, QListWidgetItem*)));

		QLabel* label = new QLabel(USERS_LABEL_TEXT);
		label->setAlignment(Qt::AlignCenter);
		label->setMaximumHeight(20);
		label->setMinimumHeight(20);
	
		verticalLayout->addWidget(label);
		verticalLayout->addWidget(m_userListWidget);
		verticalLayout->setMargin(0);
		verticalLayout->setSpacing(5);

		verticalWidget->setLayout(verticalLayout);
		return verticalWidget;
	}

	QHBoxLayout* MainFrame::CreateMainLayout(QWidget* leftWidget, QWidget* rightWidget)
	{
		QHBoxLayout* horizontalLayout = new QHBoxLayout(); 
		QSplitter* vSpliter = new QSplitter(Qt::Horizontal);
		vSpliter->addWidget(leftWidget);
		vSpliter->addWidget(rightWidget);
	
		QList<int> listSizes;
		listSizes << LEFT_PART_WIDTH << RIGHT_PART_WIDTH;
		vSpliter->setSizes(listSizes);

		horizontalLayout->setMargin(5);
		horizontalLayout->setSpacing(5);
		horizontalLayout->addWidget(vSpliter);
		return horizontalLayout;
	}

	/*------------------Slots-------------*/

	void MainFrame::ListItemChanged(QListWidgetItem* currentItem, QListWidgetItem* prevItem)
	{
		controls::UserListItem* item = static_cast<controls::UserListItem*>(currentItem);
		if (item)
		{
			item->EnableNormalStyle();
			auto iter = m_userItems.find(item->GetUserID());
			if (iter != m_userItems.end())
			{
				m_msgBoxStackedWidget->setCurrentIndex(iter->second.msgWidgetID); 
			}
		}
	}

	void MainFrame::ResizeMessagesView()
	{
		QSplitter* currentWdgSplitter = static_cast<QSplitter*>(m_msgBoxStackedWidget->currentWidget());
		if (currentWdgSplitter)
		{
			QList<int> newSizesWdg(currentWdgSplitter->sizes());
			for(auto it = m_userItems.begin(); it != m_userItems.end(); ++it)
			{
				QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStackedWidget->widget(it->second.msgWidgetID));
				if (wdgSplitter != currentWdgSplitter)
				{
					wdgSplitter->setSizes(newSizesWdg);
				}
			}
		}
	}

	void MainFrame::LogIn()
	{
		if (!login::LoginManager::Instance()->IsOnline())
		{
			login::LoginManager::Instance()->LogIn(this);
		}
	}

	void MainFrame::LogInAs()
	{
		login::LoginManager::Instance()->LogOut();
		login::LoginManager::Instance()->LogIn(this);
	}

	void MainFrame::LogOut()
	{
		login::LoginManager::Instance()->LogOut();
	}

	void MainFrame::About()
	{
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(ABOUT_TITLE);
		messageBox.setText(ABOUT_MESSAGE);
		messageBox.setIconPixmap(QPixmap(MAIN_ICON_PATH).scaled(50, 50));
		messageBox.setMaximumHeight(150);
		messageBox.setMaximumWidth(300);
		messageBox.exec();
	}

	void MainFrame::OpenSettingsDlg()
	{
		controls::SettingsDialog dlg(this, sm::SettingsManager::Instance());
		if(dlg.exec() == QDialog::Accepted)
		{
			msg::ChatMessagesManager::Instance()->ResetServer();
			msg::StateMessagesManager::Instance()->ResetServer();
			LogInAs();
		}
	}
}