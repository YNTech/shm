#include "DDSEntityManager.h"


bool DDSEntityManager::createParticipant(const char *partitiontName)
{
  dpf = DomainParticipantFactory::get_instance();
  if(!checkHandle(dpf.in(), "createParticipant::get_instance",&m_errString))
	  return false;
  m_participant_map[partitiontName].participant = dpf->create_participant(DOMAIN_ID_DEFAULT, PARTICIPANT_QOS_DEFAULT, nullptr,
    STATUS_MASK_NONE);
  if(!checkHandle(m_participant_map[partitiontName].participant.in(),
    "createParticipant::create_participant",&m_errString))
	return false;
  //partition = partitiontName;
  if(m_ddsqos.find(DefauleQoS)!= m_ddsqos.end())
  {
	  status = m_participant_map[partitiontName].participant->set_default_topic_qos(m_ddsqos[DefauleQoS].topicqos);
	  if(!checkStatus(status, "createParticipant::set_default_topic_qos",&m_errString))
		  return false;
  }
  return true;
}

bool DDSEntityManager::deleteParticipant(const char *partitiontName)
{
	if(!m_participant_map[partitiontName].participant)
		return false;
    status = dpf->delete_participant(m_participant_map[partitiontName].participant.in());
	 return checkStatus(status, "DDS::DomainParticipant::delete_participant ",&m_errString);
}
bool DDSEntityManager::deleteParticipant(DomainParticipant_var& partition)
{
	if(!partition)
		return false;
	status = dpf->delete_participant(partition.in());
	return checkStatus(status, "DDS::DomainParticipant::delete_participant ",&m_errString);
}
bool DDSEntityManager::registerType(TypeSupport *ts,const char *topicName,const char *partitiontName)
{
  const char * typeName = topicName;//ts->get_type_name();
  //m_participant_map[partitiontName].topic_map[topicName] = TopicVal();
  m_participant_map[partitiontName].topic_map[topicName].typeName = typeName;
  status = ts->register_type(m_participant_map[partitiontName].participant.in(),typeName);
  return checkStatus(status, "register_type",&m_errString);
}

bool DDSEntityManager::createTopic(const char *topicName,const char *partitiontName,ULong dataSize)
{
 // status = m_participant_map[partitiontName].participant->get_default_topic_qos(reliable_topic_qos);
 // if(!checkStatus(status, "DDS::DomainParticipant::get_default_topic_qos",&m_errString))
	//  return false;
 // reliable_topic_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
 // //reliable_topic_qos.durability.kind = PERSISTENT_DURABILITY_QOS;
 //// reliable_topic_qos.durability_service.history_kind = KEEP_ALL_HISTORY_QOS;
 // //reliable_topic_qos.destination_order.kind = BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
 // reliable_topic_qos.history.kind = KEEP_ALL_HISTORY_QOS;

 // /* Make the tailored QoS the new default. */
 // status = m_participant_map[partitiontName].participant->set_default_topic_qos(reliable_topic_qos);
 // if(!checkStatus(status, "DDS::DomainParticipant::set_default_topic_qos",&m_errString))
	//  return false;
  if(m_ddsqos.find(topicName)!= m_ddsqos.end())
  {
	  /* Use the changed policy when defining the HelloWorld topic */
	  m_participant_map[partitiontName].topic_map[topicName].topic = 
		  m_participant_map[partitiontName].participant->create_topic(
		  topicName, m_participant_map[partitiontName].topic_map[topicName].typeName.c_str(), 
		  m_ddsqos[topicName].topicqos,
		  nullptr, STATUS_MASK_NONE);
  }
  else if(m_ddsqos.find(DefauleQoS)!= m_ddsqos.end())
	  m_participant_map[partitiontName].topic_map[topicName].topic = m_participant_map[partitiontName].participant->create_topic(topicName, m_participant_map[partitiontName].topic_map[topicName].typeName.c_str(), m_ddsqos[DefauleQoS].topicqos,
	  nullptr, STATUS_MASK_NONE);
  else
	  m_participant_map[partitiontName].topic_map[topicName].topic = m_participant_map[partitiontName].participant->create_topic(topicName, m_participant_map[partitiontName].topic_map[topicName].typeName.c_str(), TOPIC_QOS_DEFAULT,
	  nullptr, STATUS_MASK_NONE);
  if(!checkHandle(m_participant_map[partitiontName].topic_map[topicName].topic.in(), "DDS::DomainParticipant::create_topic ()",&m_errString))
	  return false;
  m_participant_map[partitiontName].topic_map[topicName].dataSize = dataSize;
  return true;
}

