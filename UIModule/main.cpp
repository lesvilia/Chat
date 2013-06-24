#include <QtWidgets/QApplication>
#include "Sources/Mainframe.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	ui::MainFrame mainWindow;
	mainWindow.Show();

	return a.exec();
}
