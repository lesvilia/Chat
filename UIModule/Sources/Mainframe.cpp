#include "Mainframe.h"
#include "LoginDialog.h"
#include "LoginManager.h"
#include "Settings.h"
#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QEvent>
#include <QMessageBox>

namespace
{
	QString	WStrToQStr(const std::wstring& str)
	{
		return (QString((const QChar*)str.c_str(), str.length()));
	}

	QString SetBoldStyle(const QString& str)
	{
		return QString("<font style='font-weight: bold;'>" + str + "</font>");
	}

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
	using namespace login;
	using namespace controls;
	using namespace settings::mainframe::sizes;
	using namespace settings::mainframe::strings;

	MainFrame::MainFrame(QWidget *parent)
		: QMainWindow(parent)
		, m_userListWidget(nullptr)
		, m_msgBoxStack(nullptr)
		, m_currentItem(nullptr)
		, m_stateLabel(nullptr)
	{
		SetupUI();

		AddNewUser(L"1", L"Sialiuk Ivan"); // for testing
		AddNewUser(L"2", L"Petrov Vasia");
		AddNewUser(L"3", L"Sidorov Petia");
		AddNewUser(L"4", L"Anuta Maluta");
		resize(MAINFRAME_WIDTH, MAINFRAME_HEIGH);
		LoginManager::Instance()->Subscribe(this);
	}

	MainFrame::~MainFrame()
	{
	}

	void MainFrame::show()
	{
		QMainWindow::show();
		LogIn();
	}

	void MainFrame::EnableLoginUI()
	{
		LoginDialog dlg(this, LoginManager::Instance()->GetLoginHandler());
		dlg.exec();
	}

	void MainFrame::OnlineStateChanged()
	{
		if (LoginManager::Instance()->IsOnline())
		{
			UserDataPtr user(LoginManager::Instance()->GetCurrentUser());
			QString newUser(WStrToQStr(user->name));
			if (m_currentUserName != newUser)
			{
				Reset();
				m_currentUserName = newUser;
				QString stateMessage(tr(STATE_LABEL_FORMAT).arg(SetBoldStyle(m_currentUserName), ONLINE_STATE));
				m_stateLabel->setText(stateMessage);
			}
		}
		else
		{
			Reset();
			m_currentUserName.clear();
			m_stateLabel->setText(OFLINE_STATE);
		}
	}

	void MainFrame::AddNewUser(const std::wstring& uuid, const std::wstring& name)
	{
		UserListItem* userItem = AddUserListItem(name, uuid);
		int viewID = AddUserMsgView();
		m_userItems[uuid] = std::make_pair(userItem, viewID);
	}

	void MainFrame::RemoveUser(const std::wstring& uuid)
	{
		auto iter = m_userItems.find(uuid);
		if (iter != m_userItems.end())
		{
			m_userListWidget->removeItemWidget(iter->second.first);
			m_msgBoxStack->removeWidget(m_msgBoxStack->widget(iter->second.second));
		}
	}

	void MainFrame::AddNewMessage(const std::wstring& uuid, const std::wstring& message)
	{
		auto iter = m_userItems.find(uuid);
		if (iter != m_userItems.end())
		{

		}
	}


	void MainFrame::SetupUI()
	{
		QWidget* mainWidget = new QWidget();
		mainWidget->setObjectName(MAIN_WIDGET_NAME);
		mainWidget->setStyleSheet(MAIN_WIDGET_STYLE);
		CreateMenuBar();
		QWidget* msgWidget = CreateMessagesWidget();
		QWidget* usersWidget = CreateUsersWidget();
		QHBoxLayout* mainLayout = CreateMainLayout(usersWidget, msgWidget);
		mainWidget->setLayout(mainLayout);
		this->setCentralWidget(mainWidget);

		setWindowTitle(MAIN_TITLE);
		setWindowIcon(QIcon(MAIN_ICON_PATH));
	}

	void MainFrame::CreateMenuBar()
	{
		menuBar()->setStyleSheet(MAIN_MENU_BAR_STYLE);
		QMenu* menu = menuBar()->addMenu("Sign_In");
		menu->setStyleSheet(MENU_STYLE);
		menu->addAction("Sign_In", this, SLOT(LogIn()));
		menu->addAction("Sign_Out", this, SLOT(LogOut()));
		menu->addAction("Exit", this, SLOT(close()));
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

		m_msgBoxStack = new QStackedWidget();

		QBoxLayout* msgBotomWidget = new QBoxLayout(QBoxLayout::RightToLeft);
		QPushButton* sendMsgButton = new QPushButton(BUTTON_TEXT);
		connect(sendMsgButton, SIGNAL(clicked(bool)), SLOT(SendMessage()));
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
		verticalLayout->addWidget(m_msgBoxStack);
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
		return m_msgBoxStack->addWidget(msgViewWidget);
	}

	UserListItem* MainFrame::AddUserListItem(const std::wstring& userName, const std::wstring& uuid)
	{
		UserListItem* userItem = new UserListItem(QIcon(USER_ICON_PATH), WStrToQStr(userName), uuid);
		userItem->setSizeHint(QSize(25, 25));
	
		m_userListWidget->addItem(userItem);
		return userItem;
	}

	void MainFrame::AddMessageToView(const QString& userName, const QString& msg, QTextEdit* view)
	{
		if (!msg.isEmpty())
		{
			view->append(SetBoldStyle(userName));
			view->append(msg);
			view->append("");
		}
	}

	void MainFrame::Reset()
	{
		std::for_each(m_userItems.begin(), m_userItems.end(),
		[this](const std::pair<std::wstring, UserItem>& item)
		{
			QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStack->widget(item.second.second));
			QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
			QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
			msgView->clear();
			msgEdit->clear();
		});
	}

	/*------------------Slots-------------*/

	void MainFrame::ListItemChanged(QListWidgetItem* currentItem, QListWidgetItem* prevItem)
	{
		UserListItem* item = static_cast<UserListItem*>(currentItem);
		auto iter = m_userItems.find(item->GetUserID());
		if (iter != m_userItems.end())
		{
			m_currentItem = item;
			m_msgBoxStack->setCurrentIndex(iter->second.second); 
		}
	}

	void MainFrame::ResizeMessagesView()
	{
		QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStack->currentWidget());
		if (wdgSplitter)
		{
			QList<int> newSizesWdg(wdgSplitter->sizes());
			for(auto it = m_userItems.begin(); it != m_userItems.end(); ++it)
			{
				if (it->first != m_currentItem->GetUserID())
				{
					QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStack->widget(it->second.second));
					wdgSplitter->setSizes(newSizesWdg);
				}
			}
		}
	}

	void MainFrame::SendMessage()
	{
		if (LoginManager::Instance()->IsOnline())
		{
			QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStack->currentWidget());
			if (wdgSplitter)
			{
				QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
				QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
				QString message(msgEdit->toPlainText());
				PrepareMessage(message);
				AddMessageToView(m_currentUserName, message, msgView);
				msgEdit->clear();
			}
		}
	}

	void MainFrame::LogIn()
	{
		LoginManager::Instance()->LogIn(this);
	}

	void MainFrame::LogOut()
	{
		LoginManager::Instance()->LogOut();
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
}