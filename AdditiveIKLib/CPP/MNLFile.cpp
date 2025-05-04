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

#include <MNLFile.h>

#include <Model.h>

#define DBGH
#include <dbg.h>

using namespace std;

CMNLFile::CMNLFile()
{
	InitParams();
}

CMNLFile::~CMNLFile()
{
	DestroyObjs();
}

int CMNLFile::InitParams()
{
	CXMLIO::InitParams();
	m_model = 0;

	return 0;
}

int CMNLFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CMNLFile::WriteMNLFile(WCHAR* strpath, CModel* srcmodel)
{
	m_mode = XMLIO_WRITE;

	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	m_model = srcmodel;


	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"MNLFile : WriteMNLFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<MNL>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	//CallF( Write2File( "  <MotSpeed>%f</MotSpeed>\r\n", srcmotspeed ), return 1 );

	int minum;
	int miindex;
	minum = m_model->GetMotInfoSize();
	for (miindex = 0; miindex < minum; miindex++) {
		MOTINFO curmi = m_model->GetMotInfoByIndex(miindex);
		CallF(WriteMNEntry(miindex, curmi), return 1);
	}

	CallF( Write2File( "</MNL>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}

int CMNLFile::WriteFileInfo()
{

	CallF( Write2File( "  <FileInfo>\r\n    <kind>MotionNameList</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n" ), return 1 );

	CallF( Write2File( "  <MNLInfo>\r\n" ), return 1 );
	CallF(Write2File("    <MotionNum>%d</MotionNum>\r\n", (int)m_model->GetMotInfoSize()), return 1);
	CallF( Write2File( "  </MNLInfo>\r\n" ), return 1 );

	return 0;
}

int CMNLFile::WriteMNEntry(int srccnt, MOTINFO curmi)
{
	CallF( Write2File( "  <MNEntry>\r\n" ), return 1 );
	CallF( Write2File( "    <Index>%d</Index>\r\n", srccnt ), return 1 );
	CallF( Write2File( "    <Name>%s</Name>\r\n", curmi.motname ), return 1 );
	CallF( Write2File( "  </MNEntry>\r\n" ), return 1 );

	return 0;
}

int CMNLFile::LoadMNLFile(WCHAR* strpath, CModel* srcmodel)
{
	m_mode = XMLIO_LOAD;

	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	m_model = srcmodel;


	m_hfile = CreateFile( strpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );

	int posstep = 0;
	XMLIOBUF projinfobuf;
	ZeroMemory( &projinfobuf, sizeof( XMLIOBUF ) );
	CallF( SetXmlIOBuf( &m_xmliobuf, "<MNLInfo>", "</MNLInfo>", &projinfobuf ), return 1 );
	int motionnum = 0;
	CallF( ReadFileInfo( &projinfobuf, &motionnum ), return 1 );


	m_xmliobuf.pos = 0;
	int motioncnt;
	for(motioncnt = 0; motioncnt < motionnum; motioncnt++ ){
		XMLIOBUF motionbuf;
		ZeroMemory( &motionbuf, sizeof( XMLIOBUF ) );
		CallF( SetXmlIOBuf( &m_xmliobuf, "<MNEntry>", "</MNEntry>", &motionbuf ), return 1 );
		CallF( ReadMNEntry(motionnum, motioncnt, &motionbuf ), return 1 );
	}

	return 0;
}

/***
int CMNLFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
int CMNLFile::ReadFileInfo( XMLIOBUF* xmlbuf, int* motionnumptr )
{
	CallF( Read_Int( xmlbuf, "<MotionNum>", "</MotionNum>", motionnumptr ), return 1 );

	return 0;
}
int CMNLFile::ReadMNEntry( int motionnum, int motioncnt, XMLIOBUF* xmlbuf )
{
	int index = 0;
	char mname[256] = {0};


	CallF(Read_Int(xmlbuf, "<Index>", "</Index>", &index), return 1);
	CallF(Read_Str( xmlbuf, "<Name>", "</Name>", mname, 256 ), return 1);


	if (!m_model) {
		_ASSERT(0);
		return 1;
	}

	int minum = m_model->GetMotInfoSize();
	if ((index < 0) || (index >= minum)) {
		_ASSERT(0);
		return 0;
	}

	char changename[256] = { 0 };
	strcpy_s(changename, 256, mname);
	changename[256 - 1] = 0;
	size_t namelen = strlen(changename);
	if (namelen <= 0) {
		_ASSERT(0);
		return 0;
	}

	//check english name
	bool isengname = true;
	size_t ccnt;
	for (ccnt = 0; ccnt < namelen; ccnt++) {
		char curc = changename[ccnt];
		if (((curc >= '0') && (curc <= '9')) ||
			((curc >= 'a') && (curc <= 'z')) ||
			((curc >= 'A') && (curc <= 'Z')) ||
			((curc == '-') || (curc == '_') || (curc == '.'))) {

			//ok 
		}
		else {
			isengname = false;
			break;
		}
	}
	if (isengname == false) {
		_ASSERT(0);
		return 0;
	}


	m_model->SetMotInfoMotNameByIndex(index, changename);
	m_model->SetMotInfoEngMotNameByIndex(index, changename);

	return 0;
}
