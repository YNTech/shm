/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADER_MODEL_H__
#define __HEADER_MODEL_H__

#include "rtdb_api.h"

#pragma pack(push)
#pragma pack(1)
namespace test_Model
{
    typedef struct Table_0
    {
        int ret;
        int testArry[8];
    } *TYPE_Table_0;
    TYPE_Table_0 Model_Table_0= (TYPE_Table_0)table_mapped("test", "Model", "Table_0"); 
    //end define header_Table_0
    
    typedef struct Table_1
    {
    } *TYPE_Table_1;
    TYPE_Table_1 Model_Table_1= (TYPE_Table_1)table_mapped("test", "Model", "Table_1"); 
    //end define header_Table_1
    
}
#pragma pack(pop)

#endif//__HEADER_MODEL_H__
