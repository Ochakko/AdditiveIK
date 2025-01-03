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

#include <GrassElem.h>
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
#include <LIMFIle.h>
#include <DispGroupFile.h>
#include <ShaderTypeFile.h>
#include <MAFile.h>

#include <FootRigDlg.h>
#include <gltfLoader.h>

#include "..\\..\\AdditiveIK\FrameCopyDlg.h"


#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル AdditiveIK.cpp
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

	m_motinfo.clear();
	m_FbxFunc = nullptr;
	m_RefFunc = nullptr;
	m_ImpFunc = nullptr;
	m_GcoFunc = nullptr;
	m_ReMenu = nullptr;
	m_RgdMenu = nullptr;
	m_MorphMenu = nullptr;
	m_ImpMenu = nullptr;

	m_motspeed = 1.0f;

	m_footrigdlg = nullptr;

	return 0;
}

int CChaFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}

int CChaFile::WriteChaFile(bool limitdegflag, BPWorld* srcbpw, WCHAR* projdir, WCHAR* projname, 
	std::vector<MODELELEM>& srcmodelindex, float srcmotspeed, 
	map<CModel*, CFrameCopyDlg*> srcselbonedlgmap,
	std::vector<CGrassElem*> srcgrasselemvec)
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
			::MessageBox( NULL, L"ディレクトリの作成に失敗しました。\n書き込み禁止ディレクトリの可能性があります。\n保存場所を変えて再試行してみてください。", L"エラー", MB_OK );
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


	CallF(Write2File("  <ProjFov>%.3f</ProjFov>\r\n", g_fovy * 180.0f / (float)PI), return 1);
	CallF(Write2File("  <ProjNear>%.3f</ProjNear>\r\n", g_projnear), return 1);
	CallF(Write2File("  <ProjFar>%.3f</ProjFar>\r\n", g_projfar), return 1);

	CallF(Write2File("  <PickDistRate>%.2f</PickDistRate>\r\n", g_pickdistrate), return 1);
	CallF(Write2File("  <EditRate>%.3f</EditRate>\r\n", g_physicsmvrate), return 1);
	CallF(Write2File("  <PlayingSpeed>%.3f</PlayingSpeed>\r\n", g_dspeed), return 1);
	CallF(Write2File("  <PhysicalLimitScale>%.3f</PhysicalLimitScale>\r\n", g_physicalLimitScale), return 1);
	CallF(Write2File("  <BtMovableRate>%d</BtMovableRate>\r\n", g_physicalMovableRate), return 1);
	CallF(Write2File("  <CameraDist>%.3f</CameraDist>\r\n", g_camdist), return 1);
	CallF(Write2File("  <AKScale>%.3f</AKScale>\r\n", g_akscale), return 1);
	CallF(Write2File("  <BtVScaleOnLimitEul>%.3f</BtVScaleOnLimitEul>\r\n", g_physicalVeloScale), return 1);

	CallF(Write2File("  <BoneAxis>%d</BoneAxis>\r\n", g_boneaxis), return 1);


	CallF(Write2File("  <2L_LOD0>%.2f</2L_LOD0>\r\n", g_lodrate2L[CHKINVIEW_LOD0]), return 1);
	CallF(Write2File("  <2L_LOD1>%.2f</2L_LOD1>\r\n", g_lodrate2L[CHKINVIEW_LOD1]), return 1);

	CallF(Write2File("  <3L_LOD0>%.2f</3L_LOD0>\r\n", g_lodrate3L[CHKINVIEW_LOD0]), return 1);
	CallF(Write2File("  <3L_LOD1>%.2f</3L_LOD1>\r\n", g_lodrate3L[CHKINVIEW_LOD1]), return 1);
	CallF(Write2File("  <3L_LOD2>%.2f</3L_LOD2>\r\n", g_lodrate3L[CHKINVIEW_LOD2]), return 1);

	CallF(Write2File("  <4L_LOD0>%.2f</4L_LOD0>\r\n", g_lodrate4L[CHKINVIEW_LOD0]), return 1);
	CallF(Write2File("  <4L_LOD1>%.2f</4L_LOD1>\r\n", g_lodrate4L[CHKINVIEW_LOD1]), return 1);
	CallF(Write2File("  <4L_LOD2>%.2f</4L_LOD2>\r\n", g_lodrate4L[CHKINVIEW_LOD2]), return 1);
	CallF(Write2File("  <4L_LOD2>%.2f</4L_LOD3>\r\n", g_lodrate4L[CHKINVIEW_LOD3]), return 1);


	int modelnum = (int)m_modelindex.size();
	int modelcnt;
	for( modelcnt = 0; modelcnt < modelnum; modelcnt++ ){
		MODELELEM curme = m_modelindex[ modelcnt ];
		CGrassElem* curgrasselem = nullptr;
		if (curme.modelptr && curme.modelptr->GetGrassFlag()) {
			int grasselemnum = (int)srcgrasselemvec.size();
			int grasselemindex;
			for (grasselemindex = 0; grasselemindex < grasselemnum; grasselemindex++) {
				CGrassElem* chkgrasselem = srcgrasselemvec[grasselemindex];
				if (chkgrasselem && chkgrasselem->GetGrass() && chkgrasselem->GetGrass() == curme.modelptr) {
					curgrasselem = chkgrasselem;
					break;
				}
			}
		}
		else {
			curgrasselem = nullptr;
		}
		CallF(WriteChara(limitdegflag, &curme, projname, srcselbonedlgmap, curgrasselem), return 1 );
	}

	CallF( Write2File( "</CHA>\r\n" ), return 1 );

	return 0;
}

