#ifndef MODELH
#define MODELH


//class ID3D12Device;

//#include <d3dx9.h>
#include <wchar.h>
#include <string>
#include <map>

#include <Coef.h>
#include <OrgWindow.h>

#include <ChaVecCalc.h>
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

#include <fbxsdk.h>


#include <UndoMotion.h>
#include <CameraFbx.h>

//#include <mqoobject.h>


#include "../../MiniEngine/MiniEngine.h"
#include "../../MiniEngine/RenderContext.h"
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
#include "../../MiniEngine/InstancedSprite.h"


//using namespace std;

class CMQOMaterial;
class CMQOObject;
class CMQOFace;
class CBone;
class CMySprite;
class CMotionPoint;
class CQuaternion;
class CBtObject;
class CRigidElem;
class CEditRange;
class CThreadingLoadFbx;
class CThreadingUpdateMatrix;
class CThreadingPostIK;
class CThreadingCalcEul;
class CThreadingFKTra;
class CThreadingCopyW2LW;
class CThreadingRetarget;
class CThreadingInitMp;
class CNodeOnLoad;

typedef struct funcmpparams
{
	int slotno;
	FbxMesh* fbxmesh;
	CModel* pmodel;
	FbxNode* jointnode;
	FbxNode* linknode;
	int framestart;
	int frameend;
	CBone* curbone;
	int animno;
	int motid;
	double animleng;
	FbxCluster* cluster;
	FbxPose* pPose;
	FbxAMatrix globalcurrentpos;

	funcmpparams() {
		slotno = 0;
		fbxmesh = 0;
		pmodel = 0;
		jointnode = 0;
		linknode = 0;
		framestart = 1;
		frameend = 1;
		curbone = 0;
		animno = 0;
		motid = 0;
		animleng = 0.0;
		cluster = 0;
		pPose = 0;
		globalcurrentpos.SetIdentity();
	};
}FUNCMPPARAMS;


typedef struct tag_newmpelem
{
	CBone* boneptr;
	CMotionPoint* mpptr;

	tag_newmpelem() {
		boneptr = 0;
		mpptr = 0;
	};
}NEWMPELEM;

typedef struct tag_fbxobj
{
	FbxNode* node;
	FbxMesh* mesh;

	tag_fbxobj() {
		node = 0;
		mesh = 0;
	};
}FBXOBJ;


typedef struct tag_physikrec
{
	double time;
	CBone* pbone;
	ChaMatrix btmat;

	tag_physikrec() {
		time = 0.0;
		pbone = 0;
		btmat.SetIdentity();
	};
}PHYSIKREC;

typedef struct tag_dispgroupelem
{
	int objno;
	int depth;
	int groupno;
	FbxNode* pNode;
	CMQOObject* mqoobject;

	//bool operator< (const tag_dispgroupelem& right) const {
	//	if (!mqoobject) {
	//		return false;
	//	}
	//	double srcdist = mqoobject->GetDistFromCamera();
	//	if (!right.mqoobject) {
	//		return true;
	//	}
	//	double cmpdist = right.mqoobject->GetDistFromCamera();
	//
	//	double diffdist = srcdist - cmpdist;
	//	if (diffdist < 0) {
	//		return true;
	//	}
	//	else if (diffdist > 0) {
	//		return false;
	//	}
	//	else {
	//		return (mqoobject->GetObjectNo() < right.mqoobject->GetObjectNo());
	//	}
	//};


	void Init() {
		objno = 0;
		depth = 0;
		groupno = 1;
		pNode = 0;
		mqoobject = 0;
	};

	tag_dispgroupelem() {
		Init();
	};
}DISPGROUPELEM;

typedef struct tag_instancingparams
{
	float wmat[16];
	float vpmat[16];
	ChaVector4 diffusemult;
	ChaVector4 scale;
	ChaVector4 scaleoffset;
	void Init()
	{
		ZeroMemory(wmat, sizeof(float) * 16);
		ZeroMemory(vpmat, sizeof(float) * 16);
		diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
		scale = ChaVector4(1.0f, 1.0f, 1.0f, 0.0f);
		scaleoffset = ChaVector4(0.0f, 0.0f, 0.0f, 0.0f);
	};
	tag_instancingparams() {
		Init();
	};
}INSTANCINGPARAMS;



#define MAXPHYSIKRECCNT		(60 * 60)

//########################################
//UpdateMatrix用のスレッドの数
//最大値であってカレントの設定値ではない
//########################################
//#define MAXUPDATEMATRIXTHREAD 4
//#define MAXUPDATEMATRIXTHREAD 12
//#define MAXUPDATEMATRIXTHREAD 2
#define MAXUPDATEMATRIXTHREAD 8


//############ 2022/11/01 #########################################################
//LoadFbxは　同期の必要回数が多すぎるので　別スレッド１つの方が速い
//LOADFBXANIMTHREADを1以外にする場合には
//CBone::GetFbxAnim内のpNode->EvaluateGlobalTransformをCriticalSectionで囲む必要有
//#################################################################################
#define LOADFBXANIMTHREAD 1


//#define LOADFBXANIMTHREAD 4
//#define LOADFBXANIMTHREAD 8
//#define MAXLOADFBXANIMBONE	512



class CModel
{
public:

/**
 * @fn
 * CModel
 * @breaf コンストラクタ
 * @return なし。
 */
	CModel();

/**
 * @fn
 * ~CModel
 * @breaf デストラクタ。
 * @return デストラクタ。
 */
	~CModel();

/**
 * @fn
 * LoadMQO
 * @breaf メタセコイアで作成した３Dデータファイル*.mqoを読み込む。
 * @param (ID3D12Device* pdev) IN Direct3Dのデバイス。
 * @param (WCHAR* wfile) IN mqoファイルのフルパス。
 * @param (WCHAR* modelfolder) IN FBX書き出しの際に使用するモデルフォルダー名。ファイル名から拡張子を取ったものに通し番号を付けたものがデフォルト。
 * @param (float srcmult) IN 読み込み倍率。
 * @param (int ismedia) IN SDKmisc.cppのDXUTFindDXSDKMediaFileCchで探すディレクトリ内のファイルかどうかというフラグ。
 * @param (int texpool = 0) IN テクスチャを作成する場所。
 * @return 成功したら０。
 * @detail texpool引数にはデフォルト値があるので省略可能。
 */
	int LoadMQO( ID3D12Device* pdev, const WCHAR* wfile, const WCHAR* modelfolder, float srcmult, int ismedia, int texpool = 0 );
	
/**
 * @fn
 * LoadFBX
 * @breaf FBXファイルを読み込む。
 * @param (int skipdefref) IN デフォルト剛体設定を利用をスキップするかどうかのフラグ。chaファイルから呼び出すときはrefファイルがあるので１。FBX単体で読み込むときは０。
 * @param (ID3D12Device* pdev) IN Direct3DのDevice。
 * @param (WCHAR* wfile) IN FBXファイルのフルパス。
 * @param (WCHAR* modelfolder) IN FBX書き出し時に使用するFBXファイルがあるフォルダの名前。chaファイルがあるフォルダの中のFBXがあるフォルダの名前となる。
 * @param (float srcmult) IN 読み込み倍率。
 * @param (FbxManager* psdk) IN FBXSDKのマネージャ。
 * @param (FbxImporter** ppimporter) IN FBXSDKのインポーター。
 * @param (FbxScene** ppscene) IN FBXSDKのシーン。
 * @param (int forcenewaxisflag) 過渡期ファイルのフラグ。
 * @return 成功したら０。
 */
	int LoadFBX( int skipdefref, ID3D12Device* pdev, const WCHAR* wfile, const WCHAR* modelfolder, 
		float srcmult, FbxManager* psdk, FbxImporter** ppimporter, FbxScene** ppscene, 
		int forcenewaxisflag, BOOL motioncachebatchflag);

/**
 * @fn
 * LoadFBXAnim
 * @breaf FBXのアニメーションを読み込む。
 * @param (FbxManager* psdk) IN FBXSDKのマネージャ。
 * @param (FbxImporter* pimporter) IN FBXSDKのインポーター。
 * @param (FbxScene* pscene) IN FBXSDKのシーン。
 * @param (int (*tlfunc)( int srcmotid )) IN タイムライン初期化用の関数へのポインタ。
 * @return 成功したら０。
 * @detail LoadFBX呼び出しが成功した後でこの関数を呼び出す。
 */
	int LoadFBXAnim( FbxManager* psdk, FbxImporter* pimporter, FbxScene* pscene, int (*tlfunc)( int srcmotid ), BOOL motioncachebatchflag);


/**
 * @fn
 * OnRender
 * @breaf モデルデータを描画する。
 * @param (ID3D12Device* pdev) IN Direct3DのDevice。
 * @param (int lightflag) IN 照光処理をするかどうかのフラグ。
 * @param (ChaVector4 diffusemult) IN ディフューズ(拡散光)に乗算するRGBAの値。ライトを変えなくても明るさを変えることが出来る。
 * @param (int btflag = 0) IN bulletのシミュレーション中であるかどうかのフラグ。
 * @return 成功したら０。
 */
	int OnRender(bool withalpha, RenderContext* pRenderContext, int lightflag, ChaVector4 diffusemult, int btflag = 0, bool calcslotflag = false );

	int RenderRefArrow(bool limitdegflag, 
		myRenderer::RenderingEngine* re, ChaScene* srcchascene, ChaMatrix matVP,
		CBone* boneptr, ChaVector4 diffusemult, double refmult, std::vector<ChaVector3> vecbonepos);
	void ResetRefPosMarkInstanceScale();



	//DispGroup : test button : exclusive display
	int SetDispGroupGUI(std::vector<OrgWinGUI::OWP_CheckBoxA*>& checkboxvec, std::vector<CMQOObject*>& mqoobjvec);
	int RenderTest(bool withalpha, myRenderer::RenderingEngine* re, int lightflag, ChaVector4 diffusemult, int srcobjno);
	int SelectRenderObject(int srcobjno, std::vector<CMQOObject*>& selectedobjvec);
	void SelectRenderObjectReq(FbxNode* pNode, std::vector<CMQOObject*>& selectedobjvec);
	int GetSelectedObjTree(int srcobjno, std::vector<int>& selectedobjtree);
	void GetSelectedObjTreeReq(FbxNode* pNode, std::vector<int>& selectedobjtree);


	int CreateObjno2DigElem();
	void CreateObjno2DigElemReq(FbxNode* pNode, int* pobjno, int depth);
	int MakeDispGroupForRender();
	int RemakeHSVToonTexture(CMQOMaterial* srcmqomat);
/**
 * @fn
 * RenderBoneMark
 * @breaf ボーンマークとジョイントマークと剛体形状を表示する。
 * @param (ID3D12Device* pdev) IN Direct3DのDevice。
 * @param (CModel* bmarkptr) IN ボーンマークのモデルデータ。
 * @param (CMySprite* bcircleptr) IN ジョイント部分の表示のSprite。
 * @param (int selboneno) IN 選択中のボーンのID。
 * @param (int skiptopbonemark) IN 一番親からのボーンを表示しないフラグ。
 * @return 成功したら０。
 */
	int RenderBoneMark(myRenderer::RenderingEngine* re,
		bool limitdegflag, InstancedSprite* bcircleptr,
		int selboneno, ChaScene* srcchascene, ChaMatrix srcmatVP);
	void ResetBoneMarkInstanceScale();


	int RenderBoneCircleOne(bool limitdegflag, RenderContext* pRenderContext, CMySprite* bcircleptr, int selboneno);


	//void RenderCapsuleReq(bool limitdegflag, RenderContext* pRenderContext, CBtObject* srcbto);
	void RenderCapsuleReq(CBtObject* srcbt, myRenderer::RenderingEngine* re, bool limitdegflag,
		int selboneno, ChaScene* srcchascene, ChaMatrix srcmatVP);
	void ResetDispObjScale();

	void RenderBoneCircleReq(RenderContext* pRenderContext, CBtObject* srcbto, CMySprite* bcircleptr);

/**
 * @fn
 * GetModelBound
 * @breaf バウンダリーデータを取得する。
 * @param (MODELBOUND* dstb) OUT バウンダリーデータ（モデルを囲い込む最小範囲データ）がセットされる。
 * @return 成功したら０。
 */
	int GetModelBound( MODELBOUND* dstb );

/**
 * @fn
 * MakeObjectName
 * @breaf オブジェクトの名前からCMQOObjectを検索するためのデータをセットする。
 * @return 成功したら０。
 */
	int MakeObjectName();

	int MakeLaterMaterial();

	int SetLODNum();
	int SetLODNum(const char* srcheadname, int srcnum);
/**
 * @fn
 * MakePolyMesh3
 * @breaf メタセコイアから読み込んだデータの表示用データを作成する。
 * @return 成功したら０。
 */
	int MakePolyMesh3(bool fbxfileflag);

/**
 * @fn
 * MakePolyMesh4
 * @breaf FBXから読み込んだデータの表示用データを作成する。
 * @return 成功したら０。
 */
	int MakePolyMesh4();

/**
 * @fn
 * MakeExtLine
 * @breaf メタセコイアやFBXから読み込んだデータの線分表示用のデータを作成する。
 * @return 成功したら０。
 */
	int MakeExtLine();
	
/**
 * @fn
 * MakeDispObj
 * @breaf DirectXの描画用バッファのラッパデータを作成する。
 * @return 成功したら０。
 */	
	int MakeDispObj();

/**
 * @fn
 * UpdateMatrix
 * @breaf アニメーションデータを適用する。現在の時間の姿勢をセットする。モーフアニメがあればモーフアニメも適用する。
 * @param (ChaMatrix* wmat) IN ワールド変換行列。
 * @param (ChaMatrix* vpmat) IN View * Projection変換行列。
 * @return 成功したら０。
 */
	int UpdateMatrix(bool limitdegflag, 
		ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat, 
		bool needwaitfinished = true);// , int updateslot = 0);
	void UpdateMatrixReq(bool limitdegflag, CBone* srcbone, int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat);
	void SetUpdateSlotReq(CBone* srcbone, int srcslot);

