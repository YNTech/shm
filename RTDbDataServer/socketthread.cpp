/*!
 * 文 件 名：socketthread.cpp
 * 命名空间：
 * 功    能：
 * 作    者：YNTech
 * 通    讯：yunantech@163.com
 * 生成日期：
 * 版 本 号：V1.0.0.0
 * 修改日志：
 *
 *
*/
#include "NetCmdDef.h"
#include "socketthread.h"
#include <QHostAddress>
#include <QDir>
#include "rtdb_interface.h"
#include "rtdb_define.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
//#include "compat_file.h"
#include "commonFun.h"
//#include "../compatlib/compat_process.h"

SocketThread::SocketThread(QObject *parent, int h) :
QThread(parent), mSocket(parent), blockSize(-1)
{
    this->blockSize = 0;
    this->blockNumber = 0;

    this->mSocket.setSocketDescriptor(h);

    this->connect(&this->mSocket,SIGNAL(disconnected()),this,
                  SLOT(onSocketDisconnected()));

    this->connect(&this->mSocket,SIGNAL(readyRead()),this,
                  SLOT(onSocketReadyRead()));

    this->connect(&this->mSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                  this,SLOT(onErrorHappen(QAbstractSocket::SocketError)));
	m_flag=0;

}

QString IP;//目的IP

QTcpSocket* PSOCKET;

