#include "stdafx.h"
//#include <stdafx.h>

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

#include <Model.h>
#include <polymesh3.h>
#include <polymesh4.h>
#include <ExtLine.h>

#include <GetMaterial.h>

#include <mqofile.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#include <Bone.h>
#include <mqoface.h>

#define DBGH
#include <dbg.h>

#include <DXUT.h>
#include <DXUTcamera.h>
#include <DXUTgui.h>
#include <DXUTsettingsdlg.h>
#include <SDKmisc.h>

#include <DispObj.h>
#include <MySprite.h>

#include <MotionPoint.h>
#include <quaternion.h>
#include <VecMath.h>

#include <Collision.h>
#include <EngName.h>

#include <RigidElem.h>

#include <string>

#include <fbxsdk.h>

#include <fbxsdk/scene/shading/fbxlayeredtexture.h>


#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"


#include <BopFile.h>
#include <BtObject.h>

#include <collision.h>
#include <EditRange.h>
#include <BoneProp.h>


using namespace OrgWinGUI;

static int s_alloccnt = 0;

extern int g_boneaxis;
extern int g_dbgloadcnt;
extern int g_pseudolocalflag;
extern int g_previewFlag;			// プレビューフラグ
extern WCHAR g_basedir[ MAX_PATH ];
extern ID3DXEffect*	g_pEffect;
extern D3DXHANDLE g_hm3x4Mat;
extern D3DXHANDLE g_hmWorld;
extern float g_impscale;
extern btScalar G_ACC; // 重力加速度 : BPWorld.cpp

extern float g_l_kval[3];
extern float g_a_kval[3];

extern float g_ikfirst;
extern float g_ikrate;
extern int g_slerpoffflag;
extern int g_absikflag;
extern int g_applyendflag;
extern int g_bonemarkflag;
extern float g_physicsmvrate;

extern ChaVector3 g_camEye;
extern ChaVector3 g_camtargetpos;

extern bool g_wmatDirectSetFlag;


int g_dbgflag = 0;


//////////
static FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, char** ppTextureName);
static int IsValidCluster( FbxCluster* pcluster );
static FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition = NULL );
static FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
static FbxAMatrix GetGeometry(FbxNode* pNode);

static int s_setrigidflag = 0;
static DWORD s_rigidflag = 0;

CModel::CModel()
{
	InitParams();
	s_alloccnt++;
	m_modelno = s_alloccnt;
}
CModel::~CModel()
{
	DestroyObjs();
}
int CModel::InitParams()
{
	m_initaxismatx = 0;
	m_loadedflag = false;
	m_createbtflag = false;
	m_oldaxis_atloading = 0;
	m_ikrotaxis = ChaVector3( 1.0f, 0.0f, 0.0f );
	m_texpool = D3DPOOL_DEFAULT;
	m_tmpmotspeed = 1.0f;

	m_curreindex = 0;
	m_rgdindex = 0;
	m_rgdmorphid = -1;

	strcpy_s( m_defaultrename, MAX_PATH, "default_ref.ref" );
	strcpy_s( m_defaultimpname, MAX_PATH, "default_imp.imp" );

	m_rigidbone.clear();
	//m_btg = -1.0f;
	m_fbxobj.clear();
	m_btWorld = 0;

	m_modelno = 0;
	m_pdev = 0;

	ZeroMemory( m_filename, sizeof( WCHAR ) * MAX_PATH );
	ZeroMemory( m_dirname, sizeof( WCHAR ) * MAX_PATH );
	ZeroMemory( m_modelfolder, sizeof( WCHAR ) * MAX_PATH );
	m_loadmult = 1.0f;

	ChaMatrixIdentity( &m_matWorld );
	ChaMatrixIdentity( &m_matVP );

	m_curmotinfo = 0;

	m_modeldisp = true;
	m_topbone = 0;
	//m_firstbone = 0;

	this->m_tlFunc = 0;

	m_psdk = 0;
	m_pimporter = 0;
	m_pscene = 0;

	m_btcnt = 0;
	m_topbt = 0;

	m_rigideleminfo.clear();
	m_impinfo.clear();

	InitUndoMotion( 0 );

	return 0;
}
int CModel::DestroyObjs()
{
	DestroyMaterial();
	DestroyObject();
	DestroyAncObj();
	DestroyAllMotionInfo();

	DestroyFBXSDK();

	DestroyBtObject();

	InitParams();

	return 0;
}

int CModel::DestroyFBXSDK()
{
	
	if( m_pimporter ){
		FbxArrayDelete(mAnimStackNameArray);
	}

//	if( m_pscene ){
//		m_pscene->Destroy();
//		m_pscene = 0;
//	}

//	if( m_pimporter ){
//		m_pimporter->Destroy();// インポータの削除
//		m_pimporter = 0;
//	}

	return 0;
}


int CModel::DestroyAllMotionInfo()
{
	map<int, MOTINFO*>::iterator itrmi;
	for( itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++ ){
		MOTINFO* miptr = itrmi->second;
		if( miptr ){
			free( miptr );
		}
	}
	m_motinfo.clear();

	m_curmotinfo = 0;

	return 0;
}

int CModel::DestroyMaterial()
{

	map<int, CMQOMaterial*>::iterator itr;
	for( itr = m_material.begin(); itr != m_material.end(); itr++ ){
		CMQOMaterial* delmat = itr->second;
		if( delmat ){
			delete delmat;
		}
	}
	m_material.clear();

	return 0;
}
int CModel::DestroyObject()
{

	map<int, CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* delobj = itr->second;
		if( delobj ){
			delete delobj;
		}
	}
	m_object.clear();

	return 0;
}

int CModel::DestroyAncObj()
{
	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* delbone = itrbone->second;
		if( delbone ){
			delete delbone;
		}
	}
	m_bonelist.clear();

	m_topbone = 0;

	m_objectname.clear();
	m_bonename.clear();

	return 0;
}

int CModel::LoadMQO( ID3D10Device* pdev, WCHAR* wfile, WCHAR* modelfolder, float srcmult, int ismedia, int texpool )
{
	if( modelfolder ){
		wcscpy_s( m_modelfolder, MAX_PATH, modelfolder );
	}else{
		ZeroMemory( m_modelfolder, sizeof( WCHAR ) * MAX_PATH );
	}
	m_loadmult = srcmult;
	m_pdev = pdev;
	m_texpool = texpool;

	WCHAR fullname[MAX_PATH];

	if( ismedia == 1 ){
		WCHAR str[MAX_PATH];
		HRESULT hr;
		hr = DXUTFindDXSDKMediaFileCch( str, MAX_PATH, wfile );
		if( hr != S_OK ){
			::MessageBoxA( NULL, "media not found error !!!", "load error", MB_OK );
			_ASSERT( 0 );
			return 1;
		}
		wcscpy_s( fullname, MAX_PATH, g_basedir );
		wcscat_s( fullname, MAX_PATH, str );

	}else{
		wcscpy_s( fullname, MAX_PATH, wfile );
	}

    WCHAR* strLastSlash = NULL;
    strLastSlash = wcsrchr( fullname, TEXT( '\\' ) );
    if( strLastSlash )
    {
		*strLastSlash = 0;
		wcscpy_s( m_dirname, MAX_PATH, fullname );
		wcscpy_s( m_filename, MAX_PATH, strLastSlash + 1 );
	}else{
		ZeroMemory( m_dirname, sizeof( WCHAR ) * MAX_PATH );
		wcscpy_s( m_filename, MAX_PATH, fullname );
	}

	SetCurrentDirectory( m_dirname );


	DestroyMaterial();
	DestroyObject();

	CMQOFile mqofile;
	ChaVector3 vop( 0.0f, 0.0f, 0.0f );
	ChaVector3 vor( 0.0f, 0.0f, 0.0f );
	CallF( mqofile.LoadMQOFile( m_pdev, srcmult, m_filename, vop, vor, this ), return 1 );

	CallF( MakeObjectName(), return 1 );


	CallF( CreateMaterialTexture(), return 1 );

	SetMaterialName();

	return 0;
}


int CModel::LoadFBX(int skipdefref, ID3D10Device* pdev, WCHAR* wfile, WCHAR* modelfolder, float srcmult, FbxManager* psdk, FbxImporter** ppimporter, FbxScene** ppscene, int forcenewaxisflag)
{

	//DestroyFBXSDK();

	m_psdk = psdk;
	*ppimporter = 0;
	*ppscene = 0;

	if( modelfolder ){
		wcscpy_s( m_modelfolder, MAX_PATH, modelfolder );
	}else{
		ZeroMemory( m_modelfolder, sizeof( WCHAR ) * MAX_PATH );
	}
	m_loadmult = srcmult;
	m_pdev = pdev;

	WCHAR fullname[MAX_PATH];

	wcscpy_s( fullname, MAX_PATH, wfile );

    WCHAR* strLastSlash = NULL;
    strLastSlash = wcsrchr( fullname, TEXT( '\\' ) );
    if( strLastSlash )
    {
		*strLastSlash = 0;
		wcscpy_s( m_dirname, MAX_PATH, fullname );
		wcscpy_s( m_filename, MAX_PATH, strLastSlash + 1 );
	}else{
		ZeroMemory( m_dirname, sizeof( WCHAR ) * MAX_PATH );
		wcscpy_s( m_filename, MAX_PATH, fullname );
	}


	DestroyMaterial();
	DestroyObject();

	char utf8path[MAX_PATH] = {0};
    // Unicode 文字コードを第一引数で指定した文字コードに変換する
    ::WideCharToMultiByte( CP_UTF8, 0, wfile, -1, utf8path, MAX_PATH, NULL, NULL );	

	FbxScene* pScene = 0;
	FbxImporter* pImporter = 0;

	char scenename[256];
	sprintf_s(scenename, "scene_%d", s_alloccnt);
    pScene = FbxScene::Create(m_psdk, scenename);

    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    bool lStatus;
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	char importername[256];
	sprintf_s(importername, "importer_%d", s_alloccnt);
	pImporter = FbxImporter::Create(m_psdk, importername);

    const bool lImportStatus = pImporter->Initialize(utf8path, -1, m_psdk->GetIOSettings());
    pImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
    if( !lImportStatus )
    {
		_ASSERT( 0 );
		return 1;
	}
	//_ASSERT(0);

	if (pImporter->IsFBX())
    {
        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL,        true);
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE,         true);
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_LINK,            true);
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE,           true);
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO,            true);
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION,       true);
        (*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = pImporter->Import(pScene);
    if(lStatus == false )
    {
		_ASSERT( 0 );
		return 1;
	}

	m_oldaxis_atloading = 0;
	if (forcenewaxisflag == 0){
		FbxDocumentInfo* sceneinfo = pScene->GetSceneInfo();
		if (sceneinfo){
			FbxString oldauther = "OpenRDB user";
			if (sceneinfo->mAuthor == oldauther){
				_ASSERT(0);
				FbxString oldrevision = "rev. 1.0";
				if (sceneinfo->mRevision == oldrevision){
					m_oldaxis_atloading = 1;//!!!!!!!!!!!!!!!!!!!!
				}
			}
		}
	}

//	CallF( InitFBXManager( &pSdkManager, &pImporter, &pScene, utf8path ), return 1 );

	m_bone2node.clear();
	FbxNode *pRootNode = pScene->GetRootNode();

	m_topbone = 0;

	CreateFBXBoneReq( pScene, pRootNode, 0 );
	if ((int)m_bonelist.size() <= 1){
		_ASSERT( 0 );
		delete (CBone*)(m_bonelist.begin()->second);
		m_bonelist.clear();
		m_topbone = 0;
		_ASSERT(0);
	}
	CBone* chkbone = m_bonelist[0];
	if( !chkbone ){
		CBone* dummybone = new CBone( this );
		_ASSERT( dummybone );
		if (dummybone){
			dummybone->SetName("DummyBone");
			m_bonelist[0] = dummybone;
			dummybone->LoadCapsuleShape(m_pdev);

			//m_topbone = dummybone;
		}
		_ASSERT(0);
	}

_ASSERT(m_bonelist[0]);

	CreateFBXMeshReq( pRootNode );

	DbgOut(L"fbx bonenum %d\r\n", (int)m_bonelist.size());
_ASSERT(m_bonelist[0]);


	ChaMatrix offsetmat;
	ChaMatrixIdentity( &offsetmat );
	offsetmat._11 = srcmult;
	offsetmat._22 = srcmult;
	offsetmat._33 = srcmult;
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			CallF( curobj->MultMat( offsetmat ), return 1 );
			CallF( curobj->MultVertex(), return 1; );
		}
	}

_ASSERT(m_bonelist[0]);

	//m_ktime0.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
//	m_ktime0.SetSecondDouble( 1.0 / 300.0 );
//	m_ktime0.SetSecondDouble( 1.0 / 30.0 );


	CallF( MakePolyMesh4(), return 1 );
	CallF( MakeObjectName(), return 1 );
	CallF( CreateMaterialTexture(), return 1 );
	if( m_topbone ){
		CallF( CreateFBXSkinReq( pRootNode ), return 1 );
	}

	SetMaterialName();

_ASSERT(m_bonelist[0]);

	map<int,CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if (curbone){
			curbone->SetBtKinFlag(1);
		}
	}


	map<int,CMQOObject*>::iterator itr2;
	for( itr2 = m_object.begin(); itr2 != m_object.end(); itr2++ ){
		CMQOObject* curobj = itr2->second;
		if( curobj ){
			char* findnd = strstr( (char*)curobj->GetName(), "_ND" );
			if( findnd ){
				curobj->SetDispFlag( 0 );
			}
		}
	}

	m_rigideleminfo.clear();
	m_impinfo.clear();

	if( skipdefref == 0 ){
		REINFO reinfo;
		ZeroMemory( &reinfo, sizeof( REINFO ) );
		strcpy_s( reinfo.filename, MAX_PATH, m_defaultrename );
		reinfo.btgscale = 9.07;
		m_rigideleminfo.push_back( reinfo );
		m_impinfo.push_back( m_defaultimpname );

		if( m_topbone ){
			CreateRigidElemReq( m_topbone, 1, m_defaultrename, 1, m_defaultimpname );
		}

		SetCurrentRigidElem( 0 );
		m_curreindex = 0;
		m_curimpindex = 0;
	}


	*ppimporter = pImporter;
	*ppscene = pScene;

	m_pimporter = pImporter;
	m_pscene = pScene;


	return 0;
}

int CModel::LoadFBXAnim( FbxManager* psdk, FbxImporter* pimporter, FbxScene* pscene, int (*tlfunc)( int srcmotid ) )
{
	if( !psdk || !pimporter || !pscene ){
		_ASSERT( 0 );
		return 0;
	}

	if( !m_topbone ){
		return 0;
	}

	this->m_tlFunc = tlfunc;

	FbxNode *pRootNode = pscene->GetRootNode();
	CallF( CreateFBXAnim( pscene, pRootNode ), return 1 );

	/*
	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if (curbone){
			curbone->CalcAxisMat(1, 0.0f);
		}
	}
	*/
	//CalcBtAxismatReq(m_topbone, 1);


	return 0;
}

int CModel::CreateMaterialTexture()
{

	map<int,CMQOMaterial*>::iterator itr;
	for( itr = m_material.begin(); itr != m_material.end(); itr++ ){
		CMQOMaterial* curmat = itr->second;
		CallF( curmat->CreateTexture( m_dirname, m_texpool ), return 1 );
	}


	map<int,CMQOObject*>::iterator itrobj;
	for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		if( curobj ){
			map<int,CMQOMaterial*>::iterator itr;
			for( itr = curobj->GetMaterialBegin(); itr != curobj->GetMaterialEnd(); itr++ ){
				CMQOMaterial* curmat = itr->second;
				CallF( curmat->CreateTexture( m_dirname, m_texpool ), return 1 );
			}
		}
	}

	return 0;
}

int CModel::OnRender( ID3D10Device* pdev, int lightflag, ChaVector4 diffusemult, int btflag )
{
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj && curobj->GetDispFlag() ){
			if( curobj->GetDispObj() ){
				CallF( SetShaderConst( curobj, btflag ), return 1 );

				CMQOMaterial* rmaterial = 0;
				if( curobj->GetPm3() ){
					//g_pEffect->SetMatrix( g_hmWorld, &m_matWorld );
					CallF( curobj->GetDispObj()->RenderNormalPM3( lightflag, diffusemult ), return 1 );
				}
				else if (curobj->GetPm4()){
					rmaterial = curobj->GetMaterialBegin()->second;
					CallF( curobj->GetDispObj()->RenderNormal( rmaterial, lightflag, diffusemult ), return 1 );
				}else{
					_ASSERT( 0 );
				}
			}
			if( curobj->GetDispLine() ){
				CallF( curobj->GetDispLine()->RenderLine( diffusemult ), return 1 );
			}
		}
	}

	return 0;
}

int CModel::GetModelBound( MODELBOUND* dstb )
{
	MODELBOUND mb;
	MODELBOUND addmb;
	ZeroMemory( &mb, sizeof( MODELBOUND ) );

	int calcflag = 0;
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj->GetPm3() ){
			curobj->GetPm3()->CalcBound();
			if( calcflag == 0 ){
				mb = curobj->GetPm3()->GetBound();
			}else{
				addmb = curobj->GetPm3()->GetBound();
				AddModelBound( &mb, &addmb );
			}
			calcflag++;
		}
		if( curobj->GetPm4() ){
			curobj->GetPm4()->CalcBound();
			if( calcflag == 0 ){
				mb = curobj->GetPm4()->GetBound();
			}else{
				addmb = curobj->GetPm4()->GetBound();
				AddModelBound( &mb, &addmb );
			}
			calcflag++;
		}
		if( curobj->GetExtLine() ){
			curobj->GetExtLine()->CalcBound();
			if( calcflag == 0 ){
				mb = curobj->GetExtLine()->m_bound;
			}else{
				addmb = curobj->GetExtLine()->m_bound;
				AddModelBound( &mb, &addmb );
			}
			calcflag++;
		}
	}

	*dstb = mb;

	return 0;
}

int CModel::AddModelBound( MODELBOUND* mb, MODELBOUND* addmb )
{
	ChaVector3 newmin = mb->min;
	ChaVector3 newmax = mb->max;

	if( newmin.x > addmb->min.x ){
		newmin.x = addmb->min.x;
	}
	if( newmin.y > addmb->min.y ){
		newmin.y = addmb->min.y;
	}
	if( newmin.z > addmb->min.z ){
		newmin.z = addmb->min.z;
	}

	if( newmax.x < addmb->max.x ){
		newmax.x = addmb->max.x;
	}
	if( newmax.y < addmb->max.y ){
		newmax.y = addmb->max.y;
	}
	if( newmax.z < addmb->max.z ){
		newmax.z = addmb->max.z;
	}

	mb->center = ( newmin + newmax ) * 0.5f;
	mb->min = newmin;
	mb->max = newmax;

	ChaVector3 diff;
	diff = mb->center - newmin;
	mb->r = ChaVector3Length( &diff );

	return 0;
}



int CModel::SetShapeNoReq( CMQOFace** ppface, int facenum, int searchp, int shapeno, int* setfacenum )
{

	int fno;
	CMQOFace* findface[200];
	ZeroMemory( findface, sizeof( CMQOFace* ) * 200 );
	int findnum = 0;

	for( fno = 0; fno < facenum; fno++ ){
		CMQOFace* curface = *( ppface + fno );
		if( curface->GetShapeNo() != -1 ){
			continue;
		}

		int chki;
		for( chki = 0; chki < curface->GetPointNum(); chki++ ){
			if( searchp == curface->GetIndex( chki ) ){
				if( findnum >= 200 ){
					_ASSERT( 0 );
					return 1;
				}
				curface->SetShapeNo( shapeno );
				findface[findnum] = curface;
				findnum++;
				break;
			}
		}
	}

	if( findnum > 0 ){
		(*setfacenum) += findnum;

		int findno;
		for( findno = 0; findno < findnum; findno++ ){
			CMQOFace* fface = findface[ findno ];
			int i;
			for( i = 0; i < fface->GetPointNum(); i++ ){
				int newsearch = fface->GetIndex( i );
				if( newsearch != searchp ){
					SetShapeNoReq( ppface, facenum, newsearch, shapeno, setfacenum );
				}
			}
		}
	}
	return 0;
}

int CModel::SetFaceOfShape( CMQOFace** ppface, int facenum, int shapeno, CMQOFace** ppface2, int setfacenum )
{
	int setno = 0;
	int fno;
	for( fno = 0; fno < facenum; fno++ ){
		CMQOFace* curface = *( ppface + fno );
		if( curface->GetShapeNo() == shapeno ){
			if( setno >= setfacenum ){
				_ASSERT( 0 );
				return 1;
			}
			*( ppface2 + setno ) = curface;
			setno++;
		}
	}

	_ASSERT( setno == setfacenum );

	return 0;
}

int CModel::MakeObjectName()
{
	map<int, CMQOObject*>::iterator itrobj;
	for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		if( curobj ){
			char* nameptr = (char*)curobj->GetName();
			int sdefcmp, bdefcmp;
			sdefcmp = strncmp( nameptr, "sdef:", 5 );
			bdefcmp = strncmp( nameptr, "bdef:", 5 );
			if( (sdefcmp != 0) && (bdefcmp != 0) ){
				int leng = (int)strlen( nameptr );
				string firstname( nameptr, nameptr + leng );
				m_objectname[ firstname ] = curobj;
				curobj->SetDispName( firstname );
			}else{
				char* startptr = nameptr + 5;
				int leng = (int)strlen( startptr );
				string firstname( startptr, startptr + leng );
				m_objectname[ firstname ] = curobj;
				curobj->SetDispName( firstname );
			}
		}
	}

	return 0;
}

int CModel::DbgDump()
{
	DbgOut( L"######start DbgDump %s\r\n", GetFileName() );

	DbgOut( L"Dump Bone And InfScope\r\n" );

	if( m_topbone ){
		DbgDumpBoneReq( m_topbone, 0 );
	}

	DbgOut( L"######end DbgDump\r\n" );


	return 0;
}

int CModel::DbgDumpBoneReq( CBone* boneptr, int broflag )
{
	char mes[1024];
	WCHAR wmes[1024];

	if( boneptr->GetParent() ){
		sprintf_s( mes, 1024, "\tboneno %d, bonename %s - parent %s\r\n", boneptr->GetBoneName(), boneptr->GetBoneName(), boneptr->GetParent()->GetBoneName() );
	}else{
		sprintf_s( mes, 1024, "\tboneno %d, bonename %s - parent NONE\r\n", boneptr->GetBoneNo(), boneptr->GetBoneName() );
	}
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, mes, 1024, wmes, 1024 );
	DbgOut( wmes );

	DbgOut( L"\t\tbonepos (%f, %f, %f), (%f, %f, %f)\r\n", 
		boneptr->GetJointWPos().x, boneptr->GetJointWPos().y, boneptr->GetJointWPos().z,
		boneptr->GetJointFPos().x, boneptr->GetJointFPos().y, boneptr->GetJointFPos().z );

	
	//DbgOut( L"\t\tinfscopenum %d\r\n", boneptr->m_isnum );
	//int isno;
	//for( isno = 0; isno < boneptr->m_isnum; isno++ ){
	//	CInfScope* curis = boneptr->m_isarray[ isno ];
	//	CBone* infbone = 0;
	//	if( curis->m_applyboneno >= 0 ){
	//		infbone = m_bonelist[ curis->m_applyboneno ];
	//		sprintf_s( mes, 1024, "\t\tInfScope %d, validflag %d, facenum %d, applybone %s\r\n", 
	//			isno, curis->m_validflag, curis->m_facenum, infbone->m_bonename );
	//		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, mes, 1024, wmes, 1024 );
	//		DbgOut( wmes );
	//	}else{
	//		DbgOut( L"\t\tInfScope %d, validflag %d, facenum %d, applybone is none\r\n", 
	//			isno, curis->m_validflag, curis->m_facenum );
	//	}
	//
	//	if( curis->m_targetobj ){
	//		sprintf_s( mes, 1024, "\t\t\ttargetobj %s\r\n", curis->m_targetobj->m_name );
	//		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, mes, 1024, wmes, 1024 );
	//		DbgOut( wmes );
	//	}else{
	//		DbgOut( L"\t\t\ttargetobj is none\r\n" );
	//	}
	//}

//////////
	if( boneptr->GetChild() ){
		DbgDumpBoneReq( boneptr->GetChild(), 1 );
	}
	if( (broflag == 1) && boneptr->GetBrother() ){
		DbgDumpBoneReq( boneptr->GetBrother(), 1 );
	}



	return 0;
}

int CModel::MakePolyMesh3()
{
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			CallF( curobj->MakePolymesh3( m_pdev, m_material ), return 1 );
		}
	}

	return 0;
}
int CModel::MakePolyMesh4()
{
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			CallF( curobj->MakePolymesh4( m_pdev ), return 1 );
		}
	}

	return 0;
}

int CModel::MakeExtLine()
{
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			CallF( curobj->MakeExtLine(), return 1 );
		}
	}

	return 0;
}

int CModel::MakeDispObj()
{
	int hasbone;
	if( m_bonelist.empty() ){
		hasbone = 0;
	}else{
		hasbone = 1;
	}

	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			CallF( curobj->MakeDispObj( m_pdev, m_material, hasbone ), return 1 );
		}
	}

	return 0;
}

int CModel::Motion2Bt( int firstflag, double nextframe, ChaMatrix* mW, ChaMatrix* mVP, int selectboneno )
{
	UpdateMatrix( mW, mVP );


	if (!m_topbt){
		return 0;
	}
	if (!m_btWorld){
		return 0;
	}


	if( m_topbt ){
		if (g_previewFlag != 5){
			SetBtKinFlagReq(m_topbt, 0);
		}
		else{
			SetRagdollKinFlagReq(m_topbt, selectboneno);
		}
	}

	if (firstflag == 1){
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
			CBone* boneptr = itrbone->second;
			if (boneptr){
				boneptr->SetStartMat2(boneptr->GetCurMp().GetWorldMat());
			}
		}

	}

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
		CBone* boneptr = itrbone->second;
		if (boneptr->GetParent()){
			CRigidElem* curre = boneptr->GetParent()->GetRigidElem(boneptr);
			if (curre){
				boneptr->GetParent()->CalcRigidElemParams(boneptr, firstflag);
			}
		}

		/*
		if (boneptr){
			std::map<CBone*, CRigidElem*>::iterator itrtmpmap;
			for (itrtmpmap = boneptr->GetRigidElemMapBegin(); itrtmpmap != boneptr->GetRigidElemMapEnd(); itrtmpmap++){
				CRigidElem* curre = itrtmpmap->second;
				if (curre){
					CBone* chilbone = itrtmpmap->first;
					_ASSERT(chilbone);
					if (chilbone){
						boneptr->CalcRigidElemParams(chilbone, firstflag);
					}
				}
			}
		}
		*/
	}


	Motion2BtReq(m_topbt);

	/*
	//map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
		CBone* curbone = itrbone->second;
		if (curbone){
			if ((curbone->GetBtKinFlag() == 0) && curbone->GetParent() && (curbone->GetParent()->GetBtKinFlag() == 1)){
				map<CBone*, CBtObject*>::iterator itrbto;
				for (itrbto = curbone->GetBtObjectMapBegin(); itrbto != curbone->GetBtObjectMapEnd(); itrbto++){
					CBtObject* curbto = itrbto->second;
					if (curbto){
						CBtObject* parbto = itrbto->second->GetParBt();
						if (parbto){
							int constraintnum = parbto->GetConstraintSize();
							int cno;
							for (cno = 0; cno < constraintnum; cno++){
								CONSTRAINTELEM ce = parbto->GetConstraintElem(cno);
								if (ce.centerbone == curbone){
									btGeneric6DofSpringConstraint* dofC = ce.constraint;
									if (dofC){
										int dofid;
										for (dofid = 0; dofid < 3; dofid++){
											dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
											//dofC->enableSpring(dofid, true);
										}
										for (dofid = 3; dofid < 6; dofid++){
											//dofC->enableSpring(dofid, true);
											dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
										}
										//dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
										//dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));

										//ChaVector3 bonepos = parbto->GetParBone()->GetBtChilPos();
										//ChaVector3 bonepos = curbone->GetParent()->GetBtChilPos();
										ChaVector3 bonepos = curbone->GetBtParPos();

										dofC->setLinearLowerLimit(btVector3(bonepos.x, bonepos.y, bonepos.z));
										dofC->setLinearUpperLimit(btVector3(bonepos.x, bonepos.y, bonepos.z));
									}
								}
							}
						}
					}
				}
			}
		}
	}
	*/

	//if (g_previewFlag == 5){
	//	if (m_topbt){
	//		SetBtEquilibriumPointReq(m_topbt);
	//	}
	//}


	return 0;
}

void CModel::Motion2BtReq( CBtObject* srcbto )
{
	//if( srcbto->GetBone() && (srcbto->GetBone()->GetBtKinFlag() == 1) ){
	if (srcbto->GetBone()){
		srcbto->Motion2Bt(this);
	}

	int chilnum = srcbto->GetChilBtSize();
	int chilno;
	for( chilno = 0; chilno < chilnum; chilno++ ){
		Motion2BtReq( srcbto->GetChilBt( chilno ) );
	}
}

int CModel::UpdateMatrix( ChaMatrix* wmat, ChaMatrix* vpmat )
{
	m_matWorld = *wmat;
	m_matVP = *vpmat;

	if( !m_curmotinfo ){
		return 0;//!!!!!!!!!!!!
	}

	int curmotid = m_curmotinfo->motid;
	double curframe = m_curmotinfo->curframe;

	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone ){
			curbone->UpdateMatrix( curmotid, curframe, wmat, vpmat );
		}
	}

	/*
	// for morph anim

	//groundのUpdateMatrixでエラー


	int chkcnt = 0;
	int motionorder = -1;
	map<int, MOTINFO*>::iterator itrmi;
	for( itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++ ){
		MOTINFO* chkmi = itrmi->second;
		if( chkmi ){
			if( chkmi->motid == m_curmotinfo->motid ){
				motionorder = chkcnt;
				break;
			}
		}
		chkcnt++;
	}
	if( motionorder < 0 ){
		_ASSERT( 0 );
		return 1;
	}


	//読み込み時にアニメがなければ以下はスキップ
	const int lAnimStackCount = mAnimStackNameArray.GetCount();
	if (lAnimStackCount <= 0){
		//_ASSERT(0);
		return 0;
	}

	FbxAnimStack * lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[motionorder]->Buffer());
	if (lCurrentAnimationStack == NULL){
		_ASSERT( 0 );
		return 1;
	}
	FbxAnimLayer * mCurrentAnimLayer;
	mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
   

	FbxTime lTime;
	lTime.SetSecondDouble( m_curmotinfo->curframe / 30.0 );
	//lTime.SetSecondDouble( m_curmotinfo->curframe / 300.0 );

	map<int, CMQOObject*>::iterator itrobj;
	for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		_ASSERT( curobj );
		if( !(curobj->EmptyFindShape()) ){
			GetShapeWeight( m_fbxobj[curobj].node, m_fbxobj[curobj].mesh, lTime, mCurrentAnimLayer, curobj );

			CallF( curobj->UpdateMorphBuffer(), return 1 );
		}
	}
	*/

	return 0;
}

/***
int CModel::ComputeShapeDeformation(FbxNode* pNode, FbxMesh* pMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* curobj, char* takename )
{
    int lVertexCount = pMesh->GetControlPointsCount();
	if( lVertexCount != curobj->m_vertex ){
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( curobj->m_mpoint, curobj->m_pointbuf, sizeof( ChaVector3 ) * lVertexCount );

	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; lChannelIndex++)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if(lChannel)
			{
				// Get the percentage of influence of the shape.
				FbxAnimCurve* lFCurve;
				double lWeight = 0.0;
				lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
				if (lFCurve){
					lWeight = lFCurve->Evaluate(pTime);
				}else{
					continue;
				}

				if( lWeight == 0.0 ){
					continue;
				}

				//Find which shape should we use according to the weight.
				int lShapeCount = lChannel->GetTargetShapeCount();
				double* lFullWeights = lChannel->GetTargetShapeFullWeights();
				for(int lShapeIndex = 0; lShapeIndex<lShapeCount; lShapeIndex++)
				{
					FbxShape* lShape = NULL;
					lShape = lChannel->GetTargetShape(lShapeIndex);//lShapeIndex+1ではない！！！！！！！！！！！！！！！！
					if(lShape)
					{		
						FbxVector4* shapev = lShape->GetControlPoints();

						for (int j = 0; j < lVertexCount; j++)
						{
							// Add the influence of the shape vertex to the mesh vertex.
							ChaVector3 xv;
							ChaVector3 diffpoint;

							xv.x = (float)shapev[j][0];
							xv.y = (float)shapev[j][1];
							xv.z = (float)shapev[j][2];

							diffpoint = (xv - *(curobj->m_pointbuf + j)) * (float)lWeight * 0.01f;

							*(curobj->m_mpoint + j) += diffpoint;
						}						
					}
				}//For each target shape
			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

	return 0;
}
***/