	int ChkInView();
	//int SwapCurrentMotionPoint();
	int HierarchyRouteUpdateMatrix(bool limitdegflag, CBone* srcbone, ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat);
	//int UpdateLimitedWM(int srcmotid, double srcframe);
	int ClearLimitedWM(int srcmotid, double srcframe);
	void CopyWorldToLimitedWorldReq(CBone* srcbone, int srcmotid, double srcframe);
	void CopyLimitedWorldToWorldReq(CBone* srcbone, int srcmotid, double srcframe);
	int CopyLimitedWorldToWorldOne(CBone* srcbone, int srcmotid, double srcframe);
	void ApplyNewLimitsToWMReq(CBone* srcbone, int srcmotid, double srcframe, ChaMatrix befeditparentmat);

	void WaitUpdateMatrixFinished();
	//void CalcWorldMatAfterThreadReq(CBone* srcbone, int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat);
	//void CalcLimitedEulAfterThreadReq(CBone* srcbone, int srcmotid, double srcframe);

/**
 * @fn
 * SetShaderConst
 * @breaf シェーダーの定数をセットする。アニメーション、ライト、テクスチャなどのシェーダー定数をDirect3Dの描画命令を呼び出す前にセットしておく。
 * @return 成功したら０。
 */
	int SetShaderConst(int btflag = 0, bool calcslotflag = false);

/**
 * @fn
 * FillTimeLine
 * @breaf タイムラインにボーンの分、行を追加する。
 * @param (OrgWinGUI::OWP_Timeline& timeline) OUT タイムライン。
 * @param (map<int, int>& lineno2boneno) OUT タイムラインの行番号からボーンIDを検索するためのmap。
 * @param (map<int, int>& boneno2lineno) OUT ボーンIDからタイムラインの行番号を検索するためのmap。
 * @return 成功したら０。
 */
	int FillTimeLine( OrgWinGUI::OWP_Timeline& timeline, std::map<int, int>& lineno2boneno, std::map<int, int>& boneno2lineno );
	int FillTimeLineOne(CBone* curbone, int lineno,
		OrgWinGUI::OWP_Timeline& timeline,
		std::map<int, int>& lineno2boneno, std::map<int, int>& boneno2lineno);


/**
 * @fn
 * AddMotion
 * @breaf アニメーション保持用のエントリーを作成する。
 * @param (char* srcname) IN アニメーションの名前。
 * @param (WCHAR* wfilename) IN アニメーションファイル書き出し時のための名前。
 * @param (double srcleng) IN アニメーションの長さ。
 * @param (int* dstid) OUT アニメーションの選択に使用するIDがセットされる。
 * @return 成功したら０。
 */
	int AddMotion( const char* srcname, const WCHAR* wfilename, double srcleng, int* dstid );

/**
 * @fn
 * SetCurrentMotion
 * @breaf 現在のモーションIDを指定する。
 * @param (int srcmotid) IN モーションID。
 * @return 成功したら０。
 */
	int SetCurrentMotion( int srcmotid );
/**
 * @fn
 * SetMotionFrame
 * @breaf 再生するモーションのフレームを指定する。
 * @param (double srcframe) IN フレーム（時間）を指定する。
 * @return 成功したら０。
 */
	int SetMotionFrame(double srcframe);
	int SetMotionFrame(int srcmotid, double srcframe);


/**
 * @fn
 * GetMotionFrame
 * @breaf 現在のモーションの再生フレームを取得する。
 * @param (double* dstframe) OUT 再生フレームがセットされる。
 * @return 成功したら０。
 */

	double GetCurrentFrame();

	void SetRenderSlotFrame(double srcframe);
	double GetRenderSlotFrame();

	double GetOpeFrame(bool calcslotflag);//calcslotflagに応じて　GetCurrentFrame() or GetRenderSlotFrame()を返す

	int GetMotionName(int srcmotid, int dstnamelen, char* dstname);

	int GetCurrentMotID();
	double GetCurrentMotLeng();
	double GetCurrentMaxFrame();//return (motleng - 1.0)

	int GetCurrentMotName(char* dstname, int dstleng);
	bool ExistCurrentMotion();
	
/**
 * @fn
 * SetMotionSpeed
 * @breaf モーションの再生スピード(60fpsに対しての倍率)を指定する。
 * @param (double srcspeed) IN 再生スピード。
 * @return 成功したら０。
 */
	int SetMotionSpeed(double srcspeed);
	int SetMotionSpeed(int srcmotid, double srcspeed);

/**
 * @fn
 * GetMotionSpeed
 * @breaf モーションの再生スピードを取得する。
 * @param (double* dstspeed) OUT 再生スピードがセットされる。
 * @return 成功したら０。
 */
	int GetMotionSpeed( double* dstspeed );

/**
 * @fn
 * DeleteMotion
 * @breaf モーションを削除する。
 * @param (int motid) IN 削除するモーションのID。
 * @return 成功したら０。
 * @detail 姿勢のキーだけでなく、モーションのエントリーも削除する。
 */
	int DeleteMotion( int motid );


/**
 * @fn
 * GetSymBoneNo
 * @breaf 左右対称設定の名前のボーンのIDを取得する。
 * @param (int srcboneno) IN 元のボーンのID。
 * @param (int* dstboneno) OUT 左右対称設定のボーンのIDがセットされる。 
 * @param (int* existptr) OUT 左右対称設定のボーンが見つかれば１、無ければ０がセットされる。
 * @return 成功したら０。
 */
	int GetSymBoneNo( int srcboneno, int* dstboneno, int* existptr );


/**
 * @fn
 * PickBone
 * @breaf 指定した画面の２D座標に近いボーンを取得する。
 * @param (UIPICKINFO* pickinfo) INOUT 取得情報へのポインタ。
 * @return 成功したら０。
 * @detail pickinfoにウインドウのサイズとクリックした２D座標とボーンとの最大距離を指定して呼び出す。
 */
	int PickBone( UIPICKINFO* pickinfo );

/**
 * @fn
 * IKRotate
 * @breaf 選択ボーンの選択フレームに対してIKで姿勢を回転する。
 * @param (CEditRange* erptr) IN 選択フレーム情報を指定する。
 * @param (int srcboneno) IN 選択ボーンのIDを指定する。
 * @param (ChaVector3 targetpos) IN 選択ボーンの目標座標を指定する。
 * @param (int maxlevel) IN IK計算で何階層親までさかのぼるかを指定する。
 * @return 成功したら０。
 * @detail AdditiveIKにおいては、マニピュレータの中央の黄色をドラッグした時に呼ばれる。
 */

	int IKRotateUnderIK(bool limitdegflag, CEditRange* erptr,
		int srcboneno, ChaVector3 targetpos, int maxlevel);
	int IKRotatePostIK(bool limitdegflag, CEditRange* erptr,
		int srcboneno, int maxlevel);


	void ClearIKRotRec();
	void ClearIKRotRecUV();


	int IKRotate(bool limitdegflag, CEditRange* erptr, int srcboneno, ChaVector3 targetpos, int maxlevel, double directframe = -1.0);
	
	//2023/10/17 ChaCalcFuncに移動
	//int IKRotateForIKTarget(bool limitdegflag, CEditRange* erptr, int srcboneno, ChaVector3 targetpos, 

	//2023/10/17 ChaCalcFuncに移動
	//	int maxlevel, double directframe, bool postflag);
	//int IKTargetVec(bool limitdegflag, CEditRange* srptr, double srcframe, bool postflag);

	int SetIKTargetVec();
	void SetIKTargetVecReq(CBone* srcbone);
	int PosConstraintExecuteFromButton(bool limitdegflag, CEditRange* erptr);
	int RefreshPosConstraint();

	bool CheckIKTarget();
	void CheckIKTargetReq(CBone* srcbone, bool* pfound);

	//int PhysicsRot(CEditRange* erptr, int srcboneno, ChaVector3 targetpos, int maxlevel);
	//int PhysicsMV(CEditRange* erptr, int srcboneno, ChaVector3 diffvec);

	int CreatePhysicsPosConstraint(CBone* srcbone);
	int DestroyPhysicsPosConstraint(CBone* srcbone);
	int CreatePhysicsPosConstraintAll();
	int DestroyPhysicsPosConstraintAll();
	int CreatePhysicsPosConstraintUpper(CBone* srcbone);
	int DestroyPhysicsPosConstraintUpper(CBone* srcbone);
	int CreatePhysicsPosConstraintLower(CBone* srcbone);
	int DestroyPhysicsPosConstraintLower(CBone* srcbone);


	int Mass0_All(bool setflag);
	int Mass0_Upper(bool setflag, CBone* srcbone);
	int Mass0_Lower(bool setflag, CBone* srcbone);

	int SetMass0(CBone* srcbone);
	int RestoreMass(CBone* srcbone);

	int SetKinematicTmpLower(CBone* srcbone, bool srcflag);
	
/**
 * @fn
 * IKRotateAxisDelta
 * @breaf 選択ボーンの選択フレームに対して軸指定でIK回転をする。
 * @param (CEditRange* erptr) IN 選択フレーム情報を指定する。
 * @param (int axiskind) IN 回転軸を指定する。PICK_X, PICK_Y, PICK_Zのいずれか。
 * @param (int srcboneno) IN 選択ボーンのIDを指定する。
 * @param (float delta) IN 回転角度を指定する。
 * @param (int maxlevel) IN IK計算で何階層親までさかのぼるかを指定する。
 * @param (int ikcnt) IN マウスでドラッグを開始してから何回この関数を実行したか。
 * @return 成功したら０。
 * @detail AdditiveIKにおいては、マニピュレータのリングまたは球をドラッグした時に呼ばれる。
 */
	int IKRotateAxisDeltaUnderIK(
		bool limitdegflag, CEditRange* erptr, 
		int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat);
	int IKRotateAxisDeltaPostIK(
		bool limitdegflag, CEditRange* erptr, 
		int axiskind, int srcboneno, int maxlevel, int ikcnt);



	int IKRotateAxisDelta(bool limitdegflag, CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat);
	//int TwistBoneAxisDelta(CEditRange* erptr, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat);


	//int PhysicsRotAxisDelta(CEditRange* erptr, int axiskind, int srcboneno, float delta, int maxlevel, int ikcnt, ChaMatrix selectmat);
	int SetDofRotAxis(int srcaxiskind);


/**
 * @fn
 * RotateXDelta
 * @breaf ボーン軸に関して姿勢をねじる。
 * @param (CEditRange* erptr) IN 選択フレーム情報を指定する。
 * @param (int srcboneno) IN 選択ボーンのIDを指定する。
 * @param (float delta) IN 回転角度を指定する。
 * @return 成功したら０。
 * @detail AdditiveIKにおいては、マニピュレータのリングまたは球でボーン軸に関して回転するときに呼ばれる。
 */
	//int RotateXDelta( CEditRange* erptr, int srcboneno, float delta );

/**
 * @fn
 * FKRotate
 * @breaf 選択ボーンの選択フレームに対してFKで指定した分だけ回転する。
 * @param (double srcframe) IN 選択フレーム情報を指定する。
 * @param (int srcboneno) IN 選択ボーンのIDを指定する。
 * @param (CQuaternion rotq) IN 回転を表すクォータニオン。
 * @return 成功したら０。
 */
	int FKRotate(bool limitdegflag, bool onretarget, int reqflag,
		CBone* bvhbone, int traflag, ChaVector3 traanim, double srcframe, int srcboneno,
		CQuaternion srcq);//, int setmatflag = 0, ChaMatrix* psetmat = 0);
	//int FKRotate(double srcframe, int srcboneno, ChaMatrix srcmat);

/**
 * @fn
 * FKBoneTra
 * @breaf 選択ボーンの選択フレームに対してFKで指定した分だけ移動する。
 * @param (CEditRange* erptr) IN 選択フレーム情報を指定する。
 * @param (int srcboneno) IN 選択ボーンのIDを指定する。
 * @param (ChaVector3 addtra) IN 移動分のベクトル。
 * @return 成功したら０。
 */
	int FKBoneTraUnderFK(
		bool limitdegflag, CEditRange* erptr,
		int srcboneno, ChaVector3 addtra);
	int FKBoneTraPostFK(
		bool limitdegflag, CEditRange* erptr,
		int srcboneno);

	//2023/10/17 ChaCalcFuncに移動
	//int FKBoneTra(bool limitdegflag, int onlyoneflag, CEditRange* erptr, 
	//	int srcboneno, ChaVector3 addtra, double onlyoneframe = 0.0 );