//处理读取的数据
void SocketThread::processData(QByteArray& array)
{
    QDataStream in(&array,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_5);

    quint16 key;
    in >> key;
//    qDebug()<<"BlockNumber: "+this->blockNumber <<" size: "<<this->blockSize+sizeof(int);
//    qDebug()<<"Key: "+key<<" size: "<<data.size();
    switch(key)
    {
	//文件相关
	case DATATRANS_FILEBEGIN://接收开头数据 file name
        {
			QByteArray data;
			in >> data;
			if(file.isOpen())
				file.close();
            mFileName = mFileName.fromLocal8Bit(data.data(), data.size());
//            QMessageBox::warning(this,"file",this->mFileName);
			mFileName=QDir::cleanPath(mFileName);
			QStringList lst=mFileName.split("/");
			QDir path(QDir::currentPath());
			for (int i=0;i<lst.size()-1;i++)
			{
				if(!path.exists(lst[i]))
					path.mkdir(lst[i]);
				path.cd(lst[i]);

			}
            file.setFileName(mFileName);
            if(file.exists(mFileName))
            {
                    qDebug() << "File was exists. now remove it!";
                    file.remove(this->mFileName);
            }
            if(!file.open(QIODevice::WriteOnly))
            {
            }
			emit onRecvFile(mFileName);
            break;
        }
	case DATATRANS_FILEDATA: //接收中间数据 file data
    {
		QByteArray data;
		in >> data;
		if(file.isOpen())
		{
			file.write(data.data(), data.size());
			file.flush();
		}
        break;
    }
	case DATATRANS_FILEEND: //接收结尾数据 file EOF
    {
//            qDebug() << "File: "+file.fileName()+" transt finished.";
        file.close();
		//if(m_flag==1)
		//	emit finishUpdate();
		emit onEndRecvFile(mFileName);
        break;
    }
	case DATATRANS_DBDATA://同步数据库数据   9
	{
		g_log(QString("收到 同步数据库数据(DATATRANS_DBDATA) 指令 server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		QByteArray data;
		in >> data;
		decodeRtdb_Db(data);
	}
		break;
	//心跳
	case DATATRANS_CONNECT:
	{
		//启动心跳监测
		if (getRTDBConfig()&rtdb_heartbeat)
		{
			g_log(QString("启动客户端心跳监测(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
			m_hraetMaxTimer.setInterval(HEARTBEAT_MAXTIME);
			QObject::connect(&m_hraetMaxTimer, &QTimer::timeout, this, [this](){
				g_log(QString(tr("heart beat timeout:(%1)，will disconnect to server")).arg(mSocket.peerAddress().toString()), Log_Warning);//心跳超时,将断开服务器连接
				mSocket.disconnectFromHost();
				emit heartBeatTimeOut();
			});
		}
		//重连则不再同步RTDB
		if (!(getRTDBState() && rtdb_AlreadySync))
		{
			//发送 "同步RTDB请求"
			sendCmd(DATATRANS_RTDBSYNC);
			g_log(QString("发送 同步RTDB请求(DATATRANS_RTDBSYNC) 指令到 server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		}

	}
		break;
	case DATATRANS_HEARTBEAT:
	{
		g_log(QString(tr("recive heart beat from (%1)")).arg(mSocket.peerAddress().toString()), Log_Debug);
		sendCmd(DATATRANS_HEARTBEAT);
	}
		break;
	case DATATRANS_CHECKMD5://校验MD5 4
	{
		QString  md5;
		in >> m_curDbSetNm >> m_curDbPath >> md5;
		g_log(QString("收到 校验MD5(DATATRANS_CHECKMD5) 指令 server:(%1) dbset=%2  path=%3").arg(mSocket.peerAddress().toString()).arg(m_curDbSetNm).arg(m_curDbPath), Log_Debug);
		readRTDbSetsDef(m_curDbPath.toLocal8Bit(), m_curDbSetNm.toLatin1());
		RTDbSets* pset = getRtDbDef(m_curDbSetNm.toLatin1());
		if (pset == nullptr)
			return;
		if (QString::fromStdString(pset->md5) != md5)
		{
			g_log(QString("校验MD5未通过，发送 同步DB def请求 请求(DATATRANS_DBDEFSYNCREQ) 指令到 server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
			sendCmd(DATATRANS_DBDEFSYNCREQ); //5
		}
		else
		{
			//1.启动 RTDB 2.启动 RTDB Data同步
			char err[256];
			QByteArray dbsetsname = m_curDbSetNm.toLatin1();
			QByteArray dbrootph = m_curDbPath.toLocal8Bit();
			g_log(QString("启动 RTDB db=%1,path=%2").arg(dbsetsname.data()).arg(dbrootph.data()), Log_Debug);
			if (!loadRTDb(dbsetsname.data(), dbrootph.data(), err))
				g_log_system(Log_Error, "ERROR:%s", err);
			setDefauluDbSets(dbsetsname.data());
			//emit rtdbSyncData();
			//RTDB Data同步
			beginRTDBSyncData();
		}
	}
		break;
	case DATATRANS_DBDEFSYNCFINISH://同步DB def完成指令    6
	{
		g_log(QString("收到 同步DB define完成(DATATRANS_DBDEFSYNCFINISH) 指令 server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		//2.重建 RTDB
		//3.启动 RTDB
		//4.启动 RTDB Data同步
		char err[1024];
		QByteArray bapath = m_curDbPath.toLocal8Bit();
		QByteArray banm = m_curDbSetNm.toLatin1();
		bool ret = genRTDB(bapath.data(), banm.data(), err);
		if (!ret)
			g_log(QString(tr("rebuild db error:%1")).arg(err), Log_Warning);
		g_log(QString("启动 RTDB db=%1,path=%2").arg(banm.data()).arg(bapath.data()), Log_Debug);
		if (!loadRTDb(banm.data(), bapath.data(), err))
			g_log_system(Log_Error, "ERROR:%s", err);
		setDefauluDbSets(banm.data());
		beginRTDBSyncData();
	}
		break;
	case DATATRANS_DBDATASYNCFINISH://RTDB Data同步完成   8
	{
		g_log(QString("收到 RTDB Data同步完成(DATATRANS_DBDATASYNCFINISH) 指令 server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		setRTDBState(getRTDBState() & (~rtdb_AlreadySync));
		//4.接收数据缓存写入RTDB ,关闭缓存传输    >>>>>>>>>>>>>>>>>>>>>>>>>>
	}
		break;
	//
    default:
        break;
    }
}

void SocketThread::beginRTDBSyncData()
{
	g_log(QString("开始 RTDB Data同步 "), Log_Debug);
	//1.启动server 缓存传输，暂停写入 RTDB，接收数据缓存 >> >> >> >> >> >> >> >> >> >> >> >> >>
	g_log(QString("发送 RTDB Data同步 请求(DATATRANS_DBDATASYNCREQ) 指令到 server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
	sendCmd(DATATRANS_DBDATASYNCREQ);
	setRTDBState(getRTDBState() | rtdb_AlreadySync);
}
void SocketThread::run()
{
    this->mClientIp = this->mSocket.peerAddress().toString();

    emit this->onConnectedSignal(this->mClientIp);//发出连接上的信号

    this->exec();
}

//有数据需要读取
void SocketThread::onSocketReadyRead()
{
    while(this->mSocket.bytesAvailable() >= sizeof(qint64) || this->blockSize>0)
    {
        if(this->blockSize == 0)
        {
                if(this->mSocket.bytesAvailable() < sizeof(qint64))
                {
                    return;
                }
                this->mSocket.read((char*)&this->blockSize, sizeof(qint64));  // read blockSize
        }

        if(this->mSocket.bytesAvailable() < this->blockSize)				// have no enugh data
                return;
        QByteArray data = this->mSocket.read(this->blockSize);
        this->processData(data);
        this->blockSize = 0;
    }
}

//断开连接事件
void SocketThread::onSocketDisconnected()
{
//    qDebug()<<this->mClientIp+" socket disconnected";
    emit this->disconnectedIpSignal(this->mClientIp);

    this->exit();
}

//错误发生事件
void SocketThread::onErrorHappen(QAbstractSocket::SocketError e)
{
    qDebug()<<"Error happen: "+this->mSocket.errorString();

    emit this->errorSignal(2);
}
void SocketThread::sendCmd(qint16 cmd)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << qint16(cmd);// << fname.toLocal8Bit();//(fileName.toAscii());
	//发送数据
	qint64 size = block.size();
	mSocket.write((char*)&size, sizeof(qint64));//先发送block的大小
	mSocket.write(block.data(), size);//再发送block里面的数据
	g_log(Log_Debug, "发送指令:%d", cmd);
	if (!mSocket.waitForBytesWritten())
	{
		g_log(QString(tr("send cmd %1 failed :%2 ").arg(cmd).arg(mSocket.errorString())), Log_Error);
		return;
	}
}