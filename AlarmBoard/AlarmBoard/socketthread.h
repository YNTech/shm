#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QLocalSocket>
#include <QFile>
#include <QDataStream>
#include <QAbstractSocket>
#include <QTimer>

//#define REMOTFILEPORT 45454
//
//#define REMOTFILEPORT_UPDATE 45460

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = 0,quintptr nSock = 0);
	void sendData(QByteArray& array);
    QLocalSocket m_Socket;
public slots:
	void setShowLog(int hand,bool isShow);
private:
	//qint64 blockSize;
	void processData(QByteArray& array);
	void unregisterLog();
    QString m_ClientIp;
	int m_nHandle;
	QString m_title;
	QTimer m_hraetMaxTimer;
protected:
    void run();
signals:
    void errorSignal(int);
    //断开连接的IP
    void disconnectedIpSignal(const QString& ip);

    //连接上服务器事件
    void onConnectedSignal(const QString& ip);

private slots:
    //有数据需要读取
    void onSocketReadyRead();
    //断开连接事件
    void onSocketDisconnected();
    //错误发生事件
    void onErrorHappen(QLocalSocket::LocalSocketError e);
//signals:
//	void finishUpdate();
};

#endif // SOCKETTHREAD_H
