#ifndef __CLightsDlg_H_
#define __CLightsDlg_H_


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
// CLightsDlg
class CLightsDlg
{
public:
	CLightsDlg();
	~CLightsDlg();

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

	int CreateLightsWnd();

private:
	int ConvDir2PolarCoord(float srcdirx, float srcdiry, float srcdirz, float* dstxzdeg, float* dstydeg);
	int ConvPolarCoord2Dir(float srcxzdeg, float srcydeg, float* dstdirx, float* dstdiry, float* dstdirz);
	int CheckStr_float(const WCHAR* srcstr);

	int Dlg2LightsListener();

private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Separator* m_lightsslotsp;
	OrgWinGUI::OWP_ComboBoxA* m_lightsslotCombo;
	OrgWinGUI::OWP_Label* m_lightsslotLabel;
	//OrgWinGUI::OWP_GroupBox* m_lightsgroup[8];
	OrgWinGUI::OWP_Label* m_lightsgroupLabel[8];
	OrgWinGUI::OWP_Separator* m_lightschecksp[8];
	OrgWinGUI::OWP_CheckBoxA* m_lightsenableChk[8];
	OrgWinGUI::OWP_CheckBoxA* m_lightsviewrotChk[8];
	OrgWinGUI::OWP_Separator* m_lightscolorsp[8];
	OrgWinGUI::OWP_ColorBox* m_lightscolorBox[8];
	OrgWinGUI::OWP_Slider* m_lightsmultSlider[8];
	OrgWinGUI::OWP_Separator* m_polarsp0[8];
	OrgWinGUI::OWP_Separator* m_polarsp1[8];
	OrgWinGUI::OWP_Separator* m_polarsp2[8];
	OrgWinGUI::OWP_Label* m_polarxzLabel[8];
	OrgWinGUI::OWP_EditBox* m_polarxzEdit[8];
	//OrgWinGUI::OWP_Button* m_polarxzEdit[8];
	OrgWinGUI::OWP_Label* m_polaryLabel[8];
	OrgWinGUI::OWP_EditBox* m_polaryEdit[8];
	//OrgWinGUI::OWP_Button* m_polaryEdit[8];
	//OrgWinGUI::OWP_Separator* m_lightsapplysp;
	//OrgWinGUI::OWP_Label* m_lightsspace1Label;
	//OrgWinGUI::OWP_Button* m_lightsapplyB;



};

#endif //__ColiIDDlg_H_
