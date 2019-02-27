#pragma once

#include <QRunnable>
#include <QHostAddress>
#include <QThreadPool>
#include "NetCmdDef.h"

class SendDbThread : public QRunnable
{
	//Q_OBJECT
public:
	SendDbThread(const char* dbsetname, int addr = 0x7f000001, unsigned short port = DATARECVPORT);
protected:
	virtual void run();
private:
	void sendData(QTcpSocket& tcpSocket, QByteArray& d);
	const char* dbsetname;
	QHostAddress addr;
	unsigned short port;
};
class SendDbThreadMgr
{
public:
	void sendDbSet(const char* dbsetname, int addr = 0x7f000001, unsigned short port = DATARECVPORT);
	bool waitFinish(int msecs = -1);
private:
	QThreadPool m_threadpool;
};
