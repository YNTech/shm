/*!
 * �� �� ����socketthread.cpp
 * �����ռ䣺
 * ��    �ܣ�
 * ��    �ߣ�YNTech
 * ͨ    Ѷ��yunantech@163.com
 * �������ڣ�
 * �� �� �ţ�V1.0.0.0
 * �޸���־��
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

QString IP;//Ŀ��IP

QTcpSocket* PSOCKET;

//�����ȡ������
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
	//�ļ����
	case DATATRANS_FILEBEGIN://���տ�ͷ���� file name
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
	case DATATRANS_FILEDATA: //�����м����� file data
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
	case DATATRANS_FILEEND: //���ս�β���� file EOF
    {
//            qDebug() << "File: "+file.fileName()+" transt finished.";
        file.close();
		//if(m_flag==1)
		//	emit finishUpdate();
		emit onEndRecvFile(mFileName);
        break;
    }
	case DATATRANS_DBDATA://ͬ�����ݿ�����   9
	{
		g_log(QString("�յ� ͬ�����ݿ�����(DATATRANS_DBDATA) ָ�� server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		QByteArray data;
		in >> data;
		decodeRtdb_Db(data);
	}
		break;
	//����
	case DATATRANS_CONNECT:
	{
		//�����������
		if (getRTDBConfig()&rtdb_heartbeat)
		{
			g_log(QString("�����ͻ����������(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
			m_hraetMaxTimer.setInterval(HEARTBEAT_MAXTIME);
			QObject::connect(&m_hraetMaxTimer, &QTimer::timeout, this, [this](){
				g_log(QString(tr("heart beat timeout:(%1)��will disconnect to server")).arg(mSocket.peerAddress().toString()), Log_Warning);//������ʱ,���Ͽ�����������
				mSocket.disconnectFromHost();
				emit heartBeatTimeOut();
			});
		}
		//��������ͬ��RTDB
		if (!(getRTDBState() && rtdb_AlreadySync))
		{
			//���� "ͬ��RTDB����"
			sendCmd(DATATRANS_RTDBSYNC);
			g_log(QString("���� ͬ��RTDB����(DATATRANS_RTDBSYNC) ָ� server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		}

	}
		break;
	case DATATRANS_HEARTBEAT:
	{
		g_log(QString(tr("recive heart beat from (%1)")).arg(mSocket.peerAddress().toString()), Log_Debug);
		sendCmd(DATATRANS_HEARTBEAT);
	}
		break;
	case DATATRANS_CHECKMD5://У��MD5 4
	{
		QString  md5;
		in >> m_curDbSetNm >> m_curDbPath >> md5;
		g_log(QString("�յ� У��MD5(DATATRANS_CHECKMD5) ָ�� server:(%1) dbset=%2  path=%3").arg(mSocket.peerAddress().toString()).arg(m_curDbSetNm).arg(m_curDbPath), Log_Debug);
		readRTDbSetsDef(m_curDbPath.toLocal8Bit(), m_curDbSetNm.toLatin1());
		RTDbSets* pset = getRtDbDef(m_curDbSetNm.toLatin1());
		if (pset == nullptr)
			return;
		if (QString::fromStdString(pset->md5) != md5)
		{
			g_log(QString("У��MD5δͨ�������� ͬ��DB def���� ����(DATATRANS_DBDEFSYNCREQ) ָ� server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
			sendCmd(DATATRANS_DBDEFSYNCREQ); //5
		}
		else
		{
			//1.���� RTDB 2.���� RTDB Dataͬ��
			char err[256];
			QByteArray dbsetsname = m_curDbSetNm.toLatin1();
			QByteArray dbrootph = m_curDbPath.toLocal8Bit();
			g_log(QString("���� RTDB db=%1,path=%2").arg(dbsetsname.data()).arg(dbrootph.data()), Log_Debug);
			if (!loadRTDb(dbsetsname.data(), dbrootph.data(), err))
				g_log_system(Log_Error, "ERROR:%s", err);
			setDefauluDbSets(dbsetsname.data());
			//emit rtdbSyncData();
			//RTDB Dataͬ��
			beginRTDBSyncData();
		}
	}
		break;
	case DATATRANS_DBDEFSYNCFINISH://ͬ��DB def���ָ��    6
	{
		g_log(QString("�յ� ͬ��DB define���(DATATRANS_DBDEFSYNCFINISH) ָ�� server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		//2.�ؽ� RTDB
		//3.���� RTDB
		//4.���� RTDB Dataͬ��
		char err[1024];
		QByteArray bapath = m_curDbPath.toLocal8Bit();
		QByteArray banm = m_curDbSetNm.toLatin1();
		bool ret = genRTDB(bapath.data(), banm.data(), err);
		if (!ret)
			g_log(QString(tr("rebuild db error:%1")).arg(err), Log_Warning);
		g_log(QString("���� RTDB db=%1,path=%2").arg(banm.data()).arg(bapath.data()), Log_Debug);
		if (!loadRTDb(banm.data(), bapath.data(), err))
			g_log_system(Log_Error, "ERROR:%s", err);
		setDefauluDbSets(banm.data());
		beginRTDBSyncData();
	}
		break;
	case DATATRANS_DBDATASYNCFINISH://RTDB Dataͬ�����   8
	{
		g_log(QString("�յ� RTDB Dataͬ�����(DATATRANS_DBDATASYNCFINISH) ָ�� server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
		setRTDBState(getRTDBState() & (~rtdb_AlreadySync));
		//4.�������ݻ���д��RTDB ,�رջ��洫��    >>>>>>>>>>>>>>>>>>>>>>>>>>
	}
		break;
	//
    default:
        break;
    }
}

void SocketThread::beginRTDBSyncData()
{
	g_log(QString("��ʼ RTDB Dataͬ�� "), Log_Debug);
	//1.����server ���洫�䣬��ͣд�� RTDB���������ݻ��� >> >> >> >> >> >> >> >> >> >> >> >> >>
	g_log(QString("���� RTDB Dataͬ�� ����(DATATRANS_DBDATASYNCREQ) ָ� server:(%1)").arg(mSocket.peerAddress().toString()), Log_Debug);
	sendCmd(DATATRANS_DBDATASYNCREQ);
	setRTDBState(getRTDBState() | rtdb_AlreadySync);
}
void SocketThread::run()
{
    this->mClientIp = this->mSocket.peerAddress().toString();

    emit this->onConnectedSignal(this->mClientIp);//���������ϵ��ź�

    this->exec();
}

//��������Ҫ��ȡ
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

//�Ͽ������¼�
void SocketThread::onSocketDisconnected()
{
//    qDebug()<<this->mClientIp+" socket disconnected";
    emit this->disconnectedIpSignal(this->mClientIp);

    this->exit();
}

//�������¼�
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
	//��������
	qint64 size = block.size();
	mSocket.write((char*)&size, sizeof(qint64));//�ȷ���block�Ĵ�С
	mSocket.write(block.data(), size);//�ٷ���block���������
	g_log(Log_Debug, "����ָ��:%d", cmd);
	if (!mSocket.waitForBytesWritten())
	{
		g_log(QString(tr("send cmd %1 failed :%2 ").arg(cmd).arg(mSocket.errorString())), Log_Error);
		return;
	}
}