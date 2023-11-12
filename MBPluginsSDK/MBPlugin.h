#ifndef MBPLUGINH
#define MBPLUGINH


#ifndef MBPLUGIN_VERSION
#define MBPLUGIN_VERSION	1009
#endif


#define MBPLUGIN_EXPORT extern "C" __declspec(dllexport)
#define MBPLUGIN_API	extern "C" 

#include <math.h>



#ifndef MBPLUGIN_EXPORTS
//Version
MBPLUGIN_API int (*MBGetVersion)( int* verptr );

#endif


#endif