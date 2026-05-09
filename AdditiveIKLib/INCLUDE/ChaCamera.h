#ifndef CHACAMERAH
#define CHACAMERAH

#include <fbxsdk.h>

#include <unordered_map>
#include <string>

#include <Coef.h>
#include <ChaVecCalc.h>

class CMotionPoint;
class CModel;
class ChaScene;

class CModelFrameView
{
public:
	CModelFrameView() {
		InitParams();
	};
	~CModelFrameView() {};

	void InitParams() {
		m_validflag = false;
		m_matView.SetIdentity();
		m_motid = 0;
		m_frame = 1.0;
	};

	CModelFrameView operator= (CModelFrameView mfv) {
		m_validflag = mfv.m_validflag;
		m_matView = mfv.m_matView;
		m_frame = mfv.m_frame;
		m_motid = mfv.m_motid;
		return *this;
	};

	void SetParams(CModel* srcmodel, ChaMatrix srcmatView, int srcmotid, double srcframe)
	{
		if (srcmodel != nullptr) {
			m_matView = srcmatView;
			m_motid = srcmotid;
			m_frame = srcframe;

			m_validflag = true;
		}
	};
	void SetParams(ChaMatrix srcmatView, int srcmotid, double srcframe)
	{
		m_matView = srcmatView;
		m_motid = srcmotid;
		m_frame = srcframe;

		m_validflag = true;
	};
	void ResetValidFlag() {
		m_validflag = false;
	};
	bool GetValidFlag() {
		return m_validflag;
	};
	ChaMatrix GetMatView() {
		return m_matView;
	};
	int GetMotId() {
		return m_motid;
	};
	double GetFrame() {
		return m_frame;
	};

private:
	bool m_validflag;
	ChaMatrix m_matView;
	int m_motid;
	double m_frame;
};

class CModelFrameViewRingBuf
{
public:
	CModelFrameViewRingBuf() {
		InitParams();
	};
	~CModelFrameViewRingBuf() {};

	void InitParams() {
		m_model = nullptr;

		for (int refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
			m_ringbuf->InitParams();
			m_refposStartIndex = REFPOSMAXNUM;//リングバッファ保存位置　初回のProcessRefPosView()で0になるように初期化
			m_refposRecordCount = -1;//30フレームごとにm_refposViewを更新するためのカウンタ
		}
	};

	void ProcessRefPosView(CModel* srcmodel);
	CModelFrameView GetRefPosView(CModel* srcmodel, int srcrefposindex);
	void ResetValidFlag();

	void SetModel(CModel* srcmodel) {
		m_model = srcmodel;
	};
	CModel* GetModel() {
		return m_model;
	};

private:
	CModel* m_model;
	CModelFrameView m_ringbuf[REFPOSMAXNUM];
	int m_refposStartIndex;
	int m_refposRecordCount;
};


class ChaCamera
{
public:
	ChaCamera();
	~ChaCamera();

	void InitParams();
	
	double CalcCameraDist();
	int ChangeCameraDist(float newcamdist, bool moveeyeposflag, bool calledbyslider);
	
	double UpdateCameraDist();
	void UpdateCameraAnimFrame(double cameranextframe);
	
	void OnCameraMenu(int cameramotid);
	void OnFramePreviewCamera(double nextcameraframe);
	void OnCameraAnimMouseMoveDist();

	void SetCamera6Angle(int srcangleid);


	//for RefPosView
	bool AddRefPosViewBuf(CModel* srcmodel);
	void ProcessRefPosView();
	CModelFrameView GetRefPosView(CModel* srcmodel, int srcrefposindex);
	void ResetRefPosViewFlag();
	void ResetRefPosViewFlag(CModel* srcmodel);

private:
	void DestroyObjs();
	int FindModelFrameViewRingBufIndex(CModel* srcmodel);

public:
	void SetCamEye(ChaVector3 srcval) {
		m_camEye = srcval;
	};
	ChaVector3 GetCamEye() {
		return m_camEye;
	};
	void SetCamTargetPos(ChaVector3 srcval) {
		m_camtargetpos = srcval;
	};
	ChaVector3 GetCamTargetPos() {
		return m_camtargetpos;
	};
	void SetBefCamEye(ChaVector3 srcval) {
		m_befcamEye = srcval;
	};
	ChaVector3 GetBefCamEye() {
		return m_befcamEye;
	};
	void SetBefCamTargetPos(ChaVector3 srcval) {
		m_befcamtargetpos = srcval;
	};
	ChaVector3 GetBefCamTargetPos() {
		return m_befcamtargetpos;
	};
	void SetCamUpDir(ChaVector3 srcval) {
		ChaVector3Normalize(&srcval, &srcval);
		m_cameraupdir = srcval;
	};
	ChaVector3 GetCamUpDir() {
		return m_cameraupdir;
	};
	void SetCamDist(float srcval) {
		m_camdist = srcval;
	};
	float GetCamDist() {
		return m_camdist;
	};
	void SetFovY(float srcval) {
		m_fovy = srcval;
	};
	float GetFovY() {
		return m_fovy;
	};
	void SetProjNear(float srcval) {
		m_projnear = srcval;
	};
	float GetProjNear() {
		return m_projnear;
	};
	void SetProjFar(float srcval) {
		m_projfar = srcval;
	};
	float GetProjFar() {
		return m_projfar;
	};

