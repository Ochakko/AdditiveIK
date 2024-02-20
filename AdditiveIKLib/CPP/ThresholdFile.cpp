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

#include <ThresholdFile.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;



CThresholdFile::CThresholdFile()
{
	InitParams();
}

CThresholdFile::~CThresholdFile()
{
	DestroyObjs();
}

int CThresholdFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CThresholdFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CThresholdFile::WriteThresholdFile(const WCHAR* srcfilepath)
{
	if (!srcfilepath) {
		_ASSERT(0);
		return 1;
	}


	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile(srcfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"ThresholdFile : WriteThresholdFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<THRESHOLD>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	CallF(WriteThreshold(), return 1 );

	CallF( Write2File( "</THRESHOLD>\r\n" ), return 1 );

	return 0;
}

int CThresholdFile::WriteFileInfo()
{

	CallF(Write2File("  <FileInfo>\r\n    <kind>ThresholdFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	return 0;
}

int CThresholdFile::WriteThreshold()
{

	CallF(Write2File("  <ThDeg>%d</ThDeg>\r\n", Float2Int(g_thdeg)), return 1);
	CallF(Write2File("  <ThDeg_EndJoint>%d</ThDeg_EndJoint>\r\n", Float2Int(g_thdeg_endjoint)), return 1);

	CallF(Write2File("  <XRound>%d</XRound>\r\n", Float2Int(g_thRoundX)), return 1);
	CallF(Write2File("  <YRound>%d</YRound>\r\n", Float2Int(g_thRoundY)), return 1);
	CallF(Write2File("  <ZRound>%d</ZRound>\r\n", Float2Int(g_thRoundZ)), return 1);

	return 0;
}

int CThresholdFile::LoadThresholdFile(const WCHAR* srcfilepath)
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


	int result = ReadThreshold(&m_xmliobuf);


	return result;
}

/***
int CThresholdFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
//int CThresholdFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
//{
//	CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );
//
//	return 0;
//}
int CThresholdFile::ReadThreshold(XMLIOBUF* xmlbuf)
{
	int tmp_thdeg = Float2Int(g_thdeg);
	int tmp_thdeg_endjoint = Float2Int(g_thdeg_endjoint);
	int tmp_xround = Float2Int(g_thRoundX);
	int tmp_yround = Float2Int(g_thRoundY);
	int tmp_zround = Float2Int(g_thRoundZ);

	CallF(Read_Int(xmlbuf, "<ThDeg>", "</ThDeg>", &tmp_thdeg), return 1);
	CallF(Read_Int(xmlbuf, "<ThDeg_EndJoint>", "</ThDeg_EndJoint>", &tmp_thdeg_endjoint), return 1);

	CallF(Read_Int(xmlbuf, "<XRound>", "</XRound>", &tmp_xround), return 1);
	CallF(Read_Int(xmlbuf, "<YRound>", "</YRound>", &tmp_yround), return 1);
	CallF(Read_Int(xmlbuf, "<ZRound>", "</ZRound>", &tmp_zround), return 1);

	g_thdeg = (float)tmp_thdeg;
	g_thdeg_endjoint = (float)tmp_thdeg_endjoint;

	g_thRoundX = (float)tmp_xround;
	g_thRoundY = (float)tmp_yround;
	g_thRoundZ = (float)tmp_zround;

	return 0;
}
