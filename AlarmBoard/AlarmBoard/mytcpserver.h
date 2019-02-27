#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QLocalServer>

class MyTcpServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = 0);//flag=1，更新客户端

    void startServer();
    void stopServer();
signals:

protected:

    //处理数据到来
    void incomingConnection(quintptr socketDescriptor);

private slots:

    //错误发生事件
    void onErrorHappen(int error);

    //线程完成事件
    void onThreadFinished();

    //线程socket连接上事件
    void onClientConnected(const QString& ip);

    //线程socket断开连接事件
    void onClientSocketDisconnected(const QString& ip);

    //其他客户端到来事件
    void onOtherClientConnected(QLocalSocket* pSocket,const QString& msg);
private:
//    //发送在线信息
//    void sendOnLineMessage(const qint16& flag);
	bool m_hasFinishUpdate;
signals:
	void svrstop();
	void setShowLog(int hand,bool isShow);
};

#endif // MYTCPSERVER_H
