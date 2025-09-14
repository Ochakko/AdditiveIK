#ifndef __CLaterTransparentDlg_H_
#define __CLaterTransparentDlg_H_


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
// CLaterTransparentDlg
class CLaterTransparentDlg
{
public:
	CLaterTransparentDlg();
	~CLaterTransparentDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateLaterTransparentWnd();
	int ParamsToDlg();
	int Dlg2LaterTransparent();

public:



private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Label* m_laterlistLabel;
	OrgWinGUI::OWP_ScrollWnd* m_laterlist1Sc;
	OrgWinGUI::OWP_ListBox* m_laterlist1List;
	OrgWinGUI::OWP_Label* m_laterspacerLabel1;
	OrgWinGUI::OWP_Button* m_lateraddB;
	OrgWinGUI::OWP_Label* m_laterspacerLabel2;
	OrgWinGUI::OWP_ScrollWnd* m_laterlist2Sc;
	OrgWinGUI::OWP_ListBox* m_laterlist2List;
	OrgWinGUI::OWP_Label* m_laterspacerLabel3;
	OrgWinGUI::OWP_Separator* m_latersp1;
	OrgWinGUI::OWP_Separator* m_latersp2;
	OrgWinGUI::OWP_Separator* m_latersp3;
	OrgWinGUI::OWP_Button* m_laterdelallB;
	OrgWinGUI::OWP_Button* m_laterdelB;
	OrgWinGUI::OWP_Button* m_laterupB;
	OrgWinGUI::OWP_Button* m_laterdownB;

};

#endif //__ColiIDDlg_H_
