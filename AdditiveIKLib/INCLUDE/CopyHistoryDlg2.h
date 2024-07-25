// InfluenceDlg.h : CCopyHistoryDlg の宣言

#ifndef __CCopyHistoryDlg2_H_
#define __CCopyHistoryDlg2_H_

#include <windows.h>

#include <coef.h>
#include "../../AdditiveIK/StructHistory.h"
#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>

//#define COPYNUMFORDISP	10
#define IMPORTANCEKINDNUM	7
#define INPORTANCESTRLEN	32

class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;

class CCopyHistoryDlg2;

class CCpHistoryOWPElem
{
public:
	CCpHistoryOWPElem();
	~CCpHistoryOWPElem();

	int SetHistoryElem(
		int srccopyhistoryindex, OrgWinGUI::OrgWindow* parwnd, OrgWinGUI::OWP_Separator* parentsp, 
		HISTORYELEM srchistory);
	int AddParts(OrgWinGUI::OWP_Separator* parentsp);
	int SetEventFunc(CCopyHistoryDlg2* srcdlg);
	//int SetEventFunc(int (*srcSelChangeFunc)(int srcindex1), int (*srcDeleteFunc)(int srcindex2)) {
	//	m_SelChangeFunc = srcSelChangeFunc;
	//	m_DeleteFunc = srcDeleteFunc;
	//};

private:
	void InitParams();
	void DestroyObjs();

public:
	int GetCopyHistoryIndex() {
		return m_copyhistoryindex;
	};
	OrgWinGUI::OWP_CheckBoxA* GetNameCheckBox() {
		return m_nameChk;
	};
	OrgWinGUI::OWP_Button* GetDelButton() {
		return m_deleteB;
	};
	OrgWinGUI::OWP_Label* GetDescLabel() {
		return m_descLabel;
	};
	OrgWinGUI::OWP_Label* GetMemoLabel() {
		return m_memoLabel;
	};
private:
	CCopyHistoryDlg2* m_parentdlg;
	int m_copyhistoryindex;
	OrgWinGUI::OWP_Separator* m_namesp;
	OrgWinGUI::OWP_CheckBoxA* m_nameChk;
	OrgWinGUI::OWP_Button* m_deleteB;
	OrgWinGUI::OWP_Label* m_descLabel;
	OrgWinGUI::OWP_Label* m_memoLabel;
	OrgWinGUI::OWP_Label* m_spacerLabel;

	WCHAR m_strimportance[IMPORTANCEKINDNUM][INPORTANCESTRLEN];

	//int (*m_SelChangeFunc)(int srcindex);
	//int (*m_DeleteFunc)(int srcindex);

};




/////////////////////////////////////////////////////////////////////////////
// CCopyHistoryDlg2
class CCopyHistoryDlg2
{
public:
	CCopyHistoryDlg2();
	~CCopyHistoryDlg2();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);
	int SetNames(CModel* srcmodel, std::vector<HISTORYELEM>& copyhistory);
	int GetSelectedFileName(CModel* srcmodel, WCHAR* dstfilename);


	bool GetCreatedFlag()
	{
		return m_createdflag;
	};
	int ParamsToDlg(CModel* srcmodel, HISTORYELEM saveselectedelem);

	void SetVisible(bool srcflag);
	int OnSearch();

	int OnDelete(int delid);
	int OnRadio(int radioid);

	void InitParams();
	int DestroyObjs();

private:
	int DestroyOWPWnd();
	int DestroyOWPElem();

	int CreateOWPWnd(CModel* srcmodel);
	void EnableList(bool srcflag);

	//void SetEnableCtrls();

	bool IsCheckedMostRecent()
	{
		return m_ischeckedmostrecent;
	};

	HISTORYELEM GetFirstValidElem(int* pindex);
	HISTORYELEM GetCheckedElem();

private:

/*
#define COPYNUMFORDISP	10
#define IMPORTANCEKINDNUM	7
#define INPORTANCESTRLEN	32
*/

	bool m_createdflag;
	bool m_visible;

	CModel* m_model;
	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;


	//size_t m_namenum;//m_copyhistory.size()
	//int m_selectedindex;//m_selectindexmapへ変更
	bool m_ischeckedmostrecent;

	std::map<CModel*, std::wstring> m_selectnamemap;
	std::map<CModel*, int> m_selectindexmap;


	//int m_startno;
	std::vector<HISTORYELEM> m_copyhistory;
	std::vector<HISTORYELEM> m_savecopyhistory;

	OrgWinGUI::OrgWindow* m_dlgwnd;
	OrgWinGUI::OWP_ScrollWnd* m_dlgSc;
	OrgWinGUI::OWP_Separator* m_dlgspall;
	OrgWinGUI::OWP_Separator* m_dlgspcombo;
	OrgWinGUI::OWP_Separator* m_dlgsplist;

	OrgWinGUI::OWP_Separator* m_combosp1;
	OrgWinGUI::OWP_Separator* m_combosp2;
	OrgWinGUI::OWP_Separator* m_combosp3;
	OrgWinGUI::OWP_ComboBoxA* m_Combo1;
	OrgWinGUI::OWP_ComboBoxA* m_Combo2;
	OrgWinGUI::OWP_ComboBoxA* m_Combo3;

	OrgWinGUI::OWP_Separator* m_fromsp1;
	OrgWinGUI::OWP_Separator* m_fromsp2;
	OrgWinGUI::OWP_Separator* m_fromsp3;
	OrgWinGUI::OWP_Label* m_fromLabel;
	OrgWinGUI::OWP_ComboBoxA* m_fromComboYear;
	OrgWinGUI::OWP_ComboBoxA* m_fromComboMonth;
	OrgWinGUI::OWP_ComboBoxA* m_fromComboDay;

	OrgWinGUI::OWP_Separator* m_tosp1;
	OrgWinGUI::OWP_Separator* m_tosp2;
	OrgWinGUI::OWP_Separator* m_tosp3;
	OrgWinGUI::OWP_Label* m_toLabel;
	OrgWinGUI::OWP_ComboBoxA* m_toComboYear;
	OrgWinGUI::OWP_ComboBoxA* m_toComboMonth;
	OrgWinGUI::OWP_ComboBoxA* m_toComboDay;

	OrgWinGUI::OWP_Button* m_searchB;
	OrgWinGUI::OWP_Label* m_spacerLabel1;
	OrgWinGUI::OWP_CheckBoxA* m_recentChk;
	OrgWinGUI::OWP_Label* m_spacerLabel2;

	std::vector<CCpHistoryOWPElem*> m_owpelemvec;


	std::vector<std::wstring> m_strcombo_fbxname;
	std::vector<std::wstring> m_strcombo_motionname;
	std::vector<int> m_strcombo_bvhtype;


	bool m_initsearchcomboflag;

};

#endif //__ColiIDDlg_H_
