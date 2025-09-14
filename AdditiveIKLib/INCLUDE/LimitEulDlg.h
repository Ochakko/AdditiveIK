#ifndef __CLimitEulDlg_H_
#define __CLimitEulDlg_H_


#include <windows.h>

#include <ChaVecCalc.h>

#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>
#include <unordered_map>


//2022/12/05
#define ANGLEDLGEDITLEN	256
enum {
	eLIM2BONE_LIM2BONE_ONE,
	eLIM2BONE_LIM2BONE_DEEPER,
	eLIM2BONE_LIM2BONE_ALL,
	eLIM2BONE_BONE2LIM,
	eLIM2BONE_NONE,
	eLIM2BONE_MAX
};


class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;

/////////////////////////////////////////////////////////////////////////////
// CLimitEulDlg
class CLimitEulDlg
{
public:
	CLimitEulDlg();
	~CLimitEulDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	int SetFunctions(
		void (*srcPrepairUndo)(),
		int (*srcUpdateAfterEditAngleLimit)(int limit2boneflag, bool setcursorflag)
	);

	void SetModel(CModel* srcmodel, int srccurboneno);
	int AngleLimit2Dlg(bool updateonlycheckeul);
	//int ParamsToDlg(CMQOMaterial* srcmat, CShaderTypeParams* srcshadertypeparams);
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);
	void CallRewrite();

	int AngleLimit2Bone(int limit2boneflag);
	int AngleDlg2AngleLimit();//2022/12/05 エラー入力通知ダイアログも出す
	int Bone2AngleLimit(int srccurboneno);

private:
	int CreateLimitEulWnd();
	int GetAngleLimitEditIntOWP(OrgWinGUI::OWP_EditBox* srcedit, int* dstlimit);
	int CheckStr_SInt(const WCHAR* srcstr);
	void LimitRate2Bone_Req(CBone* srcbone, int setbroflag);

	int AngleLimit2Bone_One(CBone* srconbe);
	void AngleLimit2Bone_Req(CBone* srcbone, int setbroflag);

public:
	bool GetAngleLimitChangeFlag()
	{
		return m_changelimitangleFlag;
	};
	void SetAngleLimitChangeFlag(bool srcval)
	{
		m_changelimitangleFlag = srcval;
	};
	bool GetBefLimitDegFlag()
	{
		return m_beflimitdegflag;
	};
	void SetBefLimitDegFlag(bool srcval)
	{
		m_beflimitdegflag = srcval;
	};
	bool GetSaveLimitDegFlag()
	{
		return m_savelimitdegflag;
	};
	void SetSaveLimitDegFlag(bool srcval)
	{
		m_savelimitdegflag = srcval;
	};

	CBone* GetAngleLimitBone()
	{
		return m_anglelimitbone;
	};
	void SetAngleLimitBone(CBone* srcval)
	{
		m_anglelimitbone = srcval;
	};
	ANGLELIMIT GetAngleLimit() 
	{
		return m_anglelimit;
	};
	void SetAngleLimit(ANGLELIMIT srcval)
	{
		m_anglelimit = srcval;
	};
	ANGLELIMIT GetAngleLimitCopy() 
	{
		return m_anglelimitcopy;
	};
	void SetAngleLimitCopy(ANGLELIMIT srcval)
	{
		m_anglelimitcopy = srcval;
	};
	int GetAngleLimitUnderHScroll()
	{
		return m_underanglelimithscroll;
	};
	void SetAngleLimitUnderHScroll(int srcval)
	{
		m_underanglelimithscroll = srcval;
	};

	
