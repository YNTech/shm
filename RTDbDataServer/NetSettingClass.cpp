#include <QtWidgets>
#include "NetSettingClass.h"
#include "commonFun.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
#include "rtdb_interface.h"
#include "NetCmdDef.h"

NetSettingClass::NetSettingClass(void) /*: m_senfile(this)*/
{
	connect(&m_cmdSock, &udpSock::dealLnkSvrResq, this, &NetSettingClass::dealLnkSvrResq, Qt::QueuedConnection);
	connect(&m_cmdSock, &udpSock::endSyncRTDB, this, &NetSettingClass::endSyncRTDB, Qt::QueuedConnection);
	connect(&m_cmdSock, &udpSock::changeServer, this, [=](){//切换服务器
			this->m_cmdSock.stopSvr();
	});
	connect(&m_tcpRecv, SIGNAL(heartBeatTimeOut()), this, SLOT(reCconnectServer()));
}

NetSettingClass::~NetSettingClass(void)
{
}
bool showProgressDialog(int tms)
{
	int n = tms > 0 ? tms : 100;
	QProgressDialog progress(QObject::tr("search server node ......"), QObject::tr("cancel"), 0, n);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < n; i++) {
		progress.setValue(i);

		if (getSvrNodeId() >= 0)
			return true;
		QThread::msleep(100);

		if (progress.wasCanceled())
			return false;
		if (tms<0 && i == n)
			i = 0;
	}
	progress.setValue(n);
	return false;
}
void NetSettingClass::connectServer()
{
	int svrid = getSvrNodeId();
	if (svrid >= 0)
		return;
	//启动tcp数据接收,暂存缓存
	m_tcpRecv.startServer();
	/*
	1.发送查找svr广播
	2.等待10s，未等到返回则本机为svr
	3.
	如果本机svr
		1)启动svr指令接收
	如果svr不是本机则
	*/
	g_log(QString(QObject::tr("search server node ......")));
	m_cmdSock.brodcastFindSvr();
	if(!showProgressDialog(100))
	{
		if (QMessageBox::information(0,QObject::tr("prompt"), QObject::tr("The server node was not found. Do you set the native to the server node?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			//本机设为svr
			//启动rtdb ...
			runRTDBwithGui();
			//启动svr
			int curNodeid = getCurrentNodeId();
			if (curNodeid < 0)
			{
				QMessageBox::warning(0, tr("warnning"), tr("current node not config or network not connected!"));
				return;
			}
			setSvrNode(curNodeid);
			m_cmdSock.startSvr();
			m_cmdSock.brodcastSvrNodeId();
		}
		else
		{
			//继续查找svr
			m_cmdSock.brodcastFindSvr();
			showProgressDialog(-1);
		}
	}
	if (getSvrNodeId() < 0)
	{
		if (QMessageBox::information(0, QObject::tr("prompt"), QObject::tr("The server node was not found. Do you quit system?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			exit(0);
	}
	/*
	//同步库文件
	//m_cmdSock.sendCmd2Svr(RTDB_REQUESYNCRTDB);
	m_recvFileProssDlg.setWindowModality(Qt::WindowModal);
	m_recvFileProssDlg.setWindowTitle(tr("Recve RTDB file ..."));
	m_recvFileProssDlg.setMaximum(1);
	m_recvFileProssDlg.show();
	*/
	//启动rtdb ...
	//runRTDBwithGui();
}
void NetSettingClass::reCconnectServer()
{//客户端心跳超时，重新搜索服务器
	g_log(QString(QObject::tr("research server node ......")));
	m_cmdSock.brodcastFindSvr();
	int curNodeid = getCurrentNodeId();
	for (int i = 0; i < 10*curNodeid; i++)//node_id号小的优先设置为server
	{
		if (getSvrNodeId() >= 0)
			break;
		QThread::msleep(100);
	}
	if (getSvrNodeId() < 0)
	{
		g_log(QObject::tr("The server node was not found. Set the native to the server node."));
		//本机设为svr
		//启动svr
		int curNodeid = getCurrentNodeId();
		setSvrNode(curNodeid);
		m_cmdSock.startSvr();
		m_cmdSock.brodcastSvrNodeId();
	}

}
void NetSettingClass::dealLnkSvrResq(quint32 nIp)
{//服务端收到“连接请求”，启动连接
	/*
	1.发起TCP连接，
	2.启动TCP心跳,发送心跳
	发送“加入连接指令”				1
	收到“心跳”
	1.发送“心跳” ，超时则心跳中断		2
	服务端心跳中断：
	1.删除客户端连接、停止发送心跳
	*/
	m_mapCmd[nIp] = new cmdTCPThread(DATARECVPORT);
	m_mapCmd[nIp]->connectToHost(nIp);
	connect(m_mapCmd[nIp], &cmdTCPThread::timeOut, this, [this](quint32 ip){
		m_mapCmd.remove(ip);
	});

	//const char* dbrootpath = getBasePath();//要发送的文件夹
	//QDir dir(QCoreApplication::applicationDirPath());
	////db的相对路径//接收方存放文件夹，相对路径
	//QString relPath = dir.relativeFilePath(dbrootpath);

	////发送整个根目录
	//m_senfile.sendPath("", dbrootpath, relPath);
	////等待发送完成
	//m_senfile.waitForSendFinish();
	////m_cmdSock.sendCmd(nIp, RTDB_ENDSYNCRTDB);//发送同步RTDB完成指令
	//m_senfile.stopSend();
}
void NetSettingClass::endSyncRTDB()
{
	m_recvFileProssDlg.hide();
}
void NetSettingClass::onRecvFile(QString fnm)
{
	m_recvFileProssDlg.setLabelText(QString(tr("recive file %1")).arg(fnm));
	m_recvFileProssDlg.setMaximum(m_recvFileProssDlg.maximum() + 1);
}
void NetSettingClass::onEndRecvFile()
{//客户端传输界面退出按钮
	m_recvFileProssDlg.setValue(m_recvFileProssDlg.value() + 1);
	if (m_recvFileProssDlg.wasCanceled())
	{//此处只隐藏界面，继续传输完
		m_recvFileProssDlg.hide();
	}
}