bool DDSEntityManager::deleteTopic(const char *topicName,const char *partitiontName)
{
	if(!m_participant_map[partitiontName].participant || !m_participant_map[partitiontName].topic_map[topicName].topic)
		return false;
	status = m_participant_map[partitiontName].participant->delete_topic(m_participant_map[partitiontName].topic_map[topicName].topic);
	return checkStatus(status, "DDS.DomainParticipant.delete_topic",&m_errString);
}
bool DDSEntityManager::deleteTopic(DomainParticipant_var& participant,Topic_var topic)
{
	if(!participant || !topic)
		return false;
	status = participant->delete_topic(topic);
	return checkStatus(status, "DDS.DomainParticipant.delete_topic",&m_errString);
}
bool DDSEntityManager::createPublisher(const char *partitiontName)
{
  PublisherQos pub_qos;
  status = m_participant_map[partitiontName].participant->get_default_publisher_qos(pub_qos);
  if(!checkStatus(status, "DDS::DomainParticipant::get_default_publisher_qos",&m_errString))
	  return false;
  pub_qos.partition.name.length(1);
  pub_qos.partition.name[0] = partitiontName;

  m_participant_map[partitiontName].publisher = m_participant_map[partitiontName].participant->create_publisher(pub_qos, nullptr, STATUS_MASK_NONE);
  return checkHandle(m_participant_map[partitiontName].publisher.in(), "DDS::DomainParticipant::create_publisher",&m_errString);
}

bool DDSEntityManager::deletePublisher(const char *partitiontName)
{
	if(!m_participant_map[partitiontName].participant || !m_participant_map[partitiontName].publisher)
		return false;
	status = m_participant_map[partitiontName].participant->delete_publisher(m_participant_map[partitiontName].publisher.in());
    return checkStatus(status, "DDS::DomainParticipant::delete_publisher ",&m_errString);
}
bool DDSEntityManager::deletePublisher(DomainParticipant_var& participant,Publisher_var& publisher)
{
	if(!participant || !publisher)
		return false;
	status = participant->delete_publisher(publisher.in());
	return checkStatus(status, "DDS::DomainParticipant::delete_publisher ",&m_errString);
}

bool DDSEntityManager::createWriter(const char *topicName,const char *partitiontName)
{
  m_participant_map[partitiontName].topic_map[topicName].writer = m_participant_map[partitiontName].publisher->create_datawriter(m_participant_map[partitiontName].topic_map[topicName].topic.in(),
    DATAWRITER_QOS_USE_TOPIC_QOS, nullptr, STATUS_MASK_NONE);
  return checkHandle(m_participant_map[partitiontName].topic_map[topicName].writer, "DDS::Publisher::create_datawriter",&m_errString);
}

bool DDSEntityManager::createWriter(bool autodispose_unregistered_instances,const char *topicName,const char *partitiontName)
{
  DataWriterQos dw_qos = DATAWRITER_QOS_USE_TOPIC_QOS;
  //status = m_participant_map[partitiontName].publisher->get_default_datawriter_qos(dw_qos);
  //if(!checkStatus(status, "DDS::DomainParticipant::get_default_publisher_qos",&m_errString))
	 // return false;
  //status = m_participant_map[partitiontName].publisher->copy_from_topic_qos(dw_qos, reliable_topic_qos);
  //if(!checkStatus(status, "DDS::Publisher::copy_from_topic_qos",&m_errString))
	 // return false;
  // Set autodispose to false so that you can start
  // the subscriber after the publisher
  dw_qos.writer_data_lifecycle.autodispose_unregistered_instances =autodispose_unregistered_instances;
  m_participant_map[partitiontName].topic_map[topicName].writer = m_participant_map[partitiontName].publisher->create_datawriter(m_participant_map[partitiontName].topic_map[topicName].topic.in(), dw_qos, nullptr,
    STATUS_MASK_NONE);
  return checkHandle(m_participant_map[partitiontName].topic_map[topicName].writer, "DDS::Publisher::create_datawriter",&m_errString);
}

