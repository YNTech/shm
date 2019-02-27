#include "logwin.h"
#include <QtWidgets>

LogWin::LogWin(QWidget *parent)
: QWidget(parent),m_autoSave(false),m_nLogLevel(Log_Debug)
{
	logEdit = new QTextEdit();
	logEdit->setReadOnly(true);
	logEdit->document()->setMaximumBlockCount(5000);
	QHBoxLayout* lay =new QHBoxLayout(this);
	lay->addWidget(logEdit);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested ( const QPoint&)), this, SLOT(contextMenu( const QPoint &)));

	setMinimumSize(parent->size());	
	//setWindowTitle("Log Monitoring");
	m_logsptr = &m_logTmp1;

	startTimer(8000);
}

LogWin::~LogWin()
{
}

void LogWin::appendLogText( logstruct& log)
{
	//只记录高于选定级别的日志
	QString color="black";
	switch (log.lev) {
		case Log_Debug:
			break;
		case Log_Notify:
			color="#000080";
			break;
		case Log_Warning:
			color="#008000";
			break;
		case Log_Error:
			color="#990000";
			break;
		case Lev_5:
			color="#ff0000";
			break;	
		case Lev_6:
			color="#008080";
			break;
		case Lev_7:
			color="#800080";
			break;
	}
	QString str=QString(log.strtm+": <font color=%1>%2</font>").arg(color).arg(log.strlog);
	logEdit->append(str);
}
void LogWin::beginSaveLog(QString saveName)
{
	if(!m_autoSave)
	{
		QDateTime curtm = QDateTime::currentDateTime();
		m_logFile.setFileName(saveName+"_"+curtm.toString("yyyyMMddhhmmss")+".log");//qApp->applicationName()+
		m_logFile.open(QIODevice::WriteOnly);
		m_logStream.setDevice(&m_logFile);
	}
	m_autoSave = true;
}
void LogWin::endSaveLog()
{
	if(m_autoSave)
		m_logFile.close();
	m_autoSave = false;
}
void LogWin::closeEvent( QCloseEvent *event )
{

}

void LogWin::act_clear()
{
	logEdit->clear();
}

void LogWin::updateLogLevel(bool )
{
	QAction* act = qobject_cast<QAction*>(this->sender());
	if(act)
		m_nLogLevel = (Log_Level)act->data().toInt();
}
void LogWin::timerEvent ( QTimerEvent * event )
{
	QList<logstruct>* tmp;
	m_mutex.lock();
	if(m_logsptr==(&m_logTmp1))
	{
		m_logsptr = &m_logTmp2;
		tmp = &m_logTmp1;
	}
	else
	{
		m_logsptr = &m_logTmp1;
		tmp = &m_logTmp2;
	}
	m_mutex.unlock();
	QString strtmp;
	foreach(logstruct log,*tmp)
	{
		appendLogText(log);
		if(m_autoSave)
			strtmp=log.strtm+":"+QString("[Lev_%1]").arg(log.lev+1)+log.strlog+"\n";
	}
	if(m_autoSave)
	{
		m_logStream<<(strtmp);
		m_logStream.flush();
	}
	tmp->clear();
}
void LogWin::addLog(QString& text,Log_Level level)
{
	if(level<m_nLogLevel)
		return;
	m_mutex.lock();
	m_logsptr->append(logstruct(text,level));
	m_mutex.unlock();
}
void LogWin::contextMenu( const QPoint & pos )
{
	QMenu menu;
	menu.addAction(tr("clear"),this,SLOT(act_clear()));
	QMenu *levmenu = new QMenu(tr("log level"));
	for (int i=1; i<8; i++)
	{
		QAction *act = new QAction("Lev_"+QString::number(i),levmenu);
		connect(act,SIGNAL(toggled(bool)),this,SLOT(updateLogLevel(bool)));
		act->setCheckable(true);
		act->setData(i-1);
		if(i-1 == m_nLogLevel)
			act->setChecked(true);
		levmenu->addAction(act);
	}
	menu.addMenu(levmenu);
	//menu.addAction(tr("setting"),this,SLOT(act_setting()));
	menu.exec(QCursor::pos());
}