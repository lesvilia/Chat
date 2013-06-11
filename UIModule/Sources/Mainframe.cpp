#include "Mainframe.h"
#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QEvent>
#include "LoginDialog.h"
#include "LoginManager.h"

namespace
{
	const QString MAIN_MENU_BAR_STYLE
		(
			"QMenuBar { min-height: 17px; background-color: #9ACD32; }"
			"QMenuBar::item { background-color: #9ACD32; }"
			"QMenuBar::item:selected { background: #ffd700; }"
			"QMenuBar::item:pressed { background: #ffd700; }"
		);

	const QString MENU_STYLE 
		(
			"QMenu { background-color: #9ACD32; }"
			"QMenu::item:selected {	background: #ffd700; color: black; }"
		);

	const QString MAIN_WIDGET_STYLE
		(
			"QWidget#main_widget { background: #CAFF70; }"
			"QSplitter::handle { background: #CAFF70; }"
		);
	
	const char ONLINE_STATE[] = "Online";
	const char OFLINE_STATE[] = "Ofline";

	const char BUTTON_TEXT[] = "Send Message";
	const unsigned MIN_BUTTON_WIDTH = 90;
	
	const char USERS_LABEL_TEXT[] = "<h4>Online Users</h4>";
	const char MESSAGES_LABEL_TEXT[] = "<h4>Posted Messages</h4>";

	const unsigned MAINFRAME_WIDTH = 800;
	const unsigned MAINFRAME_HEIGH = 600;

	const unsigned LEFT_PART_WIDTH = MAINFRAME_WIDTH / 3;
	const unsigned RIGHT_PART_WIDTH = MAINFRAME_WIDTH - LEFT_PART_WIDTH;

	const unsigned BOTTOM_PART_HEIGH = MAINFRAME_HEIGH / 3;
	const unsigned TOP_PART_HEIGH = MAINFRAME_HEIGH - BOTTOM_PART_HEIGH;
}

namespace ui
{
	MainFrame::MainFrame(QWidget *parent)
		: QMainWindow(parent)
		, m_userListWidget(nullptr)
		, m_msgBoxStack(nullptr)
		, m_currentItem(nullptr)
		, m_stateLabel(nullptr)
	{
		SetupUI();

		AddNewUser("Sialiuk Ivan"); // for testing
		AddNewUser("Petrov Vasia");
		AddNewUser("Sidorov Petia");
		AddNewUser("Anuta Maluta");
		resize(MAINFRAME_WIDTH, MAINFRAME_HEIGH);
		//connect(this, SIGNAL());
	}

	MainFrame::~MainFrame()
	{
	}

	void MainFrame::Login()
	{
		login::LoginManager::Instance()->Login(this);
	}

	void MainFrame::show()
	{
		QMainWindow::show();
		Login();
	}

	void MainFrame::OnlineStateChanged()
	{
		/*login::UserDataPtr user(login::LoginManager::Instance()->GetCurrentUser());
		if(user)
		{
			QString msgState("You name: ");
			msgState.append(user->name.c_str());
					"You name:"
					user->name.c_str()

				);
			if(login::LoginManager::Instance()->IsOnline())
			{
				m_stateLabel->setText(ONLINE_STATE);
			}
			else
			{
				m_stateLabel->setText(OFLINE_STATE);
			}
		}*/
	}

	void MainFrame::SetupUI()
	{
		QWidget* mainWidget = new QWidget();
		mainWidget->setObjectName("main_widget");
		mainWidget->setStyleSheet(MAIN_WIDGET_STYLE);
		CreateMenuBar();
		QWidget* msgWidget = CreateMessagesWidget();
		QWidget* usersWidget = CreateUsersWidget();
		QHBoxLayout* mainLayout = CreateMainLayout(usersWidget, msgWidget);
		mainWidget->setLayout(mainLayout);
		this->setCentralWidget(mainWidget);

		setWindowTitle("LChat");
		setWindowIcon(QIcon("chat_icon.png"));
	}

	void MainFrame::CreateMenuBar()
	{
		menuBar()->setStyleSheet(MAIN_MENU_BAR_STYLE);
		QMenu* menu = menuBar()->addMenu("Sign_In");
		menu->setStyleSheet(MENU_STYLE);
		menu->addAction("Sign_In");
		menu->addAction("Sign_Out");
		menu->addAction("Exit");
		menuBar()->addAction("About");
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

		msgBotomWidget->addWidget(sendMsgButton);
		msgBotomWidget->addStretch(1);
		msgBotomWidget->addWidget(m_stateLabel);

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
		QTextEdit* msgEdit = new QTextEdit("Create messages."); 
		msgView->setReadOnly(true);

		msgViewWidget->addWidget(msgView);
		msgViewWidget->addWidget(msgEdit);
	
		QList<int> listSizes;
		listSizes << TOP_PART_HEIGH << BOTTOM_PART_HEIGH;
		msgViewWidget->setSizes(listSizes);
	
		connect(msgViewWidget, SIGNAL(splitterMoved(int, int)), SLOT(ResizeMessagesView()));
		return m_msgBoxStack->addWidget(msgViewWidget);
	}

	QListWidgetItem* MainFrame::AddNewUser(const std::string& name)
	{
		QListWidgetItem* userItem = AddUserListItem(name);
		int viewID = AddUserMsgView();
		m_userListItems[userItem] = viewID;
		return userItem;
	}

	QListWidgetItem* MainFrame::AddUserListItem(const std::string& userName)
	{
		QListWidgetItem* userItem = new QListWidgetItem(QIcon("user_ico.png"),userName.c_str());
		userItem->setSizeHint(QSize(25, 25));
	
		m_userListWidget->addItem(userItem);
		return userItem;
	}

	QWidget* MainFrame::SearchMsgView(QListWidgetItem* item) const
	{
		auto iter = m_userListItems.find(item);
		if (iter != m_userListItems.end())
		{
			return m_msgBoxStack->widget(iter->second);
		}
		else
		{
			return nullptr;
		}
	}

	void MainFrame::AddMessageToView(const QString& msg, QTextEdit* view)
	{
		if (!msg.isEmpty())
		{
			QString userName("<font style='font-weight: bold;'>" + m_currentItem->text() + "</font>");
			view->append(userName);
			view->append(msg);
			view->append("");
		}
	}

	/*------------------Slots-------------*/

	void MainFrame::ListItemChanged(QListWidgetItem* currentItem, QListWidgetItem* prevItem)
	{
		auto iter = m_userListItems.find(currentItem);
		if (iter != m_userListItems.end())
		{
			m_currentItem = currentItem;
			m_msgBoxStack->setCurrentIndex(iter->second); 
		}
	}

	void MainFrame::ResizeMessagesView()
	{
		QSplitter* wdgSplitter = static_cast<QSplitter*>(SearchMsgView(m_currentItem));
		if (wdgSplitter)
		{
			QList<int> newSizesWdg(wdgSplitter->sizes());
			for(auto it = m_userListItems.begin(); it != m_userListItems.end(); ++it)
			{
				if (it->first != m_currentItem)
				{
					QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStack->widget(it->second));
					wdgSplitter->setSizes(newSizesWdg);
				}
			}
		}
	}

	void MainFrame::SendMessage()
	{
		QSplitter* wdgSplitter = static_cast<QSplitter*>(SearchMsgView(m_currentItem));
		if (wdgSplitter)
		{
			QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
			QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
			AddMessageToView(msgEdit->toPlainText(), msgView);
			msgEdit->setPlainText("");
		}
	}
}