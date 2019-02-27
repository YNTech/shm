/*!
 * \file cmdTCPThread.h
 *
 * \author YNTech
 * \date 十一月 2018
 *
 * TCP传输RTDB数据，数据指令发送，心跳管理，服务端使用，tcp长连接，client监听，服务端发起连接

 心跳
   收到“连接请求”							udp
	   1.发起TCP连接，
	   2.启动TCP心跳,发送心跳
		 发送“加入连接指令”				1
   收到“心跳”
     1.发送“心跳” ，超时则心跳中断		2
   服务端心跳中断：
     1.删除客户端连接、停止发送心跳	
   
  客户端：
   收到“加入连接指令”						1
     1.启动心跳监测
     2.发送 "同步RTDB请求"					3
   收到“心跳”，服务端和客户端使用同一指令码
     2.发送心跳返回
   
   客户端心跳中断：
     1.重连SVR：重新查找svr,发送“连接请求”
	 //2.启动 RTDB Data同步

RTDB同步   只第一次启动同步一次
  服务端：
   收到"同步RTDB请求"					   3
	 1.发送dbsets名称和路径				   -
	 2.发送md5,，发送 “校验RTDB”         4
   收到“同步DB def请求”					5
	 1.发送dbdef文件夹 （“同步DB def”指令）6

  客户端
   收到“校验RTDB”							4
     1.接收：dbsets名称和路径、md5			-
	 2.校验 MD5								-
    校验不通过（RTDB定义改变）
       发送“同步DB def请求”				5
    校验通过
       1.启动 RTDB
	   2.启动 RTDB Data同步
   收到 “同步DB def”						6
     1.接收 DB def
	 2.重建 RTDB
	 3.启动 RTDB
	 4. 启动 RTDB Data同步
RTDB Data同步
   客户端：
	 1.启动server 缓存传输，暂停写入 RTDB，接收数据缓存 >>>>>>>>>>>>>>>>>>>>>>>>>>
	 2.发送 “RTDB Data同步请求”          7
	 3.收到“RTDB Data同步完成”指令		8
	 4.接收数据缓存写入RTDB ,关闭缓存传输    >>>>>>>>>>>>>>>>>>>>>>>>>>
   服务端：
     收到“RTDB Data同步请求”				7
	 1.设置同步RTDB状态
	 2.发送db
	 {//分别发送每个DB,采用单线程，顺序发送

	 }
	 发送完成，发送“RTDB Data同步完成”指令	8
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
	void sendDbMD5();//发送当前数据库集的MD5
	void sendDb(QString dbname, int dbsize, QByteArray& dbdata);
	void sendDbSets(int dbsize, QByteArray& dbdata);
	void disConnect();
	//有数据需要读取
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

