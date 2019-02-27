#include <QtNetwork>
#include "cmdTCPThread.h"
#include "NetCmdDef.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"
cmdTCPThread::cmdTCPThread(ushort port, QObject *parent)
: QObject(parent), m_port(port), blockSize(-1)
{
	QObject::connect(&tcpSocket, &QTcpSocket::readyRead, this, &cmdTCPThread::onSocketReadyRead);
}

cmdTCPThread::~cmdTCPThread()
{
}
void cmdTCPThread::connectToHost(quint32 ip)
{
	m_ip = ip;
	g_log(QString(tr("connect Client:(%1,%2)")).arg(m_ip).arg(m_port), Log_Debug);
	if (tcpSocket.state() != QAbstractSocket::ConnectedState)
	{
		tcpSocket.connectToHost(QHostAddress(m_ip), m_port);
		if (!tcpSocket.waitForConnected(3000))
		{
			g_log(QString(tr("connect Client:(%1,%2) faild --%2")).arg(m_ip).arg(m_port).arg(tcpSocket.errorString()), Log_Warning);
			return;
		}
	}
	//����������ʱ
	g_log(QString(tr("start heart beat for server.(%1,%2)")).arg(m_ip).arg(m_port), Log_Debug);
	if (getRTDBConfig()&rtdb_heartbeat)
	{
		m_hraetMaxTimer.setInterval(HEARTBEAT_MAXTIME);
		m_hraetTimer.setInterval(HEARTBEAT_TIME);
		m_hraetTimer.setSingleShot(false);
		m_hraetTimer.start();
		QObject::connect(&m_hraetMaxTimer, &QTimer::timeout, this, [this](){
			m_hraetTimer.stop();
			disConnect();
			emit timeOut(m_ip);
		});
		QObject::connect(&m_hraetTimer, &QTimer::timeout, this, [this](){sendCmd(DATATRANS_HEARTBEAT); });
	}
	sendCmd(DATATRANS_CONNECT);
	g_log(QString(tr("send DATATRANS_CONNECT to Client:(%1,%2)")).arg(m_ip).arg(m_port), Log_Debug);
}

