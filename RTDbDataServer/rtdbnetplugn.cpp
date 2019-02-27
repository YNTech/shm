#include <QtCore>
#include "rtdbnetplugn.h"
#include "rtdb_api.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"

using namespace std;
const char* g_dbNetSetting = "NetSettings";
const char* g_tblNode = "Nodes";
const char* g_tblPS = "PS";
const char* g_tblNode_name = "name";
const char* g_tblDll = "Dll";

RTDbNetPlugn::RTDbNetPlugn()
{
	if(!loadPlugns())
		return;//error
}

RTDbNetPlugn::~RTDbNetPlugn()
{

}
int RTDbNetPlugn::getNodeInd(std::string ndname,std::string *err)
{
	char err1[256];
	void* vals[1000];//假定最大行数
	//std::list<void*> vals;
	int datalen, rownum = 1000, bgrow = 0;
	int ind = 0;
	bool flag;
	while (rownum == 1000)
	{
		if (!rtdb_get_colsdata(g_dbNetSetting, g_tblNode, g_tblNode_name, bgrow, rownum, vals, datalen,err1))
		{
			*err = string("getNodeInd on rtdb_get_colsdata Error:") + err1;
			return -1;
		}
		for (int i = 0; i < rownum; i++)
		{
			if (QString::compare((const char *)(vals[i]), ndname.c_str(), Qt::CaseInsensitive))//strncmp((const char *)(*it), str, dtsize) == 0)
			{
				return ind;
			}
			ind++;
		}
		bgrow += rownum;
	}

	*err = "The Node is not exist！（ Node = " + ndname + ")";
	return -1;

}
struct dbstruct
{
	std::string dbname;
	QList<std::string> tbls;
};
bool RTDbNetPlugn::publish(int curNdInd, std::string *err/* =  nullptr*/)
{
	//list<string> tblnames;
	QMap<int,dbstruct> maptblnames;
	char (*dbnames)[db_name_length];
	int dbnums = 0;
	char cerr[256];
	if (!rtdb_get_dbnames(&dbnames, dbnums, cerr))
		return false;
	char* tblnames[max_tables_num];
	int tblnum = 0;
	for (int i = 0; i < dbnums;i++)
	{
		rtdb_get_tblnames(tblnames, tblnum, dbnames[i], cerr);
		dbstruct dbs;
		dbs.dbname = dbnames[i];
		for (int j = 0; j < tblnum; j++)
			dbs.tbls << tblnames[j];
		maptblnames[i] = dbs;
	}
	//
	int rowcount = 0;
	if (!rtdb_get_rowcount(g_dbNetSetting, g_tblPS, rowcount, cerr))
	{
		*err = string("pub_and_sub on rtdb_get_rowcount Error:") + cerr;
		return false;
	}
	psrowdata* d;
	int rwsize;
	bool ret = true;
	for(int i=0; i<rowcount; i++)
	{
		if(!rtdb_get_rowdata(g_dbNetSetting, g_tblPS, i, (void**)&d, rwsize, cerr))
		{
			*err = "pub_and_sub on rtdb_get_rowdata Error:" + *err;
			return false;
		}
		if(d->ndind == curNdInd)
		{
			psdata data;
			if(d->dbind<0 || d->dbind>=maptblnames.size())
				continue;//error
			data.dbname = maptblnames[d->dbind].dbname;
			if(d->tblind<0 || d->tblind>=maptblnames[d->dbind].tbls.size())
				continue;//error
			data.tblname= maptblnames[d->dbind].tbls[d->tblind];

			if(!rtdb_get_rowdata(data.dbname.c_str(), data.tblname.c_str(), 0, (void**)&data.pdata, (int&)data.rowsize, cerr))
			{
				*err = "pub_and_sub on rtdb_get_rowdata Error2:" + *err;
				continue;//error
			}
			if(d->pub_src != 0)
			{
				data.datasrc = d->pub_src;
				unsigned char src = d->pub_src;
				unsigned char ind = 1;
				while(ind < 8)
				{
					if(src&ind)
					{
						if(m_plugns.contains(ind))//else error
						{
							sender_handle hand = m_plugns[ind]->publish(data.rowsize,data.pdata, data.tblname.c_str(), data.dbname.c_str());
							if(hand == ErrorHandle)
							{
								m_errString = m_plugns[ind]->getLastError();
								ret = false;
							}
							else
								data.hand[ind] = hand;
						}
					}
					ind <<= 1;
				}
				m_lst_pub.push_back(data);
			}
		}
	}
	return ret;
}
bool RTDbNetPlugn::subscribe(int curNdInd, std::string *err)
{
	//getRtDbNames
	char cerr[256];
	//list<string> tblnames;
	QMap<int,dbstruct> maptblnames;
	char (*dbnames)[db_name_length];
	int dbnums = 0;
	if (!rtdb_get_dbnames(&dbnames, dbnums, cerr))
		return false;
	char* tblnames[max_tables_num];
	int tblnum = 0;
	for (int i = 0; i < dbnums; i++)
	{
		rtdb_get_tblnames(tblnames, tblnum, dbnames[i], cerr);
		dbstruct dbs;
		dbs.dbname = dbnames[i];
		for (int j = 0; j < tblnum; j++)
			dbs.tbls << tblnames[j];
		maptblnames[i] = dbs;
	}
	//
	int rowcount = 0;
	if (!rtdb_get_rowcount(g_dbNetSetting, g_tblPS, rowcount, cerr))
	{
		*err = "pub_and_sub on rtdb_get_rowcount Error:" + *err;
		return false;
	}
	psrowdata* d;
	int rwsize;
	bool ret = true;
	for(int i=0; i<rowcount; i++)
	{
		if (!rtdb_get_rowdata(g_dbNetSetting, g_tblPS, i, (void**)&d, rwsize, cerr))
		{
			*err = "pub_and_sub on rtdb_get_rowdata Error:" + *err;
			return false;
		}
		if(d->ndind == curNdInd)
		{
			psdata data;
			if(d->dbind<0 || d->dbind>=maptblnames.size())
				continue;//error
			data.dbname = maptblnames[d->dbind].dbname;
			if(d->tblind<0 || d->tblind>=maptblnames[d->dbind].tbls.size())
				continue;//error
			data.tblname= maptblnames[d->dbind].tbls[d->tblind];

			if (!rtdb_get_rowdata(data.dbname.c_str(), data.tblname.c_str(), 0, (void**)&data.pdata, (int&)data.rowsize, cerr))
			{
				*err = "pub_and_sub on rtdb_get_rowdata Error2:" + *err;
				continue;//error
			}
			if(d->sub_src != 0)
			{
				unsigned char src = d->sub_src;
				unsigned char ind = 1;
				while(ind < 8)
				{
					if(src&ind)
					{
						if(m_plugns.contains(ind))//else error
						{
							sender_handle hand = m_plugns[ind]->subscribe(data.rowsize,data.pdata, data.tblname.c_str(), data.dbname.c_str());
							if(hand == ErrorHandle)
							{
								m_errString = m_plugns[ind]->getLastError();
								ret = false;
							}
						}
					}
					ind <<= 1;
				}
			}
		}
	}
	return ret;
}
bool RTDbNetPlugn::loadPlugns()
{
	//
	char cerr[256];
	int rowcount;
	if (!rtdb_get_rowcount(g_dbNetSetting, g_tblDll, rowcount, cerr))
	{
		m_errString = cerr;
		return false;
	}
	char* val;
	int datalen;
	QMap<QString,unsigned char> plugns;
	for(int r=0; r<rowcount; r++)
	{
		if (rtdb_get_rowdata(g_dbNetSetting, g_tblDll, r, (void**)&val, datalen, cerr))
		{
			m_errString = cerr;
			plugns[QString::fromLatin1((char*)(val+1)).toLower()] = *((unsigned char*)val);
		}
	}
	//
	QDir pluginsDir = QDir(qApp->applicationDirPath());
	pluginsDir.cd("plugins");
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (plugin) 
		{
			NetInterface* plg = qobject_cast<NetInterface*>(plugin);
			QString str = fileName.left(fileName.indexOf('.')).toLower();
			if(plugns.contains(str))
				m_plugns[plugns[str]] = plg;
			else
				loader.unload();
		}
	}
	return true;
}
bool RTDbNetPlugn::initNet(const char* localnd)
{
	int ndind = getNodeInd(localnd, &m_errString);
	if(ndind<0)
		return false;
	bool ret = true;
	ret &= publish(ndind, &m_errString);
	ret &= subscribe(ndind, &m_errString);
	return ret;
}
QList<pNetRunCallback> RTDbNetPlugn::getCallBack()
{
	QList<pNetRunCallback> cb;
	foreach(unsigned char nd, m_plugns.keys())
	{
		cb << m_plugns[nd]->getCallBack();
	}
	return cb;
}
std::string RTDbNetPlugn::getLastError()
{
	return m_errString;
}
bool RTDbNetPlugn::sendData(int index, const char* tblName, const char* dbName)
{
	string strdb(dbName);
	string strtbl(tblName);
	foreach(psdata d,m_lst_pub)
	{
		if(d.tblname==strtbl && d.dbname==strdb)
		{
			bool ret = true;
			foreach(unsigned char ind,d.hand.keys())
			{
				if(!m_plugns[ind]->sendData(index,d.hand[ind]))
				{
					m_errString = m_plugns[ind]->getLastError();
					ret = false;
				}
			}
			return ret;
		}
	}
	return false;
}
sender_handle RTDbNetPlugn::getSenderHandle(const char* tblName,const char* dbName)
{
	string strdb(dbName);
	string strtbl(tblName);

	for(int i=0; i<m_lst_pub.size(); i++)
	{
		if(m_lst_pub[i].tblname==strtbl && m_lst_pub[i].dbname==strdb)
			return i;
	}
	return ErrorHandle;
}
bool RTDbNetPlugn::sendData(int index, sender_handle hdl)
{
	bool ret = true;
	psdata* plst = &m_lst_pub[hdl];
	foreach(unsigned char ind,plst->hand.keys())
	{
		if(!m_plugns[ind]->sendData(index, plst->hand[ind]))
		{
			m_errString = m_plugns[ind]->getLastError();
			ret = false;
		}
	}
	return ret;
}