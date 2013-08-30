#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QTime>
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
    using namespace login;

    ILoginManager* manager = LoginManager::Instance();
		if (manager->IsOnline())
		{
			QString stateMessage(tr(STATE_LABEL_FORMAT).arg(SetBoldStyle(WStrToQStr(manager->GetCurrentUser()->name)),
                                                      ONLINE_STATE));
			m_stateLabel->setText(stateMessage);
		}
		else
		{
			m_stateLabel->setText(OFFLINE_STATE);
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
		int viewID = CreateUserMsgView();
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
			m_userItems.erase(iter);
		}
	}

	controls::UserListItem* MainFrame::AddUserListItem(const std::wstring& userName, const std::wstring& uuid)
	{
		controls::UserListItem* userItem = new controls::UserListItem(QIcon(USER_ICON_PATH), WStrToQStr(userName), uuid);
		userItem->setSizeHint(QSize(25, 25));
		m_userListWidget->addItem(userItem);
		return userItem;
	}

	int MainFrame::CreateUserMsgView()
	{
		QSplitter* msgViewWidget = new QSplitter(Qt::Vertical); 
		QTableWidget* msgView = new QTableWidget();
    msgView->setColumnCount(COLUMN_COUNT);
    
    QHeaderView* verticalHeader = msgView->verticalHeader();
    verticalHeader->setVisible(false);
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    QHeaderView* horizontalHeader = msgView->horizontalHeader();
    horizontalHeader->setVisible(false);
    horizontalHeader->setSectionResizeMode(NAME_COLUMN, QHeaderView::Fixed);
    horizontalHeader->setSectionResizeMode(MESSAGE_COLUMN, QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(TIME_COLUMN, QHeaderView::Fixed);
    horizontalHeader->setDefaultSectionSize(DEFAULT_COLUMN_WIDTH);
		QTextEdit* msgEdit = new QTextEdit();
		msgViewWidget->addWidget(msgView);
		msgViewWidget->addWidget(msgEdit);

    QList<int> listSizes;
    if (m_msgBoxStackedWidget->count() > 0) //if users are connected splitter size maybe changed
    {
      QSplitter* currentWdgSplitter = static_cast<QSplitter*>(m_msgBoxStackedWidget->currentWidget());
      listSizes.swap(currentWdgSplitter->sizes());
    }
    else
    {
      listSizes << TOP_PART_HEIGH << BOTTOM_PART_HEIGH;
    }
		msgViewWidget->setSizes(listSizes);

		connect(msgViewWidget, SIGNAL(splitterMoved(int, int)), SLOT(ResizeMessagesView()));
		return m_msgBoxStackedWidget->addWidget(msgViewWidget);
	}

	void MainFrame::AddNewMessage(const std::wstring& uuid, const std::wstring& message)
	{
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
				QTableWidget* msgView = static_cast<QTableWidget*>(wdgSplitter->widget(0));
        std::wstring name(net::NetUsersManager::Instance()->GetNetUserName(uuid));
				AddMessageToView(name, message, msgView, true);
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
    login::ILoginManager* loginManager = login::LoginManager::Instance();
		if (loginManager->IsOnline())
		{
			QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStackedWidget->currentWidget());
			if (wdgSplitter)
			{
				QTableWidget* msgView = static_cast<QTableWidget*>(wdgSplitter->widget(0));
				QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
				QString message(msgEdit->toPlainText());
				PrepareMessage(message);
				if (!message.isEmpty())
				{
					AddMessageToView(loginManager->GetCurrentUser()->name, message.toStdWString(), msgView, false);
					controls::UserListItem* currentItem = static_cast<controls::UserListItem*>(m_userListWidget->currentItem());
					if (currentItem)
					{
						msg::ChatMessagesManager::Instance()->Send(currentItem->GetUserID(), QStrToWStr(message));
					}
				}
        msgEdit->clear();
        msgEdit->document()->clear();
			}
		}
	}

	void MainFrame::AddMessageToView(const std::wstring& userName, const std::wstring& msg,
                                   QTableWidget* view, bool isNetUser)
	{
    const int rowNum = view->rowCount();
    view->insertRow(rowNum);
//name item
    QTableWidgetItem* nameItem = new QTableWidgetItem(WStrToQStr(userName));
    nameItem->setTextAlignment(Qt::AlignTop);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsSelectable);
    if (isNetUser)
      nameItem->setTextColor(Qt::blue);
    else
      nameItem->setTextColor(Qt::gray);
//time item
    QTableWidgetItem* timeItem = new QTableWidgetItem(QTime::currentTime().toString("hh:mm"));
    timeItem->setTextColor(Qt::gray);
    timeItem->setTextAlignment(Qt::AlignTop);
    timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsSelectable);
//message item
    QTableWidgetItem* messageItem = new QTableWidgetItem(WStrToQStr(msg));
    messageItem->setFlags(messageItem->flags() & ~Qt::ItemIsSelectable);

    view->setItem(rowNum, NAME_COLUMN, nameItem);
    view->setItem(rowNum, MESSAGE_COLUMN, messageItem);
    view->setItem(rowNum, TIME_COLUMN, timeItem);
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
		m_stateLabel->setText(OFFLINE_STATE);
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
		}
	}
}
