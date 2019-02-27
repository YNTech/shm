#ifndef DDSLIBDATASPLTYPES_H
#define DDSLIBDATASPLTYPES_H

//#include "ccpp_HelloWorldData.h"
#include "ddalib_export.h"
#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>

extern c_metaObject __DDSLibData_DDSLib__load (c_base base);

extern c_metaObject __DDSLib_DDSData__load (c_base base);
extern const char * __DDSLib_DDSData__keys (void);
extern const char * __DDSLib_DDSData__name (void);

struct _DDSLib_DDSData {
	_DDSLib_DDSData()
	{
		
	}
	~_DDSLib_DDSData()
	{
	}
	c_long ind;
	union
	{
		c_char* pdata;
		c_char  rdata[1];
	};
};
extern c_bool __DDSLib_DDSData__copyIn(struct base_s * base, struct _DDSLib_DDSData *from, struct _DDSLib_DDSData *to);
extern void __DDSLib_DDSData__copyOut(void *_from, void *_to);


#endif
