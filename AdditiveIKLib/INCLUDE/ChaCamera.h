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

	void ProcessRefPosView();
	ChaMatrix GetRefPosView(int srcrefposindex);

private:
	void DestroyObjs();

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

	ChaMatrix m_refposView[REFPOSMAXNUM];
	int m_refposStartIndex;
	int m_refposRecordCount;
};


#endif