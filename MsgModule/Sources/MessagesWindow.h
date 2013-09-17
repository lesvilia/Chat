#pragma once

#include <memory>
#include <Windows.h>
#include "boost\noncopyable.hpp"


namespace msg
{
  class MessagesReceiver;

  class MessagesWindow 
    : private boost::noncopyable
  {
  public:
    MessagesWindow(MessagesReceiver* msgReceiver);
    ~MessagesWindow();
    void OnStateMessageReceived();
    void OnChatMessageReceived();

  private:
    void CreateMainWindow();
    void RegisterMainWindow();

  private:
    HWND m_window;
    MessagesReceiver* m_msgReceiver;
  };

  typedef std::unique_ptr<MessagesWindow> MessagesWindowPtr;
}