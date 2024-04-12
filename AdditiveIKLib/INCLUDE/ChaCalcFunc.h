#ifndef CHACALCFUNCH
#define CHACALCFUNCH

#include <ChaVecCalc.h>



class ChaCalcFunc
{
	//#############################################################################################################
	//2023/10/17
	//このクラスの意味
	//マルチスレッドから　同期処理なしで呼び出し可能にするため
	//ChaCalcFuncのインスタンスをスレッドごとに作成し　インスタンス単位で実行中に再入しないようにするためのクラス
	//#############################################################################################################

public:
	ChaCalcFunc() {};
	~ChaCalcFunc() {};

	int ModifyEuler360(ChaVector3* eulerA, ChaVector3* eulerB, int notmodify180flag, float throundX, float throundY, float throundZ);
	int GetRoundThreshold(float srcval, float degth);


	int GetBefNextMP(CBone* srcbone, int srcmotid, double srcframe, CMotionPoint** ppbef, CMotionPoint** ppnext, int* existptr, bool onaddmotion = false);


	int IKRotateOneFrame(CModel* srcmodel, int limitdegflag, CEditRange* erptr,
		int keyno, CBone* rotbone, CBone* parentbone,
		int srcmotid, double curframe, double startframe, double applyframe,
		CQuaternion rotq0, bool keynum1flag, bool postflag, bool fromiktarget);
	int RotAndTraBoneQReq(CBone* srcbone, bool limitdegflag, int* onlycheckptr,
		double srcstartframe, bool infooutflag, CBone* parentbone, int srcmotid, double srcframe,
		CQuaternion qForRot, CQuaternion qForHipsRot, bool fromiktarget);


	int IKTargetVec(CModel* srcmodel, bool limitdegflag, CEditRange* erptr, int srcmotid, double srcframe, bool postflag);
	int IKRotateForIKTarget(CModel* srcmodel, bool limitdegflag, CEditRange* erptr,
		int srcboneno, int srcmotid, ChaVector3 targetpos, int maxlevel, double directframe, bool postflag);

	int AdjustBoneTra(CModel* srcmodel, bool limitdegflag, CEditRange* erptr, CBone* lastpar, int srcmotid);


	int FKBoneTra(CModel* srcmodel, bool limitdegflag, int onlyoneflag, CEditRange* erptr,
		int srcboneno, int srcmotid, ChaVector3 addtra, double onlyonefarme = 0.0);

	//for threading
	int FKBoneTraOneFrame(CModel* srcmodel, bool limitdegflag, CEditRange* erptr,
		int srcboneno, int srcmotid, double srcframe, ChaVector3 addtra);

	CMotionPoint* AddBoneTraReq(CBone* srcbone, bool limitdegflag, CMotionPoint* parmp, int srcmotid, double srcframe, ChaVector3 srctra, ChaMatrix befparentwm, ChaMatrix newparentwm);


	int CalcQForRot(bool limitdegflag, bool calcaplyflag,
		int srcmotid, double srcframe, double srcapplyframe, CQuaternion srcaddrot,
		CBone* srcrotbone, CBone* srcaplybone,
		CQuaternion* dstqForRot, CQuaternion* dstqForHipsRot);
	bool CalcAxisAndRotForIKRotateAxis(CModel* srcmodel, int limitdegflag,
		CBone* parentbone, CBone* firstbone,
		int srcmotid, double curframe, ChaVector3 targetpos,
		ChaVector3 srcikaxis,
		ChaVector3* dstaxis, float* dstrotrad);


	int CalcBoneEul(CModel* srcmodel, bool limitdegflag, int srcmotid);
	int CalcBoneEulOne(CModel* srcmodel, bool limitdegflag, CBone* curbone, int srcmotid, double startframe, double endframe);
	ChaVector3 CalcLocalEulXYZ(CBone* srcbone, bool limitdegflag, int axiskind,
		int srcmotid, double srcframe, tag_befeulkind befeulkind, ChaVector3* directbefeul);

	int ChkMovableEul(CBone* srcbone, ChaVector3 srceul);
	int SetLocalEul(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, ChaVector3 srceul, CMotionPoint* srcmp);
	ChaVector3 GetLocalEul(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* srcmp);
	BEFEUL GetBefEul(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe);


	ChaVector3 LimitEul(CBone* srcbone, ChaVector3 srceul);

