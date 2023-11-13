// InfluenceDlg.h : CCopyHistoryDlg の宣言

#ifndef __CCopyHistoryDlg_H_
#define __CCopyHistoryDlg_H_

//#include "usealt.h"

#include <coef.h>
#include "StructHistory.h"

#include <atlbase.h>
#include <atlhost.h>

#include "resource.h"       // メイン シンボル

#include <vector>
#include <string>


#define COPYNUMFORDISP	10
#define IMPORTANCEKINDNUM	7
#define INPORTANCESTRLEN	32



/////////////////////////////////////////////////////////////////////////////
// CCopyHistoryDlg
class CCopyHistoryDlg : 
	public CAxDialogImpl<CCopyHistoryDlg>
{
public:
	CCopyHistoryDlg();
	~CCopyHistoryDlg();

	enum { IDD = IDD_COPYHISTORYDLG };

	int SetNames(CModel* srcmodel, std::vector<HISTORYELEM>& copyhistory);
	int GetSelectedFileName(CModel* srcmodel, WCHAR* dstfilename);

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};
	int ParamsToDlg(CModel* srcmodel);

private:

BEGIN_MSG_MAP(CCopyHistoryDlg)
	//MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(IDC_CHECK1, OnCheckMostRecent)
	COMMAND_ID_HANDLER(IDC_BUTTON3, OnSearch)
	COMMAND_ID_HANDLER(IDC_BUTTON6, OnDelete1)
	COMMAND_ID_HANDLER(IDC_BUTTON16, OnDelete2)
	COMMAND_ID_HANDLER(IDC_BUTTON17, OnDelete3)
	COMMAND_ID_HANDLER(IDC_BUTTON18, OnDelete4)
	COMMAND_ID_HANDLER(IDC_BUTTON19, OnDelete5)
	COMMAND_ID_HANDLER(IDC_BUTTON20, OnDelete6)
	COMMAND_ID_HANDLER(IDC_BUTTON21, OnDelete7)
	COMMAND_ID_HANDLER(IDC_BUTTON22, OnDelete8)
	COMMAND_ID_HANDLER(IDC_BUTTON23, OnDelete9)
	COMMAND_ID_HANDLER(IDC_BUTTON24, OnDelete10)
	COMMAND_ID_HANDLER(IDC_RADIO1, OnRadio1)
	COMMAND_ID_HANDLER(IDC_RADIO2, OnRadio2)
	COMMAND_ID_HANDLER(IDC_RADIO3, OnRadio3)
	COMMAND_ID_HANDLER(IDC_RADIO4, OnRadio4)
	COMMAND_ID_HANDLER(IDC_RADIO5, OnRadio5)
	COMMAND_ID_HANDLER(IDC_RADIO6, OnRadio6)
	COMMAND_ID_HANDLER(IDC_RADIO7, OnRadio7)
	COMMAND_ID_HANDLER(IDC_RADIO8, OnRadio8)
	COMMAND_ID_HANDLER(IDC_RADIO9, OnRadio9)
	COMMAND_ID_HANDLER(IDC_RADIO10, OnRadio10)
//	COMMAND_ID_HANDLER(IDC_CHECK1, OnChkRecent)
	COMMAND_ID_HANDLER(IDC_PREVPAGE, OnPrevPage)
	COMMAND_ID_HANDLER(IDC_NEXTPAGE, OnNextPage)
END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	//LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnCheckMostRecent(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnDelete1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete9(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnRadio1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio9(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnChkRecent(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnPrevPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNextPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void InitParams();
	int DestroyObjs();

	LRESULT OnDelete(size_t delid);
	LRESULT OnRadio(size_t radioid);
	void SetEnableCtrls();

	bool IsCheckedMostRecent()
	{
		return m_ischeckedmostrecent;
	};

	HISTORYELEM GetFirstValidElem();
	HISTORYELEM GetCheckedElem();

private:

/*
#define COPYNUMFORDISP	10
#define IMPORTANCEKINDNUM	7
#define INPORTANCESTRLEN	32
*/

	bool m_createdflag;
	CWindow m_dlg_wnd;

	CModel* m_model;

	size_t m_namenum;
	bool m_ischeckedmostrecent;
	//WCHAR m_selectname[MAX_PATH];
	std::map<CModel*, std::wstring> m_selectnamemap;


	int m_pagenum;
	int m_currentpage;
	int m_startno;
	std::vector<HISTORYELEM> m_copyhistory;
	std::vector<HISTORYELEM> m_savecopyhistory;

	UINT m_ctrlid[COPYNUMFORDISP];
	UINT m_startframeid[COPYNUMFORDISP];
	UINT m_framenumid[COPYNUMFORDISP];
	UINT m_bvhtypeid[COPYNUMFORDISP];
	UINT m_importanceid[COPYNUMFORDISP];
	UINT m_commentid[COPYNUMFORDISP];

	UINT m_text1[COPYNUMFORDISP];
	UINT m_text2[COPYNUMFORDISP];

	WCHAR m_strimportance[IMPORTANCEKINDNUM][INPORTANCESTRLEN];

	std::vector<std::wstring> m_strcombo_fbxname;
	std::vector<std::wstring> m_strcombo_motionname;
	std::vector<int> m_strcombo_bvhtype;


	bool m_initsearchcomboflag;

};

#endif //__ColiIDDlg_H_
