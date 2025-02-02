#include "stdafx.h"
// FilterDlg.cpp : CFilterDlg のインプリメンテーション
//#include "stdafx.h"

#include "FilterDlg.h"
#include <Commdlg.h>
#include "FilterType.h"

#include "SetDlgPos.h"

//extern
extern void OnDSUpdate();
extern HWND g_filterdlghwnd;
//extern void OnDSMouseHereApeal();
//extern LONG g_undertrackingRMenu;
//extern LONG g_underApealingMouseHere;


/////////////////////////////////////////////////////////////////////////////
// CFilterDlg

CFilterDlg::CFilterDlg()
	: m_filtertype_wnd(0)
{
	m_filtertype = AVGF_NONE;
	m_filtersize = avgfsize[0];

	m_cmdshow = SW_HIDE;
	m_filtertype = 0;
	m_filtersize = 0;

	m_inittimerflag = false;
	m_timerid = 0;

}

CFilterDlg::~CFilterDlg()
{
}



void CFilterDlg::StartTimer() {
	if (!m_inittimerflag) {
		g_filterdlghwnd = m_hWnd;
		SetTimer(m_timerid, 20);
		m_inittimerflag = true;
	}
};
void CFilterDlg::EndTimer() {
	if (m_inittimerflag) {
		KillTimer(m_timerid);
		m_inittimerflag = false;
		g_filterdlghwnd = 0;
	}
};

LRESULT CFilterDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_inittimerflag = false;
	m_timerid = 346;

	SetDlgPosDesktopCenter(m_hWnd, HWND_TOPMOST);
	RECT dlgrect;
	::GetWindowRect(m_hWnd, &dlgrect);
	SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);


	SetWnd();
	SetCombo();
	ParamsToDlg();

	StartTimer();


	return 1;  // システムにフォーカスを設定させます
}

LRESULT CFilterDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
///
	EndTimer();

	int combono;
	combono = (int)SendMessage( m_filtertype_wnd, CB_GETCURSEL, 0, 0);
	if( combono == CB_ERR ){
		return 0;
	}
	m_filtertype = combono;

	combono = (int)SendMessage( m_filtersize_wnd, CB_GETCURSEL, 0, 0);
	if( combono == CB_ERR ){
		return 0;
	}
	m_filtersize = combono;


	EndDialog(wID);

	return 0;
}

LRESULT CFilterDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndTimer();

	EndDialog(wID);
	return 0;
}

LRESULT CFilterDlg::OnClose(UINT, WPARAM, LPARAM, BOOL&)
{
	EndTimer();

	EndDialog(IDCANCEL);
	return 0;
}


void CFilterDlg::SetWnd()
{
	m_filtertype_wnd = GetDlgItem( IDC_FILTERTYPE );
	m_filtersize_wnd = GetDlgItem( IDC_FILTERSIZE );
}

int CFilterDlg::ParamsToDlg()
{	
	SendMessage( m_filtertype_wnd, CB_SETCURSEL, m_filtertype, 0);
	SendMessage( m_filtersize_wnd, CB_SETCURSEL, m_filtersize, 0);

	return 0;
}

int CFilterDlg::SetCombo()
{
	int fno;
	for( fno = 0; fno < AVGF_MAX; fno++ ){
		SendMessage( m_filtertype_wnd, CB_ADDSTRING, 0, (LPARAM)&stravgf[fno][0]);
	}
	SendMessage( m_filtertype_wnd, CB_SETCURSEL, 0, 0);

	for( int i=0; i<10; i++ ){
		SendMessage( m_filtersize_wnd, CB_ADDSTRING, 0, (LPARAM)&stravgfsize[i][0]);
	}
	SendMessage( m_filtersize_wnd, CB_SETCURSEL, 0, 0);

	return 0;
}


LRESULT CFilterDlg::OnTimer(UINT, WPARAM, LPARAM, BOOL&)
{
	//OnDSUpdate();
	return TRUE;
}
