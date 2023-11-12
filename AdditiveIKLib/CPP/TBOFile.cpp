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

#include <ChaFile.h>

#include <Model.h>
#include <MQOObject.h>
#include <MQOMaterial.h>

#include "..\\BTMANAGER\\BPWorld.h"
#include <ImpFile.h>
#include <GColiFile.h>
#include <RigidElemFile.h>

#include <FBXFile.h>
#include <lmtFile.h>
#include <RigFile.h>

#define DBGH
#include <dbg.h>

using namespace std;


extern float g_tmpmqomult;
extern WCHAR g_tmpmqopath[MULTIPATH];


CChaFile::CChaFile()
{
	InitParams();
}

CChaFile::~CChaFile()
{
	DestroyObjs();
}

int CChaFile::InitParams()
{
	CXMLIO::InitParams();
	m_modelindex.clear();
	ZeroMemory( m_newdirname, sizeof( WCHAR ) * MAX_PATH );

	ZeroMemory( m_wloaddir, sizeof( WCHAR ) * MAX_PATH );
	ZeroMemory( m_mloaddir, sizeof( char ) * MAX_PATH );

	return 0;
}

int CChaFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CChaFile::WriteChaFile( BPWorld* srcbpw, WCHAR* projdir, WCHAR* projname, std::vector<MODELELEM>& srcmodelindex, float srcmotspeed )
{
	m_modelindex = srcmodelindex;
	m_mode = XMLIO_WRITE;


	WCHAR strpath[MAX_PATH];
	swprintf_s( strpath, MAX_PATH, L"%s\\%s\\%s.cha", projdir, projname, projname );


	swprintf_s( m_newdirname, MAX_PATH, L"%s\\%s", projdir, projname );
	DWORD fattr;
	fattr = GetFileAttributes( m_newdirname );
	if( (fattr == -1) || ((fattr & FILE_ATTRIBUTE_DIRECTORY) == 0) ){
		int bret;
		bret = CreateDirectory( m_newdirname, NULL );
		if( bret == 0 ){
			::MessageBox( NULL, L"ディレクトリの作成に失敗しました。\nプロジェクトの保存に失敗しました。", L"エラー", MB_OK );
			_ASSERT( 0 );
			return 1;
		}
	}


	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"ChaFile : WriteChaFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<CHA>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	//CallF( Write2File( "  <MotSpeed>%f</MotSpeed>\r\n", srcmotspeed ), return 1 );


	char mprojname[256];
	ZeroMemory( mprojname, sizeof( char ) * 256 );
	WideCharToMultiByte( CP_ACP, 0, projname, -1, mprojname, 256, NULL, NULL );
	char gconame[MAX_PATH];
	ZeroMemory( gconame, sizeof( char ) * MAX_PATH );
	sprintf_s( gconame, MAX_PATH, "%s.gco", mprojname );
	CallF( Write2File( "  <GcoFile>%s</GcoFile>\r\n", gconame ), return 1 );

	WCHAR wgconame[MAX_PATH] = {0L};
	swprintf_s( wgconame, MAX_PATH, L"%s\\%s.gco", m_newdirname, projname );
	CGColiFile gcofile;
	CallF( gcofile.WriteGColiFile( wgconame, srcbpw ), return 1 );


	int modelnum = (int)m_modelindex.size();
	int modelcnt;
	for( modelcnt = 0; modelcnt < modelnum; modelcnt++ ){
		MODELELEM curme = m_modelindex[ modelcnt ];
		CallF( WriteChara( &curme, projname ), return 1 );
	}

	CallF( Write2File( "</CHA>\r\n" ), return 1 );

	return 0;
}

int CChaFile::WriteFileInfo()
{

	CallF( Write2File( "  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n" ), return 1 );

	CallF( Write2File( "  <ProjectInfo>\r\n" ), return 1 );
	CallF(Write2File("    <CharaNum>%d</CharaNum>\r\n", (int)m_modelindex.size()), return 1);
	CallF( Write2File( "  </ProjectInfo>\r\n" ), return 1 );

	return 0;
}

