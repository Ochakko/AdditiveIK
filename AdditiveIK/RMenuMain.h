#ifndef RMENU1H
#define RMENU1H

class CRMenuMain
{
public:
	CRMenuMain( int srcmenuid );
	~CRMenuMain();

	int Create( HWND srchwnd, int menuoffset );
	int CreatePopupMenu(HWND srchwnd, HMENU parmenu, WCHAR* partitle, bool grayed = false);
	int Destroy();

	HMENU GetRMenu();
	HMENU GetSubMenu();

	int TrackPopupMenu(POINT pt);
	int TrackPopupMenuReturnCmd(POINT pt);

private:
	int InitParams();
	int CheckLevelMenu( int checkid );
	int Params2Dlg();

private:
	int m_menuoffset;
	HWND m_menuwnd;
	HMENU m_rmenu;
	HMENU m_rsubmenu;
	DWORD m_menuid;

};

#endif