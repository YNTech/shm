#ifndef ALARMBOARDEXT_GLOBAL_H
#define ALARMBOARDEXT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef ALARMBOARDEXT_LIB
# define ALARMBOARDEXT_EXPORT Q_DECL_EXPORT
#else
# define ALARMBOARDEXT_EXPORT Q_DECL_IMPORT

#ifdef _DEBUG
#pragma comment(lib, "AlarmBoardExtd.lib")
#else
#pragma comment(lib, "AlarmBoardExt.lib")
#endif

#endif

#endif // ALARMBOARDEXT_GLOBAL_H