int CChaFile::WriteFileInfo()
{

	//CallF( Write2File( "  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n" ), return 1 );
	//version 1002 : 2023/03/24 1.2.0.17 RC2
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1003 : 2023/03/24 1.2.0.17 RC3
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ChatCats3D_ProjectFile</kind>\r\n    <version>1003</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1004 : 2023/07/21 1.2.0.23へ向けて
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1005</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1006 : 2024/04/03 1.0.0.14へ向けて  <PickDistRate>追加
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1006</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1007 : 2024/04/17 1.0.0.16へ向けて  <EditRate>, <PlayingSpeed>, <PhysicalLimitScale>, <CameraDist>追加
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1007</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1008 : 2024/04/22 1.0.0.17へ向けて  <4L_LOD*>, <BoneAxis>追加
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1008</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1009 : 2024/04/23 1.0.0.17へ向けて  <AKScale>追加
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1009</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1010 : 2024/05/02 1.0.0.19へ向けて  <BtMovableRate>追加
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1010</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1011 : 2024/05/04 1.0.0.19へ向けて  <BtVScaleOnLimitEul>追加
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1011</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//version 1012 : 2024/05/12 1.0.0.20へ向けて  <GrassFlag>, <GrassMat>追加
	CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIK_ProjectFile</kind>\r\n    <version>1012</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	
	CallF( Write2File( "  <ProjectInfo>\r\n" ), return 1 );
	CallF(Write2File("    <CharaNum>%d</CharaNum>\r\n", (int)m_modelindex.size()), return 1);
	CallF( Write2File( "  </ProjectInfo>\r\n" ), return 1 );

	return 0;
}

