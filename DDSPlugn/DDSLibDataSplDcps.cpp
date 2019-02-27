#include "DDSLibDataSplDcps.h"
//#include "ccpp_DDSLibData.h"

const char *
__DDSLib_DDSData__name(void)
{
    return (const char*)"DDSLib::DDSData";
}

const char *
__DDSLib_DDSData__keys(void)
{
    return (const char*)"ind";
}

#include <v_kernel.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>
#include "ddslib.h"
c_bool
__DDSLib_DDSData__copyIn(
    struct base_s *base,
    struct _DDSLib_DDSData *from,
    struct _DDSLib_DDSData *to)
{
    //c_bool result = TRUE;
    to->ind = (c_long)from->ind;
	DDSWriter& writer = (((ddslib*)(ddslib::GetInstancePtr()))->m_psdata->m_writers[from->ind>>16]);
    {
		from->pdata =  (char*)writer.pdata + (from->ind&0x0000FFFF)*writer.datalen;
        memcpy (to->rdata, from->pdata, writer.datalen);
    }
    return TRUE;
}

void
__DDSLib_DDSData__copyOut(
    void *_from,
    void *_to)
{
    struct _DDSLib_DDSData *from = (struct _DDSLib_DDSData *)_from;
    struct _DDSLib_DDSData *to = (struct _DDSLib_DDSData *)_to;
    to->ind = from->ind;
	DDSReader& reader = ((ddslib*)(ddslib::GetInstancePtr()))->m_psdata->m_readers[from->ind>>16];
    {
		to->pdata =  (char*)reader.pdata + (from->ind&0x0000FFFF)*reader.datalen;
        memcpy (to->pdata, &from->rdata, reader.datalen);
    }
}

