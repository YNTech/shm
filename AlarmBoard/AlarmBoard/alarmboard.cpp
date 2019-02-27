#include "alarmboard.h"
#include "mytcpserver.h"
#include "logwin.h"
AlarmBoard* g_AlarmBoard = NULL;
AlarmBoard::AlarmBoard(QWidget *parent)
	: QMainWindow(parent),m_nLoghd(1)
{
	g_AlarmBoard = this;
	ui.setupUi(this);
	QToolButton* btn = new QToolButton(ui.tabWidget);
	btn->setIcon(QIcon(":/Resources/showall.ico"));
	btn->setAutoRaise(true);
	ui.tabWidget->setCornerWidget(btn);
	ui.tabWidget->clear();
	connect(btn,SIGNAL(clicked()),this,SLOT(showAllLogLst()));

	LogWin* lgwg = new LogWin(ui.tabWidget);
	lgwg->setHandle(0);
	int ind = ui.tabWidget->addTab(lgwg,tr("system"));
	m_mapLogWgs[0] = LogWgData(tr("system"),lgwg);
	m_mapLogWgs[0].tabInd = ind;
	m_mapLogWgs[0].isShow = true;


	m_logserver.startServer();
}

AlarmBoard::~AlarmBoard()
{
	g_AlarmBoard = NULL;
	m_logserver.stopServer();
	QMap<int,LogWgData>::iterator it = m_mapLogWgs.begin();
	while(it != m_mapLogWgs.end())
	{
		if(!it->isShow)
			delete it->logwg;
		it++;
	}
}
void AlarmBoard::showAllLogLst()
{
	QMenu menu;
	QMap<int,LogWgData>::iterator it = m_mapLogWgs.begin();
	while(it != m_mapLogWgs.end())
	{
		QAction *act =new QAction(it->title,&menu);
		act->setData(it.key());
		connect(act,SIGNAL(triggered()),this,SLOT(showLogWg()));
		menu.addAction(act);
		it++;
	}
	menu.exec(QCursor::pos());
}
int AlarmBoard::addLogWg(QString title)
{
	m_mutexLog.lock();
	LogWin* lgwg = new LogWin(ui.tabWidget);
	lgwg->setHandle(m_nLoghd);
	int ind = ui.tabWidget->addTab(lgwg,title);
	m_mapLogWgs[m_nLoghd] = LogWgData(title,lgwg);
	m_mapLogWgs[m_nLoghd].tabInd = ind;
	m_mapLogWgs[m_nLoghd].isShow = true;
	m_mutexLog.unlock();

	m_nLoghd++;
	return m_nLoghd-1; 
}
void AlarmBoard::removeLogWg(int hand)
{
	if(hand == 0)
		return;
	m_mutexLog.lock();
	if(m_mapLogWgs.contains(hand))
	{
		ui.tabWidget->removeTab(m_mapLogWgs[hand].tabInd);
		delete m_mapLogWgs[hand].logwg;
		m_mapLogWgs.remove(hand);
	}
	m_mutexLog.unlock();
}
void AlarmBoard::log(int hand, QString& text,Log_Level level)
{
	if(m_mapLogWgs.contains(hand))
		m_mapLogWgs[hand].logwg->addLog(text,level);
}
void AlarmBoard::beginSaveLog(int& hand, QString saveName)
{
	if(m_mapLogWgs.contains(hand))
		m_mapLogWgs[hand].logwg->beginSaveLog(saveName);
}
void AlarmBoard::endSaveLog(int& hand)
{
	if(m_mapLogWgs.contains(hand))
		m_mapLogWgs[hand].logwg->endSaveLog();
}
void AlarmBoard::on_tabWidget_tabCloseRequested(int ind)
{
	m_mutexLog.lock();
	QMap<int,LogWgData>::iterator it = m_mapLogWgs.begin();
	while(it != m_mapLogWgs.end())
	{
		if(ind == it->tabInd)
		{
			if(it.key() == 0)
				return;
			it->isShow = false;
			emit m_logserver.setShowLog(it.key(),false);
			break;
		}
		it++;
	}
	m_mutexLog.unlock();
	ui.tabWidget->removeTab(ind);
}
void AlarmBoard::showLogWg()
{
	QAction* act = qobject_cast<QAction*>(sender());
	if(act)
	{
		int hand = act->data().toInt();
		if(!m_mapLogWgs.contains(hand))
			return;
		if(m_mapLogWgs[hand].isShow)
			ui.tabWidget->setCurrentIndex(m_mapLogWgs[hand].tabInd);
		else
		{
			m_mapLogWgs[hand].tabInd = ui.tabWidget->addTab(m_mapLogWgs[hand].logwg,m_mapLogWgs[hand].title);
			ui.tabWidget->setCurrentIndex(m_mapLogWgs[hand].tabInd);
		}
		m_mapLogWgs[hand].isShow = true;
		emit m_logserver.setShowLog(hand,true);
	}
}