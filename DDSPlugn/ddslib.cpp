
#include "ddslib.h"


ddslib::ddslib(void):m_ddsmgr(new DDSEntityManager),m_psdata(new ddslibdata),m_errString(new std::string)
{
}

ddslib::~ddslib(void)
{
	delete m_ddsmgr;
	delete m_psdata;
	delete m_errString;
}
sender_handle ddslib::publish(int dataSize, const void* pStData, const char* topicName, const char* partitiontName)
{
	if(!m_ddsmgr->hasParticipant(partitiontName))
	{
		if(!m_ddsmgr->createParticipant(partitiontName))
		{
			*m_errString = m_ddsmgr->getErrorSting();
			return ErrorHandle;
		}
	}
	char tmp[64];
	sprintf(tmp,"DDSLib::%s",topicName);
	const gapi_char *p[]={__get_DDSData_MetaDescriptor(topicName,dataSize)};
	DDSDataTypeSupport_var mt = new DDSDataTypeSupport(tmp,p);
	if (!m_ddsmgr->registerType(mt.in(),topicName,partitiontName))
	{
		*m_errString = m_ddsmgr->getErrorSting();
		return ErrorHandle;
	}
	if(m_ddsmgr->hasTopic(topicName,partitiontName))
	{
		if (!m_ddsmgr->createTopic(topicName,partitiontName,dataSize))
		{
			*m_errString = m_ddsmgr->getErrorSting();
			return ErrorHandle;
		}
	}
	if(!m_ddsmgr->createPublisher(partitiontName))
	{
		*m_errString = m_ddsmgr->getErrorSting();
		return ErrorHandle;
	}
	if(!m_ddsmgr->createWriter(false,topicName,partitiontName))
	{
		*m_errString = m_ddsmgr->getErrorSting();
		return ErrorHandle;
	}
	//
	DDSWriter d;
	d.datalen = dataSize;
	d.pdata    = pStData;
	DataWriter_var dwriter = m_ddsmgr->getWriter(topicName,partitiontName);
	d.datawriter = DDSDataDataWriter::_narrow(dwriter.in());
	if(!checkHandle(d.datawriter.in(), "ddslib::publish",m_errString))
		return ErrorHandle;
	m_psdata->m_writers.push_back(d);
	//
	return m_psdata->m_writers.size()-1;
}
receiver_handle ddslib::subscribe(int dataSize, void* pStData, const char* topicName, const char* partitiontName)
{
	if(!m_ddsmgr->hasParticipant(partitiontName))
	{
		if(!m_ddsmgr->createParticipant(partitiontName))
		{
			*m_errString = m_ddsmgr->getErrorSting();
			return ErrorHandle;
		}
	}
	char tmp[64];
	sprintf(tmp,"DDSLib::%s",topicName);
	const gapi_char *p[]={__get_DDSData_MetaDescriptor(topicName,dataSize)};
	DDSDataTypeSupport_var mt = new DDSDataTypeSupport(tmp,p);
	if(!m_ddsmgr->registerType(mt.in(),topicName,partitiontName))
	{
		*m_errString = m_ddsmgr->getErrorSting();
		return ErrorHandle;
	}
	if(m_ddsmgr->hasTopic(topicName,partitiontName))
	{
		if(!m_ddsmgr->createTopic(topicName,partitiontName,dataSize))
		{
			*m_errString = m_ddsmgr->getErrorSting();
			return ErrorHandle;
		}
	}
	if(!m_ddsmgr->createSubscriber(partitiontName))
	{
		*m_errString = m_ddsmgr->getErrorSting();
		return ErrorHandle;
	}
	if(!m_ddsmgr->createReader(topicName,partitiontName))
	{
		*m_errString = m_ddsmgr->getErrorSting();
		return ErrorHandle;
	}
	DDSReader d;
	d.datalen = dataSize;
	d.pdata    = pStData;
	DataReader_var dreader = m_ddsmgr->getReader(topicName,partitiontName);
	d.datareader = DDSDataDataReader::_narrow(dreader.in());
	if(!checkHandle(d.datareader.in(), "ddslib::subscribe",m_errString))
		return ErrorHandle;
	m_psdata->m_readers.push_back(d);
	//
	return m_psdata->m_readers.size()-1;
}
bool ddslib::sendData   (int index, sender_handle hand)
{
	if(hand== ErrorHandle || hand >= m_psdata->m_writers.size())
	{
		*m_errString = "Error handle!";
		return false;
	}
	_DDSLib_DDSData d;
	d.ind = (hand<<16)|(0x0000FFFF&index);//
	ReturnCode_t status = m_psdata->m_writers[hand].datawriter->write(d, DDS::HANDLE_NIL);
	return checkStatus(status, "ddslib::sendData",m_errString);
}

