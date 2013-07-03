#include "stdafx.h"
#include "MessagesWindow.h"
#include "MessagesReceiver.h"

namespace msg
{
	namespace
	{
		const LPCWSTR CLASS_NAME = L"MsgReceiverWindow";
		const UINT WM_PROCESS_STATE_MSG = WM_USER + 100;
		const UINT WM_PROCESS_CHAT_MSG = WM_USER + 101;

		LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MessagesReceiver* msgReceiver = reinterpret_cast<MessagesReceiver*>(wParam);
			if (msgReceiver != nullptr)
			{
				switch(message)
				{
				case WM_PROCESS_STATE_MSG:
					msgReceiver->ProcessStateMessage();
					return 0;

				case WM_PROCESS_CHAT_MSG:
					msgReceiver->ProcessChatMessage();
					return 0;
				}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	MessagesWindow::MessagesWindow(MessagesReceiver* msgReceiver)
		: m_window(NULL)
		, m_msgReceiver(msgReceiver)

	{
		RegisterMainWindow();
		CreateMainWindow();
	}

	MessagesWindow::~MessagesWindow()
	{
		::DestroyWindow(m_window);
	}

	void MessagesWindow::OnStateMessageReceived()
	{
		::PostMessage(m_window, WM_PROCESS_STATE_MSG, reinterpret_cast<WPARAM>(m_msgReceiver), NULL);
	}

	void MessagesWindow::OnChatMessageReceived()
	{
		::PostMessage(m_window, WM_PROCESS_CHAT_MSG, reinterpret_cast<WPARAM>(m_msgReceiver), NULL);
	}

	void MessagesWindow::RegisterMainWindow()
	{
		WNDCLASSEX wcex = {0};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpszClassName = CLASS_NAME;
		wcex.lpfnWndProc = WindowProc;
		WORD result = RegisterClassEx(&wcex);
		assert(result != NULL);
	}

	void MessagesWindow::CreateMainWindow()
	{
		m_window = CreateWindow(CLASS_NAME, L"", 0, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
								 HWND_MESSAGE, nullptr, nullptr, nullptr);
		assert(m_window != NULL);
	}
}