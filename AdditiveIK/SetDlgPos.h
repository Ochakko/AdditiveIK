#ifndef SETDLGPOSH
#define SETDLGPOSH


#ifdef DECSETDLGPOSH
int SetDlgPosDesktopCenter(HWND hDlgWnd, HWND hWndInsertAfter);
int SetDlgPosCursorPos(HWND hDlgWnd, HWND hWndInsertAfter);
POINT GetDlgPosDesktopCenter(RECT dlgrect);
#else
extern int SetDlgPosDesktopCenter(HWND hDlgWnd, HWND hWndInsertAfter);
extern int SetDlgPosCursorPos(HWND hDlgWnd, HWND hWndInsertAfter);
extern POINT GetDlgPosDesktopCenter(RECT dlgrect);
#endif


#endif