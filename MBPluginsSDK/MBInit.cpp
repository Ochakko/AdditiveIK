#include "pch.h"

//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "MBPlugin.h"

#include <crtdbg.h> // <---- _ASSERTマクロ


///////////////////

//Version
int (*MBGetVersion)( int* verptr );


MBPLUGIN_EXPORT int MBCheckVersion( DWORD exe_version, DWORD* pluginver )
{
	if (exe_version < 1005) {
		*pluginver = 0;// !!!!!!!!!!! 古いAdditiveIKでは、使用不可
	}
	else {
		*pluginver = MBPLUGIN_VERSION;
	}

	return 0;
}


#define GetAdr(proc) \
	*(FARPROC *)&proc = GetProcAddress(hModule, #proc); \
	if(proc == NULL){ _ASSERT( 0 ); goto MBINIT_EXIT; }



MBPLUGIN_EXPORT int MBInit( WCHAR *exe_name )
{
	HMODULE hModule = LoadLibrary( exe_name );
	if(hModule == NULL)
		return 1;

	int result = 1;

	//GetAdr(MBGetCurrentHSID);

	result = 0;

//MBINIT_EXIT:
	FreeLibrary(hModule);
	return result;
}



