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

#include <ImpFile.h>

#include <Model.h>
#include <mqoobject.h>
#include <Bone.h>
#include <RigidElem.h>


#define DBGH
#include <dbg.h>

#include <map>

#include <shlwapi.h>

using namespace std;

CImpFile::CImpFile()
{
	InitParams();
}

CImpFile::~CImpFile()
{
	DestroyObjs();
}

int CImpFile::InitParams()
{
	CXMLIO::InitParams();

	m_model = 0;
	return 0;
}

int CImpFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}


int CImpFile::WriteImpFile( WCHAR* strpath, CModel* srcmodel )
{
	m_model = srcmodel;
	m_mode = XMLIO_WRITE;

	if( !m_model->GetTopBone() ){
		return 0;
	}
	if (m_model->GetImpInfoSize() <= 0) {
		return 0;
	}

	WCHAR wfilename[MAX_PATH] = {0L};
	WCHAR* lasten;
	lasten = wcsrchr( strpath, TEXT('\\') );
	if( !lasten ){
		_ASSERT( 0 );
		return 1;
	}
	wcscpy_s( wfilename, MAX_PATH, lasten + 1 );

	char mfilename[MAX_PATH] = {0};
	WideCharToMultiByte( CP_ACP, 0, wfilename, -1, mfilename, MAX_PATH, NULL, NULL );

	m_strimp = m_model->GetCurImpName();


	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"ImpFile : WriteImpFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<IMPULSE>\r\n" ), return 1 );  
	CallF( Write2File( "    <FileInfo>1001-01</FileInfo>\r\n" ), return 1 );

	//WriteImpReq( m_model->GetTopBone(false) );
	std::map<int, CBone*>::iterator itrbone;
	for (itrbone = m_model->GetBoneListBegin(); itrbone != m_model->GetBoneListEnd(); itrbone++) {
		CBone* srcbone = itrbone->second;
		if (srcbone && srcbone->IsSkeleton()) {
			WriteImp(srcbone);
		}
	}


	CallF( Write2File( "</IMPULSE>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);


	return 0;
}
void CImpFile::WriteImpReq( CBone* srcbone )
{
	if (srcbone && (srcbone->IsSkeleton())) {
		WriteImp(srcbone);
	}

	if( srcbone->GetChild(false) ){
		WriteImpReq( srcbone->GetChild(false) );
	}
	if( srcbone->GetBrother(false) ){
		WriteImpReq( srcbone->GetBrother(false) );
	}
}


int CImpFile::WriteImp( CBone* srcbone )
{
/***
  <Bone>
    <Name>zentai_ido</Nama>
    <RigidElem>
	  <ChildName>koshi</ChildName>
	  <ImpulseX>0.0</ImpulseX>
	  <ImpulseY>0.0</ImpulseY>
	  <ImpulseZ>0.0</ImpulseZ>
    </RigidElem>
  </Bone>
***/
	int impnum = 0;
	impnum = srcbone->GetImpMapSize2( m_strimp );
	if( impnum <= 0 ){
		//_ASSERT(0);
		return 0;
	}

	map<string, map<CBone*,ChaVector3>>::iterator itrcurmap;
	itrcurmap = srcbone->FindImpMap(m_strimp);
	if( itrcurmap == srcbone->GetImpMapEnd() ){
		_ASSERT( 0 );
		return 0;
	}
	CallF( Write2File( "  <Bone>\r\n" ), return 1);
	CallF( Write2File( "    <Name>%s</Name>\r\n", srcbone->GetBoneName() ), return 1);

	map<CBone*,ChaVector3>::iterator itrimp;
	for( itrimp = itrcurmap->second.begin(); itrimp != itrcurmap->second.end(); itrimp++ ){
		ChaVector3 curimp = itrimp->second;
		CallF( Write2File( "    <RigidElem>\r\n" ), return 1);

		CallF( Write2File( "      <ChildName>%s</ChildName>\r\n", itrimp->first->GetBoneName() ), return 1);
		CallF( Write2File( "      <ImpulseX>%f</ImpulseX>\r\n", curimp.x ), return 1);
		CallF( Write2File( "      <ImpulseY>%f</ImpulseY>\r\n", curimp.y ), return 1);
		CallF( Write2File( "      <ImpulseZ>%f</ImpulseZ>\r\n", curimp.z ), return 1);

		CallF( Write2File( "    </RigidElem>\r\n" ), return 1);
	}

	CallF( Write2File( "  </Bone>\r\n" ), return 1);

	return 0;
}




int CImpFile::LoadImpFile( WCHAR* strpath, CModel* srcmodel )
{
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

	char mfilename[MAX_PATH] = {0};
	WideCharToMultiByte( CP_ACP, 0, wfilename, -1, mfilename, MAX_PATH, NULL, NULL );
	m_strimp = mfilename;

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

	CBone* topbone = srcmodel->GetTopBone(false);
	if( topbone ){
		srcmodel->CreateRigidElem(srcmodel->GetDefaultReName(), 0, m_strimp.c_str(), 1);
	}

	int posstep = 0;
//	XMLIOBUF fileinfobuf;
//	ZeroMemory( &fileinfobuf, sizeof( XMLIOBUF ) );
//	CallF( SetXmlIOBuf( &m_xmliobuf, "<FileInfo>", "</FileInfo>", &fileinfobuf ), return 1 );
//	m_fileversion = CheckFileVersion( &fileinfobuf );
//	if( m_fileversion <= 0 ){
//		_ASSERT( 0 );
//		return 1;
//	}

	int findflag = 1;
	while( findflag ){
		XMLIOBUF bonebuf;
		ZeroMemory( &bonebuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( &m_xmliobuf, "<Bone>", "</Bone>", &bonebuf );
		if( ret == 0 ){
			CallF( ReadBone(&bonebuf, srcmodel), return 1 );
		}else{
			findflag = 0;
		}
	}

	int newindex = srcmodel->CalcNewImpInfoIndexByName(mfilename);
	if (newindex >= 0) {
		srcmodel->SetImpInfo(newindex, mfilename);
		srcmodel->SetCurImpIndex(newindex);
	}

	return 0;
}

int CImpFile::ReadBone(XMLIOBUF* xmliobuf, CModel* srcmodel)
{

	char bonename[256];
	ZeroMemory(bonename, sizeof(char) * 256);
	CallF( Read_Str( xmliobuf, "<Name>", "</Name>", bonename, 256 ), return 1 );
	CBone* curbone = FindBoneByName(srcmodel, bonename, 256);//_Joint有無対応
	if (!curbone){
		//_ASSERT(0);
		return 0;
	}


	int findflag = 1;
	while( findflag ){
		XMLIOBUF rebuf;
		ZeroMemory( &rebuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( xmliobuf, "<RigidElem>", "</RigidElem>", &rebuf );
		if( ret == 0 ){
			CallF( ReadRE(&rebuf, srcmodel, curbone), return 1 );
		}else{
			findflag = 0;
		}
	}

	return 0;
}

int CImpFile::ReadRE(XMLIOBUF* xmlbuf, CModel* srcmodel, CBone* curbone)
{
/***
    <RigidElem>
	  <ChildName>koshi</ChildName>
	  <ImpulseX>0.0</ImpulseX>
	  <ImpulseY>0.0</ImpulseY>
	  <ImpulseZ>0.0</ImpulseZ>
    </RigidElem>
***/

	char childname[256];
	ZeroMemory( childname, sizeof( char ) * 256 );
	CallF( Read_Str( xmlbuf, "<ChildName>", "</ChildName>", childname, 256 ), return 1 );
	CBone* childbone = FindBoneByName(srcmodel, childname, 256);//_Joint有無対応
	if (!childbone){
		//_ASSERT(0);
		return 0;
	}


	float impx = 0.0f;
	CallF( Read_Float( xmlbuf, "<ImpulseX>", "</ImpulseX>", &impx ), return 1 );
	float impy = 0.0f;
	CallF( Read_Float( xmlbuf, "<ImpulseY>", "</ImpulseY>", &impy ), return 1 );
	float impz = 0.0f;
	CallF( Read_Float( xmlbuf, "<ImpulseZ>", "</ImpulseZ>", &impz ), return 1 );

	ChaVector3 imp( impx, impy, impz );

	if( curbone ){
		if( childbone ){
			curbone->SetImpOfMap( m_strimp, childbone, imp );
		}
	}

	return 0;
}


/***
int CImpFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
