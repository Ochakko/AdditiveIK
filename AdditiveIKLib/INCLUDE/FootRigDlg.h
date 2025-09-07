#ifndef __CFootRigDlg_H_
#define __CFootRigDlg_H_


#include <windows.h>

#include <ChaVecCalc.h>

#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>
#include <map>

class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;

class CFootInfo;
class CBone;

enum {
	FOOTRIG_LR_LEFT,
	FOOTRIG_LR_RIGHT,
	FOOTRIG_LR_MAX
};


/////////////////////////////////////////////////////////////////////////////
// CFootRigDlg
class CFootRigDlg
{
public:
	CFootRigDlg();
	~CFootRigDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	int SetModel(ChaScene* srcchascene, CModel* srcmodel);
	void SetVisible(bool srcflag);
	int SetEditedRig(CModel* srcmodel, CBone* srcrigbone, CUSTOMRIG updatedrig);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);
	int ParamsToDlg();

	int CreateFootRigWnd();


	int SaveFootRigFile(WCHAR* srcprojectdir, WCHAR* srcprojectname, ChaScene* srcchascene);
	int LoadFootRigFile(WCHAR* savechadir, WCHAR* saveprojname);

	int Update(bool limitdegflag);
	void SetSaveModelWM(CModel* srcmodel, ChaMatrix srcmat);

	bool IsEnableFootRig(CModel* srcmodel);
	int OnDelModel(CModel* srcmodel);
	int OnDellAllModel();


	ChaVector3 GetJointPos(bool limitdegflag, CModel* srcmodel, CBone* srcbone, ChaVector3 srcoffset, bool istoebase);
	ChaVector3 GetGroundPos(CModel* groundmodel, ChaVector3 basepos, bool gpuflag);


private:
	int Dlg2ParamsListener();
	int ParamsToDlg_LeftRig();
	int ParamsToDlg_RightRig();

	ChaMatrix BlendSaveModelWM(CModel* srcmodel, ChaMatrix srcmat, float blendrate);
	ChaMatrix GetJointWM(bool limitdegflag, CModel* srcmodel, CBone* srcbone, bool multmodelwm);
	ChaMatrix ModelShiftY(CModel* srcmodel, ChaMatrix befwm, float diffy, float blendrate, bool savewmflag);

	bool IsValidModel(CModel* srcmodel);//modelが削除されている場合はfalseを返す


	int Update(bool limitdegflag, CModel* srcmodel);
	void FootRig(bool secondcalling,
		bool limitdegflag, CModel* srcmodel,
		FOOTRIGELEM curelem,
		CFootInfo* lowerfootinfo, CFootInfo* higherfootinfo
	);

	ChaMatrix GetSaveModelWM(CModel* srcmodel);
	bool GetJumpFlag(CModel* srcmodel);
