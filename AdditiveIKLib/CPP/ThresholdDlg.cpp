#include "stdafx.h"

#include <ThresholdDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <mqoobject.h>
#include <mqomaterial.h>
#include <Bone.h>
#include <RigidElem.h>
#include <BtObject.h>
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
// CThresholdDlg

CThresholdDlg::CThresholdDlg()
{
	InitParams();
}

CThresholdDlg::~CThresholdDlg()
{
	DestroyObjs();
}
	
int CThresholdDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_ththLabel) {
		delete m_ththLabel;
		m_ththLabel = nullptr;
	}
	if (m_ththsp1) {
		delete m_ththsp1;
		m_ththsp1 = nullptr;
	}
	if (m_ththsp2) {
		delete m_ththsp2;
		m_ththsp2 = nullptr;
	}
	if (m_ththsp3) {
		delete m_ththsp3;
		m_ththsp3 = nullptr;
	}
	if (m_ththnoendjLabel) {
		delete m_ththnoendjLabel;
		m_ththnoendjLabel = nullptr;
	}
	if (m_ththnoendjEdit) {
		delete m_ththnoendjEdit;
		m_ththnoendjEdit = nullptr;
	}
	if (m_ththendjLabel) {
		delete m_ththendjLabel;
		m_ththendjLabel = nullptr;
	}
	if (m_ththendjEdit) {
		delete m_ththendjEdit;
		m_ththendjEdit = nullptr;
	}
	if (m_throundLabel) {
		delete m_throundLabel;
		m_throundLabel = nullptr;
	}
	if (m_throundsp1) {
		delete m_throundsp1;
		m_throundsp1 = nullptr;
	}
	if (m_throundsp2) {
		delete m_throundsp2;
		m_throundsp2 = nullptr;
	}
	if (m_throundsp2a) {
		delete m_throundsp2a;
		m_throundsp2a = nullptr;
	}
	if (m_throundsp2b) {
		delete m_throundsp2b;
		m_throundsp2b = nullptr;
	}
	if (m_throundsp3) {
		delete m_throundsp3;
		m_throundsp3 = nullptr;
	}
	if (m_throundsp3a) {
		delete m_throundsp3a;
		m_throundsp3a = nullptr;
	}
	if (m_throundxLabel) {
		delete m_throundxLabel;
		m_throundxLabel = nullptr;
	}
	if (m_throundxEdit) {
		delete m_throundxEdit;
		m_throundxEdit = nullptr;
	}
	if (m_throundyLabel) {
		delete m_throundyLabel;
		m_throundyLabel = nullptr;
	}
	if (m_throundyEdit) {
		delete m_throundyEdit;
		m_throundyEdit = nullptr;
	}
	if (m_throundzLabel) {
		delete m_throundzLabel;
		m_throundzLabel = nullptr;
	}
	if (m_throundzEdit) {
		delete m_throundzEdit;
		m_throundzEdit = nullptr;
	}
	if (m_thspacerLabel) {
		delete m_thspacerLabel;
		m_thspacerLabel = nullptr;
	}
	if (m_thspacerLabel1) {
		delete m_thspacerLabel1;
		m_thspacerLabel1 = nullptr;
	}
	if (m_thspacerLabel2) {
		delete m_thspacerLabel2;
		m_thspacerLabel2 = nullptr;
	}
	if (m_thapplysp1) {
		delete m_thapplysp1;
		m_thapplysp1 = nullptr;
	}
	if (m_thapplysp2) {
		delete m_thapplysp2;
		m_thapplysp2 = nullptr;
	}
	//if (m_thapplyB) {
	//	delete m_thapplyB;
	//	m_thapplyB = nullptr;
	//}
	if (m_thdefaultB) {
		delete m_thdefaultB;
		m_thdefaultB = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CThresholdDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_closeFlag = false;


	m_dlgWnd = nullptr;

	m_ththLabel = nullptr;
	m_ththsp1 = nullptr;
	m_ththsp2 = nullptr;
	m_ththsp3 = nullptr;
	m_ththnoendjLabel = nullptr;
	m_ththnoendjEdit = nullptr;
	m_ththendjLabel = nullptr;
	m_ththendjEdit = nullptr;
	m_throundLabel = nullptr;
	m_throundsp1 = nullptr;
	m_throundsp2 = nullptr;
	m_throundsp2a = nullptr;
	m_throundsp2b = nullptr;
	m_throundsp3 = nullptr;
	m_throundsp3a = nullptr;
	m_throundxLabel = nullptr;
	m_throundxEdit = nullptr;
	m_throundyLabel = nullptr;
	m_throundyEdit = nullptr;
	m_throundzLabel = nullptr;
	m_throundzEdit = nullptr;
	m_thspacerLabel = nullptr;
	m_thspacerLabel1 = nullptr;
	m_thspacerLabel2 = nullptr;
	m_thapplysp1 = nullptr;
	m_thapplysp2 = nullptr;
	//m_thapplyB = nullptr;
	m_thdefaultB = nullptr;

}

int CThresholdDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CThresholdDlg::SetFunctions(
//	void (*srcSetCamera3DFromEyePos)()
//)
//{
//	if (!srcSetCamera3DFromEyePos) {
//		_ASSERT(0);
//		return 1;
//	}
//
//	m_SetCamera3DFromEyePos = srcSetCamera3DFromEyePos;
//
//	return 0;
//}


//void CThresholdDlg::SetModel(CModel* srcmodel, int srccurboneno, std::unordered_map<CModel*, int> srcrgdindexmap)
//{
//	m_model = srcmodel;
//	m_curboneno = srccurboneno;
//	m_rgdindexmap = srcrgdindexmap;
//
//	CreateThresholdWnd();
//	ParamsToDlg();
//
//	if (m_dlgWnd && m_dlgWnd->getVisible()) {
//		//if (m_skyst_Sc) {
//		//	m_skyst_Sc->autoResize();
//		//}
//		m_dlgWnd->callRewrite();
//	}
//}


void CThresholdDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateThresholdWnd();//作成済は０リターン

		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			
			ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			//if (m_skyst_Sc) {
			//	//############
			//	//2024/07/24
			//	//############
			//	//int showposline = m_dlgSc->getShowPosLine();
			//	//m_dlgSc->setShowPosLine(showposline);
			//	//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
			//	//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
			//	//スクロールバーを少し動かすと全て表示された
			//	//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
			//	m_skyst_Sc->autoResize();
			//}
			m_dlgWnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgWnd) {
			m_dlgWnd->setVisible(false);
			//m_dlgWnd->setListenMouse(false);//<--- CloseAllRightPainWindow()を呼んだ時にwindowが反応しなくなるのはこの呼び出しが原因だった
		}
	}
	m_visible = srcflag;
}


