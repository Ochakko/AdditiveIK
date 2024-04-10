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
	//float g_shadowmap_distscale = 1.0f;
	//int g_shadowmap_lightdir = 1;
	if (g_enableshadow) {
		CallF(Write2File("    <ShadowEnable>1</ShadowEnable>\r\n"), return 1);
	}
	else {
		CallF(Write2File("    <ShadowEnable>0</ShadowEnable>\r\n"), return 1);
	}
	if (g_VSMflag) {
		CallF(Write2File("    <VarianceShadowMaps>1</VarianceShadowMaps>\r\n"), return 1);
	}
	else {
		CallF(Write2File("    <VarianceShadowMaps>0</VarianceShadowMaps>\r\n"), return 1);
	}

	char strshadowtag[9][256] = {
		"ShadowMapFov",
		"ShadowMapProjScale",
		"ShadowMapNear",
		"ShadowMapFar",
		"ShadowMapColor",
		"ShadowMapBias",
		"ShadowMapPlusUp",
		"ShadowMapDistScale",
		"ShadowMapLightDir"
	};

	int slotno, tagno;
	for (slotno = 0; slotno < SHADOWSLOTNUM; slotno++) {
		for (tagno = 0; tagno < 9; tagno++) {
			char strtag[256] = { 0 };
			if (slotno == 0) {
				sprintf_s(strtag, 256, "%s", strshadowtag[tagno]);
			}
			else {
				sprintf_s(strtag, 256, "%s%d", strshadowtag[tagno], slotno);
			}
			char strbegintag[256] = { 0 };
			char strendtag[256] = { 0 };
			sprintf_s(strbegintag, 256, "<%s>", strtag);
			sprintf_s(strendtag, 256, "</%s>", strtag);

			switch (tagno) {
			case 0:
				CallF(Write2File("    %s%.1f%s\r\n", strbegintag, g_shadowmap_fov[slotno], strendtag), return 1);
				break;
			case 1:
				CallF(Write2File("    %s%.1f%s\r\n", strbegintag, g_shadowmap_projscale[slotno], strendtag), return 1);
				break;
			case 2:
				CallF(Write2File("    %s%.1f%s\r\n", strbegintag, g_shadowmap_near[slotno], strendtag), return 1);
				break;
			case 3:
				CallF(Write2File("    %s%.1f%s\r\n", strbegintag, g_shadowmap_far[slotno], strendtag), return 1);
				break;
			case 4:
				CallF(Write2File("    %s%.2f%s\r\n", strbegintag, g_shadowmap_color[slotno], strendtag), return 1);
				break;
			case 5:
				CallF(Write2File("    %s%.3f%s\r\n", strbegintag, g_shadowmap_bias[slotno], strendtag), return 1);
				break;
			case 6:
				CallF(Write2File("    %s%.2f%s\r\n", strbegintag, g_shadowmap_plusup[slotno], strendtag), return 1);
				break;
			case 7:
				CallF(Write2File("    %s%.2f%s\r\n", strbegintag, g_shadowmap_distscale[slotno], strendtag), return 1);
				break;
			case 8:
				CallF(Write2File("    %s%d%s\r\n", strbegintag, g_shadowmap_lightdir[slotno], strendtag), return 1);
				break;
			default:
				_ASSERT(0);
				break;
			}
		}
	}

	//CallF(Write2File("    <ShadowMapFov>%.1f</ShadowMapFov>\r\n", g_shadowmap_fov), return 1);
	//CallF(Write2File("    <ShadowMapProjScale>%.1f</ShadowMapProjScale>\r\n", g_shadowmap_projscale), return 1);
	//CallF(Write2File("    <ShadowMapNear>%.1f</ShadowMapNear>\r\n", g_shadowmap_near), return 1);
	//CallF(Write2File("    <ShadowMapFar>%.1f</ShadowMapFar>\r\n", g_shadowmap_far), return 1);
	//CallF(Write2File("    <ShadowMapColor>%.2f</ShadowMapColor>\r\n", g_shadowmap_color), return 1);
	//CallF(Write2File("    <ShadowMapBias>%.3f</ShadowMapBias>\r\n", g_shadowmap_bias), return 1);
	//CallF(Write2File("    <ShadowMapPlusUp>%.2f</ShadowMapPlusUp>\r\n", g_shadowmap_plusup), return 1);
	//CallF(Write2File("    <ShadowMapDistScale>%.2f</ShadowMapDistScale>\r\n", g_shadowmap_distscale), return 1);
	//CallF(Write2File("    <ShadowMapLightDir>%d</ShadowMapLightDir>\r\n", g_shadowmap_lightdir), return 1);


	CallF( Write2File( "</SHADOWPARAMS>\r\n" ), return 1 );

	return 0;
}

