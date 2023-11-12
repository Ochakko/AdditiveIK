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
//#include <InfScope.h>
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
//#include <fbxsdk/scene/shading/fbxtexture.h>
//#include <fbxsdk/scene/shading/fbxsurfacematerial.h>
//#include <fbxsdk/scene/animation/fbxanimstack.h>
//#include <fbxsdk/scene/animation/fbxanimlayer.h>


#include <BopFile.h>
#include <BtObject.h>

#include <collision.h>
#include <EditRange.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

using namespace OrgWinGUI;

static int s_alloccnt = 0;

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


extern ChaVector3 g_camEye;
extern ChaVector3 g_camtargetpos;

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

	if( m_pimporter ){
		m_pimporter->Destroy();// インポータの削除
		m_pimporter = 0;
	}

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
	m_motinfo.erase( m_motinfo.begin(), m_motinfo.end() );

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
	m_material.erase( m_material.begin(), m_material.end() );

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
	m_object.erase( m_object.begin(), m_object.end() );

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
	m_bonelist.erase( m_bonelist.begin(), m_bonelist.end() );

	m_topbone = 0;

	m_objectname.erase( m_objectname.begin(), m_objectname.end() );
	m_bonename.erase( m_bonename.begin(), m_bonename.end() );

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

//WCHAR* dbgfind = wcsstr( wfile, L"gplane" );
//if( dbgfile ){
//	_ASSERT( 0 );
//}

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


int CModel::LoadFBX( int skipdefref, ID3D10Device* pdev, WCHAR* wfile, WCHAR* modelfolder, float srcmult, FbxManager* psdk, FbxImporter** ppimporter, FbxScene** ppscene )
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

    pScene = FbxScene::Create(m_psdk,"");

    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    bool lStatus;
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
    pImporter = FbxImporter::Create(m_psdk,"");
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


//	CallF( InitFBXManager( &pSdkManager, &pImporter, &pScene, utf8path ), return 1 );

	m_bone2node.clear();
	FbxNode *pRootNode = pScene->GetRootNode();

	//CBone* rootbone = new CBone( this );
	//_ASSERT( rootbone );
	//rootbone->SetName( "ModelRootBone" );
	//rootbone->m_topboneflag = 1;
	//m_topbone = rootbone;
	//m_bonelist[rootbone->m_boneno] = rootbone;
	//m_bonename[ rootbone->m_bonename ] = rootbone;

	m_topbone = 0;
//	CBone* dummybone = new CBone( this );
//	_ASSERT( dummybone );
//	dummybone->SetName( "dummyBone" );
//	m_bonelist[dummybone->m_boneno] = dummybone;//これをm_topboneにしてはいけない。これは０でエラーが起こらないための応急措置。
//	m_bonename[dummybone->m_bonename] = dummybone;

	CreateFBXBoneReq( pScene, pRootNode, 0 );
	if( m_bonelist.size() <= 1 ){
		_ASSERT( 0 );
		delete (CBone*)(m_bonelist.begin()->second);
		m_bonelist.clear();
		m_topbone = 0;
	}
	CBone* chkbone = m_bonelist[0];
	if( !chkbone ){
		CBone* dummybone = new CBone( this );
		_ASSERT( dummybone );
		dummybone->SetName( "DummyBone" );
	}

	CreateFBXMeshReq( pRootNode );

DbgOut( L"fbx bonenum %d\r\n", m_bonelist.size() );


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


	map<int,CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		curbone->SetBtKinFlag( 1 );
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

	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		curbone->CalcAxisMat( 1, 0.0f );
	}

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
					g_pEffect->SetMatrix( g_hmWorld, &m_matWorld );
					CallF( curobj->GetDispObj()->RenderNormalPM3( lightflag, diffusemult ), return 1 );
				}else if( curobj->GetPm4() ){
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
	DbgOut( L"######start DbgDump\r\n" );

	DbgOut( L"Dump Bone And InfScope\r\n" );

	if( m_topbone ){
		DbgDumpBoneReq( m_topbone, 0 );
	}

//	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, name, 256, wname, 256 );

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

int CModel::Motion2Bt( int firstflag, CModel* coldisp[COL_MAX], double nextframe, ChaMatrix* mW, ChaMatrix* mVP )
{
	UpdateMatrix( mW, mVP );

	if( m_topbt ){
		SetBtKinFlagReq( m_topbt, 0 );
	}

	if( firstflag == 1 ){
		map<int, CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* boneptr = itrbone->second;
			if( boneptr ){
				boneptr->SetStartMat2( boneptr->GetCurMp().GetWorldMat() );
			}
		}
	}

	if( !m_topbt ){
		return 0;
	}

	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* boneptr = itrbone->second;
		if( boneptr ){
			std::map<CBone*, CRigidElem*> tmpmap;
			boneptr->GetRigidElemMap( tmpmap );
			map<CBone*,CRigidElem*>::iterator itrre;
			for( itrre = tmpmap.begin(); itrre != tmpmap.end(); itrre++ ){
				CRigidElem* curre = itrre->second;
				if( curre && (curre->GetSkipflag() != 1) ){
					CBone* chilbone = itrre->first;
					_ASSERT( chilbone );

					boneptr->CalcRigidElemParams( coldisp, chilbone, firstflag );
				}
			}				
		}
	}

/***
CBone* chkbone1 = m_bonename[ "FLOAT_BT_twinte1_L__Joint" ];
_ASSERT( chkbone1 );
CBone* chkbone2 = m_bonename[ "atama_Joint_bunki" ];
_ASSERT( chkbone2 );

DbgOut( L"check kinflag !!!! : previewflag %d, float kinflag %d, bunki kinflag %d\r\n",
	g_previewFlag, chkbone1->m_btkinflag, chkbone2->m_btkinflag );
***/

	Motion2BtReq( m_topbt );


//	if (m_topbone){
//		SetBtEquilibriumPointReq(m_topbone);
//	}


	return 0;
}

