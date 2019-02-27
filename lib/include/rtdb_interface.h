#ifndef __RTDB_INTERFACE_H__
#define __RTDB_INTERFACE_H__
#include "rtdblib.h"
//#include <string>
#include <map>
struct RTDbSets;
struct dataTypeDef;
RTDBLIB_EXPORT bool loadRTDb(const char* dbsetsname, const char* basepath, char*  errString = 0);///<加载库
RTDBLIB_EXPORT bool closeRTDb(const char* dbsetsname, char*  errString = 0);///<关闭卸载库
RTDBLIB_EXPORT bool readRTDbSetsDef(const char* basepath, const char* dbsetsname, char*  errString = 0);///<加载实时库定义
RTDBLIB_EXPORT bool genDbSetsDefFile(const char* basepath, RTDbSets* dbsets, char* errString = 0);///<生成定义文件
RTDBLIB_EXPORT bool genDbDefFile(const char* basepath, RTDbSets* dbsets, const char* dbname, char*  errString = 0);///<生成定义文件

RTDBLIB_EXPORT bool genRTDB(const char* basepath, const char* dbsetsname, char*  errString = 0);///<重建整个实时库
RTDBLIB_EXPORT bool genDbSetsFile(const char* basepath, const char* dbsetsname, char*  errString = 0);///<生成实时库文件
RTDBLIB_EXPORT bool genDbFile(const char* basepath, const char* dbsetsname, const char* dbname, char*  errString = 0);///<生成实时库

RTDBLIB_EXPORT RTDbSets* getRtDbDef(const char* dbsetsname);///<获取指定库定义
RTDBLIB_EXPORT RTDbSets* createRtDbDef(const char* dbsetsname);///<创建空数据库
RTDBLIB_EXPORT std::map<int, dataTypeDef>& getRtDbDataType(const char* basePath);///<获取所有数据类型定义
RTDBLIB_EXPORT void setDefauluDbSets(const char* dbsets);
RTDBLIB_EXPORT const char* getDefaultDbSets();
RTDBLIB_EXPORT const char* getBasePath();
RTDBLIB_EXPORT unsigned char getRTDBState();///<获取RTDB运行状态
RTDBLIB_EXPORT void setRTDBState(unsigned char rtdb_state);
RTDBLIB_EXPORT unsigned int getRTDBConfig();///<获取RTDB配置字
RTDBLIB_EXPORT void setRTDBConfig(unsigned int rtdb_state);
RTDBLIB_EXPORT void dbsetsReName(const char* oldname,const char* newname);
RTDBLIB_EXPORT void verDbSets(RTDbSets* dbsets);//计算dbsets的版本信息
/////

//
#endif// __RTDB_INTERFACE_H__