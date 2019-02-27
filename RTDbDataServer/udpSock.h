/*!
 * \file udpSock.h
 *
 * \author YNTech
 * \date 十一月 2018
 * 1、server监听
 * 2、node 指令接收监听
 * 查找服务器，服务器查找应答，通知客户端发起连接
 */
#ifndef __UDPSOCK_H__
#define __UDPSOCK_H__
#include <QtCore>
#include <QtNetwork>
//struct myHost 
//{
//	QHostAddress  hostAddress;
//	quint16		  nPort;
//};
class udpSock :public QObject
{
	Q_OBJECT
public:
	udpSock();
	~udpSock();
	void setSvrPort(ushort port);
	void sendCmd(quint32 addr, ushort cmdCode);
	void sendCmd2Svr(ushort cmdCode);
	void startSvr();
	void stopSvr();
	void brodcastFindSvr();//广播查找服务器
	//void sendRequestSyncRTDB();//请求同步RTDB
	//void endSyncRTDB();
	void brodcastSvrNodeId();//广播服务器Id
private slots:
	void processPendingDatagrams();
	void processPendingDatagramsRemot();
private:
	
	void dealData(QByteArray& d, const QHostAddress& addr, const quint16& nPort);
	QUdpSocket *udpSocket;
	QUdpSocket *udpSvrSock;
	//QList<myHost> m_lstClient;
	quint16 m_svrPort;//svr端口
	quint16 m_recvCmdPort;//接收指令的端口

	//QString m_localAddress;
signals:
	void dealLnkSvrResq(quint32 nIp);//处理连接请求
	void endSyncRTDB();
	void changeServer();
};

#endif // __UDPSOCK_H__