private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;
	ChaScene* m_chascene;
	std::map<CModel*, FOOTRIGELEM> m_footrigelem;
	std::map<CModel*, ChaMatrix> m_savemodelwm;

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_CheckBoxA* m_enableChk;

	OrgWinGUI::OWP_Label* m_modellabel;
	OrgWinGUI::OWP_Label* m_groundlabel;
	OrgWinGUI::OWP_ComboBoxA* m_groundCombo;
	OrgWinGUI::OWP_CheckBoxA* m_gpuChk;//2024/09/15

	OrgWinGUI::OWP_Label* m_leftfootlabel;
	OrgWinGUI::OWP_Label* m_leftfootBonelabel;
	OrgWinGUI::OWP_ComboBoxA* m_leftfootBoneCombo;
	OrgWinGUI::OWP_Label* m_leftoffsetLabelY1;
	OrgWinGUI::OWP_EditBox* m_leftoffsetEditY1;
	OrgWinGUI::OWP_Label* m_leftoffsetLabelZ1;
	OrgWinGUI::OWP_EditBox* m_leftoffsetEditZ1;
	OrgWinGUI::OWP_Label* m_leftoffsetLabelY2;
	OrgWinGUI::OWP_EditBox* m_leftoffsetEditY2;
	OrgWinGUI::OWP_Label* m_leftoffsetLabelZ2;
	OrgWinGUI::OWP_EditBox* m_leftoffsetEditZ2;
	OrgWinGUI::OWP_Label* m_leftriglabel;
	OrgWinGUI::OWP_ComboBoxA* m_leftrigCombo;
	OrgWinGUI::OWP_Label* m_leftdirlabel;
	OrgWinGUI::OWP_ComboBoxA* m_leftdirCombo;

	OrgWinGUI::OWP_Label* m_rightfootlabel;
	OrgWinGUI::OWP_Label* m_rightfootBonelabel;
	OrgWinGUI::OWP_ComboBoxA* m_rightfootBoneCombo;
	OrgWinGUI::OWP_Label* m_rightoffsetLabelY1;
	OrgWinGUI::OWP_EditBox* m_rightoffsetEditY1;
	OrgWinGUI::OWP_Label* m_rightoffsetLabelZ1;
	OrgWinGUI::OWP_EditBox* m_rightoffsetEditZ1;
	OrgWinGUI::OWP_Label* m_rightoffsetLabelY2;
	OrgWinGUI::OWP_EditBox* m_rightoffsetEditY2;
	OrgWinGUI::OWP_Label* m_rightoffsetLabelZ2;
	OrgWinGUI::OWP_EditBox* m_rightoffsetEditZ2;
	OrgWinGUI::OWP_Label* m_rightriglabel;
	OrgWinGUI::OWP_ComboBoxA* m_rightrigCombo;
	OrgWinGUI::OWP_Label* m_rightdirlabel;
	OrgWinGUI::OWP_ComboBoxA* m_rightdirCombo;

	OrgWinGUI::OWP_Label* m_hdiffmaxlabel;
	OrgWinGUI::OWP_EditBox* m_hdiffmaxEdit;
	OrgWinGUI::OWP_Label* m_rigsteplabel;
	OrgWinGUI::OWP_EditBox* m_rigstepEdit;
	OrgWinGUI::OWP_Label* m_maxcountlabel;//2024/09/08
	OrgWinGUI::OWP_EditBox* m_maxcountEdit;//2024/09/08
	OrgWinGUI::OWP_Label* m_hopypersteplabel;//2024/09/16
	OrgWinGUI::OWP_EditBox* m_hopyperstepEdit;//2024/09/16
	OrgWinGUI::OWP_Label* m_wmblendlabel;//2024/10/06
	OrgWinGUI::OWP_Slider* m_wmblendSlider;//2024/10/06

	//OrgWinGUI::OWP_Button* m_applyB;

	OrgWinGUI::OWP_Separator* m_groundmeshsp;
	OrgWinGUI::OWP_Separator* m_leftfootbonesp;
	OrgWinGUI::OWP_Separator* m_leftoffsetspY1;
	OrgWinGUI::OWP_Separator* m_leftoffsetspZ1;
	OrgWinGUI::OWP_Separator* m_leftoffsetspY2;
	OrgWinGUI::OWP_Separator* m_leftoffsetspZ2;
	OrgWinGUI::OWP_Separator* m_leftrigsp;
	OrgWinGUI::OWP_Separator* m_leftdirsp;
	OrgWinGUI::OWP_Separator* m_rightfootbonesp;
	OrgWinGUI::OWP_Separator* m_rightoffsetspY1;
	OrgWinGUI::OWP_Separator* m_rightoffsetspZ1;
	OrgWinGUI::OWP_Separator* m_rightoffsetspY2;
	OrgWinGUI::OWP_Separator* m_rightoffsetspZ2;
	OrgWinGUI::OWP_Separator* m_rightrigsp;
	OrgWinGUI::OWP_Separator* m_rightdirsp;
	OrgWinGUI::OWP_Separator* m_hdiffmaxsp;
	OrgWinGUI::OWP_Separator* m_rigstepsp;
	OrgWinGUI::OWP_Separator* m_maxcountsp;//2024/09/08
	OrgWinGUI::OWP_Separator* m_gpusp;//2024/09/16
	OrgWinGUI::OWP_Separator* m_hopyperstepsp;//2024/09/16
	OrgWinGUI::OWP_Separator* m_wmblendsp;//2024/10/06

	OrgWinGUI::OWP_Label* m_spacerlabel0;
	OrgWinGUI::OWP_Label* m_spacerlabel1;
	OrgWinGUI::OWP_Label* m_spacerlabel2;
	OrgWinGUI::OWP_Label* m_spacerlabel3;
	OrgWinGUI::OWP_Label* m_spacerlabel4;
	OrgWinGUI::OWP_Label* m_spacerlabel5;
	OrgWinGUI::OWP_Label* m_spacerlabel6;

	OrgWinGUI::OWP_Label* m_leftinfolabel;
	OrgWinGUI::OWP_Label* m_rightinfolabel;


};


