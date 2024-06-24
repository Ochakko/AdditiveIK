#ifndef UNDOMOTIONH
#define UNDOMOTIONH

//#include <d3dx9.h>
//#include <ChaVecCalc.h>
#include <wchar.h>
#include <Coef.h>
#include <string>
#include <map>


class CModel;
class CBone;
class CMotionPoint;
class CMQOObject;
class CMorphKey;
class CEditRange;
class ChaScene;

typedef struct tag_undocamera
{
	bool spcameramode;
	int camtargetflag;
	bool camtargetdisp;
	bool moveeyepos;
	ChaVector3 camEyePos;
	ChaVector3 camtargetpos;
	ChaVector3 camUpVec;
	float camdist;
	CModel* cameramodel;//2024/06/24

	void Init() {
		spcameramode = false;
		camtargetflag = 0;
		camtargetdisp = false;
		moveeyepos = false;
		camEyePos.SetZeroVec3();
		camtargetpos.SetParams(0.0f, 0.0f, -100.0f);
		camUpVec.SetParams(0.0f, 1.0f, 0.0);
		camdist = 500.0f;
		cameramodel = nullptr;
	};

	tag_undocamera()
	{
		Init();
	};
}UNDOCAMERA;

typedef struct tag_undomotid
{
	int bonemotid;
	int cameramotid;
	int curmotid;
	double startframe;
	double endframe;
	double applyrate;

	void Init() {
		bonemotid = 0;
		cameramotid = 0;
		curmotid = 0;
		startframe = 1.0;
		endframe = 1.0;
		applyrate = 50.0;
	};

	tag_undomotid() {
		Init();
	};
}UNDOMOTID;


class CUndoMotion
{
public:
	CUndoMotion();
	~CUndoMotion();

	int ClearData();
	int SaveUndoMotion(bool LimitDegCheckBoxFlag, bool limitdegflag, CModel* pmodel, 
		int selectedboneno, int curbaseno,
		int srcedittarget,//アプリケーションのedittargetモード
		bool undocameraflag,//カメラアニメのUndoとして呼び出す場合にtrue
		CEditRange* srcer, double srcapplyrate,
		BRUSHSTATE srcbrushstate, UNDOCAMERA srcundocamera, 
		bool allframeflag);
	int RollBackMotion(ChaScene* pchascene, 
		bool undocameraflag,//カメラアニメのUndoとして呼び出す場合にtrue
		bool limitdegflag, CModel* pmodel, 
		int* edittarget, int* pselectedboneno, int* curbaseno,
		//double* dststartframe, double* dstendframe, double* dstapplyrate, 
		BRUSHSTATE* dstbrushstate, UNDOCAMERA* dstundocamera, UNDOMOTID* dstundomotid);

private:
	int InitParams();
	int DestroyObjs();

public:
	//accesser
	MOTINFO GetSaveMotInfo(){
		return m_savemotinfo;
	};

	int GetValidFlag(){
		return m_validflag;
	};
	void SetValidFlag( int srcval ){
		m_validflag = srcval;
	};

private:
	int m_validflag;
	MOTINFO m_savemotinfo;
	MOTINFO m_savecameramotinfo;
	std::map<CBone*, CMotionPoint*> m_bone2mp;
	std::map<CMQOObject*, CMorphKey*> m_base2mk;
	std::map<CBone*, std::map<double, int>> m_bonemotmark;
	std::map<CBone*, ANGLELIMIT> m_bone2limit;

	//int m_curboneno;
	int m_selectedboneno;
	int m_curbaseno;

	int m_keynum;
	
	int m_edittarget;

	BRUSHSTATE m_brushstate;
	UNDOCAMERA m_undocamera;
	UNDOMOTID m_undomotid;
};


#endif