#include <QtWidgets/QApplication>
#include "Sources/Mainframe.h"
#include "MessagesReceiver.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	ui::MainFrame mainWindow;
	msg::MessagesReceiver msgReceiver(&mainWindow);
	mainWindow.Show();

	return a.exec();
}
