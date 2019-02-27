#ifndef RTDBNETPLUGN_H
#define RTDBNETPLUGN_H

#include <string>
#include <QMap>
#include <QList>
#include <netinterfaces.h>

struct psdata
{
	QMap<unsigned char,sender_handle> hand;///<plugn handle
	std::string dbname;
	std::string tblname;
	unsigned char datasrc;
	void* pdata;///<表起始地址
	unsigned int rowsize;///<行大小
};

#pragma pack(push)
#pragma pack (1)
struct psrowdata
{
	int ndind;
	int dbind;
	int tblind;
	unsigned char pub_src;//数据源
	unsigned char sub_src;
};
#pragma pack(pop)



class RTDbNetPlugn
{
public:
	RTDbNetPlugn();
	~RTDbNetPlugn();
	bool initNet(const char* localnd);///<初始化，发布/订阅
	bool sendData(int index, const char* tblName, const char* dbName);
	sender_handle getSenderHandle(const char* tblName,const char* dbName);///<return handle 为m_lst_pub的index
	bool sendData(int index, sender_handle hdl);
	QList<pNetRunCallback> getCallBack();
	std::string getLastError();
private:
	int getNodeInd(std::string ndnm,std::string *err =  nullptr);
	bool getRtDbNames(std::string *err =  nullptr);
	bool publish(int curNdInd, std::string *err =  nullptr);
	bool subscribe(int curNdInd, std::string *err =  nullptr);
	bool loadPlugns();

	QList<psdata> m_lst_pub;
	QMap<unsigned char,NetInterface*> m_plugns;
	std::string m_errString;
};

#endif // RTDBNETPLUGN_H
