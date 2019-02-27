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
	bool wait(double ftm=-1);//�ȴ�ftm�����ָ�����ݵ��ﺯ�����أ����ڵȴ���������ʱʹ��,-1��ʾ���޵ȴ�������ֵ���Ƿ������ݵ���
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
	void * pdata;//������ʼ��ַ
	c_ulong datalen;
};
struct DDSWriter 
{
	DDSDataDataWriter_var datawriter;
	const void * pdata;//������ʼ��ַ
	c_ulong datalen;
};
//Ϊ����ÿ�ζ���������ƥ�����Ч�ʣ��ṩhandle�ӿ�
struct ddslibdata
{
	std::vector<DDSReader> m_readers;
	std::vector<DDSWriter> m_writers;
};


#endif//__DDSLIB_H__