int CShadowParamsFile::WriteFileInfo()
{

	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShadowParamsFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//2023/08/18 To12024 : Add <TotalScale>%.3f</TotalScale> and <Scale>%.3f</Scale>
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShadowParamsFile</kind>\r\n    <version>1002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	//2024/04/10 Add Tag of SoftShadow
	CallF(Write2File("  <FileInfo>\r\n    <kind>ShadowParamsFile</kind>\r\n    <version>1003</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

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
	int shadowenable = 1;
	int softshadow = 0;

	int result;
	m_xmliobuf.pos = 0;

	result = Read_Int(&m_xmliobuf, "<ShadowEnable>", "</ShadowEnable>",
		&shadowenable);
	if (result == 0) {
		if (shadowenable == 1) {
			g_enableshadow = true;
		}
		else {
			g_enableshadow = false;
		}
	}

	result = Read_Int(&m_xmliobuf, "<VarianceShadowMaps>", "</VarianceShadowMaps>",
		&softshadow);
	if (result == 0) {
		if (softshadow == 1) {
			g_VSMflag = true;
		}
		else {
			g_VSMflag = false;
		}
	}

	char strshadowtag[9][256] = {
		"ShadowMapFov",
		"ShadowMapProjScale",
		"ShadowMapNear",
		"ShadowMapFar",
		"ShadowMapColor",
		"ShadowMapBias",
		"ShadowMapPlusUp",
		"ShadowMapDistScale",
		"ShadowMapLightDir"
	};

	int slotno, tagno;
	for (slotno = 0; slotno < SHADOWSLOTNUM; slotno++) {
		for (tagno = 0; tagno < 9; tagno++) {
			char strtag[256] = { 0 };
			if (slotno == 0) {
				sprintf_s(strtag, 256, "%s", strshadowtag[tagno]);
			}
			else {
				sprintf_s(strtag, 256, "%s%d", strshadowtag[tagno], slotno);
			}
			char strbegintag[256] = { 0 };
			char strendtag[256] = { 0 };
			sprintf_s(strbegintag, 256, "<%s>", strtag);
			sprintf_s(strendtag, 256, "</%s>", strtag);

			switch (tagno) {
			case 0:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_fov);
				if (result == 0) {
					g_shadowmap_fov[slotno] = shadowmap_fov;
				}
			}
				break;
			case 1:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_projscale);
				if (result == 0) {
					g_shadowmap_projscale[slotno] = shadowmap_projscale;
				}
			}
				break;
			case 2:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_near);
				if (result == 0) {
					g_shadowmap_near[slotno] = shadowmap_near;
				}
			}
				break;
			case 3:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_far);
				if (result == 0) {
					g_shadowmap_far[slotno] = shadowmap_far;
				}
			}
				break;
			case 4:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_color);
				if (result == 0) {
					g_shadowmap_color[slotno] = shadowmap_color;
				}
			}
				break;
			case 5:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_bias);
				if (result == 0) {
					g_shadowmap_bias[slotno] = shadowmap_bias;
				}
			}
				break;
			case 6:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_plusup);
				if (result == 0) {
					g_shadowmap_plusup[slotno] = shadowmap_plusup;
				}
			}
				break;
			case 7:
			{
				result = Read_Float(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_plusright);
				if (result == 0) {
					g_shadowmap_distscale[slotno] = shadowmap_plusright;
				}
			}
				break;
			case 8:
			{
				result = Read_Int(&m_xmliobuf, strbegintag, strendtag,
					&shadowmap_lightdir);
				if (result == 0) {
					g_shadowmap_lightdir[slotno] = shadowmap_lightdir;
				}
			}
				break;
			default:
				_ASSERT(0);
				break;
			}
		}
	}

	return 0;
}

