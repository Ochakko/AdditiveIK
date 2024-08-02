
#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "PluginElem.h"

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

//#include "MB2Version.h"
//#define MB2VERSION 10017

//2021/11/03
//#define MB2VERSION 10018

//2021/11/04
//#define MB2VERSION 10019

//2021/11/15
//#define MB2VERSION 10020

//2021/11/22
//#define MB2VERSION 10021

//2021/11/27
//#define MB2VERSION 10022

//2022/0107
//#define MB2VERSION 10023

//2022/05/17
//#define MB2VERSION 10025

//2022/08/16
//#define MB2VERSION 10026

//2022/09/01
//#define MB2VERSION 10027

//2022/09/12
//#define MB2VERSION 10028

//2022/09/19
//#define MB2VERSION 10029

//2022/10/13
//#define MB2VERSION 10030

//2022/10/20
//#define MB2VERSION 10031

//2022/10/26
//#define MB2VERSION 10032

//2022/10/31
//#define MB2VERSION 10033

//2022/11/09
//#define MB2VERSION 10034

//2022/11/23
//#define MB2VERSION 10035

//2022/12/17 ##### 桁数を合わせるために1101ではなく11010
//#define MB2VERSION 11010

//2023/01/08
//#define MB2VERSION 11011

//2023/01/18
//#define MB2VERSION 11012

//2023/01/24
//#define MB2VERSION 11013

//2023/02/01
//#define MB2VERSION 11014

//2023/02/06
//#define MB2VERSION 12010

//2023/02/11
//#define MB2VERSION 12011

//2023/02/17
//#define MB2VERSION 12012

//2023/02/24
//#define MB2VERSION 12013

//2023/03/01
//#define MB2VERSION 12014

//2023/03/05
//#define MB2VERSION 12015

//2023/03/08
//#define MB2VERSION 12016

//2023/03/15
//#define MB2VERSION 12017

//2023/03/26
//#define MB2VERSION 12018

//2023/03/27
//#define MB2VERSION 12019

//2023/03/27
//#define MB2VERSION 12020

//2023/04/13
//#define MB2VERSION 12021

//2023/07/21
//#define MB2VERSION 12023

//2023/08/04
//#define MB2VERSION 12024

//2023/09/04
//#define MB2VERSION 12025

//2023/09/28
//#define MB2VERSION 12026

//2023/10/11
//#define MB2VERSION 12027

////2023/11/06
//#define MB2VERSION 12029

//2023/11/07
//#define MB2VERSION 12030


//2023/11/23 先頭に２を付加　最後の２桁は0.0.0.1の1を２桁表現
//#define MB2VERSION 210001

//2023/12/30
//#define MB2VERSION 210002

//2024/01/05
//#define MB2VERSION 210003

//2024/01/19
//#define MB2VERSION 210004

//2024/01/22
//#define MB2VERSION 210005

//2024/01/30 先頭に２を付加　最後の２桁は1.0.0.6の6を２桁表現
//#define MB2VERSION 210006

//2024/01/30 先頭に２を付加　最後の２桁は1.0.0.7の7を２桁表現
//#define MB2VERSION 210007

//2024/02/13 先頭に２を付加　最後の２桁は1.0.0.8の8を２桁表現
//#define MB2VERSION 210008

//2024/02/17 先頭に２を付加　最後の２桁は1.0.0.9の9を２桁表現
//#define MB2VERSION 210009

//2024/02/23 先頭に２を付加
//#define MB2VERSION 210010

//2024/03/02 先頭に２を付加
//#define MB2VERSION 210011

//2024/03/11 先頭に２を付加
//#define MB2VERSION 210012

//2024/03/22 先頭に２を付加
//#define MB2VERSION 210013

//2024/03/30 先頭に２を付加
//#define MB2VERSION 210014

//2024/04/09 先頭に２を付加
//#define MB2VERSION 210015

//2024/04/12 先頭に２を付加
//#define MB2VERSION 210016

//2024/04/22 先頭に２を付加
//#define MB2VERSION 210017

//2024/04/26 先頭に２を付加
//#define MB2VERSION 210018

//2024/05/02 先頭に２を付加
//#define MB2VERSION 210019

//2024/05/10 先頭に２を付加
//#define MB2VERSION 210020

//2024/05/16 先頭に２を付加
//#define MB2VERSION 210021

//2024/05/16 先頭に２を付加
//#define MB2VERSION 210022

//2024/05/28 先頭に２を付加
//#define MB2VERSION 210023

//2024/06/14 先頭に２を付加
//#define MB2VERSION 210024

//2024/06/21 先頭に２を付加
//#define MB2VERSION 210025

//2024/06/29 先頭に２を付加
//#define MB2VERSION 210026

//2024/07/05 先頭に２を付加
//#define MB2VERSION 210027

//2024/07/14 先頭に２を付加
//#define MB2VERSION 210028

//2024/08/02 先頭に２を付加
#define MB2VERSION 210029


CPluginElem::CPluginElem()
{
	ZeroMemory( filepath, sizeof( char ) * _MAX_PATH );
	pluginversion = 0;
	product = 0;
	id = 0;

	menuid = 0;

	ZeroMemory( pluginname, sizeof( char ) * _MAX_PATH );
	
	validflag = 0;//!!!!!!!!!!!!!!!!!!!!!!

	MBCreateMotionBrush = NULL;
	MBOnClose = NULL;
	MBOnPose = NULL;

	hModule = NULL;

}
CPluginElem::~CPluginElem()
{
	UnloadPlugin();
}

