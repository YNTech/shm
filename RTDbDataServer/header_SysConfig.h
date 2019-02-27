/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADER_SYSCONFIG_H__
#define __HEADER_SYSCONFIG_H__

#include "rtdb_api.h"
#include "rtdb_interface.h"

#pragma pack(push)
#pragma pack(1)
TYPE_header_RTDb	header_SysConfig = (TYPE_header_RTDb)header_rtdb(getDefaultDbSets(), "SysConfig");
typedef struct Nodes
{
    char name[16];
    char host[32];
    char desc[256];
    short nodeTyp;
} *TYPE_Nodes;
TYPE_Nodes SysConfig_Nodes= (TYPE_Nodes)table_mapped(getDefaultDbSets(), "SysConfig", "Nodes"); 
//end define header_Nodes
    
typedef struct PS
{
    int node;
    int channel;
    int Plugin;
    char PubAndSub;
} *TYPE_PS;
TYPE_PS SysConfig_PS = (TYPE_PS)table_mapped(getDefaultDbSets(), "SysConfig", "PS");
//end define header_PS
    
typedef struct plugin
{
    char name[16];
    char type;
    char fileName[32];
    char desc[256];
} *TYPE_plugin;
TYPE_plugin SysConfig_plugin = (TYPE_plugin)table_mapped(getDefaultDbSets(), "SysConfig", "plugin");
//end define header_plugin
    
typedef struct channel
{
    char name[32];
    char type;
    int db;
    int tbl_id;
    int col_id;
    int updateCycle;
} *TYPE_channel;
TYPE_channel SysConfig_channel = (TYPE_channel)table_mapped(getDefaultDbSets(), "SysConfig", "channel");
//end define header_channel
    
typedef struct plugin_config
{
    int plgn;
    int node;
    char connectPara[128];
} *TYPE_plugin_config;
TYPE_plugin_config SysConfig_plugin_config = (TYPE_plugin_config)table_mapped(getDefaultDbSets(), "SysConfig", "plugin_config");
//end define header_plugin_config
    
#pragma pack(pop)

#endif//__HEADER_SYSCONFIG_H__
