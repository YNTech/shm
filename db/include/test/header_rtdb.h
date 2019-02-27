/****************************************************************************
** Created by: The Resource Created by rtdb_tool
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/


#ifndef __HEADERDB_H__
#define __HEADERDB_H__

#include "rtdb_define.h"
#include "rtdb_api.h"
#include "header_SCADA.h"
#include "header_Model.h"
#include "header_SysConfig.h"

namespace test
{
    TYPE_header_RTDbSet	header_test = (TYPE_header_RTDbSet)header_rtdbset("test");
    TYPE_header_RTDb	header_SCADA = (TYPE_header_RTDb)header_rtdb("test","SCADA");
    TYPE_header_RTDb	header_Model = (TYPE_header_RTDb)header_rtdb("test","Model");
    TYPE_header_RTDb	header_SysConfig = (TYPE_header_RTDb)header_rtdb("test","SysConfig");
};

#endif//__HEADERDB_H__

