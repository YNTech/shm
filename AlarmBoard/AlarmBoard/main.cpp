#include "alarmboard.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	AlarmBoard w;
	w.show();
	return a.exec();
}
