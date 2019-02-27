#include "socketthread.h"
#include <QHostAddress>
#include "..\AlarmBoardExt\AlarmBoardCommnDef.h"
#include "alarmboard.h"
#include "..\AlarmBoardExt\alarmboardext.h"

extern AlarmBoard* g_AlarmBoard;

SocketThread::SocketThread(QObject *parent, quintptr nSock) :
    QThread(parent),m_Socket(parent),m_nHandle(0)
{
	//blockSize = 0;
    m_Socket.setSocketDescriptor(nSock);
    connect(&m_Socket,SIGNAL(disconnected()),this,SLOT(onSocketDisconnected()));
    connect(&m_Socket,SIGNAL(readyRead()),this,SLOT(onSocketReadyRead()));
    connect(&m_Socket,SIGNAL(error(QLocalSocket::LocalSocketError)),this,SLOT(onErrorHappen(QLocalSocket::LocalSocketError)));
	m_hraetMaxTimer.setInterval(HEARTBEAT_MAXTIME);
	m_hraetMaxTimer.setSingleShot(false);
	m_hraetMaxTimer.start();
	QObject::connect(&m_hraetMaxTimer, &QTimer::timeout, this, [this](){
		unregisterLog();
	});
}


//处理读取的数据
void SocketThread::processData(QByteArray& array)
{
    QDataStream in(&array,QIODevice::ReadOnly);
    //in.setVersion(QDataStream::Qt_4_5);
    unsigned char key;
    QByteArray data;
	unsigned char lev;
	QString msg;
    in >> key;

    switch(key)
    {
	case CMD_Register://
		{
			in >> data;
			if(g_AlarmBoard)
			{
				m_title = QString::fromLocal8Bit(data.data(),data.size());
				m_nHandle = g_AlarmBoard->addLogWg(m_title);
			}
			QByteArray block;
			QDataStream out(&block,QIODevice::WriteOnly);
			out<<(unsigned char)(CMD_registerReturn)<<m_nHandle;
			int size = block.size();
			m_Socket.write((char*)&size,sizeof(int));//先发送block的大小
			m_Socket.write(block.data(),size);//再发送block里面的数据
			m_Socket.flush();
		}
		break;
	case CMD_unRegister://
		unregisterLog();
		break;
	case CMD_Log://
		{
			in >> lev >> msg;
			if(g_AlarmBoard)
				g_AlarmBoard->log(m_nHandle, msg, (Log_Level)lev);
		}
		break;
	case CMD_LogSys://
		{
			in >> lev >> msg;
			if(m_nHandle>0)
				msg = m_title+ ":" + msg;
			if(g_AlarmBoard)
				g_AlarmBoard->log(0, msg, (Log_Level)lev);
		}
		break;
	case CMD_bgSave://
		{
			in >> msg;
			if(g_AlarmBoard)
				g_AlarmBoard->beginSaveLog(m_nHandle, msg);
		}
		break;
	case CMD_endSave://
		{
			if(g_AlarmBoard)
				g_AlarmBoard->endSaveLog(m_nHandle);
		}
		break;
	case CMD_HEARTBEAT:
		{
			m_hraetMaxTimer.start();
		}
		break;
    default:
        ;
    }
}


void SocketThread::run()
{
    m_ClientIp = m_Socket.serverName();

    emit onConnectedSignal(m_ClientIp);//发出连接上的信号

    exec();
}

//有数据需要读取
void SocketThread::onSocketReadyRead()
{
	int blockSize;
	while(m_Socket.bytesAvailable() > 0)
	{
		while(m_Socket.bytesAvailable() < 4)
			msleep(100);
		m_Socket.read((char*)&blockSize, 4);  // read blockSize
		int times = 0;
		while (m_Socket.bytesAvailable() < blockSize && times < 5)				// have no enugh data
		{
			msleep(100);
			times++;
			if (times == 5)
			{
				g_AlarmBoard->log(0, QString("SocketThread::onSocketReadyRead:have no enugh data"), Log_Warning);
				return;
			}
		}
		QByteArray data = m_Socket.read(blockSize);
		processData(data);
	}
}
void SocketThread::unregisterLog()
{
	if (g_AlarmBoard)
		g_AlarmBoard->removeLogWg(m_nHandle);
	quit();
}
//断开连接事件
void SocketThread::onSocketDisconnected()
{
//    qDebug()<<m_ClientIp+" socket disconnected";
    emit disconnectedIpSignal(m_ClientIp);
	unregisterLog();
    exit();
}

//错误发生事件
void SocketThread::onErrorHappen(QLocalSocket::LocalSocketError e)
{
    //qDebug()<<"Error happen: "+m_Socket.errorString();
	g_log_system(m_title+" :Error happen----"+m_Socket.errorString(),Log_Error);
	unregisterLog();
    emit errorSignal(2);
}
void SocketThread::setShowLog(int hand,bool isShow)
{
	if(hand!= m_nHandle)
		return;
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_isShow)<<(unsigned char) isShow;
	int size = block.size();
	m_Socket.write((char*)&size,sizeof(int));//先发送block的大小
	m_Socket.write(block.data(),size);//再发送block里面的数据
	m_Socket.flush();
}