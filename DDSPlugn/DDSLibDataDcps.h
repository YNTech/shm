#ifndef _DDSLIBDATADCPS_H_
#define _DDSLIBDATADCPS_H_

#include "ddalib_export.h"
#include "sacpp_mapping.h"
//#include "MUFL.h"
#include "sacpp_DDS_DCPS.h"
#include "dds_dcps_interfaces.h"
#include "dds_builtinTopics.h"
#include "dds_dcps_builtintopics.h"
#include "DDSLibDataSplDcps.h"
namespace DDSLib
{
   class DDSDataTypeSupportInterface;

   typedef DDSDataTypeSupportInterface * DDSDataTypeSupportInterface_ptr;
   typedef DDS_DCPSInterface_var < DDSDataTypeSupportInterface> DDSDataTypeSupportInterface_var;
   typedef DDS_DCPSInterface_out < DDSDataTypeSupportInterface> DDSDataTypeSupportInterface_out;


   class DDSDataDataWriter;

   typedef DDSDataDataWriter * DDSDataDataWriter_ptr;
   typedef DDS_DCPSInterface_var < DDSDataDataWriter> DDSDataDataWriter_var;
   typedef DDS_DCPSInterface_out < DDSDataDataWriter> DDSDataDataWriter_out;


   class DDSDataDataReader;

   typedef DDSDataDataReader * DDSDataDataReader_ptr;
   typedef DDS_DCPSInterface_var < DDSDataDataReader> DDSDataDataReader_var;
   typedef DDS_DCPSInterface_out < DDSDataDataReader> DDSDataDataReader_out;


   class DDSDataDataReaderView;

   typedef DDSDataDataReaderView * DDSDataDataReaderView_ptr;
   typedef DDS_DCPSInterface_var < DDSDataDataReaderView> DDSDataDataReaderView_var;
   typedef DDS_DCPSInterface_out < DDSDataDataReaderView> DDSDataDataReaderView_out;

   struct DDSDataSeq_uniq_ {};
   typedef DDS_DCPSUFLSeq < _DDSLib_DDSData, struct DDSDataSeq_uniq_> DDSDataSeq;
   typedef DDS_DCPSSequence_var < DDSDataSeq> DDSDataSeq_var;
   typedef DDS_DCPSSequence_out < DDSDataSeq> DDSDataSeq_out;
   class DDSDataTypeSupportInterface
   :
      virtual public DDS::TypeSupport
   { 
   public:
      typedef DDSDataTypeSupportInterface_ptr _ptr_type;
      typedef DDSDataTypeSupportInterface_var _var_type;

      static DDSDataTypeSupportInterface_ptr _duplicate (DDSDataTypeSupportInterface_ptr obj);
      DDS::Boolean _local_is_a (const char * id);

      static DDSDataTypeSupportInterface_ptr _narrow (DDS::Object_ptr obj);
      static DDSDataTypeSupportInterface_ptr _unchecked_narrow (DDS::Object_ptr obj);
      static DDSDataTypeSupportInterface_ptr _nil () { return 0; }
      static const char * _local_id;
      DDSDataTypeSupportInterface_ptr _this () { return this; }


   protected:
      DDSDataTypeSupportInterface () {};
      ~DDSDataTypeSupportInterface () {};
   private:
      DDSDataTypeSupportInterface (const DDSDataTypeSupportInterface &);
      DDSDataTypeSupportInterface & operator = (const DDSDataTypeSupportInterface &);
   };

