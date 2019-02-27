#ifndef NETINTERFACES_H
#define NETINTERFACES_H

#include <QtPlugin>

typedef void (*pNetRunCallback)();
typedef unsigned int sender_handle;
typedef unsigned int receiver_handle;
#define ErrorHandle 0xFFFFFFFF


class NetInterface
{
public:
    virtual ~NetInterface() {}

	virtual sender_handle publish    (int dataSize,const void* pStData, const char* tblname, const char* dbname) =0;///<发布并登记
	virtual receiver_handle subscribe  (int dataSize,void* pStData, const char* tblname, const char* dbname) =0;
	virtual bool sendData   (int index, sender_handle hand ) =0;
	virtual inline std::string getLastError() =0;
	virtual pNetRunCallback getCallBack() =0;
	virtual void stopRun() = 0;
};


QT_BEGIN_NAMESPACE

Q_DECLARE_INTERFACE(NetInterface, "YNTech.RTDB.RTDbNetExt.NetInterface")


QT_END_NAMESPACE

#endif//NETINTERFACES_H
