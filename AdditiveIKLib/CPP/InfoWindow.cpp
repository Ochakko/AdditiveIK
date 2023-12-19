#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>

#include <InfoWindow.h>

#include <crtdbg.h>
#define DBGH
#include <dbg.h>


LRESULT CALLBACK InfoWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int InitializeInfoWindow(CREATESTRUCT* createWindowArgs);

static CInfoWindow* s_contextwin = 0;

static HANDLE s_hThread = NULL;
// thread 終了トリガー
// 0 が立つと　_endthreadex で　終了　(　handle は　close しない！！！　)
static LONG s_lThread = 1;
static unsigned int s_dwMainId = 0;
static DWORD s_mainthreadid = 0;
//static DWORD WINAPI	ThreadFunc(LPVOID	lpThreadParam);
static unsigned __stdcall ThreadFunc_Info(void* pArguments);
static HANDLE s_hEvent = INVALID_HANDLE_VALUE; //手動リセットイベント

unsigned __stdcall ThreadFunc_Info(LPVOID lpThreadParam)
{
	static int isfirst = 1;
	//int ret;

	if (!lpThreadParam) {
		return 1;
	}
	CInfoWindow* infowinptr = (CInfoWindow*)lpThreadParam;

	while (s_lThread) {
		if (::MsgWaitForMultipleObjects(1, &s_hEvent, FALSE, 500, 0) == WAIT_OBJECT_0) {

			infowinptr->UpdateWindowFunc();

			isfirst = 0;
			ResetEvent(s_hEvent);
		}
	}

	//_endthreadex( 0 );//<----ThreadFuncがreturnするときに、自動的に呼ばれる。

	return 0;
}



CInfoWindow::CInfoWindow()
{
	InitParams();
}
CInfoWindow::~CInfoWindow()
{
	DestroyObjs();
}
void CInfoWindow::InitParams()
{
	s_contextwin = 0;
	m_isfirstoutput = true;

	m_hWnd = NULL;
	m_hParentWnd = NULL;
	m_hdcM = 0;

	m_stroutput = 0;

	m_dataindex = 0;
	m_viewindex = 0;

}
void CInfoWindow::DestroyObjs()
{
	if (s_hThread != NULL) {
		DWORD dwwait = WAIT_TIMEOUT;
		InterlockedExchange(&s_lThread, 0);
		Sleep(500);
		//InterlockedExchange(&m_preview_flag, 0);
		SetEvent(s_hEvent);//!!! msgwaitを解除。

		while (dwwait != WAIT_OBJECT_0) {
			dwwait = WaitForSingleObject(s_hThread, 1500);
			//INFINITEで待つと、実行スレッドが無くなってしまい、デッドロックする。
		}

		CloseHandle(s_hThread);
		//DbgOut("motparamdlg : thread handle close\n");
	}


	if (s_hEvent != NULL) {
		CloseHandle(s_hEvent);
	}



	if (m_hdcM) {
		delete m_hdcM;
		m_hdcM = 0;
	}

	if (m_stroutput) {
		free(m_stroutput);
		m_stroutput = 0;
	}
}


