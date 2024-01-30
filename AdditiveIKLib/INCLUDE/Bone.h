#ifndef BONEH
#define BONEH

//class ID3D12Device;


//#include <d3dx9.h>
//#include <quaternion.h>

#include <fbxsdk.h>

#include <map>
#include <string>

#include <Coef.h>

#include <orgwindow.h>

#include <ChaVecCalc.h>

#include <MotionPoint.h>
#include "../../MiniEngine/MiniEngine.h"
#include "../../MiniEngine/RenderContext.h"
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
#include "../../MiniEngine/InstancedSprite.h"

#define BONEPOOLBLKLEN	256


class CMQOFace;
class CMotionPoint;
class CRigidElem;
class CBtObject;
class CModel;
class ChaScene;

typedef struct tag_ikrotrec
{
	ChaVector3 targetpos;
	CQuaternion rotq;

	//rotqの回転角度が1e-4より小さい場合にtrue. 
	//ウェイトが小さいフレームにおいても　IKTargetが走るように記録する必要がある
	bool lessthanthflag;

	ChaMatrix applyframemat;
	ChaVector3 applyframeeul;

	void Init() {
		targetpos = ChaVector3(0.0f, 0.0f, 0.0f);
		rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		lessthanthflag = true;
		
		applyframemat.SetIdentity();
		applyframeeul = ChaVector3(0.0f, 0.0f, 0.0f);
	}

	tag_ikrotrec() {
		Init();
	};
}IKROTREC;




class CBone
{
public:

	static void InitColDisp();
	static void DestroyColDisp();
	static void ResetColDispInstancingParams();
	static void RenderColDisp(ChaScene* srcchascene, myRenderer::RenderingEngine* re);

	ChaVector3 m_btparentpos;//Motion2Bt時のボーンの位置(剛体行列計算用)
	ChaVector3 m_btchildpos;//Motion2Bt時のボーンの位置(剛体行列計算用)
	ChaMatrix m_btdiffmat;//Motion2Bt時のbtmatの変化分(剛体行列計算用)


	CBone() {
		InitializeCriticalSection(&m_CritSection_AddMP);
		InitializeCriticalSection(&m_CritSection_GetBefNext);
		InitializeCriticalSection(&m_CritSection_GetBefNext2);
		InitParams();
	};

/**
 * @fn
 * CBone
 * @breaf CBoneのコンストラクタ。
 * @param (CModel* parmodel) IN 親ボーンへのポインタを指定する。
 * @return なし。
 */
	CBone( CModel* parmodel );

/**
 * @fn
 * ~CBone
 * @breaf CBoneのデストラクタ。
 * @return なし。
 */
	~CBone();

/**
 * @fn 
 * SetName
 * @breaf ボーンの名前を設定する。
 * @param (char* srcname) IN 設定したいボーンの名前を指定する。
 * @return 成功したら０。
 * @detail charのボーンの名前はWCHARに変換されてボーンのUNICODE名もセットされる。
 */
	int SetName( const char* srcname );

/**
 * @fn 
 * AddChild
 * @breaf ボーンの階層構造を設定する。
 * @param (CBone* childptr) IN 子供にするCBoneのポインタを指定する。
 * @return 成功したら０。
 * @detail 子供を指定することで階層を設定する。子供だけでなく弟や子供の親の設定もする。
 */
	int AddChild( CBone* childptr );

/**
 * @fn 
 * UpdateMatrix
 * @breaf ボーンの現在の姿勢を計算する。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN モーションのフレーム（時間）を指定する。
 * @param (ChaMatrix* wmat) IN ワールド座標系の変換行列を指定する。
 * @param (ChaMatrix* vpmat) IN カメラプロジェクション座標系の変換行列を指定する。
 * @return 成功したら０。
 * @detail 指定モーションの指定時間の姿勢を計算する。グローバルな姿勢の計算である。
 */
	int UpdateMatrix(bool limitdegflag, int srcmotid, double srcframe, 
		ChaMatrix* wmat, ChaMatrix* vpmat, bool callingbythread = false, int updateslot = 0);
	//int SwapCurrentMotionPoint();


	//int UpdateMatrixFromEul(int srcmotid, double srcframe, ChaVector3 neweul, ChaMatrix* wmat, ChaMatrix* vpmat);

	//int UpdateLimitedWorldMat(int srcmotid, double srcframe);
	int ClearLimitedWorldMat(int srcmotid, double srcframe);

	//int GetFBXAnim(FbxScene* pscene, int animno, FbxUInt64 nodeindex, int motid, double animleng, bool callingbythread); // default : callingbythread = false
	//int GetFBXAnim(int bvhflag, CBone** bonelist, FbxNode** nodelist, int srcbonenum, int animno, int motid, double animleng, bool callingbythread = false);
	int GetFBXAnim(FbxNode* pNode, int animno, int motid, double animleng, bool callingbythread = false);
	//int AddMotionPointAll(int srcmotid, double animleng);
	
/**
 * @fn
 * AddMotionPoint
 * @breaf 指定モーションの指定時間にMotionPointを追加する。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN モーションのフレーム（時間）を指定する。
 * @param (int* existptr) OUT 追加する前からMotionPointが存在していたら１がセットされる、それ以外の場合は０がセットされる。
 * @return 成功したら追加したCMotionPointのポインタ、失敗時は０。
 * @detail 追加したMotionPointの姿勢はIdentity状態である。
 */
	CMotionPoint* AddMotionPoint( int srcmotid, double srcframe, int* existptr );


/**
 * @fn
 * CalcFBXMotion
 * @breaf 指定モーションの指定時間のポーズ(CMotionPoint)を計算する。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN モーションのフレーム(時間)を指定する。
 * @param (CMotionPoint* dstmpptr) OUT 計算した姿勢がセットされる。
 * @param (int* existptr) OUT 指定時間ちょうどのデータがあれば１、なければ０。無い場合は前後から計算する。
 * @return 成功したら０。
 */
	int CalcFBXMotion(bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* dstmpptr, int* existptr);

/**
 * @fn
 * CalcFBXFrame
 * @breaf 指定時間の前後の姿勢から指定時間の姿勢を計算する。
 * @param (double srcframe) IN 指定時間。
 * @param (CMotionPoint* befptr) IN 指定時間の直前の姿勢。
 * @param (CMotionPoint* nextptr) IN 指定時間の直後の姿勢。
 * @param (int existflag) IN 指定時間ちょうどに姿勢データがあるとき１。この時ちょうどのときの姿勢はbefptrに入っている。
 * @param (CMotionPoint* dstmpptr) OUT 指定時間の姿勢が計算されてセットされる。
 * @return 成功したら０。
 * @detail 指定時間ちょうどにデータがあった場合はそれが結果になる。befptrがない場合はnextptrの姿勢が結果となる。nextptrが無い場合はbefptrの姿勢が結果となる。befとnextがある場合は線形補間する。
 */
	int CalcFBXFrame(bool limitdegflag, double srcframe, CMotionPoint* befptr, CMotionPoint* nextptr, int existflag, CMotionPoint* dstmpptr);

/**
 * @fn
 * GetBefNextMP
 * @breaf 指定モーションの指定時間の直前と直後の姿勢データを取得する。
 * @param (int srcmotid) IN モーションIDを指定する。
 * @param (double srcframe) IN モーションのフレーム（時間）を指定する。
 * @param (CMotionPoint** ppbef) OUT 直前の姿勢データ。
 * @param (CMotionPoint** ppnext) OUT 直後の姿勢データ。
 * @param (int* existptr) OUT 時間ちょうどのデータがあるときに１。
 * @return 成功したら０。
 * @detail existptrの内容が１のとき、ちょうどの時間の姿勢はppbefにセットされる。
 */
	int GetBefNextMP( int srcmotid, double srcframe, CMotionPoint** ppbef, CMotionPoint** ppnext, int* existptr, bool onaddmotion = false );

/**
 * @fn
 * DeleteMotion
 * @breaf 指定したモーションIDのモーションを削除する。
 * @param (int srcmotid) IN モーションのID。
 * @return 成功したら０。
 * @detail 指定したモーションの姿勢データを削除する。
 */
	int DeleteMotion( int srcmotid );

/**
 * @fn
 * DeleteMPOutOfRange
 * @breaf 指定したモーションの長さをはみ出している(時間がはみ出している)姿勢データを削除する。
 * @param (int motid) IN モーションのIDを指定する。
 * @param (double srcleng) IN モーションの長さ(時間)を指定する。
 * @return 成功したら０。
 * @detail モーションの長さを変更するときに範囲外の姿勢を削除するために呼ばれる。
 */
	int DeleteMPOutOfRange( int motid, double srcleng );

/**
 * @fn
 * CalcRigidElemParams
 * @breaf 剛体表示用のデータを剛体のパラメータに従ってスケールするための変換行列を求めてスケールする。
 * @return 成功したら0を返す。
 * @detail 剛体をボーンの位置に表示するために、剛体表示用の形状をスケールするために呼ぶ。剛体はボーンの子供ジョイントと１対１で対応するため、指定にchildboneを使う。
 */
	int CalcRigidElemParams(bool setinstancescale, CBone* childbone, int setstartflag );

/**
 * @fn
 * CalcAxisMat
 * @breaf ボーンの軸のための変換行列を計算する。初期状態がX軸を向いていると仮定して計算する。
 * @param (int firstflag) IN 変換開始の行列を保存したいときに１を指定する。
 * @param (float delta) IN 回転角度をしていする。現在は０でしか使用していない。
 * @return 成功したら０。
 * @detail 計算したデータは、ボーンの位置にマニピュレータを表示するための変換行列に使用する。現在はCalcAxisMatZ関数でボーンの変換行列を計算している。
 */
	//int CalcAxisMat( int firstflag, float delta );
	//float CalcAxisMatX_Manipulator_T(int bindflag, CBone* childbone, ChaMatrix* dstmat, int setstartflag);//ボーン軸がX軸
	//float CalcAxisMatX_Manipulator_NotT(int bindflag, CBone* childbone, ChaMatrix* dstmat, int setstartflag, int buttonflag);//ボーン軸がX軸
	float CalcAxisMatX_Manipulator(bool limitdegflag, int srcboneaxis, int bindflag,
		CBone* childbone, ChaMatrix* dstmat, int setstartflag);//ボーン軸がX軸
	float CalcAxisMatX_RigidBody(bool limitdegflag, bool dir2xflag, int bindflag, 
		CBone* childbone, ChaMatrix* dstmat, int setstartflag);//カプセルデータX軸向き
	float CalcAxisMatX_NodeMat(CBone* childbone, ChaMatrix* dstmat);

/**
 * @fn
 * CalcAxisMatZ
 * @breaf ボーンの親と子供の位置を指定して、その軸の変換行列を計算する。
 * @param (ChaVector3* srccur) IN ボーンの親の座標を指定する。
 * @param (ChaVector3* srcchil) IN ボーンの子供の座標を指定する。
 * @return 成功したら０。
 * @detail 初期状態がZ軸を向いていると仮定して計算する。ボーンの位置にマニピュレータを表示するために使用する。
 */
	int CalcAxisMatZ( ChaVector3* srccur, ChaVector3* srcchil );


/**
 * @fn
 * CreateRigidElem
 * @breaf 剛体シミュレーション用のパラメータを保持するCRigidElemを作成する。指定ボーンに関するCRigidElemを作成する。
 * @param (CBone* parentbone) IN 自分自身とここで指定する親ジョイントで定義されるボーンに関するCRigidElemを作成する。
 * @param (int reflag) IN CRigidElemを作成する場合に１、しない場合に０を指定。
 * @param (std::string rename) IN reflagが１のとき、refファイルのファイル名を指定する。
 * @param (int impflag) IN インパルスを与えるための設定を作成する場合に１、しない場合に０を指定。
 * @param (std::string impname) IN impflagが１のとき、impファイルのファイル名を指定する。
 * @return 成功したら０。
 * @detail インパルス設定データも作成する。
 */
	//int CreateRigidElem( CBone* chil, int reflag, std::string rename, int impflag, std::string impname );
	int CreateRigidElem(CBone* parentbone, int reflag, std::string rename, int impflag, std::string impname);


/**
 * @fn
 * SetCurrentRigidElem
 * @breaf カレント設定として使用するRigidElemをセットする。
 * @param (std::string curname) カレントにしたいrefファイルの名前を指定する。
 * @return 成功したら０。
 */
	int SetCurrentRigidElem( std::string curname );


/**
 * @fn
 * AddBoneTraReq
 * @breaf ボーンの姿勢を指定ベクトルだけ移動する。子供方向に再帰的に計算する。
 * @param (CMotionPoint* parmp) IN ０を指定した時は自分の姿勢を計算する。０以外の親を指定した場合は子供方向に姿勢の変更を伝えていく。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN モーションのフレーム（時間）を指定する。
 * @param (ChaVector3 srctra) IN 移動ベクトルを指定する。
 * @return 計算した姿勢を格納したCMotionPointのポインタを返すが再帰関数であることに注意。ポインタはチェインにセットされたものである。
 * @detail 想定している使い方としては、外部からの呼び出し時にはparmpを０にする。この関数内での再帰呼び出し時にparmpに親をセットする。
 */
	CMotionPoint* AddBoneTraReq(bool limitdegflag, CMotionPoint* parmp, int srcmotid, double srcframe, ChaVector3 srctra, ChaMatrix befparentwm, ChaMatrix newparentwm);


