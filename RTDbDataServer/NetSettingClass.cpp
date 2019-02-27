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
	connect(&m_cmdSock, &udpSock::changeServer, this, [=](){//�л�������
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
	//����tcp���ݽ���,�ݴ滺��
	m_tcpRecv.startServer();
	/*
	1.���Ͳ���svr�㲥
	2.�ȴ�10s��δ�ȵ������򱾻�Ϊsvr
	3.
	�������svr
		1)����svrָ�����
	���svr���Ǳ�����
	*/
	g_log(QString(QObject::tr("search server node ......")));
	m_cmdSock.brodcastFindSvr();
	if(!showProgressDialog(100))
	{
		if (QMessageBox::information(0,QObject::tr("prompt"), QObject::tr("The server node was not found. Do you set the native to the server node?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			//������Ϊsvr
			//����rtdb ...
			runRTDBwithGui();
			//����svr
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
			//��������svr
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
	//ͬ�����ļ�
	//m_cmdSock.sendCmd2Svr(RTDB_REQUESYNCRTDB);
	m_recvFileProssDlg.setWindowModality(Qt::WindowModal);
	m_recvFileProssDlg.setWindowTitle(tr("Recve RTDB file ..."));
	m_recvFileProssDlg.setMaximum(1);
	m_recvFileProssDlg.show();
	*/
	//����rtdb ...
	//runRTDBwithGui();
}
void NetSettingClass::reCconnectServer()
{//�ͻ���������ʱ����������������
	g_log(QString(QObject::tr("research server node ......")));
	m_cmdSock.brodcastFindSvr();
	int curNodeid = getCurrentNodeId();
	for (int i = 0; i < 10*curNodeid; i++)//node_id��С����������Ϊserver
	{
		if (getSvrNodeId() >= 0)
			break;
		QThread::msleep(100);
	}
	if (getSvrNodeId() < 0)
	{
		g_log(QObject::tr("The server node was not found. Set the native to the server node."));
		//������Ϊsvr
		//����svr
		int curNodeid = getCurrentNodeId();
		setSvrNode(curNodeid);
		m_cmdSock.startSvr();
		m_cmdSock.brodcastSvrNodeId();
	}

}
void NetSettingClass::dealLnkSvrResq(quint32 nIp)
{//������յ����������󡱣���������
	/*
	1.����TCP���ӣ�
	2.����TCP����,��������
	���͡���������ָ�				1
	�յ���������
	1.���͡������� ����ʱ�������ж�		2
	����������жϣ�
	1.ɾ���ͻ������ӡ�ֹͣ��������
	*/
	m_mapCmd[nIp] = new cmdTCPThread(DATARECVPORT);
	m_mapCmd[nIp]->connectToHost(nIp);
	connect(m_mapCmd[nIp], &cmdTCPThread::timeOut, this, [this](quint32 ip){
		m_mapCmd.remove(ip);
	});

	//const char* dbrootpath = getBasePath();//Ҫ���͵��ļ���
	//QDir dir(QCoreApplication::applicationDirPath());
	////db�����·��//���շ�����ļ��У����·��
	//QString relPath = dir.relativeFilePath(dbrootpath);

	////����������Ŀ¼
	//m_senfile.sendPath("", dbrootpath, relPath);
	////�ȴ��������
	//m_senfile.waitForSendFinish();
	////m_cmdSock.sendCmd(nIp, RTDB_ENDSYNCRTDB);//����ͬ��RTDB���ָ��
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
{//�ͻ��˴�������˳���ť
	m_recvFileProssDlg.setValue(m_recvFileProssDlg.value() + 1);
	if (m_recvFileProssDlg.wasCanceled())
	{//�˴�ֻ���ؽ��棬����������
		m_recvFileProssDlg.hide();
	}
}