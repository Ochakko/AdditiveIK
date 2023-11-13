#ifndef PLUGINELEMH
#define PLUGINELEMH

#include <windows.h>

class CPluginElem
{
public:
	CPluginElem();
	~CPluginElem();


public:
	HMODULE hModule;
	WCHAR filepath[_MAX_PATH];
	DWORD pluginversion;
	DWORD product;
	DWORD id;
	
	int menuid;

	int validflag;

	WCHAR pluginname[_MAX_PATH];


	//int (*MBOnSelectPlugin)();
	int (*MBCreateMotionBrush)(double srcstartframe, double srcendframe, double srcapplyframe, double srcframeleng, int srcrepeats, int srcmirroru, int srcmirrorv, int srcdiv2, float* dstvalue);
	int (*MBOnClose)();
	int (*MBOnPose)( int motid );

public:
	//int CallOnSelectPlugin();
	int CreateMotionBrush(double srcstartframe, double srcendframe, double srcapplyframe, double srcframeleng, int srcrepeats, int srcmirroru, int srcmirrorv, int srcdiv2, float* dstvalue);
	int CallOnClose();
	int CallOnPose( int motid );

	int SetFilePath( WCHAR* pluginpath );
	int LoadPlugin();

private:
	int UnloadPlugin();


private:
	const WCHAR* (*MBGetPlugInName)();
	int (*MBGetPlugInID)(DWORD *Product, DWORD *ID);
	int (*MBCheckVersion)( DWORD exe_version, DWORD* pluginver );
	int (*MBInit)( WCHAR *exe_name );

};

#endif

