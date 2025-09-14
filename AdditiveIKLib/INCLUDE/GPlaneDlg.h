#ifndef __CGPlaneDlg_H_
#define __CGPlaneDlg_H_


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

class BPWorld;

/////////////////////////////////////////////////////////////////////////////
// CGPlaneDlg
class CGPlaneDlg
{
public:
	CGPlaneDlg();
	~CGPlaneDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!
	//int SetFunctions(
	//	void (*srcSetCamera3DFromEyePos)()
	//);


	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel, BPWorld* srcbpworld);
	int ParamsToDlg();
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateGPlaneWnd();

public:
	bool GetGPlaneCloseFlag()
	{
		return m_closeFlag;
	};
	void SetGPlaneCloseFlag(bool srcval)
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

	CModel* m_gplane;
	BPWorld* m_bpWorld;

	bool m_closeFlag;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Slider* m_ghSlider;
	OrgWinGUI::OWP_Slider* m_gsizexSlider;
	OrgWinGUI::OWP_Slider* m_gsizezSlider;
	OrgWinGUI::OWP_Label* m_ghlabel;
	OrgWinGUI::OWP_Label* m_gsizexlabel;
	OrgWinGUI::OWP_Label* m_gsizezlabel;
	OrgWinGUI::OWP_CheckBoxA* m_gpdisp;
	OrgWinGUI::OWP_Slider* m_grestSlider;
	OrgWinGUI::OWP_Label* m_grestlabel;
	OrgWinGUI::OWP_Slider* m_gfricSlider;
	OrgWinGUI::OWP_Label* m_gfriclabel;

};

#endif //__ColiIDDlg_H_
