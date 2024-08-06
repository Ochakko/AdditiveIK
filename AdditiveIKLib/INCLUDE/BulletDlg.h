#ifndef __CBulletDlg_H_
#define __CBulletDlg_H_


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
// CBulletDlg
class CBulletDlg
{
public:
	CBulletDlg();
	~CBulletDlg();

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

private:
	int CreateBulletWnd();

private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_Label* m_bulletcalccntLabel;
	OrgWinGUI::OWP_Slider* m_bulletcalccntSlider;
	OrgWinGUI::OWP_Label* m_bulletERPLabel;
	OrgWinGUI::OWP_Slider* m_bulletERPSlider;
	OrgWinGUI::OWP_Label* m_bulletlimitrateLabel;
	OrgWinGUI::OWP_Slider* m_bulletlimitrateSlider;
	OrgWinGUI::OWP_Label* m_bulletmvrateLabel;
	OrgWinGUI::OWP_Slider* m_bulletmvrateSlider;
	OrgWinGUI::OWP_Label* m_bulletspringscaleLabel;
	OrgWinGUI::OWP_Slider* m_bulletspringscaleSlider;
	OrgWinGUI::OWP_Label* m_bulletrigidspeedLabel;
	OrgWinGUI::OWP_Slider* m_bulletrigidspeedSlider;
	OrgWinGUI::OWP_Label* m_bulletcommentLabel;
	OrgWinGUI::OWP_Label* m_bulletspacer1Label;
	OrgWinGUI::OWP_Label* m_bulletspacer2Label;
	OrgWinGUI::OWP_Label* m_bulletspacer3Label;
	OrgWinGUI::OWP_Label* m_bulletspacer4Label;
	OrgWinGUI::OWP_Label* m_bulletspacer5Label;
	OrgWinGUI::OWP_Label* m_bulletspacer6Label;


};

#endif //__ColiIDDlg_H_
