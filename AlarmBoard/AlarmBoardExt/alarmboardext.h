#ifndef ALARMBOARDEXT_H
#define ALARMBOARDEXT_H

#include "alarmboardext_global.h"
#include <QLocalSocket>
#include <QTimer>
#include "AlarmBoardCommnDef.h"
class ALARMBOARDEXT_EXPORT AlarmBoardExt : public QObject
{
	Q_OBJECT
public:
	enum SendFlag{flag_Show=0x01,flag_Reg=0x02,flag_Save=0x04};
	AlarmBoardExt();
	~AlarmBoardExt();
	bool registerLog(QString& strTitle);//返回 handle
	bool unregisterLog();
	bool log(QString& text,Log_Level level = Log_Debug);
	bool log_system(QString& text,Log_Level level = Log_Debug);// system为专用显示页
	bool beginSaveLog(QString& saveName);
	bool endSaveLog();
	void setShowLog(bool isShow) { m_sendFalg |= (flag_Show&isShow); }
	bool isShowLog(){ return m_sendFalg&flag_Show; }
	QString getErrorString(){ return m_errString; }
private slots:
	void onDataReadyRead();
private:
	bool connectServer();
	bool writeData(QByteArray & data);
	void processData(QByteArray& array);
	int m_curLogInd;
	unsigned char m_sendFalg;
	QLocalSocket m_Socket;
	QString m_errString;
	QTimer m_hraetTimer;
};

ALARMBOARDEXT_EXPORT AlarmBoardExt* g_getLogExt();
ALARMBOARDEXT_EXPORT bool  g_registerLog(QString strTitle);
ALARMBOARDEXT_EXPORT bool  g_registerLog(const char* strTitle);
ALARMBOARDEXT_EXPORT bool  g_unregisterLog();
ALARMBOARDEXT_EXPORT bool  g_log(QString text,Log_Level level = Log_Debug);
ALARMBOARDEXT_EXPORT bool  g_log(Log_Level level, char* format, ...);
ALARMBOARDEXT_EXPORT bool  g_log_system(QString text,Log_Level level = Log_Debug);
ALARMBOARDEXT_EXPORT bool  g_log_system(Log_Level level, char* format, ...);
#endif // ALARMBOARDEXT_H