	int FKBoneTraAxisUnderFK(
		bool limitdegflag, CEditRange* erptr,
		int srcboneno, int axiskind, float delta, ChaMatrix selectmat);
	int FKBoneTraAxisPostFK(
		bool limitdegflag, CEditRange* erptr,
		int srcboneno);
	int FKBoneTraAxis(bool limitdegflag, int onlyoneflag, CEditRange* erptr, 
		int srcboneno, int axiskind, float delta, ChaMatrix selectmat);



	int FKBoneScale(bool limitdegflag, int onlyoneflag, CEditRange* erptr, 
		int srcboneno, ChaVector3 scalevec);

	int FKBoneScaleAxis(bool limitdegflag, int onlyoneflag, CEditRange* erptr, 
		int srcboneno, int axiskind, float scaleval);


/**
 * @fn
 * CollisionNoBoneObj_Mouse
 * @breaf 画面の２Dの位置にオブジェクトがあるかどうか調べる。
 * @param (UIPICKINFO* pickinfo) INOUT 画面の２D座標と最大距離を指定する。
 * @param (char* objnameptr) IN 調べたいオブジェクトの名前を指定する。
 * @return ２D位置にオブジェクトがあれば１、無ければ０を返す。
 */
	int CollisionNoBoneObj_Mouse(UIPICKINFO* pickinfo, const char* objnameptr, bool excludeinvface);

	int CollisionPolyMesh3_Mouse(UIPICKINFO* pickinfo, CMQOObject* pickobj, int* hitfaceindex);

/**
 * @fn
 * TransformBone
 * @breaf ボーンの座標を変換計算する。
 * @param (int winx) IN ３D表示ウインドウの幅
 * @param (int winy) IN ３D表示ウインドウの高さ
 * @param (int srcboneno) IN ボーンのID。
 * @param (ChaVector3* worldptr) OUT ワールド座標系の変換結果。
 * @param (ChaVector3* screenptr) OUT *worldptrに更にView Projectionを適用した座標。
 * @param (ChaVector3* dispptr) OUT *screenptrを-WindowSize/2から+WindowSize/2までの２D座標にしたもの。
 * @return 成功したら０。
 * @detail CBoneのUpdateMatrixが呼ばれた後で呼び出されることを想定している。(CBone::m_childworldを使用している。)
 */
	int TransformBone( int winx, int winy, int srcboneno, ChaVector3* worldptr, ChaVector3* screenptr, ChaVector3* dispptr );

/**
 * @fn
 * ChangeMotFrameLeng
 * @breaf モーションのフレーム長を変更する。
 * @param (int motid) IN モーションのID。
 * @param (double srcleng) IN 変更後のフレーム長。
 * @return 成功したら０。
 * @detail 範囲外のキーは削除される。
 */
	int ChangeMotFrameLeng( int motid, double srcleng );

/**
 * @fn
 * AdvanceTime
 * @breaf 経過時間を元に、モーションの次に再生するフレームを計算する。
 * @param (int previewflag) IN プレビュー状態を指定。
 * @param (double difftime) IN 前回描画してからの時間を指定。
 * @param (double* nextframeptr) OUT 次に描画するフレーム。
 * @param (int* endflagptr) OUT 繰り返し再生ではないモーションの最終フレームに達したかどうか。
 * @param (int* loopstartflag) 繰り返しにより最初のフレームに戻ったかどうか。
 * @param (int srcmotid) IN モーションのID。
 * @return 成功したら０。
 * @detail previewflagは停止中０、通常再生１、bullet物理シミュレーション４、bulletラグドールシミュレーション５。
 */
	int AdvanceTime(int onefps, CEditRange srcrange, int previewflag, double difftime, double* nextframeptr, int* endflagptr, int* loopstartflag, int srcmotid);

/**
 * @fn
 * MakeEnglishName
 * @breaf ファイル書き出し(主にFBXファイル)のために各種名前を英語表記にする。
 * @return 成功したら０。
 */
	int MakeEnglishName();

/**
 * @fn
 * AddDefMaterial
 * @breaf デフォルトの材質を作成する。
 * @return 成功したら０。
 * @detail メタセコイアファイルには材質が含まれていない場合がある。材質が１つもないと描画時にエラーになるのでデフォルトの材質を作成する。
 */
	int AddDefMaterialIfEmpty();

/**
 * @fn
 * CreateBtObject
 * @breaf bulletシミュレーション用のオブジェクトを作成する。
 * @param (int onfirstcreate) IN 一番最初の呼び出し時にだけ１を指定する。
 * @return 成功したら０。
 * @detail シミュレーション開始のたびに呼ぶ。一番最初の呼び出しだけonfirstcreateを１にする。
 */
	int CreateBtObject(bool limitdegflag, int onfirstcreate);

/**
 * @fn
 * SetBtMotion
 * @breaf bulletシミュレーション結果を適用する。
 * @param (int rgdollflag) IN ラグドール時に１。
 * @param (double srcframe) IN モーションのフレーム。
 * @param (ChaMatrix* wmat) IN ワールド変換行列。
 * @param (ChaMatrix* vpmat) IN ViewProj変換行列。
 * @param (double difftime) IN 前回の描画からの時間。
 * @return 成功したら０。
 * @detail bulletシミュレーション時には、CModel::Motion2Bt-->BPWorld::clientMoveAndDisplay-->CModel::SetBtMotionという流れで呼び出す。
 */
	int SetBtMotion(bool limitdegflag, CBone* srcbone, int rgdollflag, 
		double srcframe, ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat);
	int SetBtMotionOnBt(bool limitdegflag,
		double srcframe, ChaMatrix* vmat, ChaMatrix* pmat);// , int updateslot);

/**
 * @fn
 * SetBtImpulse
 * @breaf 設定されていたインパルスを適用する。
 * @return 成功したら０。
 */
	int SetBtImpulse();

/**
 * @fn
 * SetImp
 * @breaf ボーンに与えるインパルスを設定する。
 * @param (int srcboneno) IN ボーンのID。
 * @param (int kind) IN X成分指定時０、Y指定時１、Z指定時２。
 * @param (float srcval) IN インパルスの大きさ。
 * @return 成功したら０。
 */
	int SetImp( int srcboneno, int kind, float srcval );

/**
 * @fn
 * SetDispFlag
 * @breaf 表示するかどうかのフラグを設定する。
 * @param (char* objname) IN オブジェクトの名前を指定する。
 * @param (int flag) IN 表示するとき１、しないとき０。
 * @return 成功したら０。
 * @detail 表示するとき１、しないとき０。
 */
	int SetDispFlag( const char* objname, int flag );

	int SetLightFlag(bool srcflag);

/**
 * @fn
 * FindBtObject
 * @breaf 子供ジョイントの名前に対応するCBtObjectを検索する。
 * @param (int srcboneno) IN 子供ジョイントの名前。
 * @return CBtObject* 検索結果。
 */
	CBtObject* FindBtObject( int srcboneno );


	//int FillUpEmptyMotion(int motid);


	int SetAllSphrateData(int gid, int rgdindex, float srcval);
	int SetAllBoxzrateData(int gid, int rgdindex, float srcval);
	int SetAllSkipflagData(int gid, int rgdindex, int srcval);
	int SetAllForbidrotData(int gid, int rgdindex, bool srcval);
	int SetAllColtypeData(int gid, int rgdindex, int srcval);
	int SetAllBtforceData(int rgdindex, bool srcval);
	int SetAllDampAnimData( int gid, int rgdindex, float valL, float valA );
	int SetAllBtgData( int gid, int reindex, float btg );
	int SetAllImpulseData( int gid, float impx, float impy, float impz );
	int SetAllKData( int gid, int reindex, int srclk, int srcak, float srccuslk, float srccusak );
	int SetAllRestData( int gid, int reindex, float rest, float fric );
	int SetAllMassData( int gid, int reindex, float srcmass );
	int SetAllMassDataByBoneLeng(int gid, int reindex, float srcmass);

	int SetAllDmpData( int gid, int reindex, float ldmp, float admp );
	int EnableAllRigidElem(int srcrgdindex);
	int DisableAllRigidElem(int srcrgdindex);

	int SetColTypeAll(int reindex, int srctype);

	int Motion2Bt(bool limitdegflag, double nextframe, ChaMatrix* pmView, ChaMatrix* pmProj);// , int updateslot);
	void Motion2BtReq(CBtObject* srcbto);

	//int SetRagdollKinFlag(int selectbone, int physicsmvkind = 0);
	int SetKinematicFlag();
	int CreateRigidElem();
	int SetCurrentRigidElem( int curindex );
	REINFO GetCurrentRigidElemInfo(int* retindex);
	void CreateRigidElemReq( CBone* curbone, int reflag, std::string rename, int impflag, std::string impname );
	int SetBtEquilibriumPoint(bool limitdegflag);
	int SetBtEquilibriumPointReq(bool limitdegflag, CBtObject* srcbto);
	void SetDofRotAxisReq(CBtObject* srcbto, int srcaxiskind);


	int MultDispObj( ChaVector3 srcmult, ChaVector3 srctra );
	MOTINFO* GetRgdMorphInfo();

	int SetColiIDtoGroup( CRigidElem* curre );
	int ResetBt();

	int InitUndoMotion( int saveflag );
	int SaveUndoMotion(bool LimitDegCheckBoxFlag, bool limitdegflag, int curboneno, int curbaseno,
		CEditRange* srcer, double srcapplyrate, BRUSHSTATE srcbrushstate, bool allframeflag);
	int RollBackUndoMotion(bool limitdegflag, HWND hmainwnd, int redoflag, 
		int* curboneno, int* curbaseno, 
		double* dststartframe, double* dstendframe, double* dstapplyframe, BRUSHSTATE* dstbrushstate);

	int AddBoneMotMark( OrgWinGUI::OWP_Timeline* owpTimeline, int curboneno, int curlineno, double startframe, double endframe, int flag );

	int MotionID2Index( int motid );
	int MotionID2CameraIndex(int motid);
	FbxAnimLayer* GetAnimLayer( int motid );
	float GetTargetWeight( int motid, double dframe, double timescale, CMQOObject* baseobj, std::string targetname );


	int SetFirstFrameBonePos(HINFO* phinfo, CBone* srchipsbone = 0);

	int RecalcBoneAxisX(CBone* srcbone);

	int CalcBoneEul(bool limitdegflag, int srcmotid);
	void CalcBoneEulReq(bool limitdegflag, CBone* curbone, int srcmotid, double srcframe);
	void CalcBoneEulReq(bool limitdegflag, CBone* curbone, int srcmotid, double startframe, double endframe);


	int RigControl(bool limitdegflag, int depthcnt, CEditRange* erptr, 
		int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig, int buttonflag);
	int RigControlUnderRig(bool limitdegflag, int depthcnt, CEditRange* erptr,
		int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig, int buttonflag);
	int RigControlPostRig(bool limitdegflag, int depthcnt, CEditRange* erptr,
		int srcboneno, int uvno, CUSTOMRIG ikcustomrig, int buttonflag);


	//int PhysicsRigControl(int depthcnt, CEditRange* erptr, int srcboneno, int uvno, float srcdelta, CUSTOMRIG ikcustomrig);

	int DbgDump();

	CBone* GetSymPosBone(CBone* srcbone);

	int InterpolateBetweenSelection(bool limitdegflag, double srcstartframe, double srcendframe, 
		CBone* srcbone, int srckind);

	int BulletSimulationStop();
	int BulletSimulationStart();
	int ApplyBtToMotion(bool limitdegflag);
	void CalcBtAxismat(int onfirstcreate);
	//void CalcRigidElem();
	int CalcRigidElemParamsOnBt();
	void SetBtKinFlagReq(CBtObject* srcbto, int oncreateflag);
	void SetKinematicFlagReq(CBtObject* srcbto);

	void ResetMotionCache();

	void EnableRotChildren(CBone* srcbone, bool srcflag);
	void EnableRotChildrenReq(CBone* srcbone, bool srcflag);

	void DestroyScene();


	void PhysIKRec(bool limitdegflag, double srcrectime);
	void PhysIKRecReq(bool limitdegflag, CBone* srcbone, double srcrectime);
	void ApplyPhysIkRec(bool limitdegflag);
	void ApplyPhysIkRecReq(bool limitdegflag, CBone* srcbone, double srcframe, double srcrectime);

	int ResetAngleLimit(bool excludebt, int srcval, CBone* srcbone = 0);
	int AngleLimitReplace180to170(CBone* srcbone = 0);
	int AdditiveCurrentToAngleLimit(CBone* srcbone = 0);
	int AdditiveAllMotionsToAngleLimit();
	int CopyWorldToLimitedWorld();

	bool ChkBoneHasRig(CBone* srcbone);

	int CreateBoneUpdateMatrix();//g_UpdateMatrixThreads変更時にも呼ぶ
	
	int CreatePostIKThreads();
	int DestroyPostIKThreads();
	void WaitPostIKFinished();
	int SetPostIKFrame(double srcstart, double srcend);

	int CreateInitMpThreads();
	int DestroyInitMpThreads();
	void WaitInitMpFinished();
	int SetInitMpFrame(double srcstart, double srcend);

	int CreateRetargetThreads();
	int DestroyRetargetThreads();
	void WaitRetargetFinished();
	int SetRetargetFrame(double srcstart, double srcend);

	int CreateFKTraThreads();
	int DestroyFKTraThreads();
	void WaitFKTraFinished();
	int SetFKTraFrame(double srcstart, double srcend);

	int CreateCopyW2LWThreads();
	int DestroyCopyW2LWThreads();
	void WaitCopyW2LWFinished();
	int SetCopyW2LWFrame(double srcstart, double srcend);