int CInfoWindow::CreateInfoWindow(HWND srcparentwnd, int srcposx, int srcposy, int srcwidth, int srcheight)
{
	//データを作成
	m_stroutput = (WCHAR*)malloc(sizeof(WCHAR) * INFOWINDOWLINEW * INFOWINDOWLINEH);
	if (!m_stroutput) {
		return 1;
	}
	ZeroMemory(m_stroutput, sizeof(WCHAR) * INFOWINDOWLINEW * INFOWINDOWLINEH);

	HBRUSH blkbrush = CreateSolidBrush(RGB(0, 0, 0));//自分で削除しない　DestroyWindow時に解放される

	//ウィンドウクラスを登録
	WNDCLASSEX wcex;
	ZeroMemory((LPVOID)&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = InfoWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcex.hCursor = NULL;
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hbrBackground = blkbrush;// (HBRUSH)COLOR_BACKGROUND + 1;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"InfoWindow_";
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);



	//ウィンドウ作成
	int cxframe = GetSystemMetrics(SM_CXFRAME);
	int cyframe = GetSystemMetrics(SM_CYFRAME);
	//if (istopmost) {
	//	hWnd = CreateWindowEx(//WS_EX_TOOLWINDOW|WS_EX_TOPMOST,szclassName,title,WS_POPUP,
	//		WS_EX_LEFT | WS_EX_TOPMOST, szclassName, title, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
	//		pos.x, pos.y,
	//		size.x, size.y,
	//		hWndParent, NULL, hInstance, NULL);
	//}
	//else {
	m_hWnd = CreateWindowEx(
		WS_EX_LEFT, L"InfoWindow_", L"InfoWindow", WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,// | WS_THICKFRAME,
		srcposx, srcposy,
		srcwidth, srcheight - 2 * cyframe,
		srcparentwnd, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
	//}
	//SetParent(s_infownd, s_mainhwnd);

	if (m_hWnd) {
		m_hdcM = new OrgWinGUI::HDCMaster();
		if (m_hdcM) {
			m_hdcM->setHWnd(m_hWnd);

			//ウィンドウ表示
			ShowWindow(m_hWnd, SW_SHOW);

			s_contextwin = this;

			//return 0;
		}
		else {
			return 1;
		}
	}
	else {
		return 1;
	}


	// ( , 手動リセット, ノンシグナル初期化, )
	s_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	//s_hThread = BEGINTHREADEX(
	s_hThread = (HANDLE)_beginthreadex(
		NULL, 0, &ThreadFunc_Info,
		(void*)this,
		0, &s_dwMainId);
		//CREATE_SUSPENDED, &s_dwMainId );

	return 0;

}

int CInfoWindow::OutputInfo(const WCHAR* lpFormat, ...)
{
	if (!m_hWnd) {
		return 0;
	}
	if (!IsWindow(m_hWnd)) {
		return 0;
	}
	if (!m_stroutput) {
		return 1;
	}
	if (m_dataindex >= INFOWINDOWLINEH) {
		return 1;
	}

	int ret;
	va_list Marker;
	//unsigned long wleng, writeleng;
	WCHAR outchar[INFOWINDOWLINEW];

	ZeroMemory(outchar, sizeof(WCHAR) * INFOWINDOWLINEW);

	va_start(Marker, lpFormat);
	ret = vswprintf_s(outchar, INFOWINDOWLINEW, lpFormat, Marker);
	va_end(Marker);

	if (ret < 0)
		return 1;

	if (!m_isfirstoutput) {
		m_dataindex++;
		if (m_dataindex >= INFOWINDOWLINEH) {
			m_dataindex = 0;
			m_isfirstoutput = true;
			//MessageBox(m_hWnd, L"InfoWindowの出力行数が制限に達しました。", L"警告", MB_OK);
			//return 1;
		}
	}


	//最新ビューの場合にのみ最新状態に更新する。
	//ホイールでビュー位置を操作するため、最初の書き込みのときと最新ビュー以外の場合はいじらない。
	if (m_isfirstoutput || (m_viewindex == (m_dataindex - 1))) {
		m_viewindex = m_dataindex;
	}


	//wleng = (unsigned long)wcslen(outchar);
	wcscpy_s(m_stroutput + (size_t)m_dataindex * INFOWINDOWLINEW, INFOWINDOWLINEW, outchar);

	m_isfirstoutput = false;

	//WriteFile(dbgfile, outchar, sizeof(WCHAR) * wleng, &writeleng, NULL);

	return 0;
}
void CInfoWindow::UpdateWindow()
{
	if (m_hWnd && IsWindow(m_hWnd)) {
		if (s_hEvent != INVALID_HANDLE_VALUE) {
			SetEvent(s_hEvent);
		}
	}
}

void CInfoWindow::UpdateWindowFunc()
{
	if (m_hWnd && IsWindow(m_hWnd)) {
		RECT clirect;
		GetClientRect(m_hWnd, &clirect);
		InvalidateRect(m_hWnd, &clirect, true);
		::UpdateWindow(m_hWnd);
	}
}

void CInfoWindow::OnPaint()
{
	int lineview;
	if (g_4kresolution) {
		lineview = INFOWINDOWLINEVIEW4K;
	}
	else {
		lineview = INFOWINDOWLINEVIEW;
	}

	if (m_hWnd && IsWindow(m_hWnd)) {
		RECT clirect;
		GetClientRect(m_hWnd, &clirect);

		m_hdcM->beginPaint();
		//m_hdcM->setPenAndBrush(RGB(70, 50, 70), RGB(70, 50, 70));
		m_hdcM->setPenAndBrush(RGB(0, 0, 0), RGB(0, 0, 0));
		Rectangle(m_hdcM->hDC, 0, 0, clirect.right, clirect.bottom);
		m_hdcM->setFont(12, _T("ＭＳ ゴシック"));
		SetTextColor(m_hdcM->hDC, RGB(240, 240, 240));

		if (m_stroutput) {
			//TextOut(m_hdcM->hDC, 10, 2, outchar, (int)wcslen(outchar));

			//INFOWINDOWLINEH行分、古い順に描画する
			int outputno;
			int curindex = m_viewindex - (lineview - 1);
			int dispno = 0;
			for (outputno = 0; outputno < lineview; outputno++) {
				if ((curindex >= 0) && (curindex <= m_dataindex)) {

					*(m_stroutput + INFOWINDOWLINEW * INFOWINDOWLINEH - 1) = 0L;
					size_t infolen;
					infolen = wcslen(m_stroutput + (size_t)curindex * INFOWINDOWLINEW);
					if ((infolen > 0) && (infolen < INFOWINDOWLINEW)) {
						TextOut(m_hdcM->hDC, 5, 5 + 15 * dispno, m_stroutput + (size_t)curindex * INFOWINDOWLINEW, (int)infolen);
					}
					else {
						_ASSERT(0);
					}
					dispno++;
				}
				curindex++;
			}

		}
		m_hdcM->endPaint();
	}
}

void CInfoWindow::onMouseWheel(const OrgWinGUI::MouseEvent& e)
{
	//ホイールで描画する行をシフトする。
	int lineview;
	if (g_4kresolution) {
		lineview = INFOWINDOWLINEVIEW4K;
	}
	else {
		lineview = INFOWINDOWLINEVIEW;
	}

	if (e.wheeldelta < 0) {
		m_viewindex++;
		if (m_viewindex > m_dataindex) {
			m_viewindex = m_dataindex;
		}
		UpdateWindow();
	}
	else if (e.wheeldelta > 0) {
		m_viewindex--;
		if (m_viewindex < (lineview - 1)) {
			m_viewindex = (lineview - 1);
		}
		UpdateWindow();
	}
}

int CInfoWindow::GetStrNum()
{
	if (m_isfirstoutput) {
		return 0;
	}
	else {
		int retnum = m_dataindex + 1;
		if (retnum > INFOWINDOWLINEH) {
			retnum = INFOWINDOWLINEH;
		}
		return retnum;
	}
}
int CInfoWindow::GetStr(int srcindex, int srcoutleng, WCHAR* strout)
{
	if (!strout) {
		return 1;
	}
	if (srcoutleng < 0)
	{
		return 1;
	}

	if ((srcindex >= 0) && (srcindex < INFOWINDOWLINEH)) {
		wcscpy_s(strout, srcoutleng, m_stroutput + (size_t)srcindex * INFOWINDOWLINEW);
		return 0;
	}
	else {
		return 1;
	}
}



LRESULT CALLBACK InfoWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT tmpPoint;
	RECT tmpRect;
	OrgWinGUI::MouseEvent mouseEvent;

	switch (uMsg)
	{
		//case WM_TIMER:
		//	OnTimerFunc(wParam);
		//	break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		return 0;
		break;
	case WM_CREATE:
		return InitializeInfoWindow((CREATESTRUCT*)lParam);
		break;
		//case WM_COMMAND:
		//{
		//	if ((menuid >= 59900) && (menuid <= (59900 + MAXMOTIONNUM))) {
		//		ActivatePanel(0);
		//		int selindex = menuid - 59900;
		//		OnAnimMenu(selindex);
		//		ActivatePanel(1);
		//		return 0;
		//	}
		//}
		//break;
	case WM_PAINT:				//描画
		if (s_contextwin) {
			s_contextwin->OnPaint();
		}
		return 0;
	case WM_MOUSEWHEEL:
		if (s_contextwin) {
			GetCursorPos(&tmpPoint);
			GetWindowRect(s_contextwin->GetHWnd(), &tmpRect);

			mouseEvent.globalX = (int)tmpPoint.x;
			mouseEvent.globalY = (int)tmpPoint.y;
			mouseEvent.localX = mouseEvent.globalX - tmpRect.left;
			mouseEvent.localY = mouseEvent.globalY - tmpRect.top;
			mouseEvent.shiftKey = GetKeyState(VK_SHIFT) < 0;
			mouseEvent.ctrlKey = GetKeyState(VK_CONTROL) < 0;
			mouseEvent.altKey = GetKeyState(VK_MENU) < 0;
			mouseEvent.wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam);

			s_contextwin->onMouseWheel(mouseEvent);

		}
		break;
	case WM_MOUSEHOVER:
		if (s_contextwin) {
			SetCapture(s_contextwin->GetHWnd());
		}
		break;
	case WM_MOUSELEAVE:
		if (s_contextwin) {
			ReleaseCapture();
		}
		break;
	default:
		//if (uMsg != WM_SETCURSOR) {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		//}
		break;
	}

	return 0;
}

int InitializeInfoWindow(CREATESTRUCT* createWindowArgs)
{
	//TCHAR message[1024];
	//int messageResult;
	//wsprintf(message,
	//	TEXT("ウィンドウクラス:%s\nタイトル:%s\nウィンドウを生成しますか？"),
	//	createWindowArgs->lpszClass, createWindowArgs->lpszName
	//);

	//messageResult = MessageBox(NULL, message, TEXT("確認"), MB_YESNO | MB_ICONINFORMATION);

	//if (messageResult == IDNO)
	//	return -1;
	return 0;
}

