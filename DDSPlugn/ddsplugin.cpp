#include "ddsplugin.h"
#include "ddslib.h"
#include <Windows.h>

static bool m_isRuning = false;
DDSPlugin::DDSPlugin()
{
	m_isRuning = true;
}

DDSPlugin::~DDSPlugin()
{

}
sender_handle DDSPlugin::publish    (int dataSize,const void* pStData, const char* tblname, const char* dbname)
{
	return ddslib::GetInstancePtr()->publish(dataSize,pStData,tblname,dbname);
}
receiver_handle DDSPlugin::subscribe  (int dataSize,void* pStData, const char* tblname, const char* dbname) 
{
	return ddslib::GetInstancePtr()->subscribe(dataSize,pStData,tblname,dbname);
}

bool DDSPlugin::sendData   (int index, sender_handle hand)
{
	return ddslib::GetInstancePtr()->sendData(index,hand);
}

pNetRunCallback DDSPlugin::getCallBack()
{
	return ddsRecvRun;
}
std::string DDSPlugin::getLastError()
{
	return ddslib::GetInstancePtr()->getLastError();
}
void DDSPlugin::stopRun()
{
	m_isRuning = false;
}
void DDSPlugin::ddsRecvRun()
{
	int datanums = ((ddslib*)(ddslib::GetInstancePtr()))->m_psdata->m_readers.size();
	while(m_isRuning)
	{
		CommBase<ddslib>* pdds = ddslib::GetInstancePtr();
		for (int i=0; i<datanums; i++)
			pdds->recvData(i);
		Sleep(50);
	}
}