	int CreateCalcEulThreads();
	int DestroyCalcEulThreads();
	void WaitCalcEulFinished();



	//int GetFBXAnim(int animno, FbxNode* pNode, int motid, double animleng, bool callingbythread = false);//CThreadingLoadFbxからも呼ぶ CBoneに移動

	int InitMpFrame(bool limitdegflag, int srcmotid, CBone* srcbone, double srcstartframe, double srcendframe);
	void InitMPReq(bool limitdegflag, CBone* curbone, int srcmotid, double curframe);
	int InitMP(bool limitdegflag, CBone* curbone, int srcmotid, double curframe);

	void Adjust180DegReq(CBone* srcbone);

	//void FillUpEmptyKeyReq( int motid, double animleng, CBone* curbone, CBone* parentbone );

	void CreateIndexedMotionPointReq(CBone* srcbone, int srcmotid, double srcanimleng, int* perrorcount);

	CNodeOnLoad* FindNodeOnLoadByName(const char* srcname);
	FbxNode* FindNodeByBone(CBone* srcbone);
	CBone* FindBoneByNode(FbxNode* srcnode);
	bool HasCluster(FbxNode* srcnode, FbxNodeAttribute** ppAttrib);

	//NodeMatが変わらないように　SetRotationActiveは変更しないことに
	//void SetRotationActiveToBone();
	//void SetRotationActiveFalse();
	//void SetRotationActiveTrue();
	//void SetRotationActiveDefault();
	//void SetRotationActiveToBoneReq(CNodeOnLoad* srcnodeonload);
	//void SetRotationActiveFalseReq(CNodeOnLoad* srcnodeonload);
	//void SetRotationActiveTrueReq(CNodeOnLoad* srcnodeonload);
	//void SetRotationActiveDefaultReq(CNodeOnLoad* srcnodeonload);

	int GetCameraAnimParams(int cameramotid, double nextframe, double camdist, ChaVector3* pEyePos, ChaVector3* pTargetPos, ChaVector3* pcamupvec, ChaMatrix* protmat, int inheritmode);
	int GetCameraAnimParams(double nextframe, double camdist, ChaVector3* pEyePos, ChaVector3* pTargetPos, ChaVector3* pcamupvec, ChaMatrix* protmat, int inheritmode);
	int GetCameraProjParams(int cameramotid, float* pprojnear, float* pprojfar, float* pfovy, ChaVector3* pcampos, ChaVector3* pcamdir, ChaVector3* pcamupvec);
	ChaVector3 CalcCameraFbxEulXYZ(int cameramotid, double srcframe);
	ChaMatrix GetCameraTransformMat(int cameramotid, double nextframe, int inheritmode, bool multInvNodeMat);

	ChaVector3 GetCameraAdjustPos(int cameramotid);


	int SetIKStopFlag();

	int GetTextureNameVec(std::vector<std::string>& dstvec);
	int SetLaterTransparentVec(std::vector<std::wstring> srclatervec);//丸ごと設定


	int SetBefEditMat(bool limitdegflag, CEditRange* erptr, CBone* curbone, int maxlevel);
	int SetBefEditMatFK(bool limitdegflag, CEditRange* erptr, CBone* curbone);

	int Retarget(CModel* srcbvhmodel, ChaMatrix smatView, ChaMatrix smatProj,
		std::map<CBone*, CBone*>& sconvbonemap,
		int (*srcAddMotionFunc)(const WCHAR* wfilename, double srcmotleng));

	int CreateMaterialTexture();

	int SetRefPosFlag(bool srcflag);
	int SetRefPosFl4x4ToDispObj(int refposindex);


	void SetSphrateDataReq(int gid, int rgdindex, CBone* srcbone, float srcval);
	void SetBoxzrateDataReq(int gid, int rgdindex, CBone* srcbone, float srcval);
	void SetSkipflagDataReq(int gid, int rgdindex, CBone* srcbone, int srcval);
	void SetForbidrotDataReq(int gid, int rgdindex, CBone* srcbone, bool srcval);
	void SetColtypeDataReq(int gid, int rgdindex, CBone* srcbone, int srcval);
	void SetBtforceDataReq(int rgdindex, CBone* srcbone, bool srcval);
	void SetDampAnimDataReq(int gid, int rgdindex, CBone* srcbone, float valL, float valA);
	void SetBtgDataReq(int gid, int reindex, CBone* srcbone, float btg);
	void SetKDataReq(int gid, int reindex, CBone* srcbone, int srclk, int srcak, float srccuslk, float srccusak);
	void SetRestDataReq(int gid, int reindex, CBone* srcbone, float rest, float fric);
	void SetDmpDataReq(int gid, int reindex, CBone* srcbone, float ldmp, float admp);
	void SetMassDataReq(int gid, int reindex, CBone* srcbone, float srcmass);
	void SetMassDataByBoneLengReq(int gid, int reindex, CBone* srcbone, float srcmass);
	void SetColTypeReq(int reindex, CBone* srcbone, int srctype);
	void SetColiidDataReq(int reindex, CBone* srcbone, int srcgroup, std::vector<int> srccoliids, int srcmyselfflag);

private:
	int InitParams();
	int DestroyObjs();

	MODELBOUND CalcBoneBound();
	int AddModelBound( MODELBOUND* mb, MODELBOUND* addmb );

	//int AddMotionPointAll(int srcmotid, double animleng);

	int DestroyMaterial();
	int DestroyObject();
	int DestroyAncObj();
	int DestroyAllMotionInfo();
	
	//int CreateBoneUpdateMatrix();//publicへ移動
	int DestroyBoneUpdateMatrix();

	int CreateLoadFbxAnim(FbxScene* pscene);
	int DestroyLoadFbxAnim();
	void WaitLoadFbxAnimFinished();
	int PostLoadFbxAnim(FbxAnimLayer* mCurrentAnimLayer, int srcmotid);
	void PostLoadFbxAnimReq(FbxAnimLayer* mCurrentAnimLayer, int srcmotid, double animlen, CBone* srcbone);
	int PreLoadCameraFbxAnim(int srcmotid);


	//void MakeBoneReq( CBone* parentbone, CMQOFace* curface, ChaVector3* pointptr, int broflag, int* errcntptr );

	int SetShapeNoReq( CMQOFace** ppface, int facenum, int searchp, int shapeno, int* setfacenum);
	int SetFaceOfShape( CMQOFace** ppface, int facenum, int shapeno, CMQOFace** ppface2, int setfacenum );

	int CreateBtConstraint(bool limitdegflag);
	void CreateBtConstraintReq(bool limitdegflag, CBtObject* curbto);
	//void CreateBtConstraintReq(CBone* curbone);

	//void CreateIndexedMotionPointReq(CBone* srcbone, int srcmotid, double srcanimleng);//publicへ

	//void UpdateMatrixReq( int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat, 
	//	ChaMatrix* parmat, CQuaternion* parq, CBone* srcbone, int broflag );

	void FillTimelineReq( OrgWinGUI::OWP_Timeline& timeline, CBone* curbone, int* linenoptr, 
		std::map<int, int>& lineno2boneno, std::map<int, int>& boneno2lineno, int broflag );

	void SetSelectFlagReq( CBone* boneptr, int broflag );
	int CalcMouseLocalRay( UIPICKINFO* pickinfo, ChaVector3* startptr, ChaVector3* dirptr );
	//CBone* GetCalcRootBone( CBone* firstbone, int maxlevel );
	//void CalcXTransformMatrixReq( CBone* srcbone, ChaMatrix parenttra, float mult );


	//int InitFBXManager( FbxManager** ppSdkManager, FbxImporter** ppImporter, FbxScene** ppScene, char* utfname );
	void CreateFBXCameraReq(FbxNode* pNode);
	int CreateFBXMeshReq(FbxNode* pNode);
	int CreateFBXShape( FbxAnimLayer* panimlayer, double animleng, FbxTime starttime, FbxTime timestep );
	void CreateNodeOnLoadReq(CNodeOnLoad* newnodeonload);
	void DestroyNodeOnLoadReq(CNodeOnLoad* delnodeonload);

	void FindNodeOnLoadByNameReq(CNodeOnLoad* srcnodeonload, const char* srcname, CNodeOnLoad** ppfindnodeonload);


	CMQOObject* GetFBXMesh(FbxNode* pNode, FbxNodeAttribute* pAttrib);
	ChaMatrix CalcGlobalMeshMat(FbxNode* pNode, double srctime);
	ChaMatrix CalcLocalMeshMat(FbxNode* pNode, double srctime);
	void CalcMeshMatReq(FbxNode* pNode, double srctime, ChaMatrix* pmeshmat);
	int GetFBXShape(FbxMesh* pMesh, CMQOObject* curobj, FbxAnimLayer* panimlayer, double animleng, FbxTime starttime, FbxTime timestep );
	//int ComputeShapeDeformation(FbxNode* pNode, FbxMesh* pMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* curobj, char* takename );
	//int ComputeShapeDeformation2(FbxNode* pNode, FbxMesh* pMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* curobj, char* takename );

	int SetMQOMaterial( CMQOMaterial* newmqomat, FbxSurfaceMaterial* material );
	int SetMaterialTexNames(CMQOMaterial* newmqomat, char* tempname,
		FbxTexture::EWrapMode addressU, FbxTexture::EWrapMode addressV, 
		ChaVectorDbl2 chauvscale, ChaVectorDbl2 chauvoffset);
	void CreateFBXBoneReq(FbxScene* pScene, FbxNode* pNode, FbxNode* parnode );
	//void CreateExtendBoneReq(CBone* srcbone);
	int GetFBXBone(FbxScene* pScene, FbxNodeAttribute::EType type, FbxNodeAttribute *pAttrib, FbxNode* curnode, FbxNode* parnode );
	CBone* CreateNewFbxBone(FbxNodeAttribute::EType type, FbxNode* curnode, FbxNode* parnode);


	int CreateFBXAnim( FbxScene* pScene, FbxNode* prootnode, BOOL motioncachebatchflag );
	//void CreateFBXAnimReq( int animno, FbxScene* pScene, FbxPose* pPose, FbxNode* pNode, int motid, double animleng );
	//int GetFBXAnim(int animno, FbxNode* pNode, int motid, double animleng, bool callingbythread = false);//publicへ移動
	void CreateFBXSkinReq( FbxNode* pNode );
	int GetFBXSkin( FbxNodeAttribute *pAttrib, FbxNode* pNode );

	//void InitMpScaleReq(CBone* curbone, int srcmotid, double srcframe);
	//int CorrectFbxScaleAnim(int animno, FbxScene* pScene, FbxNode* pNode, FbxPose* pPose, FbxNodeAttribute* pAttrib, int motid, double animleng);


	int DestroyFBXSDK();
	int GetShapeWeight(FbxNode* pNode, FbxMesh* pMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* curobj );
	float GetFbxTargetWeight(FbxNode* pbaseNode, FbxMesh* pbaseMesh, std::string targetname, FbxTime& pTime, FbxAnimLayer * pAnimLayer, CMQOObject* baseobj );
	
	int SetDefaultBonePos(FbxScene* pScene);


	//SetDefaultBonePosReqは関数名にFbxを付けて FbxFile.h, FbxFile.cppに移動になりました。リンクエラーの関係で。
	//void SetDefaultBonePosReq( CBone* curbone, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix pParentGlobalPosition );



	//void FillUpEmptyKeyReq( int motid, double animleng, CBone* curbone, CBone* parentbone );//publicへ

	//int SetMaterialName();


	int DestroyBtObject();
	void DestroyBtObjectReq( CBtObject* curbt );
	void CreateBtObjectReq(bool limitdegflag, CBtObject* parbt, CBone* parentbone, CBone* curbone);

	void CalcBtAxismatReq( CBone* curbone, int onfirstcreate );
	//void CalcRigidElemReq(CBone* curbone);
	void SetBtMotionReq(bool limitdegflag, CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat);
	//void SetBtMotionPostLowerReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat, int kinematicadjustflag);
	//void SetBtMotionPostUpperReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat);
	//void SetBtMotionMass0BottomUpReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat);
	//void FindAndSetKinematicReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat);//Kinematicとそうでないところの境目を探してみつかったらLowerReqで親行列をセットする。
	//void SetBtMotionKinematicLowerReq(CBtObject* curbto, ChaMatrix oldparentmat, ChaMatrix newparentmat);
	//void AdjustBtMatToParent(CBone* curbone, CBone* childbone, int adjustrot = 0);
	//void AdjustBtMatToCurrent(CBone* curbone);
	//void AdjustBtMatToChild(CBone* curbone, CBone* childbone, int adjustrot = 0);
	//void InitBtMatTraAnimReq(CBtObject* curbto);

	//void BtMat2BtObjReq(CBtObject* curbto, ChaMatrix* wmat, ChaMatrix* vpmat);
	//void RecalcConstraintFrameABReq(CBtObject* curbto);

	void FindBtObjectReq( CBtObject* srcbto, int srcboneno, CBtObject** ppret );
	void SetImpulseDataReq( int gid, CBone* srcbone, ChaVector3 srcimp );
	void SetBtImpulseReq( CBone* srcbone );
	


