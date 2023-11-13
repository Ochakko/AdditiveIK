#include "stdafx.h"

#include "SelectLSDlg.h"
#include <Commdlg.h>


/////////////////////////////////////////////////////////////////////////////
// CSelectLSDlg

CSelectLSDlg::CSelectLSDlg()
{
	m_largeflag = true;
}

CSelectLSDlg::~CSelectLSDlg()
{
}


LRESULT CSelectLSDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	SetWnd();
	ParamsToDlg();

#define MAINMENUAIMBARH		32

	int totalwndwidth = (1216 + 450) * 2;
	int totalwndheight = (950 - MAINMENUAIMBARH) * 2;

	int dlgposx = 0;
	int dlgposy = 0;

	HWND desktopwnd;
	desktopwnd = ::GetDesktopWindow();
	if (desktopwnd) {
		RECT desktoprect;
		::GetClientRect(desktopwnd, &desktoprect);
		//if ((desktoprect.right >= (totalwndwidth * 2)) && (desktoprect.bottom >= ((totalwndheight - MAINMENUAIMBARH) * 2))) {
		//}

		dlgposx = (desktoprect.left + desktoprect.right) / 2;
		dlgposy = (desktoprect.top + desktoprect.bottom) / 2;
	}
	// 137, 103
	::MoveWindow(m_dlg_wnd, dlgposx, dlgposy, 137 * 2, 103 * 2, TRUE);

	return 1;  // システムにフォーカスを設定させます
}

LRESULT CSelectLSDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT check1, check2;
	check1 = IsDlgButtonChecked(IDC_RADIO1);
	check2 = IsDlgButtonChecked(IDC_RADIO2);
	if (check1 == BST_CHECKED) {
		m_largeflag = TRUE;
	}
	else {
		m_largeflag = FALSE;
	}
	
	EndDialog(wID);

	return 0;
}

LRESULT CSelectLSDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

void CSelectLSDlg::SetWnd()
{
	m_dlg_wnd = this->m_hWnd;
}

int CSelectLSDlg::ParamsToDlg()
{	
	if (m_largeflag) {
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	}
	else {
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
	}

	return 0;
}
