#include "stdafx.h"
#include <CpInfoDlg2.h>

#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <OrgWindow.h>
#include <GlobalVar.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>

using namespace std;
using namespace OrgWinGUI;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル


/////////////////////////////////////////////////////////////////////////////
// CCpInfoDlg2

CCpInfoDlg2::CCpInfoDlg2()
{
	InitParams();
}

CCpInfoDlg2::~CCpInfoDlg2()
{
	DestroyObjs();
}
	
int CCpInfoDlg2::DestroyObjs()
{
	DestroyOWPWnd();
	return 0;
}

void CCpInfoDlg2::InitParams()
{
	m_dlgresult = -1;
	m_pcpinfo = 0;

	m_dlgwnd = nullptr;
	m_dlgspall = nullptr;
	m_fbxnameLabel = nullptr;
	m_motionnameLabel = nullptr;
	m_startframeLabel = nullptr;
	m_framenumLabel = nullptr;
	m_combosp = nullptr;
	m_bvhCombo = nullptr;
	m_importanceCombo = nullptr;
	m_memoEdit = nullptr;
	m_spacerLabel = nullptr;
	m_btnsp = nullptr;
	m_okB = nullptr;
	m_cancelB = nullptr;
}

//unsigned __stdcall ThreadFunc_Show(LPVOID lpThreadParam)
//{
//	if (!lpThreadParam) {
//		_ASSERT(0);
//		return 1;
//	}
//	CCpInfoDlg2* thisdlg = (CCpInfoDlg2*)lpThreadParam;
//	if (thisdlg) {
//		thisdlg->SetVisible(true);
//
//		//while (thisdlg->m_dlgresult < 0) {
//		//	Sleep(10);
//		//}
//	}
//	return 0;
//}
//int CCpInfoDlg2::ThreadingShow()
//{
//	unsigned int threadaddr1 = 0;
//	HANDLE handle1 = (HANDLE)_beginthreadex(
//		NULL, 0, ThreadFunc_Show,
//		(void*)this,
//		0, &threadaddr1);
//
//
//	if ((handle1 != 0) && (handle1 != INVALID_HANDLE_VALUE)) {
//		DWORD affinitymask = 4;
//		ULONG_PTR  processaffinity = 0;
//		ULONG_PTR  systemaffinity = 0;
//		GetProcessAffinityMask(GetModuleHandle(NULL), &processaffinity, &systemaffinity);
//		if (processaffinity & affinitymask) {
//			SetThreadAffinityMask(handle1, affinitymask);
//		}
//
//		//WiatForしない場合には先に閉じてもOK
//		CloseHandle(handle1);
//	}
//
//	while (m_dlgresult < 0) {
//		Sleep(20);
//		if (m_dlgwnd) {
//			m_dlgwnd->callRewrite();
//		}
//	}
//
//	return 0;
//}

void CCpInfoDlg2::SetVisible(bool srcflag)
{


	if (srcflag) {
		m_dlgresult = -1;//!!!!!!!!開くときに初期化　閉じるときには後で参照するので結果を初期化しない

		RECT dlgrect;
		dlgrect.top = 0;
		dlgrect.bottom = 400;
		dlgrect.left = 0;
		dlgrect.right = 600;
		POINT setpoint = GetDlgPosDesktopCenter(dlgrect);
		CreateOWPWnd(setpoint);

		if (m_dlgwnd) {
			ParamsToDlg();

			m_dlgwnd->setListenMouse(true);
			m_dlgwnd->setVisible(true);
			m_dlgwnd->callRewrite();
			//RECT dlgrect2;
			//::GetWindowRect(m_dlgwnd->getHWnd(), &dlgrect2);
			//SetCursorPos(dlgrect2.left + 25, dlgrect2.top + 10);

			EnableWindow(g_mainhwnd, FALSE);//!!!!!!!!!!!! 疑似モーダル


		}
	}
	else {
		if (m_dlgwnd) {
			m_dlgwnd->setVisible(false);
			//m_dlgwnd->setListenMouse(false);
		}
	}
	

}

int CCpInfoDlg2::OnOK()
{
	if (m_bvhCombo) {
		int combono = m_bvhCombo->getSelectedCombo();
		if (m_pcpinfo) {
			m_pcpinfo->bvhtype = combono;
		}
	}
	if (m_importanceCombo) {
		int combono = m_importanceCombo->getSelectedCombo();
		if (m_pcpinfo) {
			m_pcpinfo->importance = combono;
		}
	}
	if (m_memoEdit && m_pcpinfo && m_pcpinfo->comment) {
		WCHAR tmpcomment[HISTORYCOMMENTLEN] = { 0L };
		ZeroMemory(tmpcomment, sizeof(WCHAR) * HISTORYCOMMENTLEN);
		m_memoEdit->getName(tmpcomment, HISTORYCOMMENTLEN);
		tmpcomment[HISTORYCOMMENTLEN - 1] = 0L;
		if (tmpcomment[0] != 0L) {
			wcscpy_s(m_pcpinfo->comment, HISTORYCOMMENTLEN, tmpcomment);
		}
		else {
			m_pcpinfo->comment[0] = 0L;
		}
	}

	m_dlgresult = 0;
	SetVisible(false);
	return IDOK;
}

