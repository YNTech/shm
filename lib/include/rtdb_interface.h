#ifndef __RTDB_INTERFACE_H__
#define __RTDB_INTERFACE_H__
#include "rtdblib.h"
//#include <string>
#include <map>
struct RTDbSets;
struct dataTypeDef;
RTDBLIB_EXPORT bool loadRTDb(const char* dbsetsname, const char* basepath, char*  errString = 0);///<���ؿ�
RTDBLIB_EXPORT bool closeRTDb(const char* dbsetsname, char*  errString = 0);///<�ر�ж�ؿ�
RTDBLIB_EXPORT bool readRTDbSetsDef(const char* basepath, const char* dbsetsname, char*  errString = 0);///<����ʵʱ�ⶨ��
RTDBLIB_EXPORT bool genDbSetsDefFile(const char* basepath, RTDbSets* dbsets, char* errString = 0);///<���ɶ����ļ�
RTDBLIB_EXPORT bool genDbDefFile(const char* basepath, RTDbSets* dbsets, const char* dbname, char*  errString = 0);///<���ɶ����ļ�

RTDBLIB_EXPORT bool genRTDB(const char* basepath, const char* dbsetsname, char*  errString = 0);///<�ؽ�����ʵʱ��
RTDBLIB_EXPORT bool genDbSetsFile(const char* basepath, const char* dbsetsname, char*  errString = 0);///<����ʵʱ���ļ�
RTDBLIB_EXPORT bool genDbFile(const char* basepath, const char* dbsetsname, const char* dbname, char*  errString = 0);///<����ʵʱ��

RTDBLIB_EXPORT RTDbSets* getRtDbDef(const char* dbsetsname);///<��ȡָ���ⶨ��
RTDBLIB_EXPORT RTDbSets* createRtDbDef(const char* dbsetsname);///<���������ݿ�
RTDBLIB_EXPORT std::map<int, dataTypeDef>& getRtDbDataType(const char* basePath);///<��ȡ�����������Ͷ���
RTDBLIB_EXPORT void setDefauluDbSets(const char* dbsets);
RTDBLIB_EXPORT const char* getDefaultDbSets();
RTDBLIB_EXPORT const char* getBasePath();
RTDBLIB_EXPORT unsigned char getRTDBState();///<��ȡRTDB����״̬
RTDBLIB_EXPORT void setRTDBState(unsigned char rtdb_state);
RTDBLIB_EXPORT unsigned int getRTDBConfig();///<��ȡRTDB������
RTDBLIB_EXPORT void setRTDBConfig(unsigned int rtdb_state);
RTDBLIB_EXPORT void dbsetsReName(const char* oldname,const char* newname);
RTDBLIB_EXPORT void verDbSets(RTDbSets* dbsets);//����dbsets�İ汾��Ϣ
/////

//
#endif// __RTDB_INTERFACE_H__