int CModel::GetFBXShape( FbxMesh* pMesh, CMQOObject* curobj, FbxAnimLayer* panimlayer, double animleng, FbxTime starttime, FbxTime timestep )
{
	int lVertexCount = pMesh->GetControlPointsCount();
	if( lVertexCount != curobj->GetVertex() ){
		_ASSERT( 0 );
		return 1;
	}

	curobj->DestroyShapeObj();

	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; lChannelIndex++)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if(lChannel)
			{
				FbxTime curtime = starttime;
				double framecnt;
				for( framecnt = 0.0; framecnt < animleng; framecnt+=1.0 ){
					FbxAnimCurve* lFCurve;
					double lWeight = 0.0;
					lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, panimlayer);
					if (lFCurve){
						lWeight = lFCurve->Evaluate( curtime );
					}else{
						curtime += timestep;
						continue;
					}
					if( lWeight == 0.0 ){
						curtime += timestep;
						continue;
					}
						
					int lShapeIndex = 0;
					FbxShape* lShape = NULL;
					lShape = lChannel->GetTargetShape(lShapeIndex);//lShapeIndex+1ではない！！！！！！！！！！！！！！！！
					if(lShape)
					{		
						const char* nameptr = lChannel->GetName();
						int existshape = 0;
						existshape = curobj->ExistShape( (char*)nameptr );
						if( existshape == 0 ){

							curobj->AddShapeName( (char*)nameptr );

							FbxVector4* shapev = lShape->GetControlPoints();
							_ASSERT( shapev );
							for (int j = 0; j < lVertexCount; j++)
							{
								ChaVector3 xv;
								xv.x = (float)shapev[j][0];
								xv.y = (float)shapev[j][1];
								xv.z = (float)shapev[j][2];
								curobj->SetShapeVert( (char*)nameptr, j, xv );
							}						
						}
					}
					curtime += timestep;
				}
			}//If lChannel is validf
		}//For each blend shape channel
	}//For each blend shape deformer

	return 0;
}

// Deform the vertex array with the shapes contained in the mesh.
int CModel::GetShapeWeight(FbxNode* pNode, FbxMesh* pMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* curobj )
{
    int lVertexCount = pMesh->GetControlPointsCount();
	if( lVertexCount != curobj->GetVertex() ){
		_ASSERT( 0 );
		return 1;
	}

	curobj->InitShapeWeight();

	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; lChannelIndex++)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if(lChannel)
			{

				const char* nameptr = lChannel->GetName();
				// Get the percentage of influence of the shape.
				FbxAnimCurve* lFCurve;
				double lWeight = 0.0;
				lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
				if (lFCurve){
					lWeight = lFCurve->Evaluate(pTime);
				}else{
					continue;
				}

				if( lWeight == 0.0 ){
					continue;
				}

				//Find which shape should we use according to the weight.
				int lShapeCount = lChannel->GetTargetShapeCount();
				double* lFullWeights = lChannel->GetTargetShapeFullWeights();
				for(int lShapeIndex = 0; lShapeIndex < lShapeCount; lShapeIndex++)
				{
					FbxShape* lShape = NULL;
					lShape = lChannel->GetTargetShape(lShapeIndex);//lShapeIndex+1ではない！！！！！！！！！！！！！！！！
					if(lShape)
					{	
						curobj->SetShapeWeight( (char*)nameptr, (float)lWeight );
					}
				}//For each target shape
			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

	return 0;
}


int CModel::SetShaderConst( CMQOObject* srcobj, int btflag )
{
	if( !m_topbone ){
		return 0;//!!!!!!!!!!!
	}

	float set3x4[MAXCLUSTERNUM][12];
	ZeroMemory( set3x4, sizeof( float ) * 12 * MAXCLUSTERNUM );

	int setclcnt = 0;
	int clcnt;
	for( clcnt = 0; clcnt < (int)srcobj->GetClusterSize(); clcnt++ ){
		CBone* curbone = srcobj->GetCluster( clcnt );
		if( !curbone ){
			_ASSERT( 0 );
			return 1;
		}


		CMotionPoint tmpmp = curbone->GetCurMp();
		if( btflag == 0 ){

			set3x4[clcnt][0] = tmpmp.GetWorldMat()._11;
			set3x4[clcnt][1] = tmpmp.GetWorldMat()._12;
			set3x4[clcnt][2] = tmpmp.GetWorldMat()._13;

			set3x4[clcnt][3] = tmpmp.GetWorldMat()._21;
			set3x4[clcnt][4] = tmpmp.GetWorldMat()._22;
			set3x4[clcnt][5] = tmpmp.GetWorldMat()._23;

			set3x4[clcnt][6] = tmpmp.GetWorldMat()._31;
			set3x4[clcnt][7] = tmpmp.GetWorldMat()._32;
			set3x4[clcnt][8] = tmpmp.GetWorldMat()._33;

			set3x4[clcnt][9] = tmpmp.GetWorldMat()._41;
			set3x4[clcnt][10] = tmpmp.GetWorldMat()._42;
			set3x4[clcnt][11] = tmpmp.GetWorldMat()._43;
		}else{
			set3x4[clcnt][0] = curbone->GetBtMat()._11;
			set3x4[clcnt][1] = curbone->GetBtMat()._12;
			set3x4[clcnt][2] = curbone->GetBtMat()._13;

			set3x4[clcnt][3] = curbone->GetBtMat()._21;
			set3x4[clcnt][4] = curbone->GetBtMat()._22;
			set3x4[clcnt][5] = curbone->GetBtMat()._23;

			set3x4[clcnt][6] = curbone->GetBtMat()._31;
			set3x4[clcnt][7] = curbone->GetBtMat()._32;
			set3x4[clcnt][8] = curbone->GetBtMat()._33;

			set3x4[clcnt][9] = curbone->GetBtMat()._41;
			set3x4[clcnt][10] = curbone->GetBtMat()._42;
			set3x4[clcnt][11] = curbone->GetBtMat()._43;
		}
		setclcnt++;

	}

	if( setclcnt > 0 ){
		HRESULT hr;
		hr = g_pEffect->SetValue( g_hm3x4Mat, (void*)set3x4, sizeof( float ) * 12 * MAXCLUSTERNUM );
		if(FAILED(hr)){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CModel::FillTimeLine( OrgWinGUI::OWP_Timeline& timeline, map<int, int>& lineno2boneno, map<int, int>& boneno2lineno )
{
	lineno2boneno.clear();
	boneno2lineno.clear();

	if( m_bonelist.empty() ){
		return 0;
	}

	int lineno = 0;
	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		_ASSERT( curbone );

		if (curbone){
			int depth = curbone->CalcBoneDepth();

			//行を追加
			if (curbone->GetType() != FBXBONE_NULL){
				timeline.newLine(depth, 0, curbone->GetWBoneName());
			}
			else{
				timeline.newLine(depth, 1, curbone->GetWBoneName());
			}

			lineno2boneno[lineno] = curbone->GetBoneNo();
			boneno2lineno[curbone->GetBoneNo()] = lineno;
			lineno++;
		}
/***
		_ASSERT( m_curmotinfo );
		CMotionPoint* curmp = curbone->m_motionkey[ m_curmotinfo->motid ];
		while( curmp ){
			timeline.newKey( curbone->m_wbonename, curmp->m_frame, (void*)curmp );
			curmp = curmp->m_next;
		}
***/
	}

/***
	int lineno = 0;
	FillTimelineReq( timeline, m_topbone, &lineno, lineno2boneno, boneno2lineno, 0 );
***/

	if (m_topbone){
		//選択行を設定
		timeline.setCurrentLineName(m_topbone->GetWBoneName());
	}
	return 0;
}

void CModel::FillTimelineReq( OrgWinGUI::OWP_Timeline& timeline, CBone* curbone, int* linenoptr, 
	map<int, int>& lineno2boneno, map<int, int>& boneno2lineno, int broflag )
{
	if (!curbone){
		return;
	}

	int depth = curbone->CalcBoneDepth();

	//行を追加
	if( curbone->GetType() != FBXBONE_NULL ){
		timeline.newLine(depth, 0, curbone->GetWBoneName());
	}else{
		timeline.newLine(depth, 1, curbone->GetWBoneName());
	}

	lineno2boneno[ *linenoptr ] = curbone->GetBoneNo();
	boneno2lineno[ curbone->GetBoneNo() ] = *linenoptr;
	(*linenoptr)++;

/***
	_ASSERT( m_curmotinfo );
	CMotionPoint* curmp = curbone->m_motionkey[ m_curmotinfo->motid ];
	while( curmp ){
		timeline.newKey( curbone->m_wbonename, curmp->m_frame, (void*)curmp );
		curmp = curmp->m_next;
	}
***/

	if( curbone->GetChild() ){
		FillTimelineReq( timeline, curbone->GetChild(), linenoptr, lineno2boneno, boneno2lineno, 1 );
	}
	if( broflag && curbone->GetBrother() ){
		FillTimelineReq( timeline, curbone->GetBrother(), linenoptr, lineno2boneno, boneno2lineno, 1 );
	}
}

int CModel::AddMotion(char* srcname, WCHAR* wfilename, double srcleng, int* dstid)
{
	*dstid = -1;
	int leng = (int)strlen(srcname);

	int maxid = 0;
	map<int, MOTINFO*>::iterator itrmi;
	for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++){
		MOTINFO* chkmi = itrmi->second;
		if (chkmi){
			if (maxid < chkmi->motid){
				maxid = chkmi->motid;
			}
		}
	}
	int newid = maxid + 1;


	MOTINFO* newmi = (MOTINFO*)malloc(sizeof(MOTINFO));
	if (!newmi){
		_ASSERT(0);
		return 1;
	}
	ZeroMemory(newmi, sizeof(MOTINFO));

	strcpy_s(newmi->motname, 256, srcname);
	if (wfilename){
		wcscpy_s(newmi->wfilename, MAX_PATH, wfilename);
	}
	else{
		ZeroMemory(newmi->wfilename, sizeof(WCHAR)* MAX_PATH);
	}
	ZeroMemory(newmi->engmotname, sizeof(char)* 256);

	newmi->motid = newid;
	newmi->frameleng = srcleng;
	newmi->curframe = 0.0;
	newmi->speed = 1.0;
	newmi->loopflag = 1;

	m_motinfo[newid] = newmi;


	*dstid = newid;

	return 0;
}


int CModel::SetCurrentMotion( int srcmotid )
{
	m_curmotinfo = m_motinfo[ srcmotid ];
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 1;
	}else{
		return 0;
	}
}
int CModel::SetMotionFrame( double srcframe )
{
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 1;
	}

	m_curmotinfo->curframe = max( 0.0, min( (m_curmotinfo->frameleng - 1), srcframe ) );

	return 0;
}
int CModel::GetMotionFrame( double* dstframe )
{
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 1;
	}

	*dstframe = m_curmotinfo->curframe;

	return 0;
}
int CModel::SetMotionSpeed( double srcspeed )
{
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 1;
	}

	m_curmotinfo->speed = srcspeed;

	return 0;
}
int CModel::GetMotionSpeed( double* dstspeed )
{
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 1;
	}

	*dstspeed = m_curmotinfo->speed;
	return 0;
}

int CModel::DeleteMotion( int motid )
{
	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone ){
			CallF( curbone->DeleteMotion( motid ), return 1 );
		}
	}

	map<int, MOTINFO*>::iterator itrmi;
	itrmi = m_motinfo.find( motid );
	if( itrmi != m_motinfo.end() ){
		MOTINFO* delmi = itrmi->second;
		if( delmi ){
			delete delmi;
		}
		m_motinfo.erase( itrmi );
	}

	int undono;
	for( undono = 0; undono < UNDOMAX; undono++ ){
		if( m_undomotion[undono].GetSaveMotInfo().motid == motid ){
			m_undomotion[undono].SetValidFlag( 0 );
		}
	}


	return 0;
}

int CModel::GetSymBoneNo( int srcboneno, int* dstboneno, int* existptr )
{
	*existptr = 0;

	CBone* srcbone = m_bonelist[ srcboneno ];
	if( !srcbone ){
		*dstboneno = -1;
		return 0;
	}

	int findflag = 0;
	WCHAR findname[256];
	ZeroMemory( findname, sizeof( WCHAR ) * 256 );
	wcscpy_s( findname, 256, srcbone->GetWBoneName() );

	WCHAR* lpat = wcsstr( findname, L"_L_" );
	if( lpat ){
		*lpat = TEXT( '_' );
		*(lpat + 1) = TEXT( 'R' );
		*(lpat + 2) = TEXT( '_' );
		//wcsncat_s( findname, 256, L"_R_", 3 );
		findflag = 1;
	}else{
		WCHAR* rpat = wcsstr( findname, L"_R_" );
		if( rpat ){
			*rpat = TEXT( '_' );
			*(rpat + 1) = TEXT( 'L' );
			*(rpat + 2) = TEXT( '_' );
			//wcsncat_s( findname, 256, L"_L_", 3 );
			findflag = 1;
		}
	}

	int setflag = 0;
	//if( findflag == 0 ){
	//	*dstboneno = srcboneno;
	//	*existptr = 0;
	//}else{
	if (findflag != 0){
		CBone* dstbone = 0;
		map<int, CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* chkbone = itrbone->second;
			if( chkbone && (wcscmp( findname, chkbone->GetWBoneName() ) == 0) ){
				dstbone = chkbone;
				break;
			}
		}
		if( dstbone ){
			*dstboneno = dstbone->GetBoneNo();
			*existptr = 1;
			setflag = 1;
		}
		//else{
		//	*dstboneno = srcboneno;
		//	*existptr = 0;
		//}
	}

	if (setflag == 0){
		CBone* symposbone = GetSymPosBone(srcbone);
		if (symposbone){
			*dstboneno = symposbone->GetBoneNo();
			*existptr = 1;
		}
		else{
			*dstboneno = srcboneno;
			*existptr = 0;
		}
	}


	return 0;
}

CBone* CModel::GetSymPosBone(CBone* srcbone)
{
	if (!srcbone){
		return 0;
	}

	CBone* findbone = 0;
	ChaVector3 srcpos = srcbone->GetJointFPos();
	srcpos.x *= -1.0f;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	float mindist = FLT_MAX;
	float matchdist;

	//MODELBOUND mb;
	//GetModelBound(&mb);
	//matchdist = mb.r * 0.001f;
	matchdist = 0.30f;

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
		CBone* curbone = itrbone->second;
		if (curbone){
			ChaVector3 curpos = curbone->GetJointFPos();
			ChaVector3 diffpos = curpos - srcpos;
			float curdist = ChaVector3Length(&diffpos);
			if ((curdist <= mindist) && (curdist <= matchdist) && (curbone != srcbone)){
				//同一位置のボーンが存在する場合があるので、親もチェックする。
				CBone* srcparbone = srcbone->GetParent();
				CBone* curparbone = curbone->GetParent();
				if (srcparbone && curparbone){
					ChaVector3 srcparpos = srcparbone->GetJointFPos();
					srcparpos.x *= -1.0f;
					ChaVector3 curparpos = curparbone->GetJointFPos();
					ChaVector3 pardiffpos = srcparpos - curparpos;
					float pardist = ChaVector3Length(&pardiffpos);
					if (pardist <= matchdist)
					{
						findbone = curbone;
						mindist = curdist;
					}
				}
			}
		}
	}

	return findbone;
}


int CModel::PickBone( PICKINFO* pickinfo )
{
	pickinfo->pickobjno = -1;

	float fw, fh;
	fw = (float)pickinfo->winx / 2.0f;
	fh = (float)pickinfo->winy / 2.0f;

	int minno = -1;
	ChaVector3 cmpsc;
	ChaVector3 picksc = ChaVector3( 0.0f, 0.0f, 0.0f );
	ChaVector3 pickworld = ChaVector3( 0.0f, 0.0f, 0.0f );
	float cmpdist;
	float mindist = 0.0f;
	int firstflag = 1;


	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone ){
			cmpsc.x = ( 1.0f + curbone->GetChildScreen().x ) * fw;
			cmpsc.y = ( 1.0f - curbone->GetChildScreen().y ) * fh;
			cmpsc.z = curbone->GetChildScreen().z;

			if( (cmpsc.z >= 0.0f) && (cmpsc.z <= 1.0f) ){
				float mag;
				mag = ( (float)pickinfo->clickpos.x - cmpsc.x ) * ( (float)pickinfo->clickpos.x - cmpsc.x ) + 
					( (float)pickinfo->clickpos.y - cmpsc.y ) * ( (float)pickinfo->clickpos.y - cmpsc.y );
				if( mag != 0.0f ){
					cmpdist = sqrtf( mag );
				}else{
					cmpdist = 0.0f;
				}

				if( (firstflag || (cmpdist <= mindist)) && (cmpdist <= (float)pickinfo->pickrange ) ){
					minno = curbone->GetBoneNo();
					mindist = cmpdist;
					picksc = cmpsc;
					pickworld = curbone->GetChildWorld();
					firstflag = 0;
				}
			}
		}
	}

	pickinfo->pickobjno = minno;
	if( minno >= 0 ){
		pickinfo->objscreen = picksc;
		pickinfo->objworld = pickworld;
	}

	return 0;
}

void CModel::SetSelectFlagReq( CBone* boneptr, int broflag )
{
	boneptr->SetSelectFlag( 1 );

	if( boneptr->GetChild() ){
		SetSelectFlagReq( boneptr->GetChild(), 1 );
	}
	if( boneptr->GetBrother() && broflag ){
		SetSelectFlagReq( boneptr->GetBrother(), 1 );
	}
}


int CModel::CollisionNoBoneObj_Mouse( PICKINFO* pickinfo, char* objnameptr )
{
	//当たったら１、当たらなかったら０を返す。エラーも０を返す。

	CMQOObject* curobj = m_objectname[ objnameptr ];
	if( !curobj ){
		_ASSERT( 0 );
		return 0;
	}

	ChaVector3 startlocal, dirlocal;
	CalcMouseLocalRay( pickinfo, &startlocal, &dirlocal );

	int colli = curobj->CollisionLocal_Ray( startlocal, dirlocal );

	return colli;
}

int CModel::CalcMouseLocalRay( PICKINFO* pickinfo, ChaVector3* startptr, ChaVector3* dirptr )
{
	ChaVector3 startsc, endsc;
	float rayx, rayy;
	rayx = (float)pickinfo->clickpos.x / ((float)pickinfo->winx / 2.0f) - 1.0f;
	rayy = 1.0f - (float)pickinfo->clickpos.y / ((float)pickinfo->winy / 2.0f);

	startsc = ChaVector3( rayx, rayy, 0.0f );
	endsc = ChaVector3( rayx, rayy, 1.0f );
	
    ChaMatrix mWVP, invmWVP;
	mWVP = m_matWorld * m_matVP;
	ChaMatrixInverse( &invmWVP, NULL, &mWVP );

	ChaVector3 startlocal, endlocal;

	D3DVec3TransformCoord( &startlocal, &startsc, &invmWVP );
	D3DVec3TransformCoord( &endlocal, &endsc, &invmWVP );

	ChaVector3 dirlocal = endlocal - startlocal;
	ChaVector3Normalize( &dirlocal, &dirlocal );

	*startptr = startlocal;
	*dirptr = dirlocal;

	return 0;
}

CBone* CModel::GetCalcRootBone( CBone* firstbone, int maxlevel )
{
	int levelcnt = 0;
	CBone* retbone = firstbone;
	CBone* curbone = firstbone;
	while( curbone && ((maxlevel == 0) || (levelcnt <= maxlevel)) )
	{
		retbone = curbone;
		curbone = curbone->GetParent();
		levelcnt++;
	}

	return retbone;
}


int CModel::TransformBone( int winx, int winy, int srcboneno, ChaVector3* worldptr, ChaVector3* screenptr, ChaVector3* dispptr )
{					
	CBone* curbone;
	curbone = m_bonelist[ srcboneno ];
	if (curbone){
		*worldptr = curbone->GetChildWorld();
		ChaMatrix mWVP;
		ChaMatrix mW;
		if (g_previewFlag != 5){
			mW = curbone->GetCurMp().GetWorldMat();
		}
		else{
			CBone* parbone = curbone->GetParent();
			if (parbone){
				mW = parbone->GetBtMat();//endjointのbtmatがおかしい可能性があるため。
			}
			else{
				mW = curbone->GetBtMat();
			}
		}

		D3DVec3TransformCoord(worldptr, &curbone->GetJointFPos(), &mW);
		mWVP = mW * m_matVP;
		D3DVec3TransformCoord(screenptr, &curbone->GetJointFPos(), &mWVP);

		float fw, fh;
		fw = (float)winx / 2.0f;
		fh = (float)winy / 2.0f;
		dispptr->x = (1.0f + screenptr->x) * fw;
		dispptr->y = (1.0f - screenptr->y) * fh;
		dispptr->z = screenptr->z;
	}
	return 0;
}


int CModel::ChangeMotFrameLeng( int motid, double srcleng )
{
	MOTINFO* dstmi = m_motinfo[ motid ];
	if( dstmi ){
		double befleng = dstmi->frameleng;

		dstmi->frameleng = srcleng;

		if( befleng > srcleng ){
			map<int, CBone*>::iterator itrbone;
			for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
				CBone* curbone = itrbone->second;
				if( curbone ){
					curbone->DeleteMPOutOfRange( motid, srcleng - 1.0 );
				}
			}
		}
	}
	return 0;
}

int CModel::AdvanceTime( CEditRange srcrange, int previewflag, double difftime, double* nextframeptr, int* endflagptr, int srcmotid)
{
	*endflagptr = 0;

	int loopflag = 0;
	MOTINFO* curmotinfo;
	if( srcmotid >= 0 ){
		curmotinfo = m_motinfo[ srcmotid ];
		loopflag = 0;
	}else{
		curmotinfo = m_curmotinfo;
		loopflag = curmotinfo->loopflag;
	}

	if( !curmotinfo ){
		return 0;
	}


	double curspeed, curframe;
	curspeed = curmotinfo->speed;
	curframe = curmotinfo->curframe;

	double nextframe;
	double oneframe = 1.0 / 30.0;
	//double oneframe = 1.0 / 300.0;

	double rangestart, rangeend;
	rangestart = srcrange.GetStartFrame();
	rangeend = srcrange.GetEndFrame();
	if (rangestart == rangeend){
		rangestart = 0.0;
		rangeend = curmotinfo->frameleng - 1.0;
	}


	if( previewflag > 0 ){
		nextframe = curframe + difftime / oneframe * curspeed;
		if( nextframe > rangeend ){
			if( loopflag == 0 ){
				nextframe = rangeend;
				*endflagptr = 1;
			}else{
				nextframe = rangestart;
			}
		}
	}else{
		nextframe = curframe - difftime / oneframe * curspeed;
		if( nextframe < rangestart ){
			if( loopflag == 0 ){
				nextframe = rangestart;
				*endflagptr = 1;
			}else{
				nextframe = rangeend;
			}
		}
	}

	*nextframeptr = nextframe;

	return 0;
}


int CModel::MakeEnglishName()
{
	map<int, CMQOObject*>::iterator itrobj;
	for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		if( curobj ){
			CallF( ConvEngName( ENGNAME_DISP, (char*)curobj->GetName(), 256, (char*)curobj->GetEngName(), 256 ), return 1 );
		}
	}


	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone ){
			CallF( ConvEngName( ENGNAME_BONE, (char*)curbone->GetBoneName(), 256, (char*)curbone->GetEngBoneName(), 256 ), return 1 );
		}
	}

	map<int, MOTINFO*>::iterator itrmi;
	for( itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++ ){
		MOTINFO* curmi = itrmi->second;
		if( curmi ){
			CallF( ConvEngName( ENGNAME_MOTION, curmi->motname, 256, curmi->engmotname, 256 ), return 1 );
		}
	}

	return 0;
}

int CModel::AddDefMaterial()
{

	CMQOMaterial* dummymat = new CMQOMaterial();
	if( !dummymat ){
		_ASSERT( 0 );
		return 1;
	}

	int defmaterialno = (int)m_material.size();
	dummymat->SetMaterialNo( defmaterialno );
	dummymat->SetName( "dummyMaterial" );

	m_material[defmaterialno] = dummymat;

	return 0;
}


int CModel::CreateFBXMeshReq( FbxNode* pNode )
{
	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if ( pAttrib ) {
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
        FbxGeometryConverter lConverter(pNode->GetFbxManager());

		char mes[256];

		int shapecnt;
		CMQOObject* newobj = 0;

		switch ( type )
		{
			case FbxNodeAttribute::eMesh:

				newobj = GetFBXMesh( pNode, pAttrib, pNode->GetName() );     // メッシュを作成
				if (newobj){
					shapecnt = pNode->GetMesh()->GetShapeCount();
					if (shapecnt > 0){
						sprintf_s(mes, 256, "%s, shapecnt %d", pNode->GetName(), shapecnt);
						MessageBoxA(NULL, mes, "check", MB_OK);
					}
				}
				break;
//			case FbxNodeAttribute::eNURB:
//			case FbxNodeAttribute::eNURBS_SURFACE:
//                lConverter.TriangulateInPlace(pNode);
//				GetFBXMesh( pAttrib, pNode->GetName() );     // メッシュを作成
				break;
			default:
				break;
		}
	}

	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for ( int i = 0; i < childNodeNum; i++ )
	{
		FbxNode *pChild = pNode->GetChild(i);  // 子ノードを取得
		CreateFBXMeshReq( pChild );
	}

	return 0;
}

int CModel::CreateFBXShape( FbxAnimLayer* panimlayer, double animleng, FbxTime starttime, FbxTime timestep )
{
	map<CMQOObject*,FBXOBJ>::iterator itrobjindex;
	for( itrobjindex = m_fbxobj.begin(); itrobjindex != m_fbxobj.end(); itrobjindex++ ){
		FBXOBJ curfbxobj = itrobjindex->second;
		FbxMesh* curmesh = curfbxobj.mesh;
		CMQOObject* curobj = itrobjindex->first;
		if( curmesh && curobj ){
			int shapecnt = curmesh->GetShapeCount();
			if( shapecnt > 0 ){
				CallF( GetFBXShape( curmesh, curobj, panimlayer, animleng, starttime, timestep ), return 1 );
			}
		}
	}
	return 0;
}


CMQOObject* CModel::GetFBXMesh( FbxNode* pNode, FbxNodeAttribute *pAttrib, const char* nodename )
{


	FbxMesh *pMesh = (FbxMesh*)pAttrib;
	if (strcmp("RootNode", pAttrib->GetName()) == 0){
		_ASSERT(0);
		return 0;
	}

	CMQOObject* newobj = new CMQOObject();
	_ASSERT( newobj );
	newobj->SetObjFrom( OBJFROM_FBX );
	newobj->SetName( (char*)nodename );
	m_object[ newobj->GetObjectNo() ] = newobj;

	WCHAR wname[256];
	ZeroMemory( wname, sizeof( WCHAR ) * 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, nodename, 256, wname, 256 );


	FBXOBJ fbxobj;
	fbxobj.node = pNode;
	fbxobj.mesh = pMesh;
	m_fbxobj[newobj] = fbxobj;

//shape
//	int morphnum = pMesh->GetShapeCount();
//	newobj->m_morphnum = morphnum;

//マテリアル
	FbxNode* node = pMesh->GetNode();
	if ( node != 0 ) {
		// マテリアルの数
		int materialNum_ = node->GetMaterialCount();
		// マテリアル情報を取得
		//for( int i = 0; i < materialNum_; ++i ) {
		for (int i = 0; i < materialNum_; i++) {
			FbxSurfaceMaterial* material = node->GetMaterial( i );
			if ( material != 0 ) {
				CMQOMaterial* newmqomat = new CMQOMaterial();
				int mqomatno = newobj->GetMaterialSize();
				newmqomat->SetMaterialNo( mqomatno );
				newobj->SetMaterial( mqomatno, newmqomat );

				SetMQOMaterial( newmqomat, material );

				//const char* texname = newmqomat->GetTex();
				//const char* nodename = node->GetName();
				//_ASSERT(0);
			}
		}
	}

//頂点
	int PolygonNum       = pMesh->GetPolygonCount();
	int PolygonVertexNum = pMesh->GetPolygonVertexCount();
	int *IndexAry        = pMesh->GetPolygonVertices();

	int controlNum = pMesh->GetControlPointsCount();   // 頂点数
	FbxVector4* src = pMesh->GetControlPoints();    // 頂点座標配列

	DbgOut(L"LDCheck : GetFBXMesh : nodename %s, controlnum %d, polygonnum %d, polygonvertexnum %d\r\n", wname, controlNum, PolygonNum, PolygonVertexNum);

	// コピー
	newobj->SetVertex( controlNum );
	newobj->SetPointBuf( (ChaVector3*)malloc( sizeof( ChaVector3 ) * controlNum ) );
	//for ( int i = 0; i < controlNum; ++i ) {
	for (int i = 0; i < controlNum; i++) {
		ChaVector3* curctrl = newobj->GetPointBuf() + i;
		curctrl->x = (float)src[ i ][ 0 ];
		curctrl->y = (float)src[ i ][ 1 ];
		curctrl->z = (float)src[ i ][ 2 ];
		//curctrl->w = (float)src[ i ][ 3 ];

//DbgOut( L"GetFBXMesh : ctrl %d, (%f, %f, %f)\r\n",
//	i, curctrl->x, curctrl->y, curctrl->z );

	}

	newobj->SetFace( PolygonNum );
	newobj->SetFaceBuf( new CMQOFace[ PolygonNum ] );
	for ( int p = 0; p < PolygonNum; p++ ) {
		int IndexNumInPolygon = pMesh->GetPolygonSize( p );  // p番目のポリゴンの頂点数
		if( (IndexNumInPolygon != 3) && (IndexNumInPolygon != 4) ){
			_ASSERT( 0 );
			return 0;
		}
		
		CMQOFace* curface = newobj->GetFaceBuf() + p;
		curface->SetPointNum( IndexNumInPolygon );

		for ( int n = 0; n < IndexNumInPolygon; n++ ) {
			// ポリゴンpを構成するn番目の頂点のインデックス番号
			int IndexNumber = pMesh->GetPolygonVertex( p, n );
			curface->SetFaceNo( p );
			curface->SetIndex(  n, IndexNumber );
			curface->SetMaterialNo( 0 );
			curface->SetBoneType( MIKOBONE_NONE );
		}
	}

	/*
    // Populate the array with vertex attribute, if by control point.
    const FbxVector4 * lControlPoints = pMesh->GetControlPoints();
    FbxVector4 lCurrentVertex;
    FbxVector4 lCurrentNormal;
    FbxVector2 lCurrentUV;
    if (mAllByControlPoint)
    {
        const FbxGeometryElementNormal * lNormalElement = NULL;
        const FbxGeometryElementUV * lUVElement = NULL;
        if (mHasNormal)
        {
            lNormalElement = pMesh->GetElementNormal(0);
        }
        if (mHasUV)
        {
            lUVElement = pMesh->GetElementUV(0);
        }
        for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
        {
            // Save the vertex position.
            lCurrentVertex = lControlPoints[lIndex];
            lVertices[lIndex * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
            lVertices[lIndex * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
            lVertices[lIndex * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
            lVertices[lIndex * VERTEX_STRIDE + 3] = 1;

            // Save the normal.
            if (mHasNormal)
            {
                int lNormalIndex = lIndex;
                if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                {
                    lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
                }
                lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                lNormals[lIndex * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
                lNormals[lIndex * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
                lNormals[lIndex * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);
            }

            // Save the UV.
            if (mHasUV)
            {
                int lUVIndex = lIndex;
                if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                {
                    lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
                }
                lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
                lUVs[lIndex * UV_STRIDE] = static_cast<float>(lCurrentUV[0]);
                lUVs[lIndex * UV_STRIDE + 1] = static_cast<float>(lCurrentUV[1]);
            }
        }

    }
	*/



//法線
	int layerNum = pMesh->GetLayerCount();
	//for ( int i = 0; i < layerNum; ++i ) {
	for (int i = 0; i < layerNum; i++) {
	   FbxLayer* layer = pMesh->GetLayer( i );
	   FbxLayerElementNormal* normalElem = layer->GetNormals();
	   if ( normalElem == 0 ) {
		  continue;   // 法線無し
	   }
	   // 法線あった！
		// 法線の数・インデックス
		int    normalNum          = normalElem->GetDirectArray().GetCount();
		int    indexNum           = normalElem->GetIndexArray().GetCount();


//DbgOut( L"GetFBXMesh : %s : normalNum %d : indexNum %d\r\n", wname, normalNum, indexNum );

		// マッピングモード・リファレンスモード取得
		FbxLayerElement::EMappingMode mappingMode = normalElem->GetMappingMode();
		FbxLayerElement::EReferenceMode refMode = normalElem->GetReferenceMode();

		if ( mappingMode == FbxLayerElement::eByPolygonVertex ) {
//DbgOut( L"GetFBXMesh : %s : mapping eByPolygonVertex\r\n", wname );
		   if ( refMode == FbxLayerElement::eDirect ) {
//DbgOut( L"GetFBXMesh : %s : ref eDirect\r\n", wname );

				newobj->SetNormalLeng( normalNum );
				newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * normalNum ) );

				// 直接取得
			  //for ( int i = 0; i < normalNum; ++i ) {
				for (int i = 0; i < normalNum; i++) {
				ChaVector3* curn = newobj->GetNormal() + i;
				curn->x = (float)normalElem->GetDirectArray().GetAt( i )[ 0 ];
				curn->y = (float)normalElem->GetDirectArray().GetAt( i )[ 1 ];
				curn->z = (float)normalElem->GetDirectArray().GetAt( i )[ 2 ];
			  }
		   }else if ( refMode == FbxLayerElement::eIndexToDirect ){
//DbgOut( L"GetFBXMesh : %s : ref eIndexToDirect\r\n", wname );

				newobj->SetNormalLeng( indexNum );
				newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * indexNum ) );

				int lIndex;
				for( lIndex = 0; lIndex < indexNum; lIndex++ ){

					int lNormalIndex = normalElem->GetIndexArray().GetAt(lIndex);

				    FbxVector4 lCurrentNormal;
					lCurrentNormal = normalElem->GetDirectArray().GetAt(lNormalIndex);
					ChaVector3* curn = newobj->GetNormal() + lIndex;
					curn->x = static_cast<float>(lCurrentNormal[0]);
					curn->y = static_cast<float>(lCurrentNormal[1]);
					curn->z = static_cast<float>(lCurrentNormal[2]);
				}
		   }
		} else if ( mappingMode == FbxLayerElement::eByControlPoint ) {
//DbgOut( L"GetFBXMesh : %s : mapping eByControlPoint\r\n", wname );
		   if ( refMode == FbxLayerElement::eDirect ) {
//DbgOut( L"GetFBXMesh : %s : ref eDirect\r\n", wname );

				newobj->SetNormalLeng( normalNum );
				newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * normalNum ) );

				// 直接取得
				//for ( int i = 0; i < normalNum; ++i ) {
				for (int i = 0; i < normalNum; i++) {
					ChaVector3* curn = newobj->GetNormal() + i;
					curn->x = (float)normalElem->GetDirectArray().GetAt( i )[ 0 ];
					curn->y = (float)normalElem->GetDirectArray().GetAt( i )[ 1 ];
					curn->z = (float)normalElem->GetDirectArray().GetAt( i )[ 2 ];
				}
		   }else{
//DbgOut( L"GetFBXMesh : %s : ref %d\r\n", wname, refMode );
		   }
		} else {
			_ASSERT( 0 );
		}

		break;
	}

