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

#include <DispGroupFile.h>

#include <Model.h>
#include <mqoobject.h>

//#include <BoneProp.h>

#define DBGH
#include <dbg.h>
#include <map>
#include <vector>

using namespace std;


CDispGroupFile::CDispGroupFile()
{
	InitParams();
}

CDispGroupFile::~CDispGroupFile()
{
	DestroyObjs();
}

int CDispGroupFile::InitParams()
{
	CXMLIO::InitParams();

	m_model = 0;
	return 0;
}

int CDispGroupFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}


int CDispGroupFile::WriteDispGroupFile( WCHAR* strpath, CModel* srcmodel )
{
	if (!srcmodel){
		_ASSERT(0);
		return 1;
	}

	m_model = srcmodel;
	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"DispGroupFile : WriteDispGroupFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CallF(Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<DispGroupFile>\r\n" ), return 1);  
	CallF(Write2File("  <FileInfo>1001-01</FileInfo>\r\n"), return 1);

	int groupindex;
	for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
		WriteDig(groupindex);
	}


	CallF( Write2File( "</DispGroupFile>\r\n" ), return 1 );

	return 0;
}

int CDispGroupFile::WriteDig(int groupindex)
{
	if (!m_model) {
		_ASSERT(0);
		return 1;
	}

	CallF(Write2File("  <DispGroup>\r\n" ), return 1);
	CallF(Write2File("    <GroupNo>%d</GroupNo>\r\n", groupindex + 1), return 1);


	bool groupON = m_model->GetDispGroupON(groupindex);
	if (groupON) {
		CallF(Write2File("    <GroupON>1</GroupON>\r\n"), return 1);
	}
	else {
		CallF(Write2File("    <GroupON>0</GroupON>\r\n"), return 1);
	}
	

	vector<DISPGROUPELEM> digelemvec;
	m_model->GetDispGroupForRender(groupindex, digelemvec);

	int elemnum = (int)digelemvec.size();
	int elemno;
	for (elemno = 0; elemno < elemnum; elemno++) {
		DISPGROUPELEM digelem = digelemvec[elemno];
		if (digelem.pNode) {
			char nodename[256] = { 0 };
			strcpy_s(nodename, 256, digelem.pNode->GetName());
			CallF(Write2File("    <NodeName>%s</NodeName>\r\n", nodename), return 1);
		}
	}

	CallF(Write2File("  </DispGroup>\r\n"), return 1);

	return 0;
}




int CDispGroupFile::LoadDispGroupFile( WCHAR* strpath, CModel* srcmodel )
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	m_model = srcmodel;
	m_mode = XMLIO_LOAD;

	m_hfile = CreateFile( strpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		//_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );


	int result = 0;
	while(result == 0){
		XMLIOBUF digbuf;
		ZeroMemory( &digbuf, sizeof( XMLIOBUF ) );
		int ret = SetXmlIOBuf( &m_xmliobuf, "<DispGroup>", "</DispGroup>", &digbuf );
		if( ret == 0 ){
			result = ReadDig(&digbuf);
		}else{
			result = 1;
		}
	}
	m_model->MakeDispGroupForRender();//ファイル読み込み途中にエラーが起きてもMakeDispGroupForRenderは実行


	return 0;
}

int CDispGroupFile::ReadDig(XMLIOBUF* xmliobuf)
{
	if (!m_model) {
		_ASSERT(0);
		return 1;
	}


	int groupno;
	CallF(Read_Int(xmliobuf, "<GroupNo>", "</GroupNo>", &groupno), return 1);
	if ((groupno < 1) || (groupno > MAXDISPGROUPNUM)) {
		_ASSERT(0);
		return 1;
	}
	int groupindex = groupno - 1;

	int groupON;
	CallF(Read_Int(xmliobuf, "<GroupON>", "</GroupON>", &groupON), return 1);
	bool dispgroupON;
	if (groupON == 1) {
		dispgroupON = true;
	}
	else {
		dispgroupON = false;
	}
	m_model->SetDispGroupON(groupindex, dispgroupON);

	
	int result = 0;
	while (result == 0) {
		XMLIOBUF namebuf;
		ZeroMemory(&namebuf, sizeof(XMLIOBUF));
		int delpatflag = 0;
		result = SetXmlIOBuf(xmliobuf, "<NodeName>", "</NodeName>", &namebuf, delpatflag);

		char nodename[256];
		ZeroMemory(nodename, sizeof(char) * 256);
		if (result == 0) {
			//strncpy_s(nodename, 256, namebuf.buf + namebuf.pos, namebuf.bufleng - namebuf.pos);
			CallF(Read_Str(&namebuf, "<NodeName>", "</NodeName>", nodename, 256), return 1);

			m_model->SetDispGroup(groupindex, nodename);
		}
	}


	return 0;
}



/***
int CDispGroupFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
