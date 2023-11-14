#pragma once



typedef LRESULT(CALLBACK* LPAPPCALLBACKMSGPROC)(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);


class GraphicsEngine;
extern HWND			g_hWnd ;				//ウィンドウハンドル。

//ゲームの初期化。
RECT InitGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName, HWND srcparentwnd, 
	int srcposx, int srcposy,
	int srcwidth, int srcheight,
	LPAPPCALLBACKMSGPROC srcmsgproc);
//ウィンドウメッセージをディスパッチ。falseが返ってきたら、ゲーム終了。
bool DispatchWindowMessage();
