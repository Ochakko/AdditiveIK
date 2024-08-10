#ifndef __CImpulseDlg_H_
#define __CImpulseDlg_H_


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
// CImpulseDlg
class CImpulseDlg
{
public:
	CImpulseDlg();
	~CImpulseDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!
	//int SetFunctions(
	//	void (*srcSetCamera3DFromEyePos)()
	//);


	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel, int srccurboneno, std::map<CModel*, int> srcrgdindexmap);
	int ParamsToDlg();
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateImpulseWnd();

public:
	bool GetImpulseCloseFlag()
	{
		return m_closeFlag;
	};
	void SetImpulseCloseFlag(bool srcval)
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

	CModel* m_model;
	int m_curboneno;
	std::map<CModel*, int> m_rgdindexmap;

	bool m_closeFlag;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_CheckBoxA* m_impgroupcheck;
	OrgWinGUI::OWP_Slider* m_impxSlider;
	OrgWinGUI::OWP_Slider* m_impySlider;
	OrgWinGUI::OWP_Slider* m_impzSlider;
	OrgWinGUI::OWP_Slider* m_impscaleSlider;
	OrgWinGUI::OWP_Label* m_impxlabel;
	OrgWinGUI::OWP_Label* m_impylabel;
	OrgWinGUI::OWP_Label* m_impzlabel;
	OrgWinGUI::OWP_Label* m_impscalelabel;
	OrgWinGUI::OWP_Button* m_impallB;


};

#endif //__ColiIDDlg_H_