int CChaFile::WriteChara( MODELELEM* srcme, WCHAR* projname )
{
	char filename[MAX_PATH] = {0};
	char modelfolder[MAX_PATH] = {0};

	CModel* curmodel = srcme->modelptr;

	WideCharToMultiByte( CP_ACP, 0, curmodel->GetFileName(), -1, filename, MAX_PATH, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, curmodel->GetModelFolder(), -1, modelfolder, MAX_PATH, NULL, NULL );

	CallF( Write2File( "  <Chara>\r\n" ), return 1 );
	CallF( Write2File( "    <ModelFolder>%s</ModelFolder>\r\n", modelfolder ), return 1 );
	CallF( Write2File( "    <ModelFile>%s</ModelFile>\r\n", filename ), return 1 );
	CallF( Write2File( "    <ModelMult>%f</ModelMult>\r\n", curmodel->GetLoadMult() ), return 1 );

	CallF(Write2File("    <ModelPositionX>%f</ModelPositionX>\r\n", curmodel->GetModelPosition().x), return 1);
	CallF(Write2File("    <ModelPositionY>%f</ModelPositionY>\r\n", curmodel->GetModelPosition().y), return 1);
	CallF(Write2File("    <ModelPositionZ>%f</ModelPositionZ>\r\n", curmodel->GetModelPosition().z), return 1);

	CallF( Write2File( "    <RGDMORPH>%d</RGDMORPH>\r\n", curmodel->GetRgdMorphIndex() ), return 1 );

	CallF( Write2File( "    <CURRE>%d</CURRE>\r\n", curmodel->GetCurReIndex() ), return 1 );
	CallF( Write2File( "    <RGD>%d</RGD>\r\n", curmodel->GetRgdIndex() ), return 1 );

	int refnum = curmodel->GetRigidElemInfoSize();
	CallF( Write2File( "    <RefNum>%d</RefNum>\r\n", refnum ), return 1 );
	int refno;
	for( refno = 0; refno < refnum; refno++ ){
		REINFO currei = curmodel->GetRigidElemInfo( refno );
		CallF( Write2File( "    <RefFile>%s</RefFile>\r\n", currei.filename ), return 1 );
	}

	int impnum = curmodel->GetImpInfoSize();
	CallF( Write2File( "    <ImpNum>%d</ImpNum>\r\n", impnum ), return 1 );
	int impno;
	for( impno = 0; impno < impnum; impno++ ){
		CallF( Write2File( "    <ImpFile>%s</ImpFile>\r\n", curmodel->GetImpInfo( impno ).c_str() ), return 1 );
	}

/***
	char mprojname[256];
	ZeroMemory( mprojname, sizeof( char ) * 256 );
	WideCharToMultiByte( CP_ACP, 0, projname, -1, mprojname, 256, NULL, NULL );

	char impname[MAX_PATH];
	ZeroMemory( impname, sizeof( char ) * MAX_PATH );
	sprintf_s( impname, MAX_PATH, "%s.imp", mprojname );
	if( curmodel->m_rgdindex >= 0 ){
		CallF( Write2File( "    <ImpFile>%s</ImpFile>\r\n", impname ), return 1 );
	}
***/
//////////////

	WCHAR charafolder[MAX_PATH] = {0L};
	swprintf_s( charafolder, MAX_PATH, L"%s\\%s", m_newdirname, curmodel->GetModelFolder() );
	DWORD fattr;
	fattr = GetFileAttributes( charafolder );
	if( (fattr == -1) || ((fattr & FILE_ATTRIBUTE_DIRECTORY) == 0) ){
		int bret;
		bret = CreateDirectory( charafolder, NULL );
		if( bret == 0 ){
			::MessageBox( NULL, L"ディレクトリの作成に失敗しました。\nプロジェクトの保存に失敗しました。", L"エラー", MB_OK );
			_ASSERT( 0 );
			return 1;
		}
	}

	BOOL bret;
	BOOL bcancel;
	WCHAR srcpath[MAX_PATH] = {0L};
	WCHAR dstpath[MAX_PATH] = {0L};
	/*
	//FBXファイルのコピー
	swprintf_s( srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), curmodel->GetFileName() );
	swprintf_s( dstpath, MAX_PATH, L"%s\\%s", charafolder, curmodel->GetFileName() );

	int chksame = wcscmp( curmodel->GetDirName(), charafolder );
	if( chksame != 0 ){
		bcancel = FALSE;
		bret = CopyFileEx( srcpath, dstpath, NULL, NULL, &bcancel, 0 );
		if( bret == 0 ){
			_ASSERT( 0 );
			return 1;
		}
	}
	*/


	WCHAR wcfbxfilename[MAX_PATH] = { 0L };//WCHAR
	char fbxpath[MAX_PATH] = { 0 };//UTF-8
	swprintf_s(wcfbxfilename, MAX_PATH, L"%s\\%s", charafolder, curmodel->GetFileName());
	WideCharToMultiByte(CP_UTF8, 0, wcfbxfilename, -1, fbxpath, MAX_PATH, NULL, NULL);
	SYSTEMTIME localtime;
	GetLocalTime(&localtime);
	char fbxdate[MAX_PATH] = { 0L };
	sprintf_s(fbxdate, MAX_PATH, "CommentForEGP_%04d%02d%02d%02d%02d%02d",
		localtime.wYear, localtime.wMonth, localtime.wDay, localtime.wHour, localtime.wMinute, localtime.wSecond);
	int ret1 = WriteFBXFile(curmodel->GetFBXSDK(), curmodel, fbxpath, fbxdate);
	_ASSERT(!ret1);


	if (curmodel->GetOldAxisFlagAtLoading() == 0){
		WCHAR lmtname[MAX_PATH] = { 0L };
		swprintf_s(lmtname, MAX_PATH, L"%s\\%s.lmt", charafolder, curmodel->GetFileName());
		CLmtFile lmtfile;
		lmtfile.WriteLmtFile(lmtname, curmodel, fbxdate);


		WCHAR rigname[MAX_PATH] = { 0L };
		swprintf_s(rigname, MAX_PATH, L"%s\\%s.rig", charafolder, curmodel->GetFileName());
		CRigFile rigfile;
		rigfile.WriteRigFile(rigname, curmodel);
	}

	/***
	//mqoファイルの場合のテクスチャ
	map<int, CMQOMaterial*>::iterator itrmat;
	for( itrmat = curmodel->m_material.begin(); itrmat != curmodel->m_material.end(); itrmat++ ){
		CMQOMaterial* curmqomat = itrmat->second;
		if( curmqomat && curmqomat->tex[0] && (curmqomat->m_texid >= 0) ){
			WCHAR wtex[256] = {0L};
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, curmqomat->tex, 256, wtex, 256 );
			swprintf_s( srcpath, MAX_PATH, L"%s\\%s", curmodel->m_dirname, wtex );
			swprintf_s( dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex );

			int chksame = wcscmp( curmodel->m_dirname, charafolder );
			if( chksame != 0 ){
				bcancel = FALSE;
				bret = CopyFileEx( srcpath, dstpath, NULL, NULL, &bcancel, 0 );
				if( bret == 0 ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
	***/
	
	//FBXファイルの場合のテクスチャ
	map<int,CMQOObject*>::iterator itrobj;
	for( itrobj = curmodel->GetMqoObjectBegin(); itrobj != curmodel->GetMqoObjectEnd(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		if( curobj ){
			map<int,CMQOMaterial*>::iterator itr;
			for( itr = curobj->GetMaterialBegin(); itr != curobj->GetMaterialEnd(); itr++ ){
				CMQOMaterial* curmqomat = itr->second;
				if( curmqomat && *(curmqomat->GetTex()) && (curmqomat->GetTexID() >= 0) ){
					WCHAR wtex[256] = {0L};
					MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, curmqomat->GetTex(), 256, wtex, 256 );
					swprintf_s( srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), wtex );
					swprintf_s( dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex );

					int chksame = wcscmp( curmodel->GetDirName(), charafolder );
					if( chksame != 0 ){
						bcancel = FALSE;
						bret = CopyFileEx( srcpath, dstpath, NULL, NULL, &bcancel, 0 );
						if( bret == 0 ){
							_ASSERT( 0 );
							return 1;
						}
					}
				}

			}
		}
	}

	int savecurreindex = curmodel->GetCurReIndex();
	for( refno = 0; refno < refnum; refno++ ){
		REINFO currei = curmodel->GetRigidElemInfo( refno );
		WCHAR wfilename[MAX_PATH] = {0L};
		ZeroMemory( wfilename, sizeof( WCHAR ) * 256 );
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, currei.filename, MAX_PATH, wfilename, MAX_PATH );

		WCHAR wrefname[MAX_PATH] = {0L};
		swprintf_s( wrefname, MAX_PATH, L"%s\\%s", charafolder, wfilename );
		CallF( curmodel->SetCurrentRigidElem( refno ), return 1 );

		CRigidElemFile refile;
		CallF( refile.WriteRigidElemFile( wrefname, curmodel, refno ), return 1 );

	}
	CallF( curmodel->SetCurrentRigidElem( savecurreindex ), return 1 );


	for( impno = 0; impno < impnum; impno++ ){
		WCHAR wfilename[MAX_PATH] = {0L};
		ZeroMemory( wfilename, sizeof( WCHAR ) * 256 );
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, curmodel->GetImpInfo( impno ).c_str(), -1, wfilename, MAX_PATH );

		WCHAR wimpname[MAX_PATH] = {0L};
		swprintf_s( wimpname, MAX_PATH, L"%s\\%s", charafolder, wfilename );

		CImpFile impfile;
		CallF( impfile.WriteImpFile( wimpname, curmodel ), return 1 );
	}


	CallF( Write2File( "  </Chara>\r\n" ), return 1 );


	return 0;
}