	CMotionPoint* AddBoneScaleReq(bool limitdegflag, CMotionPoint* parmp, int srcmotid, double srcframe, ChaVector3 srcscale, ChaMatrix befparentwm, ChaMatrix newparentwm);

/**
 * @fn
 * RotBoneQReq
 * @breaf ボーンの姿勢を指定クォータニオンの分回転する。子供方向に再帰的に計算する。
 * @param (CMotionPoint* parmp) IN ０を指定した時は自分の姿勢を計算する。０以外の親を指定した場合は子供方向に姿勢の変更を伝えていく。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN モーションのフレーム（時間）を指定する。
 * @param (CQuaternion rotq) IN 回転のためのクォータニオンを指定する。
 * @return 計算した姿勢を格納したCMotionPointのポインタを返すが再帰関数であることに注意。ポインタはチェインにセットされたものである。
 * @detail 想定している使い方としては、外部からの呼び出し時にはparmpを０にする。この関数内での再帰呼び出し時にparmpに親をセットする。
 */
 //##########################################################
 //CBone::RotBoneQReq()
 //引数rotqはグローバル回転　引数traanimはローカル移動アニメ
 //##########################################################
	CMotionPoint* RotBoneQReq(bool limitdegflag, bool infooutflag, CBone* parentbone, int srcmotid, double srcframe, 
		CQuaternion rotq, ChaMatrix srcbefparentwm, ChaMatrix srcnewparentwm, 
		CBone* bvhbone = 0, ChaVector3 traanim = ChaVector3(0.0f, 0.0f, 0.0f));// , int setmatflag = 0, ChaMatrix* psetmat = 0, bool onretarget = false);
	int RotAndTraBoneQReq(bool limitdegflag, int* onlycheckptr, double srcstartframe, bool infooutflag, CBone* parentbone, int srcmotid, double srcframe,
		CQuaternion qForRot, CQuaternion qForHipsRot, bool fromiktarget);

	//directsetで　parentの姿勢を更新　再帰
	void UpdateParentWMReq(bool limitdegflag, bool setbroflag, int srcmotid, double srcframe,
		ChaMatrix oldparentwm, ChaMatrix newparentwm);
	
	//directsetで　ツリーの姿勢を更新　再帰
	void UpdateCurrentWM(bool limitdegflag, int srcmotid, double srcframe,
		ChaMatrix newwm);


	ChaMatrix CalcNewLocalRotMatFromQofIK(bool limitdegflag, 
		int srcmotid, double srcframe, CQuaternion qForRot, 
		ChaMatrix* dstsmat, ChaMatrix* dstrmat, ChaMatrix* dsttanimmat);
	ChaMatrix CalcNewLocalTAnimMatFromSRTraAnim(ChaMatrix srcnewlocalrotmat, 
		ChaMatrix srcsmat, ChaMatrix srcrmat, ChaMatrix srctanimmat, ChaVector3 oneframetraanim);

	int SaveSRT(bool limitdegflag, int srcmotid, double srcframe);

	//CMotionPoint* RotBoneQCurrentReq(bool infooutflag, CBone* parbone, int srcmotid, double srcframe, CQuaternion rotq, CBone* bvhbone = 0, ChaVector3 traanim = ChaVector3(0.0f, 0.0f, 0.0f), int setmatflag = 0, ChaMatrix* psetmat = 0);

/**

*/
	CMotionPoint* RotBoneQOne(bool limitdegflag, CBone* srcparentbone, CMotionPoint* parmp, int srcmotid, double srcframe, ChaMatrix srcmat);

/**
 * @fn
 * PasteRotReq
 * @breaf srcframeの姿勢をdstframeの姿勢にペーストする。子供方向に再帰的に設定する。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN コピー元のフレーム（時間）を指定する。
 * @param (double dstframe) IN コピー先のフレームを指定する。
 * @return dstのCMotionPointのポインタを返す。ただし再帰的にである。
 */
	void PasteRotReq(bool limitdegflag, int srcmotid, double srcframe, double dstframe );


/**
 * @fn
 * SetAbsMatReq
 * @breaf firstframeの姿勢をそのままsrcframeに適用する。子供方向に再帰的に実行する。
 * @param (int broflag) IN このボーンの兄弟ボーンに対しても処理をしたい場合は１を、その他の場合は０を指定する。
 * @param (int srcmotid) IN モーションのIDを指定する。
 * @param (double srcframe) IN 姿勢を適用したいフレームを指定する。
 * @param (double firstframe) IN 編集の開始フレームを指定する。
 * @return 編集が適用されたボーンのCMotionPointのポインタが返される。ただし再帰的にである。
 * @detail この関数は絶対IK機能として呼ばれる。絶対IKと相対IKの説明はMain.cppの冒頭の説明を読むこと。
 */
	CMotionPoint* SetAbsMatReq(bool limitdegflag, int broflag, int srcmotid, double srcframe, double firstframe);


/**
 * @fn
 * DeleteMotionKey
 * @breaf 指定したモーションIDのモーションキーを削除する。
 * @param (int srcmotid) IN モーションのID。
 * @return 成功したら０。
 * @detail 指定したモーションの姿勢データを削除する。
 * @detail DeleteMotionがキーを消した後にモーションのエントリーを削除するのに対し、
 * @detail DestroyMotionKeyはキーを削除した後にモーションのエントリーにNULLをセットする。
 */
	int DestroyMotionKey( int srcmotid );


/**
 * @fn
 * AddBoneMotMark
 * @breaf 姿勢編集マークを付ける。
 * @param (int motid) IN モーションのIDを指定する。
 * @param (OrgWinGUI::OWP_Timeline* owpTimeline) IN タイムラインを指定する。
 * @param (int curlineno) IN タイムラインのライン番号を指定する。
 * @param (double startframe) IN 編集開始フレームを指定する。
 * @param (double endframe) IN 編集終了フレームを指定する。
 * @param (int flag) IN マークにセットするフラグ。通常１を指定。
 * @return 成功したら０。
 * @detail 姿勢編集の開始時と終了時にマークを付け、すでにその間にマークがあればそれを消す。
 */
	int AddBoneMotMark( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double startframe, double endframe, int flag );


/**
 * @fn
 * CalcLocalInfo
 * @breaf 指定モーションの指定時間の姿勢からローカルな回転クォータニオンと移動ベクトルを計算する。
 * @param (int motid) IN モーションのIDを指定する。
 * @param (double frameno) IN モーションのフレーム（時間）を指定する。
 * @param (CMotionPoint* pdstmp) OUT 計算結果を受け取るCMotionPointのポインタを指定する。
 * @return 成功したら０。
 */
	int CalcLocalInfo(bool limitdegflag, int motid, double frameno, CMotionPoint* pdstmp);
	//int CalcCurrentLocalInfo(CMotionPoint* pdstmp);
	int CalcBtLocalInfo(CMotionPoint* pdstmp);

