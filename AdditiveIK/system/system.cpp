#include "stdafx.h"
#include "system.h"
#include "GraphicsEngine.h"

#include <GlobalVar.h>

HWND			g_hWnd = NULL;				//ウィンドウハンドル。

///////////////////////////////////////////////////////////////////
//メッセージプロシージャ。
//hWndがメッセージを送ってきたウィンドウのハンドル。
//msgがメッセージの種類。
//wParamとlParamは引数。今は気にしなくてよい。
///////////////////////////////////////////////////////////////////
// 
//
// ####################################
// 2023/11/14
// InitWindow()関数の引数で渡すことにした
// ####################################
// 
LRESULT CALLBACK AppMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//送られてきたメッセージで処理を分岐させる。
	switch (msg)
	{
	case WM_DESTROY:
		//スエンジンの破棄。
		delete g_engine;
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////
// ウィンドウの初期化。
///////////////////////////////////////////////////////////////////
RECT InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName, 
	HWND srcparentwnd, 
	int srcposx, int srcposy, 
	int srcwidth, int srcheight,
	LPAPPCALLBACKMSGPROC srcmsgproc)
{
	//ウィンドウクラスのパラメータを設定(単なる構造体の変数の初期化です。)
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),		//構造体のサイズ。
		//CS_CLASSDC,				//ウィンドウのスタイル。
		CS_DBLCLKS,
								//ここの指定でスクロールバーをつけたりできるが、ゲームでは不要なのでCS_CLASSDCでよい。
		//MsgProc,				//メッセージプロシージャ(後述)
		//srcmsgproc,
		AppMsgProc,
		0,						//0でいい。
		0,						//0でいい。
		//GetModuleHandle(NULL),	//このクラスのためのウインドウプロシージャがあるインスタンスハンドル。
		hInstance,
								//何も気にしなくてよい。
		NULL,					//アイコンのハンドル。アイコンを変えたい場合ここを変更する。とりあえずこれでいい。
		//NULL,					//マウスカーソルのハンドル。NULLの場合はデフォルト。
		LoadCursor(nullptr, IDC_ARROW),
		//NULL,					//ウィンドウの背景色。NULLの場合はデフォルト。
		//(HBRUSH)GetStockObject(BLACK_BRUSH),
		//(HBRUSH)GetStockObject(GRAY_BRUSH),
		NULL,
		NULL,					//メニュー名。NULLでいい。
		appName,				//ウィンドウクラスに付ける名前。
		NULL					//NULLでいい。
	};
	//ウィンドウクラスの登録。
	RegisterClassEx(&wc);

	HMENU hMenu = NULL;
	LONG winstyle = WS_OVERLAPPEDWINDOW;
	winstyle &= ~WS_CAPTION;
	winstyle |= WS_CHILD;
	//LONG winstyle = WS_CHILD;

	RECT rc;
	SetRect(&rc, 0, 0, srcwidth, srcheight);
	AdjustWindowRect(&rc, winstyle, (hMenu) ? true : false);
	//s_mainwidth = rc.right - rc.left;
	//s_mainheight = rc.bottom - rc.top;

	// ウィンドウの作成。
	g_hWnd = CreateWindow(
		appName,				//使用するウィンドウクラスの名前。
								//先ほど作成したウィンドウクラスと同じ名前にする。
		appName,				//ウィンドウの名前。ウィンドウクラスの名前と別名でもよい。
		//WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル。ゲームでは基本的にWS_OVERLAPPEDWINDOWでいい、
		winstyle,
		srcposx,						//ウィンドウの初期X座標。
		srcposy,						//ウィンドウの初期Y座標。
		//s_mainwidth,			//ウィンドウの幅。
		//s_mainheight,			//ウィンドウの高さ。
		(rc.right - rc.left),
		(rc.bottom - rc.top),
		//srcwidth,
		//srcheight,
		srcparentwnd,			//親ウィンドウ。ゲームでは基本的にNULLでいい。
		NULL,					//メニュー。今はNULLでいい。
		hInstance,				//アプリケーションのインスタンス。
		NULL
	);

	ShowWindow(g_hWnd, nCmdShow);

	return rc;
}


//ゲームの初期化。
RECT InitGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName, 
	HWND srcparentwnd, 
	int srcposx, int srcposy,
	int srcwidth, int srcheight,
	LPAPPCALLBACKMSGPROC srcmsgproc)
{
	//ウィンドウを初期化。
	RECT rc = InitWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow, appName, 
		srcparentwnd, 
		srcposx, srcposy, 
		srcwidth, srcheight,
		srcmsgproc);
	//TKエンジンの初期化。
	g_engine = new TkEngine;
	g_engine->Init(g_hWnd, srcwidth, srcheight);

	return rc;
}
//ウィンドウメッセージをディスパッチ。falseが返ってきたら、ゲーム終了。
bool DispatchWindowMessage()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		//ウィンドウからのメッセージを受け取る。
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ウィンドウメッセージが空になった。
			break;
		}
	}
	return msg.message != WM_QUIT;
}