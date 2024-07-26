#include "stdafx.h"
//#include "stdafx.h"

#include <Windows.h>

#include "RMenuMain.h"

#define DBGH
#include <dbg.h>
#include <crtdbg.h>

#include "dlgid.h"


CRMenuMain::CRMenuMain( int srcmenuid )
{
	InitParams();
	m_menuid = srcmenuid;
}
CRMenuMain::~CRMenuMain()
{

}

int CRMenuMain::InitParams()
{
	m_rmenu = 0;
	m_rsubmenu = 0;
	m_menuid = 0;
	m_menuwnd = 0;
	return 0;
}

int CRMenuMain::Create(HWND srchwnd, int srcoffset)
{
	m_menuwnd = srchwnd;
	m_menuoffset = srcoffset;

	//m_rmenu = LoadMenu( (HINSTANCE)GetWindowLong( m_menuwnd, GWL_HINSTANCE ), MAKEINTRESOURCE( m_menuid ) );
	m_rmenu = LoadMenu((HINSTANCE)GetClassLongPtr(m_menuwnd, GCLP_HMODULE), MAKEINTRESOURCE(m_menuid));
	m_rsubmenu = ::GetSubMenu( m_rmenu, 0 );


	return 0;
}

int CRMenuMain::CreatePopupMenu(HWND srchwnd, HMENU parmenu, WCHAR* partitle, bool grayed)//default : grayed = false
{
	Create(srchwnd, m_menuoffset);

	UINT flags = MF_STRING | MF_POPUP;
	if (grayed) {
		//2024/06/24
		flags |= MF_GRAYED;
	}
	
	if (parmenu){
		AppendMenu(parmenu, flags, (UINT_PTR)m_rsubmenu, partitle);
	}

	return 0;
}


HMENU CRMenuMain::GetRMenu()
{
	return m_rmenu;
}
HMENU CRMenuMain::GetSubMenu()
{
	return m_rsubmenu;
}
int CRMenuMain::Destroy()
{
	DestroyMenu( m_rmenu );
	m_rmenu = 0;
	m_rsubmenu = 0;
	m_menuid = 0;
	return 0;
}


int CRMenuMain::TrackPopupMenu( POINT pt )
{
	MENUINFO menuinfo;
	ZeroMemory(&menuinfo, sizeof(menuinfo));
	menuinfo.cbSize = sizeof(MENUINFO);
	menuinfo.fMask = MIM_MAXHEIGHT;//設定メンバを選択
	menuinfo.cyMax = 400;//メニューの高さの最大値　ピクセル単位
	SetMenuInfo(m_rsubmenu, &menuinfo);//2024/07/26

	Params2Dlg();
	//int retmenuid;
	//retmenuid = ::TrackPopupMenu(m_rsubmenu, TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, 0, m_menuwnd, NULL);
	::TrackPopupMenu(m_rsubmenu, TPM_LEFTALIGN, pt.x, pt.y, 0, m_menuwnd, NULL);

	return 0;
}
int CRMenuMain::TrackPopupMenuReturnCmd(POINT pt)
{
	MENUINFO menuinfo;
	ZeroMemory(&menuinfo, sizeof(menuinfo));
	menuinfo.cbSize = sizeof(MENUINFO);
	menuinfo.fMask = MIM_MAXHEIGHT;//設定メンバを選択
	menuinfo.cyMax = 400;//メニューの高さの最大値　ピクセル単位
	SetMenuInfo(m_rsubmenu, &menuinfo);//2024/07/26

	Params2Dlg();
	int menuid = ::TrackPopupMenuEx(m_rsubmenu, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, m_menuwnd, NULL);
	return menuid;
}

int CRMenuMain::Params2Dlg()
{

///////
	/*
	if( m_iktrans )
		CheckMenuItem( m_rmenu, ID_RMENU_IKTRANS, MF_CHECKED );
	else
		CheckMenuItem( m_rmenu, ID_RMENU_IKTRANS, MF_UNCHECKED );

	if( m_ikskip )
		CheckMenuItem( m_rmenu, ID_RMENU_IKSKIP, MF_CHECKED );
	else
		CheckMenuItem( m_rmenu, ID_RMENU_IKSKIP, MF_UNCHECKED );
	*/

	return 0;
}


int CRMenuMain::CheckLevelMenu( int checkid )
{
	/*
	static int idtable[16] = {
		ID_RMENU_LEVEL1, ID_RMENU_LEVEL2, ID_RMENU_LEVEL3, ID_RMENU_LEVEL4, ID_RMENU_LEVEL5,
		ID_RMENU_LEVEL6, ID_RMENU_LEVEL7, ID_RMENU_LEVEL8, ID_RMENU_LEVEL9, ID_RMENU_LEVEL10,
		ID_RMENU_LEVEL11, ID_RMENU_LEVEL12, ID_RMENU_LEVEL13, ID_RMENU_LEVEL14, ID_RMENU_LEVEL15,
		ID_RMENU_LEVELINF
	};

	int no;
	int curid;
	for( no = 0; no < 16; no++ ){
		curid = idtable[no];

		if( curid == checkid ){
			CheckMenuItem( m_rmenu, curid, MF_CHECKED );
		}else{
			CheckMenuItem( m_rmenu, curid, MF_UNCHECKED );
		}
	}
	*/

	return 0;
}