	//int CalcInitLocalInfo(int motid, double frameno, CMotionPoint* pdstmp);
/**
 * @fn
 * GetBoneNum
 * @breaf このボーンを親とするボーンの数を取得する。
 * @return ボーンの数。
 * @detail 子供と子供の兄弟の数の合計を返す。
 */
	int GetBoneNum();

	/**
	* @fn
	* CalcAxisMatX_aft
	* @breaf ボーンの軸のための変換行列を計算する。初期状態がX軸を向いていると仮定して計算する。
	* @param (ChaVector3 curpos) IN ボーンの位置。
	* @param (ChaVector3 childpos) IN 子供のボーンの位置。
	* @param (ChaMatrix* destmat) OUT 変換行列を格納するデータへのポインタ。
	* @return 成功したら０。
	* @detail CalcAxisMatXから呼ばれる。
	*/
	//int CalcAxisMatX_aft(ChaVector3 curpos, ChaVector3 childpos, ChaMatrix* destmat);

	int CalcAxisMatZ_aft(ChaVector3 curpos, ChaVector3 childpos, ChaMatrix* destmat);


	int CalcFirstFrameBonePos(ChaMatrix srcmat);

	int CalcBoneDepth();

	BEFEUL GetBefEul(bool limitdegflag, int srcmotid, double srcframe);
	//ChaVector3 GetUnlimitedBefEul(int srcmotid, double srcframe);
	int GetNotModify180Flag(int srcmotid, double srcframe);
	ChaVector3 CalcLocalEulXYZ(bool limitdegflag, int axiskind, int srcmotid, double srcframe, tag_befeulkind befeulkind, ChaVector3* directbefeul = 0);//axiskind : BONEAXIS_*  or  -1(CBone::m_anglelimit.boneaxiskind)
	//ChaVector3 CalcLocalUnlimitedEulXYZ(int srcmotid, double srcframe);//motion-->anglelimit用
	//ChaVector3 CalcLocalLimitedEulXYZ(int srcmotid, double srcframe);
	//ChaVector3 CalcCurrentLocalEulXYZ(int axiskind, tag_befeulkind befeulkind, ChaVector3* directbefeul = 0);
	//ChaVector3 CalcBtLocalEulXYZ(int axiskind, tag_befeulkind befeulkind, ChaVector3* directbefeul = 0);
	ChaMatrix CalcLocalRotMatFromEul(ChaVector3 srceul, int srcmotid, double srcframe);
	//ChaMatrix CalcCurrentLocalRotMatFromEul(ChaVector3 srceul);
	//ChaVector3 CalcLocalEulAndSetLimitedEul(int srcmotid, double srcframe);

	//manipulator行列の計算には　CalcAxisX_Manipulator()を使う
	//ChaMatrix CalcManipulatorMatrix(int settraflag, int multworld, int srcmotid, double srcframe);
	//ChaMatrix CalcManipulatorPostureMatrix(int calccapsuleflag, int settraflag, int multworld, int calczeroframe);


	ChaVector3 GetWorldPos(bool limitdegflag, int srcmotid, double srcframe);
	//補間無し
	ChaMatrix GetWorldMat(bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* srcmp, ChaVector3* dsteul = 0);
	//ChaMatrix GetLimitedWorldMat(int srcmotid, double srcframe, ChaVector3* dstneweul = 0, int callingstate = 0);
	ChaVector3 GetLocalEul(bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* srcmp);
	ChaVector3 GetLimitedLocalEul(int srcmotid, double srcframe);
	ChaVector3 GetUnlimitedLocalEul(int srcmotid, double srcframe);
	int SetWorldMat(bool limitdegflag, int srcmotid, double srcframe, ChaMatrix srcmat, CMotionPoint* srcmp);
	int SetWorldMat(bool limitdegflag, bool directsetflag, bool infooutflag, int setchildflag, 
		int srcmotid, double srcframe, ChaMatrix srcmat, int onlycheck, bool fromiktarget);
	int SetWorldMatFromEul(bool limitdegflag, int inittraflag, int setchildflag, ChaMatrix befwm, ChaVector3 srceul, int srcmotid, double srcframe, int initscaleflag = 0);
	int SetWorldMatFromEulAndScaleAndTra(bool limitdegflag, int inittraflag, int setchildflag, ChaMatrix befwm, ChaVector3 srceul, ChaVector3 srcscale, ChaVector3 srctra, int srcmotid, double srcframe);
	int SetWorldMatFromEulAndTra(bool limitdegflag, int setchildflag, ChaMatrix befwm, ChaVector3 srceul, ChaVector3 srctra, int srcmotid, double srcframe);
	int SetWorldMatFromQAndTra(bool limitdegflag, int setchildflag, ChaMatrix befwm, CQuaternion axisq, CQuaternion srcq, ChaVector3 srctra, int srcmotid, double srcframe);
	int SetWorldMatFromQAndScaleAndTra(bool limitdegflag, int setchildflag, ChaMatrix befwm, CQuaternion axisq, CQuaternion srcq, ChaVector3 srcscale, ChaVector3 srctra, int srcmotid, double srcframe);
	int SetLocalEul(bool limitdegflag, int srcmotid, double srcframe, ChaVector3 srceul, CMotionPoint* srcmp);
	//int SetLimitedLocalEul(int srcmotid, double srcframe, ChaVector3 srceul);

	int CopyWorldToLimitedWorld(int srcmotid, double srcframe);//制限角度無しの姿勢を制限有りの姿勢にコピーする
	int CopyLimitedWorldToWorld(int srcmotid, double srcframe);//制限角度有りの姿勢を制限無しの姿勢にコピーする
	int ApplyNewLimitsToWM(int srcmotid, double srcframe, ChaMatrix befeditparentmat);//制限角度を変更した後の　Limited再計算



	//補間有り : 覚え方メモ：　計算済(Calclated)のLimitedWMを補間する関数として始まり　その後　未計算時にも対応した
	ChaMatrix GetCurrentWorldMat(bool multmodelwm);
	//ChaMatrix GetCurrentLimitedWorldMat();
	//int GetCalclatedLimitedWM(int srcmotid, double srcframe, ChaMatrix* plimitedworldmat, CMotionPoint** pporgbefmp = 0, int callingstate = 0);


	//ChaMatrix CalcLimitedWorldMat(int srcmotid, double srcframe, ChaVector3* dstneweul);
	//int SetBtWorldMatFromEul(int setchildflag, ChaVector3 srceul);
	ChaMatrix CalcWorldMatFromEul(bool limitdegflag, int inittraflag, int setchildflag, ChaVector3 srceul, int srcmotid, double srcframe, int initscaleflag);//initscaleflag = 1 : default
	//int CalcWorldMatAfterThread(int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat);
	ChaVector3 CalcLocalTraAnim(bool limitdegflag, int srcmotid, double srcframe);
	ChaVector3 CalcLocalScaleAnim(bool limitdegflag, int srcmotid, double srcframe);
	ChaVector3 CalcFbxScaleAnim(bool limitdegflag, int srcmotid, double srcframe);//2022/09/12 fbx書き出し専用
	ChaMatrix CalcLocalScaleRotMat(bool limitdegflag, int rotcenterflag, int srcmotid, double srcframe);
	ChaMatrix CalcLocalSymScaleRotMat(bool limitdegflag, int rotcenterflag, int srcmotid, double srcframe);
	ChaVector3 CalcLocalSymScaleVec(bool limitdegflag, int srcmotid, double srcframe);
	ChaVector3 CalcLocalSymTraAnim(bool limitdegflag, int srcmotid, double srcframe);
	ChaMatrix CalcSymXMat(bool limitdegflag, int srcmotid, double srcframe);
	ChaMatrix CalcSymXMat2(bool limitdegflag, int srcmotid, double srcframe, int symrootmode);
	int PasteMotionPoint(bool limitdegflag, int srcmotid, double srcframe, CMotionPoint srcmp);

	ChaVector3 CalcFBXEulXYZ(bool limitdegflag, int srcmotid, double srcframe);//2022/09/12 fbx書き出し専用
	ChaVector3 CalcFBXTra(bool limitdegflag, int srcmotid, double srcframe);//2022/09/12 fbx書き出し専用
	int QuaternionInOrder(bool limitdegflag, int srcmotid, double srcframe, CQuaternion* srcdstq);
	int CalcNewBtMat(CModel* srcmodel, CBone* childbone, ChaMatrix* dstmat, ChaVector3* dstpos);

	int LoadCapsuleShape(ID3D12Device* pdev);

	int ChkMovableEul(ChaVector3 srceul);


	int SetCurrentMotion(int srcmotid, double animleng);
	void ResetMotionCache();

	ChaMatrix GetCurrentZeroFrameMat(bool limitdegflag, int updateflag);//current motionのframe 0のworldmat
	ChaMatrix GetCurrentZeroFrameInvMat(bool limitdegflag, int updateflag);


	static CBone* GetNewBone(CModel* parmodel);
	static void InvalidateBone(CBone* srcmp);
	static void InitBones();
	static void DestroyBones();
	static void OnDelModel(CModel* parmodel);

	//ChaMatrix CalcParentGlobalMat(int srcmotid, double srcframe);
	//ChaMatrix CalcFirstParentGlobalSRT();
	//ChaMatrix CalcParentGlobalSRT(int srcmotid, double srcframe);

