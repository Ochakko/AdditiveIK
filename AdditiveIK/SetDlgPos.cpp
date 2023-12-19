#include "stdafx.h"

#include <windows.h>

#define DECSETDLGPOSH
#include "SetDlgPos.h"

#define DBGH
#include <dbg.h>
#include <crtdbg.h>

using namespace std;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル


int SetDlgPosDesktopCenter(HWND hDlgWnd, HWND hWndInsertAfter)
{
	int lefttopposx = 0;
	int lefttopposy = 0;
	int setposx = 0;
	int setposy = 0;

	//POINT ptCursor;
	//GetCursorPos(&ptCursor);
	////::ScreenToClient(s_3dwnd, &ptCursor);

	HWND desktopwnd;
	desktopwnd = ::GetDesktopWindow();
	if (desktopwnd) {
		RECT desktoprect;
		::GetClientRect(desktopwnd, &desktoprect);
		lefttopposx = (desktoprect.left + desktoprect.right) / 2;
		lefttopposy = (desktoprect.top + desktoprect.bottom) / 2;

		//2023/01/26 dlgサイズを考慮して　デスクトップ中央に配置


		RECT dlgrect;
		::GetClientRect(hDlgWnd, &dlgrect);
		POINT clientlefttop = { 0, 0 };
		::ClientToScreen(hDlgWnd, &clientlefttop);
		RECT apprect;
		::GetWindowRect(g_mainhwnd, &apprect);
		POINT windowlefttop = { apprect.left, apprect.top };

		//2023/12/18 Appの左上隅に表示されている場合だけセンターに持ってくる
		if (((clientlefttop.x - windowlefttop.x) >= 0) && 
			((clientlefttop.x - windowlefttop.x) <= 24) &&//16:フレームの厚さなどの分より少し大きめ
			((clientlefttop.y - windowlefttop.y) >= 0) &&
			((clientlefttop.y - windowlefttop.y) <= 100)) {//82:Menuなどの分より少し大きめ

			setposx = max(60, (lefttopposx - (dlgrect.right - dlgrect.left) / 2));
			setposy = max(60, (lefttopposy - (dlgrect.bottom - dlgrect.top) / 2));
			setposx = min(max(60, desktoprect.right - 60), setposx);
			setposy = min(max(60, desktoprect.bottom - 60), setposy);

			SetWindowPos(hDlgWnd, hWndInsertAfter, setposx, setposy, 0, 0, SWP_NOSIZE);
		}
		else {
			int dbgflag1 = 1;
		}
	}
	else {
		lefttopposx = 0;
		lefttopposy = 0;
		setposx = 0;
		setposy = 0;
	}

	//RECT dlgrect;
	//GetWindowRect(fgwnd, &dlgrect);
	//SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);
	
	//2023/01/25
	//この関数は　指定しているウインドウがフォアグランドになったときにも呼ばれる
	//機能のボタンを押した際　マウスがアップする前に呼ばれることがある
	//マウスを動かすと　ボタンが機能しないことがあるので　SetCursorPosはコメントアウト
	//SetCursorPos(posx + 25, posy + 10);

	//2023/01/26
	//SetCursorPosについて
	//この関数がフォアグランドイベントから呼び出されるので　SetCursorPosをコメントアウトした
	//この関数の外で　ダイアログ表示時にSetCursorPosすることは大丈夫

	return 0;
}

int SetDlgPosCursorPos(HWND hDlgWnd, HWND hWndInsertAfter)
{
	int posx = 0;
	int posy = 0;

	POINT ptCursor;
	GetCursorPos(&ptCursor);
	////::ScreenToClient(s_3dwnd, &ptCursor);

	posx = ptCursor.x;
	posy = ptCursor.y;

	SetWindowPos(hDlgWnd, hWndInsertAfter, posx, posy, 0, 0, SWP_NOSIZE);

	return 0;
}



