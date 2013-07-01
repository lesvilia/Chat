#include "stdafx.h"
#include "MessagesWindow.h"

namespace msg
{
	namespace
	{
		const LPCWSTR CLASS_NAME = L"MsgReceiverWindow";
		const UINT WM_PROCESS_STATE_MSG = WM_USER + 100;
		const UINT WM_PROCESS_CHAT_MSG = WM_USER + 101;

		LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
			case WM_PROCESS_STATE_MSG:
				return 0;

			case WM_PROCESS_CHAT_MSG:
				return 0;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}

	MessageWindow::MessageWindow(MessageReciever* msgReceiver)
		: m_window(NULL)
		, m_msgReceiver(msgReceiver)

	{
		RegisterMainWindow();
		CreateMainWindow();
	}

	MessageWindow::~MessageWindow()
	{
		::DestroyWindow(m_window);
	}

	void MessageWindow::OnStateMessageReceived()
	{
		::PostMessage(m_window, WM_PROCESS_STATE_MSG, reinterpret_cast<WPARAM>(m_msgReceiver), NULL);
	}

	void MessageWindow::OnChatMessageReceived()
	{
		::PostMessage(m_window, WM_PROCESS_CHAT_MSG, reinterpret_cast<WPARAM>(m_msgReceiver), NULL);
	}

	void MessageWindow::RegisterMainWindow()
	{
		WNDCLASSEX wcex = {0};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpszClassName = CLASS_NAME;
		wcex.lpfnWndProc = WindowProc;
		WORD result = RegisterClassEx(&wcex);
		assert(result != NULL);
	}

	void MessageWindow::CreateMainWindow()
	{
		m_window = CreateWindow(CLASS_NAME, L"", 0, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
								 HWND_MESSAGE, nullptr, nullptr, nullptr);
		assert(m_window != NULL);
	}
}