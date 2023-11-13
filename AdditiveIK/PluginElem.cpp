
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

//2022/12/17 ##### ���������킹�邽�߂�1101�ł͂Ȃ�11010
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
#define MB2VERSION 12030


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

	if( (validflag == 1) && MBCreateMotionBrush){//�I�I�I�I�I�I�I validflag�ɒ��ӁI�I�I�I�I�I
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
			return 2;//�t���[���͈͊O��2��Ԃ�
		}
	}

	return ret;
}
int CPluginElem::CallOnClose()
{
	int ret = 0;

	if( (validflag == 1) && MBOnClose ){//�I�I�I�I�I�I�I validflag�ɒ��ӁI�I�I�I�I�I
		ret = MBOnClose();
	}

	return ret;
}
int CPluginElem::CallOnPose( int motid )
{
	int ret = 0;

	if( (validflag == 1) && MBOnPose ){//�I�I�I�I�I�I�I validflag�ɒ��ӁI�I�I�I�I�I
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