	int SetWorldMat(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, ChaMatrix srcmat, CMotionPoint* srcmp);
	int SetWorldMat(CBone* srcbone, bool limitdegflag, bool directsetflag, bool infooutflag, int setchildflag, 
		int srcmotid, double srcframe, ChaMatrix srcmat, int onlycheck, bool fromiktarget);
	int SetBtMatLimited(CBone* srcbone, bool limitdegflag, bool directsetflag, ChaMatrix srcmat);
	int SetWorldMatFromEulAndScaleAndTra(CBone* srcbone, bool limitdegflag, int inittraflag, int setchildflag, ChaMatrix befwm, ChaVector3 srceul, ChaVector3 srcscale, ChaVector3 srctra, int srcmotid, double srcframe);

	CMotionPoint* GetMotionPoint(CBone* srcbone, int srcmotid, double srcframe, bool onaddmotion = false);
	ChaMatrix GetWorldMat(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* srcmp, ChaVector3* dsteul = 0);

	int CopyWorldToLimitedWorldOne(CModel* srcmodel, CBone* srcbone, int srcmotid, double srcframe);
	void CopyWorldToLimitedWorldReq(CModel* srcmodel, CBone* srcbone, int srcmotid, double srcframe);
	int CopyWorldToLimitedWorld(CBone* srcbone, int srcmotid, double srcframe);
	void UpdateCurrentWM(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe,
		ChaMatrix newwm);
	void UpdateParentWMReq(CBone* srcbone, bool limitdegflag, bool setbroflag, int srcmotid, double srcframe,
		ChaMatrix oldparentwm, ChaMatrix newparentwm);


