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
	int tryTimes;//尝试发送次数
};
class sendFileThread : public QThread
{
	Q_OBJECT

public:
	sendFileThread(int port,QObject *parent);//port：发送文件端口
	~sendFileThread();
	qint64 getDataSize(){ return m_nDataSize;};//待发送数据大小
	void sendFile(QString strIp,QString fName,QString path="");
	void stop();
	bool waitForSendFinish(int waitt=300000);//等待发送完成，ms
protected:
	virtual void run ();
private:
	int SendFileData(QTcpSocket &sock,const QString &filePath,const QString &path);//remote为fedclient起始的相对路径

	bool m_bIsFinished;//是否结束
	qint64 m_nDataSize;
	QList<FileData> m_sendFiles;
	volatile int m_sendFileCount;
	QString m_sendErrMsg;//发送文件时错误信息
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