	void InitAddLimitQAll();
	void RotQAddLimitQAll(int srcmotid, double srcframe);


	int CreateIndexedMotionPoint(int srcmotid, double animleng);

	//int ResizeIndexedMotionPoint(int srcmotid, double animleng);
	//void ResizeIndexedMotionPointReq(int srcmotid, double animleng);

	ChaVector3 LimitEul(ChaVector3 srceul);
	//void SetBefWorldMatReq(int srcmotid, double srcframe);

	void InitAngleLimit();
	void SetAngleLimitOff();
	int ResetAngleLimit(int srcval);
	int AngleLimitReplace180to170();
	int AdditiveCurrentToAngleLimit();
	int AdditiveAllMotionsToAngleLimit();
	//int AdditiveToAngleLimit(ChaVector3 cureul);//フルフレーム計算して最大最小をセットするように変更したため　この関数は使用しない

	int InitMP(bool limitdegflag, int srcmotid, double srcframelen);

	//int Adjust180Deg(int srcmotid, double srcleng);

private:

/**
 * @fn
 * InitParams
 * @breaf クラスメンバの初期化をする。
 * @return ０。
 */
	int InitParams();
	int InitParamsForReUse(CModel* srcparmodel);

	int SetParams(CModel* parmodel);//コンストラクタのInitParamsでは足りない部分

/**
 * @fn
 * DestroyObjs
 * @breaf クラスでアロケートしたメモリを開放する。
 * @return ０。
 */
	int DestroyObjs();


/**
 * @fn
 * CalcAxisMatX
 * @breaf ボーンの軸のための変換行列を計算する。ボーンの軸を計算してCalcAxisMatX_aftに渡して計算する。
 * @return 成功したら０。
 * @detail CalcAxisMatから呼ばれる。
 */
	//int CalcAxisMatX();


	int CalcAxisMatZ();


/**
 * @fn
 * CalcAxisMatY
 * @breaf ボーンの軸のための変換行列を計算する。初期状態がZ軸を向いていると仮定して計算する。
 * @param (CBone* childbone) IN 子供のボーン。
 * @param (ChaMatrix* dstmat) OUT 変換行列を格納するデータへのポインタ。
 * @return 成功したら０。
 */
	//int CalcAxisMatY( CBone* childbone, ChaMatrix* dstmat );

/**
 * @fn
 * CalcLocalAxisMat
 * @breaf ローカルなボーン軸の変換行列を計算する。
 * @param (ChaMatrix motmat) IN グローバルな姿勢行列。
 * @param (ChaMatrix axismatpar) IN グローバルなボーン軸変換行列。X軸基準。
 * @param (ChaMatrix gaxisy) IN グローバルなボーン軸変換行列。Y軸基準。
 * @return 成功したら０。
 * @detail 計算結果はGetAxisMatPar()で取得する。
 */
	//int CalcLocalAxisMat( ChaMatrix motmat, ChaMatrix axismatpar, ChaMatrix gaxisy );

/**
 * @fn
 * SetGroupNoByName
 * @breaf 剛体のあたり判定用のグループ番号を設定する。ボーンの名前から判定して設定する。
 * @param (CRigidElem* curre) IN 剛体のCRigidElemへのポインタ。
 * @param (CBone* childbone) IN ボーンのCBoneへのポインタ。
 * @return 成功したら０。
 * @detail BT_が名前につくものにも剛体用のグループ番号が与えられる。それ以外のグループ番号は、name_G_*** 形式の名前で指定できる。
 */
	int SetGroupNoByName( CRigidElem* curre, CBone* childbone );

/**
 * @fn
 * AddBoneMarkIfNot
 * @breaf ボーンの編集マークを付ける。
 * @param (int motid) IN モーションのIDを指定する。
 * @param (OrgWinGUI::OWP_Timeline* owpTimeline) IN タイムラインを指定する。
 * @param (int curlineno) IN タイムラインのライン番号を指定する。
 * @param (double curframe) IN モーションのフレームを指定する。
 * @param (int flag) IN マークにセットする値、通常は１を指定する。
 * @return 成功したら０。
 */
	int AddBoneMarkIfNot( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double curframe, int flag );


/**
 * @fn
 * DelBoneMarkRange
 * @breaf 指定範囲内の編集マークを削除する。
 * @param (int motid) IN モーションのIDを指定する。
 * @param (OrgWinGUI::OWP_Timeline* owpTimeline) IN タイムラインを指定する。
 * @param (int curlineno) IN タイムラインのライン番号を指定する。
 * @param (double startframe) IN 削除開始フレーム。
 * @param (double endframe) IN 削除終了フレーム。
 * @return 戻り値の説明
 */
	int DelBoneMarkRange( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double startframe, double endframe );

	//void SetStartMat2Req();
	//void CalcFirstAxisMatX();
	//void CalcFirstAxisMatZ();

	float LimitAngle(enum tag_axiskind srckind, float srcval);
	int SwapAngleLimitUpperLowerIfRev();
	int InitCustomRig();
	void CalcBtRootDiffMatFunc(CBone* srcbone);

	ChaMatrix GetCurrentZeroFrameMatFunc(bool limitdegflag, int updateflag, int inverseflag);

	//void CalcParentGlobalMatReq(ChaMatrix* dstmat, CBone* srcbone, int srcmotid, double srcframe);
	//void CalcFirstParentGlobalSRTReq(ChaMatrix* dstmat, CBone* srcbone);
	//void CalcParentGlobalSRTReq(ChaMatrix* dstmat, CBone* srcbone, int srcmotid, double srcframe);


	void InitAddLimitQReq(CBone* srcbone);
	void RotQAddLimitQReq(CBone* srcbone, int srcmotid, double srcframe);


public: //accesser
	int GetType(){ return m_type; };
	void SetType( int srctype ){ m_type = srctype; };

	bool IsHipsBone();

	int GetSelectFlag(){ return m_selectflag; };
	void SetSelectFlag( int srcflag ){ m_selectflag = srcflag; };

	int GetBoneCnt(){ return m_bonecnt; };
	void SetBoneCnt( int srccnt ){ m_bonecnt = srccnt; };

	int GetValidFlag(){ return m_validflag; };
	void SetValidFlag( int srcflag ){ m_validflag = srcflag; };

	int GetBoneNo(){ return m_boneno; };
	void SetBoneNo( int srcno ){ m_boneno = srcno; };

	int GetTopBoneFlag(){ return m_topboneflag; };
	void SetTopBoneFlag( int srcflag ){ m_topboneflag = srcflag; };

	const char* GetBoneName(){ return (const char*)m_bonename; };
	void SetBoneName( char* srcname ){ strcpy_s( m_bonename, 256, srcname ); };

	const WCHAR* GetWBoneName(){ return (const WCHAR*)m_wbonename; };
	void SetWBoneName( WCHAR* srcname ){ wcscpy_s( m_wbonename, 256, srcname ); };

	const char* GetEngBoneName(){ return (const char*)m_engbonename; };
	void SetEngBoneName( char* srcname ){ strcpy_s( m_engbonename, 256, srcname ); };

	ChaVector3 GetChildWorld();
	void SetChildWorld( ChaVector3 srcvec ){ m_childworld = srcvec; };

	ChaVector3 GetChildScreen(){ return m_childscreen; };
	void SetChildScreen( ChaVector3 srcvec ){ m_childscreen = srcvec; };

	int GetMotionKeySize(){ return (int)m_motionkey.size(); };
	CMotionPoint* GetMotionKey( int srccookie ){
		if ((srccookie >= 1) && (srccookie <= m_motionkey.size())) {
			return m_motionkey[srccookie - 1];
		}
		else {
			//_ASSERT(0);//OnAddMotionのときには有り得る
			return 0;
		}
	};
	void SetMotionKey( int srccookie, CMotionPoint* srcmk ){ 
		if ((srccookie >= 1) && (srccookie <= m_motionkey.size())) {
			m_motionkey[srccookie - 1] = srcmk;
		}
		else {
			_ASSERT(0);
		}
	};

	int GetInitIndexedMotionPointSize()
	{
		return (int)m_initindexedmotionpoint.size();
	}
	bool ExistInitIndexedMotionPoint(int srcmotid)
	{
		bool existflag = false;
		std::map<int, bool>::iterator itrinitflag;
		itrinitflag = m_initindexedmotionpoint.find(srcmotid);//initflag
		if (itrinitflag == m_initindexedmotionpoint.end()) {//エントリーがまだ無いとき
			existflag = false;
		}
		else {
			if (itrinitflag->second == false) {//初期化フラグがfalseのとき　
				existflag = false;
			}
			else {
				existflag = true;
			}
		}
		return existflag;
	}
	int GetIndexedMotionPointFrameLeng(int srcmotid)
	{
		int mpmapleng = 0;
		std::map<int, std::vector<CMotionPoint*>>::iterator itrvecmpmap;

		itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
		if (itrvecmpmap == m_indexedmotionpoint.end()) {
			return 0;
		}
		else {
			mpmapleng = (int)itrvecmpmap->second.size();
		}
		return mpmapleng;
	}

	void GetIndexedMotionPointVec(int srcmotid, std::vector<CMotionPoint*>& dstvec)
	{
		std::map<int, std::vector<CMotionPoint*>>::iterator itrvecmpmap;
		itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
		if (itrvecmpmap == m_indexedmotionpoint.end()) {
			dstvec.clear();
		}
		else {
			dstvec = itrvecmpmap->second;
		}
	}

	CMotionPoint* GetIndexedMotionPoint(int srcmotid, int srcframeindex)
	{
		CMotionPoint* retmp = 0;
		std::map<int, std::vector<CMotionPoint*>>::iterator itrvecmpmap;

		itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
		if (itrvecmpmap == m_indexedmotionpoint.end()) {
			return 0;
		}
		else {
			retmp = (itrvecmpmap->second)[srcframeindex];
		}
		return retmp;
	}

