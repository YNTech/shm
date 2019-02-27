#ifndef RTDBLIB_GLOBAL_H
#define RTDBLIB_GLOBAL_H


#ifdef RTDBLIB_LIB
	#define RTDBLIB_EXPORT __declspec(dllexport)
#else
	#define RTDBLIB_EXPORT __declspec(dllimport)	
#ifdef _DEBUG
# pragma comment(lib, "RTDbLibd.lib")
#else
# pragma comment(lib, "RTDbLib.lib")
#endif
#endif
#endif // RTDBLIB_GLOBAL_H
