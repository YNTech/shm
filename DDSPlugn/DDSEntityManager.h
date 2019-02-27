
#ifndef _DDSENTITYMGR_
#define _DDSENTITYMGR_
#include "ccpp_dds_dcps.h"
#include "DDSLibDataDcps.h"
#include "CheckStatus.h"
#include <string>
#include "mymap.h"
using namespace DDS;

  struct TopicVal
  {
	  Topic_var topic;
	  DataWriter_var writer;
	  DataReader_var reader;
	  ULong dataSize;
	  //DDSLib::DDSDataSeq dataList;
	  //DDS::SampleInfoSeq infoSeq;
	  std::string typeName;
  };
  struct DomainParticipantVal
  {
	  DomainParticipant_var participant;
	  Publisher_var publisher;
	  Subscriber_var subscriber;
	  mymap<std::string,TopicVal> topic_map;
  };
  #define DefauleQoS "DefauleQoS"
  struct  DDSQoSData
  {
	  string name;
	  TopicQos topicqos;
	  bool hasreader;
	  bool haswriter;
	  DataReaderQos readerqos;
	  DataWriterQos writerqos;
  };
  class DDSEntityManager
  {

      /* Generic DDS entities */
	  mymap<std::string,DomainParticipantVal> m_participant_map;
      DomainParticipantFactory_var dpf;
      /* QosPolicy holders */
      //TopicQos reliable_topic_qos;
      DomainId_t domain;
      ReturnCode_t status;
	  string m_errString;
    public:
	  bool hasParticipant(const char *partitiontName);
      bool createParticipant(const char *partitiontName);
      bool deleteParticipant(const char *partitiontName);
	  bool deleteParticipant(DomainParticipant_var& participant);
      bool registerType(TypeSupport *ts,const char *topicName,const char *partitiontName);
	  bool hasTopic(const char *topicName,const char *partitiontName);
      bool createTopic(const char *topicName,const char *partitiontName,ULong dataSize);
      bool deleteTopic(DomainParticipant_var& participant,Topic_var topic);
	  bool deleteTopic(const char *topicName,const char *partitiontName);
      bool createPublisher(const char *partitiontName);
      bool deletePublisher(const char *partitiontName);
	  bool deletePublisher(DomainParticipant_var& participant,Publisher_var& publisher);
      bool createWriter(const char *topicName,const char *partitiontName);
	  bool createWriter(bool autodispose_unregistered_instances,const char *topicName,const char *partitiontName);
	  bool deleteWriter(const char *topicName,const char *partitiontName);
	  bool deleteWriter(Publisher_var& publisher,DataWriter_var& writer);
	  int getDataSize(const char *topicName,const char *partitiontName);
	  //DDSLib::DDSDataSeq* getDataList(const char *topicName,const char *partitiontName);
	  //DDS::SampleInfoSeq* getInfoList(const char *topicName,const char *partitiontName);
	  //DDSLib::DDSDataSeq& getDataListRef(const char *topicName,const char *partitiontName);
	  //DDS::SampleInfoSeq& getInfoListRef(const char *topicName,const char *partitiontName);
      bool createSubscriber(const char *partitiontName);
      bool deleteSubscriber(const char *partitiontName);
	  bool deleteSubscriber(DomainParticipant_var& participant,Subscriber_var& subscriber);
      bool createReader(const char *topicName,const char *partitiontName);
      bool deleteReader(const char *topicName,const char *partitiontName);
	  bool deleteReader(Subscriber_var& subscriber,DataReader_var& reader);
      DataReader_ptr getReader(const char *topicName,const char *partitiontName);
      DataWriter_ptr getWriter(const char *topicName,const char *partitiontName);
      Publisher_ptr getPublisher(const char *partitiontName);
      Subscriber_ptr getSubscriber(const char *partitiontName);
      Topic_ptr getTopic(const char *topicName,const char *partitiontName);
      DomainParticipant_ptr getParticipant(const char *partitiontName);
	  DDSEntityManager();
      ~DDSEntityManager();
	  string getErrorSting(){ return m_errString;}

	  void loadQoSData();
	  std::map<string,DDSQoSData> m_ddsqos;
  };

#endif
