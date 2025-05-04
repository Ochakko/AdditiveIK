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

#include <DofParamsFile.h>

#include <Model.h>
#include <MQOObject.h>
#include <MQOMaterial.h>


#define DBGH
#include <dbg.h>

#include <GlobalVar.h>


using namespace std;


CDofParamsFile::CDofParamsFile()
{
	InitParams();
}

CDofParamsFile::~CDofParamsFile()
{
	DestroyObjs();
}

int CDofParamsFile::InitParams()
{
	CXMLIO::InitParams();
	return 0;
}

int CDofParamsFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();
	InitParams();
	return 0;
}

int CDofParamsFile::WriteDofParamsFile(WCHAR* filename, int srcindex)
{
	if (!filename) {
		_ASSERT(0);
		return 1;
	}
	if ((srcindex < 0) || (srcindex >= DOFSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}

	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"DofParamsFile : WriteDofParamsFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<DofParamsFile>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	//CallF( Write2File( "  <MotSpeed>%f</MotSpeed>\r\n", srcmotspeed ), return 1 );
	//char mprojname[256];
	//ZeroMemory( mprojname, sizeof( char ) * 256 );
	//WideCharToMultiByte( CP_ACP, 0, projname, -1, mprojname, 256, NULL, NULL );

	CallF(Write2File("  <DofNear>%.1f</DofNear>\r\n", g_dofparams[srcindex].x), return 1);
	CallF(Write2File("  <DofFar>%.1f</DofFar>\r\n", g_dofparams[srcindex].y), return 1);

	int skydofflag = (g_skydofflag[srcindex] == true) ? 1 : 0;
	CallF(Write2File("  <SkyDof>%d</SkyDof>\r\n", skydofflag), return 1);


	CallF( Write2File( "</DofParamsFile>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}

int CDofParamsFile::WriteFileInfo()
{
	//2024/03/07
	CallF(Write2File("  <FileInfo>\r\n    <kind>DofParamsFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);


	return 0;
}


int CDofParamsFile::LoadDofParamsFile(WCHAR* filename, int srcindex)
{
	if (!filename) {
		_ASSERT(0);
		return 1;
	}
	if ((srcindex < 0) || (srcindex >= DOFSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}


	m_hfile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		//_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );

	int posstep = 0;
	//XMLIOBUF projinfobuf;
	//ZeroMemory( &projinfobuf, sizeof( XMLIOBUF ) );
	//CallF( SetXmlIOBuf( &m_xmliobuf, "<ProjectInfo>", "</ProjectInfo>", &projinfobuf ), return 1 );
	//int charanum = 0;
	//CallF( ReadProjectInfo( &projinfobuf, &charanum ), return 1 );

	//m_xmliobuf.pos = 0;
	//m_motspeed = 1.0f;
	//Read_Float( &m_xmliobuf, "<MotSpeed>", "</MotSpeed>", &m_motspeed );




	float dofnear = g_dofparams[srcindex].x;
	Read_Float(&m_xmliobuf, "<DofNear>", "</DofNear>", &dofnear);
	g_dofparams[srcindex].x = dofnear;

	float doffar = g_dofparams[srcindex].y;
	Read_Float(&m_xmliobuf, "<DofFar>", "</DofFar>", &doffar);
	g_dofparams[srcindex].y = doffar;

	int skydofflag = (g_skydofflag[srcindex] == true) ? 1 : 0;
	Read_Int(&m_xmliobuf, "<SkyDof>", "</SkyDof>", &skydofflag);
	g_skydofflag[srcindex] = (skydofflag == 1) ? true : false;


	return 0;
}

/***
int CDofParamsFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
int CDofParamsFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
{
	//CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );

	return 0;
}
