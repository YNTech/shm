#include <QtNetwork>
#include "SendDbThread.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
#include "commonfun.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"

SendDbThread::SendDbThread(const char* dbsetname, int addr, unsigned short port) :dbsetname(dbsetname), addr(addr), port(port)
{
}
void SendDbThread::run()
{
	QTcpSocket tcpSocket;
	tcpSocket.connectToHost(addr, port);
	if (!tcpSocket.waitForConnected(3000))
	{
		g_log(QString(QObject::tr("connect ip=%1:%2 faild")).arg(addr.toString()).arg(port));
	}
	RTDbSets *dbsets = getRtDbDef(dbsetname);
	if (dbsets == nullptr)
	{
		char err[1024];
		const char* basepath = getBasePath();
		if (!readRTDbSetsDef(basepath, dbsetname, err))
			g_log(QString("SendDbThread::run ERROR01:") + err, Log_Error);
		dbsets = getRtDbDef(dbsetname);
		if (dbsets == nullptr)
			return;
	}
	int bg = 0;
	int len;
	for (auto db : dbsets->rtdbs)
	{
		bg = 0;
		do
		{
			QByteArray ba;
			len = encodeRtdb_Db(ba, db.name.c_str(), bg, MAX_PACKET_TCP);
			if (len < 0)
			{
				g_log(QString(QObject::tr("SendDbThread::run 02: error db name '%1'")).arg(dbsetname), Log_Error);
				break;
			}
			bg += len;
			sendData(tcpSocket, ba);
		} while (len > 0);
	}

	//tcpSocket.
	if (tcpSocket.state() == QAbstractSocket::ConnectedState)
		tcpSocket.disconnectFromHost();
}
void SendDbThread::sendData(QTcpSocket& tcpSocket,QByteArray& d)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << qint16(DATATRANS_DBDATA) << d;
	//发送数据
	qint64 size = block.size();
	tcpSocket.write((char*)&size, sizeof(qint64));//先发送block的大小
	tcpSocket.write(block.data(), size);//再发送block里面的数据
	if (!tcpSocket.waitForBytesWritten())
	{
		g_log(QString(QObject::tr("send cmd DATATRANS_DBDATA failed :%1 ").arg(tcpSocket.errorString())), Log_Error);
		return;
	}
}
void SendDbThreadMgr::sendDbSet(const char* dbsetname, int addr, unsigned short port)
{
	m_threadpool.start(new SendDbThread(dbsetname, addr, port));
}
bool SendDbThreadMgr::waitFinish(int msecs)
{
	return m_threadpool.waitForDone(msecs);
}