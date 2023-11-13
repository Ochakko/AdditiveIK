#include "stdafx.h"
// EditAlpDlg.cpp : CRegistDlg �̃C���v�������e�[�V����
//#include "stdafx.h"
#include "RegistDlg.h"


#include "GetDlgParams.h"

/////////////////////////////////////////////////////////////////////////////
// CRegistDlg

CRegistDlg::CRegistDlg()
{
	m_regkey[0] = 0;
	m_wregkey[0] = 0;
}

CRegistDlg::~CRegistDlg()
{
}

LRESULT CRegistDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetWnd();

	ParamsToDlg();

	return 1;  // �V�X�e���Ƀt�H�[�J�X��ݒ肳���܂�
}


int CRegistDlg::DlgToParams()
{
	int ret;
	ret = GetTextW( m_regkey_wnd, m_wregkey, 256 );
	if( ret ){
		::MessageBoxA( NULL, "���͒l���s���ł�", "���̓G���[", MB_OK );
		return 1;
	}
	
	m_regkey[0] = 0;
	WideCharToMultiByte( CP_ACP, 0, m_wregkey, -1, m_regkey, 256, NULL, NULL );


	return 0;
}

LRESULT CRegistDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int ret = 0;

	ret = DlgToParams();
	if( ret )
		return 0;

	EndDialog(wID);
	return 0;
}

LRESULT CRegistDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

int CRegistDlg::SetWnd()
{
	m_regkey_wnd = GetDlgItem( IDC_REGISTKEY );
	return 0;
}

int CRegistDlg::ParamsToDlg()
{
	m_regkey_wnd.SetWindowText( L"" );
	
	return 0;
}
