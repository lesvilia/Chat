#pragma once
#include <QString>

namespace settings
{
  namespace mainframe
  {
    namespace sizes
    {
      const unsigned MAINFRAME_WIDTH = 700;
      const unsigned MAINFRAME_HEIGH = 500;

      const unsigned LEFT_PART_WIDTH = MAINFRAME_WIDTH / 3;
      const unsigned RIGHT_PART_WIDTH = MAINFRAME_WIDTH - LEFT_PART_WIDTH;

      const unsigned BOTTOM_PART_HEIGH = MAINFRAME_HEIGH / 3;
      const unsigned TOP_PART_HEIGH = MAINFRAME_HEIGH - BOTTOM_PART_HEIGH;

      const unsigned MIN_BUTTON_WIDTH = 90;
      const unsigned DEFAULT_COLUMN_WIDTH = 70;
    }

    namespace colors
    {
      const QColor MY_MSG_COLOR(QColor(0, 0, 0));
      const QColor USERS_MSG_COLOR(QColor(0, 0, 255));
    }

    namespace strings
    {
      const QString MAIN_TITLE("LChat");
      const QString MAIN_ICON_PATH("chat_icon.png");
      const QString MAIN_WIDGET_NAME("main_widget");
      
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
        "QMenu::item:selected {  background: #ffd700; color: black; }"
      );

      const QString MAIN_WIDGET_STYLE
      (
        "QWidget#main_widget { background: #CAFF70; }"
        "QSplitter::handle { background: #CAFF70; }"
        "QListWidget::item:selected { background: #007FFF; color: #FFFFFF; }"
      );

      const QString MESSAGE_WIDGET_STYLE
      (
        "QTableView { padding: 5px; }"
        "QTableView::item { margin-bottom: 10px; }"
      );

      const char STATE_LABEL_FORMAT[] = "%1 %2";
      const QString STATE_LABEL_STYLE("QLabel { background-color: #9ACD32; }");
      const QString USER_ICON_PATH("user_ico.png");
      const QString ONLINE_STATE("Online");
      const QString OFFLINE_STATE("Offline");
      const QString BUTTON_TEXT("Send Message");
      const QString USERS_LABEL_TEXT("<h4>Online Users</h4>");
      const QString MESSAGES_LABEL_TEXT("<h4>Conversation</h4>");
      
      const QString ABOUT_TITLE("About LChat");
      const QString ABOUT_MESSAGE
      (
        "This program allows you to exchange messages on a local network. "
        "All right reserved. Ivan Sialiuk."
      );
    }
  }

  namespace logindialog
  {
    const QString LOGIN_TITLE("Log In");
    const QString REGISTRATION_TITLE("Registration");
    const QString LABEL_NAME("Name:");
    const QString LABEL_PASSWORD("Password:");

    const QString MSGBOX_REG_TITLE("Registration Error!");
    const QString MSGBOX_LOGIN_TITLE("Login Error!");

    const QString MSGBOX_WRONG_PASS("Wrong user password!");
    const QString MSGBOX_USER_EXIST("User already exists! Please enter other name.");
    const QString MSGBOX_USER_NOT_FOUND("User not found! Please check entered name.");
    const QString MSGBOX_DATA_EMPTY("You did not enter Name or Password.");
  }

  namespace settingsdialog
  {
    const QString LABEL_ADDRESS("IP address:");
    const QString LABEL_STATE_PORT("State message port:");
    const QString LABEL_CHAT_PORT("Chat message port:");
  }
}
