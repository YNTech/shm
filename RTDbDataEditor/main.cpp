#include <QtWidgets>
#include <QtCore>
#include "rtdbdataeditor.h"
#include "rtdb_api.h"
#include "rtdb_interface.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("rtdbdataeditor_") + locale);
	a.installTranslator(&translator);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	a.setStyle(QStyleFactory::create("plastique"));
	//a.setFont(QFont("wenquanyi"));  
	if(!rtdb_has_load())
	{
		QMessageBox::warning(0,QObject::tr("Error"),QString(QObject::tr("RTDb is not load!")));
		exit(-1);
	}
	RTDbDataEditor w(getBasePath());
	w.show();
	return a.exec();
}
