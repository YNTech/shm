/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADER_DB_2_H__
#define __HEADER_DB_2_H__

#include "rtdb_api.h"

#pragma pack(push)
#pragma pack(1)
namespace test_db_2
{
    typedef struct Table_0
    {
        int ret;
    } *TYPE_Table_0;
    TYPE_Table_0 db_2_Table_0= (TYPE_Table_0)table_mapped("test", "db_2", "Table_0"); 
    //end define header_Table_0
    
    typedef struct Table_1
    {
    } *TYPE_Table_1;
    TYPE_Table_1 db_2_Table_1= (TYPE_Table_1)table_mapped("test", "db_2", "Table_1"); 
    //end define header_Table_1
    
}
#pragma pack(pop)

#endif//__HEADER_DB_2_H__
