/*!
 * \file NetSettingClass.h
 *
 * \author YNTech
 * \date ʮһ�� 2018
 *
 * ����ӿڣ�������
 */
#pragma once
#ifndef __NetSettingClass_h__
#define __NetSettingClass_h__
#include "udpSock.h"
#include "mytcpserver.h"
#include "sendthreadmgr.h"
#include <QtWidgets>
#include <QMap>
#include "cmdTCPThread.h"


class NetSettingClass :public QObject
{
	Q_OBJECT
public:
	NetSettingClass(void);
	~NetSettingClass(void);
	void connectServer();//����server
private slots:
	void reCconnectServer();//����server
private:
	void dealLnkSvrResq(quint32 nIp);
	void endSyncRTDB();
	void onRecvFile(QString fnm);
	void onEndRecvFile();
	udpSock m_cmdSock;
	MyTcpServer m_tcpRecv;
	sendThreadMgr m_senfile;

	QProgressDialog m_recvFileProssDlg;

	QMap<int,cmdTCPThread*> m_mapCmd;//ip,cmdTCPThread
};

#endif//__NetSettingClass_h__