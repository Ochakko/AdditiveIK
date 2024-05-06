#include "stdafx.h"
#include "GColiIDDlg.h"
#include "GetDlgParams.h"

#include <RigidElem.h>

#include "SetDlgPos.h"

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
// CGColiIDDlg

//CGColiIDDlg::CGColiIDDlg( std::vector<int>& srcids, int srcmyself )
CGColiIDDlg::CGColiIDDlg(std::vector<int>& srcids)
{
	InitParams();
	m_coliids = srcids;
	//m_myself = srcmyself;
}

CGColiIDDlg::~CGColiIDDlg()
{
	DestroyObjs();
}
	
int CGColiIDDlg::DestroyObjs()
{
	m_coliids.clear();

	return 0;
}

void CGColiIDDlg::InitParams()
{
	m_inittimerflag = false;
	m_timerid = 344;

	m_groupid = 1;
	m_coliids.clear();
}

LRESULT CGColiIDDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int ret;

	InitCommonControls();

	SetDlgPosDesktopCenter(m_hWnd, HWND_TOPMOST);
	RECT dlgrect;
	::GetWindowRect(m_hWnd, &dlgrect);
	SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);


	m_dlg_wnd = m_hWnd;
	m_combo2_wnd = GetDlgItem( IDC_COMBO2 );
	m_list_wnd = GetDlgItem( IDC_LIST1 );

	ret = InitCombo();
	_ASSERT( !ret );

	ret = ParamsToDlg();
	_ASSERT( !ret );

	StartTimer();


	return 1;  // システムにフォーカスを設定させます
}

LRESULT CGColiIDDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndTimer();

	EndDialog(wID);
	return 0;
}

LRESULT CGColiIDDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndTimer();

	EndDialog(wID);
	return 0;
}
LRESULT CGColiIDDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndTimer();

	EndDialog(IDCANCEL);
	return 0;
}



int CGColiIDDlg::ParamsToDlg()
{
	//if( m_myself ){
	//	m_dlg_wnd.CheckDlgButton( IDC_MYSELF, BST_CHECKED );
	//}else{
	//	m_dlg_wnd.CheckDlgButton( IDC_MYSELF, BST_UNCHECKED );
	//}

	int ret;
	ret = InitList();
	_ASSERT( !ret );

	return 0;
}

int CGColiIDDlg::InitCombo()
{

	m_combo2_wnd.SendMessage( CB_RESETCONTENT, 0, 0 );

	int setno = 0;
	WCHAR tempchar[256];
	int ino;
	for( ino = 0; ino < COLIGROUPNUM; ino++ ){
		//int curid = m_coliids[ ino ];
		int curid = ino + 1;
		ZeroMemory( tempchar, sizeof( WCHAR ) * 256 );
		swprintf_s( tempchar, 256, L"bit_%02d", curid );

		m_combo2_wnd.SendMessage( CB_ADDSTRING, 0, (LPARAM)tempchar );
		m_combo2_wnd.SendMessage( CB_SETITEMDATA, setno, (LPARAM)curid );

		setno++;
	}

	m_combo2_wnd.SendMessage( CB_SETCURSEL, 0, 0 );

	return 0;
}

int CGColiIDDlg::InitList()
{
	m_list_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );

	WCHAR tempchar[256];
	int ino;
	for( ino = 0; ino < (int)m_coliids.size(); ino++ ){
		int curid = m_coliids[ ino ];
		ZeroMemory( tempchar, sizeof( WCHAR ) * 256 );
		swprintf_s( tempchar, 256, L"bit_%02d", curid );

		m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)tempchar );
		m_list_wnd.SendMessage( LB_SETITEMDATA, ino, curid );
	}

	return 0;
}

LRESULT CGColiIDDlg::OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int combono;
	combono = (int)m_combo2_wnd.SendMessage( CB_GETCURSEL, 0, 0 );
	if( combono == CB_ERR )
		return 0;
	
	int itemdata;
	itemdata = (int)m_combo2_wnd.SendMessage( CB_GETITEMDATA, combono, 0 );
	if( itemdata == CB_ERR ){
		DbgOut( L"GColiIDDlg : InitList : itemdata error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_coliids.push_back( itemdata );


	InitList();

	return 0;
}
LRESULT CGColiIDDlg::OnDel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
		DbgOut( L"GColiIDDlg : OnDel listno error !!!\n" );
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

LRESULT CGColiIDDlg::OnTimer(UINT, WPARAM, LPARAM, BOOL&)
{
	OnDSUpdate();
	return TRUE;
}