	int Motion2Bt(CModel* srcmodel, bool limitdegflag, double nextframe, 
		ChaMatrix* pmView, ChaMatrix* pmProj);//, int updateslot);


//##############
//For Retarget
//##############
public:
	void RetargetReq(CModel* srcmodel, CModel* srcbvhmodel, CBone* modelbone,
		double srcframe, CBone* befbvhbone, float hrate, std::map<CBone*, CBone*>& sconvbonemap);
	CMotionPoint* RotBoneQReq(CBone* srcbone, bool limitdegflag, bool infooutflag,
		CBone* parentbone, int srcmotid, double srcframe,
		CQuaternion rotq, ChaMatrix srcbefparentwm, ChaMatrix srcnewparentwm,
		CBone* bvhbone, ChaVector3 traanim);
	ChaMatrix CalcNewLocalRotMatFromQofIK(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe,
		CQuaternion qForRot, ChaMatrix* dstsmat, ChaMatrix* dstrmat, ChaMatrix* dsttanimmat);
	ChaMatrix GetCurrentZeroFrameMat(CBone* srcbone, bool limitdegflag, int updateflag);
	int FKRotate(CModel* srcmodel, bool limitdegflag, bool onretarget, int reqflag,
		CBone* bvhbone, int traflag, ChaVector3 traanim, double srcframe, int srcboneno,
		CQuaternion rotq);
	void GetHipsBoneReq(CModel* srcmodel, CBone* srcbone, CBone** dstppbone);
	CBone* GetTopBone(CModel* srcmodel, bool excludenullflag);//default : excludenullflag = true
	void GetTopBoneReq(CModel* srcmodel, CBone* srcbone, CBone** pptopbone, bool excludenullflag);

private:
	int ConvBoneRotation(CModel* srcmodel, CModel* srcbvhmodel, int selfflag,
		CBone* srcbone, CBone* bvhbone, double srcframe, CBone* befbvhbone, float hrate);

//#############
//For InitMP
//#############
public:
	int InitMP(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe);
	int InitMP(CModel* srcmodel, bool limitdegflag, CBone* curbone, int srcmotid, double curframe);
	void InitMPReq(CModel* srcmodel, bool limitdegflag, CBone* curbone, int srcmotid, double curframe);


//#################################################################
//Following Functions are Called From GlobalFunctions at ChaVecCalc
//#################################################################
public:
	BOOL ccfIsValidNewEul(ChaVector3 srcneweul, ChaVector3 srcbefeul);
	ChaMatrix ccfChaMatrixTranspose(ChaMatrix srcmat);
	double ccfChaVector3LengthDbl(ChaVector3* psrc);
	double ccfChaVector3DotDbl(const ChaVector3* psrc1, const ChaVector3* psrc2);
	//float ccfChaVector3LengthDbl(ChaVector3* psrc);
	void ccfChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc);
	float ccfChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2);
	void ccfChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2);
	void ccfChaVector3TransformCoord(ChaVector3* pdst, ChaVector3* psrc, ChaMatrix* pmat);
	double ccfChaVector3LengthSqDbl(ChaVector3* psrc);
	ChaVector3* ccfChaVector3TransformNormal(ChaVector3* pOut, const ChaVector3* pV, const ChaMatrix* pM);
	void ccfChaMatrixIdentity(ChaMatrix* pdst);
	ChaMatrix ccfChaMatrixScale(ChaMatrix srcmat);//スケール成分だけの行列にする
	ChaMatrix ccfChaMatrixRot(ChaMatrix srcmat);//回転成分だけの行列にする
	ChaMatrix ccfChaMatrixTra(ChaMatrix srcmat);//移動成分だけの行列にする
	ChaVector3 ccfChaMatrixScaleVec(ChaMatrix srcmat);//スケール成分のベクトルを取得
	ChaVector3 ccfChaMatrixRotVec(ChaMatrix srcmat, int notmodify180flag);//回転成分のベクトルを取得
	ChaVector3 ccfChaMatrixTraVec(ChaMatrix srcmat);//移動成分のベクトルを取得
	CQuaternion ccfChaMatrix2Q(ChaMatrix srcmat);//ChaMatrixを受け取って　CQuaternionを返す
	void ccfChaMatrixNormalizeRot(ChaMatrix* pdst);
	void ccfChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc);
	void ccfChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz);
	void ccfChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc);
	void ccfChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad);
	void ccfChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz);
	void ccfChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec);
	ChaMatrix* ccfChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf);
	ChaMatrix* ccfChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf);
	ChaMatrix* ccfChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll);
	ChaMatrix* ccfChaMatrixRotationX(ChaMatrix* pOut, float srcrad);
	ChaMatrix* ccfChaMatrixRotationY(ChaMatrix* pOut, float srcrad);
	ChaMatrix* ccfChaMatrixRotationZ(ChaMatrix* pOut, float srcrad);
	const ChaMatrix* ccfChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq);
	void ccfCQuaternionIdentity(CQuaternion* dstq);
	CQuaternion ccfCQuaternionInv(CQuaternion srcq);
	double ccfvecDotVec(ChaVector3* vec1, ChaVector3* vec2);
	double ccflengthVec(ChaVector3* vec);
	double ccfaCos(double dot);
	int ccfvec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
	int ccfvec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
	int ccfvec3RotateZ(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
	int ccfGetRound(float srcval);
	int ccfIsInitRot(ChaMatrix srcmat);
	int ccfIsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2);
	int ccfIsSameEul(ChaVector3 srceul1, ChaVector3 srceul2);
	bool ccfIsJustEqualTime(double srctime1, double srctime2);
	double ccfRoundingTime(double srctime);
	int ccfIntTime(double srctime);
	bool ccfIsEqualRoundingTime(double srctime1, double srctime2);
	double ccfVecLength(ChaVector3 srcvec);
	void ccfGetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr);
	void ccfGetSRTMatrix2(ChaMatrix srcmat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr);
	void ccfGetSRTandTraAnim(ChaMatrix srcmat, ChaMatrix srcnodemat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr, ChaMatrix* tanimmatptr);//For Local Posture
	ChaMatrix ccfChaMatrixFromSRT(bool sflag, bool tflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctmat);//For Local Posture
	ChaMatrix ccfChaMatrixFromSRTraAnim(bool sflag, bool tanimflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctanimmat);//For Local Posture
	ChaMatrix ccfGetS0RTMatrix(ChaMatrix srcmat);//拡大縮小を初期化したRT行列を返す
	ChaMatrix ccfChaMatrixKeepScale(ChaMatrix srcmat, ChaVector3 srcsvec);
	ChaMatrix ccfTransZeroMat(ChaMatrix srcmat);
	ChaMatrix ccfChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3);
	ChaMatrix ccfChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpipot);
	ChaMatrix ccfChaMatrixInv(ChaMatrix srcmat);
	CQuaternion ccfQMakeFromBtMat3x3(btMatrix3x3* eulmat);
	ChaMatrix ccfMakeRotMatFromChaMatrix(ChaMatrix srcmat);
	ChaMatrix ccfChaMatrixFromFbxAMatrix(FbxAMatrix srcmat);
	ChaMatrix ccfCalcAxisMatX(ChaVector3 vecx, ChaVector3 srcpos, ChaMatrix srcmat);
	int ccfIsInitMat(ChaMatrix srcmat);
	double ccfChaVector3LengthSq(ChaVector3* psrc);



private:
	float LimitAngle(CBone* srcbone, enum tag_axiskind srckind, float srcval);




};




#endif