//UV
	int layerCount = pMesh->GetLayerCount();   // meshはFbxMesh
	for ( int uvi = 0; uvi < layerCount; ++uvi ) {
		FbxLayer* layer = pMesh->GetLayer( uvi );
		FbxLayerElementUV* elem = layer->GetUVs();
		if ( elem == 0 ) {
			continue;
		}

		// UV情報を取得
		// UVの数・インデックス
		int UVNum = elem->GetDirectArray().GetCount();
		int indexNum = elem->GetIndexArray().GetCount();
//		int size = UVNum > indexNum ? UVNum : indexNum;

		// マッピングモード・リファレンスモード別にUV取得
		FbxLayerElement::EMappingMode mappingMode = elem->GetMappingMode();
		FbxLayerElement::EReferenceMode refMode = elem->GetReferenceMode();

		if (mappingMode == FbxLayerElement::eByPolygonVertex) {
			DbgOut(L"\r\n\r\n");
			DbgOut(L"check !!! GetFBXMesh : %s : UV : mapping eByPolygonVertex\r\n", wname);

			if (refMode == FbxLayerElement::eDirect) {
				DbgOut(L"GetFBXMesh : %s : UV : refMode eDirect\r\n", wname);
				int size = UVNum;
				newobj->SetUVLeng(size);
				newobj->SetUVBuf((D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size));

				// 直接取得
				//for (int i = 0; i < size; ++i) {
				for (int i = 0; i < size; i++) {
					(newobj->GetUVBuf() + i)->x = (float)elem->GetDirectArray().GetAt(i)[0];
					(newobj->GetUVBuf() + i)->y = (float)elem->GetDirectArray().GetAt(i)[1];
					DbgOut(L"direct %d, u : %f, v : %f\r\n", i, (newobj->GetUVBuf() + i)->x, (newobj->GetUVBuf() + i)->y);
				}
				DbgOut(L"\r\n\r\n");
			}
			else if (refMode == FbxLayerElement::eIndexToDirect) {
				DbgOut(L"\r\n\r\n");
				DbgOut(L"Check !!! GetFBXMesh : %s : UV : refMode eIndexToDirect\r\n", wname);
				int size = indexNum;
				newobj->SetUVLeng(size);
				newobj->SetUVBuf((D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size));

				// インデックスから取得
				//for (int i = 0; i < size; ++i) {
				for (int i = 0; i < size; i++) {
					int index = elem->GetIndexArray().GetAt(i);
					(newobj->GetUVBuf() + i)->x = (float)elem->GetDirectArray().GetAt(index)[0];
					(newobj->GetUVBuf() + i)->y = (float)elem->GetDirectArray().GetAt(index)[1];
					DbgOut(L"direct %d, u : %f, v : %f\r\n", i, (newobj->GetUVBuf() + i)->x, (newobj->GetUVBuf() + i)->y);
				}
				DbgOut(L"\r\n\r\n");
			}
			else {
				DbgOut(L"GetFBXMesh : %s : UV : refMode %d\r\n", wname, refMode);
				_ASSERT(0);
			}
		}
		else if (mappingMode == FbxLayerElement::eByControlPoint) {
			if (refMode == FbxLayerElement::eDirect) {
				DbgOut(L"\r\n\r\n");
				DbgOut(L"check !!! GetFBXMesh : %s : UV : refMode eDirect\r\n", wname);
				int size = UVNum;
				newobj->SetUVLeng(size);
				D3DXVECTOR2* newuv = (D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size);
				_ASSERT(newuv);
				newobj->SetUVBuf(newuv);
				//newobj->SetUVBuf((D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size));

				// 直接取得
				//for (int i = 0; i < size; ++i) {
				for (int i = 0; i < size; i++) {
					(newobj->GetUVBuf() + i)->x = (float)elem->GetDirectArray().GetAt(i)[0];
					(newobj->GetUVBuf() + i)->y = (float)elem->GetDirectArray().GetAt(i)[1];
					DbgOut(L"direct %d, u : %f, v : %f\r\n", i, (newobj->GetUVBuf() + i)->x, (newobj->GetUVBuf() + i)->y);
				}
			}
			DbgOut(L"\r\n\r\n");
		} else {
DbgOut( L"GetFBXMesh : %s : UV : mappingMode %d\r\n", wname, mappingMode );
DbgOut( L"GetFBXMesh : %s : UV : refMode %d\r\n", wname, refMode );
_ASSERT(0);
		}
		break;
	}

	return newobj;
}

int CModel::SetMQOMaterial( CMQOMaterial* newmqomat, FbxSurfaceMaterial* pMaterial )
{
	newmqomat->SetName( (char*)pMaterial->GetName() );

	char* emitex = 0;
    const FbxDouble3 lEmissive = GetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &emitex);
	if( emitex ){
		DbgOut( L"SetMQOMaterial : emitexture find\r\n" );
	}
	ChaVector3 tmpemi;

	tmpemi.x = (float)lEmissive[0];
	tmpemi.y = (float)lEmissive[1];
	tmpemi.z = (float)lEmissive[2];
	newmqomat->SetEmi3F( tmpemi );


	char* ambtex = 0;
	const FbxDouble3 lAmbient = GetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &ambtex);
	if( ambtex ){
		DbgOut( L"SetMQOMaterial : ambtexture find\r\n" );
	}
	ChaVector3 tmpamb;
	tmpamb.x = (float)lAmbient[0];
	tmpamb.y = (float)lAmbient[1];
	tmpamb.z = (float)lAmbient[2];
	newmqomat->SetAmb3F( tmpamb );

	char* diffusetex = 0;
    const FbxDouble3 lDiffuse = GetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &diffusetex);
	if( diffusetex ){
		//strcpy_s( newmqomat->tex, 256, diffusetex );
		DbgOut( L"SetMQOMaterial : diffusetexture find\r\n" );
	}
	ChaVector4 tmpdif;
	tmpdif.x = (float)lDiffuse[0];
	tmpdif.y = (float)lDiffuse[1];
	tmpdif.z = (float)lDiffuse[2];
	tmpdif.w = 1.0f;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	newmqomat->SetDif4F( tmpdif );


	char* spctex = 0;
    const FbxDouble3 lSpecular = GetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &spctex);
	if( spctex ){
		DbgOut( L"SetMQOMaterial : spctexture find\r\n" );
	}
	ChaVector3 tmpspc;
	tmpspc.x = (float)lSpecular[0];
	tmpspc.y = (float)lSpecular[1];
	tmpspc.z = (float)lSpecular[2];
	newmqomat->SetSpc3F( tmpspc );

    FbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (lShininessProperty.IsValid())
    {
        double lShininess = lShininessProperty.Get<FbxDouble>();
        newmqomat->SetPower( static_cast<float>(lShininess) );
    }


//texture

	FbxProperty pProperty;
	pProperty = pMaterial->FindProperty( FbxSurfaceMaterial::sDiffuse );
    int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
    if(lLayeredTextureCount > 0)
    {
        for(int j=0; j<lLayeredTextureCount; ++j)
        {
            FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
            int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
            for(int k =0; k<lNbTextures; ++k)
            {
                char* nameptr = (char*)lLayeredTexture->GetName();
				if( nameptr ){
					char tempname[256];
					strcpy_s( tempname, 256, nameptr );
					char* lastslash = strrchr( tempname, '/' );
					if( !lastslash ){
						lastslash = strrchr( tempname, '\\' );
					}
					if( lastslash ){
						newmqomat->SetTex( lastslash + 1 );
					}else{
						newmqomat->SetTex( tempname );
					}
					char* lastp = strrchr( (char*)newmqomat->GetTex(), '.' );
					if( !lastp ){
						newmqomat->Add2Tex( ".tga" );
					}
					WCHAR wname[256];
					ZeroMemory( wname, sizeof( WCHAR ) * 256 );
					MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, newmqomat->GetTex(), 256, wname, 256 );

DbgOut( L"SetMQOMaterial : layered texture %s\r\n", wname );

					break;
				}
            }
        }
    }
    else
    {
        //no layered texture simply get on the property
        int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();
        if(lNbTextures > 0)
        {
            for(int j =0; j<lNbTextures; ++j)
            {
                FbxFileTexture* lTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
                if(lTexture)
                {
                    char* nameptr = (char*)lTexture->GetFileName();
					if( nameptr ){
						char tempname[256];
						strcpy_s( tempname, 256, nameptr );
						char* lastslash = strrchr( tempname, '/' );
						if( !lastslash ){
							lastslash = strrchr( tempname, '\\' );
						}
						if( lastslash ){
							newmqomat->SetTex( lastslash + 1 );
						}else{
							newmqomat->SetTex( tempname );
						}
						char* lastp = strrchr( (char*)newmqomat->GetTex(), '.' );
						if( !lastp ){
							newmqomat->Add2Tex( ".tga" );
						}

						WCHAR wname[256];
						ZeroMemory( wname, sizeof( WCHAR ) * 256 );
						MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, newmqomat->GetTex(), 256, wname, 256 );

DbgOut( L"SetMQOMaterial : texture %s\r\n", wname );

						break;
					}
                }
            }
        }
    }


   return 0;
}

// Get specific property value and connected texture if any.
// Value = Property value * Factor property value (if no factor property, multiply by 1).
FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
    const char * pPropertyName,
    const char * pFactorPropertyName,
    char** ppTextureName)
{
	*ppTextureName = 0;

    FbxDouble3 lResult(0, 0, 0);
    const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
    const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
    if (lProperty.IsValid() && lFactorProperty.IsValid())
    {
        lResult = lProperty.Get<FbxDouble3>();
        double lFactor = lFactorProperty.Get<FbxDouble>();
        if (lFactor != 1)
        {
            lResult[0] *= lFactor;
            lResult[1] *= lFactor;
            lResult[2] *= lFactor;
        }
    }


    return lResult;
}



int CModel::CreateFBXBoneReq( FbxScene* pScene, FbxNode* pNode, FbxNode* parnode )
{
//	EFbxRotationOrder lRotationOrder0 = eEulerZXY;
//	EFbxRotationOrder lRotationOrder1 = eEulerXYZ;

	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if ( pAttrib ) {
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
		const char* nodename = pNode->GetName();
		WCHAR wname[256];
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, nodename, -1, wname, 256 );
		if( type == FbxNodeAttribute::eSkeleton ){
			DbgOut( L"CreateFbxBoneReq : pNode %s : type : skeleton\r\n", wname );
		}else if( type == FbxNodeAttribute::eNull ){
			DbgOut( L"CreateFbxBoneReq : pNode %s : type : null\r\n", wname );
		}else{
			DbgOut( L"CreateFbxBoneReq : pNode %s : type : other : %d\r\n", wname, type );
		}


		FbxNode* parbonenode = 0;
		if( parnode ){
			FbxNodeAttribute *parattr = parnode->GetNodeAttribute();
			if ( parattr ) {
				FbxNodeAttribute::EType partype = parattr->GetAttributeType();
				const char* parnodename = parnode->GetName();
				WCHAR parwname[256];
				MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, parnodename, -1, parwname, 256 );
				if( partype == FbxNodeAttribute::eSkeleton ){
					DbgOut( L"CreateFbxBoneReq : parnode %s : type : skeleton\r\n", parwname );
				}else if( type == FbxNodeAttribute::eNull ){
					DbgOut( L"CreateFbxBoneReq : parnode %s : type : null\r\n", parwname );
				}else{
					DbgOut( L"CreateFbxBoneReq : parnode %s : type : other : %d\r\n", parwname, partype );
				}

				switch ( partype )
				{
					case FbxNodeAttribute::eSkeleton:
					case FbxNodeAttribute::eNull:
						parbonenode = parnode;
						break;
					default:
						parbonenode = 0;
						break;
				}
			}else{
				const char* parnodename = parnode->GetName();
				WCHAR parwname[256];
				MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, parnodename, -1, parwname, 256 );
				DbgOut( L"CreateFbxBoneReq : %s : parnode name %s : parattr NULL!!!!!\r\n", wname, parwname );
			}
		}else{
			DbgOut( L"CreateFbxBoneReq : %s : parnode NULL!!!!\r\n", wname );
		}



		switch ( type )
		{
			case FbxNodeAttribute::eSkeleton:
			case FbxNodeAttribute::eNull:
	
				//EFbxRotationOrder lRotationOrder = eEULER_ZXY;
				//pNode->SetRotationOrder(FbxNode::eSourcePivot , lRotationOrder0 );
				//pNode->SetRotationOrder(FbxNode::eDestinationPivot , lRotationOrder1 );
				
				DbgOut( L"CreateFBXBoneReq : skeleton : %s\r\n", wname );
				if (strcmp(nodename, "RootNode") != 0){
					if (parnode && (strcmp(parnode->GetName(), "RootNode") != 0)){
						GetFBXBone(pScene, type, pAttrib, nodename, pNode, parbonenode);
					}
					else{
						GetFBXBone(pScene, type, pAttrib, nodename, pNode, 0);
					}
				}
				else{
					_ASSERT(0);
				}
				break;
			case FbxSkeleton::eRoot:
				_ASSERT(0);
				break;
			default:
				break;
		}

	}

	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for ( int i = 0; i < childNodeNum; i++ )
	{
		FbxNode *pChild = pNode->GetChild(i);  // 子ノードを取得
		CreateFBXBoneReq( pScene, pChild, pNode );
	}

	return 0;
}

int CModel::GetFBXBone( FbxScene* pScene, FbxNodeAttribute::EType type, FbxNodeAttribute *pAttrib, const char* nodename, FbxNode* curnode, FbxNode* parnode )
{
	int settopflag = 0;
	CBone* newbone = new CBone( this );
	_ASSERT( newbone );
	if (!newbone){
		_ASSERT(0);
		return 1;
	}
	newbone->LoadCapsuleShape(m_pdev);

	char newbonename[256];
	strcpy_s(newbonename, 256, nodename);
	TermJointRepeats(newbonename);
	newbone->SetName(newbonename);
	
	newbone->SetTopBoneFlag( 0 );
	m_bonelist[newbone->GetBoneNo()] = newbone;
	m_bonename[ newbone->GetBoneName() ] = newbone;

	if( type == FbxNodeAttribute::eSkeleton ){
		newbone->SetType( FBXBONE_NORMAL );
	}else if( type == FbxNodeAttribute::eNull ){
		newbone->SetType( FBXBONE_NULL );
	}else{
		_ASSERT( 0 );
	}

//	if( !parnode ){
//		m_firstbone = curnode;
//	}
	if( !m_topbone ){
		m_topbone = newbone;
		m_bone2node[newbone] = curnode;
		settopflag = 1;
	}else{
		m_bone2node[newbone] = curnode;
	}

	EFbxRotationOrder lRotationOrder0;
	curnode->GetRotationOrder (FbxNode::eSourcePivot, lRotationOrder0);
	EFbxRotationOrder lRotationOrder1 = eEulerXYZ;
	curnode->SetRotationOrder(FbxNode::eDestinationPivot , lRotationOrder1 );

	if( parnode ){
		//const char* parbonename = parnode->GetName();
		char parbonename[256];
		strcpy_s(parbonename, 256, parnode->GetName());
		TermJointRepeats(parbonename);

		CBone* parbone = m_bonename[ parbonename ];
		if( parbone ){
			parbone->AddChild( newbone );
//_ASSERT(0);
		}else{
			/***
			FbxNodeAttribute *parattr = parnode->GetNodeAttribute();
			if( parattr ){
				FbxNodeAttribute::EType type = parattr->GetAttributeType();
				if( type == FbxNodeAttribute::eSkeleton ){
					_ASSERT( 0 );
				}else if( type == FbxNodeAttribute::eNull ){
					_ASSERT( 0 );
				}else{
					_ASSERT( 0 );
				}
			}else{
				_ASSERT( 0 );
			}
			***/
			::MessageBoxA( NULL, "GetFBXBone : parbone NULL error ", parbonename, MB_OK );
		}
	}else{
		if( settopflag == 0 ){
			_ASSERT( 0 );
			m_topbone->AddChild( newbone );
		}else{
			_ASSERT(0);
			//::MessageBoxA( NULL, "GetFBXBone : parbone NULL error ", nodename, MB_OK );
		}
	}

	return 0;
}

int CModel::MotionID2Index( int motid )
{
	int retindex = -1;

	int chkcnt = 0;
	map<int,MOTINFO*>::iterator itrmotinfo;
	for( itrmotinfo = m_motinfo.begin(); itrmotinfo != m_motinfo.end(); itrmotinfo++ ){
		MOTINFO* curmi = itrmotinfo->second;
		if( curmi ){
			if( curmi->motid == motid ){
				retindex = chkcnt;
				break;
			}
		}
		chkcnt++;
	}
	return retindex;
}


FbxAnimLayer* CModel::GetAnimLayer( int motid )
{
	FbxAnimLayer *retAnimLayer = 0;

	int motindex = MotionID2Index( motid );
	if( motindex < 0 ){
		return 0;
	}

	FbxAnimStack *lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[motindex]->Buffer());
	if (lCurrentAnimationStack == NULL){
		_ASSERT( 0 );
		return 0;
	}
	retAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();

	return retAnimLayer;
}


int CModel::CreateFBXAnim( FbxScene* pScene, FbxNode* prootnode )
{
	static int s_dbgcnt = 0;
	s_dbgcnt++;


	SetDefaultBonePos();


	pScene->FillAnimStackNameArray(mAnimStackNameArray);
    const int lAnimStackCount = mAnimStackNameArray.GetCount();

	DbgOut( L"FBX anim num %d\r\n", lAnimStackCount );

	if( lAnimStackCount <= 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int animno;
	for( animno = 0; animno < lAnimStackCount; animno++ ){
		// select the base layer from the animation stack
		//char* animname = mAnimStackNameArray[animno]->Buffer();
		//MessageBoxA( NULL, animname, "check", MB_OK );
		FbxAnimStack * lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[animno]->Buffer());
		if (lCurrentAnimationStack == NULL){
			_ASSERT( 0 );
			return 1;
		}
		FbxAnimLayer * mCurrentAnimLayer;
		mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();

		pScene->GetEvaluator()->SetContext(lCurrentAnimationStack);

		//pScene->GetRootNode()->ConvertPivotAnimationRecursive( mAnimStackNameArray[animno]->Buffer(), FbxNode::eDestinationPivot, 30.0, true );
		//pScene->GetRootNode()->ConvertPivotAnimationRecursive( mAnimStackNameArray[animno]->Buffer(), FbxNode::eSourcePivot, 30.0, true );

		FbxTakeInfo* lCurrentTakeInfo = pScene->GetTakeInfo(*(mAnimStackNameArray[animno]));
		if (lCurrentTakeInfo)
		{
			mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();

			double dstart = mStart.GetSecondDouble();
			double dstop = mStop.GetSecondDouble();
//_ASSERT( 0 );
		}
		else
		{
			_ASSERT( 0 );
			// Take the time line value
			FbxTimeSpan lTimeLineTimeSpan;
			pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
			mStart = lTimeLineTimeSpan.GetStart();
			mStop  = lTimeLineTimeSpan.GetStop();
		}


//		int animleng = (int)mStop.GetFrame();// - mStart.GetFrame() + 1;		
		//mFrameTime.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
		//mFrameTime2.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
		mFrameTime.SetSecondDouble( 1.0 / 30.0 );
		mFrameTime2.SetSecondDouble( 1.0 / 30.0 );
		//mFrameTime.SetSecondDouble( 1.0 / 300.0 );
		//mFrameTime2.SetSecondDouble( 1.0 / 300.0 );
	

		double animleng = (int)( (mStop.GetSecondDouble() - mStart.GetSecondDouble()) * 30.0 );

		DbgOut( L"FBX anim %d, animleng %lf\r\n", animno, animleng );




		int curmotid = -1;
		AddMotion( mAnimStackNameArray[animno]->Buffer(), 0, animleng, &curmotid );


		map<int,CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* curbone = itrbone->second;
			if( curbone ){
				curbone->SetGetAnimFlag( 0 );
			}
		}

        //FbxPose* pPose = pScene->GetPose( animno );
		//FbxPose* pPose = pScene->GetPose( 10 );
		
//!!!!!!!!!!!!!!!!!!!!!		
		//FbxPose* pPose = GetBindPose();
		FbxPose* pPose = NULL;
		
		
		CreateFBXAnimReq( animno, pPose, prootnode, curmotid, animleng, mStart, mFrameTime2 );	

		FillUpEmptyKeyReq( curmotid, animleng, m_topbone, 0 );

		if( animno == 0 ){
			CallF( CreateFBXShape( mCurrentAnimLayer, animleng, mStart, mFrameTime2 ), return 1 );
		}

		(this->m_tlFunc)( curmotid );

	}

	return 0;
}

int CModel::CreateFBXAnimReq( int animno, FbxPose* pPose, FbxNode* pNode, int motid, double animleng, FbxTime mStart, FbxTime mFrameTime )
{
	//static int dbgcnt = 0;

	//int lSkinCount;

	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if ( pAttrib ) {
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();

		switch ( type )
		{
			case FbxNodeAttribute::eMesh:
//			case FbxNodeAttribute::eNURB:
//			case FbxNodeAttribute::eNURBS_SURFACE:
				GetFBXAnim( animno, pNode, pPose, pAttrib, motid, animleng, mStart, mFrameTime );     // メッシュを作成

				break;
			default:
				break;
		}
	}

	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for ( int i = 0; i < childNodeNum; i++ )
	{
		FbxNode *pChild = pNode->GetChild(i);  // 子ノードを取得
		CreateFBXAnimReq( animno, pPose, pChild, motid, animleng, mStart, mFrameTime );
	}

	return 0;
}
int CModel::GetFBXAnim( int animno, FbxNode* pNode, FbxPose* pPose, FbxNodeAttribute *pAttrib, int motid, double animleng, FbxTime mStart, FbxTime mFrameTime )
{
	FbxAMatrix pGlobalPosition;
	pGlobalPosition.SetIdentity();


	FbxMesh *pMesh = (FbxMesh*)pAttrib;


	// スキンの数を取得
	int skinCount  = pMesh->GetDeformerCount( FbxDeformer::eSkin );

	//for ( int i = 0; i < skinCount; ++i ) {
	for (int i = 0; i < skinCount; i++) {
		// i番目のスキンを取得
		FbxSkin* skin = (FbxSkin*)( pMesh->GetDeformer( i, FbxDeformer::eSkin ) );

		// クラスターの数を取得
		int clusterNum = skin->GetClusterCount();

		for ( int j = 0; j < clusterNum; ++j ) {
			// j番目のクラスタを取得
			FbxCluster* cluster = skin->GetCluster( j );

			const char* bonename = ((FbxNode*)cluster->GetLink())->GetName();
			char bonename2[256];
			strcpy_s(bonename2, 256, bonename);
			TermJointRepeats(bonename2);
			CBone* curbone = m_bonename[ (char*)bonename2 ];


WCHAR wname[256]={0L};
ZeroMemory( wname, sizeof( WCHAR ) * 256 );
MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)bonename2, 256, wname, 256 );


			if( curbone && !curbone->GetGetAnimFlag()){
				curbone->SetGetAnimFlag( 1 );


				FbxAMatrix mat;
				FbxTime ktime = mStart;
				double framecnt;
				for( framecnt = 0.0; framecnt < animleng; framecnt+=1.0 ){
					FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();

					FbxAMatrix lReferenceGlobalInitPosition;
					FbxAMatrix lReferenceGlobalCurrentPosition;
					FbxAMatrix lAssociateGlobalInitPosition;
					FbxAMatrix lAssociateGlobalCurrentPosition;
					FbxAMatrix lClusterGlobalInitPosition;
					FbxAMatrix lClusterGlobalCurrentPosition;

					FbxAMatrix lReferenceGeometry;
					FbxAMatrix lAssociateGeometry;
					FbxAMatrix lClusterGeometry;

					FbxAMatrix lClusterRelativeInitPosition;
					FbxAMatrix lClusterRelativeCurrentPositionInverse;
	

					cluster->GetTransformMatrix(lReferenceGlobalInitPosition);
					lReferenceGlobalCurrentPosition = pGlobalPosition;
					// Multiply lReferenceGlobalInitPosition by Geometric Transformation
					lReferenceGeometry = GetGeometry(pMesh->GetNode());
					lReferenceGlobalInitPosition *= lReferenceGeometry;

					// Get the link initial global position and the link current global position.
					cluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
					lClusterGlobalCurrentPosition = GetGlobalPosition(cluster->GetLink(), ktime, pPose);

					// Compute the initial position of the link relative to the reference.
					lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

					// Compute the current position of the link relative to the reference.
					lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

					// Compute the shift of the link relative to the reference.
					mat = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;


					ChaMatrix xmat;
					xmat._11 = (float)mat.Get( 0, 0 );
					xmat._12 = (float)mat.Get( 0, 1 );
					xmat._13 = (float)mat.Get( 0, 2 );
					xmat._14 = (float)mat.Get( 0, 3 );

					xmat._21 = (float)mat.Get( 1, 0 );
					xmat._22 = (float)mat.Get( 1, 1 );
					xmat._23 = (float)mat.Get( 1, 2 );
					xmat._24 = (float)mat.Get( 1, 3 );

					xmat._31 = (float)mat.Get( 2, 0 );
					xmat._32 = (float)mat.Get( 2, 1 );
					xmat._33 = (float)mat.Get( 2, 2 );
					xmat._34 = (float)mat.Get( 2, 3 );

					xmat._41 = (float)mat.Get( 3, 0 );
					xmat._42 = (float)mat.Get( 3, 1 );
					xmat._43 = (float)mat.Get( 3, 2 );
					xmat._44 = (float)mat.Get( 3, 3 );

					if ((animno == 0) && (framecnt == 0.0)){
						curbone->SetFirstMat(xmat);
						curbone->SetInitMat(xmat);
						ChaMatrix calcmat = curbone->GetNodeMat() * curbone->GetInvFirstMat();
						ChaVector3 zeropos(0.0f, 0.0f, 0.0f);
						ChaVector3 tmppos;
						D3DVec3TransformCoord(&tmppos, &zeropos, &calcmat);
						curbone->SetOldJointFPos(tmppos);
					}
					

					CMotionPoint* curmp = 0;
					int existflag = 0;
					curmp = curbone->AddMotionPoint( motid, framecnt, &existflag );
					if( !curmp ){
						_ASSERT( 0 );
						return 1;
					}
					curmp->SetWorldMat(xmat);//anglelimit無し
					//curmp->SetBefWorldMat(xmat);

					ktime += mFrameTime;
					//ktime = mFrameTime * framecnt;
				}
			}

			if (!curbone){
				_ASSERT(0);
			}
		}
	}


	return 0;
}


int CModel::CreateFBXSkinReq( FbxNode* pNode )
{
	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if ( pAttrib ) {
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();

		switch ( type )
		{
			case FbxNodeAttribute::eMesh:
//			case FbxNodeAttribute::eNURB:
//			case FbxNodeAttribute::eNURBS_SURFACE:

				GetFBXSkin( pAttrib, pNode );     // メッシュを作成
				break;
			default:
				break;
		}
	}

	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for ( int i = 0; i < childNodeNum; i++ )
	{
		FbxNode *pChild = pNode->GetChild(i);  // 子ノードを取得
		CreateFBXSkinReq( pChild );
	}

	return 0;
}
int CModel::GetFBXSkin( FbxNodeAttribute *pAttrib, FbxNode* pNode )
{
	const char* nodename = pNode->GetName();

	FbxMesh *pMesh = (FbxMesh*)pAttrib;
	CMQOObject* newobj = 0;
	newobj = m_objectname[ nodename ];
	if( !newobj ){
		_ASSERT( 0 );
		return 1;
	}

//スキン
	// スキンの数を取得
	int skinCount  = pMesh->GetDeformerCount( FbxDeformer::eSkin );

	int makecnt = 0;
	//for ( int i = 0; i < skinCount; ++i ) {
	for (int i = 0; i < skinCount; i++) {
		// i番目のスキンを取得
		FbxSkin* skin = (FbxSkin*)( pMesh->GetDeformer( i, FbxDeformer::eSkin ) );

		// クラスターの数を取得
		int clusterNum = skin->GetClusterCount();
DbgOut( L"fbx : skin : org clusternum %d\r\n", clusterNum );

		for ( int j = 0; j < clusterNum; ++j ) {
			// j番目のクラスタを取得
			FbxCluster* cluster = skin->GetCluster( j );

			int validflag = IsValidCluster( cluster );
			if( validflag == 0 ){
				continue;
			}

			const char* bonename = ((FbxNode*)cluster->GetLink())->GetName();
			char bonename2[256];
			strcpy_s(bonename2, 256, bonename);
			TermJointRepeats(bonename2);
//			int namelen = (int)strlen( clustername );
			WCHAR wname[256];
			ZeroMemory( wname, sizeof( WCHAR ) * 256 );
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, bonename2, -1, wname, 256 );
//			DbgOut( L"cluster (%d, %d), name : %s\r\n", i, j, wname );

			CBone* curbone = m_bonename[ (char*)bonename2 ];

			if( curbone ){
				int curclusterno = newobj->GetClusterSize();

				if( curclusterno >= MAXCLUSTERNUM ){
					WCHAR wmes[256];
					swprintf_s( wmes, 256, L"１つのパーツに影響できるボーンの制限数(%d個)を超えました。読み込めません。", MAXCLUSTERNUM );
					MessageBoxW( NULL, wmes, L"ボーン数エラー", MB_OK );
					_ASSERT( 0 );
					return 1;
				}


				newobj->PushBackCluster( curbone );

				int pointNum = cluster->GetControlPointIndicesCount();
				int* pointAry = cluster->GetControlPointIndices();
				double* weightAry = cluster->GetControlPointWeights();

				FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();
				int index;
				float weight;
				for ( int i2 = 0; i2 < pointNum; i2++ ) {
					// 頂点インデックスとウェイトを取得
					index  = pointAry[ i2 ];
					weight = (float)weightAry[ i2 ];

					int isadditive;
					if( lClusterMode == FbxCluster::eAdditive ){
						isadditive = 1;
					}else{
						isadditive = 0;
					}

					if( (lClusterMode == FbxCluster::eAdditive) || (weight >= 0.05f) ){
					//if ((lClusterMode == FbxCluster::eAdditive)){
						newobj->AddInfBone( curclusterno, index, weight, isadditive );
					}
				}

				makecnt++;

			}else{
				_ASSERT( 0 );
			}

		}

		newobj->NormalizeInfBone();


	}

	DbgOut( L"fbx skin : make cluster %d\r\n", makecnt );


	return 0;
}


