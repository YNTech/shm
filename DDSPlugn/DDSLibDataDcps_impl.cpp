#include "DDSLibDataDcps_impl.h"
#include "gapi.h"
#include "gapi_loanRegistry.h"
#include "DDSLibDataSplDcps.h"
//#include "mccpp_DataReaderCopy.h"
#include "ccpp_DataReaderView_impl.h"
#include "ddalib_export.h"

extern c_bool
__DDSLib_DDSData__copyIn(
    struct base_s * base,
    struct _DDSLib_DDSData *from,
    struct _DDSLib_DDSData *to);

extern void
__DDSLib_DDSData__copyOut(
    void *_from,
    void *_to);

// DDS DDSLib::DDSData TypeSupportFactory Object Body
const char* DDSLib::__get_DDSData_MetaDescriptor(const char* typname, DDS::ULong dataSize)
{
//	static char metaDescriptor[] = "<MetaData version=\"1.0.0\"><Module name=\"DDSLib\"><Struct name=\"%s\"><Member name=\"ind\"><Long/>"
//		"</Member><Member name=\"len\"><ULong/></Member><Member name=\"data\"><Array size=\"%d\"><Char/></Array></Member></Struct></Module></MetaData>                                         ";
	static char metaDescriptor[256];
	static char *fm = "<MetaData version=\"1.0.0\"><Module name=\"DDSLib\"><Struct name=\"%s\"><Member name=\"ind\"><Long/>"
		"</Member><Member name=\"len\"><ULong/></Member><Member name=\"data\"><Array size=\"%d\"><Char/></Array></Member></Struct></Module></MetaData>";
	sprintf(metaDescriptor,fm,typname,dataSize);

	return metaDescriptor;
}
::DDS::DataWriter_ptr
DDSLib::DDSDataTypeSupportFactory::create_datawriter (gapi_dataWriter handle)
{
    return new DDSLib::DDSDataDataWriter_impl(handle);
}

::DDS::DataReader_ptr
DDSLib::DDSDataTypeSupportFactory::create_datareader (gapi_dataReader handle)
{
    return new DDSLib::DDSDataDataReader_impl (handle);
}


::DDS::DataReaderView_ptr
DDSLib::DDSDataTypeSupportFactory::create_view (gapi_dataReaderView handle)
{
    return new DDSLib::DDSDataDataReaderView_impl (handle);
}

// DDS DDSLib::DDSData TypeSupport Object Body

DDSLib::DDSDataTypeSupport::DDSDataTypeSupport(const char *type_name, const char *type_def[]) :
    TypeSupport_impl(
        type_name,//__DDSLib_DDSData__name(),
        __DDSLib_DDSData__keys(),
		type_def,
        //DDSLib::DDSDataTypeSupport::metaDescriptor,
        (gapi_copyIn) __DDSLib_DDSData__copyIn,
        (gapi_copyOut) __DDSLib_DDSData__copyOut,
        (gapi_readerCopy) ::DDS::ccpp_DataReaderCopy<DDSLib::DDSDataSeq, _DDSLib_DDSData>,
		new  DDSLib::DDSDataTypeSupportFactory(),1)
{
    // Parent constructor takes care of everything.
}

DDSLib::DDSDataTypeSupport::~DDSDataTypeSupport(void)
{
    // Parent destructor takes care of everything.
}

::DDS::ReturnCode_t
DDSLib::DDSDataTypeSupport::register_type(
    ::DDS::DomainParticipant_ptr domain,
    const char * type_name) THROW_ORB_EXCEPTIONS
{
    return TypeSupport_impl::register_type(domain, type_name);
}

char *
DDSLib::DDSDataTypeSupport::get_type_name() THROW_ORB_EXCEPTIONS
{
    return TypeSupport_impl::get_type_name();
}

// DDS DDSLib::DDSData DataWriter_impl Object Body

DDSLib::DDSDataDataWriter_impl::DDSDataDataWriter_impl (
    gapi_dataWriter handle
) : ::DDS::DataWriter_impl(handle)
{
    // Parent constructor takes care of everything.
}

DDSLib::DDSDataDataWriter_impl::~DDSDataDataWriter_impl(void)
{
    // Parent destructor takes care of everything.
}

::DDS::InstanceHandle_t
DDSLib::DDSDataDataWriter_impl::register_instance(
    const _DDSLib_DDSData & instance_data) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::register_instance(&instance_data);
}

