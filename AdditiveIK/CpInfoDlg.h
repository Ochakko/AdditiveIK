// InfluenceDlg.h : CCpInfoDlg の宣言

#ifndef __CCpInfoDlg_H_
#define __CCpInfoDlg_H_

//#include "usealt.h"

#include <coef.h>
#include "StructHistory.h"

#include <atlbase.h>
#include <atlhost.h>

#include "resource.h"       // メイン シンボル

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CCpInfoDlg
class CCpInfoDlg : 
	public CAxDialogImpl<CCpInfoDlg>
{
public:
	CCpInfoDlg();
	~CCpInfoDlg();

	enum { IDD = IDD_CPINFODLG };

BEGIN_MSG_MAP(CCpInfoDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	int SetCpInfo(CPMOTINFO* srcpcpinfo);

private:
	void InitParams();
	int DestroyObjs();
	int ParamsToDlg();

private:
	CWindow m_dlg_wnd;
	CPMOTINFO* m_pcpinfo;


};

#endif //__ColiIDDlg_H_
