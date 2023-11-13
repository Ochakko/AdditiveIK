// LocateMBDlg.h : CSelectLSDlg ÇÃêÈåæ

#ifndef __LocateMBDlg_H_
#define __LocateMBDlg_H_

#include "resource.h"       // ÉÅÉCÉì ÉVÉìÉ{Éã
#include <atlbase.h>
#include <atlhost.h>

/////////////////////////////////////////////////////////////////////////////
// CSelectLSDlg
class CSelectLSDlg : 
	public CAxDialogImpl<CSelectLSDlg>
{
public:
	CSelectLSDlg();
	~CSelectLSDlg();

	enum { IDD = IDD_SELECTLSDLG };


BEGIN_MSG_MAP(CSelectLSDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	BOOL GetIsLarge() {
		return m_largeflag;
	};

private:
	void SetWnd();
	int ParamsToDlg();

private:

	CWindow m_dlg_wnd;
	BOOL m_largeflag;

	//CWindow m_w2_wnd;
	//CWindow m_h2_wnd;
};

#endif //__LocateMBDlg_H_
