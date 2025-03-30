#ifndef __CProjLodDlg_H_
#define __CProjLodDlg_H_


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
// CProjLodDlg
class CProjLodDlg
{
public:
	CProjLodDlg();
	~CProjLodDlg();

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
	int ParamsToDlg();

	int CreateProjLodWnd();

private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Label* m_lodprojLabel;
	OrgWinGUI::OWP_Separator* m_lodprojfovsp0;
	OrgWinGUI::OWP_Label* m_lodprojfovLabel;
	OrgWinGUI::OWP_Slider* m_lodprojfovSlider;
	OrgWinGUI::OWP_Separator* m_lodnearfarsp0;
	OrgWinGUI::OWP_Separator* m_lodnearfarsp1;
	OrgWinGUI::OWP_Separator* m_lodnearfarsp2;
	OrgWinGUI::OWP_Label* m_lodnearLabel;
	OrgWinGUI::OWP_EditBox* m_lodnearEdit;
	OrgWinGUI::OWP_Label* m_lodfarLabel;
	OrgWinGUI::OWP_EditBox* m_lodfarEdit;
	OrgWinGUI::OWP_Separator* m_lodpickdistsp;
	OrgWinGUI::OWP_Label* m_lodpickdistLabel;
	OrgWinGUI::OWP_Slider* m_lodpickdistSlider;
	OrgWinGUI::OWP_Label* m_lodlevel2Label;
	OrgWinGUI::OWP_Separator* m_lodlevel2lod0sp;
	OrgWinGUI::OWP_Label* m_lodlevel2lod0Label;
	OrgWinGUI::OWP_Slider* m_lodlevel2lod0Slider;
	OrgWinGUI::OWP_Separator* m_lodlevel2lod1sp;
	OrgWinGUI::OWP_Label* m_lodlevel2lod1Label;
	OrgWinGUI::OWP_Slider* m_lodlevel2lod1Slider;
	OrgWinGUI::OWP_Label* m_lodlevel3Label;
	OrgWinGUI::OWP_Separator* m_lodlevel3lod0sp;
	OrgWinGUI::OWP_Label* m_lodlevel3lod0Label;
	OrgWinGUI::OWP_Slider* m_lodlevel3lod0Slider;
	OrgWinGUI::OWP_Separator* m_lodlevel3lod1sp;
	OrgWinGUI::OWP_Label* m_lodlevel3lod1Label;
	OrgWinGUI::OWP_Slider* m_lodlevel3lod1Slider;
	OrgWinGUI::OWP_Separator* m_lodlevel3lod2sp;
	OrgWinGUI::OWP_Label* m_lodlevel3lod2Label;
	OrgWinGUI::OWP_Slider* m_lodlevel3lod2Slider;
	OrgWinGUI::OWP_Label* m_lodlevel4Label;
	OrgWinGUI::OWP_Separator* m_lodlevel4lod0sp;
	OrgWinGUI::OWP_Label* m_lodlevel4lod0Label;
	OrgWinGUI::OWP_Slider* m_lodlevel4lod0Slider;
	OrgWinGUI::OWP_Separator* m_lodlevel4lod1sp;
	OrgWinGUI::OWP_Label* m_lodlevel4lod1Label;
	OrgWinGUI::OWP_Slider* m_lodlevel4lod1Slider;
	OrgWinGUI::OWP_Separator* m_lodlevel4lod2sp;
	OrgWinGUI::OWP_Label* m_lodlevel4lod2Label;
	OrgWinGUI::OWP_Slider* m_lodlevel4lod2Slider;
	OrgWinGUI::OWP_Separator* m_lodlevel4lod3sp;
	OrgWinGUI::OWP_Label* m_lodlevel4lod3Label;
	OrgWinGUI::OWP_Slider* m_lodlevel4lod3Slider;
	//OrgWinGUI::OWP_Label* m_lodspacer1Label;
	//OrgWinGUI::OWP_Separator* m_lodapplysp;
	//OrgWinGUI::OWP_Button* m_lodapplyB;
	OrgWinGUI::OWP_Label* m_lodspacerLabel001;
	OrgWinGUI::OWP_Label* m_lodspacerLabel002;
	OrgWinGUI::OWP_Label* m_lodspacerLabel003;



};

#endif //__ColiIDDlg_H_
