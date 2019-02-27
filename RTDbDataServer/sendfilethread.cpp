#include "sendfilethread.h"
#include "NetCmdDef.h"

sendFileThread::sendFileThread(int port,QObject *parent)
	: QThread(parent),m_bIsFinished(false),m_nDataSize(0),m_remotPort(port),m_sendFileCount(0)
{

}

sendFileThread::~sendFileThread()
{
}
void sendFileThread::onSocketError(QAbstractSocket::SocketError error)
{
	QTcpSocket* sock=qobject_cast<QTcpSocket*>(sender());
	if(sock)
		emit runlog("socketError： "+sock->errorString(),6);
}
void sendFileThread::onConnected()
{
	QTcpSocket* sock=qobject_cast<QTcpSocket*>(sender());
	if(sock)
		emit runlog("connect: "+sock->peerAddress().toString(),1);
}
void sendFileThread::onDataReadyRead()
{

}
void sendFileThread::onDisconnect()
{
	QTcpSocket* sock=qobject_cast<QTcpSocket*>(sender());
	if(sock)
		emit runlog("disconnect: "+sock->peerAddress().toString(),1);
}
void sendFileThread::run ()
{
	QTcpSocket tcpSocket;
	connect(&tcpSocket,SIGNAL(connected()),this,SLOT(onConnected()));
	connect(&tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketError(QAbstractSocket::SocketError)));
	connect(&tcpSocket,SIGNAL(readyRead()),this,SLOT(onDataReadyRead()));
	connect(&tcpSocket,SIGNAL(disconnected()),this,SLOT(onDisconnect()));

	m_bIsFinished=false;
	int nt=0;
	while(!m_bIsFinished)
	{
		while(m_sendFiles.size()>0)
		{
			nt=0;
			if(m_bIsFinished)
				break;
			FileData f=m_sendFiles.takeFirst();
			if(tcpSocket.state()==QAbstractSocket::ConnectedState && f.hostIp!=tcpSocket.peerAddress())
			{
				tcpSocket.disconnectFromHost();
				if(!tcpSocket.waitForDisconnected(3000))
				{
					emit runlog("disconnect fail: "+tcpSocket.errorString(),6);
					if(f.tryTimes<TYYSENDTIMES)
					{
						f.tryTimes++;
						m_sendFiles<<f;
					}
					else
					{
						emit runlog("send file "+f.hostIp.toString()+":"+f.strFile+" failed!!! ",6);
						m_sendFileCount--;
					}
					
					continue;
				}
			}
			if(tcpSocket.state()!=QAbstractSocket::ConnectedState)
			{
				tcpSocket.connectToHost(f.hostIp,m_remotPort);
				if(!tcpSocket.waitForConnected(3000))
				{
					if(f.tryTimes<TYYSENDTIMES)
					{
						f.tryTimes++;
						m_sendFiles<<f;
					}
					else
					{
						m_sendFileCount--;
						emit runlog("send file "+f.hostIp.toString()+":"+f.strFile+" failed -----"+"connect "+f.hostIp.toString()+" failed:"+tcpSocket.errorString(),6);
					}
					continue;
				}
			}
			int ret=SendFileData(tcpSocket,f.strFile,f.hostPath);
			if(ret==-1)//读取文件失败
				emit runlog(m_sendErrMsg,6);
			if(ret==-2)
			{
				if(f.tryTimes<TYYSENDTIMES)
				{
					f.tryTimes++;
					m_sendFiles<<f;
				}
				else
				{
					m_sendFileCount--;
					emit runlog("send file "+f.hostIp.toString()+":"+f.strFile+" failed ----"+m_sendErrMsg,6);
				}
			}
			else
				m_sendFileCount--;
		}
		if(nt>60 &&tcpSocket.state()==QAbstractSocket::ConnectedState)//1min无数据发送则断开连接
		{
			tcpSocket.disconnectFromHost();
			if(tcpSocket.state() != QAbstractSocket::UnconnectedState&&(!tcpSocket.waitForDisconnected(3000)))
			{
				emit runlog("disconnect fail :"+tcpSocket.errorString(),6);
			}
			nt=0;
		}
		else
		{
			nt++;
			sleep(5);
		}
	}
	if(tcpSocket.state()==QAbstractSocket::ConnectedState)
		tcpSocket.disconnectFromHost();
}
void sendFileThread::sendFile(QString strIp,QString fName,QString path)
{
	if(!isRunning())
		start();
	QFileInfo f(fName);
	if(!f.exists())
		return;
	m_sendFileCount++;
	m_sendFiles<<FileData(QHostAddress(strIp),fName,path);
	m_nDataSize+=f.size();
}
int sendFileThread::SendFileData(QTcpSocket &sock,const QString &filePath,const QString &path)//
{
	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly))
	{
		m_sendErrMsg="send file "+filePath+" failed:"+file.errorString();
		return -1;
	}

	//QFileInfo fileInfo(filePath);
	//QString fileName = fileInfo.fileName();

	qint64 size;
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	QFileInfo f(filePath);
	QString fname;
	if (path.isEmpty())
		fname = f.fileName();
	else
		fname = path + "/" + f.fileName();
	out << qint16(DATATRANS_FILEBEGIN) << fname.toLocal8Bit();//(fileName.toAscii());

	//发送开头数据
	size = block.size();//block里面有DATATRANS_FILEBEGIN,文件名称
	sock.write((char*)&size,sizeof(qint64));//先发送block的大小
	sock.write(block.data(),size);//再发送block里面的数据

	if(!sock.waitForBytesWritten())
	{
		m_sendErrMsg="send file data "+filePath+" failed:"+sock.errorString();
		return -2;
	}

	//发送中间数据
	do
	{
		block.clear();//QByteArray
		out.device()->seek(0);
		out << qint16(DATATRANS_FILEDATA) << file.read(0x3FF0);

		size = block.size();
		//        emit this->getMessage("Send Data Size:"+QString::number(size));

		sock.write((char*)&size,sizeof(qint64));

		sock.write(block.data(),size);

		if(!sock.waitForBytesWritten())
		{
			m_sendErrMsg="send file data "+filePath+" failed:"+sock.errorString();
			return -2;
		}

	}
	while(!file.atEnd() && !m_bIsFinished);

	//发送结尾数据
	qint64 endSize;
	QByteArray endBlock;
	QDataStream endOut(&endBlock,QIODevice::WriteOnly);
	endOut.setVersion(QDataStream::Qt_4_5);

	QByteArray data = filePath.toLatin1();
	data.resize(0x8000);
	endOut << quint16(DATATRANS_FILEEND) << data << filePath.toLatin1();
	endSize = endBlock.size();

	sock.write((char*)&endSize,sizeof(qint64));
	sock.write(endBlock.data(),endSize);

	if(!sock.waitForBytesWritten())
	{
		m_sendErrMsg="send file data "+filePath+" failed:"+sock.errorString();
		return -2;
	}
	else
	{
		emit runlog("send file "+filePath+" finish.",1);
		m_nDataSize-=size;
		return 0;
	}
}

bool sendFileThread::waitForSendFinish(int waitt)
{
	int n=waitt/20;
	while(n>0)
	{
		if(m_sendFileCount>0)
		{
			msleep(20);
			n--;
		}
		else
		{
			stop();
			return true;
		}
	}
	return false;
}
void sendFileThread::stop()
{
	m_sendFileCount=0;
	m_bIsFinished=true;
}
