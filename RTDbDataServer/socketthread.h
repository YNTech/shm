/*!
 * \file socketthread.h
 *
 * \author YNTech
 * \date ʮһ�� 2018
 *
 * TCP���ݽ����̣߳��ͻ��ˣ�
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
	void beginRTDBSyncData();//���� RTDB Dataͬ��
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

    //��������

    //�Ͽ����ӵ�IP
    void disconnectedIpSignal(const QString& ip);

    //�����Ϸ������¼�
    void onConnectedSignal(const QString& ip);

    //�����ͻ��������¼�
    void otherClientConnectedSignal(QTcpSocket* pSocket,const QString& msg);

private slots:
    //��������Ҫ��ȡ
    void onSocketReadyRead();

    //�Ͽ������¼�
    void onSocketDisconnected();

    //�������¼�
    void onErrorHappen(QAbstractSocket::SocketError e);
signals:
	//void finishUpdate();
	void onRecvFile(QString fnm);
	void onEndRecvFile(QString fnm);
	void heartBeatTimeOut();//������ʱ
	//void rtdbSyncData();//���� RTDB Dataͬ��
};

#endif // SOCKETTHREAD_H