	int GetIndexedMotionPoint3(int srcmotid, int curframeindex, int nextframeindex,
		CMotionPoint** ppcurframemp, CMotionPoint** ppnextframemp, CMotionPoint** ppendframemp,
		int* pmpvecsize)
	{
		if (!ppcurframemp || !ppnextframemp || !ppendframemp || !pmpvecsize) {
			_ASSERT(0);
			return 1;
		}

		*ppcurframemp = 0;
		*ppnextframemp = 0;
		*ppendframemp = 0;
		*pmpvecsize = 0;

		std::map<int, std::vector<CMotionPoint*>>::iterator itrvecmpmap;

		itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
		if (itrvecmpmap == m_indexedmotionpoint.end()) {
			return 1;//error
		}
		else {
			if ((itrvecmpmap->second).empty()) {
				return 2;//empty
			}
			else {
				int vecsize = (int)(itrvecmpmap->second).size();
				*pmpvecsize = vecsize;

				if (vecsize >= 1) {
					if ((curframeindex >= 0) && (curframeindex < vecsize)) {
						*ppcurframemp = (itrvecmpmap->second)[curframeindex];
					}
					else if (curframeindex < 0) {
						*ppcurframemp = (itrvecmpmap->second)[0];
					}
					else {
						*ppcurframemp = (itrvecmpmap->second)[vecsize - 1];
					}

					if ((nextframeindex >= 0) && (nextframeindex < vecsize)) {
						*ppnextframemp = (itrvecmpmap->second)[nextframeindex];
					}
					else if (nextframeindex < 0) {
						*ppnextframemp = (itrvecmpmap->second)[0];
					}
					else {
						*ppnextframemp = (itrvecmpmap->second)[vecsize - 1];
					}

					*ppendframemp = (itrvecmpmap->second)[vecsize - 1];

					return 0;//success
				}
				else {
					return 2;//empty
				}
			}
		}
	}


	CMotionPoint GetCurMp(bool calcslotflag = false) {
		if (calcslotflag == false) {
			//計算済のm_curmpを取得する場合
			int renderslot = (int)(!(m_updateslot != 0));
			return m_curmp[renderslot];
		}
		else {
			//姿勢の計算中にGetCurMpする場合
			return m_curmp[m_updateslot];
		}
	};
	void SetCurMp( CMotionPoint srcmp ){ m_curmp[m_updateslot] = srcmp; };

	//CMotionPoint GetBefMp(){ return m_befmp; };
	//void SetBefMp(CMotionPoint srcmp){ m_befmp = srcmp; };


	CQuaternion GetAxisQ(){ return m_axisq; };
	void SetAxisQ( CQuaternion srcq ){ m_axisq = srcq; };
	ChaMatrix GetLAxisMat(){ return m_laxismat; };

	//ChaMatrix GetAxisMatPar(){ return m_axismat_par; };
	//ChaMatrix GetInvAxisMatPar(){
	//	ChaMatrix invaxis;
	//	ChaMatrixInverse(&invaxis, NULL, &m_axismat_par);
	//	return invaxis;
	//};

	//ChaMatrix GetStartMat2(){ return m_startmat2; };
	//ChaMatrix GetInvStartMat2(){
	//	ChaMatrix retmat;
	//	ChaMatrixInverse(&retmat, NULL, &m_startmat2);
	//	return retmat;
	//};
	//void SetStartMat2(ChaMatrix srcmat){ m_startmat2 = srcmat; };

	int GetGetAnimFlag(){ return m_getanimflag; };
	void SetGetAnimFlag( int srcflag ){ m_getanimflag = srcflag; };

	ChaMatrix GetNodeMat(){ return m_nodemat; };
	ChaMatrix GetInvNodeMat(){
		ChaMatrix retmat;
		ChaMatrixInverse(&retmat, NULL, &m_nodemat);
		return retmat;
	};
	void SetNodeMat( ChaMatrix srcmat ){ m_nodemat = srcmat; };

	ChaMatrix GetNodeAnimMat() { return m_nodeanimmat; };
	ChaMatrix GetInvNodeAnimMat() {
		ChaMatrix retmat;
		ChaMatrixInverse(&retmat, NULL, &m_nodeanimmat);
		return retmat;
	};
	void SetNodeAnimMat(ChaMatrix srcmat) { m_nodeanimmat = srcmat; };


	//NodeOnLoadのGetBindMat, SetBineMatを使う
	//FbxAMatrix GetBindMat() {
	//	return m_bindmat;
	//}
	//void SetBindMat(FbxAMatrix srcmat) { m_bindmat = srcmat; };


	ChaMatrix GetFirstMat(){ return m_firstmat; };
	void SetFirstMat( ChaMatrix srcmat ){ m_firstmat = srcmat; };

	ChaMatrix GetInvFirstMat(){ ChaMatrixInverse(&m_invfirstmat, NULL, &m_firstmat); return m_invfirstmat; };
	void SetInvFirstMat( ChaMatrix srcmat ){ m_invfirstmat = srcmat; };

	ChaMatrix GetInitMat(){ return m_initmat; };
	void SetInitMat(ChaMatrix srcmat){ m_initmat = srcmat; };

	ChaMatrix GetInvInitMat(){ ChaMatrixInverse(&m_invinitmat, NULL, &m_initmat); return m_invinitmat; };
	void SetInvInitMat(ChaMatrix srcmat){ m_invinitmat = srcmat; };

	ChaMatrix GetTmpMat(){ return m_tmpmat; };
	void SetTmpMat(ChaMatrix srcmat){ m_tmpmat = srcmat; };

	CQuaternion GetTmpQ(){ return m_tmpq; };
	void SetTmpQ(CQuaternion srcq){ m_tmpq = srcq; };


	ChaVector3 GetJointFPos();
	void SetJointFPos(ChaVector3 srcpos);
	void SetOldJointFPos(ChaVector3 srcpos);

	ChaVector3 GetJointWPos(){ return m_jointwpos; };
	void SetJointWPos( ChaVector3 srcpos ){ m_jointwpos = srcpos; };

	FbxAMatrix GetGlobalPosMat(){ return m_globalpos; };
	void SetGlobalPosMat( FbxAMatrix srcmat ){ m_globalpos = srcmat; };

	int GetRigidElemSize(){
		std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator itrremap;
		itrremap = m_remap.find(m_rigidelemname);
		if (itrremap != m_remap.end()){
			return (int)itrremap->second.size();
		}
		else{
			return 0;
		}
	};
		
	CRigidElem* GetRigidElem( CBone* srcbone ){ 
		return GetRigidElemOfMap(m_rigidelemname, srcbone);
	};
	//std::map<CBone*, CRigidElem*>::iterator GetRigidElemMapBegin(){
	//	std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator itrremap;
	//	itrremap = m_remap.find(m_rigidelemname);
	//	if (itrremap != m_remap.end()){
	//		return itrremap->second.begin();
	//	}
	//	else{
	//		return itrremap->second.end();
	//	}
	//};
	//std::map<CBone*, CRigidElem*>::iterator GetRigidElemMapEnd(){
	//	std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator itrremap;
	//	itrremap = m_remap.find(m_rigidelemname);
	//	return itrremap->second.end();
	//};
	void SetRigidElem( CBone* srcbone, CRigidElem* srcre ){ 
		SetRigidElemOfMap(m_rigidelemname, srcbone, srcre);
	};

	//std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator FindRigidElemOfMap(std::string srcname){
	//	return m_remap.find(srcname);
	//};
	int GetReMapSize(){ return (int)m_remap.size(); };
	int GetReMapSize2(std::string srcstring){
		std::map<CBone*, CRigidElem*> curmap = m_remap[ srcstring ];
		return (int)curmap.size();
	};
	CRigidElem* GetRigidElemOfMap( std::string srcstr, CBone* srcbone ){
		std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator itrremap;
		itrremap = m_remap.find(srcstr);
		if (itrremap != m_remap.end()){
			//itrremap->second[srcbone];
			std::map<CBone*, CRigidElem*>::iterator itrmap2;
			itrmap2 = itrremap->second.find(srcbone);
			if (itrmap2 != itrremap->second.end()){
				return itrmap2->second;
			}
			else{
				return 0;
			}
		}
		else{
			return 0;
		}
	};
	//std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator GetRigidElemOfMapBegin(){
	//	return m_remap.begin();
	//};
	//std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator GetRigidElemOfMapEnd(){
	//	return m_remap.end();
	//};

	void SetRigidElemOfMap(std::string srcstr, CBone* srcbone, CRigidElem* srcre);

	std::map<std::string, std::map<CBone*, ChaVector3>>::iterator FindImpMap(std::string srcname){
		return m_impmap.find(srcname);
	};
	int GetImpMapSize(){ return (int)m_impmap.size(); };
	int GetImpMapSize2(std::string srcstring){
		std::map<CBone*, ChaVector3> curmap = m_impmap[ srcstring ];
		return (int)curmap.size();
	};
	std::map<std::string, std::map<CBone*, ChaVector3>>::iterator GetImpMapBegin(){
		return m_impmap.begin();
	};
	std::map<std::string, std::map<CBone*, ChaVector3>>::iterator GetImpMapEnd(){
		return m_impmap.end();
	};
	ChaVector3 GetImpMap( std::string srcstr, CBone* srcbone ){
		std::map<std::string, std::map<CBone*, ChaVector3>>::iterator itrimpmap;
		itrimpmap = m_impmap.find(srcstr);
		if (itrimpmap != m_impmap.end()){
			std::map<CBone*, ChaVector3> curmap = itrimpmap->second;
			std::map<CBone*, ChaVector3>::iterator itrimp;
			itrimp = curmap.find(srcbone);
			if (itrimp != curmap.end()){
				return itrimp->second;
			}
			else{
				//_ASSERT(0);
				return ChaVector3(0.0f, 0.0f, 0.0f);
			}
		}
		else{
			_ASSERT(0);
			return ChaVector3(0.0f, 0.0f, 0.0f);
		}
	};
	void SetImpMap2( std::string srcstring, std::map<CBone*,ChaVector3>& srcmap ){
		m_impmap[ srcstring ] = srcmap;
	};
	void SetImpOfMap( std::string srcstr, CBone* srcbone, ChaVector3 srcimp ){
		std::map<std::string, std::map<CBone*, ChaVector3>>::iterator itrimpmap;
		itrimpmap = m_impmap.find(srcstr);
		if (itrimpmap != m_impmap.end()){
			std::map<CBone*, ChaVector3>::iterator itrsetmap;
			itrsetmap = itrimpmap->second.find(srcbone);
			if (itrsetmap != itrimpmap->second.end()){
				itrsetmap->second = srcimp;
			}
			else{
				itrimpmap->second[srcbone] = srcimp;
			}
		}
		else{
			std::map<CBone*, ChaVector3> newmap;
			newmap[srcbone] = srcimp;
			m_impmap[srcstr] = newmap;
		}
		//((m_impmap[ srcstr ])[ srcbone ]) = srcimp;
	};

