/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2012 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE
 *
 *   for full copyright notice and license terms.
 *
 */
#include "os_if.h"

#ifdef _DDSLIB
#define _DDSLIB_API OS_API_EXPORT
#else
#define _DDSLIB_API OS_API_IMPORT
//#ifdef _DEBUG
//# pragma comment(lib, "ddslibd.lib")
//#else
//# pragma comment(lib, "ddslib.lib")
//#endif
#endif