bool DDSEntityManager::deleteWriter(const char *topicName,const char *partitiontName)
{
	if(!m_participant_map[partitiontName].publisher || !m_participant_map[partitiontName].topic_map[topicName].writer)
		return false;
	status = m_participant_map[partitiontName].publisher->delete_datawriter(m_participant_map[partitiontName].topic_map[topicName].writer);
	return checkStatus(status, "DDS::Publisher::delete_datawriter ",&m_errString);
}
bool DDSEntityManager::deleteWriter(Publisher_var& publisher,DataWriter_var& writer)
{
	if(!publisher || !writer)
		return false;
	status = publisher->delete_datawriter(writer);
	return checkStatus(status, "DDS::Publisher::delete_datawriter ",&m_errString);
}
bool DDSEntityManager::createSubscriber(const char *partitiontName)
{
  SubscriberQos sub_qos;
  int status = m_participant_map[partitiontName].participant->get_default_subscriber_qos(sub_qos);
  if(!checkStatus(status, "DDS::DomainParticipant::get_default_subscriber_qos",&m_errString))
	  return false;
  sub_qos.partition.name.length(1);
  sub_qos.partition.name[0] = partitiontName;
  m_participant_map[partitiontName].subscriber = m_participant_map[partitiontName].participant->create_subscriber(sub_qos, nullptr, STATUS_MASK_NONE);
  return checkHandle(m_participant_map[partitiontName].subscriber.in(), "DDS::DomainParticipant::create_subscriber",&m_errString);
}

bool DDSEntityManager::deleteSubscriber(const char *partitiontName)
{
	if(!m_participant_map[partitiontName].participant || !m_participant_map[partitiontName].subscriber)
		return false;
  status = m_participant_map[partitiontName].participant->delete_subscriber(m_participant_map[partitiontName].subscriber);
  return checkStatus(status, "DDS::DomainParticipant::delete_subscriber ",&m_errString);
}
bool DDSEntityManager::deleteSubscriber(DomainParticipant_var& participant,Subscriber_var& subscriber)
{
	if(!participant || !subscriber)
		return false;
	status = participant->delete_subscriber(subscriber);
	return checkStatus(status, "DDS::DomainParticipant::delete_subscriber ",&m_errString);
}
bool DDSEntityManager::createReader(const char *topicName,const char *partitiontName)
{
  m_participant_map[partitiontName].topic_map[topicName].reader = m_participant_map[partitiontName].subscriber->create_datareader(m_participant_map[partitiontName].topic_map[topicName].topic.in(),
    DATAREADER_QOS_USE_TOPIC_QOS, nullptr, STATUS_MASK_NONE);
  return checkHandle(m_participant_map[partitiontName].topic_map[topicName].reader, "DDS::Subscriber::create_datareader ()",&m_errString);
}

bool DDSEntityManager::deleteReader(const char *topicName,const char *partitiontName)
{
	if(!m_participant_map[partitiontName].subscriber || !m_participant_map[partitiontName].topic_map[topicName].reader)
		return false;
	status = m_participant_map[partitiontName].subscriber->delete_datareader(m_participant_map[partitiontName].topic_map[topicName].reader);
	return checkStatus(status, "DDS::Subscriber::delete_datareader ",&m_errString);
}
bool DDSEntityManager::deleteReader(Subscriber_var& subscriber,DataReader_var& reader)
{
	if(!subscriber || !reader)
		return false;
	status = subscriber->delete_datareader(reader);
	return checkStatus(status, "DDS::Subscriber::delete_datareader ",&m_errString);
}
DataReader_ptr DDSEntityManager::getReader(const char *topicName,const char *partitiontName)
{
  return DataReader::_duplicate(m_participant_map[partitiontName].topic_map[topicName].reader.in());
}

DataWriter_ptr DDSEntityManager::getWriter(const char *topicName,const char *partitiontName)
{
  return DataWriter::_duplicate(m_participant_map[partitiontName].topic_map[topicName].writer.in());
}

Publisher_ptr DDSEntityManager::getPublisher(const char *partitiontName)
{
  return Publisher::_duplicate(m_participant_map[partitiontName].publisher.in());
}

