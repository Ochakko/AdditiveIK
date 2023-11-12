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

#include <RetargetFile.h>

#include <Model.h>
#include <Bone.h>

#define DBGH
#include <dbg.h>

using namespace std;


extern float g_tmpmqomult;
extern WCHAR g_tmpmqopath[MULTIPATH];


CRetargetFile::CRetargetFile()
{
	InitParams();
}

CRetargetFile::~CRetargetFile()
{
	DestroyObjs();
}

int CRetargetFile::InitParams()
{
	CXMLIO::InitParams();
	m_convbonemap.clear();
	m_model = 0;
	m_bvh = 0;

	return 0;
}

int CRetargetFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();
	InitParams();

	return 0;
}

int CRetargetFile::WriteRetargetFile(WCHAR* srcpath, CModel* srcmodel, CModel* srcbvh, std::map<CBone*, CBone*>& convbonemap)
{
	if (!srcpath || !srcmodel || !srcbvh || convbonemap.empty()) {
		_ASSERT(0);
		return 1;
	}

	m_convbonemap = convbonemap;
	m_model = srcmodel;
	m_bvh = srcbvh;

	m_hfile = CreateFile( srcpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"RetargetFile : WriteRetargetFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<RTG>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	int jointnum = m_model->GetBoneListSize();
	int jointcnt;
	for( jointcnt = 0; jointcnt < jointnum; jointcnt++ ){
		CBone* srcjoint = m_model->GetBoneByZeroBaseIndex(jointcnt);
		if (srcjoint) {
			CBone* bvhjoint = m_convbonemap[srcjoint];
			if (bvhjoint) {
				const char* modeljointname = srcjoint->GetBoneName();
				const char* bvhjointname = bvhjoint->GetBoneName();
				if (modeljointname && bvhjointname) {
					CallF(WriteRetargetInfo(modeljointname, bvhjointname), return 1);
				}
			}
		}
	}

	CallF(Write2File("\r\n</RTG>\r\n"), return 1);


	return 0;
}

int CRetargetFile::WriteFileInfo()
{

	CallF( Write2File( "  <FileInfo>\r\n    <kind>RetargetJointPairFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n" ), return 1 );

	return 0;
}

int CRetargetFile::WriteRetargetInfo(const char* modeljointname, const char* bvhjointname)
{

	CallF( Write2File( "  <JointPair>\r\n" ), return 1 );
	CallF( Write2File( "    <ModelJoint>%s</ModelJoint>\r\n", modeljointname ), return 1 );
	CallF( Write2File( "    <BvhJoint>%s</BvhJoint>\r\n", bvhjointname ), return 1 );
	CallF( Write2File( "  </JointPair>\r\n" ), return 1 );

	return 0;
}

int CRetargetFile::LoadRetargetFile(WCHAR* srcpath, CModel* srcmodel, CModel* srcbvh, std::map<CBone*, CBone*>& convbonemap)
{
	if (!srcpath || !srcmodel || !srcbvh) {
		_ASSERT(0);
		return 1;
	}

	convbonemap.clear();
	m_convbonemap.clear();

	m_model = srcmodel;
	m_bvh = srcbvh;

	m_hfile = CreateFile( srcpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	CallF(SetBuffer(), return 1);

	int posstep = 0;
	XMLIOBUF fileinfobuf;
	ZeroMemory( &fileinfobuf, sizeof( XMLIOBUF ) );
	CallF( SetXmlIOBuf( &m_xmliobuf, "<FileInfo>", "</FileInfo>", &fileinfobuf ), return 1 );
	CallF( ReadFileInfo( &fileinfobuf ), return 1 );

	m_xmliobuf.pos = 0;
	int jointcnt;
	for(jointcnt = 0; jointcnt < m_model->GetBoneListSize(); jointcnt++){
		XMLIOBUF jointbuf;
		ZeroMemory( &jointbuf, sizeof( XMLIOBUF ) );
		int result;
		result = SetXmlIOBuf( &m_xmliobuf, "<JointPair>", "</JointPair>", &jointbuf );
		if (result == 0) {
			CallF(ReadRetargetInfo(jointcnt, &jointbuf), return 1);
		}
	}

	convbonemap = m_convbonemap;

	return 0;
}

/***
int CRetargetFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
int CRetargetFile::ReadFileInfo( XMLIOBUF* xmlbuf )
{

	return 0;
}
int CRetargetFile::ReadRetargetInfo( int jointcnt, XMLIOBUF* xmlbuf )
{
	CBone* modeljoint = 0;
	CBone* bvhjoint = 0;
	char modeljointname[MAX_PATH] = { 0 };
	char bvhjointname[MAX_PATH] = { 0 };

	//#############################################################################
	//名前の最後に_Jointが付いているものと付いていないものの２通りずつチェックする
	//#############################################################################

	CallF( Read_Str( xmlbuf, "<ModelJoint>", "</ModelJoint>", modeljointname, MAX_PATH ), return 1 );
	CallF( Read_Str( xmlbuf, "<BvhJoint>", "</BvhJoint>", bvhjointname, MAX_PATH ), return 1 );
	modeljointname[MAX_PATH - 1] = 0;
	bvhjointname[MAX_PATH - 1] = 0;

	modeljoint = m_model->FindBoneByName(modeljointname);//_Joint有無対応
	bvhjoint = m_bvh->FindBoneByName(bvhjointname);//_Joint有無対応

	if (modeljoint) {
		if (bvhjoint) {
			m_convbonemap[modeljoint] = bvhjoint;
		}
		else {
			_ASSERT(0);
		}
	}
	else {
		_ASSERT(0);
	}

	return 0;
}
