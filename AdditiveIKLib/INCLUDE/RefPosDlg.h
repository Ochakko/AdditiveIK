#ifndef __CRefPosDlg_H_
#define __CRefPosDlg_H_


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
// CRefPosDlg
class CRefPosDlg
{
public:
	CRefPosDlg();
	~CRefPosDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	int Params2Dlg();
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);
	void CallRewrite();

	//int Dlg2Params();

private:
	int CreateRefPosWnd();

public:
	int GetRefPosNum() {
		return m_refposnum;
	}
	void SetRefPosNum(int srcval) {
		m_refposnum = srcval;
	}

	ChaVector4 GetDiffuseRate() {
		return m_diffuserate;
	}
	void SetDiffuseRate(ChaVector4 srcval) {
		m_diffuserate = srcval;
	}
	bool GetRainbowMode()
	{
		return m_rainbowmode;
	}
	void SetRainbowMode(bool srcval) {
		m_rainbowmode = srcval;
	}
	
private:

	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;
	int m_refposnum;
	ChaVector4 m_diffuserate;
	bool m_rainbowmode;
	bool m_linedisp;
	bool m_monoflag;

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Separator* m_refposnumsp;
	OrgWinGUI::OWP_Label* m_refposnumLabel;
	OrgWinGUI::OWP_Slider* m_refposnumSlider;

	OrgWinGUI::OWP_Separator* m_diffuseRsp;
	OrgWinGUI::OWP_Label* m_diffuseRLabel;
	OrgWinGUI::OWP_Slider* m_diffuseRSlider;

	OrgWinGUI::OWP_Separator* m_diffuseGsp;
	OrgWinGUI::OWP_Label* m_diffuseGLabel;
	OrgWinGUI::OWP_Slider* m_diffuseGSlider;

	OrgWinGUI::OWP_Separator* m_diffuseBsp;
	OrgWinGUI::OWP_Label* m_diffuseBLabel;
	OrgWinGUI::OWP_Slider* m_diffuseBSlider;

	OrgWinGUI::OWP_Separator* m_diffuseAsp;
	OrgWinGUI::OWP_Label* m_diffuseALabel;
	OrgWinGUI::OWP_Slider* m_diffuseASlider;

	OrgWinGUI::OWP_Separator* m_rainbowsp;
	OrgWinGUI::OWP_Label* m_rainbowLabel;
	OrgWinGUI::OWP_CheckBoxA* m_rainbowcheck;

	OrgWinGUI::OWP_Separator* m_rainbowinvsp;
	OrgWinGUI::OWP_Label* m_rainbowinvLabel;
	OrgWinGUI::OWP_CheckBoxA* m_rainbowinvcheck;

	OrgWinGUI::OWP_Separator* m_rainbowtimesp;
	OrgWinGUI::OWP_Label* m_rainbowtimeLabel;
	OrgWinGUI::OWP_CheckBoxA* m_rainbowtimecheck;

	OrgWinGUI::OWP_Separator* m_linesp;
	OrgWinGUI::OWP_Label* m_lineLabel;
	OrgWinGUI::OWP_CheckBoxA* m_linecheck;

	OrgWinGUI::OWP_Separator* m_monosp;
	OrgWinGUI::OWP_Label* m_monoLabel;
	OrgWinGUI::OWP_CheckBoxA* m_monocheck;

	OrgWinGUI::OWP_Label* m_nameLabel;
	OrgWinGUI::OWP_Label* m_space01Label;
	OrgWinGUI::OWP_Label* m_space02Label;
	OrgWinGUI::OWP_Label* m_space03Label;
	OrgWinGUI::OWP_Label* m_space04Label;
	OrgWinGUI::OWP_Label* m_space05Label;

	OrgWinGUI::OWP_Separator* m_intervalsp;
	OrgWinGUI::OWP_Label* m_intervalLabel;
	OrgWinGUI::OWP_Slider* m_intervalSlider;

	//void (*m_PrepairUndo)();
	//int (*m_UpdateAfterEditAngleLimit)(int limit2boneflag, bool setcursorflag);//default : setcursorflag = true

};

#endif //__ColiIDDlg_H_
