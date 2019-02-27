#include "alarmboardext.h"
#include <QtNetwork>
#include <QTest>

AlarmBoardExt::AlarmBoardExt():m_curLogInd(-1),m_sendFalg(flag_Show)
{
	connect(&m_Socket,SIGNAL(readyRead()),this,SLOT(onDataReadyRead()));
	m_hraetTimer.setInterval(HEARTBEAT_TIME);
	m_hraetTimer.setSingleShot(false);
	m_hraetTimer.start();
	QObject::connect(&m_hraetTimer, &QTimer::timeout, this, [this](){
		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);
		out << (unsigned char)(CMD_HEARTBEAT);
		return writeData(block);
	});
}

AlarmBoardExt::~AlarmBoardExt()
{
	if(m_sendFalg&flag_Save)
		endSaveLog();
}
bool AlarmBoardExt::registerLog(QString& strTitle)//返回 handle
{
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_Register)<<strTitle.toLocal8Bit();
	return writeData(block);
}
bool AlarmBoardExt::unregisterLog()
{
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_unRegister);
	m_sendFalg &= ~(flag_Reg);
	return writeData(block);
}
bool AlarmBoardExt::log(QString& text,Log_Level level)
{
	if(!(m_sendFalg&flag_Show))
		return true;
	if(!(m_sendFalg&flag_Reg))
	{
		m_errString = tr("not register!");
		return false;
	}
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_Log)<<(unsigned char)(level)<<text;
	return writeData(block);
}
bool AlarmBoardExt::log_system(QString& text,Log_Level level)
{
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_LogSys)<<(unsigned char)(level)<<text;
	return writeData(block);
}
bool AlarmBoardExt::beginSaveLog(QString& saveName)
{
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_bgSave)<<saveName;
	return writeData(block);
}
bool AlarmBoardExt::endSaveLog()
{
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out<<(unsigned char)(CMD_endSave);
	return writeData(block);
}
bool AlarmBoardExt::connectServer()
{
	if(m_Socket.state()!=QAbstractSocket::ConnectedState)
	{
		while(m_Socket.state() ==QAbstractSocket::ConnectingState)
		{
			if(m_Socket.waitForConnected(1000))
				return true;
		}
		m_Socket.connectToServer(ALARMBOARDNAME);
		if(!m_Socket.waitForConnected(3000))
		{
			m_errString = tr("connect server faild :")+m_Socket.errorString();
			printf(m_errString.toLocal8Bit().data());
			return false;
		}
	}
	return true;
}
void AlarmBoardExt::onDataReadyRead()
{
	int blockSize;
	while(m_Socket.bytesAvailable() > 0)
	{
		while(m_Socket.bytesAvailable() < 4)
			QTest::qSleep(100);
		m_Socket.read((char*)&blockSize, 4);  // read blockSize
		int times = 0;
		while (m_Socket.bytesAvailable() < blockSize && times < 5)				// have no enugh data
		{
			QTest::qSleep(100);
			times++;
			if (times == 5)
			{
				log(QString("AlarmBoardExt::onDataReadyRead:have no enugh data"), Log_Warning);
				return;
			}
		}
		QByteArray data = m_Socket.read(blockSize);
		processData(data);
	}
}
bool AlarmBoardExt::writeData(QByteArray & data)
{
	int size = data.size();
	if(m_Socket.write((char*)&size,sizeof(int)) == -1)//先发送block的大小
	{
		if(m_Socket.state()!=QAbstractSocket::ConnectedState)
		{//尝试重新连接，再次发送
			if(connectServer())
				m_Socket.write((char*)&size,sizeof(int));
			else
				return false;
		}
		else
		{
			m_errString = tr("send data Error:")+m_Socket.errorString();
			return false;
		}
	}
	//QTest::qSleep(100);
	if(m_Socket.write(data.data(),size) == -1)//再发送block里面的数据
	{
		m_errString = tr("send data Error:") + m_Socket.errorString();
		return false;
	}
	m_Socket.flush();
	return true;
}
void AlarmBoardExt::processData(QByteArray& array)
{
	QDataStream in(&array,QIODevice::ReadOnly);
	//in.setVersion(QDataStream::Qt_4_5);
	unsigned char key;
	in >> key;
	switch(key)
	{
	case CMD_registerReturn://
		{
			in >> m_curLogInd;
			m_sendFalg |= flag_Reg;
		}
		break;
	case CMD_isShow://
		{
			unsigned char isshow;
			in >> isshow;
			if(isshow)
				m_sendFalg |= flag_Show;
			else
				m_sendFalg &= ~(flag_Show);
		}
		break;
	default:
		;
	}
}

//////////////////////////////////////////////////////////////////////////
//AlarmBoardExt* g_alarmboard = NULL;
AlarmBoardExt* g_getLogExt()
{
	static AlarmBoardExt g_alarmboard;
	return &g_alarmboard;
}
bool g_registerLog(QString strTitle)
{
	return g_getLogExt()->registerLog(strTitle);
}
bool  g_registerLog(const char* strTitle)
{
	return g_getLogExt()->registerLog(QString(strTitle));
}
bool  g_unregisterLog()
{
	return g_getLogExt()->unregisterLog();
}
bool g_log(QString text,Log_Level level)
{
	return g_getLogExt()->log(text,level);
}
bool  g_log(Log_Level level, char* format, ...)
{
	char str_tmp[1024];
	va_list vArgList; 
	va_start (vArgList, format); 
	_vsnprintf(str_tmp, 1024, format, vArgList); 
	va_end(vArgList); 
	return g_getLogExt()->log(QString(str_tmp),level);
}
bool g_log_system(QString text,Log_Level level)
{
	return g_getLogExt()->log_system(text,level);
}
bool  g_log_system(Log_Level level, char* format, ...)
{
	char str_tmp[1024];
	va_list vArgList; 
	va_start (vArgList, format); 
	_vsnprintf(str_tmp, 1024, format, vArgList); 
	va_end(vArgList); 
	return g_getLogExt()->log_system(QString::fromLocal8Bit(str_tmp),level);
}