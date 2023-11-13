#include "stdafx.h"
#include "ColiIDDlg.h"
#include "GetDlgParams.h"

#include "SetDlgPos.h"

#include <RigidElem.h>

#define DBGH
#include <dbg.h>
#include <crtdbg.h>

using namespace std;

//extern
extern void OnDSUpdate();
//extern void OnDSMouseHereApeal();
//extern LONG g_undertrackingRMenu;
//extern LONG g_underApealingMouseHere;


/////////////////////////////////////////////////////////////////////////////
// CColiIDDlg

CColiIDDlg::CColiIDDlg( CRigidElem* curre )
{
	InitParams();
	m_curre = curre;
}

CColiIDDlg::~CColiIDDlg()
{
	DestroyObjs();
}
	
int CColiIDDlg::DestroyObjs()
{
	m_coliids.clear();
	return 0;
}

void CColiIDDlg::InitParams()
{
	m_inittimerflag = false;
	m_timerid = 343;


	m_groupid = 0;
	m_coliids.clear();
	m_setgroup = 0;
}

LRESULT CColiIDDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int ret;

	InitCommonControls();

	SetDlgPosDesktopCenter(m_hWnd, HWND_TOPMOST);
	RECT dlgrect;
	::GetWindowRect(m_hWnd, &dlgrect);
	SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);


	m_dlg_wnd = m_hWnd;
	m_combo_wnd = GetDlgItem( IDC_COMBO1 );
	m_combo2_wnd = GetDlgItem( IDC_COMBO2 );
	m_list_wnd = GetDlgItem( IDC_LIST1 );

	ret = InitCombo();
	_ASSERT( !ret );

	ret = ParamsToDlg();
	_ASSERT( !ret );

	StartTimer();


	return 1;  // システムにフォーカスを設定させます
}

LRESULT CColiIDDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndTimer();

	if( m_curre ){
		m_curre->SetGroupid( m_groupid );
		m_curre->CopyColiids( m_coliids );


		UINT ischecked;
		ischecked = m_dlg_wnd.IsDlgButtonChecked( IDC_MYSELF );
		if( ischecked == BST_CHECKED ){
			m_curre->SetMyselfflag( 1 );
		}else{
			m_curre->SetMyselfflag( 0 );
		}

		ischecked = m_dlg_wnd.IsDlgButtonChecked( IDC_SETGROUP );
		if( ischecked == BST_CHECKED ){
			m_setgroup = 1;
		}else{
			m_setgroup = 0;
		}
	}

	EndDialog(wID);
	return 0;
}

LRESULT CColiIDDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndTimer();

	EndDialog(wID);
	return 0;
}
LRESULT CColiIDDlg::OnClose(UINT, WPARAM, LPARAM, BOOL&)
{
	EndTimer();

	EndDialog(IDCANCEL);
	return 0;
}


int CColiIDDlg::ParamsToDlg()
{
	m_groupid = m_curre->GetGroupid();
	m_curre->CopyColiids2Dstvec( m_coliids );
	m_myself = m_curre->GetMyselfflag();

	SendMessage( m_combo_wnd, CB_SETCURSEL, m_groupid - 1, 0 );

	if( m_myself ){
		m_dlg_wnd.CheckDlgButton( IDC_MYSELF, BST_CHECKED );
	}else{
		m_dlg_wnd.CheckDlgButton( IDC_MYSELF, BST_UNCHECKED );
	}

	int ret;
	ret = InitList();
	_ASSERT( !ret );

	return 0;
}

int CColiIDDlg::InitCombo()
{

	m_combo_wnd.SendMessage( CB_RESETCONTENT, 0, 0 );
	m_combo2_wnd.SendMessage( CB_RESETCONTENT, 0, 0 );

	int setno = 0;
	WCHAR tempchar[256];
	int ino;
	for( ino = 0; ino < COLIGROUPNUM; ino++ ){
		//int curid = m_coliids[ ino ];
		int curid = ino + 1;
		ZeroMemory( tempchar, sizeof( WCHAR ) * 256 );
		swprintf_s( tempchar, 256, L"%02d", curid );

		m_combo_wnd.SendMessage( CB_ADDSTRING, 0, (LPARAM)tempchar );
		m_combo_wnd.SendMessage( CB_SETITEMDATA, setno, (LPARAM)curid );

		m_combo2_wnd.SendMessage( CB_ADDSTRING, 0, (LPARAM)tempchar );
		m_combo2_wnd.SendMessage( CB_SETITEMDATA, setno, (LPARAM)curid );

		setno++;
	}

	m_combo_wnd.SendMessage( CB_SETCURSEL, 0, 0 );
	m_combo2_wnd.SendMessage( CB_SETCURSEL, 0, 0 );

	return 0;
}

int CColiIDDlg::InitList()
{
	m_list_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );

	WCHAR tempchar[256];
	int ino;
	for( ino = 0; ino < (int)m_coliids.size(); ino++ ){
		int curid = m_coliids[ ino ];
		ZeroMemory( tempchar, sizeof( WCHAR ) * 256 );
		swprintf_s( tempchar, 256, L"%02d", curid );

		m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)tempchar );
		m_list_wnd.SendMessage( LB_SETITEMDATA, ino, curid );
	}

	return 0;
}

LRESULT CColiIDDlg::OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int combono;
	combono = (int)m_combo2_wnd.SendMessage( CB_GETCURSEL, 0, 0 );
	if( combono == CB_ERR )
		return 0;
	
	int itemdata;
	itemdata = (int)m_combo2_wnd.SendMessage( CB_GETITEMDATA, combono, 0 );
	if( itemdata == CB_ERR ){
		DbgOut( L"ColiIDDlg : InitList : itemdata error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_coliids.push_back( itemdata );


	InitList();

	return 0;
}
LRESULT CColiIDDlg::OnDel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int idnum = (int)m_coliids.size();

	if( idnum <= 0 ){
		return 0;
	}

	int listno;
	listno = (int)m_list_wnd.SendMessage( LB_GETCURSEL, 0, 0 );
	if( listno == LB_ERR )
		return 0;

	if( (listno < 0) || (listno >= idnum) ){
		DbgOut( L"ColiIDDlg : OnDel listno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	vector<int> newids;
	newids.clear();

	int ino;
	int setno = 0;
	for( ino = 0; ino < idnum; ino++ ){
		if( ino != listno ){
			newids.push_back( m_coliids[ ino ] );
			setno++;
		}
	}
	_ASSERT( setno == (idnum - 1) );

	m_coliids = newids;

	InitList();

	return 0;
}

LRESULT CColiIDDlg::OnSelGroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int selindex = (int)SendMessage( m_combo_wnd, CB_GETCURSEL, 0, 0 );
	if( selindex == CB_ERR )
		return 0;

	if( (selindex < 0) || (selindex >= COLIGROUPNUM) ){
		_ASSERT( 0 );
		return 1;
	}

	m_groupid = selindex + 1;

	return 0;
}

LRESULT CColiIDDlg::OnTimer(UINT, WPARAM, LPARAM, BOOL&)
{
	OnDSUpdate();
	return TRUE;
}