int CChaFile::LoadChaFile( WCHAR* strpath, CModel* (*srcfbxfunc)( int skipdefref, int inittimelineflag ), int (*srcReffunc)(), int (*srcImpFunc)(), int (*srcGcoFunc)(), 
	int (*srcReMenu)( int selindex1, int callbymenu1 ), int (*srcRgdMenu)( int selindex2, int callbymenu2 ), int (*srcMorphMenu)( int selindex3 ), int (*srcImpMenu)( int selindex4 ) )
{
	m_mode = XMLIO_LOAD;
	m_FbxFunc = srcfbxfunc;
	m_RefFunc = srcReffunc;
	m_ImpFunc = srcImpFunc;
	m_GcoFunc = srcGcoFunc;
	m_ReMenu = srcReMenu;
	m_RgdMenu = srcRgdMenu;
	m_MorphMenu = srcMorphMenu;
	m_ImpMenu = srcImpMenu;

	wcscpy_s( m_wloaddir, MAX_PATH, strpath );
	WCHAR* lasten;
	lasten = wcsrchr( m_wloaddir, TEXT('\\') );
	if( !lasten ){
		_ASSERT( 0 );
		return 1;
	}
	*lasten = 0L;

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
	CallF( SetXmlIOBuf( &m_xmliobuf, "<ProjectInfo>", "</ProjectInfo>", &projinfobuf ), return 1 );
	int charanum = 0;
	CallF( ReadProjectInfo( &projinfobuf, &charanum ), return 1 );

	//m_xmliobuf.pos = 0;
	//m_motspeed = 1.0f;
	//Read_Float( &m_xmliobuf, "<MotSpeed>", "</MotSpeed>", &m_motspeed );


	m_xmliobuf.pos = 0;
	XMLIOBUF gcobuf;
	ZeroMemory( &gcobuf, sizeof( XMLIOBUF ) );
	CallF( SetXmlIOBuf( &m_xmliobuf, "<GcoFile>", "</GcoFile>", &gcobuf, 0 ), return 1 );
	char gcofile[MAX_PATH] = {0};
	CallF( Read_Str( &gcobuf, "<GcoFile>", "</GcoFile>", gcofile, MAX_PATH ), return 1 );

	WCHAR wmodelfolder[MAX_PATH] = {0L};
	WCHAR wfilename[MAX_PATH] = {0L};
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, gcofile, MAX_PATH, wfilename, MAX_PATH );
	swprintf_s( g_tmpmqopath, MULTIPATH, L"%s\\%s", m_wloaddir, wfilename );
	int chkret;
	chkret = (this->m_GcoFunc)();
	if ((chkret != 0) && (chkret != 2)) {
		_ASSERT(0);
		return 1;
	}


	m_xmliobuf.pos = 0;
	int characnt;
	for( characnt = 0; characnt < charanum; characnt++ ){
		XMLIOBUF charabuf;
		ZeroMemory( &charabuf, sizeof( XMLIOBUF ) );
		CallF( SetXmlIOBuf( &m_xmliobuf, "<Chara>", "</Chara>", &charabuf ), return 1 );
		CallF( ReadChara( charanum, characnt, &charabuf ), return 1 );
	}

	m_xmliobuf.pos = 0;
	XMLIOBUF wallbuf;
	ZeroMemory(&wallbuf, sizeof(XMLIOBUF));
	int chkret2;
	chkret2 = SetXmlIOBuf(&m_xmliobuf, "<Wall>", "</Wall>", &wallbuf);
	if (chkret2 == 0) {
		CallF(ReadWall(&wallbuf), return 2);
	}

	return 0;
}

