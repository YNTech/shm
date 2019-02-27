/*!
 * \file mytcpserver.h
 *
 * \author YNTech
 * \date 十一月 2018
 *
 * RTDB数据库服务
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

    //处理数据到来
    void incomingConnection(int handle);

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
    void onOtherClientConnected(QTcpSocket* pSocket,
                                const QString& msg);

private:
//    //发送在线信息
//    void sendOnLineMessage(const qint16& flag);
    //Widget* w;
	//int m_flag;
	//bool m_hasFinishUpdate;
signals:
	void onRecvFile(QString fnm);
	void onEndRecvFile(QString fnm);
	void heartBeatTimeOut();//心跳超时
};

#endif // MYTCPSERVER_H
