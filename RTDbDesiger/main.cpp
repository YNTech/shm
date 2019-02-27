#include <QtGui>
#include <QtWidgets>
#include "rtdbdesiger.h"
#include "rtdb_api.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("rtdbdesiger_") + locale);
	a.installTranslator(&translator);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	a.setStyle(QStyleFactory::create("plastique"));

	if (rtdb_has_load())
	{
		QMessageBox::warning(0, QObject::tr("Error"), QObject::tr("The RTDb has been opened. First, the RTDb must be closed before editing."));
		return 0;
	}
	RTDbDesiger w;
	w.show();
	return a.exec();
}