void CModel::Motion2BtReq( CBtObject* srcbto )
{
	if( srcbto->GetBone() && (srcbto->GetBone()->GetBtKinFlag() == 1) ){
		srcbto->Motion2Bt();
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

//	ChaMatrix inimat;
//	ChaMatrixIdentity( &inimat );
//	CQuaternion iniq;
//	iniq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

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

int CModel::GetFBXShape( FbxMesh* pMesh, CMQOObject* curobj, FbxAnimLayer* panimlayer, int animleng, FbxTime starttime, FbxTime timestep )
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
				int framecnt;
				for( framecnt = 0; framecnt < animleng; framecnt++ ){
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

//WCHAR wcurname[256]={0L};
//WCHAR wobjname[256]={0L};
//ZeroMemory( wcurname, sizeof( WCHAR ) * 256 );
//MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)nameptr, 256, wcurname, 256 );
//ZeroMemory( wobjname, sizeof( WCHAR ) * 256 );
//MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)curobj->m_name, 256, wobjname, 256 );
//DbgOut( L"addmorph : objname %s, targetname %s\r\n",
//	   wobjname, wcurname );

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

//double curframe = m_curmotinfo->curframe;
//WCHAR wtargetname[256]={0L};
//MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)nameptr, 256, wtargetname, 256 );
//DbgOut( L"weight check !!! : target %s, frame %f, weight %f\r\n",
//	wtargetname, curframe, (float)lWeight );

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
			set3x4[clcnt][0] = tmpmp.GetBtMat()._11;
			set3x4[clcnt][1] = tmpmp.GetBtMat()._12;
			set3x4[clcnt][2] = tmpmp.GetBtMat()._13;

			set3x4[clcnt][3] = tmpmp.GetBtMat()._21;
			set3x4[clcnt][4] = tmpmp.GetBtMat()._22;
			set3x4[clcnt][5] = tmpmp.GetBtMat()._23;

			set3x4[clcnt][6] = tmpmp.GetBtMat()._31;
			set3x4[clcnt][7] = tmpmp.GetBtMat()._32;
			set3x4[clcnt][8] = tmpmp.GetBtMat()._33;

			set3x4[clcnt][9] = tmpmp.GetBtMat()._41;
			set3x4[clcnt][10] = tmpmp.GetBtMat()._42;
			set3x4[clcnt][11] = tmpmp.GetBtMat()._43;
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
	lineno2boneno.erase( lineno2boneno.begin(), lineno2boneno.end() );
	boneno2lineno.erase( boneno2lineno.begin(), boneno2lineno.end() );

	if( m_bonelist.empty() ){
		return 0;
	}

	int lineno = 0;
	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		_ASSERT( curbone );

		//行を追加
		if( curbone->GetType() != FBXBONE_NULL ){
			timeline.newLine( 0, curbone->GetWBoneName() );
		}else{
			timeline.newLine( 1, curbone->GetWBoneName() );
		}

		lineno2boneno[ lineno ] = curbone->GetBoneNo();
		boneno2lineno[ curbone->GetBoneNo() ] = lineno;
		lineno++;
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

	//選択行を設定
	timeline.setCurrentLineName( m_topbone->GetWBoneName() );

	return 0;
}

void CModel::FillTimelineReq( OrgWinGUI::OWP_Timeline& timeline, CBone* curbone, int* linenoptr, 
	map<int, int>& lineno2boneno, map<int, int>& boneno2lineno, int broflag )
{
	//行を追加
	if( curbone->GetType() != FBXBONE_NULL ){
		timeline.newLine( 0, curbone->GetWBoneName() );
	}else{
		timeline.newLine( 1, curbone->GetWBoneName() );
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

	if( findflag == 0 ){
		*dstboneno = srcboneno;
		*existptr = 0;
	}else{
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
		}else{
			*dstboneno = srcboneno;
			*existptr = 0;
		}
	}

	return 0;
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

	ChaVector3TransformCoord( &startlocal, &startsc, &invmWVP );
	ChaVector3TransformCoord( &endlocal, &endsc, &invmWVP );

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
	*worldptr = curbone->GetChildWorld();
	ChaMatrix mWVP = curbone->GetCurMp().GetWorldMat() * m_matVP;
	ChaVector3TransformCoord( screenptr, &curbone->GetJointFPos(), &mWVP );

	float fw, fh;
	fw = (float)winx / 2.0f;
	fh = (float)winy / 2.0f;
	dispptr->x = ( 1.0f + screenptr->x ) * fw;
	dispptr->y = ( 1.0f - screenptr->y ) * fh;
	dispptr->z = screenptr->z;

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

int CModel::AdvanceTime( int previewflag, double difftime, double* nextframeptr, int* endflagptr, int srcmotid )
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

	if( previewflag > 0 ){
		nextframe = curframe + difftime / oneframe * curspeed;
		if( nextframe > ( curmotinfo->frameleng - 1.0 ) ){
			if( loopflag == 0 ){
				nextframe = curmotinfo->frameleng - 1.0;
				*endflagptr = 1;
			}else{
				nextframe = 0.0;
			}
		}
	}else{
		nextframe = curframe - difftime / oneframe * curspeed;
		if( nextframe < 0.0 ){
			if( loopflag == 0 ){
				nextframe = 0.0;
				*endflagptr = 1;
			}else{
				nextframe = curmotinfo->frameleng - 1.0;
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

	int defmaterialno = m_material.size();
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

int CModel::CreateFBXShape( FbxAnimLayer* panimlayer, int animleng, FbxTime starttime, FbxTime timestep )
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
		for( int i = 0; i < materialNum_; ++i ) {
			FbxSurfaceMaterial* material = node->GetMaterial( i );
			if ( material != 0 ) {
				CMQOMaterial* newmqomat = new CMQOMaterial();
				int mqomatno = newobj->GetMaterialSize();
				newmqomat->SetMaterialNo( mqomatno );
				newobj->SetMaterial( mqomatno, newmqomat );

				SetMQOMaterial( newmqomat, material );

			}
		}
	}

//頂点
	int PolygonNum       = pMesh->GetPolygonCount();
	int PolygonVertexNum = pMesh->GetPolygonVertexCount();
	int *IndexAry        = pMesh->GetPolygonVertices();

	int controlNum = pMesh->GetControlPointsCount();   // 頂点数
	FbxVector4* src = pMesh->GetControlPoints();    // 頂点座標配列

	// コピー
	newobj->SetVertex( controlNum );
	newobj->SetPointBuf( (ChaVector3*)malloc( sizeof( ChaVector3 ) * controlNum ) );
	for ( int i = 0; i < controlNum; ++i ) {
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
	for ( int i = 0; i < layerNum; ++i ) {
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
			  for ( int i = 0; i < normalNum; ++i ) {
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
				for ( int i = 0; i < normalNum; ++i ) {
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
			DbgOut(L"GetFBXMesh : %s : UV : mapping eByPolygonVertex\r\n", wname);

			if (refMode == FbxLayerElement::eDirect) {
				DbgOut(L"GetFBXMesh : %s : UV : refMode eDirect\r\n", wname);
				int size = UVNum;
				newobj->SetUVLeng(size);
				newobj->SetUVBuf((D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size));

				// 直接取得
				for (int i = 0; i < size; ++i) {
					(newobj->GetUVBuf() + i)->x = (float)elem->GetDirectArray().GetAt(i)[0];
					(newobj->GetUVBuf() + i)->y = (float)elem->GetDirectArray().GetAt(i)[1];
				}
			}
			else if (refMode == FbxLayerElement::eIndexToDirect) {
				DbgOut(L"GetFBXMesh : %s : UV : refMode eIndexToDirect\r\n", wname);
				int size = indexNum;
				newobj->SetUVLeng(size);
				newobj->SetUVBuf((D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size));

				// インデックスから取得
				for (int i = 0; i < size; ++i) {
					int index = elem->GetIndexArray().GetAt(i);
					(newobj->GetUVBuf() + i)->x = (float)elem->GetDirectArray().GetAt(index)[0];
					(newobj->GetUVBuf() + i)->y = (float)elem->GetDirectArray().GetAt(index)[1];
				}
			}
			else {
				DbgOut(L"GetFBXMesh : %s : UV : refMode %d\r\n", wname, refMode);
			}
		}
		else if (mappingMode == FbxLayerElement::eByControlPoint) {
			if (refMode == FbxLayerElement::eDirect) {
				DbgOut(L"GetFBXMesh : %s : UV : refMode eDirect\r\n", wname);
				int size = UVNum;
				newobj->SetUVLeng(size);
				newobj->SetUVBuf((D3DXVECTOR2*)malloc(sizeof(D3DXVECTOR2) * size));

				// 直接取得
				for (int i = 0; i < size; ++i) {
					(newobj->GetUVBuf() + i)->x = (float)elem->GetDirectArray().GetAt(i)[0];
					(newobj->GetUVBuf() + i)->y = (float)elem->GetDirectArray().GetAt(i)[1];
				}
			}
		} else {
DbgOut( L"GetFBXMesh : %s : UV : mappingMode %d\r\n", wname, mappingMode );
DbgOut( L"GetFBXMesh : %s : UV : refMode %d\r\n", wname, refMode );
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
	


//		int fcnt = 0;
//		FbxTime chktime;
//		for( chktime = mStart; chktime < mStop; chktime += mFrameTime ){
//			fcnt++;
//		}
//		int animleng = fcnt;

		int animleng = (int)( (mStop.GetSecondDouble() - mStart.GetSecondDouble()) * 30.0 );
//		int animleng = (int)( (mStop.GetSecondDouble() - mStart.GetSecondDouble()) * 300.0 );
//		_ASSERT( 0 );

//char mes[256];
//sprintf_s( mes, 256, "%d", animleng );
//MessageBoxA( NULL, mes, "check", MB_OK );


		//_ASSERT( 0 );


		DbgOut( L"FBX anim %d, animleng %d\r\n", animno, animleng );




		int curmotid = -1;
		AddMotion( mAnimStackNameArray[animno]->Buffer(), 0, (double)animleng, &curmotid );


		map<int,CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* curbone = itrbone->second;
			if( curbone ){
				curbone->SetGetAnimFlag( 0 );
			}
		}


        //FbxPose* pPose = pScene->GetPose( animno );
		//FbxPose* pPose = pScene->GetPose( 10 );
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

int CModel::CreateFBXAnimReq( int animno, FbxPose* pPose, FbxNode* pNode, int motid, int animleng, FbxTime mStart, FbxTime mFrameTime )
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
int CModel::GetFBXAnim( int animno, FbxNode* pNode, FbxPose* pPose, FbxNodeAttribute *pAttrib, int motid, int animleng, FbxTime mStart, FbxTime mFrameTime )
{
	FbxAMatrix pGlobalPosition;
	pGlobalPosition.SetIdentity();


	FbxMesh *pMesh = (FbxMesh*)pAttrib;


	// スキンの数を取得
	int skinCount  = pMesh->GetDeformerCount( FbxDeformer::eSkin );

	for ( int i = 0; i < skinCount; ++i ) {
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
				int framecnt;
				for( framecnt = 0; framecnt < animleng; framecnt++ ){
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

					if( (animno == 0) && (framecnt == 0) ){
						curbone->SetFirstMat( xmat );
						curbone->SetInitMat( xmat );
						ChaMatrix calcmat = curbone->GetNodeMat() * curbone->GetInvFirstMat();
						ChaVector3 zeropos(0.0f, 0.0f, 0.0f);
						ChaVector3 tmppos;
						ChaVector3TransformCoord(&tmppos, &zeropos, &calcmat);
						curbone->SetJointFPos(tmppos);
					}

					CMotionPoint* curmp = 0;
					int existflag = 0;
					curmp = curbone->AddMotionPoint( motid, (double)(framecnt), &existflag );
					if( !curmp ){
						_ASSERT( 0 );
						return 1;
					}
					curmp->SetWorldMat( xmat );

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
	for ( int i = 0; i < skinCount; ++i ) {
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

int CModel::RenderBoneMark( ID3D10Device* pdev, CModel* bmarkptr, CMySprite* bcircleptr, CModel* cpslptr[COL_MAX], int selboneno, int skiptopbonemark )
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

	if( selboneno > 0 ){
		CBone* selbone = m_bonelist[ selboneno ];
		if( selbone ){
			SetSelectFlagReq( selbone, 0 );
			selbone->SetSelectFlag( 2 );
	
			CBone* parbone = selbone->GetParent();
			if( parbone ){
				CBtObject* curbto = FindBtObject( selbone->GetBoneNo() );
				if( curbto ){
					int tmpflag = selbone->GetSelectFlag() + 4;
					selbone->SetSelectFlag(tmpflag);
				}
			}
		}
	}

	//pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

	if( g_bonemarkflag && bmarkptr ){
		map<int, CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* boneptr = itrbone->second;
			if( boneptr ){
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
						ChaVector3TransformCoord(&aftbonepos, &boneptr->GetJointFPos(), &(boneptr->GetCurMp().GetWorldMat()));

						ChaVector3 aftchilpos;
						ChaVector3TransformCoord(&aftchilpos, &chilbone->GetJointFPos(), &(chilbone->GetCurMp().GetWorldMat()));


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


	if( cpslptr ){

		map<int, CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* boneptr = itrbone->second;
			if( boneptr ){
				map<CBone*,CRigidElem*> tmpmap;
				boneptr->GetRigidElemMap( tmpmap );
				map<CBone*,CRigidElem*>::iterator itrre;
				for( itrre = tmpmap.begin(); itrre != tmpmap.end(); itrre++ ){
					CRigidElem* curre = itrre->second;
					if( curre && (curre->GetSkipflag() != 1) ){
						CBone* chilbone = itrre->first;
						_ASSERT( chilbone );

						CModel* curcoldisp = cpslptr[curre->GetColtype()];
						_ASSERT( curcoldisp );

//DbgOut( L"check!!!: curbone %s, chilbone %s\r\n", boneptr->m_wbonename, chilbone->m_wbonename );

						boneptr->CalcRigidElemParams( cpslptr, chilbone, 0 );

						g_pEffect->SetMatrix( g_hmWorld, &(curre->GetCapsulemat()) );
						curcoldisp->UpdateMatrix( &(curre->GetCapsulemat()), &m_matVP );
						ChaVector4 difmult;
						//if( boneptr->GetSelectFlag() & 4 ){
						if (chilbone->GetSelectFlag() & 4){
							difmult = ChaVector4( 1.0f, 0.0f, 0.0f, 0.5f );
						}else{
							difmult = ChaVector4( 0.25f, 0.5f, 0.5f, 0.5f );
						}
						CallF( curcoldisp->OnRender( pdev, 0, difmult ), return 1 );
					}
				}
			}
		}
	}


	if( g_bonemarkflag && bcircleptr ){
		map<int, CBone*>::iterator itrbone;
		for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			CBone* boneptr = itrbone->second;
			if( boneptr && (boneptr->GetType() == FBXBONE_NORMAL) ){

				ChaMatrix bcmat;
				bcmat = boneptr->GetCurMp().GetWorldMat();
				//CBone* parbone = boneptr->GetParent();
				//CBone* chilbone = boneptr->GetChild();
				ChaMatrix transmat = bcmat * m_matVP;
				ChaVector3 scpos;
				ChaVector3 firstpos = boneptr->GetJointFPos();

				ChaVector3TransformCoord( &scpos, &firstpos, &transmat );
				scpos.z = 0.0f;
				bcircleptr->SetPos( scpos );
				D3DXVECTOR2 bsize;
				if( boneptr->GetSelectFlag() & 2 ){
					bcircleptr->SetColor( ChaVector4( 0.0f, 0.0f, 1.0f, 0.7f ) );
					bsize = D3DXVECTOR2( 0.050f, 0.050f );
					bcircleptr->SetSize( bsize );
				}else if( boneptr->GetSelectFlag() & 1 ){
					bcircleptr->SetColor( ChaVector4( 1.0f, 0.0f, 0.0f, 0.7f ) );
					bsize = D3DXVECTOR2( 0.025f, 0.025f );
					bcircleptr->SetSize( bsize );
				}else{
					bcircleptr->SetColor( ChaVector4( 1.0f, 1.0f, 1.0f, 0.7f ) );
					bsize = D3DXVECTOR2( 0.025f, 0.025f );
					bcircleptr->SetSize( bsize );
				}
				CallF( bcircleptr->OnRender(), return 1 );

			}
		}
	}

	//pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	return 0;
}
void CModel::SetDefaultBonePosReq( CBone* curbone, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition )
{
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
	ChaVector3TransformCoord( &tmppos, &zeropos, &(curbone->GetNodeMat()) );
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


int CModel::SetDefaultBonePos()
{
	if( !m_topbone ){
		return 0;
	}

	FbxPose* bindpose = 0;
	FbxPose* curpose = m_pscene->GetPose( 0 );
	int curpindex = 1;
	while( curpose ){
		if( curpose->IsBindPose() ){
			bindpose = curpose;
			break;
		}
		curpose = m_pscene->GetPose( curpindex );
		curpindex++;
	}

	if( !bindpose ){
		::MessageBoxA( NULL, "バインドポーズがありません。", "警告", MB_OK );
		bindpose = m_pscene->GetPose( 0 );
	}

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


void CModel::FillUpEmptyKeyReq( int motid, int animleng, CBone* curbone, CBone* parbone )
{
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

	int framecnt;
	for( framecnt = 0; framecnt < animleng; framecnt++ ){
		double frame = (double)framecnt;

		ChaMatrix mvmat;
		ChaMatrixIdentity( &mvmat );

		CMotionPoint* pbef = 0;
		CMotionPoint* pnext = 0;
		int existflag = 0;
		curbone->GetBefNextMP( motid, frame, &pbef, &pnext, &existflag );
		if( existflag == 0 ){
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
				newmp->SetWorldMat( tmpmat );
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
		curbone->ClearBtObject();
	}


	return 0;
}
void CModel::DestroyBtObjectReq( CBtObject* curbt )
{

	std::vector<CBtObject*> tmpbt;
	curbt->CopyChilBt( tmpbt );


	delete curbt;


	int chilno;
	for( chilno = 0; chilno < (int)tmpbt.size(); chilno++ ){
		CBtObject* chilbt = tmpbt[ chilno ];
		if( chilbt ){
			DestroyBtObjectReq( chilbt );
		}
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
					if (curre->GetSkipflag() == 0){
						srcbone->SetBtKinFlag(0);
					}
					else{
						srcbone->SetBtKinFlag(1);
					}
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

		if( (srcbone->GetBtKinFlag() == 1) && (srcbto->GetRigidBody()) ){
			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
			if( s_setrigidflag == 0 ){
				s_rigidflag = curflag;
				s_setrigidflag = 1;
			}
			//srcbto->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			srcbto->GetRigidBody()->setCollisionFlags( curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
			//srcbto->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
			//srcbto->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
			//srcbto->m_rigidbody->setActivationState(DISABLE_SIMULATION);
			//CF_STATIC_OBJECT
		}else if( srcbto->GetRigidBody() ){
			if( srcbone->GetParent() ){
				CRigidElem* curre = srcbone->GetParent()->GetRigidElem( srcbone );
				if( curre ){
					if ((m_curreindex >= 0) && (m_curreindex < m_rigideleminfo.size())){
						srcbto->GetRigidBody()->setGravity(btVector3(0.0f, curre->GetBtg() * m_rigideleminfo[m_curreindex].btgscale, 0.0f));
						srcbto->GetRigidBody()->applyGravity();
					}
					else{
						_ASSERT(0);
					}
				}
			}
		}

	}

	int chilno;
	for( chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChilBt( chilno );
		SetBtKinFlagReq( chilbto, oncreateflag );
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

	CreateBtConnectReq( m_topbone );

	return 0;
}
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
void CModel::CreateBtConnectReq(CBone* curbone)
{
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
								dofC->setBreakingImpulseThreshold(FLT_MAX);

								int l_kindex = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetLKindex();
								int a_kindex = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetAKindex();
								float l_damping = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetLDamping();
								float a_damping = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetADamping();
								float l_cusk = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetCusLk();
								float a_cusk = bto1->GetBone()->GetRigidElem(bto1->GetEndBone())->GetCusAk();

								int dofid;
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


								dofC->setEquilibriumPoint();

								bto1->PushBackConstraint(dofC);
								m_btWorld->addConstraint(dofC, false);//!!!!!!!!!!!! disable collision between linked bodies
								//m_btWorld->addConstraint(dofC, true);
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


/*
void CModel::CreateBtConnectReq( CBone* curbone )
{
	char* findpat = strstr( (char*)curbone->GetBoneName(), "bunki" );
	if( findpat ){

		map<CBone*,CBtObject*>::iterator itrbto1;
		for( itrbto1 = curbone->GetBtObjectMapBegin(); itrbto1 != curbone->GetBtObjectMapEnd(); itrbto1++ ){
			CBtObject* bto1 = itrbto1->second;

			map<CBone*,CBtObject*>::iterator itrbto2;
			for( itrbto2 = curbone->GetBtObjectMapBegin(); itrbto2 != curbone->GetBtObjectMapEnd(); itrbto2++ ){
				CBtObject* bto2 = itrbto2->second;
				//if( (bto1 != bto2) && bto1->m_rigidbody && bto2->m_rigidbody && (bto1->m_connectflag == 0) && (bto2->m_connectflag == 0) ){
				if( (bto1 != bto2) && bto1->GetRigidBody() && bto2->GetRigidBody() && (bto2->GetConnectFlag() == 0) ){
					ChaVector3 diffchil;
					diffchil = bto1->GetEndBone()->GetJointFPos() - bto2->GetEndBone()->GetJointFPos();
					float diffleng = ChaVector3Length( &diffchil );
					if( diffleng < 0.0001f ){

DbgOut( L"CreateBtConnect : bto1 %s--%s, bto2 %s--%s\r\n",
	   bto1->GetBone()->GetWBoneName(), bto1->GetEndBone()->GetWBoneName(),
	   bto2->GetBone()->GetWBoneName(), bto2->GetEndBone()->GetWBoneName()
);

						float angPAI2, angPAI;
						angPAI2 = 90.0f * (float)DEG2PAI;
						angPAI = 180.0f * (float)DEG2PAI;
			
						float lmax, lmin;
						lmax = 10000.0f;
						lmin = -10000.0f;

						btGeneric6DofSpringConstraint* dofC;
						btTransform tmpA, tmpA2;
						bto1->GetFrameA( tmpA );
						bto2->GetFrameA( tmpA2 );
						dofC = new btGeneric6DofSpringConstraint( *bto1->GetRigidBody(), *bto2->GetRigidBody(), tmpA, tmpA2, true );
						_ASSERT( dofC );

						dofC->setLinearLowerLimit( btVector3( lmin, lmin, lmin ) );
						dofC->setLinearUpperLimit( btVector3( lmax, lmax, lmax ) );
						//dofC->setAngularLowerLimit( btVector3( -angPAI, -angPAI2, -angPAI ) );
						//dofC->setAngularUpperLimit( btVector3( angPAI, angPAI2, angPAI ) );
						dofC->setAngularLowerLimit( btVector3( angPAI, angPAI2, angPAI ) );
						dofC->setAngularUpperLimit( btVector3( -angPAI, -angPAI2, -angPAI ) );
						dofC->setBreakingImpulseThreshold( FLT_MAX );
						
						int l_kindex = bto1->GetBone()->GetRigidElem( bto1->GetEndBone() )->GetLKindex();
						int a_kindex = bto1->GetBone()->GetRigidElem( bto1->GetEndBone() )->GetAKindex();
						float l_damping = bto1->GetBone()->GetRigidElem( bto1->GetEndBone() )->GetLDamping();
						float a_damping = bto1->GetBone()->GetRigidElem( bto1->GetEndBone() )->GetADamping();
						float l_cusk = bto1->GetBone()->GetRigidElem( bto1->GetEndBone() )->GetCusLk();
						float a_cusk = bto1->GetBone()->GetRigidElem( bto1->GetEndBone() )->GetCusAk();

						int dofid;
						for( dofid = 0; dofid < 3; dofid++ ){
							dofC->enableSpring( dofid, true );//!!!!!!!!!!!!!!!!!!!
							//dofC->setStiffness( dofid, 1000.0f );
							//dofC->setStiffness( dofid, 2000.0f );
							dofC->setStiffness( dofid, 1.0e12 );

							dofC->setDamping( dofid, 0.5f );
						}
						for( dofid = 3; dofid < 6; dofid++ ){
							dofC->enableSpring( dofid, true );//!!!!!!!!!!!!!!!!!
							//dofC->setStiffness( dofid, 0.5f );
							dofC->setStiffness( dofid, 80.0f );
							dofC->setDamping( dofid, 0.01f );
						}


						dofC->setEquilibriumPoint();

						bto1->PushBackConstraint( dofC );
						//m_btWorld->addConstraint(dofC, true);
						m_btWorld->addConstraint(dofC, false);//!!!!!!!!!!!! disable collision between linked bodies

						bto2->SetConnectFlag( 1 );
					}
				}
			}
			bto1->SetConnectFlag( 1 );
		}
	}


	if( curbone->GetChild() ){
		CreateBtConnectReq( curbone->GetChild() );
	}
	if( curbone->GetBrother() ){
		CreateBtConnectReq( curbone->GetBrother() );
	}
}
*/

int CModel::CreateBtObject( CModel* coldisp[COL_MAX], int onfirstcreate )
{

	DestroyBtObject();

	if( !m_topbone ){
		return 0;
	}

	CalcBtAxismatReq( coldisp, m_topbone, 0.0f );//!!!!!!!!!!!!!
	


	m_topbt = new CBtObject( 0, m_btWorld );
	if( !m_topbt ){
		_ASSERT( 0 );
		return 1;
	}
	m_topbt->SetTopFlag( 1 );

	//CBone* startbone = m_bonename[ "jiku_Joint_bunki" ];
	//CBone* startbone = m_bonename[ "jiku_Joint" ];
	CBone* startbone = m_topbone;
	//CBone* startbone = m_bonename[ "Bip01" ];
	_ASSERT( startbone );
	CreateBtObjectReq( coldisp, m_topbt, startbone, startbone->GetChild() );
	//CreateBtObjectReq( coldisp, m_topbt, startbone->m_parent, startbone );

/***
	CBone* brobone = GetValidBroBone( startbone );
	if( brobone ){
		CreateBtObjectReq( coldisp, m_topbt, brobone, brobone->m_child );
		brobone = brobone->m_brother;
	}
***/
	CreateBtConstraint();


	if( m_topbt ){
		SetBtKinFlagReq( m_topbt, onfirstcreate );
	}

	if (m_topbone){
		SetBtEquilibriumPointReq(m_topbone);
	}

	return 0;          
}

int CModel::SetBtEquilibriumPointReq( CBone* curbone )
{

	map<CBone*, CBtObject*>::iterator itrbto;
	for (itrbto = curbone->GetBtObjectMapBegin(); itrbto != curbone->GetBtObjectMapEnd(); itrbto++){
		CBtObject* curbto = itrbto->second;
		if (curbto){
			int lflag, aflag;
			double curframe = m_curmotinfo->curframe;
			if (curframe == 0.0){
				lflag = 1;
			}
			else{
				lflag = 0;
			}
			aflag = 1;
			curbto->SetEquilibriumPoint( lflag, aflag );
		}
	}

	if (curbone->GetBrother()){
		SetBtEquilibriumPointReq(curbone->GetBrother());
	}
	if (curbone->GetChild()){
		SetBtEquilibriumPointReq(curbone->GetChild());
	}

	return 0;
}



void CModel::CreateBtObjectReq( CModel* cpslptr[COL_MAX], CBtObject* parbt, CBone* parbone, CBone* curbone )
{
	map<CBone*, CRigidElem*> tmpmap;
	curbone->GetRigidElemMap(tmpmap);

	CBtObject* newbto = 0;
	CBone* chilbone = 0;

	map<CBone*, CRigidElem*>::iterator itrre;
	for (itrre = tmpmap.begin(); itrre != tmpmap.end(); itrre++){
		CRigidElem* curre = itrre->second;
		chilbone = itrre->first;
		ChaVector3 diffbone = curbone->GetJointFPos() - chilbone->GetJointFPos();
		float leng = ChaVector3Length(&diffbone);
		map<CBone*, CBone*>::iterator itrfind = m_rigidbone.find(chilbone);
		if (curre && chilbone){
			if (itrfind == m_rigidbone.end()){
				if (curre->GetSkipflag() == 0){
					DbgOut(L"CreateBtObject : curbone %s, chilbone %s\r\n",
						curbone->GetWBoneName(), chilbone->GetWBoneName());

					m_rigidbone[chilbone] = curbone;
					newbto = new CBtObject(parbt, m_btWorld);
					if (!newbto){
						_ASSERT(0);
						return;
					}
					CallF(newbto->CreateObject(parbt, parbone, curbone, chilbone), return);
					curbone->SetBtObject(chilbone, newbto);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if (curbone->GetChild()){
		CreateBtObjectReq(cpslptr, newbto, curbone, curbone->GetChild());
	}
	if (curbone->GetBrother()){
		CreateBtObjectReq(cpslptr, parbt, parbone, curbone->GetBrother());
	}
}

void CModel::CalcBtAxismatReq( CModel* coldisp[COL_MAX], CBone* curbone, float delta )
{
	int setstartflag;
	if( delta == 0.0f ){
		setstartflag = 1;
		curbone->SetStartMat2( curbone->GetCurMp().GetWorldMat() );
	}else{
		setstartflag = 0;
	}

	if( curbone->GetChild() ){
		//curbone->CalcAxisMat( firstflag, delta );
		curbone->CalcRigidElemParams( coldisp, curbone->GetChild(), setstartflag );
		CBone* bro = curbone->GetChild()->GetBrother();
		while( bro ){
			curbone->CalcRigidElemParams( coldisp, bro, setstartflag );
			bro = bro->GetBrother();
		}
DbgOut( L"check!!!!:CalcBtAxismatReq : %s, %s\r\n", curbone->GetWBoneName(), curbone->GetChild()->GetWBoneName() );

	}

	if( curbone->GetChild() ){
		CalcBtAxismatReq( coldisp, curbone->GetChild(), delta );
	}
	if( curbone->GetBrother() ){
		CalcBtAxismatReq( coldisp, curbone->GetBrother(), delta );
	}
}

int CModel::SetBtMotion( int ragdollflag, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat, double difftime )
{
	m_matWorld = *wmat;
	m_matVP = *vpmat;

	if( !m_topbt ){
		_ASSERT( 0 );
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
			CMotionPoint curmp = curbone->GetCurMp();
			curmp.SetBtFlag( 0 );
			curbone->SetCurMp( curmp );
		}
	}

	SetBtMotionReq( m_topbt, wmat, vpmat );


	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone && (curbone->GetCurMp().GetBtFlag() == 0) ){
			if( g_previewFlag == 4 ){
				if( curbone->GetBtKinFlag() == 0 ){
					if( curbone->GetParent() ){
						//curbone->m_curmp.m_btmat = curbone->m_parent->m_curmp.m_btmat;
						ChaMatrix invstart;
						ChaMatrixInverse( &invstart, NULL, &(curbone->GetParent()->GetStartMat2()) );
						ChaMatrix diffmat;
						diffmat = invstart * curbone->GetParent()->GetCurMp().GetBtMat();
						CMotionPoint curmp = curbone->GetCurMp();
						curmp.SetBtMat( curbone->GetStartMat2() * diffmat );
						curbone->SetCurMp( curmp );
					}else{
						CMotionPoint curmp = curbone->GetCurMp();
						curmp.SetBtMat( curbone->GetStartMat2() );
						curbone->SetCurMp( curmp );
					}
				}else{
					CMotionPoint curmp = curbone->GetCurMp();
					curmp.SetBtMat( curmp.GetWorldMat() );
					curbone->SetCurMp( curmp );
				}
			}else if( g_previewFlag == 5 ){
				if( curbone->GetParent() ){
					//curbone->m_curmp.m_btmat = curbone->m_parent->m_curmp.m_btmat;
					ChaMatrix invstart;
					ChaMatrixInverse( &invstart, NULL, &(curbone->GetParent()->GetStartMat2()) );
					ChaMatrix diffmat;
					diffmat = invstart * curbone->GetParent()->GetCurMp().GetBtMat();
					CMotionPoint curmp = curbone->GetCurMp();
					curmp.SetBtMat( curbone->GetStartMat2() * diffmat );
					curbone->SetCurMp( curmp );
				}else{
					CMotionPoint curmp = curbone->GetCurMp();
					curmp.SetBtMat( curbone->GetStartMat2() );
					curbone->SetCurMp( curmp );
				}
			}
			CMotionPoint curmp = curbone->GetCurMp();
			curmp.SetBtFlag( 1 );
			curbone->SetCurMp( curmp );
		}
	}


	if (m_topbone){
		SetBtEquilibriumPointReq(m_topbone);
	}


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
							btGeneric6DofSpringConstraint* curct = curbto->GetConstraint( constraintno );
							if( curct ){
								int dofid;
								for( dofid = 0; dofid < 3; dofid++ ){
									curct->setDamping( dofid, newdampl );
								}
								for( dofid = 3; dofid < 6; dofid++ ){
									curct->setDamping( dofid, newdampa );
								}
							}
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

	int motionnum = m_motinfo.size();
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
	if( g_previewFlag == 4 ){
		if( (curbto->GetTopFlag() == 0) && curbto->GetBone() && (curbto->GetBone()->GetBtKinFlag() == 0) ){
			curbto->SetBtMotion();
		}
	}else if( g_previewFlag == 5 ){
		if( (curbto->GetTopFlag() == 0) && curbto->GetBone() ){
			curbto->SetBtMotion();
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
	CBone* parbone = curbone->GetParent();
	if (parbone){
		parbone->CreateRigidElem(curbone, reflag, rename, impflag, impname);
	}

	/*
	CBone* chil = curbone->GetChild();
	if( chil ){
		curbone->CreateRigidElem( chil, reflag, rename, impflag, impname );
		
		CBone* chilbro;
		chilbro = chil->GetBrother();
		while( chilbro ){
			curbone->CreateRigidElem( chilbro, reflag, rename, impflag, impname );
			chilbro = chilbro->GetBrother();
		}
	}
	*/

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
			_ASSERT(0);
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
	if ((srcrgdindex >= 0) && (srcrgdindex < m_rigideleminfo.size())){
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
	if ((srcrgdindex >= 0) && (srcrgdindex < m_rigideleminfo.size())){
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
	if ((reindex >= 0) && (reindex < m_rigideleminfo.size())){
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

	if ((rgdindex >= 0) && (rgdindex < m_rigideleminfo.size())){
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
	if ((m_rgdindex < 0) || (m_rgdindex >= m_rigideleminfo.size())){
		return;
	}

	CBone* parbone = srcbone->GetParent();

	if( parbone ){
		int renum = m_rigideleminfo.size();
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
		_ASSERT( 0 );
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
		if ((reindex >= 0) && (reindex < m_rigideleminfo.size())){
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
		if ((reindex >= 0) && (reindex < m_rigideleminfo.size())){
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
		if ((reindex >= 0) && (reindex < m_rigideleminfo.size())){
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
		if ((reindex >= 0) && (reindex < m_rigideleminfo.size())){
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

int CModel::SetRagdollKinFlag()
{

	if( !m_topbt ){
		return 0;
	}

	SetRagdollKinFlagReq( m_topbt );

	return 0;
}
void CModel::SetRagdollKinFlagReq( CBtObject* srcbto )
{

	if( srcbto->GetBone() && srcbto->GetRigidBody() ){
		DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
		srcbto->GetRigidBody()->setCollisionFlags( curflag & ~btCollisionObject::CF_KINEMATIC_OBJECT);

//		_ASSERT( s_setrigidflag );
//		srcbto->m_rigidbody->setCollisionFlags( s_rigidflag );
//		srcbto->m_rigidbody->setCollisionFlags( s_rigidflag  | btCollisionObject::CF_STATIC_OBJECT);
//		srcbto->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);

		if( srcbto->GetBone()->GetParent() ){
			CRigidElem* curre = srcbto->GetBone()->GetParent()->GetRigidElem( srcbto->GetBone() );
			if( curre ){
				if ((m_rgdindex >= 0) && (m_rgdindex < m_rigideleminfo.size())){
					srcbto->GetRigidBody()->setGravity(btVector3(0.0f, curre->GetBtg() * m_rigideleminfo[m_rgdindex].btgscale, 0.0f));
					srcbto->GetRigidBody()->applyGravity();
				}
				else{
					_ASSERT(0);
				}
			}
		}
	}

	int chilno;
	for( chilno = 0; chilno < srcbto->GetChilBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChilBt( chilno );
		if( chilbto ){
			SetRagdollKinFlagReq( chilbto );
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
	map<int,CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		CallF( curbone->SetCurrentRigidElem( curname ), return 1 );
	}

	return 0;
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
		if ((m_curreindex >= 0) && (m_curreindex < m_rigideleminfo.size())){
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
			curbto->GetRigidBody()->setDeactivationTime(0);
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
		list<KeyInfo>::iterator itrki;
		double firstframe = 0.0;
		for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
			double curframe = itrki->time;
			int existflag3 = 0;
			CMotionPoint* editmp = 0;
			CMotionPoint* nextmp = 0;
			curbone->GetBefNextMP( m_curmotinfo->motid, curframe, &editmp, &nextmp, &existflag3 );
			if( existflag3 ){
				editmp->SetBefEditMat( editmp->GetWorldMat() ); 
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
	list<KeyInfo>::iterator itrki;
	double firstframe = 0.0;
	for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
		double curframe = itrki->time;
		int existflag3 = 0;
		CMotionPoint* editmp = 0;
		CMotionPoint* nextmp = 0;
		curbone->GetBefNextMP( m_curmotinfo->motid, curframe, &editmp, &nextmp, &existflag3 );
		if( existflag3 ){
			editmp->SetBefEditMat( editmp->GetWorldMat() ); 
		}else{
			_ASSERT( 0 );
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
				ChaVector3TransformCoord( &chilworld, &(curbone->GetJointFPos()), &(curbone->GetCurMp().GetWorldMat()) );
				ChaVector3TransformCoord( &parworld, &(parbone->GetJointFPos()), &(parbone->GetCurMp().GetWorldMat()) );

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
					CQuaternion rotq;
					rotq.SetAxisAndRot( rotaxis2, rotrad2 );
					if( keynum >= 2 ){
						int keyno = 0;
						list<KeyInfo>::iterator itrki;

						for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
							double curframe = itrki->time;
							double changerate;
							if( curframe <= applyframe ){
								if( applyframe != startframe ){
									changerate = 1.0 / (applyframe - startframe);
								}else{
									changerate = 1.0;
								}
							}else{
								if( applyframe != endframe ){
									changerate = 1.0 / (endframe - applyframe );
								}else{
									changerate = 0.0;
								}
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
										if( applyframe != startframe ){
											currate2 = changerate * (curframe - startframe);
										}else{
											currate2 = 1.0;
										}
									}else{
										currate2 = changerate * (endframe - curframe);
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
						parbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
					}


					if( g_applyendflag == 1 ){
						//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
						int tolast;
						for( tolast = (int)m_curmotinfo->curframe + 1; tolast < m_curmotinfo->frameleng; tolast++ ){
							(m_bonelist[ 0 ])->PasteRotReq( m_curmotinfo->motid, m_curmotinfo->curframe, tolast );
						}
					}

					//parbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
					UpdateMatrix( &m_matWorld, &m_matVP );
				}else{
					UpdateMatrix( &m_matWorld, &m_matVP );
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

int CModel::AdjustBoneTra( CEditRange* erptr, CBone* lastpar )
{
	if( g_applyendflag == 1 ){
		if( lastpar && (erptr->m_ki.size() >= 2) ){
			list<KeyInfo>::iterator itrki;
			int keyno = 0;
			int startframe = (int)erptr->m_ki.begin()->time;
			int endframe = (int)m_curmotinfo->frameleng - 1;
			double curframe;
			for( curframe = startframe; curframe <= endframe; curframe += 1.0 ){
				if( keyno >= 1 ){
					int existflag2 = 0;
					CMotionPoint* pbef = 0;
					CMotionPoint* pnext = 0;
					int curmotid = m_curmotinfo->motid;
					lastpar->GetBefNextMP( curmotid, curframe, &pbef, &pnext, &existflag2 );
					if( existflag2 ){
						ChaVector3 orgpos;
						ChaVector3TransformCoord( &orgpos, &(lastpar->GetJointFPos()), &(pbef->GetBefEditMat()) );

						ChaVector3 newpos;
						ChaVector3TransformCoord( &newpos, &(lastpar->GetJointFPos()), &(pbef->GetWorldMat()) );

						ChaVector3 diffpos;
						diffpos = orgpos - newpos;

						CEditRange tmper;
						KeyInfo tmpki;
						tmpki.time = curframe;
						list<KeyInfo> tmplist;
						tmplist.push_back( tmpki );
						tmper.SetRange( tmplist, curframe );
						FKBoneTra( &tmper, lastpar->GetBoneNo(), diffpos );
					}
				}
				keyno++;
			}
		}
	}else{
		if( lastpar && (erptr->m_ki.size() >= 2) ){
			list<KeyInfo>::iterator itrki;
			int keyno = 0;
			for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
				if( keyno >= 1 ){
					double curframe = itrki->time;
					int existflag2 = 0;
					CMotionPoint* pbef = 0;
					CMotionPoint* pnext = 0;
					int curmotid = m_curmotinfo->motid;
					lastpar->GetBefNextMP( curmotid, curframe, &pbef, &pnext, &existflag2 );
					if( existflag2 ){
						ChaVector3 orgpos;
						ChaVector3TransformCoord( &orgpos, &(lastpar->GetJointFPos()), &(pbef->GetBefEditMat()) );

						ChaVector3 newpos;
						ChaVector3TransformCoord( &newpos, &(lastpar->GetJointFPos()), &(pbef->GetWorldMat()) );

						ChaVector3 diffpos;
						diffpos = orgpos - newpos;

						CEditRange tmper;
						KeyInfo tmpki;
						tmpki.time = curframe;
						list<KeyInfo> tmplist;
						tmplist.push_back( tmpki );
						tmper.SetRange( tmplist, curframe );
						FKBoneTra( &tmper, lastpar->GetBoneNo(), diffpos );
					}
				}
				keyno++;
			}
		}
	}

	return 0;
}
int CModel::IKRotateAxisDelta( CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt )
{
	ChaMatrix selectmat = ChaMatrix( 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f );
	int levelcnt = 0;
	CBone* firstbone = m_bonelist[ srcboneno ];
	if( !firstbone ){
		_ASSERT( 0 );
		return 1;
	}

	CBone* curbone = firstbone;
	SetBefEditMat( erptr, curbone, maxlevel );


	CBone* calcrootbone = GetCalcRootBone( firstbone, maxlevel );
	_ASSERT( calcrootbone );
	ChaVector3 rootpos = calcrootbone->GetChildWorld();

	ChaVector3 axis0;
	ChaVector3 targetpos;
	ChaMatrix mat, befrotmat, rotmat, aftrotmat;

	float rotrad;

	if (firstbone->GetBoneLeng() > 0.00001f){
		selectmat = firstbone->GetFirstAxisMatX() * firstbone->GetInvFirstMat() * firstbone->GetCurMp().GetWorldMat();
	}
	else{
		selectmat = firstbone->GetInvFirstMat() * firstbone->GetCurMp().GetWorldMat();
	}
	selectmat._41 = 0.0f;
	selectmat._42 = 0.0f;
	selectmat._43 = 0.0f;

	if( axiskind == PICK_X ){
		axis0 = ChaVector3( 1.0f, 0.0f, 0.0f );
		ChaVector3TransformCoord( &m_ikrotaxis, &axis0, &selectmat );
		ChaVector3Normalize( &m_ikrotaxis, &m_ikrotaxis );
	}else if( axiskind == PICK_Y ){
		axis0 = ChaVector3( 0.0f, 1.0f, 0.0f );
		ChaVector3TransformCoord( &m_ikrotaxis, &axis0, &selectmat );
		ChaVector3Normalize( &m_ikrotaxis, &m_ikrotaxis );
	}else if( axiskind == PICK_Z ){
		axis0 = ChaVector3( 0.0f, 0.0f, 1.0f );
		ChaVector3TransformCoord( &m_ikrotaxis, &axis0, &selectmat );
		ChaVector3Normalize( &m_ikrotaxis, &m_ikrotaxis );
	}else{
		_ASSERT( 0 );
		return 1;
	}
	rotrad = delta / 10.0f * (float)PAI / 12.0f;

	if( fabs(rotrad) < (0.02f * (float)DEG2PAI) ){
		return 0;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange( &keynum, &startframe, &endframe, &applyframe );
	int calcnum = 3;

	CBone* lastpar = firstbone;
	double firstframe = 0.0;

	if( (firstbone == calcrootbone) && !firstbone->GetParent() ){
		int calccnt;
		for( calccnt = 0; calccnt < calcnum; calccnt++ ){
			int levelcnt = 0;

			float currate = 1.0f;


			float rotrad2 = rotrad * currate;
			if( fabs( rotrad2 ) > 1.0e-4 ){
				CQuaternion rotq;
				rotq.SetAxisAndRot( m_ikrotaxis, rotrad2 );

				if( keynum >= 2 ){
					int keyno = 0;
					list<KeyInfo>::iterator itrki;
					for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
						double curframe = itrki->time;
						double changerate;
						if( curframe <= applyframe ){
							if( applyframe != startframe ){
								changerate = 1.0 / (applyframe - startframe);
							}else{
								changerate = 1.0;
							}
						}else{
							if( applyframe != endframe ){
								changerate = 1.0 / (endframe - applyframe );
							}else{
								changerate = 0.0;
							}
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
									if( applyframe != startframe ){
										currate2 = changerate * (curframe - startframe);
									}else{
										currate2 = 1.0;
									}
								}else{
									currate2 = changerate * (endframe - curframe);
								}
								rotq.Slerp2( endq, 1.0 - currate2, &curq );

								firstbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, curq );
							}else{
								firstbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, rotq );
							}
						}else{
							if( keyno == 0 ){
								firstbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, rotq );
							}else{
								firstbone->SetAbsMatReq( 0, m_curmotinfo->motid, curframe, firstframe );
							}
						}
						keyno++;
					}


					if( g_applyendflag == 1 ){
						//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
						int tolast;
						for( tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++ ){
							(m_bonelist[ 0 ])->PasteRotReq( m_curmotinfo->motid, m_curmotinfo->curframe, tolast );
						}
					}


				}else{
					firstbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
				}

				//parbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
				UpdateMatrix( &m_matWorld, &m_matVP );

				if( (calccnt == (calcnum - 1)) && g_absikflag ){
					AdjustBoneTra( erptr, firstbone );
				}
			}
		}		
	}else{
		ChaVector3 rcen;
		ChaVector3TransformCoord( &rcen, &(firstbone->GetJointFPos()), &(firstbone->GetCurMp().GetWorldMat()) );

		ChaMatrixTranslation( &befrotmat, -rootpos.x, -rootpos.y, -rootpos.z );
		ChaMatrixTranslation( &aftrotmat, rootpos.x, rootpos.y, rootpos.z );
		ChaMatrixRotationAxis( &rotmat, &m_ikrotaxis, rotrad );
		mat = firstbone->GetCurMp().GetWorldMat() * befrotmat * rotmat * aftrotmat;
		ChaVector3TransformCoord( &targetpos, &firstbone->GetJointFPos(), &mat );

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
					//chilworld = firstbone->m_childworld;
					ChaVector3TransformCoord( &chilworld, &(curbone->GetJointFPos()), &(curbone->GetCurMp().GetWorldMat()) );
					ChaVector3TransformCoord( &parworld, &(parbone->GetJointFPos()), &(parbone->GetCurMp().GetWorldMat()) );

					ChaVector3 parbef, chilbef, tarbef;
					parbef = parworld;
					CalcShadowToPlane( chilworld, m_ikrotaxis, parworld, &chilbef );
					CalcShadowToPlane( targetpos, m_ikrotaxis, parworld, &tarbef );

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
					if( fabs( rotrad2 ) > 1.0e-4 ){
						CQuaternion rotq;
						rotq.SetAxisAndRot( rotaxis2, rotrad2 );

						if( keynum >= 2 ){
							int keyno = 0;
							list<KeyInfo>::iterator itrki;
							for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
								double curframe = itrki->time;

								double changerate;
								if( curframe <= applyframe ){
									if( applyframe != startframe ){
										changerate = 1.0 / (applyframe - startframe);
									}else{
										changerate = 1.0;
									}
								}else{
									if( applyframe != endframe ){
										changerate = 1.0 / (endframe - applyframe );
									}else{
										changerate = 0.0;
									}
								}

								if( keyno == 0 ){
									firstframe = curframe;
								}
								if( g_absikflag == 0 ){
									if( g_slerpoffflag == 0 ){
										double currate2;
										CQuaternion endq;
										CQuaternion curq;
										endq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
										if( curframe <= applyframe ){
											if( applyframe != startframe ){
												currate2 = changerate * (curframe - startframe);
											}else{
												currate2 = 1.0;
											}
										}else{
											currate2 = changerate * (endframe - curframe);
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
							parbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
						}


						if( g_applyendflag == 1 ){
							//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
							int tolast;
							for( tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++ ){
								(m_bonelist[ 0 ])->PasteRotReq( m_curmotinfo->motid, m_curmotinfo->curframe, tolast );
							}
						}

						//parbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
						UpdateMatrix( &m_matWorld, &m_matVP );
					}else{
						UpdateMatrix( &m_matWorld, &m_matVP );
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

	}

	if( lastpar ){
		return lastpar->GetBoneNo();
	}else{
		return srcboneno;
	}

}

int CModel::RotateXDelta( CEditRange* erptr, int srcboneno, float delta )
{
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

	if (firstbone->GetBoneLeng() > 0.00001f){
		selectmat = firstbone->GetFirstAxisMatX() * firstbone->GetInvFirstMat() * firstbone->GetCurMp().GetWorldMat();
	}
	else{
		selectmat = firstbone->GetInvFirstMat() * firstbone->GetCurMp().GetWorldMat();
	}
	selectmat._41 = 0.0f;
	selectmat._42 = 0.0f;
	selectmat._43 = 0.0f;

	axis0 = ChaVector3( 1.0f, 0.0f, 0.0f );
	ChaVector3TransformCoord( &rotaxis, &axis0, &selectmat );
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
	double firstframe = 0.0;

	if( keynum >= 2 ){
		int keyno = 0;
		list<KeyInfo>::iterator itrki;
		for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
			double curframe = itrki->time;
			double changerate;
			if( curframe <= applyframe ){
				if( applyframe != startframe ){
					changerate = 1.0 / (applyframe - startframe);
				}else{
					changerate = 1.0;
				}
			}else{
				if( applyframe != endframe ){
					changerate = 1.0 / (endframe - applyframe );
				}else{
					changerate = 0.0;
				}
			}

			if( keyno == 0 ){
				firstframe = curframe;
			}
			if( g_absikflag == 0 ){
				if( g_slerpoffflag == 0 ){
					double currate2;
					CQuaternion endq;
					CQuaternion curq;
					endq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
					if( curframe <= applyframe ){
						if( applyframe != startframe ){
							currate2 = changerate * (curframe - startframe);
						}else{
							currate2 = 1.0;
						}
					}else{
						currate2 = changerate * (endframe - curframe);
					}
					rotq.Slerp2( endq, 1.0 - currate2, &curq );

					curbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, curq );
				}else{
					curbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, rotq );
				}
			}else{
				if( keyno == 0 ){
					curbone->RotBoneQReq( 0, m_curmotinfo->motid, curframe, rotq );
				}else{
					curbone->SetAbsMatReq( 0, m_curmotinfo->motid, curframe, firstframe );
				}
			}
			keyno++;
		}

		if( g_applyendflag == 1 ){
			//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
			int tolast;
			for( tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++ ){
				(m_bonelist[ 0 ])->PasteRotReq( m_curmotinfo->motid, m_curmotinfo->curframe, tolast );
			}
		}


	}else{
		curbone->RotBoneQReq( 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq );
	}

	UpdateMatrix( &m_matWorld, &m_matVP );

	if( g_absikflag && curbone ){
		AdjustBoneTra( erptr, curbone );
	}

	return curbone->GetBoneNo();
}


//int CModel::FKRotate( double srcframe, int srcboneno, ChaMatrix srcmat )
int CModel::FKRotate(int reqflag, CBone* bvhbone, int traflag, ChaVector3 traanim, double srcframe, int srcboneno, CQuaternion rotq, int setmatflag, ChaMatrix* psetmat)
{
	static CMotionPoint* s_dbgmp0 = 0;
	static CMotionPoint* s_dbgmp1 = 0;

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
		CMotionPoint* pbef = 0;
		CMotionPoint* pnext = 0;
		int existflag = 0;
		parbone->GetBefNextMP(m_curmotinfo->motid, srcframe, &pbef, &pnext, &existflag);
		if ((existflag != 0) && pbef){
			parmp = pbef;
		}
	}

	if (reqflag == 1){
		curbone->RotBoneQReq(0, m_curmotinfo->motid, srcframe, rotq, bvhbone, traanim, setmatflag, psetmat);
	}
	else if(bvhbone){
		ChaMatrix setmat = bvhbone->GetTmpMat();
		curbone->RotBoneQOne(parmp, m_curmotinfo->motid, srcframe, setmat);
	}

	//UpdateMatrix(&m_matWorld, &m_matVP);
	/*
	CBone* lastpar = curbone->GetParent();
	double curframe = srcframe;
	int existflag2 = 0;
	CMotionPoint* pbef = 0;
	CMotionPoint* pnext = 0;
	int curmotid = m_curmotinfo->motid;
	lastpar->GetBefNextMP(curmotid, curframe, &pbef, &pnext, &existflag2);
	if (existflag2){
		ChaVector3 orgpos;
		ChaVector3TransformCoord(&orgpos, &(lastpar->GetJointFPos()), &(pbef->GetBefEditMat()));

		ChaVector3 newpos;
		ChaVector3TransformCoord(&newpos, &(lastpar->GetJointFPos()), &(pbef->GetWorldMat()));

		ChaVector3 diffpos;
		diffpos = orgpos - newpos;

		CEditRange tmper;
		KeyInfo tmpki;
		tmpki.time = curframe;
		list<KeyInfo> tmplist;
		tmplist.push_back(tmpki);
		tmper.SetRange(tmplist, curframe);
		FKBoneTra(&tmper, lastpar->GetBoneNo(), diffpos);
	}
	*/
	return curbone->GetBoneNo();
}

int CModel::FKBoneTra( CEditRange* erptr, int srcboneno, ChaVector3 addtra )
{
	static CMotionPoint* s_dbgmp0 = 0;
	static CMotionPoint* s_dbgmp1 = 0;

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
		float changerate = 1.0f / (float)(endframe - startframe);

		int keyno = 0;
		list<KeyInfo>::iterator itrki;
		for( itrki = erptr->m_ki.begin(); itrki != erptr->m_ki.end(); itrki++ ){
			double curframe = itrki->time;
			double changerate;
			if( curframe <= applyframe ){
				if( applyframe != startframe ){
					changerate = 1.0 / (applyframe - startframe);
				}else{
					changerate = 1.0;
				}
			}else{
				if( applyframe != endframe ){
					changerate = 1.0 / (endframe - applyframe );
				}else{
					changerate = 0.0;
				}
			}

			if( keyno == 0 ){
				firstframe = curframe;
			}
			if( g_absikflag == 0 ){
				if( g_slerpoffflag == 0 ){
					double currate2;
					if( curframe <= applyframe ){
						if( applyframe != startframe ){
							currate2 = changerate * (curframe - startframe);
						}else{
							currate2 = 1.0;
						}
					}else{
						currate2 = changerate * (endframe - curframe);
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

					int existflag0 = 0;
					CMotionPoint* pbef0 = 0;
					CMotionPoint* pnext0 = 0;
					curbone->GetBefNextMP( m_curmotinfo->motid, curframe, &pbef0, &pnext0, &existflag0 );
					_ASSERT( existflag0 );
					s_dbgmp0 = pbef0;
				}else{
					curbone->SetAbsMatReq( 0, m_curmotinfo->motid, curframe, firstframe );

					int existflag1 = 0;
					CMotionPoint* pbef1 = 0;
					CMotionPoint* pnext1 = 0;
					curbone->GetBefNextMP( m_curmotinfo->motid, curframe, &pbef1, &pnext1, &existflag1 );
					_ASSERT( existflag1 );
					s_dbgmp1 = pbef1;
				}
			}
			keyno++;

		}
	}else{
		curbone->AddBoneTraReq( 0, m_curmotinfo->motid,  startframe, addtra );
	}
	UpdateMatrix( &m_matWorld, &m_matVP );


	return curbone->GetBoneNo();
}

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

		int existflag = 0;
		CMotionPoint* befmp = 0;
		CMotionPoint* nextmp = 0;
		srcbone->GetBefNextMP(srcmotid, curframe, &befmp, &nextmp, &existflag);
		if (existflag && befmp){
			curmp = befmp;
		}
		else{
			_ASSERT(0);
			curmp = 0;
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



