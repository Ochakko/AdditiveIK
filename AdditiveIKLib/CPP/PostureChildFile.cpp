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

#include <PostureChildFile.h>

#include <ChaScene.h>
#include <Model.h>
#include <Bone.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;



CPostureChildFile::CPostureChildFile()
{
	InitParams();
}

CPostureChildFile::~CPostureChildFile()
{
	DestroyObjs();
}

int CPostureChildFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CPostureChildFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CPostureChildFile::WritePostureChildFile(const WCHAR* srcfilepath, CModel* srcmodel, ChaScene* srcchascene)
{
	if (!srcfilepath || !srcmodel || !srcchascene) {
		_ASSERT(0);
		return 1;
	}

	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile(srcfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"PostureChildFile : WritePostureChildFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<PostureChild>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );


	std::unordered_map<int, CBone*>::iterator itrbone;
	for (itrbone = srcmodel->GetBoneListBegin(); itrbone != srcmodel->GetBoneListEnd(); itrbone++) {
		CBone* curbone = (CBone*)itrbone->second;
		if (curbone && (curbone->IsSkeleton()) && (curbone->GetPostureChildModel() != nullptr)) {
			WritePostureChildElem(curbone, srcchascene);
		}
	}


	CallF( Write2File( "</PostureChild>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}

int CPostureChildFile::WriteFileInfo()
{

	CallF(Write2File("  <FileInfo>\r\n    <kind>PostureChildFile</kind>\r\n    <version>0001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	return 0;
}

int CPostureChildFile::WritePostureChildElem(CBone* srcbone, ChaScene* srcchascene)
{
	CModel* childmodel = srcbone->GetPostureChildModel();
	if (!childmodel) {
		return 0;
	}
	ChaVector3 offset_position = srcbone->GetPostureChildOffset_Position();
	ChaVector3 offset_rotation = srcbone->GetPostureChildOffset_Rotation();

	char filename[MAX_PATH] = { 0 };
	char modelfolder[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, childmodel->GetFileName(), -1, filename, MAX_PATH, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, childmodel->GetModelFolder(), -1, modelfolder, MAX_PATH, NULL, NULL);



	CallF(Write2File("  <PostureChildElem>\r\n"), return 1);

	CallF(Write2File("    <ParentBoneName>%s</ParentBoneName>\r\n", srcbone->GetBoneName()), return 1);

	CallF(Write2File("    <PostureChildModelFolder>%s</PostureChildModelFolder>\r\n", modelfolder), return 1);
	CallF(Write2File("    <PostureChildModelName>%s</PostureChildModelName>\r\n", filename), return 1);


	CallF(Write2File("    <OffsetX>%.2f</OffsetX>\r\n",
		offset_position.x), return 1);
	CallF(Write2File("    <OffsetY>%.2f</OffsetY>\r\n",
		offset_position.y), return 1);
	CallF(Write2File("    <OffsetZ>%.2f</OffsetZ>\r\n",
		offset_position.z), return 1);

	CallF(Write2File("    <OffsetRotationX>%.2f</OffsetRotationX>\r\n",
		offset_rotation.x), return 1);
	CallF(Write2File("    <OffsetRotationY>%.2f</OffsetRotationY>\r\n",
		offset_rotation.y), return 1);
	CallF(Write2File("    <OffsetRotationZ>%.2f</OffsetRotationZ>\r\n",
		offset_rotation.z), return 1);

	CallF(Write2File("  </PostureChildElem>\r\n"), return 1);

	return 0;
}

int CPostureChildFile::LoadPostureChildFile(const WCHAR* srcfilepath, CModel* srcmodel, ChaScene* srcchascene)
{
	if (!srcfilepath || !srcmodel || !srcchascene) {
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


	int result = 0;
	while ((result == 0) || (result == 2)) {
		XMLIOBUF posturechildbuf;
		ZeroMemory(&posturechildbuf, sizeof(XMLIOBUF));
		int ret = SetXmlIOBuf(&m_xmliobuf, "<PostureChildElem>", "</PostureChildElem>", &posturechildbuf);
		if (ret == 0) {
			result = ReadPostureChildElem(srcmodel, srcchascene, &posturechildbuf);
		}
		else {
			result = 1;
		}
	}

	return 0;
}

/***
int CPostureChildFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
//int CPostureChildFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
//{
//	CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );
//
//	return 0;
//}
int CPostureChildFile::ReadPostureChildElem(CModel* srcmodel, ChaScene* srcchascene, XMLIOBUF* xmlbuf)
{
	if (!srcmodel || !srcchascene || !xmlbuf) {
		_ASSERT(0);
		return 1;
	}

	/*
	CallF(Write2File("    <ParentBoneName>%s</ParentBoneName>\r\n", srcbone->GetBoneName()), return 1);
	CallF(Write2File("    <PostureChildModelFolder>%s</PostureChildModelFolder>\r\n", modelfolder), return 1);
	CallF(Write2File("    <PostureChildModelName>%s</PostureChildModelName>\r\n", filename), return 1);
	CallF(Write2File("    <OffsetX>%.2f</OffsetX>\r\n",
		offset.x), return 1);
	CallF(Write2File("    <OffsetY>%.2f</OffsetY>\r\n",
		offset.y), return 1);
	CallF(Write2File("    <OffsetZ>%.2f</OffsetZ>\r\n",
		offset.z), return 1);
	*/

	int getparentbonename = 0;
	char parentbonename[MAX_PATH] = { 0 };
	getparentbonename = Read_Str(xmlbuf, "<ParentBoneName>", "</ParentBoneName>", parentbonename, MAX_PATH);

	int getchildfoldername = 0;
	char childfoldername[MAX_PATH] = { 0 };
	getchildfoldername = Read_Str(xmlbuf, "<PostureChildModelFolder>", "</PostureChildModelFolder>", childfoldername, MAX_PATH);

	int getchildmodelname = 0;
	char childmodelname[MAX_PATH] = { 0 };
	getchildmodelname = Read_Str(xmlbuf, "<PostureChildModelName>", "</PostureChildModelName>", childmodelname, MAX_PATH);


	int getoffsetX = 0;
	float offsetX = 0.0f;
	getoffsetX = Read_Float(xmlbuf, "<OffsetX>", "</OffsetX>", &offsetX);
	int getoffsetY = 0;
	float offsetY = 0.0f;
	getoffsetY = Read_Float(xmlbuf, "<OffsetY>", "</OffsetY>", &offsetY);
	int getoffsetZ = 0;
	float offsetZ = 0.0f;
	getoffsetZ = Read_Float(xmlbuf, "<OffsetZ>", "</OffsetZ>", &offsetZ);

	int getoffsetRotationX = 0;
	float offsetRotationX = 0.0f;
	getoffsetRotationX = Read_Float(xmlbuf, "<OffsetRotationX>", "</OffsetRotationX>", &offsetRotationX);
	int getoffsetRotationY = 0;
	float offsetRotationY = 0.0f;
	getoffsetRotationY = Read_Float(xmlbuf, "<OffsetRotationY>", "</OffsetRotationY>", &offsetRotationY);
	int getoffsetRotationZ = 0;
	float offsetRotationZ = 0.0f;
	getoffsetRotationZ = Read_Float(xmlbuf, "<OffsetRotationZ>", "</OffsetRotationZ>", &offsetRotationZ);

	if ((getparentbonename == 0) && (getchildfoldername == 0) && (getchildmodelname == 0)) {
		CBone* parentbone = srcmodel->FindBoneByName(parentbonename);
		if (parentbone != nullptr) {
			CModel* childmodel = srcchascene->GetModel(childfoldername, childmodelname);
			if (childmodel) {

				bool setflag = false;
				if ((getoffsetX == 0) && (getoffsetY == 0) && (getoffsetZ == 0)) {
					ChaVector3 offset = ChaVector3(offsetX, offsetY, offsetZ);
					parentbone->SetPostureChildOffset_Position(offset);
					setflag = true;
				}
				if ((getoffsetRotationX == 0) && (getoffsetRotationY == 0) && (getoffsetRotationZ == 0)) {
					ChaVector3 offset_rotation = ChaVector3(offsetRotationX, offsetRotationY, offsetRotationZ);
					parentbone->SetPostureChildOffset_Rotation(offset_rotation);
					setflag = true;
				}

				if (setflag) {
					parentbone->SetPostureChildModel(childmodel);
					parentbone->SetPostureChildFlag(true);
				}
			}
			else {
				_ASSERT(0);
				return 2;
			}
		}
		else {
			_ASSERT(0);
			return 2;
		}
	}
	else {
		_ASSERT(0);
		return 2;
	}

	return 0;
}
