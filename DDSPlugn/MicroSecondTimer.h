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
	void MicroSecondSleep(unsigned long delayTime);//Windows΢�뼶��Sleep����
	void setBeginTm();
	unsigned long getMicroSecToCueernt();
	void SleepTo(unsigned long time);//��ʼ��ʱ������time��΢����������time��Sleep����λ��us
private:

	LARGE_INTEGER Freq;
	LARGE_INTEGER m_tcStart;
};
extern MicroSecondTimer g_microSecondTimer;
#endif//__MICROSECONDTIMER_H__