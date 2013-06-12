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
}

namespace ui
{
	using namespace login;
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

		AddNewUser("Sialiuk Ivan"); // for testing
		AddNewUser("Petrov Vasia");
		AddNewUser("Sidorov Petia");
		AddNewUser("Anuta Maluta");
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
		controls::LoginDialog dlg(this, LoginManager::Instance()->GetLoginHandler());
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

	QListWidgetItem* MainFrame::AddNewUser(const std::string& name)
	{
		QListWidgetItem* userItem = AddUserListItem(name);
		int viewID = AddUserMsgView();
		m_userListItems[userItem] = viewID;
		return userItem;
	}

	QListWidgetItem* MainFrame::AddUserListItem(const std::string& userName)
	{
		QListWidgetItem* userItem = new QListWidgetItem(QIcon(USER_ICON_PATH), userName.c_str());
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

	void MainFrame::AddMessageToView(const QString& userName, const QString& msg, QTextEdit* view)
	{
		if (!msg.isEmpty())
		{
			QString msg(tr(MSG_FORMAT).arg(SetBoldStyle(userName), msg));
			view->append(msg);
		}
	}

	void MainFrame::Reset()
	{
		std::for_each(m_userListItems.begin(), m_userListItems.end(),
		[this](const std::pair<QListWidgetItem*, int>& item)
		{
			QSplitter* wdgSplitter = static_cast<QSplitter*>(m_msgBoxStack->widget(item.second));
			QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
			QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
			msgView->clear();
			msgEdit->clear();
		});
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
		if (LoginManager::Instance()->IsOnline())
		{
			QSplitter* wdgSplitter = static_cast<QSplitter*>(SearchMsgView(m_currentItem));
			if (wdgSplitter)
			{
				QTextEdit* msgView = static_cast<QTextEdit*>(wdgSplitter->widget(0));
				QTextEdit* msgEdit = static_cast<QTextEdit*>(wdgSplitter->widget(1));
				AddMessageToView(m_currentUserName, msgEdit->toPlainText(), msgView);
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