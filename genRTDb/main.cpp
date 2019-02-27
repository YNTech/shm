#include <QtCore/QCoreApplication>
#include <QTextCodec>
#include "rtdb_define.h"
#include "rtdb_interface.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	const char* basepath = "..\\db";
	const char* dbsetsname = "test";
	char err[256];
	if (!genRTDB(basepath, dbsetsname, err))
		printf("\n ERROR:%s \n", err);
	//if(!readRTDbSetsDef(basepath, dbsetsname, err))
	//	printf("\n ERROR:%s \n",err);
	//RTDbSets *dbsets = getRtDbDef(dbsetsname);
	//if(dbsets)
	//{
	//	if(!genDbSetsFile(basepath, dbsetsname, err))
	//		printf("\n ERROR:%s \n",err);
	//	std::vector<RTDbInfo>::iterator it = dbsets->rtdbs.begin();
	//	while(it!= dbsets->rtdbs.end())
	//	{
	//		if(!genDbFile(basepath,dbsetsname, (*it).name.c_str(), err))
	//			printf("\n ERROR:%s \n",err);
	//		it++;
	//	}
	//}
	return a.exec();
}
