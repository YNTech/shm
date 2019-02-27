#include <Windows.h>
#include "dds_dcps.h"
#include <map>
#include<algorithm> 
#include "TimeMgr.h"
#include "ddslib.h"
#include "MicroSecondTimer.h"
#include "DDSMgr.h"

const char* g_registerfederate = "timemgr_registerfederate";//注册邦员
const char* g_registerfederatereturn = "timemgr_registerfederatereturn";//注册邦员
const char* g_registerstep = "timemgr_registerstep";//登记步长
const char* g_registerstepreturn = "timemgr_registerstepreturn";//登记步长返回
const char* g_requesttimeadvance = "timemgr_requesttimeadvance";//请求时间推进
const char* g_requesttimeadvancereturn = "timemgr_requesttimeadvancereturn";//时间推进请求返回，返回成功和失败及失败原因
const char* g_timeadvanced = "timemgr_timeadvanced";//请求时间推进通知
//const string g_timemgrpartion = "timemgr_partion";
unsigned int g_fedindex = 0;//邦员id或时间句柄
const char g_strRegesterReturn[][32] = {"成功","邦员注册重复！"};
const int g_nRegesterReturn = 2;
const char g_strRequestTimeAdvanceReturn[][32] = {"成功","申请时间小于当前仿真时间！","当前成员未注册！"};
const int g_nRequestTimeAdvanceReturn = 2;

DWORD WINAPI startRecvData(LPVOID lpThreadParameter);
DWORD WINAPI startTimeAdvance(LPVOID lpThreadParameter);
HANDLE g_mutexFedList = CreateMutex(NULL,FALSE,"tickets");
HANDLE g_evtHasAdvReq =CreateEvent(NULL,TRUE,FALSE,NULL); 
ddslib* g_mydds = (ddslib*)ddslib::GetInstancePtr();
//注册邦员
struct RegisterFederate
{
	unsigned int systemid;
};
//返回的注册结果
struct RegisterFederateReturn
{
	unsigned int systemid;
	double ftime;//当前时间
	unsigned int nRet;//0:成功，1：名称重复
};
//登记步长
struct RegisterStep
{
	unsigned int systemid;
	unsigned int nStep;//步长
};
//返回的登记步长结果
struct RegisterStepReturn
{
	unsigned int systemid;
	unsigned int nHandle;//仿真时间句柄
	unsigned int nRet;//0:成功
};
struct RequestTimeAdvanceData
{
	double ftime;//请求推进到的时间
};
//用index来判断哪个数据是返回给本邦员的。
struct RequestTimeAdvanceReturn
{
	unsigned int nRet;///<时间推进请求返回结果，0：成功，
};
//用index来判断
struct TimeAdvanced
{
	double ftime;
};


struct ItemTm
{
	//string fedName;//邦员名称
	unsigned int SystemId;
	double ftime;  //邦员请求推进时间,增序排列
	unsigned int index;
	bool hasrequest;
	bool operator < (const ItemTm& ti) const {  
		if(!hasrequest)
			return true;
		if(!ti.hasrequest)
			return false;
		return ftime < ti.ftime;  
	}  
};
class TimeMgrData
{
	friend TimeMgr;
private:
	TimeMgType m_tmgTyp;
	unsigned long m_curTime;//us

	//string m_fedName;
	unsigned int m_systemid;//TMG_Negotiate有效
	unsigned int m_fedInd;//TMG_Negotiate:服务器分配的邦员ID;TMG_Notice:仿真时间句柄
	sender_handle   m_registerfederate_sendhd;
	sender_handle   m_registerstep_sendhd;
	receiver_handle m_registerfederatereturn_recvhd;
	receiver_handle m_registerstepreturn_recvhd;
	sender_handle   m_requesttimeadvance_sendhd;
	receiver_handle m_requesttimeadvancereturn_recvhd;
	receiver_handle m_timeadvanced_recvhd;
};
struct TmStep
{
	TmStep(){};
	TmStep(int hand, unsigned int nst){ ind=hand; ntime=nst; };

	int ind;//时间句柄
	unsigned int ntime;//仿真时间，us
	bool operator < (const TmStep& ti) const {  
		return ntime < ti.ntime;  
	}  
/*	bool operator == (const TmStep& ti) const {  
		return step == ti.step;  
	} */ 
};
struct StepData
{
	StepData(){};
	StepData(unsigned int sysid, unsigned int ns){ systemids.push_back(sysid); step=ns; };
	list<unsigned int> systemids;
	unsigned int step;
};
class TimeMgrSvrData
{
	friend TimeMgrServer;
private:
	TimeMgType m_tmgTyp;
	unsigned long m_curTime;//us