	//void SetBtKinFlagReq(CBtObject* srcbto, int oncreateflag);
	//void SetBtGravityReq( CBtObject* srcbto );
	//void SetRagdollKinFlagReq( CBtObject* srcbto, int selectbone, int physicsmvkind = 0 );
	//void CreateBtConnectReq( CBone* curbone );
	void SetColiIDReq( CBone* srcbone, CRigidElem* srcre );
	void EnableAllRigidElemReq(CBone* srcbone, int srcrgdindex);
	void DisableAllRigidElemReq(CBone* srcbone, int srcrgdindex);

	int DampAnim( MOTINFO* rgdmorphinfo );

	void ResetBtReq( CBtObject* curbto );

	int GetNewUndoID();//Save用のundoid
	int GetValidUndoID();//undo用のid
	int GetValidRedoID();//redo用のid

	//2023/10/17 ChaCalcFuncに移動
	//int AdjustBoneTra(bool limitdegflag, CEditRange* erptr, CBone* lastpar);

	void SetFirstFrameBonePosReq(CBone* srcbone, int srcmotid, HINFO* phinfo);
	void InterpolateBetweenSelectionReq(bool limitdegflag, CBone* interpolatebone, 
		double srcstartframe, double srcendframe, bool oneflag, bool broflag);


	int DbgDumpBoneReq(int level, CBone* boneptr, int broflag);
	void DumpBtObjectReq(CBtObject* srcbto, int srcdepth);
	void DumpBtConstraintReq(CBtObject* srcbto, int srcdepth);
	FbxPose* FindBindPose();

	void CreatePhysicsPosConstraintReq(CBone* srcbone, int forceflag = 0);
	void DestroyPhysicsPosConstraintReq(CBone* srcbone, int forceflag = 0);
	void SetMass0Req(CBone* srcbone, bool forceflag);
	void RestoreMassReq(CBone* srcbone);
	void SetKinematicTmpLowerReq(CBone* srcbone, bool srcflag);

	//void PhysicsMVReq(CBone* srcbone, ChaVector3 diffvec);
	//int WithConstraint(CBone* srcbone);
	void BulletSimulationStopReq(CBtObject* srcbto);
	void BulletSimulationStartReq(CBtObject* srcbto);
	void ApplyBtToMotionReq(bool limitdegflag, CBone* srcbone);

	void SetCurrentRigidElemReq(CBone* srcbone, std::string curname);

	//2023/10/17 ChaCalcFuncクラスに移動
	//int CalcQForRot(bool limitdegflag, bool calcaplyflag,
	//	int srcmotid, double srcframe, double srcapplyframe, CQuaternion srcaddrot,
	//	CBone* srcrotbone, CBone* srcaplybone, 
	//	CQuaternion* dstqForRot, CQuaternion* dstqForHipsRot);
	int IsMovableRot(bool limitdegflag, int srcmotid, double srcframe, double srcapplyframe, 
		CQuaternion srcaddrot, CBone* srcrotbone, CBone* srcaplybone);

	//2023/10/17 ChaCalcFuncに移動
	//bool CalcAxisAndRotForIKRotateAxis(int limitdegflag,
	//	CBone* parentbone, CBone* firstbone,
	//	double curframe, ChaVector3 targetpos,
	//	ChaVector3 srcikaxis,
	//	ChaVector3* dstaxis, float* dstrotrad);


	int CalcAxisAndRotForIKRotate(int limitdegflag,
		CBone* parentbone, CBone* firstbone, 
		double curframe, ChaVector3 targetpos, 
		ChaVector3* dstaxis, float* dstrotrad);
	int CalcAxisAndRotForIKRotateVert(int limitdegflag,
		CBone* parentbone, CBone* firstbone,
		double curframe, ChaVector3 targetpos,
		ChaVector3* dstaxis, float* dstrotrad);

	//2023/10/17 ChaCalcFuncクラスに移動
	//int IKRotateOneFrame(int limitdegflag, CEditRange* erptr,
	//	int keyno, CBone* rotbone, CBone* parentbone,
	//	double curframe, double startframe, double applyframe,
	//	CQuaternion rotq0, bool keynum1flag, bool postflag, bool fromiktarget);

	void ClearIKRotRecReq(CBone* srcbone);
	void ClearIKRotRecUVReq(CBone* srcbone);


	//int GetFreeThreadIndex();
	//void WaitAllTheadOfGetFbxAnim();

	//static unsigned __stdcall ThreadFunc_MP1(void* pArguments);
	//static unsigned __stdcall ThreadFunc_MP2(void* pArguments);
	//static unsigned __stdcall ThreadFunc_MP3(void* pArguments);
	//static unsigned __stdcall ThreadFunc_MP4(void* pArguments);
	//static unsigned __stdcall ThreadFunc_MP5(void* pArguments);
	//static unsigned __stdcall ThreadFunc_MP6(void* pArguments);

	void InitCameraFbx();

	void SetBoneMatrixIndexReq(CBone* srcbone, int* pcount);


	MOTINFO* GetCurMotInfoPtr() {
		//#####################
		//for private function
		//#####################

		if (GetNoBoneFlag() == false) {
			return m_curmotinfo;
		}
		else {
			return 0;
		}
	};
	MOTINFO* GetMotInfoPtr(int srcid) {//motidは1から

		//#####################
		//for private function
		//#####################

		if (GetNoBoneFlag() == false) {
			//DeleteMotion時に要素をeraseするのでid - 1が配列のインデックスになるとは限らない//2021/08/26
			int miindex;
			miindex = MotionID2Index(srcid);
			if (miindex >= 0) {
				return m_motinfo[miindex];
			}
			else {
				_ASSERT(0);
				return 0;
			}
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};



public: //accesser
	FbxManager* GetFBXSDK(){
		return m_psdk;
	};
	int GetModelNo(){ return m_modelno; };
	void SetModelNo( int srcno ){ m_modelno = srcno; };

	bool GetModelDisp(){ return m_modeldisp; };
	void SetModelDisp( bool srcflag ){ m_modeldisp = srcflag; };

	float GetLoadMult(){ return m_loadmult; };
	void SetLoadMult( float srcmult ){ m_loadmult = srcmult; };

	bool GetInView() {
		return m_inview;
	};
	bool GetBefInView() {
		return m_befinview;
	};
	void SetInView(bool srcflag) {
		m_befinview = m_inview;
		m_inview = srcflag;
	};
	bool GetInShadow() {
		return m_inshadow;
	};
	void SetInShadow(bool srcflag) {
		m_inshadow = srcflag;
	};


	const WCHAR* GetFileName(){ return m_filename; };
	const WCHAR* GetDirName(){ return m_dirname; };
	const WCHAR* GetModelFolder(){ return m_modelfolder; };
	const char* GetDefaultReName(){ return m_defaultrename; };
	const char* GetDefaultImpName(){ return m_defaultimpname; };

	int GetMqoObjectSize(){
		return (int)m_object.size();
	};
	CMQOObject* GetMqoObject( int srcobjno ){
		std::map<int, CMQOObject*>::iterator itrobj;
		itrobj = m_object.find(srcobjno);
		if (itrobj != m_object.end()){
			return itrobj->second;
		}
		else{
			return 0;
		}
		//return m_object[ srcobjno ];
	};
	std::map<int,CMQOObject*>::iterator GetMqoObjectBegin(){
		return m_object.begin();
	};
	std::map<int,CMQOObject*>::iterator GetMqoObjectEnd(){
		return m_object.end();
	};
	void SetMqoObject( int srcindex, CMQOObject* srcobj ){
		m_object[ srcindex ] = srcobj;
	};

	CBone* GetBoneByName(std::string srcname ){

		//2023/10/26 Joint有無について高速化
		const char* pjoint = strstr(srcname.c_str(), "_Joint");
		if (pjoint) {
			size_t strjointlen = strlen(pjoint);
			if (strjointlen == strlen("_Joint")) {
				if (m_withStrJoint == 0) {
					return 0;
				}
			}
			else {
				if (m_withoutStrJoint == 0) {
					return 0;
				}
			}
		}
		else {
			if (m_withoutStrJoint == 0) {
				return 0;
			}
		}


		std::map<std::string, CBone*>::iterator itrbone;
		itrbone = m_bonename.find(srcname);
		if (itrbone != m_bonename.end()) {
			return itrbone->second;
		}
		else{
			return 0;
		}
		//return m_bonename[ srcname ];
	};


	//###############################
	//名前が部分一致するボーンを返す
	//###############################
	CBone* FindBoneByPattern(std::string srcpattern)
	{
		std::map<std::string, CBone*>::iterator itrbone;
		for (itrbone = m_bonename.begin(); itrbone != m_bonename.end(); itrbone++) {
			if (strstr(itrbone->first.c_str(), srcpattern.c_str()) != 0) {
				return itrbone->second;
			}
		}
		return 0;
	};


	//#####################################################
	//_Jointが付いている名前と付いていない名前の両方を検索
	//#####################################################
	CBone* FindBoneByName(std::string srcname) {//名前完全一致
		bool srcwithsufix = false;

		char bonename[256] = { 0 };
		char bonename1[256];//_Joint有り
		char bonename2[256];//_Joint無し
		ZeroMemory(bonename, sizeof(char) * 256);
		strcpy_s(bonename, 256, srcname.c_str());
		char* jointnameptr = strstr(bonename, "_Joint");
		if (!jointnameptr) {
			srcwithsufix = false;

			sprintf_s(bonename1, 256, "%s_Joint", bonename);
			strcpy_s(bonename2, 256, bonename);
		}
		else {
			srcwithsufix = true;

			strcpy_s(bonename1, 256, bonename);
			strcpy_s(bonename2, 256, bonename);
			int headleng = (int)(jointnameptr - bonename);
			*(bonename2 + headleng) = 0;
		}

		CBone* retbone = 0;

		//2023/10/23 srcnameに一致する方を先に調べることで速くなる可能性
		if (srcwithsufix == false) {

			if (m_withoutStrJoint > 0) {
				retbone = GetBoneByName(bonename2);
			}
			if (!retbone) {
				if (m_withStrJoint > 0) {
					retbone = GetBoneByName(bonename1);
				}
			}
		}
		else {
			if (m_withStrJoint > 0) {
				retbone = GetBoneByName(bonename1);
			}
			if (!retbone) {
				if (m_withoutStrJoint > 0) {
					retbone = GetBoneByName(bonename2);
				}
			}
		}

		return retbone;
	};

	CBone* GetBoneByWName(WCHAR* srcname){
		if (!srcname){
			return 0;
		}
		size_t namelen = wcslen(srcname);
		if ((namelen > 0) && (namelen < 256)){
			char mbname[1024] = { 0 };
			WideCharToMultiByte(CP_ACP, 0, srcname, -1, mbname, 1024, NULL, NULL);
			return GetBoneByName(mbname);
		}
		else{
			return 0;
		}
	};




	int GetBoneListSize(){//eNull含む
		return (int)m_bonelist.size();
	};
	int GetBoneForMotionSize();//eNull含まない
	int GetMaxBoneNo();

	std::map<int,CBone*>::iterator GetBoneListBegin(){
		return m_bonelist.begin();
	};
	std::map<int,CBone*>::iterator GetBoneListEnd(){
		return m_bonelist.end();
	};
	CBone* GetBoneByID( int srcid ){
		std::map<int, CBone*>::iterator itrbone;
		itrbone = m_bonelist.find(srcid);
		if (itrbone != m_bonelist.end()){
			return itrbone->second;
		}
		else{
			return 0;
		}
		//return m_bonelist[ srcid ];
	};
	CBone* GetBoneByZeroBaseIndex(int srcindex) {
		int bonenum = (int)m_bonelist.size();
		if ((srcindex < 0) || (srcindex >= bonenum)) {
			return 0;
		}
		std::map<int, CBone*>::iterator itrbone;
		int curindex = 0;
		itrbone = m_bonelist.begin();
		while (curindex < srcindex) {
			itrbone++;
			if (itrbone == m_bonelist.end()) {
				return 0;
			}
			curindex++;
		}
		if (itrbone != m_bonelist.end()) {
			return itrbone->second;
		}
		else {
			return 0;
		}
	};


	CBone* DirectGetTopBone()
	{
		return m_topbone;
	}
	CBone* GetTopBone(bool excludenullflag = true);
	void GetTopBoneReq(CBone* srcbone, CBone** pptopbone, bool excludenullflag = true);
	//CBone* GetTopBone() {
	//	return m_topbone;
	//};
	CBone* GetRootBone();
	void GetHipsBoneReq(CBone* srcbone, CBone** dstppbone);
	bool IncludeRootOrReference(FbxNode* ptopnode);
	void GetRootOrReferenceReq(FbxNode* srcnode, FbxNode** dstppnode);
	void CheckVRoidJointNameReq(CBone* srcbone, bool* dstflag);

	CBtObject* GetTopBt(){
		return m_topbt;
	};

	float GetBtGScale(int srcindex){
		if ((srcindex >= 0) && (srcindex < (int)m_rigideleminfo.size())){
			return m_rigideleminfo[srcindex].btgscale;
		}
		else{
			_ASSERT(0);
			return 0.0f;
		}
	};
	void SetBtGScale( float srcval, int srcindex ){
		if ((srcindex >= 0) && (srcindex < (int)m_rigideleminfo.size())){
			m_rigideleminfo[srcindex].btgscale = srcval;
		}
		else{
			_ASSERT(0);
		}
	};

	int GetMotInfoSize(){
		if (GetNoBoneFlag() == false) {
			return (int)m_motinfo.size();
		}
		else {
			return 0;
		}
	};
	MOTINFO GetMotInfoByIndex(int srcindex)//by array index
	{
		if ((srcindex >= 0) && (srcindex < GetMotInfoSize())) {
			if (m_motinfo[srcindex]) {
				return *m_motinfo[srcindex];
			}
			else {
				//_ASSERT(0);
				//abort();//abortしないで　呼び出し側でmotid > 0チェックをすることにした
				MOTINFO dummymi;
				dummymi.Init();
				return dummymi;
			}
		}
		else {
			//_ASSERT(0);
			//abort();//abortしないで　呼び出し側でmotid > 0チェックをすることにした
			MOTINFO dummymi;
			dummymi.Init();
			return dummymi;
		}
	}
	MOTINFO GetMotInfo(int srcid){//motidは1から
		//return m_motinfo[srcid - 1];

		if (GetNoBoneFlag() == false) {
			//DeleteMotion時に要素をeraseするのでid - 1が配列のインデックスになるとは限らない//2021/08/26
			int miindex;
			miindex = MotionID2Index(srcid);
			if ((miindex >= 0) && m_motinfo[miindex]) {
				return *m_motinfo[miindex];
			}
			else {
				//_ASSERT(0);
				//abort();//abortしないで　呼び出し側でmotid > 0チェックをすることにした
				MOTINFO dummymi;
				dummymi.Init();
				return dummymi;
			}
		}
		else {
			//_ASSERT(0);
			//abort();//abortしないで　呼び出し側でmotid > 0チェックをすることにした
			MOTINFO dummymi;
			dummymi.Init();
			return dummymi;
		}
	};

	int SetMotInfo(int srcid, MOTINFO srcmi)
	{
		if (GetNoBoneFlag() == false) {
			//DeleteMotion時に要素をeraseするのでid - 1が配列のインデックスになるとは限らない//2021/08/26
			int miindex;
			miindex = MotionID2Index(srcid);
			if ((miindex >= 0) && m_motinfo[miindex]) {
				MoveMemory(m_motinfo[miindex], &srcmi, sizeof(MOTINFO));
				return 0;
			}
			else {
				_ASSERT(0);
				abort();
				return 1;
			}
		}
		else {
			_ASSERT(0);
			abort();//!!!!!!!!!!!!!!!!
			return 1;
		}
	}
	int SetMotInfoCurFrameByIndex(int srcindex, double srcval)
	{
		if ((srcindex >= 0) && (srcindex < GetMotInfoSize())) {
			if (m_motinfo[srcindex]) {
				m_motinfo[srcindex]->curframe = srcval;
				return 0;
			}
			else {
				_ASSERT(0);
				abort();//!!!!!!!!!!!!!!!!
				return 1;
			}
		}
		else {
			_ASSERT(0);
			abort();//!!!!!!!!!!!!!!!!
			return 1;
		}
	}
	int SetMotInfoFrameLengByIndex(int srcindex, double srcval)
	{
		if ((srcindex >= 0) && (srcindex < GetMotInfoSize())) {
			if (m_motinfo[srcindex]) {
				m_motinfo[srcindex]->frameleng = srcval;
				return 0;
			}
			else {
				_ASSERT(0);
				abort();//!!!!!!!!!!!!!!!!
				return 1;
			}
		}
		else {
			_ASSERT(0);
			abort();//!!!!!!!!!!!!!!!!
			return 1;
		}
	}
	int SetMotInfoMotNameByIndex(int srcindex, char* srcname)
	{
		if (!srcname) {
			_ASSERT(0);
			return 1;
		}
		if ((srcindex >= 0) && (srcindex < GetMotInfoSize())) {
			if (m_motinfo[srcindex]) {
				strcpy_s(m_motinfo[srcindex]->motname, 256, srcname);
				return 0;
			}
			else {
				_ASSERT(0);
				abort();//!!!!!!!!!!!!!!!!
				return 1;
			}
		}
		else {
			_ASSERT(0);
			abort();//!!!!!!!!!!!!!!!!
			return 1;
		}
	}
	int SetMotInfoEngMotNameByIndex(int srcindex, char* srcname)
	{
		if (!srcname) {
			_ASSERT(0);
			return 1;
		}
		if ((srcindex >= 0) && (srcindex < GetMotInfoSize())) {
			if (m_motinfo[srcindex]) {
				strcpy_s(m_motinfo[srcindex]->engmotname, 256, srcname);
				return 0;
			}
			else {
				_ASSERT(0);
				abort();//!!!!!!!!!!!!!!!!
				return 1;
			}
		}
		else {
			_ASSERT(0);
			abort();//!!!!!!!!!!!!!!!!
			return 1;
		}
	}
	int SetMotInfoLoopFlagByIndex(int srcindex, int srcloopflag)
	{
		if ((srcindex >= 0) && (srcindex < GetMotInfoSize())) {
			if (m_motinfo[srcindex]) {
				m_motinfo[srcindex]->loopflag = srcloopflag;
				return 0;
			}
			else {
				_ASSERT(0);
				abort();//!!!!!!!!!!!!!!!!
				return 1;
			}
		}
		else {
			_ASSERT(0);
			abort();//!!!!!!!!!!!!!!!!
			return 1;
		}
	}

	//std::map<int,MOTINFO*>::iterator GetMotInfoBegin(){
	//	return m_motinfo.begin();
	//};
	//std::map<int,MOTINFO*>::iterator GetMotInfoEnd(){
	//	return m_motinfo.end();
	//};


	int GetCurrentMotion();

	MOTINFO GetCurMotInfo(){
		if (GetNoBoneFlag() == false) {
			if (m_curmotinfo) {
				return *m_curmotinfo;
			}
			else {
				MOTINFO dummymi;
				dummymi.Init();
				return dummymi;
			}
		}
		else {
			MOTINFO dummymi;
			dummymi.Init();
			return dummymi;
		}
	};
	void SetCurMotInfo( MOTINFO* srcinfo ){
		if (m_curmotinfo) {
			m_curmotinfo = srcinfo;
		}
	};
	MOTINFO GetFirstValidMotInfo()
	{
		if (GetNoBoneFlag() == false) {
			MOTINFO retmi;
			retmi.Init();
			std::map<int, MOTINFO*>::iterator itrmi;
			for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++) {
				MOTINFO* curmi = itrmi->second;
				if (curmi) {//NULLではないとき
					retmi = *curmi;
					break;
				}
			}
			return retmi;
		}
		else {
			MOTINFO dummymi;
			dummymi.Init();
			return dummymi;
		}
	};


