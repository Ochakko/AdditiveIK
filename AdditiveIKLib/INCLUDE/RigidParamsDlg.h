#ifndef __CRigidParamsDlg_H_
#define __CRigidParamsDlg_H_


#include <windows.h>

#include <ChaVecCalc.h>

#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>
#include <unordered_map>

class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;


/////////////////////////////////////////////////////////////////////////////
// CRigidParamsDlg
class CRigidParamsDlg
{
public:
	CRigidParamsDlg();
	~CRigidParamsDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	int SetModel(CModel* srcmodel, int srcboneno, 
		std::unordered_map<CModel*, int>& srcreindexmap, std::unordered_map<CModel*, int>& srcrgdindexmap);
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateRigidWnd();
	int ParamsToDlg();
	int SetRigidLeng();
	int RigidElem2WndParam();

private:
	bool m_createdflag;
	bool m_visible;

	CModel* m_model;
	int m_curboneno;
	std::unordered_map<CModel*, int> m_reindexmap;
	std::unordered_map<CModel*, int> m_rgdindexmap;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	OrgWinGUI::OrgWindow* m_rigidWnd;

	OrgWinGUI::OWP_ScrollWnd* m_rigidSc;
	OrgWinGUI::OWP_Separator* m_rigidspall;
	OrgWinGUI::OWP_Separator* m_rigidsp0;
	OrgWinGUI::OWP_CheckBoxA* m_groupcheck;
	OrgWinGUI::OWP_Slider* m_sphrateSlider;
	OrgWinGUI::OWP_Slider* m_boxzSlider;
	OrgWinGUI::OWP_Slider* m_massSlider;
	//OrgWinGUI::OWP_Separator* m_massSeparator;
	//OrgWinGUI::OWP_Separator* m_massSeparator1;
	//OrgWinGUI::OWP_Separator* m_massSeparator2;
	OrgWinGUI::OWP_Button* m_massB;
	OrgWinGUI::OWP_Button* m_thicknessB;
	OrgWinGUI::OWP_Button* m_depthB;
	OrgWinGUI::OWP_Label* m_massspacelabel;
	OrgWinGUI::OWP_CheckBoxA* m_rigidskip;
	OrgWinGUI::OWP_Button* m_skipB;
	OrgWinGUI::OWP_Separator* m_forbidSeparator;
	OrgWinGUI::OWP_Button* m_forbidB;
	OrgWinGUI::OWP_CheckBoxA* m_forbidrot;
	OrgWinGUI::OWP_Label* m_shplabel;
	OrgWinGUI::OWP_Label* m_boxzlabel;
	OrgWinGUI::OWP_Separator* m_colSeparator;
	OrgWinGUI::OWP_Button* m_colB;
	OrgWinGUI::OWP_RadioButton* m_colradio;
	OrgWinGUI::OWP_RadioButton* m_lkradio;
	OrgWinGUI::OWP_RadioButton* m_akradio;
	OrgWinGUI::OWP_Slider* m_ldmpSlider;
	OrgWinGUI::OWP_Slider* m_admpSlider;
	OrgWinGUI::OWP_Label* m_massSLlabel;
	OrgWinGUI::OWP_Label* m_namelabel;
	OrgWinGUI::OWP_Label* m_lenglabel;
	OrgWinGUI::OWP_Label* m_ldmplabel;
	OrgWinGUI::OWP_Label* m_admplabel;
	OrgWinGUI::OWP_Button* m_kB;
	OrgWinGUI::OWP_Button* m_restB;
	OrgWinGUI::OWP_Button* m_dmpB;
	OrgWinGUI::OWP_Button* m_groupB;
	OrgWinGUI::OWP_Button* m_gcoliB;
	OrgWinGUI::OWP_Separator* m_validSeparator;
	OrgWinGUI::OWP_Button* m_allrigidenableB;
	OrgWinGUI::OWP_Button* m_allrigiddisableB;

	OrgWinGUI::OWP_Separator* m_thicknessSeparator;
	OrgWinGUI::OWP_Separator* m_depthSeparator;
	OrgWinGUI::OWP_Separator* m_massSeparator;
	OrgWinGUI::OWP_Separator* m_thicknessSeparator2;
	OrgWinGUI::OWP_Separator* m_depthSeparator2;
	OrgWinGUI::OWP_Separator* m_massSeparator2;
	OrgWinGUI::OWP_Separator* m_validSeparator2;
	OrgWinGUI::OWP_Separator* m_forbidSeparator2;
	OrgWinGUI::OWP_Separator* m_colSeparator2;
	OrgWinGUI::OWP_Separator* m_springSeparator;
	OrgWinGUI::OWP_Separator* m_restitutionSeparator;
	OrgWinGUI::OWP_Separator* m_dumpingSeparator;
	OrgWinGUI::OWP_Separator* m_gSeparator;
	OrgWinGUI::OWP_Separator* m_btforceSeparator2;
	OrgWinGUI::OWP_Separator* m_coliidSeparator;
	OrgWinGUI::OWP_Button* m_thicknessDeeperB;
	OrgWinGUI::OWP_Button* m_depthDeeperB;
	OrgWinGUI::OWP_Button* m_massDeeperB;
	OrgWinGUI::OWP_Button* m_validDeeperB;
	OrgWinGUI::OWP_Button* m_forbidDeeperB;
	OrgWinGUI::OWP_Button* m_colDeeperB;
	OrgWinGUI::OWP_Button* m_springDeeperB;
	OrgWinGUI::OWP_Button* m_restitutionDeeperB;
	OrgWinGUI::OWP_Button* m_dumpingDeeperB;
	OrgWinGUI::OWP_Button* m_gDeeperB;
	OrgWinGUI::OWP_Button* m_btforceDeeperB;
	OrgWinGUI::OWP_Button* m_coliidDeeperB;
	OrgWinGUI::OWP_Separator* m_coliseparator;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel01;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel02;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel03;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel04;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel05;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel06;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel07;
	OrgWinGUI::OWP_Label* m_rigidspacerLabel08;

	OrgWinGUI::OWP_Slider* m_lkSlider;
	OrgWinGUI::OWP_Label* m_lklabel;
	OrgWinGUI::OWP_Slider* m_akSlider;
	OrgWinGUI::OWP_Label* m_aklabel;

	OrgWinGUI::OWP_Slider* m_restSlider;
	OrgWinGUI::OWP_Label* m_restlabel;
	OrgWinGUI::OWP_Slider* m_fricSlider;
	OrgWinGUI::OWP_Label* m_friclabel;

	OrgWinGUI::OWP_Slider* m_btgSlider;
	OrgWinGUI::OWP_Label* m_btglabel;
	OrgWinGUI::OWP_Slider* m_btgscSlider;
	OrgWinGUI::OWP_Label* m_btgsclabel;
	OrgWinGUI::OWP_Button* m_btgB;
	OrgWinGUI::OWP_Separator* m_btforceSeparator;
	OrgWinGUI::OWP_CheckBoxA* m_btforce;
	OrgWinGUI::OWP_Button* m_btforceB;

};

#endif //__ColiIDDlg_H_