int CCpInfoDlg2::OnCancel()
{
	m_dlgresult = 1;
	SetVisible(false);
	return IDCANCEL;
}
int CCpInfoDlg2::OnClose()
{
	m_dlgresult = 2;
	SetVisible(false);
	return IDCANCEL;
}


int CCpInfoDlg2::ParamsToDlg()
{
/*
	typedef struct tag_cpinfo
	{
		WCHAR fbxname[MAX_PATH];
		WCHAR motionname[MAX_PATH];
		double startframe;
		double framenum;
		int bvhtype;//0:undef, 1-144:bvh1 - bvh144, -1:bvh_other
		int importance;//0:undef, 1:tiny, 2:alittle, 3:normal, 4:noticed, 5:imortant, 6:very important
		WCHAR comment[32];//WCHAR * 31文字まで。３２文字目は終端記号
	}CPMOTINFO;
*/

	//m_dlg_wnd.CheckRadioButton(IDC_RADIO1, IDC_RADIO5, (g_ClearColorIndex + IDC_RADIO1));
	if (!m_pcpinfo) {
		return 1;
	}

	WCHAR strtext[MAX_PATH] = { 0L };

	swprintf_s(strtext, MAX_PATH, L"fbx:%s", m_pcpinfo->fbxname);
	if (m_fbxnameLabel) {
		m_fbxnameLabel->setName(strtext);
	}

	swprintf_s(strtext, MAX_PATH, L"motion:%s", m_pcpinfo->motionname);
	if (m_motionnameLabel) {
		m_motionnameLabel->setName(strtext);
	}

	swprintf_s(strtext, MAX_PATH, L"StartFrame:%lf", m_pcpinfo->startframe);
	if (m_startframeLabel) {
		m_startframeLabel->setName(strtext);
	}

	swprintf_s(strtext, MAX_PATH, L"FrameNum:%lf", m_pcpinfo->framenum);
	if (m_framenumLabel) {
		m_framenumLabel->setName(strtext);
	}

	if (m_bvhCombo) {
		m_bvhCombo->setSelectedCombo(m_pcpinfo->bvhtype);
	}

	if (m_importanceCombo) {
		m_importanceCombo->setSelectedCombo(m_pcpinfo->importance);
	}


	return 0;
}

int CCpInfoDlg2::SetCpInfo(CPMOTINFO* srcpcpinfo)
{
	if (!srcpcpinfo) {
		return 1;
	}
	m_pcpinfo = srcpcpinfo;

	return 0;

}