	int GetBtKinFlag(){ return m_btkinflag; };
	void SetBtKinFlag( int srcflag ){ m_btkinflag = srcflag; };

	int GetBtForce(){ return m_btforce; };
	void SetBtForce( int srcval ){ m_btforce = srcval; };

	int GetBtObjectSize(){ return (int)m_btobject.size(); };
	CBtObject* GetBtObject( CBone* srcbone ){ return m_btobject[ srcbone ]; };
	std::map<CBone*, CBtObject*>::iterator GetBtObjectMapBegin(){
		return m_btobject.begin();
	};
	std::map<CBone*, CBtObject*>::iterator GetBtObjectMapEnd(){
		return m_btobject.end();
	};

	void SetBtObject( CBone* srcbone, CBtObject* srcbo ){ m_btobject[ srcbone ] = srcbo; };
	void ClearBtObject(){ m_btobject.clear(); };


	int GetMotMarkSize(){ return (int)m_motmark.size(); };
	int GetMotMarkSize2(int srcindex){
		std::map<double, int> curmap = m_motmark[ srcindex ];
		return (int)curmap.size();
	};
	int GetMotMarkOfMap( int srcindex, double srcframe ){
		std::map<double,int> curmap = m_motmark[ srcindex ];
		return curmap[ srcframe ];
	};
	void GetMotMarkOfMap2( int srcindex, std::map<double, int>& dstmap ){
		dstmap = m_motmark[ srcindex ];
	};
	std::map<int, std::map<double, int>>::iterator GetMotMarkOfMapBegin(){
		return m_motmark.begin();
	};
	std::map<int, std::map<double, int>>::iterator GetMotMarkOfMapEnd(){
		return m_motmark.end();
	};
	std::map<int, std::map<double, int>>::iterator FindMotMarkOfMap( int srcindex ){
		return m_motmark.find( srcindex );
	};
	void SetMotMarkOfMap( int srcindex, double srcframe, int srcmark ){
		((m_motmark[ srcindex ])[ srcframe ]) = srcmark;
	};
	void SetMotMarkOfMap2( int srcindex, std::map<double, int>& srcmap ){
		m_motmark[ srcindex ] = srcmap;
	};
	void ClearMotMarkOfMap2( int srcindex ){
		m_motmark[ srcindex ].clear();
	};

	ChaVector3 GetFirstFrameBonePos()
	{
		return m_firstframebonepos;
	};


	CModel* GetParModel(){ return m_parmodel; };
	//void SetParModel( CModel* srcpar ){ m_parmodel = srcpar; };//parmodelごとのm_bonenoに注意！！！

	bool IsNull()
	{
		return (GetType() == FBXBONE_NULL);
	}
	bool IsNotNull()
	{
		return (GetType() != FBXBONE_NULL);
	}
	bool IsSkeleton()
	{
		return (GetType() == FBXBONE_SKELETON);
	}
	bool IsNotSkeleton()
	{
		return (GetType() != FBXBONE_SKELETON);
	}
	bool IsCamera()
	{
		return (GetType() == FBXBONE_CAMERA);
	}
	bool IsNotCamera()
	{
		return (GetType() != FBXBONE_CAMERA);
	}
	bool HasCameraParent();
	
	CBone* GetParent(bool excludenullflag);
	CBone* GetChild(bool excludenullflag);
	CBone* GetBrother(bool excludenullflag);
	CBone* GetSister(bool excludenullflag);
	void GetParentReq(bool excludenullflag, CBone** ppfindbone);
	void GetChildReq(bool excludenullflag, bool findbroflag, CBone** ppfindbone);
	void GetBrotherReq(bool excludenullflag, CBone** ppfindbone);


	void SetParent( CBone* srcpar ){ m_parent = srcpar; };
	void SetChild( CBone* srcchil ){ m_child = srcchil; };
	void SetBrother( CBone* srcbro ){ m_brother = srcbro; };

	bool IsBranchBone()
	{
		CBone* chksister = GetSister(false);
		CBone* chkbrother = GetBrother(false);
		if ((chksister && chksister->IsSkeleton()) || 
			(chkbrother && chkbrother->IsSkeleton())) {
			return true;
		}
		else {
			return false;
		}
	}
	CBone* GetUpperBranchBone() {
		CBone* curbone = GetParent(false);
		while (curbone) {
			if (curbone->IsSkeleton() && curbone->IsBranchBone()) {
				return curbone;
			}
			curbone = curbone->GetParent(false);
		}
		return 0;
	};
	CBone* GetLowerBranchBone() {
		CBone* curbone = GetChild(false);
		while (curbone) {
			if (curbone->IsSkeleton() && curbone->IsBranchBone()) {
				return curbone;
			}
			curbone = curbone->GetChild(false);
		}
		return 0;
	}
	

	bool GetPositionFound(){
		return m_posefoundflag;
	};
	void SetPositionFound(bool foundflag){
		m_posefoundflag = foundflag;
	};

	double GetBoneLeng(){
		if (IsSkeleton()) {
			CBone* parentbone = GetParent(false);
			if (parentbone && parentbone->IsSkeleton()) {
				ChaVector3 bonevec = GetJointFPos() - parentbone->GetJointFPos();
				double boneleng = ChaVector3LengthDbl(&bonevec);
				return boneleng;
			}
			else {
				return 0.0;
			}
		}
		else {
			return 0.0;
		}
	};
	/*
	ChaMatrix GetFirstAxisMatX()
	{
		CalcFirstAxisMatX();
		return m_firstaxismatX;
	};
	*/
	//ChaMatrix GetFirstAxisMatZ()
	//{
	//	CalcFirstAxisMatZ();
	//	return m_firstaxismatZ;
	//};

	CMotionPoint* GetMotionPoint(int srcmotid, double srcframe, bool onaddmotion = false);



	//ChaMatrix GetENullMatrix(double srctime);
	//void CalcEnullMatReq(double srctime, ChaMatrix* plocalnodemat, ChaMatrix* plocalnodeanimmat);//parent方向へ計算
	ChaMatrix GetTransformMat(double srctime, bool forceanimflag);
	ChaMatrix CalcFbxLocalMatrix(bool limitdegflag, int srcmotid, double srcframe);


	ANGLELIMIT GetAngleLimit(bool limitdegflag, int getchkflag);
	void SetAngleLimit(bool limitdegflag, ANGLELIMIT srclimit);

	int GetFreeCustomRigNo();
	CUSTOMRIG GetFreeCustomRig();
	int InvalidateCustomRig(int rigno);
	CUSTOMRIG GetCustomRig(int rigno);
	void SetCustomRig(CUSTOMRIG srccr);

	ChaMatrix GetTmpSymMat()
	{
		return m_tmpsymmat;
	};
	void SetTmpSymMat(ChaMatrix srcmat)
	{
		m_tmpsymmat = srcmat;
	};
	ChaVector3 GetBtparentpos(){
		return m_btparentpos;
	};
	ChaVector3 GetBtchildpos(){
		return m_btchildpos;
	};
	ChaMatrix GetBtDiffMat(){
		return m_btdiffmat;
	};


	//ChaMatrix GetBefBtMat(){ return m_befbtmat; };
	//void SetBefBtMat(ChaMatrix srcmat){ m_befbtmat = srcmat; };
	ChaMatrix GetBtMat(bool calcslotflag = false){
		if (calcslotflag == false) {
			//計算済のm_curmpを取得する場合
			int renderslot = (int)(!(m_updateslot != 0));
			return m_btmat[renderslot];
		}
		else {
			//姿勢の計算中にGetCurMpする場合
			return m_btmat[m_updateslot];
		}
	};
	//ChaMatrix GetInvBtMat(){ ChaMatrix retmat; ChaMatrixInverse(&retmat, NULL, &m_btmat); return retmat; };
	void SetBtMat(ChaMatrix srcmat, bool settobothflag = false){
		////if (GetBtFlag() == 0){
		//	SetBefBtMat(m_btmat[m_updateslot]);
		////}

		if (settobothflag == false) {
			m_btmat[m_updateslot] = srcmat;
		}
		else {
			m_btmat[0] = srcmat;
			m_btmat[1] = srcmat;
		}
		
	};
	void SetBtEul(ChaVector3 srceul) {
		m_bteul = srceul;
	}
	ChaVector3 GetBtEul() {
		return m_bteul;
	}



	int GetBtFlag(){ return m_setbtflag; };
	void SetBtFlag(int srcflag){ m_setbtflag = srcflag; };

	CModel* GetCurColDispInstancing(CBone* childbone, int* pinstanceno);
	CModel* GetColDisp(CBone* childbone, int srcindex);
	void SetFirstCalcRigid(bool srcflag){
		m_firstcalcrigid = srcflag;
	};

