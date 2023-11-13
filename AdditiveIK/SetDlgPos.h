#ifndef SETDLGPOSH
#define SETDLGPOSH


#ifdef DECSETDLGPOSH
int SetDlgPosDesktopCenter(HWND hDlgWnd, HWND hWndInsertAfter);
int SetDlgPosCursorPos(HWND hDlgWnd, HWND hWndInsertAfter);
#else
extern int SetDlgPosDesktopCenter(HWND hDlgWnd, HWND hWndInsertAfter);
extern int SetDlgPosCursorPos(HWND hDlgWnd, HWND hWndInsertAfter);
#endif


#endif