	map<unsigned int,StepData> m_steps; //TMG_Notice有效,key:时间句柄。单位:ms
	list<TmStep> m_tmpTimes;//TMG_Notice有效,
	list<ItemTm> m_feds;//TMG_Negotiate有效
	receiver_handle m_registerfederate_recvhd;
	receiver_handle m_registerstep_recvhd;
	sender_handle   m_registerfederatereturn_sendhd;
	sender_handle   m_registerstepreturn_sendhd;
	receiver_handle m_requesttimeadvance_recvhd;
	sender_handle   m_requesttimeadvancereturn_sendhd;
	sender_handle   m_timeadvanced_sendhd;
	HANDLE	m_hdRecvDara;
	HANDLE	m_hdTmAdv;

	list<regnode> m_lsttmp;
};
//TimeMgrData g_timeMgrData;

TimeMgr::TimeMgr(void):m_timeMgrData(new TimeMgrData),m_errString(new std::string)
{
	m_timeMgrData->m_fedInd = 0;
}


TimeMgr::~TimeMgr(void)
{
	delete m_timeMgrData;
	delete m_errString;
}

TimeMgr & TimeMgr::GetInstance() 
{  
	static TimeMgr instance;    
	return instance;  
}  
TimeMgr* TimeMgr::GetInstancePtr() 
{   
	return &GetInstance();  
}


