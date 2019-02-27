/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADER_DB_1_H__
#define __HEADER_DB_1_H__

#include "rtdb_api.h"

#pragma pack(push)
#pragma pack(1)
namespace test_db_1
{
    typedef struct tbl1
    {
        int ind;
        char name[32];
        char type;
    } *TYPE_tbl1;
    TYPE_tbl1 db_1_tbl1= (TYPE_tbl1)table_mapped("test", "db_1", "tbl1"); 
    //end define header_tbl1
    
}
#pragma pack(pop)

#endif//__HEADER_DB_1_H__