//######################################################
//2024/10/17
//片足ずつ処理をしやすいように　あえて別クラス(CFootInfo)を作成
//######################################################
class CFootInfo
{
public:
	CFootInfo(CFootRigDlg* srcdlg) {
		InitParams();
		m_footrigdlg = srcdlg;
	};
	~CFootInfo() {
		DestroyObjs();
	}
	void InitParams() {
		m_footrigdlg = nullptr;
		m_toebasejoint = nullptr;
		m_toebasepos.SetZeroVec3();
		m_footjoint = nullptr;
		m_footpos.SetZeroVec3();

		m_updatebone = nullptr;
		m_rignum = 0;

		m_offset1.SetZeroVec3();
		m_offset2.SetZeroVec3();
		m_toebaseGpos.SetZeroVec3();
		m_footGpos.SetZeroVec3();
		m_rig.Init();
		m_rigdir = 0;
		m_groundmodel = nullptr;
		m_gpucollision = false;
	};
	void DestroyObjs() { InitParams(); };

	bool IsValid() {
		if (m_footrigdlg && m_toebasejoint && m_footjoint &&
			m_updatebone && m_groundmodel) {
			return true;
		}
		else {
			return false;
		}
	};

	int CalcPos(int limitdegflag);
	bool IsHigherGPos(CFootInfo* cmpinfo);
	bool IsHigherFootThanGround(float cmpdiff) {
		bool highertoebase = (((m_toebasepos.y + m_offset1.y) - m_toebaseGpos.y) >= cmpdiff);
		bool higherfoot = (((m_footpos.y + m_offset2.y) - m_footGpos.y) >= cmpdiff);
		if (highertoebase && higherfoot) {
			return true;
		}
		else {
			return false;
		}
	};
	int AddFootHeight(float srcdiffy) {
		m_toebasepos.y += srcdiffy;
		m_footpos.y += srcdiffy;
		return 0;
	};

	int RigControlFootRig(bool limitdegflag, CModel* srcmodel, double curframe,
		float rigstep, int maxcalccount,
		ChaMatrix modelwm, ChaMatrix matView, ChaMatrix matProj);


public:
	void SetFootInfo(int footrigLR, FOOTRIGELEM srcelem);

