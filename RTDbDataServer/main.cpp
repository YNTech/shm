#include "rtdbdataserver.h"
#include <QtWidgets>
#include <QtPlugin>
#include <QtNetwork>
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"

//Q_IMPORT_PLUGIN(BasicToolsPlugin)

int main(int argc, char *argv[])
{
	QString lcName;
	if(argc>1)
		lcName = QString::fromLocal8Bit(argv[1]);
	else
		lcName = QHostInfo::localHostName();
	QApplication a(argc, argv);
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("RTDbDataServer_") + locale);
	a.installTranslator(&translator);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	a.setStyle(QStyleFactory::create("plastique"));

	if (!g_registerLog(QString("RTDbDataServer")))
	{
		printf("◊¢≤·»’÷æ ß∞‹£°");
	}

	RTDbDataServer w(lcName);
	w.show();
	return a.exec();
}
