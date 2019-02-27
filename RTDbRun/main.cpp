#include <QtCore/QCoreApplication>
#include <QTextCodec>
#include "rtdb_interface.h"
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	char* dbsets;
	if(argc>1)
		dbsets = argv[1];
	else
		dbsets = "test";
	char err[256];
	if(!loadRTDb(dbsets, "../db", err))
		printf("\n ERROR:%s \n",err);
	setDefauluDbSets(dbsets);

	return a.exec();
}
