#include "udpSock.h"
#include "NetCmdDef.h"
#include "rtdb_interface.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
#include "commonFun.h"
udpSock::udpSock() :m_svrPort(CMDSVRPORT), m_recvCmdPort(CMDRECVPORT)
{
	//����ָ��
	udpSocket = new QUdpSocket(this);
	if (!udpSocket->bind(m_recvCmdPort, QUdpSocket::ShareAddress))
		g_log(QString(tr("listen CMD port:%1 faild --%2")).arg(m_recvCmdPort).arg(udpSocket->errorString()), Log_Debug);
	connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	g_log(QString(tr("listen port:%1 ")).arg(m_recvCmdPort), Log_Debug);
}
udpSock::~udpSock()
{

}
void udpSock::setSvrPort(ushort port)
{
	m_svrPort = port;
}
void udpSock::startSvr()
{
	//1.����svr����
	//2.�㲥svr noedId
	udpSvrSock = new QUdpSocket(this);
	if (!udpSvrSock->bind(m_svrPort, QUdpSocket::ShareAddress))
		g_log(QString(tr("listen SVR port:%1 faild --%2")).arg(m_svrPort).arg(udpSvrSock->errorString()), Log_Debug);
	g_log(QString(tr("listen port:%1 ")).arg(m_svrPort), Log_Debug);
	connect(udpSvrSock, SIGNAL(readyRead()), this, SLOT(processPendingDatagramsRemot()));
	brodcastSvrNodeId();

	//3.
}
void udpSock::stopSvr()
{
	udpSvrSock->close();
	delete udpSvrSock;
	udpSvrSock = nullptr;
}
void udpSock::brodcastSvrNodeId()
{//�㲥Svr��NodeId
	QByteArray senddata;
	QDataStream outs(&senddata, QIODevice::WriteOnly);
	outs << ushort(RTDB_BCSVRID) << getCurrentNodeId();
	int ret = udpSocket->writeDatagram(senddata, QHostAddress::Broadcast, m_recvCmdPort);
}
void udpSock::brodcastFindSvr()
{
	//QByteArray senddata;
	//QDataStream outs(&senddata, QIODevice::WriteOnly);
	//outs << RTDB_CALLSVR;
	//sendData(senddata);

	QByteArray senddata;
	QDataStream outs(&senddata, QIODevice::WriteOnly);
	outs << ushort(RTDB_CALLSVR);
	int ret = udpSocket->writeDatagram(senddata, QHostAddress::Broadcast, m_svrPort);
}
//void udpSock::sendRequestSyncRTDB()
//{
//	QByteArray senddata;
//	QDataStream outs(&senddata, QIODevice::WriteOnly);
//	outs << RTDB_REQUESYNCRTDB;
//	udpSocket->writeDatagram(senddata, QHostAddress(getSvrAdress()), getSvrPort());
//}
//void udpSock::endSyncRTDB()
//{
//	QByteArray senddata;
//	QDataStream outs(&senddata, QIODevice::WriteOnly);
//	outs << RTDB_ENDSYNCRTDB;
//	udpSocket->writeDatagram(senddata, QHostAddress(getSvrAdress()), getSvrPort());
//}
void udpSock::processPendingDatagrams()
{
	while (udpSocket->hasPendingDatagrams()) {
		QByteArray datagram;
		QHostAddress  hostAddress;
		quint16		  nPort;
		datagram.resize(udpSocket->pendingDatagramSize());
		udpSocket->readDatagram(datagram.data(), datagram.size(), &hostAddress, &nPort);
		//m_localAddress = udpSocket->localAddress().toString();
		dealData(datagram, hostAddress, nPort);
	}
}
void udpSock::processPendingDatagramsRemot()
{
	while (udpSvrSock->hasPendingDatagrams()) {
		QByteArray datagram;
		QHostAddress  hostAddress;
		quint16		  nPort;
		datagram.resize(udpSvrSock->pendingDatagramSize());
		udpSvrSock->readDatagram(datagram.data(), datagram.size(), &hostAddress, &nPort);
		//m_localAddress = udpSvrSock->localAddress().toString();
		dealData(datagram, hostAddress, nPort);
	}
}
void udpSock::sendCmd2Svr(ushort cmdCode)
{
	QByteArray senddata;
	QDataStream outs(&senddata, QIODevice::WriteOnly);
	outs << cmdCode;
	udpSocket->writeDatagram(senddata, QHostAddress(getSvrAdress()), getSvrPort());
}
void udpSock::sendCmd(quint32 addr, ushort cmdCode)
{
	//QString str = hostAddress.toString();
	QByteArray senddata;
	QDataStream outs(&senddata, QIODevice::WriteOnly);
	outs << cmdCode;
	udpSocket->writeDatagram(senddata, QHostAddress(addr), m_recvCmdPort);
}
void udpSock::dealData(QByteArray& d, const QHostAddress& addr, const quint16& nPort)
{
	QDataStream in(&d, QIODevice::ReadOnly);
	ushort cmd;
	in >> cmd;
	switch (cmd)
	{
	case RTDB_CALLSVR://��ѯsvr
		g_log(Log_Debug, "�յ�����ѯsvr ָ�UDP��");
		brodcastSvrNodeId();
		break;
	case RTDB_BCSVRID://�յ�svrID
	{
		g_log(Log_Debug, "�յ���svrID����ָ�UDP��");
		int svrId;
		in >> svrId;
		int curNodeid = getCurrentNodeId();
		int localSvrId = getSvrNodeId();
		if (svrId != localSvrId)
		{
			if ((localSvrId == curNodeid && svrId<curNodeid) || localSvrId != curNodeid)//����server��������idС����Ч
			{
				g_log(Log_Debug, "���� svrID���л�����������");
				setSvrNode(svrId, addr.toIPv4Address(), nPort);
				//���͡���������
				sendCmd2Svr(RTDB_LNKSVR);
				emit changeServer();//֪ͨ�رձ���server
			}
		}
	}
		break;
	case RTDB_LNKSVR://��������
	{
		emit dealLnkSvrResq(addr.toIPv4Address());
	}
		break;
	//case RTDB_ENDSYNCRTDB://����ͬ��RTDB���ָ��
	//	emit endSyncRTDB();
	//	break;
	default:
		;
	}
}