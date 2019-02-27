/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADER_NETSETTINGS_H__
#define __HEADER_NETSETTINGS_H__

#include "rtdb_api.h"

#pragma pack(push)
#pragma pack(1)
namespace test_NetSettings
{
    typedef struct Nodes
    {
        char name[16];
        char IP[16];
        char desc[256];
        char isCurrentNd;
    } *TYPE_Nodes;
    TYPE_Nodes NetSettings_Nodes= (TYPE_Nodes)table_mapped("test", "NetSettings", "Nodes"); 
    //end define header_Nodes
    
    typedef struct PS
    {
        int channel;
        int Plugin;
        char publish;
        char subscribe;
    } *TYPE_PS;
    TYPE_PS NetSettings_PS= (TYPE_PS)table_mapped("test", "NetSettings", "PS"); 
    //end define header_PS
    
    typedef struct plugin
    {
        char type;
        char name[16];
        char fileName[32];
        char desc[256];
    } *TYPE_plugin;
    TYPE_plugin NetSettings_plugin= (TYPE_plugin)table_mapped("test", "NetSettings", "plugin"); 
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
    TYPE_channel NetSettings_channel= (TYPE_channel)table_mapped("test", "NetSettings", "channel"); 
    //end define header_channel
    
}
#pragma pack(pop)

#endif//__HEADER_NETSETTINGS_H__