   class DDSDataDataWriter
   :
      virtual public DDS::DataWriter
   { 
   public:
      typedef DDSDataDataWriter_ptr _ptr_type;
      typedef DDSDataDataWriter_var _var_type;

      static DDSDataDataWriter_ptr _duplicate (DDSDataDataWriter_ptr obj);
      DDS::Boolean _local_is_a (const char * id);

      static DDSDataDataWriter_ptr _narrow (DDS::Object_ptr obj);
      static DDSDataDataWriter_ptr _unchecked_narrow (DDS::Object_ptr obj);
      static DDSDataDataWriter_ptr _nil () { return 0; }
      static const char * _local_id;
      DDSDataDataWriter_ptr _this () { return this; }

      virtual DDS::InstanceHandle_t register_instance (const _DDSLib_DDSData& instance_data) = 0;
      virtual DDS::InstanceHandle_t register_instance_w_timestamp (const _DDSLib_DDSData& instance_data, const DDS::Time_t& source_timestamp) = 0;
      virtual DDS::ReturnCode_t unregister_instance (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::ReturnCode_t unregister_instance_w_timestamp (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle, const DDS::Time_t& source_timestamp) = 0;
      virtual DDS::ReturnCode_t write (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::ReturnCode_t write_w_timestamp (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle, const DDS::Time_t& source_timestamp) = 0;
      virtual DDS::ReturnCode_t dispose (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::ReturnCode_t dispose_w_timestamp (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle, const DDS::Time_t& source_timestamp) = 0;
      virtual DDS::ReturnCode_t writedispose (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::ReturnCode_t writedispose_w_timestamp (const _DDSLib_DDSData& instance_data, DDS::InstanceHandle_t handle, const DDS::Time_t& source_timestamp) = 0;
      virtual DDS::ReturnCode_t get_key_value (_DDSLib_DDSData& key_holder, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::InstanceHandle_t lookup_instance (const _DDSLib_DDSData& instance_data) = 0;

   protected:
      DDSDataDataWriter () {};
      ~DDSDataDataWriter () {};
   private:
      DDSDataDataWriter (const DDSDataDataWriter &);
      DDSDataDataWriter & operator = (const DDSDataDataWriter &);
   };

   class DDSDataDataReader
   :
      virtual public DDS::DataReader
   { 
   public:
      typedef DDSDataDataReader_ptr _ptr_type;
      typedef DDSDataDataReader_var _var_type;

      static DDSDataDataReader_ptr _duplicate (DDSDataDataReader_ptr obj);
      DDS::Boolean _local_is_a (const char * id);

      static DDSDataDataReader_ptr _narrow (DDS::Object_ptr obj);
      static DDSDataDataReader_ptr _unchecked_narrow (DDS::Object_ptr obj);
      static DDSDataDataReader_ptr _nil () { return 0; }
      static const char * _local_id;
      DDSDataDataReader_ptr _this () { return this; }

      virtual DDS::ReturnCode_t read (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t take (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t read_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t take_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t read_next_sample (_DDSLib_DDSData& received_data, DDS::SampleInfo& sample_info) = 0;
      virtual DDS::ReturnCode_t take_next_sample (_DDSLib_DDSData& received_data, DDS::SampleInfo& sample_info) = 0;
      virtual DDS::ReturnCode_t read_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t take_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t read_next_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t take_next_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t read_next_instance_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t take_next_instance_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::ReadCondition_ptr a_condition) = 0;
	  virtual DDS::ReturnCode_t return_loan (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq) = 0;
      virtual DDS::ReturnCode_t get_key_value (_DDSLib_DDSData& key_holder, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::InstanceHandle_t lookup_instance (const _DDSLib_DDSData& instance) = 0;

   protected:
      DDSDataDataReader () {};
      ~DDSDataDataReader () {};
   private:
      DDSDataDataReader (const DDSDataDataReader &);
      DDSDataDataReader & operator = (const DDSDataDataReader &);
   };

   class DDSDataDataReaderView
   :
      virtual public DDS::DataReaderView
   { 
   public:
      typedef DDSDataDataReaderView_ptr _ptr_type;
      typedef DDSDataDataReaderView_var _var_type;

      static DDSDataDataReaderView_ptr _duplicate (DDSDataDataReaderView_ptr obj);
      DDS::Boolean _local_is_a (const char * id);

      static DDSDataDataReaderView_ptr _narrow (DDS::Object_ptr obj);
      static DDSDataDataReaderView_ptr _unchecked_narrow (DDS::Object_ptr obj);
      static DDSDataDataReaderView_ptr _nil () { return 0; }
      static const char * _local_id;
      DDSDataDataReaderView_ptr _this () { return this; }

      virtual DDS::ReturnCode_t read (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t take (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t read_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t take_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t read_next_sample (_DDSLib_DDSData& received_data, DDS::SampleInfo& sample_info) = 0;
      virtual DDS::ReturnCode_t take_next_sample (_DDSLib_DDSData& received_data, DDS::SampleInfo& sample_info) = 0;
      virtual DDS::ReturnCode_t read_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t take_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t read_next_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t take_next_instance (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::SampleStateMask sample_states, DDS::ViewStateMask view_states, DDS::InstanceStateMask instance_states) = 0;
      virtual DDS::ReturnCode_t read_next_instance_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t take_next_instance_w_condition (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq, DDS::Long max_samples, DDS::InstanceHandle_t a_handle, DDS::ReadCondition_ptr a_condition) = 0;
      virtual DDS::ReturnCode_t return_loan (DDSDataSeq& received_data, DDS::SampleInfoSeq& info_seq) = 0;
      virtual DDS::ReturnCode_t get_key_value (_DDSLib_DDSData& key_holder, DDS::InstanceHandle_t handle) = 0;
      virtual DDS::InstanceHandle_t lookup_instance (const _DDSLib_DDSData& instance) = 0;

   protected:
      DDSDataDataReaderView () {};
      ~DDSDataDataReaderView () {};
   private:
      DDSDataDataReaderView (const DDSDataDataReaderView &);
      DDSDataDataReaderView & operator = (const DDSDataDataReaderView &);
   };

}
template <>
DDS::BuiltinTopicKey_t_slice* DDS_DCPS_ArrayHelper < DDS::BuiltinTopicKey_t, DDS::BuiltinTopicKey_t_slice, DDS::BuiltinTopicKey_t_uniq_>::alloc ();
template <>
void DDS_DCPS_ArrayHelper < DDS::BuiltinTopicKey_t, DDS::BuiltinTopicKey_t_slice, DDS::BuiltinTopicKey_t_uniq_>::copy (DDS::BuiltinTopicKey_t_slice *to, const DDS::BuiltinTopicKey_t_slice* from);
template <>
void DDS_DCPS_ArrayHelper < DDS::BuiltinTopicKey_t, DDS::BuiltinTopicKey_t_slice, DDS::BuiltinTopicKey_t_uniq_>::free (DDS::BuiltinTopicKey_t_slice *ptr);

#endif //_DDSLIBDATADCPS_H_
