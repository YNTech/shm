/*!
 * \file socketthread.h
 *
 * \author YNTech
 * \date 十一月 2018
 *
 * TCP数据接收线程（客户端）
 */
#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QAbstractSocket>
#include <QTimer>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = 0,int h=0);

	void sendCmd(qint16 cmd);
    void processData(QByteArray& array);
	//void setFlag(int f){ m_flag=f;}
private:
	void beginRTDBSyncData();//启动 RTDB Data同步
	QTcpSocket mSocket;

    QFile file;
    QString mFileName;

    qint64 blockSize;
    qint64 blockNumber;

    QString mClientIp;
	int m_flag;
	QTimer m_hraetMaxTimer;

	QString m_curDbSetNm;
	QString m_curDbPath;
protected:
    void run();

signals:
    void errorSignal(int);

    //启动窗口

    //断开连接的IP
    void disconnectedIpSignal(const QString& ip);

    //连接上服务器事件
    void onConnectedSignal(const QString& ip);

    //其他客户端连接事件
    void otherClientConnectedSignal(QTcpSocket* pSocket,const QString& msg);

private slots:
    //有数据需要读取
    void onSocketReadyRead();

    //断开连接事件
    void onSocketDisconnected();

    //错误发生事件
    void onErrorHappen(QAbstractSocket::SocketError e);
signals:
	//void finishUpdate();
	void onRecvFile(QString fnm);
	void onEndRecvFile(QString fnm);
	void heartBeatTimeOut();//心跳超时
	//void rtdbSyncData();//启动 RTDB Data同步
};

#endif // SOCKETTHREAD_H
