#ifndef __DDSMGR_H__
#define __DDSMGR_H__

#include "ddalib_export.h"
#include <list>
using namespace std;

struct DDSParticpantProduct 
{
	unsigned int systemid;
	string ExecName;//sacpp_builtintopics_sub.exe
	string ParticipantName;//sacpp_builtintopics_sub.exe
	unsigned int PID;
	string NodeName;//WORKHP
	unsigned int FederationId;//3d51ced3
	string VendorId;//1.2
	//
	unsigned int instance_states;//DDS::ALIVE_INSTANCE_STATE
};
class DDSMgrData;
class _DDSLIB_API DDSMgr
{
public:
	typedef void (* DDSParticpantCB)(DDSParticpantProduct& product);
	enum CallBackType{ CBK_Particpant};
	static DDSMgr &GetInstance();
	static DDSMgr* GetInstancePtr();
	//Participant
	unsigned int getCurrentSystemId();//获取当前进程systemid
	list<DDSParticpantProduct>* getAllHistoricalParticpant();//获取所有参与者（DDS服务参与者除外：Built-in participant,splicedaemon,cmsoap,durability,ddsi2 ）
	bool setCallBack(CallBackType typ,DDSParticpantCB pcb);

	//end Participant
	string getLastError();
	///////////////
	void run();
	void start();
	void stop();
private:
	bool init();
	void getProductFromXml(const char* strxml,DDSParticpantProduct &pdt);
	DDSMgr(void);
	~DDSMgr(void);
	DDSMgrData *m_ddsmgrData;
	volatile bool m_isRun;
};
#endif // __DDSMGR_H__


