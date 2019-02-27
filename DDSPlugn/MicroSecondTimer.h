#ifndef __MICROSECONDTIMER_H__
#define __MICROSECONDTIMER_H__

#include <windows.h>
#include <list>
using namespace std;
#define MILLION 1000000.0
class MicroSecondTimer
{
public:
	MicroSecondTimer(void);
	~MicroSecondTimer(void);
	void MicroSecondSleep(unsigned long delayTime);//Windows微秒级的Sleep函数
	void setBeginTm();
	unsigned long getMicroSecToCueernt();
	void SleepTo(unsigned long time);//开始计时到输入time的微秒数，不到time则Sleep，单位：us
private:

	LARGE_INTEGER Freq;
	LARGE_INTEGER m_tcStart;
};
extern MicroSecondTimer g_microSecondTimer;
#endif//__MICROSECONDTIMER_H__