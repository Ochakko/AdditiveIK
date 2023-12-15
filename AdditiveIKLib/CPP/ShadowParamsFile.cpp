#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <ShadowParamsFile.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;



CShadowParamsFile::CShadowParamsFile()
{
	InitParams();
}

CShadowParamsFile::~CShadowParamsFile()
{
	DestroyObjs();
}

int CShadowParamsFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CShadowParamsFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CShadowParamsFile::WriteShadowParamsFile(const WCHAR* srcfilepath)
{
	if (!srcfilepath) {
		_ASSERT(0);
		return 1;
	}



	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile(srcfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"ShadowParamsFile : WriteShadowParamsFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<SHADOWPARAMS>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	//CallF(Write2File("  <TotalScale>%.3f</TotalScale>\r\n", g_fLightScale), return 1);
	//int lightcnt;
	//for(lightcnt = 0; lightcnt < LIGHTNUMMAX; lightcnt++ ){
	//	CallF(WriteLight(slotindex, lightcnt), return 1 );
	//}

	//float g_shadowmap_fov = 60.0f;
	//float g_shadowmap_projscale = 1.0f;
	//float g_shadowmap_near = 50.0f;
	//float g_shadowmap_far = 2000.0f;
	//float g_shadowmap_color = 0.5f;
	//float g_shadowmap_bias = 0.0010f;
	//float g_shadowmap_plusup = 300.0f;
	//float g_shadowmap_plusright = 1.0f;
	//int g_shadowmap_lightdir = 1;
	CallF(Write2File("    <ShadowMapFov>%.1f</ShadowMapFov>\r\n", g_shadowmap_fov), return 1);
	CallF(Write2File("    <ShadowMapProjScale>%.1f</ShadowMapProjScale>\r\n", g_shadowmap_projscale), return 1);
	CallF(Write2File("    <ShadowMapNear>%.1f</ShadowMapNear>\r\n", g_shadowmap_near), return 1);
	CallF(Write2File("    <ShadowMapFar>%.1f</ShadowMapFar>\r\n", g_shadowmap_far), return 1);
	CallF(Write2File("    <ShadowMapColor>%.2f</ShadowMapColor>\r\n", g_shadowmap_color), return 1);
	CallF(Write2File("    <ShadowMapBias>%.3f</ShadowMapBias>\r\n", g_shadowmap_bias), return 1);
	CallF(Write2File("    <ShadowMapPlusUp>%.2f</ShadowMapPlusUp>\r\n", g_shadowmap_plusup), return 1);
	CallF(Write2File("    <ShadowMapDistScale>%.2f</ShadowMapDistScale>\r\n", g_shadowmap_plusright), return 1);
	CallF(Write2File("    <ShadowMapLightDir>%d</ShadowMapLightDir>\r\n", g_shadowmap_lightdir), return 1);


	CallF( Write2File( "</SHADOWPARAMS>\r\n" ), return 1 );

	return 0;
}

int CShadowParamsFile::WriteFileInfo()
{

	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShadowParamsFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//2023/08/18 To12024 : Add <TotalScale>%.3f</TotalScale> and <Scale>%.3f</Scale>
	CallF(Write2File("  <FileInfo>\r\n    <kind>ShadowParamsFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	return 0;
}


int CShadowParamsFile::LoadShadowParamsFile(const WCHAR* srcfilepath)
{
	if (!srcfilepath) {
		_ASSERT(0);
		return 1;
	}

	m_mode = XMLIO_LOAD;



	m_hfile = CreateFile( srcfilepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		//_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );

	//int posstep = 0;
	//XMLIOBUF projinfobuf;
	//ZeroMemory( &projinfobuf, sizeof( XMLIOBUF ) );
	//CallF( SetXmlIOBuf( &m_xmliobuf, "<ProjectInfo>", "</ProjectInfo>", &projinfobuf ), return 1 );
	//int charanum = 0;
	//CallF( ReadProjectInfo( &projinfobuf, &charanum ), return 1 );

	//m_xmliobuf.pos = 0;
	//m_motspeed = 1.0f;
	//Read_Float( &m_xmliobuf, "<MotSpeed>", "</MotSpeed>", &m_motspeed );


	float shadowmap_fov = 60.0f;
	float shadowmap_projscale = 1.0f;
	float shadowmap_near = 50.0f;
	float shadowmap_far = 2000.0f;
	float shadowmap_color = 0.5f;
	float shadowmap_bias = 0.0010f;
	float shadowmap_plusup = 300.0f;
	float shadowmap_plusright = 1.0f;
	int shadowmap_lightdir = 1;

	int result;
	m_xmliobuf.pos = 0;
	result = Read_Float(&m_xmliobuf, "<ShadowMapFov>", "</ShadowMapFov>", 
		&shadowmap_fov);
	if (result == 0) {
		g_shadowmap_fov = shadowmap_fov;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapProjScale>", "</ShadowMapProjScale>", 
		&shadowmap_projscale);
	if (result == 0) {
		g_shadowmap_projscale = shadowmap_projscale;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapNear>", "</ShadowMapNear>",
		&shadowmap_near);
	if (result == 0) {
		g_shadowmap_near = shadowmap_near;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapFar>", "</ShadowMapFar>",
		&shadowmap_far);
	if (result == 0) {
		g_shadowmap_far = shadowmap_far;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapColor>", "</ShadowMapColor>",
		&shadowmap_color);
	if (result == 0) {
		g_shadowmap_color = shadowmap_color;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapBias>", "</ShadowMapBias>",
		&shadowmap_bias);
	if (result == 0) {
		g_shadowmap_bias = shadowmap_bias;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapPlusUp>", "</ShadowMapPlusUp>",
		&shadowmap_plusup);
	if (result == 0) {
		g_shadowmap_plusup = shadowmap_plusup;
	}
	result = Read_Float(&m_xmliobuf, "<ShadowMapDistScale>", "</ShadowMapDistScale>",
		&shadowmap_plusright);
	if (result == 0) {
		g_shadowmap_plusright = shadowmap_plusright;
	}
	result = Read_Int(&m_xmliobuf, "<ShadowMapLightDir>", "</ShadowMapLightDir>",
		&shadowmap_lightdir);
	if (result == 0) {
		g_shadowmap_lightdir = shadowmap_lightdir;
	}

	return 0;
}