void cmdTCPThread::sendCmd(qint16 cmd)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << qint16(cmd);// << fname.toLocal8Bit();//(fileName.toAscii());
	//��������
	qint64 size = block.size();
	tcpSocket.write((char*)&size, sizeof(qint64));//�ȷ���block�Ĵ�С
	tcpSocket.write(block.data(), size);//�ٷ���block���������
	g_log(Log_Debug, "����ָ��:%d", cmd);
	if (!tcpSocket.waitForBytesWritten())
	{
		g_log(QString(tr("send cmd %1 failed :%2 ").arg(cmd).arg(tcpSocket.errorString())),Log_Error);
		return ;
	}
}
void cmdTCPThread::sendDbMD5()//dbname\MD5
{
	//QString strmd5 = rtdb_get_fieldnames()
	const char* dbset = getDefaultDbSets();
	const char* path = getBasePath();
	RTDbSets* pset = getRtDbDef(dbset);
	if (pset == nullptr)
	{
		readRTDbSetsDef(path, dbset);
		pset = getRtDbDef(dbset);
	}
	if (pset == nullptr)
		return;

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << qint16(DATATRANS_CHECKMD5);
	out << QString(dbset) << QString(path) << QString::fromStdString(pset->md5);
	//��������
	qint64 size = block.size();
	tcpSocket.write((char*)&size, sizeof(qint64));//�ȷ���block�Ĵ�С
	tcpSocket.write(block.data(), size);//�ٷ���block���������
	g_log(Log_Debug, "�������ݼ�MD5");
	if (!tcpSocket.waitForBytesWritten())
	{
		g_log(QString(tr("���� ���ݼ�MD5 failed :%1 ").arg(tcpSocket.errorString())), Log_Error);
		return;
	}
}
void cmdTCPThread::sendDb(QString dbname, int dbsize, QByteArray& dbdata)
{

}
void cmdTCPThread::sendDbSets(int dbsize, QByteArray& dbdata)
{

}
void cmdTCPThread::disConnect()
{
	if (tcpSocket.state() == QAbstractSocket::ConnectedState)
	{
		tcpSocket.disconnectFromHost();
		if (!tcpSocket.waitForDisconnected(3000))
		{
			g_log(QString(tr("disconnect fail:%1 ").arg(tcpSocket.errorString())));
		}
	}
}
void cmdTCPThread::onSocketReadyRead()
{
	while (tcpSocket.bytesAvailable() >= sizeof(qint64) || this->blockSize > 0)
	{
		if (this->blockSize == 0)
		{
			if (tcpSocket.bytesAvailable() < sizeof(qint64))
			{
				return;
			}
			tcpSocket.read((char*)&this->blockSize, sizeof(qint64));  // read blockSize
		}

		if (tcpSocket.bytesAvailable() < this->blockSize)				// have no enugh data
			return;
		QByteArray data = tcpSocket.read(this->blockSize);
		this->processData(data);
		this->blockSize = 0;
	}
}
//�����ȡ������
void cmdTCPThread::processData(QByteArray& array)
{
	QDataStream in(&array, QIODevice::ReadOnly);
	in.setVersion(QDataStream::Qt_4_5);

	quint16 key;
	QByteArray data;
	in >> key >> data;

	switch (key)
	{
	case DATATRANS_RTDBSYNC:
	{
		//����dbsets���ƺ�·��	
		g_log(QString("�յ�ͬ��RTDB����(DATATRANS_RTDBSYNC)ָ�(%1)").arg(tcpSocket.peerAddress().toString()), Log_Debug);
		sendDbMD5();//����MD5У��
	}
		break;
	case DATATRANS_DBDEFSYNCREQ://ͬ�����ݶ������� 6
	{
		g_log(QString("�յ�ͬ�����ݶ�������(DATATRANS_DBDEFSYNCREQ)ָ�(%1)").arg(tcpSocket.peerAddress().toString()), Log_Debug);
		//�������ݿⶨ���ļ���
		const char* dbrootpath = getBasePath();//Ҫ���͵��ļ���
		QDir dir(QCoreApplication::applicationDirPath());
		//db�����·��//���շ�����ļ��У����·��
		QString relPath = dir.relativeFilePath(dbrootpath);
		//����������Ŀ¼
		g_log(QString("����Ŀ¼:(ip=%1,path=%2)").arg(tcpSocket.peerAddress().toString()).arg(dbrootpath), Log_Debug);
		m_senfile.sendPath(tcpSocket.peerAddress().toString(), dbrootpath, relPath);
		//�ȴ��������
		m_senfile.waitForSendFinish();
		//m_cmdSock.sendCmd(nIp, RTDB_ENDSYNCRTDB);//����ͬ��RTDB���ָ��
		g_log(QString("����ͬ��RTDB���(DATATRANS_DBDEFSYNCFINISH)ָ�(%1)").arg(tcpSocket.peerAddress().toString()), Log_Debug);
		sendCmd(DATATRANS_DBDEFSYNCFINISH);// 6
		m_senfile.stopSend();
	}
		break;
	case DATATRANS_DBDATASYNCREQ: //RTDB Dataͬ������ 7
	{
		setRTDBState(getRTDBState() | rtdb_AlreadySync);

		//2.����db
		const char* dbset = getDefaultDbSets();
		m_dbsend.sendDbSet(dbset, tcpSocket.peerAddress().toIPv4Address());
		m_dbsend.waitFinish();
		//������ɣ����͡�RTDB Dataͬ����ɡ�ָ��	8
		sendCmd(DATATRANS_DBDATASYNCFINISH);// 8
		setRTDBState(getRTDBState() & (~rtdb_AlreadySync));
	}
		break;
	case DATATRANS_HEARTBEAT://
	{
		m_hraetMaxTimer.start();//���¼�ʱ
	}
		break;
	default:
		break;
	}
}