	//数学計算のIKTargetFlagに変更
	//void SetPosConstraint(int srcval){
	//	m_posconstraint = srcval;
	//};
	//int GetPosConstraint(){
	//	return m_posconstraint;
	//};
	//void SetMass0(int srcval){
	//	m_mass0 = srcval;
	//};
	//int GetMass0(){
	//	return m_mass0;
	//};

	//void SetExcludeMv(int srcval){
	//	m_excludemv = srcval;
	//};
	//int GetExcludeMv(){
	//	return m_excludemv;
	//};
	void SetCurMotID(int srcmotid);
	int GetCurMotID()
	{
		return m_curmotid;
	};
	void SetTmpKinematic(bool srcflag)
	{
		m_tmpkinematic = srcflag;
	};
	bool GetTmpKinematic()
	{
		return m_tmpkinematic;
	};
	

	int GetUseFlag()
	{
		return m_useflag;
	};
	void SetUseFlag(int srcflag)
	{
		m_useflag = srcflag;
	};
	int GetIndexOfPool()
	{
		return m_indexofpool;
	};
	void SetIndexOfPool(int srcindex)
	{
		m_indexofpool = srcindex;
	};
	int IsAllocHead()
	{
		return m_allocheadflag;
	};
	void SetIsAllocHead(int srcflag)
	{
		m_allocheadflag = srcflag;
	};


	void SetExtendFlag(bool srcflag) {
		m_extendflag = srcflag;
	};
	bool GetExtendFlag() {
		return m_extendflag;
	};

	//void InitAddLimitQ()
	//{
	//	m_addlimitq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//}
	//void Add2AddLimitQ(CQuaternion srcq);
	//CQuaternion GetAddLimitQ()
	//{
	//	return m_addlimitq;
	//}

	//CQuaternion GetLocalR0() {
	//	return m_localR0;
	//}
	//void SetLocalR0(CQuaternion srcq) {
	//	m_localR0 = srcq;
	//}
	//ChaMatrix GetLocalS0() {
	//	return m_localS0;
	//}
	//void SetLocalS0(ChaMatrix srcval) {
	//	m_localS0 = srcval;
	//}
	//ChaMatrix GetLocalT0() {
	//	return m_localT0;
	//}
	//void SetLocalT0(ChaMatrix srcval) {
	//	m_localT0 = srcval;
	//}
	//ChaMatrix GetFirstSRT() {
	//	return m_firstSRT;
	//}
	//void SetFirstSRT(ChaMatrix srcval) {
	//	m_firstSRT = srcval;
	//}
	//ChaMatrix GetFirstGlobalSRT() {
	//	return m_firstGlobalSRT;
	//}
	//void SetFirstGlobalSRT(ChaMatrix srcval) {
	//	m_firstGlobalSRT = srcval;
	//}

	//void SetTempLocalEul(ChaVector3 srcorgeul, ChaVector3 srcneweul)
	//{
	//	m_temporgeul = srcorgeul;
	//	m_tempneweul = srcneweul;
	//}
	//void GetTempLocalEul(ChaVector3* dstorgeul, ChaVector3* dstneweul)
	//{
	//	if (!dstorgeul || !dstneweul) {
	//		return;
	//	}
	//	*dstorgeul = m_temporgeul;
	//	*dstneweul = m_tempneweul;
	//}
	void SetSkipRenderBoneMark(bool srcval)
	{
		m_skipRenderBoneMark = srcval;
	}
	bool GetSkipRenderBoneMark()
	{
		return m_skipRenderBoneMark;
	}
	
	void SetFbxNodeOnLoad(FbxNode* srcnode)
	{
		m_fbxnodeonload = srcnode;
	}
	FbxNode* GetFbxNodeOnLoad() {
		return m_fbxnodeonload;
	}

	void SetIKStopFlag(bool srcflag)
	{
		m_ikstopflag = srcflag;
	}
	bool GetIKStopFlag()
	{
		return m_ikstopflag;
	}
	void SetIKTargetFlag(bool srcflag);
	bool GetIKTargetFlag();
	void SetIKTargetPos(ChaVector3 srcpos)
	{
		m_iktargetpos = srcpos;
	}
	ChaVector3 GetIKTargetPos()
	{
		return m_iktargetpos;
	}

	//2023/02/16
	//fbxの初期姿勢のジョイントの向きを書き出すために追加
	void SaveFbxNodePosture(FbxNode* pNode);
	//void RestoreFbxNodePosture(FbxNode* pNode);
	int CalcLocalNodePosture(bool bindposeflag, FbxNode* pNode, double srcframe, ChaMatrix* plocalnodemat, ChaMatrix* plocalnodeanimmat);
	void CalcNodePostureReq(bool bindposeflag, FbxNode* pNode, double srcframe, ChaMatrix* plocalnodemat, ChaMatrix* plocalnodeanimmat);


	void ClearIKRotRec()
	{
		m_ikrotrec.clear();
	}
	void AddIKRotRec(IKROTREC srcrotrec)
	{
		m_ikrotrec.push_back(srcrotrec);
	}
	int GetIKRotRecSize()
	{
		return (int)m_ikrotrec.size();
	}
	IKROTREC GetIKRotRec(int srcindex)
	{
		if ((srcindex >= 0) && (srcindex < GetIKRotRecSize())) {
			return m_ikrotrec[srcindex];
		}
		else {
			IKROTREC norec;
			norec.rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			norec.targetpos = ChaVector3(0.0f, 0.0f, 0.0f);
			norec.lessthanthflag = true;
			return norec;
		}
	}

	void ClearIKRotRecUV()
	{
		m_ikrotrec_u.clear();
		m_ikrotrec_v.clear();
	}
	void AddIKRotRec_U(IKROTREC srcrotrec)
	{
		m_ikrotrec_u.push_back(srcrotrec);
	}
	void AddIKRotRec_V(IKROTREC srcrotrec)
	{
		m_ikrotrec_v.push_back(srcrotrec);
	}
	int GetIKRotRecSize_U()
	{
		return (int)m_ikrotrec_u.size();
	}
	int GetIKRotRecSize_V()
	{
		return (int)m_ikrotrec_v.size();
	}
	IKROTREC GetIKRotRec_U(int srcindex)
	{
		if ((srcindex >= 0) && (srcindex < GetIKRotRecSize_U())) {
			return m_ikrotrec_u[srcindex];
		}
		else {
			IKROTREC norec;
			norec.rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			norec.targetpos = ChaVector3(0.0f, 0.0f, 0.0f);
			norec.lessthanthflag = true;
			return norec;
		}
	}
	IKROTREC GetIKRotRec_V(int srcindex)
	{
		if ((srcindex >= 0) && (srcindex < GetIKRotRecSize_V())) {
			return m_ikrotrec_v[srcindex];
		}
		else {
			IKROTREC norec;
			norec.rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			norec.targetpos = ChaVector3(0.0f, 0.0f, 0.0f);
			norec.lessthanthflag = true;
			return norec;
		}
	}


	FbxDouble3 GetFbxLclPos() {
		return m_fbxLclPos;
	}
	FbxDouble3 GetFbxRotOff() {
		return m_fbxRotOff;
	}
	FbxDouble3 GetFbxRotPiv() {
		return m_fbxRotPiv;
	}
	FbxDouble3 GetFbxPreRot() {
		return m_fbxPreRot;
	}
	FbxDouble3 GetFbxLclRot() {
		return m_fbxLclRot;
	}
	FbxDouble3 GetFbxPostRot() {
		return m_fbxPostRot;
	}
	FbxDouble3 GetFbxSclOff() {
		return m_fbxSclOff;
	}
	FbxDouble3 GetFbxSclPiv() {
		return m_fbxSclPiv;
	}
	FbxDouble3 GetFbxLclScl() {
		return m_fbxLclScl;
	}
	void SetFbxRotationActive(bool srcflag) {
		m_fbxrotationActive = srcflag;
	}
	bool GetFbxRotationActive() {
		return m_fbxrotationActive;
	}
	EFbxRotationOrder GetFbxRotationOrder() {
		return m_rotationorder;
	}
	

	int GetDbgCount()
	{
		return m_dbgcount;
	}
	void SetDbgCount(int srccount)
	{
		m_dbgcount = srccount;
	}

	//ChaMatrix GetLocalNodeMat()
	//{
	//	return m_localnodemat;
	//}
	//void SetLocalNodeMat(ChaMatrix srcmat)
	//{
	//	m_localnodemat = srcmat;
	//}
	//ChaMatrix GetLocalNodeAnimMat()
	//{
	//	return m_localnodeanimmat;
	//}
	//void SetLocalNodeAnimMat(ChaMatrix srcmat)
	//{
	//	m_localnodeanimmat = srcmat;
	//}

	int GetDefBonePosKind()
	{
		return m_defboneposkind;
	}
	void SetDefBonePosKind(int srckind)
	{
		if ((srckind >= 0) && (srckind < DEFBONEPOS_MAX)) {
			m_defboneposkind = srckind;
		}
		else {
			_ASSERT(0);
		}
	}

	bool HasMotionCurve(int srcmotid) 
	{
		std::map<int, bool>::iterator itrhascurve;
		itrhascurve = m_hasmotioncurve.find(srcmotid);
		if (itrhascurve != m_hasmotioncurve.end()) {
			return itrhascurve->second;
		}
		else {
			return false;
		}
	}
	void SetHasMotionCurve(int srcmotid, bool srcval)
	{
		std::map<int, bool>::iterator itrhascurve;
		itrhascurve = m_hasmotioncurve.find(srcmotid);
		if (itrhascurve != m_hasmotioncurve.end()) {
			itrhascurve->second = srcval;
		}
		else {
			m_hasmotioncurve[srcmotid] = srcval;
		}
	}

