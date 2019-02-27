#ifndef RTDBNETEXT_H
#define RTDBNETEXT_H

#include "rtdbnetext_global.h"
#include <string>
#include <QMap>
#include <QList>
#include <interfaces.h>

struct psdata
{
	QMap<unsigned char,sender_handle> hand;///<plugn handle
	std::string dbname;
	std::string tblname;
	unsigned char datasrc;
	void* pdata;///<表起始地址
	unsigned int rowsize;///<行大小
	sender_handle localHand;///<dataserver handle
};

#pragma pack(push)
#pragma pack (1)
struct psrowdata
{
	int ndind;
	int dbind;
	int tblind;
	unsigned char pub_src;
	unsigned char sub_src;
};
#pragma pack(pop)



class RTDBNETEXT_EXPORT RTDbNetExt
{
public:
	RTDbNetExt();
	~RTDbNetExt();
	bool initNet(const char* localnd);///<初始化，发布/订阅
	void sendData(int index,const void* data,const char* tblName,const char* dbName);
	sender_handle getSenderHandle(int index,const char* tblName,const char* dbName);
	void sendData(sender_handle hdl, int index);
	QList<pNetRunCallback> getCallBack();
	std::string getLastError();
private:
	int getNodeInd(std::string ndnm,std::string *err =  nullptr);
	bool getRtDbNames(std::string *err =  nullptr);
	bool pub_and_sub(int curNdInd, std::string *err =  nullptr);
	bool loadPlugns();

	std::list<psdata> m_lst_pub;
	QMap<unsigned char,NetInterface*> m_plugns;
	std::string m_errString;
};

#endif // RTDBNETEXT_H