int IsValidCluster( FbxCluster* cluster )
{
	int findflag = 0;

	int pointNum = cluster->GetControlPointIndicesCount();
	int* pointAry = cluster->GetControlPointIndices();
	double* weightAry = cluster->GetControlPointWeights();

	FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();

	int index;
	double weight;
	for ( int i2 = 0; i2 < pointNum; i2++ ) {
		// 頂点インデックスとウェイトを取得
		index  = pointAry[ i2 ];
		weight = weightAry[ i2 ];

		if( (lClusterMode == FbxCluster::eAdditive) || (weight >= 0.05) ){
		//if ((lClusterMode == FbxCluster::eAdditive)){
			findflag = 1;
			break;
		}
	}

	return findflag;
}

int CModel::RenderBoneMark( ID3D10Device* pdev, CModel* bmarkptr, CMySprite* bcircleptr, int selboneno, int skiptopbonemark )
{
	if( m_bonelist.empty() ){
		return 0;
	}

	map<int, CBone*>::iterator itrb;
	for( itrb = m_bonelist.begin(); itrb != m_bonelist.end(); itrb++ ){
		CBone* curbone = itrb->second;
		if( curbone ){
			curbone->SetSelectFlag( 0 );
		}
	}

	if (selboneno > 0){
		CBone* selbone = m_bonelist[selboneno];
		if (selbone){
			SetSelectFlagReq(selbone, 0);
			selbone->SetSelectFlag(2);

			CBone* parbone = selbone->GetParent();
			if (parbone){
				CBtObject* curbto = FindBtObject(selbone->GetBoneNo());
				if (curbto){
					int tmpflag = selbone->GetSelectFlag() + 4;
					selbone->SetSelectFlag(tmpflag);
				}
			}
		}
	}

	//pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

	//ボーンの三角錐表示
	if ((g_previewFlag != 5) && (g_previewFlag != 4)){
		if (g_bonemarkflag && bmarkptr){
			map<int, CBone*>::iterator itrbone;
			for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
				CBone* boneptr = itrbone->second;
				if (boneptr){
					CBone* chilbone = boneptr->GetChild();
					while (chilbone){
						int renderflag = 0;
						if (skiptopbonemark == 0){
							renderflag = 1;
						}
						else{
							CBone* parbone = boneptr->GetParent();
							if (parbone){
								renderflag = 1;
							}
							else{
								renderflag = 0;
							}
						}
						if (renderflag == 1){

							ChaVector3 aftbonepos;
							D3DVec3TransformCoord(&aftbonepos, &boneptr->GetJointFPos(), &(boneptr->GetCurMp().GetWorldMat()));

							ChaVector3 aftchilpos;
							//D3DVec3TransformCoord(&aftchilpos, &chilbone->GetJointFPos(), &(chilbone->GetCurMp().GetWorldMat()));
							D3DVec3TransformCoord(&aftchilpos, &chilbone->GetJointFPos(), &(boneptr->GetCurMp().GetWorldMat()));


							boneptr->CalcAxisMatZ(&aftbonepos, &aftchilpos);

							ChaMatrix bmmat;
							bmmat = boneptr->GetLAxisMat();// * boneptr->m_curmp.m_worldmat;


							ChaVector3 diffvec = aftchilpos - aftbonepos;
							float diffleng = ChaVector3Length(&diffvec);

							float fscale;
							ChaMatrix scalemat;
							ChaMatrixIdentity(&scalemat);
							fscale = diffleng / 50.0f;
							scalemat._11 = fscale;
							scalemat._22 = fscale;
							scalemat._33 = fscale;

							bmmat = scalemat * bmmat;

							bmmat._41 = aftbonepos.x;
							bmmat._42 = aftbonepos.y;
							bmmat._43 = aftbonepos.z;


							g_pEffect->SetMatrix(g_hmWorld, &bmmat);
							bmarkptr->UpdateMatrix(&bmmat, &m_matVP);
							ChaVector4 difmult;
							if (chilbone->GetSelectFlag() & 2){
								difmult = ChaVector4(1.0f, 0.0f, 0.0f, 0.5f);
							}
							else{
								difmult = ChaVector4(0.25f, 0.5f, 0.5f, 0.5f);
							}
							CallF(bmarkptr->OnRender(pdev, 0, difmult), return 1);
						}


						chilbone = chilbone->GetBrother();
					}
				}
			}
		}
	}


	//ボーンの剛体表示
	if ((g_previewFlag != 5) && (g_previewFlag != 4)){
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
			CBone* boneptr = itrbone->second;
			if (boneptr){
				CBone* childbone = boneptr->GetChild();
				while (childbone){
					CRigidElem* curre = boneptr->GetRigidElem(childbone);
					if (curre){
						boneptr->CalcRigidElemParams(childbone, 0);
						g_pEffect->SetMatrix(g_hmWorld, &(curre->GetCapsulemat()));
						boneptr->GetCurColDisp(childbone)->UpdateMatrix(&(curre->GetCapsulemat()), &m_matVP);
						ChaVector4 difmult;
						//if( boneptr->GetSelectFlag() & 4 ){
						if (childbone->GetSelectFlag() & 4){
							difmult = ChaVector4(1.0f, 0.0f, 0.0f, 0.5f);
						}
						else{
							difmult = ChaVector4(0.25f, 0.5f, 0.5f, 0.5f);
						}
						CallF(boneptr->GetCurColDisp(childbone)->OnRender(pdev, 0, difmult), return 1);
					}

					childbone = childbone->GetBrother();
				}



				/*
				std::map<CBone*, CRigidElem*>::iterator itrtmpmap;
				for (itrtmpmap = boneptr->GetRigidElemMapBegin(); itrtmpmap != boneptr->GetRigidElemMapEnd(); itrtmpmap++){
					CRigidElem* curre = itrtmpmap->second;
					if (curre){
						CBone* chilbone = itrtmpmap->first;
						_ASSERT(chilbone);
						if (chilbone){
							//DbgOut( L"check!!!: curbone %s, chilbone %s\r\n", boneptr->m_wbonename, chilbone->m_wbonename );
							boneptr->CalcRigidElemParams(chilbone, 0);
							g_pEffect->SetMatrix(g_hmWorld, &(curre->GetCapsulemat()));
							boneptr->GetCurColDisp(chilbone)->UpdateMatrix(&(curre->GetCapsulemat()), &m_matVP);
							ChaVector4 difmult;
							//if( boneptr->GetSelectFlag() & 4 ){
							if (chilbone->GetSelectFlag() & 4){
								difmult = ChaVector4(1.0f, 0.0f, 0.0f, 0.5f);
							}
							else{
								difmult = ChaVector4(0.25f, 0.5f, 0.5f, 0.5f);
							}
							CallF(boneptr->GetCurColDisp(chilbone)->OnRender(pdev, 0, difmult), return 1);
						}
					}
				}
				*/
			}
		}
	}
	else{
		RenderCapsuleReq(pdev, m_topbt);
	}
	

	//ボーンのサークル表示
	if ((g_previewFlag != 5) && (g_previewFlag != 4)){

		if (g_bonemarkflag && bcircleptr){
			map<int, CBone*>::iterator itrbone;
			for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
				CBone* boneptr = itrbone->second;
				if (boneptr && (boneptr->GetType() == FBXBONE_NORMAL)){

					ChaMatrix bcmat;
					bcmat = boneptr->GetCurMp().GetWorldMat();
					//CBone* parbone = boneptr->GetParent();
					//CBone* chilbone = boneptr->GetChild();
					ChaMatrix transmat = bcmat * m_matVP;
					ChaVector3 scpos;
					ChaVector3 firstpos = boneptr->GetJointFPos();

					D3DVec3TransformCoord(&scpos, &firstpos, &transmat);
					scpos.z = 0.0f;
					bcircleptr->SetPos(scpos);
					D3DXVECTOR2 bsize;
					if (boneptr->GetSelectFlag() & 2){
						bcircleptr->SetColor(ChaVector4(0.0f, 0.0f, 1.0f, 0.7f));
						bsize = D3DXVECTOR2(0.050f, 0.050f);
						bcircleptr->SetSize(bsize);
					}
					else if (boneptr->GetSelectFlag() & 1){
						bcircleptr->SetColor(ChaVector4(1.0f, 0.0f, 0.0f, 0.7f));
						bsize = D3DXVECTOR2(0.025f, 0.025f);
						bcircleptr->SetSize(bsize);
					}
					else{
						bcircleptr->SetColor(ChaVector4(1.0f, 1.0f, 1.0f, 0.7f));
						bsize = D3DXVECTOR2(0.025f, 0.025f);
						bcircleptr->SetSize(bsize);
					}
					CallF(bcircleptr->OnRender(), return 1);

				}
			}
		}
	}
	else{
		if (g_bonemarkflag && bcircleptr){
			RenderBoneCircleReq(m_topbt, bcircleptr);
		}
	}

	//pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	return 0;
}



void CModel::RenderCapsuleReq(ID3D10Device* pdev, CBtObject* srcbto)
{
	CBone* srcbone = srcbto->GetBone();
	CBone* chilbone = srcbto->GetEndBone();
	if (srcbone && chilbone){
		//if (srcbone->GetParent()){
			//CRigidElem* curre = srcbone->GetParent()->GetRigidElem(srcbone);
		CRigidElem* curre = srcbone->GetRigidElem(chilbone);
		if (curre){
			srcbone->CalcRigidElemParams(chilbone, 0);//形状データのスケールのために呼ぶ。ここでのカプセルマットは次のSetCapsuleBtMotionで上書きされる。
			srcbto->SetCapsuleBtMotion(curre);

			g_pEffect->SetMatrix(g_hmWorld, &(curre->GetCapsulemat()));
			srcbone->GetCurColDisp(chilbone)->UpdateMatrix(&(curre->GetCapsulemat()), &m_matVP);
			ChaVector4 difmult;
			//if( boneptr->GetSelectFlag() & 4 ){
			if (chilbone->GetSelectFlag() & 4){
				difmult = ChaVector4(1.0f, 0.0f, 0.0f, 0.5f);
			}
			else{
				difmult = ChaVector4(0.25f, 0.5f, 0.5f, 0.5f);
			}
			CallF(srcbone->GetCurColDisp(chilbone)->OnRender(pdev, 0, difmult), return);
		}
		//}
	}

	int chilno;
	for (chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChilBt(chilno);
		RenderCapsuleReq(pdev, chilbto);
	}

}


void CModel::RenderBoneCircleReq(CBtObject* srcbto, CMySprite* bcircleptr)
{
	CBone* srcbone = srcbto->GetBone();
	CBone* chilbone = srcbto->GetEndBone();
	if (srcbone && chilbone){
		CRigidElem* curre = srcbone->GetRigidElem(chilbone);
		if (curre){
			if (chilbone && (chilbone->GetType() == FBXBONE_NORMAL)){

				//ChaMatrix capsulemat;
				//capsulemat = curre->GetCapsulemat();
				////CBone* parbone = boneptr->GetParent();
				////CBone* chilbone = boneptr->GetChild();
				//ChaMatrix transmat = capsulemat * m_matVP;
				//ChaVector3 scpos;
				//ChaVector3 firstpos = ChaVector3(0.0f, 0.0f, 0.0f);
				//
				//D3DVec3TransformCoord(&scpos, &firstpos, &transmat);

				ChaMatrix transmat = srcbone->GetBtMat() * m_matVP;
				ChaVector3 firstpos = chilbone->GetJointFPos();
				ChaVector3 scpos;
				D3DVec3TransformCoord(&scpos, &firstpos, &transmat);
				scpos.z = 0.0f;

				bcircleptr->SetPos(scpos);
				D3DXVECTOR2 bsize;
				if (chilbone->GetSelectFlag() & 2){
					bcircleptr->SetColor(ChaVector4(0.0f, 0.0f, 1.0f, 0.7f));
					bsize = D3DXVECTOR2(0.050f, 0.050f);
					bcircleptr->SetSize(bsize);
				}
				else if (chilbone->GetSelectFlag() & 1){
					bcircleptr->SetColor(ChaVector4(1.0f, 0.0f, 0.0f, 0.7f));
					bsize = D3DXVECTOR2(0.025f, 0.025f);
					bcircleptr->SetSize(bsize);
				}
				else{
					bcircleptr->SetColor(ChaVector4(0.8f, 0.8f, 0.8f, 0.7f));
					bsize = D3DXVECTOR2(0.025f, 0.025f);
					bcircleptr->SetSize(bsize);
				}
				CallF(bcircleptr->OnRender(), return);

			}
		}
	}

	int chilno;
	for (chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChilBt(chilno);
		RenderBoneCircleReq(chilbto, bcircleptr);
	}
}


void CModel::SetDefaultBonePosReq( CBone* curbone, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition )
{
	if (!curbone){
		return;
	}

	FbxNode* pNode = m_bone2node[ curbone ];


	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;//バインドポーズを書き出さない場合やHipsなどの場合は０になる？


	if( pPose ){
		int lNodeIndex = pPose->Find(pNode);
		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
 			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);

	}


	ChaMatrix nodemat;

	nodemat._11 = (float)lGlobalPosition.Get( 0, 0 );
	nodemat._12 = (float)lGlobalPosition.Get( 0, 1 );
	nodemat._13 = (float)lGlobalPosition.Get( 0, 2 );
	nodemat._14 = (float)lGlobalPosition.Get( 0, 3 );

	nodemat._21 = (float)lGlobalPosition.Get( 1, 0 );
	nodemat._22 = (float)lGlobalPosition.Get( 1, 1 );
	nodemat._23 = (float)lGlobalPosition.Get( 1, 2 );
	nodemat._24 = (float)lGlobalPosition.Get( 1, 3 );

	nodemat._31 = (float)lGlobalPosition.Get( 2, 0 );
	nodemat._32 = (float)lGlobalPosition.Get( 2, 1 );
	nodemat._33 = (float)lGlobalPosition.Get( 2, 2 );
	nodemat._34 = (float)lGlobalPosition.Get( 2, 3 );

	nodemat._41 = (float)lGlobalPosition.Get( 3, 0 );
	nodemat._42 = (float)lGlobalPosition.Get( 3, 1 );
	nodemat._43 = (float)lGlobalPosition.Get( 3, 2 );
	nodemat._44 = (float)lGlobalPosition.Get( 3, 3 );

	curbone->SetPositionFound(lPositionFound);//!!!
	curbone->SetNodeMat( nodemat );
	curbone->SetGlobalPosMat( lGlobalPosition );

	ChaVector3 zeropos( 0.0f, 0.0f, 0.0f );
	ChaVector3 tmppos;
	D3DVec3TransformCoord( &tmppos, &zeropos, &(curbone->GetNodeMat()) );
	curbone->SetJointWPos( tmppos );
	curbone->SetJointFPos( tmppos );

//WCHAR wname[256];
//ZeroMemory( wname, sizeof( WCHAR ) * 256 );
//MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, curbone->m_bonename, 256, wname, 256 );
//DbgOut( L"SetDefaultBonePos : %s : wpos (%f, %f, %f)\r\n", wname, curbone->m_jointfpos.x, curbone->m_jointfpos.y, curbone->m_jointfpos.z );


	if( curbone->GetChild() ){
		SetDefaultBonePosReq( curbone->GetChild(), pTime, pPose, &curbone->GetGlobalPosMat() );
	}
	if( curbone->GetBrother() ){
		SetDefaultBonePosReq( curbone->GetBrother(), pTime, pPose, pParentGlobalPosition );
	}

}

FbxPose* CModel::GetBindPose()
{
	FbxPose* bindpose = 0;
	FbxPose* curpose = m_pscene->GetPose(0);
	int curpindex = 1;
	while (curpose){
		if (curpose->IsBindPose()){
			bindpose = curpose;//最後のバインドポーズ
		}
		curpose = m_pscene->GetPose(curpindex);
		curpindex++;
	}
	if (!bindpose){
		//::MessageBoxA(NULL, "バインドポーズがありません。", "警告", MB_OK);
		bindpose = m_pscene->GetPose(0);
	}
	return bindpose;
}


int CModel::SetDefaultBonePos()
{
	if( !m_topbone ){
		return 0;
	}

	FbxPose* bindpose = GetBindPose();

	FbxTime pTime;
	pTime.SetSecondDouble( 0.0 );

	//CBone* secbone = m_topbone->GetChild();
	CBone* secbone = m_topbone;

	if( secbone ){
		SetDefaultBonePosReq( secbone, pTime, bindpose, 0 );
	}

	return 0;
}

FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
    FbxAMatrix lGlobalPosition;
    bool        lPositionFound = false;

    if (pPose)
    {
        int lNodeIndex = pPose->Find(pNode);

        if (lNodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
            {
                lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                FbxAMatrix lParentGlobalPosition;

                if (pParentGlobalPosition)
                {
                    lParentGlobalPosition = *pParentGlobalPosition;
                }
                else
                {
                    if (pNode->GetParent())
                    {
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
                    }
                }

                FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
                lGlobalPosition = lParentGlobalPosition * lLocalPosition;
            }

            lPositionFound = true;
        }
    }

    if (!lPositionFound)
    {
        // There is no pose entry for that node, get the current global position instead.

        // Ideally this would use parent global position and local position to compute the global position.
        // Unfortunately the equation 
        //    lGlobalPosition = pParentGlobalPosition * lLocalPosition
        // does not hold when inheritance type is other than "Parent" (RSrs).
        // To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
        lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
    }

    return lGlobalPosition;
}

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
    FbxAMatrix lPoseMatrix;
    FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

// Get the geometry offset to a node. It is never inherited by the children.
FbxAMatrix GetGeometry(FbxNode* pNode)
{
    const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

    return FbxAMatrix(lT, lR, lS);
}


void CModel::FillUpEmptyKeyReq( int motid, double animleng, CBone* curbone, CBone* parbone )
{
	if (!curbone){
		return;
	}

	ChaMatrix parfirstmat, invparfirstmat;
	ChaMatrixIdentity( &parfirstmat );
	ChaMatrixIdentity( &invparfirstmat );
	if( parbone ){
		double zeroframe = 0.0;
		int existz = 0;
		CMotionPoint* parmp = parbone->AddMotionPoint( motid, zeroframe, &existz );
		if( existz && parmp ){
			parfirstmat = parmp->GetWorldMat();//!!!!!!!!!!!!!! この時点ではm_matWorldが掛かっていないから後で修正必要かも？？
			ChaMatrixInverse( &invparfirstmat, NULL, &parfirstmat );
		}else{
			ChaMatrixIdentity( &parfirstmat );
			ChaMatrixIdentity( &invparfirstmat );			
		}
	}

	double framecnt;
	for( framecnt = 0.0; framecnt < animleng; framecnt+=1.0 ){
		double frame = framecnt;

		ChaMatrix mvmat;
		ChaMatrixIdentity( &mvmat );

		CMotionPoint* pcurmp = 0;
		pcurmp = curbone->GetMotionPoint(motid, frame);
		if(!pcurmp){
			int exist2 = 0;
			CMotionPoint* newmp = curbone->AddMotionPoint( motid, frame, &exist2 );
			if( !newmp ){
				_ASSERT( 0 );
				return;
			}

			if( parbone ){
				int exist3 = 0;
				CMotionPoint* parmp = parbone->AddMotionPoint( motid, frame, &exist3 );
				ChaMatrix tmpmat = parbone->GetInvFirstMat() * parmp->GetWorldMat();//!!!!!!!!!!!!!!!!!! endjointはこれでうまく行くが、floatと分岐が不動になる。
				//newmp->SetBefWorldMat(tmpmat);
				newmp->SetWorldMat(tmpmat);//anglelimit無し

				//オイラー角初期化
				ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
				int paraxsiflag = 1;
				int isfirstbone = 0;
				cureul = curbone->CalcLocalEulZXY(paraxsiflag, motid, (double)framecnt, BEFEUL_ZERO, isfirstbone);
				curbone->SetLocalEul(motid, (double)framecnt, cureul);

			}
		}
	}

	if( curbone->GetChild() ){
		FillUpEmptyKeyReq( motid, animleng, curbone->GetChild(), curbone );
	}
	if( curbone->GetBrother() ){
		FillUpEmptyKeyReq( motid, animleng, curbone->GetBrother(), parbone );
	}

}

int CModel::FillUpEmptyMotion(int srcmotid)
{

	MOTINFO* curmi = GetMotInfo( srcmotid );
	_ASSERT(curmi);
	if (curmi){
		FillUpEmptyKeyReq(curmi->motid, curmi->frameleng, m_topbone, 0);
		return 0;
	}
	else{
		_ASSERT(0);
		return 1;
	}

}


int CModel::SetMaterialName()
{
	m_materialname.clear();

	map<int, CMQOMaterial*>::iterator itrmat;
	for( itrmat = m_material.begin(); itrmat != m_material.end(); itrmat++ ){
		CMQOMaterial* curmat = itrmat->second;
		m_materialname[ curmat->GetName() ] = curmat;
	}

	return 0;
}


int CModel::DestroyBtObject()
{
	if( m_topbt ){
		DestroyBtObjectReq( m_topbt );
	}
	m_topbt = 0;
	m_rigidbone.clear();

	map<int,CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if(curbone){
			curbone->ClearBtObject();
		}
	}


	return 0;
}
void CModel::DestroyBtObjectReq( CBtObject* curbt )
{
	vector<CBtObject*> tmpvec;

	int chilno;
	for (chilno = 0; chilno < (int)curbt->GetChilBtSize(); chilno++){
		CBtObject* chilbt = curbt->GetChilBt(chilno);
		if (chilbt){
			tmpvec.push_back(chilbt);
		}
	}

	delete curbt;

	for (chilno = 0; chilno < tmpvec.size(); chilno++){
		CBtObject* chilbt = tmpvec[chilno];
		DestroyBtObjectReq(chilbt);
	}

}

/***
int CModel::CalcBtAxismat( float delta )
{
	if( !m_topbone ){
		return 0;
	}
	CalcBtAxismatReq( m_topbone, delta );//!!!!!!!!!!!!!

	return 0;
}
***/


void CModel::SetBtKinFlagReq( CBtObject* srcbto, int oncreateflag )
{

	CBone* srcbone = srcbto->GetBone();
	if( srcbone ){
//		srcbone->m_btkinflag = 0;
		
		int cmp0 = strncmp( srcbone->GetBoneName(), "BT_", 3 );
		if( (cmp0 == 0) || (srcbone->GetBtForce() == 1)){
			if (srcbone->GetParent()){
				CRigidElem* curre = srcbone->GetParent()->GetRigidElem(srcbone);
				if (curre){
					//if (curre->GetSkipflag() == 0){
						srcbone->SetBtKinFlag(0);
					//}
					//else{
					//	srcbone->SetBtKinFlag(1);
					//}


				}
				else{
					srcbone->SetBtKinFlag(1);
				}
			}
			else{
				srcbone->SetBtKinFlag(1);
			}
		}else{
			srcbone->SetBtKinFlag( 1 );
		}

		/*
		if ((srcbone->GetBtKinFlag() == 0) && srcbone->GetParent() && (srcbone->GetParent()->GetBtKinFlag() == 1)){
			CBtObject* parbto = srcbto->GetParBt();
			if (parbto){
				int constraintnum = parbto->GetConstraintSize();
				int cno;
				for (cno = 0; cno < constraintnum; cno++){
					CONSTRAINTELEM ce = parbto->GetConstraintElem(cno);
					if (ce.centerbone == srcbone){
						btGeneric6DofSpringConstraint* dofC = ce.constraint;
						if (dofC){
							//int dofid;
							//for (dofid = 0; dofid < 3; dofid++){
							//	dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
							//dofC->enableSpring(dofid, true);
							//}
							//for (dofid = 3; dofid < 6; dofid++){
							//dofC->enableSpring(dofid, true);
							//	dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
							//}
							//dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
							//dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));

							ChaVector3 aftbonepos;
							D3DVec3TransformCoord(&aftbonepos, &srcbone->GetJointFPos(), &(srcbone->GetCurMp().GetWorldMat()));

							dofC->setLinearLowerLimit(btVector3(aftbonepos.x, aftbonepos.y, aftbonepos.z));
							dofC->setLinearUpperLimit(btVector3(aftbonepos.x, aftbonepos.y, aftbonepos.z));
						}
					}
				}
			}
		}
		*/
		/*
		else{
			int constraintnum = srcbto->GetConstraintSize();
			int cno;
			for (cno = 0; cno < constraintnum; cno++){
				CONSTRAINTELEM ce = srcbto->GetConstraintElem(cno);
				if (ce.centerbone != srcbone){
					btGeneric6DofSpringConstraint* dofC = ce.constraint;
					if (dofC){
						int dofid;
						for (dofid = 0; dofid < 3; dofid++){
							//dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
							dofC->enableSpring(dofid, true);
						}
						for (dofid = 3; dofid < 6; dofid++){
							dofC->enableSpring(dofid, true);
							//dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
						}
						//dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
						//dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
						//dofC->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
						//dofC->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
					}
				}
			}
		}
		*/
		
		/*
		if ((srcbone->GetBtKinFlag() == 1) && srcbone->GetParent() && (srcbone->GetParent()->GetBtKinFlag() == 1)){
			int constraintnum = srcbto->GetConstraintSize();
			int cno;
			for (cno = 0; cno < constraintnum; cno++){
				btGeneric6DofSpringConstraint* dofC = srcbto->GetConstraint(cno);
				if (dofC){
					int dofid;
					for (dofid = 0; dofid < 3; dofid++){
						dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
						//dofC->enableSpring(dofid, true);
					}
					for (dofid = 3; dofid < 6; dofid++){
						//dofC->enableSpring(dofid, true);
						dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
					}
					dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
					dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
					dofC->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
					dofC->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
				}
			}
		}
		else{
			int constraintnum = srcbto->GetConstraintSize();
			int cno;
			for (cno = 0; cno < constraintnum; cno++){
				btGeneric6DofSpringConstraint* dofC = srcbto->GetConstraint(cno);
				if (dofC){
					int dofid;
					for (dofid = 0; dofid < 3; dofid++){
						//dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
						dofC->enableSpring(dofid, true);
					}
					for (dofid = 3; dofid < 6; dofid++){
						dofC->enableSpring(dofid, true);
						//dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
					}

				}
			}
		}
		*/




		if( (srcbone->GetBtKinFlag() == 1) && (srcbto->GetRigidBody()) ){
			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
			if( s_setrigidflag == 0 ){
				s_rigidflag = curflag;
				s_setrigidflag = 1;
			}
			//srcbto->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			srcbto->GetRigidBody()->setCollisionFlags( curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
			//srcbto->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
			//srcbto->GetRigidBody()->setActivationState(WANTS_DEACTIVATION);
			//srcbto->m_rigidbody->setActivationState(DISABLE_SIMULATION);
			//CF_STATIC_OBJECT
		}else if( srcbto->GetRigidBody() ){
			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
			srcbto->GetRigidBody()->setCollisionFlags(curflag & ~btCollisionObject::CF_KINEMATIC_OBJECT);

			if( srcbone->GetParent() ){
				CRigidElem* curre = srcbone->GetParent()->GetRigidElem( srcbone );
				if( curre ){
					if ((m_curreindex >= 0) && (m_curreindex < (int)m_rigideleminfo.size())){
						srcbto->GetRigidBody()->setGravity(btVector3(0.0f, curre->GetBtg() * m_rigideleminfo[m_curreindex].btgscale, 0.0f));
						srcbto->GetRigidBody()->applyGravity();
					}
					else{
						_ASSERT(0);
					}
				}
			}
		}

		if (srcbto->GetRigidBody()){
			//srcbto->GetRigidBody()->setDeactivationTime(0.0);
			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 150.0);
			//srcbto->GetRigidBody()->activate();
			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
		}
	}

	int chilno;
	for( chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChilBt( chilno );
		SetBtKinFlagReq( chilbto, oncreateflag );
	}
}


int CModel::BulletSimulationStop()
{
	BulletSimulationStopReq(m_topbt);
	return 0;
}

void CModel::BulletSimulationStopReq(CBtObject* srcbto)
{
	if (!srcbto)
		return;

	if (srcbto->GetRigidBody()){
		srcbto->GetRigidBody()->setActivationState(DISABLE_SIMULATION);
	}

	int chilno;
	for (chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChilBt(chilno);
		BulletSimulationStopReq(chilbto);
	}

}

int CModel::BulletSimulationStart()
{
	BulletSimulationStartReq(m_topbt);
	return 0;
}

void CModel::BulletSimulationStartReq(CBtObject* srcbto)
{
	if (!srcbto)
		return;

	if (srcbto->GetRigidBody()){
		srcbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
		srcbto->GetRigidBody()->activate();
		srcbto->GetRigidBody()->setDeactivationTime(0.0);
		//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
	}

	int chilno;
	for (chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChilBt(chilno);
		BulletSimulationStartReq(chilbto);
	}

}


int CModel::CreateBtConstraint()
{
	if( !m_topbone ){
		return 0;
	}
	if( !m_topbt ){
		return 0;
	}

	CreateBtConstraintReq( m_topbt );
	//CreateBtConstraintReq(m_topbone);

	//CreateBtConnectReq( m_topbone );

	return 0;
}

/*
void CModel::CreateBtConstraintReq(CBone* curbone)
{
	if (!curbone){
		return;
	}

	map<CBone*, CBtObject*>::iterator itrbto;
	for (itrbto = curbone->GetBtObjectMapBegin(); itrbto != curbone->GetBtObjectMapEnd(); itrbto++){
		CBtObject* curbto = itrbto->second;
		if (curbto){
			curbto->CreateBtConstraint();
		}
	}

	if (curbone->GetChild()){
		CreateBtConstraintReq(curbone->GetChild());
	}
	if (curbone->GetBrother()){
		CreateBtConstraintReq(curbone->GetBrother());
	}
}
*/


void CModel::CreateBtConstraintReq( CBtObject* curbto )
{
	if( curbto->GetTopFlag() == 0 ){
		CallF( curbto->CreateBtConstraint(), return );
	}

	int btono;
	for( btono = 0; btono < (int)curbto->GetChilBtSize(); btono++ ){
		CBtObject* chilbto = curbto->GetChilBt( btono );
		if( chilbto ){
			CreateBtConstraintReq( chilbto );
		}
	}
}

