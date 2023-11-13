#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>

#include <GlobalVar.h>

#include "DSUpdateUnderTracking.h"

#include <process.h>

#include <crtdbg.h>
#define DBGH
#include <dbg.h>


#include "resource.h"

LRESULT CALLBACK DSUpdaterWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int InitializeDSUpdateUnderTracking(CREATESTRUCT* createWindowArgs);
HANDLE g_hUnderTrackingThread = NULL;
HANDLE g_hEvent = INVALID_HANDLE_VALUE; //手動リセットイベント

										
										
//extern
extern void OnDSUpdate();
extern void OnDSMouseHereApeal();
extern LONG g_undertrackingRMenu;
extern LONG g_underApealingMouseHere;
extern void InfoBvh2FbxBatchCnt();
extern int g_bvh2fbxbatchflag;





static CDSUpdateUnderTracking* s_contextwin = 0;
static HINSTANCE s_hinstance = 0;
// thread 終了トリガー
// 0 が立つと　_endthreadex で　終了　(　handle は　close しない！！！　)
static LONG s_lThread = 1;
static unsigned int s_dwMainId = 0;
static DWORD s_mainthreadid = 0;
//static DWORD WINAPI	ThreadFunc(LPVOID	lpThreadParam);

static unsigned __stdcall ThreadFunc_DS(LPVOID pArguments);
unsigned __stdcall ThreadFunc_DS(LPVOID lpThreadParam)
{
	static int isfirst = 1;
	//int ret;
	if (!lpThreadParam) {
		return 1;
	}
	CDSUpdateUnderTracking* dsptr = (CDSUpdateUnderTracking*)lpThreadParam;

	while (s_lThread) {
		//if (::MsgWaitForMultipleObjects(1, &g_hEvent, FALSE, 500, 0) == WAIT_OBJECT_0) {

			//infowinptr->UpdateWindowFunc();
		//if (g_undertrackingRMenu == 1) {
		if (InterlockedAdd(&g_undertrackingRMenu, 0) == 1) {
			OnDSUpdate();
		}
		//if (g_bvh2fbxbatchflag == 1) {
		//	InfoBvh2FbxBatchCnt();
		//}

		Sleep(16);

		isfirst = 0;
		//ResetEvent(g_hEvent);
	//}
	}
	//_endthreadex( 0 );//<----ThreadFuncがreturnするときに、自動的に呼ばれる。
	return 0;
}







CDSUpdateUnderTracking::CDSUpdateUnderTracking()
{
	//SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	InitParams();
}
CDSUpdateUnderTracking::~CDSUpdateUnderTracking()
{
	DestroyObjs();
}
void CDSUpdateUnderTracking::InitParams()
{
	s_contextwin = 0;
	m_isfirstoutput = true;

	m_hWnd = NULL;
	m_hParentWnd = NULL;
	//m_hdcM = 0;

	m_stroutput = 0;

	m_dataindex = 0;
	m_viewindex = 0;

	m_mhcursor = 0;
	m_arrowcursor = 0;

}
void CDSUpdateUnderTracking::DestroyObjs()
{
	if (g_hUnderTrackingThread != NULL) {


		DWORD dwwait = WAIT_TIMEOUT;
		InterlockedExchange(&s_lThread, 0);
		Sleep(500);
		//InterlockedExchange(&m_preview_flag, 0);
		SetEvent(g_hEvent);//!!! msgwaitを解除。

		while (dwwait != WAIT_OBJECT_0) {
			dwwait = WaitForSingleObject(g_hUnderTrackingThread, 1500);
			//INFINITEで待つと、実行スレッドが無くなってしまい、デッドロックする。
		}

		CloseHandle(g_hUnderTrackingThread);
		//DbgOut("motparamdlg : thread handle close\n");
	}


	if (g_hEvent != NULL) {
		CloseHandle(g_hEvent);
	}



	//if (m_hdcM) {
	//	delete m_hdcM;
	//	m_hdcM = 0;
	//}

	if (m_stroutput) {
		free(m_stroutput);
		m_stroutput = 0;
	}
}