::DDS::InstanceHandle_t
DDSLib::DDSDataDataWriter_impl::register_instance_w_timestamp(
    const _DDSLib_DDSData & instance_data,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::register_instance_w_timestamp(&instance_data, source_timestamp);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::unregister_instance(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::unregister_instance(&instance_data, handle);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::unregister_instance_w_timestamp(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::unregister_instance_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::write(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::write(&instance_data, handle);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::write_w_timestamp(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::write_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::dispose(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::dispose(&instance_data, handle);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::dispose_w_timestamp(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::dispose_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::writedispose(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::writedispose(&instance_data, handle);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::writedispose_w_timestamp(
    const _DDSLib_DDSData & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::writedispose_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataWriter_impl::get_key_value(
    _DDSLib_DDSData & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
DDSLib::DDSDataDataWriter_impl::lookup_instance(
    const _DDSLib_DDSData & instance_data) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::lookup_instance(&instance_data);
}

// DDS DDSLib::DDSData DataReader_impl Object Body

DDSLib::DDSDataDataReader_impl::DDSDataDataReader_impl (
    gapi_dataReader handle
) : ::DDS::DataReader_impl(handle,::DDS::ccpp_DataReaderParallelDemarshallingMain<DDSLib::DDSDataSeq>)
{
    // Parent constructor takes care of everything.
}

DDSLib::DDSDataDataReader_impl::~DDSDataDataReader_impl(void)
{
    // Parent destructor takes care of everything.
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::read(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::take(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::read_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::take_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::read_next_sample(
    _DDSLib_DDSData & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::read_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::take_next_sample(
    _DDSLib_DDSData & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::take_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::read_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::take_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::read_next_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::take_next_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::read_next_instance_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::take_next_instance_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::return_loan(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS
{

    ::DDS::ReturnCode_t status = ::DDS::RETCODE_OK;

    if ( received_data.length() > 0 ) {
        if (received_data.length() == info_seq.length() &&
            received_data.release() == info_seq.release() ) {
            if (!received_data.release()) {
                status = DataReader_impl::return_loan( received_data.get_buffer(),
                                                       info_seq.get_buffer() );

                if ( status == ::DDS::RETCODE_OK ) {
                    if ( !received_data.release() ) {
                        DDSLib::DDSDataSeq::freebuf( received_data.get_buffer(false) );
                        received_data.replace(0, 0, nullptr, false);
                        ::DDS::SampleInfoSeq::freebuf( info_seq.get_buffer(false) );
                        info_seq.replace(0, 0, nullptr, false);
                    }
                } else if ( status == ::DDS::RETCODE_NO_DATA ) {
                    if ( received_data.release() ) {
                        status = ::DDS::RETCODE_OK;
                    } else {
                        status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
                    }
                }
            }
        } else {
            status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
        }
    }
	
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::get_key_value(
    _DDSLib_DDSData & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
DDSLib::DDSDataDataReader_impl::lookup_instance(
    const _DDSLib_DDSData & instance) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::lookup_instance(&instance);
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReader_impl::check_preconditions(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples)
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_PRECONDITION_NOT_MET;

    if ( received_data.length() == info_seq.length() &&
         received_data.maximum() == info_seq.maximum() &&
         received_data.release() == info_seq.release() ) {
        if ( received_data.maximum() == 0 || received_data.release() ) {
            if (received_data.maximum() == 0 ||
                max_samples <= static_cast<CORBA::Long>(received_data.maximum()) ||
                max_samples == ::DDS::LENGTH_UNLIMITED ) {
                status = ::DDS::RETCODE_OK;
            }
        }
    }
    return status;
}


// DDS DDSLib::DDSData DataReaderView_impl Object Body

DDSLib::DDSDataDataReaderView_impl::DDSDataDataReaderView_impl (
    gapi_dataReaderView handle
) : ::DDS::DataReaderView_impl(handle)
{
    // Parent constructor takes care of everything.
}

DDSLib::DDSDataDataReaderView_impl::~DDSDataDataReaderView_impl(void)
{
    // Parent destructor takes care of everything.
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::read(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::take(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::read_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::take_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::read_next_sample(
    _DDSLib_DDSData & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::read_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::take_next_sample(
    _DDSLib_DDSData & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::take_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::read_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::take_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::read_next_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::take_next_instance(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::read_next_instance_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::take_next_instance_w_condition(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = DDSLib::DDSDataDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::return_loan(
    DDSLib::DDSDataSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_OK;

    if ( received_data.length() > 0 ) {
        if (received_data.length() == info_seq.length() &&
            received_data.release() == info_seq.release() ) {
            if (!received_data.release()) {
                status = DataReaderView_impl::return_loan( received_data.get_buffer(),
                                                       info_seq.get_buffer() );

                if ( status == ::DDS::RETCODE_OK ) {
                    if ( !received_data.release() ) {
                        DDSLib::DDSDataSeq::freebuf( received_data.get_buffer(false) );
                        received_data.replace(0, 0, nullptr, false);
                        ::DDS::SampleInfoSeq::freebuf( info_seq.get_buffer(false) );
                        info_seq.replace(0, 0, nullptr, false);
                    }
                } else if ( status == ::DDS::RETCODE_NO_DATA ) {
                    if ( received_data.release() ) {
                        status = ::DDS::RETCODE_OK;
                    } else {
                        status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
                    }
                }
            }
        } else {
            status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
        }
    }
    return status;
}


::DDS::ReturnCode_t
DDSLib::DDSDataDataReaderView_impl::get_key_value(
    _DDSLib_DDSData & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
DDSLib::DDSDataDataReaderView_impl::lookup_instance(
    const _DDSLib_DDSData & instance) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::lookup_instance(&instance);
}



//const char * ::DDSLib::DDSDataTypeSupport::metaDescriptor = "<MetaData version=\"1.0.0\"><Module name=\"DDSLib\"><Struct name=\"DDSData\"><Member name=\"ind\"><Long/>"
//"</Member><Member name=\"len\"><ULong/></Member><Member name=\"data\"><Array size=\"512\"><Char/></Array></Member></Struct></Module></MetaData>";