/*
void CModel::CreateBtConnectReq(CBone* curbone)
{
	if (!curbone){
		return;
	}
	if (curbone->GetChild()){
		CBone* brobone1 = curbone->GetChild()->GetBrother();
		if (brobone1){
			CBone* brobone2 = brobone1->GetBrother();
			while (brobone2){
				map<CBone*, CBtObject*>::iterator itrbto1;
				for (itrbto1 = brobone1->GetBtObjectMapBegin(); itrbto1 != brobone1->GetBtObjectMapEnd(); itrbto1++){
					CBtObject* bto1 = itrbto1->second;
					if (bto1 && bto1->GetRigidBody()){
						map<CBone*, CBtObject*>::iterator itrbto2;
						for (itrbto2 = brobone2->GetBtObjectMapBegin(); itrbto2 != brobone2->GetBtObjectMapEnd(); itrbto2++){
							CBtObject* bto2 = itrbto2->second;
							if (bto2 && bto2->GetRigidBody()){


								float angPAI2, angPAI;
								angPAI2 = 90.0f * (float)DEG2PAI;
								angPAI = 180.0f * (float)DEG2PAI;

								float lmax, lmin;
								lmax = 10000.0f;
								lmin = -10000.0f;
								//lmin = 0.0f;
								//lmax = 0.0f;
								//lmin = -1.0e-3;
								//lmax = 1.0e-3;

								btGeneric6DofSpringConstraint* dofC;
								btTransform tmpA, tmpA2;
								bto1->GetFrameA(tmpA);
								bto2->GetFrameA(tmpA2);
								dofC = new btGeneric6DofSpringConstraint(*bto1->GetRigidBody(), *bto2->GetRigidBody(), tmpA, tmpA2, true);
								_ASSERT(dofC);

								dofC->setLinearLowerLimit(btVector3(lmin, lmin, lmin));
								dofC->setLinearUpperLimit(btVector3(lmax, lmax, lmax));
								dofC->setAngularLowerLimit(btVector3(angPAI, angPAI2, angPAI));
								dofC->setAngularUpperLimit(btVector3(-angPAI, -angPAI2, -angPAI));


								//dofC->setBreakingImpulseThreshold(FLT_MAX);
								//dofC->setBreakingImpulseThreshold(0.0);//!!!!!!!!!!!!!!!!!!!!
								dofC->setBreakingImpulseThreshold(1e7);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



								int l_kindex = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetLKindex();
								int a_kindex = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetAKindex();
								float l_damping = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetLDamping();
								float a_damping = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetADamping();
								float l_cusk = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetCusLk();
								float a_cusk = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetCusAk();

								int dofid;
								//for (dofid = 0; dofid < 3; dofid++){
								//	dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
								//}
								//for (dofid = 3; dofid < 6; dofid++){
								//	dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
								//}
								
								for (dofid = 0; dofid < 3; dofid++){
									dofC->enableSpring(dofid, true);//!!!!!!!!!!!!!!!!!!!
									dofC->setStiffness(dofid, 1.0e12);
									//dofC->setStiffness(dofid, 1.0e6);

									dofC->setDamping(dofid, 0.5f);
								}
								for (dofid = 3; dofid < 6; dofid++){
									dofC->enableSpring(dofid, true);//!!!!!!!!!!!!!!!!!
									dofC->setStiffness(dofid, 80.0f);
									//dofC->setStiffness(dofid, 1000.0f);
									dofC->setDamping(dofid, 0.01f);
								}
								

								dofC->setEquilibriumPoint();//!!!!!!!!!!!!tmp disable

								CONSTRAINTELEM addelem;
								addelem.constraint = dofC;
								addelem.centerbone = 
								bto1->PushBackConstraint(dofC);
								//m_btWorld->addConstraint(dofC, false);//!!!!!!!!!!!! disable collision between linked bodies
								m_btWorld->addConstraint(dofC, true);
							}
						}
					}
				}

				brobone2 = brobone2->GetBrother();
			}
		}
	}

	if (curbone->GetChild()){
		CreateBtConnectReq(curbone->GetChild());
	}
	if (curbone->GetBrother()){
		CreateBtConnectReq(curbone->GetBrother());
	}
}
*/


int CModel::CreateBtObject( int onfirstcreate )
{

	DestroyBtObject();

	if( !m_topbone ){
		return 0;
	}
	if (!m_btWorld){
		return 0;
	}

	CalcBtAxismatReq(m_topbone, 1);//!!!!!!!!!!!!!


	m_topbt = new CBtObject( 0, m_btWorld );
	if( !m_topbt ){
		_ASSERT( 0 );
		return 1;
	}
	m_topbt->SetTopFlag( 1 );
	//m_topbt->CreateObject(NULL, NULL, m_topbone, m_topbone->GetChild());


	m_rigidbone.clear();

	CBone* startbone = m_topbone;
	_ASSERT( startbone );
	if (startbone){
		CreateBtObjectReq(m_topbt, startbone, startbone->GetChild());
		//CreateBtObjectReq(NULL, startbone, startbone->GetChild());
	}

	CreateBtConstraint();


	//if( m_topbt ){
	//	SetBtKinFlagReq( m_topbt, onfirstcreate );
	//}

	if (g_previewFlag != 5){
		SetBtKinFlagReq(m_topbt, 1);
		 //RestoreMassReq(m_topbone);
	}
	else{
		SetRagdollKinFlagReq(m_topbt, -1);
		CreatePhysicsPosConstraintReq(m_topbone);
		SetMass0Req(m_topbone);
	}


	if (m_topbt){
		SetBtEquilibriumPointReq(m_topbt);
	}


	//for debug
	DumpBtObjectReq(m_topbt, 0);
	DbgOut(L"\r\n\r\n");
	DumpBtConstraintReq(m_topbt, 0);
	DbgOut(L"\r\n\r\n");


	SetCreateBtFlag(true);

	return 0;          
}

int CModel::SetBtEquilibriumPointReq( CBtObject* srcbto )
{
	if (!srcbto){
		return 0;
	}

	//`角度、位置
	srcbto->EnableSpring(true, true);

	//srcbto->EnableSpring(false, true);
	//srcbto->EnableSpring(true, false);

	int lflag, aflag;
	aflag = 1;
	lflag = 0;
	srcbto->SetEquilibriumPoint( lflag, aflag );

	int childno;
	for (childno = 0; childno < srcbto->GetChilBtSize(); childno++){
		CBtObject* childbto = srcbto->GetChilBt(childno);
		if (childbto){
			SetBtEquilibriumPointReq(childbto);
		}
	}

	return 0;
}


int CModel::SetDofRotAxis(int srcaxiskind)
{
	if (m_topbt){
		SetDofRotAxisReq(m_topbt, srcaxiskind);
	}
	return 0;

}

void CModel::SetDofRotAxisReq(CBtObject* srcbto, int srcaxiskind)
{
	if (!srcbto){
		return;
	}

	//`角度、位置
	srcbto->SetDofRotAxis(srcaxiskind);


	int childno;
	for (childno = 0; childno < srcbto->GetChilBtSize(); childno++){
		CBtObject* childbto = srcbto->GetChilBt(childno);
		if (childbto){
			SetDofRotAxisReq(childbto, srcaxiskind);
		}
	}

	return;
}



void CModel::CreateBtObjectReq( CBtObject* parbt, CBone* parbone, CBone* curbone )
{
	if (!curbone){
		return;
	}
	if (!parbone){
		return;
	}
	CRigidElem* curre;
	curre = parbone->GetRigidElem(curbone);

	CBtObject* newbto = 0;
	CBone* chilbone = 0;

	if (curre){
		newbto = new CBtObject(parbt, m_btWorld);
		if (!newbto){
			_ASSERT(0);
			return;
		}

		//if (!m_topbt){
		//	m_topbt = newbto;
		//	m_topbt->SetTopFlag( 1 );
		//}

		if (parbt){
			parbt->AddChild(newbto);


			CallF(newbto->CreateObject(parbt, parbone->GetParent(), parbone, curbone), return);
			parbone->SetBtObject(curbone, newbto);

			//if (parbt->GetBone() && parbt->GetEndBone()){
			//	DbgOut(L"checkbt2 : CreateBtObject : parbto %s---%s, curbto %s---%s\r\n",
			//		parbt->GetBone()->GetWBoneName(), parbt->GetEndBone()->GetWBoneName(),
			//		newbto->GetBone()->GetWBoneName(), newbto->GetEndBone()->GetWBoneName());
			//}

			if (curbone->GetChild()){
				CreateBtObjectReq(newbto, curbone, curbone->GetChild());
			}
			if (curbone->GetBrother()){
				CreateBtObjectReq(parbt, parbone, curbone->GetBrother());
			}
		}
	}
}

void CModel::CalcBtAxismat(int onfirstcreate)
{
	CalcBtAxismatReq(m_topbone, onfirstcreate);
	return;
}


void CModel::CalcBtAxismatReq( CBone* curbone, int onfirstcreate )
{
	if (!curbone){
		return;
	}
	//int setstartflag;
	if( onfirstcreate != 0 ){
		if (curbone->GetParent()){
			curbone->GetParent()->CalcRigidElemParams(curbone, onfirstcreate);//firstflag 1
		}
		curbone->SetStartMat2( curbone->GetCurMp().GetWorldMat() );
	}

	if( curbone->GetChild() ){
		CalcBtAxismatReq(curbone->GetChild(), onfirstcreate);
	}
	if( curbone->GetBrother() ){
		CalcBtAxismatReq(curbone->GetBrother(), onfirstcreate);
	}
}

int CModel::SetBtMotion( int ragdollflag, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat )
{
	m_matWorld = *wmat;
	m_matVP = *vpmat;

	if( !m_topbt ){
		_ASSERT( 0 );
		return 0;
	}
	if (!m_btWorld){
		return 0;
	}
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!
	}

	int curmotid = m_curmotinfo->motid;
	double curframe = m_curmotinfo->curframe;

	ChaMatrix inimat;
	ChaMatrixIdentity( &inimat );
	CQuaternion iniq;
	iniq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone ){
			//CMotionPoint curmp = curbone->GetCurMp();
			curbone->SetBtFlag( 0 );
			//curbone->SetCurMp( curmp );
		}
	}

	SetBtMotionReq( m_topbt, wmat, vpmat );

	//if (g_previewFlag == 5){
	//	if (m_topbt){
	//		SetBtEquilibriumPointReq(m_topbt);
	//	}
	//}

	/*
	//resetbt処理との問題で一時的にコメントアウト

///// morph

	if( (ragdollflag == 1) && (m_rgdmorphid >= 0) ){
		MOTINFO* rgdmorphinfo = GetRgdMorphInfo();
		if( rgdmorphinfo ){

			double nextframe = 0.0;
			int endflag = 0;
			AdvanceTime( g_previewFlag, difftime, &nextframe, &endflag, rgdmorphinfo->motid );
			rgdmorphinfo->curframe = nextframe;

			DampAnim( rgdmorphinfo );

//DbgOut( L"!!!!! setbtmotion : rgd : morph : rgdmotid %d, curframe %f, difftime %f, frameleng %f\r\n", 
//	rgdmorphinfo->motid, rgdmorphinfo->curframe, difftime, rgdmorphinfo->frameleng );


			FbxAnimStack * lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[m_rgdmorphid]->Buffer());
			if (lCurrentAnimationStack == NULL){
				_ASSERT( 0 );
				return 1;
			}
			FbxAnimLayer * mCurrentAnimLayer;
			mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
   

			FbxTime lTime;
			lTime.SetSecondDouble( rgdmorphinfo->curframe / 30.0 );
//			lTime.SetSecondDouble( rgdmorphinfo->curframe / 300.0 );
			//lTime.SetTime(0, 0, 0, (int)rgdmorphinfo->curframe, 0, m_pscene->GetGlobalSettings().GetTimeMode());

			map<int, CMQOObject*>::iterator itrobj;
			for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
				CMQOObject* curobj = itrobj->second;
				_ASSERT( curobj );
				if( !(curobj->EmptyFindShape()) ){
					GetShapeWeight( m_fbxobj[curobj].node, m_fbxobj[curobj].mesh, lTime, mCurrentAnimLayer, curobj );
					CallF( curobj->UpdateMorphBuffer(), return 1 );
				}
			}
		}
	}
	*/
	return 0;
}


int CModel::DampAnim( MOTINFO* rgdmorphinfo )
{
	if( !rgdmorphinfo || (m_rgdindex < 0) ){
		return 0;
	}

	double diffframe = rgdmorphinfo->curframe;

	if( m_rgdindex >= (int)m_rigideleminfo.size() ){
		_ASSERT( 0 );
		return 0;
	}

	//float minval = 0.000050f;
	float minval = 0.000005f;

	map<int,CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone ){
			CBone* parbone = curbone->GetParent();
			if( parbone ){
				CRigidElem* curre = parbone->GetRigidElem( curbone );
				if( curre ){

					float newdampl = curre->GetLDamping() - (float)diffframe * curre->GetDampanimL() * curre->GetLDamping();
					if( newdampl < minval ){
						newdampl = minval;
					}
					float newdampa = curre->GetADamping() - (float)diffframe * curre->GetDampanimA() * curre->GetADamping();
					if( newdampa < minval ){
						newdampa = minval;
					}


					CBtObject* curbto = FindBtObject( curbone->GetBoneNo() );
					if( curbto ){
						int constraintnum = curbto->GetConstraintSize();
						int constraintno;
						for( constraintno = 0; constraintno < constraintnum; constraintno++ ){
							btGeneric6DofSpringConstraint* curct = curbto->GetConstraint(constraintno);
							if( curct ){
								int dofid;
							for( dofid = 0; dofid < 3; dofid++ ){
								curct->setDamping( dofid, newdampl );
							}
							for( dofid = 3; dofid < 6; dofid++ ){
								curct->setDamping( dofid, newdampa );
							}
							}

							//curct->setDamping( newdampa );

						}
					}

				}
			}
		}
	}

	return 0;
}



MOTINFO* CModel::GetRgdMorphInfo()
{
	MOTINFO* retmi = 0;

	int motionnum = (int)m_motinfo.size();
	if( m_rgdmorphid < motionnum ){
		map<int,MOTINFO*>::iterator itrmi;
		itrmi = m_motinfo.begin();
		int infcnt;
		for( infcnt = 0; infcnt < m_rgdmorphid; infcnt++ ){
			itrmi++;
			if( itrmi == m_motinfo.end() ){
				break;
			}
		}
		if( itrmi != m_motinfo.end() ){
			retmi = itrmi->second;
		}
	}

	return retmi;
}


void CModel::SetBtMotionReq( CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat )
{
	/*
	if (curbone && (curbone->GetCurMp().GetBtFlag() == 0)){
		if (g_previewFlag == 4){
			if (curbone->GetBtKinFlag() == 1){
				CMotionPoint curmp = curbone->GetCurMp();
				curmp.SetBtMat(curmp.GetWorldMat());
				curmp.SetBtFlag(1);
				curbone->SetCurMp(curmp);
			}
		}
		else if (g_previewFlag == 5){
			if (curbone->GetParent()){
				//curbone->m_curmp.m_btmat = curbone->m_parent->m_curmp.m_btmat;
				ChaMatrix invstart;
				ChaMatrixInverse(&invstart, NULL, &(curbone->GetParent()->GetStartMat2()));
				ChaMatrix diffmat;
				diffmat = invstart * curbone->GetParent()->GetCurMp().GetBtMat();
				CMotionPoint curmp = curbone->GetCurMp();
				curmp.SetBtMat(curbone->GetStartMat2() * diffmat);
				curmp.SetBtFlag(1);
				curbone->SetCurMp(curmp);
			}
			else{
				CMotionPoint curmp = curbone->GetCurMp();
				curmp.SetBtMat(curbone->GetStartMat2());
				curmp.SetBtFlag(1);
				curbone->SetCurMp(curmp);
			}
		}
	}
	*/

	if ((curbto->GetTopFlag() == 0) && curbto->GetBone()){
		CBone* curbone = curbto->GetBone();
		if (curbone){
			if (g_previewFlag == 4){
				if (curbone->GetBtKinFlag() == 0){
					//if (curbto->GetParBt() && (curbto->GetParBt()->GetBone()->GetBtKinFlag() == 1) && (curbto->GetParBt()->GetBone()->GetCurMp().GetBtFlag() == 0)){
					//	//KinFlagの変わり目部分に対応。
					//	//変わり目部分が枝分かれの時はまた後で、、、
					//	curbto->GetParBt()->SetBtMotion();
					//}
					curbto->SetBtMotion();
				}
				else if (curbone->GetBtKinFlag() == 1){
					CMotionPoint curmp = curbone->GetCurMp();
					curbone->SetBtMat(curmp.GetWorldMat());
					curbone->SetBtFlag(1);
					//curbone->SetCurMp(curmp);
				}
			}
			else if (g_previewFlag == 5){

				curbto->SetBtMotion();

				if (curbone->GetBtFlag() == 0){
					if (curbone->GetParent()){
						ChaMatrix invstart;
						ChaMatrixInverse(&invstart, NULL, &(curbone->GetParent()->GetStartMat2()));
						ChaMatrix diffmat;
						diffmat = invstart * curbone->GetParent()->GetBtMat();
						CMotionPoint curmp = curbone->GetCurMp();
						curbone->SetBtMat(curbone->GetStartMat2() * diffmat);
						curbone->SetBtFlag(1);
						//curbone->SetCurMp(curmp);
					}
					else{
						//CMotionPoint curmp = curbone->GetCurMp();
						curbone->SetBtMat(curbone->GetStartMat2());
						curbone->SetBtFlag(1);
						//curbone->SetCurMp(curmp);
					}
				}

			}
		}
	}
	int chilno;
	for( chilno = 0; chilno < curbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = curbto->GetChilBt( chilno );
		if( chilbto ){
			SetBtMotionReq( chilbto, wmat, vpmat );
		}
	}

}

void CModel::CreateRigidElemReq( CBone* curbone, int reflag, string rename, int impflag, string impname )
{
	if (!curbone){
		return;
	}
	//CBone* parbone = curbone->GetParent();
	//if (parbone){
	//	parbone->CreateRigidElem(curbone, reflag, rename, impflag, impname);
	//}

	curbone->CreateRigidElem(curbone->GetParent(), reflag, rename, impflag, impname);


	if( curbone->GetChild() ){
		CreateRigidElemReq( curbone->GetChild(), reflag, rename, impflag, impname );
	}
	if( curbone->GetBrother() ){
		CreateRigidElemReq( curbone->GetBrother(), reflag, rename, impflag, impname );
	}
}

int CModel::SetBtImpulse()
{
	if( !m_topbt ){
		return 0;
	}
	if( !m_topbone ){
		return 0;
	}

	SetBtImpulseReq( m_topbone );

	return 0;
}

void CModel::SetBtImpulseReq( CBone* srcbone )
{
	CBone* parbone = srcbone->GetParent();

	if( parbone ){
		ChaVector3 setimp( 0.0f, 0.0f, 0.0f );

		int impnum = parbone->GetImpMapSize();
		if( (m_curimpindex >= 0) && (m_curimpindex < impnum) ){

			string curimpname = m_impinfo[ m_curimpindex ];
			map<string, map<CBone*, ChaVector3>>::iterator findimpmap;
			findimpmap = parbone->FindImpMap( curimpname );
			if( findimpmap != parbone->GetImpMapEnd() ){
				map<CBone*,ChaVector3>::iterator itrimp;
				itrimp = findimpmap->second.find( srcbone );
				if( itrimp != findimpmap->second.end() ){
					setimp = itrimp->second;
				}
			}
		}
		else{
			//_ASSERT(0);
		}

		CRigidElem* curre = parbone->GetRigidElem( srcbone );
		if( curre ){
			ChaVector3 imp = setimp * g_impscale;

			CBtObject* findbto = FindBtObject( srcbone->GetBoneNo() );
			if( findbto && findbto->GetRigidBody() ){
				findbto->GetRigidBody()->applyImpulse( btVector3( imp.x, imp.y, imp.z ), btVector3( 0.0f, 0.0f, 0.0f ) );
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if( srcbone->GetChild() ){
		SetBtImpulseReq( srcbone->GetChild() );
	}
	if( srcbone->GetBrother() ){
		SetBtImpulseReq( srcbone->GetBrother() );
	}
}

CBtObject* CModel::FindBtObject( int srcboneno )
{
	CBtObject* retbto = 0;
	if( !m_topbt ){
		return 0;
	}
	FindBtObjectReq( m_topbt, srcboneno, &retbto );

	return retbto;
}

void CModel::FindBtObjectReq( CBtObject* srcbto, int srcboneno, CBtObject** ppret )
{
	if( *ppret ){
		return;
	}

	if( srcbto->GetBone() ){
		CBone* curbone;
		curbone = m_bonelist[ srcboneno ];
		if( curbone ){
			CBone* parbone;
			parbone = curbone->GetParent();
			if( parbone ){
				if( (srcbto->GetBone() == parbone) && (srcbto->GetEndBone() == curbone) ){
					*ppret = srcbto;
					return;
				}
			}
		}
	}

	int chilno;
	for( chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChilBt( chilno );
		FindBtObjectReq( chilbto, srcboneno, ppret );
	}
}
int CModel::SetDispFlag( char* srcobjname, int srcflag )
{
	CMQOObject* curobj = m_objectname[ srcobjname ];
	if( curobj ){
		curobj->SetDispFlag( srcflag );
	}

	return 0;
}

int CModel::EnableAllRigidElem(int srcrgdindex)
{
	if (!m_topbone){
		return 0;
	}

	EnableAllRigidElemReq(m_topbone, srcrgdindex);

	return 0;
}
int CModel::DisableAllRigidElem(int srcrgdindex)
{
	if (!m_topbone){
		return 0;
	}

	DisableAllRigidElemReq(m_topbone, srcrgdindex);

	return 0;
}

void CModel::EnableAllRigidElemReq(CBone* srcbone, int srcrgdindex)
{
	if ((srcrgdindex >= 0) && (srcrgdindex < (int)m_rigideleminfo.size())){
		if (srcbone->GetParent()){
			char* filename = m_rigideleminfo[srcrgdindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				//if (curre->GetBoneLeng() >= 0.00001f){
				curre->SetSkipflag(0);
				//}
			}
		}
	}
	else{
		_ASSERT(0);
	}

	if (srcbone->GetChild()){
		EnableAllRigidElemReq(srcbone->GetChild(), srcrgdindex);
	}
	if (srcbone->GetBrother()){
		EnableAllRigidElemReq(srcbone->GetBrother(), srcrgdindex);
	}

}
void CModel::DisableAllRigidElemReq(CBone* srcbone, int srcrgdindex)
{
	if ((srcrgdindex >= 0) && (srcrgdindex < (int)m_rigideleminfo.size())){
		if (srcbone->GetParent()){
			char* filename = m_rigideleminfo[srcrgdindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				curre->SetSkipflag(1);
			}
		}
	}
	else{
		_ASSERT(0);
	}

	if (srcbone->GetChild()){
		DisableAllRigidElemReq(srcbone->GetChild(), srcrgdindex);
	}
	if (srcbone->GetBrother()){
		DisableAllRigidElemReq(srcbone->GetBrother(), srcrgdindex);
	}
}

int CModel::SetAllBtgData( int gid, int reindex, float btg )
{
	if( !m_topbone ){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetBtgDataReq( gid, reindex, m_topbone, btg );

	return 0;
}
void CModel::SetBtgDataReq( int gid, int reindex, CBone* srcbone, float btg )
{
	if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
		if (srcbone->GetParent()){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					curre->SetBtg(btg);
				}
			}
		}
	}
	else{
		_ASSERT(0);
	}

	if( srcbone->GetChild() ){
		SetBtgDataReq( gid, reindex, srcbone->GetChild(), btg );
	}
	if( srcbone->GetBrother() ){
		SetBtgDataReq( gid, reindex, srcbone->GetBrother(), btg );
	}
}

int CModel::SetAllDampAnimData( int gid, int rgdindex, float valL, float valA )
{
	if( !m_topbone ){
		return 0;
	}
	if( rgdindex < 0 ){
		return 0;
	}

	SetDampAnimDataReq( gid, rgdindex, m_topbone, valL, valA );
	return 0;
}
void CModel::SetDampAnimDataReq( int gid, int rgdindex, CBone* srcbone, float valL, float valA )
{
	if( rgdindex < 0 ){
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())){
		if (srcbone->GetParent()){
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					curre->SetDampanimL(valL);
					curre->SetDampanimA(valA);
				}
			}
		}
	}
	else{
		_ASSERT(0);
	}

	if( srcbone->GetChild() ){
		SetDampAnimDataReq( gid, rgdindex, srcbone->GetChild(), valL, valA );
	}
	if( srcbone->GetBrother() ){
		SetDampAnimDataReq( gid, rgdindex, srcbone->GetBrother(), valL, valA );
	}
}


int CModel::SetAllImpulseData( int gid, float impx, float impy, float impz )
{
	if( !m_topbone ){
		return 0;
	}
	ChaVector3 srcimp = ChaVector3( impx, impy, impz );

	SetImpulseDataReq( gid, m_topbone, srcimp );

	return 0;
}

void CModel::SetImpulseDataReq( int gid, CBone* srcbone, ChaVector3 srcimp )
{
	if ((m_rgdindex < 0) || (m_rgdindex >= (int)m_rigideleminfo.size())){
		return;
	}

	CBone* parbone = srcbone->GetParent();

	if( parbone ){
		int renum = (int)m_rigideleminfo.size();
		int impnum = parbone->GetImpMapSize();
		if( (m_curimpindex >= 0) && (m_curimpindex < impnum) && (m_curreindex >= 0) && (m_curreindex < renum) ){

			char* filename = m_rigideleminfo[m_rgdindex].filename;//!!! rgdindex !!!
			CRigidElem* curre = parbone->GetRigidElemOfMap( filename, srcbone );
			if( curre ){
				if( (gid == -1) || (gid == curre->GetGroupid()) ){
					string curimpname = m_impinfo[ m_curimpindex ];
					map<string, map<CBone*, ChaVector3>>::iterator findimpmap;
					findimpmap = parbone->FindImpMap( curimpname );
					if( findimpmap != parbone->GetImpMapEnd() ){
						map<CBone*,ChaVector3>::iterator itrimp;
						itrimp = findimpmap->second.find( srcbone );
						if( itrimp != findimpmap->second.end() ){
							itrimp->second = srcimp;
						}
					}
				}
			}
		}
	}

	if( srcbone->GetChild() ){
		SetImpulseDataReq( gid, srcbone->GetChild(), srcimp );
	}
	if( srcbone->GetBrother() ){
		SetImpulseDataReq( gid, srcbone->GetBrother(), srcimp );
	}
}

int CModel::SetImp( int srcboneno, int kind, float srcval )
{
	if( !m_topbone ){
		return 0;
	}
	if( srcboneno < 0 ){
		return 0;
	}
	if( m_curimpindex < 0 ){
		return 0;
	}

	CBone* curbone = m_bonelist[srcboneno];
	if( !curbone ){
		return 0;
	}

	CBone* parbone = curbone->GetParent();
	if( !parbone ){
		return 0;
	}

	int impnum = parbone->GetImpMapSize();
	if( m_curimpindex >= impnum ){
		//_ASSERT( 0 );
		return 0;
	}

	string curimpname = m_impinfo[ m_curimpindex ];
	if( parbone->GetImpMapSize2( curimpname ) > 0 ){
		map<string, map<CBone*, ChaVector3>>::iterator itrfindmap;
		itrfindmap = parbone->FindImpMap( curimpname );
		map<CBone*,ChaVector3>::iterator itrimp;
		itrimp = itrfindmap->second.find( curbone );
		if( itrimp == itrfindmap->second.end() ){
			if (kind == 0){
				itrfindmap->second[curbone].x = srcval;
			}
			else if (kind == 1){
				itrfindmap->second[curbone].y = srcval;
			}
			else if (kind == 2){
				itrfindmap->second[curbone].z = srcval;
			}
		}
		else{
			if (kind == 0){
				itrimp->second.x = srcval;
			}
			else if (kind == 1){
				itrimp->second.y = srcval;
			}
			else if (kind == 2){
				itrimp->second.z = srcval;
			}
		}
	
	}else{
		_ASSERT( 0 );
	}

	return 0;
}


