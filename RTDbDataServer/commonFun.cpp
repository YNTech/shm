#include <QHostInfo>
#include <QtWidgets>
#include "MyDlg.h"
#include "commonFun.h"
#include "header_SysConfig.h"
#include "..\AlarmBoard\AlarmBoardExt\alarmboardext.h"
int currentNodeId = -1;
int svrNodeId = -1;
unsigned int svrAddress = 0x7f000001;
unsigned short svrPort = 3477;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
int  getCurrentNodeId()//��ǰ���������Ӧ�Ľڵ�id
{
	if (currentNodeId >= 0)
		return currentNodeId;
	/*
	1)���ҵ�ǰ�����hostname��ip
	2)�Ա�
	*/
	QStringList hostinfo;
	hostinfo << QHostInfo::localHostName();
	QHostInfo host = QHostInfo::fromName(hostinfo[0]);
	if (host.error() != QHostInfo::NoError) {
		g_log(QString("Lookup failed:%1").arg(host.errorString()), Log_Warning);
		return currentNodeId;
	}
	foreach(const QHostAddress &address, host.addresses()) 
	{
		// ���IPV4��IPv6��ַ
		if (address.protocol() == QAbstractSocket::IPv4Protocol)
			hostinfo << address.toString();
	}
	//g_log(hostinfo.join(","));

	if (header_SysConfig == nullptr || SysConfig_Nodes == nullptr)
	{
		header_SysConfig = (TYPE_header_RTDb)header_rtdb(getDefaultDbSets(), "SysConfig");
		SysConfig_Nodes = (TYPE_Nodes)table_mapped(getDefaultDbSets(), "SysConfig", "Nodes");
		if (header_SysConfig == nullptr || SysConfig_Nodes == nullptr)
			return currentNodeId;
	}

	for (int i = 0; i < header_SysConfig->tbl_nums; i++)
	{
		if (strcmp(header_SysConfig->tbl_header[i].tbl_name, "Nodes") == 0)
		{
			for (int j = 0; j < header_SysConfig->tbl_header[i].row_num; j++)
			{
				if (hostinfo.contains(SysConfig_Nodes[j].host))
				{
					currentNodeId = j;
					return currentNodeId;
				}
			}
			break;
		}
	}
	return currentNodeId;
}
int  getSvrNodeId()
{
	return svrNodeId;
}

bool g_curIsSvr()
{
	return getCurrentNodeId() == svrNodeId;
}
void setSvrNode(int nid, unsigned int addr, unsigned short port)
{
	if (svrNodeId != nid)
	{
		svrNodeId = nid;
		svrPort = port;
		svrAddress = addr;
	}
}
unsigned int getSvrAdress()//����IPv4
{
	return svrAddress;
}
unsigned short getSvrPort()
{
	return svrPort;
}

//************************************
// Method:    encodeRtdb_Db
// FullName:  encodeRtdb_Db
// Access:    public 
// Returns:   int		������Ч����
// Qualifier:
// Parameter: QByteArray & d
// Parameter: const char * dbname
// Parameter: const int bg_addr �����ʼ��ַ
// Parameter: int max_size		��󳤶�
// 1�������max_size������max_size��
// 2����С��max_size����һ����bg_addr���ͣ�������粻������������ת��һ��
//************************************
int encodeRtdb_Db(QByteArray& d, const char* dbname,const int bg_addr,int max_size)
{
	//db header
	TYPE_header_RTDb	hd = (TYPE_header_RTDb)header_rtdb(getDefaultDbSets(), dbname);
	if (hd == nullptr)
		return -1;
	QDataStream outs(&d, QIODevice::WriteOnly);
	outs << dbname;
	int nLen;
	//db 
	//�������б�
	bool isEnd = false;
	int bg = 0, n = 0, bgid = 0, endid = hd->tbl_nums - 1, datalen = 0;
	int i = 0;
	for (; i < hd->tbl_nums; i++)
	{
		char* pd = (char*)table_mapped(getDefaultDbSets(), dbname, hd->tbl_header[i].tbl_name);
		nLen = hd->tbl_header[i].row_size*hd->tbl_header[i].row_num;
		//outs << ushort(i) << hd->tbl_header[i].row_num;
		if (bg + nLen <= bg_addr)
			bg += nLen;
		else if (n==0)
		{
			bgid = i;//��ʼ��id
			n = bg_addr - bg;
			datalen += nLen - n;
			if (datalen >= max_size)//�����max_size��ְ�
			{
				datalen = max_size;
				endid = i + 1;
				//datalen = max_size;
				break;
			}
			if (n > 0)//�ְ�
			{
				endid = i + 1;
				//datalen = max_size;
				break;
			}
		}
		else//��С��max_size�������
		{
			if (datalen + nLen >= max_size)
			{
				endid = i;
				break;
			}
			datalen += nLen;
		}
	}
	if (i = hd->tbl_nums)
	{
		endid = i;
		isEnd = true;
	}
	outs << (endid-bgid);//������
	char* pd = (char*)table_mapped(getDefaultDbSets(), dbname, hd->tbl_header[bgid].tbl_name);
	nLen = hd->tbl_header[bgid].row_size*hd->tbl_header[bgid].row_num;
	int dlen = qMin(datalen, nLen - n);
	outs << bgid << hd->tbl_header[bgid].row_num << n << dlen;//tbl_id,������������λ�ã�����
	outs.writeRawData(pd + n, dlen);
	for (i = bgid + 1; i < endid; i++)
	{
		char* pd = (char*)table_mapped(getDefaultDbSets(), dbname, hd->tbl_header[i].tbl_name);
		nLen = hd->tbl_header[i].row_size*hd->tbl_header[i].row_num;
		outs << i << hd->tbl_header[i].row_num << int(0) << nLen;//tbl_id,������������λ�ã�����
		outs.writeRawData(pd, nLen);
	}
	int n1 = d.size();
	d = qCompress(d);
	g_log(Log_Debug, "compress rate = %8.2f", d.size() / (double)n1);
	if (isEnd)
		return 0;
	else
		return datalen;
}
bool decodeRtdb_Db(QByteArray& d)
{
	//db header
	d = qUncompress(d);
	char* dbname;
	int tbnum;
	QDataStream in(&d, QIODevice::ReadOnly);
	in >> dbname >> tbnum;
	TYPE_header_RTDb	hd = (TYPE_header_RTDb)header_rtdb(getDefaultDbSets(), dbname);
	if (hd == nullptr)
		return false;
	int tblid, rownum, bg, len;
	for (int i = 0; i < tbnum;i++)
	{
		in >> tblid >> rownum >> bg >> len;
		if (tblid<0 || tblid>hd->tbl_nums)
		{
			g_log(Log_Error, "decodeRtdb_Db: recive error table id = %d", tblid);
			return false;
		}
		char* pd = (char*)table_mapped(getDefaultDbSets(), dbname, hd->tbl_header[tblid].tbl_name);
		if (bg==0)
			hd->tbl_header[tblid].row_num = rownum;
		in.readRawData(pd + bg, len);
	}
	return true;
}
void runRTDBwithGui()
{
	MyDlg dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		char err[256];
		QByteArray dbsetsname = dlg.m_dbsetsnm->currentText().toLatin1();
		QByteArray dbrootph = dlg.m_leRootPath->text().toLatin1();
		if (!loadRTDb(dbsetsname.data(), dbrootph.data(), err))
			g_log_system(Log_Error, "ERROR:%s", err);
		setDefauluDbSets(dbsetsname.data());
	}
}


