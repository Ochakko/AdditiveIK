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


enum {
	UNDOKIND_EDITMOTION,//ボーンモーション、カメラアニメ編集
	UNDOKIND_SELECTMODEL_FROMTHIS,//カレントモデル選択変更
	UNDOKIND_SELECTMODEL_TOTHIS,//カレントモデル選択変更
	UNDOKIND_SELECTMOTION,//カレントモーション選択変更
	UNDOKIND_SELECTCAMERAANIM,//カレントカメラアニメ選択変更
	UNDOKIND_MAX
};

typedef struct tag_undoselect
{
	int undokind;
	CModel* from_model;
	CModel* to_model;
	
	int from_motion;
	int to_motion;

	//int from_undoR;
	//int from_undoW;
	//int to_undoR;
	//int to_undoW;

	CModel* cameramodel;
	int from_cameraanim;
	int to_cameraanim;

	void Init() {
		undokind = UNDOKIND_EDITMOTION;
		from_model = nullptr;
		to_model = nullptr;
		from_motion = 0;
		to_motion = 0;
		//from_undoR = 0;
		//from_undoW = 0;
		//to_undoR = 0;
		//to_undoW = 0;
		cameramodel = nullptr;
		from_cameraanim = 0;
		to_cameraanim = 0;
	};
	tag_undoselect() {
		Init();
	};
}UNDOSELECT;

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
	int SaveUndoMotion(UNDOSELECT srcundoselect, bool LimitDegCheckBoxFlag, bool limitdegflag, CModel* pmodel, 
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
		UNDOSELECT* dstundoselect,
		BRUSHSTATE* dstbrushstate, UNDOCAMERA* dstundocamera, UNDOMOTID* dstundomotid);

	bool IsUndoSelectModelFromThis(CModel* srcmodel) {
		if (m_undoselect.undokind == UNDOKIND_SELECTMODEL_FROMTHIS) {
			if (m_undoselect.from_model = srcmodel) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	};
	bool IsUndoSelectModelToThis(CModel* srcmodel) {
		if (m_undoselect.undokind == UNDOKIND_SELECTMODEL_TOTHIS) {
			if (m_undoselect.to_model = srcmodel) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	};


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

	UNDOSELECT GetUndoSelect()
	{
		return m_undoselect;
	}

private:
	int m_validflag;

	UNDOSELECT m_undoselect;

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