int CModel::SetAllDmpData( int gid, int reindex, float ldmp, float admp )
{
	if( !m_topbone ){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetDmpDataReq( gid, reindex, m_topbone, ldmp, admp );

	return 0;
}
void CModel::SetDmpDataReq( int gid, int reindex, CBone* srcbone, float ldmp, float admp )
{
	if( srcbone->GetParent() ){
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					curre->SetLDamping(ldmp);
					curre->SetADamping(admp);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if( srcbone->GetChild() ){
		SetDmpDataReq( gid, reindex, srcbone->GetChild(), ldmp, admp );
	}
	if( srcbone->GetBrother() ){
		SetDmpDataReq( gid, reindex, srcbone->GetBrother(), ldmp, admp );
	}
}

int CModel::SetColTypeAll(int reindex, int srctype)
{
	if (!m_topbone){
		return 0;
	}
	if (reindex < 0){
		return 0;
	}

	if ((srctype >= COL_CONE_INDEX) && (srctype < COL_MAX)){
		SetColTypeReq(reindex, m_topbone, srctype);
	}
	else{
		_ASSERT(0);
	}

	return 0;

}

void CModel::SetColTypeReq(int reindex, CBone* srcbone, int srctype)
{
	if (srcbone->GetParent()){
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				curre->SetColtype(srctype);
				curre->SetColtype(srctype);
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if (srcbone->GetChild()){
		SetColTypeReq(reindex, srcbone->GetChild(), srctype);
	}
	if (srcbone->GetBrother()){
		SetColTypeReq(reindex, srcbone->GetBrother(), srctype);
	}
}



int CModel::SetAllRestData( int gid, int reindex, float rest, float fric )
{
	if( !m_topbone ){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetRestDataReq( gid, reindex, m_topbone, rest, fric );

	return 0;
}

void CModel::SetRestDataReq( int gid, int reindex, CBone* srcbone, float rest, float fric )
{
	if( srcbone->GetParent() ){
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					curre->SetRestitution(rest);
					curre->SetFriction(fric);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if( srcbone->GetChild() ){
		SetRestDataReq( gid, reindex, srcbone->GetChild(), rest, fric );
	}
	if( srcbone->GetBrother() ){
		SetRestDataReq( gid, reindex, srcbone->GetBrother(), rest, fric );
	}
}

int CModel::SetAllKData( int gid, int reindex, int srclk, int srcak, float srccuslk, float srccusak )
{
	if( !m_topbone ){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetKDataReq( gid, reindex, m_topbone, srclk, srcak, srccuslk, srccusak );

	return 0;
}
void CModel::SetKDataReq( int gid, int reindex, CBone* srcbone, int srclk, int srcak, float srccuslk, float srccusak )
{
	if( srcbone->GetParent() ){
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					curre->SetLKindex(srclk);
					curre->SetAKindex(srcak);
					curre->SetCusLk(srccuslk);
					curre->SetCusAk(srccusak);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if( srcbone->GetChild() ){
		SetKDataReq( gid, reindex, srcbone->GetChild(), srclk, srcak, srccuslk, srccusak );
	}
	if( srcbone->GetBrother() ){
		SetKDataReq( gid, reindex, srcbone->GetBrother(), srclk, srcak, srccuslk, srccusak );
	}
}

int CModel::SetAllMassData( int gid, int reindex, float srcmass )
{
	if( !m_topbone ){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetMassDataReq( gid, reindex, m_topbone, srcmass );


	return 0;
}
void CModel::SetMassDataReq( int gid, int reindex, CBone* srcbone, float srcmass )
{
	if( srcbone->GetParent() ){
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					curre->SetMass(srcmass);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if( srcbone->GetChild() ){
		SetMassDataReq( gid, reindex, srcbone->GetChild(), srcmass );
	}
	if( srcbone->GetBrother() ){
		SetMassDataReq( gid, reindex, srcbone->GetBrother(), srcmass );
	}
}

int CModel::SetAllMassDataByBoneLeng(int gid, int reindex, float srcmass)
{
	if (!m_topbone){
		return 0;
	}
	if (reindex < 0){
		return 0;
	}

	SetMassDataByBoneLengReq(gid, reindex, m_topbone, srcmass);


	return 0;
}
void CModel::SetMassDataByBoneLengReq(int gid, int reindex, CBone* srcbone, float srcmass)
{
	if (srcbone->GetParent()){
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					ChaVector3 parpos, curpos, diffpos;
					parpos = srcbone->GetParent()->GetJointFPos();
					curpos = srcbone->GetJointFPos();
					diffpos = curpos - parpos;
					float leng = ChaVector3Length(&diffpos);
					float setmass = srcmass * leng;

					curre->SetMass(setmass);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if (srcbone->GetChild()){
		SetMassDataByBoneLengReq(gid, reindex, srcbone->GetChild(), srcmass);
	}
	if (srcbone->GetBrother()){
		SetMassDataByBoneLengReq(gid, reindex, srcbone->GetBrother(), srcmass);
	}
}

int CModel::SetRagdollKinFlag(int selectbone, int physicsmvkind)
{

	if( !m_topbt ){
		return 0;
	}

	SetRagdollKinFlagReq( m_topbt, selectbone, physicsmvkind );

	


	return 0;
}
void CModel::SetRagdollKinFlagReq(CBtObject* srcbto, int selectbone, int physicsmvkind)
{

	CBone* srcbone = srcbto->GetBone();
	if (srcbone){

		CBone* kinchildbone = GetBoneByID(selectbone);
		if (kinchildbone){
			CBone* kinbone = kinchildbone->GetParent();
			if (kinbone){
				int curboneno = srcbone->GetBoneNo();
				if (curboneno != kinbone->GetBoneNo()){
					srcbone->SetBtKinFlag(0);
				}
				else{
					srcbone->SetBtKinFlag(1);
				}

			}
		}
		else{
			srcbone->SetBtKinFlag(0);
		}

		//if ((physicsmvkind == 0) && (srcbone->GetBtKinFlag() == 1) && (srcbto->GetRigidBody())){
		if ((srcbone->GetBtKinFlag() == 1) && (srcbto->GetRigidBody())){
			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
			if (s_setrigidflag == 0){
				s_rigidflag = curflag;
				s_setrigidflag = 1;
			}
			//srcbto->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			srcbto->GetRigidBody()->setCollisionFlags(curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
			//srcbto->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
			//srcbto->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
			//srcbto->m_rigidbody->setActivationState(DISABLE_SIMULATION);
			//CF_STATIC_OBJECT
		}
		else if (srcbto->GetRigidBody()){
			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
			srcbto->GetRigidBody()->setCollisionFlags(curflag & ~btCollisionObject::CF_KINEMATIC_OBJECT);

			if (srcbone->GetParent()){
				CRigidElem* curre = srcbone->GetParent()->GetRigidElem(srcbone);
				if (curre){
					if ((m_curreindex >= 0) && (m_curreindex < (int)m_rigideleminfo.size())){
						//srcbto->GetRigidBody()->setGravity(btVector3(0.0f, curre->GetBtg() * m_rigideleminfo[m_curreindex].btgscale, 0.0f));
						srcbto->GetRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
						srcbto->GetRigidBody()->applyGravity();
					}
					else{
						_ASSERT(0);
					}
				}
			}
		}

		if (srcbto->GetRigidBody()){
			//srcbto->GetRigidBody()->activate();
			//###srcbto->GetRigidBody()->setDeactivationTime(0.0);
			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);

			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 150.0);
			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
		}
	}



	int chilno;
	for( chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChilBt( chilno );
		if( chilbto ){
			SetRagdollKinFlagReq( chilbto, selectbone );
		}
	}
}

int CModel::SetCurrentRigidElem( int curindex )
{
	if( curindex < 0 ){
		return 0;
	}
	if( curindex >= (int)m_rigideleminfo.size() ){
		_ASSERT( 0 );
		return 0;
	}

	m_curreindex = curindex;

	string curname = m_rigideleminfo[ curindex ].filename;


	SetCurrentRigidElemReq(m_topbone, curname);


	/*
	map<int,CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if (curbone){
			CallF(curbone->SetCurrentRigidElem(curname), return 1);
		}
	}
	*/
	return 0;
}


void CModel::SetCurrentRigidElemReq(CBone* srcbone, string curname)
{
	if (srcbone){

		srcbone->SetCurrentRigidElem(curname);


		if (srcbone->GetChild()){
			SetCurrentRigidElemReq(srcbone->GetChild(), curname);
		}
		if (srcbone->GetBrother()){
			SetCurrentRigidElemReq(srcbone->GetBrother(), curname);
		}

	}
}


int CModel::MultDispObj( ChaVector3 srcmult, ChaVector3 srctra )
{

	map<int,CMQOObject*>::iterator itrobj;
	for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		if( curobj ){
			CallF( curobj->MultScale( srcmult, srctra ), return 1 );			
		}
	}

	return 0;
}

int CModel::SetColiIDtoGroup( CRigidElem* srcre )
{
	if( !m_topbone ){
		return 0;
	}
	if( m_curreindex < 0 ){
		return 0;
	}

	SetColiIDReq( m_topbone, srcre );

	return 0;
}
void CModel::SetColiIDReq( CBone* srcbone, CRigidElem* srcre )
{
	if( srcbone->GetParent() ){
		if ((m_curreindex >= 0) && (m_curreindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[m_curreindex].filename;
			CRigidElem* curre = srcbone->GetParent()->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if (curre->GetGroupid() == srcre->GetGroupid()){
					curre->CopyColiids(srcre);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if( srcbone->GetChild() ){
		SetColiIDReq( srcbone->GetChild(), srcre );
	}
	if( srcbone->GetBrother() ){
		SetColiIDReq( srcbone->GetBrother(), srcre );
	}
}

int CModel::ResetBt()
{
	if( !m_topbt ){
		return 0;
	}

	ResetBtReq( m_topbt );

	return 0;
}

void CModel::ResetBtReq( CBtObject* curbto )
{
	if( curbto->GetRigidBody() ){
		if (curbto->GetRigidBody()->getMotionState())
		{
			btDefaultMotionState* myMotionState = (btDefaultMotionState*)curbto->GetRigidBody()->getMotionState();
			myMotionState->m_graphicsWorldTrans = myMotionState->m_startWorldTrans;
			curbto->GetRigidBody()->setCenterOfMassTransform( myMotionState->m_graphicsWorldTrans );
			curbto->GetRigidBody()->setInterpolationWorldTransform( myMotionState->m_startWorldTrans );
			curbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
			curbto->GetRigidBody()->activate();
			curbto->GetRigidBody()->setDeactivationTime(0.0);
			//curbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);

			//colObj->setActivationState(WANTS_DEACTIVATION);
		}
		if (curbto->GetRigidBody() && !curbto->GetRigidBody()->isStaticObject())
		{
			curbto->GetRigidBody()->setLinearVelocity(btVector3(0,0,0));
			curbto->GetRigidBody()->setAngularVelocity(btVector3(0,0,0));
		}
	}

	int chilno;
	for( chilno = 0; chilno < curbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = curbto->GetChilBt( chilno );
		if( chilbto ){
			ResetBtReq( chilbto );
		}
	}
}

int CModel::SetBefEditMat( CEditRange* erptr, CBone* curbone, int maxlevel )
{
	int levelcnt0 = 0;
	while( curbone && ((maxlevel == 0) || (levelcnt0 < (maxlevel+1))) )
	{
		int keynum;
		double startframe, endframe, applyframe;
		erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
		double curframe;
		for (curframe = startframe; curframe <= endframe; curframe += 1.0){
			CMotionPoint* editmp = 0;
			editmp = curbone->GetMotionPoint(m_curmotinfo->motid, curframe);
			if(editmp){
				if (g_previewFlag != 5){
					editmp->SetBefEditMat(editmp->GetWorldMat());
				}
				else{
					editmp->SetBefEditMat(curbone->GetBtMat());
				}
			}else{
				_ASSERT( 0 );
			}
		}
		curbone = curbone->GetParent();
		levelcnt0++;
	}
	return 0;
}

int CModel::SetBefEditMatFK( CEditRange* erptr, CBone* curbone )
{
	if (curbone){

		int keynum;
		double startframe, endframe, applyframe;
		erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
		double curframe;
		for (curframe = startframe; curframe <= endframe; curframe += 1.0){

			CMotionPoint* editmp = 0;
			editmp = curbone->GetMotionPoint(m_curmotinfo->motid, curframe);
			if (editmp){
				editmp->SetBefEditMat(editmp->GetWorldMat());
			}
			else{
				_ASSERT(0);
			}
		}
	}
	return 0;
}

int CModel::IKRotate( CEditRange* erptr, int srcboneno, ChaVector3 targetpos, int maxlevel )
{

	CBone* firstbone = m_bonelist[ srcboneno ];
	if( !firstbone ){
		_ASSERT( 0 );
		return -1;
	}


	ChaVector3 ikaxis = g_camtargetpos - g_camEye;
	ChaVector3Normalize( &ikaxis, &ikaxis );

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange( &keynum, &startframe, &endframe, &applyframe );

	CBone* curbone = firstbone;
	SetBefEditMat( erptr, curbone, maxlevel );

	CBone* lastpar = firstbone->GetParent();
	curbone = firstbone;
	int calcnum = 3;
	int calccnt;
	for( calccnt = 0; calccnt < calcnum; calccnt++ ){
		int levelcnt = 0;

		float currate = 1.0f;

		CBone* curbone = firstbone;
		while( curbone && ((maxlevel == 0) || (levelcnt < maxlevel)) )
		{
			CBone* parbone = curbone->GetParent();
			if( parbone && (curbone->GetJointFPos() != parbone->GetJointFPos()) ){
				ChaVector3 parworld, chilworld;
				//D3DVec3TransformCoord( &chilworld, &(curbone->GetJointFPos()), &(curbone->GetCurMp().GetWorldMat()) );
				D3DVec3TransformCoord( &parworld, &(parbone->GetJointFPos()), &(parbone->GetCurMp().GetWorldMat()) );
				D3DVec3TransformCoord(&chilworld, &(curbone->GetJointFPos()), &(parbone->GetCurMp().GetWorldMat()));

				ChaVector3 parbef, chilbef, tarbef;
				parbef = parworld;
				CalcShadowToPlane( chilworld, ikaxis, parworld, &chilbef );
				CalcShadowToPlane( targetpos, ikaxis, parworld, &tarbef );

				ChaVector3 vec0, vec1;
				vec0 = chilbef - parbef;
				ChaVector3Normalize( &vec0, &vec0 );
				vec1 = tarbef - parbef;
				ChaVector3Normalize( &vec1, &vec1 );

				ChaVector3 rotaxis2;
				ChaVector3Cross( &rotaxis2, &vec0, &vec1 );
				ChaVector3Normalize( &rotaxis2, &rotaxis2 );

				float rotdot2, rotrad2;
				rotdot2 = ChaVector3Dot( &vec0, &vec1 );
				rotdot2 = min( 1.0f, rotdot2 );
				rotdot2 = max( -1.0f, rotdot2 );
				rotrad2 = (float)acos( rotdot2 );
				rotrad2 *= currate;
				double firstframe = 0.0;
				if( fabs( rotrad2 ) > 1.0e-4 ){
					CQuaternion rotq0;
					rotq0.SetAxisAndRot( rotaxis2, rotrad2 );
					CQuaternion rotq;

					if( keynum >= 2 ){
						int keyno = 0;
						double curframe;
						for (curframe = startframe; curframe <= endframe; curframe += 1.0){
							CMotionPoint* curparmp;
							curparmp = parbone->GetMotionPoint(m_curmotinfo->motid, curframe);
							CMotionPoint* aplyparmp;
							aplyparmp = parbone->GetMotionPoint(m_curmotinfo->motid, applyframe);
							if (curparmp && aplyparmp && (g_pseudolocalflag == 1)){
								ChaMatrix curparrotmat = curparmp->GetWorldMat();
								curparrotmat._41 = 0.0f;
								curparrotmat._42 = 0.0f;
								curparrotmat._43 = 0.0f;
								ChaMatrix invcurparrotmat = curparmp->GetInvWorldMat();
								invcurparrotmat._41 = 0.0f;
								invcurparrotmat._42 = 0.0f;
								invcurparrotmat._43 = 0.0f;
								ChaMatrix aplyparrotmat = aplyparmp->GetWorldMat();
								aplyparrotmat._41 = 0.0f;
								aplyparrotmat._42 = 0.0f;
								aplyparrotmat._43 = 0.0f;
								ChaMatrix invaplyparrotmat = aplyparmp->GetInvWorldMat();
								invaplyparrotmat._41 = 0.0f;
								invaplyparrotmat._42 = 0.0f;
								invaplyparrotmat._43 = 0.0f;

								ChaMatrix transmat2;
								transmat2 = invcurparrotmat * aplyparrotmat * rotq0.MakeRotMatX() * invaplyparrotmat * curparrotmat;
								CMotionPoint transmp;
								transmp.CalcQandTra(transmat2, firstbone);
								rotq = transmp.GetQ();
							}
							else{
								rotq = rotq0;
							}

							double changerate;
							if( curframe <= applyframe ){
								changerate = 1.0 / (applyframe - startframe + 1);
							}else{
								changerate = 1.0 / (endframe - applyframe + 1);
							}


							if( keyno == 0 ){
								firstframe = curframe;
							}
							if( g_absikflag == 0 ){
								if( g_slerpoffflag == 0 ){
									CQuaternion endq;
									endq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
									CQuaternion curq;
									double currate2;
									if( curframe <= applyframe ){
										currate2 = changerate * (curframe - startframe + 1);
									}else{
										currate2 = changerate * (endframe - curframe + 1);
									}
									rotq.Slerp2( endq, 1.0 - currate2, &curq );

									parbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, curq );
								}else{
									parbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, rotq );
								}
							}else{
								if( keyno == 0 ){
									parbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, rotq );
								}else{
									parbone->SetAbsMatReq( 0, m_curmotinfo->motid, curframe, firstframe );
								}
							}
							keyno++;
						}
					}else{
						rotq = rotq0;
						parbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
					}


					if( g_applyendflag == 1 ){
						//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
						int tolast;
						for( tolast = (int)m_curmotinfo->curframe + 1; tolast < m_curmotinfo->frameleng; tolast++ ){
							(m_bonelist[ 0 ])->PasteRotReq( m_curmotinfo->motid, m_curmotinfo->curframe, tolast );
						}
					}

				}

			}


			if( curbone->GetParent() ){
				lastpar = curbone->GetParent();
			}
			curbone = curbone->GetParent();

			levelcnt++;

			currate = pow( g_ikrate, g_ikfirst * levelcnt );
		}

		if( (calccnt == (calcnum - 1)) && g_absikflag && lastpar ){
			AdjustBoneTra( erptr, lastpar );
		}
	}

	if( lastpar ){
		return lastpar->GetBoneNo();
	}else{
		return srcboneno;
	}

}


int CModel::PhysicsRot(CEditRange* erptr, int srcboneno, ChaVector3 targetpos, int maxlevel)
{

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone){
		_ASSERT(0);
		return -1;
	}

	CBone* curbone = firstbone;
	CBone* parbone = curbone->GetParent();
	if (!parbone){
		return 0;
	}
	if (!parbone->GetParent()){
		//grand parentがルートボーンの場合に、まだうまくいかないのでスキップ
		return 0;
	}


	ChaVector3 ikaxis = g_camtargetpos - g_camEye;
	ChaVector3Normalize(&ikaxis, &ikaxis);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	SetBefEditMat(erptr, curbone, maxlevel);

	float currate = g_physicsmvrate;





	if (parbone){
		CBone* gparbone = parbone->GetParent();
		CBone* childbone = parbone->GetChild();
		int isfirst = 1;
		while (childbone){

			if (childbone->GetJointFPos() != parbone->GetJointFPos()){
				ChaVector3 parworld, chilworld;
				D3DVec3TransformCoord(&parworld, &(parbone->GetJointFPos()), &(parbone->GetBtMat()));
				D3DVec3TransformCoord(&chilworld, &(childbone->GetJointFPos()), &(parbone->GetBtMat()));

				ChaVector3 parbef, chilbef, tarbef;
				parbef = parworld;
				CalcShadowToPlane(chilworld, ikaxis, parworld, &chilbef);
				CalcShadowToPlane(targetpos, ikaxis, parworld, &tarbef);

				ChaVector3 vec0, vec1;
				vec0 = chilbef - parbef;
				ChaVector3Normalize(&vec0, &vec0);
				vec1 = tarbef - parbef;
				ChaVector3Normalize(&vec1, &vec1);

				ChaVector3 rotaxis2;
				ChaVector3Cross(&rotaxis2, &vec0, &vec1);
				ChaVector3Normalize(&rotaxis2, &rotaxis2);

				float rotdot2, rotrad2;
				rotdot2 = ChaVector3Dot(&vec0, &vec1);
				rotdot2 = min(1.0f, rotdot2);
				rotdot2 = max(-1.0f, rotdot2);
				rotrad2 = (float)acos(rotdot2);
				rotrad2 *= currate;
				double firstframe = 0.0;
				if (fabs(rotrad2) > 1.0e-4){
					CQuaternion rotq;
					CQuaternion rotq0;
					rotq0.SetAxisAndRot(rotaxis2, rotrad2);


					ChaMatrix gparrotmat, invgparrotmat;
					CQuaternion gparrotq;
					if (parbone->GetParent()){
						gparrotmat = parbone->GetParent()->GetBtMat();
						ChaMatrixInverse(&invgparrotmat, NULL, &gparrotmat);

						gparrotmat._41 = 0.0f;
						gparrotmat._42 = 0.0f;
						gparrotmat._43 = 0.0f;

						invgparrotmat._41 = 0.0f;
						invgparrotmat._42 = 0.0f;
						invgparrotmat._43 = 0.0f;

						CMotionPoint tmpmp;
						tmpmp.CalcQandTra(gparrotmat, 0);
						gparrotq = tmpmp.GetQ();

					}
					else{
						ChaMatrixIdentity(&gparrotmat);
						ChaMatrixIdentity(&invgparrotmat);
						gparrotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
					}
					ChaMatrix parrotmat, invparrotmat;
					parrotmat = parbone->GetBtMat();
					ChaMatrixInverse(&invparrotmat, NULL, &parrotmat);


					ChaMatrix transmat2;
					transmat2 = rotq0.MakeRotMatX();

					CMotionPoint transmp;
					transmp.CalcQandTra(transmat2, parbone);
					rotq = transmp.GetQ();


					ChaMatrix newbtmat;
					ChaVector3 rotcenter;
					//rotcenter = parworld;
					rotcenter = parbone->GetJointFPos();

					ChaMatrix befrot, aftrot;
					ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
					ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
					ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
					//newbtmat = parbone->GetBtMat() * rotmat;// *tramat;
					//newbtmat = parbone->GetBtMat() * gparbone->GetInvBtMat() * rotmat * gparbone->GetBtMat();// *tramat;
					newbtmat = rotmat * parbone->GetBtMat();// *tramat;



					ChaMatrix firstworld = parbone->GetStartMat2();
					ChaMatrix invfirstworld;
					ChaMatrixInverse(&invfirstworld, NULL, &firstworld);

					ChaMatrix diffworld = invfirstworld * newbtmat;
					CRigidElem* curre = parbone->GetRigidElem(childbone);
					ChaMatrix newrigidmat;
					if (curre){
						newrigidmat = curre->GetFirstcapsulemat() * diffworld;
					}
					else{
						::MessageBoxA(NULL, "IKRotateRagdoll : curre NULL error", "error", MB_OK);
						return -1;
					}


					ChaVector3 newparpos, newchilpos;
					ChaVector3 jointfpos;
					jointfpos = parbone->GetJointFPos();
					D3DVec3TransformCoord(&newparpos, &jointfpos, &newbtmat);
					jointfpos = childbone->GetJointFPos();
					D3DVec3TransformCoord(&newchilpos, &jointfpos, &newbtmat);

					ChaVector3 rigidcenter = (newparpos + newchilpos) * 0.5f;


					CQuaternion tmpq;
					tmpq.RotationMatrix(newrigidmat);


					btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);


					btTransform worldtra;
					worldtra.setIdentity();
					worldtra.setRotation(btrotq);
					worldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));

//角度制限　ここから
					if (gparbone){
						CBtObject* parbto = gparbone->GetBtObject(parbone);
						if (parbto){
							CBtObject* setbto = parbone->GetBtObject(childbone);
							if (setbto){
								btMatrix3x3 firstworldmat = setbto->GetFirstTransformMat();

								btGeneric6DofSpringConstraint* dofC = parbto->FindConstraint(parbone, childbone);
								if (dofC){

									//constraint変化分　以下3行　　CreateBtObjectをしたときの状態を基準にした角度になっている。つまりシミュ開始時が０度。
									dofC->calculateTransforms();
									//btTransform contraA = dofC->getCalculatedTransformA();
									//btTransform contraB = dofC->getCalculatedTransformB();
									//btMatrix3x3 eulmat = contraA.getBasis().inverse() * contraB.getBasis() * contraA.getBasis();


									////親ボーンとの角度がオイラー角に入る
									//btTransform contraA = dofC->getCalculatedTransformA();
									//btTransform contraB = dofC->getCalculatedTransformB();
									//btTransform parworldtra;
									//parbto->GetRigidBody()->getMotionState()->getWorldTransform(parworldtra);
									//btMatrix3x3 diffmat = worldtra.getBasis() * parworldtra.getBasis().inverse();
									//btMatrix3x3 eulmat = contraA.getBasis().inverse() * diffmat * contraA.getBasis();

									btTransform contraA = dofC->getCalculatedTransformA();
									btTransform contraB = dofC->getCalculatedTransformB();
									btMatrix3x3 diffmat = firstworldmat.inverse() * worldtra.getBasis();
									btMatrix3x3 eulmat = contraA.getBasis().inverse() * diffmat * contraA.getBasis();



									btScalar eulz = 0.0;
									btScalar euly = 0.0;
									btScalar eulx = 0.0;
									ChaVector3 eul = ChaVector3(0.0f, 0.0f, 0.0f);
									//worldtra.getBasis().getEulerZYX(eulz, euly, eulx, 1);
									eulmat.getEulerZYX(eulz, euly, eulx, 1);
									eul.x = eulx * 180.0 / PAI;
									eul.y = euly * 180.0 / PAI;
									eul.z = eulz * 180.0 / PAI;

									int ismovable = parbone->ChkMovableEul(eul);
									char strmsg[256];
									//sprintf_s(strmsg, 256, "needmodify 0 : neweul [%f, %f, %f] : dof [%f, %f, %f] : ismovable %d\n", eul.x, eul.y, eul.z, dofx, dofy, dofz, ismovable);
									sprintf_s(strmsg, 256, "needmodify 0 : neweul [%f, %f, %f] : ismovable %d\n", eul.x, eul.y, eul.z, ismovable);
									OutputDebugStringA(strmsg);


									//Q2EulZYXbtのテスト　以下8行
									CQuaternion eulq;
									eulq.MakeFromBtMat3x3(eulmat);
									int needmodifyflag = 0;
									ChaVector3 testbefeul = ChaVector3(0.0f, 0.0f, 0.0f);
									ChaVector3 testeul = ChaVector3(0.0f, 0.0f, 0.0f);
									//eulq.Q2EulZYXbt(needmodifyflag, 0, testbefeul, &testeul);
									eulq.Q2EulXYZ(0, testbefeul, &testeul);//bulletの回転順序は数値検証の結果XYZ。(ZYXではない)。
									sprintf_s(strmsg, 256, "testeul [%f, %f, %f]\n", testeul.x, testeul.y, testeul.z);
									OutputDebugStringA(strmsg);


									if (ismovable != 1){
										childbone = childbone->GetBrother();
										continue;
									}

									setbto->GetRigidBody()->getMotionState()->setWorldTransform(worldtra);
									//setbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
									//setbto->GetRigidBody()->setDeactivationTime(30000.0);
									setbto->GetRigidBody()->setDeactivationTime(0.0);

									if (isfirst == 1){
										parbone->SetBtMat(newbtmat);
										//IKボーンはKINEMATICだから。
										parbone->GetCurMp().SetWorldMat(newbtmat);
										isfirst = 0;
									}

								}
							}
						}
//角度制限　ここまで

					}
				}

			}

			childbone = childbone->GetBrother();
		}
	}
	return srcboneno;

}

int CModel::PhysicsRotAxisDelta(CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
{
	if (!m_curmotinfo){
		return 0;
	}


	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone){
		return 0;
	}
	CBone* parbone = curbone->GetParent();
	if (!parbone){
		return 0;
	}
	if (!parbone->GetParent()){
		//grand parentがルートボーンの場合に、まだうまくいかないのでスキップ
		return 0;
	}


	int calcnum = 3;

	float rotrad = delta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
	if (fabs(rotrad) < (0.02f * (float)DEG2PAI)){
		return 0;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	SetBefEditMat(erptr, curbone, maxlevel);//!!!!!!!!!!!!

	if (parbone){
		CBone* gparbone = parbone->GetParent();
		CBone* childbone = parbone->GetChild();
		int isfirst = 1;
		float currate = 1.0f;
		double firstframe = 0.0;
		int levelcnt = 0;

		while (childbone){
			float rotrad2 = currate * rotrad;
			//float rotrad2 = rotrad;
			if (fabs(rotrad2) < (0.02f * (float)DEG2PAI)){
				continue;
			}


			//角度制限　ここから
			if (gparbone){
				CBtObject* parbto = gparbone->GetBtObject(parbone);
				if (parbto){
					CBtObject* setbto = parbone->GetBtObject(childbone);
					if (setbto){
						//btMatrix3x3 firstworldmat = setbto->GetFirstTransformMat();
						btTransform firstworldtra = setbto->GetFirstTransform();
						btTransform invfirstworldtra = firstworldtra.inverse();
						ChaMatrix firstworldmatx = setbto->GetFirstTransformMatX();

						ChaMatrix firstlocalmat = setbto->GetFirstTransformMatX() * ChaMatrixInv(parbto->GetFirstTransformMatX());
						CQuaternion firstlocalq;
						firstlocalq.MakeFromD3DXMat(firstlocalmat);

						btGeneric6DofSpringConstraint* dofC = parbto->FindConstraint(parbone, childbone);
						if (dofC){

							//constraint変化分　以下3行　　CreateBtObjectをしたときの状態を基準にした角度になっている。つまりシミュ開始時が０度。
							//btTransform contraA = dofC->getCalculatedTransformA();
							//btTransform contraB = dofC->getCalculatedTransformB();
							//btMatrix3x3 eulmat = contraA.getBasis().inverse() * contraB.getBasis() * contraA.getBasis();


							////親ボーンとの角度がオイラー角に入る
							//btTransform contraA = dofC->getCalculatedTransformA();
							//btTransform contraB = dofC->getCalculatedTransformB();
							//btTransform parworldtra;
							//parbto->GetRigidBody()->getMotionState()->getWorldTransform(parworldtra);
							//btMatrix3x3 diffmat = worldtra.getBasis() * parworldtra.getBasis().inverse();
							//btMatrix3x3 eulmat = contraA.getBasis().inverse() * diffmat * contraA.getBasis();

//// 新しい回転を求める　ここから
							dofC->calculateTransforms();
							btTransform contraA = dofC->getCalculatedTransformA();
							btTransform contraB = dofC->getCalculatedTransformB();

							btTransform curworldtra;
							curworldtra.setIdentity();
							setbto->GetRigidBody()->getMotionState()->getWorldTransform(curworldtra);
							btTransform parworldtra;
							parworldtra.setIdentity();
							parbto->GetRigidBody()->getMotionState()->getWorldTransform(parworldtra);

							ChaMatrix curlocalmat;
							curlocalmat = ChaMatrixFromBtMat3x3(curworldtra.getBasis()) * ChaMatrixFromBtMat3x3(parworldtra.getBasis().inverse());
							ChaMatrix eulmat = TransZeroMat(ChaMatrixInv(firstlocalmat)) * curlocalmat;

							double eulz = 0.0;
							double euly = 0.0;
							double eulx = 0.0;
							ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
							ChaVector3 eul = ChaVector3(0.0f, 0.0f, 0.0f);
							//worldtra.getBasis().getEulerZYX(eulz, euly, eulx, 1);
							//eulmat.getEulerZYX(eulz, euly, eulx, 1);


							CQuaternion eulq;
							eulq.MakeFromD3DXMat(eulmat);
							btTransform eultra;
							eultra.setIdentity();
							btQuaternion bteulq(eulq.x, eulq.y, eulq.z, eulq.w);
							eultra.setRotation(bteulq);
							eultra.getBasis().getEulerZYX(eulz, euly, eulx, 1);

							//CQuaternion eulq;
							//eulq.MakeFromD3DXMat(eulmat);
							//int needmodifyflag = 0;
							//eulq.Q2EulXYZ(0, befeul, &eul);//bulletの回転順序は数値検証の結果XYZ。(ZYXではない)。

							eul.x = eulx * 180.0 / PAI;
							eul.y = euly * 180.0 / PAI;
							eul.z = eulz * 180.0 / PAI;

							//eulx = eul.x * PAI / 180.0;
							//euly = eul.y * PAI / 180.0;
							//eulz = eul.z * PAI / 180.0;

							if (axiskind == PICK_X){
								eulx +=  -rotrad2;
								eul.x += -rotrad2 * 180.0 / PAI;
							}
							else if (axiskind == PICK_Y){
								euly += -rotrad2;
								eul.y += -rotrad2 * 180.0 / PAI;
							}
							else if (axiskind == PICK_Z){
								eulz += -rotrad2;
								eul.z += -rotrad2 * 180.0 / PAI;
							}

							char strmsg[256];
							sprintf_s(strmsg, 256, "%s : cureul [%f, %f, %f]\n", parbone->GetBoneName(), eul.x, eul.y, eul.z);
							OutputDebugStringA(strmsg);


							btTransform newworldtra;
							newworldtra.setIdentity();
							//newworldtra.getBasis().setEulerZYX(eul.x, eul.y, eul.z);
							CQuaternion currotx;
							CQuaternion curroty;
							CQuaternion currotz;
							currotx.SetAxisAndRot(ChaVector3(1.0f, 0.0f, 0.0f), eulx);
							curroty.SetAxisAndRot(ChaVector3(0.0f, 1.0f, 0.0f), euly);
							currotz.SetAxisAndRot(ChaVector3(0.0f, 0.0f, 1.0f), eulz);
							CQuaternion contraArot;
							CQuaternion contrainvArot;
							CQuaternion firstworldrot;
							contraArot.MakeFromBtMat3x3(contraA.getBasis());
							contrainvArot.MakeFromBtMat3x3(contraA.getBasis().inverse());
							firstworldrot.MakeFromBtMat3x3(firstworldtra.getBasis());
							CQuaternion parrotq;
							parrotq.MakeFromBtMat3x3(parworldtra.getBasis());

							CQuaternion newrotq;
							newrotq = parrotq * currotz * curroty * currotx * firstlocalq;

							btQuaternion btrotq(newrotq.x, newrotq.y, newrotq.z, newrotq.w);
							newworldtra.setRotation(btrotq);


							ChaMatrix newlocalrotmat;
							newlocalrotmat = ChaMatrixFromBtMat3x3(newworldtra.getBasis()) * ChaMatrixFromBtMat3x3(parworldtra.getBasis().inverse());

							ChaMatrix invcurlocalmat;
							ChaMatrixInverse(&invcurlocalmat, NULL, &curlocalmat);

							ChaMatrix difflocalrotmat;
							difflocalrotmat = newlocalrotmat * invcurlocalmat;
							difflocalrotmat = TransZeroMat(difflocalrotmat);

////// 新しい回転を求める　ここまで
//
//// 新しいbtmatを求める　ここから

							ChaMatrix newbtmat;
							ChaVector3 rotcenter;
							rotcenter = parbone->GetJointFPos();

							ChaMatrix befrot, aftrot;
							ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
							ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
							ChaMatrix rotmat = befrot * difflocalrotmat * aftrot;
							newbtmat = rotmat * parbone->GetBtMat();
							//newbtmat = difflocalrotmat * parbone->GetBtMat();

//// 新しいbtmatを求める　ここまで

//// 新しい剛体の中心を求める　ここから

							ChaVector3 newparpos, newchilpos;
							ChaVector3 jointfpos;
							jointfpos = parbone->GetJointFPos();
							D3DVec3TransformCoord(&newparpos, &jointfpos, &newbtmat);
							jointfpos = childbone->GetJointFPos();
							D3DVec3TransformCoord(&newchilpos, &jointfpos, &newbtmat);
							ChaVector3 rigidcenter = (newparpos + newchilpos) * 0.5f;

							newworldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));


							//btTransform invtra = newworldtra.inverse();
							//btVector3 pivotpos = invtra(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));
							//newworldtra.setOrigin(pivotpos);

							sprintf_s(strmsg, 256, "%s : newworldtra Origin [%f, %f, %f]\n", parbone->GetBoneName(), rigidcenter.x, rigidcenter.y, rigidcenter.z);
							OutputDebugStringA(strmsg);

//// 新しい剛体の中心を求める　ここまで

////　角度制限をする　ここから
							ChaMatrix chkeulmat = TransZeroMat(ChaMatrixInv(firstlocalmat)) * newlocalrotmat;

							//ChaVector3 chkbefeul = ChaVector3(0.0f, 0.0f, 0.0f);
							ChaVector3 chkeul = ChaVector3(0.0f, 0.0f, 0.0f);
							//CQuaternion chkeulq;
							//chkeulq.MakeFromD3DXMat(chkeulmat);
							//chkeulq.Q2EulXYZ(0, chkbefeul, &chkeul);//bulletの回転順序は数値検証の結果XYZ。(ZYXではない)。

							btScalar chkeulx, chkeuly, chkeulz;

							CQuaternion chkeulq;
							chkeulq.MakeFromD3DXMat(chkeulmat);
							btTransform chkeultra;
							chkeultra.setIdentity();
							btQuaternion chkbteulq(chkeulq.x, chkeulq.y, chkeulq.z, chkeulq.w);
							chkeultra.setRotation(chkbteulq);
							chkeultra.getBasis().getEulerZYX(chkeulz, chkeuly, chkeulx, 1);

							chkeul.x = chkeulx * 180.0 / PAI;
							chkeul.y = chkeuly * 180.0 / PAI;
							chkeul.z = chkeulz * 180.0 / PAI;

							int ismovable = parbone->ChkMovableEul(chkeul);
							sprintf_s(strmsg, 256, "%s : neweul [%f, %f, %f] : ismovable %d\n", parbone->GetBoneName(), chkeul.x, chkeul.y, chkeul.z, ismovable);
							OutputDebugStringA(strmsg);

							if (ismovable != 1){
								childbone = childbone->GetBrother();
								continue;
							}
//// 角度制限をする　ここまで

//// 設定をする　ここから
							setbto->GetRigidBody()->getMotionState()->setWorldTransform(newworldtra);
							//setbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
							//setbto->GetRigidBody()->setDeactivationTime(30000.0);
							setbto->GetRigidBody()->setDeactivationTime(0.0);

							//if (isfirst == 1){
							//	parbone->SetBtMat(newbtmat);
							//	//IKボーンはKINEMATICだから。
							//	parbone->GetCurMp().SetWorldMat(newbtmat);
							//	isfirst = 0;
							//}
//// 設定をする　ここまで

						}
					}
				}
			}
			childbone = childbone->GetBrother();
		}
	}

	if (curbone){
		return curbone->GetBoneNo();
	}
	else{
		return srcboneno;
	}

}

//int CModel::PhysicsRotAxisDelta(CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
//{
//	if (!m_curmotinfo){
//		return 0;
//	}
//
//
//	CBone* curbone = m_bonelist[srcboneno];
//	if (!curbone){
//		return 0;
//	}
//	CBone* parbone = curbone->GetParent();
//	if (!parbone){
//		return 0;
//	}
//	if (!parbone->GetParent()){
//		//grand parentがルートボーンの場合に、まだうまくいかないのでスキップ
//		return 0;
//	}
//
//
//	int calcnum = 3;
//
//	float rotrad = delta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
//	if (fabs(rotrad) < (0.02f * (float)DEG2PAI)){
//		return 0;
//	}
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
//
//	SetBefEditMat(erptr, curbone, maxlevel);//!!!!!!!!!!!!
//
//	if (parbone){
//		CBone* gparbone = parbone->GetParent();
//		CBone* childbone = parbone->GetChild();
//		int isfirst = 1;
//		float currate = 1.0f;
//		double firstframe = 0.0;
//		int levelcnt = 0;
//
//		while (childbone){
//			float rotrad2 = currate * rotrad;
//			//float rotrad2 = rotrad;
//			if (fabs(rotrad2) < (0.02f * (float)DEG2PAI)){
//				continue;
//			}
//
//			ChaVector3 axis0;
//			CQuaternion localq;
//			if (axiskind == PICK_X){
//				axis0 = ChaVector3(1.0f, 0.0f, 0.0f);
//				localq.SetAxisAndRot(axis0, -rotrad2);
//			}
//			else if (axiskind == PICK_Y){
//				axis0 = ChaVector3(0.0f, 1.0f, 0.0f);
//				localq.SetAxisAndRot(axis0, -rotrad2);
//			}
//			else if (axiskind == PICK_Z){
//				axis0 = ChaVector3(0.0f, 0.0f, 1.0f);
//				localq.SetAxisAndRot(axis0, -rotrad2);
//			}
//			else{
//				_ASSERT(0);
//				return 1;
//			}
//
//			ChaMatrix invselectmat;
//			ChaMatrixInverse(&invselectmat, NULL, &selectmat);
//			ChaMatrix rotselect = selectmat;
//			rotselect._41 = 0.0f;
//			rotselect._42 = 0.0f;
//			rotselect._43 = 0.0f;
//			ChaMatrix rotinvselect = invselectmat;
//			rotinvselect._41 = 0.0f;
//			rotinvselect._42 = 0.0f;
//			rotinvselect._43 = 0.0f;
//
//			ChaMatrix gparrotmat, invgparrotmat;
//			if (parbone->GetParent()){
//				gparrotmat = parbone->GetParent()->GetBtMat();
//				ChaMatrixInverse(&invgparrotmat, NULL, &gparrotmat);
//
//				gparrotmat._41 = 0.0f;
//				gparrotmat._42 = 0.0f;
//				gparrotmat._43 = 0.0f;
//
//				invgparrotmat._41 = 0.0f;
//				invgparrotmat._42 = 0.0f;
//				invgparrotmat._43 = 0.0f;
//			}
//			else{
//				ChaMatrixIdentity(&gparrotmat);
//				ChaMatrixIdentity(&invgparrotmat);
//			}
//			ChaMatrix parrotmat, invparrotmat;
//			parrotmat = parbone->GetBtMat();
//			ChaMatrixInverse(&invparrotmat, NULL, &parrotmat);
//			parrotmat._41 = 0.0f;
//			parrotmat._42 = 0.0f;
//			parrotmat._43 = 0.0f;
//
//
//			//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
//			ChaMatrix transmat = rotselect * localq.MakeRotMatX() * rotinvselect;
//
//			CMotionPoint transmp;
//			transmp.CalcQandTra(transmat, parbone);
//			CQuaternion rotq;
//			rotq = transmp.GetQ();
//
//
//			ChaVector3 parworld, chilworld;
//			D3DVec3TransformCoord(&parworld, &(parbone->GetJointFPos()), &(parbone->GetBtMat()));
//			D3DVec3TransformCoord(&chilworld, &(childbone->GetJointFPos()), &(parbone->GetBtMat()));
//
//
//			ChaMatrix newbtmat;
//			ChaVector3 rotcenter;
//			//rotcenter = parworld;
//			rotcenter = parbone->GetJointFPos();
//
//			ChaMatrix befrot, aftrot;
//			ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//			ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//			ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
//			newbtmat = rotmat * parbone->GetBtMat();
//
//
//			ChaMatrix firstworld = parbone->GetStartMat2();
//			ChaMatrix invfirstworld;
//			ChaMatrixInverse(&invfirstworld, NULL, &firstworld);
//
//			ChaMatrix diffworld = invfirstworld * newbtmat;
//			CRigidElem* curre = parbone->GetRigidElem(childbone);
//			ChaMatrix newrigidmat;
//			if (curre){
//				newrigidmat = curre->GetFirstcapsulemat() * diffworld;
//			}
//			else{
//				::MessageBoxA(NULL, "PhysicsRotAxisDelta : curre NULL error", "error", MB_OK);
//				return -1;
//			}
//
//
//			ChaVector3 newparpos, newchilpos;
//			ChaVector3 jointfpos;
//			jointfpos = parbone->GetJointFPos();
//			D3DVec3TransformCoord(&newparpos, &jointfpos, &newbtmat);
//			jointfpos = childbone->GetJointFPos();
//			D3DVec3TransformCoord(&newchilpos, &jointfpos, &newbtmat);
//
//			ChaVector3 rigidcenter = (newparpos + newchilpos) * 0.5f;
//
//
//			CQuaternion tmpq;
//			tmpq.RotationMatrix(newrigidmat);
//			btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);
//
//
//			btTransform worldtra;
//			worldtra.setIdentity();
//			worldtra.setRotation(btrotq);
//			worldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));
//
////角度制限　ここから
//			if (gparbone){
//				CBtObject* parbto = gparbone->GetBtObject(parbone);
//				if (parbto){
//					CBtObject* setbto = parbone->GetBtObject(childbone);
//					if (setbto){
//						btMatrix3x3 firstworldmat = setbto->GetFirstTransformMat();
//
//						btGeneric6DofSpringConstraint* dofC = parbto->FindConstraint(parbone, childbone);
//						if (dofC){
//
//							//constraint変化分　以下3行　　CreateBtObjectをしたときの状態を基準にした角度になっている。つまりシミュ開始時が０度。
//							//btTransform contraA = dofC->getCalculatedTransformA();
//							//btTransform contraB = dofC->getCalculatedTransformB();
//							//btMatrix3x3 eulmat = contraA.getBasis().inverse() * contraB.getBasis() * contraA.getBasis();
//
//
//							////親ボーンとの角度がオイラー角に入る
//							//btTransform contraA = dofC->getCalculatedTransformA();
//							//btTransform contraB = dofC->getCalculatedTransformB();
//							//btTransform parworldtra;
//							//parbto->GetRigidBody()->getMotionState()->getWorldTransform(parworldtra);
//							//btMatrix3x3 diffmat = worldtra.getBasis() * parworldtra.getBasis().inverse();
//							//btMatrix3x3 eulmat = contraA.getBasis().inverse() * diffmat * contraA.getBasis();
//
//							btTransform contraA = dofC->getCalculatedTransformA();
//							btTransform contraB = dofC->getCalculatedTransformB();
//							btMatrix3x3 diffmat = firstworldmat.inverse() * worldtra.getBasis();
//							btMatrix3x3 eulmat = contraA.getBasis().inverse() * diffmat * contraA.getBasis();
//							//btMatrix3x3 eulmat = contraA.getBasis() * diffmat * contraA.getBasis().inverse();
//
//
//
//							btScalar eulz = 0.0;
//							btScalar euly = 0.0;
//							btScalar eulx = 0.0;
//							ChaVector3 eul = ChaVector3(0.0f, 0.0f, 0.0f);
//							//worldtra.getBasis().getEulerZYX(eulz, euly, eulx, 1);
//							eulmat.getEulerZYX(eulz, euly, eulx, 1);
//							eul.x = eulx * 180.0 / PAI;
//							eul.y = euly * 180.0 / PAI;
//							eul.z = eulz * 180.0 / PAI;
//
//							int ismovable = parbone->ChkMovableEul(eul);
//							char strmsg[256];
//							//sprintf_s(strmsg, 256, "needmodify 0 : neweul [%f, %f, %f] : dof [%f, %f, %f] : ismovable %d\n", eul.x, eul.y, eul.z, dofx, dofy, dofz, ismovable);
//							sprintf_s(strmsg, 256, "needmodify 0 : neweul [%f, %f, %f] : ismovable %d\n", eul.x, eul.y, eul.z, ismovable);
//							OutputDebugStringA(strmsg);
//
//
//							//Q2EulZYXbtのテスト　以下8行
//							CQuaternion eulq;
//							eulq.MakeFromBtMat3x3(eulmat);
//							int needmodifyflag = 0;
//							ChaVector3 testbefeul = ChaVector3(0.0f, 0.0f, 0.0f);
//							ChaVector3 testeul = ChaVector3(0.0f, 0.0f, 0.0f);
//							//eulq.Q2EulZYXbt(needmodifyflag, 0, testbefeul, &testeul);
//							eulq.Q2EulXYZ(0, testbefeul, &testeul);//bulletの回転順序は数値検証の結果XYZ。(ZYXではない)。
//							sprintf_s(strmsg, 256, "testeul [%f, %f, %f]\n", testeul.x, testeul.y, testeul.z);
//							OutputDebugStringA(strmsg);
//
//
//							if (ismovable != 1){
//								childbone = childbone->GetBrother();
//								continue;
//							}
//
//							setbto->GetRigidBody()->getMotionState()->setWorldTransform(worldtra);
//							//setbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
//							//setbto->GetRigidBody()->setDeactivationTime(30000.0);
//							setbto->GetRigidBody()->setDeactivationTime(0.0);
//
//							if (isfirst == 1){
//								parbone->SetBtMat(newbtmat);
//								//IKボーンはKINEMATICだから。
//								parbone->GetCurMp().SetWorldMat(newbtmat);
//								isfirst = 0;
//							}
//
//						}
//					}
//				}
////角度制限　ここまで
//			}
//			childbone = childbone->GetBrother();
//		}
//	}
//
//	if (curbone){
//		return curbone->GetBoneNo();
//	}
//	else{
//		return srcboneno;
//	}
//
//}
//


int CModel::PhysicsRigControl(int depthcnt, CEditRange* erptr, int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig)
{
	if (depthcnt >= 10){
		_ASSERT(0);
		return 0;//!!!!!!!!!!!!!!!!!
	}
	depthcnt++;

	if (!m_curmotinfo){
		return 0;
	}

	int calcnum = 3;

	float rotrad = srcdelta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
	//if (fabs(rotrad) < (0.02f * (float)DEG2PAI)){
	//	return 0;
	//}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone){
		return 0;
	}
	CBone* parbone = 0;
	CBone* lastbone = 0;

	//int calccnt;
	//for (calccnt = 0; calccnt < calcnum; calccnt++){
	double firstframe = 0.0;

	int elemno;
	for (elemno = 0; elemno < ikcustomrig.elemnum; elemno++){
		RIGELEM currigelem = ikcustomrig.rigelem[elemno];
		if (currigelem.rigrigboneno >= 0){
			//rigのrig
			CBone* rigrigbone = GetBoneByID(currigelem.rigrigboneno);
			if (rigrigbone){
				int rigrigno = currigelem.rigrigno;
				if ((rigrigno >= 0) && (rigrigno < MAXRIGNUM)){
					if (currigelem.transuv[uvno].enable == 1){
						CUSTOMRIG rigrig = rigrigbone->GetCustomRig(rigrigno);
						PhysicsRigControl(depthcnt, erptr, rigrigbone->GetBoneNo(), uvno, srcdelta * currigelem.transuv[uvno].applyrate, rigrig);
					}
				}
			}
		}
		else{
			//rigelem
			curbone = GetBoneByID(currigelem.boneno);
			if (curbone){
				lastbone = curbone;
				parbone = curbone->GetParent();
				if (parbone){
					int isfirst = 1;

					//CBone* childbone = curbone;
					//CBone* childbone = parbone->GetChild();
					//while (childbone){

						int axiskind = currigelem.transuv[uvno].axiskind;
						float rotrad2 = rotrad * currigelem.transuv[uvno].applyrate;

						//float rotrad2;
						//rotrad2 = rotrad;


						//char str1[256];
						//sprintf_s(str1, 256, "AXIS_KIND %d, rotrad %f, rotrad2 %f", axiskind, rotrad, rotrad2);
						//::MessageBoxA(NULL, str1, "check", MB_OK);

						if (currigelem.transuv[uvno].enable == 1){
							if (fabs(rotrad2) >= (0.02f * (float)DEG2PAI)){
								ChaVector3 axis0;
								CQuaternion localq;
								
								if (axiskind == AXIS_X){
									axis0 = ChaVector3(1.0f, 0.0f, 0.0f);
									localq.SetAxisAndRot(axis0, rotrad2);
								}
								else if (axiskind == AXIS_Y){
									axis0 = ChaVector3(0.0f, 1.0f, 0.0f);
									localq.SetAxisAndRot(axis0, rotrad2);
								}
								else if (axiskind == AXIS_Z){
									axis0 = ChaVector3(0.0f, 0.0f, 1.0f);
									localq.SetAxisAndRot(axis0, rotrad2);
								}
								else{
									_ASSERT(0);
									return 1;
								}
								
								//axis0 = ChaVector3(1.0f, 0.0f, 0.0f);
								///localq.SetAxisAndRot(axis0, 0.0697 * 0.50);


								/*
								ChaMatrix selectmat;
								ChaMatrix invselectmat;
								//selectmat = elemaxismat[elemno];
								int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
								selectmat = childbone->CalcManipulatorMatrix(1, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
								ChaMatrixInverse(&invselectmat, NULL, &selectmat);

								ChaMatrix rotinvworld = childbone->GetCurMp().GetInvWorldMat();
								rotinvworld._41 = 0.0f;
								rotinvworld._42 = 0.0f;
								rotinvworld._43 = 0.0f;
								ChaMatrix rotselect = selectmat;
								rotselect._41 = 0.0f;
								rotselect._42 = 0.0f;
								rotselect._43 = 0.0f;
								ChaMatrix rotinvselect = invselectmat;
								rotinvselect._41 = 0.0f;
								rotinvselect._42 = 0.0f;
								rotinvselect._43 = 0.0f;


								CQuaternion rotq;


								ChaMatrix gparrotmat, invgparrotmat;
								if (parbone->GetParent()){
									gparrotmat = parbone->GetParent()->GetBtMat();
									ChaMatrixInverse(&invgparrotmat, NULL, &gparrotmat);

									gparrotmat._41 = 0.0f;
									gparrotmat._42 = 0.0f;
									gparrotmat._43 = 0.0f;

									invgparrotmat._41 = 0.0f;
									invgparrotmat._42 = 0.0f;
									invgparrotmat._43 = 0.0f;
								}
								else{
									ChaMatrixIdentity(&gparrotmat);
									ChaMatrixIdentity(&invgparrotmat);
								}
								ChaMatrix parrotmat, invparrotmat;
								parrotmat = parbone->GetBtMat();
								ChaMatrixInverse(&invparrotmat, NULL, &parrotmat);

								*/
								ChaMatrix transmat2;
								//これでは体全体が反対を向いたときに回転方向が反対向きになる。
								//transmat2 = invgparrotmat * rotq0.MakeRotMatX() * gparrotmat;

								//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
								ChaMatrix transmat = localq.MakeRotMatX();
								//以下のようにすれば体全体が回転した時にも正常に動く
								//transmat2 = invgparrotmat * parrotmat * transmat * invparrotmat * gparrotmat;
								//CMotionPoint transmp;
								//transmp.CalcQandTra(transmat2, parbone);
								//rotq = transmp.GetQ();

								ChaMatrix rotmat2;
								//rotmat2 = rotq.MakeRotMatX();
								rotmat2 = transmat;

								CBone* childbone = curbone->GetChild();
								while (childbone){
									btTransform curworldtra;
									CBtObject* setbto = curbone->GetBtObject(childbone);
									if (setbto){
										curworldtra = setbto->GetRigidBody()->getWorldTransform();
										btMatrix3x3 curworldmat = curworldtra.getBasis();

										btMatrix3x3 rotworldmat;
										rotworldmat.setIdentity();
										rotworldmat.setValue(
											rotmat2._11, rotmat2._12, rotmat2._13,
											rotmat2._21, rotmat2._22, rotmat2._23,
											rotmat2._31, rotmat2._32, rotmat2._33);

										btMatrix3x3 setrotmat;
										setrotmat = curworldmat * rotworldmat;

										curworldtra.setBasis(setrotmat);
										setbto->GetRigidBody()->getMotionState()->setWorldTransform(curworldtra);
									}
									childbone = childbone->GetBrother();
								}


								/*
								ChaVector3 parworld, chilworld;
								D3DVec3TransformCoord(&parworld, &(parbone->GetJointFPos()), &(parbone->GetBtMat()));
								D3DVec3TransformCoord(&chilworld, &(childbone->GetJointFPos()), &(parbone->GetBtMat()));

								ChaMatrix newbtmat;
								ChaVector3 rotcenter;// = m_childworld;
								rotcenter = parworld;

								ChaMatrix befrot, aftrot;
								ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
								ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
								ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
								newbtmat = parbone->GetBtMat() * rotmat;// *tramat;
								//newbtmat = parbone->GetCurMp().GetBtMat() * rotq.MakeRotMatX();// *tramat;


								ChaMatrix firstworld = parbone->GetStartMat2();
								ChaMatrix invfirstworld;
								ChaMatrixInverse(&invfirstworld, NULL, &firstworld);

								ChaMatrix diffworld = invfirstworld * newbtmat;
								CRigidElem* curre = parbone->GetRigidElem(childbone);
								ChaMatrix newrigidmat;
								if (curre){
									newrigidmat = curre->GetFirstcapsulemat() * diffworld;
								}
								else{
									::MessageBoxA(NULL, "IKRotateRagdoll : curre NULL error", "error", MB_OK);
									return -1;
								}


								ChaVector3 newparpos, newchilpos;
								ChaVector3 jointfpos;
								jointfpos = parbone->GetJointFPos();
								D3DVec3TransformCoord(&newparpos, &jointfpos, &newbtmat);
								jointfpos = childbone->GetJointFPos();
								D3DVec3TransformCoord(&newchilpos, &jointfpos, &newbtmat);

								ChaVector3 rigidcenter = (newparpos + newchilpos) * 0.5f;


								CQuaternion tmpq;
								tmpq.RotationMatrix(newrigidmat);
								btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);


								btTransform worldtra;
								worldtra.setIdentity();
								worldtra.setRotation(btrotq);
								worldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));

								CBtObject* setbto = parbone->GetBtObject(childbone);
								if (setbto){
									setbto->GetRigidBody()->getMotionState()->setWorldTransform(worldtra);
									//setbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
									//setbto->GetRigidBody()->setDeactivationTime(30000.0);
								}

								if (isfirst == 1){
									parbone->SetBtMat(newbtmat);
									//IKボーンはKINEMATICだから。
									parbone->GetCurMp().SetWorldMat(newbtmat);
									isfirst = 0;
								}
								*/

							}
						}

						//childbone = childbone->GetBrother();
					//}
				}
			}
			else{
				_ASSERT(0);
			}
		}
	}

	//if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone){
	//if (g_absikflag && lastbone){
	//		AdjustBoneTra(erptr, lastbone);
	//}
	//}

	if (lastbone){
		return lastbone->GetBoneNo();
	}
	else{
		return srcboneno;
	}
}



int CModel::PhysicsMV(CEditRange* erptr, int srcboneno, ChaVector3 diffvec)
{

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone){
		_ASSERT(0);
		return -1;
	}


	ChaVector3 ikaxis = g_camtargetpos - g_camEye;
	ChaVector3Normalize(&ikaxis, &ikaxis);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = firstbone;
	SetBefEditMat(erptr, curbone, 0);


	ChaVector3 mvvec = diffvec * g_physicsmvrate;


	int isfirst = 1;

	PhysicsMVReq(m_topbone, mvvec);

	if (m_topbone){
		return m_topbone->GetBoneNo();
	}
	else{
		return srcboneno;
	}

}

int CModel::WithConstraint(CBone* srcbone)
{
	if (srcbone){

		CBone* curbone = srcbone;
		while (curbone){
			if (curbone->GetPosConstraint() == 1){
				return 1;
			}
			curbone = curbone->GetParent();
		}
	}
	else{
		return 0;
	}

	return 0;
}


void CModel::PhysicsMVReq(CBone* srcbone, ChaVector3 mvvec)
{
	if (srcbone){
		CBone* curbone = srcbone;
		CBone* parbone = curbone->GetParent();

		ChaMatrix mvmat;
		ChaMatrixIdentity(&mvmat);
		ChaMatrixTranslation(&mvmat, mvvec.x, mvvec.y, mvvec.z);


		if (parbone && (WithConstraint(curbone) == 0) && (parbone->GetExcludeMv() == 0)){
				
			CBone* childbone = curbone;

			childbone->SetBtKinFlag(1);
			CBtObject* srcbto = parbone->GetBtObject(childbone);
			if (srcbto){
				DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
				srcbto->GetRigidBody()->setCollisionFlags(curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
				if (srcbto->GetRigidBody()){
					srcbto->GetRigidBody()->setDeactivationTime(0.0);
					//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
				}


				ChaMatrix newbtmat;
				newbtmat = parbone->GetBtMat() * mvmat;// *tramat;

				btTransform worldtra;
				srcbto->GetRigidBody()->getMotionState()->getWorldTransform(worldtra);
				btMatrix3x3 worldmat = worldtra.getBasis();
				btVector3 worldpos = worldtra.getOrigin();


				btTransform setworldtra;
				setworldtra.setIdentity();
				setworldtra.setBasis(worldmat);
				setworldtra.setOrigin(btVector3(worldpos.x() + mvvec.x, worldpos.y() + mvvec.y, worldpos.z() + mvvec.z));
				if (srcbto){
					srcbto->GetRigidBody()->getMotionState()->setWorldTransform(setworldtra);
				}
				else{
					::MessageBoxA(NULL, "IKTraRagdoll : setbto NULL !!!!", "check", MB_OK);
				}

			}
		}

		if (curbone->GetChild()){
			PhysicsMVReq(curbone->GetChild(), mvvec);
		}
		if (curbone->GetBrother()){
			PhysicsMVReq(curbone->GetBrother(), mvvec);
		}
	}
}


int CModel::AdjustBoneTra( CEditRange* erptr, CBone* lastpar )
{
	int keynum = erptr->GetKeyNum();
	double startframe = erptr->GetStartFrame();
	double endframe;
	if (g_applyendflag == 1){
		endframe = m_curmotinfo->frameleng - 1.0;
	}
	else{
		endframe = erptr->GetEndFrame();
	}

	if( lastpar && (keynum >= 2) ){
		int keyno = 0;
		double curframe;
		for( curframe = startframe; curframe <= endframe; curframe += 1.0 ){
			if( keyno >= 1 ){
				CMotionPoint* pcurmp = 0;
				int curmotid = m_curmotinfo->motid;
				pcurmp = lastpar->GetMotionPoint(curmotid, curframe);
				if(pcurmp){
					ChaVector3 orgpos;
					D3DVec3TransformCoord( &orgpos, &(lastpar->GetJointFPos()), &(pcurmp->GetBefEditMat()) );

					ChaVector3 newpos;
					D3DVec3TransformCoord( &newpos, &(lastpar->GetJointFPos()), &(pcurmp->GetWorldMat()) );

					ChaVector3 diffpos;
					diffpos = orgpos - newpos;

					CEditRange tmper;
					KeyInfo tmpki;
					tmpki.time = curframe;
					list<KeyInfo> tmplist;
					tmplist.push_back( tmpki );
					tmper.SetRange( tmplist, curframe );
					FKBoneTra( 0, &tmper, lastpar->GetBoneNo(), diffpos );
				}
			}
			keyno++;
		}
	}

	return 0;
}

int CModel::RigControl(int depthcnt, CEditRange* erptr, int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig)
{
	if (depthcnt >= 10){
		_ASSERT(0);
		return 0;//!!!!!!!!!!!!!!!!!
	}
	depthcnt++;

	if (!m_curmotinfo){
		return 0;
	}

	int calcnum = 3;

	float rotrad = srcdelta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
	if (fabs(rotrad) < (0.02f * (float)DEG2PAI)){
		return 0;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone){
		return 0;
	}
	CBone* parbone = 0;
	CBone* lastbone = 0;

	//int calccnt;
	//for (calccnt = 0; calccnt < calcnum; calccnt++){
		double firstframe = 0.0;

		int elemno;
		for (elemno = 0; elemno < ikcustomrig.elemnum; elemno++){
			RIGELEM currigelem = ikcustomrig.rigelem[elemno];
			if (currigelem.rigrigboneno >= 0){
				//rigのrig
				CBone* rigrigbone = GetBoneByID(currigelem.rigrigboneno);
				if (rigrigbone){
					int rigrigno = currigelem.rigrigno;
					if ((rigrigno >= 0) && (rigrigno < MAXRIGNUM)){
						if (currigelem.transuv[uvno].enable == 1){
							CUSTOMRIG rigrig = rigrigbone->GetCustomRig(rigrigno);
							RigControl(depthcnt, erptr, rigrigbone->GetBoneNo(), uvno, srcdelta * currigelem.transuv[uvno].applyrate, rigrig);
						}
					}
				}
			}
			else{
				//rigelem
				curbone = GetBoneByID(currigelem.boneno);
				if (curbone){
					lastbone = curbone;
					parbone = curbone->GetParent();
					int axiskind = currigelem.transuv[uvno].axiskind;
					float rotrad2 = rotrad * currigelem.transuv[uvno].applyrate;
					if (currigelem.transuv[uvno].enable == 1){
						if (fabs(rotrad2) >= (0.02f * (float)DEG2PAI)){
							ChaVector3 axis0;
							CQuaternion localq;
							if (axiskind == AXIS_X){
								axis0 = ChaVector3(1.0f, 0.0f, 0.0f);
								localq.SetAxisAndRot(axis0, rotrad2);
							}
							else if (axiskind == AXIS_Y){
								axis0 = ChaVector3(0.0f, 1.0f, 0.0f);
								localq.SetAxisAndRot(axis0, rotrad2);
							}
							else if (axiskind == AXIS_Z){
								axis0 = ChaVector3(0.0f, 0.0f, 1.0f);
								localq.SetAxisAndRot(axis0, rotrad2);
							}
							else{
								_ASSERT(0);
								return 1;
							}

							ChaMatrix selectmat;
							ChaMatrix invselectmat;
							//selectmat = elemaxismat[elemno];
							int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
							selectmat = curbone->CalcManipulatorMatrix(1, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
							ChaMatrixInverse(&invselectmat, NULL, &selectmat);

							ChaMatrix rotinvworld = curbone->GetCurMp().GetInvWorldMat();
							rotinvworld._41 = 0.0f;
							rotinvworld._42 = 0.0f;
							rotinvworld._43 = 0.0f;
							ChaMatrix rotselect = selectmat;
							rotselect._41 = 0.0f;
							rotselect._42 = 0.0f;
							rotselect._43 = 0.0f;
							ChaMatrix rotinvselect = invselectmat;
							rotinvselect._41 = 0.0f;
							rotinvselect._42 = 0.0f;
							rotinvselect._43 = 0.0f;


							CQuaternion rotq;

							if (keynum >= 2){
								int keyno = 0;
								double curframe;
								for (curframe = startframe; curframe <= endframe; curframe += 1.0){

									CMotionPoint* curparmp = 0;
									CMotionPoint* aplyparmp = 0;
									if (parbone){
										curparmp = parbone->GetMotionPoint(m_curmotinfo->motid, curframe);
										aplyparmp = parbone->GetMotionPoint(m_curmotinfo->motid, applyframe);
									}
									if (curparmp && aplyparmp && (g_pseudolocalflag == 1)){
										ChaMatrix curparrotmat = curparmp->GetWorldMat();
										curparrotmat._41 = 0.0f;
										curparrotmat._42 = 0.0f;
										curparrotmat._43 = 0.0f;
										ChaMatrix invcurparrotmat = curparmp->GetInvWorldMat();
										invcurparrotmat._41 = 0.0f;
										invcurparrotmat._42 = 0.0f;
										invcurparrotmat._43 = 0.0f;
										ChaMatrix aplyparrotmat = aplyparmp->GetWorldMat();
										aplyparrotmat._41 = 0.0f;
										aplyparrotmat._42 = 0.0f;
										aplyparrotmat._43 = 0.0f;
										ChaMatrix invaplyparrotmat = aplyparmp->GetInvWorldMat();
										invaplyparrotmat._41 = 0.0f;
										invaplyparrotmat._42 = 0.0f;
										invaplyparrotmat._43 = 0.0f;

										ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
										ChaMatrix transmat2;
										transmat2 = invcurparrotmat * aplyparrotmat * transmat * invaplyparrotmat * curparrotmat;
										CMotionPoint transmp;
										transmp.CalcQandTra(transmat2, curbone);
										rotq = transmp.GetQ();
									}
									else{
										ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
										CMotionPoint transmp;
										transmp.CalcQandTra(transmat, curbone);
										rotq = transmp.GetQ();
									}

									double changerate;
									if (curframe <= applyframe){
										changerate = 1.0 / (applyframe - startframe + 1);
									}
									else{
										changerate = 1.0 / (endframe - applyframe + 1);
									}

									if (keyno == 0){
										firstframe = curframe;
									}
									if (g_absikflag == 0){
										if (g_slerpoffflag == 0){
											double currate2;
											CQuaternion endq;
											CQuaternion curq;
											endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
											if (curframe <= applyframe){
												currate2 = changerate * (curframe - startframe + 1);
											}
											else{
												currate2 = changerate * (endframe - curframe + 1);
											}
											rotq.Slerp2(endq, 1.0 - currate2, &curq);
											curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, curq);
										}
										else{
											curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, rotq);
										}
									}
									else{
										if (keyno == 0){
											curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, rotq);
										}
										else{
											curbone->SetAbsMatReq(0, m_curmotinfo->motid, curframe, firstframe);
										}
									}
									keyno++;
								}

							}
							else{
								ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
								CMotionPoint transmp;
								transmp.CalcQandTra(transmat, curbone);
								rotq = transmp.GetQ();

								curbone->RotBoneQReq(0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);

							}
							if (g_applyendflag == 1){
								//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
								if (m_topbone){
									int tolast;
									for (tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++){
										//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
										m_topbone->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
									}
								}
							}
						}
					}
				}
				else{
					_ASSERT(0);
				}
			}
		}

		//if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone){
		//if (g_absikflag && lastbone){
		//		AdjustBoneTra(erptr, lastbone);
		//}
	//}

	if (lastbone){
		return lastbone->GetBoneNo();
	}
	else{
		return srcboneno;
	}
}

