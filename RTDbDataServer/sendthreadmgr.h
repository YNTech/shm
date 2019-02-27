
#ifndef SENDTHREADMGR_H
#define SENDTHREADMGR_H

#include <QObject>
//Զ���շ��ļ��˿�
//#define REMOTFILEPORT_FILE 45461
//���¿ͻ��˶˿�
//#define REMOTFILEPORT_UPDATE 45460
//�����ļ��߳���
#define MAX_THREADS 30

class sendFileThread;
class sendThreadMgr : public QObject
{
	Q_OBJECT

public:
	sendThreadMgr(QObject *parent=0);
	~sendThreadMgr();
	void sendPath(QString hostIp, QString fname, QString path);//fname�ļ���Ŀ¼��path���շ�����λ��
	void sendFile(QString strIp,QString fName,QString path);
	bool waitForSendFinish(int waitt=-1);//�ȴ�������ɣ�ms -1�����޵ȴ�
	void stopSend();
private:
	QList<sendFileThread*> m_lstThreads;
signals:
	void runlog(QString strlog,int lev=1);
};

#endif // SENDTHREADMGR_H
