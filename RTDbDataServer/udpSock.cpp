#include "udpSock.h"
#include "NetCmdDef.h"
#include "rtdb_interface.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
#include "commonFun.h"
udpSock::udpSock() :m_svrPort(CMDSVRPORT), m_recvCmdPort(CMDRECVPORT)
{
	//监听指令
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
	//1.开启svr监听
	//2.广播svr noedId
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
{//广播Svr的NodeId
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
	case RTDB_CALLSVR://查询svr
		g_log(Log_Debug, "收到：查询svr 指令（UDP）");
		brodcastSvrNodeId();
		break;
	case RTDB_BCSVRID://收到svrID
	{
		g_log(Log_Debug, "收到：svrID更新指令（UDP）");
		int svrId;
		in >> svrId;
		int curNodeid = getCurrentNodeId();
		int localSvrId = getSvrNodeId();
		if (svrId != localSvrId)
		{
			if ((localSvrId == curNodeid && svrId<curNodeid) || localSvrId != curNodeid)//出现server竞争，则id小的有效
			{
				g_log(Log_Debug, "更新 svrID，切换服务器……");
				setSvrNode(svrId, addr.toIPv4Address(), nPort);
				//发送“连接请求”
				sendCmd2Svr(RTDB_LNKSVR);
				emit changeServer();//通知关闭本地server
			}
		}
	}
		break;
	case RTDB_LNKSVR://连接请求
	{
		emit dealLnkSvrResq(addr.toIPv4Address());
	}
		break;
	//case RTDB_ENDSYNCRTDB://发送同步RTDB完成指令
	//	emit endSyncRTDB();
	//	break;
	default:
		;
	}
}