int CChaFile::WriteChara(bool limitdegflag, MODELELEM* srcme, WCHAR* projname, 
	map<CModel*, CFrameCopyDlg*> srcselbonedlgmap,
	CGrassElem* srcgrasselem)
{
	if (!srcme || !projname) {
		_ASSERT(0);
		return 1;
	}

	char filename[MAX_PATH] = {0};
	char modelfolder[MAX_PATH] = {0};

	CModel* curmodel = srcme->modelptr;
	if (!curmodel) {
		_ASSERT(0);
		return 1;
	}


	WideCharToMultiByte( CP_ACP, 0, curmodel->GetFileName(), -1, filename, MAX_PATH, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, curmodel->GetModelFolder(), -1, modelfolder, MAX_PATH, NULL, NULL );
	CallF( Write2File("  <Chara>\r\n" ), return 1 );
	CallF(Write2File("    <ModelFolder>%s</ModelFolder>\r\n", modelfolder ), return 1 );
	CallF(Write2File("    <ModelFile>%s</ModelFile>\r\n", filename ), return 1 );
	CallF(Write2File("    <ModelMult>%f</ModelMult>\r\n", curmodel->GetLoadMult() ), return 1 );

	if (curmodel->GetGrassFlag() && srcgrasselem) {
		CallF(Write2File("    <GrassFlag>1</GrassFlag>\r\n"), return 1);
	}
	else {
		CallF(Write2File("    <GrassFlag>0</GrassFlag>\r\n"), return 1);
	}

	if (curmodel->GetGrassFlag() && srcgrasselem) {
		int grassnum = srcgrasselem->GetGrassNum();
		int grassindex;
		for (grassindex = 0; grassindex < grassnum; grassindex++) {
			if (grassindex < GRASSINDEXMAX) {
				ChaMatrix grassmat = srcgrasselem->GetGrassMat(grassindex);
				CallF(Write2File("    <GrassMat>%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f</GrassMat>\r\n",
					grassmat.data[MATI_11], grassmat.data[MATI_12], grassmat.data[MATI_13], grassmat.data[MATI_14],
					grassmat.data[MATI_21], grassmat.data[MATI_22], grassmat.data[MATI_23], grassmat.data[MATI_24],
					grassmat.data[MATI_31], grassmat.data[MATI_32], grassmat.data[MATI_33], grassmat.data[MATI_34],
					grassmat.data[MATI_41], grassmat.data[MATI_42], grassmat.data[MATI_43], grassmat.data[MATI_44]
				), return 1);
			}
		}
	}


	CallF(Write2File("    <ModelPositionX>%f</ModelPositionX>\r\n", curmodel->GetModelPosition().x), return 1);
	CallF(Write2File("    <ModelPositionY>%f</ModelPositionY>\r\n", curmodel->GetModelPosition().y), return 1);
	CallF(Write2File("    <ModelPositionZ>%f</ModelPositionZ>\r\n", curmodel->GetModelPosition().z), return 1);

	CallF(Write2File("    <ModelRotationX>%f</ModelRotationX>\r\n", curmodel->GetModelRotation().x), return 1);
	CallF(Write2File("    <ModelRotationY>%f</ModelRotationY>\r\n", curmodel->GetModelRotation().y), return 1);
	CallF(Write2File("    <ModelRotationZ>%f</ModelRotationZ>\r\n", curmodel->GetModelRotation().z), return 1);

	ChaVector4 materialdisprate = curmodel->GetMaterialDispRate();
	CallF(Write2File("    <DiffuseDispRate>%f</DiffuseDispRate>\r\n", materialdisprate.x), return 1);
	CallF(Write2File("    <SpecularDispRate>%f</SpecularDispRate>\r\n", materialdisprate.y), return 1);
	CallF(Write2File("    <EmissiveDispRate>%f</EmissiveDispRate>\r\n", materialdisprate.z), return 1);
	CallF(Write2File("    <AmbientDispRate>%f</AmbientDispRate>\r\n", materialdisprate.w), return 1);


	//2023/07/21
	CallF(Write2File("    <CustomIKStopName>1</CustomIKStopName>\r\n"), return 1);
	int ikstopsize = curmodel->GetIKStopNameSize();
	int ikstopno;
	for (ikstopno = 0; ikstopno < ikstopsize; ikstopno++) {
		int errorno = 0;
		string stopname = curmodel->GetIKStopName(ikstopno, &errorno);
		if (errorno == 0) {
			CallF(Write2File("    <IKStopName>%s</IKStopName>\r\n", stopname.c_str()), return 1);
		}
	}

	//2023/09/25
	int laternum = curmodel->GetLaterTransparentNum();
	int laterno;
	for (laterno = 0; laterno < laternum; laterno++) {
		string latertexname = curmodel->GetLaterTransparent(laterno);
		CallF(Write2File("    <LaterTransparent>%s</LaterTransparent>\r\n", latertexname.c_str()), return 1);
	}



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
			::MessageBox(NULL, L"ディレクトリの作成に失敗しました。\n書き込み禁止ディレクトリの可能性があります。\n保存場所を変えて再試行してみてください。", L"エラー", MB_OK);
			_ASSERT( 0 );
			return 1;
		}
	}


	SYSTEMTIME localtime;
	GetLocalTime(&localtime);
	char fbxdate[MAX_PATH] = { 0L };
	sprintf_s(fbxdate, MAX_PATH, "CommentForEGP_%04u%02u%02u%02u%02u%02u",
		localtime.wYear, localtime.wMonth, localtime.wDay, localtime.wHour, localtime.wMinute, localtime.wSecond);


	WCHAR wcfbxfilename[MAX_PATH] = { 0L };//WCHAR
	char fbxpath[MAX_PATH] = { 0 };//UTF-8
	swprintf_s(wcfbxfilename, MAX_PATH, L"%s\\%s", charafolder, curmodel->GetFileName());
	WideCharToMultiByte(CP_UTF8, 0, wcfbxfilename, -1, fbxpath, MAX_PATH, NULL, NULL);
	int ret1 = WriteFBXFile(limitdegflag, curmodel->GetFBXSDK(), curmodel, fbxpath, fbxdate);
	if (ret1) {
		_ASSERT(!ret1);
		return 1;
	}
	
	map<CModel*, CFrameCopyDlg*>::iterator itrselbonedlg;
	itrselbonedlg = srcselbonedlgmap.find(curmodel);
	if (itrselbonedlg != srcselbonedlgmap.end()) {
		CFrameCopyDlg* framecopydlg = itrselbonedlg->second;
		if (framecopydlg) {
			WCHAR tboname[MAX_PATH] = { 0L };
			swprintf_s(tboname, MAX_PATH, L"%s\\%s.tbo", charafolder, curmodel->GetFileName());
			int result = framecopydlg->SaveWithProjectFile(tboname);
			_ASSERT(result == 0);
		}
	}


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


	{
		WCHAR wfilename[MAX_PATH] = { 0L };
		ZeroMemory(wfilename, sizeof(WCHAR) * 256);
		swprintf_s(wfilename, 256, L"%s.stf", curmodel->GetFileName());

		WCHAR pathname[MAX_PATH] = { 0L };
		swprintf_s(pathname, MAX_PATH, L"%s\\%s", charafolder, wfilename);

		CShaderTypeFile stfile;
		CallF(stfile.WriteShaderTypeFile(pathname, curmodel), return 1);
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
	
	{
		BOOL bret;
		BOOL bcancel;
		WCHAR srcpath[MAX_PATH] = { 0L };
		WCHAR dstpath[MAX_PATH] = { 0L };
		//################################
		//FBXファイルの場合のテクスチャ
		//################################


		int materialnum = curmodel->GetMQOMaterialSize();
		int matindex;
		for (matindex = 0; matindex < materialnum; matindex++) {
			CMQOMaterial* curmqomat = curmodel->GetMQOMaterialByIndex(matindex);
			if (curmqomat) {
				if (*(curmqomat->GetAlbedoTex()) && (curmqomat->GetAlbedoTexID() >= 0)) {
					WCHAR wtex[256] = { 0L };
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, curmqomat->GetAlbedoTex(), 256, wtex, 256);
					swprintf_s(srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), wtex);
					swprintf_s(dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex);

					int chksame = wcscmp(curmodel->GetDirName(), charafolder);
					if (chksame != 0) {
						bcancel = FALSE;
						bret = CopyFileEx(srcpath, dstpath, NULL, NULL, &bcancel, 0);
						if (bret == 0) {
							_ASSERT(0);
							//return 1;
						}
					}
				}
				else if (*(curmqomat->GetTex())) {
					WCHAR wtex[256] = { 0L };
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, curmqomat->GetTex(), 256, wtex, 256);
					swprintf_s(srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), wtex);
					swprintf_s(dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex);

					int chksame = wcscmp(curmodel->GetDirName(), charafolder);
					if (chksame != 0) {
						bcancel = FALSE;
						bret = CopyFileEx(srcpath, dstpath, NULL, NULL, &bcancel, 0);
						if (bret == 0) {
							_ASSERT(0);
							//return 1;
						}
					}
				}

				if (*(curmqomat->GetNormalTex()) && (curmqomat->GetNormalTexID() >= 0)) {
					WCHAR wtex[256] = { 0L };
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, curmqomat->GetNormalTex(), 256, wtex, 256);
					swprintf_s(srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), wtex);
					swprintf_s(dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex);

					int chksame = wcscmp(curmodel->GetDirName(), charafolder);
					if (chksame != 0) {
						bcancel = FALSE;
						bret = CopyFileEx(srcpath, dstpath, NULL, NULL, &bcancel, 0);
						if (bret == 0) {
							_ASSERT(0);
							//return 1;
						}
					}
				}

				if (*(curmqomat->GetMetalTex()) && (curmqomat->GetMetalTexID() >= 0)) {
					WCHAR wtex[256] = { 0L };
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, curmqomat->GetMetalTex(), 256, wtex, 256);
					swprintf_s(srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), wtex);
					swprintf_s(dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex);

					int chksame = wcscmp(curmodel->GetDirName(), charafolder);
					if (chksame != 0) {
						bcancel = FALSE;
						bret = CopyFileEx(srcpath, dstpath, NULL, NULL, &bcancel, 0);
						if (bret == 0) {
							_ASSERT(0);
							//return 1;
						}
					}
				}

				if (*(curmqomat->GetEmissiveTex()) && (curmqomat->GetEmissiveTexID() >= 0)) {
					WCHAR wtex[256] = { 0L };
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, curmqomat->GetEmissiveTex(), 256, wtex, 256);
					swprintf_s(srcpath, MAX_PATH, L"%s\\%s", curmodel->GetDirName(), wtex);
					swprintf_s(dstpath, MAX_PATH, L"%s\\%s", charafolder, wtex);

					int chksame = wcscmp(curmodel->GetDirName(), charafolder);
					if (chksame != 0) {
						bcancel = FALSE;
						bret = CopyFileEx(srcpath, dstpath, NULL, NULL, &bcancel, 0);
						if (bret == 0) {
							_ASSERT(0);
							//return 1;
						}
					}
				}

			}
		}
	}

	{
		//VRM1.0からembed textureオプションでコンバートしたfbxの場合
		//(fbxname).fbm/*.vrmファイルをコピーする
		CGltfLoader* gltfloader = curmodel->GetGltfLoader();
		if (gltfloader && gltfloader->GetVrmName() && (*(gltfloader->GetVrmName()) != 0L)) {
			WCHAR fbmname[MAX_PATH] = { 0L };
			wcscpy_s(fbmname, MAX_PATH, curmodel->GetFileName());
			int fbmnameleng = (int)wcslen(fbmname);
			if ((fbmnameleng >= 5) && (fbmnameleng < MAX_PATH)) {
				fbmname[fbmnameleng - 1] = TEXT('m');//.fbxを.fbmに加工
			}

			//(fbxname).fbmフォルダが無ければ作成
			WCHAR fbmfolder[MAX_PATH] = { 0L };
			swprintf_s(fbmfolder, MAX_PATH, L"%s\\%s\\%s", m_newdirname, curmodel->GetModelFolder(), fbmname);
			DWORD fattr2;
			fattr2 = GetFileAttributes(fbmfolder);
			if ((fattr2 == -1) || ((fattr2 & FILE_ATTRIBUTE_DIRECTORY) == 0)) {
				int bret2;
				bret2 = CreateDirectory(fbmfolder, NULL);
				if (bret2 == 0) {
					::MessageBox(NULL, L"ディレクトリの作成に失敗しました。\n書き込み禁止ディレクトリの可能性があります。\n保存場所を変えて再試行してみてください。", L"エラー", MB_OK);
					_ASSERT(0);
					return 1;
				}
			}

			BOOL bret;
			BOOL bcancel;
			WCHAR srcpath[MAX_PATH] = { 0L };
			WCHAR dstpath[MAX_PATH] = { 0L };

			wcscpy_s(srcpath, MAX_PATH, gltfloader->GetVrmPath());
			swprintf_s(dstpath, MAX_PATH, L"%s\\%s", fbmfolder, gltfloader->GetVrmName());

			int chksame = wcscmp(srcpath, dstpath);
			if (chksame != 0) {
				bcancel = FALSE;
				bret = CopyFileEx(srcpath, dstpath, NULL, NULL, &bcancel, 0);
				if (bret == 0) {
					_ASSERT(0);
					//return 1;
				}
			}
		}
	}


	{
		WCHAR wfilename[MAX_PATH] = { 0L };
		ZeroMemory(wfilename, sizeof(WCHAR) * 256);
		swprintf_s(wfilename, 256, L"%s.dig", curmodel->GetFileName());

		WCHAR pathname[MAX_PATH] = { 0L };
		swprintf_s(pathname, MAX_PATH, L"%s\\%s", charafolder, wfilename);

		CDispGroupFile digfile;
		CallF(digfile.WriteDispGroupFile(pathname, curmodel), return 1);
	}

	{
		WCHAR wfilename[MAX_PATH] = { 0L };
		ZeroMemory(wfilename, sizeof(WCHAR) * 256);
		swprintf_s(wfilename, 256, L"%s.moa", curmodel->GetFileName());

		WCHAR pathname[MAX_PATH] = { 0L };
		swprintf_s(pathname, MAX_PATH, L"%s\\%s", charafolder, wfilename);

		CMAFile moafile;
		CallF(moafile.SaveMAFile(pathname, curmodel, g_mainhwnd, 1), return 1);
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

int CChaFile::LoadChaFile(bool limitdegflag, WCHAR* strpath, 
	CFootRigDlg* srcfootrigdlg,
	CModel* (*srcfbxfunc)( bool callfromcha, bool dorefreshtl, int skipdefref, int inittimelineflag, std::vector<std::string> ikstopname, bool srcgrassflag),
	int (*srcReffunc)(), int (*srcImpFunc)(), int (*srcGcoFunc)(),
	int (*srcReMenu)( int selindex1, int callbymenu1 ), 
	int (*srcRgdMenu)( int selindex2, int callbymenu2 ), 
	int (*srcMorphMenu)( int selindex3 ), int (*srcImpMenu)( int selindex4 ),
	std::vector<CGrassElem*>& dstgrasselemvec)
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

	m_footrigdlg = srcfootrigdlg;

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

	bool getfov = false;
	int result = 0;
	float tempfovy = 45.0f * (float)PI / 180.0f;
	result = Read_Float(&m_xmliobuf, "<ProjFov>", "</ProjFov>", &tempfovy);
	if (result == 0) {
		//g_fovy = tempfovy * (float)PI / 180.0f;
		getfov = true;//ReadCharaより後でセット
	}

	bool getnear = false;
	float tempnear = 1.0f;
	result = Read_Float(&m_xmliobuf, "<ProjNear>", "</ProjNear>", &tempnear);
	if (result == 0) {
		//g_projnear = tempnear;
		getnear = true;//ReadCharaより後でセット
	}
	bool getfar = false;
	float tempfar = 3000.0f;
	result = Read_Float(&m_xmliobuf, "<ProjFar>", "</ProjFar>", &tempfar);
	if (result == 0) {
		//g_projfar = tempfar;
		getfar = true;//ReadCharaより後でセット
	}
	bool getpickdist = false;
	float temppickdist = (float)g_pickdistrate;
	result = Read_Float(&m_xmliobuf, "<PickDistRate>", "</PickDistRate>", &temppickdist);
	if (result == 0) {
		//g_projnear = tempnear;
		getpickdist = true;//ReadCharaより後でセット
	}


	bool geteditrate = false;
	float tempeditrate = (float)g_physicsmvrate;
	result = Read_Float(&m_xmliobuf, "<EditRate>", "</EditRate>", &tempeditrate);
	if (result == 0) {
		geteditrate = true;//ReadCharaより後でセット
	}
	bool getplayingspeed = false;
	float tempplayingspeed = (float)g_dspeed;
	result = Read_Float(&m_xmliobuf, "<PlayingSpeed>", "</PlayingSpeed>", &tempplayingspeed);
	if (result == 0) {
		getplayingspeed = true;//ReadCharaより後でセット
	}
	bool getlimitscale = false;
	float templimitscale = (float)g_physicalLimitScale;
	result = Read_Float(&m_xmliobuf, "<PhysicalLimitScale>", "</PhysicalLimitScale>", &templimitscale);
	if (result == 0) {
		getlimitscale = true;//ReadCharaより後でセット
	}
	bool getmovablerate = false;
	int tempmovablerate = g_physicalMovableRate;
	result = Read_Int(&m_xmliobuf, "<BtMovableRate>", "</BtMovableRate>", &tempmovablerate);//2024/05/02
	if (result == 0) {
		getmovablerate = true;//ReadCharaより後でセット
	}
	bool getveloscale = false;
	float tempveloscale = (float)g_physicalVeloScale;
	result = Read_Float(&m_xmliobuf, "<BtVScaleOnLimitEul>", "</BtVScaleOnLimitEul>", &tempveloscale);
	if (result == 0) {
		getveloscale = true;//ReadCharaより後でセット
	}


	bool getcameradist = false;
	float tempcameradist = (float)g_camdist;
	result = Read_Float(&m_xmliobuf, "<CameraDist>", "</CameraDist>", &tempcameradist);
	if (result == 0) {
		getcameradist = true;//ReadCharaより後でセット
	}
	bool getakscale = false;
	float tempakscale = (float)g_akscale;
	result = Read_Float(&m_xmliobuf, "<AKScale>", "</AKScale>", &tempakscale);
	if (result == 0) {
		getakscale = true;//ReadCharaより後でセット
	}

	//bool getboneaxis = false;//getしなかった場合にもtempboneaxisをセットする
	int tempboneaxis = -1;//BoneAxisタグが無かった場合には-1をセットしてAdditiveIK.cppのPostOpenChaFile()に処理を委ねる
	result = Read_Int(&m_xmliobuf, "<BoneAxis>", "</BoneAxis>", &tempboneaxis);
	//if (result == 0) {
	//	getboneaxis = true;//ReadCharaより後でセット
	//}


	float templod = 0.01f;
	result = Read_Float(&m_xmliobuf, "<2L_LOD0>", "</2L_LOD0>", &templod);
	if (result == 0) {
		g_lodrate2L[CHKINVIEW_LOD0] = templod;
	}
	result = Read_Float(&m_xmliobuf, "<2L_LOD1>", "</2L_LOD1>", &templod);
	if (result == 0) {
		g_lodrate2L[CHKINVIEW_LOD1] = templod;
	}

	result = Read_Float(&m_xmliobuf, "<3L_LOD0>", "</3L_LOD0>", &templod);
	if (result == 0) {
		g_lodrate3L[CHKINVIEW_LOD0] = templod;
	}
	result = Read_Float(&m_xmliobuf, "<3L_LOD1>", "</3L_LOD1>", &templod);
	if (result == 0) {
		g_lodrate3L[CHKINVIEW_LOD1] = templod;
	}
	result = Read_Float(&m_xmliobuf, "<3L_LOD2>", "</3L_LOD2>", &templod);
	if (result == 0) {
		g_lodrate3L[CHKINVIEW_LOD2] = templod;
	}

	result = Read_Float(&m_xmliobuf, "<4L_LOD0>", "</4L_LOD0>", &templod);
	if (result == 0) {
		g_lodrate4L[CHKINVIEW_LOD0] = templod;
	}
	result = Read_Float(&m_xmliobuf, "<4L_LOD1>", "</4L_LOD1>", &templod);
	if (result == 0) {
		g_lodrate4L[CHKINVIEW_LOD1] = templod;
	}
	result = Read_Float(&m_xmliobuf, "<4L_LOD2>", "</4L_LOD2>", &templod);
	if (result == 0) {
		g_lodrate4L[CHKINVIEW_LOD2] = templod;
	}
	result = Read_Float(&m_xmliobuf, "<4L_LOD2>", "</4L_LOD3>", &templod);
	if (result == 0) {
		g_lodrate4L[CHKINVIEW_LOD3] = templod;
	}


	m_xmliobuf.pos = 0;
	int characnt;
	for( characnt = 0; characnt < charanum; characnt++ ){
		XMLIOBUF charabuf;
		ZeroMemory( &charabuf, sizeof( XMLIOBUF ) );
		CallF( SetXmlIOBuf( &m_xmliobuf, "<Chara>", "</Chara>", &charabuf ), return 1 );
		CallF(ReadChara(limitdegflag, charanum, characnt, dstgrasselemvec, &charabuf), return 1 );
	}


	//ReadCharaより後でセット
	if (getfov) {
		g_fovy = tempfovy * (float)PI / 180.0f;
	}
	if (getnear) {
		g_projnear = tempnear;
	}
	if (getfar) {
		g_projfar = tempfar;
	}
	if (getpickdist) {
		g_pickdistrate = (double)temppickdist;
	}
	if (geteditrate) {
		g_physicsmvrate = tempeditrate;
	}
	if (getplayingspeed) {
		g_dspeed = (double)tempplayingspeed;
	}
	if (getlimitscale) {
		g_physicalLimitScale = (double)templimitscale;
	}
	if (getmovablerate) {
		g_physicalMovableRate = tempmovablerate;//2024/05/02
	}
	if (getveloscale) {
		g_physicalVeloScale = (double)tempveloscale;//2024/05/04
	}
	if (getcameradist) {
		g_camdist = tempcameradist;
	}
	if (getakscale) {
		g_akscale = tempakscale;
	}

	//BoneAxisタグが無かった場合にもtempboneaxis(-1)をセットしてAdditiveIK.cppのPostOpenChaFile()に処理を委ねる
	g_boneaxis = tempboneaxis;



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
int CChaFile::ReadChara(bool limitdegflag, int charanum, int characnt, 
	std::vector<CGrassElem*>& dstgrasselemvec, XMLIOBUF* xmlbuf)
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

	float rotx = 0.0f;
	float roty = 0.0f;
	float rotz = 0.0f;

	CallF( Read_Str( xmlbuf, "<ModelFolder>", "</ModelFolder>", modelfolder, MAX_PATH ), return 1 );
	CallF( Read_Str( xmlbuf, "<ModelFile>", "</ModelFile>", filename, MAX_PATH ), return 1 );

	CallF(Read_Float(xmlbuf, "<ModelMult>", "</ModelMult>", &modelmult), return 1);

	int grassflag = 0;
	std::vector<ChaMatrix> grassmatvec;
	Read_Int(xmlbuf, "<GrassFlag>", "</GrassFlag>", &grassflag);
	if (grassflag == 1) {
		ChaMatrix grassmat;
		grassmat.SetIdentity();
		int result0 = 0;
		while (result0 == 0) {
			XMLIOBUF grassmatbuf;
			ZeroMemory(&grassmatbuf, sizeof(XMLIOBUF));
			int delpatflag = 0;
			int errorno = SetXmlIOBuf(xmlbuf, "<GrassMat>", "</GrassMat>", &grassmatbuf, delpatflag);
			
			if (errorno == 0) {
				result0 = Read_Matrix(xmlbuf, "<GrassMat>", "</GrassMat>", &grassmat);
				if (result0 == 0) {
					grassmatvec.push_back(grassmat);
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
	}


	//modelposは必須ではない
	Read_Float(xmlbuf, "<ModelPositionX>", "</ModelPositionX>", &posx);
	Read_Float(xmlbuf, "<ModelPositionY>", "</ModelPositionY>", &posy);
	Read_Float(xmlbuf, "<ModelPositionZ>", "</ModelPositionZ>", &posz);

	//modelrotationは必須ではない
	Read_Float(xmlbuf, "<ModelRotationX>", "</ModelRotationX>", &rotx);
	Read_Float(xmlbuf, "<ModelRotationY>", "</ModelRotationY>", &roty);
	Read_Float(xmlbuf, "<ModelRotationZ>", "</ModelRotationZ>", &rotz);

	//DispRateは必須ではない
	ChaVector4 materialdisprate;
	materialdisprate.SetParams(1.0f, 1.0f, 1.0f, 1.0f);
	Read_Float(xmlbuf, "<DiffuseDispRate>", "</DiffuseDispRate>", &materialdisprate.x);
	Read_Float(xmlbuf, "<SpecularDispRate>", "</SpecularDispRate>", &materialdisprate.y);
	Read_Float(xmlbuf, "<EmissiveDispRate>", "</EmissiveDispRate>", &materialdisprate.z);
	Read_Float(xmlbuf, "<AmbientDispRate>", "</AmbientDispRate>", &materialdisprate.w);

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


	//2023/07/21
	std::vector<std::string> ikstopname;
	{
		XMLIOBUF custombuf;
		ZeroMemory(&custombuf, sizeof(XMLIOBUF));
		int errorno1 = SetXmlIOBuf(xmlbuf, "<CustomIKStopName>", "</CustomIKStopName>", &custombuf, 0);//このタグがある場合には　新しいファイルとみなす
		if (errorno1 == 0) {
			bool findikstopname = true;
			while (findikstopname) {
				XMLIOBUF stopikbuf;
				ZeroMemory(&stopikbuf, sizeof(XMLIOBUF));
				int delpatflag = 0;
				int errorno = SetXmlIOBuf(xmlbuf, "<IKStopName>", "</IKStopName>", &stopikbuf, delpatflag);
				if (errorno != 0) {
					findikstopname = false;
					break;
				}
				char stopname[256];
				ZeroMemory(stopname, sizeof(char) * 256);
				CallF(Read_Str(&stopikbuf, "<IKStopName>", "</IKStopName>", stopname, 256), return 1);
				//strncpy_s(stopname, 256, stopikbuf.buf + stopikbuf.pos, stopikbuf.bufleng - stopikbuf.pos);

				ikstopname.push_back(stopname);
			}
		}
		else {
			//古いファイルには　IKStopNameタグエントリーは無い　デフォルト設定する
			ikstopname.push_back("Shoulder");
			ikstopname.push_back("UpperLeg");
		}
	}


	//2023/09/25
	std::vector<std::string> latername;
	{
		bool findlater = true;
		while (findlater) {
			XMLIOBUF laterbuf;
			ZeroMemory(&laterbuf, sizeof(XMLIOBUF));
			int delpatflag = 0;
			int errorno = SetXmlIOBuf(xmlbuf, "<LaterTransparent>", "</LaterTransparent>", &laterbuf, delpatflag);
			if (errorno != 0) {
				findlater = false;
				break;
			}
			char strlatername[256];
			ZeroMemory(strlatername, sizeof(char) * 256);
			//strncpy_s(strlatername, 256, laterbuf.buf + laterbuf.pos, laterbuf.bufleng - laterbuf.pos);
			CallF(Read_Str(&laterbuf, "<LaterTransparent>", "</LaterTransparent>", strlatername, 256), return 1);

			latername.push_back(strlatername);
		}
	}



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
	bool callfromcha = true;
	newmodel = (this->m_FbxFunc)( callfromcha, (characnt == (charanum - 1)), skipdefref, inittimeline, ikstopname, (grassflag == 1) );
	
	if (!newmodel) {
		_ASSERT(0);
		return 1;
	}

	
	//newmodel->m_tmpmotspeed = m_motspeed;
	if (grassflag == 1) {
		CGrassElem* newgrasselem = new CGrassElem(newmodel);
		if (!newgrasselem) {
			_ASSERT(0);
			abort();
			return 1;
		}
		
		int grassnum = (int)grassmatvec.size();
		int grassindex;
		for (grassindex = 0; grassindex < grassnum; grassindex++) {
			newgrasselem->AddGrassMat(grassmatvec[grassindex]);
		}

		dstgrasselemvec.push_back(newgrasselem);
	}
	else {
		newmodel->SetGrassFlag(false);
	}

	newmodel->SetModelPosition(ChaVector3(posx, posy, posz));
	newmodel->SetModelRotation(ChaVector3(rotx, roty, rotz));
	newmodel->CalcModelWorldMatOnLoad(m_footrigdlg);
	newmodel->SetMaterialDispRate(materialdisprate);


	int laternamenum = (int)latername.size();
	int laterno;
	for (laterno = 0; laterno < laternamenum; laterno++) {
		newmodel->AddLaterTransparent(latername[laterno]);
	}
	newmodel->MakeLaterMaterial();

	{
		WCHAR stfilename[MAX_PATH] = { 0L };
		ZeroMemory(stfilename, sizeof(WCHAR) * MAX_PATH);
		swprintf_s(stfilename, 256, L"%s.stf", newmodel->GetFileName());

		WCHAR pathname[MAX_PATH] = { 0L };
		swprintf_s(pathname, MAX_PATH, L"%s\\%s\\%s", m_wloaddir, wmodelfolder, stfilename);

		CShaderTypeFile stfile;
		stfile.LoadShaderTypeFile(pathname, newmodel);
		newmodel->RemakeHSVToonTexture(nullptr);//2024/02/13
	}


	{
		WCHAR digfilename[MAX_PATH] = { 0L };
		ZeroMemory(digfilename, sizeof(WCHAR) * MAX_PATH);
		swprintf_s(digfilename, 256, L"%s.dig", newmodel->GetFileName());

		WCHAR pathname[MAX_PATH] = { 0L };
		swprintf_s(pathname, MAX_PATH, L"%s\\%s\\%s", m_wloaddir, wmodelfolder, digfilename);

		CDispGroupFile digfile;
		digfile.LoadDispGroupFile(pathname, newmodel);
	}

	{
		WCHAR moafilename[MAX_PATH] = { 0L };
		ZeroMemory(moafilename, sizeof(WCHAR) * MAX_PATH);
		swprintf_s(moafilename, 256, L"%s.moa", newmodel->GetFileName());

		WCHAR pathname[MAX_PATH] = { 0L };
		swprintf_s(pathname, MAX_PATH, L"%s\\%s\\%s", m_wloaddir, wmodelfolder, moafilename);

		CMAFile moafile;
		moafile.LoadMAFile(pathname, newmodel);
	}


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

	newmodel->CreateBtObject(limitdegflag, 1);//初回
	//newmodel->CalcBoneEul(-1);//


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