Subscriber_ptr DDSEntityManager::getSubscriber(const char *partitiontName)
{
  return Subscriber::_duplicate(m_participant_map[partitiontName].subscriber.in());
}

Topic_ptr DDSEntityManager::getTopic(const char *topicName,const char *partitiontName)
{
  return Topic::_duplicate(m_participant_map[partitiontName].topic_map[topicName].topic.in());
}

DomainParticipant_ptr DDSEntityManager::getParticipant(const char *partitiontName)
{
  return DomainParticipant::_duplicate(m_participant_map[partitiontName].participant.in());
}
DDSEntityManager::DDSEntityManager()
{
	loadQoSData();
}
DDSEntityManager::~DDSEntityManager()
{
	std::map<std::string,DomainParticipantVal>::iterator ptit = m_participant_map.begin();
	while(ptit != m_participant_map.end())
	{
		std::map<std::string,TopicVal>::iterator tpit = ptit->second.topic_map.begin();
		while(tpit != ptit->second.topic_map.end())
		{
			deleteWriter(ptit->second.publisher,tpit->second.writer);

			//DDSLib::DDSDataDataReader_var datareader = DDSLib::DDSDataDataReader::_narrow(tpit->second.reader.in());
			//if(datareader)
			//{
			//	ReturnCode_t status = datareader->return_loan(tpit->second.dataList, tpit->second.infoSeq);
			//	if(!checkStatus(status, "DDSEntityManager::~DDSEntityManager:return_loan",&m_errString))
			//		cout<<m_errString;
			//}

			deleteReader(ptit->second.subscriber,tpit->second.reader);
			deleteTopic(ptit->second.participant,tpit->second.topic);
			tpit++; 
		}
		deletePublisher(ptit->second.participant,ptit->second.publisher);
		deleteSubscriber(ptit->second.participant,ptit->second.subscriber);
		deleteParticipant(ptit->second.participant);
		ptit++;
	}
	ptit = m_participant_map.begin();
	while(ptit != m_participant_map.end())
	{
		ptit->second.topic_map.erase(ptit->second.topic_map.begin(),ptit->second.topic_map.end());
		ptit++;
	}
}
bool DDSEntityManager::hasParticipant(const char *partitiontName)
{
	return m_participant_map.find(partitiontName) != m_participant_map.end();
}
bool DDSEntityManager::hasTopic(const char *topicName,const char *partitiontName)
{
	if(m_participant_map.find(partitiontName) == m_participant_map.end())
		return false;
	return m_participant_map[partitiontName].topic_map.find(topicName) != m_participant_map[partitiontName].topic_map.end();
}

