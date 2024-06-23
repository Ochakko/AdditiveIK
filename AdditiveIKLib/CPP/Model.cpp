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

#include <InfoWindow.h>

#include <FbxFile.h>

#include <EGPFile.h>

#define DBGH
#include <dbg.h>

//#include <DXUT.h>
//#include <DXUTcamera.h>
//#include <DXUTgui.h>
//#include <DXUTsettingsdlg.h>
//#include <SDKmisc.h>

#include <DispObj.h>
#include <MySprite.h>

#include <MotionPoint.h>
//#include <quaternion.h>
//#include <ChaVecCalc.h>//TheadingCalcEul.hでChaCalcFunc.hからChaVecCalc.hはインクルート

#include <Collision.h>
#include <EngName.h>

#include <RigidElem.h>
#include <ChaScene.h>
#include <CSChkInView.h>

#include <string>

#include <fbxsdk.h>
#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <fbxsdk/scene/animation/fbxanimevaluator.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"


#include <BopFile.h>
#include <BtObject.h>
#include <FbxMisc.h>

#include <collision.h>
#include <EditRange.h>
//#include <BoneProp.h>

#include <ThreadingLoadFbx.h>
#include <ThreadingUpdateMatrix.h>
#include <ThreadingPostIK.h>
#include <ThreadingFKTra.h>
#include <ThreadingCopyW2LW.h>
#include <ThreadingCalcEul.h>
#include <ThreadingRetarget.h>
#include <ThreadingInitMp.h>

#include <NodeOnLoad.h>

#include <OrgWindow.h>

#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
#include "../../MiniEngine/TResourceBank.h"

//#include <DXUT.h>
#include <io.h>


//for __nop()
#include <intrin.h>


using namespace std;


//extern
//extern TResourceBank<CMQOMaterial> g_materialbank;


static bool s_workingChkinView = false;


////######################################
//// Custom stream 
//
//static long long int s_mpos = 0;
//
//class CustomStreamClass : public FbxStream
//{
//public:
//	CustomStreamClass(FbxManager* pSdkManager, const char* mode, const char* ut8name)
//	{
//		mFile = NULL;
//		mbuffer = 0;
//		msize = 0;
//		s_mpos = 0;
//
//		// expect the mode to contain something
//		if (mode == NULL) return;
//
//
//		FBXSDK_strcpy(mFileName, MAX_PATH, ut8name);
//		FBXSDK_strcpy(mMode, 3, (mode) ? mode : "r");
//
//		if (mode[0] == 'r')
//		{
//			const char* format = "FBX (*.fbx)";
//			mReaderID = pSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription(format);
//			mWriterID = -1;
//		}
//		else
//		{
//			//const char* format = "FBX ascii (*.fbx)";
//			//mWriterID = pSdkManager->GetIOPluginRegistry()->FindWriterIDByDescription(format);
//			//mReaderID = -1;
//			mWriterID = -1;
//			mReaderID = -1;
//		}
//	}
//
//	~CustomStreamClass()
//	{
//		Close();
//	}
//
//	virtual EState GetState()
//	{
//		return mFile ? FbxStream::eOpen : eClosed;
//	}
//
//	virtual bool Open(void* /*pStreamData*/)
//	{
//		if (mMode[0] != 'r') {
//			return false;
//		}
//
//		if (mFile == NULL) {
//			fopen_s(&mFile, mFileName, mMode);
//			if (!mFile) {
//				return false;
//			}
//
//			msize = _filelengthi64(_fileno(mFile));
//			if (msize == -1L) {
//				return false;
//			}
//
//			mbuffer = (char*)malloc(sizeof(char) * msize);
//			if (!mbuffer) {
//				return false;
//			}
//			ZeroMemory(mbuffer, sizeof(char) * msize);
//
//			long long int doread = 0;
//			while (doread < msize) {
//				doread += fread(mbuffer + doread, 1, (msize - doread), mFile);
//			}
//			s_mpos = 0;
//			return true;
//		}
//		else {
//			return false;
//		}
//
//
//	}
//
//	virtual bool Close()
//	{
//		// This method can be called several times during the
//		// Initialize phase so it is important that it can handle multiple closes
//		if (mFile) {
//			fclose(mFile);
//			mFile = NULL;
//		}
//		if (mbuffer) {
//			free(mbuffer);
//			mbuffer = 0;
//		}
//		s_mpos = 0;
//
//		mWriterID = -1;
//		mReaderID = -1;
//
//		return true;
//	}
//
//	virtual bool Flush()
//	{
//		return true;
//	}
//
//	virtual int Write(const void* pData, int pSize)
//	{
//		//if (mFile == NULL)
//		//	return 0;
//		//return (int)fwrite(pData, 1, pSize, mFile);
//		return 0;
//	}
//
//	virtual int Read(void* pData, int pSize) const
//	{
//		if (mFile == NULL) {
//			return 0;
//		}
//		if (!mbuffer) {
//			return 0;
//		}
//
//
//		long long int readsize;
//		if ((s_mpos >= 0) && ((s_mpos + pSize) <= msize)) {
//			readsize = pSize;
//		}
//		else if ((s_mpos >= 0) && ((s_mpos + pSize) > msize)) {
//			readsize = msize - s_mpos;
//		}
//		else {
//			readsize = 0;
//		}
//
//		if (readsize > 0) {
//			MoveMemory(pData, mbuffer + s_mpos, readsize);
//		}
//
//		s_mpos = s_mpos + readsize;//constなのでメンバー変数を変えることが出来ない。mposを外で定義する。
//
//		return readsize;
//	}
//
//	virtual int GetReaderID() const
//	{
//		return mReaderID;
//	}
//
//	virtual int GetWriterID() const
//	{
//		return mWriterID;
//	}
//
//	void Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos)
//	{
//		switch (pSeekPos)
//		{
//		case FbxFile::eBegin:
//			//fseek(mFile, (long)pOffset, SEEK_SET);
//			s_mpos = (long)pOffset;
//			break;
//		case FbxFile::eCurrent:
//			//fseek(mFile, (long)pOffset, SEEK_CUR);
//			s_mpos = s_mpos + (long)pOffset;
//			break;
//		case FbxFile::eEnd:
//			//fseek(mFile, (long)pOffset, SEEK_END);
//			s_mpos = msize - (long)pOffset;
//			break;
//		}
//	}
//
//	virtual long GetPosition() const
//	{
//		if (mFile == NULL) {
//			return 0;
//		}
//		if (!mbuffer) {
//			return 0;
//		}
//		//return ftell(mFile);
//		return s_mpos;
//	}
//	virtual void SetPosition(long pPosition)
//	{
//		if (mFile && mbuffer) {
//			//fseek(mFile, pPosition, SEEK_SET);
//			s_mpos = pPosition;
//		}
//	}
//
//	virtual int GetError() const
//	{
//		if (mFile == NULL) {
//			return 0;
//		}
//		if (!mbuffer) {
//			return 0;
//		}
//		//return ferror(mFile);
//		if ((s_mpos < 0) || (s_mpos > msize)) {
//			return -1;
//		}
//		return 0;
//	}
//	virtual void ClearError()
//	{
//		if (mFile != NULL) {
//			//clearerr(mFile);
//			s_mpos = 0;
//		}
//	}
//
//private:
//	FILE* mFile;
//	int             mReaderID;
//	int             mWriterID;
//	char    mFileName[MAX_PATH];
//	char    mMode[3];
//	char* mbuffer;
//	long long int msize;
//	//long long int mpos;
//};
//
//
////#######################################
////#######################################


using namespace OrgWinGUI;

static int s_alloccnt = 0;
static FbxManager* s_psdk = 0;
static FbxAnimEvaluator* s_animevaluator = 0;

#define ADDITIVEIKLIBGLOBALVAR
#include <GlobalVar.h>

extern LONG g_retargetbatchflag;
extern bool g_btsimurecflag;

extern CRITICAL_SECTION g_CritSection_GetGP;
extern int CalcLocalMeshMat(FbxMesh* pMesh, ChaMatrix* dstmeshmat);//FbxFile.cpp


/*
extern ID3D11DepthStencilState *g_pDSStateZCmp;
extern ID3D11DepthStencilState *g_pDSStateZCmpAlways;

extern int g_boneaxis;
extern int g_dbgloadcnt;
extern int g_pseudolocalflag;
extern int g_previewFlag;			// プレビューフラグ
extern WCHAR g_basedir[ MAX_PATH ];

extern ID3D11Effect*		g_pEffect;
extern ID3D11EffectTechnique* g_hRenderBoneL0;
extern ID3D11EffectTechnique* g_hRenderBoneL1;
extern ID3D11EffectTechnique* g_hRenderBoneL2;
extern ID3D11EffectTechnique* g_hRenderBoneL3;
extern ID3D11EffectTechnique* g_hRenderNoBoneL0;
extern ID3D11EffectTechnique* g_hRenderNoBoneL1;
extern ID3D11EffectTechnique* g_hRenderNoBoneL2;
extern ID3D11EffectTechnique* g_hRenderNoBoneL3;
extern ID3D11EffectTechnique* g_hRenderLine;
extern ID3D11EffectTechnique* g_hRenderSprite;

extern ID3D11EffectMatrixVariable* g_hm4x4Mat;
extern ID3D11EffectMatrixVariable* g_hmWorld;
extern ID3D11EffectMatrixVariable* g_hmVP;

extern ID3D11EffectVectorVariable* g_hEyePos;
extern ID3D11EffectScalarVariable* g_hnNumLight;
extern ID3D11EffectVectorVariable* g_hLightDir;
extern ID3D11EffectVectorVariable* g_hLightDiffuse;
extern ID3D11EffectVectorVariable* g_hLightAmbient;

extern ID3D11EffectVectorVariable*g_hdiffuse;
extern ID3D11EffectVectorVariable* g_hambient;
extern ID3D11EffectVectorVariable* g_hspecular;
extern ID3D11EffectScalarVariable* g_hpower;
extern ID3D11EffectVectorVariable* g_hemissive;
extern ID3D11EffectShaderResourceVariable* g_hMeshTexture;


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

//extern bool g_wmatDirectSetFlag;
*/

int g_dbgflag = 0;


//////////



//以下の５つの関数は関数名にFbxを付けて FbxFile.h, FbxFile.cppに移動になりました。リンクエラーの関係で。
// 
// 
// 調査の結果、リンクエラーの原因はFbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);だったのですが以下の５つの関数はFbxFileに移動のままで。
// 
// 
//static FbxAMatrix GetGlobalPosition((CModel* srcmodel, FbxScene* pScene, FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition = NULL );
//static FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
//static FbxAMatrix GetGeometry(FbxNode* pNode);
//static FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial* pMaterial, const char* pPropertyName, const char* pFactorPropertyName, char** ppTextureName);
//static int IsValidCluster(FbxCluster* pcluster);


static void ChaMatrix2FbxAMatrix(FbxAMatrix& retmat, ChaMatrix& srcmat);
static void FbxAMatrix2ChaMatrix(ChaMatrix& retmat, FbxAMatrix srcmat);


void ChaMatrix2FbxAMatrix(FbxAMatrix& retmat, ChaMatrix& srcmat)
{
	retmat.SetIdentity();
	retmat.SetRow(0, FbxVector4(srcmat.data[MATI_11], srcmat.data[MATI_12], srcmat.data[MATI_13], srcmat.data[MATI_14]));
	retmat.SetRow(1, FbxVector4(srcmat.data[MATI_21], srcmat.data[MATI_22], srcmat.data[MATI_23], srcmat.data[MATI_24]));
	retmat.SetRow(2, FbxVector4(srcmat.data[MATI_31], srcmat.data[MATI_32], srcmat.data[MATI_33], srcmat.data[MATI_34]));
	retmat.SetRow(3, FbxVector4(srcmat.data[MATI_41], srcmat.data[MATI_42], srcmat.data[MATI_43], srcmat.data[MATI_44]));
	//retmat.SetRow(0, FbxVector4(srcmat._11, srcmat._21, srcmat._31, srcmat._41));
	//retmat.SetRow(1, FbxVector4(srcmat._12, srcmat._22, srcmat._32, srcmat._42));
	//retmat.SetRow(2, FbxVector4(srcmat._13, srcmat._23, srcmat._33, srcmat._43));
	//retmat.SetRow(3, FbxVector4(srcmat._14, srcmat._24, srcmat._34, srcmat._44));

}

void FbxAMatrix2ChaMatrix(ChaMatrix& retmat, FbxAMatrix srcmat)
{
	ChaMatrixIdentity(&retmat);
	retmat.SetParams(
		(float)srcmat.Get(0, 0), (float)srcmat.Get(0, 1), (float)srcmat.Get(0, 2), (float)srcmat.Get(0, 3),
		(float)srcmat.Get(1, 0), (float)srcmat.Get(1, 1), (float)srcmat.Get(1, 2), (float)srcmat.Get(1, 3),
		(float)srcmat.Get(2, 0), (float)srcmat.Get(2, 1), (float)srcmat.Get(2, 2), (float)srcmat.Get(2, 3),
		(float)srcmat.Get(3, 0), (float)srcmat.Get(3, 1), (float)srcmat.Get(3, 2), (float)srcmat.Get(3, 3)
	);
}


static int s_setrigidflag = 0;
static DWORD s_rigidflag = 0;



CModel::CModel() : m_camerafbx(), m_frustum(), m_undomotion()
{
	InitializeCriticalSection(&m_CritSection_Node);

	InitParams();
	s_alloccnt++;
	m_modelno = s_alloccnt;
}
CModel::~CModel()
{
	//DeleteCriticalSection(&m_CritSection_Node);//DestroyObjsのthread終了よりも後に移動


	DestroyObjs();
}
int CModel::InitParams()
{
	m_selectedboneno = 0;

	int index1;
	for (index1 = 0; index1 < REFPOSMAXNUM; index1++) {
		m_inview[index1] = false;
		m_befinview[index1] = false;
		m_inshadow[index1] = false;
	}
	m_inmorph = false;

	m_bound.Init();
	m_chkinview = nullptr;

	m_refposflag = false;
	m_updateslot = 0;

	m_csfirstdispatchflag = true;


	m_instancingnum = 0;
	m_instancingdrawnum = 0;
	ZeroMemory(m_instancingparams, sizeof(INSTANCINGPARAMS) * RIGMULTINDEXMAX);

	m_materialbank.InitParams();
	m_vroidjointname = false;

	m_updatefl4x4flag = false;

	m_Under_CalcEul = false;
	m_Under_CopyW2LW = false;
	m_Under_PostFKTra = false;
	m_Under_InitMP = false;
	m_Under_LoadFbx = false;
	//m_Under_PostIK = false;
	m_Under_Retarget = false;
	//m_Under_UpdateMatrix = false;
	m_Under_UpdateTimeline = false;
	m_Under_IKRot = false;
	m_Under_IKRotApplyFrame = false;
	m_Under_Motion2Bt = false;
	m_Under_Bt2Motion = false;


	m_loopstartflag = 0;

	m_vrmtexcount = 0;

	m_iktargetbonevec.clear();
	m_nodeonload = 0;
	m_node2mqoobj.clear();
	m_node2bone.clear();
	m_objno2digelem.clear();

	int groupindex;
	for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
		m_dispgroup[groupindex].clear();
		m_dispgroupON[groupindex] = true;
	}

	m_ikstopname.clear();

	InitCameraFbx();

	m_enulltime = 0.0;//SetShaderConstにおける　ボーンの無いメッシュアニメ enull evaluate用の時間


	m_materialdisprate.SetParams(1.0f, 1.0f, 1.0f, 1.0f);//diffuse, specular, emissive, ambient
	m_currentanimlayer = 0;

	m_fromBvh = false;
	m_fromNoBindPose = false;

	m_loadingmotionnum = 0;

	m_physikrec0.clear();
	m_physikrec.clear();
	m_phyikrectime = 0.0;

	m_bonelist.clear();
	m_bonename.clear();

	m_boneupdatematrix = 0;
	m_LoadFbxAnim = 0;
	m_PostIKThreads = 0;
	m_CalcEulThreads = 0;
	m_FKTraThreads = 0;
	m_CopyW2LWThreads = 0;
	m_RetargetThreads = 0;
	m_InitMpThreads = 0;
	m_creatednum_boneupdatematrix = 0;//スレッド数の変化に対応。作成済の数。処理用。
	m_creatednum_loadfbxanim = 0;//スレッド数の変化に対応。作成済の数。処理用。

	m_loadbonecount = 0;

	::ZeroMemory(m_fbxfullname, sizeof(WCHAR) * MAX_PATH);
	m_useegpfile = false;

	m_hasbindpose = 1;

	//ZeroMemory(m_armpparams, sizeof(FUNCMPPARAMS*) * 6);
	//ZeroMemory(m_arhthread, sizeof(HANDLE) * 6);

	m_physicsikcnt = 0;
	//ChaMatrixIdentity(&m_worldmat);
	m_modelposition.SetParams(0.0f, 0.0f, 0.0f);//読み込み時位置
	m_modelrotation.SetParams(0.0f, 0.0f, 0.0f);//読み込み時向き
	m_initaxismatx = 0;
	m_loadedflag = false;
	m_createbtflag = false;
	m_oldaxis_atloading = 0;
	m_ikrotaxis.SetParams( 1.0f, 0.0f, 0.0f );
	m_texpool = 0;
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

	::ZeroMemory( m_filename, sizeof( WCHAR ) * MAX_PATH );
	::ZeroMemory( m_dirname, sizeof( WCHAR ) * MAX_PATH );
	::ZeroMemory( m_modelfolder, sizeof( WCHAR ) * MAX_PATH );
	m_loadmult = 1.0f;

	ChaMatrixIdentity( &m_matWorld );
	ChaMatrixIdentity( &m_matVP );
	m_matView.SetIdentity();
	m_matProj.SetIdentity();

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
	m_btovec.clear();

	m_rigideleminfo.clear();
	m_impinfo.clear();


	//g_motionbrush_startframe = 0.0;
	//g_motionbrush_endframe = 0.0;
	//g_motionbrush_numframe = 1;
	//g_motionbrush_value = 0;

	//::ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);//SetShaderConst用
	::ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);//SetShaderConst用

	m_noboneflag = false;

	m_latertransparent.clear();

	InitUndoMotion( 0 );

	m_withStrJoint = 0;
	m_withoutStrJoint = 0;

	m_bindpose = nullptr;
	m_skyflag = false;
	m_groundflag = false;
	m_grassflag = false;

	m_secondCallOfMotion2Bt = false;

	return 0;
}

void CModel::InitCameraFbx()
{
	m_camerafbx.Clear();
	m_cameramotionid = -1;
}
int CModel::DestroyObjs()
{
	InitUndoMotion(0);

	//スレッドを先に止める
	WaitUpdateMatrixFinished();
	DestroyBoneUpdateMatrix();
	WaitLoadFbxAnimFinished();
	DestroyLoadFbxAnim();
	WaitCalcEulFinished();
	DestroyCalcEulThreads();
	WaitPostIKFinished();
	DestroyPostIKThreads();
	WaitFKTraFinished();
	DestroyFKTraThreads();
	WaitCopyW2LWFinished();
	DestroyCopyW2LWThreads();
	WaitRetargetFinished();
	DestroyRetargetThreads();
	WaitInitMpFinished();
	DestroyInitMpThreads();

	
	DeleteCriticalSection(&m_CritSection_Node);//スレッド終了よりも後



	DestroyMaterial();
	DestroyObject();
	DestroyAncObj();
	DestroyAllMotionInfo();

	DestroyFBXSDK();

	DestroyBtObject();


	DestroyNodeOnLoadReq(m_nodeonload);
	m_nodeonload = 0;


	//2023/08/24 ポインタではないメンバの後処理　必要
	//この後処理を忘れると　カメラアニメ(CAMERANODE)１つ当り240バイトメモリリーク
	m_camerafbx.DestroyObjs();

	if (m_chkinview) {
		delete m_chkinview;
		m_chkinview = nullptr;
	}


	InitParams();

	return 0;
}



int CModel::DestroyFBXSDK()
{
	
	//if( m_pimporter ){
	
	if(mAnimStackNameArray.Size() >= 1){
		FbxArrayDelete(mAnimStackNameArray);//2024/02/08 やっぱり必要だった。コメントアウトするとアプリ終了時にモーション名がメモリリーク
	}

	if (m_pimporter) {
		m_pimporter->Destroy();// インポータの削除
		m_pimporter = 0;
	}

	if( m_pscene ){
		m_pscene->Destroy();
		m_pscene = 0;
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
	m_motinfo.clear();

	m_curmotinfo = 0;

	return 0;
}

int CModel::DestroyMaterial()
{
	//m_materialbankのCMQOMaterial*は
	//非ポインタメンバ変数　m_materialbankのデストラクタでunique_ptrの機能で解放される 

	m_materialbank.DestroyObjs();

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
			//delete delbone;
			CBone::InvalidateBone(delbone);
		}
	}
	m_bonelist.clear();


	m_bonename.clear();

	m_withStrJoint = 0;
	m_withoutStrJoint = 0;

	map<CMQOObject*, FBXOBJ*>::iterator itrobjindex;
	for (itrobjindex = m_fbxobj.begin(); itrobjindex != m_fbxobj.end(); itrobjindex++) {
		FBXOBJ* curfbxobj = itrobjindex->second;
		if (curfbxobj) {
			free(curfbxobj);
		}
	}
	m_fbxobj.clear();


	m_topbone = 0;

	m_objectname.clear();
	m_bonename.clear();

	return 0;
}

int CModel::LoadMQO( ID3D12Device* pdev, const WCHAR* wfile, const WCHAR* modelfolder, float srcmult, int ismedia, int texpool )
{
	if( modelfolder ){
		wcscpy_s( m_modelfolder, MAX_PATH, modelfolder );
	}else{
		::ZeroMemory( m_modelfolder, sizeof( WCHAR ) * MAX_PATH );
	}
	m_loadmult = srcmult;
	m_pdev = pdev;
	m_texpool = texpool;

	WCHAR fullname[MAX_PATH];

	if( ismedia == 1 ){
		/*
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
		*/
		_ASSERT(0);
		wcscpy_s(fullname, MAX_PATH, g_basedir);
		wcscat_s(fullname, MAX_PATH, wfile);

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
		::ZeroMemory( m_dirname, sizeof( WCHAR ) * MAX_PATH );
		wcscpy_s( m_filename, MAX_PATH, fullname );
	}

	SetCurrentDirectory( m_dirname );


	DestroyMaterial();
	DestroyObject();


	SetNoBoneFlag(true);//2023/07/28


	CMQOFile mqofile;
	ChaVector3 vop( 0.0f, 0.0f, 0.0f );
	ChaVector3 vor( 0.0f, 0.0f, 0.0f );
	CallF( mqofile.LoadMQOFile( m_pdev, srcmult, m_filename, vop, vor, this ), return 1 );

	CallF( MakeObjectName(), return 1 );


	CallF( CreateMaterialTexture(), return 1 );

	//SetMaterialName();

	
	CallF(MakeDispObj(), return 1);

	MakeLaterMaterial();

	int chkret = CreateChkInView();
	_ASSERT(chkret == 0);

	return 0;
}

int CModel::LoadFBX(int skipdefref, ID3D12Device* pdev, const WCHAR* wfile, const WCHAR* modelfolder, 
	float srcmult, FbxManager* psdk, FbxImporter** ppimporter, FbxScene** ppscene, 
	int forcenewaxisflag, BOOL motioncachebatchflag)
{
	if (!pdev || !wfile || !modelfolder || !psdk) {
		_ASSERT(0);
		return 1;
	}

	//DestroyFBXSDK();

	m_psdk = psdk;
	s_psdk = psdk;
	*ppimporter = 0;
	*ppscene = 0;

	m_vrmtexcount = 0;


	if( modelfolder ){
		wcscpy_s( m_modelfolder, MAX_PATH, modelfolder );
	}else{
		::ZeroMemory( m_modelfolder, sizeof( WCHAR ) * MAX_PATH );
	}
	m_loadmult = srcmult;
	m_pdev = pdev;

	WCHAR fullname[MAX_PATH];
	wcscpy_s( m_fbxfullname, MAX_PATH, wfile );//save org fullpath
	wcscpy_s(fullname, MAX_PATH, wfile);

    WCHAR* strLastSlash = NULL;
    strLastSlash = wcsrchr( fullname, TEXT( '\\' ) );
    if( strLastSlash )
    {
		*strLastSlash = 0;
		wcscpy_s( m_dirname, MAX_PATH, fullname );
		wcscpy_s( m_filename, MAX_PATH, strLastSlash + 1 );
	}else{
		::ZeroMemory( m_dirname, sizeof( WCHAR ) * MAX_PATH );
		wcscpy_s( m_filename, MAX_PATH, fullname );
	}

	DestroyBoneUpdateMatrix();
	DestroyMaterial();
	DestroyObject();

	char utf8path[MAX_PATH] = {0};
    // Unicode 文字コードを第一引数で指定した文字コードに変換する
    ::WideCharToMultiByte( CP_UTF8, 0, wfile, -1, utf8path, MAX_PATH, NULL, NULL );	

	FbxScene* pScene = 0;
	FbxImporter* pImporter = 0;

	char scenename[256];
	sprintf_s(scenename, "scene_%05d", s_alloccnt);
    pScene = FbxScene::Create(m_psdk, scenename);
	*ppscene = pScene;//!!!!!!!!!
	m_pscene = pScene;//!!!!!!!!!


    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    bool lStatus;
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	char importername[256];
	sprintf_s(importername, "importer_%d", s_alloccnt);
	pImporter = FbxImporter::Create(m_psdk, importername);
	*ppimporter = pImporter;//!!!!!!!!!

    const bool lImportStatus = pImporter->Initialize(utf8path, -1, m_psdk->GetIOSettings());
	if (!lImportStatus) {
		_ASSERT(0);
		return -1;
	}

	//if (s_animevaluator) {
	//	s_animevaluator->Destroy();
	//	s_animevaluator = 0;
	//}
	//s_animevaluator = FbxAnimEvaluator::Create(s_psdk, "animevaluator1");


	//Initialize(FbxStream * pStream, void* pStreamData = NULL, const int pFileFormat = -1, FbxIOSettings * pIOSettings = NULL)


	////###############################################################
	//// load from buffer using FbxStream CustomStreamClass 2021/05/06
	////###############################################################
	//FbxIOSettings* ios = FbxIOSettings::Create(m_psdk, IOSROOT);
	//CustomStreamClass stream(m_psdk, "rb", utf8path);
	//// can pass in a void* data pointer to be passed to the stream on FileOpen
	//void* streamData = NULL;
	//// initialize the importer with a stream
	//if (!pImporter->Initialize(&stream, streamData, -1, ios)) {
	//	_ASSERT(0);
	//	return -1;
	//}

    pImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

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

		(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_EXT_SDK_GRP, true);
		(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_MODEL, true);
		(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_MODEL_COUNT, true);
		(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_POLYGROUP, true);


		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, false);
		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, true);
		//(*(m_psdk->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }



    // Import the scene.
    lStatus = pImporter->Import(pScene);
    if(lStatus == false )
    {
		_ASSERT( 0 );
		return 1;
	}

	FbxDocumentInfo* sceneinfo = pScene->GetSceneInfo();
	if (sceneinfo) {
		m_fbxcomment = sceneinfo->mComment;
		m_oldaxis_atloading = 0;
		if (forcenewaxisflag == 0) {
			FbxString oldauthor = "OpenRDB user";
			if (sceneinfo->mAuthor == oldauthor) {
				_ASSERT(0);
				FbxString oldrevision = "rev. 1.0";
				if (sceneinfo->mRevision == oldrevision) {
					m_oldaxis_atloading = 1;//!!!!!!!!!!!!!!!!!!!!
				}
			}
		}
		FbxString bvhmark = "BVH animation";
		if (sceneinfo->mKeywords == bvhmark) {
			SetFromBvhFlag(true);
		}
		else {
			SetFromBvhFlag(false);
		}

		FbxString nobindposemark = "skinmesh animation, start from no bindpose fbx";
		if (sceneinfo->mKeywords == nobindposemark) {
			SetFromNoBindPoseFlag(true);
		}
		else {
			SetFromNoBindPoseFlag(false);
		}
	}


//	CallF( InitFBXManager( &pSdkManager, &pImporter, &pScene, utf8path ), return 1 );


	m_bone2node.clear();
	FbxNode *pRootNode = pScene->GetRootNode();
	m_topbone = 0;


	CreateFBXBoneReq(pScene, pRootNode, 0);
	if ((int)m_bonelist.size() <= 0){
		//_ASSERT( 0 );
		if (!m_bonelist.empty()) {
			CBone* delbone = (CBone*)(m_bonelist.begin()->second);
			if (delbone) {
				CBone::InvalidateBone(delbone);
			}
		}
		m_bonelist.clear();
		m_topbone = 0;
		//_ASSERT(0);
	}
	m_vroidjointname = false;
	CheckVRoidJointNameReq(GetTopBone(false), &m_vroidjointname);


	InitCameraFbx();
	CreateFBXCameraReq(pRootNode);//CreateFBXBoneReqよりも後　cameranodeとcameraboneが対応付いてから呼ぶ



	//WCHAR* findmqo = wcsstr((WCHAR*)this->GetFileName(), L".mqo");
	//if (findmqo == 0) {//mqo以外の場合には　m_topbone必要
	size_t chklistsize = m_bonelist.size();
		if ((chklistsize == 0) || (GetBoneForMotionSize() == 0)) {//eNullしかない場合にも対応
			CBone* dummybone = CBone::GetNewBone(this);
			//_ASSERT( dummybone );
			if (dummybone) {
				//dummybone->SetName("RootNode_");
				dummybone->SetName("AddForEmptyTree");

				m_bonelist[dummybone->GetBoneNo()] = dummybone;
				m_bonename[dummybone->GetBoneName()] = dummybone;
				dummybone->SetType(FBXBONE_SKELETON);
				dummybone->SetTopBoneFlag(0);
				//m_bone2node[dummybone] = 0;
				//m_node2bone[curnode] = newbone;

				if (m_topbone) {
					m_topbone->AddChild(dummybone);
				}
				else {
					m_topbone = dummybone;
				}
				//m_bonelist[0] = dummybone;

				dummybone->LoadCapsuleShape(m_pdev);//!!!!!!!!!!
				//m_topbone = dummybone;
			}
			SetNoBoneFlag(true);
			//_ASSERT(0);

			CBone* testtopbone = GetTopBone();

		}
		else {
			SetNoBoneFlag(false);
		}
	//}


	//CreateExtendBoneReq(m_topbone);

_ASSERT(m_bonelist[0]);

	if (GetNoBoneFlag() == false) {
		CreateBoneUpdateMatrix();
		CreateCalcEulThreads();
		CreatePostIKThreads();
		CreateFKTraThreads();
		CreateCopyW2LWThreads();
		CreateRetargetThreads();
		CreateInitMpThreads();
	}


	ChaMatrix firstmeshmat;
	firstmeshmat.SetIdentity();
	CreateFBXMeshReq(pRootNode);


	//jointとmeshを読み込んだ後で
	m_nodeonload = new CNodeOnLoad(pRootNode);
	CreateNodeOnLoadReq(m_nodeonload);



	DbgOut(L"fbx bonenum %d\r\n", (int)m_bonelist.size());
	_ASSERT(m_bonelist[0]);



	if (motioncachebatchflag == FALSE) {
		CallF(CreateMaterialTexture(), return 1);
		CallF(MakeObjectName(), return 1);
	}

	
	int matrixindex = 0;
	SetBoneMatrixIndexReq(GetTopBone(false), &matrixindex);//2023/11/30

	size_t debugcount = 0;
	ChaMatrix offsetmat;
	ChaMatrixIdentity( &offsetmat );
	offsetmat.SetScale(ChaVector3(srcmult, srcmult, srcmult));
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			
			debugcount++;

			CallF( curobj->MultMat( offsetmat ), return 1 );
			CallF( curobj->MultVertex(), return 1; );

			if (motioncachebatchflag == FALSE) {
				//int clusternum = (int)curobj->GetClusterSize();//mqoobjectのclustersizeはCreateFbxSkinReqでセットされるので　まだ使えない

				CNodeOnLoad* nodeonload = FindNodeOnLoadByName(curobj->GetName());
				if (nodeonload) {
					FbxNode* fbxnode = nodeonload->GetNode();
					if (fbxnode) {
						FbxNodeAttribute* pAttrib = 0;
						bool hascluster = HasCluster(fbxnode, &pAttrib);
						if (hascluster == true) {
							//#####################
							//SkinMesh : PolyMesh4
							//#####################
							CallF(curobj->MakePolymesh4(m_pdev, this), return 1);
							GetFBXSkin(pAttrib, fbxnode);
						}
						else {
							//########################
							//NonSkinMesh : PolyMesh3
							//########################
							bool fbxfileflag = true;
							CallF(curobj->MakePolymesh3(fbxfileflag, m_pdev, this), return 1);
						}

						//##################################################################################################
						//Peakメモリ使用量が大きくなり過ぎないように　CQMOObjectごとにMakeDispObjして同時に不要メモリを削除
						//##################################################################################################
						int hasbone = 0;
						int clusternum = (int)curobj->GetClusterSize();
						if ((GetNoBoneFlag() == false) && (clusternum >= 1)) {
							hasbone = 1;
						}
						else {
							hasbone = 0;
						}

						//MakeDispObj内で　不要メモリ削除
						//PolyMesh4は マウスとの当たり判定をしない前提で不要メモリを削除
						//PolyMesh3は　fbxfileflagがtrue --> マウスとの当たり判定をしない前提で不要メモリを削除
						CallF(curobj->MakeDispObj(m_pdev, hasbone, GetGrassFlag()), return 1);

					}
				}
			}
		}
	}

	//if (GetNoBoneFlag() == false) {
	//	CreateFBXSkinReq(pRootNode);//clustersizeも決まる
	//}


	_ASSERT(m_bonelist[0]);

	//m_ktime0.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
//	m_ktime0.SetSecondDouble( 1.0 / 300.0 );
//	m_ktime0.SetSecondDouble( 1.0 / 30.0 );


	if (motioncachebatchflag == FALSE) {


		//メモリ使用量のpeak値を小さくするためには　MakePolyMesh*, MakeDispObjをCMQOObject毎に呼ぶ必要有　少し上のコードでそのように実行
		//if (GetNoBoneFlag() == false) {
		//	CallF(CreateMaterialTexture(pRenderContext), return 1);
		//	CallF(MakePolyMesh4(), return 1);
		//	CallF(MakeObjectName(), return 1);
		//	CreateFBXSkinReq(pRootNode);
		//}
		//else {
		//	//Boneが無い場合には　PolyMesh3
		//	CallF(CreateMaterialTexture(pRenderContext), return 1);
		//	bool fbxfileflag = true;
		//	CallF(MakePolyMesh3(fbxfileflag), return 1);
		//	CallF(MakeObjectName(), return 1);
		//}
		//SetMaterialName();

		//for dispgroup
		CreateObjno2DigElem();
		MakeDispGroupForRender();

		MakeLaterMaterial();//*.cha経由で読み込まれる場合は　ChaFile.cpp内でLaterTransparentタグ読み込み後に　MakeLaterMaterial()が改めて呼ばれる
		SetLODNum();

		int chkret = CreateChkInView();
		_ASSERT(chkret == 0);

		_ASSERT(m_bonelist[0]);

		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone && (curbone->IsSkeleton() || curbone->IsCamera() || curbone->IsNull())) {
				curbone->SetBtKinFlag(1);
			}
		}


		map<int, CMQOObject*>::iterator itr2;
		for (itr2 = m_object.begin(); itr2 != m_object.end(); itr2++) {
			CMQOObject* curobj = itr2->second;
			if (curobj) {
				char* findnd = strstr((char*)curobj->GetName(), "_ND");
				if (findnd) {
					curobj->SetDispFlag(0);
				}
			}
		}



		if (GetVRoidJointName()) {
			//2024/04/22 ボーンの軸としてBONEAXIS_BINDPOSEを指定することにより
			//オイラーグラフの軸とIK回転の軸とボーンの軸が一致する
			//VRoidの場合にはバインドポーズとしてボーン軸が設定されているのでBONEAXIS_BINDPOSEの方が使いやすい
			//(Rigの軸指定の種類のNODE*とも一致するようになる)
			g_boneaxis = BONEAXIS_BINDPOSE;
		}
		else {
			//2024/04/22 ボーン軸としてBONEAXIS_CURRENTを指定すると
			//RokDeBone2のようにバインドポーズとしてグローバル軸が設定されている場合に
			//IK操作をボーン軸で行うことが可能
			g_boneaxis = BONEAXIS_CURRENT;
		}



		m_rigideleminfo.clear();
		m_impinfo.clear();

		if (skipdefref == 0) {
			REINFO reinfo;
			::ZeroMemory(&reinfo, sizeof(REINFO));
			strcpy_s(reinfo.filename, MAX_PATH, m_defaultrename);
			reinfo.btgscale = 9.07f;
			m_rigideleminfo.push_back(reinfo);
			m_impinfo.push_back(m_defaultimpname);

			//if (GetTopBone()) {
			if (GetNoBoneFlag() == false) {
				CreateRigidElemReq(GetTopBone(false), 1, m_defaultrename, 1, m_defaultimpname);
				SetCurrentRigidElem(0);
			}

			m_curreindex = 0;
			m_curimpindex = 0;
		}
	}

	//*ppimporter = pImporter;
	//*ppscene = pScene;

	////m_pimporter = pImporter;
	//m_pscene = pScene;

	if (s_animevaluator) {
		s_animevaluator->Destroy();
		s_animevaluator = 0;
	}
	pImporter->Destroy();


	//CallF(MakeDispObj(), return 1);//MakePlymesh直後にCMQOObjectごとに呼ぶように変更

	return 0;
}

int CModel::LoadFBXAnim( FbxManager* psdk, FbxScene* pscene, int (*tlfunc)( int srcmotid ), BOOL motioncachebatchflag)
{
	if( !psdk || !pscene ){
		_ASSERT( 0 );
		return 0;
	}

	//モーフアニメがあるかもしれないのでリターンしない
	//if(!GetTopBone() || GetNoBoneFlag()){
	//	return 0;
	//}


	this->m_tlFunc = tlfunc;//未使用


	FbxNode *pRootNode = pscene->GetRootNode();
	CallF( CreateFBXAnim( pscene, pRootNode, motioncachebatchflag ), return 1 );



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

	int materialnum = m_materialbank.GetSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* curmaterial = m_materialbank.Get(matindex);
		if (curmaterial) {
			CallF(curmaterial->CreateTexture(m_dirname, m_texpool), return 1);
		}
		else {
			_ASSERT(0);
		}
	}


	//map<int,CMQOObject*>::iterator itrobj;
	//for( itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++ ){
	//	CMQOObject* curobj = itrobj->second;
	//	if( curobj ){
	//		map<int,CMQOMaterial*>::iterator itrmat;
	//		for( itrmat = curobj->GetMaterialBegin(); itrmat != curobj->GetMaterialEnd(); itrmat++ ){
	//			CMQOMaterial* curmat = itrmat->second;
	//			if (curmat) {
	//				CallF(curmat->CreateTexture(m_dirname, m_texpool), return 1);
	//			}
	//			else {
	//				_ASSERT(0);
	//			}
	//		}

	//		if (curobj->GetExtLine()) {
	//			//ExtLineの　m_rootsignatureとDescriptorHeap用　のマテリアル初期化
	//			if (curobj->GetExtLine()->GetMaterial()) {
	//				curobj->GetExtLine()->GetMaterial()->CreateTexture(m_dirname, m_texpool);
	//			}
	//		}
	//	}
	//}


	return 0;
}

//CMQOMaterial* CModel::GetMQOMaterialByName(std::string srcname)
//{
//	CMQOMaterial* retmat = 0;
//
//	map<int, CMQOObject*>::iterator itrobj;
//	for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
//		CMQOObject* curobj = itrobj->second;
//		if (curobj) {
//			map<int, CMQOMaterial*>::iterator itrmat;
//			for (itrmat = curobj->GetMaterialBegin(); itrmat != curobj->GetMaterialEnd(); itrmat++) {
//				CMQOMaterial* curmat = itrmat->second;
//				if (strcmp(curmat->GetName(), srcname.c_str()) == 0) {
//					retmat = curmat;
//					break;
//				}
//			}
//		}
//	}
//
//	return retmat;
//
//}




//int CModel::OnRender(bool withalpha, 
//	RenderContext* pRenderContext, int lightflag, ChaVector4 diffusemult, int btflag, bool calcslotflag )
//{
//
//	if (GetInView() == false) {
//		return 0;
//	}
//
//
//	ChaVector4 materialdisprate = GetMaterialDispRate();
//
//	//map<int,CMQOObject*>::iterator itr;
//	//for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
//	//	CMQOObject* curobj = itr->second;
//	int groupindex;
//	for(groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++){
//
//		if ((m_dispgroupON[groupindex] == true) && !(m_dispgroup[groupindex].empty())) {
//
//			int elemnum = (int)m_dispgroup[groupindex].size();
//			int elemno;
//			for (elemno = 0; elemno < elemnum; elemno++) {
//
//				CMQOObject* curobj = (m_dispgroup[groupindex])[elemno].mqoobject;
//
//				if (curobj && curobj->GetVisible()) {
//
//					//if (curobj->GetDispLine()) {
//					//	int dbgflag1 = 1;
//					//}
//
//
//					bool found_noalpha = false;
//					bool found_alpha = false;
//					int result = curobj->IncludeTransparent(diffusemult.w, &found_noalpha, &found_alpha);//2023/09/24
//					if (result == 1) {
//						_ASSERT(0);
//						return 1;
//					}
//					else if (result == 2) {
//						continue;
//					}
//
//					if ((withalpha == false) && (found_noalpha == false)) {
//						//不透明描画時　１つも不透明がなければ　レンダースキップ
//						continue;
//					}
//					if ((withalpha == true) && (found_alpha == false)) {
//						//半透明描画時　１つも半透明がなければ　レンダースキップ
//						continue;
//					}
//
//					//if (curobj->GetDispObj()) {
//					//	if (curobj->GetPm3()) {
//					//		CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
//					//		CallF(curobj->GetDispObj()->RenderNormalPM3(withalpha, pRenderContext, lightflag, diffusemult, materialdisprate, curobj), return 1);
//					//	}
//					//	else if (curobj->GetPm4()) {
//					//		CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
//					//		CallF(curobj->GetDispObj()->RenderNormal(withalpha, pRenderContext, lightflag, diffusemult, materialdisprate, curobj), return 1);
//					//	}
//					//	else {
//					//		_ASSERT(0);
//					//	}
//					//}
//					//else if (curobj->GetDispLine()) {
//					//	CallF(curobj->GetDispLine()->RenderLine(withalpha, pRenderContext, diffusemult, materialdisprate), return 1);
//					//}
//				}
//			}
//		}
//	}
//
//	return 0;
//}


int CModel::SelectRenderObject(int srcobjno, std::vector<CMQOObject*>& selectedobjvec)
{
	selectedobjvec.clear();

	map<int, DISPGROUPELEM>::iterator itrdigelem;
	itrdigelem = m_objno2digelem.find(srcobjno);
	if (itrdigelem == m_objno2digelem.end()) {
		return 0;
	}

	DISPGROUPELEM digelem = itrdigelem->second;

	SelectRenderObjectReq(digelem.pNode, selectedobjvec);

	return 0;
}
void CModel::SelectRenderObjectReq(FbxNode* pNode, std::vector<CMQOObject*>& selectedobjvec)
{
	if (pNode) {

		map<FbxNode*, CMQOObject*>::iterator itrobj;
		itrobj = m_node2mqoobj.find(pNode);
		if (itrobj != m_node2mqoobj.end()) {
			CMQOObject* mqoobject = itrobj->second;
			if (mqoobject) {
				selectedobjvec.push_back(mqoobject);
			}
		}

		int childNodeNum;
		childNodeNum = pNode->GetChildCount();
		for (int i = 0; i < childNodeNum; i++)
		{
			FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
			if (pChild) {
				SelectRenderObjectReq(pChild, selectedobjvec);
			}
		}
	}
}

int CModel::GetSelectedObjTree(int srcobjno, std::vector<int>& selectedobjtree)
{
	selectedobjtree.clear();

	map<int, DISPGROUPELEM>::iterator itrdigelem;
	itrdigelem = m_objno2digelem.find(srcobjno);
	if (itrdigelem == m_objno2digelem.end()) {
		return 0;
	}

	DISPGROUPELEM digelem = itrdigelem->second;

	GetSelectedObjTreeReq(digelem.pNode, selectedobjtree);

	return 0;
}
void CModel::GetSelectedObjTreeReq(FbxNode* pNode, std::vector<int>& selectedobjtree)
{
	if (pNode) {

		int selectedobjno = -1;
		map<int, DISPGROUPELEM>::iterator itrdigelem;
		for (itrdigelem = m_objno2digelem.begin(); itrdigelem != m_objno2digelem.end(); itrdigelem++) {
			DISPGROUPELEM digelem = itrdigelem->second;
			if (digelem.pNode == pNode) {
				selectedobjno = digelem.objno;
				break;
			}
		}

		if (selectedobjno >= 0) {
			selectedobjtree.push_back(selectedobjno);
		}


		int childNodeNum;
		childNodeNum = pNode->GetChildCount();
		for (int i = 0; i < childNodeNum; i++)
		{
			FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
			if (pChild) {
				GetSelectedObjTreeReq(pChild, selectedobjtree);
			}
		}
	}
}




int CModel::RenderTest(bool withalpha, ChaScene* srcchascene, int lightflag, ChaVector4 diffusemult, int srcobjno)
{
	if (!srcchascene) {
		_ASSERT(0);
		return 1;
	}

	if (GetInView(0) == false) {
		return 0;
	}

	int btflag = 0;

	ChaVector4 materialdisprate = GetMaterialDispRate();

	vector<CMQOObject*> selectedobjvec;
	SelectRenderObject(srcobjno, selectedobjvec);

	//map<int, FbxNode*>::iterator itrnode;
	//itrnode = m_objno2node.find(srcobjno);
	//if (itrnode == m_objno2node.end()) {
	//	return 0;
	//}
	//map<FbxNode*, CMQOObject*>::iterator itrobj;
	//itrobj = m_node2mqoobj.find(itrnode->second);
	//if (itrobj == m_node2mqoobj.end()) {
	//	return 0;
	//}

	int selectednum = (int)selectedobjvec.size();
	int selectedno;
	for (selectedno = 0; selectedno < selectednum; selectedno++) {

		CMQOObject* curobj = selectedobjvec[selectedno];

		if (curobj && curobj->GetVisible(0)) {

			bool found_noalpha = false;
			bool found_alpha = false;
			int result = curobj->IncludeTransparent(diffusemult.w, &found_noalpha, &found_alpha);//2023/09/24
			if (result == 1) {
				_ASSERT(0);
				return 1;
			}
			else if (result == 2) {
				continue;
			}

			if ((withalpha == false) && (found_noalpha == false)) {
				//不透明描画時　１つも不透明がなければ　レンダースキップ
				continue;
			}
			if ((withalpha == true) && (found_alpha == false)) {
				//半透明描画時　１つも半透明がなければ　レンダースキップ
				continue;
			}

			bool forcewithalpha = false;
			bool calcslotflag = false;
			//bool calcslotflag = true;
			bool zcmpalways = false;
			bool zenable = true;
			int refposindex = 0;

			myRenderer::RENDEROBJ renderobj;
			renderobj.Init();
			renderobj.pmodel = this;
			renderobj.mqoobj = curobj;
			renderobj.shadertype = MQOSHADER_TOON;//!!!!!!!!!!! マニピュレータと地面はNOLIGHTで表示
			renderobj.withalpha = withalpha;
			renderobj.forcewithalpha = forcewithalpha;
			renderobj.lightflag = lightflag;
			renderobj.diffusemult = diffusemult;
			renderobj.materialdisprate = materialdisprate;
			renderobj.mWorld = GetWorldMat().TKMatrix();
			renderobj.calcslotflag = calcslotflag;
			renderobj.btflag = btflag;
			renderobj.zcmpalways = zcmpalways;
			renderobj.zenable = zenable;//2024/02/08
			renderobj.renderkind = RENDERKIND_NORMAL;
			renderobj.refposindex = refposindex;//2024/02/06

			vector<myRenderer::RENDEROBJ> rendervec;
			rendervec.push_back(renderobj);

			srcchascene->Add3DModelToForwardRenderPass(rendervec);
		}
	}

	return 0;

}





int CModel::GetModelBound( MODELBOUND* dstb )
{
	MODELBOUND mb;
	MODELBOUND addmb;
	//::ZeroMemory(&mb, sizeof(MODELBOUND));
	mb.Init();
	addmb.Init();

	int calcflag = 0;
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if (curobj && !curobj->IsND()) {
			if (curobj->GetPm3()) {
				//curobj->GetPm3()->CalcBound();//MakeDispObj()に移動　ここでは既にpointbufなどが削除された後
				if (calcflag == 0) {
					mb = curobj->GetPm3()->GetBound();
				}
				else {
					addmb = curobj->GetPm3()->GetBound();
					AddModelBound(&mb, &addmb);
				}
				calcflag++;
			}
			if (curobj->GetPm4()) {
				//curobj->GetPm4()->CalcBound();//MakeDispObj()に移動　ここでは既にpointbufなどが削除された後
				if (calcflag == 0) {
					mb = curobj->GetPm4()->GetBound();
				}
				else {
					addmb = curobj->GetPm4()->GetBound();
					AddModelBound(&mb, &addmb);
				}
				calcflag++;
			}
			if (curobj->GetExtLine()) {
				//curobj->GetExtLine()->CalcBound();//MakeDispObj()に移動　ここでは既にpointbufなどが削除された後
				if (calcflag == 0) {
					mb = curobj->GetExtLine()->GetBound();
				}
				else {
					addmb = curobj->GetExtLine()->GetBound();
					AddModelBound(&mb, &addmb);
				}
				calcflag++;
			}
		}
	}

	if (GetTopBone() && (calcflag == 0)) {//メッシュが無いときだけボーンのバウンダリを使用
		if (calcflag == 0) {
			mb = CalcBoneBound();
		}
		else {
			addmb = CalcBoneBound();
			AddModelBound(&mb, &addmb);
		}
		calcflag++;
	}


	*dstb = mb;
	m_bound = mb;

	return 0;
}

MODELBOUND CModel::CalcBoneBound()
{
	MODELBOUND mb;
	//::ZeroMemory(&mb, sizeof(MODELBOUND));
	mb.Init();

	//ChaVector3 min.SetParams(0.0f, 0.0f, 0.0f);
	//ChaVector3 max.SetParams(0.0f, 0.0f, 0.0f);
	ChaVector3 min;
	min.SetParams(FLT_MAX, FLT_MAX, FLT_MAX);
	ChaVector3 max;
	max.SetParams(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	ChaVector3 center;
	center.SetParams(0.0f, 0.0f, 0.0f);
	float r = 0.0f;

	bool firstflag = true;

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {

			//ボーンの初期位置は全て０の場合があるので、０フレームのアニメ付きのボーン位置を計算する
			ChaVector3 fpos0 = curbone->GetJointFPos();
			ChaVector3 fpos = fpos0;
			if (ExistCurrentMotion()) {
				ChaMatrix curwmat;
				CMotionPoint* curmp = curbone->GetMotionPoint(GetCurrentMotID(), 0.0);
				if (curmp) {
					curwmat = curmp->GetWorldMat();
				}
				else {
					curwmat.SetIdentity();
				}
				ChaVector3TransformCoord(&fpos, &fpos0, &curwmat);
			}


			if (firstflag == true) {
				min = fpos;
				max = fpos;

				firstflag = false;
			}
			else {
				if (fpos.x < min.x) {
					min.x = fpos.x;
				}
				if (fpos.y < min.y) {
					min.y = fpos.y;
				}
				if (fpos.z < min.z) {
					min.z = fpos.z;
				}

				if (fpos.x > max.x) {
					max.x = fpos.x;
				}
				if (fpos.y > max.y) {
					max.y = fpos.y;
				}
				if (fpos.z > max.z) {
					max.z = fpos.z;
				}
			}
		}
	}

	center = (min + max) * 0.5f;
	ChaVector3 diff = center - min;
	r = (float)ChaVector3LengthDbl(&diff);

	mb.min = min;
	mb.max = max;
	mb.center = center;
	mb.r = r;

	if (!firstflag) {
		mb.SetIsValid(true);
	}

	return mb;
}

int CModel::AddModelBound( MODELBOUND* mb, MODELBOUND* addmb )
{
	if (!mb || !addmb) {
		_ASSERT(0);
		return 1;
	}

	if (addmb->IsValid() == false) {
		return 0;
	}

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
	mb->r = (float)ChaVector3LengthDbl( &diff );

	mb->SetIsValid(true);

	return 0;
}



int CModel::SetShapeNoReq( CMQOFace** ppface, int facenum, int searchp, int shapeno, int* setfacenum )
{

	int fno;
	CMQOFace* findface[200];
	::ZeroMemory( findface, sizeof( CMQOFace* ) * 200 );
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
			if (nameptr) {
				*(nameptr + 256 - 1) = 0;
				int sdefcmp, bdefcmp;
				sdefcmp = strncmp(nameptr, "sdef:", 5);
				bdefcmp = strncmp(nameptr, "bdef:", 5);
				if ((sdefcmp != 0) && (bdefcmp != 0)) {
					size_t leng = strlen(nameptr);
					if ((leng > 0) && (leng < 4098)) {
						string firstname(nameptr, nameptr + leng);
						m_objectname[firstname] = curobj;
						curobj->SetDispName(firstname);
					}
					else {
						_ASSERT(0);
						curobj->SetDispName("MakeObjecctName_Error");
					}
				}
				else {
					char* startptr = nameptr + 5;
					size_t leng = strlen(startptr);
					if ((leng > 0) && (leng < 4098)) {
						string firstname(startptr, startptr + leng);
						m_objectname[firstname] = curobj;
						curobj->SetDispName(firstname);
					}
					else {
						_ASSERT(0);
						curobj->SetDispName("MakeObjecctName_Error");
					}
				}
			}
		}
	}

	return 0;
}

int CModel::DbgDump()
{

	//2023/06/24
	//ジョイントが５１２個以上あるファイルで　スタックオーバーフローするので　呼び出し停止中

	//DbgOut( L"######start DbgDump %s\r\n", GetFileName() );
	//DbgOut( L"Dump Bone And InfScope\r\n" );
	//if( m_topbone ){
	//	DbgDumpBoneReq(0, m_topbone, 0);
	//}
	//DbgOut( L"######end DbgDump\r\n" );

	return 0;
}

int CModel::DbgDumpBoneReq(int level, CBone* boneptr, int broflag)
{

	//2023/06/24
	//ジョイントが５１２個以上あるファイルで　スタックオーバーフローするので　呼び出し停止中


	//サイズが大きいと　複雑なファイル読み込み時に　スタックオーバーフローする
	char mes[512] = { 0 };
	char strtab[30] = { 0 };
	WCHAR wmes[512] = { 0L };
	WCHAR wstrtab[30] = { 0L };


	ZeroMemory(mes, sizeof(char) * 512);
	ZeroMemory(strtab, sizeof(char) * 30);
	ZeroMemory(wmes, sizeof(WCHAR) * 512);
	ZeroMemory(wstrtab, sizeof(WCHAR) * 30);
	sprintf_s(strtab, 30, "\t");
	swprintf_s(wstrtab, 30, L"\t");
	int tabmax = min(level, (30 - 1));
	int tabno;
	for (tabno = 0; tabno < tabmax; tabno++) {
		strcat_s(strtab, 30, "\t");
		wcscat_s(wstrtab, 30, L"\t");
	}


	if( boneptr->GetParent(false) ){
		sprintf_s( mes, 512, "%sboneno %d, bonename %s - parent %s\r\n", 
			strtab, boneptr->GetBoneNo(), boneptr->GetBoneName(), boneptr->GetParent(false)->GetBoneName() );
	}else{
		sprintf_s( mes, 512, "%sboneno %d, bonename %s - parent NONE\r\n",
			strtab, boneptr->GetBoneNo(), boneptr->GetBoneName() );
	}
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, mes, 512, wmes, 512 );
	DbgOut( wmes );

	DbgOut( L"%sbonepos (%.3f, %.3f, %.3f)\r\n", 
		wstrtab,
		//boneptr->GetJointWPos().x, boneptr->GetJointWPos().y, boneptr->GetJointWPos().z,
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
	if( boneptr->GetChild(false) ){
		int nextlevel = level + 1;
		DbgDumpBoneReq(nextlevel, boneptr->GetChild(false), 1);
	}
	if( (broflag == 1) && boneptr->GetBrother(false) ){
		DbgDumpBoneReq(level, boneptr->GetBrother(false), 1);
	}



	return 0;
}

int CModel::MakePolyMesh3(bool fbxfileflag)
{
	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){
			CallF(curobj->MakePolymesh3(fbxfileflag, m_pdev, this), return 1);
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
			CallF( curobj->MakePolymesh4(m_pdev, this), return 1 );
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
			CallF( curobj->MakeExtLine(this), return 1 );
		}
	}

	return 0;
}

int CModel::MakeDispObj()
{
	int hasbone;
	//if( m_bonelist.empty() ){
	//	hasbone = 0;
	//}else{
	//	hasbone = 1;
	//}

	//if (GetNoBoneFlag() == false) {
	//	hasbone = 1;
	//}
	//else {
	//	hasbone = 0;
	//}

	map<int,CMQOObject*>::iterator itr;
	for( itr = m_object.begin(); itr != m_object.end(); itr++ ){
		CMQOObject* curobj = itr->second;
		if( curobj ){

			int clusternum = (int)curobj->GetClusterSize();
			if ((GetNoBoneFlag() == false) && (clusternum >= 1)) {
				hasbone = 1;
			}
			else {
				hasbone = 0;
			}

			CallF( curobj->MakeDispObj(m_pdev, hasbone, GetGrassFlag()), return 1 );
		}
	}

	return 0;
}

int CModel::Motion2Bt(bool limitdegflag, double nextframe, ChaMatrix* pmView, ChaMatrix* pmProj)//, int updateslot)
{
	if (!pmView || !pmProj) {
		_ASSERT(0);
		return 1;
	}

	ChaCalcFunc chacalcfunc;
	chacalcfunc.Motion2Bt(this, limitdegflag, nextframe, pmView, pmProj);//, updateslot);

	return 0;
}

void CModel::Motion2BtReq(CBtObject* srcbto)
{
	//if( srcbto->GetBone() && (srcbto->GetBone()->GetBtKinFlag() == 1) ){
	if (srcbto->GetBone() && ExistCurrentMotion()){
		srcbto->Motion2Bt(this, GetCurrentMotID(), GetCurrentFrame());
	}

	int chilnum = srcbto->GetChildBtSize();
	int chilno;
	for( chilno = 0; chilno < chilnum; chilno++ ){
		Motion2BtReq( srcbto->GetChildBt( chilno ) );
	}
}

//void CModel::CalcLimitedEulAfterThreadReq(CBone* srcbone, int srcmotid, double srcframe)
//{
//	//スレッド後処理用
//	//CalcWorldMatAfterThreadReqで計算した　limitedwmを元に
//	//limitedeulを計算する
//
//	if (srcbone) {
//		ChaVector3 limitedeul.SetParams(0.0f, 0.0f, 0.0f);
//		limitedeul = srcbone->CalcLocalLimitedEulXYZ(srcmotid, srcframe);
//		srcbone->SetLimitedLocalEul(srcmotid, srcframe, limitedeul);
//
//		if (srcbone->GetBrother()) {
//			CalcLimitedEulAfterThreadReq(srcbone->GetBrother(), srcmotid, srcframe);
//		}
//		if (srcbone->GetChild()) {
//			CalcLimitedEulAfterThreadReq(srcbone->GetChild(), srcmotid, srcframe);
//		}
//	}
//}



//void CModel::CalcWorldMatAfterThreadReq(CBone* srcbone, int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//	if (srcbone) {
//
//
//		srcbone->CalcWorldMatAfterThread(srcmotid, srcframe, wmat, vpmat);
//
//
//		if (srcbone->GetBrother()) {
//			CalcWorldMatAfterThreadReq(srcbone->GetBrother(), srcmotid, srcframe, wmat, vpmat);
//		}
//		if (srcbone->GetChild()) {
//			CalcWorldMatAfterThreadReq(srcbone->GetChild(), srcmotid, srcframe, wmat, vpmat);
//		}
//	}
//}

//int CModel::SwapCurrentMotionPoint()
//{
//	map<int, CBone*>::iterator itrbone;
//	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
//		CBone* curbone = itrbone->second;
//		if (curbone && (curbone->IsSkeleton())) {
//			curbone->SwapCurrentMotionPoint();
//		}
//	}
//	return 0;
//}

void CModel::SetUpdateSlotReq(CBone* srcbone, int srcslot)
{
	SetUpdateSlot(srcslot);

	if (srcbone) {

		srcbone->SetUpdateSlot(srcslot);

		if (srcbone->GetChild(false)) {
			SetUpdateSlotReq(srcbone->GetChild(false), srcslot);
		}
		if (srcbone->GetBrother(false)) {
			SetUpdateSlotReq(srcbone->GetBrother(false), srcslot);
		}
	}
}


int CModel::UpdateMatrix(bool limitdegflag, 
	ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat, 
	bool needwaitfinished, int refposindex)//, int updateslot)
// default : needwaitfinished = false, updateslot = 0
{
	m_matWorld = *wmat;
	m_matView = *vmat;
	m_matProj = *pmat;
	m_matVP = m_matView * m_matProj;

	
	ChkInView(refposindex);//2023/08/25 //2024/03/24
	if (GetInView(refposindex) == false) {
		return 0;
	}


	if(!ExistCurrentMotion()){
		return 0;//!!!!!!!!!!!!
	}


	//morphアニメがあるかもしれないので、ボーンが無くてもリターンしない
	//if (GetBoneForMotionSize() <= 0) {
	//	return 0;
	//}


	//if (m_topbone) {
	//	m_topbone->InitAddLimitQAll();
	//}

	int curmotid = GetCurrentMotID();
	double curframe = GetCurrentFrame();


	//if (g_previewFlag != 0) {
	//	WCHAR dbgline[1024] = { 0 };
	//	swprintf_s(dbgline, 1024, L"UpdateMatrix ### curmotid %d, curframe %.3f\n",
	//		curmotid, curframe);
	//	OutputDebugString(dbgline);
	//}


	if (GetNoBoneFlag() == false) {
		//if ((m_boneupdatematrix != NULL) && (m_bonelist.size() >= (m_creatednum_boneupdatematrix * 4))) {
		if ((m_boneupdatematrix != NULL) && (m_bonelist.size() >= (m_creatednum_boneupdatematrix * 2))) {

			//WaitUpdateMatrixFinished();//needwaitfinishedがfalseのときにも必要
			//SwapCurrentMotionPoint();//<--- この方式は角度制限を有効にしたときに顕著にぎくしゃくするのでやめた


			//2023/02/02
			//limitdegflag == true時にも　UpdateMatrixは計算済を補間するだけなので　再帰呼び出ししなくてOKになった
			//limitdegflag == true時にも　UpdateMatrixでオイラー角もセットされるようになった(スレッド後処理は不要)
			int updatecount;
			for (updatecount = 0; updatecount < m_creatednum_boneupdatematrix; updatecount++) {
				CThreadingUpdateMatrix* curupdate = m_boneupdatematrix + updatecount;
				curupdate->UpdateMatrix(limitdegflag, curmotid, curframe, wmat, vmat, pmat, refposindex);// , updateslot);
			}

			if (needwaitfinished) {
				WaitUpdateMatrixFinished();
			}


			//if (g_limitdegflag == true) {
			//	CalcWorldMatAfterThreadReq(m_topbone, curmotid, curframe, wmat, vpmat);//curframe : 時間補間有り
			//	//CalcLimitedEulAfterThreadReq(m_topbone, curmotid, curframe);
			//}
		}
		else {
			//map<int, CBone*>::iterator itrbone;
			//for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
			//	CBone* curbone = itrbone->second;
			//	if( curbone ){
			//		curbone->UpdateMatrix( curmotid, curframe, wmat, vpmat );
			//	}
			//}
			UpdateMatrixReq(limitdegflag, GetTopBone(false), curmotid, curframe, wmat, vmat, pmat, refposindex);
		}
	}


	if (GetInMorph() && (IsCameraMotion(curmotid) == false)) {//2024/06/07 カメラモーションにはモーフ情報は無い前提

		//2024/05/22 BlendShapeプレートメニューのblendshapeDistスライダーによる指定距離内の場合にモーフ再生
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			if (!(curobj->EmptyShape())) {
				CallF(curobj->UpdateMorphWeight(curmotid, IntTime(curframe)), return 1);
				CallF(curobj->UpdateMorphBuffer(), return 1);
			}
		}
	}



	//if (m_topbone) {
	//	m_topbone->RotQAddLimitQAll(curmotid, curframe);
	//}


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
		if( !(curobj->EmptyShape()) ){
			GetShapeWeight( m_fbxobj[curobj].node, m_fbxobj[curobj].mesh, lTime, mCurrentAnimLayer, curobj );

			CallF( curobj->UpdateMorphBuffer(), return 1 );
		}
	}
	*/

	return 0;
}

void CModel::UpdateMatrixReq(bool limitdegflag, CBone* srcbone, int srcmotid, double srcframe, 
	ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat,
	int refposindex)
{
	if (srcbone) {

		if (srcbone->IsSkeleton()) {
			bool calledbythread = false;
			srcbone->UpdateMatrix(limitdegflag, srcmotid, srcframe, wmat, vmat, pmat, calledbythread, refposindex);
		}
		
		if (srcbone->GetChild(false)) {
			UpdateMatrixReq(limitdegflag, srcbone->GetChild(false), srcmotid, srcframe, wmat, vmat, pmat, refposindex);
		}
		if (srcbone->GetBrother(false)) {
			UpdateMatrixReq(limitdegflag, srcbone->GetBrother(false), srcmotid, srcframe, wmat, vmat, pmat, refposindex);
		}
	}
}

int CModel::CopyLimitedWorldToWorldOne(CBone* srcbone, int srcmotid, double srcframe)
{
	if (srcbone && srcbone->IsSkeleton()) {
		srcbone->CopyLimitedWorldToWorld(srcmotid, srcframe);
	}
	return 0;
}

void CModel::CopyLimitedWorldToWorldReq(CBone* srcbone, int srcmotid, double srcframe)
{
	if (srcbone) {
		if (srcbone->IsSkeleton()) {
			srcbone->CopyLimitedWorldToWorld(srcmotid, srcframe);
		}

		if (srcbone->GetChild(false)) {
			CopyLimitedWorldToWorldReq(srcbone->GetChild(false), srcmotid, srcframe);
		}
		if (srcbone->GetBrother(false)) {
			CopyLimitedWorldToWorldReq(srcbone->GetBrother(false), srcmotid, srcframe);
		}
	}
}


void CModel::CopyWorldToLimitedWorldReq(CBone* srcbone, int srcmotid, double srcframe)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.CopyWorldToLimitedWorldReq(this, srcbone, srcmotid, srcframe);
}


void CModel::ApplyNewLimitsToWMReq(CBone* srcbone, int srcmotid, double srcframe, ChaMatrix befeditparentmat)
{

	//2023/02/03
	//この関数を実行する前に　CopyWorldToLimitedWorldを実行しておく

	//###########################################################################################################
	//2023/10/25
	// SetWorldMat()内にて　directset == falseの部分で
	//角度制限により動かさない場合にbefeul.befframeeulを使うので　ApplyNewLimitsToWM()はマルチスレッド化出来ない
	//###########################################################################################################

	double roundingframe = RoundingTime(srcframe);

	if (srcbone) {
		ChaMatrix befeditcurrentwm;
		befeditcurrentwm.SetIdentity();

		if (srcbone->IsSkeleton()) {
			bool limitdegflag = true;//!!!
			befeditcurrentwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

			srcbone->ApplyNewLimitsToWM(srcmotid, roundingframe, befeditparentmat);
		}

		if (srcbone->GetChild(false)) {
			ApplyNewLimitsToWMReq(srcbone->GetChild(false), srcmotid, roundingframe, befeditcurrentwm);
		}
		if (srcbone->GetBrother(false)) {
			ApplyNewLimitsToWMReq(srcbone->GetBrother(false), srcmotid, roundingframe, befeditparentmat);
		}
	}
}



int CModel::ClearLimitedWM(int srcmotid, double srcframe)
{
	if (!m_curmotinfo) {
		return 0;//!!!!!!!!!!!!
	}

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {
			curbone->ClearLimitedWorldMat(srcmotid, srcframe);
		}
	}

	return 0;
}


int CModel::HierarchyRouteUpdateMatrix(bool limitdegflag, CBone* srcbone, 
	ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat,
	int refposindex)
{
	m_matWorld = *wmat;
	m_matView = *vmat;
	m_matProj = *pmat;
	m_matVP = m_matView * m_matProj;

	if (!ExistCurrentMotion()) {
		return 0;//!!!!!!!!!!!!
	}

	if (!srcbone) {
		return 0;
	}

	std::vector<CBone*> vecroute;
	vecroute.clear();
	
	CBone* routebone = srcbone;
	while (routebone) {
		if (routebone->IsSkeleton()) {
			vecroute.push_back(routebone);
		}
		routebone = routebone->GetParent(false);
	}
	std::reverse(vecroute.begin(), vecroute.end());

	int curmotid = GetCurrentMotID();
	double curframe = GetCurrentFrame();

	std::vector<CBone*>::iterator itrbone;
	for (itrbone = vecroute.begin(); itrbone != vecroute.end(); itrbone++) {
		CBone* curbone = *itrbone;
		if (curbone) {
			bool calledbythread = false;
			curbone->UpdateMatrix(limitdegflag, curmotid, curframe, wmat, vmat, pmat, calledbythread, refposindex);
		}
	}

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


float CModel::GetTargetWeight(
	int motid, double srcframe, double srctimescale,
	CMQOObject* srcbaseobj, std::string channelname, int channelindex,
	FbxMesh* srcMesh)
{

	FbxAnimLayer* srcanimlayer = GetAnimLayer(motid);
	if (!srcanimlayer || !srcbaseobj) {
		return 0.0f;
	}


	return srcbaseobj->GetShapeAnimWeight(motid, IntTime(srcframe), channelindex);


	//FbxTime lTime;
	//lTime.SetSecondDouble(srcframe / srctimescale);
	//return GetFbxTargetWeight(srcMesh, channelname, channelindex, lTime, srcanimlayer, srcbaseobj);
	////return 1.0f;
}

float CModel::GetFbxTargetWeight(FbxMesh* srcMesh, std::string channelname, int channelindex, 
	FbxTime& pTime, FbxAnimLayer* pAnimLayer, CMQOObject* baseobj)
{
	if (!srcMesh || !pAnimLayer || !baseobj) {
		_ASSERT(0);
		return 0.0f;
	}

	int lVertexCount = srcMesh->GetControlPointsCount();
	if (lVertexCount != baseobj->GetVertex()) {
		_ASSERT(0);
		return 0.0f;
	}

	int lBlendShapeDeformerCount = srcMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	//for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	int lBlendShapeIndex = 0;
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)srcMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		if (lBlendShape) {
			//int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			//for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; lChannelIndex++)
			//{
				//FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(channelindex);
			if (lChannel)
			{
				//const char* nameptr = lChannel->GetName();
				int cmp0;
				cmp0 = strcmp(lChannel->GetName(), channelname.c_str());
				if (cmp0 == 0) {
					FbxAnimCurve* lFCurve;
					double lWeight = 0.0;
					//lFCurve = srcMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
					lFCurve = srcMesh->GetShapeChannel(lBlendShapeIndex, channelindex, pAnimLayer);
					if (lFCurve) {
						lWeight = lFCurve->Evaluate(pTime);
					}
					else {
						lWeight = 0.0;
					}

					if (lWeight != 0.0) {
						int dbgflag1 = 1;
					}

					return (float)lWeight;
				}
			}//If lChannel is valid
			else {
				int dbgflag2 = 1;
			}
			//}//For each blend shape channel
		}
		else {
			int dbgflag3 = 1;
		}
	}//For each blend shape deformer

	return 0.0f;

}

int CModel::GetFBXShape(FbxMesh* pMesh, CMQOObject* curobj)//2024/05/16 morphAnimは別関数で
{
	int lVertexCount = pMesh->GetControlPointsCount();
	if (lVertexCount <= 0) {
		return 0;
	}
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
				int lShapeIndex = 0;
				FbxShape* lShape = NULL;
				lShape = lChannel->GetTargetShape(lShapeIndex);//lShapeIndex+1ではない！！！！！！！！！！！！！！！！
				if(lShape)
				{	
					char shapename[256] = { 0 };
					strcpy_s(shapename, 256, lShape->GetName());

					int existshape = 0;
					existshape = curobj->ExistShape(shapename);
					if( existshape == 0 ){
						FbxVector4* shapev = lShape->GetControlPoints();
						if (shapev) {
							int shapevertnum = lShape->GetControlPointsCount();
							if (shapevertnum == lVertexCount) {
								curobj->AddShapeName(shapename);

								for (int j = 0; j < lVertexCount; j++)
								{
									ChaVector3 xv;
									xv.x = (float)shapev[j][0];
									xv.y = (float)shapev[j][1];
									xv.z = (float)shapev[j][2];
									curobj->SetShapeVert(shapename, j, xv);
								}
							}
							else {
								char errormsg[1024] = { 0 };
								sprintf_s(errormsg, 1024, "shape:%s, mismatch vnum : (%d-%d)",
									shapename, lVertexCount, shapevertnum);
								::MessageBoxA(NULL, errormsg, "Error", MB_OK | MB_ICONERROR);
								_ASSERT(0);
								abort();
							}
						}
						else {
							char errormsg[1024] = { 0 };
							sprintf_s(errormsg, 1024, "shape:%s, fbxshape null error",
								shapename);
							::MessageBoxA(NULL, errormsg, "Error", MB_OK | MB_ICONERROR);
							_ASSERT(0);
							abort();
						}
					}
				}
			}//If lChannel is validf
		}//For each blend shape channel
	}//For each blend shape deformer

	return 0;
}

void CModel::GetFBXShapeAnimReq(CNodeOnLoad* nodeonload, FbxAnimLayer* panimlayer, int curmotid, double animleng, int* perrorcount)
{
	if (nodeonload && panimlayer && (animleng > 0) && perrorcount) {
		FbxNode* pNode = nodeonload->GetNode();
		CMQOObject* pmqoobj = nodeonload->GetMqoObject();
		if (pNode && pmqoobj && !pmqoobj->EmptyShape()) {
			FbxMesh* pMesh = pNode->GetMesh();
			if (pMesh) {
				int result1 = GetFBXShapeAnim(pMesh, pmqoobj, panimlayer, curmotid, animleng);
				if (result1 != 0) {
					_ASSERT(0);
					(*perrorcount)++;
				}
			}
		}

		int childnum = nodeonload->GetChildNum();
		int childno;
		for (childno = 0; childno < childnum; childno++) {
			CNodeOnLoad* pchild = nodeonload->GetChild(childno);
			if (pchild) {
				GetFBXShapeAnimReq(pchild, panimlayer, curmotid, animleng, perrorcount);
			}
		}
	}
}


int CModel::GetFBXShapeAnim(FbxMesh* pMesh, CMQOObject* curobj, FbxAnimLayer* pAnimLayer, int curmotid, double animleng)
{
	int lVertexCount = pMesh->GetControlPointsCount();
	if (lVertexCount != curobj->GetVertex()) {
		_ASSERT(0);
		return 1;
	}


	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; lChannelIndex++)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if (lChannel)
			{

				int lShapeIndex = 0;
				FbxShape* lShape = NULL;
				lShape = lChannel->GetTargetShape(lShapeIndex);//lShapeIndex+1ではない！！！！！！！！！！！！！！！！
				if (lShape)
				{
					char shapename[256] = { 0 };
					strcpy_s(shapename, 256, lShape->GetName());


					curobj->DestroyShapeAnim(shapename, curmotid);//!!!!!!!!!!!!


					int existshape = 0;
					existshape = curobj->ExistShape(shapename);
					if (existshape == 1) {

						//####################################################
						//AddShapeAnim()はAddMotion()で実行済だが、
						//上記でDestroyShapeAnimしているので、AddShapeAnim()を呼ぶ
						//####################################################
						int result1 = curobj->AddShapeAnim(shapename, curmotid, IntTime(animleng));
						if (result1 != 0) {
							_ASSERT(0);
							abort();
						}

						FbxAnimCurve* lFCurve;
						double lWeight = 0.0;
						lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
						if (lFCurve) {
							int framecnt;
							for (framecnt = 0; framecnt < animleng; framecnt++) {
								double timescale = 30.0;
								FbxTime lTime;
								lTime.SetSecondDouble((double)framecnt / timescale);
								lWeight = lFCurve->Evaluate(lTime);

								if (lWeight != 0.0) {
									int dbgflag0 = 1;
								}

								int result2 = curobj->SetShapeAnim(shapename, curmotid, framecnt, (float)lWeight);
								if (result2 != 0) {
									_ASSERT(0);
									abort();
								}
							}
						}
						else {
							//_ASSERT(0);
							int dbgflag1 = 1;
						}

					}
				}
			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

	return 0;

}



int CModel::SetShaderConst(int btflag, bool calcslotflag)
{
	//ボーンが無くても　非スキンメッシュを表示することはある
	//g_hmWorld->SetMatrix(m_matWorld.GetDataPtr());

	if( !GetTopBone()){
		//_ASSERT(0);
		return 0;//!!!!!!!!!!!
	}
	if (GetNoBoneFlag() == true) {
		return 0;
	}

	MOTINFO* curmi = 0;
	int curmotid;
	double opeframe;
	if (!ExistCurrentMotion()) {
		//_ASSERT(0);
		return 0;
	}

	curmotid = GetCurrentMotID();
	opeframe = GetOpeFrame(calcslotflag);

	int setclcnt = 0;

	std::map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && curbone->IsSkeleton()) {
			bool currentlimitdegflag = g_limitdegflag;
			CMotionPoint curmp = curbone->GetCurMp(calcslotflag);

			ChaMatrix clustermat;
			clustermat.SetIdentity();

			int matrixindex = curbone->GetMatrixIndex();
			if (matrixindex >= 0) {

				if (setclcnt >= MAXBONENUM) {
					_ASSERT(0);
					::MessageBox(NULL, L"Bone num overflow error. Exit the app.", GetFileName(),
						MB_OK | MB_ICONERROR);
					abort();
				}
				else {
					//CMotionPoint tmpmp = curbone->GetCurMp();
					if (btflag == 0) {
						//set4x4[matrixindex] = tmpmp.GetWorldMat();
						clustermat = curbone->GetWorldMat(currentlimitdegflag, curmotid, opeframe, &curmp);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}
					else if (btflag == 1) {
						//物理シミュ
						//set4x4[matrixindex] = curbone->GetBtMat();
						clustermat = curbone->GetBtMat(calcslotflag);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}
					else if (btflag == 2) {
						//物理IK
						//set4x4[matrixindex] = curbone->GetBtMat();
						clustermat = curbone->GetBtMat(calcslotflag);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}
					else {
						//set4x4[matrixindex] = tmpmp.GetWorldMat();
						clustermat = curbone->GetWorldMat(currentlimitdegflag, curmotid, opeframe, &curmp);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}

					setclcnt++;
				}				
			}
		}
	}

	return 0;
}

int CModel::FillTimeLineOne(CBone* curbone, int lineno,
	OrgWinGUI::OWP_Timeline& timeline, 
	std::map<int, int>& lineno2boneno, std::map<int, int>& boneno2lineno)
{

	if (curbone) {
		int depth = curbone->CalcBoneDepth();
		bool ikstopflag = curbone->GetIKStopFlag();
		bool iktargetflag = curbone->GetIKTargetFlag();

		WCHAR dispname[JOINTNAMELENG];
		ZeroMemory(dispname, sizeof(WCHAR) * JOINTNAMELENG);
		if (curbone->GetWBoneName()) {
			size_t namelen = wcslen(curbone->GetWBoneName());
			size_t cplen;
			if (namelen < JOINTNAMELENG) {
				cplen = namelen;
			}
			else {
				
				_ASSERT(0);

				//タイムラインのnameとして　途中までしか登録されないので
				//タイムラインにおける名前での検索は　みつからなくなる
				cplen = JOINTNAMELENG - 1;
			}
			wcsncpy_s(dispname, JOINTNAMELENG, curbone->GetWBoneName(), cplen);
		}
		else {
			_ASSERT(0);
			wcscpy_s(dispname, JOINTNAMELENG, L"No Name");
		}

		//行を追加
		if (curbone->IsBranchBone()) {
			timeline.newLine(depth, 2, ikstopflag, iktargetflag, dispname);
		}
		else if (curbone->IsSkeleton()) {
			timeline.newLine(depth, 0, ikstopflag, iktargetflag, dispname);
		}
		else {
			timeline.newLine(depth, 1, ikstopflag, iktargetflag, dispname);
		}

		timeline.setHasRigFlag(curbone->GetWBoneName(), ChkBoneHasRig(curbone));


		lineno2boneno[lineno] = curbone->GetBoneNo();
		boneno2lineno[curbone->GetBoneNo()] = lineno;
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CModel::FillTimeLine(OrgWinGUI::OWP_Timeline& timeline, map<int, int>& lineno2boneno, map<int, int>& boneno2lineno)
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

		//if (curbone) {
		if (curbone && (curbone->IsSkeleton()) && (strstr(curbone->GetBoneName(), "Root") == 0)) {//2023/04/27
			//eNullはTreeViewに表示しない　Root, RootNodeもTreeViewに表示しない　TreeViewに表示しないものにはモーションも付けない

			FillTimeLineOne(curbone, lineno,
				timeline,
				lineno2boneno, boneno2lineno);

			lineno++;
		}
		else {
			//eNullはここを通る
			
			//_ASSERT(0);
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

	if (GetTopBone()){
		//選択行を設定
		timeline.setCurrentLineName(GetTopBone()->GetWBoneName());
	}
	return 0;
}

void CModel::FillTimelineReq( OrgWinGUI::OWP_Timeline& timeline, CBone* curbone, int* linenoptr, 
	map<int, int>& lineno2boneno, map<int, int>& boneno2lineno, int broflag )
{
	if (!curbone || !linenoptr){
		return;
	}

	if (curbone->IsSkeleton()) {
		FillTimeLineOne(curbone, *linenoptr,
			timeline,
			lineno2boneno, boneno2lineno);

		(*linenoptr)++;
	}

/***
	_ASSERT( m_curmotinfo );
	CMotionPoint* curmp = curbone->m_motionkey[ m_curmotinfo->motid ];
	while( curmp ){
		timeline.newKey( curbone->m_wbonename, curmp->m_frame, (void*)curmp );
		curmp = curmp->m_next;
	}
***/

	if( curbone->GetChild(false) ){
		FillTimelineReq( timeline, curbone->GetChild(false), linenoptr, lineno2boneno, boneno2lineno, 1 );
	}
	if( broflag && curbone->GetBrother(false) ){
		FillTimelineReq( timeline, curbone->GetBrother(false), linenoptr, lineno2boneno, boneno2lineno, 1 );
	}
}

int CModel::AddMotion(const char* srcname, const WCHAR* wfilename, double srcleng, int* dstid)
{

	bool limitdegflagOnAddMotion = false;//読み込みに関しては　unlimitedWorld


	*dstid = -1;
	//int leng = (int)strlen(srcname);


	//モーフアニメがあるかもしれないのでリターンしない
	//if (GetNoBoneFlag() == true) {
	//	return 0;
	//}



	//int samenamecount = 0;

	int maxid = 0;
	map<int, MOTINFO*>::iterator itrmi;
	for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++){
		MOTINFO* chkmi = itrmi->second;
		if (chkmi){
			if (maxid < chkmi->motid){
				maxid = chkmi->motid;
			}

			//if (strcmp(chkmi->motname, srcname) == 0) {
			//	samenamecount++;
			//}
		}
	}
	int newid = maxid + 1;


	MOTINFO* newmi = (MOTINFO*)malloc(sizeof(MOTINFO));
	if (!newmi){
		_ASSERT(0);
		return 1;
	}
	::ZeroMemory(newmi, sizeof(MOTINFO));

	//if (samenamecount == 0) {
		strcpy_s(newmi->motname, 256, srcname);
	//}
	//else {
	//	sprintf_s(newmi->motname, 256, "%s_%d", srcname, samenamecount);
	//}
	
	if (wfilename){
		wcscpy_s(newmi->wfilename, MAX_PATH, wfilename);
	}
	else{
		::ZeroMemory(newmi->wfilename, sizeof(WCHAR)* MAX_PATH);
	}
	::ZeroMemory(newmi->engmotname, sizeof(char)* 256);

	newmi->motid = newid;
	newmi->frameleng = srcleng;
	//newmi->curframe = 0.0;
	newmi->curframe = 1.0;
	newmi->speed = 1.0;
	newmi->loopflag = 1;

	m_motinfo[(int)m_motinfo.size()] = newmi;//2021/08/26 eraseすることがあるのでindex = motid - 1とは限らない




	//初期化　2022/08/28
	SetCurrentMotion(newid);
	
	
	//2023/02/11
	//この関数AddMotionにおいて
	//InitMPReqとCreateIndexedMotionPointReqは
	//fbxのモーションを読み込み済の場合に　新規モーション追加の際だけ意味がある
	// 
	//fbxAnim読み込み時は　姿勢データ読み込み後に　CreateIndexedMotionPointを呼ぶ(2023/02/11)
	// 
	//モーションを持たないfbxに対して　MotionPointを作成するのは　
	//Main.cppのInitCurMotion()から呼ばれるInitMPReqとCreateIndexedMotionPointReq
	// 
	// 
	//2023/10/27
	//モーションを持たないfbxに対して　MotionPointを作成するのは　
	//Main.cppのAddMotion()から呼ばれる　本関数(CModel::AddMotion())のInitMPReqとCreateIndexedMotionPointReq


	if (GetLoadedFlag() == true) {

		//####################################################################################################
		//2023/10/27
		//この部分においては　シングルスレッド版を使う
		//ここで実行する時点においては　モーションポイントのインデックスが未作成
		//インデックスが未作成の場合には GetBefNextMP()はキャッシュを参照しながら　チェインをたどる処理をする
		//インデックス未作成時にマルチスレッド版を使うと　キャッシュヒット率が低い
		//よって　ここでは　キャッシュ付きのシングルスレッド版を使う方が高速
		//####################################################################################################
		double framecnt;
		for (framecnt = 0.0; framecnt < srcleng; framecnt += 1.0) {
			InitMPReq(limitdegflagOnAddMotion, GetTopBone(false), newid, framecnt);//motionpointが無い場合は作成も
		}


		// 
		//2023/10/26 マルチスレッド版を使う
		//InitMpFrame(limitdegflagOnAddMotion, newid, GetTopBone(false), 0.0, srcleng - 1.0);


		int errorcount = 0;
		CreateIndexedMotionPoint(newid, srcleng, &errorcount);//2022/10/30
		if (errorcount != 0) {
			if (GetBoneForMotionSize() > 1) {
				_ASSERT(0);
			}
		}

		//for debug 2024/06/15
		//CheckBoneTreeForDebugReq(GetTopBone(false), newid);

	}


	//#########################
	//BlendShape
	// anim格納用のデータ作成だけ
	//#########################
	map<int, CMQOObject*>::iterator itrobj;
	for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
		CMQOObject* curobj = itrobj->second;
		_ASSERT(curobj);
		if (!(curobj->EmptyShape())) {
			int shapenum = curobj->GetShapeNameNum();
			int shapeindex;
			for (shapeindex = 0; shapeindex < shapenum; shapeindex++) {
				int error0 = 0;
				string strshapename = curobj->GetShapeName(shapeindex, &error0);
				if (error0 == 0) {
					char shapename[256] = { 0 };
					strcpy_s(shapename, 256, strshapename.c_str());
					int result1 = curobj->AddShapeAnim(shapename, newid, IntTime(srcleng));
					if (result1 != 0) {
						_ASSERT(0);
						abort();
					}
				}
				else {
					_ASSERT(0);
					abort();
				}
			}
		}
	}


	*dstid = newid;

	return 0;
}


int CModel::SetCurrentMotion( int srcmotid )
{
	int motnum;
	motnum = (int)m_motinfo.size();
	//if ((srcmotid <= 0) || (srcmotid > motnum)) {
	//	_ASSERT(0);
	//	return 1;
	//}

	//m_curmotinfo = m_motinfo[ srcmotid - 1 ];//idは１から
	m_curmotinfo = GetMotInfoPtr(srcmotid);//2021/08/26 eraseすることがあるのでindex = motid - 1とは限らない
	if( !m_curmotinfo ){
		_ASSERT( 0 );
		return 1;
	}else{
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone){
				//if (curbone->IsSkeleton() || curbone->IsCamera() || curbone->IsNullAndChildIsCamera()) {
				if (curbone->IsSkeleton()) {
					curbone->SetCurrentMotion(srcmotid, m_curmotinfo->frameleng);
				}
				//else if (curbone->IsCamera()) {
				//	if ((GetCameraMotionId() >= 0)) {
				//		int cameramotid = GetCameraMotionId();
				//		MOTINFO* camerami = GetMotInfo(cameramotid);
				//		if (camerami) {
				//			curbone->SetCurrentMotion(cameramotid, camerami->frameleng);
				//		}
				//		else {
				//			_ASSERT(0);
				//			curbone->SetCurrentMotion(srcmotid, m_curmotinfo->frameleng);
				//		}
				//	}
				//	else {
				//		curbone->SetCurrentMotion(srcmotid, m_curmotinfo->frameleng);
				//	}
				//}
			}
		}
		//ResetMotionCache();

		m_curmotinfo->curframe = 1.0;
		m_curmotinfo->befframe = 1.0;

		//if ((m_curmotinfo->fbxanimno >= 0) && GetScene()) {
		//	FbxAnimStack* lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[m_curmotinfo->fbxanimno]->Buffer());
		//	if (lCurrentAnimationStack != NULL) {
		//		FbxAnimLayer* mCurrentAnimLayer;
		//		mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
		//		SetCurrentAnimLayer(mCurrentAnimLayer);
		//		GetScene()->SetCurrentAnimationStack(lCurrentAnimationStack);
		//	}
		//}

		return 0;
	}
}
int CModel::SetMotionFrame(double srcframe)
{
	if( !m_curmotinfo ){
		if (GetNoBoneFlag()) {
			//return 0;//エラーではない　ボーンが無いだけ
			//エラーの可能性がある
			_ASSERT(0);
			return 1;
		}
		else {
			//エラーの可能性がある
			_ASSERT(0);
			return 1;
		}
	}

	m_curmotinfo->curframe = max(0.0, min((m_curmotinfo->frameleng - 1.0), srcframe));

	return 0;
}
int CModel::SetMotionFrame(int srcmotid, double srcframe)
{
	MOTINFO* curmi = GetMotInfoPtr(srcmotid);
	if (curmi) {
		curmi->curframe = max(0.0, min((curmi->frameleng - 1.0), srcframe));
	}
	else {
		if (GetNoBoneFlag()) {
			return 0;//エラーではない　ボーンが無いだけ
		}
		else {
			//エラーの可能性がある
			_ASSERT(0);
			return 1;
		}
	}

	return 0;
}

int CModel::SetMotionName(int srcmotid, char* srcname)
{
	if (!srcname) {
		_ASSERT(0);
		return 1;
	}
	if (srcname[0] == 0) {
		_ASSERT(0);
		return 1;
	}

	MOTINFO* curmi = GetMotInfoPtr(srcmotid);
	if (curmi) {
		strcpy_s(curmi->motname, 256, srcname);
		return 0;
	}
	else {
		_ASSERT(0);
		return 1;
	}
	return 0;
}

int CModel::GetMotionName(int srcmotid, int dstnamelen, char* dstname)
{
	if (!dstname) {
		_ASSERT(0);
		return 1;
	}
	if (dstnamelen != 256) {//!!!!!!!!!!!
		_ASSERT(0);
		return 1;
	}

	MOTINFO* curmi = GetMotInfoPtr(srcmotid);
	if (curmi) {
		strcpy_s(dstname, dstnamelen, curmi->motname);
		return 0;
	}
	else {
		_ASSERT(0);
		return 1;
	}

}

//int CModel::GetMotionFrame(double* dstframe)
//{
//	if( !m_curmotinfo ){
//		_ASSERT( 0 );
//		return 1;
//	}
//
//	*dstframe = GetCurrentFrame();
//
//	return 0;
//}
int CModel::SetMotionSpeed( double srcspeed )
{
	if( !m_curmotinfo ){
		//_ASSERT( 0 );
		return 1;
	}

	m_curmotinfo->speed = srcspeed;

	return 0;
}
int CModel::SetMotionSpeed(int srcmotid, double srcspeed)
{
	MOTINFO* curmi = GetMotInfoPtr(srcmotid);
	if (curmi) {
		curmi->speed = srcspeed;
	}

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
	//##############################################################
	//削除によりmapのm_motinfo[]の添え字は変わるがmotidは変わらない
	//##############################################################


	if (motid == GetCameraMotionId()) {
		//カメラアニメIDを無効に
		SetCameraMotionId(-1);
	}


	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		if( curbone && (curbone->IsSkeleton())){
			CallF( curbone->DeleteMotion( motid ), return 1 );
		}
	}

	int miindex;
	miindex = MotionID2Index(motid);
	if (miindex >= 0) {
		MOTINFO* delmi = m_motinfo[miindex];
		if (delmi) {
			//delete delmi;
			free(delmi);
		}

		std::map<int, MOTINFO*> tmpmimap;
		tmpmimap.clear();

		int destindex = 0;
		int srcindex;
		for (srcindex = 0; srcindex < m_motinfo.size(); srcindex++) {
			if (srcindex != miindex) {
				tmpmimap[destindex] = m_motinfo[srcindex];
				destindex++;
			}
		}

		m_motinfo.clear();

		for (srcindex = 0; srcindex < tmpmimap.size(); srcindex++) {
			m_motinfo[srcindex] = tmpmimap[srcindex];
		}

	}

	int undono;
	for( undono = 0; undono < UNDOMAX; undono++ ){
		if( m_undomotion[undono].GetSaveMotInfo().motid == motid ){
			m_undomotion[undono].SetValidFlag( 0 );
		}
	}

	ResetMotionCache();

	return 0;
}

void CModel::ResetMotionCache()
{
	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {
			curbone->ResetMotionCache();
		}
	}
}

int CModel::GetSymBoneNo( int srcboneno, int* dstboneno, int* existptr )
{
	*existptr = 0;

	CBone* srcbone = m_bonelist[ srcboneno ];
	if( !srcbone ){
		*dstboneno = -1;
		return 0;
	}
	if (srcbone->IsNotSkeleton()) {
		*dstboneno = -1;
		return 0;
	}

	//int findflag = 0;
	//WCHAR findname[256];
	//ZeroMemory( findname, sizeof( WCHAR ) * 256 );
	//wcscpy_s( findname, 256, srcbone->GetWBoneName() );
	//WCHAR* lpat = wcsstr( findname, L"_L_" );
	//if( lpat ){
	//	*lpat = TEXT( '_' );
	//	*(lpat + 1) = TEXT( 'R' );
	//	*(lpat + 2) = TEXT( '_' );
	//	//wcsncat_s( findname, 256, L"_R_", 3 );
	//	findflag = 1;
	//}else{
	//	WCHAR* rpat = wcsstr( findname, L"_R_" );
	//	if( rpat ){
	//		*rpat = TEXT( '_' );
	//		*(rpat + 1) = TEXT( 'L' );
	//		*(rpat + 2) = TEXT( '_' );
	//		//wcsncat_s( findname, 256, L"_L_", 3 );
	//		findflag = 1;
	//	}
	//}
	//int setflag = 0;
	////if( findflag == 0 ){
	////	*dstboneno = srcboneno;
	////	*existptr = 0;
	////}else{
	//if (findflag != 0){
	//	CBone* dstbone = 0;
	//	map<int, CBone*>::iterator itrbone;
	//	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
	//		CBone* chkbone = itrbone->second;
	//		if( chkbone && (wcscmp( findname, chkbone->GetWBoneName() ) == 0) ){
	//			dstbone = chkbone;
	//			break;
	//		}
	//	}
	//	if( dstbone ){
	//		*dstboneno = dstbone->GetBoneNo();
	//		*existptr = 1;
	//		setflag = 1;
	//	}
	//	//else{
	//	//	*dstboneno = srcboneno;
	//	//	*existptr = 0;
	//	//}
	//}

	bool setflag = false;
	{
		string strcurbonename = srcbone->GetBoneName();
		string strLeft = "Left";
		string strRight = "Right";

		string chkLeft = strcurbonename;
		string chkRight = strcurbonename;

		std::string::size_type leftpos = chkLeft.find(strLeft);
		if (leftpos != std::string::npos) {
			//Leftの部分をRightに変えてボーンが存在すればそのボーンに移動
			chkLeft.replace(leftpos, strLeft.length(), strRight);
			CBone* rightbone = FindBoneByName(chkLeft.c_str());
			if (rightbone) {
				int nextboneno = rightbone->GetBoneNo();
				if (nextboneno >= 0) {
					*dstboneno = nextboneno;
					*existptr = 1;
					setflag = true;
				}
			}
		}
		else {
			std::string::size_type rightpos = chkRight.find(strRight);
			if (rightpos != std::string::npos) {
				//Rightの部分をLeftに変えてボーンが存在すればそのボーンに移動
				chkRight.replace(rightpos, strRight.length(), strLeft);
				CBone* leftbone = FindBoneByName(chkRight.c_str());
				if (leftbone) {
					int nextboneno = leftbone->GetBoneNo();
					if (nextboneno >= 0) {
						*dstboneno = nextboneno;
						*existptr = 1;
						setflag = true;
					}
				}
			}
		}
	}

	if(setflag == false)
	{
		string strcurbonename = srcbone->GetBoneName();
		string strLeft = "_L_";
		string strRight = "_R_";

		string chkLeft = strcurbonename;
		string chkRight = strcurbonename;

		std::string::size_type leftpos = chkLeft.find(strLeft);
		if (leftpos != std::string::npos) {
			//_L_の部分を_R_に変えてボーンが存在すればそのボーンに移動
			chkLeft.replace(leftpos, strLeft.length(), strRight);
			CBone* rightbone = FindBoneByName(chkLeft.c_str());
			if (rightbone) {
				int nextboneno = rightbone->GetBoneNo();
				if (nextboneno >= 0) {
					*dstboneno = nextboneno;
					*existptr = 1;
					setflag = true;
				}
			}
		}
		else {
			std::string::size_type rightpos = chkRight.find(strRight);
			if (rightpos != std::string::npos) {
				//_R_の部分を_L_に変えてボーンが存在すればそのボーンに移動
				chkRight.replace(rightpos, strRight.length(), strLeft);
				CBone* leftbone = FindBoneByName(chkRight.c_str());
				if (leftbone) {
					int nextboneno = leftbone->GetBoneNo();
					if (nextboneno >= 0) {
						*dstboneno = nextboneno;
						*existptr = 1;
						setflag = true;
					}
				}
			}
		}
	}


	if (setflag == false) {
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
	if (srcbone->IsNotSkeleton()) {
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
		if (curbone && (curbone->IsSkeleton())){
			ChaVector3 curpos = curbone->GetJointFPos();
			ChaVector3 diffpos = curpos - srcpos;
			float curdist = (float)ChaVector3LengthDbl(&diffpos);
			if ((curdist <= mindist) && (curdist <= matchdist) && (curbone != srcbone)){
				//同一位置のボーンが存在する場合があるので、親もチェックする。
				CBone* srcparentbone = srcbone->GetParent(false);
				CBone* curparentbone = curbone->GetParent(false);
				if (srcparentbone && srcparentbone->IsSkeleton() && 
					curparentbone && curparentbone->IsSkeleton()){
					ChaVector3 srcparentpos = srcparentbone->GetJointFPos();
					srcparentpos.x *= -1.0f;
					ChaVector3 curparentpos = curparentbone->GetJointFPos();
					ChaVector3 pardiffpos = srcparentpos - curparentpos;
					float pardist = (float)ChaVector3LengthDbl(&pardiffpos);
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


int CModel::PickBone( UIPICKINFO* pickinfo )
{
	pickinfo->pickobjno = -1;

	float fw, fh;
	fw = (float)pickinfo->winx / 2.0f;
	fh = (float)pickinfo->winy / 2.0f;

	int minno = -1;
	ChaVector3 cmpsc;
	ChaVector3 picksc;
	picksc.SetParams(0.0f, 0.0f, 0.0f);
	ChaVector3 pickworld;
	pickworld.SetParams(0.0f, 0.0f, 0.0f);
	float cmpdist;
	float mindist = 0.0f;
	int firstflag = 1;
	ChaVector2 firstdiff;
	firstdiff.SetParams(0.0f, 0.0f);

	map<int, CBone*>::iterator itrbone;
	for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;

		//if( curbone && !curbone->GetSkipRenderBoneMark()){
		
		//ボーンマーク表示をスキップしているボーンはPick対象から外す 2022/09/16
		//eNullもPick対象から外す 2023/04/27
		if (curbone && !curbone->GetSkipRenderBoneMark() && (curbone->IsSkeleton())) {
			cmpsc.x = ( 1.0f + curbone->GetChildScreen().x ) * fw;
			cmpsc.y = ( 1.0f - curbone->GetChildScreen().y ) * fh;
			cmpsc.z = curbone->GetChildScreen().z;

			if( (cmpsc.z >= 0.0f) && (cmpsc.z <= 1.0f) ){
				float mag;
				firstdiff.x = (float)pickinfo->clickpos.x - cmpsc.x;
				firstdiff.y = (float)pickinfo->clickpos.y - cmpsc.y;
				mag = firstdiff.x * firstdiff.x + firstdiff.y * firstdiff.y;
				if (mag != 0.0f) {
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
		pickinfo->firstdiff = firstdiff;
	}

	return 0;
}

void CModel::SetSelectFlagReq( CBone* boneptr, int broflag )
{
	if (boneptr->IsSkeleton()) {
		boneptr->SetSelectFlag(1);
	}

	if( boneptr->GetChild(false) ){
		SetSelectFlagReq( boneptr->GetChild(false), 1 );
	}
	if( boneptr->GetBrother(false) && broflag ){
		SetSelectFlagReq( boneptr->GetBrother(false), 1 );
	}
}

int CModel::CollisionPolyMesh_Mouse(UIPICKINFO* pickinfo, CMQOObject* pickobj, int* hitfaceindex, ChaVector3* dsthitpos)
{
	//当たったら１、当たらなかったら０を返す。エラーも０を返す。

	//ComputeShader版　polymesh4用

	if (!pickinfo || !pickobj || !hitfaceindex || !dsthitpos) {
		_ASSERT(0);
		return 0;
	}
	*hitfaceindex = -1;
	dsthitpos->SetParams(0.0f, 0.0f, 0.0f);

	ChaVector3 startglobal, dirglobal;
	CalcMouseGlobalRay(pickinfo, &startglobal, &dirglobal);

	ChaVector3 startlocal, dirlocal;
	CalcMouseLocalRay(pickinfo, &startlocal, &dirlocal);

	bool excludeinvface = true;
	int colli = 0;
	colli = pickobj->CollisionGlobal_Ray_Pm(startglobal, dirglobal, startlocal, dirlocal,
		excludeinvface, hitfaceindex, dsthitpos);//polymesh4の場合のhitposはグローバル座標系で返ってくる
	return colli;

}

int CModel::CollisionPolyMesh3_Mouse(UIPICKINFO* pickinfo, CMQOObject* pickobj, int* hitfaceindex, ChaVector3* dsthitpos)
{
	//当たったら１、当たらなかったら０を返す。エラーも０を返す。

	//CPU版　polymesh3用

	if (!pickinfo || !pickobj || !hitfaceindex || !dsthitpos) {
		_ASSERT(0);
		return 0;
	}
	*hitfaceindex = -1;
	dsthitpos->SetParams(0.0f, 0.0f, 0.0f);

	ChaVector3 startlocal, dirlocal;
	CalcMouseLocalRay(pickinfo, &startlocal, &dirlocal);
	bool excludeinvface = true;
	int colli = 0;
	ChaVector3 tmphitpos;
	tmphitpos.SetParams(0.0f, 0.0f, 0.0f);
	colli = pickobj->CollisionLocal_Ray_Pm3(startlocal, dirlocal, excludeinvface, hitfaceindex, &tmphitpos);


	//2024/06/16
	//CollisionLocal_Ray_Pm3()で取得するhitposはメッシュのローカル座標系での座標値
	//アプリで使用しやすいように　ワールド座標系に直してから返す
	if (colli != 0) {
		ChaMatrix tmpwm = GetWorldMat();
		ChaVector3TransformCoord(dsthitpos, &tmphitpos, &tmpwm);
	}

	return colli;

}



int CModel::GetResultOfPickRay(CMQOObject* pickobj, int* hitfaceindex, ChaVector3* dsthitpos)
{
	//当たったら１、当たらなかったら０を返す。エラーも０を返す。

	//ComputeShader版　polymesh3, polymesh4両方OK

	if (!pickobj || !hitfaceindex || !dsthitpos) {
		_ASSERT(0);
		return 0;
	}
	*hitfaceindex = -1;
	dsthitpos->SetParams(0.0f, 0.0f, 0.0f);

	int colli = 0;
	colli = pickobj->GetResultOfPickRay(hitfaceindex, dsthitpos);
	return colli;

}

int CModel::CollisionNoBoneObj_Mouse( UIPICKINFO* pickinfo, const char* objnameptr, 
	bool excludeinvface)
{
	//当たったら１、当たらなかったら０を返す。エラーも０を返す。
	//マニピュレータピック用

	CMQOObject* curobj = m_objectname[ objnameptr ];
	if( !curobj ){
		_ASSERT( 0 );
		return 0;
	}

	ChaVector3 startlocal, dirlocal;
	CalcMouseLocalRay( pickinfo, &startlocal, &dirlocal );

	int colli = curobj->CollisionLocal_Ray(startlocal, dirlocal, excludeinvface);

	return colli;
}

int CModel::CalcMouseLocalRay( UIPICKINFO* pickinfo, ChaVector3* startptr, ChaVector3* dirptr )
{
	ChaVector3 startsc, endsc;
	float rayx, rayy;
	rayx = (float)pickinfo->clickpos.x / ((float)pickinfo->winx / 2.0f) - 1.0f;
	rayy = 1.0f - (float)pickinfo->clickpos.y / ((float)pickinfo->winy / 2.0f);

	startsc.SetParams( rayx, rayy, 0.0f );
	endsc.SetParams( rayx, rayy, 1.0f );
	
    ChaMatrix mWVP, invmWVP;
	mWVP = m_matWorld * m_matVP;
	//ChaMatrixInverse( &invmWVP, NULL, &mWVP );
	invmWVP = ChaMatrixInv(mWVP);

	ChaVector3 startlocal, endlocal;

	ChaVector3TransformCoord( &startlocal, &startsc, &invmWVP );
	ChaVector3TransformCoord( &endlocal, &endsc, &invmWVP );

	ChaVector3 dirlocal = endlocal - startlocal;
	ChaVector3Normalize( &dirlocal, &dirlocal );

	*startptr = startlocal;
	*dirptr = dirlocal;

	return 0;
}

int CModel::CalcMouseGlobalRay(UIPICKINFO* pickinfo, ChaVector3* startptr, ChaVector3* dirptr)
{
	ChaVector3 startsc, endsc;
	float rayx, rayy;
	rayx = (float)pickinfo->clickpos.x / ((float)pickinfo->winx / 2.0f) - 1.0f;
	rayy = 1.0f - (float)pickinfo->clickpos.y / ((float)pickinfo->winy / 2.0f);

	startsc.SetParams(rayx, rayy, 0.0f);
	endsc.SetParams(rayx, rayy, 1.0f);

	ChaMatrix invmVP;
	invmVP = ChaMatrixInv(m_matVP);

	ChaVector3 startglobal, endglobal;

	ChaVector3TransformCoord(&startglobal, &startsc, &invmVP);
	ChaVector3TransformCoord(&endglobal, &endsc, &invmVP);

	ChaVector3 dirglobal = endglobal - startglobal;
	ChaVector3Normalize(&dirglobal, &dirglobal);

	*startptr = startglobal;
	*dirptr = dirglobal;

	return 0;
}


//CBone* CModel::GetCalcRootBone( CBone* firstbone, int maxlevel )
//{
//	int levelcnt = 0;
//	CBone* retbone = firstbone;
//	CBone* curbone = firstbone;
//	while( curbone && ((maxlevel == 0) || (levelcnt <= maxlevel)) )
//	{
//		retbone = curbone;
//		curbone = curbone->GetParent();
//		levelcnt++;
//	}
//
//	return retbone;
//}


int CModel::TransformBone( int winx, int winy, int srcboneno, ChaVector3* worldptr, ChaVector3* screenptr, ChaVector3* dispptr )
{					
	CBone* curbone;
	curbone = m_bonelist[ srcboneno ];
	if (curbone && curbone->IsSkeleton()){
		*worldptr = curbone->GetChildWorld();
		ChaMatrix mWVP;
		ChaMatrix mW;
		if (g_previewFlag != 5){
			if (curbone->GetParent(false) && curbone->GetParent(false)->IsSkeleton()) {
				//mW = curbone->GetParent()->GetCurMp().GetWorldMat();
				mW = curbone->GetParent(false)->GetCurrentWorldMat(true, true);
			}
			else {
				//mW = curbone->GetCurMp().GetWorldMat();
				mW = curbone->GetCurrentWorldMat(true, true);
			}
		}
		else{
			CBone* parentbone = curbone->GetParent(false);
			if (parentbone && parentbone->IsSkeleton()){
				mW = parentbone->GetBtMat(true);//endjointのbtmatがおかしい可能性があるため。
			}
			else{
				mW = curbone->GetBtMat(true);
			}
		}

		ChaVector3 tmpfpos = curbone->GetJointFPos();
		ChaVector3TransformCoord(worldptr, &tmpfpos, &mW);
		mWVP = mW * m_matVP;
		ChaVector3TransformCoord(screenptr, &tmpfpos, &mWVP);

	}
	else {
		ChaMatrix mWVP;
		ChaMatrix mW;
		mW.SetIdentity();
		mWVP = m_matVP;

		ChaVector3 tmpfpos;
		tmpfpos.SetParams(0.0f, 0.0f, 0.0f);
		ChaVector3TransformCoord(worldptr, &tmpfpos, &mW);
		ChaVector3TransformCoord(screenptr, &tmpfpos, &mWVP);

	}

	float fw, fh;
	fw = (float)winx / 2.0f;
	fh = (float)winy / 2.0f;
	dispptr->x = (1.0f + screenptr->x) * fw;
	dispptr->y = (1.0f - screenptr->y) * fh;
	dispptr->z = screenptr->z;


	return 0;
}


int CModel::ChangeMotFrameLeng( int motid, double srcleng )
{
	//MOTINFO* dstmi = m_motinfo[ motid - 1 ];//idは１から
	MOTINFO* dstmi = GetMotInfoPtr(motid);//2021/08/26 eraseすることがあるのでindex = motid - 1とは限らない
	if( dstmi ){
		double befleng = dstmi->frameleng;

		dstmi->frameleng = srcleng;

		if( befleng > srcleng ){
			map<int, CBone*>::iterator itrbone;
			for( itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++ ){
				CBone* curbone = itrbone->second;
				if( curbone && (curbone->IsSkeleton() || curbone->IsCamera() || curbone->IsNullAndChildIsCamera())){
					curbone->DeleteMPOutOfRange( motid, srcleng - 1.0 );
				}
			}
		}

		//2024/06/07 blendshapeアニメ　モーフアニメのアニメ長変更
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			if (!(curobj->EmptyShape())) {
				curobj->ChangeMorphAnimFrameLeng(motid, srcleng);
			}
		}

	}
	return 0;
}

int CModel::AdvanceTime( int onefps, CEditRange srcrange, int previewflag, double difftime, double* nextframeptr, int* endflagptr, int* loopstartflag, int srcmotid)
{
	if (!nextframeptr || !endflagptr || !loopstartflag) {
		return 1;
	}

	*endflagptr = 0;
	*loopstartflag = 0;

	int loopflag = 0;
	MOTINFO curmotinfo;
	curmotinfo.Init();
	if( srcmotid > 0 ){
		//curmotinfo = m_motinfo[ srcmotid - 1];//idは１から
		curmotinfo = GetMotInfo(srcmotid);//2021/08/26 eraseすることがあるのでindex = motid - 1とは限らない
		//loopflag = 0;
	}else{
		//わざとsrcmotidに-1をセットして呼び出した場合は　カレントモーションに対する処理をする
		curmotinfo = GetCurMotInfo();
		//loopflag = curmotinfo.loopflag;
	}
	loopflag = curmotinfo.loopflag;

	if(curmotinfo.motid <= 0){
		return 0;
	}


	double curspeed, curframe;
	curspeed = curmotinfo.speed;
	//curframe = GetCurrentFrame();//<-- カメラの処理をしている場合、GetCurrentFrame()はカメラのフレームとは限らない
	curframe = curmotinfo.curframe;//2024/06/02

	double nextframe;
	double oneframe = 1.0 / 30.0;
	//double oneframe = 1.0 / 300.0;

	double rangestart, rangeend;
	rangestart = srcrange.GetStartFrame();
	rangeend = srcrange.GetEndFrame();
	if (rangestart == rangeend){
		rangestart = 1.0;
		rangeend = curmotinfo.frameleng - 1.0;
	}

	rangestart = max(1.0, rangestart);


	if( previewflag >= 0 ){
		if (onefps == 0) {
			nextframe = curframe + difftime / oneframe * curspeed;
		}
		else {
			nextframe = curframe + 1.0;
		}
		if( nextframe > rangeend ){
			if( loopflag == 0 ){
				nextframe = rangeend;
				*endflagptr = 1;
			}else{
				nextframe = rangestart;
				*loopstartflag = 1;
			}
		}
	}else{
		if (onefps == 0) {
			nextframe = curframe - difftime / oneframe * curspeed;
		}
		else {
			nextframe = curframe + 1.0;
		}
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

int CModel::AddDefMaterialIfEmpty()
{
	if (m_materialbank.GetSize() <= 0) {
		char dummyname[256] = { 0 };
		strcpy_s(dummyname, 256, "dummyMaterial");

		CMQOMaterial* dummymat = GetMQOMaterialByName(dummyname);//既に存在するかどうかチェック
		if (!dummymat) {
			dummymat = new CMQOMaterial();
			if (!dummymat) {
				_ASSERT(0);
				return 1;
			}
			dummymat->SetName("dummyMaterial");
			SetMQOMaterial(dummyname, dummymat);
		}
	}

	return 0;
}

void CModel::CreateNodeOnLoadReq(CNodeOnLoad* newnodeonload)
{
	if (!newnodeonload) {
		_ASSERT(0);
		return;
	}

	FbxNode* pNode = newnodeonload->GetNode();

	if (!pNode) {
		_ASSERT(0);
		return;
	}
	const char* srcname = pNode->GetName();


	FbxNodeAttribute* pAttrib = pNode->GetNodeAttribute();
	if (pAttrib) {
		FbxNodeAttribute::EType type = (FbxNodeAttribute::EType)(pAttrib->GetAttributeType());

		switch (type)
		{
		case FbxNodeAttribute::eMesh:
		{
			newnodeonload->SetType(NOL_MESH);
			map<FbxNode*, CMQOObject*>::iterator itrmqoobj;
			itrmqoobj = m_node2mqoobj.find(pNode);
			if (itrmqoobj != m_node2mqoobj.end()) {
				newnodeonload->SetMqoObject(itrmqoobj->second);
			}
			else {
				_ASSERT(0);
				newnodeonload->SetMqoObject(0);
			}
		}
			break;
		case FbxNodeAttribute::eSkeleton:
		case FbxNodeAttribute::eCamera://2023/05/23
		{
			newnodeonload->SetType(NOL_SKELETON);
			CBone* pbone = FindBoneByNode(pNode);
			if (pbone) {
				newnodeonload->SetBone(pbone);
			}
			else {
				if (m_pscene && (m_pscene->GetRootNode() == pNode)) {
					//Rootのときには　正常
				}
				else if (strstr(srcname, "Root") != 0) {
					//RootNodeのときには　正常
				}
				else {
					_ASSERT(0);
				}
				newnodeonload->SetBone(0);
			}
		}
			break;
		case FbxNodeAttribute::eNull:
		{
			newnodeonload->SetType(NOL_NULL);
			CBone* pbone = FindBoneByNode(pNode);
			if (pbone) {
				newnodeonload->SetBone(pbone);
			}
			else {
				newnodeonload->SetBone(0);
			}		
		}
			break;
		default:
			newnodeonload->SetType(NOL_OTHER);
			break;
		}
	}
	else {
		newnodeonload->SetType(NOL_NULL);
		CBone* pbone = FindBoneByNode(pNode);
		if (pbone) {
			newnodeonload->SetBone(pbone);
		}
		else {
			newnodeonload->SetBone(0);
		}
	}


	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for (int i = 0; i < childNodeNum; i++)
	{
		FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
		if (pChild) {
			CNodeOnLoad* childnodeonload = new CNodeOnLoad(pChild);
			newnodeonload->AddChildOnLoad(childnodeonload);
			CreateNodeOnLoadReq(childnodeonload);
		}
	}
}

void CModel::DestroyNodeOnLoadReq(CNodeOnLoad* delnodeonload)
{
	if (delnodeonload) {
		vector<CNodeOnLoad*> savechild;
		savechild.clear();
		int childnum = delnodeonload->GetChildNum();
		int childno;
		for (childno = 0; childno < childnum; childno++) {
			CNodeOnLoad* pchild = delnodeonload->GetChild(childno);
			savechild.push_back(pchild);
		}

		delete delnodeonload;


		for (childno = 0; childno < childnum; childno++) {
			CNodeOnLoad* pchild = savechild[childno];
			if (pchild) {
				DestroyNodeOnLoadReq(pchild);
			}
		}
	}
}


void CModel::CreateFBXCameraReq(FbxNode* pNode)
{
	if (!pNode) {
		return;
	}


	FbxNodeAttribute* pAttrib = pNode->GetNodeAttribute();
	if (pAttrib) {
		FbxNodeAttribute::EType type = (FbxNodeAttribute::EType)(pAttrib->GetAttributeType());
		switch (type)
		{
		case FbxNodeAttribute::eCamera:
		{
			CBone* camerabone = FindBoneByNode(pNode);
			if (camerabone) {
				m_camerafbx.AddFbxCamera(pNode, camerabone);
			}
			else {
				_ASSERT(0);
			}
		}
		break;
		case FbxNodeAttribute::eCameraStereo:
		{
			int dummyint1 = 1;
		}
		break;
		case FbxNodeAttribute::eCameraSwitcher:
		{
			int dummyint2 = 1;
		}
		break;
		default:
			break;
		}
	}

	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for (int i = 0; i < childNodeNum; i++)
	{
		FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
		if (pChild) {
			CreateFBXCameraReq(pChild);
		}
	}

}



int CModel::CreateFBXMeshReq( FbxNode* pNode)
{
	if (!pNode) {
		return 0;
	}

	ChaMatrix curmeshmat;
	curmeshmat.SetIdentity();
	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if ( pAttrib ) {
		FbxNodeAttribute::EType type = (FbxNodeAttribute::EType)(pAttrib->GetAttributeType());
        //FbxGeometryConverter lConverter(pNode->GetFbxManager());

		//char mes[256];

		int shapecnt;
		CMQOObject* newobj = 0;

		switch ( type )
		{
			//case FbxNodeAttribute::eLODGroup:
			//{
			//	FbxLODGroup* plod;// = (FbxLODGroup*)pAttrib;
			//	plod = pNode->GetLodGroup();
			//	if (plod) {
			//		char lodname[256] = { 0 };
			//		strcpy_s(lodname, 256, plod->GetName());
			//		int lodnum = plod->GetNumDisplayLevels();
			//		int thnum = plod->GetNumThresholds();
			//		int lodno;
			//		//for (lodno = 0; lodno < lodnum; lodno++) {
			//		FbxDouble lodthreshvalue1 = 0.0;
			//		//plod->GetThreshold(lodno, lodthreshvalue1);
			//		plod->GetThreshold(0, lodthreshvalue1);
			//		if (lodthreshvalue1 != FbxDouble(0.0)) {
			//			int dbgflag1 = 1;
			//		}
			//		//}

			//		//for (lodno = 0; lodno < thnum; lodno++) {
			//		FbxDouble lodthreshvalue2 = 0.0;
			//		//plod->GetThreshold(lodno, lodthreshvalue2);
			//		plod->GetThreshold(0, lodthreshvalue2);
			//		if (lodthreshvalue2 != FbxDouble(0.0)) {
			//			int dbgflag2 = 1;
			//		}
			//		//}
			//	}
			//}
			//	break;
			case FbxNodeAttribute::eMesh:
				//{

				//	FbxLODGroup* plod;// = (FbxLODGroup*)pAttrib;
				//	plod = pNode->GetLodGroup();
				//	if (plod) {
				//		char lodname[256] = { 0 };
				//		strcpy_s(lodname, 256, plod->GetName());
				//		int lodnum = plod->GetNumDisplayLevels();
				//		int thnum = plod->GetNumThresholds();
				//		int lodno;
				//		//for (lodno = 0; lodno < lodnum; lodno++) {
				//			FbxDouble lodthreshvalue1 = 0.0;
				//			//plod->GetThreshold(lodno, lodthreshvalue1);
				//			plod->GetThreshold(0, lodthreshvalue1);
				//			if (lodthreshvalue1 != FbxDouble(0.0)) {
				//				int dbgflag1 = 1;
				//			}
				//		//}

				//		//for (lodno = 0; lodno < thnum; lodno++) {
				//			FbxDouble lodthreshvalue2 = 0.0;
				//			//plod->GetThreshold(lodno, lodthreshvalue2);
				//			plod->GetThreshold(0, lodthreshvalue2);
				//			if (lodthreshvalue2 != FbxDouble(0.0)) {
				//				int dbgflag2 = 1;
				//			}
				//		//}
				//	}

				//}

				newobj = GetFBXMesh(pNode, pAttrib);     // メッシュを作成
				if (newobj){
					shapecnt = pNode->GetMesh()->GetShapeCount();
					if (shapecnt > 0){
						//sprintf_s(mes, 256, "%s, shapecnt %d", pNode->GetName(), shapecnt);
						//MessageBoxA(NULL, mes, "check", MB_OK);


						//###########
						//BlendShape
						//###########
						int resultshape = GetFBXShape(pNode->GetMesh(), newobj);
						if (resultshape) {
							_ASSERT(0);
							char errormsg[1024] = { 0L };
							sprintf_s(errormsg, 1024, "%s : Invalid Shapes is found. Exit this app.", pNode->GetName());
							MessageBoxA(NULL, errormsg, "Error", MB_OK | MB_ICONERROR);

							abort();//!!!!!!!!!!!
							
							return 0;
						}
					}
				}
				break;
//			case FbxNodeAttribute::eNURB:
//			case FbxNodeAttribute::eNURBS_SURFACE:
//                lConverter.TriangulateInPlace(pNode);
//				GetFBXMesh( pAttrib, pNode->GetName() );     // メッシュを作成
//				break;
			case FbxNodeAttribute::eNull:
			//{
			//	FbxLODGroup* plod;// = (FbxLODGroup*)pAttrib;
			//	plod = pNode->GetLodGroup();
			//	if (plod) {
			//		char lodname[256] = { 0 };
			//		strcpy_s(lodname, 256, plod->GetName());
			//		int lodnum = plod->GetNumDisplayLevels();
			//		int thnum = plod->GetNumThresholds();
			//		int lodno;
			//		//for (lodno = 0; lodno < lodnum; lodno++) {
			//		FbxDouble lodthreshvalue1 = 0.0;
			//		//plod->GetThreshold(lodno, lodthreshvalue1);
			//		plod->GetThreshold(0, lodthreshvalue1);
			//		if (lodthreshvalue1 != FbxDouble(0.0)) {
			//			int dbgflag1 = 1;
			//		}
			//		//}

			//		//for (lodno = 0; lodno < thnum; lodno++) {
			//		FbxDouble lodthreshvalue2 = 0.0;
			//		//plod->GetThreshold(lodno, lodthreshvalue2);
			//		plod->GetThreshold(0, lodthreshvalue2);
			//		if (lodthreshvalue2 != FbxDouble(0.0)) {
			//			int dbgflag2 = 1;
			//		}
			//		//}
			//	}
			//}
				break;
			case FbxNodeAttribute::eSkeleton:
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
		if (pChild) {
			CreateFBXMeshReq(pChild);
		}
	}

	return 0;
}



ChaMatrix CModel::CalcLocalMeshMat(FbxNode* pNode, double srctime)
{

	//現在 eMeshにCBone*メンバが無い　eMeshの親にeMeshがある場合などに対応するために　とりあえず　CBone*を介さないで計算するためのCalcMeshMatReq()


	//###########################################################################
	//CBone::SaveFbxNodePosture()とCBone::CalcLocalNodePosture()と内容を合わせる
	//###########################################################################


	ChaMatrix retmat;
	retmat.SetIdentity();

	if (!pNode) {
		_ASSERT(0);
		retmat.SetIdentity();
		return retmat;
	}

	//#########################################################################################
	//2023/06/28
	//Enullノードの子供のCancel2Modeで正常に再生可能なカメラアニメ保存読み込みで検証したところ
	//Lcl*.Get()を保存するとアニメが変質したが　EvaluateLocal*を保存すると変質しなかった
	//#########################################################################################
	//FbxDouble3 fbxLclPos = pNode->LclTranslation.Get();
	//FbxDouble3 fbxLclRot = pNode->LclRotation.Get();
	//FbxDouble3 fbxLclScl = pNode->LclScaling.Get();

	FbxTime fbxtime;
	//fbxtime.SetSecondDouble(0.0);
	fbxtime.SetSecondDouble(srctime / 30.0);

	bool fbxRotationActive = pNode->GetRotationActive();//lclrot入りかどうかはGetRotationActive()で判断

	FbxDouble3 fbxLclPos = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot, true, true);//Bone.cpp SaveFbxNodePosture()と合わせる
	FbxDouble3 fbxLclRot = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
	FbxDouble3 fbxLclScl = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot, true, true);

	FbxVector4 fbxRotOff = pNode->GetRotationOffset(FbxNode::eSourcePivot);
	FbxVector4 fbxRotPiv = pNode->GetRotationPivot(FbxNode::eSourcePivot);
	FbxVector4 fbxPreRot = pNode->GetPreRotation(FbxNode::eSourcePivot);
	FbxVector4 fbxPostRot = pNode->GetPostRotation(FbxNode::eSourcePivot);
	FbxVector4 fbxSclOff = pNode->GetScalingOffset(FbxNode::eSourcePivot);
	FbxVector4 fbxSclPiv = pNode->GetScalingPivot(FbxNode::eSourcePivot);
	bool fbxrotationActive = pNode->GetRotationActive();
	EFbxRotationOrder rotationorder;
	pNode->GetRotationOrder(FbxNode::eSourcePivot, rotationorder);
	//InheritType = pNode->InheritType.Get();


	ChaMatrix fbxT, fbxRoff, fbxRp, fbxRpinv, fbxSoff, fbxSp, fbxS, fbxSpinv;
	fbxT.SetIdentity();
	fbxRoff.SetIdentity();
	fbxRp.SetIdentity();
	fbxRpinv.SetIdentity();
	fbxSoff.SetIdentity();
	fbxSp.SetIdentity();
	fbxS.SetIdentity();
	fbxSpinv.SetIdentity();

	fbxT.SetTranslation(ChaVector3(fbxLclPos));//##### at fbxtime
	fbxRoff.SetTranslation(ChaVector3(fbxRotOff));
	fbxRp.SetTranslation(ChaVector3(fbxRotPiv));

	CQuaternion rotQ1, rotQ2, preQ, lclQ, postQ;
	//preQ.SetRotation(rotationorder, 0, ChaVector3(fbxPreRot));
	//lclQ.SetRotation(rotationorder, 0, ChaVector3(fbxLclRot));//##### at fbxtime
	//postQ.SetRotation(rotationorder, 0, ChaVector3(fbxPostRot));

	//##############################################################################################################################
	//2023/06/28
	//Mesh用の変換行列計算(CModel::CalcMeshMatReq())時には　prerot, postrotlclrotはXYZ順　lclrotはrotationorder順でうまくいくようだ
	//		ただし　カメラの子供のメッシュについては　うまくいっていない(parentがeCameraの場合の計算に対応していない)
	// 
	//TheHunt Street1 Camera_1の　HUDの位置向き　壁の位置向き　　　TheHunt City1 Camera_1の子供のArmsのなどで検証
	// 読み書き読み書き読みで変わらないことを確認(カメラの子供のスキンメッシュは未対応. カメラの子供のスキンメッシュについては形は崩れない程度.)
	//##############################################################################################################################
	preQ.SetRotationXYZ(0, ChaVector3(fbxPreRot));//!!!!!! prerotはXYZ
	lclQ.SetRotation(rotationorder, 0, ChaVector3(fbxLclRot));//##### at fbxtime
	postQ.SetRotationXYZ(0, ChaVector3(fbxPostRot));//!!!!!! postrotはXYZ
	rotQ1 = preQ * lclQ * postQ;
	rotQ2 = preQ * postQ;


	fbxRpinv = ChaMatrixInv(fbxRp);
	fbxSoff.SetTranslation(ChaVector3(fbxSclOff));
	fbxSp.SetTranslation(ChaVector3(fbxSclPiv));
	fbxS.SetScale(ChaVector3(fbxLclScl));//##### at fbxtime
	fbxSpinv = ChaMatrixInv(fbxSp);


	ChaMatrix localnodemat, localnodeanimmat;
	localnodeanimmat = fbxSpinv * fbxS * fbxSp * fbxSoff * fbxRpinv * rotQ1.MakeRotMatX() * fbxRp * fbxRoff * fbxT;//2023/06/23
	//0フレームアニメ無し : fbxR無し
	localnodemat = fbxSpinv * fbxS * fbxSp * fbxSoff * fbxRpinv * rotQ2.MakeRotMatX() * fbxRp * fbxRoff * fbxT;


	//#################################################################################################################################
	//eSkeletonを特別扱いするのはbindpose計算時. カレントポーズ計算時には通常通りGetRotationActiveに依存させる
	//#################################################################################################################################
	if (fbxRotationActive) {
		retmat = localnodeanimmat;
	}
	else {
		retmat = localnodemat;
	}


	return retmat;

}

ChaMatrix CModel::CalcGlobalMeshMat(FbxNode* pNode, double srctime)
{
	ChaMatrix retmat;
	retmat.SetIdentity();
	if (!pNode) {
		_ASSERT(0);
		retmat.SetIdentity();
		return retmat;
	}

	//現在 eMeshにCBone*メンバが無い　eMeshの親にeMeshがある場合などに対応するために　とりあえず　CBone*を介さないで計算するためのCalcMeshMatReq()
	CalcMeshMatReq(pNode, srctime, &retmat);
	return retmat;
}

void CModel::CalcMeshMatReq(FbxNode* pNode, double srctime, ChaMatrix* pmeshmat)
{


	//現在 eMeshにCBone*メンバが無い　eMeshの親にeMeshがある場合などに対応するために　とりあえず　CBone*を介さないで計算するためのCalcMeshMatReq()

	//###########################################################################
	//CBone::SaveFbxNodePosture()とCBone::CalcLocalNodePosture()と内容を合わせる
	//###########################################################################

	if (!pNode || !pmeshmat) {
		_ASSERT(0);
		return;
	}

	ChaMatrix localnodeanimmat;
	localnodeanimmat.SetIdentity();
	localnodeanimmat = CalcLocalMeshMat(pNode, srctime);

	*pmeshmat = *pmeshmat * localnodeanimmat;//親方向へ辿りながら実行するので　pmeshmatにとってlocalnodeanimmatはparent

	if (pNode->GetParent()) {
		CalcMeshMatReq(pNode->GetParent(), srctime, pmeshmat);
	}
}



CMQOObject* CModel::GetFBXMesh(FbxNode* pNode, FbxNodeAttribute *pAttrib)
{
	if (!pNode || !pAttrib) {
		_ASSERT(0);
		return 0;
	}
	if (!pNode->GetName()) {
		_ASSERT(0);
	}

	FbxMesh *pMesh = (FbxMesh*)pAttrib;
	if (strcmp("RootNode", pAttrib->GetName()) == 0){
		_ASSERT(0);
		return 0;
	}


	char nodename[256] = { 0 };
	strcpy_s(nodename, 256, pNode->GetName());
	WCHAR wnodename[256] = L"none for debug";
	ZeroMemory(wnodename, sizeof(WCHAR) * 256);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, nodename, 256, wnodename, 256);//256長の配列にコピーしてから変換する
	char meshname[256] = { 0 };
	strcpy_s(meshname, 256, pMesh->GetName());



	ChaMatrix globalmeshmat;//頂点用
	ChaMatrix globalnormalmat;//法線用
	globalmeshmat.SetIdentity();
	globalnormalmat.SetIdentity();
	{
		globalmeshmat.SetIdentity();

		//FbxTime fbxtime0;
		//fbxtime0.SetSecondDouble(0.0);
		//FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(fbxtime0, FbxNode::eSourcePivot, true, true);
		////FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(fbxtime0, FbxNode::eSourcePivot);
		//globalmeshmat = ChaMatrixFromFbxAMatrix(lGlobalPosition);


		//CalcMeshMatReq(pNode, 0.0, &globalmeshmat);

		//2023/07/05
		globalmeshmat = CalcGlobalMeshMat(pNode, 0.0);

		//2023/07/04
		//GetFbxMeshでのmeshmatはmesh自体のローカル変換だけにしておいて　アニメ可能なparentのeNullの変換をSetShaderConst()で合成する
		//localmeshmat = CalcLocalMeshMat(pNode);




		////for debug
		//{
		//	char strdbg[1024] = { 0 };
		//	WCHAR wstrdbg[1024] = { 0L };
		//	sprintf_s(strdbg, 1024, "MeshMat : (%s)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n",
		//		pNode->GetName(),
		//		globalmeshmat.data[MATI_11], globalmeshmat.data[MATI_12], globalmeshmat.data[MATI_13], globalmeshmat.data[MATI_14],
		//		globalmeshmat.data[MATI_21], globalmeshmat.data[MATI_22], globalmeshmat.data[MATI_23], globalmeshmat.data[MATI_24],
		//		globalmeshmat.data[MATI_31], globalmeshmat.data[MATI_32], globalmeshmat.data[MATI_33], globalmeshmat.data[MATI_34],
		//		globalmeshmat.data[MATI_41], globalmeshmat.data[MATI_42], globalmeshmat.data[MATI_43], globalmeshmat.data[MATI_44]
		//	);
		//	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strdbg, 1024, wstrdbg, 1024);
		//	DbgOut(wstrdbg);
		//}

		globalnormalmat = globalmeshmat;
		globalnormalmat.SetTranslation(ChaVector3(0.0f, 0.0f, 0.0f));
		globalnormalmat.data[MATI_44] = 0.0f;
	}
	


	CMQOObject* newobj = new CMQOObject();
	_ASSERT( newobj );
	if (!newobj) {
		_ASSERT(0);
		return 0;
	}
	newobj->SetObjFrom(OBJFROM_FBX);
	newobj->SetName(nodename);
	newobj->SetFbxNode(pNode);
	m_object[ newobj->GetObjectNo() ] = newobj;
	m_node2mqoobj[pNode] = newobj;





	FBXOBJ* fbxobj = (FBXOBJ*)malloc(sizeof(FBXOBJ));
	if (!fbxobj) {
		_ASSERT(0);
		return 0;
	}
	fbxobj->node = pNode;
	fbxobj->mesh = pMesh;
	m_fbxobj[newobj] = fbxobj;

//shape
//	int morphnum = pMesh->GetShapeCount();
//	newobj->m_morphnum = morphnum;

//マテリアル
	int materialNum_ = 1;


	//FbxNode* node = pMesh->GetNode();
	//#######################################################################################################
	//2023/12/04 mesh->GetNode()に対してGetMaterial(i)を呼んでマテリアル情報を取得しようとすると不正な情報が入っていた
	//マテリアル名に本来のマテリアル情報とは異なるtypo_2という名前, テクスチャにtype_2.pngが入っていた
	//mesh->GetNode()はマテリアル情報のためには使わないことに.
	//########################################################################################################
	//if ( node != 0 ) {
		// マテリアルの数
		materialNum_ = pNode->GetMaterialCount();

		//for dbginfo
		//if (materialNum_ != 1) {
			//char strinfo[MAX_PATH] = { 0 };
			//sprintf_s(strinfo, MAX_PATH, "mesh %s, materialNum_ %d", pNode->GetName(), materialNum_);
			//::MessageBoxA(NULL, strinfo, "check!!!", MB_OK);
		//}
		DbgOut(L"mesh %s, materialNum_ %d\r\n", wnodename, materialNum_);



		// マテリアル情報を取得
		//for( int i = 0; i < materialNum_; ++i ) {
		for (int i = 0; i < materialNum_; i++) {
			FbxSurfaceMaterial* material = pNode->GetMaterial( i );
			if ( material != 0 ) {
				char materialname[256] = { 0 };
				strcpy_s(materialname, 256, material->GetName());

				//if ((strstr(meshname, "plane__35") != 0) || (strstr(materialname, "plane__35") != 0) || (strstr(nodename, "plane__35") != 0)) {
				//	int dbgflag2 = 1;
				//}
				//if ((strstr(meshname, "plane__35") != 0) || (strstr(materialname, "Caution") != 0) || (strstr(nodename, "Neon_Other_4") != 0)) {
				//	int dbgflag2 = 1;
				//}

				CMQOMaterial* currentmaterial = GetMQOMaterialByName(materialname);//既に存在するかどうかチェック
				if (!currentmaterial) {
					//未登録テクスチャの登録処理
					currentmaterial = new CMQOMaterial();
					SetMQOMaterial(currentmaterial, material);//内容をセット
					SetMQOMaterial(materialname, currentmaterial);//表にセット
				}
				else {
					//登録済　何もしない
				}

				//オブジェクト毎のマテリアル読込順序を保存して　書き出し時に使うための処理
				//CMQOMaterial* onloadmaterial = newobj->GetOnLoadMaterialByName(materialname);
				//if (!onloadmaterial) {
					newobj->AddOnLoadMaterial(currentmaterial);
				//}

				//const char* texname = newmqomat->GetTex();
				//const char* nodename = node->GetName();
				//_ASSERT(0);
			}
			else {
				_ASSERT(0);
			}
		}
	//}


//頂点
	int PolygonNum       = pMesh->GetPolygonCount();
	int PolygonVertexNum = pMesh->GetPolygonVertexCount();
	int *IndexAry        = pMesh->GetPolygonVertices();

	int controlNum = pMesh->GetControlPointsCount();   // 頂点数
	FbxVector4* src = pMesh->GetControlPoints();    // 頂点座標配列

	DbgOut(L"LDCheck : GetFBXMesh : nodename %s, controlnum %d, polygonnum %d, polygonvertexnum %d\r\n", wnodename, controlNum, PolygonNum, PolygonVertexNum);

	// コピー
	newobj->SetVertex( controlNum );
	newobj->SetPointBuf( (ChaVector3*)malloc( sizeof( ChaVector3 ) * controlNum ) );
	newobj->SetMeshMat(globalmeshmat);
	//for ( int i = 0; i < controlNum; ++i ) {
	for (int i = 0; i < controlNum; i++) {
		ChaVector3 tmpp;
		tmpp.x = (float)src[ i ][ 0 ];
		tmpp.y = (float)src[ i ][ 1 ];
		tmpp.z = (float)src[ i ][ 2 ];
		//curctrl->w = (float)src[ i ][ 3 ];


		//eNullのtransformによるメッシュ頂点の変換
		//アニメーションカーブが無い場合には　CBone::GetFbxAnimでworldmatをidentityにしないと副作用が出る
		ChaVector3* curctrl = newobj->GetPointBuf() + i;
		ChaVector3TransformCoord(curctrl, &tmpp, &globalmeshmat);


//DbgOut( L"GetFBXMesh : ctrl %d, (%f, %f, %f)\r\n",
//	i, curctrl->x, curctrl->y, curctrl->z );

	}

	//const FbxLayerElementMaterial* pPolygonMaterials = NULL;
	//FbxGeometryElement::EMappingMode materialmappingMode = FbxGeometryElement::eAllSame;
	//pPolygonMaterials = pMesh->GetElementMaterial();
	//if (pPolygonMaterials != NULL) {
	//	materialmappingMode = pPolygonMaterials->GetMappingMode();
	//}

	newobj->SetFace(PolygonNum);
	newobj->SetFaceBuf(new CMQOFace[PolygonNum]);


	//１メッシュに複数マテリアルが設定してある場合においても
	//通常はマテリアル番号オブジェクト(FbxGeometryElementMaterial)は１つ
	int materialnum = pMesh->GetElementMaterialCount();
	int materialindex;
	if (materialnum >= 2) {
		int dbgflag1 = 1;
	}
	for (materialindex = 0; materialindex < materialnum; materialindex++) {
		FbxGeometryElementMaterial* pPolygonMaterials = pMesh->GetElementMaterial(materialindex);
		if (pPolygonMaterials != NULL) {
			FbxGeometryElement::EMappingMode materialmappingMode = pPolygonMaterials->GetMappingMode();

			for (int p = 0; p < PolygonNum; p++) {
				int IndexNumInPolygon = pMesh->GetPolygonSize(p);  // p番目のポリゴンの頂点数
				if ((IndexNumInPolygon != 3) && (IndexNumInPolygon != 4)) {
					_ASSERT(0);
					return 0;
				}

				CMQOFace* curface = newobj->GetFaceBuf() + p;
				curface->SetPointNum(IndexNumInPolygon);

				curface->SetFaceNo(p);
				curface->SetBoneType(MIKOBONE_NONE);

				int lookupIndex = 0;
				if (pPolygonMaterials) {
					switch (materialmappingMode) {
					case FbxGeometryElement::eByPolygon:
						lookupIndex = p;//triangleNo.
						break;
					case FbxGeometryElement::eAllSame:
						lookupIndex = 0;
						break;
					default:
						lookupIndex = 0;
						//lookupIndex = p;
						break;
					}

					int materialIndex = pPolygonMaterials->mIndexArray->GetAt(lookupIndex);//Mesh単位のマテリアルへのインデックス
					FbxSurfaceMaterial* material = pNode->GetMaterial(materialIndex);
					if (material != 0) {
						char materialname[256] = { 0 };
						strcpy_s(materialname, 256, material->GetName());
						CMQOMaterial* currentmaterial = GetMQOMaterialByName(materialname);//既に存在するかどうかチェック
						if (!currentmaterial) {
							_ASSERT(0);
							lookupIndex = 0;
							curface->SetMaterialNo(0);
						}
						else {
							int curmaterialno = currentmaterial->GetMaterialNo();
							curface->SetMaterialNo(curmaterialno);
						}
					}
					//else {
					//	_ASSERT(0);
					//	lookupIndex = 0;
					//	curface->SetMaterialNo(0);
					//}
				}
				else {
					lookupIndex = 0;
					curface->SetMaterialNo(0);
				}
				for (int n = 0; n < IndexNumInPolygon; n++) {
					// ポリゴンpを構成するn番目の頂点のインデックス番号
					int IndexNumber = pMesh->GetPolygonVertex(p, n);
					curface->SetIndex(n, IndexNumber);
				}
			}
		}
		else {	
			//マテリアル番号情報が無い場合
			_ASSERT(0);

			for (int p = 0; p < PolygonNum; p++) {
				int IndexNumInPolygon = pMesh->GetPolygonSize(p);  // p番目のポリゴンの頂点数
				if ((IndexNumInPolygon != 3) && (IndexNumInPolygon != 4)) {
					_ASSERT(0);
					return 0;
				}

				CMQOFace* curface = newobj->GetFaceBuf() + p;
				curface->SetPointNum(IndexNumInPolygon);

				curface->SetFaceNo(p);
				curface->SetBoneType(MIKOBONE_NONE);

				curface->SetMaterialNo(0);//!!!!!!!!!!!!!!!!!!

				for (int n = 0; n < IndexNumInPolygon; n++) {
					// ポリゴンpを構成するn番目の頂点のインデックス番号
					int IndexNumber = pMesh->GetPolygonVertex(p, n);
					curface->SetIndex(n, IndexNumber);
				}
			}
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
	bool quadwarningflag = false;

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


//DbgOut( L"GetFBXMesh : %s : normalNum %d : indexNum %d\r\n", wnodename, normalNum, indexNum );

		// マッピングモード・リファレンスモード取得
		FbxLayerElement::EMappingMode mappingMode = normalElem->GetMappingMode();
		FbxLayerElement::EReferenceMode refMode = normalElem->GetReferenceMode();

		newobj->SetNormalMappingMode(0);


		int facenum0 = pMesh->GetPolygonCount();
		newobj->SetNormalLeng( facenum0 * 3 );
		newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * (facenum0 * 3) ) );

		int faceno0;
		for (faceno0 = 0; faceno0 < facenum0; faceno0++) {
			int vnum = pMesh->GetPolygonSize(faceno0);
			int vno;

			if (vnum != 3) {
				//_ASSERT(0);
				if (quadwarningflag == false) {
					WCHAR message[1024] = { 0L };
					swprintf_s(message, 1024, 
						L"[node:%s] Only support triangles. Normal of faces will be not correct.",
						wnodename
					);
					::MessageBox(NULL, message, L"Not Triangles Warning.", MB_OK | MB_ICONWARNING);
				}
				quadwarningflag = true;

				vnum = 3;
			}

			for (vno = 0; vno < vnum; vno++) {
				FbxVector4 srcnormal;
				pMesh->GetPolygonVertexNormal(faceno0, vno, srcnormal);
				
				ChaVector3* curn = newobj->GetNormal() + faceno0 * 3 + vno;
				ChaVector3 tmpn;
				tmpn.SetParams(srcnormal);
				ChaVector3TransformCoord(curn, &tmpn, &globalnormalmat);
				ChaVector3Normalize(curn, curn);
			}
		}


//		if ( mappingMode == FbxLayerElement::eByPolygonVertex ) {
////DbgOut( L"GetFBXMesh : %s : mapping eByPolygonVertex\r\n", wnodename );
//
//			newobj->SetNormalMappingMode(0);
//
//			if ( refMode == FbxLayerElement::eDirect ) {
////DbgOut( L"GetFBXMesh : %s : ref eDirect\r\n", wnodename );
//
//				newobj->SetNormalLeng( normalNum );
//				newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * normalNum ) );
//
//				// 直接取得
//			  //for ( int i = 0; i < normalNum; ++i ) {
//				for (int i2 = 0; i2 < normalNum; i2++) {
//					ChaVector3* curn = newobj->GetNormal() + i2;
//					ChaVector3 tmpn;
//					tmpn.x = (float)normalElem->GetDirectArray().GetAt( i2 )[ 0 ];
//					tmpn.y = (float)normalElem->GetDirectArray().GetAt( i2 )[ 1 ];
//					tmpn.z = (float)normalElem->GetDirectArray().GetAt( i2 )[ 2 ];
//
//					//*curn = tmpn;
//					ChaVector3TransformCoord(curn, &tmpn, &globalnormalmat);
//					ChaVector3Normalize(curn, curn);
//				}
//		   }else if ( refMode == FbxLayerElement::eIndexToDirect ){
////DbgOut( L"GetFBXMesh : %s : ref eIndexToDirect\r\n", wnodename );
//
//				newobj->SetNormalLeng( indexNum );
//				newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * indexNum ) );
//
//				int lIndex;
//				for( lIndex = 0; lIndex < indexNum; lIndex++ ){
//
//					int lNormalIndex = normalElem->GetIndexArray().GetAt(lIndex);
//
//				    FbxVector4 lCurrentNormal;
//					lCurrentNormal = normalElem->GetDirectArray().GetAt(lNormalIndex);
//					ChaVector3* curn = newobj->GetNormal() + lIndex;
//					ChaVector3 tmpn;
//					tmpn.x = static_cast<float>(lCurrentNormal[0]);
//					tmpn.y = static_cast<float>(lCurrentNormal[1]);
//					tmpn.z = static_cast<float>(lCurrentNormal[2]);
//
//					//*curn = tmpn;
//					ChaVector3TransformCoord(curn, &tmpn, &globalnormalmat);
//					ChaVector3Normalize(curn, curn);
//				}
//		   }
//		} else if ( mappingMode == FbxLayerElement::eByControlPoint ) {
////DbgOut( L"GetFBXMesh : %s : mapping eByControlPoint\r\n", wnodename );
//
//			newobj->SetNormalMappingMode(1);
//
//
//		   if ( refMode == FbxLayerElement::eDirect ) {
////DbgOut( L"GetFBXMesh : %s : ref eDirect\r\n", wnodename );
//
//				newobj->SetNormalLeng( normalNum );
//				newobj->SetNormal( (ChaVector3*)malloc( sizeof( ChaVector3 ) * normalNum ) );
//
//				// 直接取得
//				//for ( int i = 0; i < normalNum; ++i ) {
//				for (int i2 = 0; i2 < normalNum; i2++) {
//					ChaVector3* curn = newobj->GetNormal() + i2;
//					ChaVector3 tmpn;
//					tmpn.x = (float)normalElem->GetDirectArray().GetAt( i2 )[ 0 ];
//					tmpn.y = (float)normalElem->GetDirectArray().GetAt( i2 )[ 1 ];
//					tmpn.z = (float)normalElem->GetDirectArray().GetAt( i2 )[ 2 ];
//
//					//*curn = tmpn;
//					ChaVector3TransformCoord(curn, &tmpn, &globalnormalmat);
//					ChaVector3Normalize(curn, curn);
//				}
//		   }else{
////DbgOut( L"GetFBXMesh : %s : ref %d\r\n", wnodename, refMode );
//			   _ASSERT(0);
//		   }
//		} else {
//			_ASSERT( 0 );
//		}

		break;
	}

//UV
	int uvlayercount = pMesh->GetUVLayerCount();
	int layerno;
	
	int getuvnum = 0;
	newobj->SetUVNum(0);

	for (layerno = 0; layerno < uvlayercount; layerno++) {
		FbxGeometryElementUV* lUVDiffuseElement = pMesh->GetElementUV(layerno);

		if (!lUVDiffuseElement) {
			continue;
		}

		if (getuvnum < 2) {
			int facenum1 = pMesh->GetPolygonCount();
			if (getuvnum == 0) {
				newobj->SetUVLeng(facenum1 * 3);
				newobj->SetUVBuf((ChaVector2*)malloc(sizeof(ChaVector2)* (facenum1 * 3)));
			}
			else {
				newobj->SetUVBuf1((ChaVector2*)malloc(sizeof(ChaVector2)* (facenum1 * 3)));
			}

			int faceno1;
			for (faceno1 = 0; faceno1 < facenum1; faceno1++) {
				int vnum = pMesh->GetPolygonSize(faceno1);
				int vno;
				for (vno = 0; vno < vnum; vno++) {
					FbxVector2 srcuv;
					bool bunmapped = false;
					if (lUVDiffuseElement) {
						pMesh->GetPolygonVertexUV(faceno1, vno, lUVDiffuseElement->GetName(), srcuv, bunmapped);
						if (getuvnum == 0) {
							ChaVector2* curuv = newobj->GetUVBuf() + faceno1 * 3 + vno;
							curuv->SetParams((float)srcuv[0], 1.0f - (float)srcuv[1]);//!!! 1.0f - uv.y
						}
						else {
							ChaVector2* curuv1 = newobj->GetUVBuf1() + faceno1 * 3 + vno;
							curuv1->SetParams((float)srcuv[0], 1.0f - (float)srcuv[1]);//!!! 1.0f - uv.y
						}
					}
				}
			}
			getuvnum++;
			newobj->SetUVNum(getuvnum);
		}
	}

	return newobj;
}

int CModel::SetMQOMaterial( CMQOMaterial* newmqomat, FbxSurfaceMaterial* pMaterial )
{
	if (!newmqomat || !pMaterial) {
		_ASSERT(0);
		return 1;
	}

	char tmpname[512];
	strcpy_s(tmpname, 512, pMaterial->GetName());
	newmqomat->SetName(tmpname);


	//::MessageBoxA(NULL, tmpname, "MaterialName", MB_OK);


	char* emitex = 0;
    const FbxDouble3 lEmissive = FbxGetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &emitex);
	if( emitex ){
		DbgOut( L"SetMQOMaterial : emitexture find\r\n" );
	}
	ChaVector3 tmpemi;

	tmpemi.x = (float)lEmissive[0] * g_EmissiveFactorAtLoading;
	tmpemi.y = (float)lEmissive[1] * g_EmissiveFactorAtLoading;
	tmpemi.z = (float)lEmissive[2] * g_EmissiveFactorAtLoading;
	newmqomat->SetEmi3F( tmpemi );


	char* ambtex = 0;
	const FbxDouble3 lAmbient = FbxGetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &ambtex);
	if( ambtex ){
		DbgOut( L"SetMQOMaterial : ambtexture find\r\n" );
	}
	ChaVector3 tmpamb;
	tmpamb.x = (float)lAmbient[0] * g_AmbientFactorAtLoading;
	tmpamb.y = (float)lAmbient[1] * g_AmbientFactorAtLoading;
	tmpamb.z = (float)lAmbient[2] * g_AmbientFactorAtLoading;
	newmqomat->SetAmb3F( tmpamb );

	char* diffusetex = 0;
    const FbxDouble3 lDiffuse = FbxGetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &diffusetex);
	if( diffusetex ){
		//strcpy_s( newmqomat->tex, 256, diffusetex );
		DbgOut( L"SetMQOMaterial : diffusetexture find\r\n" );
	}
	ChaVector4 tmpdif;
	tmpdif.x = (float)lDiffuse[0] * g_DiffuseFactorAtLoading;
	tmpdif.y = (float)lDiffuse[1] * g_DiffuseFactorAtLoading;
	tmpdif.z = (float)lDiffuse[2] * g_DiffuseFactorAtLoading;
	tmpdif.w = 1.0f;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!　alpha値はtexture依存　透過オンはTransparencyFactor!=0.0で
	newmqomat->SetDif4F( tmpdif );


	char* spctex = 0;
    const FbxDouble3 lSpecular = FbxGetMaterialProperty(pMaterial,
        FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &spctex);
	if( spctex ){
		DbgOut( L"SetMQOMaterial : spctexture find\r\n" );
	}
	ChaVector3 tmpspc;
	tmpspc.x = (float)lSpecular[0] * g_SpecularFactorAtLoading;
	tmpspc.y = (float)lSpecular[1] * g_SpecularFactorAtLoading;
	tmpspc.z = (float)lSpecular[2] * g_SpecularFactorAtLoading;
	newmqomat->SetSpc3F( tmpspc );

    //FbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);//<-- link error : AdditiveIKLib.lib外だから？　
	//char* shininesstex = 0;
	//FbxProperty lShininessProperty = FbxGetMaterialProperty(pMaterial,
	//	FbxSurfaceMaterial::sShininess, FbxSurfaceMaterial::sShininessFactor, &shininesstex);//<-- sSnininessFactorは無い. sShininessは有る.
 //   if (lShininessProperty.IsValid())
 //   {
 //       double lShininess = lShininessProperty.Get<FbxDouble>();
 //       newmqomat->SetPower( static_cast<float>(lShininess) );
 //   }
	FbxDouble Shininess = FbxGetMaterialShininessProperty(pMaterial);
	newmqomat->SetPower(static_cast<float>(Shininess));



	//char strtransparent[256] = { 0 };
	//if (pMaterial->sTransparencyFactor) {
	//	strcpy_s(strtransparent, pMaterial->sTransparencyFactor);//"TransparencyFactor"という文字列
	//	FbxGetMaterialProperty(pMaterial,
	//		FbxSurfaceMaterial::sTransparencyFactor, FbxSurfaceMaterial::sTransparencyFactor, &transparencyfactor);
	//}
	//}
	//char strtransparentcolor[256] = { 0 };
	//if (pMaterial->sTransparentColor) {
	//	strcpy_s(strtransparentcolor, pMaterial->sTransparentColor);//"TransparentColor"という文字列
	//	::MessageBoxA(NULL, strtransparentcolor, "check transparent", MB_OK);
	//}
	// Opacity.
	//FbxDouble opacity = ((FbxSurfacePhong*)surfaceMaterial)->TransparencyFactor;//ちゃんとセットされる保証なし



	//2023/09/24 VRoidの服の裾(すそ)に透過の印を付ける
	//次に続くtexture読み込み部分でも　_13.png, _15.pngについてもSetTransparent(1)
	FbxDouble transparent = ((FbxSurfacePhong*)pMaterial)->TransparencyFactor;//ちゃんとセットされる保証なし

	bool oldtransparent = false;
	if (m_pscene) {
		FbxDocumentInfo* sceneinfo = m_pscene->GetSceneInfo();
		if (sceneinfo) {
			FbxString chkauthor = "OchakkoLab";
			if (sceneinfo->mAuthor == chkauthor) {
				FbxString currentrev1 = "rev. 3.1";
				FbxString currentrev2 = "rev. 3.2";
				FbxString currentrev3 = "rev. 3.3";
				FbxString currentrev4 = "rev. 3.4";
				FbxString currentrev5 = "rev. 3.5";
				FbxString currentrev6 = "rev. 3.6";
				FbxString currentrev7 = "rev. 3.7";
				FbxString currentrev8 = "rev. 3.8";
				if ((sceneinfo->mRevision != currentrev1) &&
					(sceneinfo->mRevision != currentrev2) &&
					(sceneinfo->mRevision != currentrev3) &&
					(sceneinfo->mRevision != currentrev4) &&
					(sceneinfo->mRevision != currentrev5) &&
					(sceneinfo->mRevision != currentrev6) &&
					(sceneinfo->mRevision != currentrev7) &&
					(sceneinfo->mRevision != currentrev8)
					) {
					oldtransparent = true;//!!!!!!!!!!!!!!!!!!!!
				}
			}
		}
	}

	if(oldtransparent) {
		//rev. 3.1よりも昔のEditMotで作成したfbxにおいては　TransparencyFactor = diffuse.w
		if (transparent <= 0.99999) {
			newmqomat->SetTransparent(1);
		}
	}
	else {
		if (transparent != 0.0) {//0.0:Opaque, 1.0:Transparent (alphaはテクスチャ依存)
			newmqomat->SetTransparent(1);
		}
	}
	//if (strstr(newmqomat->GetName(), "_CLOTH_02") != 0) {//シャツのすそ
	//	newmqomat->SetTransparent(1);
	//}




//texture

	//if (strstr(tmpname, "plane__35_") != 0) {
	//	int dbgflag1 = 1;
	//}

	//if ((strstr(tmpname, "Terrain") != 0) || (strstr(tmpname, "terrain") != 0)) {
	//	int dbgflag2 = 1;
	//}

	//bool findalbedometal = false;
	{
		FbxProperty pProperty;
		pProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
		if (lLayeredTextureCount > 0)
		{
			for (int j = 0; j < lLayeredTextureCount; ++j)
			{
				FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < lNbTextures; ++k)
				{
					//char* nameptr = (char*)lLayeredTexture->GetName();
					if (lLayeredTexture->GetName()) {
						FbxVector2 fbxuvscale = lLayeredTexture->GetUVScaling();
						ChaVectorDbl2 chauvscale;
						chauvscale.SetParams(fmax(1.0, fbxuvscale[0]), fmax(1.0, fbxuvscale[1]));
						FbxVector2 fbxuvoffset = lLayeredTexture->GetUVTranslation();
						ChaVectorDbl2 chauvoffset;
						chauvoffset.SetParams(fbxuvoffset[0], fbxuvoffset[1]);

						char tempname[256];
						strcpy_s(tempname, 256, lLayeredTexture->GetName());
						bool emissiveflag = false;
						SetMaterialTexNames(newmqomat, tempname,
							lLayeredTexture->GetWrapModeU(), lLayeredTexture->GetWrapModeV(),
							chauvscale, chauvoffset, emissiveflag);	
						break;
					}
				}
			}
		}
		else
		{
			//no layered texture simply get on the property
			int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();
			if (lNbTextures > 0)
			{
				for (int j = 0; j < lNbTextures; ++j)
				{
					FbxFileTexture* lTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
					if (lTexture)
					{
						char* nameptr = (char*)lTexture->GetFileName();
						if (nameptr) {
							FbxVector2 fbxuvscale = lTexture->GetUVScaling();
							ChaVectorDbl2 chauvscale;
							chauvscale.SetParams(fmax(1.0, fbxuvscale[0]), fmax(1.0, fbxuvscale[1]));
							FbxVector2 fbxuvoffset = lTexture->GetUVTranslation();
							ChaVectorDbl2 chauvoffset;
							chauvoffset.SetParams(fbxuvoffset[0], fbxuvoffset[1]);

							char tempname[256];
							strcpy_s(tempname, 256, nameptr);
							bool emissiveflag = false;
							SetMaterialTexNames(newmqomat, tempname,
								lTexture->GetWrapModeU(), lTexture->GetWrapModeV(),
								chauvscale, chauvoffset, emissiveflag);
						}
					}
				}
			}
		}
	}

	{
		FbxProperty pProperty;
		pProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
		if (lLayeredTextureCount > 0)
		{
			for (int j = 0; j < lLayeredTextureCount; ++j)
			{
				FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < lNbTextures; ++k)
				{
					//char* nameptr = (char*)lLayeredTexture->GetName();
					if (lLayeredTexture->GetName()) {
						FbxVector2 fbxuvscale = lLayeredTexture->GetUVScaling();
						ChaVectorDbl2 chauvscale;
						chauvscale.SetParams(fmax(1.0, fbxuvscale[0]), fmax(1.0, fbxuvscale[1]));
						FbxVector2 fbxuvoffset = lLayeredTexture->GetUVTranslation();
						ChaVectorDbl2 chauvoffset;
						chauvoffset.SetParams(fbxuvoffset[0], fbxuvoffset[1]);

						char tempname[256];
						strcpy_s(tempname, 256, lLayeredTexture->GetName());
						bool emissiveflag = false;
						SetMaterialTexNames(newmqomat, tempname,
							lLayeredTexture->GetWrapModeU(), lLayeredTexture->GetWrapModeV(),
							chauvscale, chauvoffset, emissiveflag);
						break;
					}
				}
			}
		}
		else
		{
			//no layered texture simply get on the property
			int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();
			if (lNbTextures > 0)
			{
				for (int j = 0; j < lNbTextures; ++j)
				{
					FbxFileTexture* lTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
					if (lTexture)
					{
						char* nameptr = (char*)lTexture->GetFileName();
						if (nameptr) {
							FbxVector2 fbxuvscale = lTexture->GetUVScaling();
							ChaVectorDbl2 chauvscale;
							chauvscale.SetParams(fmax(1.0, fbxuvscale[0]), fmax(1.0, fbxuvscale[1]));
							FbxVector2 fbxuvoffset = lTexture->GetUVTranslation();
							ChaVectorDbl2 chauvoffset;
							chauvoffset.SetParams(fbxuvoffset[0], fbxuvoffset[1]);

							char tempname[256];
							strcpy_s(tempname, 256, nameptr);
							bool emissiveflag = false;
							SetMaterialTexNames(newmqomat, tempname,
								lTexture->GetWrapModeU(), lTexture->GetWrapModeV(),
								chauvscale, chauvoffset, emissiveflag);
						}
					}
				}
			}
		}
	}

	{
		FbxProperty pProperty;
		pProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
		if (lLayeredTextureCount > 0)
		{
			for (int j = 0; j < lLayeredTextureCount; ++j)
			{
				FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < lNbTextures; ++k)
				{
					//char* nameptr = (char*)lLayeredTexture->GetName();
					if (lLayeredTexture->GetName()) {
						FbxVector2 fbxuvscale = lLayeredTexture->GetUVScaling();
						ChaVectorDbl2 chauvscale;
						chauvscale.SetParams(fmax(1.0, fbxuvscale[0]), fmax(1.0, fbxuvscale[1]));
						FbxVector2 fbxuvoffset = lLayeredTexture->GetUVTranslation();
						ChaVectorDbl2 chauvoffset;
						chauvoffset.SetParams(fbxuvoffset[0], fbxuvoffset[1]);

						char tempname[256];
						strcpy_s(tempname, 256, lLayeredTexture->GetName());
						bool emissiveflag = true;
						SetMaterialTexNames(newmqomat, tempname,
							lLayeredTexture->GetWrapModeU(), lLayeredTexture->GetWrapModeV(),
							chauvscale, chauvoffset, emissiveflag);
						break;
					}
				}
			}
		}
		else
		{
			//no layered texture simply get on the property
			int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();
			if (lNbTextures > 0)
			{
				for (int j = 0; j < lNbTextures; ++j)
				{
					FbxFileTexture* lTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
					if (lTexture)
					{
						char* nameptr = (char*)lTexture->GetFileName();
						if (nameptr) {
							FbxVector2 fbxuvscale = lTexture->GetUVScaling();
							ChaVectorDbl2 chauvscale;
							chauvscale.SetParams(fmax(1.0, fbxuvscale[0]), fmax(1.0, fbxuvscale[1]));
							FbxVector2 fbxuvoffset = lTexture->GetUVTranslation();
							ChaVectorDbl2 chauvoffset;
							chauvoffset.SetParams(fbxuvoffset[0], fbxuvoffset[1]);

							char tempname[256];
							strcpy_s(tempname, 256, nameptr);
							bool emissiveflag = true;
							SetMaterialTexNames(newmqomat, tempname,
								lTexture->GetWrapModeU(), lTexture->GetWrapModeV(),
								chauvscale, chauvoffset, emissiveflag);
						}
					}
				}
			}
		}
	}



	//{
	//	FbxProperty pProperty;
	//	pProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	//	int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
	//	if (lLayeredTextureCount > 0)
	//	{
	//		for (int j = 0; j < lLayeredTextureCount; ++j)
	//		{
	//			FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
	//			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
	//			for (int k = 0; k < lNbTextures; ++k)
	//			{
	//				//char* nameptr = (char*)lLayeredTexture->GetName();
	//				if (lLayeredTexture->GetName()) {
	//					char tempname[256];
	//					strcpy_s(tempname, 256, lLayeredTexture->GetName());
	//					SetMaterialTexNames(newmqomat, tempname);
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		//no layered texture simply get on the property
	//		int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();
	//		if (lNbTextures > 0)
	//		{
	//			for (int j = 0; j < lNbTextures; ++j)
	//			{
	//				FbxFileTexture* lTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
	//				if (lTexture)
	//				{
	//					char* nameptr = (char*)lTexture->GetFileName();
	//					if (nameptr) {
	//						char tempname[256];
	//						strcpy_s(tempname, 256, nameptr);
	//						SetMaterialTexNames(newmqomat, tempname);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//if (newmqomat->GetTex() && (strlen(newmqomat->GetTex()) > 0)) {
	//	if ((strstr(newmqomat->GetTex(), "_13.png") != 0) || (strstr(newmqomat->GetTex(), "_15.png") != 0) || (strstr(newmqomat->GetTex(), "_16.png") != 0)) {
	//		//2023/09/24 VRoidの服の裾(すそ)に透過の印を付ける
	//		//上方において　((FbxSurfacePhong*)pMaterial)->TransparencyFactor != 0.0についてもSetTransparent(1)
	//		newmqomat->SetTransparent(1);
	//	}
	//}


   return 0;
}

int CModel::SetMaterialTexNames(CMQOMaterial* newmqomat, char* tempname,
	FbxTexture::EWrapMode addressU, FbxTexture::EWrapMode addressV,
	ChaVectorDbl2 chauvscale, ChaVectorDbl2 chauvoffset,
	bool emissiveflag)
{
	char temptexname[256] = {0};

	char* lastslash = strrchr(tempname, '/');
	if (!lastslash) {
		lastslash = strrchr(tempname, '\\');
	}
	if (lastslash) {
		//newmqomat->SetTex(lastslash + 1);
		strcpy_s(temptexname, 256, lastslash + 1);
	}
	else {
		//newmqomat->SetTex(tempname);
		strcpy_s(temptexname, 256, tempname);
	}

	//char* lastp = strrchr((char*)newmqomat->GetTex(), '.');
	char* lastp = strrchr(temptexname, '.');
	if (!lastp) {
		//newmqomat->Add2Tex(".tga");
		strcat_s(temptexname, ".tga");
	}
	else if (strcmp(lastp, ".psd") == 0) {
		strcpy(lastp, ".png");
	}
	else if (strcmp(lastp, ".vrm") == 0) {
		//VRM1.0のテクスチャパスは　vrmファイルの絶対パス(全テクスチャ名が同じvrmパス)
		// テクスチャとしてUnityでみえるのは　*.texture2Dファイル
		// .texture2DファイルはUnityスクリプトでpngに変換可能なので　そのようにしたpngをユーザが用意していることを想定
		// 2023/08/28現在はVRM0.0もサポートされていて　VRM0.0をUnityに読み込むとテクスチャはpngのものが生成される

		m_vrmtexcount++;
		if (m_vrmtexcount == 5) {
			//char convname[256] = { 0 };
			//strcpy_s(convname, 256, "_05.normal.png");
			//newmqomat->SetTex(convname);

			m_vrmtexcount++;

			char convname[256] = { 0 };
			sprintf_s(convname, 256, "_%02d.png", m_vrmtexcount);
			//newmqomat->SetTex(convname);
			strcpy_s(temptexname, convname);
		}
		else if (m_vrmtexcount == 8) {

			m_vrmtexcount++;

			char convname[256] = { 0 };
			sprintf_s(convname, 256, "_%02d.png", m_vrmtexcount);
			//newmqomat->SetTex(convname);
			strcpy_s(temptexname, convname);
		}
		else if (m_vrmtexcount == 11) {
			////char convname[256] = { 0 };
			////strcpy_s(convname, 256, "_11.normal.png");
			////newmqomat->SetTex(convname);

			m_vrmtexcount++;

			char convname[256] = { 0 };
			sprintf_s(convname, 256, "_%02d.png", m_vrmtexcount);
			//newmqomat->SetTex(convname);
			strcpy_s(temptexname, convname);
		}
		else {
			char convname[256] = { 0 };
			sprintf_s(convname, 256, "_%02d.png", m_vrmtexcount);
			//newmqomat->SetTex(convname);
			strcpy_s(temptexname, convname);
		}
	}
	else {
		//拡張子付きの通常ファイル名　そのまま
	}

	WCHAR wname[256];
	::ZeroMemory(wname, sizeof(WCHAR) * 256);
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, newmqomat->GetTex(), 256, wname, 256);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, temptexname, 256, wname, 256);

	DbgOut(L"SetMaterialTextureName : texture %s\r\n", wname);

	//if (strstr(temptexname, "Glass_A_Base") != 0) {
	//	int dbgflag1 = 1;
	//}



	newmqomat->SetUVScale(chauvscale);//2024/03/05
	newmqomat->SetUVOffset(chauvoffset);//2024/03/05


	if (emissiveflag && (newmqomat->GetEmissiveTex() && !(newmqomat->GetEmissiveTex()[0]))) {
		//2024/06/14 emissive texture
		newmqomat->SetEmissiveTex(temptexname);
		newmqomat->SetAddressU_emissive(addressU);
		newmqomat->SetAddressV_emissive(addressV);
	}
	else if ((newmqomat->GetAlbedoTex() && !(newmqomat->GetAlbedoTex()[0])) &&
		(strstr(temptexname, "albedo") != 0) || (strstr(temptexname, "Albedo") != 0) || 
		(strstr(temptexname, "_Base") != 0)) {
		newmqomat->SetAlbedoTex(temptexname);
		newmqomat->SetAddressU_albedo(addressU);
		newmqomat->SetAddressV_albedo(addressV);
	}
	else if ((newmqomat->GetNormalTex() && !(newmqomat->GetNormalTex()[0])) &&
		(strstr(temptexname, "normal") != 0) || (strstr(temptexname, "Normal") != 0)) {
		newmqomat->SetNormalTex(temptexname);
		newmqomat->SetAddressU_normal(addressU);
		newmqomat->SetAddressV_normal(addressV);
	}
	else if ((newmqomat->GetMetalTex() && !(newmqomat->GetMetalTex()[0])) &&
		(strstr(temptexname, "metal") != 0) || (strstr(temptexname, "Metal") != 0)) {
		newmqomat->SetMetalTex(temptexname);
		newmqomat->SetAddressU_metal(addressU);
		newmqomat->SetAddressV_metal(addressV);
	}else if (newmqomat->GetTex() && !(newmqomat->GetTex()[0])) {
		//TexNameに一回もセットされていない場合に　TexNameにtemptexnameをセット
		newmqomat->SetTex(temptexname);
		newmqomat->SetAddressU_albedo(addressU);
		newmqomat->SetAddressV_albedo(addressV);
	}


	return 0;
}



//// Get specific property value and connected texture if any.
//// Value = Property value * Factor property value (if no factor property, multiply by 1).
//FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
//    const char * pPropertyName,
//    const char * pFactorPropertyName,
//    char** ppTextureName)
//{
//	*ppTextureName = 0;
//
//    FbxDouble3 lResult(0, 0, 0);
//    const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
//    const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
//    if (lProperty.IsValid() && lFactorProperty.IsValid())
//    {
//        lResult = lProperty.Get<FbxDouble3>();
//        double lFactor = lFactorProperty.Get<FbxDouble>();
//        if (lFactor != 1)
//        {
//            lResult[0] *= lFactor;
//            lResult[1] *= lFactor;
//            lResult[2] *= lFactor;
//        }
//    }
//
//
//    return lResult;
//}

//void CModel::CreateExtendBoneReq(CBone* srcbone)
//{
//	if (srcbone) {
//		if (!srcbone->GetExtendFlag() && !srcbone->GetChild()) {
//			CBone* newbone = CBone::GetNewBone(this);
//			_ASSERT(newbone);
//			if (!newbone) {
//				_ASSERT(0);
//				return;
//			}
//			newbone->SetExtendFlag(true);
//			srcbone->AddChild(newbone);
//
//			char newbonename[256];
//			strcpy_s(newbonename, 256, "ext_");
//			strcat_s(newbonename, 256, srcbone->GetBoneName());
//			newbone->SetName(newbonename);
//
//			newbone->SetTopBoneFlag(0);
//			m_bonelist[newbone->GetBoneNo()] = newbone;
//			m_bonename[newbone->GetBoneName()] = newbone;
//
//			//if (type == FbxNodeAttribute::eSkeleton) {
//			newbone->SetType(FBXBONE_SKELETON);
//			//}
//			//else if (type == FbxNodeAttribute::eNull) {
//			//	newbone->SetType(FBXBONE_NULL);
//			//}
//			//else {
//			//	_ASSERT(0);
//			//}
//
//		}
//		else {
//			if (srcbone->GetChild()) {
//				CreateExtendBoneReq(srcbone->GetChild());
//			}
//		}
//
//		if (srcbone->GetBrother()) {
//			CreateExtendBoneReq(srcbone->GetBrother());
//		}
//	}
//}



void CModel::CreateFBXBoneReq(FbxScene* pScene, FbxNode* pNode, FbxNode* parnode )
{
//	EFbxRotationOrder lRotationOrder0 = eEulerZXY;
//	EFbxRotationOrder lRotationOrder1 = eEulerXYZ;

	if (!pScene || !pNode) {
		return;
	}

	bool createdflag = false;


	const char* nodename = pNode->GetName();
	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();


	//parentbonenodeは　parentが　skeletonまたはeNullの場合だけ変更する
	FbxNode* parentbonenode = 0;
	if (parnode) {
		FbxNodeAttribute* parattr = parnode->GetNodeAttribute();
		if (parattr) {
			FbxNodeAttribute::EType partype = parattr->GetAttributeType();
			////const char* parnodename = parnode->GetName();
			switch (partype)
			{
			case FbxNodeAttribute::eSkeleton:
			case FbxNodeAttribute::eNull:
			case FbxNodeAttribute::eCamera://2023/05/23
				parentbonenode = parnode;
				break;
			default:
				parentbonenode = 0;
				break;
			}
		}
		else {
			parentbonenode = parnode;//parent eNull
		}
	}
	else {
		//DbgOut(L"CreateFbxBoneReq : %s : parnode NULL!!!!\r\n", wname);
	}


	if (strcmp(nodename, "RootNode") == 0) {
		//GetFBXBone(pScene, FbxNodeAttribute::eSkeleton, pAttrib, pNode, 0);
		GetFBXBone(pScene, FbxNodeAttribute::eNull, pAttrib, pNode, 0);
		createdflag = true;
	}
	else if ( pAttrib ) {

		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
		switch ( type )
		{
			case FbxNodeAttribute::eSkeleton:
			case FbxNodeAttribute::eNull:
			case FbxNodeAttribute::eCamera://2023/05/23
				GetFBXBone(pScene, type, pAttrib, pNode, parentbonenode);
				createdflag = true;
				break;
			case FbxSkeleton::eRoot:
				_ASSERT(0);
				break;
			default:
				break;
		}

	}
	else {
		//attrib無しの　eNull
		GetFBXBone(pScene, FbxNodeAttribute::eNull, pAttrib, pNode, parentbonenode);
		createdflag = true;
	}

	int childNodeNum;
	childNodeNum = pNode->GetChildCount();
	for ( int i = 0; i < childNodeNum; i++ )
	{
		FbxNode *pChild = pNode->GetChild(i);  // 子ノードを取得
		if (createdflag == true) {
			CreateFBXBoneReq(pScene, pChild, pNode);//カレントで作成したボーンが　次の子供にとっての親
		}
		else {
			CreateFBXBoneReq(pScene, pChild, parentbonenode);//カレントでボーンを作成しなかった　親の変更なし
		}
	}

	return;
}

CBone* CModel::CreateNewFbxBone(FbxNodeAttribute::EType type, FbxNode* curnode, FbxNode* parnode)
{
	int settopflag = 0;

	if (!curnode) {
		_ASSERT(0);
		return 0;
	}


	CBone* newbone = CBone::GetNewBone(this);
	_ASSERT(newbone);
	if (!newbone) {
		_ASSERT(0);
		return 0;
	}

	//RenderContext* pRenderContext = DXUTGetD3D11DeviceContext();
	//newbone->LoadCapsuleShape(m_pdev, pRenderContext);//!!!!!!!!!!!
	newbone->LoadCapsuleShape(m_pdev);//!!!!!!!!!!!

	char newbonename[JOINTNAMELENG];
	strcpy_s(newbonename, JOINTNAMELENG, curnode->GetName());
	
	TermJointRepeats(newbonename);
	newbone->SetName(newbonename);
	newbone->SetFbxNodeOnLoad(curnode);

	m_bonelist[newbone->GetBoneNo()] = newbone;
	m_bonename[newbone->GetBoneName()] = newbone;


	//2023/07/21 SetIKStopFlag()に移動　chaファイルで指定した名前のジョイントにセット
	//2023/02/19
	//とりあえず　ジョイント名をみて　自動でIKStopFlagをセット
	//if ((strstr(newbone->GetBoneName(), "UpperLeg") != 0) ||
	//	//(strstr(newbone->GetBoneName(), "UpperArm") != 0)) {
	//	(strstr(newbone->GetBoneName(), "Shoulder") != 0)) {
	//	newbone->SetIKStopFlag(true);
	//}
	//else {
	//	newbone->SetIKStopFlag(false);
	//}
	



	//const char* strextend;
	//strextend = strstr(newbone->GetBoneName(), "_extend_");
	//if (!strextend) {
	if (strcmp(newbone->GetBoneName(), "RootNode") == 0) {
		//GetTopBone()からボーン階層をたどるときに　RootNodeの１つ下の階層に複数のジョイントがある場合に対応
		newbone->SetType(FBXBONE_ROOTNODE);
	}
	else if ((strstr(newbone->GetBoneName(), "Root") != 0) ||
		(strstr(newbone->GetBoneName(), "Reference") != 0)) {

		newbone->SetType(FBXBONE_NULL);
	}
	else if (type == FbxNodeAttribute::eSkeleton) {
		newbone->SetType(FBXBONE_SKELETON);


		//2023/10/26 GetBoneByName()高速化用
		const char* pjoint = strstr(newbonename, "_Joint");
		if (pjoint) {
			size_t strjointlen = strlen(pjoint);
			if (strjointlen == strlen("_Joint")) {
				m_withStrJoint++;
			}
			else {
				m_withoutStrJoint++;
			}
		}
		else {
			m_withoutStrJoint++;
		}


	}
	else if (type == FbxNodeAttribute::eNull) {
		newbone->SetType(FBXBONE_NULL);
	}
	else if (type == FbxNodeAttribute::eCamera) {
		newbone->SetType(FBXBONE_CAMERA);//2023/05/23
	}
	else {
		_ASSERT(0);
		newbone->SetType(FBXBONE_NULL);
	}
	//}
	//else {
	//	newbone->SetType(FBXBONE_NULL);
	//}
	if (!m_topbone) {
		m_topbone = newbone;
		newbone->SetTopBoneFlag(1);
		settopflag = 1;
	}
	else {
		newbone->SetTopBoneFlag(0);
	}
	
	m_bone2node[newbone] = curnode;
	m_node2bone[curnode] = newbone;


	if (parnode) {
		//const char* parentbonename = parnode->GetName();
		//char parentbonename[JOINTNAMELENG];
		//strcpy_s(parentbonename, JOINTNAMELENG, parnode->GetName());
		//TermJointRepeats(parentbonename);
		//CBone* parentbone = m_bonename[parentbonename];

		CBone* parentbone = FindBoneByNode(parnode);
		if (parentbone) {
			parentbone->AddChild(newbone);
			//_ASSERT(0);
		}
		else {
			_ASSERT(0);
			::MessageBoxA(NULL, "GetFBXBone : parentbone NULL error ", parnode->GetName(), MB_OK);
		}
	}
	else {
		if (settopflag == 0) {
			//この関数呼び出し時にparnode == 0で　この関数呼び出し以前にm_topboneがすでにセットされている場合
			m_topbone->AddChild(newbone);
		}
		else {
			//_ASSERT(0);
			//::MessageBoxA( NULL, "GetFBXBone : parentbone NULL error ", nodename, MB_OK );
		}
	}


	return newbone;

}


int CModel::GetFBXBone(FbxScene* pScene, FbxNodeAttribute::EType type, FbxNodeAttribute *pAttrib, FbxNode* curnode, FbxNode* parnode )
{
	//if (!pScene || !pAttrib || !curnode) {
	if (!pScene || !curnode) {
		_ASSERT(0);
		return 1;
	}



	CBone* newbone = 0;
	//if (!m_topbone) {
	//	newbone = CreateNewFbxBone(type, curnode, 0);
	//}
	//else {
	//	newbone = CreateNewFbxBone(type, curnode, parnode);
	//}

	newbone = CreateNewFbxBone(type, curnode, parnode);


	if (!newbone) {
		_ASSERT(0);
		return 1;
	}
	else {
		return 0;
	}
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

int CModel::MotionID2CameraIndex(int motid)
{
	int retindex = -1;

	int chkcnt = 0;
	map<int, MOTINFO*>::iterator itrmotinfo;
	for (itrmotinfo = m_motinfo.begin(); itrmotinfo != m_motinfo.end(); itrmotinfo++) {
		MOTINFO* curmi = itrmotinfo->second;
		if (curmi && curmi->cameramotion) {
			if (curmi->motid == motid) {
				retindex = chkcnt;
				break;
			}
			chkcnt++;//cameraのときだけカウントを増やす
		}
	}
	return retindex;
}

FbxAnimLayer* CModel::GetAnimLayer( int motid )
{
	FbxAnimLayer *retAnimLayer = 0;

	if (mAnimStackNameArray.Size() <= 0) {
		return 0;
	}

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

//void CModel::InitMpScaleReq(CBone* curbone, int srcmotid, double srcframe)
//{
//	double roundingframe = RoundingTime(srcframe);
//
//	if (curbone) {
//		ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
//		int paraxiskind = -1;//2021/11/18
//		//int isfirstbone = 0;
//		cureul = curbone->CalcLocalEulXYZ(paraxiskind, srcmotid, roundingframe, BEFEUL_BEFFRAME);
//
//		int inittraflag1 = 0;
//		int setchildflag1 = 1;
//		int initscaleflag1 = 1;//!!!!!!!
//		ChaMatrix befwm = curbone->GetWorldMat(srcmotid, roundingframe, 0);
//		curbone->SetWorldMatFromEul(inittraflag1, setchildflag1, befwm, cureul, srcmotid, roundingframe, initscaleflag1);
//
//		if (curbone->GetChild()) {
//			InitMpScaleReq(curbone->GetChild(), srcmotid, roundingframe);
//		}
//		if (curbone->GetBrother()) {
//			InitMpScaleReq(curbone->GetBrother(), srcmotid, roundingframe);
//		}
//
//	}
//}

int CModel::CreateFBXAnim( FbxScene* pScene, FbxNode* prootnode, BOOL motioncachebatchflag)
{
	static int s_dbgcnt = 0;
	s_dbgcnt++;


	SetDefaultBonePos(pScene);


	pScene->FillAnimStackNameArray(mAnimStackNameArray);
    const int lAnimStackCount = mAnimStackNameArray.GetCount();

	SetLoadingMotionCount(lAnimStackCount);

	DbgOut( L"FBX anim num %d\r\n", lAnimStackCount );


	//モーフアニメがあるかもしれないのでリターンしない
	//if (GetNoBoneFlag() == true) {
	//	return 0;
	//}

	if( lAnimStackCount <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}
	else {

		if (GetNoBoneFlag() == false) {
			CreateLoadFbxAnim(pScene);
		}
		//#### 2022/11/01 ################################################
		//この時点において　m_bonelistにはCBone*がセットされている 
		//LoadFbx()内で　CBone::SetFbxNodeOnLoadするようにした
		//よって　スレッド準備は　CreateLoadFbxAnimでしてしまうことが可能
		//CreateFbxAnimReq不要
		//#### 2022/11/01 ################################################
		//CreateFBXAnimReq(animno, pScene, pPose, prootnode, curmotid, animleng);//2022/10/21 : 最終フレームのモーションフレーム無し問題対応


		int animno;
		for (animno = 0; animno < lAnimStackCount; animno++) {





			// select the base layer from the animation stack
			char* animname = mAnimStackNameArray[animno]->Buffer();
			char chkanimname[256] = { 0 };
			strcpy_s(chkanimname, 256, animname);
			//MessageBoxA( NULL, chkanimname, "check", MB_OK );



			FbxAnimStack* lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[animno]->Buffer());
			if ((lCurrentAnimationStack == NULL) && (GetNoBoneFlag() == false)) {
				_ASSERT(0);
				DestroyLoadFbxAnim();
				return 1;
			}


			////RokDeBone2のデータを読み込んだ場合にはZXYをXYZにコンバートする
			//pScene->GetRootNode()->ConvertPivotAnimationRecursive(lCurrentAnimationStack, FbxNode::eDestinationPivot, 30.0, true);//2022/07/28コメントアウト


			FbxAnimLayer* mCurrentAnimLayer;
			mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
			SetCurrentAnimLayer(mCurrentAnimLayer);

			//pScene->GetEvaluator()->SetContext(lCurrentAnimationStack);
			pScene->SetCurrentAnimationStack(lCurrentAnimationStack);
			//pScene->GetAnimationEvaluator()->SetContext(lCurrentAnimationStack);



			//pScene->GetRootNode()->ConvertPivotAnimationRecursive( mAnimStackNameArray[animno]->Buffer(), FbxNode::eDestinationPivot, 30.0, true );
			//pScene->GetRootNode()->ConvertPivotAnimationRecursive( mAnimStackNameArray[animno]->Buffer(), FbxNode::eSourcePivot, 30.0, true );


			//const char* cameraname = pScene->GlobalCameraSettings().GetDefaultCamera();


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
				_ASSERT(0);
				// Take the time line value
				FbxTimeSpan lTimeLineTimeSpan;
				pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
				mStart = lTimeLineTimeSpan.GetStart();
				mStop = lTimeLineTimeSpan.GetStop();
			}


			//		int animleng = (int)mStop.GetFrame();// - mStart.GetFrame() + 1;		
					//mFrameTime.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
					//mFrameTime2.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
			mFrameTime.SetSecondDouble(1.0 / 30.0);
			mFrameTime2.SetSecondDouble(1.0 / 30.0);
			//mFrameTime.SetSecondDouble( 1.0 / 300.0 );
			//mFrameTime2.SetSecondDouble( 1.0 / 300.0 );


			double animleng = (int)((mStop.GetSecondDouble() - mStart.GetSecondDouble()) * 30.0);

			DbgOut(L"FBX anim %d, animleng %lf\r\n", animno, animleng);


			//######################################################################################
			//### AddMotion() : MotInfo, AddMotionPoint, CreateIndexedMotionPoint #################
			//######################################################################################
			int curmotid = -1;
			//AddMotion(mAnimStackNameArray[animno]->Buffer(), 0, (animleng - 1), &curmotid);// animleng - 1 !!!!!
			AddMotion(mAnimStackNameArray[animno]->Buffer(), 0, animleng, &curmotid);//2022/10/21 : animleng - 1だと最終フレームにモーションポイントが出来ない


			SetHasMotionCurveReq(mCurrentAnimLayer, GetTopBone(false), curmotid);


			bool cameraanimflag = false;
			//2023/05/23
			if ((strstr(chkanimname, "camera") != 0) || (strstr(chkanimname, "Camera") != 0)) {

				SetCameraMotion(curmotid);//2023/06/18 cameraのmotionであるという印をする

				//if (GetCameraFbx()) {
				//	GetCameraFbx()->SetAnimLayer(m_pscene, chkanimname);
				//}

				PreLoadCameraFbxAnim(curmotid);
				SetCameraMotionId(curmotid);

				cameraanimflag = true;
			}



			map<int, CBone*>::iterator itrbone;
			for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
				CBone* curbone = itrbone->second;
				if (curbone && (curbone->IsSkeleton() || curbone->IsCamera() || curbone->IsNull())) {
					curbone->SetGetAnimFlag(0);
				}
			}

			//FbxPose* pPose = pScene->GetPose( animno );
			//FbxPose* pPose = pScene->GetPose( 10 );

	//!!!!!!!!!!!!!!!!!!!!!		
			//FbxPose* pPose = FindBindPose();
			FbxPose* pPose = NULL;



			//Local情報からモーションを計算する
			m_loadbonecount = 0;

			//if (animno == 0) {//2023/11/04 anim loopが始まる前の場所に引っ越し
			//	CreateLoadFbxAnim(pScene);
			//	//#### 2022/11/01 ################################################
			//	//この時点において　m_bonelistにはCBone*がセットされている 
			//	//LoadFbx()内で　CBone::SetFbxNodeOnLoadするようにした
			//	//よって　スレッド準備は　CreateLoadFbxAnimでしてしまうことが可能
			//	//CreateFbxAnimReq不要
			//	//#### 2022/11/01 ################################################
			//	//CreateFBXAnimReq(animno, pScene, pPose, prootnode, curmotid, animleng);//2022/10/21 : 最終フレームのモーションフレーム無し問題対応
			//}

			//スレッド読み込みしない場合
			//map<int, CBone*>::iterator itranimbone;
			//for (itranimbone = m_bonelist.begin(); itranimbone != m_bonelist.end(); itranimbone++) {
			//	CBone* curbone = itranimbone->second;
			//	if (curbone) {
			//		FbxNode* curnode = curbone->GetFbxNodeOnLoad();
			//		if (curnode) {
			//			curbone->GetFBXAnim((int)GetFromBvhFlag(), curnode, animno, curmotid, animleng, false);
			//		}
			//	}
			//}


			if (GetNoBoneFlag() == false) {

				//マルチスレッド読み込み
				//if ((m_LoadFbxAnim != NULL) && (m_bonelist.size() >= (LOADFBXANIMTHREAD * 4))) {
				if ((m_LoadFbxAnim != NULL) && (m_bonelist.size() >= 1)) {
					int loadcount;
					for (loadcount = 0; loadcount < m_creatednum_loadfbxanim; loadcount++) {
						CThreadingLoadFbx* curload = m_LoadFbxAnim + loadcount;
						//curload->LoadFbxAnim(animno, curmotid, (animleng - 1));
						curload->LoadFbxAnim(animno, curmotid, animleng);//2022/10/21 : 最終フレームのモーションフレーム無し問題対応
					}

					WaitLoadFbxAnimFinished();//読み込み終了待ち


					//#### 2022/11/01 ###########################################################################
					//CBone::GetFbxAnimがlocalではなくglobal計算になったので　Post処理は不要になった
					//CBone::SetFirstMot()は　CBone::GetFbxAnimに移動　SetLocalMatは都度計算するのでここでは不要
					//ローカルオイラー角情報は　必要時に　Main.cppのrefreshEulerGraph()で計算される　
					//###########################################################################################



					//#### 2023/01/31 ####################################################################
					//読み込み時にLocalEulとLimitedLocalEulの初期化をするべきなので　復活
					//####################################################################################
					bool skeletonflag = true;
					PostLoadFbxAnim(curmotid, skeletonflag);//並列化出来なかった計算をする
					


					//2023/02/11
					int errorcount = 0;
					CreateIndexedMotionPoint(curmotid, animleng, &errorcount);
					if (errorcount != 0) {
						//_ASSERT(0);
						int dbgflag1 = 1;
					}

					//for debug 2024/06/15
					//CheckBoneTreeForDebugReq(GetTopBone(false), curmotid);

				}
				else {
				}
			}

			int errorcount2 = 0;
			GetFBXShapeAnimReq(GetNodeOnLoad(), mCurrentAnimLayer, curmotid, animleng, &errorcount2);
			if (errorcount2 != 0) {
				_ASSERT(0);
				abort();
			}

			//if (animno == (lAnimStackCount - 1)) {//2023/11/04 anim loopを出たところに引っ越し
			//	DestroyLoadFbxAnim();
			//}


			//motioncreatebatchflagが立っていた場合ここまで
			//if (motioncachebatchflag == FALSE) {


				//FillUpEmptyKeyReq(curmotid, (animleng - 1), m_topbone, 0);//nullジョイント対策？ 初期姿勢で初期化する処理はmotid==1以外のときにAddMotion内で行う


			//if (animno == 0) {
			//	//2022/11/01 Comment out
			//	//CallF(CreateFBXShape(mCurrentAnimLayer, animleng, mStart, mFrameTime2), return 1);//2022/10/21 : 最終フレームのモーションフレーム無し問題対応
			//}

			MOTINFO* pmotinfo = GetMotInfoPtr(curmotid);
			if (pmotinfo) {
				pmotinfo->fbxanimno = animno;//SetCurrentMotion時に　SetCurrentAnimLayer()と　SetCurrentAnimationStack()するために保存
			}
			SetCurrentMotion(curmotid);


			//}
		}

		if (GetNoBoneFlag() == false) {
			DestroyLoadFbxAnim();
		}


		////読み込みが終わったら最初のモーションをセットしておく
		//{
		//	int animno0 = 0;
		//	FbxAnimStack* lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[animno0]->Buffer());
		//	if (lCurrentAnimationStack == NULL) {
		//		_ASSERT(0);
		//		return 1;
		//	}
		//	FbxAnimLayer* mCurrentAnimLayer;
		//	mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
		//	SetCurrentAnimLayer(mCurrentAnimLayer);
		//	pScene->SetCurrentAnimationStack(lCurrentAnimationStack);
		//}


		//読み込みが終わったら　カメラのモーションをセットしておく
		{
			int animno0 = 0;
			int cameramotionid = GetCameraMotionId();
			if (cameramotionid >= 1) {
				MOTINFO motinfo = GetMotInfo(cameramotionid);
				if (motinfo.motid > 0) {
					animno0 = motinfo.fbxanimno;
				}
				else {
					_ASSERT(0);
					animno0 = 0;
				}
			}
			else {
				animno0 = 0;
			}
			FbxAnimStack* lCurrentAnimationStack = m_pscene->FindMember<FbxAnimStack>(mAnimStackNameArray[animno0]->Buffer());
			if (lCurrentAnimationStack == NULL) {
				_ASSERT(0);
				return 1;
			}
			FbxAnimLayer* mCurrentAnimLayer;
			mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
			SetCurrentAnimLayer(mCurrentAnimLayer);
			pScene->SetCurrentAnimationStack(lCurrentAnimationStack);
		}
	}


	//ボーンモーション読込ループでカメラアニメを読み込むと
	//ボーンモーションでカメラノードの姿勢が上書きされることがあるので
	//ループを抜けてからまとめて読み込む
	map<int, MOTINFO*>::iterator itrmi;
	for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++) {
		MOTINFO* miptr = itrmi->second;
		if (miptr && miptr->cameramotion) {
			GetFBXCameraAnim(miptr->motid, miptr->frameleng);

			bool skeletonflag = false;
			PostLoadFbxAnim(miptr->motid, skeletonflag);//並列化出来なかった計算をする
		}

		//if (miptr && (miptr->motid > 0)) {
		//	bool skeletonflag = false;
		//	PostLoadFbxAnim(miptr->motid, skeletonflag);//並列化出来なかった計算をする
		//}
	}


	return 0;
}


//int CModel::AddMotionPointAll(int srcmotid, double animleng)
//{
//	map<int, CBone*>::iterator itrbone;
//	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
//		CBone* curbone = itrbone->second;
//		if (curbone) {
//			int result = curbone->AddMotionPointAll(srcmotid, animleng);
//			if (result != 0) {
//				_ASSERT(0);
//				return result;
//			}
//		}
//	}
//	return 0;
//}

//void CModel::CheckBoneTreeForDebugReq(CBone* srcbone, int srcmotid)
//{
//	if (srcbone) {
//		if (srcbone->IsConcerned(srcmotid)) {//2024/06/10
//			int dbgflag1 = 1;
//		}
//
//		if (srcbone->GetChild(false)) {
//			CheckBoneTreeForDebugReq(srcbone->GetChild(false), srcmotid);
//		}
//		if (srcbone->GetBrother(false)) {
//			CheckBoneTreeForDebugReq(srcbone->GetBrother(false), srcmotid);
//		}
//	}
//}

void CModel::CreateIndexedMotionPoint(int srcmotid, double srcanimleng, int* perrorcount)
{
	if (!perrorcount) {
		_ASSERT(0);
		return;
	}

	char motionname[256] = { 0 };
	GetMotionName(srcmotid, 256, motionname);


	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* srcbone = itrbone->second;

		////if (srcbone->IsSkeleton() || srcbone->IsCamera() || (srcbone->IsNull() && strcmp(srcbone->GetBoneName(), motionname) == 0)) {
		//if (srcbone->IsSkeleton() || srcbone->IsCamera() || srcbone->IsNullAndChildIsCamera()) {
		////if (srcbone->IsSkeleton()) {
		if (srcbone && srcbone->IsConcerned(srcmotid)) {//2024/06/10
			int result;
			result = srcbone->CreateIndexedMotionPoint(srcmotid, srcanimleng);
			//_ASSERT(result == 0);
			if (result != 0) {
				(*perrorcount)++;
			}
		}
	}

}



//void CModel::CreateFBXAnimReq( int animno, FbxScene* pScene, FbxPose* pPose, FbxNode* pNode, int motid, double animleng )
//{
//	//static int dbgcnt = 0;
//
//	//int lSkinCount;
//
//	if (!pNode) {
//		return;
//	}
//
//
//	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
//	if ( pAttrib ) {
//		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
//
//		switch ( type )
//		{
////			case FbxNodeAttribute::eMesh:
////			case FbxNodeAttribute::eNURB:
////			case FbxNodeAttribute::eNURBS_SURFACE:
//			case FbxNodeAttribute::eSkeleton:
//				{
//					//GetFBXAnim( animno, pNode, motid, animleng );
//
//					if (m_LoadFbxAnim) {
//						//マルチスレッド読み込み準備
//						char bonename2[256];
//						strcpy_s(bonename2, 256, pNode->GetName());
//						TermJointRepeats(bonename2);
//						CBone* curbone = m_bonename[(char*)bonename2];
//						if (curbone) {
//							int threadcount;
//							int bonenointhread;
//							int bonenum;
//							int maxbonenuminthread;
//
//							bonenum = m_bonelist.size();
//							maxbonenuminthread = bonenum / m_creatednum_loadfbxanim + 1;
//							threadcount = m_loadbonecount / maxbonenuminthread;
//							CThreadingLoadFbx* curload = m_LoadFbxAnim + threadcount;
//							if (curload) {
//								bonenointhread = curload->GetBoneNum();
//								curload->SetBoneList(bonenointhread, pNode, curbone);
//								m_loadbonecount++;
//							}
//						}
//					}
//				}
//				break;
//			default:
//				break;
//		}
//	}
//
//	int childNodeNum;
//	childNodeNum = pNode->GetChildCount();
//	for ( int i = 0; i < childNodeNum; i++ )
//	{
//		FbxNode *pChild = pNode->GetChild(i);  // 子ノードを取得
//		CreateFBXAnimReq( animno, pScene, pPose, pChild, motid, animleng );
//	}
//
//	return;
//}


//int CModel::GetFreeThreadIndex()
//{
//	//４つのスレッドの中から空きスレッドを探す、なければ１つ以上スレッドが終了するのを待って終了したスレッドインデックスを返す
//
//	int chkindex;
//	for (chkindex = 0; chkindex < 6; chkindex++) {
//		HANDLE chkhandle = m_arhthread[chkindex];
//		if (chkhandle == 0) {
//			return chkindex;
//		}
//	}
//
//	//空きがみつからなかったとき
//	DWORD dwstatus;
//	dwstatus = WaitForMultipleObjects(6, m_arhthread, FALSE, INFINITE);
//
//	unsigned int signaledstatus;
//	signaledstatus = dwstatus & 0x0000007F;
//
//	int signaledindex = signaledstatus - WAIT_OBJECT_0;
//	if((signaledindex >= 0) && (signaledindex < 6)) {
//		
//		m_arhthread[signaledindex] = 0;
//		
//		FUNCMPPARAMS* curparams = m_armpparams[signaledindex];
//		if (curparams) {
//			free(curparams);
//		}
//		m_armpparams[signaledindex] = 0;
//
//		return signaledindex;
//	}
//	else {
//		return -1;
//	}
//
//	return -1;
//}
//
//void CModel::WaitAllTheadOfGetFbxAnim()
//{
//	int threadnum = 0;
//	HANDLE remainthread[6] = { 0, 0, 0, 0, 0, 0 };
//	int chkno;
//	for (chkno = 0; chkno < 6; chkno++) {
//		if (m_arhthread[chkno] != 0) {
//			remainthread[threadnum] = m_arhthread[chkno];
//			threadnum++;
//		}
//	}
//
//	if (threadnum >= 1) {
//		WaitForMultipleObjects(threadnum, remainthread, TRUE, INFINITE);
//
//		int paramno;
//		for (paramno = 0; paramno < 6; paramno++) {
//
//			FUNCMPPARAMS* curparams = m_armpparams[paramno];
//			if (curparams) {
//				free(curparams);
//			}
//		}
//	}
//
//	int resetindex;
//	for (resetindex = 0; resetindex < 6; resetindex++) {
//		m_armpparams[resetindex] = 0;
//		m_arhthread[resetindex] = 0;
//	}
//}


//#################################################
// マルチスレッド化のためにCBoneに移動 2022/08/14
//#################################################

//int CModel::GetFBXAnim(int animno, FbxNode* pNode, int motid, double animleng, bool callingbythread) // default : callingbythread = false
//{
//	//const char* bonename = ((FbxNode*)cluster->GetLink())->GetName();
//	char bonename2[256];
//	//strcpy_s(bonename2, 256, clusterlink->GetName());
//	strcpy_s(bonename2, 256, pNode->GetName());
//	TermJointRepeats(bonename2);
//	CBone* curbone = m_bonename[ (char*)bonename2 ];
//
//	WCHAR wname[256]={0L};
//	ZeroMemory( wname, sizeof( WCHAR ) * 256 );
//	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)bonename2, 256, wname, 256 );
//
//	if( curbone && !curbone->GetGetAnimFlag() && pNode){
//		curbone->SetGetAnimFlag( 1 );
//
//		int iships = 0;
//		char* phips = strstr(bonename2, "Hips");
//		if (phips) {
//			iships = 1;
//		}
//		else {
//			iships = 0;
//		}
//
//
//
//
//		FbxTime fbxtime;
//		fbxtime.SetSecondDouble(0.0);
//		FbxTime difftime;
//		difftime.SetSecondDouble(1.0 / 30);
//		double framecnt;
//		//for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
//		for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {//関数呼び出し時にanimleng - 1している
//
//		//##################################
//		//calclate motion by local info
//		//##################################
//
//			FbxAMatrix correctscalemat;
//			correctscalemat.SetIdentity();
//			FbxAMatrix currentmat;
//			currentmat.SetIdentity();
//			FbxAMatrix parentmat;
//			parentmat.SetIdentity();
//			//const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eDestinationPivot);
//			//const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eDestinationPivot);
//			//const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eDestinationPivot);
//			const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot);
//			const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot);
//			const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot);
//			FbxAMatrix lSRT = pNode->EvaluateLocalTransform(fbxtime, FbxNode::eSourcePivot);
//			FbxAMatrix lGlobalSRT = pNode->EvaluateGlobalTransform(fbxtime, FbxNode::eSourcePivot);
//
//
//			ChaVector3 chatra.SetParams((float)lT2[0], (float)lT2[1], (float)lT2[2]);
//			ChaVector3 chaeul.SetParams((float)lR2[0], (float)lR2[1], (float)lR2[2]);
//			ChaVector3 chascale.SetParams((float)lS2[0], (float)lS2[1], (float)lS2[2]);
//			ChaMatrix chaSRT;
//			chaSRT = ChaMatrixFromFbxAMatrix(lSRT);
//			ChaMatrix chaGlobalSRT;
//			chaGlobalSRT = ChaMatrixFromFbxAMatrix(lGlobalSRT);
//
//			
//			//if ((GetHasBindPose() == 0) && (framecnt == 0.0) && (animno == 0)) {
//			//	//#########################################################################################
//			//	//2022/07/28-2022/07/29  bindpose無し、0フレームアニメ在りの場合のジョイント位置計算し直し
//			//	//#########################################################################################
//
//			//	//ChaMatrix firstSRT = chascalemat * charotmat * chatramat;
//			//	ChaMatrix firstSRT = chaSRT;
//			//	curbone->SetFirstSRT(firstSRT);
//
//			//	
//			//	ChaMatrix globalSRT;
//			//	ChaMatrix parentglobalSRT;
//			//	parentglobalSRT = curbone->CalcParentGlobalSRT();
//			//	globalSRT = firstSRT * parentglobalSRT;
//			//	curbone->SetFirstGlobalSRT(globalSRT);
//
//
//			//	ChaMatrix newnodemat = curbone->GetNodeMat() * ChaMatrixInv(globalSRT);
//			//	ChaVector3 zeropos.SetParams(0.0f, 0.0f, 0.0f);
//			//	ChaVector3 truefpos;
//			//	ChaVector3TransformCoord(&truefpos, &zeropos, &newnodemat);
//
//
//			//	FbxAMatrix fbxmat;
//			//	fbxmat.SetIdentity();
//			//	fbxmat.SetRow(0, FbxVector4(newnodemat._11, newnodemat._12, newnodemat._13, newnodemat._14));
//			//	fbxmat.SetRow(1, FbxVector4(newnodemat._21, newnodemat._22, newnodemat._23, newnodemat._24));
//			//	fbxmat.SetRow(2, FbxVector4(newnodemat._31, newnodemat._32, newnodemat._33, newnodemat._34));
//			//	fbxmat.SetRow(3, FbxVector4(newnodemat._41, newnodemat._42, newnodemat._43, newnodemat._44));
//			//	
//			//	curbone->SetPositionFound(true);//!!!
//			//	//curbone->SetPositionFound(false);//!!!
//			//	curbone->SetNodeMat(newnodemat);
//			//	curbone->SetGlobalPosMat(fbxmat);
//			//	curbone->SetJointFPos(truefpos);
//			//	//curbone->SetJointWPos(truefpos);//0frameのアニメ付きジョイント位置を保持するため、ここでは更新しない。
//			//}
//
//
//
//
//
//			//####################
//			//calc joint position
//			//####################
//			ChaVector3 jointpos;
//			jointpos = curbone->GetJointFPos();
//			ChaVector3 parentjointpos;
//			if (curbone->GetParent()) {
//				parentjointpos = curbone->GetParent()->GetJointFPos();
//			}
//			else {
//				parentjointpos.SetParams(0.0f, 0.0f, 0.0f);
//			}
//
//			//##############
//			//calc rotation
//			//##############
//			CQuaternion chaq;
//			chaq.SetRotationXYZ(0, chaeul);
//			ChaMatrix charotmat;
//			charotmat = chaq.MakeRotMatX();
//
//			ChaMatrix befrotmat, aftrotmat;
//			ChaMatrixTranslation(&befrotmat, -jointpos.x, -jointpos.y, -jointpos.z);
//			ChaMatrixTranslation(&aftrotmat, jointpos.x, jointpos.y, jointpos.z);
//
//
//
//			//#################
//			//calc translation
//			//#################
//			ChaMatrix chatramat;
//			ChaMatrixIdentity(&chatramat);
//			ChaMatrixTranslation(&chatramat, chatra.x - jointpos.x + parentjointpos.x, chatra.y - jointpos.y + parentjointpos.y, chatra.z - jointpos.z + parentjointpos.z);
//			
//
//			//##############
//			//calc scalling
//			//##############
//			ChaMatrix chascalemat;
//			ChaMatrixScaling(&chascalemat, chascale.x, chascale.y, chascale.z);
//
//
//
//			//Set Local frame0
//			if (framecnt == 0.0) {
//				curbone->SetLocalR0(chaq);
//				curbone->SetLocalT0(chatramat);
//				curbone->SetLocalS0(chascalemat);
//				curbone->SetFirstSRT(chaSRT);
//			}
//
//
//			//##############
//			//calc localmat
//			//##############
//			ChaMatrix localmat;
//			ChaMatrixIdentity(&localmat);
//			ChaMatrix globalmat;
//			ChaMatrixIdentity(&globalmat);
//
//			CMotionPoint* curmp = 0;
//			int existflag = 0;
//			curmp = curbone->AddMotionPoint(motid, framecnt, &existflag);
//			if (!curmp) {
//				_ASSERT(0);
//				return 1;
//			}
//
//			curmp->SetSRT(chaSRT);
//			//curmp->SetSRT(befrotmat * chaSRT);//!!!!!!!!!!!!!
//
//
//			if (GetHasBindPose()) {
//				//######################################
//				// バインドポーズがある場合
//				//######################################
//
//				localmat = befrotmat * chascalemat * charotmat * aftrotmat * chatramat;
//
//
//				//#############
//				//set localmat
//				//#############
//				curmp->SetLocalMat(localmat);//anglelimit無し
//
//
//
//				if (callingbythread == false) {//並列化していない場合に実行。並列化時には他のboneの姿勢に依存しないように。
//					//###############
//					//calc globalmat
//					//###############
//					ChaMatrix parentglobalmat;
//					//parentglobalmat = curbone->CalcParentGlobalMat(motid, framecnt);//間にモーションを持たないジョイントが入っても正しくするためにこの関数で再帰計算する必要あり
//					if (curbone->GetParent()) {
//						parentglobalmat = curbone->GetParent()->GetWorldMat(motid, framecnt);
//					}
//					else {
//						ChaMatrixIdentity(&parentglobalmat);
//					}
//
//					globalmat = localmat * parentglobalmat;
//					curmp->SetWorldMat(globalmat);//anglelimit無し
//				}
//
//			}
//			else {
//				//############################################################
//				// バインドポーズが無い場合
//				// 0フレームにアニメーションが設定してあっても正常に再生可能
//				//############################################################
//
//
//				//###############
//				//calc globalmat
//				//###############
//				globalmat = (ChaMatrixInv(curbone->GetNodeMat())* chaGlobalSRT);
//				curmp->SetWorldMat(globalmat);//anglelimit無し
//
//
//
//				if (callingbythread == false) {//並列化していない場合に実行。並列化時には他のboneの姿勢に依存しないように。
//
//					//#############
//					//set localmat
//					//#############
//					ChaMatrix parentglobalmat;
//					//parentglobalmat = curbone->CalcParentGlobalMat(motid, framecnt);//間にモーションを持たないジョイントが入っても正しくするためにこの関数で再帰計算する必要あり
//					if (curbone->GetParent()) {
//						parentglobalmat = curbone->GetParent()->GetWorldMat(motid, framecnt);
//					}
//					else {
//						ChaMatrixIdentity(&parentglobalmat);
//					}
//					localmat = globalmat * ChaMatrixInv(parentglobalmat);
//					curmp->SetLocalMat(localmat);//anglelimit無し
//				}
//			}
//
//
//
//
//				//#########################################################
//				//説明しよう
//				//Fbxのローカル姿勢表現とAdditiveIKのローカル姿勢表現の互換について
//				//#########################################################
//
//				//fbxはひとつのボーンの姿勢を T * S * Rであらわす
//				//２つのボーンを表現すると　T1 S1 R1  T2 S2 R2 これは実は省略形というか効率化した形式
//
//				//本ソフトでは省略しない表現で計算していて　-T0 S0 R0 T0 Tanim_0  -Ta Sa Ra Ta Tanim_a  -Tb Sb Rb Tb Tanim_b　と解釈する
//
//				//この２つの表現形式は互換である。
//
//				//省略形を表現しなおすと　　...(T0 Tanim_a -Ta) Sa Ra   (Ta Tanim_b -Tb) Sb Rb    (Tb Tanim_c -Tc) ....
//				// LclTranslation = T0 + Tanim_a - Ta = parentjointpos + Tanim_a - jointpos (式１)
//				// 
//				//求めたいのは省略しない形式における-T S R T の後ろに掛けるTanim_aであり　式１より　Tanim_a = LclTranslation - jointpos + parentjointpos (式２)となる（トリッキー）
//				// 
//				// 
//				//-Ta Sa Ra Ta Tanim_a のTanim_aが式２により求まった。
//				//この関数内では　-Ta = befrotmat, Sa = chascalemat, Ra = charotmat, Ta = aftrotmat, Tanim_a = chatramat.
//				
//
//				//ここで実はものすごいトリッキーなことが起こった？？？？？
//				//SRの前にTanimとSRの後に掛けるTanimとは同じ記号で書いたが同じとは限らない。そのようになるように(TanimにSRを施してTanim_aになるように)計算しているのであろう.たぶん.
//				//##### ここまで説明してよく分からないということがわかった。 (試行錯誤の苦行によりなんとなく出来たのである) #####	
//				
//
//
//
//			//##################
//			//For old version
//			//##################
//			if ((animno == 0) && (framecnt == 0.0)) {
//				curbone->SetFirstMat(globalmat);
//				curbone->SetInitMat(globalmat);
//				//ChaMatrix calcmat = curbone->GetNodeMat() * curbone->GetInvFirstMat();
//
//				//curbone->SetFirstMat(curbone->GetNodeMat());//!!!!!!!!!! 2022/07/29
//				//curbone->SetInitMat(curbone->GetNodeMat());//!!!!!!!!!! 2022/07/29
//				ChaMatrix calcmat = curbone->GetNodeMat();//!!!!!!!!!! 2022/07/29
//
//				ChaVector3 zeropos(0.0f, 0.0f, 0.0f);
//				ChaVector3 tmppos;
//				ChaVector3TransformCoord(&tmppos, &zeropos, &calcmat);
//				curbone->SetOldJointFPos(tmppos);
//			}
//				  
//
//			////############################
//			////Check For Debug
//			////############################
//			// 
//			// Tanim_a = LclTranslation - jointpos + parentjointpos (式２)の確認
//			// 		
//			//ChaVector3 traanim;
//			//traanim = curbone->CalcLocalTraAnim(motid, framecnt);
//
//			//ChaVector3 calcTraAnim;
//			//calcTraAnim = chatra - jointpos + parentjointpos;
//
//			//if (wcsstr(m_filename, L"yuri") != 0) {
//			//	WCHAR strdbg[4098];
//			//	swprintf_s(strdbg, 4098, L"#### framecnt %lf, bonename %s, LclTranslation : (%f, %f, %f), traanim : (%f, %f, %f), CalcTraAnim : (%f, %f, %f)\r\n",
//			//		framecnt, curbone->GetWBoneName(), chatra.x, chatra.y, chatra.z, traanim.x, traanim.y, traanim.z,
//			//		calcTraAnim.x, calcTraAnim.y, calcTraAnim.z);
//			//	DbgOut(strdbg);
//			//}
//
//			// Check OK.
//
//
//			//###########
//			//step time
//			//###########
//			fbxtime = fbxtime + difftime;
//
//			Sleep(0);
//		}
//
//	}
//
//	return 0;
//}

int CModel::PreLoadCameraFbxAnim(int srcmotid)
{
	MOTINFO curmi = GetMotInfo(srcmotid);
	if ((curmi.motid > 0) && curmi.cameramotion) {
		double animlen = curmi.frameleng;


		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* srcbone = itrbone->second;

			if (srcbone && srcbone->IsConcernedCamera(curmi.motname)) {
				//#######################################################
				//MotionNameとBoneNameが同じ場合だけ　それらを対応付ける
				//#######################################################
				m_camerafbx.PreLoadFbxAnim(srcbone, srcmotid);//srcboneはFbxCamera*を持つFbxNodeに対応するCBone*　motionがコンポーネントとしてアタッチされていないboneの場合もある
				break;//!!!!!!
			}

			//if (srcbone && srcbone->IsCamera()) {
			//	if (strcmp(curmi.motname, srcbone->GetBoneName()) == 0) {
			//		//#######################################################
			//		//MotionNameとBoneNameが同じ場合だけ　それらを対応付ける
			//		//#######################################################
			//		m_camerafbx.PreLoadFbxAnim(srcbone, srcmotid);//srcboneはFbxCamera*を持つFbxNodeに対応するCBone*　motionがコンポーネントとしてアタッチされていないboneの場合もある
			//		break;//!!!!!!
			//	}
			//}
		}
	}

	return 0;
}


int CModel::PostLoadFbxAnim(int srcmotid, bool skeletonflag)
{
	MOTINFO curmi = GetMotInfo(srcmotid);
	if (curmi.motid > 0) {
		double animlen = curmi.frameleng;

		PostLoadFbxAnimReq(srcmotid, animlen, GetTopBone(false), skeletonflag);
	}
	return 0;
}

void CModel::PostLoadFbxAnimReq(int srcmotid, double animlen, CBone* srcbone, bool skeletonflag)
{
	if (srcbone) {
		//bool opeflag = false;
		//if (skeletonflag) {
		//	opeflag = srcbone->IsSkeleton();
		//}
		//else {
		//	opeflag = srcbone->IsNullAndChildIsCamera();
		//}
		//if (opeflag) {
		if (srcbone->IsConcerned(srcmotid)) {//2024/06/10
			double curframe;
			for (curframe = 0.0; curframe < animlen; curframe += 1.0) {//関数呼び出し時にanimleng - 1している

				CMotionPoint* curmp;
				curmp = srcbone->GetMotionPoint(srcmotid, curframe);
				if (curmp) {
					//#############
					//set localmat
					//#############

					//CCameraFbx::PostLoadFbxAnim()でのmotidとCAMERANODE*の対応表エントリーよりも後で呼ぶ
					//if (srcbone->IsSkeleton()) { 
					//if (srcbone->IsSkeleton() || srcbone->IsNullAndChildIsCamera()) { 
						//(srcbone->HasMotionCurve(srcmotid) && srcbone->IsCamera() && IsCameraMotion(srcmotid)) ||
						//(srcbone->HasMotionCurve(srcmotid) && srcbone->IsNullAndChildIsCamera() && IsCameraMotion(srcmotid))) {
						bool limitdegflag = false;
						ChaVector3 cureul = srcbone->CalcFBXEulXYZ(limitdegflag, srcmotid, curframe);
						curmp->SetLocalEul(cureul);
						curmp->SetLimitedLocalEul(cureul);
						curmp->SetCalcLimitedWM(2);
					//}
				}
			}
		}


		if (srcbone->GetChild(false)) {
			PostLoadFbxAnimReq(srcmotid, animlen, srcbone->GetChild(false), skeletonflag);
		}
		if (srcbone->GetBrother(false)) {
			PostLoadFbxAnimReq(srcmotid, animlen, srcbone->GetBrother(false), skeletonflag);
		}
	}
}

void CModel::SetHasMotionCurveReq(FbxAnimLayer* mCurrentAnimLayer, CBone* srcbone, int srcmotid)
{
	if (srcbone && mCurrentAnimLayer) {
		FbxNode* pNode = FindNodeByBone(srcbone);
		if (pNode) {
			const char* strChannel;
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;//X成分でチェック

			//移動のカーブがあるかどうか
			FbxAnimCurve* lCurveT;
			bool createflag = false;
			lCurveT = pNode->LclTranslation.GetCurve(mCurrentAnimLayer, strChannel, createflag);
			if (lCurveT) {
				srcbone->SetHasMotionCurve(srcmotid, true);
			}
			else {
				//回転のカーブがあるかどうか
				FbxAnimCurve* lCurveR;
				lCurveR = pNode->LclRotation.GetCurve(mCurrentAnimLayer, strChannel, createflag);
				if (lCurveR) {
					srcbone->SetHasMotionCurve(srcmotid, true);
				}
				else {
					srcbone->SetHasMotionCurve(srcmotid, false);
				}
			}
		}
		else {
			srcbone->SetHasMotionCurve(srcmotid, false);
		}

		if (srcbone->GetChild(false)) {
			SetHasMotionCurveReq(mCurrentAnimLayer, srcbone->GetChild(false), srcmotid);
		}
		if (srcbone->GetBrother(false)) {
			SetHasMotionCurveReq(mCurrentAnimLayer, srcbone->GetBrother(false), srcmotid);
		}

	}
}


//int CModel::CorrectFbxScaleAnim(int animno, FbxScene* pScene, FbxNode* pNode, FbxPose* pPose, FbxNodeAttribute* pAttrib, int motid, double animleng)
//{
//	//const char* bonename = ((FbxNode*)cluster->GetLink())->GetName();
//	char bonename2[256];
//	//strcpy_s(bonename2, 256, clusterlink->GetName());
//	strcpy_s(bonename2, 256, pNode->GetName());
//	TermJointRepeats(bonename2);
//	CBone* curbone = m_bonename[(char*)bonename2];
//
//	WCHAR wname[256] = { 0L };
//	::ZeroMemory(wname, sizeof(WCHAR) * 256);
//	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)bonename2, 256, wname, 256);
//
//	if (curbone && !curbone->GetGetAnimFlag() && pNode) {
//		curbone->SetGetAnimFlag(1);
//
//		FbxTime fbxtime;
//		fbxtime.SetSecondDouble(0.0);
//		FbxTime difftime;
//		difftime.SetSecondDouble(1.0 / 30);
//		double framecnt;
//		for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
//
//			//#######################
//			//calclate correct scale
//			//#######################
//				FbxAMatrix correctscalemat;
//				correctscalemat.SetIdentity();
//				FbxAMatrix currentmat;
//				currentmat.SetIdentity();
//				FbxAMatrix parentmat;
//				parentmat.SetIdentity();
//				const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eDestinationPivot);
//				const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eDestinationPivot);
//				const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eDestinationPivot);
//				//FbxVector4 lT3 = FbxVector4(0.0, 0.0, 0.0, 1.0);
//				//currentmat.SetTRS(lT3, lR2, lS2);
//				//if (curbone->GetParent() && pNode->GetParent()) {
//				//	CMotionPoint* parentmp = curbone->GetParent()->GetMotionPoint(motid, framecnt);
//				//	if (parentmp) {
//				//		ChaMatrix parentworldmat = parentmp->GetWorldMat();
//				//		ChaMatrix2FbxAMatrix(parentmat, parentworldmat);
//				//		correctscalemat = parentmat * currentmat;//scaleは合っている
//				//	}
//				//	else {
//				//		correctscalemat = currentmat;
//				//	}
//				//}
//				//else {
//				//	correctscalemat = currentmat;
//				//}
//				//const FbxVector4 correctscale = correctscalemat.GetS();
//
//			////#############################
//			////adjust scale and set motion
//			////#############################
//
//				//正しいスケールで姿勢をセットし直し
//				ChaVector3 chascale.SetParams((float)lS2[0], (float)lS2[1] , (float)lS2[2]);
//				ChaVector3 chatra.SetParams((float)lT2[0], (float)lT2[1], (float)lT2[2]);
//				ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
//				int paraxiskind = -1;//2021/11/18
//				cureul = curbone->CalcLocalEulXYZ(paraxiskind, motid, framecnt, BEFEUL_BEFFRAME);
//				int inittraflag1 = 0;
//				int setchildflag1 = 0;
//				int initscaleflag = 1;//!!!!!!!!!!!!
//				ChaMatrix befwm = curbone->GetWorldMat(motid, framecnt);
//				curbone->SetWorldMatFromEulAndScaleAndTra(inittraflag1, setchildflag1, befwm, cureul, chascale, chatra, motid, framecnt);
//
//
//			fbxtime = fbxtime + difftime;
//
//		}
//
//	}
//
//	return 0;
//}




void CModel::CreateFBXSkinReq( FbxNode* pNode )
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

	return;
}


bool CModel::HasCluster(FbxNode* srcnode, FbxNodeAttribute** ppAttrib)
{
	if (!srcnode) {
		_ASSERT(0);
		return false;
	}
	if (!ppAttrib) {
		_ASSERT(0);
		return false;
	}


	bool hascluster = false;

	FbxNodeAttribute* pAttrib = srcnode->GetNodeAttribute();
	if (pAttrib) {

		*ppAttrib = pAttrib;

		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
		if (type == FbxNodeAttribute::eMesh) {
			FbxMesh* pMesh = (FbxMesh*)pAttrib;
			int skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (skinCount >= 1) {
				for (int i = 0; i < skinCount; i++) {
					// i番目のスキンを取得
					FbxSkin* skin = (FbxSkin*)(pMesh->GetDeformer(i, FbxDeformer::eSkin));
					if (skin) {
						// クラスターの数を取得
						int clusterNum = skin->GetClusterCount();
						if (clusterNum >= 1) {
							hascluster = true;
							break;
						}
					}
				}
			}
		}
	}
	else {
		*ppAttrib = 0;
	}

	return hascluster;
}


int CModel::GetFBXSkin( FbxNodeAttribute *pAttrib, FbxNode* pNode )
{
	if (!pAttrib || !pNode) {
		_ASSERT(0);
		return 1;
	}


	//const char* nodename = pNode->GetName();

	FbxMesh *pMesh = (FbxMesh*)pAttrib;
	CMQOObject* newobj = 0;
	newobj = m_objectname[ pNode->GetName() ];
	if( !newobj ){
		_ASSERT( 0 );
		return 1;
	}

//スキン
	// スキンの数を取得
	int skinCount  = pMesh->GetDeformerCount( FbxDeformer::eSkin );

	int makecnt = 0;
	if (skinCount >= 1) {
		//for ( int i = 0; i < skinCount; ++i ) {
		for (int i = 0; i < skinCount; i++) {
			// i番目のスキンを取得
			FbxSkin* skin = (FbxSkin*)(pMesh->GetDeformer(i, FbxDeformer::eSkin));

			// クラスターの数を取得
			int clusterNum = skin->GetClusterCount();
			DbgOut(L"fbx : skin : org clusternum %d\r\n", clusterNum);

			for (int j = 0; j < clusterNum; ++j) {
				// j番目のクラスタを取得
				FbxCluster* cluster = skin->GetCluster(j);

				int validflag = IsValidFbxCluster(cluster);
				if (validflag == 0) {
					continue;
				}

				//const char* bonename = ((FbxNode*)cluster->GetLink())->GetName();
				char bonename2[256];
				FbxNode* clusterlink = (FbxNode*)cluster->GetLink();
				if (!clusterlink) {
					_ASSERT(0);
					continue;
				}
				strcpy_s(bonename2, 256, clusterlink->GetName());


				////for debug
				//int dbgflag = -1;
				//if (strstr(bonename2, "Hips_1") != 0) {
				//	dbgflag = 1;
				//}
				//if (strstr(bonename2, "Hips_2") != 0) {
				//	dbgflag = 2;
				//}
				//if (strstr(bonename2, "Hips_3") != 0) {
				//	dbgflag = 3;
				//}


				TermJointRepeats(bonename2);
				//			int namelen = (int)strlen( clustername );
				WCHAR wname[256];
				::ZeroMemory(wname, sizeof(WCHAR) * 256);
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, bonename2, -1, wname, 256);
				//			DbgOut( L"cluster (%d, %d), name : %s\r\n", i, j, wname );

				CBone* curbone = m_bonename[(char*)bonename2];

				if (curbone) {
					int curclusterno = newobj->GetClusterSize();

					if ((curclusterno < 0) || (curclusterno >= MAXCLUSTERNUM)) {
						WCHAR wmes[256];
						swprintf_s(wmes, 256, L"１つのパーツに影響できるボーンの制限数(%d個)を超えました。読み込めません。", MAXCLUSTERNUM);
						MessageBoxW(NULL, wmes, L"ボーン数エラー", MB_OK);
						_ASSERT(0);
						return 1;
					}

					int pointNum = cluster->GetControlPointIndicesCount();
					int* pointAry = cluster->GetControlPointIndices();
					double* weightAry = cluster->GetControlPointWeights();

					if ((pointNum >= 1) && pointAry) {//2023/07/29
						newobj->PushBackCluster(curbone);

						FbxCluster::ELinkMode lClusterMode = (FbxCluster::ELinkMode)cluster->GetLinkMode();
						int index;
						float weight;
						for (int i2 = 0; i2 < pointNum; i2++) {
							// 頂点インデックスとウェイトを取得
							index = pointAry[i2];

							//2023/07/29
							if (lClusterMode == FbxCluster::eTotalOne) {
								weight = 1.0f;
							}
							else {
								weight = (float)weightAry[i2];
							}

							int isadditive;
							if (lClusterMode == FbxCluster::eAdditive) {
								isadditive = 1;
							}
							else {
								isadditive = 0;
							}


							//if ((lClusterMode == FbxCluster::eAdditive) || (weight >= 0.05f)) {
							if ((lClusterMode == FbxCluster::eAdditive) || (weight >= 0.001f)) {
								//if ((lClusterMode == FbxCluster::eAdditive)){
								//newobj->AddInfBone(curclusterno, index, weight, isadditive);
								newobj->AddInfBone(curbone->GetMatrixIndex(), index, weight, isadditive);//2023/11/30
							}
						}

						makecnt++;
					}

				}
				else {
					//RootNodeという名前のジョイントが無いときにもここを通る
					//_ASSERT( 0 );
				}

			}

			newobj->NormalizeInfBone();
		}
	}
	else {
		//#################################
		//boneが　TopBoneただ１つだけの場合
		//#################################

		CBone* curbone = GetTopBone();

		if (curbone) {
			newobj->PushBackCluster(curbone);

			CPolyMesh4* pm4 = newobj->GetPm4();
			if (pm4) {
				int pointNum = pm4->GetOrgPointNum();

				int pointindex;
				float weight;
				for (pointindex = 0; pointindex < pointNum; pointindex++) {
					// 頂点インデックスとウェイトを取得

					weight = 1.0f;//topbone１つに1.0の重み

					int isadditive;
					isadditive = 1;
					int curclusterno = 0;
					//newobj->AddInfBone(curclusterno, pointindex, weight, isadditive);
					newobj->AddInfBone(curbone->GetMatrixIndex(), pointindex, weight, isadditive);//2023/11/30
				}

				makecnt++;
			}
		}
		else {
			//RootNodeという名前のジョイントが無いときにもここを通る
			//_ASSERT( 0 );
		}
		newobj->NormalizeInfBone();
	}

	

	DbgOut( L"fbx skin : make cluster %d\r\n", makecnt );


	return 0;
}

int CModel::RenderRefArrow(bool limitdegflag,
	myRenderer::RenderingEngine* re, ChaScene* srcchascene, ChaMatrix matVP,
	CBone* boneptr, ChaVector4 diffusemult, double refmult, std::vector<ChaVector3> vecbonepos)
{
	if (!re || !srcchascene || !boneptr) {
		_ASSERT(0);
		return 1;
	}

	if(vecbonepos.empty()){
		return 0;
	}
	int vecsize = (int)vecbonepos.size();
	if (vecsize <= 1) {
		return 0;
	}

	ResetRefPosMarkInstanceScale();

	CModel* refposmark = nullptr;
	int curinstanceno = 0;
	int vecno;
	for (vecno = 0; vecno < (vecsize - 1); vecno++) {
		refposmark = boneptr->GetRefPosMarkInstancing(&curinstanceno);
		if (refposmark) {
			ChaVector3 diffvec = vecbonepos[vecno + 1] - vecbonepos[vecno];
			double diffleng = ChaVector3LengthDbl(&diffvec);
			ChaVector3Normalize(&diffvec, &diffvec);

			ChaVector3 orgdir;
			orgdir.SetParams(1.0f, 0.0f, 0.0f);

			CQuaternion rotq;
			rotq.RotationArc(orgdir, diffvec);

			ChaMatrix bmmat;
			bmmat = rotq.MakeRotMatX();

			double fscalex = diffleng / 200.0f;
			double fscaleyz = diffleng / 800.0f;
			ChaMatrix scalemat;
			ChaMatrixIdentity(&scalemat);
			scalemat.SetScale(ChaVector3((float)(fscalex), (float)(fscaleyz * (double)refmult), (float)(fscaleyz * (double)refmult)));
			bmmat = scalemat * bmmat;
			bmmat.SetTranslation(vecbonepos[vecno]);

			//g_hmWorld->SetMatrix(bmmat.GetDataPtr());//2023/11/13 tmp comment out
			////g_pEffect->SetMatrix(g_hmWorld, &(bmmat.D3DX()));
			//this->UpdateMatrix(limitdegflag, &bmmat, &m_matVP);

			_ASSERT((curinstanceno >= 0) && (curinstanceno < RIGMULTINDEXMAX));
			refposmark->SetInstancingParams(curinstanceno, bmmat, matVP, diffusemult);

			//CallF(this->OnRender(withalpha, pRenderContext, 0, diffusemult), return 1);
		}
	}

	CBone::RenderRefPosMark(srcchascene, re, diffusemult);

	return 0;
}



int CModel::RenderBoneMark(bool limitdegflag, InstancedSprite* bcircleptr,
	int selboneno, ChaScene* srcchascene, ChaMatrix srcmatVP)
{
	if (!bcircleptr || !srcchascene) {
		_ASSERT(0);
		return 1;
	}


	if( m_bonelist.empty() ){
		return 0;
	}

	if (GetInView(0) == false) {
		return 0;
	}


	int curmotid;
	double roundingframe;
	if (ExistCurrentMotion()) {
		curmotid = GetCurrentMotID();
		//roundingframe = RoundingTime(GetCurrentFrame());
		roundingframe = RoundingTime(GetRenderSlotFrame());//2024/03/13
	}
	else {
		return 0;
	}



	map<int, CBone*>::iterator itrb;
	for( itrb = m_bonelist.begin(); itrb != m_bonelist.end(); itrb++ ){
		CBone* curbone = itrb->second;
		if( curbone && (curbone->IsSkeleton())){
			curbone->SetSelectFlag( 0 );
		}
	}

	if (selboneno > 0){
		CBone* selbone = m_bonelist[selboneno];
		if (selbone && (selbone->IsSkeleton())){
			SetSelectFlagReq(selbone, 0);
			selbone->SetSelectFlag(2);

			CBone* parentbone = selbone->GetParent(false);
			if (parentbone){//hipsを表示するために　parentboneはeNullも可
				CBtObject* curbto = FindBtObject(selbone->GetBoneNo());
				if (curbto){
					int tmpflag = selbone->GetSelectFlag() + 4;
					selbone->SetSelectFlag(tmpflag);
				}
			}
		}
	}

	////pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	//pRenderContext->OMSetDepthStencilState(g_pDSStateZCmpAlways, 1);//2023/11/13 tmp comment out
	g_zcmpalways = true;


	////ボーンの剛体表示
	if (g_rigidmarkflag && (g_previewFlag != 5) && (g_previewFlag != 4)){
		ResetBoneMarkInstanceScale();

		////RenderCapsuleReq(limitdegflag, rc, m_topbt);
		//RenderCapsuleReq(m_topbt, limitdegflag,
		//	selboneno, srcchascene, srcmatVP);
		RenderCapsuleBtoVec(limitdegflag,
			selboneno, srcchascene, srcmatVP);

		CBone::RenderColDisp(srcchascene);
	}


	//ボーンのサークル表示
	if ((g_previewFlag != 5) && (g_previewFlag != 4)){

		if (g_bonemarkflag){

			int instanceno = 0;//!!!!!!!

			map<int, CBone*>::iterator itrbone;
			for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++){
				CBone* boneptr = itrbone->second;
				if (boneptr && (boneptr->IsSkeleton()) && !boneptr->GetSkipRenderBoneMark()){

					CMotionPoint curmp = boneptr->GetCurMp();

					ChaMatrix bcmat;
					bcmat = boneptr->GetWorldMat(limitdegflag, curmotid, roundingframe, &curmp);
					//CBone* parentbone = boneptr->GetParent();
					//CBone* childbone = boneptr->GetChild();
					ChaMatrix transmat = bcmat * m_matVP;
					ChaVector3 wpos, scpos;
					ChaVector3 firstpos = boneptr->GetJointFPos();

					ChaVector3TransformCoord(&wpos, &firstpos, &bcmat);
					ChaVector3 cam2mark = wpos - g_camEye;
					ChaVector3Normalize(&cam2mark, &cam2mark);
					ChaVector3 camdir = g_camtargetpos - g_camEye;
					ChaVector3Normalize(&camdir, &camdir);
					double dot1 = ChaVector3Dot(&cam2mark, &camdir);
					if (dot1 < 0.0f) {
						//2023/03/23 カメラの後ろにあるマークが　逆さに表示されないように
						continue;
					}

					ChaVector3TransformCoord(&scpos, &firstpos, &transmat);
					scpos.x = (scpos.x + 1.0f) * (float)g_graphicsEngine->GetFrameBufferWidth() / 2.0f;//2023/11/25
					scpos.y = (-scpos.y + 1.0f) * (float)g_graphicsEngine->GetFrameBufferHeight() / 2.0f;//2023/11/25
					//scpos.z = scpos.z;
					scpos.z = 0.00001f;

					//bcircleptr->SetPos(scpos);
					ChaVector2 bsize;
					bsize.SetParams(1.0f, 1.0f);
					ChaVector4 bcolor;
					bcolor.SetParams(1.0f, 1.0f, 1.0f, 0.7f);
					if (boneptr->GetSelectFlag() & 2){
						bcolor.SetParams(0.0f, 0.0f, 1.0f, 0.7f);
						bsize.SetParams(0.050f, 0.050f);
						if (g_4kresolution) {
							bsize = bsize * 0.5f;
						}
						//bcircleptr->SetSize(bsize);
					}
					else if (boneptr->GetSelectFlag() & 1){
						bcolor.SetParams(g_bonemark_bright, 0.0f, 0.0f, 0.7f);//2024/01/12 bright
						bsize.SetParams(0.025f, 0.025f);
						if (g_4kresolution) {
							bsize = bsize * 0.5f;
						}
						//bcircleptr->SetSize(bsize);
					}
					else{
						//bcolor.SetParams(1.0f, 1.0f, 1.0f, 0.7f);
						bcolor.SetParams(g_bonemark_bright, g_bonemark_bright, g_bonemark_bright, 0.7f);//2024/01/12 bright
						bsize.SetParams(0.025f, 0.025f);
						if (g_4kresolution) {
							bsize = bsize * 0.5f;
						}
						//bcircleptr->SetSize(bsize);
					}

					ChaVector2 circlesize;
					circlesize.SetParams(6.0f, 6.0f);

					bcircleptr->UpdateScreen(instanceno, scpos, circlesize, bcolor);
					instanceno++;
					//CallF(bcircleptr->OnRender(pRenderContext), return 1);

				}
			}

			if (instanceno > 0) {
				//bcircleptr.DrawScreen(rc);

				myRenderer::RENDERSPRITE rendersprite;
				rendersprite.Init();
				rendersprite.pinstancedsprite = bcircleptr;
				srcchascene->AddSpriteToForwardRenderPass(rendersprite);
			}
		}
	}
	//else{
	//	if (g_bonemarkflag && bcircleptr){
	//		RenderBoneCircleReq(pRenderContext, m_topbt, bcircleptr);
	//	}
	//}


	////pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	//pRenderContext->OMSetDepthStencilState(g_pDSStateZCmp, 1);//2023/11/13 tmp comment out
	g_zcmpalways = false;

	return 0;
}

void CModel::ResetDispObjScale()
{
	map<int, CMQOObject*>::iterator itr;
	for (itr = m_object.begin(); itr != m_object.end(); itr++) {
		CMQOObject* curobj = itr->second;
		if (curobj && curobj->GetDispObj()) {
			curobj->GetDispObj()->ResetScaleInstancing();
		}
	}
}


int CModel::RenderBoneCircleOne(bool limitdegflag, RenderContext* pRenderContext, CMySprite* bcircleptr, int selboneno)
{
	if (!pRenderContext || !bcircleptr) {
		_ASSERT(0);
		return 1;
	}
	if (selboneno < 0) {
		return 0;
	}

	int curmotid;
	double roundingframe;
	if (ExistCurrentMotion()) {
		curmotid = GetCurrentMotID();
		roundingframe = RoundingTime(GetCurrentFrame());
	}
	else {
		return 0;
	}

	if (bcircleptr) {
		CBone* boneptr = GetBoneByID(selboneno);
		if (boneptr) {
			CMotionPoint curmp = boneptr->GetCurMp();

			ChaMatrix bcmat;
			bcmat = boneptr->GetWorldMat(limitdegflag, curmotid, roundingframe, &curmp);
			ChaMatrix transmat = bcmat * m_matVP;
			ChaVector3 scpos;
			ChaVector3 firstpos = boneptr->GetJointFPos();

			ChaVector3TransformCoord(&scpos, &firstpos, &transmat);
			scpos.z = 0.0f;
			bcircleptr->SetPos(scpos);
			ChaVector2 bsize;
			bcircleptr->SetColor(ChaVector4(1.0f, 0.0f, 0.0f, 0.7f));
			bsize.SetParams(0.025f, 0.025f);
			//if (g_4kresolution) {
			//	bsize = bsize * 0.5f;
			//}
			bcircleptr->SetSize(bsize);

			CallF(bcircleptr->OnRender(pRenderContext), return 1);
		}
	}
	return 0;
}

void CModel::RenderCapsuleBtoVec(bool limitdegflag,
	int selboneno, ChaScene* srcchascene, ChaMatrix srcmatVP)
{
	if (!srcchascene) {
		return;
	}

	vector<CBtObject*>::iterator itrbtovec;
	for (itrbtovec = m_btovec.begin(); itrbtovec != m_btovec.end(); itrbtovec++) {
		CBtObject* srcbto = *itrbtovec;
		if (srcbto) {
			CBone* srcbone = srcbto->GetBone();
			CBone* childbone = srcbto->GetEndBone();
			if (srcbone && childbone && !srcbone->GetSkipRenderBoneMark() && srcbone->IsSkeleton() && childbone->IsSkeleton()) {
				CRigidElem* curre = srcbone->GetRigidElem(childbone);
				if (curre && (curre->GetSkipflag() == 0)){
					CModel* curcoldisp;
					int curinstanceno;
					curcoldisp = srcbone->GetCurColDispInstancing(childbone, &curinstanceno);
					//if (curcoldisp) {
					if (curcoldisp && (curinstanceno < RIGMULTINDEXMAX)) {//2024/06/16 RIGMULTINDEXMAX以上は描画しない
						//################################################################################
						//以降SetInstancingParamsまでの間で処理をスキップしない。
						// スキップするとgetnoとinstancenoが一致しなくなり　CDispObjに保存してあるscale情報がずれる
						// 
						// 2024/06/16　ただし最大数以上は描画しないように
						//################################################################################

						bool setinstancescale = true;
						bool calcslotflag = false;
						srcbone->CalcRigidElemParams(setinstancescale, childbone, 0, calcslotflag);//形状データのスケールのために呼ぶ。
						//srcbto->SetCapsuleBtMotion(curre);

						ChaMatrix tmpcapmat = curre->GetCapsulematForColiShape(limitdegflag, 0, calcslotflag);//2023/01/18 //2024/03/29 calcslotflag
						//curcoldisp->UpdateMatrix(limitdegflag, &tmpcapmat, &m_matVP);

						ChaVector4 difmult;
						//if( boneptr->GetSelectFlag() & 4 ){
						if (childbone->GetSelectFlag() & 4) {
							difmult.SetParams(1.0f, 0.0f, 0.0f, 0.5f);
						}
						else {
							difmult.SetParams(0.25f, 0.5f, 0.5f, 0.5f);
						}
						curcoldisp->SetInstancingParams(curinstanceno, tmpcapmat, srcmatVP, difmult);
					}
				}
			}
		}
	}
}



//void CModel::RenderCapsuleReq(CBtObject* srcbto, bool limitdegflag,
//	int selboneno, ChaScene* srcchascene, ChaMatrix srcmatVP)
//{
//	if (!srcbto || !srcchascene) {
//		return;
//	}
//
//	CBone* srcbone = srcbto->GetBone();
//	CBone* childbone = srcbto->GetEndBone();
//	//if (srcbone && childbone && !srcbone->GetSkipRenderBoneMark()){
//	if (srcbone && childbone && !srcbone->GetSkipRenderBoneMark() && srcbone->IsSkeleton() && childbone->IsSkeleton()){//2024/06/16 IsSkeleton()
//		//if (srcbone->GetParent()){
//			//CRigidElem* curre = srcbone->GetParent()->GetRigidElem(srcbone);
//		CRigidElem* curre = srcbone->GetRigidElem(childbone);
//		if (curre && (curre->GetSkipflag() == 0)){
//			if ((strstr(childbone->GetBoneName(), "J_Sec_L_CoatSkirt") != 0) ||
//				(strstr(childbone->GetBoneName(), "J_Sec_R_CoatSkirt") != 0))
//			{
//				curre->SetSkipflag(1);//足の横にあるスカート用のジョイントの剛体は自動でスキップ(Invalidate)
//			}
//			else {
//				CModel* curcoldisp;
//				int curinstanceno;
//				curcoldisp = srcbone->GetCurColDispInstancing(childbone, &curinstanceno);
//				//if (curcoldisp) {
//				if (curcoldisp && (curinstanceno < RIGMULTINDEXMAX)) {//2024/06/16 RIGMULTINDEXMAX以上は描画しない
//					//################################################################################
//					//以降SetInstancingParamsまでの間で処理をスキップしない。
//					// スキップするとgetnoとinstancenoが一致しなくなり　CDispObjに保存してあるscale情報がずれる
//					// 
//					// 2024/06/16　ただし最大数以上は描画しないように
//					//################################################################################
//
//					bool setinstancescale = true;
//					bool calcslotflag = false;
//					srcbone->CalcRigidElemParams(setinstancescale, childbone, 0, calcslotflag);//形状データのスケールのために呼ぶ。
//					//srcbto->SetCapsuleBtMotion(curre);
//
//					ChaMatrix tmpcapmat = curre->GetCapsulematForColiShape(limitdegflag, 0, calcslotflag);//2023/01/18 //2024/03/29 calcslotflag
//					//curcoldisp->UpdateMatrix(limitdegflag, &tmpcapmat, &m_matVP);
//
//					ChaVector4 difmult;
//					//if( boneptr->GetSelectFlag() & 4 ){
//					if (childbone->GetSelectFlag() & 4) {
//						difmult.SetParams(1.0f, 0.0f, 0.0f, 0.5f);
//					}
//					else {
//						difmult.SetParams(0.25f, 0.5f, 0.5f, 0.5f);
//					}
//					curcoldisp->SetInstancingParams(curinstanceno, tmpcapmat, srcmatVP, difmult);
//				}
//			}
//		}
//		//}
//	}
//
//	int childno;
//	for (childno = 0; childno < srcbto->GetChildBtSize(); childno++){
//		CBtObject* childbto = srcbto->GetChildBt(childno);
//		RenderCapsuleReq(childbto, limitdegflag,
//			selboneno, srcchascene, srcmatVP);
//	}
//
//}


void CModel::RenderBoneCircleReq(RenderContext* pRenderContext, CBtObject* srcbto, CMySprite* bcircleptr)
{
	if (!srcbto || !bcircleptr) {
		return;
	}

	CBone* srcbone = srcbto->GetBone();
	CBone* childbone = srcbto->GetEndBone();
	if (srcbone && childbone && !srcbone->GetSkipRenderBoneMark()){
		CRigidElem* curre = srcbone->GetRigidElem(childbone);
		if (curre){
			if (childbone && (childbone->IsSkeleton())){

				//ChaMatrix capsulemat;
				//capsulemat = curre->GetCapsulemat();
				////CBone* parentbone = boneptr->GetParent();
				////CBone* childbone = boneptr->GetChild();
				//ChaMatrix transmat = capsulemat * m_matVP;
				//ChaVector3 scpos;
				//ChaVector3 firstpos.SetParams(0.0f, 0.0f, 0.0f);
				//
				//ChaVector3TransformCoord(&scpos, &firstpos, &transmat);
				ChaMatrix btmat = srcbone->GetBtMat();
				ChaMatrix transmat = btmat * m_matVP;
				ChaVector3 firstpos = childbone->GetJointFPos();
				ChaVector3 wpos, scpos;
				ChaVector3TransformCoord(&wpos, &firstpos, &btmat);

				ChaVector3 cam2mark = wpos - g_camEye;
				ChaVector3Normalize(&cam2mark, &cam2mark);
				ChaVector3 camdir = g_camtargetpos - g_camEye;
				ChaVector3Normalize(&camdir, &camdir);
				double dot1 = ChaVector3Dot(&cam2mark, &camdir);
				if (dot1 < 0.0f) {
					//2023/03/23 カメラの後ろにあるマークが　逆さに表示されないように
					return;
				}

				ChaVector3TransformCoord(&scpos, &firstpos, &transmat);
				scpos.z = 0.0f;

				bcircleptr->SetPos(scpos);
				ChaVector2 bsize;
				if (childbone->GetSelectFlag() & 2){
					bcircleptr->SetColor(ChaVector4(0.0f, 0.0f, 1.0f, 0.7f));
					bsize.SetParams(0.050f, 0.050f);
					bcircleptr->SetSize(bsize);
				}
				else if (childbone->GetSelectFlag() & 1){
					bcircleptr->SetColor(ChaVector4(1.0f, 0.0f, 0.0f, 0.7f));
					bsize.SetParams(0.025f, 0.025f);
					bcircleptr->SetSize(bsize);
				}
				else{
					bcircleptr->SetColor(ChaVector4(0.8f, 0.8f, 0.8f, 0.7f));
					bsize.SetParams(0.025f, 0.025f);
					bcircleptr->SetSize(bsize);
				}
				CallF(bcircleptr->OnRender(pRenderContext), return);

			}
		}
	}

	int chilno;
	for (chilno = 0; chilno < srcbto->GetChildBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChildBt(chilno);
		RenderBoneCircleReq(pRenderContext, chilbto, bcircleptr);
	}
}


//void CModel::SetDefaultBonePosReq( CBone* curbone, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix ParentGlobalPosition )
//{
//	if (!curbone){
//		return;
//	}
//
//	FbxNode* pNode = m_bone2node[ curbone ];
//
//
//
//
//	FbxAMatrix lGlobalPosition;
//	bool        lPositionFound = false;//バインドポーズを書き出さない場合やHipsなどの場合は０になる？
//
//	lGlobalPosition.SetIdentity();
//
//	if (pNode) {
//		if (pPose) {
//			int lNodeIndex = pPose->Find(pNode);
//			if (lNodeIndex > -1)
//			{
//				// The bind pose is always a global matrix.
//				// If we have a rest pose, we need to check if it is
//				// stored in global or local space.
//				if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
//				{
//					lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
//				}
//				else
//				{
//					// We have a local matrix, we need to convert it to
//					// a global space matrix.
//					FbxAMatrix lParentGlobalPosition;
//
//					//if (pParentGlobalPosition)
//					if (curbone->GetParent())
//					{
//						lParentGlobalPosition = ParentGlobalPosition;
//					}
//					else
//					{
//						if (pNode->GetParent())
//						{
//							lParentGlobalPosition = GetGlobalPosition(this, pNode->GetScene(), pNode->GetParent(), pTime, pPose);
//						}
//					}
//
//					FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
//					lGlobalPosition = lParentGlobalPosition * lLocalPosition;
//				}
//
//				lPositionFound = true;
//			}
//		}
//	}
//
//	if (!lPositionFound)
//	{
//		// There is no pose entry for that node, get the current global position instead.
//
//		// Ideally this would use parent global position and local position to compute the global position.
//		// Unfortunately the equation 
//		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
//		// does not hold when inheritance type is other than "Parent" (RSrs).
//		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
//		if (pNode) {
//			lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
//		}
//	}
//
//	ChaMatrix nodemat;
//
//	nodemat._11 = (float)lGlobalPosition.Get(0, 0);
//	nodemat._12 = (float)lGlobalPosition.Get(0, 1);
//	nodemat._13 = (float)lGlobalPosition.Get(0, 2);
//	nodemat._14 = (float)lGlobalPosition.Get(0, 3);
//
//	nodemat._21 = (float)lGlobalPosition.Get(1, 0);
//	nodemat._22 = (float)lGlobalPosition.Get(1, 1);
//	nodemat._23 = (float)lGlobalPosition.Get(1, 2);
//	nodemat._24 = (float)lGlobalPosition.Get(1, 3);
//
//	nodemat._31 = (float)lGlobalPosition.Get(2, 0);
//	nodemat._32 = (float)lGlobalPosition.Get(2, 1);
//	nodemat._33 = (float)lGlobalPosition.Get(2, 2);
//	nodemat._34 = (float)lGlobalPosition.Get(2, 3);
//
//	nodemat._41 = (float)lGlobalPosition.Get(3, 0);
//	nodemat._42 = (float)lGlobalPosition.Get(3, 1);
//	nodemat._43 = (float)lGlobalPosition.Get(3, 2);
//	nodemat._44 = (float)lGlobalPosition.Get(3, 3);
//
//	curbone->SetPositionFound(lPositionFound);//!!!
//	curbone->SetNodeMat(nodemat);
//	curbone->SetGlobalPosMat(lGlobalPosition);
//
//	ChaVector3 zeropos(0.0f, 0.0f, 0.0f);
//	ChaVector3 tmppos;
//	ChaVector3TransformCoord(&tmppos, &zeropos, &(curbone->GetNodeMat()));
//	curbone->SetJointWPos(tmppos);
//	curbone->SetJointFPos(tmppos);
//
//
////WCHAR wname[256];
////ZeroMemory( wname, sizeof( WCHAR ) * 256 );
////MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, curbone->m_bonename, 256, wname, 256 );
////DbgOut( L"SetDefaultBonePos : %s : wpos (%f, %f, %f)\r\n", wname, curbone->m_jointfpos.x, curbone->m_jointfpos.y, curbone->m_jointfpos.z );
//
//
//	if( curbone->GetChild() ){
//		//if (curbone->GetChild()->GetChild()) {
//			SetDefaultBonePosReq(curbone->GetChild(), pTime, pPose, curbone->GetGlobalPosMat());
//		//}
//		//else {
//		//	SetDefaultBonePosReq(curbone->GetBrother(), pTime, pPose, ParentGlobalPosition);
//		//}
//	}
//	if( curbone->GetBrother() ){
//		SetDefaultBonePosReq( curbone->GetBrother(), pTime, pPose, ParentGlobalPosition );
//	}
//
//}

FbxPose* CModel::FindBindPose()
{
	//###################################################################################
	//商用のfbxにはbindposeを取得できないものがあるが、そういうfbxには手を出さない方針で
	//###################################################################################


/*
		if (pNode) {
			if (pPose) {
				int lNodeIndex = pPose->Find(pNode);
				if (lNodeIndex > -1)
				{
					// The bind pose is always a global matrix.
					// If we have a rest pose, we need to check if it is
					// stored in global or local space.
					if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
					{
						lGlobalPosition = FbxGetPoseMatrix(pPose, lNodeIndex);
					}
*/


	if (!m_pscene) {
		return 0;
	}

	int posecount = m_pscene->GetPoseCount();
	if (posecount != 1) {
		int dbgflag1 = 1;
	}



	FbxPose* bindpose = 0;
	FbxPose* lastpose = 0;
	int lastpindex = 0;
	FbxPose* curpose = m_pscene->GetPose(0);
	lastpose = curpose;
	int curpindex = 1;
	while (curpose){
		if (curpose->IsBindPose()){
			bindpose = curpose;//最後のバインドポーズ
		}
		lastpose = curpose;
		lastpindex = curpindex;
		curpose = m_pscene->GetPose(curpindex);
		curpindex++;
	}
	if (!bindpose){
		//::MessageBoxA(NULL, "バインドポーズがありません。", "警告", MB_OK);
		////if (lastpose) {
		////	bindpose = lastpose;
		////	char strmes[256] = { 0 };
		////	sprintf_s(strmes, 256, "%d番目のポーズをバインドポーズとして使用します。", lastpindex);
		////	::MessageBoxA(NULL, strmes, "注意", MB_OK);
		////}
		////else {
		////	//bindpose = m_pscene->GetPose(0);
		////	bindpose = 0;
		////	::MessageBoxA(NULL, "ポーズが１つもありません。", "注意", MB_OK);
		////}

		bindpose = m_pscene->GetPose(0);
		if (!bindpose) {
			bindpose = m_pscene->GetPose(-1);
			//if (!bindpose) {
			//	::MessageBoxA(NULL, "ポーズが１つもありません。", "警告", MB_OK);
			//}
		}

		SetHasBindPose(0);
		SetFromNoBindPoseFlag(true);
	}
	else {
		SetHasBindPose(1);
	}
	return bindpose;
}


int CModel::SetDefaultBonePos(FbxScene* pScene)
{
	if( !pScene || !m_topbone ){
		return 0;
	}


	//2023/07/04 
	//NodeMatが変わらないように　SetRotationActiveは変更しないことに
	//SetRotationActiveToBone();//SetFbxDefaultBonePosReq()よりも前で呼ぶ


	FbxPose* bindpose = FindBindPose();
	SetBindPose(bindpose);


	FbxTime pTime;
	pTime.SetSecondDouble( 0.0 );

	////CBone* secbone = m_topbone->GetChild();
	//CBone* secbone = m_topbone;
	//FbxAMatrix inimat;
	//inimat.SetIdentity();
	//if( secbone ){
	//	FbxSetDefaultBonePosReq(pScene, this, secbone, pTime, bindpose, &inimat);
	//}

	if (GetNodeOnLoad()) {
		FbxAMatrix inimat;
		inimat.SetIdentity();
		FbxSetDefaultBonePosReq(pScene, this, GetNodeOnLoad(), pTime, bindpose, &inimat);
	}

	return 0;
}


//void CModel::FillUpEmptyKeyReq( int motid, double animleng, CBone* curbone, CBone* parentbone )
//{
//	if (!curbone){
//		return;
//	}
//
//	curbone->InitMP(motid, animleng);
//
//
//	if( curbone->GetChild() ){
//		FillUpEmptyKeyReq( motid, animleng, curbone->GetChild(), curbone );
//	}
//	if( curbone->GetBrother() ){
//		FillUpEmptyKeyReq( motid, animleng, curbone->GetBrother(), parentbone );
//	}
//
//}
//
//int CModel::FillUpEmptyMotion(int srcmotid)
//{
//
//	MOTINFO* curmi = GetMotInfo( srcmotid );
//	_ASSERT(curmi);
//	if (curmi){
//		FillUpEmptyKeyReq(curmi->motid, curmi->frameleng, m_topbone, 0);
//		return 0;
//	}
//	else{
//		_ASSERT(0);
//		return 1;
//	}
//
//}


//int CModel::SetMaterialName()
//{
//	m_materialname.clear();
//
//	map<int, CMQOMaterial*>::iterator itrmat;
//	for( itrmat = m_material.begin(); itrmat != m_material.end(); itrmat++ ){
//		CMQOMaterial* curmat = itrmat->second;
//		m_materialname[ curmat->GetName() ] = curmat;
//	}
//
//	return 0;
//}

int CModel::GetTextureNameVec(std::vector<std::string>& dstvec)
{
	dstvec.clear();

	vector<string> tmpvec;

	////MQOファイル用のマテリアルを格納
	//map<int, CMQOMaterial*>::iterator itrmat;
	//for (itrmat = m_material.begin(); itrmat != m_material.end(); itrmat++) {
	//	CMQOMaterial* curmat = itrmat->second;
	//	if (curmat) {
	//		if (curmat->GetTex() && (strlen(curmat->GetTex()) > 0)) {
	//			string curtexname = curmat->GetTex();


	//			bool sameflag = false;
	//			vector<string>::iterator itrchk;
	//			for (itrchk = dstvec.begin(); itrchk != dstvec.end(); itrchk++) {
	//				if (*itrchk == curtexname) {
	//					sameflag = true;
	//					break;
	//				}
	//			}

	//			if (sameflag == false) {
	//				dstvec.push_back(curtexname);//重複を除いて格納
	//			}
	//			tmpvec.push_back(curtexname);//textureが在るものは重複可で全部格納
	//		}
	//	}
	//}
	int materialnum = GetMQOMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* curmat = m_materialbank.Get(matindex);
		if (curmat) {
			//if (curmat->GetTex() && (strlen(curmat->GetTex()) > 0)) {
			//	string curtexname = curmat->GetTex();
			if (curmat->GetAlbedoTex() && (strlen(curmat->GetAlbedoTex()) > 0)) {//2024/01/08
				string curtexname = curmat->GetAlbedoTex();


				bool sameflag = false;
				vector<string>::iterator itrchk;
				for (itrchk = dstvec.begin(); itrchk != dstvec.end(); itrchk++) {
					if (*itrchk == curtexname) {
						sameflag = true;
						break;
					}
				}

				if (sameflag == false) {
					dstvec.push_back(curtexname);//重複を除いて格納
				}
				tmpvec.push_back(curtexname);//textureが在るものは重複可で全部格納
			}
		}
	}


	////fbxファイル用のマテリアルを格納
	//map<int, CMQOObject*>::iterator itrobj;
	//for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
	//	CMQOObject* curobj = itrobj->second;
	//	if (curobj) {
	//		map<int, CMQOMaterial*>::iterator itrmat2;
	//		for (itrmat2 = curobj->GetMaterialBegin(); itrmat2 != curobj->GetMaterialEnd(); itrmat2++) {
	//			CMQOMaterial* curmat2 = itrmat2->second;
	//			if (curmat2) {
	//				if (curmat2->GetTex() && (strlen(curmat2->GetTex()) > 0)) {
	//					string curtexname2 = curmat2->GetTex();
	//						
	//					bool sameflag2 = false;
	//					vector<string>::iterator itrchk2;
	//					for (itrchk2 = dstvec.begin(); itrchk2 != dstvec.end(); itrchk2++) {
	//						if (*itrchk2 == curtexname2) {
	//							sameflag2 = true;
	//							break;
	//						}
	//					}

	//					if (sameflag2 == false) {
	//						dstvec.push_back(curtexname2);//重複を除いて格納
	//					}
	//					tmpvec.push_back(curtexname2);//textureが在るものは重複可で全部格納

	//				}
	//			}
	//		}
	//	}
	//}

	return 0;
}



int CModel::DestroyBtObject()
{
	if( m_topbt ){
		DestroyBtObjectReq( m_topbt );
	}
	m_topbt = 0;
	m_btovec.clear();
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
	for (chilno = 0; chilno < (int)curbt->GetChildBtSize(); chilno++){
		CBtObject* chilbt = curbt->GetChildBt(chilno);
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
	if (!srcbto) {
		return;
	}

	CBone* srcbone = srcbto->GetBone();
	if( srcbone ){
		if (!srcbone->GetParent(false) || (srcbone->GetParent(false) && (srcbone->GetParent(false)->GetTmpKinematic() == false))) {
			int cmp0 = strncmp(srcbone->GetBoneName(), "BT_", 3);
			const char* pcomp1 = strstr(srcbone->GetBoneName(), "Hair1_");
			const char* pcomp2 = strstr(srcbone->GetBoneName(), "Hair2_");
			const char* pcomp3 = strstr(srcbone->GetBoneName(), "Hair3_");
			const char* pcomp4 = strstr(srcbone->GetBoneName(), "Hair4_");
			const char* pcomp5 = strstr(srcbone->GetBoneName(), "Hair5_");
			const char* pcomp6 = strstr(srcbone->GetBoneName(), "Hair6_");

			if ((cmp0 == 0) || (pcomp1 != NULL) || (pcomp2 != NULL) || (pcomp3 != NULL) || (pcomp4 != NULL) || (pcomp5 != NULL) || (pcomp6 != NULL)) {
				//2022/07/11 auto simu settings force
				srcbone->SetBtForce(1);
			}

			if (srcbone->GetBtForce() == 1) {
				if (srcbone->GetParent(false)) {
					CRigidElem* curre = srcbone->GetParent(false)->GetRigidElem(srcbone);
					if (curre) {
						//if (curre->GetSkipflag() == 0){
						srcbone->SetBtKinFlag(0);
						//}
						//else{
						//	srcbone->SetBtKinFlag(1);
						//}


					}
					else {
						srcbone->SetBtKinFlag(1);
					}
				}
				else {
					srcbone->SetBtKinFlag(1);
				}
			}
			else {
				srcbone->SetBtKinFlag(1);
			}
		}
		else {
			srcbone->SetBtKinFlag(1);
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
			//srcbto->GetRigidBody()->setActivationState(WANTS_DEACTIVATION);
			//srcbto->m_rigidbody->setActivationState(DISABLE_SIMULATION);
			//CF_STATIC_OBJECT
		}else if( srcbto->GetRigidBody() ){
			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
			srcbto->GetRigidBody()->setCollisionFlags(curflag & ~btCollisionObject::CF_KINEMATIC_OBJECT);// ~

			if( srcbone->GetParent(false) ){
				CRigidElem* curre = srcbone->GetParent(false)->GetRigidElem( srcbone );
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
	for( chilno = 0; chilno < srcbto->GetChildBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChildBt( chilno );
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
	for (chilno = 0; chilno < srcbto->GetChildBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChildBt(chilno);
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

		//srcbto->GetRigidBody()->setDeactivationTime(0.0);
		////srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);

		srcbto->GetRigidBody()->setDeactivationTime(0.0f);
		srcbto->GetRigidBody()->setSleepingThresholds(0.0f, 0.0f);
		srcbto->GetRigidBody()->activate();
	}

	int chilno;
	for (chilno = 0; chilno < srcbto->GetChildBtSize(); chilno++){
		CBtObject* chilbto = srcbto->GetChildBt(chilno);
		BulletSimulationStartReq(chilbto);
	}

}


int CModel::CreateBtConstraint(bool limitdegflag)
{
	if( !GetTopBone()){
		return 0;
	}
	if( !m_topbt ){
		return 0;
	}

	CreateBtConstraintReq(limitdegflag, m_topbt);
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


void CModel::CreateBtConstraintReq(bool limitdegflag, CBtObject* curbto)
{
	if(curbto->GetTopFlag() == 0){
		CallF(curbto->CreateBtConstraint(limitdegflag), return);
	}

	int btono;
	for(btono = 0; btono < (int)curbto->GetChildBtSize(); btono++){
		CBtObject* chilbto = curbto->GetChildBt(btono);
		if( chilbto ){
			CreateBtConstraintReq(limitdegflag, chilbto);
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


int CModel::CreateBtObject(bool limitdegflag, int onfirstcreate )
{

	DestroyBtObject();

	if(!GetTopBone()){
		return 0;
	}
	if (GetNoBoneFlag()) {
		return 0;
	}
	if (!m_btWorld){
		return 0;
	}

	CalcBtAxismat(1);//!!!!!!!!!!!!!


	m_topbt = new CBtObject( 0, m_btWorld );
	if( !m_topbt ){
		_ASSERT( 0 );
		return 1;
	}
	m_topbt->SetTopFlag( 1 );
	//m_topbt->CreateObject(NULL, NULL, m_topbone, m_topbone->GetChild());


	m_rigidbone.clear();

	CBone* topbone = GetTopBone(false);
	if (topbone){
		//CreateBtObjectReq側でeNullは除外する
		CBone* childbone = topbone->GetChild(false);
		while (childbone) {
			CreateBtObjectReq(limitdegflag, m_topbt, topbone, childbone);
			//CreateBtObjectReq(NULL, startbone, startbone->GetChild());
			childbone = childbone->GetBrother(false);
		}
	}

	CreateBtConstraint(limitdegflag);


	//if( m_topbt ){
	//	SetBtKinFlagReq( m_topbt, onfirstcreate );
	//}


	if (g_previewFlag != 5){
		SetBtKinFlagReq(m_topbt, 1);
		 //RestoreMassReq(m_topbone);
	}
	else{
		//SetRagdollKinFlagReq(m_topbt, -1);
		//CreatePhysicsPosConstraintReq(m_topbone);
		//bool forceflag = false;
		//SetMass0Req(m_topbone, forceflag);
	}


	if (m_topbt){
		SetBtEquilibriumPointReq(limitdegflag, m_topbt);
	}


	////for debug
	//DumpBtObjectReq(m_topbt, 0);
	//DbgOut(L"\r\n\r\n");
	//DumpBtConstraintReq(m_topbt, 0);
	//DbgOut(L"\r\n\r\n");


	SetCreateBtFlag(true);

	return 0;          
}


int CModel::SetBtEquilibriumPoint(bool limitdegflag)
{
	if (m_topbt) {
		SetBtEquilibriumPointReq(limitdegflag, m_topbt);
	}
	return 0;
}

int CModel::SetBtEquilibriumPointReq(bool limitdegflag, CBtObject* srcbto)
{
	if (!srcbto){
		return 0;
	}

	//`角度、位置
	if (g_previewFlag != 5) {
		srcbto->EnableSpring(true, true);
	}
	else {
		srcbto->EnableSpring(false, false);
		//srcbto->EnableSpring(true, true);
		//srcbto->EnableSpring(false, true);
	}
	//srcbto->EnableSpring(false, true);
	//srcbto->EnableSpring(true, false);

	int lflag, aflag;
	aflag = 1;
	lflag = 0;
	srcbto->SetEquilibriumPoint(limitdegflag, lflag, aflag);

	int childno;
	for (childno = 0; childno < srcbto->GetChildBtSize(); childno++){
		CBtObject* childbto = srcbto->GetChildBt(childno);
		if (childbto){
			SetBtEquilibriumPointReq(limitdegflag, childbto);
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
	for (childno = 0; childno < srcbto->GetChildBtSize(); childno++){
		CBtObject* childbto = srcbto->GetChildBt(childno);
		if (childbto){
			SetDofRotAxisReq(childbto, srcaxiskind);
		}
	}

	return;
}



void CModel::CreateBtObjectReq(bool limitdegflag, CBtObject* parbt, CBone* notuseparentbone, CBone* curbone)
{
	if (!curbone){
		return;
	}
	//if (!parentbone){
	//	return;
	//}

	bool createdflag = false;
	CBtObject* newbto = 0;

	if (curbone->IsSkeleton()) {
		CBone* parentbone = curbone->GetParent(false);
		if (parentbone && 
			(parentbone->IsSkeleton() || (curbone->IsHipsBone()))) {//childがhipsの場合は　parentはeNullも可
			CRigidElem* curre;
			curre = parentbone->GetRigidElem(curbone);

			CBone* childbone = 0;

			if (curre) {
				newbto = new CBtObject(parbt, m_btWorld);
				if (!newbto) {
					_ASSERT(0);
					return;
				}
				createdflag = true;


				if (curre->GetSkipflag() == 0) {
					m_btovec.push_back(newbto);//2024/06/16
				}


				//if (!m_topbt){
				//	m_topbt = newbto;
				//	m_topbt->SetTopFlag( 1 );
				//}

				if (parbt) {
					parbt->AddChild(newbto);

					int curmotid;
					double curframe;
					if (ExistCurrentMotion()) {
						curmotid = GetCurrentMotID();
						curframe = RoundingTime(GetCurrentFrame());
						CallF(newbto->CreateObject(limitdegflag, curmotid, curframe, parbt, parentbone->GetParent(false), parentbone, curbone), return);
					}
					parentbone->SetBtObject(curbone, newbto);

					//if (parbt->GetBone() && parbt->GetEndBone()){
					//	DbgOut(L"checkbt2 : CreateBtObject : parbto %s---%s, curbto %s---%s\r\n",
					//		parbt->GetBone()->GetWBoneName(), parbt->GetEndBone()->GetWBoneName(),
					//		newbto->GetBone()->GetWBoneName(), newbto->GetEndBone()->GetWBoneName());
					//}
				}
			}
		}

	}

	if (curbone->GetChild(false)){
		if (createdflag == true) {
			CreateBtObjectReq(limitdegflag, newbto, 0, curbone->GetChild(false));
		}
		else {
			CreateBtObjectReq(limitdegflag, parbt, 0, curbone->GetChild(false));
		}
	}
	if (curbone->GetBrother(false)){
		CreateBtObjectReq(limitdegflag, parbt, 0, curbone->GetBrother(false));
	}
}

int CModel::SetBtObjectVec()
{
	m_btovec.clear();
	SetBtObjectVecReq(GetTopBt());
	return 0;
}
void CModel::SetBtObjectVecReq(CBtObject* srcbto)
{
	if (srcbto) {
		CBone* srcbone = srcbto->GetBone();
		CBone* childbone = srcbto->GetEndBone();
		if (srcbone && childbone && !srcbone->GetSkipRenderBoneMark() && srcbone->IsSkeleton() && childbone->IsSkeleton()) {//2024/06/16 IsSkeleton()
			CRigidElem* curre = srcbone->GetRigidElem(childbone);
			if (curre && (curre->GetSkipflag() == 0)) {
				if ((strstr(childbone->GetBoneName(), "J_Sec_L_CoatSkirt") != 0) ||
					(strstr(childbone->GetBoneName(), "J_Sec_R_CoatSkirt") != 0))
				{
					curre->SetSkipflag(1);//足の横にあるスカート用のジョイントの剛体は自動でスキップ(Invalidate)
				}
				else {
					m_btovec.push_back(srcbto);//2024/06/16
				}
			}
		}

		int childno;
		for (childno = 0; childno < srcbto->GetChildBtSize(); childno++) {
			CBtObject* childbto = srcbto->GetChildBt(childno);
			if (childbto) {
				SetBtObjectVecReq(childbto);
			}
		}
	}
}





//void CModel::CalcRigidElem()
//{
//	CalcRigidElemReq(GetTopBone(false));
//}
//
//
//void CModel::CalcRigidElemReq(CBone* curbone)
//{
//	if (!curbone) {
//		return;
//	}
//
//	int onfirstcreate = 1;
//	bool setinstancescale = false;
//
//
//	//int setstartflag;
//	//if (onfirstcreate != 0) {
//		if ((curbone->IsSkeleton()) && curbone->GetParent(false) && 
//			(curbone->GetParent(false)->IsSkeleton() || curbone->IsHipsBone())) {//childがhipsの場合は parentはeNullも可
//
//			curbone->GetParent(false)->CalcRigidElemParams(setinstancescale, curbone, onfirstcreate);//firstflag 1
//		}
//		////curbone->SetStartMat2( curbone->GetCurMp().GetWorldMat() );
//		//curbone->SetStartMat2(curbone->GetCurrentZeroFrameMat(0));
//	//}
//
//	if (curbone->GetChild(false)) {
//		CalcRigidElemReq(curbone->GetChild(false));
//	}
//	if (curbone->GetBrother(false)) {
//		CalcRigidElemReq(curbone->GetBrother(false));
//	}
//
//
//}

int CModel::CalcRigidElemParamsOnBt()
{
	if (!GetTopBt()) {
		return 0;
	}
	if (!GetBtWorld()) {
		return 0;
	}

	int firstflag = 0;//OnBtのときには　firstflag = 0 !!!! CalcRigidElem()との違い
	bool setinstancescale = false;

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* boneptr = itrbone->second;
		if (boneptr && boneptr->GetParent(false) && (boneptr->IsSkeleton())) {
			CRigidElem* curre = boneptr->GetParent(false)->GetRigidElem(boneptr);
			if (curre) {
				//bool calcslotflag = true;
				bool calcslotflag = false;//2024/03/15 Motion2Bt()から呼ばれる場合　参照すべきcurmpは前回の結果
				boneptr->GetParent(false)->CalcRigidElemParams(setinstancescale, boneptr, firstflag, calcslotflag);
			}
		}
	}

	return 0;

}

void CModel::CalcBtAxismat(int onfirstcreate)
{
	CalcBtAxismatReq(GetTopBone(false), onfirstcreate);
	return;
}


void CModel::CalcBtAxismatReq( CBone* curbone, int onfirstcreate )
{
	if (!curbone){
		return;
	}
	//int setstartflag;

	bool setinstancescale = false;

	if( onfirstcreate != 0 ){
		if ((curbone->IsSkeleton()) && curbone->GetParent(false) &&
			(curbone->GetParent(false)->IsSkeleton() || curbone->IsHipsBone())) {//childがhipsの場合は parentはeNullも可
			bool calcslotflag = true;
			curbone->GetParent(false)->CalcRigidElemParams(setinstancescale, curbone, onfirstcreate, calcslotflag);//firstflag 1
		}
		////curbone->SetStartMat2( curbone->GetCurMp().GetWorldMat() );
		//curbone->SetStartMat2(curbone->GetCurrentZeroFrameMat(0));
	}

	if( curbone->GetChild(false) ){
		CalcBtAxismatReq(curbone->GetChild(false), onfirstcreate);
	}
	if( curbone->GetBrother(false) ){
		CalcBtAxismatReq(curbone->GetBrother(false), onfirstcreate);
	}
}


int CModel::SetBtMotionOnBt(bool limitdegflag,
	double srcframe, ChaMatrix* vmat, ChaMatrix* pmat)//, int updateslot)
{
	if (!vmat || !pmat) {
		_ASSERT(0);
		return 1;
	}

	//m_matWorld = *wmat;//そのまま
	m_matView = *vmat;
	m_matProj = *pmat;
	m_matVP = m_matView * m_matProj;

	if (!m_topbt) {
		//_ASSERT( 0 );
		return 0;
	}
	if (!m_btWorld) {
		return 0;
	}
	if (!ExistCurrentMotion()) {
		_ASSERT(0);
		return 0;//!!!!!!!!!!!!
	}

	int curmotid = GetCurrentMotID();
	double curframe = GetCurrentFrame();

	ChaMatrix inimat;
	ChaMatrixIdentity(&inimat);
	CQuaternion iniq;
	iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone) {
			//CMotionPoint curmp = curbone->GetCurMp();
			curbone->SetBtFlag(0);
			//curbone->SetCurMp( curmp );
		}
	}

	SetBtMotionReq(limitdegflag, m_topbt, &m_matWorld, vmat, pmat);

	//#########################################################################################
	//2022/07/09
	// endjointが頂点に影響度を持つ場合、物理時に動かないendjointに頂点が引っ張られる不具合解消
	//2022/07/11
	// SetShaderConstから本関数SetBtMotionに処理を移動。
	//#########################################################################################

	double roundingframe = RoundingTime(curframe);

	map<int, CBone*>::iterator itrbone2;
	for (itrbone2 = m_bonelist.begin(); itrbone2 != m_bonelist.end(); itrbone2++) {
		CBone* curbone2 = itrbone2->second;
		if (curbone2 && (curbone2->IsSkeleton())) {
			if ((curbone2->GetChild(false) == NULL) || (curbone2->GetChild(false)->IsNull())) {//2023/05/09 eNullの場合も
				if (curbone2->GetParent(false)) {
					//2023/05/09
					//Kinematic == falseの場合だけ　BtMatはセットされている
					//Kinematic == trueの場合には　BtMatはセットされていない

					if ((curbone2->GetParent(false)->GetBtKinFlag() != 0) ||
						(curbone2->GetParent(false)->GetTmpKinematic() == true)) {

						//parentがkinematicの場合　worldmat, limitedworldmatをセット
						bool calcslotflag = true;
						//bool calcslotflag = false;
						CMotionPoint tmpmp2 = curbone2->GetParent(false)->GetCurMp(calcslotflag);//motid, curframeを参照してもうなくいかない。GetCurMpを使う。
						if (limitdegflag == false) {
							curbone2->SetBtMat(tmpmp2.GetWorldMat());
						}
						else {
							curbone2->SetBtMat(tmpmp2.GetLimitedWM());
						}

					}
					else {
						//parentが　simuの場合　btmatをセット
						curbone2->SetBtMat(curbone2->GetParent(false)->GetBtMat(true));
					}
				}
				else {
					bool calcslotflag = true;
					//bool calcslotflag = false;
					CMotionPoint tmpmp4 = curbone2->GetCurMp(calcslotflag);//motid, curframeを参照してもうなくいかない。GetCurMpを使う。
					if (limitdegflag == false) {
						curbone2->SetBtMat(tmpmp4.GetWorldMat());
					}
					else {
						curbone2->SetBtMat(tmpmp4.GetLimitedWM());
					}
				}
			}
		}
	}

	if (limitdegflag == true) {
		//2024/04/13メモ
		//物理を柔らかく揺らすために剛体の結合を柔らかく設定した
		//剛体の接続を柔らかく設定した影響で物理の角度制限が効かなくなっていた
		//よって一度は取り下げたが自前の角度制限を物理に対しても適用することにした
		// 
		//制限角度計算はPhysIKRec()よりも前に処理する必要がある
		//よってSetBtMotionOnBt()の最後に処理を行う
		bool setchildflag = true;
		LimitBtMatReq(limitdegflag, setchildflag, GetTopBone(false));
	}

	return 0;
}


int CModel::SetBtMotion(bool limitdegflag, CBone* srcbone, int ragdollflag, 
	double srcframe, ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat)
{
	m_matWorld = *wmat;
	m_matView = *vmat;
	m_matProj = *pmat;
	m_matVP = m_matView * m_matProj;

	if( !m_topbt ){
		//_ASSERT( 0 );
		return 0;
	}
	if (!m_btWorld){
		return 0;
	}

	//morphアニメがあるかもしれないので、ボーンが無くてもリターンしない
	//if( !ExistCurrentMotion() ){
	//	_ASSERT( 0 );
	//	return 0;//!!!!!!!!!!!!
	//}

	int curmotid = GetCurrentMotID();
	double curframe = GetCurrentFrame();


	if (GetNoBoneFlag() == false) {
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		CQuaternion iniq;
		iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);



		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone) {
				//CMotionPoint curmp = curbone->GetCurMp();
				curbone->SetBtFlag(0);
				//curbone->SetCurMp( curmp );
			}
		}

		SetBtMotionReq(limitdegflag, m_topbt, wmat, vmat, pmat);

		//if (g_previewFlag == 5) {
		//	//物理IK用
		//	
		//	//SetBtMotionPostReq(m_topbt, wmat, vpmat);
		//	if (srcbone && srcbone->GetParent()) {
		//		CBtObject* startbto = srcbone->GetParent()->GetBtObject(srcbone);
		//		SetBtMotionPostLowerReq(startbto, wmat, vpmat, 1);//kinematicadjust = 0
		//		SetBtMotionPostUpperReq(startbto, wmat, vpmat);
		//		//SetBtMotionPostLowerReq(m_topbt, wmat, vpmat, 0);//mass0adjust = 0
		//		SetBtMotionMass0BottomUpReq(m_topbt, wmat, vpmat);//for mass0
		//		SetBtMotionPostLowerReq(m_topbt, wmat, vpmat, 0);//kinematicadjust = 0

		//		FindAndSetKinematicReq(m_topbt, wmat, vpmat);//Kinematicとそうでないところの境目を探してみつかったらLowerReqで親行列をセットする。

		//		//InitBtMatTraAnimReq(m_topbt);//2022/12/15 comment out

		//		
		//		BtMat2BtObjReq(m_topbt, wmat, vpmat);
		//		//RecalcConstraintFrameABReq(m_topbt);

		//		m_physicsikcnt++;
		//	}
		//}
		//else {
		//	//if (srcbone && srcbone->GetParent()) {
		//	//	RecalcConstraintFrameABReq(m_topbt);
		//	//}
		//}


		//#########################################################################################
		//2022/07/09
		// endjointが頂点に影響度を持つ場合、物理時に動かないendjointに頂点が引っ張られる不具合解消
		//2022/07/11
		// SetShaderConstから本関数SetBtMotionに処理を移動。
		//#########################################################################################

		double roundingframe = RoundingTime(curframe);

		map<int, CBone*>::iterator itrbone2;
		for (itrbone2 = m_bonelist.begin(); itrbone2 != m_bonelist.end(); itrbone2++) {
			CBone* curbone2 = itrbone2->second;
			if (curbone2 && (curbone2->IsSkeleton())) {
				if ((curbone2->GetChild(false) == NULL) || (curbone2->GetChild(false)->IsNull())) {//2023/05/09 eNullの場合も
					if (curbone2->GetParent(false)) {
						//2023/05/09
						//Kinematic == falseの場合だけ　BtMatはセットされている
						//Kinematic == trueの場合には　BtMatはセットされていない

						if ((curbone2->GetParent(false)->GetBtKinFlag() != 0) ||
							(curbone2->GetParent(false)->GetTmpKinematic() == true)) {

							//parentがkinematicの場合　worldmat, limitedworldmatをセット
							bool calcslotflag = true;
							//bool calcslotflag = false;
							CMotionPoint tmpmp2 = curbone2->GetParent(false)->GetCurMp(calcslotflag);//motid, curframeを参照してもうなくいかない。GetCurMpを使う。
							if (limitdegflag == false) {
								curbone2->SetBtMat(tmpmp2.GetWorldMat());
							}
							else {
								curbone2->SetBtMat(tmpmp2.GetLimitedWM());
							}

						}
						else {
							//parentが　simuの場合　btmatをセット
							curbone2->SetBtMat(curbone2->GetParent(false)->GetBtMat(true));
						}
					}
					else {
						bool calcslotflag = true;
						//bool calcslotflag = false;
						CMotionPoint tmpmp4 = curbone2->GetCurMp(calcslotflag);//motid, curframeを参照してもうなくいかない。GetCurMpを使う。
						//curbone2->SetBtMat(tmpmp4.GetWorldMat());
						if (limitdegflag == false) {
							curbone2->SetBtMat(tmpmp4.GetWorldMat());
						}
						else {
							curbone2->SetBtMat(tmpmp4.GetLimitedWM());
						}
					}
				}
			}
		}
	}


	if (GetInMorph()) {
		//2024/05/22 BlendShapeプレートメニューのblendshapeDistスライダーによる指定距離内の場合にモーフ再生
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			if (!(curobj->EmptyShape())) {
				CallF(curobj->UpdateMorphWeight(curmotid, IntTime(curframe)), return 1);
				CallF(curobj->UpdateMorphBuffer(), return 1);
			}
		}
	}

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
				if( !(curobj->EmptyShape()) ){
					GetShapeWeight( m_fbxobj[curobj].node, m_fbxobj[curobj].mesh, lTime, mCurrentAnimLayer, curobj );
					CallF( curobj->UpdateMorphBuffer(), return 1 );
				}
			}
		}
	}
	*/
	return 0;
}

void CModel::LimitBtMatReq(bool limitdegflag, bool setchildflag, CBone* srcbone)
{
	//2024/04/13メモ
	//物理を柔らかく揺らすために剛体の結合を柔らかく設定した
	//剛体の接続を柔らかく設定した影響で物理の角度制限が効かなくなっていた
	//よって一度は取り下げたが自前の角度制限を物理に対しても適用することにした

	if (srcbone) {

		if ((srcbone->GetChild(false)) &&
			//((srcbone->GetBtKinFlag() == 0) || (srcbone->GetTmpKinematic() == false))) {
			(srcbone->GetBtKinFlag() == 0)) {
			ChaMatrix curbtmat = srcbone->GetBtMat(true);
			bool directsetflag = false;
			srcbone->SetBtMatLimited(limitdegflag, directsetflag, setchildflag, curbtmat);
		}

		if (srcbone->GetChild(false)) {
			LimitBtMatReq(limitdegflag, setchildflag, srcbone->GetChild(false));
		}
		if (srcbone->GetBrother(false)) {
			LimitBtMatReq(limitdegflag, setchildflag, srcbone->GetBrother(false));
		}
	}
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
		if(curbone && curbone->IsSkeleton()){
			CBone* parentbone = curbone->GetParent(false);
			if(parentbone && (parentbone->IsSkeleton() || curbone->IsHipsBone())){//childがhipsの場合は parentはeNullも可
				CRigidElem* curre = parentbone->GetRigidElem(curbone);
				if (curre) {

					float newdampl = curre->GetLDamping() - (float)diffframe * curre->GetDampanimL() * curre->GetLDamping();
					if (newdampl < minval) {
						newdampl = minval;
					}
					float newdampa = curre->GetADamping() - (float)diffframe * curre->GetDampanimA() * curre->GetADamping();
					if (newdampa < minval) {
						newdampa = minval;
					}


					CBtObject* curbto = FindBtObject(curbone->GetBoneNo());
					if (curbto) {
						int constraintnum = curbto->GetConstraintSize();
						int constraintno;
						for (constraintno = 0; constraintno < constraintnum; constraintno++) {
							btGeneric6DofSpringConstraint* curct = curbto->GetConstraint(constraintno);
							if (curct) {
								int dofid;
								for (dofid = 0; dofid < 3; dofid++) {
									curct->setDamping( dofid, newdampl );
								}
								for (dofid = 3; dofid < 6; dofid++) {
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


//void CModel::InitBtMatTraAnimReq(CBtObject* curbto)
//{
//	if (!curbto) {
//		return;
//	}
//
//	if ((curbto->GetTopFlag() == 0) && curbto->GetBone()) {
//		CBone* curbone = curbto->GetBone();
//
//		ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
//		int paraxsiflag1 = 1;
//		//int isfirstbone = 0;
//		cureul = curbone->CalcBtLocalEulXYZ(paraxsiflag1, BEFEUL_BEFFRAME);
//
//		int inittraflag1 = 1;
//		int setchildflag1 = 1;
//		curbone->SetBtWorldMatFromEul(setchildflag1, cureul);
//
//	}
//	int chilno;
//	for (chilno = 0; chilno < curbto->GetChildBtSize(); chilno++) {
//		CBtObject* chilbto = curbto->GetChildBt(chilno);
//		if (chilbto) {
//			InitBtMatTraAnimReq(chilbto);
//		}
//	}
//}


void CModel::SetBtMotionReq(bool limitdegflag, CBtObject* curbto, 
	ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat)
{
	if (!curbto) {
		return;
	}

	if (ExistCurrentMotion()) {
		int curmotid = GetCurrentMotID();
		double curframe = GetCurrentFrame();//curframe : 時間補間有り

		if ((curbto->GetTopFlag() == 0) && curbto->GetBone()) {
			CBone* curbone = curbto->GetBone();
			CBone* childbone = curbto->GetEndBone();



		//Motion側の　traanimを取得する
			ChaMatrix curwm;
			curwm.SetIdentity();
			ChaMatrix curtraanim;
			curtraanim.SetIdentity();
			{
				//if (curbone->IsHipsBone() && (curframe >= 50.0)) {
				//	_ASSERT(0);//for debug
				//}

				//ChaMatrix curwm = curbone->GetWorldMat(curmotid, curframe);
				//ChaMatrix curwm = curbone->GetCurMp().GetWorldMat();
				curwm = curbone->GetCurrentWorldMat(true, true);

				ChaMatrix parentwm;
				parentwm.SetIdentity();
				if (curbone->GetParent(false)) {
					//parentwm = curbone->GetParent()->GetWorldMat(curmotid, curframe);
					//parentwm = curbone->GetParent()->GetCurMp().GetWorldMat();
					parentwm = curbone->GetParent(false)->GetCurrentWorldMat(true, true);
				}
				else {
					parentwm.SetIdentity();
				}
				ChaMatrix curlocalmat;
				curlocalmat = curwm * ChaMatrixInv(parentwm);

				ChaMatrix smat, rmat, tmat;
				GetSRTandTraAnim(curlocalmat, curbone->GetNodeMat(), &smat, &rmat, &tmat, &curtraanim);
			}

			if (curbone) {
				if (curbone->GetBtKinFlag() == 0) {//2023/01/28
					if (g_previewFlag == 4) {
						curbto->SetBtMotion(limitdegflag, curtraanim);
					}
					else if (g_previewFlag == 5) {
						curbto->SetBtMotion(limitdegflag, curtraanim);
					}
				}
				else {
					curbone->SetBtMat(curwm);
					curbone->SetBtFlag(1);
				}
			}

			/*
			ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
			int paraxsiflag1 = 1;
			//int isfirstbone = 0;
			cureul = curbone->CalcBtLocalEulXYZ(paraxsiflag1, BEFEUL_ZERO);

			int inittraflag1 = 1;
			int setchildflag1 = 1;
			curbone->SetBtWorldMatFromEul(setchildflag1, cureul);//SetBtWorldMatFromEul関数内で設定部分がコメントアウトされていた
			*/
		}
	}


	int chilno;
	for( chilno = 0; chilno < curbto->GetChildBtSize(); chilno++ ){
		CBtObject* chilbto = curbto->GetChildBt( chilno );
		if( chilbto ){
			SetBtMotionReq(limitdegflag, chilbto, wmat, vmat, pmat);
		}
	}

}

////adjustrot = 0
//void CModel::AdjustBtMatToParent(CBone* curbone, CBone* childbone, int adjustrot)
//{
//	if (!curbone) {
//		return;
//	}
//	if (!curbone->GetParent()) {
//		return;
//	}
//	if (!childbone) {
//		return;
//	}
//
//	ChaMatrix parentmat = curbone->GetParent()->GetBtMat();
//	ChaMatrix currentmat = curbone->GetBtMat();
//	ChaMatrix smat, rmat, tmat;
//	GetSRTMatrix2(currentmat, &smat, &rmat, &tmat);
//
//	ChaVector3 curpos1, curpos2;
//	ChaVector3 tmpfpos = curbone->GetJointFPos();
//	ChaMatrix tmpbtmat = curbone->GetBtMat();
//	ChaMatrix tmpparentbtmat = curbone->GetParent()->GetBtMat();
//	ChaVector3TransformCoord(&curpos1, &tmpfpos, &tmpbtmat);
//	ChaVector3TransformCoord(&curpos2, &tmpfpos, &tmpparentbtmat);
//	ChaVector3 adjustvec = curpos2 - curpos1;
//	//ChaVector3 adjustvec.SetParams(0.0, 0.0, 0.0);//for debug
//
//	if (adjustrot == 1) {
//		ChaVector3 childpos;
//		ChaVector3 tmpchildfpos = childbone->GetJointFPos();
//		ChaMatrix tmpbtmat2 = curbone->GetBtMat();
//		ChaVector3TransformCoord(&childpos, &tmpchildfpos, &tmpbtmat2);
//
//		ChaVector3 oldbonevec = childpos - curpos1;
//		ChaVector3Normalize(&oldbonevec, &oldbonevec);
//
//		ChaVector3 newcurpos;
//		newcurpos = curpos1 + adjustvec;
//		ChaVector3 newbonevec = childpos - newcurpos;
//		ChaVector3Normalize(&newbonevec, &newbonevec);
//
//		CQuaternion addrotq;
//		addrotq.RotationArc(oldbonevec, newbonevec);
//
//		rmat = rmat * addrotq.MakeRotMatX();
//
//		currentmat = smat * rmat * tmat;
//		//ChaVector3 tmpfpos = curbone->GetJointFPos();
//		ChaVector3TransformCoord(&curpos1, &tmpfpos, &currentmat);
//		adjustvec = curpos2 - curpos1;
//		GetSRTMatrix2(currentmat, &smat, &rmat, &tmat);
//	}
//
//	if (ChaVector3LengthDbl(&adjustvec) >= 0.050f) {
//		tmat.data[MATI_41] += adjustvec.x;
//		tmat.data[MATI_42] += adjustvec.y;
//		tmat.data[MATI_43] += adjustvec.z;
//
//		ChaMatrix newmat;
//		newmat = smat * rmat * tmat;
//
//		curbone->SetBtMat(newmat);
//		curbone->SetBtFlag(1);
//	}
//}
//
////int adjustrot = 0
//void CModel::AdjustBtMatToCurrent(CBone* curbone)
//{
//	if (!curbone) {
//		return;
//	}
//	if (!curbone->GetParent()) {
//		return;
//	}
//
//	ChaMatrix parentmat = curbone->GetParent()->GetBtMat();
//	ChaMatrix currentmat = curbone->GetBtMat();
//	ChaMatrix smat, rmat, tmat;
//	GetSRTMatrix2(parentmat, &smat, &rmat, &tmat);
//
//	ChaVector3 basepos, curpos;
//	ChaVector3 tmpfpos = curbone->GetJointFPos();
//	ChaMatrix tmpbtmat = curbone->GetBtMat();
//	ChaMatrix tmpparentbtmat = curbone->GetParent()->GetBtMat();
//	ChaVector3TransformCoord(&basepos, &tmpfpos, &tmpparentbtmat);
//	ChaVector3TransformCoord(&curpos, &tmpfpos, &tmpbtmat);
//	ChaVector3 adjustvec = curpos - basepos;
//	//ChaVector3 adjustvec.SetParams(0.0, 0.0, 0.0);//for debug
//
//	if (ChaVector3LengthDbl(&adjustvec) >= 0.050f) {
//		tmat.data[MATI_41] += adjustvec.x;
//		tmat.data[MATI_42] += adjustvec.y;
//		tmat.data[MATI_43] += adjustvec.z;
//
//		ChaMatrix newmat;
//		newmat = smat * rmat * tmat;
//
//		curbone->GetParent()->SetBtMat(newmat);
//		curbone->GetParent()->SetBtFlag(1);
//	}
//}
//
////int adjustrot = 0
//void CModel::AdjustBtMatToChild(CBone* curbone, CBone* childbone, int adjustrot)
//{
//	if (!curbone) {
//		return;
//	}
//	if (!curbone->GetParent()) {
//		return;
//	}
//	if (!childbone) {
//		return;
//	}
//	ChaMatrix parentmat = curbone->GetParent()->GetBtMat();
//	ChaMatrix currentmat = curbone->GetBtMat();
//	ChaMatrix smat, rmat, tmat;
//	GetSRTMatrix2(currentmat, &smat, &rmat, &tmat);
//
//	ChaVector3 childpos1, childpos2;
//	ChaVector3 tmpchildfpos = childbone->GetJointFPos();
//	ChaMatrix tmpbtmat = curbone->GetBtMat();
//	ChaMatrix tmpchildbtmat = childbone->GetBtMat();
//	ChaVector3TransformCoord(&childpos1, &tmpchildfpos, &tmpbtmat);
//	ChaVector3TransformCoord(&childpos2, &tmpchildfpos, &tmpchildbtmat);
//	ChaVector3 adjustvec = childpos2 - childpos1;
//	//ChaVector3 adjustvec.SetParams(0.0, 0.0, 0.0);//for debug
//
//	if (adjustrot == 1) {
//		ChaVector3 curpos;
//		ChaVector3 tmpfpos = curbone->GetJointFPos();
//		ChaVector3TransformCoord(&curpos, &tmpfpos, &tmpbtmat);
//
//		ChaVector3 oldbonevec = childpos1 - curpos;
//		ChaVector3Normalize(&oldbonevec, &oldbonevec);
//
//		ChaVector3 newchildpos;
//		newchildpos = childpos1 + adjustvec;
//		ChaVector3 newbonevec = newchildpos - curpos;
//		ChaVector3Normalize(&newbonevec, &newbonevec);
//
//		CQuaternion addrotq;
//		addrotq.RotationArc(oldbonevec, newbonevec);
//
//		rmat = rmat * addrotq.MakeRotMatX();
//
//		currentmat = smat * rmat * tmat;
//		ChaVector3TransformCoord(&childpos1, &tmpchildfpos, &currentmat);
//		adjustvec = childpos2 - childpos1;
//		GetSRTMatrix2(currentmat, &smat, &rmat, &tmat);
//	}
//
//
//	if (ChaVector3LengthDbl(&adjustvec) >= 0.050f) {
//		tmat.data[MATI_41] += adjustvec.x;
//		tmat.data[MATI_42] += adjustvec.y;
//		tmat.data[MATI_43] += adjustvec.z;
//
//		ChaMatrix newmat;
//		newmat = smat * rmat * tmat;
//
//		curbone->SetBtMat(newmat);
//		curbone->SetBtFlag(1);
//	}
//}
//
//
////Post処理。SetBtMotionReqを呼び出した後で呼び出す。
//void CModel::SetBtMotionPostLowerReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat, int kinematicadjustflag)
//{
//	//後処理
//	MOTINFO* curmi = 0;
//	int curmotid;
//	double curframe;
//	curmi = GetCurMotInfo();
//	if (!curmi) {
//		_ASSERT(0);
//		return;
//	}
//
//	curmotid = curmi->motid;
//	curframe = RoundingTime(curmi->curframe);
//
//
//	if ((curbto->GetTopFlag() == 0) && curbto->GetBone()) {
//		CBone* curbone = curbto->GetBone();
//		if (curbone && curbone->GetParent()) {
//			if (g_previewFlag == 4) {
//				if (curbone->GetBtKinFlag() == 1) {
//					CMotionPoint curmp = curbone->GetCurMp();
//					curbone->SetBtMat(curbone->GetWorldMat(curmotid, curframe, &curmp));
//					curbone->SetBtFlag(1);
//				}
//			}
//			else if (g_previewFlag == 5) {
//				//GetCurMp().GetWorldMat()には物理IK開始時の姿勢が入っている。
//				if ((curbone->GetBtFlag() == 0) || ((curbone->GetMass0() == FALSE) && (curbone->GetParent()->GetTmpKinematic() == true))) {//mass0とkinematic併用の場合にはmass0を適用
//				//if ((curbone->GetBtFlag() == 0) || ((curbone->GetMass0() == FALSE) && (curbone->GetTmpKinematic() == true))) {//mass0とkinematic併用の場合にはmass0を適用
//					if (kinematicadjustflag == 1) {
//						if (curbone->GetParent()) {
//
//							//2023/02/02 後で修正必要　btmatとGetCurMpには　モデルのworldmatが掛かっている
//
//							ChaMatrix newparmat = curbone->GetParent()->GetBtMat();
//							ChaMatrix firstparmat = curbone->GetParent()->GetCurMp().GetWorldMat();
//							ChaMatrix newcurmat = curbone->GetCurMp().GetWorldMat() * ChaMatrixInv(firstparmat) * newparmat;
//							curbone->SetBtMat(newcurmat);
//							curbone->SetBtFlag(1);
//						}
//						else {
//							curbone->SetBtMat(curbone->GetCurMp().GetWorldMat());
//							curbone->SetBtFlag(1);
//						}
//					}
//				}
//				else if ((curbone->GetBtFlag() == 1) && (curbone->GetParent()->GetBtFlag() == 1) && 
//					((curbone->GetParent()->GetTmpKinematic() == false) || (curbone->GetMass0() == TRUE))) {//mass0とkinematic併用の場合にはmass0を適用
//					//((curbone->GetTmpKinematic() == false) || (curbone->GetMass0() == TRUE))) {//mass0とkinematic併用の場合にはmass0を適用
//					//親の剛体とカレントの剛体のすきま、つまり関節の隙間を無くすために補正をする。
//					if (curbto->GetParBt()) {
//						if (curbone->GetParent()->GetParent()) {
//							//CBone* childbone = curbone->GetChild();
//							CBone* childbone = curbto->GetEndBone();
//							if (childbone) {
//								if (curbone->GetMass0() == 0) {
//									int adjustrot = 0;
//									AdjustBtMatToParent(curbone, childbone, adjustrot);
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	int chilno;
//	for (chilno = 0; chilno < curbto->GetChildBtSize(); chilno++) {
//		CBtObject* chilbto = curbto->GetChildBt(chilno);
//		if (chilbto) {
//			SetBtMotionPostLowerReq(chilbto, wmat, vpmat, kinematicadjustflag);
//		}
//	}
//
//
//}
//
////Post処理。SetBtMotionReqを呼び出した後で呼び出す。
//void CModel::SetBtMotionPostUpperReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//	//後処理
//
//	MOTINFO* curmi = 0;
//	int curmotid;
//	double curframe;
//	curmi = GetCurMotInfo();
//	if (!curmi) {
//		_ASSERT(0);
//		return;
//	}
//
//	curmotid = curmi->motid;
//	curframe = RoundingTime(curmi->curframe);
//
//	if ((curbto->GetTopFlag() == 0) && curbto->GetBone()) {
//		CBone* curbone = curbto->GetBone();
//		if (curbone && curbone->GetParent()) {
//			if (g_previewFlag == 4) {
//				if (curbone->GetBtKinFlag() == 1) {
//					CMotionPoint curmp = curbone->GetCurMp();
//					curbone->SetBtMat(curbone->GetWorldMat(curmotid, curframe, &curmp));
//					curbone->SetBtFlag(1);
//				}
//			}
//			else if (g_previewFlag == 5) {
//				//GetCurMp().GetWorldMat()には物理IK開始時の姿勢が入っている。
//				if ((curbone->GetBtFlag() == 1) && (curbone->GetParent()->GetBtFlag() == 1) && 
//					((curbone->GetParent()->GetTmpKinematic() == false) || (curbone->GetMass0() == TRUE))) {//mass0とkinematic併用の場合にはmass0を適用
//					//((curbone->GetTmpKinematic() == false) || (curbone->GetMass0() == TRUE))) {//mass0とkinematic併用の場合にはmass0を適用
//					//親の剛体とカレントの剛体のすきま、つまり関節の隙間を無くすために補正をする。
//					if (curbto->GetParBt()) {
//						if (curbone->GetParent()->GetParent()) {
//							if (curbone->GetMass0() == 1) {
//							//	int adjustrot = 1;
//							//	AdjustBtMatToCurrent(curbone, adjustrot);
//							}
//							else {
//								AdjustBtMatToCurrent(curbone);
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	CBtObject* parentbto = curbto->GetParBt();
//	if (parentbto) {
//		SetBtMotionPostUpperReq(parentbto, wmat, vpmat);
//	}
//
//}
//
//
////Post処理。SetBtMotionReqを呼び出した後で呼び出す。
//void CModel::SetBtMotionMass0BottomUpReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//	//後処理
//
//	//先に階層をたどってから処理をする。つまり末端から処理を始める。
//	int chilno;
//	for (chilno = 0; chilno < curbto->GetChildBtSize(); chilno++) {
//		CBtObject* chilbto = curbto->GetChildBt(chilno);
//		if (chilbto) {
//			SetBtMotionMass0BottomUpReq(chilbto, wmat, vpmat);
//		}
//	}
//
//	if ((curbto->GetTopFlag() == 0) && curbto->GetBone()) {
//		CBone* curbone = curbto->GetBone();
//		if (curbone && curbone->GetParent()) {
//			if (g_previewFlag == 5) {
//				if (curbone->GetBtFlag() == 1) {
//					//親の剛体とカレントの剛体のすきま、つまり関節の隙間を無くすために補正をする。
//					if (curbto->GetParBt()) {
//						if (curbone->GetParent()->GetParent()) {
//							if ((curbone->GetParent()->GetTmpKinematic() == false) || (curbone->GetMass0() == TRUE)) {//mass0とkinematic併用の場合にはmass0を適用
//							//if ((curbone->GetTmpKinematic() == false) || (curbone->GetMass0() == TRUE)) {//mass0とkinematic併用の場合にはmass0を適用
//								CBone* childbone = curbto->GetEndBone();
//								if (childbone) {
//									if (childbone->GetChild()) {
//										int adjustrot = 1;//!!!!!!!!!!!!
//										AdjustBtMatToChild(curbone, childbone, adjustrot);
//									}
//									else {
//										AdjustBtMatToCurrent(curbone);
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//}
//
//
//void CModel::FindAndSetKinematicReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//	//Kinematicとそうでないところの境目を探してみつかったらLowerReqで親行列をセットする。
//
//	//先に階層をたどってから処理をする。つまり末端から処理を始める。
//	int childno;
//	for (childno = 0; childno < curbto->GetChildBtSize(); childno++) {
//		CBtObject* childbto = curbto->GetChildBt(childno);
//		if (childbto) {
//			FindAndSetKinematicReq(childbto, wmat, vpmat);
//		}
//	}
//
//	if (curbto) {
//		CBone* curbone = curbto->GetBone();
//		if (curbone) {
//			CBone* childbone = curbto->GetEndBone();
//			CBone* parentbone = curbone->GetParent();
//			if (parentbone) {
//				if ((parentbone->GetMass0() == FALSE) && (curbone->GetMass0() == FALSE)) {
//					if ((parentbone->GetTmpKinematic() == FALSE) && (curbone->GetTmpKinematic() == TRUE)) {
//						//境目を発見
//
//						//親剛体の姿勢（角度含む）の補正
//						//ChaMatrix oldparentmat = parentbone->GetBtMat();
//						//AdjustBtMatToChild(parentbone, curbone, 1);
//
//						ChaMatrix curmatrix = curbone->GetBtMat();
//						//ChaMatrix newmatrix = curmatrix * ChaMatrixInv(oldparentmat) * parentbone->GetBtMat();
//
//						//SetBtMotionKinematicLowerReq(curbto, curmatrix, curmatrix);
//
//						int childno2;
//						for (childno2 = 0; childno2 < curbto->GetChildBtSize(); childno2++) {
//							CBtObject* childbto2 = curbto->GetChildBt(childno2);
//							if (childbto2) {
//								//SetBtMotionKinematicLowerReq(childbto2, curmatrix, newmatrix);
//								SetBtMotionKinematicLowerReq(childbto2, curmatrix, curmatrix);
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//
//}
//void CModel::SetBtMotionKinematicLowerReq(CBtObject* curbto, ChaMatrix oldparentmat, ChaMatrix newparentmat)
//{
//	//LowerReqで親行列をセットする。
//	
//	if (curbto) {
//		CBone* curbone = curbto->GetBone();
//		if ((curbone->GetMass0() == FALSE) && (curbone->GetTmpKinematic() == TRUE)) {
//			ChaMatrix oldcurmat, newcurmat;
//			if (curbone && curbone->GetParent()) {
//				oldcurmat = curbone->GetBtMat();
//
//				//newcurmat = oldcurmat * ChaMatrixInv(oldparentmat) * newparentmat;//形状修正なしの場合
//
//
//				//2023/02/02 注意　後でチェック！！！　GetCurMpとBtMatにはモデルのwmが掛かっている
//				//そして　CurMpとBtMatには　モデルのwmをかけなければならない
//				//newparentmatには？？？
//
//
//				ChaMatrix curkinematicmat, parentkinematicmat;
//				curkinematicmat = curbone->GetCurMp().GetWorldMat();
//				parentkinematicmat = curbone->GetParent()->GetCurMp().GetWorldMat();
//				newcurmat = curkinematicmat * ChaMatrixInv(parentkinematicmat) * newparentmat;//形状を保持する場合
//
//				curbone->SetBtMat(newcurmat);
//				curbone->SetBtFlag(1);
//
//
//				int chilno;
//				for (chilno = 0; chilno < curbto->GetChildBtSize(); chilno++) {
//					CBtObject* chilbto = curbto->GetChildBt(chilno);
//					if (chilbto) {
//						SetBtMotionKinematicLowerReq(chilbto, oldcurmat, newcurmat);
//					}
//				}
//			}
//		}
//	}
//}
//
//void CModel::BtMat2BtObjReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//
//	if ((curbto->GetTopFlag() == 0) && curbto->GetBone()) {
//		CBone* curbone = curbto->GetBone();
//		if (curbone && curbone->GetParent()) {
//			if (curbone->GetBtFlag() == 1){
//				if (g_previewFlag == 5) {
//
//					//GetCurMp().GetWorldMat()には物理IK開始時の姿勢が入っている。
//					if ((curbone->GetMass0() == FALSE) && (curbone->GetParent()->GetTmpKinematic() == true)) {
//						ChaMatrix newparmat = curbone->GetParent()->GetBtMat();
//						ChaMatrix firstparmat = curbone->GetParent()->GetCurMp().GetWorldMat();
//						ChaMatrix newcurmat = curbone->GetCurMp().GetWorldMat() * ChaMatrixInv(firstparmat) * newparmat;
//
//						//2023/02/02 注意　後でチェック！！！　GetCurMpとBtMatにはモデルのwmが掛かっている
//						//そして　CurMpとBtMatには　モデルのwmをかけなければならない
//						//curbto->GetFirstTransformMatX()には？？？
//
//
//						ChaMatrix newbtmat;
//						newbtmat = curbto->GetFirstTransformMatX() * ChaMatrixInv(firstparmat) * newparmat;
//
//						ChaVector3 aftcurpos;
//						ChaVector3 befcurpos = curbone->GetJointFPos();
//						ChaVector3TransformCoord(&aftcurpos, &befcurpos, &newcurmat);
//						ChaVector3 aftchildpos;
//						ChaVector3 befchildpos = curbto->GetEndBone()->GetJointFPos();
//						ChaVector3TransformCoord(&aftchildpos, &befchildpos, &newcurmat);
//						ChaVector3 rigidcenter = (aftcurpos + aftchildpos) * 0.5f;
//						//ChaVector3 aftparentpos;
//						//ChaVector3 befparentpos = curbone->GetParent()->GetJointFPos();
//						//ChaVector3TransformCoord(&aftparentpos, &befparentpos, &newparmat);
//						//ChaVector3 aftchildpos;
//						//ChaVector3 befchildpos = curbone->GetJointFPos();
//						//ChaVector3TransformCoord(&aftchildpos, &befchildpos, &newcurmat);
//						//ChaVector3 rigidcenter = (aftparentpos + aftchildpos) * 0.5f;
//						curbto->SetPosture2Bt(newbtmat, rigidcenter, 0);
//
//					}
//					else {
//						if (curbto->GetParBt() && (curbone->GetBtFlag() == 1) && (curbone->GetParent()->GetBtFlag() == 1)) {
//							if (curbone->GetParent()->GetParent()) {
//								ChaMatrix newparmat = curbone->GetParent()->GetBtMat();
//								ChaMatrix newcurmat = curbone->GetBtMat();
//								//ChaMatrix invfirstcurmat = ChaMatrixInv(curbone->GetCurrentZeroFrameMat(0));
//								ChaMatrix invfirstcurmat = ChaMatrixInv(curbone->GetStartMat2());
//								ChaMatrix diffmat = invfirstcurmat * newcurmat;
//
//								ChaMatrix newbtmat;
//								newbtmat = curbto->GetFirstTransformMatX() * diffmat;
//								ChaVector3 aftcurpos;
//								ChaVector3 befcurpos = curbone->GetJointFPos();
//								ChaVector3TransformCoord(&aftcurpos, &befcurpos, &newcurmat);
//								ChaVector3 aftchildpos;
//								ChaVector3 befchildpos = curbto->GetEndBone()->GetJointFPos();
//								ChaVector3TransformCoord(&aftchildpos, &befchildpos, &newcurmat);
//								ChaVector3 rigidcenter = (aftcurpos + aftchildpos) * 0.5f;
//
//								curbto->SetPosture2Bt(newbtmat, rigidcenter, 0);
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	int chilno;
//	for (chilno = 0; chilno < curbto->GetChildBtSize(); chilno++) {
//		CBtObject* chilbto = curbto->GetChildBt(chilno);
//		if (chilbto) {
//			BtMat2BtObjReq(chilbto, wmat, vpmat);
//		}
//	}
//
//}

//void CModel::RecalcConstraintFrameABReq(CBtObject* curbto)
//{
//	if (curbto) {
//		CBone* curbone = curbto->GetBone();
//		if (curbone && curbone->GetParent()) {
//			if (g_previewFlag == 5) {
//				//if (curbto->GetParBt() && (curbone->GetBtFlag() == 1) && (curbone->GetParent()->GetBtFlag() == 1)) {
//					if (curbone->GetParent()->GetParent()) {
//						curbto->RecalcConstraintFrameAB();
//					}
//				//}
//			}
//		}
//		int chilno;
//		for (chilno = 0; chilno < curbto->GetChildBtSize(); chilno++) {
//			CBtObject* chilbto = curbto->GetChildBt(chilno);
//			if (chilbto) {
//				RecalcConstraintFrameABReq(chilbto);
//			}
//		}
//	}
//}

int CModel::CreateRigidElem()
{
	if (GetTopBone()) {
		//CreateRigidElemReq(m_topbone, 1, m_defaultrename, 1, m_defaultimpname);
		CreateRigidElemReq(GetTopBone(false), 1, m_defaultrename, 0, m_defaultimpname);
	}

	//SetCurrentRigidElem(0);
	m_curreindex = 0;
	m_curimpindex = 0;

	return 0;
}

void CModel::CreateRigidElemReq( CBone* curbone, int reflag, string rename, int impflag, string impname )
{
	if (!curbone){
		return;
	}

	if (GetNoBoneFlag() == true) {
		return;
	}

	//CBone* parentbone = curbone->GetParent();
	//if (parentbone){
	//	parentbone->CreateRigidElem(curbone, reflag, rename, impflag, impname);
	//}

	if (curbone->IsSkeleton() && curbone->GetParent(false) &&
		(curbone->GetParent(false)->IsSkeleton() || curbone->IsHipsBone())) {//childがhipsの場合は parentはeNullも可

		curbone->CreateRigidElem(curbone->GetParent(false), reflag, rename, impflag, impname);
	}

	if (curbone->GetChild(false)) {
		CreateRigidElemReq(curbone->GetChild(false), reflag, rename, impflag, impname);
	}
	if (curbone->GetBrother(false)) {
		CreateRigidElemReq(curbone->GetBrother(false), reflag, rename, impflag, impname);
	}
}

int CModel::SetBtImpulse()
{
	//if( !m_topbt ){
	//	return 0;
	//}
	//if(!GetTopBone()){
	//	return 0;
	//}

	//SetBtImpulseReq(GetTopBone());

	return 0;
}

void CModel::SetBtImpulseReq( CBone* srcbone )
{
	//CBone* parentbone = srcbone->GetParent();

	//if( parentbone ){
	//	ChaVector3 setimp( 0.0f, 0.0f, 0.0f );

	//	int impnum = parentbone->GetImpMapSize();
	//	if( (m_curimpindex >= 0) && (m_curimpindex < impnum) ){

	//		string curimpname = m_impinfo[ m_curimpindex ];
	//		map<string, map<CBone*, ChaVector3>>::iterator findimpmap;
	//		findimpmap = parentbone->FindImpMap( curimpname );
	//		if( findimpmap != parentbone->GetImpMapEnd() ){
	//			map<CBone*,ChaVector3>::iterator itrimp;
	//			itrimp = findimpmap->second.find( srcbone );
	//			if( itrimp != findimpmap->second.end() ){
	//				setimp = itrimp->second;
	//			}
	//		}
	//	}
	//	else{
	//		//_ASSERT(0);
	//	}

	//	CRigidElem* curre = parentbone->GetRigidElem( srcbone );
	//	if( curre ){
	//		ChaVector3 imp = setimp * g_impscale;

	//		CBtObject* findbto = FindBtObject( srcbone->GetBoneNo() );
	//		if( findbto && findbto->GetRigidBody() ){
	//			findbto->GetRigidBody()->applyImpulse( btVector3( imp.x, imp.y, imp.z ), btVector3( 0.0f, 0.0f, 0.0f ) );
	//		}
	//	}
	//	else{
	//		_ASSERT(0);
	//	}
	//}

	//if( srcbone->GetChild() ){
	//	SetBtImpulseReq( srcbone->GetChild() );
	//}
	//if( srcbone->GetBrother() ){
	//	SetBtImpulseReq( srcbone->GetBrother() );
	//}
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

	if(srcbto->GetBone()){
		CBone* curbone;
		curbone = m_bonelist[ srcboneno ];
		if (curbone && (curbone->IsSkeleton())) {
			CBone* parentbone;
			parentbone = curbone->GetParent(false);
			if (parentbone) {
				if( (srcbto->GetBone() == parentbone) && (srcbto->GetEndBone() == curbone) ){
					*ppret = srcbto;
					return;
				}
			}
		}
	}

	int chilno;
	for( chilno = 0; chilno < srcbto->GetChildBtSize(); chilno++ ){
		CBtObject* chilbto = srcbto->GetChildBt( chilno );
		FindBtObjectReq( chilbto, srcboneno, ppret );
	}
}

int CModel::SetLightFlag(bool srcflag)
{
	int materialnum = m_materialbank.GetSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* curmaterial = m_materialbank.Get(matindex);
		if (curmaterial) {
			curmaterial->SetLightingFlag(srcflag);
		}
		else {
			_ASSERT(0);
		}
	}

	return 0;
}

int CModel::SetDispFlag( const char* srcobjname, int srcflag )
{
	CMQOObject* curobj = m_objectname[ srcobjname ];
	if( curobj ){
		curobj->SetDispFlag( srcflag );
	}

	return 0;
}

int CModel::EnableAllRigidElem(int srcrgdindex)
{
	if (!GetTopBone()){
		return 0;
	}

	EnableAllRigidElemReq(GetTopBone(false), srcrgdindex);
	SetBtObjectVec();//2024/06/16
	return 0;
}
int CModel::DisableAllRigidElem(int srcrgdindex)
{
	if (!GetTopBone()){
		return 0;
	}

	DisableAllRigidElemReq(GetTopBone(false), srcrgdindex);
	SetBtObjectVec();//2024/06/16
	return 0;
}

void CModel::EnableAllRigidElemReq(CBone* srcbone, int srcrgdindex)
{
	if ((srcrgdindex >= 0) && (srcrgdindex < (int)m_rigideleminfo.size())){
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)){
			char* filename = m_rigideleminfo[srcrgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if (srcbone->GetChild(false)){
		EnableAllRigidElemReq(srcbone->GetChild(false), srcrgdindex);
	}
	if (srcbone->GetBrother(false)){
		EnableAllRigidElemReq(srcbone->GetBrother(false), srcrgdindex);
	}

}
void CModel::DisableAllRigidElemReq(CBone* srcbone, int srcrgdindex)
{
	if ((srcrgdindex >= 0) && (srcrgdindex < (int)m_rigideleminfo.size())){
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[srcrgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				curre->SetSkipflag(1);
			}
		}
	}
	else{
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)){
		DisableAllRigidElemReq(srcbone->GetChild(false), srcrgdindex);
	}
	if (srcbone->GetBrother(false)){
		DisableAllRigidElemReq(srcbone->GetBrother(false), srcrgdindex);
	}
}

int CModel::SetAllBtgData( int gid, int reindex, float btg )
{
	if(!GetTopBone()){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetBtgDataReq( gid, reindex, GetTopBone(false), btg );

	return 0;
}
void CModel::SetBtgDataReq( int gid, int reindex, CBone* srcbone, float btg )
{
	if (!srcbone) {
		return;
	}

	if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetBtgDataReq( gid, reindex, srcbone->GetChild(false), btg );
	}
	if( srcbone->GetBrother(false) ){
		SetBtgDataReq( gid, reindex, srcbone->GetBrother(false), btg );
	}
}


int CModel::SetAllSphrateData(int gid, int rgdindex, float srcval)
{
	if (!GetTopBone()) {
		return 0;
	}
	if (rgdindex < 0) {
		return 0;
	}

	SetSphrateDataReq(gid, rgdindex, GetTopBone(false), srcval);
	return 0;
}

//void CModel::SetColiidDataReq(int rgdindex, CBone* srcbone, int srcgroupid, std::vector<int> srccoliids, int srcmyselfflag)
void CModel::SetColiidDataReq(int rgdindex, CBone* srcbone, int srcgroupid, std::vector<int> srccoliids)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre) {
				curre->SetGroupid(srcgroupid);
				curre->CopyColiids(srccoliids);
				//curre->SetMyselfflag(srcmyselfflag);
			}
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetColiidDataReq(rgdindex, srcbone->GetChild(false), srcgroupid, srccoliids);// , srcmyselfflag);
	}
	if (srcbone->GetBrother(false)) {
		SetColiidDataReq(rgdindex, srcbone->GetBrother(false), srcgroupid, srccoliids);//, srcmyselfflag);
	}
}


void CModel::SetSphrateDataReq(int gid, int rgdindex, CBone* srcbone, float srcval)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre) {
				if ((gid == -1) || (gid == curre->GetGroupid())) {
					curre->SetSphrate(srcval);
				}
			}
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetSphrateDataReq(gid, rgdindex, srcbone->GetChild(false), srcval);
	}
	if (srcbone->GetBrother(false)) {
		SetSphrateDataReq(gid, rgdindex, srcbone->GetBrother(false), srcval);
	}
}

int CModel::SetAllBoxzrateData(int gid, int rgdindex, float srcval)
{
	if (!GetTopBone()) {
		return 0;
	}
	if (rgdindex < 0) {
		return 0;
	}

	SetBoxzrateDataReq(gid, rgdindex, GetTopBone(false), srcval);
	return 0;
}
void CModel::SetBoxzrateDataReq(int gid, int rgdindex, CBone * srcbone, float srcval)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre) {
				if ((gid == -1) || (gid == curre->GetGroupid())) {
					curre->SetBoxzrate(srcval);
				}
			}
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetBoxzrateDataReq(gid, rgdindex, srcbone->GetChild(false), srcval);
	}
	if (srcbone->GetBrother(false)) {
		SetBoxzrateDataReq(gid, rgdindex, srcbone->GetBrother(false), srcval);
	}
}

int CModel::SetAllSkipflagData(int gid, int rgdindex, int srcval)
{
	if (!GetTopBone()) {
		return 0;
	}
	if (rgdindex < 0) {
		return 0;
	}

	SetSkipflagDataReq(gid, rgdindex, GetTopBone(false), srcval);
	SetBtObjectVec();//2024/06/16
	return 0;
}
void CModel::SetSkipflagDataReq(int gid, int rgdindex, CBone* srcbone, int srcval)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre) {
				if ((gid == -1) || (gid == curre->GetGroupid())) {
					curre->SetSkipflag(srcval);
				}
			}
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetSkipflagDataReq(gid, rgdindex, srcbone->GetChild(false), srcval);
	}
	if (srcbone->GetBrother(false)) {
		SetSkipflagDataReq(gid, rgdindex, srcbone->GetBrother(false), srcval);
	}
}

int CModel::SetAllForbidrotData(int gid, int rgdindex, bool srcval)
{
	if (!GetTopBone()) {
		return 0;
	}
	if (rgdindex < 0) {
		return 0;
	}

	SetForbidrotDataReq(gid, rgdindex, GetTopBone(false), srcval);
	return 0;
}
void CModel::SetForbidrotDataReq(int gid, int rgdindex, CBone* srcbone, bool srcval)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre) {
				if ((gid == -1) || (gid == curre->GetGroupid())) {
					curre->SetForbidRotFlag(srcval);
				}
			}
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetForbidrotDataReq(gid, rgdindex, srcbone->GetChild(false), srcval);
	}
	if (srcbone->GetBrother(false)) {
		SetForbidrotDataReq(gid, rgdindex, srcbone->GetBrother(false), srcval);
	}
}

int CModel::SetAllColtypeData(int gid, int rgdindex, int srcval)
{
	if (!GetTopBone()) {
		return 0;
	}
	if (rgdindex < 0) {
		return 0;
	}

	SetColtypeDataReq(gid, rgdindex, GetTopBone(false), srcval);
	return 0;
}
void CModel::SetColtypeDataReq(int gid, int rgdindex, CBone* srcbone, int srcval)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre) {
				if ((gid == -1) || (gid == curre->GetGroupid())) {
					curre->SetColtype(srcval);
				}
			}
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetColtypeDataReq(gid, rgdindex, srcbone->GetChild(false), srcval);
	}
	if (srcbone->GetBrother(false)) {
		SetColtypeDataReq(gid, rgdindex, srcbone->GetBrother(false), srcval);
	}
}

int CModel::SetAllBtforceData(int rgdindex, bool srcval)
{
	if (!GetTopBone()) {
		return 0;
	}
	if (rgdindex < 0) {
		return 0;
	}

	SetBtforceDataReq(rgdindex, GetTopBone(false), srcval);
	return 0;
}
void CModel::SetBtforceDataReq(int rgdindex, CBone* srcbone, bool srcval)
{
	if (!srcbone) {
		return;
	}

	if (rgdindex < 0) {
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())) {
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			srcbone->GetParent(false)->SetBtForce((int)srcval);
		}
	}
	else {
		_ASSERT(0);
	}

	if (srcbone->GetChild(false)) {
		SetBtforceDataReq(rgdindex, srcbone->GetChild(false), srcval);
	}
	if (srcbone->GetBrother(false)) {
		SetBtforceDataReq(rgdindex, srcbone->GetBrother(false), srcval);
	}
}



int CModel::SetAllDampAnimData( int gid, int rgdindex, float valL, float valA )
{
	if(!GetTopBone()){
		return 0;
	}
	if( rgdindex < 0 ){
		return 0;
	}

	SetDampAnimDataReq( gid, rgdindex, GetTopBone(false), valL, valA );
	return 0;
}
void CModel::SetDampAnimDataReq( int gid, int rgdindex, CBone* srcbone, float valL, float valA )
{
	if (!srcbone) {
		return;
	}

	if( rgdindex < 0 ){
		return;
	}

	if ((rgdindex >= 0) && (rgdindex < (int)m_rigideleminfo.size())){
		if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
			char* filename = m_rigideleminfo[rgdindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetDampAnimDataReq( gid, rgdindex, srcbone->GetChild(false), valL, valA );
	}
	if( srcbone->GetBrother(false) ){
		SetDampAnimDataReq( gid, rgdindex, srcbone->GetBrother(false), valL, valA );
	}
}


int CModel::SetAllImpulseData( int gid, float impx, float impy, float impz )
{
	if(!GetTopBone()){
		return 0;
	}
	ChaVector3 srcimp;
	srcimp.SetParams(impx, impy, impz);

	//SetImpulseDataReq( gid, GetTopBone(false), srcimp );

	return 0;
}

void CModel::SetImpulseDataReq( int gid, CBone* srcbone, ChaVector3 srcimp )
{
	//if (!srcbone) {
	//	return;
	//}

	//if ((m_rgdindex < 0) || (m_rgdindex >= (int)m_rigideleminfo.size())){
	//	return;
	//}

	//CBone* parentbone = srcbone->GetParent();

	//if( parentbone ){
	//	int renum = (int)m_rigideleminfo.size();
	//	int impnum = parentbone->GetImpMapSize();
	//	if( (m_curimpindex >= 0) && (m_curimpindex < impnum) && (m_curreindex >= 0) && (m_curreindex < renum) ){

	//		char* filename = m_rigideleminfo[m_rgdindex].filename;//!!! rgdindex !!!
	//		CRigidElem* curre = parentbone->GetRigidElemOfMap( filename, srcbone );
	//		if( curre ){
	//			if( (gid == -1) || (gid == curre->GetGroupid()) ){
	//				string curimpname = m_impinfo[ m_curimpindex ];
	//				map<string, map<CBone*, ChaVector3>>::iterator findimpmap;
	//				findimpmap = parentbone->FindImpMap( curimpname );
	//				if( findimpmap != parentbone->GetImpMapEnd() ){
	//					map<CBone*,ChaVector3>::iterator itrimp;
	//					itrimp = findimpmap->second.find( srcbone );
	//					if( itrimp != findimpmap->second.end() ){
	//						itrimp->second = srcimp;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//if( srcbone->GetChild() ){
	//	SetImpulseDataReq( gid, srcbone->GetChild(), srcimp );
	//}
	//if( srcbone->GetBrother() ){
	//	SetImpulseDataReq( gid, srcbone->GetBrother(), srcimp );
	//}
}

int CModel::SetImp( int srcboneno, int kind, float srcval )
{
	//if(!GetTopBone()){
	//	return 0;
	//}
	//if( srcboneno < 0 ){
	//	return 0;
	//}
	//if( m_curimpindex < 0 ){
	//	return 0;
	//}

	//CBone* curbone = m_bonelist[srcboneno];
	//if( !curbone ){
	//	return 0;
	//}

	//CBone* parentbone = curbone->GetParent();
	//if( !parentbone ){
	//	return 0;
	//}

	//if ((curbone->IsNotSkeleton()) || (parentbone->IsNotSkeleton())) {
	//	return 0;
	//}


	//int impnum = parentbone->GetImpMapSize();
	//if( m_curimpindex >= impnum ){
	//	//_ASSERT( 0 );
	//	return 0;
	//}

	//string curimpname = m_impinfo[ m_curimpindex ];
	//if( parentbone->GetImpMapSize2( curimpname ) > 0 ){
	//	map<string, map<CBone*, ChaVector3>>::iterator itrfindmap;
	//	itrfindmap = parentbone->FindImpMap( curimpname );
	//	map<CBone*,ChaVector3>::iterator itrimp;
	//	itrimp = itrfindmap->second.find( curbone );
	//	if( itrimp == itrfindmap->second.end() ){
	//		if (kind == 0){
	//			itrfindmap->second[curbone].x = srcval;
	//		}
	//		else if (kind == 1){
	//			itrfindmap->second[curbone].y = srcval;
	//		}
	//		else if (kind == 2){
	//			itrfindmap->second[curbone].z = srcval;
	//		}
	//	}
	//	else{
	//		if (kind == 0){
	//			itrimp->second.x = srcval;
	//		}
	//		else if (kind == 1){
	//			itrimp->second.y = srcval;
	//		}
	//		else if (kind == 2){
	//			itrimp->second.z = srcval;
	//		}
	//	}
	//
	//}else{
	//	_ASSERT( 0 );
	//}

	return 0;
}


int CModel::SetAllDmpData( int gid, int reindex, float ldmp, float admp )
{
	if(!GetTopBone()){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetDmpDataReq( gid, reindex, GetTopBone(false), ldmp, admp );

	return 0;
}
void CModel::SetDmpDataReq( int gid, int reindex, CBone* srcbone, float ldmp, float admp )
{
	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetDmpDataReq( gid, reindex, srcbone->GetChild(false), ldmp, admp );
	}
	if( srcbone->GetBrother(false) ){
		SetDmpDataReq( gid, reindex, srcbone->GetBrother(false), ldmp, admp );
	}
}

int CModel::SetColTypeAll(int reindex, int srctype)
{
	if (!GetTopBone()){
		return 0;
	}
	if (reindex < 0){
		return 0;
	}

	if ((srctype >= COL_CONE_INDEX) && (srctype < COL_MAX)){
		SetColTypeReq(reindex, GetTopBone(false), srctype);
	}
	else{
		_ASSERT(0);
	}

	return 0;

}

void CModel::SetColTypeReq(int reindex, CBone* srcbone, int srctype)
{
	if (!srcbone) {
		return;
	}
	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				curre->SetColtype(srctype);
				curre->SetColtype(srctype);
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if (srcbone->GetChild(false)){
		SetColTypeReq(reindex, srcbone->GetChild(false), srctype);
	}
	if (srcbone->GetBrother(false)){
		SetColTypeReq(reindex, srcbone->GetBrother(false), srctype);
	}
}



int CModel::SetAllRestData( int gid, int reindex, float rest, float fric )
{
	if(!GetTopBone()){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetRestDataReq( gid, reindex, GetTopBone(false), rest, fric );

	return 0;
}

void CModel::SetRestDataReq( int gid, int reindex, CBone* srcbone, float rest, float fric )
{
	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetRestDataReq( gid, reindex, srcbone->GetChild(false), rest, fric );
	}
	if( srcbone->GetBrother(false) ){
		SetRestDataReq( gid, reindex, srcbone->GetBrother(false), rest, fric );
	}
}


int CModel::SetAllKData( int gid, int reindex, int srclk, int srcak, float srccuslk, float srccusak )
{
	if(!GetTopBone()){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetKDataReq( gid, reindex, GetTopBone(false), srclk, srcak, srccuslk, srccusak );

	return 0;
}
void CModel::SetKDataReq( int gid, int reindex, CBone* srcbone, int srclk, int srcak, float srccuslk, float srccusak )
{
	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetKDataReq( gid, reindex, srcbone->GetChild(false), srclk, srcak, srccuslk, srccusak );
	}
	if( srcbone->GetBrother(false) ){
		SetKDataReq( gid, reindex, srcbone->GetBrother(false), srclk, srcak, srccuslk, srccusak );
	}
}

int CModel::SetAllMassData( int gid, int reindex, float srcmass )
{
	if(!GetTopBone()){
		return 0;
	}
	if( reindex < 0 ){
		return 0;
	}

	SetMassDataReq( gid, reindex, GetTopBone(false), srcmass );


	return 0;
}
void CModel::SetMassDataReq( int gid, int reindex, CBone* srcbone, float srcmass )
{
	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetMassDataReq( gid, reindex, srcbone->GetChild(false), srcmass );
	}
	if( srcbone->GetBrother(false) ){
		SetMassDataReq( gid, reindex, srcbone->GetBrother(false), srcmass );
	}
}

int CModel::SetAllMassDataByBoneLeng(int gid, int reindex, float srcmass)
{
	if (!GetTopBone()){
		return 0;
	}
	if (reindex < 0){
		return 0;
	}

	SetMassDataByBoneLengReq(gid, reindex, GetTopBone(false), srcmass);


	return 0;
}
void CModel::SetMassDataByBoneLengReq(int gid, int reindex, CBone* srcbone, float srcmass)
{
	if (!srcbone) {
		return;
	}
	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((reindex >= 0) && (reindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[reindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
			if (curre){
				if ((gid == -1) || (gid == curre->GetGroupid())){
					ChaVector3 parentpos, curpos, diffpos;
					parentpos = srcbone->GetParent(false)->GetJointFPos();
					curpos = srcbone->GetJointFPos();
					diffpos = curpos - parentpos;
					float leng = (float)ChaVector3LengthDbl(&diffpos);
					float setmass = srcmass * leng;

					curre->SetMass(setmass);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}

	if (srcbone->GetChild(false)){
		SetMassDataByBoneLengReq(gid, reindex, srcbone->GetChild(false), srcmass);
	}
	if (srcbone->GetBrother(false)){
		SetMassDataByBoneLengReq(gid, reindex, srcbone->GetBrother(false), srcmass);
	}
}

int CModel::SetKinematicFlag()
{
	if (!m_topbt) {
		return 0;
	}

	SetKinematicFlagReq(m_topbt);

	return 0;
}

void CModel::SetKinematicFlagReq(CBtObject* srcbto)
{
	if (!srcbto) {
		return;
	}

	CBone* srcbone = srcbto->GetBone();
	if (srcbone) {

		srcbone->SetBtKinFlag(1);

		DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
		if (s_setrigidflag == 0) {
			s_rigidflag = curflag;
			s_setrigidflag = 1;
		}
		//srcbto->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		srcbto->GetRigidBody()->setCollisionFlags(curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
		//srcbto->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
		//srcbto->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
		//srcbto->m_rigidbody->setActivationState(DISABLE_SIMULATION);
		//CF_STATIC_OBJECT

		if (srcbto->GetRigidBody()) {
			//srcbto->GetRigidBody()->activate();
			//###srcbto->GetRigidBody()->setDeactivationTime(0.0);
			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);

			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 150.0);
			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
		}
	}

	int childno;
	for (childno = 0; childno < srcbto->GetChildBtSize(); childno++) {
		CBtObject* childbto = srcbto->GetChildBt(childno);
		if (childbto) {
			SetKinematicFlagReq(childbto);
		}
	}
}


//int CModel::SetRagdollKinFlag(int selectbone, int physicsmvkind)
//{
//
//	if( !m_topbt ){
//		return 0;
//	}
//
//	SetRagdollKinFlagReq( m_topbt, selectbone, physicsmvkind );
//
//	
//
//
//	return 0;
//}
//void CModel::SetRagdollKinFlagReq(CBtObject* srcbto, int selectbone, int physicsmvkind)
//{
//	//Mass0が設定されていた場合、TmpKinematicは強制的に無効
//
//
//	CBone* srcbone = srcbto->GetBone();
//	if (srcbone && srcbone->GetParent()){
//		if ((srcbone->GetMass0() == TRUE) || (srcbone->GetParent()->GetMass0() == TRUE) || (srcbone->GetParent()->GetTmpKinematic() == false)) {
//			CBone* kinchildbone = GetBoneByID(selectbone);
//			if (kinchildbone) {
//				CBone* kinbone = kinchildbone->GetParent();
//				if (kinbone) {
//					int curboneno = srcbone->GetBoneNo();
//					if (curboneno != kinbone->GetBoneNo()) {
//						srcbone->SetBtKinFlag(0);
//					}
//					else {
//						srcbone->SetBtKinFlag(1);
//					}
//
//				}
//			}
//			else {
//				srcbone->SetBtKinFlag(0);
//			}
//		}
//		else {
//			srcbone->SetBtKinFlag(1);
//		}
//
//		//if ((physicsmvkind == 0) && (srcbone->GetBtKinFlag() == 1) && (srcbto->GetRigidBody())){
//		if ((srcbone->GetBtKinFlag() == 1) && (srcbto->GetRigidBody())){
//			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
//			if (s_setrigidflag == 0){
//				s_rigidflag = curflag;
//				s_setrigidflag = 1;
//			}
//			//srcbto->GetRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
//			srcbto->GetRigidBody()->setCollisionFlags(curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
//			//srcbto->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
//			//srcbto->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
//			//srcbto->m_rigidbody->setActivationState(DISABLE_SIMULATION);
//			//CF_STATIC_OBJECT
//		}
//		else if (srcbto->GetRigidBody()){
//			DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
//			srcbto->GetRigidBody()->setCollisionFlags(curflag & ~btCollisionObject::CF_KINEMATIC_OBJECT);
//
//			if (srcbone->GetParent()){
//				CRigidElem* curre = srcbone->GetParent()->GetRigidElem(srcbone);
//				if (curre){
//					if ((m_curreindex >= 0) && (m_curreindex < (int)m_rigideleminfo.size())){
//						//srcbto->GetRigidBody()->setGravity(btVector3(0.0f, curre->GetBtg() * m_rigideleminfo[m_curreindex].btgscale, 0.0f));
//						srcbto->GetRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
//						srcbto->GetRigidBody()->applyGravity();
//					}
//					else{
//						_ASSERT(0);
//					}
//				}
//			}
//		}
//
//		if (srcbto->GetRigidBody()){
//			//srcbto->GetRigidBody()->activate();
//			//###srcbto->GetRigidBody()->setDeactivationTime(0.0);
//			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
//
//			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 150.0);
//			//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
//		}
//	}
//
//
//
//	int chilno;
//	for( chilno = 0; chilno < srcbto->GetChildBtSize(); chilno++ ){
//		CBtObject* chilbto = srcbto->GetChildBt( chilno );
//		if( chilbto ){
//			SetRagdollKinFlagReq( chilbto, selectbone );
//		}
//	}
//}

int CModel::SetCurrentRigidElem( int curindex )
{
	if (GetNoBoneFlag() == true) {
		return 0;
	}


	if( curindex < 0 ){
		return 0;
	}
	if( curindex >= (int)m_rigideleminfo.size() ){
		//_ASSERT( 0 );
		return 0;
	}

	m_curreindex = curindex;

	string curname = m_rigideleminfo[ curindex ].filename;


	SetCurrentRigidElemReq(GetTopBone(false), curname);


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

REINFO CModel::GetCurrentRigidElemInfo(int* retindex)
{
	REINFO retinfo;

	if ((m_curreindex >= 0) && (m_curreindex < (int)m_rigideleminfo.size())) {
		*retindex = m_curreindex;
		retinfo = m_rigideleminfo[m_curreindex];
	}
	else {
		*retindex = -1;
		retinfo.btgscale = 1.0f;
		strcpy_s(retinfo.filename, 256, "unknown");
	}

	return retinfo;
}

void CModel::SetCurrentRigidElemReq(CBone* srcbone, string curname)
{
	if (GetNoBoneFlag() == true) {
		return;
	}

	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		srcbone->SetCurrentRigidElem(curname);
	}

	if (srcbone->GetChild(false)){
		SetCurrentRigidElemReq(srcbone->GetChild(false), curname);
	}
	if (srcbone->GetBrother(false)){
		SetCurrentRigidElemReq(srcbone->GetBrother(false), curname);
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
	if(!GetTopBone()){
		return 0;
	}
	if( m_curreindex < 0 ){
		return 0;
	}

	SetColiIDReq(GetTopBone(false), srcre);

	return 0;
}
void CModel::SetColiIDReq( CBone* srcbone, CRigidElem* srcre )
{
	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton()) && srcbone->GetParent(false)) {
		if ((m_curreindex >= 0) && (m_curreindex < (int)m_rigideleminfo.size())){
			char* filename = m_rigideleminfo[m_curreindex].filename;
			CRigidElem* curre = srcbone->GetParent(false)->GetRigidElemOfMap(filename, srcbone);
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

	if( srcbone->GetChild(false) ){
		SetColiIDReq( srcbone->GetChild(false), srcre );
	}
	if( srcbone->GetBrother(false) ){
		SetColiIDReq( srcbone->GetBrother(false), srcre );
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
			
			//curbto->GetRigidBody()->activate();
			//curbto->GetRigidBody()->setDeactivationTime(0.0);

			//curbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
			//colObj->setActivationState(WANTS_DEACTIVATION);


			curbto->GetRigidBody()->setDeactivationTime(0.0f);
			curbto->GetRigidBody()->setSleepingThresholds(0.0f, 0.0f);
			curbto->GetRigidBody()->activate();
		}
		if (curbto->GetRigidBody() && !curbto->GetRigidBody()->isStaticObject())
		{
			curbto->GetRigidBody()->setLinearVelocity(btVector3(0,0,0));
			curbto->GetRigidBody()->setAngularVelocity(btVector3(0,0,0));
		}
	}

	int chilno;
	for( chilno = 0; chilno < curbto->GetChildBtSize(); chilno++ ){
		CBtObject* chilbto = curbto->GetChildBt( chilno );
		if( chilbto ){
			ResetBtReq( chilbto );
		}
	}
}

int CModel::SetBefEditMat(bool limitdegflag, CEditRange* erptr, CBone* curbone, int maxlevel )
{
	if (!ExistCurrentMotion()) {
		return 0;
	}

	int levelcnt0 = 0;
	while( curbone && ((maxlevel == 0) || (levelcnt0 < (maxlevel+1))) )
	{
		if (curbone->IsSkeleton()) {
			int keynum;
			double startframe, endframe, applyframe;
			erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
			double curframe;
			for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
				CMotionPoint* editmp = 0;
				int curmotid = GetCurrentMotID();
				editmp = curbone->GetMotionPoint(curmotid, curframe);
				if (editmp) {
					if (g_previewFlag != 5) {
						editmp->SetBefEditMat(curbone->GetWorldMat(limitdegflag, curmotid, curframe, editmp));
					}
					else {
						editmp->SetBefEditMat(curbone->GetBtMat());
					}
				}
				else {
					_ASSERT(0);
				}
			}
		}
		curbone = curbone->GetParent(false);
		levelcnt0++;
	}
	return 0;
}

int CModel::SetBefEditMatFK(bool limitdegflag, CEditRange* erptr, CBone* curbone)
{
	if (ExistCurrentMotion() && curbone && curbone->IsSkeleton()){

		int keynum;
		double startframe, endframe, applyframe;
		erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0){

			CMotionPoint* editmp = 0;
			int curmotid = GetCurrentMotID();
			editmp = curbone->GetMotionPoint(curmotid, curframe);
			if (editmp){
				editmp->SetBefEditMat(curbone->GetWorldMat(limitdegflag, curmotid, curframe, editmp));
			}
			else{
				_ASSERT(0);
			}
		}
	}
	return 0;
}


int CModel::CalcAxisAndRotForIKRotate(int limitdegflag, 
	CBone* parentbone, CBone* firstbone, 
	double curframe, ChaVector3 targetpos, 
	ChaVector3* dstaxis, float* dstrotrad)
{

	//########################################################
	//2023/03/24
	//model座標系で計算：modelのWorldMatの影響を無くして計算
	//########################################################


	if (!parentbone || !firstbone || !dstaxis || !dstrotrad) {
		_ASSERT(0);
		return 1;
	}

	if (!firstbone->GetParent(false)) {
		_ASSERT(0);
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();

	ChaMatrix invmodelwm;
	invmodelwm = ChaMatrixInv(GetWorldMat());
	ChaVector3 modelcamtarget, modelcameye;
	ChaVector3TransformCoord(&modelcamtarget, &g_camtargetpos, &invmodelwm);
	ChaVector3TransformCoord(&modelcameye, &g_camEye, &invmodelwm);

	//ChaVector3 ikaxis = g_camtargetpos - g_camEye;
	ChaVector3 ikaxis = modelcamtarget - modelcameye;
	ChaVector3Normalize(&ikaxis, &ikaxis);

	ChaVector3 modelparentpos, modelchildpos;
	{
		ChaVector3 parentworld, childworld;
		parentworld = parentbone->GetWorldPos(limitdegflag, curmotid, curframe);
		ChaVector3TransformCoord(&modelparentpos, &parentworld, &invmodelwm);

		ChaMatrix parentmat = firstbone->GetParent(false)->GetWorldMat(limitdegflag, curmotid, curframe, 0);// *GetWorldMat();
		ChaVector3 tmpfirstfpos = firstbone->GetJointFPos();
		ChaVector3TransformCoord(&modelchildpos, &tmpfirstfpos, &parentmat);
	}


	ChaVector3 parbef, chilbef, tarbef;
	parbef = modelparentpos;
	CalcShadowToPlane(modelchildpos, ikaxis, modelparentpos, &chilbef);
	CalcShadowToPlane(targetpos, ikaxis, modelparentpos, &tarbef);

	ChaVector3 vec0, vec1;
	vec0 = chilbef - parbef;
	ChaVector3Normalize(&vec0, &vec0);
	vec1 = tarbef - parbef;
	ChaVector3Normalize(&vec1, &vec1);

	ChaVector3 rotaxis2;
	ChaVector3Cross(&rotaxis2, (const ChaVector3*)&vec0, (const ChaVector3*)&vec1);
	ChaVector3Normalize(&rotaxis2, &rotaxis2);

	float rotdot2, rotrad2;
	rotdot2 = ChaVector3Dot(&vec0, &vec1);
	rotdot2 = min(1.0f, rotdot2);
	rotdot2 = max(-1.0f, rotdot2);
	rotrad2 = (float)acos(rotdot2);

	*dstaxis = rotaxis2;
	*dstrotrad = rotrad2;

	return 0;
}


//camera vertical
int CModel::CalcAxisAndRotForIKRotateVert(int limitdegflag,
	CBone* parentbone, CBone* firstbone,
	double curframe, ChaVector3 targetpos,
	ChaVector3* dstaxis, float* dstrotrad)
{
	if (!parentbone || !firstbone || !dstaxis || !dstrotrad) {
		_ASSERT(0);
		return 1;
	}

	if (!firstbone->GetParent(false)) {
		_ASSERT(0);
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();

	ChaVector3 ikaxis = g_camtargetpos - g_camEye;
	ChaVector3Normalize(&ikaxis, &ikaxis);

	ChaVector3 parworld, chilworld;
	parworld.SetParams(0.0f, 0.0f, 0.0f);
	chilworld.SetParams(0.0f, 0.0f, 0.0f);
	{
		parworld = parentbone->GetWorldPos(limitdegflag, curmotid, curframe);
		ChaMatrix parworldmat = firstbone->GetParent(false)->GetWorldMat(limitdegflag, curmotid, curframe, 0);// *GetWorldMat();
		ChaVector3 tmpfirstfpos = firstbone->GetJointFPos();
		ChaVector3TransformCoord(&chilworld, &tmpfirstfpos, &parworldmat);
	}

	ChaVector3 rotaxis2;
	rotaxis2.SetParams(0.0f, 0.0f, 0.0f);
	ChaVector3 rotaxis3;
	rotaxis3.SetParams(0.0f, 0.0f, 0.0f);;
	{
		ChaVector3 parbef, chilbef, tarbef;
		parbef = parworld;
		CalcShadowToPlane(chilworld, ikaxis, parworld, &chilbef);
		CalcShadowToPlane(targetpos, ikaxis, parworld, &tarbef);
		ChaVector3 vec0, vec1;
		vec0 = chilbef - parbef;
		ChaVector3Normalize(&vec0, &vec0);
		vec1 = tarbef - parbef;
		ChaVector3Normalize(&vec1, &vec1);

		ChaVector3Cross(&rotaxis2, (const ChaVector3*)&vec0, (const ChaVector3*)&vec1);
		ChaVector3Normalize(&rotaxis2, &rotaxis2);


		//////// vertical
		rotaxis3.SetParams(0.0f, 0.0f, 0.0f);
		ChaVector3Cross(&rotaxis3, (const ChaVector3*)&ikaxis, (const ChaVector3*)&vec0);
		ChaVector3Normalize(&rotaxis3, &rotaxis3);
	}


	ChaVector3 rotaxis4;
	rotaxis4.SetParams(0.0f, 0.0f, 0.0f);
	float rotrad4 = 0.0f;
	{
		ChaVector3 parbef, chilbef, tarbef;
		parbef = parworld;
		CalcShadowToPlane(chilworld, rotaxis3, parworld, &chilbef);
		CalcShadowToPlane(targetpos, rotaxis3, parworld, &tarbef);
		ChaVector3 vec0, vec1;
		vec0 = chilbef - parbef;
		ChaVector3Normalize(&vec0, &vec0);
		vec1 = tarbef - parbef;
		ChaVector3Normalize(&vec1, &vec1);

		ChaVector3Cross(&rotaxis4, (const ChaVector3*)&vec0, (const ChaVector3*)&vec1);
		ChaVector3Normalize(&rotaxis4, &rotaxis4);

		float rotdot4;
		rotdot4 = ChaVector3Dot(&vec0, &vec1);
		rotdot4 = min(1.0f, rotdot4);
		rotdot4 = max(-1.0f, rotdot4);
		rotrad4 = (float)acos(rotdot4);
	}

	*dstaxis = rotaxis4;
	*dstrotrad = rotrad4;

	return 0;
}


void CModel::ClearIKRotRec()
{
	ClearIKRotRecReq(GetTopBone(false));
}
void CModel::ClearIKRotRecUV()
{
	ClearIKRotRecUVReq(GetTopBone(false));
}

void CModel::ClearIKRotRecReq(CBone* srcbone)
{
	if (srcbone) {

		srcbone->ClearIKRotRec();

		if (srcbone->GetChild(false)) {
			ClearIKRotRecReq(srcbone->GetChild(false));
		}
		if (srcbone->GetBrother(false)) {
			ClearIKRotRecReq(srcbone->GetBrother(false));
		}
	}
}
void CModel::ClearIKRotRecUVReq(CBone* srcbone)
{
	if (srcbone) {

		srcbone->ClearIKRotRecUV();

		if (srcbone->GetChild(false)) {
			ClearIKRotRecUVReq(srcbone->GetChild(false));
		}
		if (srcbone->GetBrother(false)) {
			ClearIKRotRecUVReq(srcbone->GetBrother(false));
		}
	}
}

int CModel::IKRotateUnderIK(bool limitdegflag, CEditRange* erptr,
	int srcboneno, ChaVector3 targetpos, int maxlevel)
{
	ChaCalcFunc chacalcfunc;

	SetIKTargetVec();

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone) {
		_ASSERT(0);
		////g_underIKRot = false;//2023/01/14 parent limited or not
		return -1;
	}

	if (firstbone->IsNotSkeleton()) {
		return -1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	if (GetNoBoneFlag()) {
		return 0;
	}

	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	bool ishipsjoint = false;
	if (firstbone->GetParent(false)) {
		ishipsjoint = firstbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	int keynum;
	double startframe0, endframe0;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe0, &endframe0, &applyframe);
	startframe = startframe0;
	endframe = endframe0;


	CBone* curbone = firstbone;
	CBone* lastpar = curbone;
	SetBefEditMat(limitdegflag, erptr, curbone, maxlevel);
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}

	//CBone* lastpar = firstbone->GetParent(false);
	//int calcnum = 20;
	int calcnum = 1;//今はtargetposを細かく刻んでいないので１で良い

	int calccnt;
	for (calccnt = 0; calccnt < calcnum; calccnt++) {
		curbone = firstbone;
		lastpar = curbone;

		int levelcnt = 0;
		float currate = g_ikrate;

		while (curbone && lastpar && lastpar->GetParent(false) && ((maxlevel == 0) || (levelcnt < maxlevel)))
		{

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			////g_underIKRot = true;

			//CBone* parentbone = curbone->GetParent();
			CBone* parentbone = lastpar->GetParent(false);
			if (parentbone && parentbone->IsSkeleton() && (curbone->GetJointFPos() != parentbone->GetJointFPos())) {
				//UpdateMatrix(limitdegflag, &m_matWorld, &m_matVP);//curmp更新

				CRigidElem* curre = GetRigidElem(lastpar->GetBoneNo());
				if (curre && (curre->GetForbidRotFlag() != 0)) {

					//_ASSERT(0);

					//回転禁止の場合処理をスキップ
					if (parentbone) {
						lastpar = parentbone;
					}
					levelcnt++;
					currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
					continue;
				}

				ChaVector3 rotaxis2;
				float rotrad2;
				CalcAxisAndRotForIKRotate(limitdegflag,
					parentbone, firstbone, 
					GetCurrentFrame(), targetpos,
					&rotaxis2, &rotrad2);

				rotrad2 *= currate;

				double firstframe = 0.0;
				if (fabs(rotrad2) > 1.0e-4) {
					CQuaternion rotq0;
					rotq0.SetAxisAndRot(rotaxis2, rotrad2);

					//parentbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
					// 
					//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
					parentbone->SaveSRT(limitdegflag, curmotid, startframe);

					//2023/01/22 : topposスライダーの位置のフレーム(３D表示中のフレーム)において　
					//制限角度により　回転出来ない場合は　リターンする
					//if (g_limitdegflag != 0) {
					if ((limitdegflag != false) && (g_wallscrapingikflag == 0)) {//2023/01/23
						//2023/01/28 IK時は　GetBtForce()チェックはしない　BtForce == 1でも角度制限する
						int ismovable = IsMovableRot(limitdegflag, curmotid, applyframe, applyframe,
							rotq0, parentbone, parentbone);
						if (ismovable == 0) {
							////g_underIKRot = false;//2023/01/14 parent limited or not
							if (editboneforret) {
								return editboneforret->GetBoneNo();
							}
							else {
								return srcboneno;
							}
						}
					}


					double curframe = applyframe;
					int ismovable2 = 1;
					if (keynum >= 2) {
						int keyno = 0;

						bool keynum1flag = false;
						bool postflag = false;
						bool fromiktarget = false;
						ismovable2 = chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
							keyno, 
							parentbone, parentbone,
							curmotid, curframe, startframe, applyframe,
							rotq0, keynum1flag, postflag, fromiktarget);

						keyno++;
					}
					else {
						bool keynum1flag = true;
						bool postflag = false;
						bool fromiktarget = false;
						ismovable2 = chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
							0, 
							parentbone, parentbone,
							curmotid, GetCurrentFrame(), startframe, applyframe,
							rotq0, keynum1flag, postflag, fromiktarget);
					}


					//2023/03/04 制限角度に引っ掛かった場合には　やめて　次のジョイントの回転へ
					if ((ismovable2 == 0) && (g_wallscrapingikflag == 0)) {
						continue;
					}


					//parentboneのrotqを保存
					IKROTREC currotrec;
					currotrec.applyframemat = parentbone->GetWorldMat(limitdegflag, curmotid, applyframe, 0);
					currotrec.applyframeeul = parentbone->GetLocalEul(limitdegflag, curmotid, applyframe, 0);
					currotrec.rotq = rotq0;
					currotrec.targetpos = targetpos;
					currotrec.lessthanthflag = false;
					parentbone->AddIKRotRec(currotrec);
					
					if (g_applyendflag == 1) {
						//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
						int tolast;
						for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
							(m_bonelist[0])->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
						}
					}
				}
				else {
					//rotqの回転角度が1e-4より小さい場合
					//ウェイトが小さいフレームにおいても　IKTargetが走るように記録する必要がある

					CQuaternion rotq0;
					rotq0.SetAxisAndRot(rotaxis2, rotrad2);

					IKROTREC currotrec;
					currotrec.applyframemat = parentbone->GetWorldMat(limitdegflag, curmotid, applyframe, 0);
					currotrec.applyframeeul = parentbone->GetLocalEul(limitdegflag, curmotid, applyframe, 0);
					currotrec.rotq = rotq0;
					currotrec.targetpos = targetpos;
					currotrec.lessthanthflag = true;//!!!!!!!!!!!
					parentbone->AddIKRotRec(currotrec);
				}

			}

			if (parentbone) {
				lastpar = parentbone;

				//check ikstopflag
				if (parentbone->GetIKStopFlag()) {
					break;
				}
			}

			levelcnt++;

			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
		}

		//絶対モードの場合
		if ((calccnt == (calcnum - 1)) && g_absikflag && lastpar) {
			chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastpar, curmotid);
		}
	}

	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret)
	{
		return editboneforret->GetBoneNo();
	}
	else {
		return srcboneno;
	}

}

int CModel::OnBlendWeightChanged(CEditRange* erptr, CMQOObject* srcmqoobj, int channelindex, float srcvalue)
{
	if (!erptr || !srcmqoobj) {
		_ASSERT(0);
		return 1;
	}

	if (srcmqoobj->EmptyShape()) {
		return 0;
	}

	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	int keynum;
	double startframe0, endframe0;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe0, &endframe0, &applyframe);
	startframe = startframe0;
	endframe = endframe0;

	if (keynum == 1) {
		return 0;
	}

	int framecnt;
	for (framecnt = IntTime(startframe); framecnt < IntTime(endframe); framecnt++) {
		if (framecnt != IntTime(applyframe)) {
			double changerate;
			changerate = (double)(*(g_motionbrush_value + framecnt));
			float setvalue = (float)(changerate * (double)srcvalue);

			int result0 = srcmqoobj->SetShapeAnimWeight(channelindex,
				curmotid, framecnt, setvalue);
			if (result0 != 0) {
				_ASSERT(0);
				return 1;
			}
		}
	}

	return 0;
}

int CModel::IKRotatePostIK(bool limitdegflag, CEditRange* erptr,
	int srcboneno, int maxlevel)
{
	ChaCalcFunc chacalcfunc;


	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone) {
		_ASSERT(0);
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return -1;
	}
	if (firstbone->IsNotSkeleton()) {
		return -1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	if (GetNoBoneFlag()) {
		return 0;
	}

	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	bool ishipsjoint = false;
	if (firstbone->GetParent(false)) {
		ishipsjoint = firstbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	int keynum;
	double startframe0, endframe0;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe0, &endframe0, &applyframe);
	startframe = startframe0;
	endframe = endframe0;


	SetPostIKFrame(RoundingTime(startframe), RoundingTime(endframe));//2023/10/17


	CBone* curbone = firstbone;
	CBone* lastpar = curbone;
	SetBefEditMat(limitdegflag, erptr, curbone, maxlevel);
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}


	//CBone* lastpar = firstbone->GetParent(false);
	//int calcnum = 20;
	int calcnum = 1;//今はtargetposを細かく刻んでいないので１で良い
	int calccnt;
	for (calccnt = 0; calccnt < calcnum; calccnt++) {
		curbone = firstbone;
		lastpar = curbone;

		int levelcnt = 0;
		float currate = g_ikrate;

		while (curbone && lastpar && lastpar->GetParent(false) && ((maxlevel == 0) || (levelcnt < maxlevel)))
		{

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			//g_underIKRot = true;

			double firstframe = 0.0;

			//CBone* parentbone = curbone->GetParent();
			CBone* parentbone = lastpar->GetParent(false);
			if (parentbone && parentbone->IsSkeleton() && (curbone->GetJointFPos() != parentbone->GetJointFPos())) {
				int rotrecsize = parentbone->GetIKRotRecSize();
				if (rotrecsize > 0) {
					int rotrecno;
					for (rotrecno = 0; rotrecno < rotrecsize; rotrecno++) {
						IKROTREC currotrec = parentbone->GetIKRotRec(rotrecno);
						CQuaternion rotq0 = currotrec.rotq;
						bool lessthanthflag = currotrec.lessthanthflag;

						//UpdateMatrix(limitdegflag, &m_matWorld, &m_matVP);//curmp更新

						CRigidElem* curre = GetRigidElem(lastpar->GetBoneNo());
						if (curre && (curre->GetForbidRotFlag() != 0)) {

							//_ASSERT(0);

							//回転禁止の場合処理をスキップ
							if (parentbone) {
								lastpar = parentbone;
							}
							levelcnt++;
							currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
							break;//ikrotrec loopを抜けて　parentbone loopへ
						}

						//parentbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
						// 
						//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
						parentbone->SaveSRT(limitdegflag, curmotid, startframe);


						if (lessthanthflag == false) {
							if (keynum >= 2) {
								int keyno = 0;
								bool keynum1flag = false;
								bool postflag = true;
								bool fromiktarget = false;

								if (m_PostIKThreads) {
									//SetUnderPostIK(true);
									int updatecount;
									for (updatecount = 0; updatecount < POSTIK_THREADSNUM; updatecount++) {
										CThreadingPostIK* curupdate = m_PostIKThreads + updatecount;
										curupdate->IKRotateOneFrame(this, limitdegflag, erptr,
											keyno,
											parentbone, parentbone,
											curmotid, startframe, applyframe,
											rotq0, keynum1flag, postflag, fromiktarget);
									}
									WaitPostIKFinished();
									//SetUnderPostIK(false);
								}


								//double curframe;
								//for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
								//	if (curframe != applyframe) {
								//		bool keynum1flag = false;
								//		bool postflag = true;
								//		bool fromiktarget = false;
								//		chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
								//			keyno, 
								//			parentbone, parentbone,
								//			m_curmotinfo->motid, curframe, startframe, applyframe,
								//			rotq0, keynum1flag, postflag, fromiktarget);
								//	}
								//	//else {//2023/10/16_1 Q2EulXYZusingQ()にて　IK時にbefeul.currentframeeulを使用することにしたので不要に
								//	//	//2023/10/12
								//	//	//applyframeにも　オイラー角の計算は必要
								//	//	//ChaVector3 neweul.SetParams(0.0f, 0.0f, 0.0f);
								//	//	//neweul = parentbone->CalcLocalEulXYZ(limitdegflag, -1, m_curmotinfo->motid, curframe, BEFEUL_BEFFRAME);
								//	//	//parentbone->SetLocalEul(limitdegflag, m_curmotinfo->motid, curframe, neweul, 0);
								//	//	
								//	//	//2023/10/16
								//	//	// applyframeで急激に値が変わるとカーブが想定と違うことがあるので　計算済を徐々にトレースしながら
								//	//	parentbone->SetWorldMat(limitdegflag, m_curmotinfo->motid, curframe, currotrec.applyframemat, 0);
								//	//	ChaVector3 neweul.SetParams(0.0f, 0.0f, 0.0f);
								//	//	neweul = parentbone->CalcLocalEulXYZ(limitdegflag, -1, m_curmotinfo->motid, curframe, BEFEUL_BEFFRAME);
								//	//	parentbone->SetLocalEul(limitdegflag, m_curmotinfo->motid, curframe, neweul, 0);
								//	//}
								//}
								//keyno++;
							}
						}
						//else {
						//	bool keynum1flag = true;
						//	chacalcfunc.IKRotateOneFrame(limitdegflag, erptr,
						//		0, parentbone,
						//		m_curmotinfo->curframe, startframe, applyframe,
						//		rotq0, keynum1flag);
						//}
					}
				}

				if (g_applyendflag == 1) {
					//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
					int tolast;
					for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
						(m_bonelist[0])->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
					}
				}
			}

			if (parentbone) {
				lastpar = parentbone;

				//check ikstopflag
				if (parentbone->GetIKStopFlag()) {
					break;
				}
			}
			levelcnt++;
			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);

		}


		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
			if (curframe != applyframe) {
				bool postflag = true;
				chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, curframe, postflag);
			}
		}
	}

	//絶対モードの場合
	if ((calccnt == (calcnum - 1)) && g_absikflag && lastpar) {
		chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastpar, curmotid);
	}


	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret)
	{
		return editboneforret->GetBoneNo();
	}
	else {
		return srcboneno;
	}

}

int CModel::IKRotate(bool limitdegflag, CEditRange* erptr, 
	int srcboneno, ChaVector3 targetpos, int maxlevel, double directframe)//default : directframe = -1.0
{
	ChaCalcFunc chacalcfunc;

	CBone* firstbone = m_bonelist[ srcboneno ];
	if( !firstbone ){
		_ASSERT( 0 );
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return -1;
	}
	if (firstbone->IsNotSkeleton()) {
		return -1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	bool ishipsjoint = false;
	if (firstbone->GetParent(false)) {
		ishipsjoint = firstbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	int keynum;
	double startframe0, endframe0;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe0, &endframe0, &applyframe);
	if (directframe < 0.0) {
		startframe = startframe0;
		endframe = endframe0;
	}
	else {
		startframe = directframe;
		endframe = directframe;	
		keynum = 1;
	}
	

	CBone* curbone = firstbone;
	CBone* lastpar = curbone; 
	SetBefEditMat(limitdegflag, erptr, curbone, maxlevel);
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}


	//CBone* lastpar = firstbone->GetParent();
	//int calcnum = 20;
	int calcnum = 1;//今はtargetposを細かく刻んでいないので１で良い
	
	int calccnt;
	for( calccnt = 0; calccnt < calcnum; calccnt++ ){
		curbone = firstbone;
		lastpar = curbone;

		int levelcnt = 0;
		float currate = g_ikrate;

		while( curbone && lastpar && lastpar->GetParent(false) && ((maxlevel == 0) || (levelcnt < maxlevel)) )
		{

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			//g_underIKRot = true;

			//CBone* parentbone = curbone->GetParent();
			CBone* parentbone = lastpar->GetParent(false);
			if( parentbone && parentbone->IsSkeleton() && (curbone->GetJointFPos() != parentbone->GetJointFPos()) ){
				//UpdateMatrix(limitdegflag, &m_matWorld, &m_matVP);//curmp更新

				CRigidElem* curre = GetRigidElem(lastpar->GetBoneNo());
				if (curre && (curre->GetForbidRotFlag() != 0)) {
					
					//_ASSERT(0);

					//回転禁止の場合処理をスキップ
					if (parentbone) {
						lastpar = parentbone;
					}
					levelcnt++;
					currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
					continue;
				}

				ChaVector3 rotaxis2;
				float rotrad2;
				CalcAxisAndRotForIKRotate(limitdegflag,
					parentbone, firstbone, 
					GetCurrentFrame(), targetpos,
					&rotaxis2, &rotrad2);

				rotrad2 *= currate;
				
				double firstframe = 0.0;
				if( fabs( rotrad2 ) > 1.0e-4 ){
					CQuaternion rotq0;
					rotq0.SetAxisAndRot( rotaxis2, rotrad2 );

					//parentbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
					// 
					//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
					parentbone->SaveSRT(limitdegflag, curmotid, startframe);


					//2023/01/22 : topposスライダーの位置のフレーム(３D表示中のフレーム)において　
					//制限角度により　回転出来ない場合は　リターンする
					//if (g_limitdegflag != 0) {
					if ((limitdegflag != false) && (g_wallscrapingikflag == 0)) {//2023/01/23
						//2023/01/28 IK時は　GetBtForce()チェックはしない　BtForce == 1でも角度制限する
						int ismovable = IsMovableRot(limitdegflag, curmotid, applyframe, applyframe,
							rotq0, parentbone, parentbone);
						if (ismovable == 0) {
							//g_underIKRot = false;//2023/01/14 parent limited or not
							if (editboneforret) {
								return editboneforret->GetBoneNo();
							}
							else {
								return srcboneno;
							}
						}
					}


					if (keynum >= 2) {
						int keyno = 0;
						double curframe;
						for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
							
							bool keynum1flag = false;
							bool postflag = false;
							bool fromiktarget = false;
							chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
								keyno, 
								parentbone, parentbone, 
								curmotid, curframe, startframe, applyframe,
								rotq0, keynum1flag, postflag, fromiktarget);

							keyno++;
						}
					}
					else {
						bool keynum1flag = true;
						bool postflag = false;
						bool fromiktarget = false;
						chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
							0, 
							parentbone, parentbone,
							curmotid, GetCurrentFrame(), startframe, applyframe,
							rotq0, keynum1flag, postflag, fromiktarget);
					}


					if (g_applyendflag == 1) {
						//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
						int tolast;
						for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
							(m_bonelist[0])->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
						}
					}
				}

			}


			//if( curbone->GetParent() ){
			//	lastpar = curbone->GetParent();
			//}
			//curbone = curbone->GetParent();
			if (parentbone) {
				lastpar = parentbone;

				//check ikstopflag
				if (parentbone->GetIKStopFlag()) {
					break;
				}
			}

			levelcnt++;

			currate = (float)pow( (double)g_ikrate, (double)g_ikfirst * (double)levelcnt );
		}

		//絶対モードの場合
		if( (calccnt == (calcnum - 1)) && g_absikflag && lastpar ){
			chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastpar, curmotid);
		}
	}

	//if( lastpar ){
	//	return lastpar->GetBoneNo();
	//}else{
	//	return srcboneno;
	//}


	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret)
	{
		return editboneforret->GetBoneNo();
	}
	else {
		return srcboneno;
	}

}


//int CModel::PhysicsRot(CEditRange* erptr, int srcboneno, ChaVector3 targetpos, int maxlevel)
//{
//
//	//Memo 20180602
//	//PhysicsRotの結果が不安定になるケースが絞り込めてきた。
//	//parrent-->grand parentのボーンが視線に対して平行のときにこの関数を使うとcurrent-->parentボーンがぐるんぐるん回ってしまうことが多い。
//	//腕-->肩と鎖骨が直角なときに腕のIKで起こりやすい。
//	//PhysicsRotAxisDelta関数で回転軸を指定してIKすると安定する。
//
//	//Memo 20180604
//	//上記のぐるんぐるん回る問題
//	//Mediaフォルダのサンプルのfbxファイルの「腕の座標軸がX軸ベースになっていない」ことが分かった。
//	//PhysicsRotボタンを押すとサンプルの該当ボーンの座標軸がX軸ベースでなくなるようだった。
//	//CalcShadowToPlaneの問題の可能性もある。見直したが忘れていてわからない部分があった。
//
//	//Memo 20180606
//	//座標軸は直した。
//	//カレント座標系をデフォルトにした。
//	//ぐるんぐるんはまだ直っていない。
//
//	//Memo 20180619
//	//ぐるんぐるん症状が直った（ただしBtApplyボタンを押さない場合）
//	//newbtmatの計算を修正した。
//
//	//Memo 20180623
//	//BtApplyボタンを押してもぐるんぐるん症状が出なくなった。
//	//ぐるんぐるんする場合は、ボーンが反対回りしているようだった。
//	//回転してターゲットに近づかない場合にはキャンセルすることにより直った。
//	//Bindcapsulematを利用するようにした。
//	//制限角度部分をBindcapsulematを利用するように修正する必要あり（バインドポーズ基点の角度にする必要あり）。
//
//	//Memo 20180922
//	//マニピュレータの座標系（Bindcapsulemat基準）のオイラー角による角度制限。検証中。
//
//	//Memo 20180925
//	//Eulerは通常IK時のものを使用。物理マトリックスは通常IKのdiffで計算。
//
//
//	CBone* firstbone = m_bonelist[srcboneno];
//	if (!firstbone){
//		_ASSERT(0);
//		return -1;
//	}
//
//	CBone* curbone = firstbone;
//	CBone* lastpar = curbone;
//	CBone* parentbone = curbone->GetParent();
//	CBone* gparentbone = 0;
//	if (!parentbone){
//		return 0;
//	}
//	gparentbone = parentbone->GetParent();
//	if (!gparentbone){
//		//grand parentがルートボーンの場合に、まだうまくいかないのでスキップ
//		return 0;
//	}
//	CBone* editboneforret = 0;
//	if (firstbone->GetParent()) {
//		editboneforret = firstbone->GetParent();
//	}
//	else {
//		editboneforret = firstbone;
//	}
//
//
//	ChaVector3 ikaxis = g_camtargetpos - g_camEye;
//	ChaVector3Normalize(&ikaxis, &ikaxis);
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
//
//	SetBefEditMat(erptr, curbone, maxlevel);
//
//	float currate = g_physicsmvrate;
//
//	MOTINFO* curmi = GetCurMotInfo();
//	if (!curmi) {
//		return 0;
//	}
//
//	if (parentbone){
//		//CBone* gparentbone = parentbone->GetParent();
//		//CBone* childbone = parentbone->GetChild();
//		CBone* childbone = curbone;
//		int isfirst = 1;
//		//while (childbone){
//		if (childbone){
//			if (childbone->GetJointFPos() != parentbone->GetJointFPos()){
//				double dist0, dist1;// , dist2;
//				ChaVector3 parworld, chilworld;
//				ChaVector3 tmpparentfpos = parentbone->GetJointFPos();
//				ChaVector3 tmpchildfpos = childbone->GetJointFPos();
//				ChaMatrix tmpparentbtmat = parentbone->GetBtMat();
//				ChaVector3TransformCoord(&parworld, &tmpparentfpos, &tmpparentbtmat);
//				ChaVector3TransformCoord(&chilworld, &tmpchildfpos, &tmpparentbtmat);
//				//ChaVector3TransformCoord(&parworld, &(parentbone->GetJointFPos()), &(parentbone->GetBtMat()));
//				//ChaVector3TransformCoord(&chilworld, &(childbone->GetJointFPos()), &(childbone->GetBtMat()));//<--乱れる
//
//
//				ChaVector3 parbef, chilbef, tarbef;
//
//				//parent jointが乗っている平面上
//				parbef = parworld;
//				CalcShadowToPlane(chilworld, ikaxis, parworld, &chilbef);
//				CalcShadowToPlane(targetpos, ikaxis, parworld, &tarbef);
//
//				////ドラッグするジョイントが乗っている平面上
//				//chilbef = chilworld;
//				//CalcShadowToPlane(parworld, ikaxis, chilworld, &parbef);
//				//CalcShadowToPlane(targetpos, ikaxis, chilworld, &tarbef);
//				
//
//				ChaVector3 child2target;
//				child2target = targetpos - chilworld;
//				dist0 = ChaVector3LengthDbl(&child2target);
//
//
//				ChaVector3 vec0, vec1;
//				vec0 = chilbef - parbef;
//				ChaVector3Normalize(&vec0, &vec0);
//				vec1 = tarbef - parbef;
//				ChaVector3Normalize(&vec1, &vec1);
//
//				//double chkdot = ChaVector3DotDbl(&vec0, &vec1);
//				//if (fabs(chkdot) >= 0.999) {
//				//	return srcboneno;//!!!!!!!!!!!!!!!!!!!
//				//}
//
//				ChaVector3 rotaxis2;
//				ChaVector3Cross(&rotaxis2, (const ChaVector3*)&vec0, (const ChaVector3*)&vec1);
//				ChaVector3Normalize(&rotaxis2, &rotaxis2);
//
//				double rotdot2, rotrad2;
//				rotdot2 = ChaVector3DotDbl(&vec0, &vec1);
//				rotdot2 = min(1.0f, rotdot2);
//				rotdot2 = max(-1.0f, rotdot2);
//				rotrad2 = acos(rotdot2);
//				rotrad2 *= currate;
//				double firstframe = 0.0;
//				if (fabs(rotrad2) > 1.0e-4){
//					//double curframe = startframe;//!!!!!!!!!
//					double curframe = applyframe;//!!!!!!!!!
//
//					CQuaternion rotq1;
//					rotq1.SetAxisAndRot(rotaxis2, rotrad2);
//
//					ChaVector3 rotcenter;
//					ChaVector3 tmpparentfpos2 = parentbone->GetJointFPos();
//					ChaMatrix tmpparentbtmat2 = parentbone->GetBtMat();
//					ChaVector3TransformCoord(&rotcenter, &tmpparentfpos2, &tmpparentbtmat2);
//					ChaMatrix befrot, aftrot;
//					ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//					ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//					ChaMatrix rotmat1 = befrot * rotq1.MakeRotMatX() * aftrot;
//					//ChaMatrix newbtmat1 = parentbone->GetBtMat() * rotmat1;// *tramat;
//
//					ChaMatrix tmpmat0 = parentbone->GetBtMat() * rotmat1;// *tramat;
//					ChaVector3 tmppos;
//					ChaVector3TransformCoord(&tmppos, &tmpparentfpos2, &tmpmat0);
//					ChaVector3 diffvec;
//					diffvec = rotcenter - tmppos;
//					ChaMatrix tmptramat;
//					ChaMatrixIdentity(&tmptramat);
//					ChaMatrixTranslation(&tmptramat, diffvec.x, diffvec.y, diffvec.z);
//					ChaMatrix newbtmat1 = tmpmat0 * tmptramat;// *tramat;
//
//
//					ChaVector3 childworld1;
//					ChaVector3 tmpchildfpos2 = childbone->GetJointFPos();
//					ChaVector3TransformCoord(&childworld1, &tmpchildfpos2, &newbtmat1);
//					child2target = targetpos - childworld1;
//					dist1 = ChaVector3LengthDbl(&child2target);
//
//
//					//比較と選択
//					CQuaternion rotq;
//					ChaMatrix newbtmat;
//					//if ((dist0 * 0.9) >= dist1) {//10%近づかなかったらキャンセル。乱れ防止策。
//						rotq = rotq1;
//						newbtmat = newbtmat1;
//					//}
//					//else {
//					//	return srcboneno;
//					//}
//						bool infooutflag = true;
//					int onlycheck = 1;
//					int isbonemovable;
//					if (g_limitdegflag == true) {
//						isbonemovable = parentbone->SetWorldMat(infooutflag, 1, curmi->motid, curframe, newbtmat, onlycheck);
//						if (isbonemovable == 0) {
//							return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
//						}
//					}
//					else {
//						isbonemovable = 1;
//					}
//
//					ChaMatrix invfirstmat = parentbone->GetCurrentZeroFrameInvMat(0);
//					ChaMatrix diffworld = invfirstmat * newbtmat;
//					CBtObject* setbto = parentbone->GetBtObject(childbone);
//					ChaMatrix newrigidmat;
//					if (setbto) {
//						newrigidmat = setbto->GetFirstTransformMatX() * diffworld;
//					}
//					else {
//						::MessageBoxA(NULL, "IKRotateRagdoll : setbto NULL error", "error", MB_OK);
//						return -1;
//					}
//					
//					ChaVector3 newparentpos, newchildpos;
//					ChaVector3 jointfpos;
//					jointfpos = parentbone->GetJointFPos();
//					ChaVector3TransformCoord(&newparentpos, &jointfpos, &newbtmat);
//					jointfpos = childbone->GetJointFPos();
//					ChaVector3TransformCoord(&newchildpos, &jointfpos, &newbtmat);
//
//					ChaVector3 rigidcenter = (newparentpos + newchildpos) * 0.5f;
//
//
//					CQuaternion tmpq;
//					tmpq.RotationMatrix(newrigidmat);
//					btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);
//
//					btTransform worldtra;
//					worldtra.setIdentity();
//					worldtra.setRotation(btrotq);
//					worldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));
//
//					if (isbonemovable == 1) {
//						//CQuaternion eulnewrot;
//						//eulnewrot.SetRotationXYZ(&eulaxisq, eul);
//
//						setbto->GetRigidBody()->getMotionState()->setWorldTransform(worldtra);
//						//setbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
//						//setbto->GetRigidBody()->setDeactivationTime(30000.0);
//						setbto->GetRigidBody()->setDeactivationTime(0.0);
//
//						//if (isfirst == 1) {
//						//parentbone->SetBtMat(newbtmat);
//						//IKボーンはKINEMATICだから。
//						//parentbone->GetCurMp().SetWorldMat(newbtmat);
//						isfirst = 0;
//						//}
//
//						//MOTINFO* curmi = GetCurMotInfo();
//						//parentbone->SetWorldMat(0, curmi->motid, curmi->curframe, newbtmat);
//						//parentbone->SetWorldMat(1, curmi->motid, applyframe, newbtmat);
//					}
//
//				}
//
//			}
//
//			//childbone = childbone->GetBrother();
//		}
//	}
//	//return srcboneno;
//
//	if (editboneforret) {
//		return editboneforret->GetBoneNo();
//	}
//	else {
//		return srcboneno;
//	}
//
//
//}

//int CModel::PhysicsRotAxisDelta(CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
//{
//	if (!m_curmotinfo) {
//		return 0;
//	}
//
//
//
//	//float rotrad = delta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
//	int calcnum = 1;
//	//int calcnum = 4;//ctrlを押しながらドラッグでdelta * 0.25になっている。
//	float rotrad = delta / 10.0f * (float)PAI / 12.0f / (float)calcnum * g_physicsmvrate;//PhysicsIKプレートのEditRateスライダーで倍率設定.
//	if (fabs(rotrad) < (0.020 * DEG2PAI)) {
//		return 0;
//	}
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
//
//	CBone* curbone = m_bonelist[srcboneno];
//	if (!curbone) {
//		return 0;
//	}
//	CBone* firstbone = curbone;
//	CBone* parentbone = 0;
//	CBone* lastbone = 0;
//	CBone* topbone = GetTopBone();
//	CBone* editboneforret = 0;
//	if (firstbone->GetParent()) {
//		editboneforret = firstbone->GetParent();
//	}
//	else {
//		editboneforret = firstbone;
//	}
//
//
//	int calccnt;
//	for (calccnt = 0; calccnt < calcnum; calccnt++) {
//		curbone = firstbone;
//		if (!curbone) {
//			return 0;
//		}
//		lastbone = curbone;
//
//		float currate = 1.0f;
//
//		double firstframe = 0.0;
//		int levelcnt = 0;
//
//		//while (curbone && ((maxlevel == 0) || (levelcnt < maxlevel))) {
//		parentbone = curbone->GetParent();
//		if (!parentbone) {
//			break;
//		}
//
//		float rotrad2 = currate * rotrad;
//		//float rotrad2 = rotrad;
//		if (fabs(rotrad2) < (0.02 * DEG2PAI)) {
//			break;
//		}
//
//		CRigidElem* curre = GetRigidElem(curbone->GetBoneNo());
//		if (curre && curre->GetForbidRotFlag() != 0) {
//			//回転禁止の場合処理をスキップ
//			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
//			lastbone = curbone;
//			curbone = curbone->GetParent();
//			levelcnt++;
//			continue;
//		}
//
//
//		ChaVector3 axis0;
//		CQuaternion localq;
//		//if (axiskind == PICK_X) {
//		//	axis0.SetParams(1.0f, 0.0f, 0.0f);
//		//	localq.SetAxisAndRot(axis0, rotrad2);
//		//}
//		//else if (axiskind == PICK_Y) {
//		//	axis0.SetParams(0.0f, 1.0f, 0.0f);
//		//	localq.SetAxisAndRot(axis0, rotrad2);
//		//}
//		//else if (axiskind == PICK_Z) {
//		//	axis0.SetParams(0.0f, 0.0f, 1.0f);
//		//	localq.SetAxisAndRot(axis0, rotrad2);
//		//}
//		//else {
//		//	_ASSERT(0);
//		//	return 1;
//		//}
//		if ((axiskind == PICK_X) || (axiskind == PICK_SPA_X)) {
//			axis0.SetParams(selectmat.data[MATI_11], selectmat.data[MATI_12], selectmat.data[MATI_13]);
//		}
//		else if ((axiskind == PICK_Y) || (axiskind == PICK_SPA_Y)) {
//			axis0.SetParams(selectmat.data[MATI_21], selectmat.data[MATI_22], selectmat.data[MATI_23]);
//		}
//		else if ((axiskind == PICK_Z) || (axiskind == PICK_SPA_Z)) {
//			axis0.SetParams(selectmat.data[MATI_31], selectmat.data[MATI_32], selectmat.data[MATI_33]);
//		}
//		else {
//			_ASSERT(0);
//			return 1;
//		}
//		ChaVector3Normalize(&axis0, &axis0);
//		localq.SetAxisAndRot(axis0, rotrad2);
//		//ChaMatrix transmat;
//		//transmat = localq.MakeRotMatX();
//
//
//
//		//ChaMatrix selectmat;
//		ChaMatrix invselectmat;
//		//int multworld = 1;
//		//selectmat = curbone->CalcManipulatorMatrix(0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
//		ChaMatrixInverse(&invselectmat, NULL, &selectmat);
//
//
//		//ChaMatrix rotinvworld = firstbone->GetCurMp().GetInvWorldMat();
//		ChaMatrix rotinvworld = ChaMatrixInv(firstbone->GetBtMat());
//		rotinvworld.data[MATI_41] = 0.0f;
//		rotinvworld.data[MATI_42] = 0.0f;
//		rotinvworld.data[MATI_43] = 0.0f;
//		ChaMatrix rotselect = selectmat;
//		rotselect.data[MATI_41] = 0.0f;
//		rotselect.data[MATI_42] = 0.0f;
//		rotselect.data[MATI_43] = 0.0f;
//		ChaMatrix rotinvselect = invselectmat;
//		rotinvselect.data[MATI_41] = 0.0f;
//		rotinvselect.data[MATI_42] = 0.0f;
//		rotinvselect.data[MATI_43] = 0.0f;
//
//		CQuaternion rotinvselectq, rotselectq;
//		rotinvselectq.RotationMatrix(rotinvselect);
//		rotselectq.RotationMatrix(rotselect);
//
//		CQuaternion rotq;
//		if (parentbone) {
//			//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
//			////CMotionPoint transmp;
//			////transmp.CalcQandTra(transmat, firstbone);
//			////rotq = transmp.GetQ();
//			//rotq.RotationMatrix(transmat);
//			//rotq = rotinvselectq * localq * rotselectq;
//			rotq = localq;
//
//			ChaVector3 rotcenter;
//			ChaVector3 tmpparentfpos = parentbone->GetJointFPos();
//			ChaMatrix tmpparentbtmat = parentbone->GetBtMat();
//			ChaVector3TransformCoord(&rotcenter, &tmpparentfpos, &tmpparentbtmat);
//			ChaMatrix befrot, aftrot;
//			ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//			ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//			ChaMatrix rotmat1 = befrot * rotq.MakeRotMatX() * aftrot;
//			
//			//ChaMatrix newbtmat1 = parentbone->GetBtMat() * rotmat1;// *tramat;
//
//			ChaMatrix tmpmat0 = parentbone->GetBtMat() * rotmat1;// *tramat;
//			ChaVector3 tmppos;
//			ChaVector3TransformCoord(&tmppos, &tmpparentfpos, &tmpmat0);
//			ChaVector3 diffvec;
//			diffvec = rotcenter - tmppos;
//			ChaMatrix tmptramat;
//			ChaMatrixIdentity(&tmptramat);
//			ChaMatrixTranslation(&tmptramat, diffvec.x, diffvec.y, diffvec.z);
//			ChaMatrix newbtmat1 = tmpmat0 * tmptramat;// *tramat;
//
//			bool infooutflag = true;
//			int onlycheck = 1;
//			int isbonemovable;
//			if (g_limitdegflag == true) {
//				isbonemovable = parentbone->SetWorldMat(infooutflag, 1, m_curmotinfo->motid, startframe, newbtmat1, onlycheck);
//			}
//			else {
//				isbonemovable = 1;
//			}
//			//parentbone->RotBoneQReq(0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);
//			if (isbonemovable == 1) {
//				ChaMatrix invfirstmat = parentbone->GetCurrentZeroFrameInvMat(0);
//				ChaMatrix diffworld = invfirstmat * newbtmat1;
//				CBtObject* setbto = parentbone->GetBtObject(curbone);
//				ChaMatrix newrigidmat;
//				if (setbto) {
//					newrigidmat = setbto->GetFirstTransformMatX() * diffworld;
//				}
//				else {
//					::MessageBoxA(NULL, "IKRotateRagdoll : setbto NULL error", "error", MB_OK);
//					return -1;
//				}
//
//				ChaVector3 newparentpos, newchildpos;
//				ChaVector3 jointfpos;
//				jointfpos = parentbone->GetJointFPos();
//				ChaVector3TransformCoord(&newparentpos, &jointfpos, &newbtmat1);
//				jointfpos = curbone->GetJointFPos();
//				ChaVector3TransformCoord(&newchildpos, &jointfpos, &newbtmat1);
//
//				ChaVector3 rigidcenter = (newparentpos + newchildpos) * 0.5f;
//
//				CQuaternion tmpq;
//				tmpq.RotationMatrix(newrigidmat);
//				btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);
//
//				btTransform worldtra;
//				worldtra.setIdentity();
//				worldtra.setRotation(btrotq);
//				worldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));
//
//				setbto->GetRigidBody()->getMotionState()->setWorldTransform(worldtra);
//				setbto->GetRigidBody()->setDeactivationTime(0.0);
//
//			}
//
//
//		}
//
//		//}
//	}
//
//	if (editboneforret){
//		return editboneforret->GetBoneNo();
//	}
//	else{
//		return srcboneno;
//	}
//
//}

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
//	CBone* parentbone = curbone->GetParent();
//	if (!parentbone){
//		return 0;
//	}
//	if (!parentbone->GetParent()){
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
//	if (parentbone){
//		CBone* gparentbone = parentbone->GetParent();
//		CBone* childbone = parentbone->GetChild();
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
//				axis0.SetParams(1.0f, 0.0f, 0.0f);
//				localq.SetAxisAndRot(axis0, -rotrad2);
//			}
//			else if (axiskind == PICK_Y){
//				axis0.SetParams(0.0f, 1.0f, 0.0f);
//				localq.SetAxisAndRot(axis0, -rotrad2);
//			}
//			else if (axiskind == PICK_Z){
//				axis0.SetParams(0.0f, 0.0f, 1.0f);
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
//			if (parentbone->GetParent()){
//				gparrotmat = parentbone->GetParent()->GetBtMat();
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
//			parrotmat = parentbone->GetBtMat();
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
//			transmp.CalcQandTra(transmat, parentbone);
//			CQuaternion rotq;
//			rotq = transmp.GetQ();
//
//
//			ChaVector3 parworld, chilworld;
//			ChaVector3TransformCoord(&parworld, &(parentbone->GetJointFPos()), &(parentbone->GetBtMat()));
//			ChaVector3TransformCoord(&chilworld, &(childbone->GetJointFPos()), &(parentbone->GetBtMat()));
//
//
//			ChaMatrix newbtmat;
//			ChaVector3 rotcenter;
//			//rotcenter = parworld;
//			rotcenter = parentbone->GetJointFPos();
//
//			ChaMatrix befrot, aftrot;
//			ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//			ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//			ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
//			newbtmat = rotmat * parentbone->GetBtMat();
//
//
//			ChaMatrix firstworld = parentbone->GetStartMat2();
//			ChaMatrix invfirstworld;
//			ChaMatrixInverse(&invfirstworld, NULL, &firstworld);
//
//			ChaMatrix diffworld = invfirstworld * newbtmat;
//			CRigidElem* curre = parentbone->GetRigidElem(childbone);
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
//			ChaVector3 newparentpos, newchildpos;
//			ChaVector3 jointfpos;
//			jointfpos = parentbone->GetJointFPos();
//			ChaVector3TransformCoord(&newparentpos, &jointfpos, &newbtmat);
//			jointfpos = childbone->GetJointFPos();
//			ChaVector3TransformCoord(&newchildpos, &jointfpos, &newbtmat);
//
//			ChaVector3 rigidcenter = (newparentpos + newchildpos) * 0.5f;
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
//			if (gparentbone){
//				CBtObject* parbto = gparentbone->GetBtObject(parentbone);
//				if (parbto){
//					CBtObject* setbto = parentbone->GetBtObject(childbone);
//					if (setbto){
//						btMatrix3x3 firstworldmat = setbto->GetFirstTransformMat();
//
//						btGeneric6DofSpringConstraint* dofC = parbto->FindConstraint(parentbone, childbone);
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
//							ChaVector3 eul.SetParams(0.0f, 0.0f, 0.0f);
//							//worldtra.getBasis().getEulerZYX(eulz, euly, eulx, 1);
//							eulmat.getEulerZYX(eulz, euly, eulx, 1);
//							eul.x = eulx * 180.0 / PAI;
//							eul.y = euly * 180.0 / PAI;
//							eul.z = eulz * 180.0 / PAI;
//
//							int ismovable = parentbone->ChkMovableEul(eul);
//							char strmsg[256];
//							//sprintf_s(strmsg, 256, "needmodify 0 : neweul [%f, %f, %f] : dof [%f, %f, %f] : ismovable %d\n", eul.x, eul.y, eul.z, dofx, dofy, dofz, ismovable);
//							sprintf_s(strmsg, 256, "needmodify 0 : neweul [%f, %f, %f] : ismovable %d\n", eul.x, eul.y, eul.z, ismovable);
//							OutputDebugStringA(strmsg);
//
//
//							//Q2EulZYXbtのテスト　以下8行
//							CQuaternion eulq;
//							eulq = QMakeFromBtMat3x3(eulmat);
//							int needmodifyflag = 0;
//							ChaVector3 testbefeul.SetParams(0.0f, 0.0f, 0.0f);
//							ChaVector3 testeul.SetParams(0.0f, 0.0f, 0.0f);
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
//								parentbone->SetBtMat(newbtmat);
//								//IKボーンはKINEMATICだから。
//								parentbone->GetCurMp().SetWorldMat(newbtmat);
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


//int CModel::PhysicsRigControl(int depthcnt, CEditRange* erptr, int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig)
//{
//	if (depthcnt >= 10){
//		_ASSERT(0);
//		return 0;//!!!!!!!!!!!!!!!!!
//	}
//	depthcnt++;
//
//	if (!m_curmotinfo){
//		return 0;
//	}
//
//	int calcnum = 3;
//
//	float rotrad = srcdelta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
//	//if (fabs(rotrad) < (0.02f * (float)DEG2PAI)){
//	//	return 0;
//	//}
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
//
//	CBone* curbone = m_bonelist[srcboneno];
//	if (!curbone){
//		return 0;
//	}
//	CBone* parentbone = 0;
//	CBone* lastbone = 0;
//
//	//int calccnt;
//	//for (calccnt = 0; calccnt < calcnum; calccnt++){
//	double firstframe = 0.0;
//
//	int elemno;
//	for (elemno = 0; elemno < ikcustomrig.elemnum; elemno++){
//		RIGELEM currigelem = ikcustomrig.rigelem[elemno];
//		if (currigelem.rigrigboneno >= 0){
//			//rigのrig
//			CBone* rigrigbone = GetBoneByID(currigelem.rigrigboneno);
//			if (rigrigbone){
//				int rigrigno = currigelem.rigrigno;
//				if ((rigrigno >= 0) && (rigrigno < MAXRIGNUM)){
//					if (currigelem.transuv[uvno].enable == 1){
//						CUSTOMRIG rigrig = rigrigbone->GetCustomRig(rigrigno);
//						PhysicsRigControl(depthcnt, erptr, rigrigbone->GetBoneNo(), uvno, srcdelta * currigelem.transuv[uvno].applyrate, rigrig);
//					}
//				}
//			}
//		}
//		else{
//			//rigelem
//			curbone = GetBoneByID(currigelem.boneno);
//			if (curbone){
//				lastbone = curbone;
//				parentbone = curbone->GetParent();
//				if (parentbone){
//					int isfirst = 1;
//
//					//CBone* childbone = curbone;
//					//CBone* childbone = parentbone->GetChild();
//					//while (childbone){
//
//						int axiskind = currigelem.transuv[uvno].axiskind;
//						float rotrad2 = rotrad * currigelem.transuv[uvno].applyrate;
//
//						//float rotrad2;
//						//rotrad2 = rotrad;
//
//
//						//char str1[256];
//						//sprintf_s(str1, 256, "AXIS_KIND %d, rotrad %f, rotrad2 %f", axiskind, rotrad, rotrad2);
//						//::MessageBoxA(NULL, str1, "check", MB_OK);
//
//						if (currigelem.transuv[uvno].enable == 1){
//							if (fabs(rotrad2) >= (0.02f * (float)DEG2PAI)){
//								ChaVector3 axis0;
//								CQuaternion localq;
//								
//								//if (axiskind == AXIS_X){
//								//	axis0.SetParams(1.0f, 0.0f, 0.0f);
//								//	localq.SetAxisAndRot(axis0, rotrad2);
//								//}
//								//else if (axiskind == AXIS_Y){
//								//	axis0.SetParams(0.0f, 1.0f, 0.0f);
//								//	localq.SetAxisAndRot(axis0, rotrad2);
//								//}
//								//else if (axiskind == AXIS_Z){
//								//	axis0.SetParams(0.0f, 0.0f, 1.0f);
//								//	localq.SetAxisAndRot(axis0, rotrad2);
//								//}
//								//else{
//								//	_ASSERT(0);
//								//	return 1;
//								//}
//								
//								//axis0.SetParams(1.0f, 0.0f, 0.0f);
//								///localq.SetAxisAndRot(axis0, 0.0697 * 0.50);
//
//								if ((axiskind == PICK_X) || (axiskind == PICK_SPA_X)) {
//									axis0.SetParams(selectmat.data[MATI_11], selectmat.data[MATI_12], selectmat.data[MATI_13]);
//								}
//								else if ((axiskind == PICK_Y) || (axiskind == PICK_SPA_Y)) {
//									axis0.SetParams(selectmat.data[MATI_21], selectmat.data[MATI_22], selectmat.data[MATI_23]);
//								}
//								else if ((axiskind == PICK_Z) || (axiskind == PICK_SPA_Z)) {
//									axis0.SetParams(selectmat.data[MATI_31], selectmat.data[MATI_32], selectmat.data[MATI_33]);
//								}
//								else {
//									_ASSERT(0);
//									return 1;
//								}
//								ChaVector3Normalize(&axis0, &axis0);
//								localq.SetAxisAndRot(axis0, rotrad2);
//								ChaMatrix transmat;
//								transmat = localq.MakeRotMatX();
//
//								/*
//								ChaMatrix selectmat;
//								ChaMatrix invselectmat;
//								//selectmat = elemaxismat[elemno];
//								int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
//								selectmat = childbone->CalcManipulatorMatrix(1, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
//								ChaMatrixInverse(&invselectmat, NULL, &selectmat);
//
//								ChaMatrix rotinvworld = childbone->GetCurMp().GetInvWorldMat();
//								rotinvworld._41 = 0.0f;
//								rotinvworld._42 = 0.0f;
//								rotinvworld._43 = 0.0f;
//								ChaMatrix rotselect = selectmat;
//								rotselect._41 = 0.0f;
//								rotselect._42 = 0.0f;
//								rotselect._43 = 0.0f;
//								ChaMatrix rotinvselect = invselectmat;
//								rotinvselect._41 = 0.0f;
//								rotinvselect._42 = 0.0f;
//								rotinvselect._43 = 0.0f;
//
//
//								CQuaternion rotq;
//
//
//								ChaMatrix gparrotmat, invgparrotmat;
//								if (parentbone->GetParent()){
//									gparrotmat = parentbone->GetParent()->GetBtMat();
//									ChaMatrixInverse(&invgparrotmat, NULL, &gparrotmat);
//
//									gparrotmat._41 = 0.0f;
//									gparrotmat._42 = 0.0f;
//									gparrotmat._43 = 0.0f;
//
//									invgparrotmat._41 = 0.0f;
//									invgparrotmat._42 = 0.0f;
//									invgparrotmat._43 = 0.0f;
//								}
//								else{
//									ChaMatrixIdentity(&gparrotmat);
//									ChaMatrixIdentity(&invgparrotmat);
//								}
//								ChaMatrix parrotmat, invparrotmat;
//								parrotmat = parentbone->GetBtMat();
//								ChaMatrixInverse(&invparrotmat, NULL, &parrotmat);
//
//								*/
//								ChaMatrix transmat2;
//								//これでは体全体が反対を向いたときに回転方向が反対向きになる。
//								//transmat2 = invgparrotmat * rotq0.MakeRotMatX() * gparrotmat;
//
//								//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
//								ChaMatrix transmat = localq.MakeRotMatX();
//								//以下のようにすれば体全体が回転した時にも正常に動く
//								//transmat2 = invgparrotmat * parrotmat * transmat * invparrotmat * gparrotmat;
//								//CMotionPoint transmp;
//								//transmp.CalcQandTra(transmat2, parentbone);
//								//rotq = transmp.GetQ();
//
//								ChaMatrix rotmat2;
//								//rotmat2 = rotq.MakeRotMatX();
//								rotmat2 = transmat;
//
//								CBone* childbone = curbone->GetChild();
//								while (childbone){
//									btTransform curworldtra;
//									CBtObject* setbto = curbone->GetBtObject(childbone);
//									if (setbto){
//										curworldtra = setbto->GetRigidBody()->getWorldTransform();
//										btMatrix3x3 curworldmat = curworldtra.getBasis();
//
//										btMatrix3x3 rotworldmat;
//										rotworldmat.setIdentity();
//										rotworldmat.setValue(
//											rotmat2.data[MATI_11], rotmat2.data[MATI_12], rotmat2.data[MATI_13],
//											rotmat2.data[MATI_21], rotmat2.data[MATI_22], rotmat2.data[MATI_23],
//											rotmat2.data[MATI_31], rotmat2.data[MATI_32], rotmat2.data[MATI_33]);
//
//										btMatrix3x3 setrotmat;
//										setrotmat = curworldmat * rotworldmat;
//
//										curworldtra.setBasis(setrotmat);
//										setbto->GetRigidBody()->getMotionState()->setWorldTransform(curworldtra);
//									}
//									childbone = childbone->GetBrother();
//								}
//
//
//								/*
//								ChaVector3 parworld, chilworld;
//								ChaVector3TransformCoord(&parworld, &(parentbone->GetJointFPos()), &(parentbone->GetBtMat()));
//								ChaVector3TransformCoord(&chilworld, &(childbone->GetJointFPos()), &(parentbone->GetBtMat()));
//
//								ChaMatrix newbtmat;
//								ChaVector3 rotcenter;// = m_childworld;
//								rotcenter = parworld;
//
//								ChaMatrix befrot, aftrot;
//								ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//								ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//								ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
//								newbtmat = parentbone->GetBtMat() * rotmat;// *tramat;
//								//newbtmat = parentbone->GetCurMp().GetBtMat() * rotq.MakeRotMatX();// *tramat;
//
//
//								ChaMatrix firstworld = parentbone->GetStartMat2();
//								ChaMatrix invfirstworld;
//								ChaMatrixInverse(&invfirstworld, NULL, &firstworld);
//
//								ChaMatrix diffworld = invfirstworld * newbtmat;
//								CRigidElem* curre = parentbone->GetRigidElem(childbone);
//								ChaMatrix newrigidmat;
//								if (curre){
//									newrigidmat = curre->GetFirstcapsulemat() * diffworld;
//								}
//								else{
//									::MessageBoxA(NULL, "IKRotateRagdoll : curre NULL error", "error", MB_OK);
//									return -1;
//								}
//
//
//								ChaVector3 newparentpos, newchildpos;
//								ChaVector3 jointfpos;
//								jointfpos = parentbone->GetJointFPos();
//								ChaVector3TransformCoord(&newparentpos, &jointfpos, &newbtmat);
//								jointfpos = childbone->GetJointFPos();
//								ChaVector3TransformCoord(&newchildpos, &jointfpos, &newbtmat);
//
//								ChaVector3 rigidcenter = (newparentpos + newchildpos) * 0.5f;
//
//
//								CQuaternion tmpq;
//								tmpq.RotationMatrix(newrigidmat);
//								btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);
//
//
//								btTransform worldtra;
//								worldtra.setIdentity();
//								worldtra.setRotation(btrotq);
//								worldtra.setOrigin(btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z));
//
//								CBtObject* setbto = parentbone->GetBtObject(childbone);
//								if (setbto){
//									setbto->GetRigidBody()->getMotionState()->setWorldTransform(worldtra);
//									//setbto->GetRigidBody()->forceActivationState(ACTIVE_TAG);
//									//setbto->GetRigidBody()->setDeactivationTime(30000.0);
//								}
//
//								if (isfirst == 1){
//									parentbone->SetBtMat(newbtmat);
//									//IKボーンはKINEMATICだから。
//									parentbone->GetCurMp().SetWorldMat(newbtmat);
//									isfirst = 0;
//								}
//								*/
//
//							}
//						}
//
//						//childbone = childbone->GetBrother();
//					//}
//				}
//			}
//			else{
//				_ASSERT(0);
//			}
//		}
//	}
//
//	//if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone){
//	//if (g_absikflag && lastbone){
//	//		AdjustBoneTra(erptr, lastbone);
//	//}
//	//}
//
//	if (lastbone){
//		return lastbone->GetBoneNo();
//	}
//	else{
//		return srcboneno;
//	}
//}



//int CModel::PhysicsMV(CEditRange* erptr, int srcboneno, ChaVector3 diffvec)
//{
//
//	CBone* firstbone = m_bonelist[srcboneno];
//	if (!firstbone){
//		_ASSERT(0);
//		return -1;
//	}
//
//
//	ChaVector3 ikaxis = g_camtargetpos - g_camEye;
//	ChaVector3Normalize(&ikaxis, &ikaxis);
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
//
//	CBone* curbone = firstbone;
//	SetBefEditMat(erptr, curbone, 0);
//
//
//	ChaVector3 mvvec = diffvec * g_physicsmvrate;
//
//
//	int isfirst = 1;
//
//	PhysicsMVReq(m_topbone, mvvec);
//
//	if (m_topbone){
//		return m_topbone->GetBoneNo();
//	}
//	else{
//		return srcboneno;
//	}
//
//}

//int CModel::WithConstraint(CBone* srcbone)
//{
//	if (srcbone){
//
//		CBone* curbone = srcbone;
//		while (curbone){
//			if (curbone->GetPosConstraint() == 1){
//				return 1;
//			}
//			curbone = curbone->GetParent();
//		}
//	}
//	else{
//		return 0;
//	}
//
//	return 0;
//}


//void CModel::PhysicsMVReq(CBone* srcbone, ChaVector3 mvvec)
//{
//	if (srcbone){
//		CBone* curbone = srcbone;
//		CBone* parentbone = curbone->GetParent();
//
//		ChaMatrix mvmat;
//		ChaMatrixIdentity(&mvmat);
//		ChaMatrixTranslation(&mvmat, mvvec.x, mvvec.y, mvvec.z);
//
//
//		if (parentbone && (WithConstraint(curbone) == 0) && (parentbone->GetExcludeMv() == 0)){
//				
//			CBone* childbone = curbone;
//
//			//childbone->SetBtKinFlag(1);
//			CBtObject* srcbto = parentbone->GetBtObject(childbone);
//			if (srcbto){
//				//DWORD curflag = srcbto->GetRigidBody()->getCollisionFlags();
//				//srcbto->GetRigidBody()->setCollisionFlags(curflag | btCollisionObject::CF_KINEMATIC_OBJECT);
//				//if (srcbto->GetRigidBody()){
//				//	srcbto->GetRigidBody()->setDeactivationTime(0.0);
//				//	//srcbto->GetRigidBody()->setDeactivationTime(0.016 / 4.0);
//				//}
//
//				if (parentbone->GetBtKinFlag() == 1) {
//					ChaMatrix newbtmat;
//					newbtmat = parentbone->GetBtMat() * mvmat;// *tramat;
//
//					btTransform worldtra;
//					srcbto->GetRigidBody()->getMotionState()->getWorldTransform(worldtra);
//					btMatrix3x3 worldmat = worldtra.getBasis();
//					btVector3 worldpos = worldtra.getOrigin();
//
//
//					btTransform setworldtra;
//					setworldtra.setIdentity();
//					setworldtra.setBasis(worldmat);
//					setworldtra.setOrigin(btVector3(worldpos.x() + mvvec.x, worldpos.y() + mvvec.y, worldpos.z() + mvvec.z));
//					if (srcbto) {
//						srcbto->GetRigidBody()->getMotionState()->setWorldTransform(setworldtra);
//					}
//					else {
//						::MessageBoxA(NULL, "IKTraRagdoll : setbto NULL !!!!", "check", MB_OK);
//					}
//				}
//			}
//		}
//
//		if (curbone->GetChild()){
//			PhysicsMVReq(curbone->GetChild(), mvvec);
//		}
//		if (curbone->GetBrother()){
//			PhysicsMVReq(curbone->GetBrother(), mvvec);
//		}
//	}
//}



int CModel::RigControl(bool limitdegflag, int depthcnt, CEditRange* erptr, int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig, int buttonflag)
{
	ChaCalcFunc chacalcfunc;

	bool fromiktarget = false;


	if (depthcnt >= 10){
		_ASSERT(0);
		return 0;//!!!!!!!!!!!!!!!!!
	}
	depthcnt++;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	//rigからrigを呼ぶので　再入禁止には別手段が必要
	//if (g_underIKRot == true) {
	//	return 0;//2023/01/27　再入禁止でギザギザは無くなるかどうかテスト
	//}

	//g_underIKRot = true;//2023/01/14 parent limited or not


	int calcnum = 3;

	float rotrad = srcdelta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
	//if (fabs(rotrad) < (0.020 * DEG2PAI)){
	if (fabs(rotrad) < (0.020 * DEG2PAI)) {//2023/02/11
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}	


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone){
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (curbone->IsNotSkeleton()) {
		return 0;
	}


	CBone* parentbone = 0;
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
							RigControl(limitdegflag, depthcnt, erptr, rigrigbone->GetBoneNo(), 
								uvno, srcdelta * currigelem.transuv[uvno].applyrate, rigrig, buttonflag);
						}
					}
				}
			}
			else{
				//rigelem
				curbone = GetBoneByID(currigelem.boneno);
				if (curbone && curbone->IsSkeleton()){
					lastbone = curbone;
					parentbone = curbone->GetParent(false);

					CBone* aplybone;
					if (parentbone && parentbone->IsSkeleton()) {
						aplybone = parentbone;
					}
					else {
						aplybone = curbone;
					}


					//int axiskind = currigelem.transuv[uvno].axiskind;
					int rigaxiskind = currigelem.transuv[uvno].axiskind;
					int rigaxis0, rigaxis1;
					rigaxis0 = rigaxiskind / 3;//BONEAXIS_CURRENT, BONEAXIS_PARENT, BONEAXIS_GLOBAL, BONEAXIS_BINDPOSE
					rigaxis1 = rigaxiskind % 3;//AXIS_X, AXIS_Y, AXIS_Z

					float rotrad2 = rotrad * currigelem.transuv[uvno].applyrate;
					if (currigelem.transuv[uvno].enable == 1){
						//if (fabs(rotrad2) >= (0.020 * DEG2PAI)){
						if (fabs(rotrad2) >= (0.020 * DEG2PAI)) {//2023/02/11

							if (fabs(rotrad2) > (0.0550 * DEG2PAI)) {//2023/02/11
								rotrad2 = 0.0550f * fabs(rotrad2) / rotrad2;
							}

							ChaVector3 axis0;
							CQuaternion localq;
							//if (axiskind == AXIS_X){
							//	axis0.SetParams(1.0f, 0.0f, 0.0f);
							//	localq.SetAxisAndRot(axis0, rotrad2);
							//}
							//else if (axiskind == AXIS_Y){
							//	axis0.SetParams(0.0f, 1.0f, 0.0f);
							//	localq.SetAxisAndRot(axis0, rotrad2);
							//}
							//else if (axiskind == AXIS_Z){
							//	axis0.SetParams(0.0f, 0.0f, 1.0f);
							//	localq.SetAxisAndRot(axis0, rotrad2);
							//}
							//else{
							//	_ASSERT(0);
							//	return 1;
							//}

							ChaMatrix selectmat;
							ChaMatrix invselectmat;
							selectmat.SetIdentity();
							invselectmat.SetIdentity();
							//selectmat = elemaxismat[elemno];
							//int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
							//selectmat = curbone->CalcManipulatorMatrix(0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
							if (curbone && curbone->GetParent(false)) {
								//curbone->GetParent(false)->CalcAxisMatX_Manipulator(limitdegflag, g_boneaxis, 0, curbone, &selectmat, 0);
								curbone->GetParent(false)->CalcAxisMatX_Manipulator(limitdegflag, rigaxis0, 0, curbone, &selectmat, 0);
							}
							else {
								selectmat.SetIdentity();
							}
							ChaMatrixInverse(&invselectmat, NULL, &selectmat);
							if (rigaxis1 == AXIS_X) {
								axis0 = selectmat.GetRow(0);
							}
							else if (rigaxis1 == AXIS_Y) {
								axis0 = selectmat.GetRow(1);
							}
							else if (rigaxis1 == AXIS_Z) {
								axis0 = selectmat.GetRow(2);
							}
							else {
								_ASSERT(0);
								//g_underIKRot = false;//2023/01/14 parent limited or not
								return 1;
							}
							ChaVector3Normalize(&axis0, &axis0);
							localq.SetAxisAndRot(axis0, rotrad2);
							//ChaMatrix transmat;
							//transmat = localq.MakeRotMatX();


							////ChaMatrix rotinvworld = curbone->GetCurMp().GetInvWorldMat();
							//ChaMatrix rotinvworld = curbone->GetCurrentLimitedWorldMat();
							//rotinvworld.data[MATI_41] = 0.0f;
							//rotinvworld.data[MATI_42] = 0.0f;
							//rotinvworld.data[MATI_43] = 0.0f;
							//ChaMatrix rotselect = selectmat;
							//rotselect.data[MATI_41] = 0.0f;
							//rotselect.data[MATI_42] = 0.0f;
							//rotselect.data[MATI_43] = 0.0f;
							//ChaMatrix rotinvselect = invselectmat;
							//rotinvselect.data[MATI_41] = 0.0f;
							//rotinvselect.data[MATI_42] = 0.0f;
							//rotinvselect.data[MATI_43] = 0.0f;

							//CQuaternion rotq;
							CQuaternion qForRot;
							CQuaternion qForHipsRot;

							//curbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
							// 
							//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
							curbone->SaveSRT(limitdegflag, curmotid, startframe);


							//2023/01/23 : Rigの場合は　回転できなくても処理を継続
							////2023/01/22 : topposスライダーの位置のフレーム(３D表示中のフレーム)において　
							////制限角度により　回転出来ない場合は　リターンする
							////if (g_limitdegflag != false) {
							//if ((g_limitdegflag != false) && (g_wallscrapingikflag == 0)) {//2023/01/23
							//	int ismovable = IsMovableRot(m_curmotinfo->motid, applyframe, applyframe, 
							//		localq, curbone, aplybone);
							//	if (ismovable == 0) {
							//		//g_underIKRot = false;//2023/01/14 parent limited or not
							//		if (lastbone) {
							//			return lastbone->GetBoneNo();
							//		}
							//		else {
							//			return srcboneno;
							//		}
							//	}
							//}


							if (keynum >= 2){
								int keyno = 0;
								double curframe;
								for (curframe = startframe; curframe <= endframe; curframe += 1.0){

									if (aplybone && (g_pseudolocalflag == 1)) {
										bool calcaplyflag = true;
										chacalcfunc.CalcQForRot(limitdegflag, calcaplyflag,
											curmotid, curframe, applyframe, localq,
											curbone, aplybone,
											&qForRot, &qForHipsRot);
									}
									else{
										qForRot = localq;
										qForHipsRot = localq;
									}

									double changerate;
									//if (curframe <= applyframe){
									//	changerate = 1.0 / (applyframe - startframe + 1);
									//}
									//else{
									//	changerate = 1.0 / (endframe - applyframe + 1);
									//}
									changerate = (double)(*(g_motionbrush_value + (int)curframe));

									bool infooutflag;
									if (curframe == applyframe) {
										infooutflag = true;
									}
									else {
										infooutflag = false;
									}


									if (keyno == 0){
										firstframe = curframe;
									}
									if (g_absikflag == 0){
										if (g_slerpoffflag == 0){
											CQuaternion endq;
											CQuaternion curqForRot;
											CQuaternion curqForHipsRot;
											endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
											qForRot.Slerp2(endq, 1.0 - changerate, &curqForRot);
											qForHipsRot.Slerp2(endq, 1.0 - changerate, &curqForHipsRot);

											curbone->RotAndTraBoneQReq(limitdegflag, 0, RoundingTime(startframe),
												infooutflag, 0, curmotid, curframe, 
												curqForRot, curqForHipsRot, fromiktarget);
										}
										else{
											curbone->RotAndTraBoneQReq(limitdegflag, 0, RoundingTime(startframe),
												infooutflag, 0, curmotid, curframe, 
												qForRot, qForHipsRot, fromiktarget);
										}
									}
									else{
										if (keyno == 0){
											curbone->RotAndTraBoneQReq(limitdegflag, 0, RoundingTime(startframe),
												infooutflag, 0, curmotid, curframe, 
												qForRot, qForHipsRot, fromiktarget);
										}
										else{
											curbone->SetAbsMatReq(limitdegflag, 0, curmotid, curframe, firstframe);
										}
									}
									keyno++;
								}

							}
							else{
								bool infooutflag = true;
								qForRot = localq;
								qForHipsRot = localq;
								curbone->RotAndTraBoneQReq(limitdegflag, 0, RoundingTime(startframe),
									infooutflag, 0, curmotid, GetCurrentFrame(),
									qForRot, qForHipsRot, fromiktarget);
							}
							if (g_applyendflag == 1){
								//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
								if (GetTopBone(false)){
									int tolast;
									for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++){
										//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
										GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
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

	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (lastbone){
		return lastbone->GetBoneNo();
	}
	else{
		return srcboneno;
	}
}

int CModel::RigControlUnderRig(bool limitdegflag, int depthcnt, 
	CEditRange* erptr, int srcboneno, 
	int uvno, float srcdelta, 
	CUSTOMRIG ikcustomrig, int buttonflag)
{
	ChaCalcFunc chacalcfunc;

	SetIKTargetVec();


	if (depthcnt >= 10) {
		_ASSERT(0);
		return -1;//!!!!!!!!!!!!!!!!!
	}
	depthcnt++;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	//rigからrigを呼ぶので　再入禁止には別手段が必要
	//if (g_underIKRot == true) {
	//	return 0;//2023/01/27　再入禁止でギザギザは無くなるかどうかテスト
	//}
	//g_underIKRot = true;//2023/01/14 parent limited or not


	//float rotrad = srcdelta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
	float rotrad = srcdelta / 10.0f * (float)PAI / 20.0f * g_physicsmvrate;//2023/03/04
	//if (fabs(rotrad) < (0.020 * DEG2PAI)) {//2023/02/11
	if (fabs(rotrad) < (0.010 * DEG2PAI)) {//2023/03/04
		return 0;
	}
	if (fabs(rotrad) > (0.0550 * DEG2PAI)) {//2023/03/04
		rotrad = 0.0550f * fabs(rotrad) / rotrad;
	}


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone) {
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return -1;
	}
	if (curbone->IsNotSkeleton()) {
		return -1;
	}


	CBone* parentbone = 0;
	CBone* lastbone = 0;



	//int calccnt;
	//for (calccnt = 0; calccnt < calcnum; calccnt++){
	double firstframe = 0.0;

	int elemno;
	for (elemno = 0; elemno < ikcustomrig.elemnum; elemno++) {
		RIGELEM currigelem = ikcustomrig.rigelem[elemno];

		//2023/03/04
		//同じ軸で２倍回転しないように　重複処理はスキップ
		if ((uvno == 1) &&
			(currigelem.transuv[0].enable == 1) && (currigelem.transuv[1].enable == 1) &&
			(currigelem.transuv[0].axiskind == currigelem.transuv[1].axiskind)) {
			continue;
		}

		if (currigelem.rigrigboneno >= 0) {
			//rigのrig
			CBone* rigrigbone = GetBoneByID(currigelem.rigrigboneno);
			if (rigrigbone) {
				int rigrigno = currigelem.rigrigno;
				if ((rigrigno >= 0) && (rigrigno < MAXRIGNUM)) {
					if (currigelem.transuv[uvno].enable == 1) {
						CUSTOMRIG rigrig = rigrigbone->GetCustomRig(rigrigno);
						RigControlUnderRig(limitdegflag, depthcnt, erptr, rigrigbone->GetBoneNo(),
							uvno, srcdelta * currigelem.transuv[uvno].applyrate, rigrig, buttonflag);
					}
				}
			}
		}
		else {
			//rigelem
			curbone = GetBoneByID(currigelem.boneno);
			if (curbone) {
				lastbone = curbone;
				parentbone = curbone->GetParent(false);

				CBone* aplybone;
				if (parentbone && parentbone->IsSkeleton()) {
					aplybone = parentbone;
				}
				else {
					aplybone = curbone;
				}


				int rigaxiskind = currigelem.transuv[uvno].axiskind;
				int rigaxis0, rigaxis1;
				rigaxis0 = rigaxiskind / 3;//BONEAXIS_CURRENT, BONEAXIS_PARENT, BONEAXIS_GLOBAL, BONEAXIS_BINDPOSE
				rigaxis1 = rigaxiskind % 3;//AXIS_X, AXIS_Y, AXIS_Z

				float rotrad2 = rotrad * currigelem.transuv[uvno].applyrate;
				if (currigelem.transuv[uvno].enable == 1) {

					ChaVector3 axis0;
					CQuaternion localq;

					ChaMatrix selectmat;
					ChaMatrix invselectmat;
					selectmat.SetIdentity();
					invselectmat.SetIdentity();
					if (curbone && curbone->GetParent(false)) {
						//curbone->GetParent(false)->CalcAxisMatX_Manipulator(limitdegflag, g_boneaxis, 0, curbone, &selectmat, 0);
						
						//2024/01/09
						curbone->GetParent(false)->CalcAxisMatX_Manipulator(limitdegflag, rigaxis0, 0, curbone, &selectmat, 0);
					}
					else {
						selectmat.SetIdentity();
					}
					ChaMatrixInverse(&invselectmat, NULL, &selectmat);
					if (rigaxis1 == AXIS_X) {
						axis0 = selectmat.GetRow(0);
					}
					else if (rigaxis1 == AXIS_Y) {
						axis0 = selectmat.GetRow(1);
					}
					else if (rigaxis1 == AXIS_Z) {
						axis0 = selectmat.GetRow(2);
					}
					else {
						_ASSERT(0);
						//g_underIKRot = false;//2023/01/14 parent limited or not
						return -1;
					}
					ChaVector3Normalize(&axis0, &axis0);


					//if (fabs(rotrad2) >= (0.020 * DEG2PAI)) {//2023/02/11
					if (fabs(rotrad2) >= (0.010 * DEG2PAI)) {//2023/03/04

						if (fabs(rotrad2) > (0.0550 * DEG2PAI)) {//2023/02/11
							rotrad2 = 0.0550f * fabs(rotrad2) / rotrad2;
						}
						localq.SetAxisAndRot(axis0, rotrad2);

						CQuaternion qForRot;
						CQuaternion qForHipsRot;

						//curbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
						// 
						//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
						curbone->SaveSRT(limitdegflag, curmotid, startframe);

						int ismovable2 = 1;

						if (keynum >= 2) {
							int keyno = 0;
							double curframe = applyframe;//!!!!!!
							bool keynum1flag = false;
							bool postflag = false;
							bool fromiktarget = false;
							ismovable2 = chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
								keyno, 
								//curbone, aplybone,
								curbone, curbone,//2024/04/18 applybone-->curbone Test 1009_5モデル167フレームをapplyframeにして足の青いリグドラッグ
								curmotid, curframe, startframe, applyframe,
								localq, keynum1flag, postflag, fromiktarget);
						}
						else {
							bool keynum1flag = true;
							bool postflag = false;
							bool fromiktarget = false;
							ismovable2 = chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
								0, 
								//curbone, aplybone,
								curbone, curbone,//2024/04/18 applybone-->curbone Test 1009_5モデル167フレームをapplyframeにして足の青いリグドラッグ
								curmotid, GetCurrentFrame(), startframe, applyframe,
								localq, keynum1flag, postflag, fromiktarget);
						}


						//2023/03/04 制限角度に引っ掛かった場合には　やめて　次のジョイントの回転へ
						if ((ismovable2 == 0) && (g_wallscrapingikflag == 0)) {
							continue;
						}

						//curboneのrotqを保存
						IKROTREC currotrec;
						currotrec.rotq = localq;
						currotrec.targetpos.SetParams(0.0f, 0.0f, 0.0f);
						currotrec.lessthanthflag = false;
						if (uvno == 0) {
							curbone->AddIKRotRec_U(currotrec);
						}
						else if (uvno == 1) {
							curbone->AddIKRotRec_V(currotrec);
						}
						else {
							_ASSERT(0);
							return -1;
						}
						
						if (g_applyendflag == 1) {
							//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
							if (GetTopBone(false)) {
								int tolast;
								for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
									GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
								}
							}
						}
					}
					else {
						//rotqの回転角度が1e-4より小さい場合
						//ウェイトが小さいフレームにおいても　IKTargetが走るように記録する必要がある
						if (fabs(rotrad2) > (0.0550 * DEG2PAI)) {//2023/02/11
							rotrad2 = 0.0550f * fabs(rotrad2) / rotrad2;
						}
						localq.SetAxisAndRot(axis0, rotrad2);

						IKROTREC currotrec;
						currotrec.rotq = localq;
						currotrec.targetpos.SetParams(0.0f, 0.0f, 0.0f);
						currotrec.lessthanthflag = true;//!!!!!!!!!!!
						if (uvno == 0) {
							curbone->AddIKRotRec_U(currotrec);
						}
						else if (uvno == 1) {
							curbone->AddIKRotRec_V(currotrec);
						}
						else {
							_ASSERT(0);
							return -1;
						}
					}
				}
			}
			else {
				_ASSERT(0);
			}
		}
	}

	if (lastbone) {
		return lastbone->GetBoneNo();
	}
	else {
		return srcboneno;
	}
}

int CModel::RigControlPostRig(bool limitdegflag, int depthcnt, 
	CEditRange* erptr, int srcboneno, 
	int uvno,
	CUSTOMRIG ikcustomrig, int buttonflag)
{
	ChaCalcFunc chacalcfunc;

	if (depthcnt >= 10) {
		_ASSERT(0);
		return 0;//!!!!!!!!!!!!!!!!!
	}
	depthcnt++;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	//rigからrigを呼ぶので　再入禁止には別手段が必要
	//if (g_underIKRot == true) {
	//	return 0;//2023/01/27　再入禁止でギザギザは無くなるかどうかテスト
	//}
	//g_underIKRot = true;//2023/01/14 parent limited or not


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);


	SetPostIKFrame(RoundingTime(startframe), RoundingTime(endframe));//2023/10/20


	double roundingstartframe, roundingendframe, roundingapplyframe;
	roundingstartframe = RoundingTime(startframe);
	roundingendframe = RoundingTime(endframe);
	roundingapplyframe = RoundingTime(applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone) {
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (curbone->IsNotSkeleton()) {
		return 0;
	}

	CBone* parentbone = 0;
	CBone* lastbone = 0;



	//int calccnt;
	//for (calccnt = 0; calccnt < calcnum; calccnt++){
	double firstframe = 0.0;

	int elemno;
	for (elemno = 0; elemno < ikcustomrig.elemnum; elemno++) {
		RIGELEM currigelem = ikcustomrig.rigelem[elemno];

		//2023/03/04
		//同じ軸で２倍回転しないように　重複処理はスキップ
		if ((uvno == 1) &&
			(currigelem.transuv[0].enable == 1) && (currigelem.transuv[1].enable == 1) &&
			(currigelem.transuv[0].axiskind == currigelem.transuv[1].axiskind)) {
			continue;
		}

		if (currigelem.rigrigboneno >= 0) {
			//rigのrig
			CBone* rigrigbone = GetBoneByID(currigelem.rigrigboneno);
			if (rigrigbone) {
				int rigrigno = currigelem.rigrigno;
				if ((rigrigno >= 0) && (rigrigno < MAXRIGNUM)) {
					if (currigelem.transuv[uvno].enable == 1) {
						CUSTOMRIG rigrig = rigrigbone->GetCustomRig(rigrigno);
						RigControlPostRig(limitdegflag, depthcnt, erptr, rigrigbone->GetBoneNo(),
							uvno, rigrig, buttonflag);
					}
				}
			}
		}
		else {
			//rigelem
			curbone = GetBoneByID(currigelem.boneno);
			if (curbone) {
				lastbone = curbone;
				parentbone = curbone->GetParent(false);

				CBone* aplybone;
				if (parentbone && parentbone->IsSkeleton()) {
					aplybone = parentbone;
				}
				else {
					aplybone = curbone;
				}

				int rotrecsize;
				if (uvno == 0) {
					rotrecsize = curbone->GetIKRotRecSize_U();
				}
				else if (uvno == 1) {
					rotrecsize = curbone->GetIKRotRecSize_V();
				}
				else {
					_ASSERT(0);
					return 0;
				}
				if (rotrecsize > 0) {
					int rotrecno;
					for (rotrecno = 0; rotrecno < rotrecsize; rotrecno++) {
						IKROTREC currotrec;
						if (uvno == 0) {
							currotrec = curbone->GetIKRotRec_U(rotrecno);
						}
						else if (uvno == 1) {
							currotrec = curbone->GetIKRotRec_V(rotrecno);
						}
						else {
							_ASSERT(0);
							return 0;
						}
						CQuaternion localq = currotrec.rotq;
						bool lessthanthflag = currotrec.lessthanthflag;

						//curbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
						// 
						//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
						curbone->SaveSRT(limitdegflag, curmotid, roundingstartframe);

						//2023/01/23 : Rigの場合は　回転できなくても処理を継続
						
						if (lessthanthflag == false) {
							if (keynum >= 2) {
								int keyno = 0;
								bool keynum1flag = false;
								bool postflag = true;
								bool fromiktarget = false;

								if (m_PostIKThreads) {
									//SetUnderPostIK(true);
									int updatecount;
									for (updatecount = 0; updatecount < POSTIK_THREADSNUM; updatecount++) {
										CThreadingPostIK* curupdate = m_PostIKThreads + updatecount;
										curupdate->IKRotateOneFrame(this, limitdegflag, erptr,
											keyno,
											//curbone, aplybone,
											curbone, curbone,//2024/04/18 applybone-->curbone Test 1009_5モデル167フレームをapplyframeにして足の青いリグドラッグ
											curmotid, startframe, applyframe,
											localq, keynum1flag, postflag, fromiktarget);
									}
									WaitPostIKFinished();
									//SetUnderPostIK(false);
								}
							}
						}

						if (g_applyendflag == 1) {
							//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
							if (GetTopBone(false)) {
								int tolast;
								for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
									GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
								}
							}
						}
					}
				}
			}
			else {
				_ASSERT(0);
			}
		}
	}


	if (uvno == 1) {
		double curframe;
		for (curframe = roundingstartframe; curframe <= roundingendframe; curframe += 1.0) {
			if (curframe != roundingapplyframe) {
				bool postflag = true;
				chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, curframe, postflag);
			}
		}
	}

	if (lastbone) {
		return lastbone->GetBoneNo();
	}
	else {
		return srcboneno;
	}
}



int CModel::InterpolateBetweenSelection(bool limitdegflag, int curmotid, double srcstartframe, double srcendframe, 
	CBone* interpolatebone, int srckind)
{
	//2023/02/06
	//return operationgJointNo

	int operatingjointno = -1;

	//if (!ExistCurrentMotion()){
	//	return operatingjointno;
	//}

	if (!interpolatebone) {
		return operatingjointno;
	}

	//CBone* parentone;
	//if (srcbone->GetParent()) {
	//	parentone = srcbone->GetParent();
	//}
	//else {
	//	parentone = srcbone;
	//}

	bool firstbroflag = false;

	if (srckind == 1) {
		//all
		bool oneflag = false;
		InterpolateBetweenSelectionReq(limitdegflag, GetTopBone(false), curmotid, srcstartframe, srcendframe, oneflag, firstbroflag);
		operatingjointno = GetTopBone(false)->GetBoneNo();
	}
	else if (srckind == 2) {
		//parent one
		bool oneflag = true;
		InterpolateBetweenSelectionReq(limitdegflag, interpolatebone, curmotid, srcstartframe, srcendframe, oneflag, firstbroflag);
		operatingjointno = interpolatebone->GetBoneNo();
	}
	else if (srckind == 3) {
		//parent deeper
		bool oneflag = false;
		InterpolateBetweenSelectionReq(limitdegflag, interpolatebone, curmotid, srcstartframe, srcendframe, oneflag, firstbroflag);
		operatingjointno = interpolatebone->GetBoneNo();
	}
	else {
		//unknown
	}


	UpdateMatrix(limitdegflag, &m_matWorld, &m_matView, &m_matProj, true, 0);

	return operatingjointno;
}

void CModel::InterpolateBetweenSelectionReq(bool limitdegflag, CBone* srcbone, 
	int curmotid,
	double srcstartframe, double srcendframe, bool oneflag, bool broflag)
{
	if (!srcbone){
		return;
	}
	if ((srcstartframe < 0.0) || (srcendframe < 0.0) || (srcendframe <= srcstartframe)){
		return;
	}
	//if (!ExistCurrentMotion()){
	//	return;
	//}

	double roundingstartframe = RoundingTime(srcstartframe);
	double roundingendframe = RoundingTime(srcendframe);

	if (srcbone){
		if (srcbone->IsSkeleton()) {
			//int curmotid = GetCurMotInfo().motid;
			CMotionPoint startmp, endmp;
			srcbone->CalcLocalInfo(limitdegflag, curmotid, roundingstartframe, &startmp);
			srcbone->CalcLocalInfo(limitdegflag, curmotid, roundingendframe, &endmp);
			CQuaternion startq, endq;
			ChaVector3 starttra, endtra;
			ChaVector3 startsc, endsc;
			startq = startmp.GetQ();
			endq = endmp.GetQ();
			starttra = srcbone->CalcLocalTraAnim(limitdegflag, curmotid, roundingstartframe);
			endtra = srcbone->CalcLocalTraAnim(limitdegflag, curmotid, roundingendframe);
			startsc = startmp.GetLocalScale();
			endsc = endmp.GetLocalScale();

			double frame;
			for (frame = roundingstartframe; frame <= roundingendframe; frame += 1.0) {
				CQuaternion setq;
				ChaVector3 settra;
				ChaVector3 setsc;
				if (IsEqualRoundingTime(frame, roundingstartframe)) {
					setq = startq;
					settra = starttra;
					setsc = startsc;
				}
				else if (IsEqualRoundingTime(frame, roundingendframe)) {
					setq = endq;
					settra = endtra;
					setsc = endsc;
				}
				else {
					double changerate;
					if ((frame >= roundingstartframe) && (frame <= roundingendframe)) {
						changerate = (double)(*(g_motionbrush_value + (int)(frame + 0.1)));
					}
					else {
						changerate = (frame - roundingstartframe) / (roundingendframe - roundingstartframe + 1.0);
					}

					startq.Slerp2(endq, changerate, &setq);
					settra = starttra + (endtra - starttra) * changerate;
					setsc = startsc + (endsc - startsc) * changerate;
				}
				int setchildflag1 = 1;
				CQuaternion iniq;
				iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
				ChaMatrix befwm = srcbone->GetWorldMat(limitdegflag, curmotid, frame, 0);

				//2024/01/31 support scale!
				srcbone->SetWorldMatFromQAndScaleAndTra(limitdegflag, setchildflag1, befwm, iniq, setq, setsc, settra, curmotid, frame);
			}
		}
		else if (srcbone->IsCamera() || srcbone->IsNullAndChildIsCamera()) {
			//int curmotid = GetCurMotInfo().motid;
			CMotionPoint* startmp = srcbone->GetMotionPoint(curmotid, roundingstartframe);
			CMotionPoint* endmp = srcbone->GetMotionPoint(curmotid, roundingendframe);
			if (startmp && endmp) {
				CQuaternion startqW, endqW;
				CQuaternion startqL, endqL;
				ChaMatrix startrotW, endrotW;
				ChaMatrix startrotL, endrotL;
				ChaVector3 starttraW, endtraW;
				ChaVector3 starttraL, endtraL;
				ChaVector3 startscW, endscW;
				ChaVector3 startscL, endscL;
				GetSRTMatrix(startmp->GetWorldMat(), &startscW, &startrotW, &starttraW);
				GetSRTMatrix(endmp->GetWorldMat(), &endscW, &endrotW, &endtraW);
				GetSRTMatrix(startmp->GetLocalMat(), &startscL, &startrotL, &starttraL);
				GetSRTMatrix(endmp->GetLocalMat(), &endscL, &endrotL, &endtraL);

				startqW.RotationMatrix(startrotW);
				endqW.RotationMatrix(endrotW);
				startqL.RotationMatrix(startrotL);
				endqL.RotationMatrix(endrotL);

				double frame;
				for (frame = roundingstartframe; frame <= roundingendframe; frame += 1.0) {
					CQuaternion setqW;
					CQuaternion setqL;
					ChaVector3 settraW;
					ChaVector3 settraL;
					ChaVector3 setscW;
					ChaVector3 setscL;
					if (IsEqualRoundingTime(frame, roundingstartframe)) {
						setqW = startqW;
						settraW = starttraW;
						setscW = startscW;
						setqL = startqL;
						settraL = starttraL;
						setscL = startscL;
					}
					else if (IsEqualRoundingTime(frame, roundingendframe)) {
						setqW = endqW;
						settraW = endtraW;
						setscW = endscW;
						setqL = endqL;
						settraL = endtraL;
						setscL = endscL;
					}
					else {
						double changerate;
						if ((frame >= roundingstartframe) && (frame <= roundingendframe)) {
							changerate = (double)(*(g_motionbrush_value + (int)(frame + 0.1)));
						}
						else {
							changerate = (frame - roundingstartframe) / (roundingendframe - roundingstartframe + 1.0);
						}

						startqW.Slerp2(endqW, changerate, &setqW);
						settraW = starttraW + (endtraW - starttraW) * changerate;
						setscW = startscW + (endscW - startscW) * changerate;
						startqL.Slerp2(endqL, changerate, &setqL);
						settraL = starttraL + (endtraL - starttraL) * changerate;
						setscL = startscL + (endscL - startscL) * changerate;
					}

					ChaMatrix setmatW, setmatL;
					ChaMatrix scmatW, scmatL;
					ChaMatrix tramatW, tramatL;
					scmatW.SetIdentity();
					scmatW.SetScale(setscW);
					scmatL.SetIdentity();
					scmatL.SetScale(setscL);
					tramatW.SetIdentity();
					tramatW.SetTranslation(settraW);
					tramatL.SetIdentity();
					tramatL.SetTranslation(settraL);
					
					setmatW = scmatW * setqW.MakeRotMatX() * tramatW;
					setmatL = scmatL * setqL.MakeRotMatX() * tramatL;

					CMotionPoint* setmp = srcbone->GetMotionPoint(curmotid, frame);
					if (setmp) {
						setmp->SetLocalMat(setmatL);
						setmp->SetWorldMat(setmatW);
						setmp->SetLimitedWM(setmatW);

						ChaVector3 seteul = srcbone->CalcLocalEulXYZ(false, -1, curmotid, frame, BEFEUL_BEFFRAME);
						setmp->SetLocalEul(seteul);
					}
					else {
						_ASSERT(0);
						return;
					}
				}
			}
			else {
				_ASSERT(0);
				return;
			}
		}

		if (oneflag == false) {
			if (srcbone->GetChild(false)) {
				bool broflag2 = true;
				InterpolateBetweenSelectionReq(limitdegflag, srcbone->GetChild(false), 
					curmotid, roundingstartframe, roundingendframe, oneflag, broflag2);
			}
			if (srcbone->GetBrother(false) && broflag) {
				InterpolateBetweenSelectionReq(limitdegflag, srcbone->GetBrother(false), 
					curmotid, roundingstartframe, roundingendframe, oneflag, broflag);
			}
		}
	}
}


////IKRotateAxisDeltaのPICK_Xで行うのでコメントアウト
//int CModel::TwistBoneAxisDelta(CEditRange* erptr, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
//{
//	if (!m_curmotinfo) {
//		return 0;
//	}
//
//
//	//int calcnum = 3;
//	int calcnum = 1;//今はtargetposを細かく刻んでいないので１で良い
//
//	//float rotrad = delta / 10.0f * (float)PAI / 12.0f;// / (float)calcnum;
//	float rotrad;
//	if (delta > 0.0f) {
//		rotrad = 1.0f * (float)DEG2PAI;
//	}
//	else if (delta < 0.0f) {
//		rotrad = -1.0f * (float)DEG2PAI;
//	}
//	else {
//		return 0;
//	}
//
//	if (fabs(rotrad) < (0.020 * DEG2PAI)) {
//		return 0;
//	}
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
//
//	CBone* curbone = m_bonelist[srcboneno];
//	if (!curbone) {
//		return 0;
//	}
//	CBone* firstbone = curbone;
//	CBone* parentbone = 0;
//	CBone* lastbone = 0;
//	CBone* topbone = GetTopBone();
//	CBone* editboneforret = 0;
//	if (firstbone->GetParent()) {
//		editboneforret = firstbone->GetParent();
//	}
//	else {
//		editboneforret = firstbone;
//	}
//
//	int calccnt;
//	for (calccnt = 0; calccnt < calcnum; calccnt++) {
//		curbone = firstbone;
//		if (!curbone) {
//			return 0;
//		}
//		lastbone = curbone;
//
//		float currate = 1.0f;
//
//		double firstframe = 0.0;
//		int levelcnt = 0;
//
//		while (curbone && ((maxlevel == 0) || (levelcnt < maxlevel))) {
//			parentbone = curbone->GetParent();
//
//			//parentboneの無いcurboneをドラッグした時はbreakしない
//			if (!parentbone && (firstbone != curbone)) {
//				break;
//			}
//
//			float rotrad2 = currate * rotrad;
//			//float rotrad2 = rotrad;
//			if (fabs(rotrad2) < (0.020 * DEG2PAI)) {
//				break;
//			}
//
//			CRigidElem* curre = GetRigidElem(curbone->GetBoneNo());
//			if (curre && curre->GetForbidRotFlag() != 0) {
//				//回転禁止の場合処理をスキップ
//				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
//				lastbone = curbone;
//				curbone = curbone->GetParent();
//				levelcnt++;
//				continue;
//			}
//
//
//			ChaVector3 axis0;
//			CQuaternion localq;
//			ChaVector3 curfpos, curfpos2;
//			ChaVector3 parfpos, parfpos2;
//			curfpos = curbone->GetJointFPos();
//			parfpos = parentbone->GetJointFPos();
//			ChaMatrix tmpapplywm = curbone->GetWorldMat(m_curmotinfo->motid, applyframe);
//			ChaMatrix tmpparentapplywm = parentbone->GetWorldMat(m_curmotinfo->motid, applyframe);
//			ChaVector3TransformCoord(&curfpos2, &curfpos, &tmpapplywm);
//			ChaVector3TransformCoord(&parfpos2, &parfpos, &tmpparentapplywm);
//			axis0 = curfpos2 - parfpos2;
//			ChaVector3Normalize(&axis0, &axis0);
//			localq.SetAxisAndRot(axis0, rotrad2);
//
//			//ChaMatrix selectmat;
//			ChaMatrix invselectmat;
//			//int multworld = 1;
//			//selectmat = curbone->CalcManipulatorMatrix(0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);//curmotinfo!!!
//			ChaMatrixInverse(&invselectmat, NULL, &selectmat);
//
//
//			ChaMatrix rotinvworld = firstbone->GetCurMp().GetInvWorldMat();
//			rotinvworld.data[MATI_41] = 0.0f;
//			rotinvworld.data[MATI_42] = 0.0f;
//			rotinvworld.data[MATI_43] = 0.0f;
//			ChaMatrix rotselect = selectmat;
//			rotselect.data[MATI_41] = 0.0f;
//			rotselect.data[MATI_42] = 0.0f;
//			rotselect.data[MATI_43] = 0.0f;
//			ChaMatrix rotinvselect = invselectmat;
//			rotinvselect.data[MATI_41] = 0.0f;
//			rotinvselect.data[MATI_42] = 0.0f;
//			rotinvselect.data[MATI_43] = 0.0f;
//
//			CQuaternion rotq;
//
//			if (keynum >= 2) {
//				int keyno = 0;
//				double curframe;
//				for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
//
//					CMotionPoint* curparmp = 0;
//					CMotionPoint* aplyparmp = 0;
//					if (parentbone) {
//						curparmp = parentbone->GetMotionPoint(m_curmotinfo->motid, curframe);
//						aplyparmp = parentbone->GetMotionPoint(m_curmotinfo->motid, applyframe);
//					}
//					else {
//						//parentboneが無いときには、curparmpとapplyparmpはcurrentboneのもの
//						curparmp = curbone->GetMotionPoint(m_curmotinfo->motid, curframe);
//						aplyparmp = curbone->GetMotionPoint(m_curmotinfo->motid, applyframe);
//					}
//					if (curparmp && aplyparmp && (g_pseudolocalflag == 1)) {
//						ChaMatrix curparrotmat = curparmp->GetWorldMat();
//						curparrotmat.data[MATI_41] = 0.0f;
//						curparrotmat.data[MATI_42] = 0.0f;
//						curparrotmat.data[MATI_43] = 0.0f;
//						ChaMatrix invcurparrotmat = curparmp->GetInvWorldMat();
//						invcurparrotmat.data[MATI_41] = 0.0f;
//						invcurparrotmat.data[MATI_42] = 0.0f;
//						invcurparrotmat.data[MATI_43] = 0.0f;
//						ChaMatrix aplyparrotmat = aplyparmp->GetWorldMat();
//						aplyparrotmat.data[MATI_41] = 0.0f;
//						aplyparrotmat.data[MATI_42] = 0.0f;
//						aplyparrotmat.data[MATI_43] = 0.0f;
//						ChaMatrix invaplyparrotmat = aplyparmp->GetInvWorldMat();
//						invaplyparrotmat.data[MATI_41] = 0.0f;
//						invaplyparrotmat.data[MATI_42] = 0.0f;
//						invaplyparrotmat.data[MATI_43] = 0.0f;
//
//						//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
//						ChaMatrix transmat = localq.MakeRotMatX();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//						ChaMatrix transmat2;
//						transmat2 = invcurparrotmat * aplyparrotmat * transmat * invaplyparrotmat * curparrotmat;
//						CMotionPoint transmp;
//						transmp.CalcQandTra(transmat2, firstbone);
//						rotq = transmp.GetQ();
//					}
//					else {
//						//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
//						ChaMatrix transmat = localq.MakeRotMatX();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//						//CMotionPoint transmp;
//						//transmp.CalcQandTra(transmat, firstbone);
//						//rotq = transmp.GetQ();
//						rotq.RotationMatrix(transmat);
//					}
//
//					double changerate;
//					//if (curframe <= applyframe){
//					//	changerate = 1.0 / (applyframe - startframe + 1);
//					//}
//					//else{
//					//	changerate = 1.0 / (endframe - applyframe + 1);
//					//}
//					changerate = (double)(*(g_motionbrush_value + (int)curframe));
//
//					if (keyno == 0) {
//						firstframe = curframe;
//					}
//
//					bool infooutflag;
//					if (curframe == applyframe) {
//						infooutflag = true;
//					}
//					else {
//						infooutflag = false;
//					}
//
//					if (g_absikflag == 0) {
//						if (g_slerpoffflag == 0) {
//							//double currate2;
//							CQuaternion endq;
//							CQuaternion curq;
//							endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//							//if (curframe <= applyframe){
//							//	currate2 = changerate * (curframe - startframe + 1);
//							//}
//							//else{
//							//	currate2 = changerate * (endframe - curframe + 1);
//							//}
//							//rotq.Slerp2(endq, 1.0 - currate2, &curq);
//							rotq.Slerp2(endq, 1.0 - changerate, &curq);
//
//							//_ASSERT(0);
//
//							if (parentbone) {
//								parentbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, curq);
//							}
//							else {
//								curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, curq);
//							}
//						}
//						else {
//							if (parentbone) {
//								parentbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, rotq);
//							}
//							else {
//								curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, rotq);
//							}
//						}
//					}
//					else {
//						if (keyno == 0) {
//							if (parentbone) {
//								parentbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, rotq);
//							}
//							else {
//								curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, rotq);
//							}
//						}
//						else {
//							if (parentbone) {
//								parentbone->SetAbsMatReq(0, m_curmotinfo->motid, curframe, firstframe);
//							}
//							else {
//								curbone->SetAbsMatReq(0, m_curmotinfo->motid, curframe, firstframe);
//							}
//						}
//					}
//					keyno++;
//				}
//
//			}
//			else {
//				//ChaMatrix transmat = rotinvselect * localq.MakeRotMatX() * rotselect;
//				ChaMatrix transmat = localq.MakeRotMatX();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				//CMotionPoint transmp;
//				//transmp.CalcQandTra(transmat, firstbone);
//				//rotq = transmp.GetQ();
//				rotq.RotationMatrix(transmat);
//
//				bool infooutflag = true;
//
//				if (parentbone) {
//					parentbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);
//				}
//				else {
//					curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);
//				}
//			}
//
//
//			if (g_applyendflag == 1) {
//				//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
//				if (m_topbone) {
//					int tolast;
//					for (tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++) {
//						//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
//						m_topbone->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
//					}
//				}
//			}
//
//			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
//			lastbone = curbone;
//			curbone = curbone->GetParent();
//			levelcnt++;
//		}
//
//		if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone) {
//			//if (g_absikflag && lastbone){
//			AdjustBoneTra(erptr, lastbone);
//		}
//	}
//
//	if (editboneforret) {
//		return editboneforret->GetBoneNo();
//	}
//	else {
//		return srcboneno;
//	}
//
//}


int CModel::IsMovableRot(bool limitdegflag, int srcmotid, double srcframe, double srcapplyframe, CQuaternion srcaddrot,
	CBone* srcrotbone, CBone* srcaplybone)
{
	ChaCalcFunc chacalcfunc;

	bool fromiktarget = false;

	//#############################################
	//movable : return 1,  not movable : return 0
	//#############################################
	if (!srcrotbone || !srcaplybone) {
		_ASSERT(0);
		return 0;//not movable
	}
	if (srcrotbone->IsNotSkeleton()) {
		return 0;//not movable
	}

	if (limitdegflag == false) {
		return 1;//movable
	}



	double roundingframe = RoundingTime(srcframe);
	double roundingapplyframe = RoundingTime(srcapplyframe);

	CQuaternion qForRot;
	CQuaternion qForHipsRot;
	bool calcaplyflag = true;
	chacalcfunc.CalcQForRot(limitdegflag, calcaplyflag,
		srcmotid, roundingframe, roundingapplyframe, srcaddrot,
		srcrotbone, srcaplybone,
		&qForRot, &qForHipsRot);

	double changerate;
	changerate = (double)(*(g_motionbrush_value + (int)roundingframe));

	bool infooutflag = true;

	CQuaternion endq;
	CQuaternion curqForRot;
	CQuaternion curqForHipsRot;
	endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	qForRot.Slerp2(endq, 1.0 - changerate, &curqForRot);
	qForHipsRot.Slerp2(endq, 1.0 - changerate, &curqForHipsRot);

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();
	double dummystartframe = 0.0;
	int onlycheckIsMovable = 0;
	srcrotbone->RotAndTraBoneQReq(limitdegflag, &onlycheckIsMovable, dummystartframe,
		infooutflag, 0, srcmotid, roundingframe, 
		curqForRot, curqForHipsRot, fromiktarget);

	return onlycheckIsMovable;
}


//int CModel::UpdateCameraMatFromENull(int cameramotid)
//{
//	//2024/06/05その後
//	//IsCamera()==trueのボーンに関してもアンドゥ処理をすることによりUpdateCameramatFromENull()は不要になった　コメントアウト
//
//	//2024/06/05
//	//カメラアニメのアンドゥーはIsNullAndChildIsCamera()==trueのボーンに対して行っている
//	//アンドゥ結果をIsCamera()==trueのボーンに反映するためにUpdateCameraMatFromENull()を呼ぶ
//
//	CBone* camerabone = nullptr;
//	CBone* enullbone = nullptr;
//	CAMERANODE* cnptr = GetCAMERANODE(cameramotid);
//	if (cnptr && cnptr->pbone && cnptr->pbone->GetParent(false)) {
//		camerabone = cnptr->pbone;
//		enullbone = cnptr->pbone->GetParent(false);
//	}
//	else {
//		_ASSERT(0);
//		return 1;
//	}
//
//	MOTINFO camerami = GetMotInfo(cameramotid);
//	if (camerami.motid > 0) {
//		int frameleng = (int)camerami.frameleng;
//		int curframe;
//		for (curframe = 0; curframe < frameleng; curframe++) {
//			CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, (double)curframe);
//			CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, (double)curframe);
//			if (enullmp && cameramp) {
//				ChaMatrix enullwm = enullmp->GetWorldMat();
//				ChaMatrix cameralocal = cameramp->GetLocalMat();
//				ChaMatrix camerawm = cameralocal * enullwm;
//				cameramp->SetWorldMat(camerawm);
//
//				//ChaVector3 neweul;
//				//neweul = enullbone->CalcLocalEulXYZ(false, -1, cameramotid, (double)curframe, BEFEUL_BEFFRAME);
//				//enullmp->SetLocalEul(neweul);
//			}
//			else {
//				_ASSERT(0);
//				return 1;
//			}
//		}
//	}
//	else {
//		_ASSERT(0);
//		return 1;
//	}
//
//	return 0;
//}

int CModel::CameraAnimDiffRotMatView(CEditRange* erptr, ChaMatrix befmatView, ChaMatrix newmatView)
{
	if (!erptr) {
		_ASSERT(0);
		return 0;
	}

	ChaCalcFunc chacalcfunc;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int cameramotid = GetCameraMotionId();
	MOTINFO camerami = GetMotInfo(cameramotid);
	if (camerami.motid <= 0) {
		return 0;
	}
	int curframeleng = IntTime(camerami.frameleng);


	CBone* camerabone = nullptr;
	CBone* enullbone = nullptr;
	CAMERANODE* cnptr = GetCAMERANODE(cameramotid);
	if (cnptr && cnptr->pbone && cnptr->pbone->GetParent(false)) {
		camerabone = cnptr->pbone;
		enullbone = cnptr->pbone->GetParent(false);
	}
	else {
		_ASSERT(0);
		return 0;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	ChaMatrix rotmat0;
	//rotmat0 = ChaMatrixInv(befmatView) * newmatView;
	rotmat0 = ChaMatrixInv(ChaMatrixInv(befmatView) * newmatView);//カメラをプラス回転することは世界をマイナス回転させること　つまりそれは逆行列
	CQuaternion rotq0;
	rotq0.RotationMatrix(rotmat0);


	if (keynum >= 2) {
		int keyno = 0;
		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {

			double changerate;
			changerate = (double)(*(g_motionbrush_value + (int)curframe));
			CQuaternion endq;
			CQuaternion qForRot;
			endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			rotq0.Slerp2(endq, 1.0 - changerate, &qForRot);

			CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, curframe, false);
			if (enullmp) {
				ChaMatrix parentLocalNodeAnimMat = enullmp->GetLocalMat();
				ChaMatrix parentGlobalNodeMat = enullmp->GetWorldMat();

				ChaMatrix befrot, aftrot;
				befrot.SetIdentity();
				aftrot.SetIdentity();
				//befrot.SetTranslation(-g_camtargetpos);
				//aftrot.SetTranslation(g_camtargetpos);
				befrot.SetTranslation(-g_camEye);
				aftrot.SetTranslation(g_camEye);
				ChaMatrix rotmat = qForRot.MakeRotMatX();
				ChaMatrix localaddrot = befrot * rotmat * aftrot;

				//式2024/06/04_1 : ChaMatrix newparentGlobalNodeMat = newparentLocalNodeAnimMat * ChaMatrixInv(parentLocalNodeAnimMat) * parentGlobalNodeMat;

				ChaMatrix newparentGlobalNodeMat = parentGlobalNodeMat * localaddrot;//式2024/06/04_2

				////式2024/06/04_1に//式2024/06/04_2を代入してlocalを求める
				ChaMatrix newparentLocalNodeAnimMat = parentGlobalNodeMat * localaddrot * ChaMatrixInv(parentGlobalNodeMat) * parentLocalNodeAnimMat;

				CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, curframe, false);
				if (cameramp) {
					ChaMatrix localnodeanimmat = cameramp->GetLocalMat();
					ChaMatrix newcameramat = localnodeanimmat * newparentGlobalNodeMat;

					//###########
					//upvecの補正
					//###########
					ChaVector3 befupvec;
					//befupvec = ChaMatrixInv(newcameramat).GetRow(1);
					befupvec = newcameramat.GetRow(1);
					ChaVector3Normalize(&befupvec, &befupvec);
					ChaVector3 aftupvec;
					//aftupvec = newmatView.GetRow(1);
					aftupvec = ChaMatrixInv(newmatView).GetRow(1);
					ChaVector3Normalize(&aftupvec, &aftupvec);
					CQuaternion rotupq0;
					rotupq0.RotationArc(befupvec, aftupvec);
					CQuaternion endqUp;
					CQuaternion qForRotUp;
					endqUp.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
					//rotupq0.Slerp2(endq, 1.0 - changerate, &qForRotUp);
					qForRotUp = rotupq0;//upvecの補正は補間しない
					ChaMatrix rotmatUp = qForRotUp.MakeRotMatX();

					ChaMatrix localaddrot2 = befrot * rotmatUp * aftrot;

					ChaMatrix newparentGlobalNodeMat2 = newparentGlobalNodeMat * localaddrot2;
					ChaMatrix newparentLocalNodeAnimMat2 = newparentGlobalNodeMat * localaddrot2 * ChaMatrixInv(newparentGlobalNodeMat) * newparentLocalNodeAnimMat;

					ChaMatrix localnodeanimmat2 = localnodeanimmat;
					ChaMatrix newcameramat2 = localnodeanimmat2 * newparentGlobalNodeMat2;

					enullmp->SetLocalMat(newparentLocalNodeAnimMat2);
					enullmp->SetWorldMat(newparentGlobalNodeMat2);
					enullmp->SetLimitedWM(newparentGlobalNodeMat2);

					cameramp->SetWorldMat(newcameramat2);
					cameramp->SetLimitedWM(newcameramat2);

					ChaVector3 neweul;
					neweul = enullbone->CalcLocalEulXYZ(false, -1, cameramotid, curframe, BEFEUL_BEFFRAME);
					enullmp->SetLocalEul(neweul);
				}
			}

			keyno++;
		}

	}
	else {

		double curframe = RoundingTime(applyframe);
		CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, curframe, false);
		if (enullmp) {
			ChaMatrix parentLocalNodeAnimMat = enullmp->GetLocalMat();
			ChaMatrix parentGlobalNodeMat = enullmp->GetWorldMat();

			ChaMatrix befrot, aftrot;
			befrot.SetIdentity();
			aftrot.SetIdentity();
			//befrot.SetTranslation(-g_camtargetpos);
			//aftrot.SetTranslation(g_camtargetpos);
			befrot.SetTranslation(-g_camEye);
			aftrot.SetTranslation(g_camEye);
			ChaMatrix rotmat = rotq0.MakeRotMatX();
			ChaMatrix localaddrot = befrot * rotmat * aftrot;

			//式2024/06/04_1 : ChaMatrix newparentGlobalNodeMat = newparentLocalNodeAnimMat * ChaMatrixInv(parentLocalNodeAnimMat) * parentGlobalNodeMat;

			ChaMatrix newparentGlobalNodeMat = parentGlobalNodeMat * localaddrot;//式2024/06/04_2

			////式2024/06/04_1に//式2024/06/04_2を代入してlocalを求める
			ChaMatrix newparentLocalNodeAnimMat = parentGlobalNodeMat * localaddrot * ChaMatrixInv(parentGlobalNodeMat) * parentLocalNodeAnimMat;

			CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, curframe, false);
			if (cameramp) {
				ChaMatrix localnodeanimmat = cameramp->GetLocalMat();
				ChaMatrix newcameramat = localnodeanimmat * newparentGlobalNodeMat;

				//###########
				//upvecの補正
				//###########
				ChaVector3 befupvec;
				//befupvec = ChaMatrixInv(newcameramat).GetRow(1);
				befupvec = newcameramat.GetRow(1);
				ChaVector3Normalize(&befupvec, &befupvec);
				ChaVector3 aftupvec;
				//aftupvec = newmatView.GetRow(1);
				aftupvec = ChaMatrixInv(newmatView).GetRow(1);
				ChaVector3Normalize(&aftupvec, &aftupvec);
				CQuaternion rotupq0;
				rotupq0.RotationArc(befupvec, aftupvec);
				ChaMatrix rotmatUp = rotupq0.MakeRotMatX();

				ChaMatrix localaddrot2 = befrot * rotmatUp * aftrot;

				ChaMatrix newparentGlobalNodeMat2 = newparentGlobalNodeMat * localaddrot2;
				ChaMatrix newparentLocalNodeAnimMat2 = newparentGlobalNodeMat * localaddrot2 * ChaMatrixInv(newparentGlobalNodeMat) * newparentLocalNodeAnimMat;

				ChaMatrix localnodeanimmat2 = localnodeanimmat;
				ChaMatrix newcameramat2 = localnodeanimmat2 * newparentGlobalNodeMat2;

				enullmp->SetLocalMat(newparentLocalNodeAnimMat2);
				enullmp->SetWorldMat(newparentGlobalNodeMat2);
				enullmp->SetLimitedWM(newparentGlobalNodeMat2);

				cameramp->SetWorldMat(newcameramat2);
				cameramp->SetLimitedWM(newcameramat2);

				ChaVector3 neweul;
				neweul = enullbone->CalcLocalEulXYZ(false, -1, cameramotid, curframe, BEFEUL_BEFFRAME);
				enullmp->SetLocalEul(neweul);
			}
		}
	}

	return camerabone->GetBoneNo();
}

int CModel::CameraRotateAxisDelta(
	bool limitdegflag, CEditRange* erptr,
	int axiskind,
	float delta, int ikcnt)
{
	if (!erptr) {
		_ASSERT(0);
		return 0;
	}

	ChaCalcFunc chacalcfunc;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int cameramotid = GetCameraMotionId();
	MOTINFO camerami = GetMotInfo(cameramotid);
	if (camerami.motid <= 0) {
		return 0;
	}
	int curframeleng = IntTime(camerami.frameleng);


	CBone* camerabone = nullptr;
	CBone* enullbone = nullptr;
	CAMERANODE* cnptr = GetCAMERANODE(cameramotid);
	if (cnptr && cnptr->pbone && cnptr->pbone->GetParent(false)) {
		camerabone = cnptr->pbone;
		enullbone = cnptr->pbone->GetParent(false);
	}
	else {
		_ASSERT(0);
		return 0;
	}



	//int calcnum = 4;//ctrlを押しながらドラッグでdelta * 0.25になっている.多フレーム選択時の重さを考えると処理を重くすることは出来ないのでゆっくりドラッグする他ない.
	float rotrad2 = delta / 10.0f * (float)PAI / 12.0f * g_physicsmvrate;//PhysicsIKプレートのEditRateスライダーで倍率設定.
	//if (fabs(rotrad) < (0.020 * DEG2PAI)) {
	if (fabs(rotrad2) < (0.020 * DEG2PAI)) {//2023/02/11
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (fabs(rotrad2) > (0.0550 * DEG2PAI)) {//2023/02/11
		rotrad2 = 0.0550f * fabs(rotrad2) / rotrad2;
	}

	//if (fabs(rotrad2) < (0.010 * DEG2PAI)) {
	//	//g_underIKRot = false;//2023/01/14 parent limited or not
	//	return 0;
	//}
	//if (fabs(rotrad2) > (0.30 * DEG2PAI)) {
	//	rotrad2 = 0.30f * fabs(rotrad2) / rotrad2;
	//}


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);


	//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
	//g_underIKRot = true;




	ChaVector3 axis0;
	CQuaternion localq;

	ChaMatrix selectmat;
	selectmat.SetIdentity();//!!!!!!!!!!!!!!
	
	if ((axiskind == PICK_X) || (axiskind == PICK_SPA_X)) {
		axis0 = selectmat.GetRow(0);
	}
	else if ((axiskind == PICK_Y) || (axiskind == PICK_SPA_Y)) {
		axis0 = selectmat.GetRow(1);
	}
	else if ((axiskind == PICK_Z) || (axiskind == PICK_SPA_Z)) {
		axis0 = selectmat.GetRow(2);
	}
	else {
		_ASSERT(0);
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 1;
	}
	ChaVector3Normalize(&axis0, &axis0);
	localq.SetAxisAndRot(axis0, rotrad2);


	//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
	//camerabone->SaveSRT(limitdegflag, cameramotid, startframe);

	if (keynum >= 2) {
		int keyno = 0;
		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {

			double changerate;
			changerate = (double)(*(g_motionbrush_value + (int)curframe));
			CQuaternion endq;
			CQuaternion qForRot;
			endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			localq.Slerp2(endq, 1.0 - changerate, &qForRot);

			CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, curframe, false);
			if (enullmp) {
				ChaMatrix parentLocalNodeAnimMat = enullmp->GetLocalMat();
				ChaMatrix parentGlobalNodeMat = enullmp->GetWorldMat();

				ChaMatrix befrot, aftrot;
				befrot.SetIdentity();
				aftrot.SetIdentity();
				befrot.SetTranslation(-g_camtargetpos);
				aftrot.SetTranslation(g_camtargetpos);
				ChaMatrix rotmat = qForRot.MakeRotMatX();
				ChaMatrix localaddrot = befrot * rotmat * aftrot;

				//式2024/06/04_1 : ChaMatrix newparentGlobalNodeMat = newparentLocalNodeAnimMat * ChaMatrixInv(parentLocalNodeAnimMat) * parentGlobalNodeMat;
				
				ChaMatrix newparentGlobalNodeMat = parentGlobalNodeMat * localaddrot;//式2024/06/04_2

				////式2024/06/04_1に//式2024/06/04_2を代入してlocalを求める
				ChaMatrix newparentLocalNodeAnimMat = parentGlobalNodeMat * localaddrot * ChaMatrixInv(parentGlobalNodeMat) * parentLocalNodeAnimMat;

				CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, curframe, false);
				if (cameramp) {
					ChaMatrix localnodeanimmat = cameramp->GetLocalMat();
					ChaMatrix newcameramat = localnodeanimmat * newparentGlobalNodeMat;

					enullmp->SetLocalMat(newparentLocalNodeAnimMat);
					enullmp->SetWorldMat(newparentGlobalNodeMat);
					enullmp->SetLimitedWM(newparentGlobalNodeMat);

					cameramp->SetWorldMat(newcameramat);
					cameramp->SetLimitedWM(newcameramat);

					ChaVector3 neweul;
					neweul = enullbone->CalcLocalEulXYZ(limitdegflag, -1, cameramotid, curframe, BEFEUL_BEFFRAME);
					enullmp->SetLocalEul(neweul);
				}
			}

			keyno++;
		}

	}
	else {

		double curframe = RoundingTime(applyframe);
		CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, curframe, false);
		if (enullmp) {
			ChaMatrix parentLocalNodeAnimMat = enullmp->GetLocalMat();
			ChaMatrix parentGlobalNodeMat = enullmp->GetWorldMat();


			ChaMatrix befrot, aftrot;
			befrot.SetIdentity();
			aftrot.SetIdentity();
			befrot.SetTranslation(-g_camtargetpos);
			aftrot.SetTranslation(g_camtargetpos);
			ChaMatrix rotmat = localq.MakeRotMatX();
			ChaMatrix localaddrot = befrot * rotmat * aftrot;


			//式2024/06/04_1 : ChaMatrix newparentGlobalNodeMat = newparentLocalNodeAnimMat * ChaMatrixInv(parentLocalNodeAnimMat) * parentGlobalNodeMat;

			ChaMatrix newparentGlobalNodeMat = parentGlobalNodeMat * localaddrot;//式2024/06/04_2

			////式2024/06/04_1に//式2024/06/04_2を代入してlocalを求める
			ChaMatrix newparentLocalNodeAnimMat = parentGlobalNodeMat * localaddrot * ChaMatrixInv(parentGlobalNodeMat) * parentLocalNodeAnimMat;

			CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, curframe, false);
			if (cameramp) {
				ChaMatrix localnodeanimmat = cameramp->GetLocalMat();
				ChaMatrix newcameramat = localnodeanimmat * newparentGlobalNodeMat;

				enullmp->SetLocalMat(newparentLocalNodeAnimMat);
				enullmp->SetWorldMat(newparentGlobalNodeMat);
				enullmp->SetLimitedWM(newparentGlobalNodeMat);

				cameramp->SetWorldMat(newcameramat);
				cameramp->SetLimitedWM(newcameramat);

				ChaVector3 neweul;
				neweul = enullbone->CalcLocalEulXYZ(limitdegflag, -1, cameramotid, curframe, BEFEUL_BEFFRAME);
				enullmp->SetLocalEul(neweul);

			}
		}

	}


	//if (g_applyendflag == 1) {
	//	//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
	//	if (GetTopBone(false)) {
	//		int tolast;
	//		for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
	//			//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
	//			GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
	//		}
	//	}
	//}


	////絶対モードの場合
	//if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone) {
	//	chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastbone, curmotid);
	//}

	return camerabone->GetBoneNo();

}

int CModel::CameraTranslateAxisDelta(
	CEditRange* erptr, int axiskind, float delta, ChaMatrix matView)
{
	ChaVector3 basevec;
	ChaVector3 vecx(1.0f, 0.0f, 0.0f);
	ChaVector3 vecy(0.0f, 1.0f, 0.0f);
	ChaVector3 vecz(0.0f, 0.0f, 1.0f);

	ChaMatrix invmatView;
	invmatView = ChaMatrixInv(matView);

	if (axiskind == 0) {
		basevec = invmatView.GetRow(0);
	}
	else if (axiskind == 1) {
		basevec = invmatView.GetRow(1);
	}
	else if (axiskind == 2) {
		basevec = invmatView.GetRow(2);
	}
	else {
		_ASSERT(0);
		basevec = invmatView.GetRow(0);
	}
	ChaVector3Normalize(&basevec, &basevec);

	ChaVector3 addtra;
	addtra = basevec * delta * g_physicsmvrate;//2024/01/30 DispAndLimitsPlateMenu : EditRateSlider

	return CameraTranslateAxis(erptr, addtra);

}
int CModel::CameraTranslateAxis(
	CEditRange* erptr, ChaVector3 addtra)
{
	if (!erptr) {
		_ASSERT(0);
		return 0;
	}

	ChaCalcFunc chacalcfunc;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int cameramotid = GetCameraMotionId();
	MOTINFO camerami = GetMotInfo(cameramotid);
	if (camerami.motid <= 0) {
		return 0;
	}
	int curframeleng = IntTime(camerami.frameleng);


	CBone* camerabone = nullptr;
	CBone* enullbone = nullptr;
	CAMERANODE* cnptr = GetCAMERANODE(cameramotid);
	if (cnptr && cnptr->pbone && cnptr->pbone->GetParent(false)) {
		camerabone = cnptr->pbone;
		enullbone = cnptr->pbone->GetParent(false);
	}
	else {
		_ASSERT(0);
		return 0;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	if (keynum >= 2) {
		int keyno = 0;
		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {

			double changerate;
			changerate = (double)(*(g_motionbrush_value + (int)curframe));
			ChaVector3 frameaddtra = addtra * changerate;
			ChaMatrix frameaddtramat;
			frameaddtramat.SetIdentity();
			frameaddtramat.SetTranslation(frameaddtra);

			CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, curframe, false);
			if (enullmp) {
				ChaMatrix parentLocalNodeAnimMat = enullmp->GetLocalMat();
				ChaMatrix parentGlobalNodeMat = enullmp->GetWorldMat();


				//式2024/06/04_1 : ChaMatrix newparentGlobalNodeMat = newparentLocalNodeAnimMat * ChaMatrixInv(parentLocalNodeAnimMat) * parentGlobalNodeMat;

				ChaMatrix newparentGlobalNodeMat = parentGlobalNodeMat * frameaddtramat;//式2024/06/04_2

				////式2024/06/04_1に//式2024/06/04_2を代入してlocalを求める
				ChaMatrix newparentLocalNodeAnimMat = parentGlobalNodeMat * frameaddtramat * ChaMatrixInv(parentGlobalNodeMat) * parentLocalNodeAnimMat;

				CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, curframe, false);
				if (cameramp) {
					ChaMatrix localnodeanimmat = cameramp->GetLocalMat();
					ChaMatrix newcameramat = localnodeanimmat * newparentGlobalNodeMat;

					enullmp->SetLocalMat(newparentLocalNodeAnimMat);
					enullmp->SetWorldMat(newparentGlobalNodeMat);
					enullmp->SetLimitedWM(newparentGlobalNodeMat);

					cameramp->SetWorldMat(newcameramat);
					cameramp->SetLimitedWM(newcameramat);

					//ChaVector3 neweul;
					//neweul = enullbone->CalcLocalEulXYZ(limitdegflag, -1, cameramotid, curframe, BEFEUL_BEFFRAME);
					//enullmp->SetLocalEul(neweul);
				}
			}

			keyno++;
		}

	}
	else {
		double curframe = RoundingTime(applyframe);
		CMotionPoint* enullmp = enullbone->GetMotionPoint(cameramotid, curframe, false);
		if (enullmp) {
			ChaMatrix parentLocalNodeAnimMat = enullmp->GetLocalMat();
			ChaMatrix parentGlobalNodeMat = enullmp->GetWorldMat();

			double changerate;
			changerate = (double)(*(g_motionbrush_value + (int)curframe));
			ChaVector3 frameaddtra = addtra * changerate;
			ChaMatrix frameaddtramat;
			frameaddtramat.SetIdentity();
			frameaddtramat.SetTranslation(frameaddtra);

			//式2024/06/04_1 : ChaMatrix newparentGlobalNodeMat = newparentLocalNodeAnimMat * ChaMatrixInv(parentLocalNodeAnimMat) * parentGlobalNodeMat;

			ChaMatrix newparentGlobalNodeMat = parentGlobalNodeMat * frameaddtramat;//式2024/06/04_2

			////式2024/06/04_1に//式2024/06/04_2を代入してlocalを求める
			ChaMatrix newparentLocalNodeAnimMat = parentGlobalNodeMat * frameaddtramat * ChaMatrixInv(parentGlobalNodeMat) * parentLocalNodeAnimMat;

			CMotionPoint* cameramp = camerabone->GetMotionPoint(cameramotid, curframe, false);
			if (cameramp) {
				ChaMatrix localnodeanimmat = cameramp->GetLocalMat();
				ChaMatrix newcameramat = localnodeanimmat * newparentGlobalNodeMat;

				enullmp->SetLocalMat(newparentLocalNodeAnimMat);
				enullmp->SetWorldMat(newparentGlobalNodeMat);
				enullmp->SetLimitedWM(newparentGlobalNodeMat);

				cameramp->SetWorldMat(newcameramat);
				cameramp->SetLimitedWM(newcameramat);

				//ChaVector3 neweul;
				//neweul = enullbone->CalcLocalEulXYZ(limitdegflag, -1, cameramotid, curframe, BEFEUL_BEFFRAME);
				//enullmp->SetLocalEul(neweul);
			}
		}

	}

	return camerabone->GetBoneNo();

}


int CModel::IKRotateAxisDeltaUnderIK( 
	bool limitdegflag, CEditRange* erptr, 
	int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
{
	//####################################################################################################
	//2022/11/04
	//1.0.0.32までのバージョンにおいては　回転軸の計算に苦労した
	//1.0.0.33からは　selectmatの使い方を見直し
	//XYZのどれかの軸で回転する場合においては　selectmatから回転軸がすぐに求まる
	//回転平面がブレない
	//スードローカル動作確認 : 体全体を回転しながら　足をRectブラシで開くと　体が反対を向いても足が開く
	//####################################################################################################

	ChaCalcFunc chacalcfunc;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	if (GetNoBoneFlag()) {
		return 0;
	}

	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	SetIKTargetVec();

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();


	//int calcnum = 3;
	//int calcnum = 1;//今はtargetposを細かく刻んでいないので１で良い

	int calcnum = 1;
	//int calcnum = 4;//ctrlを押しながらドラッグでdelta * 0.25になっている.多フレーム選択時の重さを考えると処理を重くすることは出来ないのでゆっくりドラッグする他ない.
	//float rotrad = delta / 10.0f * (float)PAI / 12.0f * g_physicsmvrate;//PhysicsIKプレートのEditRateスライダーで倍率設定.
	float rotrad = delta / 10.0f * (float)PAI / 20.0f * g_physicsmvrate;//2023/03/04
	//if (fabs(rotrad) < (0.020 * DEG2PAI)) {//2023/02/11
	if (fabs(rotrad) < (0.010 * DEG2PAI)) {//2023/03/04
		return 0;
	}
	if (fabs(rotrad) > (0.0550 * DEG2PAI)) {//2023/02/11
		rotrad = 0.0550f * fabs(rotrad) / rotrad;
	}


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone) {
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (curbone->IsNotSkeleton()) {
		return 0;
	}

	bool ishipsjoint = false;
	if (curbone && curbone->GetParent(false)) {
		ishipsjoint = curbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	CBone* firstbone = curbone;
	CBone* parentbone = 0;
	CBone* lastbone = 0;
	//CBone* topbone = GetTopBone();
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}

	int calccnt;
	for (calccnt = 0; calccnt < calcnum; calccnt++) {
		curbone = firstbone;
		if (!curbone) {
			//g_underIKRot = false;//2023/01/14 parent limited or not
			return 0;
		}
		lastbone = curbone;

		float currate = 1.0f;

		double firstframe = 0.0;
		int levelcnt = 0;

		while (curbone && ((maxlevel == 0) || (levelcnt < maxlevel))) {

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			//g_underIKRot = true;


			parentbone = curbone->GetParent(false);

			//parentboneの無いcurboneをドラッグした時はbreakしない
			if (!parentbone && (firstbone != curbone)) {
				break;
			}

			float rotrad2 = currate * rotrad;
			////float rotrad2 = rotrad;
			////if (fabs(rotrad2) < (0.020 * DEG2PAI)){
			if (fabs(rotrad2) < (0.020 * DEG2PAI)) {
				break;
			}
			if (fabs(rotrad2) > (0.0550 * DEG2PAI)) {//2023/02/11
				rotrad2 = 0.0550f * fabs(rotrad2) / rotrad2;
			}

			CRigidElem* curre = GetRigidElem(curbone->GetBoneNo());
			if (curre && curre->GetForbidRotFlag() != 0) {
				//回転禁止の場合処理をスキップ
				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
				lastbone = curbone;
				curbone = curbone->GetParent(false);
				levelcnt++;
				continue;
			}

			CBone* aplybone;
			if (parentbone && parentbone->IsSkeleton()) {
				aplybone = parentbone;
			}
			else {
				aplybone = curbone;
			}

			if (aplybone->IsNotSkeleton()) {
				//eNullの場合処理をスキップ
				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
				lastbone = curbone;
				curbone = curbone->GetParent(false);
				levelcnt++;
				continue;
			}


			ChaVector3 axis0;
			CQuaternion localq;
			//2022/11/03 回転軸はselectmatがあれば自明！！
			if ((axiskind == PICK_X) || (axiskind == PICK_SPA_X)) {
				axis0 = selectmat.GetRow(0);
			}
			else if ((axiskind == PICK_Y) || (axiskind == PICK_SPA_Y)) {
				axis0 = selectmat.GetRow(1);
			}
			else if ((axiskind == PICK_Z) || (axiskind == PICK_SPA_Z)) {
				axis0 = selectmat.GetRow(2);
			}
			else {
				_ASSERT(0);
				//g_underIKRot = false;//2023/01/14 parent limited or not
				return 1;
			}
			ChaVector3Normalize(&axis0, &axis0);
			localq.SetAxisAndRot(axis0, rotrad2);
			//ChaMatrix transmat;
			//transmat = localq.MakeRotMatX();


			if (fabs(rotrad2) >= 1e-4) {
				CQuaternion qForRot;
				CQuaternion qForHipsRot;


				//aplybone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
				// 
				//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
				aplybone->SaveSRT(limitdegflag, curmotid, startframe);


				//2023/01/22 : topposスライダーの位置のフレーム(３D表示中のフレーム)において　
				//制限角度により　回転出来ない場合は　リターンする
				//if (g_limitdegflag != false) {
				if ((limitdegflag != false) && (g_wallscrapingikflag == 0)) {//2023/01/23
					//2023/01/28 IK時は　GetBtForce()チェックはしない　BtForce == 1でも角度制限する
					int ismovable = IsMovableRot(limitdegflag, curmotid, applyframe, applyframe, localq, aplybone, aplybone);
					if (ismovable == 0) {
						////g_underIKRot = false;//2023/01/14 parent limited or not
						//if (editboneforret) {
						//	return editboneforret->GetBoneNo();
						//}
						//else {
						//	return srcboneno;
						//}


						//2023/02/12 returnやめ　動くボーンは動かすことに
						lastbone = curbone;
						curbone = curbone->GetParent(false);
						levelcnt++;
						continue;
					}
				}

				int ismovable2 = 1;
				if (keynum >= 2) {
					int keyno = 0;
					double curframe = applyframe;
					//for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {

					bool keynum1flag = false;
					bool postflag = false;
					bool fromiktarget = false;
					ismovable2 = chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
						keyno, 
						aplybone, aplybone,
						curmotid, curframe, startframe, applyframe,
						localq, keynum1flag, postflag, fromiktarget);

					keyno++;
					//}

				}
				else {
					//CMotionPoint transmp;
					//rotq.RotationMatrix(transmat);

					bool keynum1flag = true;
					bool postflag = false;
					bool fromiktarget = false;
					ismovable2 = chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
						0, 
						aplybone, aplybone,
						curmotid, GetCurrentFrame(), startframe, applyframe,
						localq, keynum1flag, false, fromiktarget);
				}


				//2023/03/04 制限角度に引っ掛かった場合には　やめて　次のジョイントの回転へ
				if ((ismovable2 == 0) && (g_wallscrapingikflag == 0)) {

					//2023/03/14 continueの前にすべきこと追加
					//RotAndTraBoneQReq内でGetMotionPointがNULLの場合にも　ismovable2 == 0
					lastbone = curbone;
					curbone = curbone->GetParent(false);
					levelcnt++;
					
					continue;
				}


				if (g_applyendflag == 1) {
					//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
					if (GetTopBone(false)) {
						int tolast;
						for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
							//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
							GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
						}
					}
				}

				////parentboneのrotqを保存
				IKROTREC currotrec;
				currotrec.applyframemat = aplybone->GetWorldMat(limitdegflag, curmotid, applyframe, 0);
				currotrec.applyframeeul = aplybone->GetLocalEul(limitdegflag, curmotid, applyframe, 0);
				currotrec.rotq = localq;
				currotrec.targetpos.SetParams(0.0f, 0.0f, 0.0f);
				currotrec.lessthanthflag = false;
				aplybone->AddIKRotRec(currotrec);
			}
			else {
				//rotqの回転角度が1e-4より小さい場合
				//ウェイトが小さいフレームにおいても　IKTargetが走るように記録する必要がある
				IKROTREC currotrec;
				currotrec.applyframemat = aplybone->GetWorldMat(limitdegflag, curmotid, applyframe, 0);
				currotrec.applyframeeul = aplybone->GetLocalEul(limitdegflag, curmotid, applyframe, 0);
				currotrec.rotq = localq;
				currotrec.targetpos.SetParams(0.0f, 0.0f, 0.0f);
				currotrec.lessthanthflag = true;//!!!!!!!!!!!
				aplybone->AddIKRotRec(currotrec);
			}

			if (aplybone) {
				if (aplybone->GetIKStopFlag() == true) {
					break;
				}
			}


			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
			lastbone = curbone;
			curbone = curbone->GetParent(false);
			levelcnt++;
		}

		//絶対モードの場合
		if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone) {
			chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastbone, curmotid);
		}
	}


	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret) {
		return editboneforret->GetBoneNo();
	}
	else {
		return srcboneno;
	}

}

int CModel::IKRotateAxisDeltaPostIK(
	bool limitdegflag, CEditRange* erptr,
	int axiskind, int srcboneno, int maxlevel, int ikcnt)
{
	ChaCalcFunc chacalcfunc;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	if (GetNoBoneFlag()) {
		return 0;
	}

	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();

	int calcnum = 1;
	//int calcnum = 4;//ctrlを押しながらドラッグでdelta * 0.25になっている.多フレーム選択時の重さを考えると処理を重くすることは出来ないのでゆっくりドラッグする他ない.


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone) {
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (curbone->IsNotSkeleton()) {
		return 0;
	}


	SetPostIKFrame(RoundingTime(startframe), RoundingTime(endframe));//2023/10/17


	bool ishipsjoint = false;
	if (curbone && curbone->GetParent(false)) {
		ishipsjoint = curbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	CBone* firstbone = curbone;
	CBone* parentbone = 0;
	CBone* lastbone = 0;
	CBone* lastaplybone = 0;
	//CBone* topbone = GetTopBone();
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}

	int calccnt;
	for (calccnt = 0; calccnt < calcnum; calccnt++) {
		curbone = firstbone;
		if (!curbone) {
			//g_underIKRot = false;//2023/01/14 parent limited or not
			return 0;
		}
		lastbone = curbone;

		float currate = 1.0f;

		double firstframe = 0.0;
		int levelcnt = 0;

		while (curbone && ((maxlevel == 0) || (levelcnt < maxlevel))) {

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			//g_underIKRot = true;


			parentbone = curbone->GetParent(false);

			//parentboneの無いcurboneをドラッグした時はbreakしない
			if (!parentbone && (firstbone != curbone)) {
				break;
			}

			CRigidElem* curre = GetRigidElem(curbone->GetBoneNo());
			if (curre && curre->GetForbidRotFlag() != 0) {
				//回転禁止の場合処理をスキップ
				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
				lastbone = curbone;
				curbone = curbone->GetParent(false);
				levelcnt++;
				continue;
			}

			CBone* aplybone;
			if (parentbone && parentbone->IsSkeleton()) {
				aplybone = parentbone;
			}
			else {
				aplybone = curbone;
			}

			if (aplybone->IsNotSkeleton()) {
				//eNullの場合処理をスキップ
				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
				lastbone = curbone;
				curbone = curbone->GetParent(false);
				levelcnt++;
				continue;
			}


			int rotrecsize = aplybone->GetIKRotRecSize();
			if (rotrecsize > 0) {
				int rotrecno;
				for (rotrecno = 0; rotrecno < rotrecsize; rotrecno++) {
					IKROTREC currotrec = aplybone->GetIKRotRec(rotrecno);
					CQuaternion localq = currotrec.rotq;
					bool lessthanthflag = currotrec.lessthanthflag;

					//aplybone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
					// 
					//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
					aplybone->SaveSRT(limitdegflag, curmotid, startframe);

					if (lessthanthflag == false) {
						if (keynum >= 2) {
							int keyno = 0;
							bool keynum1flag = false;
							bool postflag = true;
							bool fromiktarget = false;

							if (m_PostIKThreads) {
								//SetUnderPostIK(true);
								int updatecount;
								for (updatecount = 0; updatecount < POSTIK_THREADSNUM; updatecount++) {
									CThreadingPostIK* curupdate = m_PostIKThreads + updatecount;
									curupdate->IKRotateOneFrame(this, limitdegflag, erptr,
										keyno,
										aplybone, aplybone,
										curmotid, startframe, applyframe,
										localq, keynum1flag, postflag, fromiktarget);
								}
								WaitPostIKFinished();
								//SetUnderPostIK(false);
							}

							//int keyno = 0;
							//double curframe;
							//for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
							//	if (curframe != applyframe) {
							//		bool keynum1flag = false;
							//		bool postflag = true;
							//		bool fromiktarget = false;
							//		chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
							//			keyno, 
							//			aplybone, aplybone,
							//			m_curmotinfo->motid, curframe, startframe, applyframe,
							//			localq, keynum1flag, postflag, fromiktarget);
							//	}
							//	//else {//2023/10/16_1 Q2EulXYZusingQ()にて　IK時にbefeul.currentframeeulを使用することにしたので不要に
							//	//	//2023/10/12
							//	//	//applyframeにも　オイラー角の計算は必要
							//	//	//ChaVector3 neweul.SetParams(0.0f, 0.0f, 0.0f);
							//	//	//neweul = aplybone->CalcLocalEulXYZ(limitdegflag, -1, m_curmotinfo->motid, curframe, BEFEUL_BEFFRAME);
							//	//	//aplybone->SetLocalEul(limitdegflag, m_curmotinfo->motid, curframe, neweul, 0);

							//	//	//2023/10/16
							//	//	// applyframeで急激に値が変わるとカーブが想定と違うことがあるので　計算済を徐々にトレースしながら
							//	//	aplybone->SetWorldMat(limitdegflag, m_curmotinfo->motid, curframe, currotrec.applyframemat, 0);
							//	//	ChaVector3 neweul.SetParams(0.0f, 0.0f, 0.0f);
							//	//	neweul = aplybone->CalcLocalEulXYZ(limitdegflag, -1, m_curmotinfo->motid, curframe, BEFEUL_BEFFRAME);
							//	//	aplybone->SetLocalEul(limitdegflag, m_curmotinfo->motid, curframe, neweul, 0);

							//	//}
							//	keyno++;
							//}
						}
					}
					//else {
					//	//CMotionPoint transmp;
					//	//rotq.RotationMatrix(transmat);

					//	bool keynum1flag = true;
					//	chacalcfunc.IKRotateOneFrame(limitdegflag, erptr,
					//		0, aplybone,
					//		m_curmotinfo->curframe, startframe, applyframe,
					//		localq, keynum1flag);
					//}

				}
			}
			
			lastaplybone = aplybone;

			if (aplybone) {
				//check ikstopflag
				if (aplybone->GetIKStopFlag()) {
					break;
				}
			}

			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
			lastbone = curbone;
			curbone = curbone->GetParent(false);
			levelcnt++;
		}

		if (g_applyendflag == 1) {
			//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
			if (GetTopBone(false)) {
				int tolast;
				for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++) {
					//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
					GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
				}
			}
		}


		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
			if (curframe != applyframe) {
				bool postflag = true;
				chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, curframe, postflag);
			}
		}


	}

	//絶対モードの場合
	if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone) {
		chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastbone, curmotid);
	}

	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret) {
		return editboneforret->GetBoneNo();
	}
	else {
		return srcboneno;
	}

}


int CModel::IKRotateAxisDelta(bool limitdegflag, CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat)
{
	//####################################################################################################
	//2022/11/04
	//1.0.0.32までのバージョンにおいては　回転軸の計算に苦労した
	//1.0.0.33からは　selectmatの使い方を見直し
	//XYZのどれかの軸で回転する場合においては　selectmatから回転軸がすぐに求まる
	//回転平面がブレない
	//スードローカル動作確認 : 体全体を回転しながら　足をRectブラシで開くと　体が反対を向いても足が開く
	//####################################################################################################

	ChaCalcFunc chacalcfunc;

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();


	//int calcnum = 3;
	//int calcnum = 1;//今はtargetposを細かく刻んでいないので１で良い

	int calcnum = 1;
	//int calcnum = 4;//ctrlを押しながらドラッグでdelta * 0.25になっている.多フレーム選択時の重さを考えると処理を重くすることは出来ないのでゆっくりドラッグする他ない.
	float rotrad = delta / 10.0f * (float)PAI / 12.0f * g_physicsmvrate;//PhysicsIKプレートのEditRateスライダーで倍率設定.
	//if (fabs(rotrad) < (0.020 * DEG2PAI)) {
	if (fabs(rotrad) < (0.020 * DEG2PAI)) {//2023/02/11
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (fabs(rotrad) > (0.0550 * DEG2PAI)) {//2023/02/11
		rotrad = 0.0550f * fabs(rotrad) / rotrad;
	}


	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	CBone* curbone = m_bonelist[srcboneno];
	if (!curbone){
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return 0;
	}
	if (curbone->IsNotSkeleton()) {
		return 0;
	}

	bool ishipsjoint = false;
	if (curbone && curbone->GetParent(false)) {
		ishipsjoint = curbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	CBone* firstbone = curbone;
	CBone* parentbone = 0;
	CBone* lastbone = 0;
	//CBone* topbone = GetTopBone();
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}

	int calccnt;
	for (calccnt = 0; calccnt < calcnum; calccnt++){
		curbone = firstbone;
		if (!curbone){
			//g_underIKRot = false;//2023/01/14 parent limited or not
			return 0;
		}
		lastbone = curbone;

		float currate = 1.0f;

		double firstframe = 0.0;
		int levelcnt = 0;

		while (curbone && ((maxlevel == 0) || (levelcnt < maxlevel))){

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			//g_underIKRot = true;


			parentbone = curbone->GetParent(false);

			//parentboneの無いcurboneをドラッグした時はbreakしない
			if (!parentbone && (firstbone != curbone)) {
				break;
			}

			float rotrad2 = currate * rotrad;
			////float rotrad2 = rotrad;
			////if (fabs(rotrad2) < (0.020 * DEG2PAI)){
			if (fabs(rotrad2) < (0.020 * DEG2PAI)) {
				break;
			}
			if (fabs(rotrad2) > (0.0550 * DEG2PAI)) {//2023/02/11
				rotrad2 = 0.0550f * fabs(rotrad2) / rotrad2;
			}

			CRigidElem* curre = GetRigidElem(curbone->GetBoneNo());
			if (curre && curre->GetForbidRotFlag() != 0) {
				//回転禁止の場合処理をスキップ
				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
				lastbone = curbone;
				curbone = curbone->GetParent(false);
				levelcnt++;
				continue;
			}

			CBone* aplybone;
			if (parentbone) {
				aplybone = parentbone;
			}
			else {
				aplybone = curbone;
			}

			if (aplybone->IsNotSkeleton()) {
				//eNullの場合処理をスキップ
				currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
				lastbone = curbone;
				curbone = curbone->GetParent(false);
				levelcnt++;
				continue;
			}

			ChaVector3 axis0;
			CQuaternion localq;
			//2022/11/03 回転軸はselectmatがあれば自明！！
			if ((axiskind == PICK_X) || (axiskind == PICK_SPA_X)) {
				axis0 = selectmat.GetRow(0);
			}
			else if ((axiskind == PICK_Y) || (axiskind == PICK_SPA_Y)) {
				axis0 = selectmat.GetRow(1);
			}
			else if ((axiskind == PICK_Z) || (axiskind == PICK_SPA_Z)) {
				axis0 = selectmat.GetRow(2);
			}
			else {
				_ASSERT(0);
				//g_underIKRot = false;//2023/01/14 parent limited or not
				return 1;
			}
			ChaVector3Normalize(&axis0, &axis0);
			localq.SetAxisAndRot(axis0, rotrad2);
			//ChaMatrix transmat;
			//transmat = localq.MakeRotMatX();

			CQuaternion qForRot;
			CQuaternion qForHipsRot;


			//aplybone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
			// 
			//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
			aplybone->SaveSRT(limitdegflag, curmotid, startframe);


			//2023/01/22 : topposスライダーの位置のフレーム(３D表示中のフレーム)において　
			//制限角度により　回転出来ない場合は　リターンする
			//if (g_limitdegflag != false) {
			if ((limitdegflag != false) && (g_wallscrapingikflag == 0)) {//2023/01/23
				//2023/01/28 IK時は　GetBtForce()チェックはしない　BtForce == 1でも角度制限する
				int ismovable = IsMovableRot(limitdegflag, curmotid, applyframe, applyframe, localq, aplybone, aplybone);
				if (ismovable == 0) {
					////g_underIKRot = false;//2023/01/14 parent limited or not
					//if (editboneforret) {
					//	return editboneforret->GetBoneNo();
					//}
					//else {
					//	return srcboneno;
					//}


					//2023/02/12 returnやめ　動くボーンは動かすことに
					lastbone = curbone;
					curbone = curbone->GetParent(false);
					levelcnt++;
					continue;
				}
			}

			if (keynum >= 2){
				int keyno = 0;
				double curframe;
				for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0){

					bool keynum1flag = false;
					bool postflag = false;
					bool fromiktarget = false;
					chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
						keyno, 
						aplybone, aplybone,
						curmotid, curframe, startframe, applyframe,
						localq, keynum1flag, postflag, fromiktarget);

					keyno++;
				}

			}
			else{
				//CMotionPoint transmp;
				//rotq.RotationMatrix(transmat);

				bool keynum1flag = true;
				bool postflag = false;
				bool fromiktarget = false;
				chacalcfunc.IKRotateOneFrame(this, limitdegflag, erptr,
					0, 
					aplybone, aplybone,
					curmotid, GetCurrentFrame(), startframe, applyframe,
					localq, keynum1flag, postflag, fromiktarget);
			}


			if (g_applyendflag == 1){
				//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
				if (GetTopBone(false)){
					int tolast;
					for (tolast = (int)GetCurrentFrame() + 1; tolast < curframeleng; tolast++){
						//(m_bonelist[0])->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
						GetTopBone(false)->PasteRotReq(limitdegflag, curmotid, GetCurrentFrame(), tolast);
					}
				}
			}

			currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
			lastbone = curbone;
			curbone = curbone->GetParent(false);
			levelcnt++;
		}

		//絶対モードの場合
		if ((calccnt == (calcnum - 1)) && g_absikflag && lastbone){
			chacalcfunc.AdjustBoneTra(this, limitdegflag, erptr, lastbone, curmotid);
		}
	}


	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret){
		return editboneforret->GetBoneNo();
	}
	else{
		return srcboneno;
	}

}

//int CModel::RotateXDelta( CEditRange* erptr, int srcboneno, float delta )
//{
//	if (!m_curmotinfo){
//		return 0;
//	}
//
//	CBone* firstbone = m_bonelist[ srcboneno ];
//	if( !firstbone ){
//		_ASSERT( 0 );
//		return 1;
//	}
//
//	CBone* curbone = firstbone;
//	SetBefEditMatFK( erptr, curbone );
//
//	CBone* lastpar = firstbone->GetParent();
//
//
//	float rotrad;
//	ChaVector3 axis0, rotaxis;
//	ChaMatrix selectmat;
//	ChaMatrix invselectmat;
//	selectmat.SetIdentity();
//	invselectmat.SetIdentity();
//	CBone* parentbone = curbone->GetParent();
//	//int multworld = 1;
//	//selectmat = curbone->CalcManipulatorMatrix(0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);
//	if (curbone && curbone->GetParent()) {
//		curbone->GetParent()->CalcAxisMatX_Manipulator(0, curbone, &selectmat, 0);
//	}
//	else {
//		selectmat.SetIdentity();
//	}
//	ChaMatrixInverse(&invselectmat, NULL, &selectmat);
//	selectmat.data[MATI_41] = 0.0f;
//	selectmat.data[MATI_42] = 0.0f;
//	selectmat.data[MATI_43] = 0.0f;
//
//	axis0.SetParams( 1.0f, 0.0f, 0.0f );
//	ChaVector3TransformCoord( &rotaxis, &axis0, &selectmat );
//	ChaVector3Normalize( &rotaxis, &rotaxis );
//	rotrad = delta / 10.0f * (float)PAI / 12.0f;
//
//	if( fabs(rotrad) < (0.020 * DEG2PAI) ){
//		return 0;
//	}
//
//	CQuaternion rotq;
//	rotq.SetAxisAndRot( rotaxis, rotrad );
//
//	int keynum;
//	double startframe, endframe, applyframe;
//	erptr->GetRange( &keynum, &startframe, &endframe, &applyframe );
//	
//	curbone = firstbone;
//	if (!curbone){
//		return 0;
//	}
//
//	double firstframe = 0.0;
//	if (keynum >= 2){
//		int keyno = 0;
//		double curframe;
//		for (curframe = startframe; curframe <= endframe; curframe += 1.0){
//			double changerate;
//			//if (curframe <= applyframe){
//			//	changerate = 1.0 / (applyframe - startframe + 1);
//			//}
//			//else{
//			//	changerate = 1.0 / (endframe - applyframe + 1);
//			//}
//			changerate = (double)(*(g_motionbrush_value + (int)curframe));
//
//			if (keyno == 0){
//				firstframe = curframe;
//			}
//
//			bool infooutflag;
//			if (curframe == applyframe) {
//				infooutflag = true;
//			}
//			else {
//				infooutflag = false;
//			}
//
//			if (g_absikflag == 0){
//				if (g_slerpoffflag == 0){
//					//double currate2;
//					CQuaternion endq;
//					CQuaternion curq;
//					endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//					//if (curframe <= applyframe){
//					//	currate2 = changerate * (curframe - startframe + 1);
//					//}
//					//else{
//					//	currate2 = changerate * (endframe - curframe + 1);
//					//}
//					//rotq.Slerp2(endq, 1.0 - currate2, &curq);
//					rotq.Slerp2(endq, 1.0 - changerate, &curq);
//
//					curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, curq);
//				}
//				else{
//					curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, rotq);
//				}
//			}
//			else{
//				if (keyno == 0){
//					curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, curframe, rotq);
//				}
//				else{
//					curbone->SetAbsMatReq(0, m_curmotinfo->motid, curframe, firstframe);
//				}
//			}
//			keyno++;
//		}
//
//		if (g_applyendflag == 1){
//			//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
//			if (m_topbone){
//				int tolast;
//				for (tolast = (int)m_curmotinfo->curframe + 1; tolast < (int)m_curmotinfo->frameleng; tolast++){
//					m_topbone->PasteRotReq(m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
//				}
//			}
//		}
//
//
//	}
//	else{
//		bool infooutflag = true;
//		curbone->RotBoneQReq(infooutflag, 0, m_curmotinfo->motid, m_curmotinfo->curframe, rotq);
//	}
//
//	if( g_absikflag && curbone ){
//		AdjustBoneTra( erptr, curbone );
//	}
//
//	return curbone->GetBoneNo();
//}


//int CModel::FKRotate( double srcframe, int srcboneno, ChaMatrix srcmat )
int CModel::FKRotate(bool limitdegflag, bool onretarget, int reqflag, 
	CBone* bvhbone, int traflag, ChaVector3 traanim, double srcframe, int srcboneno, 
	CQuaternion rotq)//, int setmatflag, ChaMatrix* psetmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.FKRotate(this, limitdegflag, onretarget, reqflag,
		bvhbone, traflag, traanim, srcframe, srcboneno,
		rotq);
}

int CModel::FKBoneTraAxisUnderFK(
	bool limitdegflag, CEditRange* erptr, int srcboneno, int axiskind, float delta, ChaMatrix selectmat)
{
	if ((srcboneno < 0) && !GetTopBone()) {
		return 0;
	}

	CBone* curbone = GetBoneByID(srcboneno);
	if (!curbone) {
		_ASSERT(0);
		return 0;
	}

	SetIKTargetVec();

	ChaVector3 basevec;
	ChaVector3 vecx(1.0f, 0.0f, 0.0f);
	ChaVector3 vecy(0.0f, 1.0f, 0.0f);
	ChaVector3 vecz(0.0f, 0.0f, 1.0f);

	if (axiskind == 0) {
		basevec = selectmat.GetRow(0);
	}
	else if (axiskind == 1) {
		basevec = selectmat.GetRow(1);
	}
	else if (axiskind == 2) {
		basevec = selectmat.GetRow(2);
	}
	else {
		_ASSERT(0);
		basevec = selectmat.GetRow(0);
	}
	ChaVector3Normalize(&basevec, &basevec);

	ChaVector3 addtra;
	addtra = basevec * delta * g_physicsmvrate;//2024/01/30 DispAndLimitsPlateMenu : EditRateSlider

	FKBoneTraUnderFK(limitdegflag, erptr, srcboneno, addtra);

	return 0;
}

int CModel::FKBoneTraAxisPostFK(
	bool limitdegflag, CEditRange* erptr, int srcboneno)
{
	if ((srcboneno < 0) && !GetTopBone()) {
		return 0;
	}

	CBone* curbone = GetBoneByID(srcboneno);
	if (!curbone) {
		_ASSERT(0);
		return 0;
	}

	FKBoneTraPostFK(limitdegflag, erptr, srcboneno);

	return 0;
}


int CModel::FKBoneTraAxis(bool limitdegflag, int onlyoneflag, CEditRange* erptr, int srcboneno, int axiskind, float delta, ChaMatrix selectmat)
{
	ChaCalcFunc chacalcfunc;

	if ((srcboneno < 0) && !GetTopBone()){
		return 0;
	}

	CBone* curbone = GetBoneByID(srcboneno);
	if (!curbone){
		_ASSERT(0);
		return 0;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	ChaVector3 basevec;
	ChaVector3 vecx(1.0f, 0.0f, 0.0f);
	ChaVector3 vecy(0.0f, 1.0f, 0.0f);
	ChaVector3 vecz(0.0f, 0.0f, 1.0f);

	if (axiskind == 0){
		basevec = selectmat.GetRow(0);
	}
	else if (axiskind == 1){
		basevec = selectmat.GetRow(1);
	}
	else if (axiskind == 2){
		basevec = selectmat.GetRow(2);
	}
	else{
		_ASSERT(0);
		basevec = selectmat.GetRow(0);
	}
	ChaVector3Normalize(&basevec, &basevec);

	ChaVector3 addtra;
	addtra = basevec * delta;

	chacalcfunc.FKBoneTra(this, limitdegflag, 0, erptr, srcboneno, curmotid, addtra);

	return 0;
}

int CModel::FKBoneTraUnderFK(
	bool limitdegflag, CEditRange* erptr,
	int srcboneno, ChaVector3 addtra)
{
	ChaCalcFunc chacalcfunc;


	if (srcboneno < 0) {
		_ASSERT(0);
		return 1;
	}

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone) {
		_ASSERT(0);
		return 1;
	}
	if (firstbone->IsNotSkeleton()) {
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	SetIKTargetVec();

	CBone* curbone = firstbone;
	SetBefEditMatFK(limitdegflag, erptr, curbone);

	CBone* lastpar = firstbone->GetParent(false);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	curbone = firstbone;
	double firstframe = 0.0;

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();//Req関数の最初の呼び出し時は　Identityを渡せばよい

	ChaVector3 translation;
	translation.SetParams(0.0f, 0.0f, 0.0f);

	if (keynum >= 2) {
		//float changerate = 1.0f / (float)(endframe - startframe + 1);

		int keyno = 0;
		double curframe = applyframe;
		//for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
			double changerate;
			changerate = (double)(*(g_motionbrush_value + (int)curframe));


			if (keyno == 0) {
				firstframe = curframe;
			}
			if (g_absikflag == 0) {
				if (g_slerpoffflag == 0) {
					translation = addtra * (float)changerate;
					curbone->AddBoneTraReq(limitdegflag, 0, curmotid, curframe, translation, dummyparentwm, dummyparentwm);
				}
				else {
					translation = addtra;
					curbone->AddBoneTraReq(limitdegflag, 0, curmotid, curframe, translation, dummyparentwm, dummyparentwm);
				}
			}
			else {
				if (keyno == 0) {
					translation = addtra;
					curbone->AddBoneTraReq(limitdegflag, 0, curmotid, curframe, translation, dummyparentwm, dummyparentwm);
				}
				//else {
				//	curbone->SetAbsMatReq(limitdegflag, 0, m_curmotinfo->motid, curframe, firstframe);
				//}
			}

			bool postflag = false;
			chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, curframe, postflag);


			keyno++;

		//}
	}
	else {
		translation = addtra;
		curbone->AddBoneTraReq(limitdegflag, 0, curmotid, startframe, translation, dummyparentwm, dummyparentwm);

		bool postflag = false;
		chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, startframe, postflag);

	}

	IKROTREC currotrec;
	currotrec.rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	currotrec.targetpos = translation;
	currotrec.lessthanthflag = false;
	curbone->AddIKRotRec(currotrec);

	return curbone->GetBoneNo();
}

int CModel::FKBoneTraPostFK(
	bool limitdegflag, CEditRange* erptr,
	int srcboneno)
{

	ChaCalcFunc chacalcfunc;

	if (srcboneno < 0) {
		_ASSERT(0);
		return 1;
	}

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone) {
		_ASSERT(0);
		return 1;
	}
	if (firstbone->IsNotSkeleton()) {
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	CBone* curbone = firstbone;
	SetBefEditMatFK(limitdegflag, erptr, curbone);

	CBone* lastpar = firstbone->GetParent(false);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);


	SetFKTraFrame(RoundingTime(startframe), RoundingTime(endframe));//2023/10/20


	curbone = firstbone;
	double firstframe = 0.0;

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();//Req関数の最初の呼び出し時は　Identityを渡せばよい

	int rotrecsize = curbone->GetIKRotRecSize();
	if (rotrecsize > 0) {
		int rotrecno;
		for (rotrecno = 0; rotrecno < rotrecsize; rotrecno++) {
			IKROTREC currotrec = curbone->GetIKRotRec(rotrecno);
			ChaVector3 translation = currotrec.targetpos;
			bool lessthanthflag = currotrec.lessthanthflag;

			if (keynum >= 2) {
				if (m_FKTraThreads) {
					int updatecount;
					for (updatecount = 0; updatecount < POSTFKTRA_THREADSNUM; updatecount++) {
						CThreadingFKTra* curupdate = m_FKTraThreads + updatecount;
						curupdate->FKBoneTraOneFrame(this, limitdegflag, erptr,
							curbone->GetBoneNo(), curmotid, translation);
					}
					WaitFKTraFinished();
				}
			}
			//else {
			//	translation = addtra;
			//	curbone->AddBoneTraReq(limitdegflag, 0, m_curmotinfo->motid, startframe, translation, dummyparentwm, dummyparentwm);
			//}
		}
	}


	double curframe;
	for (curframe = RoundingTime(startframe); curframe <= RoundingTime(endframe); curframe += 1.0) {
		if (curframe != RoundingTime(applyframe)) {
			bool postflag = true;
			chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, curframe, postflag);
		}
	}


	return curbone->GetBoneNo();
}


bool CModel::CheckIKTarget()
{
	bool hastarget = false;
	CheckIKTargetReq(GetTopBone(false), &hastarget);
	return hastarget;
}
void CModel::CheckIKTargetReq(CBone* srcbone, bool* pfound)
{
	if (srcbone) {

		if ((srcbone->IsSkeleton()) && srcbone->GetIKTargetFlag()) {
			if (pfound) {
				*pfound = true;
			}
			return;
		}

		if (srcbone->GetChild(false)) {
			CheckIKTargetReq(srcbone->GetChild(false), pfound);
		}
		if (srcbone->GetBrother(false)) {
			CheckIKTargetReq(srcbone->GetBrother(false), pfound);
		}
	}
}


int CModel::SetIKTargetVec()
{
	m_iktargetbonevec.clear();

	SetIKTargetVecReq(GetTopBone(false));

	return 0;
}
void CModel::SetIKTargetVecReq(CBone* srcbone)
{
	if (srcbone) {

		if ((srcbone->IsSkeleton()) && srcbone->GetIKTargetFlag()) {
			m_iktargetbonevec.push_back(srcbone);
		}


		if (srcbone->GetChild(false)) {
			SetIKTargetVecReq(srcbone->GetChild(false));
		}
		if (srcbone->GetBrother(false)) {
			SetIKTargetVecReq(srcbone->GetBrother(false));
		}
	}

}

int CModel::RefreshPosConstraint()
{
	SetIKTargetVec();

	std::vector<CBone*>::iterator itrtargetbone;
	for (itrtargetbone = m_iktargetbonevec.begin(); itrtargetbone != m_iktargetbonevec.end(); itrtargetbone++) {
		CBone* srcbone = *itrtargetbone;
		if (srcbone && srcbone->GetParent(false) && srcbone->GetIKTargetFlag()) {
			srcbone->SetIKTargetFlag(true);//注意：CBone::SetIKTargetFlag(true)は　フラグだけではなく　コンストレイントの位置も更新する
		}
		else {
			_ASSERT(0);
			continue;
		}
	}

	return 0;
}

int CModel::PosConstraintExecuteFromButton(bool limitdegflag, CEditRange* erptr)
{
	ChaCalcFunc chacalcfunc;

	if (!erptr) {
		_ASSERT(0);
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	if (m_curmotinfo) {
		SetIKTargetVec();
		int keynum;
		double startframe, endframe, applyframe;
		erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= RoundingTime(endframe); curframe += 1.0) {
			bool postflag = true;
			int calccount;
			for (calccount = 0; calccount < 1; calccount++) {//IKTargetVecでも複数回計算している
				chacalcfunc.IKTargetVec(this, limitdegflag, erptr, curmotid, curframe, postflag);
			}
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}


int CModel::FKBoneScaleAxis(bool limitdegflag, int onlyoneflag, CEditRange* erptr, int srcboneno, int axiskind, float scaleval)
{
	if ((srcboneno < 0) && !GetTopBone()) {
		return 0;
	}

	CBone* curbone = GetBoneByID(srcboneno);
	if (!curbone) {
		_ASSERT(0);
		return 0;
	}

	//ChaVector3 basevec;
	//ChaVector3 vecx(1.0f, 0.0f, 0.0f);
	//ChaVector3 vecy(0.0f, 1.0f, 0.0f);
	//ChaVector3 vecz(0.0f, 0.0f, 1.0f);

	//int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
	//ChaMatrix selectmat = curbone->CalcManipulatorMatrix(0, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);

	//if (axiskind == 0) {
	//	ChaVector3TransformCoord(&basevec, &vecx, &selectmat);
	//}
	//else if (axiskind == 1) {
	//	ChaVector3TransformCoord(&basevec, &vecy, &selectmat);
	//}
	//else if (axiskind == 2) {
	//	ChaVector3TransformCoord(&basevec, &vecz, &selectmat);
	//}
	//else {
	//	_ASSERT(0);
	//	ChaVector3TransformCoord(&basevec, &vecx, &selectmat);
	//}

	//ChaVector3Normalize(&basevec, &basevec);

	//ChaVector3 addtra;
	//addtra = basevec * delta;


	//scaleはworldmatの先頭に掛ける。マニピュレータ行列変換ではない
	ChaVector3 scalevec;
	if (axiskind == -1) {
		//2024/01/30 Shiftキー＋X,Y,ZどれかのスプライトドラッグでPICK_CENTERドラッグ処理
		scalevec.x = scaleval;
		scalevec.y = scaleval;
		scalevec.z = scaleval;
	}
	else if (axiskind == 0) {
		scalevec.x = scaleval;
		scalevec.y = 1.0f;
		scalevec.z = 1.0f;
	}
	else if (axiskind == 1) {
		scalevec.x = 1.0f;
		scalevec.y = scaleval;
		scalevec.z = 1.0f;
	}
	else if (axiskind == 2) {
		scalevec.x = 1.0f;
		scalevec.y = 1.0f;
		scalevec.z = scaleval;
	}
	else {
		_ASSERT(0);
		scalevec.x = scaleval;
		scalevec.y = 1.0f;
		scalevec.z = 1.0f;
	}

	//ChaVector3 basevec.SetParams(0.0f, 0.0f, 0.0f);
	//ChaVector3 vecx.SetParams(1.0f, 0.0f, 0.0f);
	//ChaVector3 vecy.SetParams(0.0f, 1.0f, 0.0f);
	//ChaVector3 vecz.SetParams(0.0f, 0.0f, 1.0f);
	//ChaVector3 vec1.SetParams(1.0f, 1.0f, 1.0f);
	//int multworld = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!
	//ChaMatrix selectmat = curbone->CalcManipulatorMatrix(0, 0, multworld, m_curmotinfo->motid, m_curmotinfo->curframe);
	//if (axiskind == 0) {
	//	ChaVector3TransformCoord(&basevec, &vecx, &selectmat);
	//}
	//else if (axiskind == 1) {
	//	ChaVector3TransformCoord(&basevec, &vecy, &selectmat);
	//}
	//else if (axiskind == 2) {
	//	ChaVector3TransformCoord(&basevec, &vecz, &selectmat);
	//}
	//else {
	//	_ASSERT(0);
	//	ChaVector3TransformCoord(&basevec, &vecx, &selectmat);
	//}
	//ChaVector3Normalize(&basevec, &basevec);
	//
	//ChaVector3 scalevec;
	//if (scaleval >= 1.0) {
	//	scalevec = vec1 + basevec * 0.1f;
	//}
	//else {
	//	scalevec = vec1 - basevec * 0.1f;
	//}
	
	//if (fabs(scalevec.x) < 0.0001f) {
	//	scalevec.x = 0.0001f;
	//}
	//if (fabs(scalevec.y) < 0.0001f) {
	//	scalevec.y = 0.0001f;
	//}
	//if (fabs(scalevec.z) < 0.0001f) {
	//	scalevec.z = 0.0001f;
	//}

	FKBoneScale(limitdegflag, 0, erptr, srcboneno, scalevec);

	return 0;
}

int CModel::FKBoneScale(bool limitdegflag, int onlyoneflag, CEditRange* erptr, int srcboneno, ChaVector3 scalevec)
{

	if (srcboneno < 0) {
		_ASSERT(0);
		return 1;
	}

	CBone* firstbone = m_bonelist[srcboneno];
	if (!firstbone) {
		_ASSERT(0);
		return 1;
	}
	if (firstbone->IsNotSkeleton()) {
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());


	//モーションブラシの0から1のウェイトを掛ける準備
	ChaVector3 scalediffvec;
	scalediffvec.x = scalevec.x - 1.0f;
	scalediffvec.y = scalevec.y - 1.0f;
	scalediffvec.z = scalevec.z - 1.0f;



	CBone* curbone = firstbone;
	SetBefEditMatFK(limitdegflag, erptr, curbone);

	CBone* lastpar = firstbone->GetParent(false);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);

	curbone = firstbone;
	double firstframe = 0.0;

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();//Req関数の最初の呼び出し時は　Identityを渡せばよい


	if (keynum >= 2) {
		//float changerate = 1.0f / (float)(endframe - startframe + 1);

		int keyno = 0;
		double curframe;
		for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
			double changerate;
			//if( curframe <= applyframe ){
			//	changerate = 1.0 / (applyframe - startframe + 1);
			//}else{
			//	changerate = 1.0 / (endframe - applyframe + 1);
			//}
			changerate = (double)(*(g_motionbrush_value + IntTime(curframe)));


			if (keyno == 0) {
				firstframe = curframe;
			}
			if (g_absikflag == 0) {
				if (g_slerpoffflag == 0) {
					//double currate2;
					//if( curframe <= applyframe ){
					//	currate2 = changerate * (curframe - startframe + 1);
					//}else{
					//	currate2 = changerate * (endframe - curframe + 1);
					//}
					//ChaVector3 curtra;
					//curtra = addtra * (float)currate2;
					ChaVector3 iniscale;
					iniscale.SetParams(1.0f, 1.0f, 1.0f);
					ChaVector3 curscale;
					curscale = iniscale + (scalediffvec * (float)changerate);

					//currate2 = changerate * keyno;
					//ChaVector3 curtra;
					//curtra = (1.0 - currate2) * addtra;

					curbone->AddBoneScaleReq(limitdegflag, 0, 
						curmotid, RoundingTime(curframe), 
						curscale, dummyparentwm, dummyparentwm);
				}
				else {
					curbone->AddBoneScaleReq(limitdegflag, 0, 
						curmotid, RoundingTime(curframe),
						scalevec, dummyparentwm, dummyparentwm);
				}
			}
			else {
				if (keyno == 0) {
					curbone->AddBoneScaleReq(limitdegflag, 0, 
						curmotid, RoundingTime(curframe),
						scalevec, dummyparentwm, dummyparentwm);
				}
				else {
					curbone->SetAbsMatReq(limitdegflag, 0, 
						curmotid, RoundingTime(curframe), firstframe);
				}
			}
			keyno++;

		}
	}
	else {
		curbone->AddBoneScaleReq(limitdegflag, 0, 
			curmotid, RoundingTime(startframe), 
			scalevec, dummyparentwm, dummyparentwm);
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
	//ChaVector3 impulse.SetParams(100.0f, 0.0f ,0.0f);

	CBone* parentbone = curbone->GetParent();
	if (parentbone){
		CBtObject* findbto = parentbone->GetBtObject(curbone);
		if (findbto && findbto->GetRigidBody()){
			findbto->GetRigidBody()->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(0.0f, 0.0f, 0.0f));
		}
	}

	return curbone->GetBoneNo();
}
*/

int CModel::InitUndoMotion( int saveflag )
{
	m_undo_firstflag = true;

	int undono;
	for( undono = 0; undono < UNDOMAX; undono++ ){
		m_undomotion[ undono ].ClearData();
	}

	m_undo_readpoint = 0;
	m_undo_writepoint = 0;

	int savewritepoint = m_undo_writepoint;
	m_undo_writepoint = GetNewUndoID();

	//if( saveflag ){
	//	BRUSHSTATE brushstate;
	//	brushstate.Init();
	//	brushstate.brushmirrorUflag = g_brushmirrorUflag;
	//	brushstate.brushmirrorVflag = g_brushmirrorVflag;
	//	brushstate.ifmirrorVDiv2flag = g_ifmirrorVDiv2flag;
	//	brushstate.limitdegflag = g_limitdegflag;
	//	brushstate.motionbrush_method = g_motionbrush_method;
	//	brushstate.wallscrapingikflag = g_wallscrapingikflag;
	//	brushstate.brushrepeats = g_brushrepeats;
	//	int result = m_undomotion[m_undo_writepoint].SaveUndoMotion( this, -1, -1, 0, 50.0, brushstate );
	//	if (result == 1) {
	//		m_undo_writepoint = savewritepoint;
	//	}
	//	else {
	//		m_undo_firstflag = false;
	//	}
	//}

	return 0;
}

int CModel::SaveUndoMotion(bool LimitDegCheckBoxFlag, bool limitdegflag, int curboneno, int curbaseno, 
	int srcedittarget, CEditRange* srcer,
	double srcapplyrate, 
	BRUSHSTATE srcbrushstate, UNDOCAMERA srcundocamera, 
	bool allframeflag)
{
	//saveによって次回のundo位置は変わる
	//undoによって次回のsave位置は変わらない


	if( m_bonelist.empty() || !m_curmotinfo ){
		return 0;
	}

	int saveundoid = m_undo_writepoint;
	m_undo_writepoint = GetNewUndoID();


	//2023/10/27 1.2.0.27 RC5 : LimitDegCheckBoxFlag == true時　つまり　LimitEulボタンのオンオフ時はモーションの保存をスキップ
	int result = m_undomotion[m_undo_writepoint].SaveUndoMotion(LimitDegCheckBoxFlag, limitdegflag, this,
		GetSelectedBoneNo(), curbaseno, srcedittarget, srcer, srcapplyrate, srcbrushstate, srcundocamera, allframeflag);
	if (result == 1) {//result == 2はエラーにしない
		_ASSERT(0);
		m_undo_writepoint = saveundoid;
		return 1;
	}
	else {
		//成功した場合
		m_undo_readpoint = m_undo_writepoint;
		m_undo_firstflag = false;
	}



	return 0;
}
int CModel::RollBackUndoMotion(bool limitdegflag, HWND hmainwnd, int redoflag, 
	int* edittarget, int* pselectedboneno, int* curbaseno,
	//double* dststartframe, double* dstendframe, double* dstapplyrate, 
	BRUSHSTATE* dstbrushstate, UNDOCAMERA* dstundocamera, UNDOMOTID* dstundomotid)
{
	if (!edittarget || !pselectedboneno || !curbaseno || 
		!dstbrushstate || !dstundocamera || !dstundomotid) {
		_ASSERT(0);
		return 1;
	}

	//saveによって次回のundo位置は変わる
	//undoによって次回のsave位置は変わらない

	if( m_bonelist.empty() || !m_curmotinfo ){
		return 0;
	}

	if (m_undo_firstflag) {
		return 0;
	}


	//2022/11/08
	//アンドゥリドゥの保存バッファは　リングバッファとして使う
	//一番古い保存と一番新しい保存の間を　古い保存へアンドゥする場合には　バッファの最初と最後の境界を越える
	//一番古い保存と一番新しい保存の間を　新しい保存へリドゥする場合にも　バッファの最初と最後の境界を越える
	//しかし　
	//古い保存から新しい保存へはアンドゥ出来ない && 新しい保存よりも古い保存へはリドゥ出来ない
	//
	//アンドゥリドゥは変更部分についてだけ行っている 上記の禁止をしないとすると
	//例えば　一番新しい保存から一番古い保存へとリドゥするとしたら　一番新しい保存から一番古い保存までの間の全てのアンドゥを実行しなければならない　(それをしないことにした)


	int savereadpoint = m_undo_readpoint;
	if( redoflag == 0 ){
		int tmpreadpoint = GetValidUndoID();
		if (tmpreadpoint == m_undo_writepoint) {//2022/11/08
			::MessageBox(hmainwnd, L"最初の保存ポイントよりも昔にはアンドゥ出来ません。", L"can't go to older than the oldest.", MB_OK);
			//returnせずにそのままのm_undo_readpointでRollBackMotionすることにより　ブラシ状態を保つ
		}
		else {
			m_undo_readpoint = tmpreadpoint;
		}
	}else{
		if (m_undo_readpoint == m_undo_writepoint) {//2022/11/08
			::MessageBox(hmainwnd, L"書き出しポイントよりも未来にはリドゥ出来ません。", L"can't go to newer than the newest.", MB_OK);
			//returnせずにそのままのm_undo_readpointでRollBackMotionすることにより　ブラシ状態を保つ
		}
		else {
			m_undo_readpoint = GetValidRedoID();
		}
	}

	if(m_undo_readpoint >= 0 ){
		int result = m_undomotion[m_undo_readpoint].RollBackMotion(limitdegflag, this,
			edittarget,
			pselectedboneno, curbaseno,
			//dststartframe, dstendframe, dstapplyrate, 
			dstbrushstate, dstundocamera, dstundomotid);
	}

	return 0;
}

int CModel::GetNewUndoID()
{
	//Save用のundoid
	int writepoint = m_undo_writepoint;

	if (m_undo_firstflag == false) {
		writepoint++;
	}
	else {
		writepoint = 0;
	}
	
	if (writepoint >= UNDOMAX) {
		writepoint = 0;
	}

	return writepoint;
}
int CModel::GetValidUndoID()
{
	//undo用のid

	int retid = -1;

	int chkcnt;
	int curid = m_undo_readpoint;
	for( chkcnt = 0; chkcnt < UNDOMAX; chkcnt++ ){
		curid = curid - 1;
		if( curid < 0 ){
			//curid = UNDOMAX - 1;
			curid = m_undo_writepoint;//配列の最後では無く記録の最後
		}

		if( m_undomotion[curid].GetValidFlag() == 1 ){
			retid = curid;
			break;
		}
	}

	return retid;
}
int CModel::GetValidRedoID()
{
	//redo用のid

	int retid = -1;

	int chkcnt;
	int curid = m_undo_readpoint;
	for( chkcnt = 0; chkcnt < UNDOMAX; chkcnt++ ){
		curid = curid + 1;
		if( curid >= UNDOMAX){
			curid = 0;
		}

		if( m_undomotion[curid].GetValidFlag() == 1 ){
			retid = curid;
			break;
		}
	}

	return retid;
}

int CModel::AddBoneMotMark( OWP_Timeline* owpTimeline, int curboneno, int curlineno, double startframe, double endframe, int flag )
{
	if( (curboneno < 0) || (curlineno < 0) ){
		_ASSERT( 0 );
		return 1;
	}

	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	CBone* curbone = m_bonelist[ curboneno ];
	if( curbone && (curbone->IsSkeleton())){
		curbone->AddBoneMotMark( curmotid, owpTimeline, curlineno, startframe, endframe, flag );
	}

	return 0;
}


int CModel::SetFirstFrameBonePos(HINFO* phinfo, CBone* srchipsbone)
{
	if (!ExistCurrentMotion()) {
		return 0;
	}
	int curmotid = GetCurrentMotID();
	int curframeleng = IntTime(GetCurrentMotLeng());

	if (curmotid <= 0){
		_ASSERT(0);
		return 0;
	}
	if (!GetTopBone()){
		_ASSERT(0);
		return 0;
	}

	if (srchipsbone) {
		SetFirstFrameBonePosReq(srchipsbone, curmotid, phinfo);
	}
	else {
		SetFirstFrameBonePosReq(GetTopBone(false), curmotid, phinfo);
	}
	

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
	if (!srcbone) {
		return;
	}

	if (srcbone && (srcbone->IsSkeleton())) {
		CMotionPoint* curmp = 0;
		double curframe = 0.0;

		bool onaddmotion = true;//for getbychain
		curmp = srcbone->GetMotionPoint(srcmotid, curframe, onaddmotion);
		if (!curmp){
			_ASSERT(0);
		}

		if (curmp){
			//ChaMatrix firstmat = curmp->GetWorldMat();
			//ChaMatrix firstmat = srcbone->GetFirstMat();//2022/07/29

			ChaMatrix firstmat = srcbone->GetNodeMat() * curmp->GetWorldMat();//2023/02/08


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

	if (srcbone->GetChild(false)){
		SetFirstFrameBonePosReq(srcbone->GetChild(false), srcmotid, phinfo);
	}
	if (srcbone->GetBrother(false)){
		SetFirstFrameBonePosReq(srcbone->GetBrother(false), srcmotid, phinfo);
	}
}

int CModel::RecalcBoneAxisX(CBone* srcbone)
{
	bool limitdegflag = false;//unlimited初期姿勢用

	//2023/02/07
	//この関数に関しては　現在は重要に考えていない
	//昔のものを　とりあえず動くようにしているだけ
	//nodematの編集については　後で考える


	if (GetOldAxisFlagAtLoading() == 1) {
		_ASSERT(0);
		return 1;
	}

	if (!srcbone) {
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone && (curbone->IsSkeleton())) {
				ChaMatrix axismat;
				//axismat = curbone->GetFirstAxisMatZ();
				if (curbone->GetParent(false)) {
					curbone->GetParent(false)->CalcAxisMatX_Manipulator(limitdegflag, BONEAXIS_CURRENT, 1, curbone, &axismat, 1);//nodemat用？　_Manipulatorを使う
				}
				else {
					axismat.SetIdentity();
				}
				axismat.SetTranslation(curbone->GetJointFPos());
				curbone->SetNodeMat(axismat);
			}
		}
	}
	else if (srcbone->IsSkeleton()){

		ChaMatrix axismat;
		//axismat = srcbone->GetFirstAxisMatZ();
		if (srcbone->GetParent(false)) {
			srcbone->GetParent(false)->CalcAxisMatX_Manipulator(limitdegflag, BONEAXIS_CURRENT, 1, srcbone, &axismat, 1);//nodemat用？
		}
		else {
			ChaMatrixIdentity(&axismat);
		}
		axismat.SetTranslation(srcbone->GetJointFPos());
		srcbone->SetNodeMat(axismat);
	}

	return 0;
}

//###############################################
//この関数の下に　１フレームだけ計算する関数あり
//###############################################
void CModel::CalcBoneEulReq(bool limitdegflag, CBone* curbone, int srcmotid, double startframe, double endframe)
{
	if (!curbone) {
		return;
	}

	if (curbone->IsSkeleton()) {
		ChaVector3 cureul;
		cureul.SetParams(0.0f, 0.0f, 0.0f);
		int paraxiskind = -1;
		double srcframe;
		for (srcframe = startframe; srcframe <= endframe; srcframe += 1.0) {
			cureul = curbone->CalcLocalEulXYZ(limitdegflag, paraxiskind, srcmotid, srcframe, BEFEUL_BEFFRAME);
			curbone->SetLocalEul(limitdegflag, srcmotid, srcframe, cureul, 0);
		}
	}

	if (curbone->GetChild(false)) {
		CalcBoneEulReq(limitdegflag, curbone->GetChild(false), srcmotid, startframe, endframe);
	}
	if (curbone->GetBrother(false)) {
		CalcBoneEulReq(limitdegflag, curbone->GetBrother(false), srcmotid, startframe, endframe);
	}
}

//#################################################
//この関数の上に　フレーム範囲を　計算する関数あり
//#################################################
void CModel::CalcBoneEulReq(bool limitdegflag, CBone* curbone, int srcmotid, double srcframe)
{
	if (!curbone){
		return;
	}

	if (curbone->IsSkeleton()) {
		ChaVector3 cureul;
		cureul.SetParams(0.0f, 0.0f, 0.0f);
		int paraxiskind = -1;//2021/11/18
		//int isfirstbone = 0;
		//cureul = curbone->CalcLocalEulXYZ(paraxsiflag, srcmotid, srcframe, BEFEUL_ZERO);
		cureul = curbone->CalcLocalEulXYZ(limitdegflag, paraxiskind, srcmotid, srcframe, BEFEUL_BEFFRAME);
		curbone->SetLocalEul(limitdegflag, srcmotid, srcframe, cureul, 0);
	}

	if (curbone->GetChild(false)){
		CalcBoneEulReq(limitdegflag, curbone->GetChild(false), srcmotid, srcframe);
	}
	if (curbone->GetBrother(false)){
		CalcBoneEulReq(limitdegflag, curbone->GetBrother(false), srcmotid, srcframe);
	}
}


int CModel::CalcBoneEul(bool limitdegflag, int srcmotid)
{
	if (GetNoBoneFlag() == true) {
		return 0;
	}
	
	//ChaCalcFunc chacalcfunc;
	//chacalcfunc.CalcBoneEul(this, limitdegflag, srcmotid);

	if (m_CalcEulThreads) {
		//g_underCalcEul = true;//2023/10/19 このフラグが立っている間だけ　CalcEulスレッドがHighRpmになる
		SetUnderCalcEul(true);
		int updatecount;
		for (updatecount = 0; updatecount < CALCEUL_THREADSNUM; updatecount++) {
			CThreadingCalcEul* curupdate = m_CalcEulThreads + updatecount;
			curupdate->CalcBoneEul(this, limitdegflag, srcmotid);
		}
		WaitCalcEulFinished();
		//g_underCalcEul = false;//2023/10/19
		SetUnderCalcEul(false);
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

	int childnum = srcbto->GetChildBtSize();
	int childno;
	for (childno = 0; childno < childnum; childno++){
		CBtObject* childbto = srcbto->GetChildBt(childno);
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

		int childnum = srcbto->GetChildBtSize();
		int childno;
		for (childno = 0; childno < childnum; childno++){
			CBtObject* childbto = srcbto->GetChildBt(childno);
			if (childbto){
				DumpBtConstraintReq(childbto, srcdepth + 1);
			}
		}
	}
}

//int CModel::CreatePhysicsPosConstraint(CBone* srcbone)
//{
//	if (!srcbone){
//		return 0;
//	}
//
//	CBone* parentbone = srcbone->GetParent();
//	if (parentbone){
//		CBtObject* curbto = parentbone->GetBtObject(srcbone);
//		if (curbto){
//			curbto->CreatePhysicsPosConstraint();
//			srcbone->SetPosConstraint(1);
//		}
//	}
//
//	return 0;
//}
//
//int CModel::DestroyPhysicsPosConstraint(CBone* srcbone)
//{
//	if (!srcbone){
//		return 0;
//	}
//
//	CBone* parentbone = srcbone->GetParent();
//	if (parentbone){
//		CBtObject* curbto = parentbone->GetBtObject(srcbone);
//		if (curbto){
//			curbto->DestroyPhysicsPosConstraint();
//			srcbone->SetPosConstraint(0);
//		}
//	}
//
//	return 0;
//}
//int CModel::CreatePhysicsPosConstraintAll()
//{
//	if (!m_topbone) {
//		return 0;
//	}
//
//	int forceflag = 1;
//	CreatePhysicsPosConstraintReq(m_topbone, forceflag);
//
//	return 0;
//}
//
//int CModel::DestroyPhysicsPosConstraintAll()
//{
//	if (!m_topbone) {
//		return 0;
//	}
//
//	int forceflag = 1;
//	DestroyPhysicsPosConstraintReq(m_topbone, forceflag);
//
//	return 0;
//}
//int CModel::CreatePhysicsPosConstraintUpper(CBone* srcbone)
//{
//	if (!srcbone) {
//		return 0;
//	}
//
//	CBone* upperbone = srcbone;
//	while (upperbone) {
//		CBone* parentbone = upperbone->GetParent();
//		if (parentbone) {
//			CBtObject* curbto = parentbone->GetBtObject(srcbone);
//			if (curbto) {
//				curbto->CreatePhysicsPosConstraint();
//				srcbone->SetPosConstraint(1);
//			}
//		}
//		upperbone = parentbone;
//	}
//
//	return 0;
//}
//
//int CModel::DestroyPhysicsPosConstraintUpper(CBone* srcbone)
//{
//	if (!srcbone) {
//		return 0;
//	}
//
//	CBone* upperbone = srcbone;
//	while (upperbone) {
//		CBone* parentbone = upperbone->GetParent();
//		if (parentbone) {
//			CBtObject* curbto = parentbone->GetBtObject(srcbone);
//			if (curbto) {
//				curbto->DestroyPhysicsPosConstraint();
//				srcbone->SetPosConstraint(0);
//			}
//		}
//		upperbone = parentbone;
//	}
//
//
//	return 0;
//}
//
//int CModel::CreatePhysicsPosConstraintLower(CBone* srcbone)
//{
//	if (!srcbone) {
//		return 0;
//	}
//
//	int forceflag = 1;
//	CreatePhysicsPosConstraintReq(srcbone, forceflag);
//
//	return 0;
//}
//
//int CModel::DestroyPhysicsPosConstraintLower(CBone* srcbone)
//{
//	if (!srcbone) {
//		return 0;
//	}
//
//	int forceflag = 1;
//	DestroyPhysicsPosConstraintReq(srcbone, forceflag);
//
//
//	return 0;
//}
//
//
////forceflag = 0
//void CModel::CreatePhysicsPosConstraintReq(CBone* srcbone, int forceflag)
//{
//	if (srcbone){
//		if ((forceflag == 1) || (srcbone->GetPosConstraint() == 1)){
//			CreatePhysicsPosConstraint(srcbone);
//		}
//
//		if (srcbone->GetChild()){
//			CreatePhysicsPosConstraintReq(srcbone->GetChild(), forceflag);
//		}
//		if (srcbone->GetBrother()){
//			CreatePhysicsPosConstraintReq(srcbone->GetBrother(), forceflag);
//		}
//	}
//}
//
////forceflag = 0
//void CModel::DestroyPhysicsPosConstraintReq(CBone* srcbone, int forceflag)
//{
//	if (srcbone) {
//		if ((forceflag == 1) || (srcbone->GetPosConstraint() == 0)) {
//			DestroyPhysicsPosConstraint(srcbone);
//		}
//
//		if (srcbone->GetChild()) {
//			DestroyPhysicsPosConstraintReq(srcbone->GetChild(), forceflag);
//		}
//		if (srcbone->GetBrother()) {
//			DestroyPhysicsPosConstraintReq(srcbone->GetBrother(), forceflag);
//		}
//	}
//}

int CModel::SetKinematicTmpLower(CBone* srcbone, bool srcflag)
{
	if (!srcbone) {
		return 1;
	}
	SetKinematicTmpLowerReq(srcbone, srcflag);
	return 0;
}

void CModel::SetKinematicTmpLowerReq(CBone* srcbone, bool srcflag)
{
	if (!srcbone) {
		return;
	}


	if (srcbone->IsSkeleton()) {
		srcbone->SetTmpKinematic(srcflag);
	}

	if (srcbone->GetChild(false)) {
		SetKinematicTmpLowerReq(srcbone->GetChild(false), srcflag);
	}
	if (srcbone->GetBrother(false)) {
		SetKinematicTmpLowerReq(srcbone->GetBrother(false), srcflag);
	}
}


//int CModel::Mass0_All(bool setflag)
//{
//	if (!m_topbone) {
//		return 0;
//	}
//	if (setflag == true) {
//		bool forceflag = true;
//		SetMass0Req(m_topbone, forceflag);
//	}
//	else {
//		RestoreMassReq(m_topbone);
//	}
//	return 0;
//}
//int CModel::Mass0_Upper(bool setflag, CBone* srcbone)
//{
//	if (!srcbone) {
//		return 1;
//	}
//	bool forceflag = true;
//	CBone* setbone = srcbone;
//	while (setbone) {
//		if (setflag == true) {
//			SetMass0(setbone);
//		}
//		else {
//			RestoreMass(setbone);
//		}
//		setbone = setbone->GetParent();
//	}
//	return 0;
//}
//int CModel::Mass0_Lower(bool setflag, CBone* srcbone)
//{
//	if (!srcbone) {
//		return 1;
//	}
//	bool forceflag = true;
//	if (setflag == true) {
//		SetMass0Req(srcbone, forceflag);
//	}
//	else {
//		RestoreMassReq(srcbone);
//	}
//	return 0;
//}
//
//int CModel::SetMass0(CBone* srcbone)
//{
//	if (!srcbone){
//		return 0;
//	}
//
//	CBone* parentbone = srcbone->GetParent();
//	if (parentbone){
//		CBtObject* curbto = parentbone->GetBtObject(srcbone);
//		if (curbto){
//			btVector3 localInertia(0, 0, 0);
//			curbto->GetRigidBody()->setMassProps(0.0, localInertia);
//			srcbone->SetMass0(1);
//		}
//	}
//	return 0;
//}
//int CModel::RestoreMass(CBone* srcbone)
//{
//	if (!srcbone){
//		return 0;
//	}
//
//	CBone* parentbone = srcbone->GetParent();
//	if (parentbone){
//		btScalar setmass = 0.0;
//		CRigidElem* curre = parentbone->GetRigidElem(srcbone);
//		if (curre){
//			setmass = curre->GetMass();
//		}
//
//		CBtObject* curbto = parentbone->GetBtObject(srcbone);
//		if (curbto){
//			btVector3 localInertia(0, 0, 0);
//			curbto->GetRigidBody()->setMassProps(setmass, localInertia);
//			srcbone->SetMass0(0);
//		}
//	}
//	return 0;
//
//}
//
//void CModel::SetMass0Req(CBone* srcbone, bool forceflag)
//{
//	if (srcbone){
//		//強制設定or設定の復元
//		if ((forceflag == true) || (srcbone->GetMass0() == 1)){
//			SetMass0(srcbone);
//		}
//
//		if (srcbone->GetChild()){
//			SetMass0Req(srcbone->GetChild(), forceflag);
//		}
//		if (srcbone->GetBrother()){
//			SetMass0Req(srcbone->GetBrother(), forceflag);
//		}
//	}
//}
//
//void CModel::RestoreMassReq(CBone* srcbone)
//{
//	if (srcbone){
//		RestoreMass(srcbone);
//
//		if (srcbone->GetChild()){
//			RestoreMassReq(srcbone->GetChild());
//		}
//		if (srcbone->GetBrother()){
//			RestoreMassReq(srcbone->GetBrother());
//		}
//	}
//}


int CModel::ApplyBtToMotion(bool limitdegflag)
{
	ApplyBtToMotionReq(limitdegflag, GetTopBone(false));

	return 0;
}

void CModel::ApplyBtToMotionReq(bool limitdegflag, CBone* srcbone)
{
	if (!srcbone)
		return;

	if (!ExistCurrentMotion()) {
		return;
	}
	int curmotid = GetCurrentMotID();

	if (srcbone->IsSkeleton()){
		ChaMatrix btmat;
		if (srcbone->GetChild(false)){
			btmat = srcbone->GetBtMat(true);
		}
		else{
			if (srcbone->GetParent(false) && srcbone->GetParent(false)->IsSkeleton()) {
				btmat = srcbone->GetParent(false)->GetBtMat(true);
			}
			else {
				btmat.SetIdentity();
			}	
		}

		//ChaMatrix btrotmat;
		//btrotmat = MakeRotMatFromChaMatrix(btmat);
		//ChaMatrix setmat = btrotmat;
		//btrotmat._41 = srcbone->GetParent()->GetJointFPos().x;
		//btrotmat._42 = srcbone->GetParent()->GetJointFPos().y;
		//btrotmat._43 = srcbone->GetParent()->GetJointFPos().z;


		ChaMatrix setmat = btmat;

		bool infooutflag = false;
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		srcbone->SetWorldMat(limitdegflag, directsetflag, infooutflag, 0, 
			curmotid, GetCurrentFrame(), setmat,
			onlycheck, fromiktarget);

	}

	if (srcbone->GetChild(false)){
		ApplyBtToMotionReq(limitdegflag, srcbone->GetChild(false));
	}
	if (srcbone->GetBrother(false)){
		ApplyBtToMotionReq(limitdegflag, srcbone->GetBrother(false));
	}

}

CRigidElem* CModel::GetRigidElem(int srcboneno)
{
	CRigidElem* retre = 0;

	//if ((srcboneno >= 0) && (srcreindex >= 0)) {
	if (srcboneno >= 0) {
		CBone* curbone = GetBoneByID(srcboneno);
		if (curbone && curbone->IsSkeleton()) {
			CBone* parentbone = curbone->GetParent(false);
			if (parentbone) {
				//char* filename = GetRigidElemInfo(srcreindex).filename;
				//CRigidElem* curre = parentbone->GetRigidElemOfMap(filename, curbone);
				CRigidElem* curre = parentbone->GetRigidElem(curbone);
				if (curre) {
					retre = curre;
				}
				else {
					retre = 0;
				}
			}
			else {
				retre = 0;
			}
		}
		else {
			retre = 0;
		}
	}
	else {
		retre = 0;
	}

	return retre;
}

CRigidElem* CModel::GetRgdRigidElem(int srcrgdindex, int srcboneno)
{
	CRigidElem* retre = 0;

	if ((srcboneno >= 0) && (srcrgdindex >= 0)) {
		CBone* curbone = GetBoneByID(srcboneno);
		if (curbone && curbone->IsSkeleton()) {
			CBone* parentbone = curbone->GetParent(false);
			if (parentbone) {
				char* filename = GetRigidElemInfo(srcrgdindex).filename;
				CRigidElem* curre = parentbone->GetRigidElemOfMap(filename, curbone);
				if (curre) {
					retre = curre;
				}
				else {
					retre = 0;
				}
			}
			else {
				retre = 0;
			}
		}
		else {
			retre = 0;
		}
	}
	else {
		retre = 0;
	}

	return retre;
}

void CModel::EnableRotChildren(CBone* srcbone, bool srcflag)
{
	if (!srcbone) {
		return;
	}

	//カレントボーンも含めて再帰的に設定する。
	EnableRotChildrenReq(srcbone, srcflag);

}


void CModel::EnableRotChildrenReq(CBone* srcbone, bool srcflag)
{
	if (!srcbone) {
		return;
	}


	if (srcbone->IsSkeleton()) {
		CRigidElem* curre = GetRigidElem(srcbone->GetBoneNo());
		if (curre) {
			if (srcflag == true) {
				curre->SetForbidRotFlag(0);
			}
			else {
				curre->SetForbidRotFlag(1);
			}
		}
	}


	if (srcbone->GetChild(false)) {
		EnableRotChildrenReq(srcbone->GetChild(false), srcflag);
	}
	if (srcbone->GetBrother(false)) {
		EnableRotChildrenReq(srcbone->GetBrother(false), srcflag);
	}
}

void CModel::DestroyScene()
{
	if (m_pscene) {
		m_pscene->Destroy(true);
		m_pscene = 0;
	}
}


void CModel::PhysIKRec(bool limitdegflag, double srcrectime)
{
	if (srcrectime == 0.0) {
		m_physikrec0.clear();
		m_physikrec.clear();
	}

	if (!GetTopBone()) {
		return;
	}

	PhysIKRecReq(limitdegflag, GetTopBone(false), srcrectime);

}

void CModel::PhysIKRecReq(bool limitdegflag, CBone* srcbone, double srcrectime)
{
	if (!srcbone) {
		return;
	}

	if (srcbone->IsSkeleton()) {
		if (!ExistCurrentMotion()) {
			return;
		}

		PHYSIKREC currec;
		currec.time = srcrectime;
		currec.pbone = srcbone;
		currec.btmat = srcbone->GetBtMat(true);

		//currec.btmat = srcbone->GetWorldMat(curmi->motid, curmi->curframe);

		if (srcrectime == 0.0) {
			m_physikrec0.push_back(currec);
		}
		m_physikrec.push_back(currec);
		m_phyikrectime = srcrectime;
	}

	if (srcbone->GetChild(false)) {
		PhysIKRecReq(limitdegflag, srcbone->GetChild(false), srcrectime);
	}
	if (srcbone->GetBrother(false)) {
		PhysIKRecReq(limitdegflag, srcbone->GetBrother(false), srcrectime);
	}
}


void CModel::ApplyPhysIkRec(bool limitdegflag)
{
	if (!GetTopBone()) {
		return;
	}

	if (g_motionbrush_frameleng == 0.0) {
		return;
	}

	double srcmaxtime = m_phyikrectime;

	/*
		g_motionbrush_startframe = startframe;
		g_motionbrush_endframe = endframe;
		g_motionbrush_numframe = endframe - startframe + 1;
		g_motionbrush_frameleng = frameleng;
	*/
	if (g_btsimurecflag == false) {
		//Physics IK

		double curframe;
		for (curframe = g_motionbrush_startframe; curframe <= g_motionbrush_endframe; curframe += 1.0) {
			double currectime;
			if (curframe == g_motionbrush_applyframe) {
				currectime = srcmaxtime - 1.0;
			}
			else if (curframe < g_motionbrush_applyframe) {
				double rectimestep;
				rectimestep = srcmaxtime / (g_motionbrush_applyframe - g_motionbrush_startframe + 1);
				currectime = (double)((int)(rectimestep * (curframe - g_motionbrush_startframe)));
				currectime = (double)((int)min(currectime, (m_phyikrectime - 1.0)));
				currectime = (double)((int)max(0.0, currectime));
			}
			else {
				double rectimestep;
				rectimestep = srcmaxtime / (g_motionbrush_endframe - g_motionbrush_applyframe + 1);
				currectime = (double)((int)(m_phyikrectime - rectimestep * (curframe - g_motionbrush_applyframe)));
				currectime = (double)((int)min(currectime, (m_phyikrectime - 1.0)));
				currectime = (double)((int)max(0.0, currectime));
			}

			ApplyPhysIkRecReq(limitdegflag, GetTopBone(false), curframe, currectime);
		}
	}
	else {
		//bt simulation

		double curframe;
		for (curframe = g_motionbrush_startframe; curframe <= g_motionbrush_endframe; curframe += 1.0) {
			double currectime;
			double rectimestep;
			rectimestep = srcmaxtime / (g_motionbrush_endframe - g_motionbrush_startframe + 1.0);
			currectime = (double)((int)(rectimestep * (curframe - g_motionbrush_startframe)));
			currectime = (double)((int)min(currectime, (m_phyikrectime - 1.0)));
			currectime = (double)((int)max(0.0, currectime));

			ApplyPhysIkRecReq(limitdegflag, GetTopBone(false), curframe, currectime);

		}
	}

	return;
}

void CModel::ApplyPhysIkRecReq(bool limitdegflag, CBone* srcbone, double srcframe, double srcrectime)
{
	if (!srcbone)
		return;

	if (g_motionbrush_numframe == 0) {
		return;
	}

	double roundingframe = RoundingTime(srcframe);

	if (srcbone->IsSkeleton()) {
		if (!ExistCurrentMotion())
			return;


		CBone* btbone = 0;
		//if (srcbone->GetParent()) {
		//	if (srcbone->GetChild()) {
		//		btbone = srcbone;
		//	}
		//	else {
		//		btbone = srcbone->GetParent();
		//	}
		//}
		btbone = srcbone;

		if (btbone) {
			PHYSIKREC recdata0;
			PHYSIKREC recdata;
			bool foundrecdata0 = false;
			bool foundrecdata = false;

			std::vector<PHYSIKREC>::iterator itrfind0;
			for (itrfind0 = m_physikrec0.begin(); itrfind0 != m_physikrec0.end(); itrfind0++) {
				PHYSIKREC curdata = *itrfind0;
				if (curdata.pbone == btbone) {
					recdata0 = curdata;
					foundrecdata0 = true;
					break;
				}
			}

			std::vector<PHYSIKREC>::iterator itrfind;
			for (itrfind = m_physikrec.begin(); itrfind != m_physikrec.end(); itrfind++) {
				PHYSIKREC curdata = *itrfind;
				if (curdata.time == srcrectime) {
					if (curdata.pbone == btbone) {
						recdata = curdata;
						foundrecdata = true;
						break;
					}
				}
			}


			if (foundrecdata0 && foundrecdata) {
				int curmotid = GetCurrentMotID();
				ChaMatrix worldmat0 = btbone->GetWorldMat(limitdegflag, curmotid, RoundingTime(g_motionbrush_applyframe), 0);//or srcframe
				ChaMatrix btmat0 = recdata0.btmat * ChaMatrixInv(GetWorldMat());//カレントボーンのApplyFrameにおけるドラッグ時間ゼロの姿勢
				ChaMatrix btmat = recdata.btmat * ChaMatrixInv(GetWorldMat());//カレントボーンのApplyFrameにおけるドラッグ時間カレントの姿勢

				ChaMatrix curworldmat = btbone->GetWorldMat(limitdegflag, curmotid, roundingframe, 0);


				//#######################################################################################
				// quaternionのローカル座標系の式　invAxis * B * Axis は Matrixでは　Axis * B * InvAxis
				// グローバルに戻すときには InvAxis * (Axis * B * InvAxis ) * Axis
				// 
				// DiffにBtを使う理由
				// worldmatにはマウスドラッグ直後の姿勢が入っている
				// その後、物理のループを回して他の剛体に力を伝え、またフィードバックをもらい剛体の接続を保つ
				// 剛体の接続を保っているのはbtmat
				// 
				// worldmatでdiffをとる場合には
				// TopDownとBottomUpを組み合わせてジョイントの接続を保つ
				// しかしそれをリアルタイムでしない場合、結果は記録と異なるかもしれない
				// 
				// 
				// Btmatを使った姿勢式は以下（シミュレーションしてみて多数の候補の中から選定した数式）
				// ChaMatrix setmat = curworldmat * ChaMatrixInv(btmat0) * btmat;
				// 
				// 
				// 解釈その１
				// world * worldDiff
				// world * InvAxis * LocalDIff * Axis
				// world * InvAxis * (Axis * Diff * InvAxis) * Axis
				// world * InvAxis * (Axis * (InvBt0 * Bt) * InvAxis) * Axis
				// world * InvBt0 * Bt
				// 
				// ######################################################################################

				//########################################################
				//物理IKの数式（シミュレーションしてみて多数の候補の中から選定した数式）
				//########################################################

				ChaMatrix setmat;

				if (g_btsimurecflag == false) {
					//physic IK

					setmat = curworldmat * ChaMatrixInv(btmat0) * btmat;

					int isinitrot = IsInitRot(setmat);
					if (isinitrot == 1) {
						//解釈その３における特異点　編集効果無し
						setmat = curworldmat;
					}
				}
				else {
					//bt simulation
					setmat = btmat;
				}


				//ChaMatrix setmat = curworldmat * ChaMatrixInv(worldmat0) * ChaMatrixInv(btmat0) * btmat;//姿勢が変化している場所で試すと一見合っていたが、やはり全体として次元も違うしInvworld0は必要ない


				//２つのうまくいかない数式
				//worldmatを多用するとworldmatは接続を保っていないので形状が分断することがある。
				//それを直すにはボーン構造をTopDownとBottomUp両方で処理する必要があると思われる。しかしその場合記録した結果と異なるかもしれない。
				//ChaMatrix setmat = curworldmat * curworldmat * ChaMatrixInv(btmat0) * btmat * ChaMatrixInv(curworldmat);
				//ChaMatrix setmat = curworldmat * ChaMatrixInv(curworldmat) * ChaMatrixInv(btmat0) * btmat * curworldmat;


				//if (btbone->GetMass0() == 0) {

				bool infooutflag = false;
				bool directsetflag = true;

				//srcbone->SetWorldMat(0, curmi->motid, roundingframe, setmat);
				if (btbone->GetChild(false)) {
					int onlycheck = 0;
					bool fromiktarget = false;
					btbone->SetWorldMat(limitdegflag, directsetflag, infooutflag, 0,
						curmotid, roundingframe, setmat,
						onlycheck, fromiktarget);
				}
				else {
					//endjointでparentがある場合
					int onlycheck = 0;
					bool fromiktarget = false;
					ChaMatrix parsetmat;
					if (btbone->GetParent(false) && btbone->GetParent(false)->IsSkeleton()) {
						parsetmat = btbone->GetParent(false)->GetWorldMat(limitdegflag, curmotid, roundingframe, 0);//limited????
					}
					else {
						parsetmat = setmat;
					}
					btbone->SetWorldMat(limitdegflag, directsetflag, infooutflag, 0,
						curmotid, roundingframe, parsetmat,
						onlycheck, fromiktarget);
					//btbone->SetWorldMat(1, curmi->motid, roundingframe, parsetmat);
				}

				//CalcBoneEulReq(btbone, curmi->motid, roundingframe);


				//}
			}
		}
	}

	if (srcbone->GetChild(false)) {
		ApplyPhysIkRecReq(limitdegflag, srcbone->GetChild(false), roundingframe, srcrectime);
	}
	if (srcbone->GetBrother(false)) {
		ApplyPhysIkRecReq(limitdegflag, srcbone->GetBrother(false), roundingframe, srcrectime);
	}

}

int CModel::ResetAngleLimit(bool excludebt, int srcval, CBone* srcbone)
{
	if (!srcbone) {
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone && 
				(curbone->IsSkeleton()) &&
				((excludebt == false) || ((excludebt == true) && (curbone->GetBtForce() == 0)))) {
				curbone->ResetAngleLimit(srcval);
			}
		}
	}
	else {
		if (srcbone->IsSkeleton()) {
			if ((excludebt == false) || ((excludebt == true) && (srcbone->GetBtForce() == 0))) {
				srcbone->ResetAngleLimit(srcval);
			}
		}
	}
	return 0;
}

int CModel::AngleLimitReplace180to170(CBone* srcbone)
{
	if (!srcbone) {
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone && (curbone->IsSkeleton())) {
				curbone->AngleLimitReplace180to170();
			}
		}
	}
	else {
		if (srcbone->IsSkeleton()) {
			srcbone->AngleLimitReplace180to170();
		}
	}
	return 0;
}

int CModel::CopyWorldToLimitedWorld()
{
	if (GetNoBoneFlag() == true) {
		return 0;
	}

	if (ExistCurrentMotion()) {
		int curmotid = GetCurrentMotID();
		double maxframe = RoundingTime(GetCurrentMaxFrame());
		SetCopyW2LWFrame(RoundingTime(1.0), maxframe);//2023/10/20

		if (m_CopyW2LWThreads) {

			//g_underCopyW2LW = true;//!!!!!!
			SetUnderCopyW2LW(true);

			int updatecount;
			for (updatecount = 0; updatecount < COPYW2LW_THREADSNUM; updatecount++) {
				CThreadingCopyW2LW* curupdate = m_CopyW2LWThreads + updatecount;
				curupdate->CopyWorldToLimitedWorld(GetTopBone(false), curmotid, 1.0, maxframe);
			}
			WaitCopyW2LWFinished();

			//g_underCopyW2LW = false;//!!!!!!
			SetUnderCopyW2LW(false);
		}


		//unlimtedの計算
		//2023/10/21 CopyW2LWは前フレーム姿勢を考慮していないマルチスレッド　CalcBoneEul()で前フレーム考慮マルチスレッドによる後処理
		//CopyW2LWとCalcBoneEulはセットで呼び出す
		CalcBoneEul(false, curmotid);

	}

	return 0;
}


int CModel::AdditiveCurrentToAngleLimit(CBone* srcbone)
{

	if (ExistCurrentMotion()) {
		bool calclimitdegflag = false;
		CalcBoneEul(calclimitdegflag, GetCurrentMotID());

		if (!srcbone) {
			map<int, CBone*>::iterator itrbone;
			for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
				CBone* curbone = itrbone->second;
				//if (curbone) {
		//物理シミュ用のボーンの制限角度は上書きしないことにした
		//物理シミュ以外のボーンの制限角度を変えるたびに　何回も物理シミュボーンの設定をやり直すのが非常に手間だから
				if (curbone && (curbone->GetBtForce() == 0) && (curbone->IsSkeleton())) {
					curbone->AdditiveCurrentToAngleLimit();
				}
			}
		}
		else {
			//物理シミュ用のボーンの制限角度は上書きしないことにした
			//物理シミュ以外のボーンの制限角度を変えるたびに　何回も物理シミュボーンの設定をやり直すのが非常に手間だから
			if (srcbone && (srcbone->GetBtForce() == 0) && (srcbone->IsSkeleton())) {
				srcbone->AdditiveCurrentToAngleLimit();
			}
		}
	}
	return 0;
}

int CModel::AdditiveAllMotionsToAngleLimit()
{
	if (ExistCurrentMotion()) {
		bool calclimitdegflag = false;
		CalcBoneEul(calclimitdegflag, GetCurrentMotID());

		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			//if (curbone) {
		//物理シミュ用のボーンの制限角度は上書きしないことにした
		//物理シミュ以外のボーンの制限角度を変えるたびに　何回も物理シミュボーンの設定をやり直すのが非常に手間だから
			if (curbone && (curbone->GetBtForce() == 0) && (curbone->IsSkeleton())) {
				curbone->AdditiveAllMotionsToAngleLimit();
			}
		}
	}

	return 0;

}


bool CModel::ChkBoneHasRig(CBone* srcbone)
{
	if (srcbone) {
		int rigno;
		for (rigno = 0; rigno < MAXRIGNUM; rigno++) {
			CUSTOMRIG currig = srcbone->GetCustomRig(rigno);
			if (currig.useflag == 2) {//0: free, 1: allocated, 2: valid
				return true;
			}
		}
	}
	else {
		return false;
	}

	return false;
}

//####################################
// Manager func of CThreadingLoadFbx
//####################################

int CModel::CreateLoadFbxAnim(FbxScene* pscene)
{
	DestroyLoadFbxAnim();
	Sleep(100);


	//if (m_bonelist[0] == NULL) {
	if (GetNoBoneFlag()) {
		//_ASSERT(0);
		return 1;
	}


	m_LoadFbxAnim = new CThreadingLoadFbx[LOADFBXANIMTHREAD];
	if (!m_LoadFbxAnim) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < LOADFBXANIMTHREAD; createno++) {
		CThreadingLoadFbx* curload = m_LoadFbxAnim + createno;
		curload->SetScene(pscene);
		curload->SetModel(this);
		curload->ClearBoneList();
		curload->CreateThread((DWORD)1);
	}

	m_creatednum_loadfbxanim = LOADFBXANIMTHREAD;

	int threadcount = 0;
	int befthreadcount = 0;
	int bonenointhread = 0;
	int bonecount;
	int bonenum = (int)m_bonelist.size();
	//int maxbonenuminthread = bonenum / LOADFBXANIMTHREAD + 1;
	int maxbonenuminthread = bonenum / LOADFBXANIMTHREAD;
	if (maxbonenuminthread == 0) {
		maxbonenuminthread = 1;
	}

	for (bonecount = 0; bonecount < bonenum; bonecount++) {
		CBone* curbone = m_bonelist[bonecount];
		FbxNode* pNode = FindNodeByBone(curbone);


		//カメラとカメラを子供に持つeNullについては、AddMotionPointする目的で対象に加える
		//ボーンモーション読込時にカメラもAddMotionPointしておかないと
		//カメラアニメ読込時のAddMotionの際にmotoidとm_motionkey.size()の比較でエラーになる
		//
		//2024/06/10
		//motidとm_motionkey.size()の間の関係が-1ではないものにも対応
		//スレッド計算対象としてはskeleton, camera, nullandchildiscameraを登録
		//スレッドから呼び出される関数CBone::GetFbxAnim()内でモーションにより読み込み処理をする対象を選択
		if (curbone && (curbone->IsSkeleton() || curbone->IsCamera() || (pNode && IsNullAndChildIsCameraNode(pNode)))) {
			FbxNode* curnode = curbone->GetFbxNodeOnLoad();
			if (curnode) {
				CThreadingLoadFbx* curupdate = m_LoadFbxAnim + threadcount;

				curupdate->SetBoneList(bonenointhread, curnode, curbone);

				threadcount = bonecount / maxbonenuminthread;
				if (threadcount >= LOADFBXANIMTHREAD) {
					threadcount = LOADFBXANIMTHREAD - 1;//余りは全部　最終スレッドに任せる
				}

				if (threadcount == befthreadcount) {
					bonenointhread++;
				}
				else {
					bonenointhread = 0;
				}

				befthreadcount = threadcount;
			}
		}
	}

	return 0;

}

int CModel::DestroyLoadFbxAnim()
{
	if (m_LoadFbxAnim) {
		delete[] m_LoadFbxAnim;
	}
	m_LoadFbxAnim = 0;

	return 0;
}

void CModel::WaitLoadFbxAnimFinished()
{
	if (m_LoadFbxAnim != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int loadcount;
			for (loadcount = 0; loadcount < m_creatednum_loadfbxanim; loadcount++) {
				CThreadingLoadFbx* curload = m_LoadFbxAnim + loadcount;
				if (curload->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == m_creatednum_loadfbxanim) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
			Sleep(0);
		}

	}

}

//########################################
// Manager func of CThreadingUpdateMatrix
//########################################

int CModel::CreateBoneUpdateMatrix()
{

	DestroyBoneUpdateMatrix();
	//Sleep(100);


	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}


	//m_boneupdatematrix = new CThreadingUpdateMatrix[g_UpdateMatrixThreads];
	CThreadingUpdateMatrix* newthreads = new CThreadingUpdateMatrix[g_UpdateMatrixThreads];//作成途中でアクセスされないように
	if (!newthreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < g_UpdateMatrixThreads; createno++) {
		CThreadingUpdateMatrix* curupdate = newthreads + createno;
		curupdate->ClearBoneList();
		curupdate->CreateThread((DWORD)(1 << createno));//2023/08/27

		curupdate->SetModel(this);//2023/08/27
	}


	int threadcount = 0;
	int befthreadcount = 0;
	int bonenointhread = 0;
	int bonecount;
	int bonenum = (int)m_bonelist.size();
	//int maxbonenuminthread = bonenum / g_UpdateMatrixThreads + 1;
	int maxbonenuminthread = bonenum / g_UpdateMatrixThreads;
	if (maxbonenuminthread == 0) {
		maxbonenuminthread = 1;
	}

	for (bonecount = 0; bonecount < bonenum; bonecount++) {
		CBone* curbone = m_bonelist[bonecount];
		if (curbone && (curbone->IsSkeleton() || curbone->IsCamera())) {
			CThreadingUpdateMatrix* curupdate = newthreads + threadcount;

			curupdate->AddBoneList(curbone);

			//threadcount++;
			//threadcount = (threadcount % g_UpdateMatrixThreads);
			//if (threadcount == 0) {
			//	bonenointhread++;
			//}


			threadcount = bonecount / maxbonenuminthread;
			if (threadcount >= g_UpdateMatrixThreads) {
				threadcount = g_UpdateMatrixThreads - 1;//余りは全部　最終スレッドに任せる
			}


			if (threadcount == befthreadcount) {
				bonenointhread++;
			}
			else {
				bonenointhread = 0;
			}

			befthreadcount = threadcount;
		}
	}

	m_boneupdatematrix = newthreads;//作成途中でアクセスされないように　作成完了後にセット
	m_creatednum_boneupdatematrix = g_UpdateMatrixThreads;//DestroyBoneUpdateMatrixで0セットされていて　ここで値をセット


	return 0;
}

int CModel::DestroyBoneUpdateMatrix()
{

	if (m_boneupdatematrix) {
		delete[] m_boneupdatematrix;
	}
	m_boneupdatematrix = 0;
	m_creatednum_boneupdatematrix = 0;

	return 0;
}

void CModel::WaitUpdateMatrixFinished()
{
	if (GetNoBoneFlag() == true) {
		//2023/11/03 ボーンが無いモデルの場合　即リターン
		return;
	}

	if (m_boneupdatematrix != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < m_creatednum_boneupdatematrix; updatecount++) {
				CThreadingUpdateMatrix* curupdate = m_boneupdatematrix + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == m_creatednum_boneupdatematrix) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int CModel::CreateCalcEulThreads()
{
	DestroyCalcEulThreads();
	//Sleep(100);

	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}

	m_CalcEulThreads = new CThreadingCalcEul[CALCEUL_THREADSNUM];
	if (!m_CalcEulThreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < CALCEUL_THREADSNUM; createno++) {
		CThreadingCalcEul* curupdate = m_CalcEulThreads + createno;
		curupdate->ClearBoneList();
		curupdate->CreateThread((DWORD)(1 << createno));

		curupdate->SetModel(this);
	}



	//int threadcount = 0;
	//int bonecount;
	//int bonenum = (int)m_bonelist.size();
	//for (bonecount = 0; bonecount < bonenum; bonecount++) {
	//	CBone* curbone = m_bonelist[bonecount];
	//	if (curbone && curbone->IsSkeleton()) {
	//		CThreadingCalcEul* curupdate = m_CalcEulThreads + threadcount;
	//
	//		curupdate->AddBoneList(curbone);
	//
	//		threadcount++;
	//		if (threadcount >= CALCEUL_THREADSNUM) {
	//			threadcount = 0;
	//		}
	//	}
	//}

	int threadcount = 0;
	int befthreadcount = 0;
	int bonenointhread = 0;
	int bonecount;
	int bonenum = (int)m_bonelist.size();
	//int maxbonenuminthread = bonenum / g_UpdateMatrixThreads + 1;
	int maxbonenuminthread = bonenum / CALCEUL_THREADSNUM;
	if (maxbonenuminthread == 0) {
		maxbonenuminthread = 1;
	}

	for (bonecount = 0; bonecount < bonenum; bonecount++) {
		CBone* curbone = m_bonelist[bonecount];
		if (curbone && (curbone->IsSkeleton() || curbone->IsNullAndChildIsCamera())) {
			CThreadingCalcEul* curupdate = m_CalcEulThreads + threadcount;

			curupdate->AddBoneList(curbone);

			//threadcount++;
			//threadcount = (threadcount % g_UpdateMatrixThreads);
			//if (threadcount == 0) {
			//	bonenointhread++;
			//}


			threadcount = bonecount / maxbonenuminthread;
			if (threadcount >= CALCEUL_THREADSNUM) {
				threadcount = CALCEUL_THREADSNUM - 1;//余りは全部　最終スレッドに任せる
			}


			if (threadcount == befthreadcount) {
				bonenointhread++;
			}
			else {
				bonenointhread = 0;
			}

			befthreadcount = threadcount;
		}
	}


	return 0;
}

int CModel::DestroyCalcEulThreads()
{
	if (m_CalcEulThreads) {
		delete[] m_CalcEulThreads;
	}
	m_CalcEulThreads = 0;

	return 0;

}
void CModel::WaitCalcEulFinished()
{
	if (m_CalcEulThreads != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < CALCEUL_THREADSNUM; updatecount++) {
				CThreadingCalcEul* curupdate = m_CalcEulThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == CALCEUL_THREADSNUM) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}
	}

}

int CModel::CreatePostIKThreads()
{

	DestroyPostIKThreads();
	Sleep(100);


	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}


	m_PostIKThreads = new CThreadingPostIK[POSTIK_THREADSNUM];
	if (!m_PostIKThreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < POSTIK_THREADSNUM; createno++) {
		CThreadingPostIK* curupdate = m_PostIKThreads + createno;
		curupdate->ClearFrameList();
		curupdate->CreateThread((DWORD)(1 << createno));

		curupdate->SetModel(this);
	}

	//int threadcount = 0;
	//int befthreadcount = 0;
	//int bonenointhread = 0;
	//int bonecount;
	//int bonenum = (int)m_bonelist.size();
	//int maxbonenuminthread = bonenum / g_UpdateMatrixThreads + 1;



	//for (bonecount = 0; bonecount < bonenum; bonecount++) {
	//	CBone* curbone = m_bonelist[bonecount];
	//	if (curbone && (curbone->IsSkeleton() || curbone->IsCamera())) {
	//		CThreadingUpdateMatrix* curupdate = m_boneupdatematrix + threadcount;

	//		curupdate->SetBoneList(bonenointhread, curbone);

	//		//threadcount++;
	//		//threadcount = (threadcount % g_UpdateMatrixThreads);
	//		//if (threadcount == 0) {
	//		//	bonenointhread++;
	//		//}


	//		threadcount = bonecount / maxbonenuminthread;

	//		if (threadcount == befthreadcount) {
	//			bonenointhread++;
	//		}
	//		else {
	//			bonenointhread = 0;
	//		}

	//		befthreadcount = threadcount;
	//	}
	//}

	//m_creatednum_boneupdatematrix = g_UpdateMatrixThreads;


	return 0;
}

int CModel::DestroyPostIKThreads()
{

	if (m_PostIKThreads) {
		delete[] m_PostIKThreads;
	}
	m_PostIKThreads = 0;

	return 0;
}

void CModel::WaitPostIKFinished()
{
	if (m_PostIKThreads != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < POSTIK_THREADSNUM; updatecount++) {
				CThreadingPostIK* curupdate = m_PostIKThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == POSTIK_THREADSNUM) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int CModel::SetPostIKFrame(double srcstart, double srcend)
{
	if (!m_PostIKThreads) {
		_ASSERT(0);
		return 1;
	}


	int updatecount;
	for (updatecount = 0; updatecount < POSTIK_THREADSNUM; updatecount++) {
		CThreadingPostIK* curupdate = m_PostIKThreads + updatecount;
		curupdate->ClearFrameList();
	}


	//double setframe;
	//int threadcount = 0;
	//for (setframe = RoundingTime(srcstart); setframe <= RoundingTime(srcend); setframe+= 1.0) {
	//	CThreadingPostIK* curupdate = m_PostIKThreads + threadcount;
	//
	//	curupdate->AddFramenoList(setframe);
	//
	//	threadcount++;
	//	if (threadcount >= POSTIK_THREADSNUM) {
	//		threadcount = 0;
	//	}
	//}

	int threadcount = 0;
	int befthreadcount = 0;
	int framenointhread = 0;
	int framecount;
	int framenum = IntTime(srcend) - IntTime(srcstart) + 1;
	int maxframenuminthread = framenum / POSTIK_THREADSNUM;
	if (maxframenuminthread == 0) {
		maxframenuminthread = 1;
	}

	for (framecount = 0; framecount < framenum; framecount++) {
		double curframe = (double)(framecount + IntTime(srcstart));
		CThreadingPostIK* curupdate = m_PostIKThreads + threadcount;

		curupdate->AddFramenoList(curframe);

		threadcount = framecount / maxframenuminthread;
		if (threadcount >= POSTIK_THREADSNUM) {
			threadcount = POSTIK_THREADSNUM - 1;//余りは全部　最終スレッドに任せる
		}

		if (threadcount == befthreadcount) {
			framenointhread++;
		}
		else {
			framenointhread = 0;
		}

		befthreadcount = threadcount;
	}



	return 0;
}

int CModel::CreateInitMpThreads()
{

	DestroyInitMpThreads();
	//Sleep(100);


	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}

	m_InitMpThreads = new CThreadingInitMp[INITMP_THREADSNUM];
	if (!m_InitMpThreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < INITMP_THREADSNUM; createno++) {
		CThreadingInitMp* curupdate = m_InitMpThreads + createno;
		curupdate->ClearFrameList();
		curupdate->CreateThread((DWORD)(1 << createno));

		curupdate->SetModel(this);
	}

	//int threadcount = 0;
	//int befthreadcount = 0;
	//int bonenointhread = 0;
	//int bonecount;
	//int bonenum = (int)m_bonelist.size();
	//int maxbonenuminthread = bonenum / g_UpdateMatrixThreads + 1;



	//for (bonecount = 0; bonecount < bonenum; bonecount++) {
	//	CBone* curbone = m_bonelist[bonecount];
	//	if (curbone && (curbone->IsSkeleton() || curbone->IsCamera())) {
	//		CThreadingUpdateMatrix* curupdate = m_boneupdatematrix + threadcount;

	//		curupdate->SetBoneList(bonenointhread, curbone);

	//		//threadcount++;
	//		//threadcount = (threadcount % g_UpdateMatrixThreads);
	//		//if (threadcount == 0) {
	//		//	bonenointhread++;
	//		//}


	//		threadcount = bonecount / maxbonenuminthread;

	//		if (threadcount == befthreadcount) {
	//			bonenointhread++;
	//		}
	//		else {
	//			bonenointhread = 0;
	//		}

	//		befthreadcount = threadcount;
	//	}
	//}

	//m_creatednum_boneupdatematrix = g_UpdateMatrixThreads;


	return 0;
}

int CModel::DestroyInitMpThreads()
{

	if (m_InitMpThreads) {
		delete[] m_InitMpThreads;
	}
	m_InitMpThreads = 0;

	return 0;
}

void CModel::WaitInitMpFinished()
{
	if (m_InitMpThreads != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < INITMP_THREADSNUM; updatecount++) {
				CThreadingInitMp* curupdate = m_InitMpThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == INITMP_THREADSNUM) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int CModel::SetInitMpFrame(double srcstart, double srcend)
{
	if (!m_InitMpThreads) {
		_ASSERT(0);
		return 1;
	}


	int updatecount;
	for (updatecount = 0; updatecount < INITMP_THREADSNUM; updatecount++) {
		CThreadingInitMp* curupdate = m_InitMpThreads + updatecount;
		curupdate->ClearFrameList();
	}


	//double setframe;
	//int threadcount = 0;
	//for (setframe = RoundingTime(srcstart); setframe <= RoundingTime(srcend); setframe += 1.0) {
	//	CThreadingInitMp* curupdate = m_InitMpThreads + threadcount;
	//
	//	curupdate->AddFramenoList(setframe);
	//
	//	threadcount++;
	//	if (threadcount >= INITMP_THREADSNUM) {
	//		threadcount = 0;
	//	}
	//}

	int threadcount = 0;
	int befthreadcount = 0;
	int framenointhread = 0;
	int framecount;
	int framenum = IntTime(srcend) - IntTime(srcstart) + 1;
	int maxframenuminthread = framenum / INITMP_THREADSNUM;
	if (maxframenuminthread == 0) {
		maxframenuminthread = 1;
	}

	for (framecount = 0; framecount < framenum; framecount++) {
		double curframe = (double)(framecount + IntTime(srcstart));
		CThreadingInitMp* curupdate = m_InitMpThreads + threadcount;

		curupdate->AddFramenoList(curframe);

		threadcount = framecount / maxframenuminthread;
		if (threadcount >= INITMP_THREADSNUM) {
			threadcount = INITMP_THREADSNUM - 1;//余りは全部　最終スレッドに任せる
		}

		if (threadcount == befthreadcount) {
			framenointhread++;
		}
		else {
			framenointhread = 0;
		}

		befthreadcount = threadcount;
	}



	return 0;
}

int CModel::CreateRetargetThreads()
{

	DestroyRetargetThreads();
	//Sleep(100);


	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}

	m_RetargetThreads = new CThreadingRetarget[RETARGET_THREADSNUM];
	if (!m_RetargetThreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < RETARGET_THREADSNUM; createno++) {
		CThreadingRetarget* curupdate = m_RetargetThreads + createno;
		curupdate->ClearFrameList();
		curupdate->CreateThread((DWORD)(1 << createno));

		curupdate->SetModel(this);
	}

	return 0;
}

int CModel::DestroyRetargetThreads()
{

	if (m_RetargetThreads) {
		delete[] m_RetargetThreads;
	}
	m_RetargetThreads = 0;

	return 0;
}

void CModel::WaitRetargetFinished()
{
	if (m_RetargetThreads != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < RETARGET_THREADSNUM; updatecount++) {
				CThreadingRetarget* curupdate = m_RetargetThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == RETARGET_THREADSNUM) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int CModel::SetRetargetFrame(double srcstart, double srcend)
{
	if (!m_RetargetThreads) {
		_ASSERT(0);
		return 1;
	}


	int updatecount;
	for (updatecount = 0; updatecount < RETARGET_THREADSNUM; updatecount++) {
		CThreadingRetarget* curupdate = m_RetargetThreads + updatecount;
		curupdate->ClearFrameList();
	}


	//double setframe;
	//int threadcount = 0;
	//for (setframe = RoundingTime(srcstart); setframe <= RoundingTime(srcend); setframe += 1.0) {
	//	CThreadingRetarget* curupdate = m_RetargetThreads + threadcount;
	//
	//	curupdate->AddFramenoList(setframe);
	//
	//	threadcount++;
	//	if (threadcount >= RETARGET_THREADSNUM) {
	//		threadcount = 0;
	//	}
	//}

	int threadcount = 0;
	int befthreadcount = 0;
	int framenointhread = 0;
	int framecount;
	int framenum = IntTime(srcend) - IntTime(srcstart) + 1;
	int maxframenuminthread = framenum / RETARGET_THREADSNUM;
	if (maxframenuminthread == 0) {
		maxframenuminthread = 1;
	}

	for (framecount = 0; framecount < framenum; framecount++) {
		double curframe = (double)(framecount + IntTime(srcstart));
		CThreadingRetarget* curupdate = m_RetargetThreads + threadcount;

		curupdate->AddFramenoList(curframe);

		threadcount = framecount / maxframenuminthread;
		if (threadcount >= RETARGET_THREADSNUM) {
			threadcount = RETARGET_THREADSNUM - 1;//余りは全部　最終スレッドに任せる
		}

		if (threadcount == befthreadcount) {
			framenointhread++;
		}
		else {
			framenointhread = 0;
		}

		befthreadcount = threadcount;
	}


	return 0;
}


int CModel::CreateFKTraThreads()
{

	DestroyFKTraThreads();
	//Sleep(100);


	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}

	m_FKTraThreads = new CThreadingFKTra[POSTFKTRA_THREADSNUM];
	if (!m_FKTraThreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < POSTFKTRA_THREADSNUM; createno++) {
		CThreadingFKTra* curupdate = m_FKTraThreads + createno;
		curupdate->ClearFrameList();
		curupdate->CreateThread((DWORD)(1 << createno));

		curupdate->SetModel(this);
	}

	return 0;
}

int CModel::DestroyFKTraThreads()
{

	if (m_FKTraThreads) {
		delete[] m_FKTraThreads;
	}
	m_FKTraThreads = 0;

	return 0;
}

void CModel::WaitFKTraFinished()
{
	if (m_FKTraThreads != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < POSTFKTRA_THREADSNUM; updatecount++) {
				CThreadingFKTra* curupdate = m_FKTraThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == POSTFKTRA_THREADSNUM) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int CModel::SetFKTraFrame(double srcstart, double srcend)
{
	if (!m_FKTraThreads) {
		_ASSERT(0);
		return 1;
	}


	int updatecount;
	for (updatecount = 0; updatecount < POSTFKTRA_THREADSNUM; updatecount++) {
		CThreadingFKTra* curupdate = m_FKTraThreads + updatecount;
		curupdate->ClearFrameList();
	}


	//double setframe;
	//int threadcount = 0;
	//for (setframe = RoundingTime(srcstart); setframe <= RoundingTime(srcend); setframe += 1.0) {
	//	CThreadingFKTra* curupdate = m_FKTraThreads + threadcount;
	//
	//	curupdate->AddFramenoList(setframe);
	//
	//	threadcount++;
	//	if (threadcount >= POSTFKTRA_THREADSNUM) {
	//		threadcount = 0;
	//	}
	//}

	int threadcount = 0;
	int befthreadcount = 0;
	int framenointhread = 0;
	int framecount;
	int framenum = IntTime(srcend) - IntTime(srcstart) + 1;
	int maxframenuminthread = framenum / POSTFKTRA_THREADSNUM;
	if (maxframenuminthread == 0) {
		maxframenuminthread = 1;
	}

	for (framecount = 0; framecount < framenum; framecount++) {
		double curframe = (double)(framecount + IntTime(srcstart));
		CThreadingFKTra* curupdate = m_FKTraThreads + threadcount;

		curupdate->AddFramenoList(curframe);

		threadcount = framecount / maxframenuminthread;
		if (threadcount >= POSTFKTRA_THREADSNUM) {
			threadcount = POSTFKTRA_THREADSNUM - 1;//余りは全部　最終スレッドに任せる
		}

		if (threadcount == befthreadcount) {
			framenointhread++;
		}
		else {
			framenointhread = 0;
		}

		befthreadcount = threadcount;
	}

	return 0;
}

int CModel::CreateCopyW2LWThreads()
{

	DestroyCopyW2LWThreads();
	//Sleep(100);


	if (m_bonelist[0] == NULL) {
		_ASSERT(0);
		return 1;
	}

	if (GetNoBoneFlag()) {
		return 0;
	}

	m_CopyW2LWThreads = new CThreadingCopyW2LW[COPYW2LW_THREADSNUM];
	if (!m_CopyW2LWThreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < COPYW2LW_THREADSNUM; createno++) {
		CThreadingCopyW2LW* curupdate = m_CopyW2LWThreads + createno;
		curupdate->ClearFrameList();
		curupdate->CreateThread((DWORD)(1 << createno));

		curupdate->SetModel(this);
	}

	return 0;
}

int CModel::DestroyCopyW2LWThreads()
{

	if (m_CopyW2LWThreads) {
		delete[] m_CopyW2LWThreads;
	}
	m_CopyW2LWThreads = 0;

	return 0;
}

void CModel::WaitCopyW2LWFinished()
{
	if (m_CopyW2LWThreads != NULL) {

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < COPYW2LW_THREADSNUM; updatecount++) {
				CThreadingCopyW2LW* curupdate = m_CopyW2LWThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == COPYW2LW_THREADSNUM) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int CModel::SetCopyW2LWFrame(double srcstart, double srcend)
{
	if (!m_CopyW2LWThreads) {
		_ASSERT(0);
		return 1;
	}


	int updatecount;
	for (updatecount = 0; updatecount < COPYW2LW_THREADSNUM; updatecount++) {
		CThreadingCopyW2LW* curupdate = m_CopyW2LWThreads + updatecount;
		curupdate->ClearFrameList();
	}


	//double setframe;
	//int threadcount = 0;
	//for (setframe = RoundingTime(srcstart); setframe <= RoundingTime(srcend); setframe += 1.0) {
	//	CThreadingCopyW2LW* curupdate = m_CopyW2LWThreads + threadcount;
	//
	//	curupdate->AddFramenoList(setframe);
	//
	//	threadcount++;
	//	if (threadcount >= COPYW2LW_THREADSNUM) {
	//		threadcount = 0;
	//	}
	//}


	int threadcount = 0;
	int befthreadcount = 0;
	int framenointhread = 0;
	int framecount;
	int framenum = IntTime(srcend) - IntTime(srcstart) + 1;
	int maxframenuminthread = framenum / COPYW2LW_THREADSNUM;
	if (maxframenuminthread == 0) {
		maxframenuminthread = 1;
	}

	for (framecount = 0; framecount < framenum; framecount++) {
		double curframe = (double)(framecount + IntTime(srcstart));
		CThreadingCopyW2LW* curupdate = m_CopyW2LWThreads + threadcount;

		curupdate->AddFramenoList(curframe);

		threadcount = framecount / maxframenuminthread;
		if (threadcount >= COPYW2LW_THREADSNUM) {
			threadcount = COPYW2LW_THREADSNUM - 1;//余りは全部　最終スレッドに任せる
		}

		if (threadcount == befthreadcount) {
			framenointhread++;
		}
		else {
			framenointhread = 0;
		}

		befthreadcount = threadcount;
	}


	return 0;
}


int CModel::InitMpFrame(bool limitdegflag, int srcmotid, CBone* srcbone, double srcstartframe, double srcendframe)
{
	if (!GetTopBone(false) || !srcbone) {
		return 0;
	}

	//g_underInitMp = true;//!!!!!!!!!!!!!!!!!!
	
	MOTINFO curmi = GetMotInfo(srcmotid);
	if (curmi.motid > 0) {
		double startframe = RoundingTime(srcstartframe);
		double endframe = RoundingTime(srcendframe);
		

		//double curframe;
		//for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
		//	InitMPReq(limitdegflag, GetTopBone(false), srcmotid, curframe);
		//}


		//###################
		//マルチスレッド処理
		//###################
		SetInitMpFrame(RoundingTime(startframe), RoundingTime(endframe));//2023/10/17
		if (m_InitMpThreads) {
			SetUnderInitMP(true);
			int updatecount;
			for (updatecount = 0; updatecount < INITMP_THREADSNUM; updatecount++) {
				CThreadingInitMp* curupdate = m_InitMpThreads + updatecount;
				curupdate->InitMPReq(limitdegflag, srcbone, srcmotid);
			}
			WaitInitMpFinished();
			SetUnderInitMP(false);
		}
	}
	
	//g_underInitMp = false;//!!!!!!!!!!!!!!!!!!
	

	return 0;
}


void CModel::InitMPReq(bool limitdegflag, CBone* curbone, int srcmotid, double curframe)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.InitMPReq(this, limitdegflag, curbone, srcmotid, curframe);
}



int CModel::InitMP(bool limitdegflag, CBone* curbone, int srcmotid, double curframe)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.InitMP(this, limitdegflag, curbone, srcmotid, curframe);


	//CMotionPoint* pcurmp = 0;
	//pcurmp = curbone->GetMotionPoint(GetCurMotInfo()->motid, curframe);

	//if (pcurmp) {

	//	//pcurmp->SetBefWorldMat(pcurmp->GetWorldMat());

	//	ChaMatrix xmat = curbone->GetFirstMat();
	//	pcurmp->SetWorldMat(xmat);
	//	curbone->SetInitMat(xmat);

	//}
	//else {
	//	CMotionPoint* curmp3 = 0;
	//	int existflag3 = 0;
	//	curmp3 = curbone->AddMotionPoint(GetCurMotInfo()->motid, curframe, &existflag3);
	//	if (!curmp3) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//	ChaMatrix xmat = curbone->GetFirstMat();
	//	curmp3->SetWorldMat(xmat);
	//	curbone->SetInitMat(xmat);
	//	//_ASSERT( 0 );
	//}

	////オイラー角初期化
	//ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
	//int paraxsiflag = 1;
	////int isfirstbone = 0;
	//cureul = curbone->CalcLocalEulXYZ(paraxsiflag, GetCurMotInfo()->motid, curframe, BEFEUL_ZERO);
	//curbone->SetLocalEul(GetCurMotInfo()->motid, curframe, cureul);

	
	if ((GetNoBoneFlag() == false) && curbone && (curbone->IsSkeleton())) {
		curbone->InitMP(limitdegflag, srcmotid, curframe);
	}
	

	return 0;
}


bool CModel::IncludeRootOrReference(FbxNode* ptopnode)
{
	FbxNode* foundnode = 0;
	GetRootOrReferenceReq(ptopnode, &foundnode);
	if (foundnode != 0) {
		return true;
	}
	else {
		return false;
	}
}
void CModel::GetRootOrReferenceReq(FbxNode* srcnode, FbxNode** dstppnode)
{
	if (srcnode && dstppnode && !(*dstppnode)) {

		if ((strstr(srcnode->GetName(), "Root") != 0) || (strstr(srcnode->GetName(), "Reference") != 0)) {
			*dstppnode = srcnode;
			return;
		}

		if (!(*dstppnode)) {
			int childNodeNum;
			childNodeNum = srcnode->GetChildCount();
			for (int i = 0; i < childNodeNum; i++)
			{
				FbxNode* pChild = srcnode->GetChild(i);  // 子ノードを取得
				if (pChild) {
					GetRootOrReferenceReq(pChild, dstppnode);
				}
			}
		}
	}

}

void CModel::CheckVRoidJointNameReq(CBone* srcbone, bool* dstflag)
{
	if (!srcbone || !dstflag) {
		return;
	}

	if (*dstflag == true) {
		return;
	}

	if (strcmp(srcbone->GetBoneName(), "J_Bip_C_Hips") == 0) {
		*dstflag = true;
		return;
	}
	else {
		if (srcbone->GetBrother(false)) {
			CheckVRoidJointNameReq(srcbone->GetBrother(false), dstflag);
		}
		if (srcbone->GetChild(false)) {
			CheckVRoidJointNameReq(srcbone->GetChild(false), dstflag);
		}
	}
}


void CModel::GetHipsBoneReq(CBone* srcbone, CBone** dstppbone)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.GetHipsBoneReq(this, srcbone, dstppbone);
}

void CModel::CalcModelWorldMatOnLoad()
{
	ChaMatrix scalemat;
	scalemat.SetIdentity();
	ChaMatrix rotmat;
	rotmat.SetIdentity();
	rotmat.SetXYZRotation(0, GetModelRotation());
	ChaMatrix tramat;
	tramat.SetIdentity();
	tramat.SetTranslation(GetModelPosition());
	ChaMatrix modelnodemat;
	modelnodemat.SetIdentity();

	ChaMatrix worldmatonload;
	worldmatonload.SetIdentity();
	worldmatonload = ChaMatrixFromSRT(true, true, modelnodemat, &scalemat, &rotmat, &tramat);

	//m_worldmat = worldmatonload;
	m_matWorld = worldmatonload;
}

CBone* CModel::GetTopBone(bool excludenullflag)//default : excludenullflag = true
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetTopBone(this, excludenullflag);
}
void CModel::GetTopBoneReq(CBone* srcbone, CBone** pptopbone, bool excludenullflag)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.GetTopBoneReq(this, srcbone, pptopbone, excludenullflag);
}


int CModel::GetBoneForMotionSize() {//eNull含まない
	int retsize = 0;
	std::map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {
			retsize++;
		}
	}
	return retsize;
};

int CModel::GetMaxBoneNo()
{
	int retno = 0;
	std::map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {
			int cmpno = curbone->GetBoneNo();
			if (cmpno > retno) {
				retno = cmpno;
			}
		}
	}
	return retno;
}


CBone* CModel::GetRootBone()
{
	return m_topbone;//GetTopBone()ではなく　eNullのRootNodeを想定
}

CNodeOnLoad* CModel::FindNodeOnLoadByName(const char* srcname)
{
	if (srcname) {
		if (GetNodeOnLoad()) {
			CNodeOnLoad* findnodeonload = 0;
			FindNodeOnLoadByNameReq(GetNodeOnLoad(), srcname, &findnodeonload);
			return findnodeonload;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}
void CModel::FindNodeOnLoadByNameReq(CNodeOnLoad* srcnodeonload, const char* srcname, CNodeOnLoad** ppfindnodeonload)
{
	if (srcnodeonload && !(*ppfindnodeonload)) {

		FbxNode* pnode = srcnodeonload->GetNode();
		if (pnode) {
			if (strcmp(pnode->GetName(), srcname) == 0) {
				*ppfindnodeonload = srcnodeonload;
				return;
			}
		}

		if (!(*ppfindnodeonload)) {
			int childnum = srcnodeonload->GetChildNum();
			int childno;
			for (childno = 0; childno < childnum; childno++) {
				CNodeOnLoad* pchild = srcnodeonload->GetChild(childno);
				if (pchild) {
					FindNodeOnLoadByNameReq(pchild, srcname, ppfindnodeonload);
				}
			}
		}
	}
}

FbxNode* CModel::FindNodeByBone(CBone* srcbone)
{
	if (srcbone) {
		FbxNode* retnode = 0;
		map<CBone*, FbxNode*>::iterator itrnode;
		itrnode = m_bone2node.find(srcbone);
		if (itrnode != m_bone2node.end()) {
			retnode = itrnode->second;
			return retnode;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}

CBone* CModel::FindBoneByNode(FbxNode* srcnode)
{
	if (srcnode) {
		CBone* retbone = 0;
		map<FbxNode*, CBone*>::iterator itrbone;
		itrbone = m_node2bone.find(srcnode);
		if (itrbone != m_node2bone.end()) {
			retbone = itrbone->second;
			return retbone;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}


//2023/07/04 NodeMatが変わらないように　SetRotationActiveは変更しないことに
//void CModel::SetRotationActiveToBone()
//{
//	SetRotationActiveToBoneReq(GetNodeOnLoad());
//}
//void CModel::SetRotationActiveFalse()
//{
//	SetRotationActiveFalseReq(GetNodeOnLoad());
//}
//void CModel::SetRotationActiveTrue()
//{
//	SetRotationActiveTrueReq(GetNodeOnLoad());
//}
//void CModel::SetRotationActiveDefault()
//{
//	SetRotationActiveDefaultReq(GetNodeOnLoad());
//}
//
//void CModel::SetRotationActiveToBoneReq(CNodeOnLoad* srcnodeonload)
//{
//	if (srcnodeonload) {
//
//		FbxNode* pNode = srcnodeonload->GetNode();
//		if (pNode) {
//			CBone* curbone = srcnodeonload->GetBone();
//			if (curbone) {
//				curbone->SetFbxRotationActive(pNode->GetRotationActive());
//			}
//		}
//
//		int childnum = srcnodeonload->GetChildNum();
//		int childno;
//		for (childno = 0; childno < childnum; childno++) {
//			CNodeOnLoad* childonload = srcnodeonload->GetChild(childno);
//			if (childonload) {
//				SetRotationActiveToBoneReq(childonload);
//			}
//		}
//	}
//}
//void CModel::SetRotationActiveFalseReq(CNodeOnLoad* srcnodeonload)
//{
//	if (srcnodeonload) {
//
//		FbxNode* pNode = srcnodeonload->GetNode();
//		if (pNode) {
//			CBone* curbone = srcnodeonload->GetBone();
//			if (curbone) {
//				if (curbone->IsSkeleton()) {
//					pNode->SetRotationActive(false);
//				}
//				else {
//					pNode->SetRotationActive(true);
//				}
//			}
//		}
//
//		int childnum = srcnodeonload->GetChildNum();
//		int childno;
//		for (childno = 0; childno < childnum; childno++) {
//			CNodeOnLoad* childonload = srcnodeonload->GetChild(childno);
//			if (childonload) {
//				SetRotationActiveFalseReq(childonload);
//			}
//		}
//	}
//}
//void CModel::SetRotationActiveTrueReq(CNodeOnLoad* srcnodeonload)
//{
//	if (srcnodeonload) {
//
//		FbxNode* pNode = srcnodeonload->GetNode();
//		if (pNode) {
//			CBone* curbone = srcnodeonload->GetBone();
//			if (curbone && curbone->IsSkeleton()) {
//				pNode->SetRotationActive(true);
//			}
//		}
//
//		int childnum = srcnodeonload->GetChildNum();
//		int childno;
//		for (childno = 0; childno < childnum; childno++) {
//			CNodeOnLoad* childonload = srcnodeonload->GetChild(childno);
//			if (childonload) {
//				SetRotationActiveTrueReq(childonload);
//			}
//		}
//	}
//
//}
//void CModel::SetRotationActiveDefaultReq(CNodeOnLoad* srcnodeonload)
//{
//	if (srcnodeonload) {
//
//		FbxNode* pNode = srcnodeonload->GetNode();
//		if (pNode) {
//			CBone* curbone = srcnodeonload->GetBone();
//			if (curbone) {
//				pNode->SetRotationActive(curbone->GetFbxRotationActive());
//			}
//		}
//
//		int childnum = srcnodeonload->GetChildNum();
//		int childno;
//		for (childno = 0; childno < childnum; childno++) {
//			CNodeOnLoad* childonload = srcnodeonload->GetChild(childno);
//			if (childonload) {
//				SetRotationActiveDefaultReq(childonload);
//			}
//		}
//	}
//
//}

int CModel::GetFBXCameraAnim(int cameramotid, double animleng)
{
	if (!m_camerafbx.IsLoaded()) {
		_ASSERT(0);
		return 1;
	}

	SetCameraMotionId(cameramotid);

	CAMERANODE* curcn = m_camerafbx.GetCameraNode(cameramotid);
	if (!curcn) {
		_ASSERT(0);
		return 1;
	}


	double framecnt;
	for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {
		bool calcbynode = true;
		bool setmotionpoint = true;
		ChaMatrix cameramat = GetCameraTransformMat(cameramotid, framecnt, CAMERA_INHERIT_ALL, 
			calcbynode, setmotionpoint);
	}

	//int result;
	//result = curcn->pbone->CreateIndexedMotionPoint(cameramotid, animleng);
	//if (result != 0) {
	//	_ASSERT(0);
	//	return 1;
	//}

	return 0;
}


CAMERANODE* CModel::GetCAMERANODE(int cameramotid)
{
	if (!m_camerafbx.IsLoaded()) {
		_ASSERT(0);
		return nullptr;
	}

	return m_camerafbx.GetCameraNode(cameramotid);
}

ChaMatrix CModel::GetCameraTransformMat(int cameramotid, double nextframe, int inheritmode, 
	bool calcbynode, bool setmotionpoint)
{
	ChaMatrix retmat;

	if (!m_camerafbx.IsLoaded()) {
		_ASSERT(0);
		retmat.SetIdentity();
		return retmat;
	}

	return m_camerafbx.GetCameraTransformMat(this, cameramotid, nextframe, inheritmode, 
		calcbynode, setmotionpoint);

}

ChaVector3 CModel::CalcCameraFbxEulXYZ(int cameramotid, double srcframe)
{
	ChaVector3 reteul;
	reteul.SetParams(0.0f, 0.0f, 0.0f);

	if (!m_camerafbx.IsLoaded()) {
		_ASSERT(0);
		return reteul;
	}

	CAMERANODE* curcn = m_camerafbx.GetCameraNode(cameramotid);
	if (!curcn) {
		_ASSERT(0);
		return reteul;
	}

	return m_camerafbx.CalcCameraFbxEulXYZ(cameramotid, srcframe);
}


int CModel::GetCameraAnimParams(double nextframe, double camdist, 
	ChaVector3* pEyePos, ChaVector3* pTargetPos, ChaVector3* pcamupvec, 
	ChaMatrix* protmat, int inheritmode)
{
	if (!pEyePos || !pTargetPos || !pcamupvec) {
		//###################################################
		//protmatがNULLの場合も許可　rotmatをセットしないだけ
		//###################################################
		_ASSERT(0);
		return 1;
	}

	if (!m_camerafbx.IsLoaded()) {
		return 0;
	}


	//###########################################################################################
	// UnityAsset TheHuntのStreet1でテスト
	// テスト方法
	// Documents/TroubleShooting/AboutCameraAnim.docxを参照のこと
	//
	//###########################################################################################


	int cameramotionid = GetCameraMotionId();
	return GetCameraAnimParams(cameramotionid, nextframe, camdist, pEyePos, pTargetPos, pcamupvec, protmat, inheritmode);

}

int CModel::GetCameraAnimParams(int cameramotionid, double nextframe, double camdist, ChaVector3* pEyePos, ChaVector3* pTargetPos, ChaVector3* pcamupvec, ChaMatrix* protmat, int inheritmode)
{
	if (!pEyePos || !pTargetPos || !pcamupvec) {
		//###################################################
		//protmatがNULLの場合も許可　rotmatをセットしないだけ
		//###################################################
		_ASSERT(0);
		return 1;
	}

	if (!m_camerafbx.IsLoaded()) {
		return 0;
	}

	CAMERANODE* curcn = m_camerafbx.GetCameraNode(cameramotionid);
	if (!curcn) {
		curcn = m_camerafbx.GetFirstValidCameraNode();//anim無しのカメラ用
		if (!curcn) {
			return 0;
		}
	}

	if (nextframe == 0.0) {
		*pEyePos = curcn->position;
		*pTargetPos = *pEyePos + curcn->dirvec * camdist;
	}
	else {
		m_camerafbx.GetCameraAnimParams(this, cameramotionid, nextframe, camdist, pEyePos, pTargetPos, pcamupvec, protmat, inheritmode);
	}

	return 0;
}


ChaVector3 CModel::GetCameraAdjustPos(int cameramotid)
{
	ChaVector3 rettra;
	rettra.SetParams(0.0f, 0.0f, 0.0f);

	if (!m_camerafbx.IsLoaded()) {
		return rettra;
	}

	CAMERANODE* curcn = m_camerafbx.GetCameraNode(cameramotid);
	if (!curcn) {
		return rettra;
	}


	rettra = curcn->adjustpos;
	return rettra;
}





int CModel::GetCameraProjParams(int cameramotid, float* pprojnear, float* pprojfar, float* pfovy, ChaVector3* pcampos, ChaVector3* pcamdir, ChaVector3* pcamupvec)
{
	if (!pprojnear || !pprojfar || !pfovy || !pcampos || !pcamdir || !pcamupvec) {
		_ASSERT(0);
		return 1;
	}


	CAMERANODE* curcn = m_camerafbx.GetCameraNode(cameramotid);
	if (!curcn) {
		return 0;
	}

	*pprojnear = (float)curcn->nearZ;
	*pprojfar = (float)curcn->farZ;
	*pfovy = (float)curcn->fovY;
	*pcampos = curcn->position;
	*pcamdir = curcn->dirvec;
	*pcamupvec = curcn->upvec;

	return 0;
}



//CCameraFbx CModel::GetCameraFbx()
//{
//	return m_camerafbx;
//}
void CModel::SetCameraMotionId(int srcid)
{
	m_cameramotionid = srcid;

	MOTINFO camerami = GetMotInfo(srcid);
	if (camerami.motid > 0) {
		map<int, CBone*>::iterator itrbone;
		for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone) {
				if (curbone->IsCamera() || (curbone->IsNull() && (strcmp(curbone->GetBoneName(), camerami.motname) == 0))) {
					curbone->SetCurrentMotion(srcid, camerami.frameleng);
				}
			}
		}
		//ResetMotionCache();

		if ((camerami.fbxanimno >= 0) && GetScene()) {
			FbxAnimStack* lCurrentAnimationStack = GetScene()->FindMember<FbxAnimStack>(mAnimStackNameArray[camerami.fbxanimno]->Buffer());
			if (lCurrentAnimationStack != NULL) {
				FbxAnimLayer* mCurrentAnimLayer;
				mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
				SetCurrentAnimLayer(mCurrentAnimLayer);
				GetScene()->SetCurrentAnimationStack(lCurrentAnimationStack);
			}
		}
	}
}
int CModel::GetCameraMotionId()
{
	return m_cameramotionid;
}
void CModel::SetCameraMotionFrame(int cameramotid, double srcframe)
{
	SetCameraMotionId(cameramotid);
	MOTINFO* miptr = GetMotInfoPtr(cameramotid);
	if (miptr && miptr->cameramotion) {
		double setframe = max(0.0, min(srcframe, (miptr->frameleng - 1.0)));
		miptr->curframe = setframe;
	}
	else {
		_ASSERT(0);
	}

}



int CModel::GetCurrentMotion()
{
	if (m_curmotinfo) {
		return m_curmotinfo->motid;
	}
	else {
		return -1;
	}
}
//double CModel::GetCurrentMotionFrame()
//{
//	return GetCurrentFrame();
//}

//2023/07/21
int CModel::SetIKStopFlag()
{
	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_bonelist.begin(); itrbone != m_bonelist.end(); itrbone++) {
		CBone* srcbone = itrbone->second;
		if (srcbone) {
			if (IsIKStopName(srcbone->GetBoneName())) {
				srcbone->SetIKStopFlag(true);
			}
			else {
				srcbone->SetIKStopFlag(false);
			}
		}
	}

	return 0;
}

int CModel::ChkInView(int refposindex)
{
	if (s_workingChkinView) {
		//2024/04/06
		//IMComputeは即時実行用であり、複数同時実行できない仕様
		//ChkInViewは再入禁止
		//
		//CBone::UpdateMatrixはコンテクスト限定マルチスレッド可能だが
		//CModel::UpdateMatrixはChkInViewを呼び出すのでマルチスレッド不可
		//CModel::UpdataMatrixはシングルスレッド部分から呼び出すことにした
		//ここは実行時に通らなくなった
		return 0;
	}
	s_workingChkinView = true;


	if (!m_chkinview || !g_cameraShadow) {
		_ASSERT(0);
		s_workingChkinView = false;
		return 0;
	}


	if (GetUnderRetarget() == true) {

		//2024/06/21
		//リターゲット中は強制的に視野内として設定
		//視野外のままリターゲットすると　計算が行われなかったり　1フレーム目の移動アニメが２倍になったりした

		SetInView(true, refposindex);
		SetInShadow(false, refposindex);

		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			if (curobj) {
				curobj->SetInView(true, refposindex);
			}
		}
		if (GetSkyFlag()) {
			SetDistChkInView(500000.0f, refposindex);
		}
		else {
			SetDistChkInView(0.01f, refposindex);//2024/03/25
		}

		SetInMorph(false);
	}
	else if (GetSkyFlag() || (wcsstr(GetFileName(), L".mqo") != 0)) {

		//このアプリにおいては　mqoファイル(マニピュレータや地面格子)はクリッピングしない用途に使用しているので　常に描画するように

		SetInView(true, refposindex);
		SetInShadow(false, refposindex);

		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			if (curobj) {
				curobj->SetInView(true, refposindex);
			}
		}
		if (GetSkyFlag()) {
			SetDistChkInView(500000.0f, refposindex);
		}
		else {
			SetDistChkInView(0.01f, refposindex);//2024/03/25
		}

		SetInMorph(false);
	}
	else if (m_object.empty() || (GetFromBvhFlag())) {

		//bvhにはメッシュが無いが　UpdateMatrixを実行するためにInViewである必要

		SetInView(true, refposindex);
		SetInShadow(false, refposindex);

		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			if (curobj) {
				curobj->SetInView(true, refposindex);
			}
		}

		SetDistChkInView(0.01f, refposindex);//2024/03/25

		SetInMorph(false);
	}
	else if (GetGrassFlag()) {

		//2024/05/14
		//草はインスタンシング表示で　位置情報がCModelには無い
		//草はローポリゴンでインスタンシング表示は軽い
		//１つずつ視野内判定しても速くならない可能性が高い(後で確認する必要があるが)
		//とりあえずは　草は全て視野内として描画する

		int refposindexZero = 0;

		SetInView(true, refposindexZero);
		SetInShadow(false, refposindexZero);

		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			if (curobj) {
				if ((strstr(curobj->GetName(), "LOD1") != 0) ||
					(strstr(curobj->GetName(), "LOD2") != 0) ||
					(strstr(curobj->GetName(), "LOD3") != 0)) {
					curobj->SetInView(false, refposindexZero);
				}
				else {
					curobj->SetInView(true, refposindexZero);
				}
			}
		}
		SetDistChkInView(0.01f, refposindex);

		SetInMorph(false);
	}
	else {
/*
	int mBufferSize[4];//[0]:bsnum
	Matrix mWorld;		//ワールド行列。
	float camEye[4];
	float camDir[4];
	float params1[4];//[0]:BACKPOSCOEF, [1]:g_fovy, [2]:g_projfar, [3]:g_projnear
	float lodrate2L[4];
	float lodrate3L[4];
	float shadowPos[4];
	float shadowDir[4];
	float shadowparams1[4];//[0]:shadowfov, [1]:shadowmaxdist(g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno])
	float frustumPlanes[6][4];
	float frustumCorners[8][4];
	float shadowPlanes[6][4];
	float shadowCorners[8][4];
*/
		ChaMatrix chkMatWorld = GetMatrixForChkInView(m_matWorld);

		CSConstantBufferChkInView cb;
		cb.Init();
		cb.mWorld = chkMatWorld.TKMatrix();
		cb.camEye[0] = g_camEye.x;
		cb.camEye[1] = g_camEye.y;
		cb.camEye[2] = g_camEye.z;
		cb.camEye[3] = 1.0f;
		ChaVector3 camdir = g_camtargetpos - g_camEye;
		ChaVector3Normalize(&camdir, &camdir);
		cb.params1[0] = CHKINVIEW_BACKPOSCOEF;
		cb.params1[1] = (float)cos(g_fovy * 0.85f);
		cb.params1[2] = g_projfar;
		cb.params1[3] = g_projnear;
		//cb.lodrate2L[0] = g_lodrate2L[0];
		//cb.lodrate2L[1] = g_lodrate2L[1];
		//cb.lodrate2L[2] = 1.0f;
		//cb.lodrate2L[3] = 1.0f;
		//cb.lodrate3L[0] = g_lodrate3L[0];
		//cb.lodrate3L[1] = g_lodrate3L[1];
		//cb.lodrate3L[2] = g_lodrate3L[2];
		//cb.lodrate3L[3] = 1.0f;
		//cb.lodrate4L[0] = g_lodrate4L[0];
		//cb.lodrate4L[1] = g_lodrate4L[1];
		//cb.lodrate4L[2] = g_lodrate4L[2];
		//cb.lodrate4L[3] = g_lodrate4L[3];


		//##########################################################################
		//2024/04/25 シェーダーでif文を実行すると重いので　あらかじめ配列に計算結果を入れておく
		//##########################################################################
		cb.lodmindist[0][0] = 0.0f;                      //lod無し　mindist
		cb.lodmaxdist[0][0] = g_projfar;                 //lod無し　maxdist
			
		cb.lodmindist[1][0] = 0.0f;                      //num:2levels lod:1 mindist
		cb.lodmaxdist[1][0] = g_lodrate2L[0] * g_projfar;//num:2levels lod:1 maxdist
		cb.lodmindist[1][1] = g_lodrate2L[0] * g_projfar;//num:2levels lod:2 mindist
		cb.lodmaxdist[1][1] = g_lodrate2L[1] * g_projfar;//num:2levels lod:2 maxdist

		cb.lodmindist[2][0] = 0.0f;                      //num:3levels lod1 mindist
		cb.lodmaxdist[2][0] = g_lodrate3L[0] * g_projfar;//num:3levels lod1 maxdist
		cb.lodmindist[2][1] = g_lodrate3L[0] * g_projfar;//num:3levels lod:2 mindist
		cb.lodmaxdist[2][1] = g_lodrate3L[1] * g_projfar;//num:3levels lod:2 maxdist
		cb.lodmindist[2][2] = g_lodrate3L[1] * g_projfar;//num:3levels lod:3 mindist
		cb.lodmaxdist[2][2] = g_lodrate3L[2] * g_projfar;//num:3levels lod:3 maxdist

		cb.lodmindist[3][0] = 0.0f;                      //num:4levels lod1 mindist
		cb.lodmaxdist[3][0] = g_lodrate4L[0] * g_projfar;//num:4levels lod1 maxdist
		cb.lodmaxdist[3][0] = g_lodrate4L[0] * g_projfar;//num:4levels lod1 maxdist
		cb.lodmindist[3][1] = g_lodrate4L[0] * g_projfar;//num:4levels lod:2 mindist
		cb.lodmaxdist[3][1] = g_lodrate4L[1] * g_projfar;//num:4levels lod:2 maxdist
		cb.lodmindist[3][2] = g_lodrate4L[1] * g_projfar;//num:4levels lod:3 mindist
		cb.lodmaxdist[3][2] = g_lodrate4L[2] * g_projfar;//num:4levels lod:3 maxdist
		cb.lodmindist[3][3] = g_lodrate4L[2] * g_projfar;//num:4levels lod:3 mindist
		cb.lodmaxdist[3][3] = g_lodrate4L[3] * g_projfar;//num:4levels lod:3 maxdist


		ChaVector3 lightpos;
		if (g_cameraShadow) {
			lightpos.SetParams(g_cameraShadow->GetPosition());
		}
		else {
			lightpos = g_camEye;
		}
		cb.shadowPos[0] = lightpos.x;
		cb.shadowPos[1] = lightpos.y;
		cb.shadowPos[2] = lightpos.z;
		cb.shadowPos[3] = 1.0f;
		cb.shadowparams1[0] = (float)cos(g_shadowmap_fov[g_shadowmap_slotno]);
		cb.shadowparams1[1] = g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno];
	
		ChaMatrix mWVP = chkMatWorld * m_matVP;
		GetFrustumPlanes(mWVP, &(cb.frustumPlanes[0][0]));
		GetFrustumCorners(mWVP, &(cb.frustumCorners[0][0]));

		Matrix shadowvp;
		shadowvp = g_cameraShadow->GetViewProjectionMatrix();
		ChaMatrix chashadowwvp = chkMatWorld * ChaMatrix(shadowvp);//2024/04/09 chkMatWorld * 
		GetFrustumPlanes(chashadowwvp, &(cb.shadowPlanes[0][0]));
		GetFrustumCorners(chashadowwvp, &(cb.shadowCorners[0][0]));

		//#######################
		//コンピュートシェーダで実行
		//#######################
		m_chkinview->ComputeChkInView(cb);



		int objnum = 0;
		int inviewnum = 0;
		int inshadownum = 0;
		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			if (curobj && (curobj->GetDispObj() || curobj->GetDispLine())) {


				//##################################
				//コンピュートシェーダによる計算結果を取得
				//##################################
				CSInView csresult = m_chkinview->GetResultOfChkInView(curobj);


				if (csresult.inview[0] == 1) {
					curobj->SetInView(true, refposindex);
					inviewnum++;//!!!!!
				}
				else {
					curobj->SetInView(false, refposindex);
				}
				if (csresult.inview[1] == 1) {
					curobj->SetInShadow(true, refposindex);
					inshadownum++;//!!!!!
				}
				else {
					curobj->SetInShadow(false, refposindex);
				}

				objnum++;
			}
		}

		if (inviewnum != 0) {
			SetInView(true, refposindex);//メッシュ１つでも視野内にある場合には　モデルとして視野内のマークをする

			ChaVector3 cam2model = ChaMatrixTraVec(m_matWorld) - g_camEye;
			double distcam2model = ChaVector3LengthDbl(&cam2model);
			double distthreshold = g_projfar * g_blendshapedist;
			if (distcam2model <= distthreshold) {
				SetInMorph(true);
			}
			else {
				SetInMorph(false);
			}
		}
		else {
			SetInView(false, refposindex);//全てのメッシュが視野外の場合　モデルとして視野外のマークをする

			SetInMorph(false);
		}
		if (inshadownum != 0) {
			SetInShadow(true, refposindex);
		}
		else {
			SetInShadow(false, refposindex);
		}
	}





	s_workingChkinView = false;
	return 0;
}


//int CModel::ChkInView(int refposindex)
//{
//
//	if (GetSkyFlag() || (wcsstr(GetFileName(), L".mqo") != 0)) {
//
//		//このアプリにおいては　mqoファイル(マニピュレータや地面格子)はクリッピングしない用途に使用しているので　常に描画するように
//
//		SetInView(true, refposindex);
//		SetInShadow(false, refposindex);
//
//		map<int, CMQOObject*>::iterator itr;
//		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
//			CMQOObject* curobj = itr->second;
//			if (curobj) {
//				curobj->SetInView(true, refposindex);
//			}
//		}
//		if (GetSkyFlag()) {
//			SetDistChkInView(500000.0f, refposindex);
//		}
//		else {
//			SetDistChkInView(0.01f, refposindex);//2024/03/25
//		}
//	}
//	else if (m_object.empty() || (GetFromBvhFlag())) {
//
//		//bvhにはメッシュが無いが　UpdateMatrixを実行するためにInViewである必要
//
//		SetInView(true, refposindex);
//		SetInShadow(false, refposindex);
//
//		map<int, CMQOObject*>::iterator itr;
//		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
//			CMQOObject* curobj = itr->second;
//			if (curobj) {
//				curobj->SetInView(true, refposindex);
//			}
//		}
//
//		SetDistChkInView(0.01f, refposindex);//2024/03/25
//	}
//	else {
//
//		////###########################################
//		////モデル全体のバウンダリーで　まずは粗く判定
//		////###########################################
//		// モデルには非スキンメッシュとスキンメッシュが混在している
//		// 非スキンメッシュの判定はm_matWorldで行い
//		// スキンメッシュの判定は　hipsworld * m_matWorldで行う
//		// まとめて１回では判定できない
//		// よってコメントアウト
//		//m_frustum.ChkInView(m_bound, m_matWorld);
//		//if (m_frustum.GetVisible() == false) {
//		//	SetInView(false);//!!!!!!!!!!!!!!!!!!
//		//	return 0;
//		//}
//
//
//		//###############################################################
//		//モデル全体が視野内に掛かっている場合　メッシュごとに判定をする
//		// 
//		// CMQOObject::ChkInView()内で
//		// スキンメッシュ(CPolyMesh4)の場合
//		// clusterの最初のボーンを親方向へさかのぼりhipsを探す
//		// hipsのworldmatをm_matWorldに掛けて判定することにより
//		// モーションでの全体移動に対応
//		//###############################################################
//
//		if (GetSkyFlag() == false) {
//			int objnum = 0;
//			int inviewnum = 0;
//			int inshadownum = 0;
//			map<int, CMQOObject*>::iterator itr;
//			for (itr = m_object.begin(); itr != m_object.end(); itr++) {
//				CMQOObject* curobj = itr->second;
//				if (curobj && (curobj->GetDispObj() || curobj->GetDispLine())) {
//					//curobj->ChkInView(m_matWorld, m_matVP);
//					curobj->ChkInView(m_matWorld, m_matVP, refposindex);
//					if (curobj->GetVisible(refposindex)) {
//						inviewnum++;
//					}
//					if (curobj->GetInShadow(refposindex)) {
//						inshadownum++;
//					}
//					objnum++;
//				}
//			}
//
//			if (inviewnum != 0) {
//				SetInView(true, refposindex);//メッシュ１つでも視野内にある場合には　モデルとして視野内のマークをする
//			}
//			else {
//				SetInView(false, refposindex);//全てのメッシュが視野外の場合　モデルとして視野外のマークをする
//			}
//			if (inshadownum != 0) {
//				SetInShadow(true, refposindex);
//			}
//			else {
//				SetInShadow(false, refposindex);
//			}
//		}
//		else {
//			//skyは視野内の一番遠く
//
//			SetInView(true, refposindex);
//			SetInShadow(false, refposindex);
//
//			map<int, CMQOObject*>::iterator itr;
//			for (itr = m_object.begin(); itr != m_object.end(); itr++) {
//				CMQOObject* curobj = itr->second;
//				if (curobj) {
//					curobj->SetInView(true, refposindex);
//				}
//			}
//			SetDistChkInView(0.0f, refposindex);//2024/03/25 skyは不透明描画時にソートする際に一番最初になるように
//		}
//	}
//
//
//	return 0;
//}

int CModel::GetCurrentMotID()
{
	MOTINFO* curmi = GetCurMotInfoPtr();
	if (curmi) {
		return curmi->motid;
	}
	else {
		return 0;
	}
}
double CModel::GetCurrentMotLeng()
{
	MOTINFO* curmi = GetCurMotInfoPtr();
	if (curmi) {
		return curmi->frameleng;
	}
	else {
		return 0.0;
	}
}
double CModel::GetCurrentMaxFrame()
{
	//return (motleng - 1.0)

	double frameleng;
	frameleng = GetCurrentMotLeng();
	if (frameleng >= 1.0) {
		return (frameleng - 1.0);
	}
	else {
		return 0.0;
	}
}

double CModel::GetCurrentFrame()
{
	MOTINFO* curmi = GetCurMotInfoPtr();
	if (curmi) {
		return curmi->curframe;
	}
	else {
		return 1.0;
	}
}

bool CModel::ExistCurrentMotion()
{
	MOTINFO* curmi = GetCurMotInfoPtr();
	if (curmi) {
		int curmotid;
		double frameleng;
		curmotid = curmi->motid;
		frameleng = curmi->frameleng;
		if ((curmotid > 0) && (frameleng > 0.0)) {
			return true;
		}
		else {
			_ASSERT(0);
			return false;
		}
	}
	else {
		return false;
	}
}

int CModel::GetCurrentMotName(char* dstname, int dstleng)
{
	if (!dstname || (dstleng <= 0) || (dstleng >= 10000)) {
		_ASSERT(0);
		return 1;
	}

	MOTINFO* curmi = GetCurMotInfoPtr();
	if (curmi) {
		strcpy_s(dstname, dstleng, curmi->motname);
		return 0;
	}
	else {

		return 1;
	}
}

void CModel::SetRenderSlotFrame(double srcframe)
{
	//RenderBoneMark()で　表示に合わせた剛体位置を表示するために使用
	if (m_curmotinfo) {
		m_curmotinfo->befframe = max(0.0, min((m_curmotinfo->frameleng - 1.0), srcframe));;
	}
}
double CModel::GetRenderSlotFrame()
{
	if (m_curmotinfo) {
		return m_curmotinfo->befframe;
	}
	else {
		return 1.0f;
	}
}

double CModel::GetOpeFrame(bool calcslotflag)
{
	//calcslotflagに応じて　GetCurrentFrame() or GetRenderSlotFrame()を返す

	//if (calcslotflag == false) {
	if (calcslotflag == true) {//2024/03/20
		return GetCurrentFrame();
	}
	else {
		return GetRenderSlotFrame();
	}
}

int CModel::SetLODNum()
{
	//#######################################################################
	//LODが2Levelsなのか3Levelsなのか4Levelsを調べて　LODGroupにLODNumをセットする
	//#######################################################################

	map<int, CMQOObject*>::iterator itr;
	for (itr = m_object.begin(); itr != m_object.end(); itr++) {
		CMQOObject* curobj = itr->second;
		if (curobj) {
			char objname[256] = { 0 };
			strcpy_s(objname, 256, curobj->GetName());
			char headname[256] = { 0 };

			const char* plod1 = strstr(objname, "LOD1");
			const char* plod2 = strstr(objname, "LOD2");
			const char* plod3 = strstr(objname, "LOD3");//2024/04/22
			if ((plod1 != 0) && ((plod1 - objname) >= 1)) {
				size_t headlen = (size_t)(plod1 - objname - 1);
				strncpy_s(headname, 256, objname, headlen);
				*(headname + headlen) = 0;

				SetLODNum((const char*)headname, 2);
			}
			else if ((plod2 != 0) && ((plod2 - objname) >= 1)) {
				size_t headlen = (size_t)(plod2 - objname - 1);
				strncpy_s(headname, 256, objname, headlen);
				*(headname + headlen) = 0;

				SetLODNum((const char*)headname, 3);
			}
			else if ((plod3 != 0) && ((plod3 - objname) >= 1)) {
				size_t headlen = (size_t)(plod3 - objname - 1);
				strncpy_s(headname, 256, objname, headlen);
				*(headname + headlen) = 0;

				SetLODNum((const char*)headname, 4);//2024/04/22
			}
		}
	}
	return 0;
}

int CModel::SetLODNum(const char* srcheadname, int srcnum)
{
	map<int, CMQOObject*>::iterator itr;
	for (itr = m_object.begin(); itr != m_object.end(); itr++) {
		CMQOObject* curobj = itr->second;
		if (curobj) {
			if (strstr(curobj->GetName(), srcheadname) != 0) {
				curobj->SetLODNum(srcnum);//srcnumがm_lodnumより大きい場合にだけセットされる
			}
		}
	}

	return 0;
}

int CModel::CreateObjno2DigElem()
{
	m_objno2digelem.clear();

	if (m_pscene) {
		//#########
		//fbx file
		//#########
		int objno = 0;
		int depth = 0;
		CreateObjno2DigElemReq(m_pscene->GetRootNode(), &objno, depth);
	}
	else {
		//#########
		//mqo file
		//#########
		int objno = 0;
		int depth = 0;
		map<int,CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;

			DISPGROUPELEM digelem;
			digelem.Init();
			digelem.objno = objno;
			digelem.depth = depth;
			digelem.groupno = 1;// default value : groupno = groupindex + 1
			digelem.pNode = 0;
			digelem.mqoobject = curobj;

			m_objno2digelem[objno] = digelem;
			objno++;
		}
	}

	return 0;

}
void CModel::CreateObjno2DigElemReq(FbxNode* pNode, int* pobjno, int depth)
{
	if (pNode) {

		bool opeflag = false;

		FbxNodeAttribute* pAttrib = pNode->GetNodeAttribute();
		if (pAttrib) {
			FbxNodeAttribute::EType type = (FbxNodeAttribute::EType)(pAttrib->GetAttributeType());

			if (type == FbxNodeAttribute::eMesh) {
				opeflag = true;
			}
		}
		else {
			char nodename[256] = { 0 };
			strcpy_s(nodename, 256, pNode->GetName());
			if ((strstr(nodename, "J_Sec_") != 0) && (strstr(nodename, "_end") != 0)) {
				//VRoid VRM1.0のendjointはeNullを含むが　表示オブジェクトの親のeNullにはならないと仮定してopeflag = false;
			}
			else {
				opeflag = true;
			}
		}

		if (opeflag) {
			DISPGROUPELEM digelem;
			digelem.Init();
			digelem.objno = *pobjno;
			digelem.depth = depth;
			digelem.groupno = 1;// default value : groupno = groupindex + 1
			digelem.pNode = pNode;
			
			map<FbxNode*, CMQOObject*>::iterator itrmqoobj;
			itrmqoobj = m_node2mqoobj.find(pNode);
			if (itrmqoobj != m_node2mqoobj.end()) {
				digelem.mqoobject = itrmqoobj->second;
			}
			else {
				digelem.mqoobject = 0;
			}

			m_objno2digelem[*pobjno] = digelem;

			(*pobjno)++;
		}

		int childNodeNum;
		childNodeNum = pNode->GetChildCount();
		int childdepth = depth + 1;
		for (int i = 0; i < childNodeNum; i++)
		{
			FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
			if (pChild) {
				CreateObjno2DigElemReq(pChild, pobjno, childdepth);
			}
		}
	}
}

int CModel::SetBlendShapeGUI(std::vector<CBlendShapeElem>& blendshapeelem)
{
	blendshapeelem.clear();

	map<int, CMQOObject*>::iterator itrobj;
	for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
		CMQOObject* curobj = itrobj->second;
		_ASSERT(curobj);
		if (!(curobj->EmptyShape())) {
			CBlendShapeElem addelem;
			addelem.Init();

			int channelnum = curobj->GetShapeNameNum();
			int channelindex;
			for (channelindex = 0; channelindex < channelnum; channelindex++) {
				int result0 = addelem.SetBlendShape(this, curobj, channelindex);
				if (result0 != 0) {
					_ASSERT(0);
					return 1;
				}
				blendshapeelem.push_back(addelem);
			}
		}
	}

	return 0;
}

int CModel::SetDispGroupGUI(std::vector<OrgWinGUI::OWP_CheckBoxA*>& checkboxvec,
	std::vector<CMQOObject*>& mqoobjvec)
{
	checkboxvec.clear();
	mqoobjvec.clear();

	int objnum = (int)m_objno2digelem.size();
	int objno;
	for (objno = 0; objno < objnum; objno++) {
		DISPGROUPELEM digelem = m_objno2digelem[objno];
	
		if (digelem.pNode) {
			char meshnameA[256] = { 0 };
			strcpy_s(meshnameA, 256, digelem.pNode->GetName());
			WCHAR meshnameW[256] = { 0L };
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, meshnameA, 256, meshnameW, 256);

			WCHAR labelnameW[1024] = { 0L };
			int depthindex;
			for (depthindex = 0; depthindex < digelem.depth; depthindex++) {
				wcscat_s(labelnameW, 1024, L"  ");
			}
			wcscat_s(labelnameW, 1024, meshnameW);

			OWP_CheckBoxA* newchk = new OWP_CheckBoxA(labelnameW, 0);
			if (!newchk) {
				_ASSERT(0);
				return 1;
			}

			checkboxvec.push_back(newchk);
			mqoobjvec.push_back(digelem.mqoobject);
		}
	}


	return 0;
}

int CModel::MakeDispGroupForRender()
{
	int groupindex;
	for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
		m_dispgroup[groupindex].clear();
	}


	for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
		int objnum = (int)m_objno2digelem.size();
		int objno;
		for (objno = 0; objno < objnum; objno++) {
			DISPGROUPELEM digelem = m_objno2digelem[objno];

			if (digelem.mqoobject && (digelem.groupno == (groupindex + 1))) {//2024/01/20 eNullは除外
				m_dispgroup[groupindex].push_back(digelem);
			}
		}
	}

	return 0;
}

int CModel::MakeLaterMaterial()
{
	map<int, CMQOObject*>::iterator itrobj;
	for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
		CMQOObject* curobj = itrobj->second;
		if (curobj) {
			int result = curobj->MakeLaterMaterial(m_latertransparent);
			_ASSERT(result == 0);
		}
	}

	return 0;
}

int CModel::SetLaterTransparentVec(std::vector<std::wstring> srclatervec)
{
	//###########
	//丸ごと設定
	//###########
	m_latertransparent.clear();

	int newlaternum = (int)srclatervec.size();
	int laterno;
	for (laterno = 0; laterno < newlaternum; laterno++) {
		WCHAR wcname[512] = { 0L };
		wcscpy_s(wcname, 512, srclatervec[laterno].c_str());
		char mbname[512] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, wcname, 512, mbname, 512, NULL, NULL);
		m_latertransparent.push_back(mbname);
	}

	MakeLaterMaterial();

	return 0;
}


//###############################################################################################
//ModifyEuler360 と　CalcLocalEulXYZとでは　結果が違った (CalcLocalEulXYZの方が大分きれい)
// PostIKの後処理としては　内部でCalcLocalEulXYZを呼び出しているCalcBoneEulReq()を使用することに
//###############################################################################################
//void CModel::ModifyEuler360Req(int limitdegflag, CBone* srcbone, int srcmotid, double startframe, double endframe)
//{
//	if (srcbone) {
//
//		if (srcbone->IsSkeleton()) {
//			double curframe;
//			for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
//				if (curframe >= 1.01) {
//					double befframe;
//					befframe = curframe - 1.0;
//
//					//ChaMatrix befwm = srcbone->GetWorldMat(limitdegflag, srcmotid, curframe, 0);
//					//ChaVector3 befeul = srcbone->GetLocalEul(limitdegflag, srcmotid, befframe, 0);
//					//ChaVector3 cureul = srcbone->GetLocalEul(limitdegflag, srcmotid, curframe, 0);
//					//ChaModifyEuler360(&cureul, &befeul, 0, 180.0f, 180.0f, 180.0f);
//					//ChaModifyEuler360(&cureul, &befeul, 0, 91.0f, 181.0f, 181.0f);
//					//srcbone->SetWorldMatFromEul(limitdegflag, 0, 0, befwm, cureul, srcmotid, curframe, 0);
//					//srcbone->SetLocalEul(limitdegflag, srcmotid, curframe, cureul, 0);
//
//					ChaVector3 cureul;
//					cureul = srcbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, curframe, BEFEUL_BEFFRAME);
//					srcbone->SetLocalEul(limitdegflag, srcmotid, curframe, cureul, 0);
//				}
//			}
//		}
//
//		if (srcbone->GetChild(false)) {
//			ModifyEuler360Req(limitdegflag, srcbone->GetChild(false), srcmotid, startframe, endframe);
//		}
//		if (srcbone->GetBrother(false)) {
//			ModifyEuler360Req(limitdegflag, srcbone->GetBrother(false), srcmotid, startframe, endframe);
//		}
//	}
//
//}

int CModel::Retarget(CModel* srcbvhmodel, ChaMatrix smatView, ChaMatrix smatProj,
	std::map<CBone*, CBone*>& sconvbonemap,
	int (*srcAddMotionFunc)(const WCHAR* wfilename, double srcmotleng))
{

	//retargetは　unlimitedに対して行い　unlimitedにセットする
	bool limitdegflag = false;

	if (!srcbvhmodel || !srcAddMotionFunc) {
		return 0;
	}

	//g_underRetargetFlag = true;//!!!!!!!!!!!!
	SetUnderRetarget(true);

	//MOTINFO* bvhmi = srcbvhmodel->GetMotInfoBegin()->second;
	//if (!bvhmi) {
	//	::MessageBox(NULL, L"motion of bvh is not found error.", L"error!!!", MB_OK);
	//	g_underRetargetFlag = false;
	//	return 1;
	//}

	//############################################################
	//2023/08/14 : bvh側の全モーションに関してリターゲットループ
	//############################################################
	int bvhminum;
	int bvhmiindex;
	bvhminum = srcbvhmodel->GetMotInfoSize();
	for (bvhmiindex = 0; bvhmiindex < bvhminum; bvhmiindex++) {
		MOTINFO bvhmi = srcbvhmodel->GetMotInfoByIndex(bvhmiindex);
		if ((bvhmi.motid <= 0) || (bvhmi.frameleng <= 0.0) || bvhmi.cameramotion) {//2024/06/21 CameraMotionは除外
			//::MessageBox(NULL, L"motion of bvh is not found error.", L"error!!!", MB_OK);
			//g_underRetargetFlag = false;
			//return 1;
			int dbgflag1 = 1;
			continue;
		}

		double motleng = bvhmi.frameleng;//2022/11/01
		//double motleng = bvhmi->frameleng - 1.0;//2021/10/13
		int bvhmotid = bvhmi.motid;//motion側のmotid
		srcbvhmodel->SetCurrentMotion(bvhmotid);


		//AddMotion内部でSetCurrentMotionされる
		(srcAddMotionFunc)(0, motleng);//model側のaddmotion　model側のSetCurrentMotionもされる

		//(srcInitCurMotionFunc)(0, 0);//CModel::AddMotionで初期化することにしたのでコメントアウト　2022/08/28


		CBone* modelbone = 0;
		int modelmotid = 0;
		if (ExistCurrentMotion()) {
			modelmotid = GetCurrentMotID();
			CBone* modeltopbone = GetTopBone();
			CBone* modelhipsbone = 0;
			if (!modeltopbone) {
				::MessageBox(NULL, L"modelside bone is not found error.", L"error!!!", MB_OK);
				//g_underRetargetFlag = false;
				SetUnderRetarget(false);
				return 1;
			}
			else {
				GetHipsBoneReq(modeltopbone, &modelhipsbone);
				if (modelhipsbone) {
					modelbone = modelhipsbone;
				}
				else {
					modelbone = modeltopbone;
				}
			}
			//modelbone = srcmodel->GetTopBone();
		}
		else {
			::MessageBox(NULL, L"modelside motion is not found error.", L"error!!!", MB_OK);
			SetUnderRetarget(false);
			return 1;
		}


		CBone* bvhtopbone = 0;
		CBone* bvhhipsbone = 0;
		CBone* befbvhbone = 0;
		bvhtopbone = srcbvhmodel->GetTopBone();
		if (bvhtopbone) {
			srcbvhmodel->GetHipsBoneReq(bvhtopbone, &bvhhipsbone);
			if (bvhhipsbone) {
				befbvhbone = bvhhipsbone;
			}
			else {
				befbvhbone = bvhtopbone;
			}
		}
		else {
			::MessageBox(NULL, L"bvhside motion is not found error.", L"error!!!", MB_OK);
			SetUnderRetarget(false);
			return 1;
		}


		HINFO bvhhi;
		bvhhi.minh = 1e7;
		bvhhi.maxh = -1e7;
		bvhhi.height = 0.0f;
		srcbvhmodel->SetFirstFrameBonePos(&bvhhi, befbvhbone);//hips指定

		HINFO modelhi;
		modelhi.minh = 1e7;
		modelhi.maxh = -1e7;
		modelhi.height = 0.0f;
		SetFirstFrameBonePos(&modelhi, modelbone);//hips指定

		float hrate;
		if (bvhhi.height != 0.0f) {
			hrate = modelhi.height / bvhhi.height;
		}
		else {
			//hrate = 0.0f;
			hrate = 1.0f;
			_ASSERT(0);
		}

		//2023/02/08
		if (fabs(hrate) <= 0.0001f) {
			hrate = 1.0f;
		}

		//2024/06/22_2
		//修正前のver1.0.0.24においてもsrcとdstの両方を視野内に入れてマルチスレッドリターゲットしたところ正常だったことに着目
		//リターゲットのsrcとdstの "両方に" 強制的に視野内フラグをつけたところマルチスレッドでも問題は出なかった
		//視野外の場合にいくつかの計算がスキップされることが原因だった
		//
		//2024/06/22 リターゲットのマルチスレッド計算で問題が生じた
		//(AddMotionPointが絡むと難しい)
		//一度シングルスレッドに戻す
		//
		//////リターゲットマルチスレッド計算 2023/10/23
		SetRetargetFrame(1.0, RoundingTime(motleng - 1.0));
		if (m_RetargetThreads) {
			int updatecount;
			for (updatecount = 0; updatecount < RETARGET_THREADSNUM; updatecount++) {
				CThreadingRetarget* curupdate = m_RetargetThreads + updatecount;
		
				curupdate->RetargetReqOne(limitdegflag, this, srcbvhmodel, modelbone,
					bvhtopbone, hrate, sconvbonemap);
			}
			WaitRetargetFinished();
		}


		//double frame;
		//////for (frame = 0.0; frame < motleng; frame += 1.0) {
		//for (frame = 1.0; frame < motleng; frame += 1.0) {//2023/03/27 : 0フレームはInitMPの姿勢のままにする
		//	if (modelbone) {
		//		if (bvhtopbone) {
		//			ChaCalcFunc chacalcfunc;
		//			chacalcfunc.RetargetReq(this, srcbvhmodel, modelbone, frame, bvhtopbone, hrate, sconvbonemap);
		//		}
		//	}
		//}


		//###############################
		// マルチスレッドの後処理
		//前フレーム考慮のオイラー角計算
		//###############################
		SetUnderRetarget(false);//!!!!!!!!!!!!
		CalcBoneEul(limitdegflag, modelmotid);
		SetUnderRetarget(true);//!!!!!!!!!!!!
		

		ChaMatrix tmpwm = GetWorldMat();
		UpdateMatrix(limitdegflag, &tmpwm, &smatView, &smatProj, true, 0);


	}


	SetUnderRetarget(false);//!!!!!!!!!!!!

	//if (!g_retargetbatchflag) {
	if (InterlockedAdd(&g_retargetbatchflag, 0) == 0) {
		::MessageBox(NULL, L"Finish of convertion.", L"check!!!", MB_OK);
	}

	return 0;

}

CMQOObject* CModel::GetMQOObjectByName(const char* findpattern) {
	std::map<int, CMQOObject*>::iterator itrobj;
	for (itrobj = m_object.begin(); itrobj != m_object.end(); itrobj++) {
		CMQOObject* curobj = itrobj->second;
		if (curobj) {
			if (strcmp(curobj->GetName(), findpattern) == 0) {
				return curobj;
			}
		}
	}
	return 0;
};

void CModel::SetBoneMatrixIndexReq(CBone* srcbone, int* pcount)
{
	if (srcbone) {
		
		if (srcbone->IsSkeleton()) {
			srcbone->SetMatrixIndex(*pcount);
			(*pcount)++;
		}

		if (srcbone->GetChild(false)) {
			SetBoneMatrixIndexReq(srcbone->GetChild(false), pcount);
		}
		if (srcbone->GetBrother(false)) {
			SetBoneMatrixIndexReq(srcbone->GetBrother(false), pcount);
		}
	}
}

int CModel::GetMQOMaterialSize() {
	return m_materialbank.GetSize();
};
CMQOMaterial* CModel::GetMQOMaterial(int srcmatno) {//indexではなくmaterialno
	int materialnum = GetMQOMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* chkmat = m_materialbank.Get(matindex);
		if (chkmat && (chkmat->GetMaterialNo() == srcmatno)) {
			return chkmat;
		}
	}
	return 0;
};
int CModel::ExistMQOMaterial(CMQOMaterial* srcmat) {
	int materialnum = GetMQOMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* chkmat = m_materialbank.Get(matindex);
		if (chkmat && (chkmat == srcmat)) {
			return chkmat->GetMaterialNo();
		}
	}
	return -1;
};

void CModel::SetMQOMaterial(const char* srcname, CMQOMaterial* srcmat) {
	if (srcname && srcname[0] && srcmat) {
		m_materialbank.Regist(srcname, srcmat);
	}
	else {
		_ASSERT(0);
	}
};

CMQOMaterial* CModel::GetMQOMaterialByName(std::string srcname) {
	return m_materialbank.Get(srcname.c_str());
};
CMQOMaterial* CModel::GetMQOMaterialByIndex(int srcmatindex)
{
	return m_materialbank.Get(srcmatindex);
}
CMQOMaterial* CModel::GetMQOMaterialByMaterialNo(int srcmatno)
{
	return GetMQOMaterial(srcmatno);
}


void CModel::ResetUpdateFl4x4Flag()//materialのリセットも行う
{

	m_updatefl4x4flag = false;

	int materialnum = GetMQOMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* resetmqomat = m_materialbank.Get(matindex);
		if (resetmqomat) {
			resetmqomat->ResetUpdateFl4x4Flag();
			resetmqomat->ResetUpdateLightsFlag();
		}
	}
}

void CModel::ResetBoneMarkInstanceScale()
{
	CBone::ResetColDispInstancingParams();
	ResetInstancingParams();
	//ResetDispObjScale();//<--- 2024/06/17 RigidMarkとは関係ない　1000を越える個数のメッシュを持つモデルに対して呼ぶとかなり重い　コメントアウト
}
void CModel::ResetRefPosMarkInstanceScale()
{
	CBone::ResetRefPosMarkInstancingParams();
	ResetInstancingParams();
	ResetDispObjScale();
}

int CModel::SetRefPosFl4x4ToDispObj(int refposindex)
{
	int materialnum = GetMQOMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* curmqomat = m_materialbank.Get(matindex);
		if (curmqomat) {
			curmqomat->SetRefPosFl4x4(this, refposindex);
		}
	}
	return 0;
}

int CModel::SetRefPosFlag(bool srcflag)
{
	m_refposflag = srcflag;

	int materialnum = GetMQOMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* curmqomat = m_materialbank.Get(matindex);
		if (curmqomat) {
			curmqomat->SetRefPosFlag(srcflag);
		}
	}
	return 0;
}

int CModel::RemakeHSVToonTexture(CMQOMaterial* srcmqomat)
{
	if (srcmqomat) {
		srcmqomat->RemakeDiffuseTexture();
	}
	else {
		int materialnum = GetMQOMaterialSize();
		int materialindex2;
		for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
			CMQOMaterial* setmqomat = GetMQOMaterialByIndex(materialindex2);
			if (setmqomat) {
				setmqomat->RemakeDiffuseTexture();
			}
		}
	}

	return 0;
}

void CModel::SetDistChkInView(float srcval, int refposindex)
{
	if ((refposindex < 0) || (refposindex >= REFPOSMAXNUM)) {
		return;
	}
	else {
		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			if (curobj) {
				curobj->SetDistFromCamera(srcval, refposindex);
			}
		}
	}

}


int CModel::CreateChkInView()
{
	m_chkinview = new CSChkInView();
	if (!m_chkinview) {
		_ASSERT(0);
		return 1;
	}

	if (GetSkyFlag() || (wcsstr(GetFileName(), L".mqo") != 0)) {

		//このアプリにおいては　mqoファイル(マニピュレータや地面格子)はクリッピングしない用途に使用しているので　常に描画するように

		//map<int, CMQOObject*>::iterator itr;
		//for (itr = m_object.begin(); itr != m_object.end(); itr++) {
		//	CMQOObject* curobj = itr->second;
		//	if (curobj) {
		//		MODELBOUND mb;
		//		mb.Init();
		//		int forceinview = 1;
		//		int forceinshadow = 0;
		//		m_chkinview->AddBoundary(curobj, mb, 1, forceinview, forceinshadow);
		//	}
		//}
		//CallF(m_chkinview->CreateDispObj(m_pdev), return 1);
	}
	else if (m_object.empty() || (GetFromBvhFlag())) {

		//bvhにはメッシュが無いが　UpdateMatrixを実行するためにInViewである必要


		//2024/04/07 メッシュが無い場合はBoundaryも無いので実行用バッファは作成しない


		//map<int, CMQOObject*>::iterator itr;
		//for (itr = m_object.begin(); itr != m_object.end(); itr++) {
		//	CMQOObject* curobj = itr->second;
		//	if (curobj) {
		//		MODELBOUND mb;
		//		mb.Init();
		//		int forceinview = 1;
		//		int forceinshadow = 0;
		//		m_chkinview->AddBoundary(curobj, mb, 1, forceinview, forceinshadow);
		//	}
		//}
		//CallF(m_chkinview->CreateDispObj(m_pdev), return 1);
	}
	else {

		////###########################################
		////モデル全体のバウンダリーで　まずは粗く判定
		////###########################################
		// モデルには非スキンメッシュとスキンメッシュが混在している
		// 非スキンメッシュの判定はm_matWorldで行い
		// スキンメッシュの判定は　hipsworld * m_matWorldで行う
		// まとめて１回では判定できない
		// よってコメントアウト
		//m_frustum.ChkInView(m_bound, m_matWorld);
		//if (m_frustum.GetVisible() == false) {
		//	SetInView(false);//!!!!!!!!!!!!!!!!!!
		//	return 0;
		//}


		//###############################################################
		//モデル全体が視野内に掛かっている場合　メッシュごとに判定をする
		// 
		// CMQOObject::ChkInView()内で
		// スキンメッシュ(CPolyMesh4)の場合
		// clusterの最初のボーンを親方向へさかのぼりhipsを探す
		// hipsのworldmatをm_matWorldに掛けて判定することにより
		// モーションでの全体移動に対応
		//###############################################################

		map<int, CMQOObject*>::iterator itr;
		for (itr = m_object.begin(); itr != m_object.end(); itr++) {
			CMQOObject* curobj = itr->second;
			//if (curobj && (curobj->GetDispObj() || curobj->GetDispLine())) {
			if (curobj && curobj->GetDispObj()) {
				MODELBOUND mb;
				mb.Init();
				mb = curobj->GetBound();
				int forceinview = -1;
				int forceinshadow = -1;
				if (!curobj->GetPm3() && !curobj->GetPm4()) {
					//extlineはInViewでshadow無し
					forceinview = 1;
					forceinshadow = 0;
				}
				m_chkinview->AddBoundary(curobj, mb, curobj->GetLODNum(), forceinview, forceinshadow);
			}
		}
		CallF(m_chkinview->CreateDispObj(m_pdev), return 1);
	}

	return 0;
}

ChaMatrix CModel::GetMatrixForChkInView(ChaMatrix matWorld)
{
	ChaMatrix chkmatworld;
	chkmatworld.SetIdentity();

	if (GetBoneForMotionSize() == 0) {
		chkmatworld = matWorld;
	}
	else {
		CBone* hipsbone = nullptr;
		GetHipsBoneReq(GetTopBone(), &hipsbone);
		if (hipsbone) {
			int currentmotid = GetCurrentMotID();
			if (currentmotid > 0) {
				double roundingframe = (double)((int)GetCurrentFrame());
				ChaMatrix hipsworld = hipsbone->GetWorldMat(g_limitdegflag, currentmotid, roundingframe, 0);
				chkmatworld = hipsworld * matWorld;
			}
			else {
				chkmatworld = matWorld;
			}
		}
		else {
			chkmatworld = matWorld;
		}
	}

	return chkmatworld;
}


void CModel::GetFrustumPlanes(ChaMatrix vp, float* planes)
{
	if (!planes) {
		_ASSERT(0);
		return;
	}

	//vp.transpose();

	//ChaVector4 vright.SetParams(vp.data[MATI_11], vp.data[MATI_12], vp.data[MATI_13], vp.data[MATI_14]);
	//ChaVector4 vup.SetParams(vp.data[MATI_21], vp.data[MATI_22], vp.data[MATI_23], vp.data[MATI_24]);
	//ChaVector4 vforward.SetParams(vp.data[MATI_31], vp.data[MATI_32], vp.data[MATI_33], vp.data[MATI_34]);
	//ChaVector4 vpos.SetParams(vp.data[MATI_41], vp.data[MATI_42], vp.data[MATI_43], vp.data[MATI_44]);

	ChaVector4 vright;
	vright.SetParams(vp.data[MATI_11], vp.data[MATI_21], vp.data[MATI_31], vp.data[MATI_41]);
	ChaVector4 vup;
	vup.SetParams(vp.data[MATI_12], vp.data[MATI_22], vp.data[MATI_32], vp.data[MATI_42]);
	ChaVector4 vforward;
	vforward.SetParams(vp.data[MATI_13], vp.data[MATI_23], vp.data[MATI_33], vp.data[MATI_43]);
	ChaVector4 vpos;
	vpos.SetParams(vp.data[MATI_14], vp.data[MATI_24], vp.data[MATI_34], vp.data[MATI_44]);


	ChaVector4 planes0 = vpos + vright;		// left
	ChaVector4 planes1 = vpos - vright;		// right
	ChaVector4 planes2 = vpos + vup;			// bottom
	ChaVector4 planes3 = vpos - vup;			// top
	ChaVector4 planes4 = vforward;			// near
	ChaVector4 planes5 = vpos - vforward;	// far

	*(planes) = planes0.x;
	*(planes + 1) = planes0.y;
	*(planes + 2) = planes0.z;
	*(planes + 3) = planes0.w;

	*(planes + 4) = planes1.x;
	*(planes + 4 + 1) = planes1.y;
	*(planes + 4 + 2) = planes1.z;
	*(planes + 4 + 3) = planes1.w;

	*(planes + 8) = planes2.x;
	*(planes + 8 + 1) = planes2.y;
	*(planes + 8 + 2) = planes2.z;
	*(planes + 8 + 3) = planes2.w;

	*(planes + 12) = planes3.x;
	*(planes + 12 + 1) = planes3.y;
	*(planes + 12 + 2) = planes3.z;
	*(planes + 12 + 3) = planes3.w;

	*(planes + 16) = planes4.x;
	*(planes + 16 + 1) = planes4.y;
	*(planes + 16 + 2) = planes4.z;
	*(planes + 16 + 3) = planes4.w;

	*(planes + 20) = planes5.x;
	*(planes + 20 + 1) = planes5.y;
	*(planes + 20 + 2) = planes5.z;
	*(planes + 20 + 3) = planes5.w;

}

void CModel::GetFrustumCorners(ChaMatrix vp, float* points)
{
	ChaVector3 corners[] = {
		ChaVector3(-1.0f, -1.0f, -1.0f),
		ChaVector3(1.0f, -1.0f, -1.0f),
		ChaVector3(1.0f,  1.0f, -1.0f),
		ChaVector3(-1.0f,  1.0f, -1.0f),
		ChaVector3(-1.0f, -1.0f,  1.0f),
		ChaVector3(1.0f, -1.0f,  1.0f),
		ChaVector3(1.0f,  1.0f,  1.0f),
		ChaVector3(-1.0f,  1.0f,  1.0f) };

	ChaMatrix invVP = ChaMatrixInv(vp);

	for (int i = 0; i < 8; i++)
	{
		ChaVector3 q;
		ChaVector3TransformCoord(&q, &(corners[i]), &invVP);
		*(points + 4 * i) = q.x;
		*(points + 4 * i + 1) = q.y;
		*(points + 4 * i + 2) = q.z;
		*(points + 4 * i + 3) = 1.0f;
	}
}

void CModel::ResetBtMovableReq(CBone* srcbone)
{
	if (srcbone) {

		srcbone->SetBtMovable(true);

		if (srcbone->GetChild(false)) {
			ResetBtMovableReq(srcbone->GetChild(false));
		}
		if (srcbone->GetBrother(false)) {
			ResetBtMovableReq(srcbone->GetBrother(false));
		}
	}
}

int CModel::GetChildCameraBoneAndNode(CBone* enullbone, CBone** ppbone, FbxNode** ppnode)
{
	if (!enullbone || !ppbone || !ppnode) {
		_ASSERT(0);
		return 1;
	}

	*ppbone = nullptr;
	*ppnode = nullptr;


	FbxNode* enullnode = enullbone->GetFbxNodeOnLoad();
	if (!enullnode) {
		return 1;
	}

	int childNodeNum;
	childNodeNum = enullnode->GetChildCount();
	for (int i = 0; i < childNodeNum; i++)
	{
		FbxNode* pChild = enullnode->GetChild(i);  // 子ノードを取得
		if (pChild) {
			FbxNodeAttribute* pAttrib2 = pChild->GetNodeAttribute();
			if (pAttrib2) {
				FbxNodeAttribute::EType type2 = (FbxNodeAttribute::EType)(pAttrib2->GetAttributeType());
				if (type2 == FbxNodeAttribute::eCamera) {
					*ppnode = pChild;
					CBone* camerabone = FindBoneByNode(pChild);
					if (camerabone) {
						*ppbone = camerabone;
						return 0;//!!!!!!!!!!!!!!
					}
					else {
						return 1;
					}
				}
			}
		}
	}

	return 1;//子供のcameraがみつからなかった場合は　return 1

}


const char* CModel::GetBoneName(int srcboneno) {
	CBone* pbone = GetBoneByID(srcboneno);
	if (pbone) {
		return pbone->GetBoneName();
	}
	else {
		return nullptr;
	}
}
const WCHAR* CModel::GetWBoneName(int srcboneno) {
	CBone* pbone = GetBoneByID(srcboneno);
	if (pbone) {
		return pbone->GetWBoneName();
	}
	else {
		return nullptr;
	}
}
