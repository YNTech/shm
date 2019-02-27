#ifndef __RTDB_DEFINE_H__
#define __RTDB_DEFINE_H__

#include <string>
//#include <list>
#include <vector>

#define db_name_length 32
#define tbl_name_length 32
#define col_name_length 32
//The maximum number of DB in each DBSet
//#define max_db_num  128
//The maximum number of tables in each database
#define max_tables_num  256
#define max_columns_num  128
//dataTypeProp_varSize = 0x01,
//struct RTDbDataType
//{
//	int ntype;
//	std::string stype;
//};
//RTDbDataType g_rtdbDTp[]={
//	1,
//}
enum enumrtdbdtp{dataTp_int=1,dataTp_float=2,dataTp_double=3,dataTp_short=4,dataTp_char=5,dataTp_string=11,dataTp_varbtye=12,dataTp_image=13,};
enum eRTDBState{ 
	//rtdb_stop,  //初始状态，未启动
	rtdb_run = 0x01,   //运行
	rtdb_Synching = 0x02,  //同步中，editor不可修改rtdb
	rtdb_AlreadySync = 0x04,//已同步过，切换服务器不再同步
};
enum eRTDBConfig{
	rtdb_heartbeat=0x01,  //是否配置心跳
};
struct dataTypeDef
{
	enum dataTypeProp{dataTypeProp_value = 1,dataTypeProp_string = 2,dataTypeProp_image = 3};
	int id;
	std::string name;
	int nsize;
	unsigned char dataProp;
};
//
struct  coldef
{
	std::string name;
	std::string dispname;
	int datatype;
	int tplen;//数据长度
	std::string data_constraint;
	std::string default_val;
};

struct tbldef
{
	std::string name;
	int row_num;
	//int col_num;
	std::vector<coldef> cols;
};

struct RTDbInfo
{
	//int tbl_num;
	std::string name;
	std::string md5;
	int ver;
	time_t modifyTm;
	std::vector<tbldef> tbls;
};

struct RTDbSets
{
	std::string dbsets;
	std::string md5;//所有db的md5值合并计算MD5
	int ver;
	time_t modifyTm;
	unsigned short dbnums;
	std::vector<RTDbInfo> rtdbs;
};
//////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack (1)
//connom def
struct RTDBCommon
{
	unsigned char  RTDBState;
	char defaultDbSets[32];
	char basePath[256];
	unsigned int rtdb_config;
};
//
//dbsets.db://2+dbnums*db_name_length
struct ShmDbTblHeaderdef 
{
	unsigned int tbl_address;//表偏移
	unsigned int tbl_coldef_addr;//列定义偏移
	unsigned int max_row_num;//最大行数
	unsigned int row_size; //每行大小
	unsigned int row_num;//有效行数
	unsigned int col_num;//列数
	char tbl_name[tbl_name_length];//表名
};
struct ShmDbColDef
{
	unsigned int col_size;//列数据大小
	char col_name[col_name_length];//列名称
};
//db.db//tbl_num*row_num*row_size
//struct ShmDbTblDef 
//{
//	unsigned char* table;
//};

typedef struct 
{
	unsigned short db_nums;//数据库个数
	char db_name[][db_name_length];
} *TYPE_header_RTDbSet;
//TYPE_header_RTDbSet	header_rtdbset;

typedef struct
{
	unsigned short tbl_nums;//表个数
	ShmDbTblHeaderdef tbl_header[];
} *TYPE_header_RTDb;
//TYPE_header_RTDb	header_rtdbXX;

#pragma pack(pop)

//
//////////////////////////////////////////////////////////////////////////
#endif//__RTDB_DEFINE_H__