int DDSEntityManager::getDataSize(const char *topicName,const char *partitiontName)
{
	return m_participant_map[partitiontName].topic_map[topicName].dataSize;
}
//DDSLib::DDSDataSeq* DDSEntityManager::getDataList(const char *topicName,const char *partitiontName)
//{
//	//if(m_participant_map.find(partitiontName) == m_participant_map.end())
//	//	return DDSLib::DDSDataSeq;
//	TopicVal& topic = &(m_participant_map[partitiontName].topic_map[topicName]);
//	int nid = topic.getBufId();
//	return &(m_participant_map[partitiontName].topic_map[topicName].dataList);
//}
//DDS::SampleInfoSeq* DDSEntityManager::getInfoList(const char *topicName,const char *partitiontName)
//{
//	return &(m_participant_map[partitiontName].topic_map[topicName].infoSeq);
//}
//DDSLib::DDSDataSeq& DDSEntityManager::getDataListRef(const char *topicName,const char *partitiontName)
//{
//	//if(m_participant_map.find(partitiontName) == m_participant_map.end())
//	//	return DDSLib::DDSDataSeq;
//	return m_participant_map[partitiontName].topic_map[topicName].dataList;
//}
//DDS::SampleInfoSeq& DDSEntityManager::getInfoListRef(const char *topicName,const char *partitiontName)
//{
//	return m_participant_map[partitiontName].topic_map[topicName].infoSeq;
//}
void DDSEntityManager::loadQoSData()
{
	m_ddsqos.clear();
	FILE* file = fopen("dds_topic_qos.data","rb");
	if (file)  
	{
		while(!feof(file))
		{
			int nums = 0;
			char nm[32];
			fread( &nums, sizeof( int ), 1, file );
			for(int i=0; i<nums; i++)
			{
				fread( nm,  sizeof( char ), 32, file );
				fread( (char*)&(m_ddsqos[nm].topicqos),sizeof(TopicQos), 1, file );
				m_ddsqos[nm].haswriter = false;
				m_ddsqos[nm].hasreader = false;
			}
		}
		fclose(file);
	}
	file = fopen("dds_reader_qos.data","rb+");
	if (file)  
	{
		char nm[32];
		while(!feof(file))
		{
			fread( nm,  sizeof( char ), 32, file );
			fread( (char*)&(m_ddsqos[nm].readerqos),sizeof(DataReaderQos), 1, file );
			m_ddsqos[nm].hasreader = true;
		}
		fclose(file);
	}
	file = fopen("dds_writer_qos.data","rb+");
	if (file)  
	{
		char nm[32];
		while(!feof(file))
		{
			fread( nm,  sizeof( char ), 32, file );
			fread( (char*)&(m_ddsqos[nm].writerqos),sizeof(DataWriterQos), 1, file );
			m_ddsqos[nm].haswriter = true;
		}
		fclose(file);
	}
	// get default
	if(m_ddsqos.find(DefauleQoS) == m_ddsqos.end())
	{
		m_ddsqos[DefauleQoS].hasreader = true;
		m_ddsqos[DefauleQoS].haswriter = true;
		string errstring;
		DomainParticipantFactory_var dpf = DomainParticipantFactory::get_instance();
		if(!checkHandle(dpf.in(), "DDSEntityManager::loadQoSData:get_instance",&errstring))
		{
			std::cout<<errstring<<endl;
			return ;
		}
		DomainParticipant_var participant = dpf->create_participant(DOMAIN_ID_DEFAULT, PARTICIPANT_QOS_DEFAULT, nullptr,STATUS_MASK_NONE);
		if(!checkHandle(participant.in(),"DDSEntityManager::loadQoSData:create_participant",&errstring)) 
		{
			std::cout<<errstring<<endl;
			return ;
		}
		ReturnCode_t ret = participant->get_default_topic_qos(m_ddsqos[DefauleQoS].topicqos);
		if(!checkStatus(ret,"DDSEntityManager::loadQoSData:get_default_topic_qos",&errstring)) 
		{
			std::cout<<errstring<<endl;
			return ;
		}
		PublisherQos pub_qos;
		ret = participant->get_default_publisher_qos(pub_qos);
		if(!checkStatus(ret, "DDSEntityManager::loadQoSData:get_default_publisher_qos",&errstring))
		{
			std::cout<<errstring<<endl;
			return ;
		}
		Publisher_var publisher = participant->create_publisher(pub_qos, nullptr, STATUS_MASK_NONE);
		if(!checkHandle(publisher.in(), "DDSEntityManager::loadQoSData:create_publisher",&errstring))
		{
			std::cout<<errstring<<endl;
			return ;
		}
		ret = publisher->get_default_datawriter_qos(m_ddsqos[DefauleQoS].writerqos);
		if(!checkStatus(ret,"DDSEntityManager::loadQoSData:get_default_datawriter_qos",&errstring)) 
		{
			std::cout<<errstring<<endl;
			return ;
		}
		SubscriberQos sub_qos;
		ret = participant->get_default_subscriber_qos(sub_qos);
		if(!checkStatus(ret, "DDSEntityManager::loadQoSData:get_default_subscriber_qos",&errstring))
		{
			std::cout<<errstring<<endl;
			return ;
		}
		Subscriber_var subscribe = participant->create_subscriber(sub_qos, nullptr, STATUS_MASK_NONE);
		if(!checkHandle(publisher.in(), "DDSEntityManager::loadQoSData:create_subscriber",&errstring))
		{
			std::cout<<errstring<<endl;
			return ;
		}
		ret = subscribe->get_default_datareader_qos(m_ddsqos[DefauleQoS].readerqos);
		if(!checkStatus(ret,"DDSEntityManager::loadQoSData:get_default_datareader_qos",&errstring)) 
		{
			std::cout<<errstring<<endl;
			return ;
		}
	}
}