/***
int CChaFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
int CChaFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
{
	CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );

	return 0;
}
int CChaFile::ReadChara( int charanum, int characnt, XMLIOBUF* xmlbuf )
{
/***
	CallF( Write2File( "  <Chara>\r\n" ), return 1 );
	CallF( Write2File( "    <ModelFolder>%s</ModelFolder>\r\n", modelfolder ), return 1 );
	CallF( Write2File( "    <ModelFile>%s</ModelFile>\r\n", filename ), return 1 );
	CallF( Write2File( "    <ModelMult>%f</ModelMult>\r\n", curmodel->m_loadmult ), return 1 );
	int refnum = curmodel->m_rigideleminfo.size();
	CallF( Write2File( "    <RefNum>%d</RefNum>\r\n", refnum ), return 1 );
	int refno;
	for( refno = 0; refno < refnum; refno++ ){
		REINFO currei = curmodel->m_rigideleminfo[ refno ];
		CallF( Write2File( "    <RefFile>%s</RefFile>\r\n", currei.filename ), return 1 );
	}

	char mprojname[256];
	ZeroMemory( mprojname, sizeof( char ) * 256 );
	WideCharToMultiByte( CP_ACP, 0, projname, -1, mprojname, 256, NULL, NULL );

	char impname[MAX_PATH];
	ZeroMemory( impname, sizeof( char ) * MAX_PATH );
	sprintf_s( impname, MAX_PATH, "%s.imp", mprojname );
	if( curmodel->m_rgdindex >= 0 ){
		CallF( Write2File( "    <ImpFile>%s</ImpFile>\r\n", impname ), return 1 );
	}
	CallF( Write2File( "  </Chara>\r\n" ), return 1 );
***/

	char modelfolder[MAX_PATH] = {0};
	char filename[MAX_PATH] = {0};
	float modelmult = 1.0f;
	int refnum = 0;
	int impnum = 0;
	int curre = 0;
	int rgd = 0;
	int morphindex = -1;

	float posx = 0.0f;
	float posy = 0.0f;
	float posz = 0.0f;

	CallF( Read_Str( xmlbuf, "<ModelFolder>", "</ModelFolder>", modelfolder, MAX_PATH ), return 1 );
	CallF( Read_Str( xmlbuf, "<ModelFile>", "</ModelFile>", filename, MAX_PATH ), return 1 );

	//modelposは必須ではない
	Read_Float(xmlbuf, "<ModelPositionX>", "</ModelPositionX>", &posx);
	Read_Float(xmlbuf, "<ModelPositionY>", "</ModelPositionY>", &posy);
	Read_Float(xmlbuf, "<ModelPositionZ>", "</ModelPositionZ>", &posz);

	CallF( Read_Float( xmlbuf, "<ModelMult>", "</ModelMult>", &modelmult ), return 1 );

	CallF( Read_Int( xmlbuf, "<CURRE>", "</CURRE>", &curre ), return 1 );
	CallF( Read_Int( xmlbuf, "<RGD>", "</RGD>", &rgd ), return 1 );

	CallF( Read_Int( xmlbuf, "<RefNum>", "</RefNum>", &refnum ), return 1 );
	int retimp;
	retimp = Read_Int( xmlbuf, "<ImpNum>", "</ImpNum>", &impnum );
	if( retimp ){
		//impnum = 1;
		impnum = 0;
	}

	Read_Int( xmlbuf, "<RGDMORPH>", "</RGDMORPH>", &morphindex );


	char mmqopath[MAX_PATH] = {0};
	WCHAR wmodelfolder[MAX_PATH] = {0L};
	WCHAR wfilename[MAX_PATH] = {0L};

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, modelfolder, MAX_PATH, wmodelfolder, MAX_PATH );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, filename, MAX_PATH, wfilename, MAX_PATH );
	swprintf_s( g_tmpmqopath, MULTIPATH, L"%s\\%s\\%s", m_wloaddir, wmodelfolder, wfilename );
	g_tmpmqomult = modelmult;

	int inittimeline = 0;
	if (characnt == (charanum - 1)) {
		inittimeline = 1;
	}
	
	
	int skipdefref = (int)(refnum != 0);//default_ref.refが無い場合にCModel::LoadFBXでdefault_ref.refを作るためのフラグ
	//int skipdefref = 0;//CModel::LoadFBXでCreateRigidElemReqを呼ぶ必要がある。FBXだけ読み込んでいる状態でdefault_refが必要。
	CModel* newmodel = 0;
	newmodel = (this->m_FbxFunc)( skipdefref, inittimeline );
	_ASSERT( newmodel );

	
	//newmodel->m_tmpmotspeed = m_motspeed;

	newmodel->SetModelPosition(ChaVector3(posx, posy, posz));


	if( refnum > 0 ){
		int refcnt;

		for( refcnt = 0; refcnt < refnum; refcnt++ ){
			XMLIOBUF refbuf;
			ZeroMemory( &refbuf, sizeof( XMLIOBUF ) );
			CallF( SetXmlIOBuf( xmlbuf, "<RefFile>", "</RefFile>", &refbuf, 0 ), return 1 );
			
			char reffile[MAX_PATH] = {0};
			CallF( Read_Str( &refbuf, "<RefFile>", "</RefFile>", reffile, MAX_PATH ), return 1 );
			WCHAR wreffile[MAX_PATH] = {0L};
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, reffile, MAX_PATH, wreffile, MAX_PATH );
			swprintf_s( g_tmpmqopath, MULTIPATH, L"%s\\%s\\%s", m_wloaddir, wmodelfolder, wreffile );

			if (refcnt >= 1) {//refcnt == 0 の１つのめのrefファイルdefault_ref.refはm_FBXFuncで読み込む
				int chkret;
				chkret = (this->m_RefFunc)();
				if ((chkret != 0) && (chkret != 2)) {
					_ASSERT(0);
					return 1;
				}
			}
		}
	}

	if( impnum > 0 ){
		int impcnt;

		for( impcnt = 0; impcnt < impnum; impcnt++ ){
			XMLIOBUF refbuf;
			ZeroMemory( &refbuf, sizeof( XMLIOBUF ) );
			CallF( SetXmlIOBuf( xmlbuf, "<ImpFile>", "</ImpFile>", &refbuf, 0 ), return 1 );
			
			char impfile[MAX_PATH] = {0};
			CallF( Read_Str( &refbuf, "<ImpFile>", "</ImpFile>", impfile, MAX_PATH ), return 1 );
			WCHAR wimpfile[MAX_PATH] = {0L};
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, impfile, MAX_PATH, wimpfile, MAX_PATH );
			swprintf_s( g_tmpmqopath, MULTIPATH, L"%s\\%s\\%s", m_wloaddir, wmodelfolder, wimpfile );

			int chkret2;
			chkret2 = (this->m_ImpFunc)();
			if ((chkret2 != 0) && (chkret2 != 2)) {
				_ASSERT(0);
				return 1;
			}
		}
	}

	CallF( (this->m_ReMenu)( curre, 0 ), return 1 );
	CallF( (this->m_RgdMenu)( rgd, 0 ), return 1 );
	CallF( (this->m_MorphMenu)( morphindex ), return 1 );
	CallF( (this->m_ImpMenu)( 0 ), return 1 );

	newmodel->CreateBtObject(1);//初回
	newmodel->CalcBoneEul(-1);//


	return 0;
}


int CChaFile::ReadWall(XMLIOBUF* xmlbuf)
{
	float boxsize = 1.0f;
	int wallw = 1;
	int wallh = 1;
	float wallx = 0.0f;
	float wally = 0.0f;
	float wallz = 0.0f;
	float wallrot = 0.0f;

	CallF(Read_Float(xmlbuf, "<BOXSIZE>", "</BOXSIZE>", &boxsize), return 1);
	CallF(Read_Int(xmlbuf, "<WALLW>", "</WALLW>", &wallw), return 1);
	CallF(Read_Int(xmlbuf, "<WALLH>", "</WALLH>", &wallh), return 1);
	CallF(Read_Float(xmlbuf, "<WALLPOSX>", "</WALLPOSX>", &wallx), return 1);
	CallF(Read_Float(xmlbuf, "<WALLPOSY>", "</WALLPOSY>", &wally), return 1);
	CallF(Read_Float(xmlbuf, "<WALLPOSZ>", "</WALLPOSZ>", &wallz), return 1);
	CallF(Read_Float(xmlbuf, "<WALLROT>", "</WALLROT>", &wallrot), return 1);


	return 0;

}