int CDSUpdateUnderTracking::CreateDSUpdateUnderTracking(HINSTANCE srchinstance)
{
	//SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	s_hinstance = srchinstance;


	////ウィンドウクラスを登録
	//WNDCLASSEX wcex;
	//ZeroMemory((LPVOID)&wcex, sizeof(WNDCLASSEX));
	//wcex.cbSize = sizeof(WNDCLASSEX);
	//wcex.style = 0;
	//wcex.lpfnWndProc = InfoWndProc;
	//wcex.cbClsExtra = 0;
	//wcex.cbWndExtra = 0;
	//wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	//wcex.hIcon = NULL;
	//wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = NULL;
	//wcex.lpszClassName = L"DSUpdateUnderTracking_";
	//wcex.hIconSm = NULL;
	//RegisterClassEx(&wcex);



	////ウィンドウ作成
	//int cxframe = GetSystemMetrics(SM_CXFRAME);
	//int cyframe = GetSystemMetrics(SM_CYFRAME);
	////if (istopmost) {
	////	hWnd = CreateWindowEx(//WS_EX_TOOLWINDOW|WS_EX_TOPMOST,szclassName,title,WS_POPUP,
	////		WS_EX_LEFT | WS_EX_TOPMOST, szclassName, title, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
	////		pos.x, pos.y,
	////		size.x, size.y,
	////		hWndParent, NULL, hInstance, NULL);
	////}
	////else {
	//m_hWnd = CreateWindowEx(
	//	WS_EX_LEFT, L"DSUpdateUnderTracking_", L"DSUpdateUnderTracking", WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_THICKFRAME,
	//	srcposx, srcposy,
	//	srcwidth, srcheight - 2 * cyframe,
	//	srcparentwnd, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
	////}
	////SetParent(s_infownd, s_mainhwnd);

	//if (m_hWnd) {
	//	m_hdcM = new OrgWinGUI::HDCMaster();
	//	if (m_hdcM) {
	//		m_hdcM->setHWnd(m_hWnd);

	//		//ウィンドウ表示
	//		ShowWindow(m_hWnd, SW_SHOW);

	//		s_contextwin = this;

	//		//return 0;
	//	}
	//	else {
	//		return 1;
	//	}
	//}
	//else {
	//	return 1;
	//}


	// ( , 手動リセット, ノンシグナル初期化, )
	g_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	//g_hUnderTrackingThread = BEGINTHREADEX(
	g_hUnderTrackingThread = (HANDLE)_beginthreadex(
		NULL, 0, &ThreadFunc_DS,
		(void*)this,
		0, &s_dwMainId);
		//CREATE_SUSPENDED, &s_dwMainId );

	return 0;

}

int CDSUpdateUnderTracking::OutputInfo(WCHAR* lpFormat, ...)
{
	//if (!m_hWnd) {
	//	return 0;
	//}
	//if (!IsWindow(m_hWnd)) {
	//	return 0;
	//}
	//if (!m_stroutput) {
	//	return 1;
	//}
	//if (m_dataindex >= DSUpdateUnderTrackingLINEH) {
	//	return 1;
	//}

	//int ret;
	//va_list Marker;
	//unsigned long wleng, writeleng;
	//WCHAR outchar[DSUpdateUnderTrackingLINEW];

	//ZeroMemory(outchar, sizeof(WCHAR) * DSUpdateUnderTrackingLINEW);

	//va_start(Marker, lpFormat);
	//ret = vswprintf_s(outchar, DSUpdateUnderTrackingLINEW, lpFormat, Marker);
	//va_end(Marker);

	//if (ret < 0)
	//	return 1;

	//if (!m_isfirstoutput) {
	//	m_dataindex++;
	//	if (m_dataindex >= DSUpdateUnderTrackingLINEH) {
	//		m_dataindex = 0;
	//		m_isfirstoutput = true;
	//		//MessageBox(m_hWnd, L"DSUpdateUnderTrackingの出力行数が制限に達しました。", L"警告", MB_OK);
	//		//return 1;
	//	}
	//}


	////最新ビューの場合にのみ最新状態に更新する。
	////ホイールでビュー位置を操作するため、最初の書き込みのときと最新ビュー以外の場合はいじらない。
	//if (m_isfirstoutput || (m_viewindex == (m_dataindex - 1))) {
	//	m_viewindex = m_dataindex;
	//}


	////wleng = (unsigned long)wcslen(outchar);
	//wcscpy_s(m_stroutput + m_dataindex * DSUpdateUnderTrackingLINEW, DSUpdateUnderTrackingLINEW, outchar);

	//m_isfirstoutput = false;

	////WriteFile(dbgfile, outchar, sizeof(WCHAR) * wleng, &writeleng, NULL);

	return 0;
}
void CDSUpdateUnderTracking::UpdateWindow()
{
	//if (m_hWnd && IsWindow(m_hWnd)) {
	//	if (s_hEvent != INVALID_HANDLE_VALUE) {
	//		SetEvent(s_hEvent);
	//	}
	//}
}

void CDSUpdateUnderTracking::UpdateWindowFunc()
{
	//if (m_hWnd && IsWindow(m_hWnd)) {
	//	RECT clirect;
	//	GetClientRect(m_hWnd, &clirect);
	//	InvalidateRect(m_hWnd, &clirect, true);
	//	::UpdateWindow(m_hWnd);
	//}
}

