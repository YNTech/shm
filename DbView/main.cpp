#include <QtWidgets>
#include "dbview.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("DbView_") + locale);
	a.installTranslator(&translator);

	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	a.setStyle(QStyleFactory::create("plastique"));
	DbView w;
	w.show();
	return a.exec();
}
