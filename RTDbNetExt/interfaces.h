#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>

typedef void (*pNetRunCallback)();
typedef unsigned int sender_handle;
typedef unsigned int receiver_handle;
#define ErrorHandle 0xFFFFFFFF


class NetInterface
{
public:
    virtual ~NetInterface() {}

	virtual sender_handle publish    (int dataSize,const char* tblname, const char* dbname) =0;///<发布并登记
	virtual receiver_handle subscribe  (int dataSize,const char* tblname, const char* dbname) =0;
	virtual bool sendData   (int index, const void* data,const char* tblname,const char* dbname ) =0;
	virtual bool sendData   (sender_handle hand, const void* data) =0;
	virtual int  recvData   (int& id,char* data,const char* tblname,const char* dbname ) =0;//return recived data bytes ret=0:NOData;ret<0:error)
	virtual inline std::string getLastError() =0;
	virtual pNetRunCallback getCallBack() =0;
};


QT_BEGIN_NAMESPACE

Q_DECLARE_INTERFACE(NetInterface, "YNTech.RTDB.RTDbNetExt.NetInterface")


QT_END_NAMESPACE

#endif
