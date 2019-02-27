#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QThread>
#include <QtNetwork>
#define TYYSENDTIMES 3
struct FileData
{
	FileData(QHostAddress ip,QString f,QString p)
	{
		hostIp=ip;
		strFile=f;
		hostPath=p;
		tryTimes=0;
	}
	QHostAddress hostIp;
	QString strFile;
	QString hostPath;//
	int tryTimes;//���Է��ʹ���
};
class sendFileThread : public QThread
{
	Q_OBJECT

public:
	sendFileThread(int port,QObject *parent);//port�������ļ��˿�
	~sendFileThread();
	qint64 getDataSize(){ return m_nDataSize;};//���������ݴ�С
	void sendFile(QString strIp,QString fName,QString path="");
	void stop();
	bool waitForSendFinish(int waitt=300000);//�ȴ�������ɣ�ms
protected:
	virtual void run ();
private:
	int SendFileData(QTcpSocket &sock,const QString &filePath,const QString &path);//remoteΪfedclient��ʼ�����·��

	bool m_bIsFinished;//�Ƿ����
	qint64 m_nDataSize;
	QList<FileData> m_sendFiles;
	volatile int m_sendFileCount;
	QString m_sendErrMsg;//�����ļ�ʱ������Ϣ
	int m_remotPort;
public slots:
	void onSocketError(QAbstractSocket::SocketError error);
	void onConnected();
	void onDataReadyRead();
	void onDisconnect();
signals:
	void runlog(QString strlog,int lev=1);
};

#endif // SENDFILETHREAD_H
