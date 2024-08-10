#ifndef __CThresholdDlg_H_
#define __CThresholdDlg_H_


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


#define THDLGEDITLEN	256


/////////////////////////////////////////////////////////////////////////////
// CThresholdDlg
class CThresholdDlg
{
public:
	CThresholdDlg();
	~CThresholdDlg();

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
	int CreateThresholdWnd();
	int DlgToParams();
	int GetThresholdEditIntOWP(OrgWinGUI::OWP_EditBox* srcedit, int* dstlimit);
	int CheckStr_SInt(const WCHAR* srcstr);

public:
	bool GetThresholdCloseFlag()
	{
		return m_closeFlag;
	};
	void SetThresholdCloseFlag(bool srcval)
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

	OrgWinGUI::OWP_Label* m_ththLabel;
	OrgWinGUI::OWP_Separator* m_ththsp1;
	OrgWinGUI::OWP_Separator* m_ththsp2;
	OrgWinGUI::OWP_Separator* m_ththsp3;
	OrgWinGUI::OWP_Label* m_ththnoendjLabel;
	OrgWinGUI::OWP_EditBox* m_ththnoendjEdit;
	OrgWinGUI::OWP_Label* m_ththendjLabel;
	OrgWinGUI::OWP_EditBox* m_ththendjEdit;
	OrgWinGUI::OWP_Label* m_throundLabel;
	OrgWinGUI::OWP_Separator* m_throundsp1;
	OrgWinGUI::OWP_Separator* m_throundsp2;
	OrgWinGUI::OWP_Separator* m_throundsp2a;
	OrgWinGUI::OWP_Separator* m_throundsp2b;
	OrgWinGUI::OWP_Separator* m_throundsp3;
	OrgWinGUI::OWP_Separator* m_throundsp3a;
	OrgWinGUI::OWP_Label* m_throundxLabel;
	OrgWinGUI::OWP_EditBox* m_throundxEdit;
	OrgWinGUI::OWP_Label* m_throundyLabel;
	OrgWinGUI::OWP_EditBox* m_throundyEdit;
	OrgWinGUI::OWP_Label* m_throundzLabel;
	OrgWinGUI::OWP_EditBox* m_throundzEdit;
	OrgWinGUI::OWP_Label* m_thspacerLabel;
	OrgWinGUI::OWP_Separator* m_thapplysp1;
	OrgWinGUI::OWP_Separator* m_thapplysp2;
	OrgWinGUI::OWP_Button* m_thapplyB;
	OrgWinGUI::OWP_Button* m_thdefaultB;
	OrgWinGUI::OWP_Label* m_thspacerLabel1;
	OrgWinGUI::OWP_Label* m_thspacerLabel2;


};

#endif //__ColiIDDlg_H_
