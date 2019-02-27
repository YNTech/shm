#include <QtCore>
#include "rtdbnetext.h"
#include "rtdb_api.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"

using namespace std;
const string g_dbNetSetting = "NetSetting";
const string g_tblNode = "Node";
const string g_tblPS = "PS";
const string g_tblNode_name = "name";
const string g_tblDll = "Dll";
unsigned int g_localHandle = 1;

RTDbNetExt::RTDbNetExt()
{

}

RTDbNetExt::~RTDbNetExt()
{

}
int RTDbNetExt::getNodeInd(std::string ndname,std::string *err)
{
	//std::string err;
	list<void*> ndnames;
	int dtsize =0;
	if(!rtdb_get_colsdata(g_dbNetSetting, g_tblNode, g_tblNode_name, ndnames, dtsize, err));
	{
		*err = "getNodeInd on rtdb_get_colsdata Error:"+(*err);
		return -1;
	}
	list<void*>::const_iterator it= ndnames.begin();
	const char* str = ndname.c_str();
	int ind = -1;
	while(it != ndnames.end())
	{
		if(strncmp((const char *)(*it), str, dtsize) == 0)
			break;
		ind++;
		it++;
	}
	if(ind < 0)
		*err = "The table is not exist£¡£¨Db = " + g_dbNetSetting + ", Table = " + ndname;

	return ind;
}

bool RTDbNetExt::pub_and_sub(int curNdInd, std::string *err)
{
	//getRtDbNames
	list<string> dbnames;
	list<string> tblnames;
	QMap<std::string,QList<std::string>> maptblnames;
	if(!rtdb_get_dbnames(dbnames,err))
		return false;
	list<string>::iterator it = dbnames.begin();
	while(it != dbnames.end())
	{
		rtdb_get_tblnames(tblnames, *it, err);
		maptblnames[*it] = QList<string>::fromStdList(tblnames);
		it++;
	}
	//
	int rowcount = 0;
	if(!rtdb_get_rowcount(g_dbNetSetting, g_tblPS, rowcount, err));
	{
		*err = "pub_and_sub on rtdb_get_rowcount Error:" + *err;
		return false;
	}
	psrowdata* d;
	int rwsize;
	bool ret = true;
	for(int i=0; i<rowcount; i++)
	{
		if(!rtdb_get_rowdata(g_dbNetSetting, g_tblPS, i, (void**)&d, rwsize, err));
		{
			*err = "pub_and_sub on rtdb_get_rowdata Error:" + *err;
			return false;
		}
		if(d->ndind == curNdInd)
		{
			psdata data;
			if(d->dbind<0 || d->dbind>=maptblnames.size())
				continue;//error
			data.dbname = maptblnames.keys().value(d->dbind);
			if(d->tblind<0 || d->tblind>=maptblnames[data.dbname].size())
				continue;//error
			data.tblname= maptblnames[data.dbname].value(d->tblind);

			if(!rtdb_get_rowdata(data.dbname, data.tblname, 0, (void**)&data.pdata, (int&)data.rowsize, err));
			{
				*err = "pub_and_sub on rtdb_get_rowdata Error2:" + *err;
				continue;//error
			}
			if(d->pub_src != 0)
			{
				data.datasrc = d->pub_src;
				data.localHand = g_localHandle++;
				unsigned char src = d->pub_src;
				unsigned char ind = 1;
				while(src>0)
				{
					if(src&ind)
					{
						if(m_plugns.contains(ind))//else error
						{
							sender_handle hand = m_plugns[ind]->publish(data.rowsize, data.tblname.c_str(), data.dbname.c_str());
							if(hand == ErrorHandle)
							{
								m_errString = m_plugns[ind]->getLastError();
								ret = false;
							}
							else
								data.hand[ind] = hand;
						}
					}
					src >>= 1;
					ind <<= 1;
				}
				m_lst_pub.push_back(data);
			}
			else
			{
				unsigned char src = d->sub_src;
				unsigned char ind = 1;
				while(src>0)
				{
					if(src&ind)
					{
						if(m_plugns.contains(ind))//else error
						{
							sender_handle hand = m_plugns[ind]->subscribe(data.rowsize, data.tblname.c_str(), data.dbname.c_str());
							if(hand == ErrorHandle)
							{
								m_errString = m_plugns[ind]->getLastError();
								ret = false;
							}
						}
					}
					src >>= 1;
					ind <<= 1;
				}
			}
		}
	}
	return ret;
}
bool RTDbNetExt::loadPlugns()
{
	//
	int rowcount;
	if(!rtdb_get_rowcount(g_dbNetSetting,g_tblDll,rowcount,&m_errString))
		return false;
	char* val;
	int datalen;
	QMap<QString,unsigned char> plugns;
	for(int r=0; r<rowcount; r++)
	{
		if(rtdb_get_rowdata(g_dbNetSetting,g_tblDll,r,(void**)&val,datalen,&m_errString))
		{
			plugns[QString::fromLatin1((char*)(val+1),16).toLower()] = *((unsigned char*)val);
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
bool RTDbNetExt::initNet(const char* localnd)
{
	int ndind = getNodeInd(localnd, &m_errString);
	if(ndind<0)
		return false;
	return pub_and_sub(ndind, &m_errString);
}
QList<pNetRunCallback> RTDbNetExt::getCallBack()
{
	QList<pNetRunCallback> cb;
	foreach(unsigned char nd, m_plugns.keys())
	{
		cb << m_plugns[nd]->getCallBack();
	}
	return cb;
}
std::string RTDbNetExt::getLastError()
{
	return m_errString;
}
void RTDbNetExt::sendData(int index,const void* data,const char* tblName,const char* dbName)
{
	string strdb(dbName);
	string strtbl(tblName);
	foreach(psdata d,m_lst_pub)
	{
		if(d.tblname==strtbl && d.dbname==strdb)
		{
			//sender_handle hand = d.datasrc & 0x00FFFFFF;
			//unsigned char plugnInd = d.datasrc>>24;
			//m_plugns[plugnInd]->sendData(hand,);
			break;
		}
	}
}
sender_handle RTDbNetExt::getSenderHandle(int index,const char* tblName,const char* dbName)
{
	string strdb(dbName);
	string strtbl(tblName);
	foreach(psdata d,m_lst_pub)
	{
		if(d.tblname==strtbl && d.dbname==strdb)
		{
			return d.localHand;
			break;
		}
	}
	return ErrorHandle;
}
void RTDbNetExt::sendData(sender_handle hdl, int index)
{

}