	void SetFirstGetMatrix(ChaMatrix srcmat)
	{
		m_firstgetflag = 1;
		m_firstgetmatrix = srcmat;
	}
	void SetInvFirstGetMatrix(ChaMatrix srcmat)
	{
		m_firstgetflag = 1;
		m_invfirstgetmatrix = srcmat;
	}

	CMotionPoint* GetMPCache(int srcmotid)
	{
		if ((srcmotid >= 1) && (srcmotid <= MAXMOTIONNUM)) {//m_cachebefmp[MAXMOTIONPOINT + 1]
			return m_cachebefmp[srcmotid - 1];
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}
	void SetMPCache(int srcmotid, CMotionPoint* srcmp)
	{
		if ((srcmotid >= 1) && (srcmotid <= MAXMOTIONNUM)) {//m_cachebefmp[MAXMOTIONPOINT + 1]
			m_cachebefmp[srcmotid - 1] = srcmp;
		}
		else {
			_ASSERT(0);
			return;
		}
	}
	bool GetPasteDoneFlag()
	{
		return m_pastedoneflag;
	}
	void SetPasteDoneFlag(bool srcflag)
	{
		m_pastedoneflag = srcflag;
	}

	void SetMatrixIndex(int srcindex) {
		if ((srcindex < 0) || (srcindex >= MAXBONENUM)) {
			_ASSERT(0);
			m_matrixindex = 0;
		}
		else {
			m_matrixindex = srcindex;
		}
	}
	int GetMatrixIndex()
	{
		return m_matrixindex;
	}

public:
	CRITICAL_SECTION m_CritSection_GetBefNext;
	CRITICAL_SECTION m_CritSection_GetBefNext2;

private:
	CRITICAL_SECTION m_CritSection_AddMP;

	int m_useflag;//0: not use, 1: in use
	int m_indexofpool;//index of pool vector
	int m_allocheadflag;//1: head pointer at allocated




	//int m_posconstraint;
	//int m_mass0;
	//int m_excludemv;
	bool m_firstcalcrigid;
	int m_type;
	int m_selectflag;
	int m_bonecnt;
	int m_validflag;
	int m_boneno;
	int m_topboneflag;
	char m_bonename[256];
	WCHAR m_wbonename[256];
	char m_engbonename[256];
	int m_curmotid;

	int m_upkind;//m_gaxismatXpar計算時のupvec

	bool m_posefoundflag;//BindPoseの中にこのボーンの位置情報があった場合true。


	//ChaMatrix m_localS0;//local Scale at Frame0
	//CQuaternion m_localR0;//local Rotation at Frame0
	//ChaMatrix m_localT0;//local Translation at Frame0
	//ChaMatrix m_firstSRT;//SRT matrix at Frame0
	//ChaMatrix m_firstGlobalSRT;//globalSRT matrix at Frame0


	ChaVector3 m_childworld;//ボーンの子供のモーション行列適用後の座標。
	ChaVector3 m_childscreen;//ボーンの子供のWVP適用後の座標。




	std::map<int, CMotionPoint*> m_motionkey;//m_motionkey[ モーションID ]でモーションの最初のフレームの姿勢にアクセスできる。
	CMotionPoint m_curmp[2];//現在のWVP適用後の姿勢データ。 ### 計算済 ###
	int m_updateslot;
	//CMotionPoint m_calccurmp;////現在のWVP適用後の姿勢データ。 ### 計算中 ###
	//CMotionPoint m_befmp;//一回前の姿勢データ。
	CMotionPoint* m_cachebefmp[MAXMOTIONNUM + 1];//motidごとのキャッシュ

	//std::vector<CMotionPoint*> m_indexedmp;
	std::map<int, std::vector<CMotionPoint*>> m_indexedmotionpoint;
	std::map<int, bool> m_initindexedmotionpoint;
	CMotionPoint m_dummymp;

	CQuaternion m_axisq;//ボーンの軸のクォータニオン表現。
	ChaMatrix m_laxismat;//Zボーンのaxismat

	//ChaMatrix m_gaxismatXpar;//Xボーンのグローバルのaxismat
	//ChaMatrix m_gaxismatYpar;//Yボーンのグローバルのaxismat
	//ChaMatrix m_axismat_par;//Xボーンのローカルのaxismat
	//ChaMatrix m_firstaxismatX;//初期状態でのXボーンのグローバルaxismat
	//ChaMatrix m_firstaxismatZ;//初期状態でのZボーンのグローバルaxismat


	//ChaMatrix m_startmat2;//ワールド行列を保存しておくところ。剛体シミュレーションを始める際などに保存する。

	int m_getanimflag;//FBXファイルを読み込む際にアニメーションを読み込んだら１。
	
	ChaMatrix m_nodemat;//ジョイントの初期位置を計算するときに使用する。FBX読み込み時にセットして使用する。(0frameアニメ無し)
	ChaMatrix m_nodeanimmat;//ジョイントの初期位置を計算するときに使用する。FBX読み込み時にセットして使用する。(0frameアニメ有り)

	//FbxAMatrix m_bindmat;//getpose bindpose// コメントアウト：NodeOnLoadのbindmatを使う
	ChaMatrix m_firstmat;//ジョイントの初期位置を計算するときに使用する。FBX読み込み時にセットして使用する。
	ChaMatrix m_invfirstmat;//ジョイントの初期位置を計算するときに使用する。FBX読み込み時にセットして使用する。
	ChaVector3 m_jointfpos;//ジョイントの初期位置。
	ChaVector3 m_oldjointfpos;//ジョイント初期位置（旧データ互換）
	ChaVector3 m_jointwpos;//FBXにアニメーションが入っていない時のジョイントの初期位置。
	FbxAMatrix m_globalpos;//ジョイントの初期位置を計算するときに使用する。FBX読み込み時にセットして使用する。
	ChaMatrix m_initmat;
	ChaMatrix m_invinitmat;
	ChaMatrix m_tmpmat;//一時使用目的
	CQuaternion m_tmpq;
	ChaMatrix m_tmpsymmat;

	int m_defboneposkind;//FbxFile.cpp FbxSetDefaultBonePosReq()でセット　BPの有無など


	//CQuaternion m_addlimitq;

	ChaMatrix m_btmat[2];
	//ChaMatrix m_befbtmat[2];
	int m_setbtflag;
	ChaVector3 m_bteul;

	ChaVector3 m_firstframebonepos;

	ANGLELIMIT m_anglelimit;

	int m_initcustomrigflag;
	CUSTOMRIG m_customrig[MAXRIGNUM];

	//CBone*は子供ジョイントのポインタ。子供の数だけエントリーがある。
	//std::map<CBone*, CRigidElem*> m_rigidelem;
	std::string m_rigidelemname;


	//m_remapは、jointの名前でセットすればmap<string,CRigidElem*>で済む。
	//名前で一意なRigidElemを選択するために子供のボーンの名前を使用する。
	//そのため子供のボーンの名前とCRigidElemのセットを使う。
	//m_impmapについても同様のことがいえる。
	std::map<std::string, std::map<CBone*, CRigidElem*>> m_remap;//map<設定ファイル名, map<子供ボーン, 剛体設定>>
	std::map<std::string, std::map<CBone*, ChaVector3>> m_impmap;//map<設定ファイル名, map<子供ボーン, インパルス設定>>


	int m_btkinflag;//bullet kinematic flag。剛体シミュレーションの根元のボーンが固定モーションに追随する際は０を指定する。その他は１。
	int m_btforce;//bullet 強制フラグ。モデルに設定されているbtkinflagである。m_btkinflagはシミュレーション開始と終了で変化するが、このフラグは読み込み時に設定されるものである。
	std::map<CBone*,CBtObject*> m_btobject;//CBtObjectはbulletの剛体オブジェクトをラップしたクラスである。ボーンとCBtObjectのmap。

	//タイムラインのモーション編集マーク
	//最初のintはmotid。次のmapはframenoと更新フラグ。更新フラグは読み込み時のマークは０、それ以後の編集マークは１にする予定。色を変えるかも。
	std::map<int, std::map<double,int>> m_motmark;


	CModel* m_parmodel;

	int m_firstgetflag;//GetCurrentZeroFrameMat用
	ChaMatrix m_firstgetmatrix;//GetCurrentZeroFrameMat用
	ChaMatrix m_invfirstgetmatrix;//GetCurrentZeroFrameMat用

	bool m_tmpkinematic;

	bool m_extendflag;

	double m_befupdatetime;


	//ChaVector3 m_temporgeul;//制限角度有り、並列化の際の一時置き場
	//ChaVector3 m_tempneweul;//制限角度有り、並列化の際の一時置き場

	bool m_skipRenderBoneMark;

	FbxNode* m_fbxnodeonload;//2022/11/01


	//2023/02/16
	//jointの向きの書き出しに対応するためメンバ追加
	FbxDouble3 m_fbxLclPos;
	FbxDouble3 m_fbxRotOff;
	FbxDouble3 m_fbxRotPiv;
	FbxDouble3 m_fbxPreRot;
	FbxDouble3 m_fbxLclRot;
	FbxDouble3 m_fbxPostRot;
	FbxDouble3 m_fbxSclOff;
	FbxDouble3 m_fbxSclPiv;
	FbxDouble3 m_fbxLclScl;
	bool m_fbxrotationActive;
	EFbxRotationOrder m_rotationorder;
	FbxTransform::EInheritType m_InheritType;

	//ChaMatrix m_localnodemat;
	//ChaMatrix m_localnodeanimmat;

	std::map<int, bool> m_hasmotioncurve;

	bool m_ikstopflag;
	bool m_iktargetflag;
	ChaVector3 m_iktargetpos;

	std::vector<IKROTREC> m_ikrotrec;
	std::vector<IKROTREC> m_ikrotrec_u;
	std::vector<IKROTREC> m_ikrotrec_v;

	int m_dbgcount;

	bool m_pastedoneflag;
	int m_matrixindex;//for constant buffer

	CBone* m_parent;
	CBone* m_child;
	CBone* m_brother;
};

#endif