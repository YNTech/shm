/*!
 * \file mytcpserver.h
 *
 * \author YNTech
 * \date ʮһ�� 2018
 *
 * RTDB���ݿ����
 */
#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>


class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = 0);

    void startServer();
    void stopServer();
	bool waitFinish(int t=-1);
signals:

protected:

    //�������ݵ���
    void incomingConnection(int handle);

private slots:

    //�������¼�
    void onErrorHappen(int error);

    //�߳�����¼�
    void onThreadFinished();

    //�߳�socket�������¼�
    void onClientConnected(const QString& ip);

    //�߳�socket�Ͽ������¼�
    void onClientSocketDisconnected(const QString& ip);

    //�����ͻ��˵����¼�
    void onOtherClientConnected(QTcpSocket* pSocket,
                                const QString& msg);

private:
//    //����������Ϣ
//    void sendOnLineMessage(const qint16& flag);
    //Widget* w;
	//int m_flag;
	//bool m_hasFinishUpdate;
signals:
	void onRecvFile(QString fnm);
	void onEndRecvFile(QString fnm);
	void heartBeatTimeOut();//������ʱ
};

#endif // MYTCPSERVER_H
