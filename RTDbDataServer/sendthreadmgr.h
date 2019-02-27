
#ifndef SENDTHREADMGR_H
#define SENDTHREADMGR_H

#include <QObject>
//远端收发文件端口
//#define REMOTFILEPORT_FILE 45461
//更新客户端端口
//#define REMOTFILEPORT_UPDATE 45460
//发送文件线程数
#define MAX_THREADS 30

class sendFileThread;
class sendThreadMgr : public QObject
{
	Q_OBJECT

public:
	sendThreadMgr(QObject *parent=0);
	~sendThreadMgr();
	void sendPath(QString hostIp, QString fname, QString path);//fname文件或目录，path接收方保存位置
	void sendFile(QString strIp,QString fName,QString path);
	bool waitForSendFinish(int waitt=-1);//等待发送完成，ms -1：无限等待
	void stopSend();
private:
	QList<sendFileThread*> m_lstThreads;
signals:
	void runlog(QString strlog,int lev=1);
};

#endif // SENDTHREADMGR_H
