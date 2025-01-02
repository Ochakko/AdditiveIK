// CMCChildDlg のインプリメンテーション
#include "stdafx.h"
#include "MCChildDlg.h"

#include <Model.h>

#include <GetDlgParams.h>

/////////////////////////////////////////////////////////////////////////////
// CMCChildDlg



CMCChildDlg::CMCChildDlg(CModel* srcmodel, MCELEM* srcmce)
{

	m_model = srcmodel;
	m_mce = srcmce;

	if( srcmce ){
		m_cookie = srcmce->id;
		m_frameno1 = srcmce->frameno1;
		m_frameno2 = srcmce->frameno2;
		m_eventno1 = srcmce->eventno1;
		m_notfu = srcmce->notfu;
		m_nottoidle = srcmce->nottoidle;
	}else{
		m_cookie = 0;
		m_frameno1 = -1;
		m_frameno2 = 0;
		m_eventno1 = 0;
		m_notfu = 0;
		m_nottoidle = 0;
	}

	if( m_frameno1 >= 0 ){
		m_always = 0;
	}else{
		m_always = 1;
	}
}

CMCChildDlg::~CMCChildDlg()
{
}


LRESULT CMCChildDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int ret;
	ret = SetWnd();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if (!m_model) {
		_ASSERT(0);
		return 1;
	}

	int motnum = m_model->GetMotInfoSize();
	int miindex;
	for (miindex = 0; miindex < motnum; miindex++) {
		MOTINFO currentmi = m_model->GetMotInfoByIndex(miindex);
		m_name_wnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)currentmi.motname);
	}
	m_name_wnd.SendMessage(CB_SETCURSEL, m_cookie, 0);


	ret = Params2Dlg();
	_ASSERT( !ret );

	return 1;  // システムにフォーカスを設定させます
}

int CMCChildDlg::Params2Dlg()
{
	WCHAR tempchar[256];
	swprintf_s( tempchar, 256, L"%d", m_frameno1 );
	m_fixframe_wnd.SetWindowText( tempchar );

	swprintf_s( tempchar, 256, L"%d", m_frameno2 );
	m_jumpto_wnd.SetWindowText( tempchar );

	if( m_always == 0 ){
		m_dlg_wnd.CheckRadioButton( IDC_RADIO_ALW, IDC_RADIO_FIX, IDC_RADIO_FIX );
	}else{
		m_dlg_wnd.CheckRadioButton( IDC_RADIO_ALW, IDC_RADIO_FIX, IDC_RADIO_ALW );
	}

	swprintf_s( tempchar, 256, L"%d", m_eventno1 );
	m_eventno_wnd.SetWindowText( tempchar );


	if( m_notfu != 0 ){
		m_dlg_wnd.CheckDlgButton( IDC_CHECK_NOFU, BST_CHECKED );
	}else{
		m_dlg_wnd.CheckDlgButton( IDC_CHECK_NOFU, BST_UNCHECKED );
	}

	if( m_nottoidle != 0 ){
		::CheckDlgButton( m_dlg_wnd, IDC_NOTTOIDLE, BST_CHECKED );
	}else{
		::CheckDlgButton( m_dlg_wnd, IDC_NOTTOIDLE, BST_UNCHECKED );
	}

	return 0;
}

int CMCChildDlg::SetWnd()
{
	m_dlg_wnd = m_hWnd;

	m_eventno_wnd = GetDlgItem( IDC_EVENTNO );

	m_fixframe_wnd = GetDlgItem( IDC_FIXFRAME );
	m_jumpto_wnd = GetDlgItem( IDC_JUMPTO );
	m_name_wnd = GetDlgItem( IDC_COMBO_NAME );

	return 0;
}

LRESULT CMCChildDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int ret = 0;

	UINT ischecked;
	ischecked = m_dlg_wnd.IsDlgButtonChecked( IDC_RADIO_ALW );
	if( ischecked == BST_CHECKED ){
		m_always = 1;
	}else{
		m_always = 0;
	}
	
	int tmpint;
	if( m_always == 0 ){
		ret = GetInt( m_fixframe_wnd, &tmpint );
		if( ret ){
			::MessageBox( m_hWnd, L"分岐元フレーム番号が不正です。", L"入力エラー", MB_OK );
			return 0;//!!!!!!!!!!!!
		}
		m_frameno1 = tmpint;
	}

	ret = GetInt( m_jumpto_wnd, &tmpint );
	if( ret || (tmpint < 0) ){
		::MessageBox( m_hWnd, L"分岐先フレーム番号が不正です。", L"入力エラー", MB_OK );
		return 0;//!!!!!!!!!!!
	}
	m_frameno2 = tmpint;


	ret = GetInt( m_eventno_wnd, &tmpint );
	if( ret || (tmpint < 0) ){
		::MessageBox( m_hWnd, L"イベント番号が不正です。", L"入力エラー", MB_OK );
		return 0;//!!!!!!!!!!!!
	}
	m_eventno1 = tmpint;


	int combono;
	combono = (int)m_name_wnd.SendMessage( CB_GETCURSEL, 0, 0 );
	if( combono == CB_ERR ){
		_ASSERT( 0 );
		return 1;
	}
	m_cookie = combono;


	ischecked = m_dlg_wnd.IsDlgButtonChecked( IDC_CHECK_NOFU );
	if( ischecked == BST_CHECKED ){
		m_notfu = 1;
	}else{
		m_notfu = 0;
	}


	ischecked = m_dlg_wnd.IsDlgButtonChecked( IDC_NOTTOIDLE );
	if( ischecked == BST_CHECKED ){
		m_nottoidle = 1;
	}else{
		m_nottoidle = 0;
	}


	EndDialog(wID);

	return 0;
}

LRESULT CMCChildDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);

	return 0;
}


