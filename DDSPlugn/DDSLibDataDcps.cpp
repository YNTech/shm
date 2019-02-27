

#include "DDSLibDataDcps.h"

#if DDS_USE_EXPLICIT_TEMPLATES
template class DDS_DCPSUFLSeq < DDSLib::DDSData, struct DDSDataSeq_uniq_>;
#endif

const char * DDSLib::DDSDataTypeSupportInterface::_local_id = "IDL:DDSLib/DDSDataTypeSupportInterface:1.0";

DDSLib::DDSDataTypeSupportInterface_ptr DDSLib::DDSDataTypeSupportInterface::_duplicate (DDSLib::DDSDataTypeSupportInterface_ptr p)
{
   if (p) p->m_count++;
   return p;
}

DDS::Boolean DDSLib::DDSDataTypeSupportInterface::_local_is_a (const char * _id)
{
   if (strcmp (_id, DDSLib::DDSDataTypeSupportInterface::_local_id) == 0)
   {
      return true;
   }

   typedef DDS::TypeSupport NestedBase_1;

   if (NestedBase_1::_local_is_a (_id))
   {
      return true;
   }

   return false;
}

DDSLib::DDSDataTypeSupportInterface_ptr DDSLib::DDSDataTypeSupportInterface::_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataTypeSupportInterface_ptr result = nullptr;
   if (p && p->_is_a (DDSLib::DDSDataTypeSupportInterface::_local_id))
   {
      result = dynamic_cast < DDSLib::DDSDataTypeSupportInterface_ptr> (p);
      if (result) result->m_count++;
   }
   return result;
}

DDSLib::DDSDataTypeSupportInterface_ptr DDSLib::DDSDataTypeSupportInterface::_unchecked_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataTypeSupportInterface_ptr result;
   result = dynamic_cast < DDSLib::DDSDataTypeSupportInterface_ptr> (p);
   if (result) result->m_count++;
   return result;
}

const char * DDSLib::DDSDataDataWriter::_local_id = "IDL:DDSLib/DDSDataDataWriter:1.0";

DDSLib::DDSDataDataWriter_ptr DDSLib::DDSDataDataWriter::_duplicate (DDSLib::DDSDataDataWriter_ptr p)
{
   if (p) p->m_count++;
   return p;
}

DDS::Boolean DDSLib::DDSDataDataWriter::_local_is_a (const char * _id)
{
   if (strcmp (_id, DDSLib::DDSDataDataWriter::_local_id) == 0)
   {
      return true;
   }

   typedef DDS::DataWriter NestedBase_1;

   if (NestedBase_1::_local_is_a (_id))
   {
      return true;
   }

   return false;
}

DDSLib::DDSDataDataWriter_ptr DDSLib::DDSDataDataWriter::_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataDataWriter_ptr result = nullptr;
   if (p && p->_is_a (DDSLib::DDSDataDataWriter::_local_id))
   {
      result = dynamic_cast < DDSLib::DDSDataDataWriter_ptr> (p);
      if (result) result->m_count++;
   }
   return result;
}

DDSLib::DDSDataDataWriter_ptr DDSLib::DDSDataDataWriter::_unchecked_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataDataWriter_ptr result;
   result = dynamic_cast < DDSLib::DDSDataDataWriter_ptr> (p);
   if (result) result->m_count++;
   return result;
}

const char * DDSLib::DDSDataDataReader::_local_id = "IDL:DDSLib/DDSDataDataReader:1.0";

DDSLib::DDSDataDataReader_ptr DDSLib::DDSDataDataReader::_duplicate (DDSLib::DDSDataDataReader_ptr p)
{
   if (p) p->m_count++;
   return p;
}

DDS::Boolean DDSLib::DDSDataDataReader::_local_is_a (const char * _id)
{
   if (strcmp (_id, DDSLib::DDSDataDataReader::_local_id) == 0)
   {
      return true;
   }

   typedef DDS::DataReader NestedBase_1;

   if (NestedBase_1::_local_is_a (_id))
   {
      return true;
   }

   return false;
}

DDSLib::DDSDataDataReader_ptr DDSLib::DDSDataDataReader::_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataDataReader_ptr result = nullptr;
   if (p && p->_is_a (DDSLib::DDSDataDataReader::_local_id))
   {
      result = dynamic_cast < DDSLib::DDSDataDataReader_ptr> (p);
      if (result) result->m_count++;
   }
   return result;
}

DDSLib::DDSDataDataReader_ptr DDSLib::DDSDataDataReader::_unchecked_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataDataReader_ptr result;
   result = dynamic_cast < DDSLib::DDSDataDataReader_ptr> (p);
   if (result) result->m_count++;
   return result;
}

const char * DDSLib::DDSDataDataReaderView::_local_id = "IDL:DDSLib/DDSDataDataReaderView:1.0";

DDSLib::DDSDataDataReaderView_ptr DDSLib::DDSDataDataReaderView::_duplicate (DDSLib::DDSDataDataReaderView_ptr p)
{
   if (p) p->m_count++;
   return p;
}

DDS::Boolean DDSLib::DDSDataDataReaderView::_local_is_a (const char * _id)
{
   if (strcmp (_id, DDSLib::DDSDataDataReaderView::_local_id) == 0)
   {
      return true;
   }

   typedef DDS::DataReaderView NestedBase_1;

   if (NestedBase_1::_local_is_a (_id))
   {
      return true;
   }

   return false;
}

DDSLib::DDSDataDataReaderView_ptr DDSLib::DDSDataDataReaderView::_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataDataReaderView_ptr result = nullptr;
   if (p && p->_is_a (DDSLib::DDSDataDataReaderView::_local_id))
   {
      result = dynamic_cast < DDSLib::DDSDataDataReaderView_ptr> (p);
      if (result) result->m_count++;
   }
   return result;
}

DDSLib::DDSDataDataReaderView_ptr DDSLib::DDSDataDataReaderView::_unchecked_narrow (DDS::Object_ptr p)
{
   DDSLib::DDSDataDataReaderView_ptr result;
   result = dynamic_cast < DDSLib::DDSDataDataReaderView_ptr> (p);
   if (result) result->m_count++;
   return result;
}



