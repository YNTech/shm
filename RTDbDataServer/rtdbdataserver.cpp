#include <QtWidgets>
#include "rtdbdataserver.h"
#include "netsetting.h"
#include "rtdb_api.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"
#include "CallBackThread.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"

using namespace std;


RTDbDataServer::RTDbDataServer(QString nd,QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_curNd = nd;
}

RTDbDataServer::~RTDbDataServer()
{
	g_unregisterLog();
}
void RTDbDataServer::on_actionNet_setting_triggered()
{
	netSetting netui;
	netui.exec();
}

void RTDbDataServer::on_actionStart_triggered()
{
	//if(!rtdb_has_load())
	//{
	//	QMessageBox::warning(0,QObject::tr("Error"),QString(QObject::tr("RTDb is not load!")));
	//	return;
	//}
	g_log(Log_Debug,"initial network ......");
	m_cmdSvr.connectServer();
	return;
	if(!m_netplugn.initNet(m_curNd.toLocal8Bit()))
	{
		g_log_system(Log_Error,"RTDbDataServer initial network failed: %s",m_netplugn.getLastError().c_str());
		return;
	}
	QList<pNetRunCallback> cbs = m_netplugn.getCallBack();
	foreach(pNetRunCallback cb,cbs)
	{
		QThreadPool::globalInstance()->start(new CallBackThread(cb));
	}
}
void RTDbDataServer::on_actionStop_triggered()
{//test
	m_netplugn.sendData(0,"tab_1","db_1");
}