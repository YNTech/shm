#ifndef __DDSPLUGN_H__
#define __DDSPLUGN_H__

#include "..\RTDbDataServer\netinterfaces.h"

class DDSPlugin : public QObject,public NetInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "YNTech.RTDB.RTDbNetExt.NetInterface" FILE "NetInterface.json")
	Q_INTERFACES(NetInterface)
public:
	DDSPlugin();
	~DDSPlugin();
	virtual sender_handle publish    (int dataSize,const void* pStData, const char* tblname, const char* dbname) ;
	virtual receiver_handle subscribe  (int dataSize,void* pStData, const char* tblname, const char* dbname) ;
	virtual bool sendData   (int index, sender_handle hand) ;
	virtual inline std::string getLastError();
	virtual pNetRunCallback getCallBack();
	virtual void stopRun();
private:
	static void ddsRecvRun();
};

#endif // __DDSPLUGN_H__
