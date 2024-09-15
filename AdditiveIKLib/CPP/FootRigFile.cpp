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

#include <FootRigFile.h>

#include <ChaScene.h>
#include <Model.h>
#include <Bone.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;



CFootRigFile::CFootRigFile()
{
	InitParams();
}

CFootRigFile::~CFootRigFile()
{
	DestroyObjs();
}

int CFootRigFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CFootRigFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CFootRigFile::WriteFootRigFile(const WCHAR* srcfilepath, FOOTRIGELEM srcfootrigelem)
{
	if (!srcfilepath) {
		_ASSERT(0);
		return 1;
	}

	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile(srcfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"FootRigFile : WriteFootRigFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<FootRig>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	WriteFootRigElem(srcfootrigelem);

	CallF( Write2File( "</FOOTRIG>\r\n" ), return 1 );

	return 0;
}

int CFootRigFile::WriteFileInfo()
{

	//CallF(Write2File("  <FileInfo>\r\n    <kind>FootRigFile</kind>\r\n    <version>0001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	//2024/09/08 ver1002
	//CallF(Write2File("  <FileInfo>\r\n    <kind>FootRigFile</kind>\r\n    <version>0002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	//2024/09/15 ver1003 <GPUInteraction>追加
	CallF(Write2File("  <FileInfo>\r\n    <kind>FootRigFile</kind>\r\n    <version>0003</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	return 0;
}

int CFootRigFile::WriteFootRigElem(FOOTRIGELEM srcfootrigelem)
{
	CallF(Write2File("  <FootRigElem>\r\n"), return 1);

	int enablefootrig = srcfootrigelem.enablefootrig ? 1 : 0;
	CallF(Write2File("    <EnableFootRig>%d</EnableFootRig>\r\n", enablefootrig), return 1);

	if (srcfootrigelem.groundmodel) {
		WCHAR wfilename[MAX_PATH] = { 0L };
		wcscpy_s(wfilename, MAX_PATH, srcfootrigelem.groundmodel->GetFileName());
		char mbfilename[MAX_PATH] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, wfilename, -1, mbfilename, MAX_PATH, NULL, NULL);
		CallF(Write2File("    <GroundModel>%s</GroundModel>\r\n", mbfilename), return 1);
	}

	if (srcfootrigelem.leftfootbone && srcfootrigelem.leftfootbone->GetParModel()) {
		CallF(Write2File("    <LeftFootBone>%s</LeftFootBone>\r\n", 
			srcfootrigelem.leftfootbone->GetBoneName()), return 1);
	}

	if (srcfootrigelem.rightfootbone && srcfootrigelem.rightfootbone->GetParModel()) {
		CallF(Write2File("    <RightFootBone>%s</RightFootBone>\r\n",
			srcfootrigelem.rightfootbone->GetBoneName()), return 1);
	}

	if ((srcfootrigelem.leftrig.useflag == 2) && (srcfootrigelem.leftrig.rigname[0] != 0L)) {
		WCHAR wrigname[MAX_PATH] = { 0L };
		wcscpy_s(wrigname, MAX_PATH, srcfootrigelem.leftrig.rigname);
		char mbrigname[MAX_PATH] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, wrigname, -1, mbrigname, MAX_PATH, NULL, NULL);
		CallF(Write2File("    <LeftFootRig>%s</LeftFootRig>\r\n",
			mbrigname), return 1);
	}

	if ((srcfootrigelem.rightrig.useflag == 2) && (srcfootrigelem.rightrig.rigname[0] != 0L)) {
		WCHAR wrigname[MAX_PATH] = { 0L };
		wcscpy_s(wrigname, MAX_PATH, srcfootrigelem.rightrig.rigname);
		char mbrigname[MAX_PATH] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, wrigname, -1, mbrigname, MAX_PATH, NULL, NULL);
		CallF(Write2File("    <RightFootRig>%s</RightFootRig>\r\n",
			mbrigname), return 1);
	}

	CallF(Write2File("    <LeftDir>%d</LeftDir>\r\n", 
		srcfootrigelem.leftdir), return 1);
	CallF(Write2File("    <RightDir>%d</RightDir>\r\n",
		srcfootrigelem.rightdir), return 1);

	CallF(Write2File("    <LeftOffset>%.2f</LeftOffset>\r\n",
		srcfootrigelem.leftoffset), return 1);
	CallF(Write2File("    <RightOffset>%.2f</RightOffset>\r\n",
		srcfootrigelem.rightoffset), return 1);

	CallF(Write2File("    <HDiffMax>%.2f</HDiffMax>\r\n",
		srcfootrigelem.hdiffmax), return 1);
	CallF(Write2File("    <RigStep>%.2f</RigStep>\r\n",
		srcfootrigelem.rigstep), return 1);

	CallF(Write2File("    <MaxCalcCount>%d</MaxCalcCount>\r\n",
		srcfootrigelem.maxcalccount), return 1);//2024/09/08 ver1002

	int gpuinteraction;
	gpuinteraction = (srcfootrigelem.gpuinteraction) ? 1 : 0;
	CallF(Write2File("    <GPUInteraction>%d</GPUInteraction>\r\n",
		gpuinteraction), return 1);//2024/09/15 ver1003



	CallF(Write2File("  </FootRigElem>\r\n"), return 1);

	return 0;
}

