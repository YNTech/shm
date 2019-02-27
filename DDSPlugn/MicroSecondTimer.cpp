#include "MicroSecondTimer.h"

MicroSecondTimer g_microSecondTimer;

MicroSecondTimer::MicroSecondTimer(void)
{
	if (!QueryPerformanceFrequency(&Freq))
	{
		Freq.QuadPart = 0;
	}
}

MicroSecondTimer::~MicroSecondTimer(void)
{
}

void MicroSecondTimer::MicroSecondSleep(unsigned long delayTime)//us
{
	if(Freq.QuadPart==0)
		return Sleep(0);
	LARGE_INTEGER tcStart={0};
	QueryPerformanceCounter(&tcStart);
	LARGE_INTEGER tcEnd={0};
	unsigned long time = 0;
	while(1)
	{
		if( delayTime-time >6000)//大于6ms时用Sleep避免CPU占用过高
			Sleep(4);
		QueryPerformanceCounter(&tcEnd);
		time = ((tcEnd.QuadPart - tcStart.QuadPart)*1000000)/Freq.QuadPart;
		if (time >= delayTime)
			break;
	}
}
void MicroSecondTimer::setBeginTm()
{
	QueryPerformanceCounter(&m_tcStart);
}
unsigned long MicroSecondTimer::getMicroSecToCueernt()
{
	LARGE_INTEGER tcEnd={0};
	QueryPerformanceCounter(&tcEnd);
	return  ((tcEnd.QuadPart - m_tcStart.QuadPart)*1000000)/Freq.QuadPart;
}

void MicroSecondTimer::SleepTo(unsigned long time)
{
	LARGE_INTEGER tcEnd={0};
	QueryPerformanceCounter(&tcEnd);
	long delayTime = time - ((tcEnd.QuadPart - m_tcStart.QuadPart)*1000000)/Freq.QuadPart;

	if(delayTime >10000)//大于10ms时调用Sleep避免CPU占用过高
		Sleep(delayTime/1000-5);

	long tdly = 0;
	while(true)
	{
		QueryPerformanceCounter(&tcEnd);
		tdly = (((tcEnd.QuadPart - m_tcStart.QuadPart)*1000000)/Freq.QuadPart);
		if (tdly >= delayTime)
			break;
		Sleep(0);
	}
}