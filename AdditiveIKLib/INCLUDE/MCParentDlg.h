// MCParentDlg.h : CMCParentDlg の宣言

#ifndef MCPARENTDLGH
#define MCPARENTDLGH

#include "../AdditiveIK/resource.h"       // メイン シンボル
#include <atlhost.h>

#include <coef.h>


/////////////////////////////////////////////////////////////////////////////
// CMCParentDlg
class CMCParentDlg : 
	public CAxDialogImpl<CMCParentDlg>
{
public:
	CMCParentDlg( char* srcmname, WCHAR* srcfname, int srctotalframe, int srcev0idle, int srccommonid, int srcforbidnum, int* srcforbidid, int srcnotfu );
	~CMCParentDlg();

	enum { IDD = IDD_MCPARENTDLG };

BEGIN_MSG_MAP(CMCParentDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(IDC_ADDFORBID, OnAddForbid)
	COMMAND_ID_HANDLER(IDC_DELFORBID, OnDelForbid)
END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnAddForbid(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelForbid(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	void SetWnd();
	int ParamsToDlg();
	int InitList1();

public:
	const char* GetName() {
		return name;
	};
	const WCHAR* GetFName() {
		return fname;
	};
	int GetTotalFrame() {
		return totalframe;
	};
	void SetTotalFrame(int srcval) {
		totalframe = srcval;
	};
	int GetEv0Idle() {
		return ev0idle;
	};
	void SetEv0Idle(int srcval) {
		ev0idle = srcval;
	};
	int GetCommonId() {
		return commonid;
	};
	void SetCommonId(int srcval) {
		commonid = srcval;
	};
	int GetForbidNum() {
		return forbidnum;
	};
	void SetForbidNum(int srcval) {
		forbidnum = srcval;
	};
	int* GetForbidId() {
		return forbidid;
	};
	int GetNotFU() {
		return notfu;
	};
	void SetNotFU(int srcval) {
		notfu = srcval;
	};

private:
	char name[256];//multibytes
	WCHAR fname[256];//wchar
	int totalframe;
	//int idling;

	int ev0idle;
	int commonid;
	int forbidnum;
	int forbidid[ MAXFORBIDNUM ];
	int notfu;

private:
	CWindow m_dlg_wnd;
	CWindow m_name_wnd;
	CWindow m_frame_wnd;
	CWindow m_fname_wnd;
	CWindow m_commonid_wnd;
	CWindow m_forbid_wnd;
	CWindow m_list1_wnd;
	CWindow m_checknotfu_wnd;

};

#endif //
