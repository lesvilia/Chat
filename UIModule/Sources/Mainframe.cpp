#include "Mainframe.h"

#include <QSplitter>
#include <QLabel>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QTime>

#include "UsersMessageView.h"
#include "NetUsersManager.h"
#include "LoginDialog.h"
#include "SettingsDialog.h"
#include "SettingsManager.h"
#include "LoginManager.h"
#include "FileTransferManager.h"
#include "ChatMessagesManager.h"
#include "StateMessagesManager.h"
#include "UserListItem.h"
#include "UISettings.h"
#include "QtHelpers.h"

namespace
{
  std::wstring CurrentTimeToStr(const char* format)
  {
    return QTime::currentTime().toString(format).toStdWString();
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
    connect(this, SIGNAL(FileMessageReceived(const std::wstring&, const std::wstring&, const msg::CompletionCallback&)),
            this, SLOT(AddNewFileMessage(const std::wstring&, const std::wstring&, const msg::CompletionCallback&)));
    AddNewUser(L"123");
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

  void MainFrame::HandleDropFileResult(const std::wstring& path)
  {
    login::ILoginManager* loginManager = login::LoginManager::Instance();
    if (loginManager->IsOnline())
    {
      QFileInfo file(WStrToQStr(path));
      MessageInfo msg(loginManager->GetCurrentUser()->name, file.fileName().toStdWString(),
                      CurrentTimeToStr("hh:mm"), false);
      UsersMessageView* msgView = static_cast<UsersMessageView*>(
        m_msgBoxStackedWidget->currentWidget());
      IProgressUIObserver* observer = msgView->AppendFileMessage(msg);

      controls::UserListItem* currentItem = static_cast<controls::UserListItem*>(
        m_userListWidget->currentItem());

      msg::FileTransferManager::Instance()->SendFile(currentItem->GetUserID(), path, observer);
    }
  }

  void MainFrame::AddNewUser(const std::wstring& uuid)
  {
    std::wstring userName(/*net::NetUsersManager::Instance()->GetNetUserName(uuid)*/L"Elena Sialiuk");
    controls::UserListItem* userItem = AddUserListItem(userName, uuid);
    int viewID = CreateUserMsgView();
    m_userItems.insert(std::make_pair(uuid, UserItem(userItem, viewID)));
    m_userListWidget->setCurrentItem(userItem);
  }

  void MainFrame::RemoveUser(const std::wstring& uuid)
  {
    auto iter = m_userItems.find(uuid);
    if (iter != m_userItems.end())
    {
      std::unique_ptr<QListWidgetItem> listItem(m_userListWidget->takeItem (
         m_userListWidget->row(iter->second.userlistItem)));
      m_msgBoxStackedWidget->removeWidget(m_msgBoxStackedWidget->widget(iter->second.msgWidgetID));
      m_userItems.erase(iter);
    }
  }

  controls::UserListItem* MainFrame::AddUserListItem(const std::wstring& userName, const std::wstring& uuid)
  {
    controls::UserListItem* userItem = new controls::UserListItem(QIcon(USER_ICON_PATH),
                                                                  WStrToQStr(userName), uuid);
    userItem->setSizeHint(QSize(25, 25));
    m_userListWidget->addItem(userItem);
    return userItem;
  }

  QList<int> MainFrame::GetMsgViewSizes()
  {
    QList<int> listSizes;
    if (m_msgBoxStackedWidget->count() > 0) //if users are connected splitter size maybe changed
    {
      UsersMessageView* currentMsgView = static_cast<UsersMessageView*>(
        m_msgBoxStackedWidget->currentWidget());
      listSizes.swap(currentMsgView->sizes());
    }
    else
    {
      listSizes << TOP_PART_HEIGH << BOTTOM_PART_HEIGH;
    }
    return listSizes;
  }

  int MainFrame::CreateUserMsgView()
  {
    UsersMessageView* msgView = new UsersMessageView(this);
    msgView->setSizes(GetMsgViewSizes());
    connect(msgView, SIGNAL(splitterMoved(int, int)), SLOT(ResizeMessagesView()));
    return m_msgBoxStackedWidget->addWidget(msgView);
  }

  UsersMessageView* MainFrame::GetUsersMessageView(const std::wstring& uuid)
  {
    UsersMessageView* view = nullptr;
    auto iter = m_userItems.find(uuid);
    if (iter != m_userItems.end())
    {
      UserItem item(iter->second);
      controls::UserListItem* curentItem = static_cast<controls::UserListItem*>(
        m_userListWidget->currentItem());
      if (uuid != curentItem->GetUserID())
      {
        item.userlistItem->EnableNotifyStyle();
      }
      view = static_cast<UsersMessageView*>(m_msgBoxStackedWidget->widget(item.msgWidgetID));
    }
    return view;
  }

  void MainFrame::OnTextMessageReceived(const std::wstring& uuid, const std::wstring& message)
  {
    UsersMessageView* msgView = GetUsersMessageView(uuid);
    if (msgView)
    {
      std::wstring name(net::NetUsersManager::Instance()->GetNetUserName(uuid));
      MessageInfo msg(name, message, CurrentTimeToStr("hh:mm"), true);
      msgView->AppendTxtMessage(msg);
    }
  }

  void MainFrame::OnFileMessageReceived(const std::wstring& uuid, const std::wstring& fileName,
                                        const msg::CompletionCallback& callback)
  {
    emit FileMessageReceived(uuid, fileName, callback);
  }

  void MainFrame::AddNewFileMessage(const std::wstring& uuid, const std::wstring& fileName,
                                    const msg::CompletionCallback& callback)
  {
    UsersMessageView* msgView = GetUsersMessageView(uuid);
    if (msgView)
    {
      std::wstring name(net::NetUsersManager::Instance()->GetNetUserName(uuid));
      MessageInfo msg(name, fileName, CurrentTimeToStr("hh:mm"), true);
      IProgressUIObserver* observer = msgView->AppendFileMessage(msg);
      callback(observer);
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
    if (loginManager->IsOnline() && m_msgBoxStackedWidget->count() > 0)
    {
      UsersMessageView* msgView = static_cast<UsersMessageView*>(
        m_msgBoxStackedWidget->currentWidget());

      std::wstring text;
      if (msgView->GetTextFromEdit(&text))
      {
        MessageInfo msg(loginManager->GetCurrentUser()->name, text, CurrentTimeToStr("hh:mm"), false);
        msgView->AppendTxtMessage(msg);
        controls::UserListItem* currentItem = static_cast<controls::UserListItem*>(
          m_userListWidget->currentItem());
        msg::ChatMessagesManager::Instance()->Send(currentItem->GetUserID(), text);
      }
    }
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
    UsersMessageView* currentWdgSplitter = static_cast<UsersMessageView*>(
      m_msgBoxStackedWidget->currentWidget());
    if (currentWdgSplitter)
    {
      QList<int> newSizesWdg(currentWdgSplitter->sizes());
      for(auto it = m_userItems.begin(); it != m_userItems.end(); ++it)
      {
        UsersMessageView* wdgSplitter = static_cast<UsersMessageView*>(
          m_msgBoxStackedWidget->widget(it->second.msgWidgetID));
        wdgSplitter->setSizes(newSizesWdg);
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
