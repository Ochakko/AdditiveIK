#ifndef __CGrassDlg_H_
#define __CGrassDlg_H_


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
// CGrassDlg
class CGrassDlg
{
public:
	CGrassDlg();
	~CGrassDlg();

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
	int CreateGrassWnd();

public:
	ChaVector4 GetDiffuseRate() {
		return m_diffuserate;
	}
	void SetDiffuseRate(ChaVector4 srcval) {
		m_diffuserate = srcval;
	}
	ChaVector3 GetShapeScale() {
		return m_shapescale;
	}
	void SetShapeScale(ChaVector3 srcval) {
		m_shapescale = srcval;
	}
	float GetBendScale() {
		return m_bendscale;
	}
	void SetBendScale(float srcval) {
		m_bendscale = srcval;
	}

private:

	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;
	ChaVector3 m_shapescale;
	ChaVector4 m_diffuserate;
	float m_bendscale;

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Label* m_diffuseLabel;
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


	OrgWinGUI::OWP_Label* m_shapescaleLabel;
	OrgWinGUI::OWP_Separator* m_shapescaleXsp;
	OrgWinGUI::OWP_Label* m_shapescaleXLabel;
	OrgWinGUI::OWP_Slider* m_shapescaleXSlider;

	OrgWinGUI::OWP_Separator* m_shapescaleYsp;
	OrgWinGUI::OWP_Label* m_shapescaleYLabel;
	OrgWinGUI::OWP_Slider* m_shapescaleYSlider;

	OrgWinGUI::OWP_Separator* m_shapescaleZsp;
	OrgWinGUI::OWP_Label* m_shapescaleZLabel;
	OrgWinGUI::OWP_Slider* m_shapescaleZSlider;


	OrgWinGUI::OWP_Label* m_bendLabel;
	OrgWinGUI::OWP_Separator* m_bendscalesp;
	OrgWinGUI::OWP_Label* m_bendscaleLabel;
	OrgWinGUI::OWP_Slider* m_bendscaleSlider;

	OrgWinGUI::OWP_Label* m_nameLabel;
	OrgWinGUI::OWP_Label* m_grassLabel;
	OrgWinGUI::OWP_Label* m_space01Label;
	OrgWinGUI::OWP_Label* m_space02Label;
	OrgWinGUI::OWP_Label* m_space03Label;
	OrgWinGUI::OWP_Label* m_space04Label;
	OrgWinGUI::OWP_Label* m_space05Label;
	OrgWinGUI::OWP_Label* m_space06Label;
	OrgWinGUI::OWP_Label* m_space07Label;


	//void (*m_PrepairUndo)();
	//int (*m_UpdateAfterEditAngleLimit)(int limit2boneflag, bool setcursorflag);//default : setcursorflag = true

};

#endif //__ColiIDDlg_H_
