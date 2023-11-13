// InfluenceDlg.h : CColiIDDlg の宣言

#ifndef __ColiIDDlg_H_
#define __ColiIDDlg_H_

//#include "usealt.h"

#include <coef.h>

#include <atlbase.h>
#include <atlhost.h>

#include "resource.h"       // メイン シンボル

#include <vector>

class CRigidElem;

/////////////////////////////////////////////////////////////////////////////
// CColiIDDlg
class CColiIDDlg : 
	public CAxDialogImpl<CColiIDDlg>
{
public:
	CColiIDDlg( CRigidElem* curre );
	~CColiIDDlg();

	enum { IDD = IDD_COLIIDDLG };

	void StartTimer() {
		if (!m_inittimerflag) {
			SetTimer(m_timerid, 20);
			m_inittimerflag = true;
		}
	};
	void EndTimer() {
		if (m_inittimerflag) {
			KillTimer(m_timerid);
			m_inittimerflag = false;
		}
	};

BEGIN_MSG_MAP(CColiIDDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(IDC_ADD, OnAdd)
	COMMAND_ID_HANDLER(IDC_DEL, OnDel)
	COMMAND_ID_HANDLER(IDC_COMBO1, OnSelGroup)
END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelGroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTimer(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL&);

private:
	void InitParams();
	int DestroyObjs();
	int ParamsToDlg();
	int InitCombo();
	int InitList();


private:
	bool m_inittimerflag;
	int m_timerid;

	CRigidElem* m_curre;
	CWindow m_combo_wnd;
	CWindow m_combo2_wnd;
	CWindow m_list_wnd;
	CWindow m_dlg_wnd;

public:
	int m_groupid;
	std::vector<int> m_coliids;
	int m_myself;
	int m_setgroup;
};

#endif //__ColiIDDlg_H_
