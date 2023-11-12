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

#include <BopFile.h>

#include <Model.h>
#include <mqoobject.h>
#include <Bone.h>

#define DBGH
#include <dbg.h>

#include <string>

using namespace std;

CBopFile::CBopFile()
{
	InitParams();
}

CBopFile::~CBopFile()
{
	DestroyObjs();
}

int CBopFile::InitParams()
{
	CXMLIO::InitParams();

	m_topbone = 0;
	m_bonenum = 0;
	m_motid = -1;
	m_motinfo = 0;

	m_fileversion = 0;
	m_basenum = 0;
	return 0;
}

int CBopFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CBopFile::LoadBopFile( WCHAR* strpath, CModel* srcmodel )
{
	m_model = srcmodel;
	m_motid = -1;
	m_bonenum = 0;
	m_mode = XMLIO_LOAD;

	m_hfile = CreateFile( strpath, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );

	int posstep = 0;
	XMLIOBUF filebuf;
	ZeroMemory( &filebuf, sizeof( XMLIOBUF ) );
	CallF( SetXmlIOBuf( &m_xmliobuf, "<BOP>", "</BOP>", &filebuf ), return 1 );

	char* elempat = 0;
	elempat = strstr( filebuf.buf + filebuf.pos, "<BopEntry>" );
	while( elempat ){
		XMLIOBUF elembuf;
		ZeroMemory( &elembuf, sizeof( XMLIOBUF ) );
		CallF( SetXmlIOBuf( &filebuf, "<BopEntry>", "</BopEntry>", &elembuf ), return 1 );
		CallF( ReadBopElem( &elembuf ), return 1 );

		elempat = strstr( filebuf.buf + filebuf.pos, "<BopEntry>" );
	}

	return 0;
}


int CBopFile::ReadBopElem( XMLIOBUF* xmlbuf )
{
//    <Name>bonename1</Name>
//    <MPNum>2</MPNum>

	char bonename[256] = {0};
	ChaVector3 wpos;
	ChaVector3 fpos;

	CallF( Read_Str( xmlbuf, "<JointName>", "</JointName>", bonename, 256 ), return 1 );
	CallF( Read_Vec3( xmlbuf, "<World>", "</World>", &wpos ), return 1 );
	CallF( Read_Vec3( xmlbuf, "<Form>", "</Form>", &fpos ), return 1 );

	CBone* curbone = m_model->GetBoneByName( bonename );

	if( curbone ){
		curbone->SetJointWPos( wpos );
		curbone->SetJointFPos( fpos );
	}else{
		_ASSERT( 0 );
	}

	return 0;
}