int CModel::InterpolateBetweenSelection(double srcstartframe, double srcendframe)
{
	if (!GetCurMotInfo()){
		return 0;
	}

	InterpolateBetweenSelectionReq(GetTopBone(), srcstartframe, srcendframe);

	return 0;
}

void CModel::InterpolateBetweenSelectionReq(CBone* srcbone, double srcstartframe, double srcendframe)
{
	if (!srcbone){
		return;
	}
	if ((srcstartframe < 0.0) || (srcendframe < 0.0) || (srcendframe <= srcstartframe)){
		return;
	}
	if (!GetCurMotInfo()){
		return;
	}

	if (srcbone){
		int curmotid = GetCurMotInfo()->motid;
		CMotionPoint startmp, endmp;
		srcbone->CalcLocalInfo(curmotid, srcstartframe, &startmp);
		srcbone->CalcLocalInfo(curmotid, srcendframe, &endmp);
		CQuaternion startq, endq;
		ChaVector3 starttra, endtra;
		startq = startmp.GetQ();
		endq = endmp.GetQ();
		starttra = srcbone->CalcLocalTraAnim(curmotid, srcstartframe);
		endtra = srcbone->CalcLocalTraAnim(curmotid, srcendframe);

		double frame;
		for (frame = srcstartframe; frame <= srcendframe; frame += 1.0){
			double slerpt;
			CQuaternion setq;
			ChaVector3 settra;
			if (IsTimeEqual(frame, srcstartframe)){
				setq = startq;
				settra = starttra;
			}
			else if (IsTimeEqual(frame, srcendframe)){
				setq = endq;
				settra = endtra;
			}
			else{
				slerpt = (frame - srcstartframe) / (srcendframe - srcstartframe);//srcendframe==srcstartframeは冒頭でreturnしている。
				startq.Slerp2(endq, slerpt, &setq);
				settra = starttra + slerpt * (endtra - starttra);
			}
			int setchildflag1 = 1;
			CQuaternion iniq;
			srcbone->SetWorldMatFromQAndTra(setchildflag1, iniq, setq, settra, curmotid, frame);
		}

		if (srcbone->GetChild()){
			InterpolateBetweenSelectionReq(srcbone->GetChild(), srcstartframe, srcendframe);
		}
		if (srcbone->GetBrother()){
			InterpolateBetweenSelectionReq(srcbone->GetBrother(), srcstartframe, srcendframe);
		}
	}
}

