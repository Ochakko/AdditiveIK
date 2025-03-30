#ifndef __CShadowDlg_H_
#define __CShadowDlg_H_


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
// CShadowDlg
class CShadowDlg
{
public:
	CShadowDlg();
	~CShadowDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!
	int SetFunctions(
		void (*srcSetCamera3DFromEyePos)()
	);


	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	//void SetModel(CModel* srcmodel, CShaderTypeParams* srcshadertypeparams);
	int ParamsToDlg();
	void SetVisible(bool srcflag);
	//int OnFrameSkyParams();

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateShadowWnd();
	void CheckShadowDirectionButton(int srcshadowdir);

public:


private:

	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Separator* m_shadowslotsp;
	OrgWinGUI::OWP_CheckBoxA* m_shadowenableChk;
	OrgWinGUI::OWP_ComboBoxA* m_shadowslotCombo;
	OrgWinGUI::OWP_Separator* m_shadowvsmsp;
	OrgWinGUI::OWP_CheckBoxA* m_shadowvsmChk;
	OrgWinGUI::OWP_CheckBoxA* m_shadowvsmblurChk;
	OrgWinGUI::OWP_Label* m_shadowcamposLabel;
	OrgWinGUI::OWP_Separator* m_shadowcamposupsp0;
	OrgWinGUI::OWP_Separator* m_shadowcamposupsp1;
	OrgWinGUI::OWP_Separator* m_shadowcamposupsp2;
	OrgWinGUI::OWP_Label* m_shadowcamposupLabel;
	OrgWinGUI::OWP_EditBox* m_shadowcamposupEdit;
	OrgWinGUI::OWP_Separator* m_shadowcamposdistsp0;
	OrgWinGUI::OWP_Separator* m_shadowcamposdistsp1;
	OrgWinGUI::OWP_Separator* m_shadowcamposdistsp2;
	OrgWinGUI::OWP_Label* m_shadowcamposdistLabel;
	OrgWinGUI::OWP_EditBox* m_shadowcamposdistEdit;
	OrgWinGUI::OWP_Label* m_shadowcamposdistxLabel;
	OrgWinGUI::OWP_Label* m_shadowcamdirLabel;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp0;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp1;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp2;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp3;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp4;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp5;
	OrgWinGUI::OWP_Separator* m_shadowcamdirsp6;
	OrgWinGUI::OWP_CheckBoxA* m_shadowcamdirChk[8];
	OrgWinGUI::OWP_Label* m_shadowprojLabel;
	OrgWinGUI::OWP_Separator* m_shadowprojfovsp;
	OrgWinGUI::OWP_Label* m_shadowprojfovLabel;
	OrgWinGUI::OWP_Slider* m_shadowprojfovSlider;
	OrgWinGUI::OWP_Separator* m_shadowprojnearfarsp0;
	OrgWinGUI::OWP_Separator* m_shadowprojnearfarsp1;
	OrgWinGUI::OWP_Separator* m_shadowprojnearfarsp2;
	OrgWinGUI::OWP_Label* m_shadowprojnearLabel;
	OrgWinGUI::OWP_EditBox* m_shadowprojnearEdit;
	OrgWinGUI::OWP_Label* m_shadowprojfarLabel;
	OrgWinGUI::OWP_EditBox* m_shadowprojfarEdit;
	OrgWinGUI::OWP_Label* m_shadowotherLabel;
	OrgWinGUI::OWP_Separator* m_shadowothersp1;
	OrgWinGUI::OWP_Label* m_shadowothercolorLabel;
	OrgWinGUI::OWP_Slider* m_shadowothercolorSlider;
	OrgWinGUI::OWP_Separator* m_shadowothersp2;
	OrgWinGUI::OWP_Label* m_shadowotherbiasLabel;
	OrgWinGUI::OWP_Slider* m_shadowotherbiasSlider;
	OrgWinGUI::OWP_Separator* m_shadowothersp3;
	OrgWinGUI::OWP_Label* m_shadowothermultLabel;
	OrgWinGUI::OWP_Slider* m_shadowothermultSlider;
	OrgWinGUI::OWP_Label* m_shadowspacerLabel;
	OrgWinGUI::OWP_Separator* m_shadowapplysp;
	//OrgWinGUI::OWP_Button* m_shadowapplyB;
	OrgWinGUI::OWP_Button* m_shadowinitB;
	OrgWinGUI::OWP_Label* m_shadowspacerLabel001;
	OrgWinGUI::OWP_Label* m_shadowspacerLabel002;
	OrgWinGUI::OWP_Label* m_shadowspacerLabel003;


	void (*m_SetCamera3DFromEyePos)();

};

#endif //__ColiIDDlg_H_
