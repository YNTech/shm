#ifndef __RTDB_API_H__
#define __RTDB_API_H__

#include "rtdblib.h"
#include <string>
#include "rtdb_define.h"
//#include <list>

RTDBLIB_EXPORT bool rtdb_has_load	(const char* dbsets = nullptr);///<实时库是否加载
RTDBLIB_EXPORT bool rtdb_get_dbnames(char(**dbnames)[db_name_length], int& dbnum, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_tblnames(char *(&tblnames)[max_tables_num], int& tablenum, const char* dbname, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_fieldnames(char *(&fieldnames)[], int& fieldnum, const char* dbname, const char* tablename, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_fieldnames(char *(&fieldnames)[], int& fieldnum, const char* dbname, const int table_index, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_value(const char* dbname, const int table_index, const int field_index, const int row, void** val, int& datalen, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_value(const char* dbname, const char* tablename, const char* fieldname, const int row, void** val, int& datalen, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_set_value(const char* dbname, const int table_index, const int field_index, const int row, const void* val, int datalen, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_set_value(const char* dbname, const char* tablename, const char* fieldname, const int row, const void* val, int datalen, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_rowsize(const char* dbname, const char* tablename, int& rowsize, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_rowcount(const char* dbname, const char* tablename, int& rowcount, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_rowdata(const char* dbname, const char* tablename, const int row, void** val, int& datalen, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_set_rowdata(const char* dbname, const char* tablename, const int row, const void* val, int datalen, char *errString = nullptr, const char* dbsets = nullptr);
RTDBLIB_EXPORT bool rtdb_get_colsdata(const char* dbname, const char* tablename, const char* fieldname, const int beginrow, int& valnum, void **val, int& datalen, char *errString = nullptr, const char* dbsets = nullptr);///<取一列数据,beginrow起始行，valnum输入最大读取行数，返回实际读取行数

////////////
RTDBLIB_EXPORT void* header_rtdbset(const char* dbsetsname);//TYPE_header_RTDbSet
RTDBLIB_EXPORT void* header_rtdb(const char* dbsetsname, const char* dbname);//TYPE_header_RTDb
RTDBLIB_EXPORT void* header_table(const char* dbsetsname, const char* dbname, const char* tablename, char* errString = 0);//ShmDbTblHeaderdef*
RTDBLIB_EXPORT void* header_columndef(const char* dbsetsname, char* dbname, char* tablename, char* errString = 0);//ShmDbColDef*
RTDBLIB_EXPORT void* header_columndef(void* header_rtdb,void* header_table);//ShmDbColDef*
RTDBLIB_EXPORT void* data_rtdb(const char* dbsetsname, const char* dbname,char* errString = 0);//db_data
RTDBLIB_EXPORT void* table_mapped(const char* dbsetsname, const char* dbname, const char* tablename, char* errString = 0);//data_tableXX
RTDBLIB_EXPORT const char* rtdb_lastError();
//////////
#endif // __RTDB_API_H__
