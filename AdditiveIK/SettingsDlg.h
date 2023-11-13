// InfluenceDlg.h : CSettingsDlg の宣言

#ifndef __CSettingsDlg_H_
#define __CSettingsDlg_H_

//#include "usealt.h"

#include <coef.h>

#include <atlbase.h>
#include <atlhost.h>

#include "resource.h"       // メイン シンボル

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg
class CSettingsDlg : 
	public CAxDialogImpl<CSettingsDlg>
{
public:
	CSettingsDlg();
	~CSettingsDlg();

	enum { IDD = IDD_SETTINGSDLG };

BEGIN_MSG_MAP(CSettingsDlg)
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

private:
	void InitParams();
	int DestroyObjs();
	int ParamsToDlg();

private:
	CWindow m_dlg_wnd;
	int m_savebgcolindex;


};

#endif //__ColiIDDlg_H_