int CThresholdDlg::CreateThresholdWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("ThresholdDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("ThresholdDlg"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		false,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否

	int labelheight;
	if (g_4kresolution) {
		labelheight = 28;
	}
	else {
		labelheight = 20;
	}

	if (m_dlgWnd) {
		double rate50 = 0.50;

		m_ththLabel = new OWP_Label(L"Threshold Degree", labelheight);
		if (!m_ththLabel) {
			_ASSERT(0);
			abort();
		}
		m_ththsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_ththsp1) {
			_ASSERT(0);
			abort();
		}
		m_ththsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_ththsp2) {
			_ASSERT(0);
			abort();
		}
		m_ththsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_ththsp3) {
			_ASSERT(0);
			abort();
		}
		m_ththnoendjLabel = new OWP_Label(L"NoEndJoint", labelheight);
		if (!m_ththnoendjLabel) {
			_ASSERT(0);
			abort();
		}
		m_ththnoendjEdit = new OWP_EditBox(true, L"NoEndJ Edit", labelheight, EDIT_BUFLEN_NUM);//g_thdeg
		if (!m_ththnoendjEdit) {
			_ASSERT(0);
			abort();
		}
		m_ththendjLabel = new OWP_Label(L"EndJoint", labelheight);
		if (!m_ththendjLabel) {
			_ASSERT(0);
			abort();
		}
		m_ththendjEdit = new OWP_EditBox(true, L"EndJ Edit", labelheight, EDIT_BUFLEN_NUM);//g_thdeg_endjoint
		if (!m_ththendjEdit) {
			_ASSERT(0);
			abort();
		}
		m_throundLabel = new OWP_Label(L"Rounding Degree", labelheight);
		if (!m_throundLabel) {
			_ASSERT(0);
			abort();
		}
		m_throundsp1 = new OWP_Separator(m_dlgWnd, true, 0.667, true);
		if (!m_throundsp1) {
			_ASSERT(0);
			abort();
		}
		m_throundsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_throundsp2) {
			_ASSERT(0);
			abort();
		}
		m_throundsp2a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_throundsp2a) {
			_ASSERT(0);
			abort();
		}
		m_throundsp2b = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_throundsp2b) {
			_ASSERT(0);
			abort();
		}
		m_throundsp3 = new OWP_Separator(m_dlgWnd, true, 0.95, true);
		if (!m_throundsp3) {
			_ASSERT(0);
			abort();
		}
		m_throundsp3a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_throundsp3a) {
			_ASSERT(0);
			abort();
		}
		m_throundxLabel = new OWP_Label(L"X Round", labelheight);
		if (!m_throundxLabel) {
			_ASSERT(0);
			abort();
		}
		m_throundxEdit = new OWP_EditBox(true, L"XEdit", labelheight, EDIT_BUFLEN_NUM);//g_thRoundX
		if (!m_throundxEdit) {
			_ASSERT(0);
			abort();
		}
		m_throundyLabel = new OWP_Label(L"Y Round", labelheight);
		if (!m_throundyLabel) {
			_ASSERT(0);
			abort();
		}
		m_throundyEdit = new OWP_EditBox(true, L"YEdit", labelheight, EDIT_BUFLEN_NUM);//g_thRoundY
		if (!m_throundyEdit) {
			_ASSERT(0);
			abort();
		}
		m_throundzLabel = new OWP_Label(L"Z Round", labelheight);
		if (!m_throundzLabel) {
			_ASSERT(0);
			abort();
		}
		m_throundzEdit = new OWP_EditBox(true, L"ZEdit", labelheight, EDIT_BUFLEN_NUM);//g_thRoundZ
		if (!m_throundzEdit) {
			_ASSERT(0);
			abort();
		}
		m_thspacerLabel = new OWP_Label(L"     ", labelheight);
		if (!m_thspacerLabel) {
			_ASSERT(0);
			abort();
		}
		m_thspacerLabel1 = new OWP_Label(L"     ", labelheight);
		if (!m_thspacerLabel1) {
			_ASSERT(0);
			abort();
		}
		m_thspacerLabel2 = new OWP_Label(L"     ", labelheight);
		if (!m_thspacerLabel2) {
			_ASSERT(0);
			abort();
		}
		m_thapplysp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_thapplysp1) {
			_ASSERT(0);
			abort();
		}
		m_thapplysp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_thapplysp2) {
			_ASSERT(0);
			abort();
		}
		//m_thapplyB = new OWP_Button(L"Apply", 32);
		//if (!m_thapplyB) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_thapplyB->setTextColor(RGB(168, 129, 129));
		m_thdefaultB = new OWP_Button(L"Default", 32);
		if (!m_thdefaultB) {
			_ASSERT(0);
			abort();
		}
		m_thdefaultB->setTextColor(RGB(168, 129, 129));


		m_dlgWnd->addParts(*m_ththLabel);
		m_dlgWnd->addParts(*m_ththsp1);
		m_ththsp1->addParts1(*m_ththsp2);
		m_ththsp1->addParts2(*m_ththsp3);
		m_ththsp2->addParts1(*m_ththnoendjLabel);
		m_ththsp2->addParts2(*m_ththnoendjEdit);
		m_ththsp3->addParts1(*m_ththendjLabel);
		m_ththsp3->addParts2(*m_ththendjEdit);
		m_dlgWnd->addParts(*m_thspacerLabel1);
		m_dlgWnd->addParts(*m_thspacerLabel2);
		m_dlgWnd->addParts(*m_throundLabel);
		m_dlgWnd->addParts(*m_throundsp1);
		m_throundsp1->addParts1(*m_throundsp2);
		m_throundsp2->addParts1(*m_throundsp2a);
		m_throundsp2->addParts2(*m_throundsp2b);
		m_throundsp1->addParts2(*m_throundsp3);
		m_throundsp3->addParts1(*m_throundsp3a);
		m_throundsp2a->addParts1(*m_throundxLabel);
		m_throundsp2a->addParts2(*m_throundxEdit);
		m_throundsp2b->addParts1(*m_throundyLabel);
		m_throundsp2b->addParts2(*m_throundyEdit);
		m_throundsp3a->addParts1(*m_throundzLabel);
		m_throundsp3a->addParts2(*m_throundzEdit);
		m_dlgWnd->addParts(*m_thspacerLabel);
		m_dlgWnd->addParts(*m_thapplysp1);
		m_thapplysp1->addParts2(*m_thapplysp2);
		//m_thapplysp2->addParts1(*m_thapplyB);
		m_thapplysp2->addParts2(*m_thdefaultB);


		//########
		//EditBox
		//########
		DlgToParamsListener();


		//m_thapplyB->setButtonListener([=, this]() {
		//	//m_changelimitangleFlag = true;
		//	//PrepairUndo();//全フレーム変更の前に全フレーム保存

		//	DlgToParams();

		//	//PrepairUndo();//全フレーム変更後に全フレーム保存
		//	//m_changelimitangleFlag = false;
		//	});
		m_thdefaultB->setButtonListener([=, this]() {
			g_thdeg = 181.0f;
			g_thdeg_endjoint = 159.0f;
			g_thRoundX = 179.0f;
			g_thRoundY = 179.0f;
			g_thRoundZ = 179.0f;

			ParamsToDlg();
			});


		m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgWnd->setPos(WindowPos(m_posx, m_posy));

		//１クリック目問題対応
		m_dlgWnd->refreshPosAndSize();

		m_dlgWnd->callRewrite();
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CThresholdDlg::ParamsToDlg()
{
	if (m_dlgWnd != 0) {
		WCHAR stredit[256] = { 0L };

		swprintf_s(stredit, 256, L"%d", Float2Int(g_thdeg_endjoint));
		if (m_ththendjEdit) {
			m_ththendjEdit->setName(stredit);
		}
		swprintf_s(stredit, 256, L"%d", Float2Int(g_thdeg));
		if (m_ththnoendjEdit) {
			m_ththnoendjEdit->setName(stredit);
		}


		swprintf_s(stredit, 256, L"%d", Float2Int(g_thRoundX));
		if (m_throundxEdit) {
			m_throundxEdit->setName(stredit);
		}
		swprintf_s(stredit, 256, L"%d", Float2Int(g_thRoundY));
		if (m_throundyEdit) {
			m_throundyEdit->setName(stredit);
		}
		swprintf_s(stredit, 256, L"%d", Float2Int(g_thRoundZ));
		if (m_throundzEdit) {
			m_throundzEdit->setName(stredit);
		}

		m_dlgWnd->callRewrite();
	}
	else {
		_ASSERT(0);
	}

	return 0;
}

