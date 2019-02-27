#include "CallBackThread.h"

CallBackThread::CallBackThread(pCB cb)
{
	m_cb = cb;
}

CallBackThread::~CallBackThread()
{

}
void CallBackThread::run()
{
	m_cb();
}