	ChaVector3 GetFootOffset1() {
		return m_offset1;
	};
	ChaVector3 GetFootOffset2() {
		return m_offset2;
	};
	ChaVector3 GetLowerFootOffset()
	{
		ChaVector3 retpos;
		retpos.SetZeroVec3();

		if (m_toebasejoint) {
			if (m_footjoint) {
				if (m_toebaseGpos.y <= m_footGpos.y) {
					//return m_toebaseGpos;
					return m_offset1;
				}
				else {
					//return m_footGpos;
					return m_offset2;
				}
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}

		return retpos;
	};
	ChaVector3 GetHigherFootOffset()
	{
		ChaVector3 retpos;
		retpos.SetZeroVec3();

		if (m_toebasejoint) {
			if (m_footjoint) {
				if (m_toebaseGpos.y >= m_footGpos.y) {
					//return m_toebaseGpos;
					return m_offset1;
				}
				else {
					//return m_footGpos;
					return m_offset2;
				}
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}

		return retpos;
	};





	ChaVector3 GetHigherGPos()
	{
		ChaVector3 retpos;
		retpos.SetZeroVec3();

		if (m_toebasejoint) {
			if (m_footjoint) {
				if (m_toebaseGpos.y >= m_footGpos.y) {
					return m_toebaseGpos;
				}
				else {
					return m_footGpos;
				}
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}

		return retpos;
	};
	ChaVector3 GetLowerGPos()
	{
		ChaVector3 retpos;
		retpos.SetZeroVec3();

		if (m_toebasejoint) {
			if (m_footjoint) {
				if (m_toebaseGpos.y <= m_footGpos.y) {
					return m_toebaseGpos;
				}
				else {
					return m_footGpos;
				}
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}

		return retpos;
	};

	float GetDiffBetweenHigherGPosAndFoot()
	{
		float retdiff = 0.0f;

		//低い方の足を高い方の地面にまで上げるための計算

		if (m_toebasejoint) {
			if (m_footjoint) {
				if (m_toebaseGpos.y >= m_footGpos.y) {
					//地面の高さとして高い方を採用

					//足の高さとして低い方を採用
					//retdiff = m_toebaseGpos.y - (m_toebasepos.y + m_offset1.y);
					if ((m_toebasepos.y + m_offset1.y) <= (m_footpos.y + m_offset2.y)) {
						retdiff = m_toebaseGpos.y - (m_toebasepos.y + m_offset1.y);
					}
					else {
						retdiff = m_toebaseGpos.y - (m_toebasepos.y + m_offset1.y);
					}					
				}
				else {
					//retdiff = m_footGpos.y - (m_footpos.y + m_offset2.y);
					if ((m_toebasepos.y + m_offset1.y) <= (m_footpos.y + m_offset2.y)) {
						retdiff = m_footGpos.y - (m_toebasepos.y + m_offset1.y);
					}
					else {
						retdiff = m_footGpos.y - (m_toebasepos.y + m_offset1.y);
					}

				}
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}

		return retdiff;
	};

	CBone* GetUpdateBoneForUpdateMatrix();
	//float GetDiffBetweenLowerGPosAndFoot()
	//{
	//	float retdiff = 0.0f;
	//
	//	if (m_toebasejoint) {
	//		if (m_footjoint) {
	//			if (m_toebaseGpos.y <= m_footGpos.y) {
	//				retdiff = m_toebaseGpos.y - (m_toebasepos.y + m_offset1.y);
	//			}
	//			else {
	//				retdiff = m_footGpos.y - (m_footpos.y + m_offset2.y);
	//			}
	//		}
	//		else {
	//			_ASSERT(0);
	//		}
	//	}
	//	else {
	//		_ASSERT(0);
	//	}
	//
	//	return retdiff;
	//};




private:
	CBone* GetUpdateBone(CModel* srcmodel, CBone* footbone, CUSTOMRIG footrig, int rigdir, int* prignum);//Rigで回転するボーンの内の一番親のボーンを返す

	int RigControlFootRigFunc(bool istoebase, 
		bool limitdegflag, CModel* srcmodel, double curframe,
		float rigstep, int maxcalccount,
		ChaMatrix modelwm, ChaMatrix matView, ChaMatrix matProj);


private:
	CFootRigDlg* m_footrigdlg;
	CBone* m_toebasejoint;
	CBone* m_footjoint;

	CBone* m_updatebone;
	int m_rignum;
	
	ChaVector3 m_toebasepos;
	ChaVector3 m_footpos;
	ChaVector3 m_offset1;
	ChaVector3 m_offset2;
	ChaVector3 m_toebaseGpos;
	ChaVector3 m_footGpos;
	CUSTOMRIG m_rig;
	int m_rigdir;
	CModel* m_groundmodel;
	bool m_gpucollision;
};



#endif //__ColiIDDlg_H_
