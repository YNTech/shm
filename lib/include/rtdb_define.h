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
	//rtdb_stop,  //��ʼ״̬��δ����
	rtdb_run = 0x01,   //����
	rtdb_Synching = 0x02,  //ͬ���У�editor�����޸�rtdb
	rtdb_AlreadySync = 0x04,//��ͬ�������л�����������ͬ��
};
enum eRTDBConfig{
	rtdb_heartbeat=0x01,  //�Ƿ���������
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
	int tplen;//���ݳ���
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
	std::string md5;//����db��md5ֵ�ϲ�����MD5
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
	unsigned int tbl_address;//��ƫ��
	unsigned int tbl_coldef_addr;//�ж���ƫ��
	unsigned int max_row_num;//�������
	unsigned int row_size; //ÿ�д�С
	unsigned int row_num;//��Ч����
	unsigned int col_num;//����
	char tbl_name[tbl_name_length];//����
};
struct ShmDbColDef
{
	unsigned int col_size;//�����ݴ�С
	char col_name[col_name_length];//������
};
//db.db//tbl_num*row_num*row_size
//struct ShmDbTblDef 
//{
//	unsigned char* table;
//};

typedef struct 
{
	unsigned short db_nums;//���ݿ����
	char db_name[][db_name_length];
} *TYPE_header_RTDbSet;
//TYPE_header_RTDbSet	header_rtdbset;

typedef struct
{
	unsigned short tbl_nums;//�����
	ShmDbTblHeaderdef tbl_header[];
} *TYPE_header_RTDb;
//TYPE_header_RTDb	header_rtdbXX;

#pragma pack(pop)

//
//////////////////////////////////////////////////////////////////////////
#endif//__RTDB_DEFINE_H__