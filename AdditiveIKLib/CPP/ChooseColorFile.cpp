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

#include <ChooseColorFile.h>


#define DBGH
#include <dbg.h>

using namespace std;



CChooseColorFile::CChooseColorFile()
{
	InitParams();
}

CChooseColorFile::~CChooseColorFile()
{
	DestroyObjs();
}

int CChooseColorFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CChooseColorFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CChooseColorFile::WriteChooseColorFile(const WCHAR* srcfilepath, const COLORREF srccolref[16])
{
	m_mode = XMLIO_WRITE;


	m_hfile = CreateFile(srcfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"ChooseColorFile : WriteChooseColorFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<CHOOSECOLOR>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	int colorcnt;
	for(colorcnt = 0; colorcnt < 16; colorcnt++ ){
		COLORREF curcolor = srccolref[colorcnt];
		CallF(WriteChooseColor(colorcnt, curcolor), return 1 );
	}

	CallF( Write2File( "</CHOOSECOLOR>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}

int CChooseColorFile::WriteFileInfo()
{

	//CallF( Write2File( "  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n" ), return 1 );
	//version 1002 : 2023/03/24 1.2.0.17 RC2
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1003 : 2023/03/24 1.2.0.17 RC3
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1003</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1004 : 2023/07/21 1.2.0.23‚ÖŒü‚¯‚Ä
	CallF(Write2File("  <FileInfo>\r\n    <kind>ChooseColorFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	CallF(Write2File("  <ChooseColorNum>%d</ChooseColorNum>\r\n", 16), return 1);

	return 0;
}

int CChooseColorFile::WriteChooseColor(int colorindex, COLORREF srccolor)
{
	if ((colorindex < 0) || (colorindex >= 16)) {
		_ASSERT(0);
		return 1;
	}


	CallF(Write2File("  <Color>\r\n"), return 1);
	CallF(Write2File("    <R255>%d</R255>\r\n", GetRValue(srccolor)), return 1);
	CallF(Write2File("    <G255>%d</G255>\r\n", GetGValue(srccolor)), return 1);
	CallF(Write2File("    <B255>%d</B255>\r\n", GetBValue(srccolor)), return 1);
	CallF(Write2File("  </Color>\r\n"), return 1);

	return 0;
}

int CChooseColorFile::LoadChooseColorFile(const WCHAR* srcfilepath, COLORREF* dstcolref)
{
	if (!srcfilepath || !dstcolref) {
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


	m_xmliobuf.pos = 0;
	int colorcnt;
	for (colorcnt = 0; colorcnt < 16; colorcnt++) {
		XMLIOBUF colorbuf;
		ZeroMemory( &colorbuf, sizeof( XMLIOBUF ) );
		CallF( SetXmlIOBuf( &m_xmliobuf, "<Color>", "</Color>", &colorbuf ), return 1 );
		CallF( ReadChooseColor(colorcnt, &colorbuf, (dstcolref + colorcnt)), return 1 );
	}


	return 0;
}

/***
int CChooseColorFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
//int CChooseColorFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
//{
//	CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );
//
//	return 0;
//}
int CChooseColorFile::ReadChooseColor(int colorcnt, XMLIOBUF* xmlbuf, COLORREF* dstcolref)
{
	if (!dstcolref) {
		_ASSERT(0);
		return 1;
	}
	if ((colorcnt < 0) || (colorcnt >= 16)) {
		_ASSERT(0);
		return 1;
	}


	int tmpR255 = 0;
	int tmpG255 = 0;
	int tmpB255 = 0;

	CallF(Read_Int(xmlbuf, "<R255>", "</R255>", &tmpR255), return 1);
	CallF(Read_Int(xmlbuf, "<G255>", "</G255>", &tmpG255), return 1);
	CallF(Read_Int(xmlbuf, "<B255>", "</B255>", &tmpB255), return 1);

	COLORREF colorloaded = RGB(tmpR255, tmpG255, tmpB255);
	*dstcolref = colorloaded;

	return 0;
}
