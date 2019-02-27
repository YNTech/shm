#include "mytcpserver.h"
#include "socketthread.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
#include "commonFun.h"
#include "NetCmdDef.h"
MyTcpServer::MyTcpServer(QObject *parent) :
    QTcpServer(parent)
{
}

//启动服务器
void MyTcpServer::startServer()
{
	this->listen(QHostAddress::Any, DATARECVPORT);
	g_log(QString(tr("started data recive port: %1")).arg(DATARECVPORT),Log_Debug);
}

void MyTcpServer::stopServer()
{
    this->close();
}

//处理数据到来
void MyTcpServer::incomingConnection(int handle)
{
    SocketThread* thread = new SocketThread(this,handle);
	//thread->setFlag(m_flag);

//    this->connect(thread,SIGNAL(errorSignal(int)),this,
//                  SLOT(onErrorHappen(int)));

    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
	connect(thread, SIGNAL(onRecvFile(QString)), this, SIGNAL(onRecvFile(QString)));
	connect(thread, SIGNAL(onEndRecvFile(QString)), this, SIGNAL(onEndRecvFile(QString)));
	connect(thread, SIGNAL(heartBeatTimeOut()), this, SIGNAL(heartBeatTimeOut()));
    thread->start();
}

//错误发生事件
void MyTcpServer::onErrorHappen(int error)
{
    qDebug()<<"Error happen: "+this->errorString();
}

//线程socket连接上事件
void MyTcpServer::onClientConnected(const QString& ip)
{
    qDebug()<<"Client: "+ip+" is connected";
}

//线程socket断开连接事件
void MyTcpServer::onClientSocketDisconnected(const QString& ip)
{
    qDebug()<<"Client: "+ip+" was disconnected";
}

//线程完成事件
void MyTcpServer::onThreadFinished()
{
//    this->sendOnLineMessage(0x0004);

    qDebug()<<"A client socket thread finished\n";
}

////发送在线信息
//void MyTcpServer::sendOnLineMessage(const qint16& flag)
//{
//    QList<QString> arrKeys = MAP.values();
//    QString strKey = "";//192.168.1.87,192.168.1.188
//    foreach(QString key,arrKeys)
//    {
//        strKey += key+",";
//    }

//    strKey = strKey.left(strKey.length() - 1);

//    qint64 size;
//    QByteArray block;
//    QDataStream out(&block,QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_5);
//    out<<qint16(flag)<<strKey.toUtf8();//0x0004为更新在线IP标记

//    size = block.size();

//    for(QMap<QTcpSocket*,QString>::const_iterator iter=MAP.begin();
//            iter!=MAP.end(); iter++)
//    {
//        QTcpSocket* socket = iter.key();

//        socket->write((char*)&size,sizeof(qint64));
//        socket->write(block.data(),size);
//    }
//}

//其他客户端到来事件
void MyTcpServer::onOtherClientConnected(QTcpSocket* pSocket,
                            const QString& msg)
{
    qDebug()<<"coming";
//    this->sendOnLineMessage(0x0004);
}