int ddslib::recvData(receiver_handle hand)
{
	ReturnCode_t status = m_psdata->m_readers[hand].datareader->take((m_psdata->m_readers[hand].datalst),(m_psdata->m_readers[hand].infolst) ,-1,ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if(!checkStatus(status, "ddslib::recvData",m_errString))
		return -2;
	if(status == DDS::RETCODE_OK)
	{
		int len = m_psdata->m_readers[hand].datalst.length();
		m_psdata->m_readers[hand].datareader->return_loan((m_psdata->m_readers[hand].datalst),(m_psdata->m_readers[hand].infolst));
		return len;
	}
	return 0;
}

dds_wait* ddslib::createWait()
{
	dds_wait* wt = new dds_wait(m_ddsmgr,m_psdata);
	return wt;
}
void ddslib::releaseWait(dds_wait* wt)
{
	if(wt)
		delete wt;
}

//////////////////////////////////////////////////////////////////////////
struct dds_wait_data
{
	WaitSet wait;
	ConditionSeq condSeq;
	string errString;
};
dds_wait::dds_wait(DDSEntityManager* ddsmgr,ddslibdata* psdata):m_ddsmgr(ddsmgr),m_psdata(psdata),m_waitdata(new dds_wait_data)
{

}
dds_wait::~dds_wait()
{
	delete m_waitdata;
}
bool dds_wait::addWait(const char *topicName, const char *partitiontName)
{
	DataReader_var reader = m_ddsmgr->getReader(topicName,partitiontName);
	if(!reader)
	{
		m_waitdata->errString="dds_wait::addWait:To obtain DataReader failed!";
		return false;
	}
	ReadCondition_var readCond = reader->create_readcondition(ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if(!checkHandle(readCond.in(), "dds_wait::addWait::create_readcondition",&m_waitdata->errString))
		return false;
	ReturnCode_t status = m_waitdata->wait.attach_condition(readCond.in());
	if(!checkStatus(status, "dds_wait::addWait::attach_condition",&m_waitdata->errString))
		return false;
	m_waitdata->condSeq.length(m_waitdata->condSeq.length()+1);
	return true;
}
bool dds_wait::addWait(receiver_handle hand)
{
	DataReader_var reader = m_psdata->m_readers[hand].datareader;
	if(!reader)
	{
		m_waitdata->errString="dds_wait::addWait:To obtain DataReader failed!";
		return false;
	}
	ReadCondition_var readCond = reader->create_readcondition(ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if(!checkHandle(readCond.in(), "dds_wait::addWait::create_readcondition",&m_waitdata->errString))
		return false;
	ReturnCode_t status = m_waitdata->wait.attach_condition(readCond.in());
	if(!checkStatus(status, "dds_wait::addWait::attach_condition",&m_waitdata->errString))
		return false;
	m_waitdata->condSeq.length(m_waitdata->condSeq.length()+1);
	return true;
}
bool dds_wait::wait(double ftm)//等待ftm秒或有指定数据到达函数返回，用于等待接收数据时使用,-1表示无限等待，返回值：是否有数据到达
{
	DDS::Duration_t dt;
	if(ftm>=0)
	{
		dt.sec = (long)ftm;
		dt.nanosec = (unsigned int)((ftm - dt.sec)*1000000000);
	}
	else
		dt = DDS::DURATION_INFINITE;
	ReturnCode_t status = m_waitdata->wait.wait(m_waitdata->condSeq, dt);
	return status == DDS::RETCODE_OK;
}
const char* dds_wait::getLastError()
{
	return m_waitdata->errString.c_str();
}