int CFootRigFile::LoadFootRigFile(const WCHAR* srcfilepath, CModel* srcmodel, ChaScene* srcchascene, FOOTRIGELEM* dstfootrigelem)
{
	if (!srcfilepath || !srcmodel || !dstfootrigelem) {
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


	XMLIOBUF footrigbuf;
	ZeroMemory( &footrigbuf, sizeof( XMLIOBUF ) );
	int result1 = SetXmlIOBuf(&m_xmliobuf, "<FootRigElem>", "</FootRigElem>", &footrigbuf);
	if (result1 != 0) {
		_ASSERT(0);
	}
	CallF(ReadFootRigElem(srcmodel, srcchascene, dstfootrigelem, &footrigbuf), return 1);


	return 0;
}

/***
int CFootRigFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
//int CFootRigFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
//{
//	CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );
//
//	return 0;
//}
int CFootRigFile::ReadFootRigElem(CModel* srcmodel, ChaScene* srcchascene, FOOTRIGELEM* dstfootrigelem, XMLIOBUF* xmlbuf)
{
	if (!srcmodel || !srcchascene || !dstfootrigelem) {
		_ASSERT(0);
		return 1;
	}

	int getenablefootrig = 0;
	int enablefootrig = 0;
	getenablefootrig = Read_Int(xmlbuf, "<EnableFootRig>", "</EnableFootRig>", &enablefootrig);

	int getgroundfilename = 0;
	char groundfilename[MAX_PATH] = { 0 };
	getgroundfilename = Read_Str(xmlbuf, "<GroundModel>", "</GroundModel>", groundfilename, MAX_PATH);

	int getleftbonename = 0;
	char leftbonename[MAX_PATH] = { 0 };
	getleftbonename = Read_Str(xmlbuf, "<LeftFootBone>", "</LeftFootBone>", leftbonename, MAX_PATH);

	int getrightbonename = 0;
	char rightbonename[MAX_PATH] = { 0 };
	getrightbonename = Read_Str(xmlbuf, "<RightFootBone>", "</RightFootBone>", rightbonename, MAX_PATH);

	int getleftrigname = 0;
	char leftrigname[MAX_PATH] = { 0 };
	getleftrigname = Read_Str(xmlbuf, "<LeftFootRig>", "</LeftFootRig>", leftrigname, MAX_PATH);

	int getrightrigname = 0;
	char rightrigname[MAX_PATH] = { 0 };
	getrightrigname = Read_Str(xmlbuf, "<RightFootRig>", "</RightFootRig>", rightrigname, MAX_PATH);

	int getleftdir = 0;
	int leftdir = 0;
	getleftdir = Read_Int(xmlbuf, "<LeftDir>", "</LeftDir>", &leftdir);

	int getrightdir = 0;
	int rightdir = 0;
	getrightdir = Read_Int(xmlbuf, "<RightDir>", "</RightDir>", &rightdir);

	int getleftoffset = 0;
	float leftoffset = 0;
	getleftoffset = Read_Float(xmlbuf, "<LeftOffset>", "</LeftOffset>", &leftoffset);

	int getrightoffset = 0;
	float rightoffset = 0;
	getrightoffset = Read_Float(xmlbuf, "<RightOffset>", "</RightOffset>", &rightoffset);

	int gethdiffmax = 0;
	float hdiffmax = 0;
	gethdiffmax = Read_Float(xmlbuf, "<HDiffMax>", "</HDiffMax>", &hdiffmax);

	int getrigstep = 0;
	float rigstep = 0;
	getrigstep = Read_Float(xmlbuf, "<RigStep>", "</RigStep>", &rigstep);

	int getmaxcalccount = 0;
	int maxcalccount = 50;
	getmaxcalccount = Read_Int(xmlbuf, "<MaxCalcCount>", "</MaxCalcCount>", &maxcalccount);//2024/09/08 ver1002

	int getgpuinteraction = 0;
	int gpuinteraction = 0;
	getgpuinteraction = Read_Int(xmlbuf, "<GPUInteraction>", "</GPUInteraction>", &gpuinteraction);//2024/09/15 ver1003



	dstfootrigelem->Init();

	if (getenablefootrig == 0) {
		dstfootrigelem->enablefootrig = (enablefootrig != 0);
	}

	if (getgroundfilename == 0) {
		WCHAR wfilename[MAX_PATH] = { 0L };
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
			groundfilename, -1, wfilename, MAX_PATH);
		CModel* gmodel = srcchascene->GetModel(wfilename);
		if (gmodel) {
			dstfootrigelem->groundmodel = gmodel;
		}
	}
	if (getleftbonename == 0) {
		CBone* leftbone = srcmodel->GetBoneByName(leftbonename);
		if (leftbone) {
			dstfootrigelem->leftfootbone = leftbone;
		}
	}
	if (getrightbonename == 0) {
		CBone* rightbone = srcmodel->GetBoneByName(rightbonename);
		if (rightbone) {
			dstfootrigelem->rightfootbone = rightbone;
		}
	}

	if ((getleftrigname == 0) && dstfootrigelem->leftfootbone) {
		CUSTOMRIG leftrig = dstfootrigelem->leftfootbone->GetCustomRig(leftrigname);
		dstfootrigelem->leftrig = leftrig;
	}
	if ((getrightrigname == 0) && dstfootrigelem->rightfootbone) {
		CUSTOMRIG rightrig = dstfootrigelem->rightfootbone->GetCustomRig(rightrigname);
		dstfootrigelem->rightrig = rightrig;
	}

	if (getleftdir == 0) {
		dstfootrigelem->leftdir = leftdir;
	}
	if (getrightdir == 0) {
		dstfootrigelem->rightdir = rightdir;
	}

	if (getleftoffset == 0) {
		dstfootrigelem->leftoffset = leftoffset;
	}
	if (getrightoffset == 0) {
		dstfootrigelem->rightoffset = rightoffset;
	}

	if (gethdiffmax == 0) {
		dstfootrigelem->hdiffmax = hdiffmax;
	}
	if (getrigstep == 0) {
		dstfootrigelem->rigstep = rigstep;
	}
	if (getmaxcalccount == 0) {
		dstfootrigelem->maxcalccount = maxcalccount;
	}
	if (getgpuinteraction == 0) {
		dstfootrigelem->gpuinteraction = (gpuinteraction == 1) ? true : false;

		//friファイル読込はfootrigdlg->SetModel()後に呼ばれるので　読込時に以下の処理が必要
		if (dstfootrigelem->groundmodel) {
			int result = dstfootrigelem->groundmodel->SetGPUInteraction(dstfootrigelem->gpuinteraction);
			if (result != 0) {
				_ASSERT(0);
			}
		}
	}

	return 0;
}
