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
int  getCurrentNodeId()//当前计算机所对应的节点id
{
	if (currentNodeId >= 0)
		return currentNodeId;
	/*
	1)查找当前计算机hostname和ip
	2)对比
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
		// 输出IPV4、IPv6地址
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
unsigned int getSvrAdress()//返回IPv4
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
// Returns:   int		返回有效长度
// Qualifier:
// Parameter: QByteArray & d
// Parameter: const char * dbname
// Parameter: const int bg_addr 打包起始地址
// Parameter: int max_size		最大长度
// 1、表大于max_size，发送max_size长
// 2、表小于max_size，第一个表按bg_addr发送，其余表如不能完整发送则转下一包
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
	//遍历所有表
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
			bgid = i;//起始表id
			n = bg_addr - bg;
			datalen += nLen - n;
			if (datalen >= max_size)//表大于max_size需分包
			{
				datalen = max_size;
				endid = i + 1;
				//datalen = max_size;
				break;
			}
			if (n > 0)//分包
			{
				endid = i + 1;
				//datalen = max_size;
				break;
			}
		}
		else//表小于max_size需多表组包
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
	outs << (endid-bgid);//表数量
	char* pd = (char*)table_mapped(getDefaultDbSets(), dbname, hd->tbl_header[bgid].tbl_name);
	nLen = hd->tbl_header[bgid].row_size*hd->tbl_header[bgid].row_num;
	int dlen = qMin(datalen, nLen - n);
	outs << bgid << hd->tbl_header[bgid].row_num << n << dlen;//tbl_id,表行数，表内位置，长度
	outs.writeRawData(pd + n, dlen);
	for (i = bgid + 1; i < endid; i++)
	{
		char* pd = (char*)table_mapped(getDefaultDbSets(), dbname, hd->tbl_header[i].tbl_name);
		nLen = hd->tbl_header[i].row_size*hd->tbl_header[i].row_num;
		outs << i << hd->tbl_header[i].row_num << int(0) << nLen;//tbl_id,表行数，表内位置，长度
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