int CModel::IKRotateAxisDelta(CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
{
	if (!m_curmotinfo){
		return 0;
	}


	int calcnum = 3;

	float rotrad = delta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
	if (fabs(rotrad) < (0.02f * (float)DEG2PAI)){
		return 0;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone){
		return 0;
	}
	CBone* firstbone = curbone;
	CBone* parbone = 0;
	CBone* lastbone = 0;
	CBone* topbone = GetTopBone();

	int calccnt;
	for (calccnt = 0; calccnt < calcnum; calccnt++){
		curbone = firstbone;
		if (!curbone){
			return 0;
		}
		lastbone = curbone;

		float currate = 1.0f;

		double firstframe = 0.0;
		int levelcnt = 0;

		while (curbone && ((maxlevel == 0) || (levelcnt < maxlevel))){
			parbone = curbone->GetParent();
			float rotrad2 = currate * rotrad;
			//float rotrad2 = rotrad;
			if (fabs(rotrad2) < (0.02f * (float)DEG2PAI)){
				break;
			}

			ChaVector3 axis0;
			CQuaternion localq;
			if (axiskind == PICK_X){
				axis0 = ChaVector3(1.0f, 0.0f, 0.0f);
				localq.SetAxisAndRot(axis0, rotrad2);
			}
			else if (axiskind == PICK_Y){
				axis0 = ChaVector3(0.0f, 1.0f, 0.0f);
				localq.SetAxisAndRot(axis0, rotrad2);
			}
			else if (axiskind == PICK_Z){
				axis0 = ChaVector3(0.0f, 0.0f, 1.0f);
				localq.SetAxisAndRot(axis0, rotrad2);
			}
			else{
				_ASSERT(0);
				return 1;
			}

			//ChaMatrix selectmat;
			ChaMatrix invselectmat;
			//int multworld = 1;
			//selectmat = curbone->CalcManipulatorMatrix(0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
			ChaMatrixInverse(&invselectmat, NULL, &selectmat);


			ChaMatrix rotinvworld = firstbone->GetCurMp().GetInvWorldMat();
			rotinvworld._41 = 0.0f;
			rotinvworld._42 = 0.0f;
			rotinvworld._43 = 0.0f;
			ChaMatrix rotselect = selectmat;
			rotselect._41 = 0.0f;
			rotselect._42 = 0.0f;
			rotselect._43 = 0.0f;
			ChaMatrix rotinvselect = invselectmat;
			rotinvselect._41 = 0.0f;
			rotinvselect._42 = 0.0f;
			rotinvselect._43 = 0.0f;

			CQuaternion rotq;

			if (keynum >= 2){
				int keyno = 0;
				double curframe;
				for (curframe = startframe; curframe <= endframe; curframe += 1.0){

					CMotionPoint* curparmp = 0;
					CMotionPoint* aplyparmp = 0;
					if (parbone){
						curparmp = parbone->GetMotionPoint(m_curmotinfo->motid, curframe);
						aplyparmp = parbone->GetMotionPoint(m_curmotinfo->motid, applyframe);
					}
					if (curparmp && aplyparmp && (g_pseudolocalflag == 1)){
						ChaMatrix curparrotmat = curparmp->GetWorldMat();
						curparrotmat._41 = 0.0f;
						curparrotmat._42 = 0.0f;
						curparrotmat._43 = 0.0f;
						ChaMatrix invcurparrotmat = curparmp->GetInvWorldMat();
						invcurparrotmat._41 = 0.0f;
						invcurparrotmat._42 = 0.0f;
						invcurparrotmat._43 = 0.0f;
						ChaMatrix aplyparrotmat = aplyparmp->GetWorldMat();
						aplyparrotmat._41 = 0.0f;
						aplyparrotmat._42 = 0.0f;
						aplyparrotmat._43 = 0.0f;
						ChaMatrix invaplyparrotmat = aplyparmp->GetInvWorldMat();
						invaplyparrotmat._41 = 0.0f;
						invaplyparrotmat._42 = 0.0f;
						invaplyparrotmat._43 = 0.0f;

						ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
						ChaMatrix transmat2;
						transmat2 = invcurparrotmat * aplyparrotmat * transmat * invaplyparrotmat * curparrotmat;
						CMotionPoint transmp;
						transmp.CalcQandTra(transmat2, firstbone);
						rotq = transmp.GetQ();
					}
					else{
						ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
						CMotionPoint transmp;
						transmp.CalcQandTra(transmat, firstbone);
						rotq = transmp.GetQ();
					}

					double changerate;
					if (curframe <= applyframe){
						changerate = 1.0 / (applyframe - startframe + 1);
					}
					else{
						changerate = 1.0 / (endframe - applyframe + 1);
					}

					if (keyno == 0){
						firstframe = curframe;
					}

					if (g_absikflag == 0){
						if (g_slerpoffflag == 0){
							double currate2;
							CQuaternion endq;
							CQuaternion curq;
							endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
							if (curframe <= applyframe){
								currate2 = changerate * (curframe - startframe + 1);
							}
							else{
								currate2 = changerate * (endframe - curframe + 1);
							}
							rotq.Slerp2(endq, 1.0 - currate2, &curq);

							_ASSERT(0);


							curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, curq);
						}
						else{
							curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, rotq);
						}
					}
					else{
						if (keyno == 0){
							curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, rotq);
						}
						else{
							curbone->SetAbsMatReq(0, m_curmotinfo->motid, curframe, firstframe);
						}
					}
					keyno++;
				}

			}
			else{
				ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
				CMotionPoint transmp;
				transmp.CalcQandTra(transmat, firstbone);
				rotq = transmp.GetQ();

				curbone->RotBoneQReq(0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);
			}


			if (g_applyendflag == 1){
				//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
				if (m_topbone){
					int tolast;
					for (tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++){
						//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
						m_topbone->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
					}
				}
			}

			currate = pow(g_ikrate, g_ikfirst * levelcnt);
			lastbone = curbone;
			curbone = curbone->GetParent();
			levelcnt++;
		}

		if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone){
		//if (g_absikflag && lastbone){
			AdjustBoneTra(erptr, lastbone);
		}
	}

	if (lastbone){
		return lastbone->GetBoneNo();
	}
	else{
		return srcboneno;
	}

}

int CModel::RotateXDelta( CEditRange* erptr, int srcboneno, float delta )
{
	if (!m_curmotinfo){
		return 0;
	}

	CBone* firstbone = m_bonelist[ srcboneno ];
	if( !firstbone ){
		_ASSERT( 0 );
		return 1;
	}

	CBone* curbone = firstbone;
	SetBefEditMatFK( erptr, curbone );

	CBone* lastpar = firstbone->GetParent();


	float rotrad;
	ChaVector3 axis0, rotaxis;
	ChaMatrix selectmat;
	ChaMatrix invselectmat;
	CBone* parbone = curbone->GetParent();
	int multworld = 1;
	selectmat = curbone->CalcManipulatorMatrix(0, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);
	ChaMatrixInverse(&invselectmat, NULL, &selectmat);
	selectmat._41 = 0.0f;
	selectmat._42 = 0.0f;
	selectmat._43 = 0.0f;

	axis0 = ChaVector3( 1.0f, 0.0f, 0.0f );
	D3DVec3TransformCoord( &rotaxis, &axis0, &selectmat );
	ChaVector3Normalize( &rotaxis, &rotaxis );
	rotrad = delta / 10.0f * (float)PAI / 12.0f;

	if( fabs(rotrad) < (0.02f * (float)DEG2PAI) ){
		return 0;
	}

	CQuaternion rotq;
	rotq.SetAxisAndRot( rotaxis, rotrad );

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange( &keynum, &startframe, &endframe, &applyframe );
	
	curbone = firstbone;
	if (!curbone){
		return 0;
	}

	double firstframe = 0.0;
	if (keynum >= 2){
		int keyno = 0;
		double curframe;
		for (curframe = startframe; curframe <= endframe; curframe += 1.0){
			double changerate;
			if (curframe <= applyframe){
				changerate = 1.0 / (applyframe - startframe + 1);
			}
			else{
				changerate = 1.0 / (endframe - applyframe + 1);
			}

			if (keyno == 0){
				firstframe = curframe;
			}
			if (g_absikflag == 0){
				if (g_slerpoffflag == 0){
					double currate2;
					CQuaternion endq;
					CQuaternion curq;
					endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
					if (curframe <= applyframe){
						currate2 = changerate * (curframe - startframe + 1);
					}
					else{
						currate2 = changerate * (endframe - curframe + 1);
					}
					rotq.Slerp2(endq, 1.0 - currate2, &curq);

					curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, curq);
				}
				else{
					curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, rotq);
				}
			}
			else{
				if (keyno == 0){
					curbone->RotBoneQReq(0, m_curmotinfo->motid, curframe, rotq);
				}
				else{
					curbone->SetAbsMatReq(0, m_curmotinfo->motid, curframe, firstframe);
				}
			}
			keyno++;
		}

		if (g_applyendflag == 1){
			//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
			if (m_topbone){
				int tolast;
				for (tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++){
					m_topbone->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
				}
			}
		}


	}
	else{
		curbone->RotBoneQReq(0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);
	}

	if( g_absikflag && curbone ){
		AdjustBoneTra( erptr, curbone );
	}

	return curbone->GetBoneNo();
}


//int CModel::FKRotate( double srcframe, int srcboneno, ChaMatrix srcmat )
int CModel::FKRotate(int reqflag, CBone* bvhbone, int traflag, ChaVector3 traanim, double srcframe, int srcboneno, CQuaternion rotq, int setmatflag, ChaMatrix* psetmat)
{

	if( srcboneno < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	CBone* firstbone = m_bonelist[ srcboneno ];
	if( !firstbone ){
		_ASSERT( 0 );
		return 1;
	}

	CBone* curbone = firstbone;
	CBone* parbone = curbone->GetParent();
	CMotionPoint* parmp = 0;
	if (parbone){
		parmp = parbone->GetMotionPoint(m_curmotinfo->motid, srcframe);
	}

	if (reqflag == 1){
		curbone->RotBoneQReq(0, m_curmotinfo->motid, srcframe, rotq, bvhbone, traanim, setmatflag, psetmat);
	}
	else if(bvhbone){
		ChaMatrix setmat = bvhbone->GetTmpMat();
		//int setmatflag1 = 1;
		curbone->RotBoneQOne(parmp, m_curmotinfo->motid, srcframe, setmat);
		//curbone->RotBoneQReq(0, m_curmotinfo->motid, srcframe, rotq, bvhbone, traanim, setmatflag1, &setmat);
	}

	return curbone->GetBoneNo();
}


int CModel::FKBoneTraAxis(int onlyoneflag, CEditRange* erptr, int srcboneno, int axiskind, float delta)
{
	if ((srcboneno < 0) && !GetTopBone()){
		return 0;
	}

	CBone* curbone = GetBoneByID(srcboneno);
	if (!curbone){
		_ASSERT(0);
		return 0;
	}

	ChaVector3 basevec;
	ChaVector3 vecx(1.0f, 0.0f, 0.0f);
	ChaVector3 vecy(0.0f, 1.0f, 0.0f);
	ChaVector3 vecz(0.0f, 0.0f, 1.0f);

	int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
	ChaMatrix selectmat = curbone->CalcManipulatorMatrix(0, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);

	if (axiskind == 0){
		D3DVec3TransformCoord(&basevec, &vecx, &selectmat);
	}
	else if (axiskind == 1){
		D3DVec3TransformCoord(&basevec, &vecy, &selectmat);
	}
	else if (axiskind == 2){
		D3DVec3TransformCoord(&basevec, &vecz, &selectmat);
	}
	else{
		_ASSERT(0);
		D3DVec3TransformCoord(&basevec, &vecx, &selectmat);
	}

	ChaVector3Normalize(&basevec, &basevec);

	ChaVector3 addtra;
	addtra = basevec * delta;

	FKBoneTra(0, erptr, srcboneno, addtra);

	return 0;
}

int CModel::FKBoneTra( int onlyoneflag, CEditRange* erptr, int srcboneno, ChaVector3 addtra )
{

	if( srcboneno < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	CBone* firstbone = m_bonelist[ srcboneno ];
	if( !firstbone ){
		_ASSERT( 0 );
		return 1;
	}

	CBone* curbone = firstbone;
	SetBefEditMatFK( erptr, curbone );

	CBone* lastpar = firstbone->GetParent();

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange( &keynum, &startframe, &endframe, &applyframe );

	curbone = firstbone;
	double firstframe = 0.0;

	if( keynum >= 2 ){
		float changerate = 1.0f / (float)(endframe - startframe + 1);

		int keyno = 0;
		double curframe;
		for (curframe = startframe; curframe <= endframe; curframe += 1.0){
			double changerate;
			if( curframe <= applyframe ){
				changerate = 1.0 / (applyframe - startframe + 1);
			}else{
				changerate = 1.0 / (endframe - applyframe + 1);
			}

			if( keyno == 0 ){
				firstframe = curframe;
			}
			if( g_absikflag == 0 ){
				if( g_slerpoffflag == 0 ){
					double currate2;
					if( curframe <= applyframe ){
						currate2 = changerate * (curframe - startframe + 1);
					}else{
						currate2 = changerate * (endframe - curframe + 1);
					}
					ChaVector3 curtra;
					curtra = (float)currate2 * addtra;

					//currate2 = changerate * keyno;
					//ChaVector3 curtra;
					//curtra = (1.0 - currate2) * addtra;

					curbone->AddBoneTraReq( 0, m_curmotinfo->motid, curframe, curtra );
				}else{
					curbone->AddBoneTraReq( 0, m_curmotinfo->motid, curframe, addtra );
				}
			}else{
				if( keyno == 0 ){
					curbone->AddBoneTraReq( 0, m_curmotinfo->motid, curframe, addtra );
				}else{
					curbone->SetAbsMatReq( 0, m_curmotinfo->motid, curframe, firstframe );
				}
			}
			keyno++;

		}
	}else{
		curbone->AddBoneTraReq( 0, m_curmotinfo->motid,  startframe, addtra );
	}


	return curbone->GetBoneNo();
}

/*
int CModel::ImpulseBoneRagdoll(int onlyoneflag, CEditRange* erptr, int srcboneno, ChaVector3 addtra)
{

	if (srcboneno < 0){
		_ASSERT(0);
		return 1;
	}

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone){
		_ASSERT(0);
		return 1;
	}

	CBone* curbone = firstbone;
	SetBefEditMatFK(erptr, curbone);

	CBone* lastpar = firstbone->GetParent();

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	curbone = firstbone;
	double firstframe = 0.0;

	ChaVector3 impulse = addtra * g_physicsmvrate;
	//ChaVector3 impulse = ChaVector3(100.0f, 0.0f ,0.0f);

	CBone* parbone = curbone->GetParent();
	if (parbone){
		CBtObject* findbto = parbone->GetBtObject(curbone);
		if (findbto && findbto->GetRigidBody()){
			findbto->GetRigidBody()->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(0.0f, 0.0f, 0.0f));
		}
	}

	return curbone->GetBoneNo();
}
*/

int CModel::InitUndoMotion( int saveflag )
{
	int undono;
	for( undono = 0; undono < UNDOMAX; undono++ ){
		m_undomotion[ undono ].ClearData();
	}

	m_undoid = 0;

	if( saveflag ){
		m_undomotion[0].SaveUndoMotion( this, -1, -1 );
	}


	return 0;
}

int CModel::SaveUndoMotion( int curboneno, int curbaseno )
{
	if( m_bonelist.empty() || !m_curmotinfo ){
		return 0;
	}

	int nextundoid;
	nextundoid = m_undoid + 1;
	if( nextundoid >= UNDOMAX ){
		nextundoid = 0;
	}
	m_undoid = nextundoid;

	CallF( m_undomotion[m_undoid].SaveUndoMotion( this, curboneno, curbaseno ), return 1 );

	return 0;
}
int CModel::RollBackUndoMotion( int redoflag, int* curboneno, int* curbaseno )
{
	if( m_bonelist.empty() || !m_curmotinfo ){
		return 0;
	}

	int rbundoid = -1;
	if( redoflag == 0 ){
		GetValidUndoID( &rbundoid );
	}else{
		GetValidRedoID( &rbundoid );
	}

	if( rbundoid >= 0 ){
		m_undomotion[ rbundoid ].RollBackMotion( this, curboneno, curbaseno );
		m_undoid = rbundoid;
	}

	return 0;
}
int CModel::GetValidUndoID( int* rbundoid )
{
	int retid = -1;

	int chkcnt;
	int curid = m_undoid;
	for( chkcnt = 0; chkcnt < UNDOMAX; chkcnt++ ){
		curid = curid - 1;
		if( curid < 0 ){
			curid = UNDOMAX - 1;
		}

		if( m_undomotion[curid].GetValidFlag() == 1 ){
			retid = curid;
			break;
		}
	}
	*rbundoid = retid;
	return 0;
}
int CModel::GetValidRedoID( int* rbundoid )
{
	int retid = -1;

	int chkcnt;
	int curid = m_undoid;
	for( chkcnt = 0; chkcnt < UNDOMAX; chkcnt++ ){
		curid = curid + 1;
		if( curid >= UNDOMAX ){
			curid = 0;
		}

		if( m_undomotion[curid].GetValidFlag() == 1 ){
			retid = curid;
			break;
		}
	}
	*rbundoid = retid;

	return 0;
}

int CModel::AddBoneMotMark( OWP_Timeline* owpTimeline, int curboneno, int curlineno, double startframe, double endframe, int flag )
{
	if( (curboneno < 0) || (curlineno < 0) ){
		_ASSERT( 0 );
		return 1;
	}
	CBone* curbone = m_bonelist[ curboneno ];
	if( curbone ){
		curbone->AddBoneMotMark( m_curmotinfo->motid, owpTimeline, curlineno, startframe, endframe, flag );
	}

	return 0;
}

float CModel::GetTargetWeight( int motid, double srcframe, double srctimescale, CMQOObject* srcbaseobj, std::string srctargetname )
{
	FbxAnimLayer* curanimlayer = GetAnimLayer( motid );
	if( !curanimlayer ){
		return 0.0f;
	}

	FbxTime lTime;
	lTime.SetSecondDouble( srcframe / srctimescale );

	return GetFbxTargetWeight( m_fbxobj[srcbaseobj].node, m_fbxobj[srcbaseobj].mesh, srctargetname, lTime, curanimlayer, srcbaseobj );
}

float CModel::GetFbxTargetWeight(FbxNode* pbaseNode, FbxMesh* pbaseMesh, std::string targetname, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* baseobj )
{
    int lVertexCount = pbaseMesh->GetControlPointsCount();
	if( lVertexCount != baseobj->GetVertex() ){
		_ASSERT( 0 );
		return 0.0f;
	}

	int lBlendShapeDeformerCount = pbaseMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)pbaseMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; lChannelIndex++)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if(lChannel)
			{
				const char* nameptr = lChannel->GetName();
				int cmp0;
				cmp0 = strcmp( nameptr, targetname.c_str() );
				if( cmp0 == 0 ){
					FbxAnimCurve* lFCurve;
					double lWeight = 0.0;
					lFCurve = pbaseMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
					if (lFCurve){
						lWeight = lFCurve->Evaluate(pTime);
					}else{
						lWeight = 0.0;
					}

					return (float)lWeight;
				}
			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

	return 0.0f;

}

int CModel::SetFirstFrameBonePos(HINFO* phinfo)
{
	int motid = m_curmotinfo->motid;
	if (motid < 0){
		_ASSERT(0);
		return 0;
	}
	if (!GetTopBone()){
		_ASSERT(0);
		return 0;
	}

	SetFirstFrameBonePosReq(GetTopBone(), motid, phinfo);

	if (phinfo->maxh >= phinfo->minh){
		phinfo->height = phinfo->maxh - phinfo->minh;
	}
	else{
		phinfo->height = 0.0f;
		_ASSERT(0);
	}


	return 0;
}

void CModel::SetFirstFrameBonePosReq(CBone* srcbone, int srcmotid, HINFO* phinfo)
{
	if (srcbone){
		CMotionPoint* curmp = 0;
		double curframe = 0.0;

		curmp = srcbone->GetMotionPoint(srcmotid, curframe);
		if (!curmp){
			_ASSERT(0);
		}

		if (curmp){
			ChaMatrix firstmat = curmp->GetWorldMat();
			srcbone->CalcFirstFrameBonePos(firstmat);
			ChaVector3 firstpos = srcbone->GetFirstFrameBonePos();
			if (firstpos.y < phinfo->minh){
				phinfo->minh = firstpos.y;
			}
			if (firstpos.y > phinfo->maxh){
				phinfo->maxh = firstpos.y;
			}
		}
	}

	if (srcbone->GetChild()){
		SetFirstFrameBonePosReq(srcbone->GetChild(), srcmotid, phinfo);
	}
	if (srcbone->GetBrother()){
		SetFirstFrameBonePosReq(srcbone->GetBrother(), srcmotid, phinfo);
	}
}

int CModel::RecalcBoneAxisX(CBone* srcbone)
{
	if (GetOldAxisFlagAtLoading() == 1){
		_ASSERT(0);
		return 1;
	}

	if (!srcbone){
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
			CBone* curbone = itrbone->second;
			if (curbone){
				ChaMatrix axismat;
				//axismat = curbone->GetFirstAxisMatZ();
				if (curbone->GetParent()){
					curbone->GetParent()->CalcAxisMatX(curbone, &axismat, 1);
				}
				else{
					ChaMatrixIdentity(&axismat);
				}
				axismat._41 = curbone->GetJointFPos().x;
				axismat._42 = curbone->GetJointFPos().y;
				axismat._43 = curbone->GetJointFPos().z;
				curbone->SetNodeMat(axismat);
			}
		}
	}
	else{
		ChaMatrix axismat;
		//axismat = srcbone->GetFirstAxisMatZ();
		if (srcbone->GetParent()){
			srcbone->GetParent()->CalcAxisMatX(srcbone, &axismat, 1);
		}
		else{
			ChaMatrixIdentity(&axismat);
		}
		axismat._41 = srcbone->GetJointFPos().x;
		axismat._42 = srcbone->GetJointFPos().y;
		axismat._43 = srcbone->GetJointFPos().z;
		srcbone->SetNodeMat(axismat);
	}

	return 0;
}

void CModel::CalcBoneEulReq(CBone* curbone, int srcmotid, double srcframe)
{
	if (!curbone){
		return;
	}

	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
	int paraxsiflag = 1;
	int isfirstbone = 0;
	cureul = curbone->CalcLocalEulZXY(paraxsiflag, srcmotid, srcframe, BEFEUL_ZERO, isfirstbone);
	curbone->SetLocalEul(srcmotid, srcframe, cureul);

	if (curbone->GetChild()){
		CalcBoneEulReq(curbone->GetChild(), srcmotid, srcframe);
	}
	if (curbone->GetBrother()){
		CalcBoneEulReq(curbone->GetBrother(), srcmotid, srcframe);
	}
}


int CModel::CalcBoneEul(int srcmotid)
{
	if (srcmotid >= 0){
		MOTINFO* mi = GetMotInfo(srcmotid);
		if (mi){
			double frame;
			for (frame = 0.0; frame < mi->frameleng; frame += 1.0){
				CalcBoneEulReq(GetTopBone(), mi->motid, frame);
			}
		}
	}
	else{
		map<int, MOTINFO*>::iterator itrmi;
		for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++){
			int motid = itrmi->first;
			if (motid >= 0){
				CalcBoneEul(motid);
			}
		}
	}

	return 0;
}

void CModel:: DumpBtObjectReq(CBtObject* srcbto, int srcdepth)
{
	if (!srcbto){
		return;
	}

	if (srcdepth == 0){
		DbgOut(L"\r\n\r\nStart DumpBtObjectReq\r\n");
	}

	if (srcbto){
		int tabno;
		for (tabno = 0; tabno < srcdepth; tabno++){
			DbgOut(L"\t");
		}
		if (srcbto->GetBone() && srcbto->GetEndBone()){
			DbgOut(L"BtObject : %s---%s\r\n",
				srcbto->GetBone()->GetWBoneName(), srcbto->GetEndBone()->GetWBoneName());
		}
		else{
			DbgOut(L"BtObject : (NULL)---(NULL)\r\n");
		}
	}

	int childnum = srcbto->GetChilBtSize();
	int childno;
	for (childno = 0; childno < childnum; childno++){
		CBtObject* childbto = srcbto->GetChilBt(childno);
		if (childbto){
			DumpBtObjectReq(childbto, srcdepth + 1);
		}
	}
}

void CModel::DumpBtConstraintReq(CBtObject* srcbto, int srcdepth)
{
	if (!srcbto){
		return;
	}
	if (srcdepth == 0){
		DbgOut(L"\r\n\r\nStart DumpBtConstraintReq\r\n");
	}

	if (srcbto){
		CONSTRAINTELEM curce;
		int ceno;
		for (ceno = 0; ceno < srcbto->GetConstraintSize(); ceno++){
			curce = srcbto->GetConstraintElem(ceno);
			if (curce.centerbone && curce.childbto && curce.constraint){
				int nullflag = 0;
				if (!srcbto->GetBone() || !srcbto->GetEndBone()){
					int tabno;
					for (tabno = 0; tabno < srcdepth; tabno++){
						DbgOut(L"\t");
					}
					DbgOut(L"Constraint : src NULL Bone\r\n");
					nullflag++;
				}
				if (!curce.childbto->GetBone() || !curce.childbto->GetEndBone()){
					int tabno;
					for (tabno = 0; tabno < srcdepth; tabno++){
						DbgOut(L"\t");
					}
					DbgOut(L"Constraint : child NULL Bone\r\n");
					nullflag++;
				}
				if(nullflag == 0){
					int tabno;
					for (tabno = 0; tabno < srcdepth; tabno++){
						DbgOut(L"\t");
					}
					DbgOut(L"Constraint : (%s---%s) +%s+ (%s---%s)\r\n",
						srcbto->GetBone()->GetWBoneName(), srcbto->GetEndBone()->GetWBoneName(),
						curce.centerbone->GetWBoneName(),
						curce.childbto->GetBone()->GetWBoneName(), curce.childbto->GetEndBone()->GetWBoneName());
				}
			}
		}

		int childnum = srcbto->GetChilBtSize();
		int childno;
		for (childno = 0; childno < childnum; childno++){
			CBtObject* childbto = srcbto->GetChilBt(childno);
			if (childbto){
				DumpBtConstraintReq(childbto, srcdepth + 1);
			}
		}
	}
}

int CModel::CreatePhysicsPosConstraint(CBone* srcbone)
{
	if (!srcbone){
		return 0;
	}

	CBone* parbone = srcbone->GetParent();
	if (parbone){
		CBtObject* curbto = parbone->GetBtObject(srcbone);
		if (curbto){
			curbto->CreatePhysicsPosConstraint();
			srcbone->SetPosConstraint(1);
		}
	}

	return 0;
}

int CModel::DestroyPhysicsPosConstraint(CBone* srcbone)
{
	if (!srcbone){
		return 0;
	}

	CBone* parbone = srcbone->GetParent();
	if (parbone){
		CBtObject* curbto = parbone->GetBtObject(srcbone);
		if (curbto){
			curbto->DestroyPhysicsPosConstraint();
			srcbone->SetPosConstraint(0);
		}
	}

	return 0;
}

void CModel::CreatePhysicsPosConstraintReq(CBone* srcbone)
{
	if (srcbone){
		if (srcbone->GetPosConstraint() == 1){
			CreatePhysicsPosConstraint(srcbone);
		}

		if (srcbone->GetChild()){
			CreatePhysicsPosConstraintReq(srcbone->GetChild());
		}
		if (srcbone->GetBrother()){
			CreatePhysicsPosConstraintReq(srcbone->GetBrother());
		}
	}
}


int CModel::SetMass0(CBone* srcbone)
{
	if (!srcbone){
		return 0;
	}

	CBone* parbone = srcbone->GetParent();
	if (parbone){
		CBtObject* curbto = parbone->GetBtObject(srcbone);
		if (curbto){
			btVector3 localInertia(0, 0, 0);
			curbto->GetRigidBody()->setMassProps(0.0, localInertia);
			srcbone->SetMass0(1);
		}
	}
	return 0;
}
int CModel::RestoreMass(CBone* srcbone)
{
	if (!srcbone){
		return 0;
	}

	CBone* parbone = srcbone->GetParent();
	if (parbone){
		btScalar setmass = 0.0;
		CRigidElem* curre = parbone->GetRigidElem(srcbone);
		if (curre){
			setmass = curre->GetMass();
		}

		CBtObject* curbto = parbone->GetBtObject(srcbone);
		if (curbto){
			btVector3 localInertia(0, 0, 0);
			curbto->GetRigidBody()->setMassProps(setmass, localInertia);
			srcbone->SetMass0(0);
		}
	}
	return 0;

}

void CModel::SetMass0Req(CBone* srcbone)
{
	if (srcbone){
		if (srcbone->GetMass0() == 1){
			SetMass0(srcbone);
		}

		if (srcbone->GetChild()){
			SetMass0Req(srcbone->GetChild());
		}
		if (srcbone->GetBrother()){
			SetMass0Req(srcbone->GetBrother());
		}
	}
}

void CModel::RestoreMassReq(CBone* srcbone)
{
	if (srcbone){
		RestoreMass(srcbone);

		if (srcbone->GetChild()){
			RestoreMassReq(srcbone->GetChild());
		}
		if (srcbone->GetBrother()){
			RestoreMassReq(srcbone->GetBrother());
		}
	}
}


int CModel::ApplyBtToMotion()
{
	ApplyBtToMotionReq(m_topbone);

	return 0;
}

void CModel::ApplyBtToMotionReq(CBone* srcbone)
{
	if (!srcbone)
		return;

	if (!m_curmotinfo)
		return;

	if (srcbone->GetParent()){
		ChaMatrix setmat;
		if (srcbone->GetChild()){
			setmat = srcbone->GetBtMat();
		}
		else{
			setmat = srcbone->GetParent()->GetBtMat();
		}

		g_wmatDirectSetFlag = true;
		srcbone->SetWorldMat(0, m_curmotinfo->motid, m_curmotinfo->curframe, setmat);
		g_wmatDirectSetFlag = false;

	}

	if (srcbone->GetChild()){
		ApplyBtToMotionReq(srcbone->GetChild());
	}
	if (srcbone->GetBrother()){
		ApplyBtToMotionReq(srcbone->GetBrother());
	}

}

