#include "sendthreadmgr.h"
#include "sendfilethread.h"
#include "windows.h"
#include "NetCmdDef.h"

bool lessThan(sendFileThread *p1, sendFileThread *p2)
{
	return p1->getDataSize() < p2->getDataSize();
}

sendThreadMgr::sendThreadMgr(QObject *parent)
	: QObject(parent)
{
}

sendThreadMgr::~sendThreadMgr()
{
	while(m_lstThreads.size()>0)
	{
		sendFileThread *p=m_lstThreads.takeFirst();
		p->stop();
		Sleep(100);
		while(!p->isFinished())
			Sleep(10);
		delete p;
	}
}
void sendThreadMgr::sendPath(QString hostIp, QString fname, QString path)
{
	QFileInfo f(fname);
	if (f.isFile())
		sendFile(hostIp, fname, path);
	else if (f.isDir())
	{
		QDir d(fname);
		if (!path.isEmpty())
			path += "\\";
		path += d.dirName();
		QStringList fs = d.entryList();
		foreach(QString strF, fs)
		{
			if (strF == "." || strF == "..")
				continue;
			sendPath(hostIp, d.canonicalPath() + "\\" + strF, path);
		}
	}
}
void sendThreadMgr::sendFile(QString strIp,QString fName,QString path)
{
	int threadsize=m_lstThreads.size();
	if(threadsize<MAX_THREADS)
	{
		if(threadsize%10==0)
			Sleep(100);
		sendFileThread *p = new sendFileThread(DATARECVPORT, this);
		connect(p,SIGNAL(runlog(QString,int)),this,SIGNAL(runlog(QString,int)));
		m_lstThreads<<p;
		p->sendFile(strIp,fName,path);
	}
	else
	{
		qSort(m_lstThreads.begin(), m_lstThreads.end(), lessThan);
		m_lstThreads.first()->sendFile(strIp,fName,path);
	}
}
bool sendThreadMgr::waitForSendFinish(int waitt)
{
	bool f=false;
	QTime t1=QTime::currentTime(),t2;
	while(!f)
	{
		f=true;
		for(int i=0;i<m_lstThreads.size();i++)
		{
			if(!m_lstThreads[i]->waitForSendFinish(100))
				f=false;
			t2=QTime::currentTime();
			if(waitt>0 && t1.msecsTo(t2)>waitt)
				return false;
			qApp->processEvents();
		}
	}
	return true;
}
void sendThreadMgr::stopSend()
{
	//for (int i=0;i<m_lstThreads.size();i++)
	//{
	//	m_lstThreads[i]->stop();
	//}
	while (m_lstThreads.size() > 0)
	{
		sendFileThread *p = m_lstThreads.takeFirst();
		p->stop();
		Sleep(100);
		while (!p->isFinished())
			Sleep(10);
		delete p;
	}
}