int CCpInfoDlg2::DestroyOWPWnd()
{
	if (m_dlgspall) {
		delete m_dlgspall;
		m_dlgspall = nullptr;
	}
	if (m_fbxnameLabel) {
		delete m_fbxnameLabel;
		m_fbxnameLabel = nullptr;
	}
	if (m_motionnameLabel) {
		delete m_motionnameLabel;
		m_motionnameLabel = nullptr;
	}
	if (m_startframeLabel) {
		delete m_startframeLabel;
		m_startframeLabel = nullptr;
	}
	if (m_framenumLabel) {
		delete m_framenumLabel;
		m_framenumLabel = nullptr;
	}
	if (m_combosp) {
		delete m_combosp;
		m_combosp = nullptr;
	}
	if (m_bvhCombo) {
		delete m_bvhCombo;
		m_bvhCombo = nullptr;
	}
	if (m_importanceCombo) {
		delete m_importanceCombo;
		m_importanceCombo = nullptr;
	}
	if (m_memoEdit) {
		delete m_memoEdit;
		m_memoEdit = nullptr;
	}
	if (m_spacerLabel) {
		delete m_spacerLabel;
		m_spacerLabel = nullptr;
	}
	if (m_btnsp) {
		delete m_btnsp;
		m_btnsp = nullptr;
	}
	if (m_okB) {
		delete m_okB;
		m_okB = nullptr;
	}
	if (m_cancelB) {
		delete m_cancelB;
		m_cancelB = nullptr;
	}

	if (m_dlgwnd) {
		delete m_dlgwnd;
		m_dlgwnd = nullptr;
	}

	return 0;
}
int CCpInfoDlg2::CreateOWPWnd(POINT setpoint)
{
	DestroyOWPWnd();

	m_dlgwnd = new OrgWindow(
		1,
		_T("CpInfoDlg2"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(setpoint.x, setpoint.y),
		WindowSize(600, 400),		//サイズ
		_T("CpInfoDlg2"),	//タイトル
		//g_mainhwnd,	//親ウィンドウハンドル
		NULL,
		//false,					//表示・非表示状態
		true,
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否

	int labelheight;
	//if (g_4kresolution) {
		labelheight = 28;
	//}
	//else {
	//	labelheight = 20;
	//}

	if (m_dlgwnd) {

		////モーダル化
		//SetWindowLong(m_dlgwnd->getHWnd(), GWL_EXSTYLE, WS_EX_DLGMODALFRAME);
		//SetWindowLong(m_dlgwnd->getHWnd(), GWL_STYLE, WS_CAPTION);

		m_dlgwnd->setSizeMin(WindowSize(150, 150));
		m_dlgwnd->setSize(WindowSize(600, 260));
		m_dlgwnd->setPos(WindowPos(setpoint.x, setpoint.y));

		double rate50 = 0.50;

		m_dlgspall = new OWP_Separator(m_dlgwnd, true, 0.995, false);
		if (!m_dlgspall) {
			_ASSERT(0);
			abort();
		}
		m_dlgwnd->addParts(*m_dlgspall);

		m_fbxnameLabel = new OWP_Label(L"fbxname", labelheight, MAX_PATH);
		if (!m_fbxnameLabel) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_fbxnameLabel);

		m_motionnameLabel = new OWP_Label(L"motionname", labelheight, MAX_PATH);
		if (!m_motionnameLabel) {
			m_startframeLabel;
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_motionnameLabel);

		m_startframeLabel = new OWP_Label(L"startframe", labelheight, MAX_PATH);
		if (!m_startframeLabel) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_startframeLabel);

		m_framenumLabel = new OWP_Label(L"framenum", labelheight, MAX_PATH);
		if (!m_framenumLabel) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_framenumLabel);

		m_combosp = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_combosp) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_combosp);

		m_bvhCombo = new OWP_ComboBoxA(L"bvhcombo", labelheight);
		if (!m_bvhCombo) {
			_ASSERT(0);
			abort();
		}
		m_bvhCombo->addString("UnDef");
		char tempchar[MAX_PATH];
		int ino;
		for (ino = 1; ino <= 144; ino++) {
			ZeroMemory(tempchar, sizeof(char) * MAX_PATH);
			sprintf_s(tempchar, MAX_PATH, "bvh_%03d", ino);
			m_bvhCombo->addString(tempchar);
		}
		m_bvhCombo->setSelectedCombo(0);
		m_combosp->addParts1(*m_bvhCombo);

		m_importanceCombo = new OWP_ComboBoxA(L"importancecombo", labelheight);
		if (!m_importanceCombo) {
			_ASSERT(0);
			abort();
		}
		m_importanceCombo->addString("UnDef.");
		m_importanceCombo->addString("tiny.");
		m_importanceCombo->addString("ALittle.");
		m_importanceCombo->addString("Normal.");
		m_importanceCombo->addString("Noticed.");
		m_importanceCombo->addString("Important.");
		m_importanceCombo->addString("VeryImportant.");
		m_importanceCombo->setSelectedCombo(0);
		m_combosp->addParts2(*m_importanceCombo);

		m_memoEdit = new OWP_EditBox(false, L"memo", labelheight, HISTORYCOMMENTLEN);
		if (!m_memoEdit) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_memoEdit);

		m_spacerLabel = new OWP_Label(L"     ", labelheight, MAX_PATH);
		if (!m_spacerLabel) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_spacerLabel);

		m_btnsp = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_btnsp) {
			_ASSERT(0);
			abort();
		}
		m_dlgspall->addParts1(*m_btnsp);

		m_okB = new OWP_Button(L"OK", labelheight);
		if (!m_okB) {
			_ASSERT(0);
			abort();
		}
		m_btnsp->addParts1(*m_okB);

		m_cancelB = new OWP_Button(L"Cancel", labelheight);
		if (!m_cancelB) {
			_ASSERT(0);
			abort();
		}
		m_btnsp->addParts2(*m_cancelB);



		m_bvhCombo->setButtonListener([=, this]() {
			int combono = m_bvhCombo->trackPopUpMenu();
		});
		m_importanceCombo->setButtonListener([=, this]() {
			int combono = m_importanceCombo->trackPopUpMenu();
		});
		m_okB->setButtonListener([=, this]() {
			EnableWindow(g_mainhwnd, TRUE);//!!!!!!!!!!!! 疑似モーダル解除
			OnOK();
			::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + 97), (LPARAM)m_dlgresult);
		});
		m_cancelB->setButtonListener([=, this]() {
			EnableWindow(g_mainhwnd, TRUE);//!!!!!!!!!!!! 疑似モーダル解除
			OnCancel();
			::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + 97), (LPARAM)m_dlgresult);
		});
		m_dlgwnd->setCloseListener([]() {
			EnableWindow(g_mainhwnd, TRUE);//!!!!!!!!!!!! 疑似モーダル解除
		});


		m_dlgwnd->setSize(WindowSize(600, 260));
		m_dlgwnd->setPos(WindowPos(setpoint.x, setpoint.y));

		//１クリック目問題対応
		m_dlgwnd->refreshPosAndSize();

		m_dlgwnd->callRewrite();

	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}


