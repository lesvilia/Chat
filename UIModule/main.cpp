#include <QtWidgets/QApplication>
#include "Sources/Mainframe.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MainFrame mainWindow;
	mainWindow.show();

	return a.exec();
}
