#ifndef __CFogDlg_H_
#define __CFogDlg_H_


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
// CFogDlg
class CFogDlg
{
public:
	CFogDlg();
	~CFogDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!
	//int SetFunctions(
	//	void (*srcSetCamera3DFromEyePos)()
	//);


	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	//void SetModel(CModel* srcmodel, int srccurboneno, std::unordered_map<CModel*, int> srcrgdindexmap);
	int ParamsToDlg();
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateFogWnd();
	//int DlgToParams();
	void CheckFogKindParamsButton(int srckind);

public:
	bool GetFogCloseFlag()
	{
		return m_closeFlag;
	};
	void SetFogCloseFlag(bool srcval)
	{
		m_closeFlag = srcval;
	};


private:

	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	bool m_closeFlag;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_ComboBoxA* m_fogslotCombo;
	OrgWinGUI::OWP_Separator* m_fogkindsp1;
	OrgWinGUI::OWP_Separator* m_fogkindsp2;
	OrgWinGUI::OWP_Separator* m_fogkindsp3;
	OrgWinGUI::OWP_CheckBoxA* m_fogkindnoChk;
	OrgWinGUI::OWP_CheckBoxA* m_fogkinddistChk;
	OrgWinGUI::OWP_CheckBoxA* m_fogkindheightChk;
	OrgWinGUI::OWP_Label* m_fogspacerLabel1;
	OrgWinGUI::OWP_Label* m_fogdistLabel;
	OrgWinGUI::OWP_Separator* m_fogdistsp1;
	OrgWinGUI::OWP_Separator* m_fogdistsp2;
	OrgWinGUI::OWP_Separator* m_fogdistsp3;
	OrgWinGUI::OWP_Label* m_fogdistnearLabel;
	OrgWinGUI::OWP_EditBox* m_fogdistnearEdit;
	OrgWinGUI::OWP_Label* m_fogdistfarLabel;
	OrgWinGUI::OWP_EditBox* m_fogdistfarEdit;
	OrgWinGUI::OWP_Separator* m_fogdistsp4;
	OrgWinGUI::OWP_ColorBox* m_fogdistColor;
	OrgWinGUI::OWP_Slider* m_fogdistSlider;
	OrgWinGUI::OWP_Label* m_fogspacerLabel2;
	OrgWinGUI::OWP_Separator* m_fogheightsp1;
	OrgWinGUI::OWP_Separator* m_fogheightsp2;
	OrgWinGUI::OWP_Separator* m_fogheightsp3;
	OrgWinGUI::OWP_Label* m_fogheightminLabel;
	OrgWinGUI::OWP_EditBox* m_fogheightminEdit;
	OrgWinGUI::OWP_Label* m_fogheightmaxLabel;
	OrgWinGUI::OWP_EditBox* m_fogheightmaxEdit;
	OrgWinGUI::OWP_Separator* m_fogheightsp4;
	OrgWinGUI::OWP_ColorBox* m_fogheightColor;
	OrgWinGUI::OWP_Slider* m_fogheightSlider;
	//OrgWinGUI::OWP_Label* m_fogspacerLabel3;
	//OrgWinGUI::OWP_Separator* m_fogapplysp;
	//OrgWinGUI::OWP_Button* m_fogapplyB;

};

#endif //__ColiIDDlg_H_
