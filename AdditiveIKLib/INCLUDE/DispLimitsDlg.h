#ifndef __CDispLimitsDlg_H_
#define __CDispLimitsDlg_H_


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
// CDispLimitsDlg
class CDispLimitsDlg
{
public:
	CDispLimitsDlg();
	~CDispLimitsDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateDispLimitsWnd();
	int ParamsToDlg();

private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_CheckBoxA* m_lightsChk;
	OrgWinGUI::OWP_Separator* m_lightssp;
	OrgWinGUI::OWP_Slider* m_lightsSlider;
	OrgWinGUI::OWP_Separator* m_threadssp;
	OrgWinGUI::OWP_Label* m_threadsLabel;
	OrgWinGUI::OWP_Slider* m_threadsSlider;
	OrgWinGUI::OWP_CheckBoxA* m_highRpmChk;
	OrgWinGUI::OWP_Separator* m_bonemarksp;
	OrgWinGUI::OWP_CheckBoxA* m_bonemarkChk;
	OrgWinGUI::OWP_Slider* m_bonemarkSlider;
	OrgWinGUI::OWP_Separator* m_rigidmarksp;
	OrgWinGUI::OWP_CheckBoxA* m_rigidmarkChk;
	OrgWinGUI::OWP_Slider* m_rigidmarkSlider;
	OrgWinGUI::OWP_Separator* m_rigmarksp;
	OrgWinGUI::OWP_Label* m_rigmarkLabel;
	OrgWinGUI::OWP_Slider* m_rigmarkSlider;
	OrgWinGUI::OWP_Separator* m_refpossp;
	OrgWinGUI::OWP_Label* m_refposLabel;
	OrgWinGUI::OWP_Slider* m_refposSlider;
	OrgWinGUI::OWP_Separator* m_iklevelssp;
	OrgWinGUI::OWP_Label* m_iklevelsLabel;
	OrgWinGUI::OWP_ComboBoxA* m_iklevelsCombo;
	OrgWinGUI::OWP_Separator* m_axiskindsp;
	OrgWinGUI::OWP_Label* m_axiskindLabel;
	OrgWinGUI::OWP_ComboBoxA* m_axiskindCombo;
	OrgWinGUI::OWP_Separator* m_uvsetsp;
	OrgWinGUI::OWP_Label* m_uvsetLabel;
	OrgWinGUI::OWP_ComboBoxA* m_uvsetCombo;
	OrgWinGUI::OWP_Separator* m_dispsp1;
	OrgWinGUI::OWP_CheckBoxA* m_x180Chk;
	OrgWinGUI::OWP_CheckBoxA* m_rottraChk;
	OrgWinGUI::OWP_Separator* m_dispsp2;
	OrgWinGUI::OWP_CheckBoxA* m_dofChk;
	OrgWinGUI::OWP_CheckBoxA* m_bloomChk;
	OrgWinGUI::OWP_Separator* m_dispsp3;
	OrgWinGUI::OWP_CheckBoxA* m_alphaChk;
	OrgWinGUI::OWP_CheckBoxA* m_zcmpChk;
	OrgWinGUI::OWP_Separator* m_dispsp4;
	OrgWinGUI::OWP_CheckBoxA* m_freefpsChk;
	OrgWinGUI::OWP_CheckBoxA* m_skydispChk;
	OrgWinGUI::OWP_Separator* m_dispsp5;
	OrgWinGUI::OWP_CheckBoxA* m_graphskipMOAChk;
	OrgWinGUI::OWP_Label* m_dispspacerLabel001;
	OrgWinGUI::OWP_Label* m_dispspacerLabel002;
	OrgWinGUI::OWP_Label* m_dispspacerLabel003;

};

#endif //__ColiIDDlg_H_
