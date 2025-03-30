#ifndef __CDofDlg_H_
#define __CDofDlg_H_


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
// CDofDlg
class CDofDlg
{
public:
	CDofDlg();
	~CDofDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!
	//int SetFunctions(
	//	void (*srcSetCamera3DFromEyePos)()
	//);


	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	//void SetModel(CModel* srcmodel, int srccurboneno, std::map<CModel*, int> srcrgdindexmap);
	int ParamsToDlg();
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateDofWnd();
	//int DlgToParams();

public:
	bool GetDofCloseFlag()
	{
		return m_closeFlag;
	};
	void SetDofCloseFlag(bool srcval)
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

	OrgWinGUI::OWP_Label* m_dofLabel;
	OrgWinGUI::OWP_ComboBoxA* m_dofslotCombo;
	OrgWinGUI::OWP_Label* m_dofspacerLabel1;
	OrgWinGUI::OWP_Separator* m_dofdistsp1;
	OrgWinGUI::OWP_Separator* m_dofdistsp2;
	OrgWinGUI::OWP_Separator* m_dofdistsp3;
	OrgWinGUI::OWP_Label* m_dofdistnearLabel;
	OrgWinGUI::OWP_EditBox* m_dofdistnearEdit;
	OrgWinGUI::OWP_Label* m_dofdistfarLabel;
	OrgWinGUI::OWP_EditBox* m_dofdistfarEdit;
	OrgWinGUI::OWP_Label* m_dofspacerLabel2;
	OrgWinGUI::OWP_CheckBoxA* m_dofskyChk;
	//OrgWinGUI::OWP_Label* m_dofspacerLabel3;
	//OrgWinGUI::OWP_Separator* m_dofapplysp;
	//OrgWinGUI::OWP_Button* m_dofapplyB;

};

#endif //__ColiIDDlg_H_
