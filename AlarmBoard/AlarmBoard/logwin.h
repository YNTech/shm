#ifndef LOGWIN_H
#define LOGWIN_H


#include <QtWidgets>
#include "..\AlarmBoardExt\AlarmBoardCommnDef.h"

class QTextEdit;
class QCheckBox;
class QComboBox;

struct logstruct
{
	logstruct(QString& text,Log_Level& level)
	{
		strlog = text; 
		lev = level;
		strtm = QDateTime::currentDateTime().time().toString();
	}
	QString strlog;
	Log_Level lev;
	QString strtm;
};
class LogWin : public QWidget
{
	Q_OBJECT

public:
	LogWin(QWidget *parent = 0);
	~LogWin();
	void addLog(QString& text,Log_Level level = Log_Debug);
	void beginSaveLog(QString saveName);
	void endSaveLog();
	void setHandle(int hand){m_handle = hand;}
	int getHandle(){return m_handle;};

	QString m_title;
	int		m_index;
public slots:
	void closeEvent( QCloseEvent *event );
protected:
	void timerEvent ( QTimerEvent * event );
private slots:
	void contextMenu( const QPoint & pos );
	void act_clear();
	void updateLogLevel(bool);
private:
	void appendLogText(logstruct& log);

	QTextEdit *logEdit;
	QString fileName;
	QFile m_logFile;
	int m_handle;

	Log_Level m_nLogLevel;
	QTextStream m_logStream;
	bool m_autoSave;
	QMutex m_mutex;
	QList<logstruct>* m_logsptr;
	QList<logstruct> m_logTmp1;
	QList<logstruct> m_logTmp2;
//signals:
//	void loglevel(int);
};
#endif // LOGWIN_H
