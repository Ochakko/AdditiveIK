#include "stdafx.h"
#include "CpInfoDlg.h"
#include "GetDlgParams.h"
#include "SetDlgPos.h"


#include <GlobalVar.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>

/////////////////////////////////////////////////////////////////////////////
// CCpInfoDlg

CCpInfoDlg::CCpInfoDlg()
{
	InitParams();
}

CCpInfoDlg::~CCpInfoDlg()
{
	DestroyObjs();
}
	
int CCpInfoDlg::DestroyObjs()
{
	return 0;
}

void CCpInfoDlg::InitParams()
{
	m_pcpinfo = 0;
}

LRESULT CCpInfoDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int ret;

	InitCommonControls();

	SetDlgPosDesktopCenter(m_hWnd, HWND_TOPMOST);
	RECT dlgrect;
	::GetWindowRect(m_hWnd, &dlgrect);
	SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);


	m_dlg_wnd = m_hWnd;

	ret = ParamsToDlg();
	_ASSERT( !ret );


	return 1;  // システムにフォーカスを設定させます
}

LRESULT CCpInfoDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{

	CWindow combobvhtypewnd = m_dlg_wnd.GetDlgItem(IDC_COMBO_BVHSKELETON);
	if (combobvhtypewnd.IsWindow()) {
		int combono;
		combono = (int)combobvhtypewnd.SendMessage(CB_GETCURSEL, 0, 0);
		if (combono != CB_ERR) {
			int itemdata;
			itemdata = (int)combobvhtypewnd.SendMessage(CB_GETITEMDATA, combono, 0);
			if (itemdata != CB_ERR) {
				if (m_pcpinfo) {
					m_pcpinfo->bvhtype = itemdata;
				}
			}
		}
	}

	CWindow comboimportancewnd = m_dlg_wnd.GetDlgItem(IDC_COMBO_IMPORTANCE);
	if (comboimportancewnd.IsWindow()) {
		int combono;
		combono = (int)comboimportancewnd.SendMessage(CB_GETCURSEL, 0, 0);
		if (combono != CB_ERR) {
			int itemdata;
			itemdata = (int)comboimportancewnd.SendMessage(CB_GETITEMDATA, combono, 0);
			if (itemdata != CB_ERR) {
				if (m_pcpinfo) {
					m_pcpinfo->importance = itemdata;
				}
			}
		}
	}

	if (m_pcpinfo) {
		WCHAR tmpcomment[32] = { 0L };
		ZeroMemory(tmpcomment, sizeof(WCHAR) * 32);
		m_dlg_wnd.GetDlgItemTextW(IDC_EDIT_COMMENT, tmpcomment, 32);
		tmpcomment[32 - 1] = 0L;
		if (tmpcomment[0] != 0L) {
			wcscpy_s(m_pcpinfo->comment, 32, tmpcomment);
		}
	}

	EndDialog(wID);
	return 0;
}

LRESULT CCpInfoDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{

	EndDialog(wID);
	return 0;
}
LRESULT CCpInfoDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}



int CCpInfoDlg::ParamsToDlg()
{
/*
	typedef struct tag_cpinfo
	{
		WCHAR fbxname[MAX_PATH];
		WCHAR motionname[MAX_PATH];
		double startframe;
		double framenum;
		int bvhtype;//0:undef, 1-144:bvh1 - bvh144, -1:bvh_other
		int importance;//0:undef, 1:tiny, 2:alittle, 3:normal, 4:noticed, 5:imortant, 6:very important
		WCHAR comment[32];//WCHAR * 31文字まで。３２文字目は終端記号
	}CPMOTINFO;
*/

	//m_dlg_wnd.CheckRadioButton(IDC_RADIO1, IDC_RADIO5, (g_ClearColorIndex + IDC_RADIO1));
	if (!m_pcpinfo) {
		return 1;
	}

	WCHAR strtext[MAX_PATH] = { 0L };

	m_dlg_wnd.SetDlgItemTextW(IDC_FBXNAME, m_pcpinfo->fbxname);
	m_dlg_wnd.SetDlgItemTextW(IDC_MOTIONNAME, m_pcpinfo->motionname);
	
	swprintf_s(strtext, MAX_PATH, L"%lf", m_pcpinfo->startframe);
	m_dlg_wnd.SetDlgItemTextW(IDC_STARTFRAME, strtext);

	swprintf_s(strtext, MAX_PATH, L"%lf", m_pcpinfo->framenum);
	m_dlg_wnd.SetDlgItemTextW(IDC_FRAMENUM, strtext);

	CWindow combobvhtypewnd = m_dlg_wnd.GetDlgItem(IDC_COMBO_BVHSKELETON);
	if (combobvhtypewnd.IsWindow()) {
		combobvhtypewnd.SendMessage(CB_RESETCONTENT, 0, 0);

		combobvhtypewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"UnDef.");
		combobvhtypewnd.SendMessage(CB_SETITEMDATA, 0, (LPARAM)0);

		WCHAR tempchar[MAX_PATH];
		int ino;
		for (ino = 1; ino <= 144; ino++) {
			ZeroMemory(tempchar, sizeof(WCHAR) * MAX_PATH);
			swprintf_s(tempchar, MAX_PATH, L"bvh_%03d", ino);

			combobvhtypewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)tempchar);
			combobvhtypewnd.SendMessage(CB_SETITEMDATA, ino, (LPARAM)ino);
		}
		combobvhtypewnd.SendMessage(CB_SETCURSEL, 0, 0);

	}

	CWindow comboimportancewnd = m_dlg_wnd.GetDlgItem(IDC_COMBO_IMPORTANCE);
	if (comboimportancewnd.IsWindow()) {
		comboimportancewnd.SendMessage(CB_RESETCONTENT, 0, 0);

		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"UnDef.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 0, (LPARAM)0);

		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"tiny.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 1, (LPARAM)1);
		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"ALittle.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 2, (LPARAM)2);
		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"Normal.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 3, (LPARAM)3);
		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"Noticed.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 4, (LPARAM)4);
		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"Important.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 5, (LPARAM)5);
		comboimportancewnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)L"VeryImportant.");
		comboimportancewnd.SendMessage(CB_SETITEMDATA, 6, (LPARAM)6);

		comboimportancewnd.SendMessage(CB_SETCURSEL, 0, 0);
	}


	return 0;
}

int CCpInfoDlg::SetCpInfo(CPMOTINFO* srcpcpinfo)
{
	if (!srcpcpinfo) {
		return 1;
	}
	m_pcpinfo = srcpcpinfo;

	return 0;

}