void CDSUpdateUnderTracking::OnPaint()
{
	//if (m_hWnd && IsWindow(m_hWnd)) {

	//	RECT clirect;
	//	GetClientRect(m_hWnd, &clirect);

	//	m_hdcM->beginPaint();
	//	m_hdcM->setPenAndBrush(RGB(70, 50, 70), RGB(70, 50, 70));
	//	Rectangle(m_hdcM->hDC, 0, 0, clirect.right, clirect.bottom);
	//	m_hdcM->setFont(12, _T("ＭＳ ゴシック"));
	//	SetTextColor(m_hdcM->hDC, RGB(255, 255, 255));
	//	
	//	//TextOut(m_hdcM->hDC, 10, 2, outchar, (int)wcslen(outchar));

	//	//DSUpdateUnderTrackingLINEH行分、古い順に描画する
	//	int outputno;
	//	int curindex = m_viewindex - (DSUpdateUnderTrackingLINEVIEW - 1);
	//	int dispno = 0;
	//	for (outputno = 0; outputno < DSUpdateUnderTrackingLINEVIEW; outputno++) {
	//		if ((curindex >= 0) && (curindex <= m_dataindex)) {
	//			TextOut(m_hdcM->hDC, 5, 5 + 15 * dispno, m_stroutput + curindex * DSUpdateUnderTrackingLINEW, (int)wcslen(m_stroutput + curindex * DSUpdateUnderTrackingLINEW));
	//			dispno++;
	//		}
	//		curindex++;
	//	}
	//	m_hdcM->endPaint();
	//}
}

//void CDSUpdateUnderTracking::onMouseWheel(const OrgWinGUI::MouseEvent& e)
//{
//	////ホイールで描画する行をシフトする。
//
//	//if (e.wheeldelta < 0) {
//	//	m_viewindex++;
//	//	if (m_viewindex > m_dataindex) {
//	//		m_viewindex = m_dataindex;
//	//	}
//	//	UpdateWindow();
//	//}
//	//else if (e.wheeldelta > 0) {
//	//	m_viewindex--;
//	//	if (m_viewindex < (DSUpdateUnderTrackingLINEVIEW - 1)) {
//	//		m_viewindex = (DSUpdateUnderTrackingLINEVIEW - 1);
//	//	}
//	//	UpdateWindow();
//	//}
//}

int CDSUpdateUnderTracking::GetStrNum()
{
	//if (m_isfirstoutput) {
	//	return 0;
	//}
	//else {
	//	int retnum = m_dataindex + 1;
	//	if (retnum > DSUpdateUnderTrackingLINEH) {
	//		retnum = DSUpdateUnderTrackingLINEH;
	//	}
	//	return retnum;
	//}
	return 0;
}
int CDSUpdateUnderTracking::GetStr(int srcindex, int srcoutleng, WCHAR* strout)
{
	//if (!strout) {
	//	return 1;
	//}
	//if (srcoutleng < 0)
	//{
	//	return 1;
	//}

	//if ((srcindex >= 0) && (srcindex < DSUpdateUnderTrackingLINEH)) {
	//	wcscpy_s(strout, srcoutleng, m_stroutput + srcindex * DSUpdateUnderTrackingLINEW);
	//	return 0;
	//}
	//else {
	//	return 1;
	//}
	return 0;
}



LRESULT CALLBACK DSUpdaterWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//POINT tmpPoint;
	//RECT tmpRect;
	//OrgWinGUI::MouseEvent mouseEvent;

	//switch (uMsg)
	//{
	//	//case WM_TIMER:
	//	//	OnTimerFunc(wParam);
	//	//	break;
	//case WM_DESTROY:
	//	//PostQuitMessage(0);
	//	return 0;
	//	break;
	//case WM_CREATE:
	//	return InitializeDSUpdateUnderTracking((CREATESTRUCT*)lParam);
	//	break;
	//	//case WM_COMMAND:
	//	//{
	//	//	if ((menuid >= 59900) && (menuid <= (59900 + MAXMOTIONNUM))) {
	//	//		ActivatePanel(0);
	//	//		int selindex = menuid - 59900;
	//	//		OnAnimMenu(selindex);
	//	//		ActivatePanel(1);
	//	//		return 0;
	//	//	}
	//	//}
	//	//break;
	//case WM_PAINT:				//描画
	//	if (s_contextwin) {
	//		s_contextwin->OnPaint();
	//	}
	//	return 0;
	//case WM_MOUSEWHEEL:
	//	if (s_contextwin) {
	//		GetCursorPos(&tmpPoint);
	//		GetWindowRect(s_contextwin->GetHWnd(), &tmpRect);

	//		mouseEvent.globalX = (int)tmpPoint.x;
	//		mouseEvent.globalY = (int)tmpPoint.y;
	//		mouseEvent.localX = mouseEvent.globalX - tmpRect.left;
	//		mouseEvent.localY = mouseEvent.globalY - tmpRect.top;
	//		mouseEvent.shiftKey = GetKeyState(VK_SHIFT) < 0;
	//		mouseEvent.ctrlKey = GetKeyState(VK_CONTROL) < 0;
	//		mouseEvent.altKey = GetKeyState(VK_MENU) < 0;
	//		mouseEvent.wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam);

	//		s_contextwin->onMouseWheel(mouseEvent);

	//	}
	//	break;
	//case WM_MOUSEHOVER:
	//	if (s_contextwin) {
	//		SetCapture(s_contextwin->GetHWnd());
	//	}
	//	break;
	//case WM_MOUSELEAVE:
	//	if (s_contextwin) {
	//		ReleaseCapture();
	//	}
	//	break;
	//default:
	//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	//}

	//if (uMsg != WM_SETCURSOR) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	//}


	return 0;
}

int InitializeDSUpdateUnderTracking(CREATESTRUCT* createWindowArgs)
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

