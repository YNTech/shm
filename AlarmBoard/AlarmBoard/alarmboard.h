#ifndef ALARMBOARD_H
#define ALARMBOARD_H

#include <QtWidgets>
#include "ui_alarmboard.h"
#include "mytcpserver.h"
#include "..\AlarmBoardExt\AlarmBoardCommnDef.h"

class LogWin;
struct LogWgData 
{
	LogWgData():logwg(NULL),isShow(false){}
	LogWgData(QString t,LogWin*wg)
	{
		title = t;
		logwg = wg;
	}
	QString title;
	LogWin* logwg;
	int tabInd;
	bool isShow;
};

class AlarmBoard : public QMainWindow
{
	Q_OBJECT

public:
	AlarmBoard(QWidget *parent = 0);
	~AlarmBoard();
	int addLogWg(QString title);
	void removeLogWg(int hand);
	void log(int hand, QString& text,Log_Level level = Log_Debug);
	void beginSaveLog(int& hand, QString saveName);
	void endSaveLog(int& hand);
private slots:
	void showAllLogLst();
	void on_tabWidget_tabCloseRequested(int);//Ö»ÊÇÒþ²Ø£¬Í£Ö¹ÏÔÊ¾
	void showLogWg();
private:
	
	Ui::AlarmBoardClass ui;
	MyTcpServer m_logserver;
	QMutex m_mutexLog;
	
	QMap<int,LogWgData> m_mapLogWgs;//handle/LogWgData
	int m_nLoghd;
};

#endif // ALARMBOARD_H
