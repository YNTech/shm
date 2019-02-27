#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QLocalServer>

class MyTcpServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = 0);//flag=1�����¿ͻ���

    void startServer();
    void stopServer();
signals:

protected:

    //�������ݵ���
    void incomingConnection(quintptr socketDescriptor);

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
    void onOtherClientConnected(QLocalSocket* pSocket,const QString& msg);
private:
//    //����������Ϣ
//    void sendOnLineMessage(const qint16& flag);
	bool m_hasFinishUpdate;
signals:
	void svrstop();
	void setShowLog(int hand,bool isShow);
};

#endif // MYTCPSERVER_H
