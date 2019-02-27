/*!
 * \file cmdTCPThread.h
 *
 * \author YNTech
 * \date ʮһ�� 2018
 *
 * TCP����RTDB���ݣ�����ָ��ͣ��������������ʹ�ã�tcp�����ӣ�client����������˷�������

 ����
   �յ�����������							udp
	   1.����TCP���ӣ�
	   2.����TCP����,��������
		 ���͡���������ָ�				1
   �յ���������
     1.���͡������� ����ʱ�������ж�		2
   ����������жϣ�
     1.ɾ���ͻ������ӡ�ֹͣ��������	
   
  �ͻ��ˣ�
   �յ�����������ָ�						1
     1.�����������
     2.���� "ͬ��RTDB����"					3
   �յ���������������˺Ϳͻ���ʹ��ͬһָ����
     2.������������
   
   �ͻ��������жϣ�
     1.����SVR�����²���svr,���͡���������
	 //2.���� RTDB Dataͬ��

RTDBͬ��   ֻ��һ������ͬ��һ��
  ����ˣ�
   �յ�"ͬ��RTDB����"					   3
	 1.����dbsets���ƺ�·��				   -
	 2.����md5,������ ��У��RTDB��         4
   �յ���ͬ��DB def����					5
	 1.����dbdef�ļ��� ����ͬ��DB def��ָ�6

  �ͻ���
   �յ���У��RTDB��							4
     1.���գ�dbsets���ƺ�·����md5			-
	 2.У�� MD5								-
    У�鲻ͨ����RTDB����ı䣩
       ���͡�ͬ��DB def����				5
    У��ͨ��
       1.���� RTDB
	   2.���� RTDB Dataͬ��
   �յ� ��ͬ��DB def��						6
     1.���� DB def
	 2.�ؽ� RTDB
	 3.���� RTDB
	 4. ���� RTDB Dataͬ��
RTDB Dataͬ��
   �ͻ��ˣ�
	 1.����server ���洫�䣬��ͣд�� RTDB���������ݻ��� >>>>>>>>>>>>>>>>>>>>>>>>>>
	 2.���� ��RTDB Dataͬ������          7
	 3.�յ���RTDB Dataͬ����ɡ�ָ��		8
	 4.�������ݻ���д��RTDB ,�رջ��洫��    >>>>>>>>>>>>>>>>>>>>>>>>>>
   ����ˣ�
     �յ���RTDB Dataͬ������				7
	 1.����ͬ��RTDB״̬
	 2.����db
	 {//�ֱ���ÿ��DB,���õ��̣߳�˳����

	 }
	 ������ɣ����͡�RTDB Dataͬ����ɡ�ָ��	8
 */

#pragma once
#ifndef __CMDTCPTHREAD_H__
#define __CMDTCPTHREAD_H__

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QAbstractSocket>
#include <QTimer>
#include "sendthreadmgr.h"
#include "SendDbThread.h"

class cmdTCPThread : public QObject
{
	Q_OBJECT

public:
	cmdTCPThread( ushort port, QObject *parent=0);
	~cmdTCPThread();
	QTcpSocket tcpSocket;
	void setAddress(quint32 ip){ m_ip = ip; };
	void sendCmd(qint16 cmd);
	void connectToHost(quint32 ip);
	void sendDbMD5();//���͵�ǰ���ݿ⼯��MD5
	void sendDb(QString dbname, int dbsize, QByteArray& dbdata);
	void sendDbSets(int dbsize, QByteArray& dbdata);
	void disConnect();
	//��������Ҫ��ȡ
	void onSocketReadyRead();
	void processData(QByteArray& array);
private:
	quint32 m_ip;
	ushort m_port;

	qint64 blockSize;
	qint64 blockNumber;
	QTimer m_hraetMaxTimer;
	QTimer m_hraetTimer;

	sendThreadMgr m_senfile;
	SendDbThreadMgr m_dbsend;

signals:
	void timeOut(quint32 ip);
};

#endif // __CMDTCPTHREAD_H__

