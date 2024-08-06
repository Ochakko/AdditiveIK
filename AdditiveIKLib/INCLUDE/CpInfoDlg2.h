// InfluenceDlg.h : CCpInfoDlg2 の宣言

#ifndef __CCpInfoDlg22_H_
#define __CCpInfoDlg22_H_

#include <windows.h>

#include <coef.h>
#include "../../AdditiveIK/StructHistory.h"
#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>

class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;


/////////////////////////////////////////////////////////////////////////////
// CCpInfoDlg2
class CCpInfoDlg2
{
public:
	CCpInfoDlg2();
	~CCpInfoDlg2();


	int OnClose();
	int OnOK();
	int OnCancel();

	int ThreadingShow();
	int SetCpInfo(CPMOTINFO* srcpcpinfo);
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();


	int GetDlgResult()
	{
		return m_dlgresult;
	}
private:
	int DestroyOWPWnd();
	int CreateOWPWnd(POINT setpoipnt);

	int ParamsToDlg();

public:
	LONG m_dlgresult;

private:
	CPMOTINFO* m_pcpinfo;

	OrgWinGUI::OrgWindow* m_dlgwnd;
	OrgWinGUI::OWP_Separator* m_dlgspall;
	OrgWinGUI::OWP_Label* m_fbxnameLabel;
	OrgWinGUI::OWP_Label* m_motionnameLabel;
	OrgWinGUI::OWP_Label* m_startframeLabel;
	OrgWinGUI::OWP_Label* m_framenumLabel;
	OrgWinGUI::OWP_Separator* m_combosp;
	OrgWinGUI::OWP_ComboBoxA* m_bvhCombo;
	OrgWinGUI::OWP_ComboBoxA* m_importanceCombo;
	OrgWinGUI::OWP_EditBox* m_memoEdit;
	OrgWinGUI::OWP_Label* m_spacerLabel;
	OrgWinGUI::OWP_Separator* m_btnsp;
	OrgWinGUI::OWP_Button* m_okB;
	OrgWinGUI::OWP_Button* m_cancelB;

};

#endif //__ColiIDDlg_H_
