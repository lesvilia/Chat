#include <QtWidgets/QApplication>
#include "Sources/Mainframe.h"
#include "MessagesReceiver.h"

namespace 
{
	const wchar_t MUTEX_NAME[] = { L"4BEC4994-AEF4-48F4-A2EA-294563154342" };
		
}



int main(int argc, char *argv[])
{
	HANDLE mutexHandle = ::CreateMutex(NULL, FALSE, MUTEX_NAME);
	if (mutexHandle && ::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(mutexHandle);
		::MessageBox(NULL, L" Application already is running!", L"Error instance", MB_OK);
		return 0;
	}

	QApplication a(argc, argv);

	ui::MainFrame mainWindow;
	msg::MessagesReceiver msgReceiver(&mainWindow);
	mainWindow.Show();

	int result =  a.exec();
	::CloseHandle(mutexHandle);
	return result;
}
