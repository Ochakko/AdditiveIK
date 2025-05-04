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

#include <LightsForEditFile.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;



CLightsForEditFile::CLightsForEditFile()
{
	InitParams();
}

CLightsForEditFile::~CLightsForEditFile()
{
	DestroyObjs();
}

int CLightsForEditFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CLightsForEditFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CLightsForEditFile::WriteLightsForEditFile(const WCHAR* srcfilepath, int slotindex)
{
	if (!srcfilepath) {
		_ASSERT(0);
		return 1;
	}
	if ((slotindex < 0) || (slotindex >= LIGHTSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}


	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile(srcfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"LightsForEditFile : WriteLightsForEditFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<LIGHTSFOREDIT>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	CallF(Write2File("  <TotalScale>%.3f</TotalScale>\r\n", g_fLightScale), return 1);

	int lightcnt;
	for(lightcnt = 0; lightcnt < LIGHTNUMMAX; lightcnt++ ){
		CallF(WriteLight(slotindex, lightcnt), return 1 );
	}

	CallF( Write2File( "</LIGHTSFOREDIT>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}

int CLightsForEditFile::WriteFileInfo()
{

	//CallF(Write2File("  <FileInfo>\r\n    <kind>LightsForEditFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//2023/08/18 To12024 : Add <TotalScale>%.3f</TotalScale> and <Scale>%.3f</Scale>
	CallF(Write2File("  <FileInfo>\r\n    <kind>LightsForEditFile</kind>\r\n    <version>1002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);


	CallF(Write2File("  <LightsNum>%d</LightsNum>\r\n", LIGHTNUMMAX), return 1);

	return 0;
}

int CLightsForEditFile::WriteLight(int slotindex, int lightindex)
{
	if ((lightindex < 0) || (lightindex >= LIGHTNUMMAX)) {
		_ASSERT(0);
		return 1;
	}
	if ((slotindex < 0) || (slotindex >= LIGHTSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}

	CallF(Write2File("  <Light>\r\n"), return 1);

	CallF(Write2File("    <Enable>%d</Enable>\r\n", (int)g_lightEnable[slotindex][lightindex]), return 1);
	CallF(Write2File("    <DirWithView>%d</DirWithView>\r\n", (int)g_lightDirWithView[slotindex][lightindex]), return 1);

	CallF(Write2File("    <DirX>%.3f</DirX>\r\n", g_lightDir[slotindex][lightindex].x), return 1);
	CallF(Write2File("    <DirY>%.3f</DirY>\r\n", g_lightDir[slotindex][lightindex].y), return 1);
	CallF(Write2File("    <DirZ>%.3f</DirZ>\r\n", g_lightDir[slotindex][lightindex].z), return 1);


	int r255, g255, b255;
	r255 = (int)(g_lightDiffuse[slotindex][lightindex].x * 255.0f + 0.0001f);
	r255 = max(0, r255);
	r255 = min(255, r255);

	g255 = (int)(g_lightDiffuse[slotindex][lightindex].y * 255.0f + 0.0001f);
	g255 = max(0, g255);
	g255 = min(255, g255);

	b255 = (int)(g_lightDiffuse[slotindex][lightindex].z * 255.0f + 0.0001f);
	b255 = max(0, b255);
	b255 = min(255, b255);

	CallF(Write2File("    <R255>%d</R255>\r\n", r255), return 1);
	CallF(Write2File("    <G255>%d</G255>\r\n", g255), return 1);
	CallF(Write2File("    <B255>%d</B255>\r\n", b255), return 1);

	CallF(Write2File("    <Scale>%.3f</Scale>\r\n", g_lightScale[slotindex][lightindex]), return 1);

	CallF(Write2File("  </Light>\r\n"), return 1);

	return 0;
}

int CLightsForEditFile::LoadLightsForEditFile(const WCHAR* srcfilepath, int slotindex)
{
	if (!srcfilepath) {
		_ASSERT(0);
		return 1;
	}
	if ((slotindex < 0) || (slotindex >= LIGHTSLOTNUM)) {
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


	//ë∂ç›Ç∑ÇÈÇ∆Ç´ÇæÇØ
	float totalscale = 1.0f;
	int result0 = Read_Float( &m_xmliobuf, "<TotalScale>", "</TotalScale>", &totalscale);
	if (result0 == 0) {
		g_fLightScale = totalscale;
	}


	m_xmliobuf.pos = 0;
	int lightcnt;
	for (lightcnt = 0; lightcnt < LIGHTNUMMAX; lightcnt++) {
		XMLIOBUF lightbuf;
		ZeroMemory( &lightbuf, sizeof( XMLIOBUF ) );
		int result1 = SetXmlIOBuf(&m_xmliobuf, "<Light>", "</Light>", &lightbuf);
		if (result1 != 0) {
			_ASSERT(0);
			break;
		}
		CallF(ReadLight(slotindex, lightcnt, &lightbuf), return 1);
	}


	return 0;
}

/***
int CLightsForEditFile::CheckFileVersion( XMLIOBUF* xmlbuf )
{
	char kind[256];
	char version[256];
	char type[256];
	ZeroMemory( kind, sizeof( char ) * 256 );
	ZeroMemory( version, sizeof( char ) * 256 );
	ZeroMemory( type, sizeof( char ) * 256 );

	CallF( Read_Str( xmlbuf, "<kind>", "</kind>", kind, 256 ), return 1 );
	CallF( Read_Str( xmlbuf, "<version>", "</version>", version, 256 ), return 1 );
	CallF( Read_Str( xmlbuf, "<type>", "</type>", type, 256 ), return 1 );

	int cmpkind, cmpversion, cmptype;
	cmpkind = strcmp( kind, "OpenRDBProjectFile" );
	cmpversion = strcmp( version, "1001" );
	cmptype = strcmp( type, "0" );

	if( (cmpkind == 0) && (cmpversion == 0) && (cmptype == 0) ){
		return 0;
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/
//int CLightsForEditFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
//{
//	CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );
//
//	return 0;
//}
int CLightsForEditFile::ReadLight(int slotindex, int lightcnt, XMLIOBUF* xmlbuf)
{
	if ((lightcnt < 0) || (lightcnt >= LIGHTNUMMAX)) {
		_ASSERT(0);
		return 1;
	}
	if ((slotindex < 0) || (slotindex >= LIGHTSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}

	//CallF(Write2File("    <Enable>%d</Enable>\r\n", (int)g_lightenable[lightindex]), return 1);
	//CallF(Write2File("    <DirWithView>%d</DirWithView>\r\n", (int)g_lightdirwithview[lightindex]), return 1);

	int tmpenable = 0;
	int tmpdirwithview = 1;
	CallF(Read_Int(xmlbuf, "<Enable>", "</Enable>", &tmpenable), return 1);
	CallF(Read_Int(xmlbuf, "<DirWithView>", "</DirWithView>", &tmpdirwithview), return 1);

	float tmpdirx = 0.0f;
	float tmpdiry = 0.0f;
	float tmpdirz = -1.0f;

	CallF(Read_Float(xmlbuf, "<DirX>", "</DirX>", &tmpdirx), return 1);
	CallF(Read_Float(xmlbuf, "<DirY>", "</DirY>", &tmpdiry), return 1);
	CallF(Read_Float(xmlbuf, "<DirZ>", "</DirZ>", &tmpdirz), return 1);

	int tmpR255 = 0;
	int tmpG255 = 0;
	int tmpB255 = 0;

	CallF(Read_Int(xmlbuf, "<R255>", "</R255>", &tmpR255), return 1);
	CallF(Read_Int(xmlbuf, "<G255>", "</G255>", &tmpG255), return 1);
	CallF(Read_Int(xmlbuf, "<B255>", "</B255>", &tmpB255), return 1);

	float r01, g01, b01;
	r01 = (float)((double)tmpR255 / 255.0f);
	r01 = fmax(0.0f, r01);
	r01 = fmin(1.0f, r01);

	g01 = (float)((double)tmpG255 / 255.0f);
	g01 = fmax(0.0f, g01);
	g01 = fmin(1.0f, g01);

	b01 = (float)((double)tmpB255 / 255.0f);
	b01 = fmax(0.0f, b01);
	b01 = fmin(1.0f, b01);

	float tmpscale = 1.0f;
	int result2 = Read_Float(xmlbuf, "<Scale>", "</Scale>", &tmpscale);
	if (result2 == 0) {
		g_lightScale[slotindex][lightcnt] = tmpscale;
	}
	else {
		g_lightScale[slotindex][lightcnt] = 1.0f;
	}

	g_lightEnable[slotindex][lightcnt] = (tmpenable != 0);
	g_lightDirWithView[slotindex][lightcnt] = (tmpdirwithview != 0);


	ChaVector3 loadeddir, ndir;
	ndir.SetParams(0.0f, 0.0f, -1.0f);
	loadeddir.SetParams(tmpdirx, tmpdiry, tmpdirz);
	ChaVector3Normalize(&ndir, &loadeddir);
	g_lightDir[slotindex][lightcnt] = ndir;

	g_lightDiffuse[slotindex][lightcnt].SetParams(r01, g01, b01);

	return 0;
}
