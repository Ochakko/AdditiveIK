#include "stdafx.h"

#include <DofDlg.h>
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
// CDofDlg

CDofDlg::CDofDlg()
{
	InitParams();
}

CDofDlg::~CDofDlg()
{
	DestroyObjs();
}
	
int CDofDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_dofLabel) {
		delete m_dofLabel;
		m_dofLabel = nullptr;
	}
	if (m_dofslotCombo) {
		delete m_dofslotCombo;
		m_dofslotCombo = nullptr;
	}
	if (m_dofspacerLabel1) {
		delete m_dofspacerLabel1;
		m_dofspacerLabel1 = nullptr;
	}
	if (m_dofdistsp1) {
		delete m_dofdistsp1;
		m_dofdistsp1 = nullptr;
	}
	if (m_dofdistsp2) {
		delete m_dofdistsp2;
		m_dofdistsp2 = nullptr;
	}
	if (m_dofdistsp3) {
		delete m_dofdistsp3;
		m_dofdistsp3 = nullptr;
	}
	if (m_dofdistnearLabel) {
		delete m_dofdistnearLabel;
		m_dofdistnearLabel = nullptr;
	}
	if (m_dofdistnearEdit) {
		delete m_dofdistnearEdit;
		m_dofdistnearEdit = nullptr;
	}
	if (m_dofdistfarLabel) {
		delete m_dofdistfarLabel;
		m_dofdistfarLabel = nullptr;
	}
	if (m_dofdistfarEdit) {
		delete m_dofdistfarEdit;
		m_dofdistfarEdit = nullptr;
	}
	if (m_dofspacerLabel2) {
		delete m_dofspacerLabel2;
		m_dofspacerLabel2 = nullptr;
	}
	if (m_dofskyChk) {
		delete m_dofskyChk;
		m_dofskyChk = nullptr;
	}
	//if (m_dofspacerLabel3) {
	//	delete m_dofspacerLabel3;
	//	m_dofspacerLabel3 = nullptr;
	//}
	//if (m_dofapplysp) {
	//	delete m_dofapplysp;
	//	m_dofapplysp = nullptr;
	//}
	//if (m_dofapplyB) {
	//	delete m_dofapplyB;
	//	m_dofapplyB = nullptr;
	//}



	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CDofDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_dlgWnd = nullptr;

	m_dofLabel = nullptr;
	m_dofslotCombo = nullptr;
	m_dofspacerLabel1 = nullptr;
	m_dofdistsp1 = nullptr;
	m_dofdistsp2 = nullptr;
	m_dofdistsp3 = nullptr;
	m_dofdistnearLabel = nullptr;
	m_dofdistnearEdit = nullptr;
	m_dofdistfarLabel = nullptr;
	m_dofdistfarEdit = nullptr;
	m_dofspacerLabel2 = nullptr;
	m_dofskyChk = nullptr;
	//m_dofspacerLabel3 = nullptr;
	//m_dofapplysp = nullptr;
	//m_dofapplyB = nullptr;

}

int CDofDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CDofDlg::SetFunctions(
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


//void CDofDlg::SetModel(CModel* srcmodel, int srccurboneno, std::unordered_map<CModel*, int> srcrgdindexmap)
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


void CDofDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateDofWnd();//作成済は０リターン

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


int CDofDlg::CreateDofWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("DOF_Dlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("DOF_Dlg"),	//タイトル
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

		m_dofLabel = new OWP_Label(L"被写界深度 Depth of Field(DOF)", labelheight);
		if (!m_dofLabel) {
			_ASSERT(0);
			abort();
		}
		m_dofslotCombo = new OWP_ComboBoxA(L"DOF_Combo", labelheight);//g_dofindex
		if (!m_dofslotCombo) {
			_ASSERT(0);
			abort();
		}
		int slotindex;
		for (slotindex = 0; slotindex < 8; slotindex++) {
			char strslot[128] = { 0 };
			sprintf_s(strslot, 128, "Slot_%d", slotindex);
			m_dofslotCombo->addString(strslot);
		}
		m_dofslotCombo->setSelectedCombo(g_dofindex);
		m_dofspacerLabel1 = new OWP_Label(L"     ", 32);
		if (!m_dofspacerLabel1) {
			_ASSERT(0);
			abort();
		}
		m_dofdistsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dofdistsp1) {
			_ASSERT(0);
			abort();
		}
		m_dofdistsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dofdistsp2) {
			_ASSERT(0);
			abort();
		}
		m_dofdistsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dofdistsp3) {
			_ASSERT(0);
			abort();
		}
		m_dofdistnearLabel = new OWP_Label(L"near", labelheight);
		if (!m_dofdistnearLabel) {
			_ASSERT(0);
			abort();
		}
		m_dofdistnearEdit = new OWP_EditBox(true, L"DofNearEdit", labelheight, EDIT_BUFLEN_NUM);//g_dofparams[g_dofindex].x
		if (!m_dofdistnearEdit) {
			_ASSERT(0);
			abort();
		}
		m_dofdistfarLabel = new OWP_Label(L"far", labelheight);//g_dofparams[g_dofindex].y
		if (!m_dofdistfarLabel) {
			_ASSERT(0);
			abort();
		}
		m_dofdistfarEdit = new OWP_EditBox(true, L"DofFarEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_dofdistfarEdit) {
			_ASSERT(0);
			abort();
		}
		m_dofspacerLabel2 = new OWP_Label(L"     ", 32);
		if (!m_dofspacerLabel2) {
			_ASSERT(0);
			abort();
		}
		m_dofskyChk = new OWP_CheckBoxA(L"Sky DOF", g_skydofflag[g_dofindex], labelheight, false);
		if (!m_dofskyChk) {
			_ASSERT(0);
			abort();
		}
		//m_dofspacerLabel3 = new OWP_Label(L"     ", 32);
		//if (!m_dofspacerLabel3) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_dofapplysp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		//if (!m_dofapplysp) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_dofapplyB = new OWP_Button(L"Apply(適用)", 38);
		//if (!m_dofapplyB) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_dofapplyB->setTextColor(RGB(168, 129, 129));


		m_dlgWnd->addParts(*m_dofLabel);
		m_dlgWnd->addParts(*m_dofslotCombo);
		m_dlgWnd->addParts(*m_dofspacerLabel1);
		m_dlgWnd->addParts(*m_dofdistsp1);
		m_dofdistsp1->addParts1(*m_dofdistsp2);
		m_dofdistsp1->addParts2(*m_dofdistsp3);
		m_dofdistsp2->addParts1(*m_dofdistnearLabel);
		m_dofdistsp2->addParts2(*m_dofdistnearEdit);
		m_dofdistsp3->addParts1(*m_dofdistfarLabel);
		m_dofdistsp3->addParts2(*m_dofdistfarEdit);
		m_dlgWnd->addParts(*m_dofspacerLabel2);
		m_dlgWnd->addParts(*m_dofskyChk);
		//m_dlgWnd->addParts(*m_dofspacerLabel3);
		//m_dlgWnd->addParts(*m_dofapplysp);
		//m_dofapplysp->addParts2(*m_dofapplyB);


		m_dofslotCombo->setButtonListener([=, this]() {
			int comboid = m_dofslotCombo->trackPopUpMenu();
			g_dofindex = comboid;

			ParamsToDlg();
			});

		m_dofskyChk->setButtonListener([=, this]() {
			bool value = m_dofskyChk->getValue();
			g_skydofflag[g_dofindex] = value;
			});

		//########
		//EditBox
		//########
		m_dofdistnearEdit->setExitDialogListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			float tempeditvalue;
			m_dofdistnearEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_dofparams[g_dofindex].x = tempeditvalue;
			}
			else {
				//if (m_dlgWnd) {
				//	::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : Near", L"Invalid Value", MB_OK);
				//}
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_dofparams[g_dofindex].x);
				m_dofdistnearEdit->setName(strnotchange);
			}
		});

		m_dofdistfarEdit->setExitDialogListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			float tempeditvalue;
			m_dofdistfarEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_dofparams[g_dofindex].y = tempeditvalue;
			}
			else {
				//if (m_dlgWnd) {
				//	::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : Far", L"Invalid Value", MB_OK);
				//}
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_dofparams[g_dofindex].y);
				m_dofdistfarEdit->setName(strnotchange);
			}
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

int CDofDlg::ParamsToDlg()
{
	if (!m_dlgWnd) {
		_ASSERT(0);
		return 1;
	}

	if ((g_dofindex < 0) || (g_dofindex >= DOFSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}

	//#########
	//ComboBox
	//#########
	if (m_dofslotCombo) {
		m_dofslotCombo->setSelectedCombo(g_dofindex);
	}

	//#####
	//Text
	//#####
	WCHAR strdlg[256] = { 0L };
	swprintf_s(strdlg, 256, L"%.2f", g_dofparams[g_dofindex].x);
	if (m_dofdistnearEdit) {
		m_dofdistnearEdit->setName(strdlg);
	}

	swprintf_s(strdlg, 256, L"%.2f", g_dofparams[g_dofindex].y);
	if (m_dofdistfarEdit) {
		m_dofdistfarEdit->setName(strdlg);
	}


	//#########
	//CheckBox
	//#########
	if (m_dofskyChk) {
		m_dofskyChk->setValue(g_skydofflag[g_dofindex], false);
	}


	m_dlgWnd->callRewrite();

	return 0;
}

//int CDofDlg::DlgToParams()
//{
//}


const HWND CDofDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CDofDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CDofDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

