
#ifndef __GColiIDDlg_H_
#define __GColiIDDlg_H_

//#include "usealt.h"

#include <coef.h>

#include <atlbase.h>
#include <atlhost.h>

#include "resource.h"       // メイン シンボル

#include <vector>

class CRigidElem;

/////////////////////////////////////////////////////////////////////////////
// CGColiIDDlg
class CGColiIDDlg : 
	public CAxDialogImpl<CGColiIDDlg>
{
public:
	CGColiIDDlg( std::vector<int>& srcids, int srcmyself );
	~CGColiIDDlg();

	enum { IDD = IDD_GCOLIIDDLG };

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

BEGIN_MSG_MAP(CGColiIDDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(IDC_ADD, OnAdd)
	COMMAND_ID_HANDLER(IDC_DEL, OnDel)
END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTimer(UINT, WPARAM, LPARAM, BOOL&);

private:
	void InitParams();
	int DestroyObjs();
	int ParamsToDlg();
	int InitCombo();
	int InitList();


private:
	bool m_inittimerflag;
	int m_timerid;

	CWindow m_combo2_wnd;
	CWindow m_list_wnd;
	CWindow m_dlg_wnd;

public:
	int m_groupid;
	std::vector<int> m_coliids;
	int m_myself;
};

#endif //__GColiIDDlg_H_
