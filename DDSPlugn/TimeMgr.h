#ifndef __TIMEMGR_H__
#define __TIMEMGR_H__
/********************************************************************
	created:	2014/12/29
	created:	29:12:2014   16:57
	filename: 	d:\test\ddslib_src\ddslib\TimeMgr.cpp
	file path:	d:\test\ddslib_src\ddslib
	file base:	TimeMgr
	file ext:	cpp
	author:		MYS
	
	purpose:	DDS时间管理器，支持两种模式时间管理。1）通知方式时间管理：由服务器发送时间通知指令，其他成员以接收到的时间通知指令计时；
	2）协商方式步进时间管理：仿真节点发送时间推进请求，由服务器决定时间推进，以时间推进通知指令通知仿真节点
	时间管理用法：
	1）通知方式：
	服务器端：
			a)初始化（initSvr）
			b)启动时间服务startSerrvice();
	仿真节点：
			a)初始化（initMember）
			b)登记仿真时间步长（registerTimeStep）

			c)循环查询时间推进通知指令、收发数据。(hasTimeAdvanced)
	2）协商方式步进时间管理：
	服务器端：
			a)初始化（initSvr）
			b)启动时间服务startSerrvice();
	仿真节点：
			a)初始化（initMember）
			b)注册邦员并获取当前仿真时间 （registerFederate）

			c)发送时间推进请求(RequestTimeAdvance)
			d)循环查询时间推进通知指令、收发数据。(hasTimeAdvanced)
			e)重复c)、d)
*********************************************************************/
#include <list>
#include "ddalib_export.h"
using namespace std;

class TimeMgrData;
class TimeMgrSvrData;
enum TimeMgType{ TMG_Notice,TMG_Negotiate };
//仿真节点服务接口
class _DDSLIB_API TimeMgr
{
public:
	static TimeMgr & GetInstance();
	static TimeMgr* GetInstancePtr();

//初始化  
	///仿真节点初始化，此函数只调用一次
	bool initMember(TimeMgType typ);//typ必须与服务端设置一致
	//************************************
	// Method:    节点注册，（此函数只调用一次），TMG_Notice时不需调用此函数。
	// 参数：     ftime为返回注册时仿真时间。
	// 返回值:    为操作结果,当返回false时，调用getLastError可获取错误信息。
	//************************************
	bool registerFederate(double &ftime);
	//************************************
	// Method:    登记仿真时间步长，当TMG_Notice类型时使用
	//            当服务器未启动时返回超时信息
	//************************************
	bool registerTimeStep(unsigned int step);
//
	///时间推进请求，TMG_Negotiate时使用
	bool RequestTimeAdvance(double t);///<请求时间推进到t,t必须大于等于当前时间,返回请求结果。(协商步进方式有效)
	///查询时间推进通知指令
	bool hasTimeAdvanced(double& t);///<查询时间推进通知指令,返回值：true:接收到通知指令，时间推进到t；false：未收到通知指令，请求推进的时间未到达
//
	inline string getLastError(){ return *m_errString;}
	double currentTime();///<当前时间
protected:
	TimeMgr(void);
	~TimeMgr(void);
private:
	string *m_errString;
	TimeMgrData *m_timeMgrData;
};
struct regnode
{
	regnode(unsigned int nd,double st,unsigned int hd){sysid=nd;step=st;hand=hd;}
	unsigned int sysid;
	double step;
	unsigned int hand;
};
//服务器控制服务接口
class _DDSLIB_API TimeMgrServer
{
public:
	static TimeMgrServer & GetInstance();
	static TimeMgrServer* GetInstancePtr();
//	
	bool initSvr(TimeMgType typ,bool autoStart = true);///<服务器初始化，typ：服务运行类型，autoStart = true开始服务后自动开始时间推进
//
	void startSerrvice();//启动服务，接受注册、登记
	void stopSerrvice();//终止时间服务
	bool isRunning();//判断服务是否已启动
	void start();//开始时间推进
	void pause();//暂停时间推进
	void restart(); /*仿真时间从零重新开始,
					//TMG_Negotiate：通知所有已注册成员时间推进到0，即调用hasTimeAdvanced可获取t=0;
				    // TMG_Notice:所有已登记成员会从0时刻开始推进。
					*/
//
	inline string getLastError(){ return *m_errString;}
	double currentTime();///<当前时间,S
	list<regnode> getUpdateNode();

	/////////////////////////////////////////////////
	//内部调用
	DWORD WINAPI reciveData();
	DWORD WINAPI timeAdvance();
	void updateDDSProduct(unsigned int sysid,unsigned int inst_state);
	//
protected:
	TimeMgrServer(void);
	~TimeMgrServer(void);
private:
	string *m_errString;
	TimeMgrSvrData *m_timeMgrData;
	volatile bool m_isRun;
	volatile bool m_hasPause;
	volatile bool m_brestart;
};
#endif // __TIMEMGR_H__

