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
    //�Ͽ����ӵ�IP
    void disconnectedIpSignal(const QString& ip);

    //�����Ϸ������¼�
    void onConnectedSignal(const QString& ip);

private slots:
    //��������Ҫ��ȡ
    void onSocketReadyRead();
    //�Ͽ������¼�
    void onSocketDisconnected();
    //�������¼�
    void onErrorHappen(QLocalSocket::LocalSocketError e);
//signals:
//	void finishUpdate();
};

#endif // SOCKETTHREAD_H