double TimeMgr::currentTime()
{
	return (double)m_timeMgrData->m_curTime/MILLION;
}
bool TimeMgr::initMember(TimeMgType typ)
{
	m_timeMgrData->m_tmgTyp = typ;
	ddslib* mydds = (ddslib*)ddslib::GetInstancePtr();
	if(typ == TMG_Negotiate)
	{
		m_timeMgrData->m_registerfederate_sendhd = mydds->publish(sizeof(RegisterFederate),g_registerfederate);
		if(m_timeMgrData->m_registerfederate_sendhd == ErrorHandle)
		{
			*m_errString =string("publish RegisterFederate:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_requesttimeadvance_sendhd = mydds->publish(sizeof(RequestTimeAdvanceData),g_requesttimeadvance);
		if(m_timeMgrData->m_requesttimeadvance_sendhd == ErrorHandle)
		{
			*m_errString = string("publish RequestTimeAdvance:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_registerfederatereturn_recvhd = mydds->subscribe(sizeof(RegisterFederateReturn),g_registerfederatereturn);
		if(m_timeMgrData->m_registerfederatereturn_recvhd == ErrorHandle)
		{
			*m_errString = string("subscribe RegisterFederateReturn:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_requesttimeadvancereturn_recvhd = mydds->subscribe(sizeof(RequestTimeAdvanceReturn),g_requesttimeadvancereturn);
		if(m_timeMgrData->m_requesttimeadvancereturn_recvhd == ErrorHandle)
		{
			*m_errString = string("subscribe RequestTimeAdvanceReturn:") + mydds->getLastError();
			return false;
		}
	}
	else
	{
		m_timeMgrData->m_registerstep_sendhd = mydds->publish(sizeof(RegisterStep),g_registerstep);
		if(m_timeMgrData->m_registerstep_sendhd == ErrorHandle)
		{
			*m_errString =string("publish RegisterStep:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_registerstepreturn_recvhd = mydds->subscribe(sizeof(RegisterStepReturn),g_registerstepreturn);
		if(m_timeMgrData->m_registerstepreturn_recvhd == ErrorHandle)
		{
			*m_errString = string("subscribe RegisterStepReturn:") + mydds->getLastError();
			return false;
		}
	}
	m_timeMgrData->m_timeadvanced_recvhd = mydds->subscribe(sizeof(TimeAdvanced),g_timeadvanced);
	if(m_timeMgrData->m_timeadvanced_recvhd == ErrorHandle)
	{
		*m_errString = string("subscribe TimeAdvanced:") + mydds->getLastError();
		return false;
	}
	return true;
}
bool TimeMgr::registerFederate(double &ftime)
{
	printf("仿真成员注册……\n");
	if(m_timeMgrData->m_tmgTyp == TMG_Notice)
	{
		*m_errString = "当前邦员设置为通知方式更新时间，不能发送时间推进请求！";
		return false;
	}
	m_timeMgrData->m_systemid = DDSMgr::GetInstancePtr()->getCurrentSystemId();
	if(m_timeMgrData->m_systemid==0)
	{
		*m_errString = "获取邦员systemid出错:"+DDSMgr::GetInstancePtr()->getLastError();
		return false;
	}
	//邦员注册
	RegisterFederate rfd;
	rfd.systemid = m_timeMgrData->m_systemid ;
	if(g_mydds->sendData(0,m_timeMgrData->m_registerfederate_sendhd,&rfd))
	{
		int ind;
		RegisterFederateReturn rfrd;
		int n = 30;
		while(n)
		{
			int ret = g_mydds->recvData(ind, (char*)&rfrd, m_timeMgrData->m_registerfederatereturn_recvhd);
			if(ret>0)
			{
				if(rfrd.systemid == m_timeMgrData->m_systemid)
				{
					m_timeMgrData->m_fedInd = ind;
					if(rfrd.nRet == 0)
					{
						ftime = rfrd.ftime;
						printf("注册成员成功：systemid=%d\n",rfrd.systemid);
						return true;
					}
					else
					{
						if(rfrd.nRet < g_nRegesterReturn)
							*m_errString = string("register federate error(02):")+g_strRegesterReturn[rfrd.nRet];
						else
							*m_errString = "register federate error(02):错误的返回值。";
						return false;
					}
				}
			}
			else if(ret == 0)
			{
				Sleep(100);
				n--;
			}
			else
			{
				*m_errString = string("register federate error(01):读取返回数据出错，")+g_mydds->getLastError();
				return false;
			}
		}
		if(!n)
		{
			*m_errString = string("register federate error(03):读取返回数据超时！请确认服务器节点已启动！");
			return false;
		}
	}
	else
	{
		*m_errString = string("register federate error:发送注册数据出错，")+g_mydds->getLastError();
		return false;
	}
	return true;
}
bool TimeMgr::registerTimeStep(unsigned int step)
{
	printf("登记仿真时间步长……\n");
	if(m_timeMgrData->m_tmgTyp != TMG_Notice)
	{
		*m_errString = "当前邦员设置为协商步进方式更新时间，不能登记仿真时间步长！";
		return false;
	}
	m_timeMgrData->m_systemid = DDSMgr::GetInstancePtr()->getCurrentSystemId();
	if(m_timeMgrData->m_systemid==0)
	{
		*m_errString = "获取邦员systemid出错:"+DDSMgr::GetInstancePtr()->getLastError();
		return false;
	}
	//登记仿真时间步长
	RegisterStep rsd;
	rsd.systemid = m_timeMgrData->m_systemid ;
	rsd.nStep = step;
	if(g_mydds->sendData(0,m_timeMgrData->m_registerstep_sendhd,&rsd))
	{
		int ind;
		RegisterStepReturn rsrd;
		int n = 30;
		while(n)
		{
			int ret = g_mydds->recvData(ind, (char*)&rsrd, m_timeMgrData->m_registerstepreturn_recvhd);
			if(ret>0)
			{
				if(rsrd.systemid == m_timeMgrData->m_systemid)
				{
					if(rsrd.nRet == 0)
					{
						m_timeMgrData->m_fedInd = rsrd.nHandle;
						printf("登记时间步长成功：systemid=%d,handle=%d\n",rsrd.systemid,rsrd.nHandle);
						return true;
					}
					else
					{
						*m_errString = "register federate error(02):错误的返回值。";
						return false;
					}
				}
			}
			else if(ret == 0)
			{
				Sleep(100);
				n--;
			}
			else
			{
				*m_errString = string("register tiem step error(01):读取返回数据出错，")+g_mydds->getLastError();
				return false;
			}
		}
		if(!n)
		{
			*m_errString = string("register tiem step error(03):读取返回数据超时！请确认服务器节点已启动！");
			return false;
		}
	}
	else
	{
		*m_errString = string("register tiem step error:发送登记步长数据出错，")+g_mydds->getLastError();
		return false;
	}
	return true;
}
bool TimeMgr::RequestTimeAdvance(double t)
{
	if(m_timeMgrData->m_tmgTyp == TMG_Notice)
	{
		*m_errString = "当前邦员设置为通知方式更新时间，不能发送时间推进请求！";
		return false;
	}
	ddslib* mydds = (ddslib*)ddslib::GetInstancePtr();
	RequestTimeAdvanceData rtad;
	rtad.ftime = t;
	if(mydds->sendData(m_timeMgrData->m_fedInd,m_timeMgrData->m_requesttimeadvance_sendhd, (char*)&rtad))
	{
		int ind;
		RequestTimeAdvanceReturn rtar;
		int n = 50;
		while(n)
		{
			int ret = mydds->recvData(ind, (char*)&rtar, m_timeMgrData->m_requesttimeadvancereturn_recvhd);
			if(ret>0)
			{
				if(ind == m_timeMgrData->m_fedInd)
				{
					if(rtar.nRet == 0)
						break;
					else
					{
						if(rtar.nRet < g_nRequestTimeAdvanceReturn)
							*m_errString = string("request time advance federate error(02):")+g_strRequestTimeAdvanceReturn[rtar.nRet];
						else
							*m_errString = "request time advance error(02):错误的返回值。";
						return false;
					}
				}
			}
			else if(ret == 0)
			{
				Sleep(20);
				n--;
			}
			else
			{
				*m_errString = string("request time advance error(01):读取返回数据出错，")+mydds->getLastError();
				return false;
			}
		}
		if(!n)
		{
			*m_errString = string("request time advance error(03):读取返回数据超时！");
			return false;
		}
	}
	else
	{
		*m_errString = string("发送时间推进请求失败：")+mydds->getLastError();
		return false;
	}
	return true;
}
bool TimeMgr::hasTimeAdvanced(double& t)
{
	list<int> inds;
	TimeAdvanced d[50];
	ddslib* mydds = (ddslib*)ddslib::GetInstancePtr();
	int ret = mydds->recvDatas(inds, (char*)&d, 50, m_timeMgrData->m_timeadvanced_recvhd);
	if(ret>0)
	{
		list<int>::iterator it = inds.begin();
		int ind = 0;
		while(it!= inds.end())
		{
			if(*it == m_timeMgrData->m_fedInd)
			{
				t = d[ind].ftime;
				return true;
			}
			it++;
			ind++;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
TimeMgrServer::TimeMgrServer(void):m_timeMgrData(new TimeMgrSvrData),m_errString(new std::string)
{
}


TimeMgrServer::~TimeMgrServer(void)
{
	delete m_timeMgrData;
	delete m_errString;
}
TimeMgrServer & TimeMgrServer::GetInstance() 
{  
	static TimeMgrServer instance;    
	return instance;  
}  
TimeMgrServer* TimeMgrServer::GetInstancePtr() 
{   
	return &GetInstance();  
}
bool TimeMgrServer::initSvr(TimeMgType typ,bool autoStart)
{
	g_fedindex = 1;
	m_timeMgrData->m_tmgTyp = typ;
	m_timeMgrData->m_curTime =0;
	m_hasPause = !autoStart;
	ddslib* mydds = (ddslib*)ddslib::GetInstancePtr();
	if(typ == TMG_Negotiate)
	{
		m_timeMgrData->m_registerfederate_recvhd = mydds->subscribe(sizeof(RegisterFederate),g_registerfederate);
		if(m_timeMgrData->m_registerfederate_recvhd == ErrorHandle)
		{
			*m_errString =string("subscribe RegisterFederate:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_requesttimeadvance_recvhd = mydds->subscribe(sizeof(RequestTimeAdvanceData),g_requesttimeadvance);
		if(m_timeMgrData->m_requesttimeadvance_recvhd == ErrorHandle)
		{
			*m_errString = string("subscribe RequestTimeAdvance:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_registerfederatereturn_sendhd = mydds->publish(sizeof(RegisterFederateReturn),g_registerfederatereturn);
		if(m_timeMgrData->m_registerfederatereturn_sendhd == ErrorHandle)
		{
			*m_errString = string("publish RegisterFederateReturn:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_requesttimeadvancereturn_sendhd = mydds->publish(sizeof(RequestTimeAdvanceReturn),g_requesttimeadvancereturn);
		if(m_timeMgrData->m_requesttimeadvancereturn_sendhd == ErrorHandle)
		{
			*m_errString = string("publish RequestTimeAdvanceReturn:") + mydds->getLastError();
			return false;
		}
	}
	else
	{
		m_timeMgrData->m_registerstep_recvhd = mydds->subscribe(sizeof(RegisterStep),g_registerstep);
		if(m_timeMgrData->m_registerstep_recvhd == ErrorHandle)
		{
			*m_errString =string("subscribe RegisterStep:") + mydds->getLastError();
			return false;
		}
		m_timeMgrData->m_registerstepreturn_sendhd = mydds->publish(sizeof(RegisterStepReturn),g_registerstepreturn);
		if(m_timeMgrData->m_registerstepreturn_sendhd == ErrorHandle)
		{
			*m_errString = string("publish RegisterStepReturn:") + mydds->getLastError();
			return false;
		}
	}
	m_timeMgrData->m_timeadvanced_sendhd = mydds->publish(sizeof(TimeAdvanced),g_timeadvanced);
	if(m_timeMgrData->m_timeadvanced_sendhd == ErrorHandle)
	{
		*m_errString = string("publish TimeAdvanced:") + mydds->getLastError();
		return false;
	}

	return true;
}
//void TimeMgrServer::setTimeSteps(const list<int>& steps)
//{
//	if(m_timeMgrData->m_tmgTyp != TMG_Notice)
//		return;
//	m_timeMgrData->m_steps.clear();
//	list<int>::const_iterator nit = steps.begin();
//	while(nit!=steps.end())
//	{
//		m_timeMgrData->m_steps.push_back(*nit);
//		nit++;
//	}
//}
double TimeMgrServer::currentTime()
{
	return (double)m_timeMgrData->m_curTime/MILLION;
}
void TimeMgrServer::updateDDSProduct(unsigned int sysid,unsigned int inst_state)
{
	if(inst_state&DDS::NOT_ALIVE_DISPOSED_INSTANCE_STATE)
	{
		if(m_timeMgrData->m_tmgTyp==TMG_Negotiate)
		{
			WaitForSingleObject(g_mutexFedList,INFINITE);
			list<ItemTm>::iterator it = m_timeMgrData->m_feds.begin();
			while(it!=m_timeMgrData->m_feds.end())
			{
				if( sysid == it->SystemId )
				{
					m_timeMgrData->m_feds.erase(it);
					printf("成员（systemid:%d）退出！\n",sysid);
					break;
				}
				it++;
			}
			ReleaseMutex(g_mutexFedList);
		}
		else
		{
			map<unsigned int,StepData>::iterator it = m_timeMgrData->m_steps.begin();
			while(it!=m_timeMgrData->m_steps.end())
			{
				list<unsigned int>::iterator it2 = find(it->second.systemids.begin(), it->second.systemids.end(), sysid);
				if(it2!=it->second.systemids.end())
				{
					it->second.systemids.erase(it2);
					if(it->second.systemids.size()==0)
					{
						WaitForSingleObject(g_mutexFedList,INFINITE);
						list<TmStep>::iterator ittm = m_timeMgrData->m_tmpTimes.begin();
						while(ittm!=m_timeMgrData->m_tmpTimes.end())
						{
							if(ittm->ind==it->first)
							{
								m_timeMgrData->m_tmpTimes.erase(ittm);
								break;
							}
							ittm++;
						}
						m_timeMgrData->m_steps.erase(it);
						ReleaseMutex(g_mutexFedList);
					}
					printf("成员（systemid:%d）退出！\n",sysid);
					break;
				}
				it++;
			}
		}
	}
}
void getDDSProduct(DDSParticpantProduct& product)
{
	TimeMgrServer::GetInstancePtr()->updateDDSProduct(product.systemid,product.instance_states);
}
void TimeMgrServer::startSerrvice()
{
	if(!m_isRun)
	{
		m_isRun = true;
		DWORD	dwThreadId;
		printf("start server......\n");
		DDSMgr::GetInstancePtr()->setCallBack(DDSMgr::CBK_Particpant,getDDSProduct);
		DDSMgr::GetInstancePtr()->start();//启动成员监控服务
		Sleep(1000);
		//if(m_timeMgrData->m_tmgTyp==TMG_Negotiate)
		m_timeMgrData->m_hdRecvDara = CreateThread(NULL,0,startRecvData,NULL,0,&dwThreadId);
		m_timeMgrData->m_hdTmAdv    = CreateThread(NULL,0,startTimeAdvance,NULL,0,&dwThreadId);
		printf("server has started!  (Type:%s)\n",m_timeMgrData->m_tmgTyp==TMG_Notice?"Notice":"Negotiate");
	}
}
void TimeMgrServer::stopSerrvice()
{
	m_isRun = false;
	DDSMgr::GetInstancePtr()->stop();
	if(m_timeMgrData->m_tmgTyp==TMG_Negotiate)
	{
		if(WaitForSingleObject(m_timeMgrData->m_hdRecvDara,5000)!=WAIT_OBJECT_0)
		{
			if (m_timeMgrData->m_hdRecvDara)
			{
				TerminateThread(m_timeMgrData->m_hdRecvDara,0);
				//// Close all thread handles upon completion.
			}
		}
		CloseHandle(m_timeMgrData->m_hdRecvDara);
	}

	if(WaitForSingleObject(m_timeMgrData->m_hdTmAdv,5000)!=WAIT_OBJECT_0)
	{
		if (m_timeMgrData->m_hdTmAdv)
		{
			TerminateThread(m_timeMgrData->m_hdTmAdv,0);
			//// Close all thread handles upon completion.
		}
	}
	CloseHandle(m_timeMgrData->m_hdTmAdv);
	printf("server is stoped!\n");
}
bool TimeMgrServer::isRunning()
{
	return m_isRun;
}
DWORD WINAPI TimeMgrServer::reciveData()
{
	list<int> inds;
	RegisterFederate d[10];
	RequestTimeAdvanceData rtad[10];
	RegisterStep rsd[10];
	unsigned int regret = 0;
	int nind = 0;
	unsigned int reqret;
	dds_wait* ddswait = g_mydds->createWait();
	if(m_timeMgrData->m_tmgTyp == TMG_Notice)
		ddswait->addWait(g_registerstep);
	else if(m_timeMgrData->m_tmgTyp == TMG_Negotiate)
	{
		ddswait->addWait(g_registerfederate);
		ddswait->addWait(g_requesttimeadvance);
	}
	while(m_isRun)
	{
		inds.clear();
		if(m_timeMgrData->m_tmgTyp == TMG_Notice)
		{
			ddswait->wait();
			int ret = g_mydds->recvDatas(inds, (char*)&rsd, 10, m_timeMgrData->m_registerstep_recvhd);
			regret = 0;
			nind = 0;
			if(ret>0)
			{
				for (int i=0; i<ret; i++)
				{
					map<unsigned int,StepData>::iterator it = m_timeMgrData->m_steps.begin();
					while(it!=m_timeMgrData->m_steps.end())
					{
						if(it->second.step == rsd[i].nStep)
						{
							nind = it->first;
							it->second.systemids.push_back(rsd[i].systemid);
							break;
						}
						it++;
					}
					if(it==m_timeMgrData->m_steps.end())
					{
						nind = g_fedindex;
						m_timeMgrData->m_steps[nind]=StepData(rsd[i].systemid,rsd[i].nStep);
						g_fedindex++;
						WaitForSingleObject(g_mutexFedList,INFINITE);
						unsigned int ntm = m_timeMgrData->m_curTime+rsd[i].nStep*1000;
						list<TmStep>::iterator ittm = m_timeMgrData->m_tmpTimes.begin();
						while(ittm!=m_timeMgrData->m_tmpTimes.end())
						{
							if(ittm->ntime > ntm)
							{
								m_timeMgrData->m_tmpTimes.insert(ittm,TmStep(nind,ntm));
								break;
							}
							ittm++;
						}
						if(ittm == m_timeMgrData->m_tmpTimes.end())
							m_timeMgrData->m_tmpTimes.push_back(TmStep(nind,ntm));
						ReleaseMutex(g_mutexFedList);
						//SetEvent(g_evtHasAdvReq);//唤醒timeAdvance
					}
					printf("成员(SystemID:%d)登记!\n",rsd[i].systemid);
					m_timeMgrData->m_lsttmp.push_back(regnode(rsd[i].systemid,rsd[i].nStep/1000.0,nind));
					RegisterStepReturn data;
					data.systemid = rsd[i].systemid;
					data.nHandle = nind;
					data.nRet = regret;
					if(!g_mydds->sendData(nind,m_timeMgrData->m_registerstepreturn_sendhd, (char*)&data))
					{
						printf("发送时间句柄登记返回指令失败：%s\n",g_mydds->getLastError());
					}
				}
			}
			else if(ret<0)
				printf("读取时间句柄登记指令失败：%s\n",g_mydds->getLastError());
		}
		else
		{
			ddswait->wait();
			int ret = g_mydds->recvDatas(inds, (char*)&d, 10, m_timeMgrData->m_registerfederate_recvhd);
			regret = 0;
			nind = 0;
			if(ret>0)
			{
				for (int i=0; i<ret; i++)
				{
					unsigned int sysid = d[i].systemid;
					WaitForSingleObject(g_mutexFedList,INFINITE);
					list<ItemTm>::iterator it = m_timeMgrData->m_feds.begin();
					while(it!=m_timeMgrData->m_feds.end())
					{
						if( sysid==it->SystemId )
						{
							regret = 1;//重复注册
							break;
						}
						it++;
					}
					if(it==m_timeMgrData->m_feds.end())
					{
						ItemTm fedit;
						fedit.SystemId = sysid;
						fedit.ftime = (double)m_timeMgrData->m_curTime/MILLION;
						fedit.index = g_fedindex;
						fedit.hasrequest =false;
						nind = g_fedindex;
						m_timeMgrData->m_feds.push_back(fedit);
						printf("成员(SystemID:%d)注册!\n",sysid);
						m_timeMgrData->m_lsttmp.push_back(regnode(sysid,fedit.ftime,nind));
						g_fedindex++;
						regret = 0;//成功
					}
					ReleaseMutex(g_mutexFedList);
					RegisterFederateReturn data;
					data.systemid = sysid;
					data.nRet = regret;
					data.ftime = (double)m_timeMgrData->m_curTime/MILLION;
					if(!g_mydds->sendData(nind,m_timeMgrData->m_registerfederatereturn_sendhd, (char*)&data))
					{
						printf("发送注册返回指令失败：%s\n",g_mydds->getLastError());
					}
				}
			}
			else if(ret<0)
				printf("读取注册指令失败：%s\n",g_mydds->getLastError());
			//////////////////////////////////////////////////////////////
			inds.clear();

			ddswait->wait();
			ret = g_mydds->recvDatas(inds, (char*)&rtad, 10, m_timeMgrData->m_requesttimeadvance_recvhd);
			reqret = 0;
			nind = 0;
			if(ret>0)
			{
				int i=0;
				list<int>::iterator idit = inds.begin();
				while(idit!=inds.end())
				{
					nind = *idit;
					if(rtad[i].ftime*MILLION<m_timeMgrData->m_curTime)
						reqret = 1;//时间已过
					else
					{
						WaitForSingleObject(g_mutexFedList,INFINITE);
						list<ItemTm>::iterator it = m_timeMgrData->m_feds.begin();
						while(it!=m_timeMgrData->m_feds.end())
						{
							if( *idit==it->index )
							{
								reqret = 0;//成功
								it->hasrequest = true;
								it->ftime = rtad[i].ftime;
								m_timeMgrData->m_feds.sort();
								SetEvent(g_evtHasAdvReq);//唤醒timeAdvance
								break;
							}
							it++;
						}
						if(it==m_timeMgrData->m_feds.end())
							reqret = 2;//未注册
						ReleaseMutex(g_mutexFedList);
					}
					RequestTimeAdvanceReturn data;
					data.nRet = reqret;
					if(!g_mydds->sendData(nind,m_timeMgrData->m_requesttimeadvancereturn_sendhd, (char*)&data))
					{
						printf("发送requesttimeadvancereturn指令失败：%s\n",g_mydds->getLastError());
					}
					i++;
					idit++;
				}
			}
			else if(ret<0)
				printf("读取RequestTimeAdvance指令失败：%s\n",g_mydds->getLastError());
		}
	}
	g_mydds->releaseWait(ddswait);
	return 0;
}
void TimeMgrServer::start()
{
	m_hasPause = false;
}
void TimeMgrServer::pause()
{
	m_hasPause = true;
}
void TimeMgrServer::restart()
{
	m_hasPause = true;
	m_brestart = true;
}
DWORD WINAPI TimeMgrServer::timeAdvance()
{
	m_hasPause = true;
	TimeAdvanced tmadv;
	if(m_timeMgrData->m_tmgTyp==TMG_Notice)
	{
		//////////////////////////////////////////////////////////////////////////
		MicroSecondTimer microTmer;
		TmStep tmpt;
		list<TmStep>::iterator it;
		TmStep t;
		m_brestart = true;
		while(m_isRun)
		{
			if(m_brestart)
			{
				microTmer.setBeginTm();
				m_timeMgrData->m_curTime = 0;
				WaitForSingleObject(g_mutexFedList,INFINITE);
				map<unsigned int,StepData>::iterator itsp = m_timeMgrData->m_steps.begin();
				while(itsp!=m_timeMgrData->m_steps.end())
				{
					tmpt.ind  = itsp->first;
					tmpt.ntime = itsp->second.step*1000;
					m_timeMgrData->m_tmpTimes.push_back(tmpt);
					itsp++;
				}
				m_timeMgrData->m_tmpTimes.sort();
				ReleaseMutex(g_mutexFedList);
				m_brestart = false;
			}
			if(m_hasPause)
			{
				Sleep(5);
				continue;
			}

			WaitForSingleObject(g_mutexFedList,INFINITE);
			it = m_timeMgrData->m_tmpTimes.begin();
			if(it == m_timeMgrData->m_tmpTimes.end())
			{//没有成员登记按1S推进
				m_timeMgrData->m_curTime += 1000000;
				microTmer.SleepTo(m_timeMgrData->m_curTime);
				ReleaseMutex(g_mutexFedList);
				continue;
			}
			m_timeMgrData->m_curTime = it->ntime;
			microTmer.SleepTo(m_timeMgrData->m_curTime);
			//send
			tmadv.ftime = m_timeMgrData->m_curTime/MILLION;
			if(!g_mydds->sendData(it->ind,m_timeMgrData->m_timeadvanced_sendhd,&tmadv))
				printf("send TimeAdvance error:%s\n", g_mydds->getLastError());
			//else
			//	printf("send TimeAdvance:id=%d,time=%f\n",it->ind,tmadv.ftime);
			
			t.ind = it->ind;
			t.ntime = m_timeMgrData->m_curTime+m_timeMgrData->m_steps[t.ind].step*1000;
			it++;
			while(it!=m_timeMgrData->m_tmpTimes.end())
			{
				if(it->ntime > t.ntime)
				{
					m_timeMgrData->m_tmpTimes.insert(it,t);
					break;
				}
				it++;
			}
			if(it == m_timeMgrData->m_tmpTimes.end())
				m_timeMgrData->m_tmpTimes.push_back(t);
			m_timeMgrData->m_tmpTimes.pop_front();
			ReleaseMutex(g_mutexFedList);
		}
	}
	else
	{
		bool canTmAdv;
		while(m_isRun)
		{
			if(m_brestart)
			{
				m_timeMgrData->m_curTime = 0;
				//通知所有已注册成员时间归0
				WaitForSingleObject(g_mutexFedList,INFINITE);
				list<ItemTm>::iterator it0= m_timeMgrData->m_feds.begin();
				tmadv.ftime = 0;
				while(it0!= m_timeMgrData->m_feds.end())
				{
					it0->hasrequest = false;
					if(!g_mydds->sendData(it0->index,m_timeMgrData->m_timeadvanced_sendhd,&tmadv))
						printf("send TimeAdvance error:%s\n", g_mydds->getLastError());
					it0++;
				}
				ReleaseMutex(g_mutexFedList);
				//
				m_brestart = false;
			}
			if(m_hasPause)
			{
				Sleep(5);
				continue;
			}

			WaitForSingleObject(g_mutexFedList,INFINITE);
			ResetEvent(g_evtHasAdvReq);
			canTmAdv = m_timeMgrData->m_feds.size()>0;
			list<ItemTm>::iterator it= m_timeMgrData->m_feds.begin();
			while(it!=m_timeMgrData->m_feds.end())
			{
				if(!it->hasrequest)
				{
					canTmAdv = false;
					break;
				}
				it++;
			}
			if(canTmAdv)
			{
				it= m_timeMgrData->m_feds.begin();
				m_timeMgrData->m_curTime = (unsigned long)(it->ftime*MILLION);
				tmadv.ftime = it->ftime;
				it->hasrequest = false;
				ReleaseMutex(g_mutexFedList);
				if(!g_mydds->sendData(it->index,m_timeMgrData->m_timeadvanced_sendhd,&tmadv))
					printf("send TimeAdvance error:%s\n", g_mydds->getLastError());
			}
			else
			{
				ReleaseMutex(g_mutexFedList);
				WaitForSingleObject(g_evtHasAdvReq,INFINITE);
			}
		}
	}
	return 0;
}
list<regnode> TimeMgrServer::getUpdateNode()
{
	list<regnode> lst;
	while(m_timeMgrData->m_lsttmp.size()>0)
	{
		lst.push_back(m_timeMgrData->m_lsttmp.front());
		m_timeMgrData->m_lsttmp.pop_front();
	}
	return lst;
}
//////////////////////////////////////////////////////////////////////////

TimeMgrServer* g_timeMgr = TimeMgrServer::GetInstancePtr();
DWORD WINAPI startRecvData(LPVOID lpThreadParameter)
{
	return g_timeMgr->reciveData();
}
DWORD WINAPI startTimeAdvance(LPVOID lpThreadParameter)
{
	return g_timeMgr->timeAdvance();
}