int CThresholdDlg::DlgToParamsListener()
{
	if (m_ththnoendjEdit) {
		m_ththnoendjEdit->setExitDialogListener([=, this]() {
			int val_thdeg = Float2Int(g_thdeg);
			int result_thdeg = GetThresholdEditIntOWP(m_ththnoendjEdit, &val_thdeg);
			if (result_thdeg != 0) {
				//::MessageBox(g_mainhwnd, L"ThresholdDlgのNotEndJointの入力値が不正です。", L"入力し直してください。", MB_OK);
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_thdeg);
				m_ththnoendjEdit->setName(strnotchange);
			}
			else {
				g_thdeg = (float)val_thdeg;
			}
		});
	}
	if (m_ththendjEdit) {
		m_ththendjEdit->setExitDialogListener([=, this]() {
			int val_thdeg_endjoint = Float2Int(g_thdeg_endjoint);
			int result_thdeg_endjoint = GetThresholdEditIntOWP(m_ththendjEdit, &val_thdeg_endjoint);
			if (result_thdeg_endjoint != 0) {
				//::MessageBox(g_mainhwnd, L"ThresholdDlgのEndJointの入力値が不正です。", L"入力し直してください。", MB_OK);
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_thdeg_endjoint);
				m_ththendjEdit->setName(strnotchange);
			}
			else {
				g_thdeg_endjoint = (float)val_thdeg_endjoint;
			}
		});
	}
	if (m_throundxEdit) {
		m_throundxEdit->setExitDialogListener([=, this]() {
			int val_xround = Float2Int(g_thRoundX);
			int result_xround = GetThresholdEditIntOWP(m_throundxEdit, &val_xround);
			if (result_xround != 0) {
				//::MessageBox(g_mainhwnd, L"ThresholdDlgのXRoundの入力値が不正です。", L"入力し直してください。", MB_OK);
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_thRoundX);
				m_throundxEdit->setName(strnotchange);
			}
			else {
				g_thRoundX = (float)val_xround;
			}
		});
	}
	if (m_throundyEdit) {
		m_throundyEdit->setExitDialogListener([=, this]() {
			int val_yround = Float2Int(g_thRoundY);
			int result_yround = GetThresholdEditIntOWP(m_throundyEdit, &val_yround);
			if (result_yround != 0) {
				//::MessageBox(g_mainhwnd, L"ThresholdDlgのYRoundの入力値が不正です。", L"入力し直してください。", MB_OK);
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_thRoundY);
				m_throundyEdit->setName(strnotchange);
			}
			else {
				g_thRoundY = (float)val_yround;
			}
		});
	}
	if (m_throundzEdit) {
		m_throundzEdit->setExitDialogListener([=, this]() {
			int val_zround = Float2Int(g_thRoundZ);
			int result_zround = GetThresholdEditIntOWP(m_throundzEdit, &val_zround);
			if (result_zround != 0) {
				//::MessageBox(g_mainhwnd, L"ThresholdDlgのZRoundの入力値が不正です。", L"入力し直してください。", MB_OK);
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_thRoundZ);
				m_throundzEdit->setName(strnotchange);
			}
			else {
				g_thRoundZ = (float)val_zround;
			}
		});
	}

	return 0;
}

