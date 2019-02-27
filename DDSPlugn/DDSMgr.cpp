#include <Windows.h>
#include "DDSMgr.h"
#include "ccpp_dds_dcps.h"
#include "DDSLibDataDcps.h"
#include "CheckStatus.h"
#include <map>
#include "rapidxml.hpp"
#include <string.h>
#include <process.h>
using namespace DDS;

DWORD WINAPI starDDSSvrRun(LPVOID lpThreadParameter);
class DDSMgrData
{
	friend DDSMgr;
private:
	//DomainParticipantFactory_var dpf;
	//DomainParticipant_var participant;
	CMParticipantBuiltinTopicDataDataReader_var participantReader;
	DomainId_t domain;
	Topic_var m_CMParticipantTopic;
	string m_errString;
	list<DDSParticpantProduct> m_lstParticpantProduct;
	unsigned int m_currentSysId;
	//
	map<DDSMgr::CallBackType,DDSMgr::DDSParticpantCB> m_cbk;
	HANDLE	m_hdRun;

};

DDSMgr::DDSMgr(void):m_ddsmgrData(new DDSMgrData)
{
	if(!init())
	{
		printf("DDSMgr 初始化失败：%s\n",getLastError().c_str());
	}
}


DDSMgr::~DDSMgr(void)
{
	delete m_ddsmgrData;
}
DDSMgr & DDSMgr::GetInstance() 
{  
	static DDSMgr instance;    
	return instance;  
}  
DDSMgr* DDSMgr::GetInstancePtr() 
{   
	return &GetInstance();  
}
bool DDSMgr::init()
{
	printf("init ddsmgr.....\n");
	DomainParticipantFactory_var dpf = DomainParticipantFactory::get_instance();
	if(!checkHandle( dpf.in(), "DDSMgr::init:get_instance",&m_ddsmgrData->m_errString))
		return false;
	DomainParticipant_var   participant = dpf->create_participant(DOMAIN_ID_DEFAULT, PARTICIPANT_QOS_DEFAULT, NULL,
		STATUS_MASK_NONE);
	if(!checkHandle( participant.in(),	"DDSMgr::init:create_participant",&m_ddsmgrData->m_errString))
		return false;
	//
	
	Subscriber_var builtinSubscriber = participant->get_builtin_subscriber();
	DataReader_var reader = builtinSubscriber->lookup_datareader("CMParticipant");
	//Topic_var topic = 		  participant->find_topic("CMParticipant",Duration_t());
	//Subscriber_var builtinSubscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, NULL, STATUS_MASK_NONE);
	//DataReader_var reader1 = builtinSubscriber->lookup_datareader("CMParticipant");
	//DataReader_var reader = builtinSubscriber->create_datareader(topic.in(),DATAREADER_QOS_USE_TOPIC_QOS, NULL, STATUS_MASK_NONE);

	m_ddsmgrData->participantReader =CMParticipantBuiltinTopicDataDataReader::_narrow(reader.in());
	if(!checkHandle(m_ddsmgrData->participantReader.in(), "DDSMgr::init:ParticipantBuiltinTopicDataDataReader_narrow",&m_ddsmgrData->m_errString))
		return false;
	m_ddsmgrData->participantReader->wait_for_historical_data(DDS::DURATION_INFINITE);

	m_ddsmgrData->m_cbk[CBK_Particpant]=NULL;
	//
	printf("init ddsmgr complete\n");
	return true;
}
string DDSMgr::getLastError()
{
	return m_ddsmgrData->m_errString;
}
list<DDSParticpantProduct>* DDSMgr::getAllHistoricalParticpant()
{
	return  &m_ddsmgrData->m_lstParticpantProduct;
}
bool DDSMgr::setCallBack(CallBackType typ,DDSParticpantCB pcb)
{
	if(m_ddsmgrData->m_cbk[typ]!=NULL)
	{
		m_ddsmgrData->m_errString = "回调函数已被其他进程设置！";
		return false;
	}
	m_ddsmgrData->m_cbk[typ] = pcb;
	return true;
}
unsigned int DDSMgr::getCurrentSystemId()
{
	CMParticipantBuiltinTopicDataSeq data;
	SampleInfoSeq info;
	////获取当前程序名
	//TCHAR szPath[MAX_PATH];
	//if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	//{
	//	m_ddsmgrData->m_errString ="DDSMgr::getCurrentSystemId:GetModuleFileName failed (%d)\n", GetLastError();
	//	return 0;
	//}
	//char* p = strrchr(szPath, '\\');
	//string curtExecName = p+1;
	int iPid = (int)getpid();
	//
	ReturnCode_t status = m_ddsmgrData->participantReader->take(data, info, LENGTH_UNLIMITED,ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if(!checkStatus(status, "DDS::ParticipantBuiltinTopicDataDataReader::take",&m_ddsmgrData->m_errString))
		printf("DDSMgr 读取participant数据失败：%s\n",getLastError().c_str());
	if (status == DDS::RETCODE_OK)
	{
		for (unsigned int i = 0; i < data.length(); i++)
		{
			int loalid=data[i].key[1];
			if(loalid>200)
			{
				DDSParticpantProduct pdt;
				getProductFromXml(data[i].product.value,pdt);
				if(iPid == pdt.PID)
					return data[i].key[0];
			}
		}
	}
	m_ddsmgrData->participantReader->return_loan(data, info);
	return 0;
}
void DDSMgr::run()
{
	ReadCondition_var readCond = m_ddsmgrData->participantReader->create_readcondition(ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if(!checkHandle(readCond.in(), "DDS::DataReader::create_readcondition",&m_ddsmgrData->m_errString))
	{
		printf("DDSMgr 初始化失败：%s\n",getLastError().c_str());
		return;
	}
	WaitSet aWaitSet;
	ReturnCode_t status = aWaitSet.attach_condition(readCond.in());
	if(!checkStatus(status, "DDS::WaitSet::attach_condition",&m_ddsmgrData->m_errString))
	{
		printf("DDSMgr 初始化失败：%s\n",getLastError().c_str());
		return;
	}
	ConditionSeq condSeq;
	condSeq.length(1);
	status = aWaitSet.wait(condSeq, DDS::DURATION_INFINITE);
	CMParticipantBuiltinTopicDataSeq data;
	SampleInfoSeq info;
	m_ddsmgrData->m_lstParticpantProduct.clear();
	while(m_isRun)
	{
		status = m_ddsmgrData->participantReader->take(data, info, LENGTH_UNLIMITED,ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if(!checkStatus(status, "DDS::CMParticipantBuiltinTopicDataDataReader::take",&m_ddsmgrData->m_errString))
			printf("DDSMgr 读取participant数据失败：%s\n",getLastError().c_str());
		if (status == DDS::RETCODE_OK)
		{
			for (unsigned int i = 0; i < data.length(); i++)
			{
				int loalid=data[i].key[1];
				if(loalid>200)
				{
					list<DDSParticpantProduct>::iterator it = m_ddsmgrData->m_lstParticpantProduct.begin();
					while(it!=m_ddsmgrData->m_lstParticpantProduct.end())
					{
						if(it->systemid==data[i].key[0])
						{
							it->instance_states = info[i].instance_state;
							if(m_ddsmgrData->m_cbk[CBK_Particpant])
								m_ddsmgrData->m_cbk[CBK_Particpant](*it);
							break;
						}
						it++;
					}
					if(it==m_ddsmgrData->m_lstParticpantProduct.end())
					{
						DDSParticpantProduct pdt;
						getProductFromXml(data[i].product.value,pdt);
						pdt.systemid = data[i].key[0];
						pdt.instance_states = info[i].instance_state;
						if(m_ddsmgrData->m_cbk[CBK_Particpant])
							m_ddsmgrData->m_cbk[CBK_Particpant](pdt);
						m_ddsmgrData->m_lstParticpantProduct.push_back(pdt);
					}
				}
			}
		}
		m_ddsmgrData->participantReader->return_loan(data, info);
		status = aWaitSet.wait(condSeq, DDS::DURATION_INFINITE);
	}

}
void DDSMgr::getProductFromXml(const char* strxml,DDSParticpantProduct &pdt)
{
	//DDSParticpantProduct pdt;
	rapidxml::xml_document<char> doc; 
	std::string str(strxml);  
	doc.parse<0>(const_cast<char *>(str.c_str()));  

	rapidxml::xml_node<char> * node = doc.first_node("Product");
	pdt.ExecName		= node->first_node("ExecName")->first_node()->value();
	pdt.ParticipantName = node->first_node("ParticipantName")->first_node()->value();
	pdt.PID				= atoi(node->first_node("PID")->value());
	pdt.NodeName		= node->first_node("NodeName")->first_node()->value();
	pdt.FederationId	= strtol(node->first_node("FederationId")->value(),NULL,16);
	pdt.VendorId		= node->first_node("VendorId")->value();
	//return pdt;
}
void DDSMgr::start()
{
	if(!m_isRun)
	{
		m_isRun = true;
		DWORD	dwThreadId;
		m_ddsmgrData->m_hdRun  = CreateThread(NULL,0,starDDSSvrRun,NULL,0,&dwThreadId);
	}
}
void DDSMgr::stop()
{
	m_isRun = false;
	if(WaitForSingleObject(m_ddsmgrData->m_hdRun,5000)!=WAIT_OBJECT_0)
	{
		if (m_ddsmgrData->m_hdRun)
		{
			TerminateThread(m_ddsmgrData->m_hdRun,0);
		}
	}
	CloseHandle(m_ddsmgrData->m_hdRun);
}
DWORD WINAPI starDDSSvrRun(LPVOID lpThreadParameter)
{
	DDSMgr::GetInstancePtr()->run();
	return 0;
}