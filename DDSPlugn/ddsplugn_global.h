#ifndef DDSPLUGN_GLOBAL_H
#define DDSPLUGN_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DDSPLUGN_LIB
# define DDSPLUGN_EXPORT Q_DECL_EXPORT
#else
# define DDSPLUGN_EXPORT Q_DECL_IMPORT
#endif

#endif // DDSPLUGN_GLOBAL_H