	void SetCameraHeight(float srcval) {
		m_cameraheight = srcval;
	};
	float GetCameraHeight() {
		return m_cameraheight;
	};
	void SetCameraHeightFlag(int srcval) {
		m_cameraheightflag = srcval;
	};
	int GetCameraHeightFlag() {
		return m_cameraheightflag;
	};
	void SetCameraGModel(CModel* srcval) {
		m_cameragmodel = srcval;
	};
	CModel* GetCameraGModel() {
		return m_cameragmodel;
	};
	void SetCamTargetFlag(int srcval) {
		m_camtargetflag = srcval;
	};
	int GetCamTargetFlag() {
		return m_camtargetflag;
	};
	void SetCamTargetOnceFlag(int srcval) {
		m_camtargetOnceflag = srcval;
	};
	int GetCamTargetOnceFlag() {
		return m_camtargetOnceflag;
	};
	void SetCamTargetDisp(bool srcval) {
		m_camtargetdisp = srcval;
	};
	bool GetCamTargetDisp() {
		return m_camtargetdisp;
	};
	void SetCamMoveEyePos(bool srcval) {
		m_moveeyepos = srcval;
	};
	bool GetCamMoveEyePos() {
		return m_moveeyepos;
	};
	void SetCameraHasChildFlag(bool srcval) {
		m_cameraHasChildFlag = srcval;
	};
	bool GetCameraHasChildFlag() {
		return m_cameraHasChildFlag;
	};
	void SetCameraAnimModel(CModel* srcval) {
		m_cameramodel = srcval;
	};
	CModel* GetCameraAnimModel() {
		return m_cameramodel;
	};
	void SetCameraFrame(double srcval) {
		m_cameraframe = srcval;
	};
	double GetCameraFrame() {
		return m_cameraframe;
	};
	void SetCameraAnimMode(int srcval) {
		m_cameraanimmode = srcval;//0: OFF, 1:ON, 2:ON and RootMotionOption ON
	};
	int GetCameraAnimMode() {
		return m_cameraanimmode;//0: OFF, 1:ON, 2:ON and RootMotionOption ON
	};
	void SetCameraInheritMode(int srcval) {
		m_cameraInheritMode = srcval;
	};
	int GetCameraInheritMode()
	{
		return m_cameraInheritMode;
	};
	void SetCamTwistFlag(bool srcval) {
		m_twistcameraFlag = srcval;
	};
	bool GetCamTwistFlag() {
		return m_twistcameraFlag;
	};
	void SetCamDollyFlag(bool srcval) {
		m_cameradollyFlag = srcval;
	};
	bool GetCamDollyFlag() {
		return m_cameradollyFlag;
	};
	void SetCamMvStep(float srcval) {
		m_cammvstep = srcval;
	};
	float GetCamMvStep() {
		return m_cammvstep;
	};
	void SetInitCamDist(float srcval) {
		m_initcamdist = srcval;
	};
	float GetInitCamDist() {
		return m_initcamdist;
	}
	float GetMaxCamDist() {
		return m_maxcamdist;
	};

private:
	ChaVector3 m_camEye;
	ChaVector3 m_camtargetpos;
	ChaVector3 m_befcamEye;
	ChaVector3 m_befcamtargetpos;
	ChaVector3 m_cameraupdir;
	float m_camdist;
	float m_initcamdist;
	float m_maxcamdist;
	float m_fovy;
	float m_projnear;
	float m_projfar;
	float m_fAspectRatio;

	//カメラの地面からの高さ制御用
	float m_cameraheight;
	int m_cameraheightflag;
	CModel* m_cameragmodel;

	//選択ジョイント注視用
	int m_camtargetflag;
	int m_camtargetOnceflag;
	bool m_camtargetdisp;//カメラターゲット位置にマニピュレータを表示するかどうかのフラグ
	bool m_moveeyepos;//s_sidemenu_camdistSlider動作の種類　true:eyeposが動く、false:targetposが動く

	//カメラの子供モデル
	bool m_cameraHasChildFlag;//１つでもCameraの子供モデルがあればTRUE. OnUserFrameMove()でセット.

	//カメラアニメ用
	CModel* m_cameramodel;
	double m_cameraframe;
	int m_cameraanimmode;//0: OFF, 1:ON, 2:ON and RootMotionOption ON
	int m_cameraInheritMode;

	//マウスでの操作用
	bool m_twistcameraFlag;
	bool m_cameradollyFlag;
	float m_cammvstep;

	int m_ringbufIndexCache;
	std::vector<CModelFrameViewRingBuf> m_modelframeviewRingBuf;
};


#endif