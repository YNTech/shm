#ifndef CALLBACKTHREAD_H
#define CALLBACKTHREAD_H

#include <QRunnable>
typedef void (*pCB)();

class CallBackThread : public QRunnable
{
public:
	CallBackThread(pCB cb);
	~CallBackThread();
	void run();
private:
	pCB m_cb;
};

#endif // CALLBACKTHREAD_H