	void SetCameraMotion(int srcmotid) {
		MOTINFO* curmi = GetMotInfoPtr(srcmotid);
		if (curmi) {
			curmi->cameramotion = true;
		}
		else {
			_ASSERT(0);
		}
	};
	bool IsCameraMotion(int srcmotid) {
		MOTINFO* curmi = GetMotInfoPtr(srcmotid);
		if (curmi && curmi->cameramotion) {
			return true;
		}
		else {
			return false;
		}
	};
	bool IsCameraLoaded()
	{
		return m_camerafbx.IsLoaded();
	};
	int GetCameraMotInfoSize() {
		int retsize = 0;
		std::map<int, MOTINFO*>::iterator itrmi;
		for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++) {
			MOTINFO* chkmi = itrmi->second;
			if (chkmi && chkmi->cameramotion) {
				retsize++;
			}
		}
		return retsize;
	};
	MOTINFO* GetCameraMotInfoByCameraIndex(int srcindex) {//番目のカメラモーション
		MOTINFO* retmi = 0;
		int count = 0;
		std::map<int, MOTINFO*>::iterator itrmi;
		for (itrmi = m_motinfo.begin(); itrmi != m_motinfo.end(); itrmi++) {
			MOTINFO* chkmi = itrmi->second;
			if (chkmi && chkmi->cameramotion) {
				if (count == srcindex) {
					retmi = chkmi;
					break;
				}
				count++;//cameraのときだけカウントを増やす
			}
		}
		return retmi;
	};
	MOTINFO* GetCameraMotInfoByMotId(int srcmotid) {//カメラモーション
		//DeleteMotion時に要素をeraseするのでid - 1が配列のインデックスになるとは限らない//2021/08/26
		int miindex;
		miindex = MotionID2Index(srcmotid);
		if (miindex >= 0) {
			MOTINFO* chkmi = m_motinfo[miindex];
			if (chkmi && chkmi->cameramotion) {
				return chkmi;
			}
			else {
				//not camera
				//_ASSERT(0);
				return 0;
			}
		}
		else {
			//invalid id
			return 0;
		}
	};


	CRigidElem* GetRigidElem(int srcboneno);
	CRigidElem* GetRgdRigidElem(int srcrgdindex, int srcboneno);

	int GetRigidElemInfoSize(){
		return (int)m_rigideleminfo.size();
	};
	REINFO GetRigidElemInfo( int srcindex ){
		if ((srcindex >= 0) && (srcindex < m_rigideleminfo.size())) {
			return m_rigideleminfo[srcindex];
		}
		else {
			_ASSERT(0);

			REINFO errorinfo;
			errorinfo.Init();
			strcpy_s(errorinfo.filename, MAX_PATH, "error");
			return errorinfo;
		}
		
	};
	void PushBackRigidElemInfo( REINFO srcinfo ){
		m_rigideleminfo.push_back( srcinfo );
	};
	void SetRigidElemInfo( int srcindex, REINFO srcinfo ){
		m_rigideleminfo[ srcindex ] = srcinfo;
	};

	int GetImpInfoSize(){
		return (int)m_impinfo.size();
	};
	std::string GetImpInfo( int srcindex ){
		return m_impinfo[ srcindex ];
	};
	std::string GetCurImpName(){
		if ((m_curimpindex >= 0) && (m_curimpindex < m_impinfo.size())) {
			std::string curimpname = m_impinfo[m_curimpindex];
			return curimpname;
		}
		else {
			_ASSERT(0);
			std::string strerror = "error";
			return strerror;
		}
	};
	void PushBackImpInfo( std::string srcname )
	{
		m_impinfo.push_back( srcname );
	};


	int GetCurReIndex(){
		return m_curreindex;
	};

	int GetCurImpIndex(){
		return m_curimpindex;
	};
	void SetCurImpIndex( int srcindex ){
		m_curimpindex = srcindex;
	};


	int GetRgdIndex(){
		return m_rgdindex;
	};
	void SetRgdIndex( int srcindex ){
		m_rgdindex = srcindex;
	};

	int GetRgdMorphIndex(){
		return m_rgdmorphid;
	};
	void SetRgdMorphIndex( int srcindex ){
		m_rgdmorphid = srcindex;
	};

	float GetTmpMotSpeed(){
		return m_tmpmotspeed;
	};
	void SetTmpMotSpeed(float srcval) {
		m_tmpmotspeed = srcval;
	};


	int GetMQOMaterialSize();
	CMQOMaterial* GetMQOMaterial( int srcmatno );
	int ExistMQOMaterial(CMQOMaterial* srcmat);
	void SetMQOMaterial(const char* srcname, CMQOMaterial* srcmat);
	CMQOMaterial* GetMQOMaterialByName(std::string srcname );//マテリアル名
	CMQOMaterial* GetMQOMaterialByIndex(int srcmatindex);//配列インデックス
	CMQOMaterial* GetMQOMaterialByMaterialNo(int srcmatno);//マテリアル番号


	CMQOObject* GetMQOObjectByName(const char* findpattern);

	FbxScene* GetScene(){
		return m_pscene;
	};

	FbxString* GetAnimStackName( int srcindex ){
		if ((srcindex >= 0) && (srcindex < mAnimStackNameArray.Size())) {
			return mAnimStackNameArray[srcindex];
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};

	int GetBtCnt(){
		return m_btcnt;
	};
	void PlusPlusBtCnt(){
		m_btcnt++;
	};
	void ZeroBtCnt(){
		m_btcnt = 0;
	}

	void SetBtWorld( btDynamicsWorld* srcworld ){
		m_btWorld = srcworld;
	};
	btDynamicsWorld* GetBtWorld()
	{
		return m_btWorld;
	};

	int GetOldAxisFlagAtLoading()
	{
		return m_oldaxis_atloading;
	};
	void SetOldAxisFlagAtLoading(int srcflag)
	{
		m_oldaxis_atloading = srcflag;
	};

	CMQOObject* GetObjectByName(std::string strname)
	{
		std::map<std::string, CMQOObject*>::iterator itrobjname;
		itrobjname = m_objectname.find(strname);
		if (itrobjname == m_objectname.end()){
			return 0;
		}
		else{
			return itrobjname->second;
		}
	};

	bool GetCreateBtFlag()
	{
		return m_createbtflag;
	};
	void SetCreateBtFlag(bool srcflag)
	{
		m_createbtflag = srcflag;
	};
	bool GetLoadedFlag()
	{
		return m_loadedflag;
	};
	void SetLoadedFlag(bool srcflag)
	{
		m_loadedflag = srcflag;
	};
	int GetInitAxisMatX(){
		return m_initaxismatx;
	};
	void SetInitAxisMatX(int srcval){
		m_initaxismatx = srcval;
	};

	void SetModelPosition(ChaVector3 srcpos) {
		m_modelposition = srcpos;//読み込み時位置
	};
	ChaVector3 GetModelPosition() {
		return m_modelposition;//読み込み時位置
	};
	void SetModelRotation(ChaVector3 srcdir) {
		m_modelrotation = srcdir;
	};
	ChaVector3 GetModelRotation() {
		return m_modelrotation;
	};
	void CalcModelWorldMatOnLoad();


	//void SetWorldMatFromCamera(ChaMatrix srcmat){
	//	m_worldmat = srcmat;
	//	m_worldmat.data[MATI_41] = m_modelposition.x;
	//	m_worldmat.data[MATI_42] = m_modelposition.y;
	//	m_worldmat.data[MATI_43] = m_modelposition.z;
	//};


	//###########################################
	//使うのはm_matWorld. m_worldmatではなくて.
	//###########################################
	//void SetWorldMat(ChaMatrix srcmat) {
	//	m_worldmat = srcmat;
	//};
	//ChaMatrix GetWorldMat()
	//{
	//	return m_worldmat;
	//	//m_worldmat = CalcModelWorldMatOnLoad();
	//	//return m_worldmat;
	//};
	ChaMatrix GetWorldMat()
	{
		return m_matWorld;
	};
	ChaMatrix GetViewMat()
	{
		return m_matView;
	};
	ChaMatrix GetProjMat()
	{
		return m_matProj;
	};
	ChaMatrix GetVPMat()
	{
		return m_matVP;
	};

	int GetFbxComment(char* dstcomment, int dstlen)
	{
		if (!dstcomment) {
			_ASSERT(0);
			return 1;
		}
		if ((dstlen < 0) || (dstlen > MAX_PATH)) {
			_ASSERT(0);
			return 1;
		}
		strcpy_s(dstcomment, dstlen, m_fbxcomment.Buffer());
		return 0;
	}

	FbxNode* GetBoneNode(CBone* curbone) {
		if (!curbone) {
			return 0;
		}
		else {
			FbxNode* pNode = m_bone2node[curbone];
			return pNode;
		}
	}

	int GetHasBindPose() {
		return m_hasbindpose;
	}
	void SetHasBindPose(int srcval) {
		m_hasbindpose = srcval;
	}

	bool GetFromNoBindPoseFlag() {
		return m_fromNoBindPose;
	}
	void SetFromNoBindPoseFlag(bool srcflag) {
		m_fromNoBindPose = srcflag;
	}

	bool GetFromBvhFlag() {
		return m_fromBvh;
	}
	void SetFromBvhFlag(bool srcflag) {
		m_fromBvh = srcflag;
	}

	int GetCurrentUndoR() {
		return m_undo_readpoint;
	}
	int GetCurrentUndoW() {
		return m_undo_writepoint;
	}
	int GetCurrentUndo1st() {
		return m_undo_firstflag;
	}

	int GetLoadingMotionCount() {
		return m_loadingmotionnum;
	}
	void SetLoadingMotionCount(int srcval) {
		m_loadingmotionnum = srcval;
	}

	FbxAnimLayer* GetCurrentAnimLayer() {
		return m_currentanimlayer;
	}
	void SetCurrentAnimLayer(FbxAnimLayer* currentanimlayer) {
		m_currentanimlayer = currentanimlayer;
	}

	ChaVector4 GetMaterialDispRate()
	{
		return m_materialdisprate;
	}
	float GetDiffuseDispRate()
	{
		return m_materialdisprate.x;
	}
	float GetSpecularDispRate()
	{
		return m_materialdisprate.y;
	}
	float GetEmissiveDispRate()
	{
		return m_materialdisprate.z;
	}
	float GetAmbientDispRate()
	{
		return m_materialdisprate.w;
	}

	void SetMaterialDispRate(ChaVector4 srcrate)
	{
		m_materialdisprate = srcrate;
	}
	void SetDiffuseDispRate(float srcval)
	{
		m_materialdisprate.x = srcval;
	}
	void SetSpecularDispRate(float srcval)
	{
		m_materialdisprate.y = srcval;
	}
	void SetEmissiveDispRate(float srcval)
	{
		m_materialdisprate.z = srcval;
	}
	void SetAmbientDispRate(float srcval)
	{
		m_materialdisprate.w = srcval;
	}
	CNodeOnLoad* GetNodeOnLoad()
	{
		return m_nodeonload;
	}
	void SetNoBoneFlag(bool srcflag)
	{
		m_noboneflag = srcflag;
	}
	bool GetNoBoneFlag()
	{
		return m_noboneflag;
	}


	//CCameraFbx GetCameraFbx();
	void SetCameraMotionId(int srcid);
	int GetCameraMotionId();


	void SetENullTime(double srcenulltime)
	{
		//SetShaderConstにおける　ボーンの無いメッシュアニメ enull evaluate用の時間
		m_enulltime = srcenulltime;
	};
	double GetENullTime()
	{
		return m_enulltime;
	}

	void ClearIKStopName()
	{
		m_ikstopname.clear();
	}
	void AddIKStopName(const char* srcname)
	{
		if (!srcname) {
			_ASSERT(0);
			return;
		}
		size_t len = strlen(srcname);
		if ((len == 0) || (len >= 256)) {
			_ASSERT(0);
			return;
		}
		m_ikstopname.push_back(srcname);

	}
	int GetIKStopNameSize()
	{
		return (int)m_ikstopname.size();
	}
	std::string GetIKStopName(int srcindex, int* perror)
	{
		if (!perror) {
			_ASSERT(0);
			return std::string("error!!!");
		}
		*perror = 0;

		int num = GetIKStopNameSize();
		if (num <= 0) {
			*perror = 1;
			return std::string("none!!!");
		}
		if ((srcindex >= 0) && (srcindex < num)) {
			*perror = 0;
			return m_ikstopname[srcindex];
		}
		else {
			*perror = 2;
			return std::string("outofrange!!!");
		}
	}
	bool IsIKStopName(const char* srcname)
	{
		if (!srcname) {
			_ASSERT(0);
			return false;
		}
		size_t len = strlen(srcname);
		if ((len == 0) || (len >= 256)) {
			_ASSERT(0);
			return false;
		}

		int stopnamesize = GetIKStopNameSize();
		int cmpno;
		for (cmpno = 0; cmpno < stopnamesize; cmpno++) {
			int errorno = 0;
			std::string cmpname = GetIKStopName(cmpno, &errorno);
			if (errorno == 0) {
				if (strstr(srcname, cmpname.c_str()) != 0) {
					return true;
				}
			}
		}

		return false;
	}

	int GetDispGroupForRender(int srcgroupindex, std::vector<DISPGROUPELEM>& dstvec)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			dstvec = m_dispgroup[srcgroupindex];
			return 0;
		}
		else {
			dstvec.clear();
			return 1;
		}
	}
	void SetDispGroup(int srcgroupindex, const char* srcnodename)
	{
		if (!srcnodename) {
			_ASSERT(0);
			return;
		}

		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			std::map<int, DISPGROUPELEM>::iterator itrdigelem;
			for (itrdigelem = m_objno2digelem.begin(); itrdigelem != m_objno2digelem.end(); itrdigelem++) {
				DISPGROUPELEM digelem = itrdigelem->second;
				if (digelem.pNode) {
					char chknodename[256] = { 0 };
					strcpy_s(chknodename, 256, digelem.pNode->GetName());
					if (strcmp(chknodename, srcnodename) == 0) {//名前が一致
						itrdigelem->second.groupno = srcgroupindex + 1;//groupno = groupindex + 1
					}
				}
			}
		}
		else {
			_ASSERT(0);
		}
	}
	void SetDispGroup(int srcgroupindex, int srcobjno)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			std::map<int, DISPGROUPELEM>::iterator itrdigelem;
			itrdigelem = m_objno2digelem.find(srcobjno);
			if (itrdigelem != m_objno2digelem.end()) {
				itrdigelem->second.groupno = srcgroupindex + 1;//groupno = groupindex + 1
			}
		}
		else {
			_ASSERT(0);
		}
	}
	bool GetDispGroupON(int srcgroupindex)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			return m_dispgroupON[srcgroupindex];
		}
		else {
			_ASSERT(0);
			return false;
		}
	}
	void SetDispGroupON(int srcgroupindex, bool srcflag)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			m_dispgroupON[srcgroupindex] = srcflag;
		}
		else {
			_ASSERT(0);
		}
	}
	bool DispGroupEmpty(int srcgroupindex)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			return (m_dispgroup[srcgroupindex]).empty();
		}
		else {
			_ASSERT(0);
			return true;
		}
	}
	int GetDispGroupSize(int srcgroupindex)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			return (int)m_dispgroup[srcgroupindex].size();
		}
		else {
			_ASSERT(0);
			return 0;
		}		
	}
	CMQOObject* GetDispGroupMQOObject(int srcgroupindex, int srcelemno)
	{
		if ((srcgroupindex >= 0) && (srcgroupindex < MAXDISPGROUPNUM)) {
			int elemnum = (int)m_dispgroup[srcgroupindex].size();
			if ((srcelemno >= 0) && (srcelemno < elemnum)) {
				return (m_dispgroup[srcgroupindex])[srcelemno].mqoobject;
			}
			else {
				_ASSERT(0);
				return 0;
			}
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}



	void AddLaterTransparent(std::string srctexname)
	{
		m_latertransparent.push_back(srctexname);
	}
	int GetLaterTransparentNum()
	{
		return (int)m_latertransparent.size();
	}
	std::string GetLaterTransparent(int srcindex)
	{
		if ((srcindex >= 0) && (srcindex < GetLaterTransparentNum())) {
			return m_latertransparent[srcindex];
		}
		else {
			_ASSERT(0);
			std::string strerror = "indexError";
			return strerror;
		}
	}

	int GetIKTargetBoneVecSize()
	{
		return (int)m_iktargetbonevec.size();
	}
	CBone* GetIKTargetBone(int srcindex)
	{
		int leng = GetIKTargetBoneVecSize();
		if ((srcindex >= 0) && (srcindex < leng)) {
			return m_iktargetbonevec[srcindex];
		}
		else {
			return 0;
		}
	}

	void SetUnderMotion2Bt(bool srcflag)
	{
		m_Under_Motion2Bt = srcflag;
	}
	bool GetUnderMotion2Bt()
	{
		return m_Under_Motion2Bt;
	}
	void SetUnderBt2Motion(bool srcflag)
	{
		m_Under_Bt2Motion = srcflag;
	}
	bool GetUnderBt2Motion()
	{
		return m_Under_Bt2Motion;
	}
	void SetUnderIKRot(bool srcflag)
	{
		m_Under_IKRot = srcflag;
	}
	bool GetUnderIKRot()
	{
		return m_Under_IKRot;
	}
	void SetUnderIKRotApplyFrame(bool srcflag)
	{
		m_Under_IKRotApplyFrame = srcflag;
	}
	bool GetUnderIKRotApplyFrame()
	{
		return m_Under_IKRotApplyFrame;
	}
	void SetUnderCalcEul(bool srcflag)
	{
		m_Under_CalcEul = srcflag;
	}
	bool GetUnderCalcEul()
	{
		return m_Under_CalcEul;
	}
	void SetUnderCopyW2LW(bool srcflag)
	{
		m_Under_CopyW2LW = srcflag;
	}
	bool GetUnderCopyW2LW()
	{
		return m_Under_CopyW2LW;
	}
	void SetUnderPostFKTra(bool srcflag)
	{
		m_Under_PostFKTra = srcflag;
	}
	bool GetUnderPostFKTra()
	{
		return m_Under_PostFKTra;
	}
	void SetUnderInitMP(bool srcflag)
	{
		m_Under_InitMP = srcflag;
	}
	bool GetUnderInitMP()
	{
		return m_Under_InitMP;
	}
	void SetUnderLoadFbx(bool srcflag)
	{
		m_Under_LoadFbx = srcflag;
	}
	bool GetUnderLoadFbx()
	{
		return m_Under_LoadFbx;
	}
	//void SetUnderPostIK(bool srcflag)
	//{
	//	m_Under_PostIK = srcflag;
	//}
	//bool GetUnderPostIK()
	//{
	//	return m_Under_PostIK;
	//}
	void SetUnderRetarget(bool srcflag)
	{
		m_Under_Retarget = srcflag;
	}
	bool GetUnderRetarget()
	{
		return m_Under_Retarget;
	}
	void SetUnderUpdateTimeline(bool srcflag)
	{
		m_Under_UpdateTimeline = srcflag;
	}
	bool GetUnderUpdateTimeline()
	{
		return m_Under_UpdateTimeline;
	}
	//m_Under_UpdateMatrix;

	CThreadingUpdateMatrix* GetThreadingUpdateMatrix()
	{
		return m_boneupdatematrix;
	}
	int GetThreadingUpdateMatrixNum()
	{
		return m_creatednum_boneupdatematrix;
	}

	int GetLoopStartFlag()
	{
		return m_loopstartflag;
	}
	void SetLoopStartFlag(int srcflag)
	{
		m_loopstartflag = srcflag;
	}

	void ResetUpdateFl4x4Flag();//materialのリセットも行う
	void SetUpdateFl4x4Flag()
	{
		m_updatefl4x4flag = true;
	}
	bool GetUpdateFl4x4Flag()
	{
		return m_updatefl4x4flag;
	}

	void GetBoneMatrix(float* dstfl4x4, int dstmatrixnum) 
	{
		if (!dstfl4x4 || (dstmatrixnum != MAXBONENUM)) {
			_ASSERT(0);
			return;
		}
		MoveMemory(dstfl4x4, m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);
	};

	void SetVRoidJointName(bool srcflag)
	{
		m_vroidjointname = srcflag;
	}
	bool GetVRoidJointName()
	{
		return m_vroidjointname;
	}

	void SetInstancingNum(int srcnum)
	{
		if ((srcnum >= 1) && (srcnum <= RIGMULTINDEXMAX)) {
			m_instancingnum = srcnum;
		}
		else {
			_ASSERT(0);
		}
	}
	int GetInstancingNum()
	{
		if ((m_instancingnum >= 1) && (m_instancingnum <= RIGMULTINDEXMAX)) {
			return m_instancingnum;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}
	int ResetInstancingParams()
	{
		m_instancingdrawnum = 0;
		ZeroMemory(m_instancingparams, sizeof(INSTANCINGPARAMS) * RIGMULTINDEXMAX);
		return 0;
	}
	int GetInstancingDrawNum()
	{
		return m_instancingdrawnum;
	}
	void SetInstancingDrawNum(int srcnum) 
	{
		if ((srcnum >= 0) && (srcnum <= RIGMULTINDEXMAX)) {
			m_instancingdrawnum = srcnum;
		}
		else {
			_ASSERT(0);
		}
	}
	int SetInstancingParams(int srcinstancingno, ChaMatrix srcwmat, ChaMatrix srcvpmat, ChaVector4 srcdiffusemult)
	{
		//#######################################################################################
		//srcinstancingnoは参考程度の値
		//GetしたCModelのインデックスと　様々なスキップ処理を経てDrawするCModelのインデックスには違いが生じる
		//#######################################################################################

		if ((m_instancingdrawnum >= 0) && (m_instancingdrawnum < RIGMULTINDEXMAX)) {

			INSTANCINGPARAMS params;
			params.Init();

			MoveMemory(&(params.wmat[0]), srcwmat.GetDataPtr(), sizeof(float) * 16);
			MoveMemory(&(params.vpmat[0]), srcvpmat.GetDataPtr(), sizeof(float) * 16);
			params.diffusemult = srcdiffusemult;

			m_instancingparams[m_instancingdrawnum] = params;
			m_instancingdrawnum++;
		}
		else {
			_ASSERT(0);
			return 1;
		}


		return 0;
	}
	INSTANCINGPARAMS* GetInstancingParams()
	{
		return m_instancingparams;
	}

	void SetBindPose(FbxPose* srcpose)
	{
		m_bindpose = srcpose;
	}
	FbxPose* GetBindPose()
	{
		return m_bindpose;
	}

	bool GetRefPosFlag()
	{
		return m_refposflag;
	}

	void SetSkyFlag(bool srcflag)
	{
		m_skyflag = srcflag;
	}
	bool GetSkyFlag()
	{
		return m_skyflag;
	}
	void SetUpdateSlot(int srcslot)
	{
		m_updateslot = srcslot;
	}
	int GetUpdateSlot()
	{
		return m_updateslot;
	}


public:
	//CRITICAL_SECTION m_CritSection_GetGP;
	//FUNCMPPARAMS* m_armpparams[6];
	//HANDLE m_arhthread[6];

	bool m_loadedflag;//初期の読み込み処理が終了したらtrue;
	bool m_modeldisp;//表示するかどうか
	bool m_createbtflag;//CreateBtObjectを読んだことがあればtrue。
	CRITICAL_SECTION m_CritSection_Node;

private:
	bool m_inview;
	bool m_befinview;
	bool m_inshadow;
	ChaFrustumInfo m_frustum;
	MODELBOUND m_bound;

	bool m_updatefl4x4flag;

	int m_vrmtexcount;

	int m_physicsikcnt;
	int m_initaxismatx;
	int m_modelno;//モデルのID
	float m_loadmult;//表示倍率
	int m_oldaxis_atloading;//FBX読み込み時に旧データ互換チェックボックスにチェックをしていたかどうか。
	int m_hasbindpose;

	bool m_fromBvh;
	bool m_fromNoBindPose;

	WCHAR m_filename[MAX_PATH];//モデルファイル名、フルパス。
	WCHAR m_dirname[MAX_PATH];//モデルファイルのディレクトリのパス。ファイル名の手前まで。末尾に\\無し。
	WCHAR m_modelfolder[MAX_PATH];//モデルファイルが入っているディレクトリの名前(パスではない)。*.chaファイルがあるディレクトリの中のFBXが入っているディレクトリの名前。
	char m_defaultrename[MAX_PATH];//RigidEelemファイル*.refのデフォルトのファイル名。
	char m_defaultimpname[MAX_PATH];//インパルスファイル*.impのデフォルトのファイル名。

	std::map<int, CMQOObject*> m_object;//オブジェクト。別の言葉でいうと３Dモデルにおける名前が付けられているパーツや部品。
	std::map<int, CBone*> m_bonelist;//ボーンをボーンIDから検索できるようにしたmap。
	std::map<std::string, CBone*> m_bonename;//ボーンを名前から検索できるようにしたmap。
	std::vector<std::string> m_ikstopname;//2023/07/21 IKStopフラグを設定するジョイントの名前の一部

	CBone* m_topbone;//一番親のボーン。
	CBtObject* m_topbt;//一番親のbullet剛体オブジェクト。
	//float m_btgscale;//bulletの重力に掛け算するスケール。--> m_rigideleminfoのbtgscaleに移動。


	CThreadingUpdateMatrix* m_boneupdatematrix;
	CThreadingLoadFbx* m_LoadFbxAnim;
	CThreadingPostIK* m_PostIKThreads;
	CThreadingCalcEul* m_CalcEulThreads;
	CThreadingFKTra* m_FKTraThreads;
	CThreadingCopyW2LW* m_CopyW2LWThreads;
	CThreadingRetarget* m_RetargetThreads;
	CThreadingInitMp* m_InitMpThreads;
	int m_creatednum_boneupdatematrix;//スレッド数の変化に対応。作成済の数。処理用。
	int m_creatednum_loadfbxanim;//スレッド数の変化に対応。作成済の数。処理用。


	//2023/10/19
	//threadsnumをCoef.hの定数に移動し　4から8に変更　CPUコアの数に応じて調整
	//#define POSTIK_THREADSNUM		8
	//#define CALCEUL_THREADSNUM	8
	//int m_postikthreadsnum;//4
	//int m_calceulthreadsnum;//4


	std::map<int, MOTINFO*> m_motinfo;//モーションのプロパティをモーションIDから検索できるようにしたmap。
	MOTINFO* m_curmotinfo;//m_motinfoの中の現在再生中のMOTINFOへのポインタ。

	std::vector<REINFO> m_rigideleminfo;//剛体設定ファイル*.refのファイル情報のvector。
	std::vector<std::string> m_impinfo;//インパルスファイル*.impのファイル名のvector。

	int m_curreindex;//現在有効になっている剛体ファイルのインデックス。剛体ファイルは複数読み込むことが出来て、カレントを設定できる。
	int m_curimpindex;//現在有効になっているインパルスファイルのインデックス。インパルスファイルは複数読み込むことが出来、カレントを設定できる。
	int m_rgdindex;//ラグドールシミュレーション用の剛体設定ファイルのインデックス。
	int m_rgdmorphid;//ラグドール時に使用するアニメーションへのインデックス。ラグドール時にはモーフアニメを通常アニメ時と変えることが多いためこの設定が存在する。例えばやられ顔のモーフアニメを再生したりする。

	float m_tmpmotspeed;//モーション再生倍率の一時保存用。

	////polymesh3用のマテリアル。polymesh4はmqoobjectのmqomaterialを使用する。
	//std::map<int, CMQOMaterial*> m_material;
	//std::map<std::string, CMQOMaterial*> m_materialname;
	TResourceBank<CMQOMaterial> m_materialbank;


	FbxScene* m_pscene;//FBX SDKのシーンへのポインタ。CModel内でアロケート。
	FbxArray<FbxString*> mAnimStackNameArray;//アニメーション名を保存するFBX形式のデータ。
	FbxPose* m_bindpose;

	btDynamicsWorld* m_btWorld;//bulletのシミュレーション用オブジェクトへのポインタ。
	int m_btcnt;//bulletのシミュレーションをしたフレーム数を記録する。初回のシミュレーション時に異なる処理をするために必要である。


//以下、privateかつローカルからしか参照しないデータだからアクセッサーも無し。
	ID3D12Device* m_pdev;//外部メモリ。Direct3DのDevice。
	FbxManager* m_psdk;//外部メモリ。FBX SDKのマネージャ。
	FbxImporter* m_pimporter;//FBX SDKのインポーター。CModel内でアロケート。
	FbxString m_fbxcomment;
	WCHAR m_fbxfullname[MAX_PATH];
	bool m_useegpfile;

	ChaMatrix m_matWorld;//ワールド変換行列。
	ChaMatrix m_matView;
	ChaMatrix m_matProj;
	ChaMatrix m_matVP;//View * Projection 変換行列。

	std::map<CMQOObject*, FBXOBJ*> m_fbxobj;//FbxNodeのラッパークラスとCMQOObjectとのmap。
	std::map<std::string, CMQOObject*> m_objectname;//CMQOObjectを名前で検索するためのmap。


	int (*m_tlFunc)( int srcmotid );//Main.cppに実態があるタイムライン初期化用の関数へのポインタ。データ読み込み時にCModelから初期化関数を呼ぶ。

	mutable FbxTime mStart, mStop, mFrameTime, mFrameTime2;//Fbxでの時間表現。アニメーションの時間(フレーム)指定などに使用。

	std::map<CBone*, FbxNode*> m_bone2node;//ボーンとFbxNodeの対応表。FbxNodeはFBXファイル内のオブジェクトの階層をたどる際などに利用する。
	std::map<CBone*,CBone*> m_rigidbone;//剛体１つはボーン１つに対応している。ボーンは親のジョイントと子供のジョイントからなる。ジョイントとボーンは同じように呼ぶことがある。剛体の親ボーンを子供ボーンからけんさくすることに使用する。

	int m_texpool;//Direct3Dのテクスチャ作成プール（場所）。システムメモリかビデオメモリかマネージドか選ぶ。通常は0でビデオメモリを指定する。
	ChaVector3 m_ikrotaxis;//IK, FKでボーン回転するための回転軸を一時的に保存する。
	CUndoMotion m_undomotion[ UNDOMAX ];//アンドゥー機能のためのCUndoMotionの配列。CUndoMotionの１つのインスタンスは１フレーム分のモーションを保存する。
	//int m_undoid;//アンドゥー用データをリングバッファで使用するための現在位置へのインデックス。
	//int m_undoSavedNum;//保存中のアンドゥーの数
	int m_undo_readpoint;
	int m_undo_writepoint;
	int m_undo_firstflag;

	//ChaMatrix m_worldmat;
	ChaVector3 m_modelposition;//読み込み時位置
	ChaVector3 m_modelrotation;//読み込み時向き

	std::vector<PHYSIKREC> m_physikrec0;
	std::vector<PHYSIKREC> m_physikrec;
	double m_phyikrectime;

	int m_loadingmotionnum;//ローディング中のfbxに含まれるモーションの数

	//float m_setfl4x4[16 * MAXCLUSTERNUM];//SetShaderConst用
	float m_setfl4x4[16 * MAXBONENUM];//SetShaderConst用

	std::vector<CBone*> m_iktargetbonevec;

	FbxAnimLayer* m_currentanimlayer;

	ChaVector4 m_materialdisprate;//diffuse, specular, emissive, ambient

	CNodeOnLoad* m_nodeonload;//CNodeOnLoad of Root Node.
	std::map<FbxNode*, CMQOObject*> m_node2mqoobj;
	std::map<FbxNode*, CBone*> m_node2bone;


	std::map<int, DISPGROUPELEM> m_objno2digelem;
	std::vector<DISPGROUPELEM> m_dispgroup[MAXDISPGROUPNUM];//m_dispgroup[groupno] = vector<objno>
	bool m_dispgroupON[MAXDISPGROUPNUM];
	std::vector<std::string> m_latertransparent;


	bool m_noboneflag;
	CCameraFbx m_camerafbx;
	int m_cameramotionid;

	int m_loadbonecount;//GetFbxAnim用

	double m_enulltime;//SetShaderConstにおける　ボーンの無いメッシュアニメ enull evaluate用の時間


	//2023/10/26
	//GetBoneByName()の高速化のために　_Jointが名前に付いているジョイントと付いていないジョイントの数をセットしておく
	// ０個のときに即座に０リターン
	int m_withStrJoint;
	int m_withoutStrJoint;


	//For High Rpm Threading
	bool m_Under_IKRot;
	bool m_Under_IKRotApplyFrame;
	bool m_Under_CalcEul;
	bool m_Under_CopyW2LW;
	bool m_Under_PostFKTra;
	bool m_Under_InitMP;
	bool m_Under_LoadFbx;
	//bool m_Under_PostIK;
	bool m_Under_Retarget;
	//bool m_Under_UpdateMatrix;
	bool m_Under_UpdateTimeline;
	bool m_Under_Motion2Bt;
	bool m_Under_Bt2Motion;

	int m_loopstartflag;
	bool m_vroidjointname;//J_Bip_C_Hipsという名前のジョイントがあればtrue

	int m_instancingnum;
	int m_instancingdrawnum;
	INSTANCINGPARAMS m_instancingparams[RIGMULTINDEXMAX];

	bool m_refposflag;
	bool m_skyflag;

	int m_updateslot;
};

#endif