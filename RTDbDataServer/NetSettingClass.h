/*!
 * \file NetSettingClass.h
 *
 * \author YNTech
 * \date 十一月 2018
 *
 * 网络接口，管理类
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
	void connectServer();//连接server
private slots:
	void reCconnectServer();//重连server
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