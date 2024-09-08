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

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);
	int ParamsToDlg();

	int CreateFootRigWnd();


	int SaveFootRigFile(WCHAR* srcprojectdir, WCHAR* srcprojectname, ChaScene* srcchascene);
	int LoadFootRigFile(WCHAR* savechadir, WCHAR* saveprojname);

	int Update();
	int Update(CModel* srcmodel);
	void FootRig(bool secondcalling,
		CModel* srcmodel,
		FOOTRIGELEM curelem,
		CBone* lowerfoot, CBone* higherfoot,
		CBone* lowerupdatebone, CBone* higherupdatebone,
		CUSTOMRIG lowerrig, CUSTOMRIG higherrig,
		ChaVector3 lowerjointpos, ChaVector3 higherjointpos,
		ChaVector3 lowergpos, ChaVector3 highergpos,
		float loweroffset, float higheroffset,
		int lowerdir, int higherdir,
		int lowerrignum, int higherrignum
	);

	void SetSaveModelWM(CModel* srcmodel, ChaMatrix srcmat);
	ChaMatrix GetSaveModelWM(CModel* srcmodel);
	bool IsEnableFootRig(CModel* srcmodel);

private:
	int Dlg2Params();
	int ParamsToDlg_LeftRig();
	int ParamsToDlg_RightRig();

	ChaMatrix BlendSaveModelWM(CModel* srcmodel, ChaMatrix srcmat, float blendrate);
	ChaMatrix GetJointWM(CModel* srcmodel, CBone* srcbone, bool multmodelwm);
	ChaVector3 GetJointPos(CModel* srcmodel, CBone* srcbone);
	ChaVector3 GetGroundPos(CModel* groundmodel, ChaVector3 basepos);
	ChaMatrix ModelShiftY(CModel* srcmodel, ChaMatrix befwm, float diffy);
	ChaVector3 RigControlFootRig(CModel* srcmodel, CBone* footbone, CBone* updatebone, double curframe,
		ChaVector3 bonepos,
		int rigdir, float posoffset, float rigstep, int maxcalccount, 
		CUSTOMRIG footrig, int rignum,
		ChaMatrix modelwm, ChaMatrix matView, ChaMatrix matProj,
		CModel* groundmodel, ChaVector3* pgroundpos);
	CBone* GetUpdateBone(CModel* srcmodel, CBone* footbone, CUSTOMRIG footrig, int rigdir, int* prignum);//Rigで回転するボーンの内の一番親のボーンを返す

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

	OrgWinGUI::OWP_Label* m_leftfootlabel;
	OrgWinGUI::OWP_Label* m_leftfootBonelabel;
	OrgWinGUI::OWP_ComboBoxA* m_leftfootBoneCombo;
	OrgWinGUI::OWP_Label* m_leftoffsetLabel;
	OrgWinGUI::OWP_EditBox* m_leftoffsetEdit;
	OrgWinGUI::OWP_Label* m_leftriglabel;
	OrgWinGUI::OWP_ComboBoxA* m_leftrigCombo;
	OrgWinGUI::OWP_Label* m_leftdirlabel;
	OrgWinGUI::OWP_ComboBoxA* m_leftdirCombo;

	OrgWinGUI::OWP_Label* m_rightfootlabel;
	OrgWinGUI::OWP_Label* m_rightfootBonelabel;
	OrgWinGUI::OWP_ComboBoxA* m_rightfootBoneCombo;
	OrgWinGUI::OWP_Label* m_rightoffsetLabel;
	OrgWinGUI::OWP_EditBox* m_rightoffsetEdit;
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

	OrgWinGUI::OWP_Button* m_applyB;

	OrgWinGUI::OWP_Separator* m_groundmeshsp;
	OrgWinGUI::OWP_Separator* m_leftfootbonesp;
	OrgWinGUI::OWP_Separator* m_leftoffsetsp;
	OrgWinGUI::OWP_Separator* m_leftrigsp;
	OrgWinGUI::OWP_Separator* m_leftdirsp;
	OrgWinGUI::OWP_Separator* m_rightfootbonesp;
	OrgWinGUI::OWP_Separator* m_rightoffsetsp;
	OrgWinGUI::OWP_Separator* m_rightrigsp;
	OrgWinGUI::OWP_Separator* m_rightdirsp;
	OrgWinGUI::OWP_Separator* m_hdiffmaxsp;
	OrgWinGUI::OWP_Separator* m_rigstepsp;
	OrgWinGUI::OWP_Separator* m_maxcountsp;//2024/09/08

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

#endif //__ColiIDDlg_H_
