#ifndef __CDampAnimDlg_H_
#define __CDampAnimDlg_H_


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
// CDampAnimDlg
class CDampAnimDlg
{
public:
	CDampAnimDlg();
	~CDampAnimDlg();

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
	int CreateDampAnimWnd();

public:
	bool GetDampAnimCloseFlag()
	{
		return m_closeFlag;
	};
	void SetDampAnimCloseFlag(bool srcval)
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

	OrgWinGUI::OWP_CheckBoxA* m_dmpgroupcheck;
	OrgWinGUI::OWP_Label* m_dmpanimLlabel;
	OrgWinGUI::OWP_Slider* m_dmpanimLSlider;
	OrgWinGUI::OWP_Label* m_dmpanimAlabel;
	OrgWinGUI::OWP_Slider* m_dmpanimASlider;
	OrgWinGUI::OWP_Button* m_dmpanimB;


};

#endif //__ColiIDDlg_H_
