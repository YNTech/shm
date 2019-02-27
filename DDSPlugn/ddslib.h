#pragma once
#ifndef __DDSLIB_H__
#define __DDSLIB_H__
#include <string>
#include "ddalib_export.h"
#include "CommBase.h"
#include <vector>
#include <list>

class DDSEntityManager;
struct ddslibdata;
class dds_wait;
class ddslib: public CommBase<ddslib>
{
	__CommBase(ddslib)
public:
	sender_handle publish(int dataSize, const void* pStData,  const char* topicName, const char* partitiontName = defaultPartitiontName);
	receiver_handle subscribe(int dataSize, void* pStData, const char* topicName, const char* partitiontName = defaultPartitiontName);
	bool sendData(int index, sender_handle hand);
	int recvData(receiver_handle hand);
	int recvDatas(std::list<int>& index, char* data,const int max_recv,receiver_handle hand);
	inline const char* getLastError(){ return m_errString->c_str(); };
	dds_wait* createWait();
	void releaseWait(dds_wait* wt);
private:
	ddslib(void);
	~ddslib(void);

	DDSEntityManager* m_ddsmgr;
	std::string*	  m_errString;
public:
	ddslibdata*       m_psdata;
};
//////////////////////////////////////////////////////////////////////////
struct dds_wait_data;
class dds_wait
{
public:
	dds_wait(DDSEntityManager* ddsmgr,ddslibdata* psdata);
	~dds_wait();
	bool addWait(const char *topicName, const char *partitiontName= defaultPartitiontName);
	bool addWait(receiver_handle hand);
	bool wait(double ftm=-1);//等待ftm秒或有指定数据到达函数返回，用于等待接收数据时使用,-1表示无限等待，返回值：是否有数据到达
	const char* getLastError();
private:
	DDSEntityManager* m_ddsmgr;
	ddslibdata* m_psdata;
	dds_wait_data* m_waitdata;
};
//////////////////////////////////////////////////////////////////////////
#include "DDSEntityManager.h"
#include "DDSLibDataDcps_impl.h"
using namespace DDSLib;

struct DDSReader 
{
	DDSDataDataReader_var datareader;
	DDSLib::DDSDataSeq datalst;
	DDS::SampleInfoSeq infolst;
	void * pdata;//数据起始地址
	c_ulong datalen;
};
struct DDSWriter 
{
	DDSDataDataWriter_var datawriter;
	const void * pdata;//数据起始地址
	c_ulong datalen;
};
//为避免每次都进行名称匹配提高效率，提供handle接口
struct ddslibdata
{
	std::vector<DDSReader> m_readers;
	std::vector<DDSWriter> m_writers;
};


#endif//__DDSLIB_H__