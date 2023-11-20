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

#include <GColiFile.h>

#include "..\\BTMANAGER\\BPWorld.h"
#include <model.h>

#define DBGH
#include <dbg.h>
#include <vector>

#include <shlwapi.h>

using namespace std;

CGColiFile::CGColiFile()
{
	InitParams();
}

CGColiFile::~CGColiFile()
{
	DestroyObjs();
}

int CGColiFile::InitParams()
{
	CXMLIO::InitParams();

	m_bpw = 0;
	m_model = 0;
	return 0;
}

int CGColiFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}


int CGColiFile::WriteGColiFile( WCHAR* strpath, BPWorld* srcbpw )
{
	m_bpw = srcbpw;
	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"GColiFile : WriteGColiFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<GCOLI>\r\n" ), return 1 );  
	CallF( Write2File( "    <FileInfo>1001-01</FileInfo>\r\n" ), return 1 );

	WriteColiID();

	CallF( Write2File( "</GCOLI>\r\n" ), return 1 );

	return 0;
}

int CGColiFile::WriteColiID()
{

	CallF( Write2File( "    <GROUP>%d</GROUP>\r\n", m_bpw->m_groupid ), return 1);
	CallF( Write2File( "    <MYSELF>%d</MYSELF>\r\n", m_bpw->m_myselfflag ), return 1);

	int idnum = (int)m_bpw->m_coliids.size();
	CallF( Write2File( "    <COLIIDNUM>%d</COLIIDNUM>\r\n", idnum ), return 1);
	int ino;
	for( ino = 0; ino < idnum; ino++ ){
		CallF( Write2File( "    <COLIID>%d</COLIID>\r\n", m_bpw->m_coliids[ino] ), return 1);
	}

	CallF( Write2File( "    <GH>%f</GH>\r\n", m_bpw->m_gplaneh ), return 1);
	CallF( Write2File( "    <GDISPH>%f</GDISPH>\r\n", m_bpw->m_gdisph ), return 1);
	CallF( Write2File( "    <SIZEX>%f</SIZEX>\r\n", m_bpw->m_gplanesize.x ), return 1);
	CallF( Write2File( "    <SIZEZ>%f</SIZEZ>\r\n", m_bpw->m_gplanesize.y ), return 1);
	CallF( Write2File( "    <DISP>%d</DISP>\r\n", m_bpw->m_gplanedisp ), return 1);
	CallF( Write2File( "    <RESTITUTION>%f</RESTITUTION>\r\n", m_bpw->m_restitution ), return 1);
	CallF( Write2File( "    <FRICTION>%f</FRICTION>\r\n", m_bpw->m_friction ), return 1);

	return 0;
}


int CGColiFile::LoadGColiFile( CModel* srcmodel, WCHAR* strpath, BPWorld* srcbpw )
{
	m_bpw = srcbpw;
	m_model = srcmodel;
	m_mode = XMLIO_LOAD;

	WCHAR wfilename[MAX_PATH] = {0L};
	WCHAR* lasten;
	lasten = wcsrchr( strpath, TEXT('\\') );
	if( !lasten ){
		_ASSERT( 0 );
		return 1;
	}
	wcscpy_s( wfilename, MAX_PATH, lasten + 1 );
	char mfilename[MAX_PATH];
	ZeroMemory( mfilename, sizeof( char ) * MAX_PATH );
	WideCharToMultiByte( CP_ACP, 0, wfilename, -1, mfilename, MAX_PATH, NULL, NULL );

	BOOL bexist;
	bexist = PathFileExists(strpath);
	if (!bexist)
	{
		_ASSERT(0);
		return 2;//not found
	}

	m_hfile = CreateFile( strpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );
	CallF( ReadColiID( &m_xmliobuf ), return 1 );

	return 0;
}

int CGColiFile::ReadColiID( XMLIOBUF* xmlbuf )
{
	int gid = 0;
	int retgid = Read_Int( xmlbuf, "<GROUP>", "</GROUP>", &gid );
	if( retgid ){
		gid = 1;
	}
	int myself = 0;
	int retmyself = Read_Int( xmlbuf, "<MYSELF>", "</MYSELF>", &myself );
	if( retmyself ){
		myself = 1;
	}
	int idnum = 0;
	int retidnum = Read_Int( xmlbuf, "<COLIIDNUM>", "</COLIIDNUM>", &idnum );
	if( retidnum ){
		idnum = 0;
	}

	vector<int> tmpids;
	tmpids.clear();

	int ino;
	for( ino = 0; ino < idnum; ino++ ){
		XMLIOBUF idbuf;
		ZeroMemory( &idbuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( xmlbuf, "<COLIID>", "</COLIID>", &idbuf, 0 );
		if( ret == 0 ){
			int id = 0;
			int retid = Read_Int( &idbuf, "<COLIID>", "</COLIID>", &id );
			if( retid ){
				_ASSERT( 0 );
				return 1;
			}
			tmpids.push_back( id );

		}else{
			_ASSERT( 0 );
			break;
		}
	}

	float gh = -1.0f;
	Read_Float( xmlbuf, "<GH>", "</GH>", &gh );

	float gdisph = -1.0f;
	Read_Float( xmlbuf, "<GDISPH>", "</GDISPH>", &gdisph );

	float sizex = 5.0f;
	Read_Float( xmlbuf, "<SIZEX>", "</SIZEX>", &sizex );

	float sizez = 5.0f;
	Read_Float( xmlbuf, "<SIZEZ>", "</SIZEZ>", &sizez );

	int dflag = 1;
	Read_Int( xmlbuf, "<DISP>", "</DISP>", &dflag );

	float rest = 0.5f;
	Read_Float( xmlbuf, "<RESTITUTION>", "</RESTITUTION>", &rest );

	float fric = 0.5f;
	Read_Float( xmlbuf, "<FRICTION>", "</FRICTION>", &fric );

	_ASSERT( m_bpw );
	if (m_bpw) {
		m_bpw->m_groupid = gid;
		m_bpw->m_myselfflag = myself;
		m_bpw->m_coliids = tmpids;

		m_bpw->m_gplaneh = gh;
		m_bpw->m_gdisph = gdisph;
		m_bpw->m_gplanesize = ChaVector2(sizex, sizez);
		m_bpw->m_gplanedisp = dflag;
		m_bpw->m_restitution = rest;
		m_bpw->m_friction = fric;

		m_bpw->RemakeG();

		ChaVector3 tra(0.0f, 0.0f, 0.0f);
		ChaVector3 mult(m_bpw->m_gplanesize.x, 1.0f, m_bpw->m_gplanesize.y);
		if (m_model) {
			CallF(m_model->MultDispObj(mult, tra), return 1);
		}
	}

	return 0;
}


/***
int CGColiFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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

	int cmpkind, cmpversion1, cmpversion2, cmptype;
	cmpkind = strcmp( kind, "OpenRDBMotionFile" );
	cmpversion1 = strcmp( version, "1001" );
	cmpversion2 = strcmp( version, "1002" );
	cmptype = strcmp( type, "0" );

	if( (cmpkind == 0) && (cmptype == 0) ){
		if( cmpversion1 == 0 ){
			return 1;
		}else if( cmpversion2 == 0 ){
			return 2;
		}else{
			return 0;
		}

		return 0;
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}
***/
