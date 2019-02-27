/*!
 * \file udpSock.h
 *
 * \author YNTech
 * \date ʮһ�� 2018
 * 1��server����
 * 2��node ָ����ռ���
 * ���ҷ�����������������Ӧ��֪ͨ�ͻ��˷�������
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
	void brodcastFindSvr();//�㲥���ҷ�����
	//void sendRequestSyncRTDB();//����ͬ��RTDB
	//void endSyncRTDB();
	void brodcastSvrNodeId();//�㲥������Id
private slots:
	void processPendingDatagrams();
	void processPendingDatagramsRemot();
private:
	
	void dealData(QByteArray& d, const QHostAddress& addr, const quint16& nPort);
	QUdpSocket *udpSocket;
	QUdpSocket *udpSvrSock;
	//QList<myHost> m_lstClient;
	quint16 m_svrPort;//svr�˿�
	quint16 m_recvCmdPort;//����ָ��Ķ˿�

	//QString m_localAddress;
signals:
	void dealLnkSvrResq(quint32 nIp);//������������
	void endSyncRTDB();
	void changeServer();
};

#endif // __UDPSOCK_H__