int CPluginElem::CreateMotionBrush(double srcstartframe, double srcendframe, double srcapplyframe, double srcframeleng, int srcrepeats, int srcmirroru, int srcmirrorv, int srcdiv2, float* dstvalue)
{
	int ret = 0;

	if( (validflag == 1) && MBCreateMotionBrush){//！！！！！！！ validflagに注意！！！！！！
		//ret = MBOnSelectPlugin();
		//if ((srcstartframe >= 0.0) && (srcstartframe < 1e5) && (srcendframe >= srcstartframe) && (srcendframe < 1e5) &&
		//	(srcapplyframe >= srcstartframe) && (srcapplyframe <= srcendframe) &&
		//	(srcframeleng > srcendframe) && (srcframeleng < 1e5) && dstvalue) {

		if ((srcstartframe >= 0.0) && 
			(srcstartframe < 1e5) && (srcendframe >= srcstartframe) && 
			(srcapplyframe < 1e5) && (srcapplyframe <= srcendframe) &&
			(srcendframe < 1e5) && (srcframeleng >= srcendframe) &&
			(srcframeleng < 1e5) && dstvalue) {

			ret = MBCreateMotionBrush(srcstartframe, srcendframe, srcapplyframe, srcframeleng, srcrepeats, srcmirroru, srcmirrorv, srcdiv2, dstvalue);
		}
		else {
			_ASSERT(0);
			return 2;//フレーム範囲外は2を返す
		}
	}

	return ret;
}
int CPluginElem::CallOnClose()
{
	int ret = 0;

	if( (validflag == 1) && MBOnClose ){//！！！！！！！ validflagに注意！！！！！！
		ret = MBOnClose();
	}

	return ret;
}
int CPluginElem::CallOnPose( int motid )
{
	int ret = 0;

	if( (validflag == 1) && MBOnPose ){//！！！！！！！ validflagに注意！！！！！！
		ret = MBOnPose( motid );
	}

	return ret;
}

int CPluginElem::SetFilePath( WCHAR* pluginpath )
{
	if (!pluginpath) {
		_ASSERT(0);
		return 1;
	}

	size_t leng;
	*(pluginpath + MAX_PATH - 1) = 0L;//!!!
	leng = wcslen( pluginpath );

	if( (leng <= 0) || (leng >= _MAX_PATH) ){
		DbgOut( L"pluginelem : SetFilePath : leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	wcscpy_s( filepath, _MAX_PATH, pluginpath );

	return 0;
}


#define GPA(proc) \
	*(FARPROC *)&proc = GetProcAddress(hModule, #proc);


int CPluginElem::LoadPlugin()
{
	validflag = 0;//!!!!!!!!!!!!

	hModule = LoadLibrary( filepath );
	if(hModule == NULL){
		DbgOut( L"pluginelem : LoadPlugin : LoadLibrary error %s!!!\r\n", filepath );
		_ASSERT( 0 );
		return 0;
	}

	GPA(MBGetPlugInName);
	GPA(MBGetPlugInID);
	GPA(MBCheckVersion);
	GPA(MBInit);

	if( !MBGetPlugInName || !MBGetPlugInID || !MBCheckVersion || !MBInit ){
		DbgOut( L"pluginelem : LoadPlugin : this dll is not for MB2 %s\r\n", filepath );
		//DbgOut( L"%x %x %x %x\r\n", MBGetPlugInName, MBGetPlugInID, MBCheckVersion, MBInit );
		return 0;//!!!!!!!!!!!!!!!!!
	}

/////////////

	if( MBGetPlugInName ){
		const WCHAR* nameptr = MBGetPlugInName();
		if( !nameptr ){
			DbgOut( L"pluginelem : LoadPlugin : MBGetPlugInName return NULL error %s\r\n", filepath );
			_ASSERT( 0 );
			return 0;
		}

		wcscpy_s( pluginname, _MAX_PATH, nameptr );
		pluginname[_MAX_PATH - 1] = 0;
	}

////////////
	if( MBGetPlugInID ){
		MBGetPlugInID( &product, &id );
	}
////////////
	if( MBCheckVersion ){
		MBCheckVersion( (DWORD)MB2VERSION, &pluginversion );
		if( pluginversion == 0 ){
			DbgOut( L"pluginelem : LoadPlugin : this plugin is not supported by MB2 ver%d : %s\r\n", MB2VERSION, filepath );
			_ASSERT( 0 );
			return 0;
		}
	}


/////////////
	int ret;
	if( MBInit ){

		WCHAR filename[2048];
		ZeroMemory( filename, 2048 );

		ret = GetModuleFileName( NULL, filename, 2048 );
		if( ret == 0 ){
			_ASSERT( 0 );
			DbgOut( L"pluginelem : LoadPlugin : GetModuleFileName error !!!\n" );
			return 1;
		}

		ret = MBInit( filename );
		if( ret ){
			DbgOut( L"pluginelem : LoadPlugin : MBInit error %s\r\n", filepath );
			_ASSERT( 0 );
			return 0;
		}
	}

////////////
	validflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!	


////////////
	GPA(MBCreateMotionBrush);
	GPA(MBOnClose);
	GPA(MBOnPose);

	return 0;
}
int CPluginElem::UnloadPlugin()
{
	validflag = 0;//!!!!!!!!!!!!!!!!

	if( hModule ){
		FreeLibrary( hModule );
		hModule = NULL;
	}
	
	return 0;
}