int CThresholdDlg::GetThresholdEditIntOWP(OWP_EditBox* srcedit, int* dstlimit)
{
	if (!srcedit || !dstlimit) {
		_ASSERT(0);
		return 1;
	}

	WCHAR stredit[THDLGEDITLEN] = { 0L };
	::ZeroMemory(stredit, sizeof(WCHAR) * THDLGEDITLEN);
	srcedit->getName(stredit, THDLGEDITLEN);

	stredit[THDLGEDITLEN - 1] = 0L;
	int result1;
	result1 = CheckStr_SInt(stredit);
	if (result1 == 0) {
		stredit[THDLGEDITLEN - 1] = 0L;

		SetLastError(0);
		int tmpint = _wtoi(stredit);
		DWORD dwresult = GetLastError();
		if (dwresult == 0) {
			*dstlimit = tmpint;
			return 0;
		}
		else {
			_ASSERT(0);
			*dstlimit = 0;
			return 1;
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}
}
int CThresholdDlg::CheckStr_SInt(const WCHAR* srcstr)
{
	if (!srcstr) {
		return 1;
	}
	size_t strleng = wcslen(srcstr);
	if ((strleng <= 0) || (strleng >= THDLGEDITLEN)) {
		_ASSERT(0);
		return 1;
	}

	bool errorflag = false;
	size_t strindex;
	for (strindex = 0; strindex < strleng; strindex++) {
		WCHAR curwc = *(srcstr + strindex);
		if (((curwc >= TEXT('0')) && (curwc <= TEXT('9'))) || (curwc == TEXT('+')) || (curwc == TEXT('-'))) {

		}
		else {
			errorflag = true;
			break;
		}
	}

	if (errorflag == false) {
		return 0;
	}
	else {
		return 1;
	}
}


const HWND CThresholdDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CThresholdDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CThresholdDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