private:

	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;
	bool m_dispanglelimit;
	ANGLELIMIT m_anglelimit;
	ANGLELIMIT m_anglelimitcopy;
	CBone* m_anglelimitbone;
	bool m_beflimitdegflag;
	bool m_savelimitdegflag;
	bool m_changelimitangleFlag;
	int m_underanglelimithscroll;

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Label* m_limitnameLabel;
	OrgWinGUI::OWP_Separator* m_limitxsp1;
	OrgWinGUI::OWP_Separator* m_limitxsp2;
	OrgWinGUI::OWP_Separator* m_limitxsp2a;
	OrgWinGUI::OWP_Separator* m_limitxsp2b;
	OrgWinGUI::OWP_Separator* m_limitxsp3;
	OrgWinGUI::OWP_Separator* m_limitxsp3a;
	OrgWinGUI::OWP_Label* m_limitxupperLabel;
	OrgWinGUI::OWP_EditBox* m_limitxupperEdit;
	OrgWinGUI::OWP_Label* m_limitxlowerLabel;
	OrgWinGUI::OWP_EditBox* m_limitxlowerEdit;
	OrgWinGUI::OWP_Label* m_limitxchkLabel;
	OrgWinGUI::OWP_EditBox* m_limitxchkEdit;
	OrgWinGUI::OWP_Separator* m_limitysp1;
	OrgWinGUI::OWP_Separator* m_limitysp2;
	OrgWinGUI::OWP_Separator* m_limitysp2a;
	OrgWinGUI::OWP_Separator* m_limitysp2b;
	OrgWinGUI::OWP_Separator* m_limitysp3;
	OrgWinGUI::OWP_Separator* m_limitysp3a;
	OrgWinGUI::OWP_Label* m_limityupperLabel;
	OrgWinGUI::OWP_EditBox* m_limityupperEdit;
	OrgWinGUI::OWP_Label* m_limitylowerLabel;
	OrgWinGUI::OWP_EditBox* m_limitylowerEdit;
	OrgWinGUI::OWP_Label* m_limitychkLabel;
	OrgWinGUI::OWP_EditBox* m_limitychkEdit;
	OrgWinGUI::OWP_Separator* m_limitzsp1;
	OrgWinGUI::OWP_Separator* m_limitzsp2;
	OrgWinGUI::OWP_Separator* m_limitzsp2a;
	OrgWinGUI::OWP_Separator* m_limitzsp2b;
	OrgWinGUI::OWP_Separator* m_limitzsp3;
	OrgWinGUI::OWP_Separator* m_limitzsp3a;
	OrgWinGUI::OWP_Label* m_limitzupperLabel;
	OrgWinGUI::OWP_EditBox* m_limitzupperEdit;
	OrgWinGUI::OWP_Label* m_limitzlowerLabel;
	OrgWinGUI::OWP_EditBox* m_limitzlowerEdit;
	OrgWinGUI::OWP_Label* m_limitzchkLabel;
	OrgWinGUI::OWP_EditBox* m_limitzchkEdit;
	OrgWinGUI::OWP_Separator* m_limiteulsp1;
	OrgWinGUI::OWP_Separator* m_limiteulsp2;
	OrgWinGUI::OWP_Separator* m_limiteulsp3;
	OrgWinGUI::OWP_Button* m_limiteuloneB;
	OrgWinGUI::OWP_Button* m_limiteuldeeperB;
	OrgWinGUI::OWP_Button* m_limiteulallB;
	OrgWinGUI::OWP_Label* m_limitcppsLabel;
	OrgWinGUI::OWP_Separator* m_limitcpsp1;
	OrgWinGUI::OWP_Separator* m_limitcpsp2;
	OrgWinGUI::OWP_Separator* m_limitcpsp3;
	OrgWinGUI::OWP_Button* m_limitcptosymB;
	OrgWinGUI::OWP_Button* m_limitcpfromsymB;
	OrgWinGUI::OWP_Button* m_limitcpB;
	OrgWinGUI::OWP_Button* m_limitpsB;
	OrgWinGUI::OWP_Label* m_limitsetLabel;
	OrgWinGUI::OWP_Separator* m_limitsetsp1;
	OrgWinGUI::OWP_Separator* m_limitsetsp2;
	OrgWinGUI::OWP_Separator* m_limitsetsp3;
	OrgWinGUI::OWP_Button* m_limitreset180B;
	OrgWinGUI::OWP_Button* m_limit180to170B;
	OrgWinGUI::OWP_Button* m_limitreset0B;
	OrgWinGUI::OWP_Button* m_limitfrommotB;
	OrgWinGUI::OWP_Label* m_limitsetallLabel;
	OrgWinGUI::OWP_Separator* m_limitsetallsp1;
	OrgWinGUI::OWP_Separator* m_limitsetallsp2;
	OrgWinGUI::OWP_Separator* m_limitsetallsp3;
	OrgWinGUI::OWP_Button* m_limitresetall180B;
	OrgWinGUI::OWP_Button* m_limitall180to170B;
	OrgWinGUI::OWP_Button* m_limitallreset0B;
	OrgWinGUI::OWP_Button* m_limitallfrommotB;
	OrgWinGUI::OWP_Label* m_limitallboneallmotLabel;
	OrgWinGUI::OWP_Button* m_limitallboneallmotB;
	OrgWinGUI::OWP_Label* m_limitphysicsLabel;
	OrgWinGUI::OWP_Separator* m_limitphysicssp;
	OrgWinGUI::OWP_Label* m_limitphysicsrateLabel;
	OrgWinGUI::OWP_Slider* m_limitphysicsrateSlider;
	OrgWinGUI::OWP_Separator* m_limitphsysicssp1;
	OrgWinGUI::OWP_Separator* m_limitphsysicssp2;
	OrgWinGUI::OWP_Button* m_limitphysicsDepperB;
	OrgWinGUI::OWP_Button* m_limitphysicsAllB;
	OrgWinGUI::OWP_Label* m_limitspacerLabel001;
	OrgWinGUI::OWP_Label* m_limitspacerLabel002;
	OrgWinGUI::OWP_Label* m_limitspacerLabel003;
	OrgWinGUI::OWP_Label* m_limitspacerLabel004;


	void (*m_PrepairUndo)();
	int (*m_UpdateAfterEditAngleLimit)(int limit2boneflag, bool setcursorflag);//default : setcursorflag = true

};

#endif //__ColiIDDlg_H_
