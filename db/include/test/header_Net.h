/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADER_NET_H__
#define __HEADER_NET_H__

#include "rtdb_api.h"

#pragma pack(push)
#pragma pack(1)
namespace test_Net
{
    typedef struct scada_channel
    {
        int index;
        char name;
        char type;
        int db_id;
        int tbl_id;
    } *TYPE_scada_channel;
    TYPE_scada_channel Net_scada_channel= (TYPE_scada_channel)table_mapped("test", "Net", "scada_channel"); 
    //end define header_scada_channel
    
}
#pragma pack(pop)

#endif//__HEADER_NET_H__
