#include "stdafx.h"

#include <LimitEulDlg.h>
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
// CLimitEulDlg

CLimitEulDlg::CLimitEulDlg()
{
	InitParams();
}

CLimitEulDlg::~CLimitEulDlg()
{
	DestroyObjs();
}
	
int CLimitEulDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_limitnameLabel) {
		delete m_limitnameLabel;
		m_limitnameLabel = nullptr;
	}
	if (m_limitxsp1) {
		delete m_limitxsp1;
		m_limitxsp1 = nullptr;
	}
	if (m_limitxsp2) {
		delete m_limitxsp2;
		m_limitxsp2 = nullptr;
	}
	if (m_limitxsp2a) {
		delete m_limitxsp2a;
		m_limitxsp2a = nullptr;
	}
	if (m_limitxsp2b) {
		delete m_limitxsp2b;
		m_limitxsp2b = nullptr;
	}
	if (m_limitxsp3) {
		delete m_limitxsp3;
		m_limitxsp3 = nullptr;
	}
	if (m_limitxsp3a) {
		delete m_limitxsp3a;
		m_limitxsp3a = nullptr;
	}
	if (m_limitxupperLabel) {
		delete m_limitxupperLabel;
		m_limitxupperLabel = nullptr;
	}
	if (m_limitxupperEdit) {
		delete m_limitxupperEdit;
		m_limitxupperEdit = nullptr;
	}
	if (m_limitxlowerLabel) {
		delete m_limitxlowerLabel;
		m_limitxlowerLabel = nullptr;
	}
	if (m_limitxlowerEdit) {
		delete m_limitxlowerEdit;
		m_limitxlowerEdit = nullptr;
	}
	if (m_limitxchkLabel) {
		delete m_limitxchkLabel;
		m_limitxchkLabel = nullptr;
	}
	if (m_limitxchkEdit) {
		delete m_limitxchkEdit;
		m_limitxchkEdit = nullptr;
	}
	if (m_limitysp1) {
		delete m_limitysp1;
		m_limitysp1 = nullptr;
	}
	if (m_limitysp2) {
		delete m_limitysp2;
		m_limitysp2 = nullptr;
	}
	if (m_limitysp2a) {
		delete m_limitysp2a;
		m_limitysp2a = nullptr;
	}
	if (m_limitysp2b) {
		delete m_limitysp2b;
		m_limitysp2b = nullptr;
	}
	if (m_limitysp3) {
		delete m_limitysp3;
		m_limitysp3 = nullptr;
	}
	if (m_limitysp3a) {
		delete m_limitysp3a;
		m_limitysp3a = nullptr;
	}
	if (m_limityupperLabel) {
		delete m_limityupperLabel;
		m_limityupperLabel = nullptr;
	}
	if (m_limityupperEdit) {
		delete m_limityupperEdit;
		m_limityupperEdit = nullptr;
	}
	if (m_limitylowerLabel) {
		delete m_limitylowerLabel;
		m_limitylowerLabel = nullptr;
	}
	if (m_limitylowerEdit) {
		delete m_limitylowerEdit;
		m_limitylowerEdit = nullptr;
	}
	if (m_limitychkLabel) {
		delete m_limitychkLabel;
		m_limitychkLabel = nullptr;
	}
	if (m_limitychkEdit) {
		delete m_limitychkEdit;
		m_limitychkEdit = nullptr;
	}
	if (m_limitzsp1) {
		delete m_limitzsp1;
		m_limitzsp1 = nullptr;
	}
	if (m_limitzsp2) {
		delete m_limitzsp2;
		m_limitzsp2 = nullptr;
	}
	if (m_limitzsp2a) {
		delete m_limitzsp2a;
		m_limitzsp2a = nullptr;
	}
	if (m_limitzsp2b) {
		delete m_limitzsp2b;
		m_limitzsp2b = nullptr;
	}
	if (m_limitzsp3) {
		delete m_limitzsp3;
		m_limitzsp3 = nullptr;
	}
	if (m_limitzsp3a) {
		delete m_limitzsp3a;
		m_limitzsp3a = nullptr;
	}
	if (m_limitzupperLabel) {
		delete m_limitzupperLabel;
		m_limitzupperLabel = nullptr;
	}
	if (m_limitzupperEdit) {
		delete m_limitzupperEdit;
		m_limitzupperEdit = nullptr;
	}
	if (m_limitzlowerLabel) {
		delete m_limitzlowerLabel;
		m_limitzlowerLabel = nullptr;
	}
	if (m_limitzlowerEdit) {
		delete m_limitzlowerEdit;
		m_limitzlowerEdit = nullptr;
	}
	if (m_limitzchkLabel) {
		delete m_limitzchkLabel;
		m_limitzchkLabel = nullptr;
	}
	if (m_limitzchkEdit) {
		delete m_limitzchkEdit;
		m_limitzchkEdit = nullptr;
	}
	if (m_limiteulsp1) {
		delete m_limiteulsp1;
		m_limiteulsp1 = nullptr;
	}
	if (m_limiteulsp2) {
		delete m_limiteulsp2;
		m_limiteulsp2 = nullptr;
	}
	if (m_limiteulsp3) {
		delete m_limiteulsp3;
		m_limiteulsp3 = nullptr;
	}
	if (m_limiteuloneB) {
		delete m_limiteuloneB;
		m_limiteuloneB = nullptr;
	}
	if (m_limiteuldeeperB) {
		delete m_limiteuldeeperB;
		m_limiteuldeeperB = nullptr;
	}
	if (m_limiteulallB) {
		delete m_limiteulallB;
		m_limiteulallB = nullptr;
	}
	if (m_limitcppsLabel) {
		delete m_limitcppsLabel;
		m_limitcppsLabel = nullptr;
	}
	if (m_limitcpsp1) {
		delete m_limitcpsp1;
		m_limitcpsp1 = nullptr;
	}
	if (m_limitcpsp2) {
		delete m_limitcpsp2;
		m_limitcpsp2 = nullptr;
	}
	if (m_limitcpsp3) {
		delete m_limitcpsp3;
		m_limitcpsp3 = nullptr;
	}
	if (m_limitcptosymB) {
		delete m_limitcptosymB;
		m_limitcptosymB = nullptr;
	}
	if (m_limitcpfromsymB) {
		delete m_limitcpfromsymB;
		m_limitcpfromsymB = nullptr;
	}
	if (m_limitcpB) {
		delete m_limitcpB;
		m_limitcpB = nullptr;
	}
	if (m_limitpsB) {
		delete m_limitpsB;
		m_limitpsB = nullptr;
	}
	if (m_limitsetLabel) {
		delete m_limitsetLabel;
		m_limitsetLabel = nullptr;
	}
	if (m_limitsetsp1) {
		delete m_limitsetsp1;
		m_limitsetsp1 = nullptr;
	}
	if (m_limitsetsp2) {
		delete m_limitsetsp2;
		m_limitsetsp2 = nullptr;
	}
	if (m_limitsetsp3) {
		delete m_limitsetsp3;
		m_limitsetsp3 = nullptr;
	}
	if (m_limitreset180B) {
		delete m_limitreset180B;
		m_limitreset180B = nullptr;
	}
	if (m_limit180to170B) {
		delete m_limit180to170B;
		m_limit180to170B = nullptr;
	}
	if (m_limitreset0B) {
		delete m_limitreset0B;
		m_limitreset0B = nullptr;
	}
	if (m_limitfrommotB) {
		delete m_limitfrommotB;
		m_limitfrommotB = nullptr;
	}
	if (m_limitsetallLabel) {
		delete m_limitsetallLabel;
		m_limitsetallLabel = nullptr;
	}
	if (m_limitsetallsp1) {
		delete m_limitsetallsp1;
		m_limitsetallsp1 = nullptr;
	}
	if (m_limitsetallsp2) {
		delete m_limitsetallsp2;
		m_limitsetallsp2 = nullptr;
	}
	if (m_limitsetallsp3) {
		delete m_limitsetallsp3;
		m_limitsetallsp3 = nullptr;
	}
	if (m_limitresetall180B) {
		delete m_limitresetall180B;
		m_limitresetall180B = nullptr;
	}
	if (m_limitall180to170B) {
		delete m_limitall180to170B;
		m_limitall180to170B = nullptr;
	}
	if (m_limitallreset0B) {
		delete m_limitallreset0B;
		m_limitallreset0B = nullptr;
	}
	if (m_limitallfrommotB) {
		delete m_limitallfrommotB;
		m_limitallfrommotB = nullptr;
	}
	if (m_limitallboneallmotLabel) {
		delete m_limitallboneallmotLabel;
		m_limitallboneallmotLabel = nullptr;
	}
	if (m_limitallboneallmotB) {
		delete m_limitallboneallmotB;
		m_limitallboneallmotB = nullptr;
	}
	if (m_limitphysicsLabel) {
		delete m_limitphysicsLabel;
		m_limitphysicsLabel = nullptr;
	}
	if (m_limitphysicssp) {
		delete m_limitphysicssp;
		m_limitphysicssp = nullptr;
	}
	if (m_limitphysicsrateLabel) {
		delete m_limitphysicsrateLabel;
		m_limitphysicsrateLabel = nullptr;
	}
	if (m_limitphysicsrateSlider) {
		delete m_limitphysicsrateSlider;
		m_limitphysicsrateSlider = nullptr;
	}
	if (m_limitphsysicssp1) {
		delete m_limitphsysicssp1;
		m_limitphsysicssp1 = nullptr;
	}
	if (m_limitphsysicssp2) {
		delete m_limitphsysicssp2;
		m_limitphsysicssp2 = nullptr;
	}
	if (m_limitphysicsDepperB) {
		delete m_limitphysicsDepperB;
		m_limitphysicsDepperB = nullptr;
	}
	if (m_limitphysicsAllB) {
		delete m_limitphysicsAllB;
		m_limitphysicsAllB = nullptr;
	}
	if (m_limitspacerLabel001) {
		delete m_limitspacerLabel001;
		m_limitspacerLabel001 = nullptr;
	}
	if (m_limitspacerLabel002) {
		delete m_limitspacerLabel002;
		m_limitspacerLabel002 = nullptr;
	}
	if (m_limitspacerLabel003) {
		delete m_limitspacerLabel003;
		m_limitspacerLabel003 = nullptr;
	}
	if (m_limitspacerLabel004) {
		delete m_limitspacerLabel004;
		m_limitspacerLabel004 = nullptr;
	}



	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CLimitEulDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_PrepairUndo = nullptr;
	m_UpdateAfterEditAngleLimit = nullptr;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;
	m_dispanglelimit = false;
	m_anglelimit.Init();
	m_anglelimitcopy.Init();
	m_anglelimitbone = nullptr;
	m_beflimitdegflag = g_limitdegflag;
	m_savelimitdegflag = g_limitdegflag;
	m_changelimitangleFlag = false;
	m_underanglelimithscroll = 0;

	m_dlgWnd = nullptr;

	m_limitnameLabel = nullptr;
	m_limitxsp1 = nullptr;
	m_limitxsp2 = nullptr;
	m_limitxsp2a = nullptr;
	m_limitxsp2b = nullptr;
	m_limitxsp3 = nullptr;
	m_limitxsp3a = nullptr;
	m_limitxupperLabel = nullptr;
	m_limitxupperEdit = nullptr;
	m_limitxlowerLabel = nullptr;
	m_limitxlowerEdit = nullptr;
	m_limitxchkLabel = nullptr;
	m_limitxchkEdit = nullptr;
	m_limitysp1 = nullptr;
	m_limitysp2 = nullptr;
	m_limitysp2a = nullptr;
	m_limitysp2b = nullptr;
	m_limitysp3 = nullptr;
	m_limitysp3a = nullptr;
	m_limityupperLabel = nullptr;
	m_limityupperEdit = nullptr;
	m_limitylowerLabel = nullptr;
	m_limitylowerEdit = nullptr;
	m_limitychkLabel = nullptr;
	m_limitychkEdit = nullptr;
	m_limitzsp1 = nullptr;
	m_limitzsp2 = nullptr;
	m_limitzsp2a = nullptr;
	m_limitzsp2b = nullptr;
	m_limitzsp3 = nullptr;
	m_limitzsp3a = nullptr;
	m_limitzupperLabel = nullptr;
	m_limitzupperEdit = nullptr;
	m_limitzlowerLabel = nullptr;
	m_limitzlowerEdit = nullptr;
	m_limitzchkLabel = nullptr;
	m_limitzchkEdit = nullptr;
	m_limiteulsp1 = nullptr;
	m_limiteulsp2 = nullptr;
	m_limiteulsp3 = nullptr;
	m_limiteuloneB = nullptr;
	m_limiteuldeeperB = nullptr;
	m_limiteulallB = nullptr;
	m_limitcppsLabel = nullptr;
	m_limitcpsp1 = nullptr;
	m_limitcpsp2 = nullptr;
	m_limitcpsp3 = nullptr;
	m_limitcptosymB = nullptr;
	m_limitcpfromsymB = nullptr;
	m_limitcpB = nullptr;
	m_limitpsB = nullptr;
	m_limitsetLabel = nullptr;
	m_limitsetsp1 = nullptr;
	m_limitsetsp2 = nullptr;
	m_limitsetsp3 = nullptr;
	m_limitreset180B = nullptr;
	m_limit180to170B = nullptr;
	m_limitreset0B = nullptr;
	m_limitfrommotB = nullptr;
	m_limitsetallLabel = nullptr;
	m_limitsetallsp1 = nullptr;
	m_limitsetallsp2 = nullptr;
	m_limitsetallsp3 = nullptr;
	m_limitresetall180B = nullptr;
	m_limitall180to170B = nullptr;
	m_limitallreset0B = nullptr;
	m_limitallfrommotB = nullptr;
	m_limitallboneallmotLabel = nullptr;
	m_limitallboneallmotB = nullptr;
	m_limitphysicsLabel = nullptr;
	m_limitphysicssp = nullptr;
	m_limitphysicsrateLabel = nullptr;
	m_limitphysicsrateSlider = nullptr;
	m_limitphsysicssp1 = nullptr;
	m_limitphsysicssp2 = nullptr;
	m_limitphysicsDepperB = nullptr;
	m_limitphysicsAllB = nullptr;
	m_limitspacerLabel001 = nullptr;
	m_limitspacerLabel002 = nullptr;
	m_limitspacerLabel003 = nullptr;
	m_limitspacerLabel004 = nullptr;

}

int CLimitEulDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

int CLimitEulDlg::SetFunctions(
	void (*srcPrepairUndo)(),
	int (*srcUpdateAfterEditAngleLimit)(int limit2boneflag, bool setcursorflag)
)
{
	if (!srcPrepairUndo || !srcUpdateAfterEditAngleLimit) {
		_ASSERT(0);
		return 1;
	}
	m_PrepairUndo = srcPrepairUndo;
	m_UpdateAfterEditAngleLimit = srcUpdateAfterEditAngleLimit;

	return 0;
}

void CLimitEulDlg::SetModel(CModel* srcmodel, int srccurboneno)
{
	m_model = srcmodel;
	CreateLimitEulWnd();//作成済の場合は０リターン

	Bone2AngleLimit(srccurboneno);
	bool updateonlycheckeul = false;
	AngleLimit2Dlg(updateonlycheckeul);

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_st_Sc) {
		//	m_st_Sc->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CLimitEulDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			//if (m_st_Sc) {
			//	//############
			//	//2024/07/24
			//	//############
			//	//int showposline = m_dlgSc->getShowPosLine();
			//	//m_dlgSc->setShowPosLine(showposline);
			//	//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
			//	//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
			//	//スクロールバーを少し動かすと全て表示された
			//	//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
			//	m_st_Sc->autoResize();
			//}
			m_dlgWnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgWnd) {
			m_dlgWnd->setVisible(false);
			m_dlgWnd->setListenMouse(false);

			m_underanglelimithscroll = 0;
		}
	}
	m_visible = srcflag;
}


int CLimitEulDlg::CreateLimitEulWnd()
{
	m_underanglelimithscroll = 0;

	if (m_dlgWnd) {
		return 0;//既に作成済　０リターン
	}

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}

	//if (m_curboneno < 0) {
	//	return 0;
	//}
	if (!m_model->GetTopBone()) {
		return 0;
	}

	if (m_model->GetOldAxisFlagAtLoading() == 1) {
		::MessageBox(NULL, L"Work Only After Setting Of Axis.\nRetry after Saving FBX file.", L"error!!!", MB_OK);
		return 0;
	}

	//m_dseullimitctrls.clear();
	//ChaMatrix tmpwm = m_model->GetWorldMat();
	//m_model->UpdateMatrix(g_limitdegflag, &tmpwm, &m_matView, &m_matProj, true, 0);
	//Bone2AngleLimit();


	m_dlgWnd = new OrgWindow(
		0,
		_T("LimitDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("LimitDlg"),	//タイトル
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
		double rate1 = 0.350;
		double rate50 = 0.50;


		m_limitnameLabel = new OWP_Label(L"BoneName", labelheight);
		if (!m_limitnameLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitxsp1 = new OWP_Separator(m_dlgWnd, true, 0.667, true);
		if (!m_limitxsp1) {
			_ASSERT(0);
			abort();
		}
		m_limitxsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitxsp2) {
			_ASSERT(0);
			abort();
		}
		m_limitxsp3 = new OWP_Separator(m_dlgWnd, true, 0.95, true);
		if (!m_limitxsp3) {
			_ASSERT(0);
			abort();
		}
		m_limitxsp2a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitxsp2a) {
			_ASSERT(0);
			abort();
		}
		m_limitxsp2b = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitxsp2b) {
			_ASSERT(0);
			abort();
		}
		m_limitxsp3a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitxsp3a) {
			_ASSERT(0);
			abort();
		}
		m_limitxupperLabel = new OWP_Label(L"X Upper", labelheight);
		if (!m_limitxupperLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitxupperEdit = new OWP_EditBox(true, L"XUpperEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitxupperEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitxlowerLabel = new OWP_Label(L"X Lower", labelheight);
		if (!m_limitxlowerLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitxlowerEdit = new OWP_EditBox(true, L"XLowerEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitxlowerEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitxchkLabel = new OWP_Label(L"X Check", labelheight);;
		if (!m_limitxchkLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitxchkEdit = new OWP_EditBox(true, L"XCheckEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitxchkEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitysp1 = new OWP_Separator(m_dlgWnd, true, 0.667, true);
		if (!m_limitysp1) {
			_ASSERT(0);
			abort();
		}
		m_limitysp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitysp2) {
			_ASSERT(0);
			abort();
		}
		m_limitysp3 = new OWP_Separator(m_dlgWnd, true, 0.95, true);
		if (!m_limitysp3) {
			_ASSERT(0);
			abort();
		}
		m_limitysp2a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitysp2a) {
			_ASSERT(0);
			abort();
		}
		m_limitysp2b = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitysp2b) {
			_ASSERT(0);
			abort();
		}
		m_limitysp3a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitysp3a) {
			_ASSERT(0);
			abort();
		}
		m_limityupperLabel = new OWP_Label(L"Y Upper", labelheight);
		if (!m_limityupperLabel) {
			_ASSERT(0);
			abort();
		}
		m_limityupperEdit = new OWP_EditBox(true, L"YUpperEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limityupperEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitylowerLabel = new OWP_Label(L"Y Lower", labelheight);
		if (!m_limitylowerLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitylowerEdit = new OWP_EditBox(true, L"YLowerEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitylowerEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitychkLabel = new OWP_Label(L"Y Check", labelheight);
		if (!m_limitychkLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitychkEdit = new OWP_EditBox(true, L"YCheckEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitychkEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitzsp1 = new OWP_Separator(m_dlgWnd, true, 0.667, true);
		if (!m_limitzsp1) {
			_ASSERT(0);
			abort();
		}
		m_limitzsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitzsp2) {
			_ASSERT(0);
			abort();
		}
		m_limitzsp3 = new OWP_Separator(m_dlgWnd, true, 0.95, true);
		if (!m_limitzsp3) {
			_ASSERT(0);
			abort();
		}
		m_limitzsp2a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitzsp2a) {
			_ASSERT(0);
			abort();
		}
		m_limitzsp2b = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitzsp2b) {
			_ASSERT(0);
			abort();
		}
		m_limitzsp3a = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitzsp3a) {
			_ASSERT(0);
			abort();
		}
		m_limitzupperLabel = new OWP_Label(L"Z Upper", labelheight);
		if (!m_limitzupperLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitzupperEdit = new OWP_EditBox(true, L"ZUpperEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitzupperEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitzlowerLabel = new OWP_Label(L"Z Lower", labelheight);
		if (!m_limitzlowerLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitzlowerEdit = new OWP_EditBox(true, L"ZLowerEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitzlowerEdit) {
			_ASSERT(0);
			abort();
		}
		m_limitzchkLabel = new OWP_Label(L"Z Check", labelheight);
		if (!m_limitzchkLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitzchkEdit = new OWP_EditBox(true, L"ZCheckEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_limitzchkEdit) {
			_ASSERT(0);
			abort();
		}
		m_limiteulsp1 = new OWP_Separator(m_dlgWnd, true, 0.667, true);
		if (!m_limiteulsp1) {
			_ASSERT(0);
			abort();
		}
		m_limiteulsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limiteulsp2) {
			_ASSERT(0);
			abort();
		}
		m_limiteulsp3 = new OWP_Separator(m_dlgWnd, true, 0.95, true);
		if (!m_limiteulsp3) {
			_ASSERT(0);
			abort();
		}
		m_limiteuloneB = new OWP_Button(L"One", labelheight);
		if (!m_limiteuloneB) {
			_ASSERT(0);
			abort();
		}
		m_limiteuloneB->setTextColor(RGB(168, 129, 129));
		m_limiteuldeeperB = new OWP_Button(L"Deeper", labelheight);
		if (!m_limiteuldeeperB) {
			_ASSERT(0);
			abort();
		}
		m_limiteuldeeperB->setTextColor(RGB(168, 129, 129));
		m_limiteulallB = new OWP_Button(L"All", labelheight);
		if (!m_limiteulallB) {
			_ASSERT(0);
			abort();
		}
		m_limiteulallB->setTextColor(RGB(168, 129, 129));
		m_limitcppsLabel = new OWP_Label(L"CopyPaste To Current Bone", labelheight);
		if (!m_limitcppsLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitcpsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitcpsp1) {
			_ASSERT(0);
			abort();
		}
		m_limitcpsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitcpsp2) {
			_ASSERT(0);
			abort();
		}
		m_limitcpsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitcpsp3) {
			_ASSERT(0);
			abort();
		}
		m_limitcptosymB = new OWP_Button(L"Cp2Sym", labelheight);
		if (!m_limitcptosymB) {
			_ASSERT(0);
			abort();
		}
		m_limitcpfromsymB = new OWP_Button(L"CpFromSym", labelheight);
		if (!m_limitcpfromsymB) {
			_ASSERT(0);
			abort();
		}
		m_limitcpB = new OWP_Button(L"Copy", labelheight);
		if (!m_limitcpB) {
			_ASSERT(0);
			abort();
		}
		m_limitpsB = new OWP_Button(L"Paste", labelheight);
		if (!m_limitpsB) {
			_ASSERT(0);
			abort();
		}
		m_limitsetLabel = new OWP_Label(L"Set to Current Bone", labelheight);
		if (!m_limitsetLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitsetsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitsetsp1) {
			_ASSERT(0);
			abort();
		}
		m_limitsetsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitsetsp2) {
			_ASSERT(0);
			abort();
		}
		m_limitsetsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitsetsp3) {
			_ASSERT(0);
			abort();
		}
		m_limitreset180B = new OWP_Button(L"180", labelheight);
		if (!m_limitreset180B) {
			_ASSERT(0);
			abort();
		}
		m_limit180to170B = new OWP_Button(L"180to170", labelheight);
		if (!m_limit180to170B) {
			_ASSERT(0);
			abort();
		}
		m_limitreset0B = new OWP_Button(L"0", labelheight);
		if (!m_limitreset0B) {
			_ASSERT(0);
			abort();
		}
		m_limitfrommotB = new OWP_Button(L"CurMot", labelheight);
		if (!m_limitfrommotB) {
			_ASSERT(0);
			abort();
		}
		m_limitsetallLabel = new OWP_Label(L"Set to All Bone", labelheight);
		if (!m_limitsetallLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitsetallsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitsetallsp1) {
			_ASSERT(0);
			abort();
		}
		m_limitsetallsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitsetallsp2) {
			_ASSERT(0);
			abort();
		}
		m_limitsetallsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitsetallsp3) {
			_ASSERT(0);
			abort();
		}
		m_limitresetall180B = new OWP_Button(L"180", labelheight);
		if (!m_limitresetall180B) {
			_ASSERT(0);
			abort();
		}
		m_limitall180to170B = new OWP_Button(L"180to170", labelheight);
		if (!m_limitall180to170B) {
			_ASSERT(0);
			abort();
		}
		m_limitallreset0B = new OWP_Button(L"0", labelheight);
		if (!m_limitallreset0B) {
			_ASSERT(0);
			abort();
		}
		m_limitallfrommotB = new OWP_Button(L"CurMot", labelheight);
		if (!m_limitallfrommotB) {
			_ASSERT(0);
			abort();
		}
		m_limitallboneallmotLabel = new OWP_Label(L"All Bone All Motion", labelheight);
		if (!m_limitallboneallmotLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitallboneallmotB = new OWP_Button(L"From All Retartgeted Motions", labelheight);
		if (!m_limitallboneallmotB) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicsLabel = new OWP_Label(L"limit rate for phsycis", labelheight);
		if (!m_limitphysicsLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicssp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_limitphysicssp) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicsrateLabel = new OWP_Label(L"rate", labelheight);
		if (!m_limitphysicsrateLabel) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicsrateSlider = new OWP_Slider((double)m_anglelimit.limitrate, 100.0, 0.0);
		if (!m_limitphysicsrateSlider) {
			_ASSERT(0);
			abort();
		}
		m_limitphsysicssp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitphsysicssp1) {
			_ASSERT(0);
			abort();
		}
		m_limitphsysicssp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_limitphsysicssp2) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicsDepperB = new OWP_Button(L"Deeper", labelheight);
		if (!m_limitphysicsDepperB) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicsDepperB->setTextColor(RGB(168, 129, 129));
		m_limitphysicsAllB = new OWP_Button(L"All", labelheight);
		if (!m_limitphysicsAllB) {
			_ASSERT(0);
			abort();
		}
		m_limitphysicsAllB->setTextColor(RGB(168, 129, 129));
		m_limitspacerLabel001 = new OWP_Label(L"     ", 32);
		if (!m_limitspacerLabel001) {
			_ASSERT(0);
			abort();
		}
		m_limitspacerLabel002 = new OWP_Label(L"     ", 32);
		if (!m_limitspacerLabel002) {
			_ASSERT(0);
			abort();
		}
		m_limitspacerLabel003 = new OWP_Label(L"     ", 32);
		if (!m_limitspacerLabel003) {
			_ASSERT(0);
			abort();
		}
		m_limitspacerLabel004 = new OWP_Label(L"     ", 32);
		if (!m_limitspacerLabel004) {
			_ASSERT(0);
			abort();
		}

		m_dlgWnd->addParts(*m_limitnameLabel);
		m_dlgWnd->addParts(*m_limitxsp1);
		m_limitxsp1->addParts1(*m_limitxsp2);
		m_limitxsp1->addParts2(*m_limitxsp3);
		m_limitxsp2->addParts1(*m_limitxsp2a);
		m_limitxsp2->addParts2(*m_limitxsp2b);
		m_limitxsp3->addParts1(*m_limitxsp3a);
		m_limitxsp2a->addParts1(*m_limitxlowerLabel);
		m_limitxsp2a->addParts2(*m_limitxlowerEdit);
		m_limitxsp2b->addParts1(*m_limitxupperLabel);
		m_limitxsp2b->addParts2(*m_limitxupperEdit);
		m_limitxsp3a->addParts1(*m_limitxchkLabel);
		m_limitxsp3a->addParts2(*m_limitxchkEdit);

		m_dlgWnd->addParts(*m_limitysp1);
		m_limitysp1->addParts1(*m_limitysp2);
		m_limitysp1->addParts2(*m_limitysp3);
		m_limitysp2->addParts1(*m_limitysp2a);
		m_limitysp2->addParts2(*m_limitysp2b);
		m_limitysp3->addParts1(*m_limitysp3a);
		m_limitysp2a->addParts1(*m_limitylowerLabel);
		m_limitysp2a->addParts2(*m_limitylowerEdit);
		m_limitysp2b->addParts1(*m_limityupperLabel);
		m_limitysp2b->addParts2(*m_limityupperEdit);
		m_limitysp3a->addParts1(*m_limitychkLabel);
		m_limitysp3a->addParts2(*m_limitychkEdit);

		m_dlgWnd->addParts(*m_limitzsp1);
		m_limitzsp1->addParts1(*m_limitzsp2);
		m_limitzsp1->addParts2(*m_limitzsp3);
		m_limitzsp2->addParts1(*m_limitzsp2a);
		m_limitzsp2->addParts2(*m_limitzsp2b);
		m_limitzsp3->addParts1(*m_limitzsp3a);
		m_limitzsp2a->addParts1(*m_limitzlowerLabel);
		m_limitzsp2a->addParts2(*m_limitzlowerEdit);
		m_limitzsp2b->addParts1(*m_limitzupperLabel);
		m_limitzsp2b->addParts2(*m_limitzupperEdit);
		m_limitzsp3a->addParts1(*m_limitzchkLabel);
		m_limitzsp3a->addParts2(*m_limitzchkEdit);
		m_dlgWnd->addParts(*m_limiteulsp1);
		m_limiteulsp1->addParts1(*m_limiteulsp2);
		m_limiteulsp1->addParts2(*m_limiteulsp3);
		m_limiteulsp2->addParts1(*m_limiteuloneB);
		m_limiteulsp2->addParts2(*m_limiteuldeeperB);
		m_limiteulsp3->addParts1(*m_limiteulallB);
		m_dlgWnd->addParts(*m_limitspacerLabel001);//
		m_dlgWnd->addParts(*m_limitcppsLabel);
		m_dlgWnd->addParts(*m_limitcpsp1);
		m_limitcpsp1->addParts1(*m_limitcpsp2);
		m_limitcpsp1->addParts2(*m_limitcpsp3);
		m_limitcpsp2->addParts1(*m_limitcptosymB);
		m_limitcpsp2->addParts2(*m_limitcpfromsymB);
		m_limitcpsp3->addParts1(*m_limitcpB);
		m_limitcpsp3->addParts2(*m_limitpsB);
		m_dlgWnd->addParts(*m_limitspacerLabel002);//
		m_dlgWnd->addParts(*m_limitsetLabel);
		m_dlgWnd->addParts(*m_limitsetsp1);
		m_limitsetsp1->addParts1(*m_limitsetsp2);
		m_limitsetsp1->addParts2(*m_limitsetsp3);
		m_limitsetsp2->addParts1(*m_limitreset180B);
		m_limitsetsp2->addParts2(*m_limit180to170B);
		m_limitsetsp3->addParts1(*m_limitreset0B);
		m_limitsetsp3->addParts2(*m_limitfrommotB);
		m_dlgWnd->addParts(*m_limitspacerLabel003);//
		m_dlgWnd->addParts(*m_limitsetallLabel);
		m_dlgWnd->addParts(*m_limitsetallsp1);
		m_limitsetallsp1->addParts1(*m_limitsetallsp2);
		m_limitsetallsp1->addParts2(*m_limitsetallsp3);
		m_limitsetallsp2->addParts1(*m_limitresetall180B);
		m_limitsetallsp2->addParts2(*m_limitall180to170B);
		m_limitsetallsp3->addParts1(*m_limitallreset0B);
		m_limitsetallsp3->addParts2(*m_limitallfrommotB);
		m_dlgWnd->addParts(*m_limitallboneallmotLabel);
		m_dlgWnd->addParts(*m_limitallboneallmotB);
		m_dlgWnd->addParts(*m_limitspacerLabel004);//
		m_dlgWnd->addParts(*m_limitphysicsLabel);
		m_dlgWnd->addParts(*m_limitphysicssp);
		m_limitphysicssp->addParts1(*m_limitphysicsrateLabel);
		m_limitphysicssp->addParts2(*m_limitphysicsrateSlider);
		m_dlgWnd->addParts(*m_limitphsysicssp1);
		m_limitphsysicssp1->addParts2(*m_limitphsysicssp2);
		m_limitphsysicssp2->addParts1(*m_limitphysicsDepperB);
		m_limitphsysicssp2->addParts2(*m_limitphysicsAllB);


		//##########
		//Slider
		//##########
		m_limitphysicsrateSlider->setCursorListener([=, this]() {
			double value = m_limitphysicsrateSlider->getValue();
			int setvalue = (int)(value + 0.0001);
			m_anglelimit.limitrate = setvalue;
			m_limitphysicsrateSlider->setValue(setvalue, false);//intに丸めてセットし直し
			});

		//########
		//Button
		//########
		m_limiteuloneB->setButtonListener([=, this]() {
			m_changelimitangleFlag = true;
			if (m_PrepairUndo) {
				(this->m_PrepairUndo)();
			}

			int result1 = 0;
			result1 = AngleDlg2AngleLimit();//エラー入力通知ダイアログも出す
			if (result1 == 0) {
				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_LIM2BONE_ONE, true);
				}
			}

			if (m_PrepairUndo) {
				(this->m_PrepairUndo)();
			}
			m_changelimitangleFlag = false;
			});
		m_limiteuldeeperB->setButtonListener([=, this]() {
			m_changelimitangleFlag = true;
			//全フレーム変更の前に全フレーム保存
			if (m_PrepairUndo) {
				(this->m_PrepairUndo)();
			}

			int result1 = 0;
			result1 = AngleDlg2AngleLimit();//エラー入力通知ダイアログも出す
			if (result1 == 0) {
				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_LIM2BONE_DEEPER, true);
				}
			}

			//全フレーム変更後に全フレーム保存
			if (m_PrepairUndo) {
				(this->m_PrepairUndo)();
			}
			m_changelimitangleFlag = false;
			});
		m_limiteulallB->setButtonListener([=, this]() {
			m_changelimitangleFlag = true;
			//全フレーム変更の前に全フレーム保存
			if (m_PrepairUndo) {
				(this->m_PrepairUndo)();
			}


			int result1 = 0;
			result1 = AngleDlg2AngleLimit();//エラー入力通知ダイアログも出す
			if (result1 == 0) {
				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_LIM2BONE_ALL, true);
				}
			}

			//全フレーム変更後に全フレーム保存
			if (m_PrepairUndo) {
				(this->m_PrepairUndo)();
			}

			m_changelimitangleFlag = false;
			});

		m_limitcptosymB->setButtonListener([=, this]() {
			//CopyToSymBone
			if (m_anglelimitbone && m_dlgWnd) {
				int symboneno = 0;
				int existflag = 0;
				m_model->GetSymBoneNo(m_anglelimitbone->GetBoneNo(), &symboneno, &existflag);
				if (symboneno >= 0) {
					CBone* symbone = m_model->GetBoneByID(symboneno);
					if (symbone) {
						ANGLELIMIT symanglelimit = m_anglelimit;
						//symanglelimit.lower[1] *= -1;
						symanglelimit.lower[2] *= -1;
						//symanglelimit.upper[1] *= -1;
						symanglelimit.upper[2] *= -1;

						if (m_model->ExistCurrentMotion()) {
							m_changelimitangleFlag = true;
							//全フレーム変更の前に全フレーム保存
							if (m_PrepairUndo) {
								(this->m_PrepairUndo)();
							}


							symbone->SetAngleLimit(g_limitdegflag, symanglelimit);

							if (m_UpdateAfterEditAngleLimit) {
								(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_NONE, true);
							}

							//全フレーム変更後に全フレーム保存
							if (m_PrepairUndo) {
								(this->m_PrepairUndo)();
							}
							m_changelimitangleFlag = false;
						}
					}
				}
			}
			});
		m_limitcpfromsymB->setButtonListener([=, this]() {
			//CopyFromSymBone
			if (m_anglelimitbone && m_dlgWnd) {
				int symboneno = 0;
				int existflag = 0;
				m_model->GetSymBoneNo(m_anglelimitbone->GetBoneNo(), &symboneno, &existflag);
				if (symboneno >= 0) {
					CBone* symbone = m_model->GetBoneByID(symboneno);
					if (symbone) {
						if (m_model->ExistCurrentMotion()) {

							m_changelimitangleFlag = true;
							//全フレーム変更の前に全フレーム保存
							if (m_PrepairUndo) {
								(this->m_PrepairUndo)();
							}


							ANGLELIMIT anglelimit = symbone->GetAngleLimit(g_limitdegflag, 0);
							ANGLELIMIT symanglelimit = anglelimit;
							//symanglelimit.lower[1] *= -1;
							symanglelimit.lower[2] *= -1;
							//symanglelimit.upper[1] *= -1;
							symanglelimit.upper[2] *= -1;

							//m_anglelimitbone->SetAngleLimit(symanglelimit, curmi->motid, curmi->curframe);
							m_anglelimit = symanglelimit;
							if (m_UpdateAfterEditAngleLimit) {
								(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_LIM2BONE_ONE, true);
							}

							bool updateonlycheckeul = false;
							AngleLimit2Dlg(updateonlycheckeul);
							//UpdateWindow(m_anglelimitdlg);
							m_dlgWnd->callRewrite();

							//全フレーム変更後に全フレーム保存
							if (m_PrepairUndo) {
								(this->m_PrepairUndo)();
							}

							m_changelimitangleFlag = false;

						}

					}
				}
			}
			});
		m_limitcpB->setButtonListener([=, this]() {
			//copy button
			if (m_anglelimitbone && m_dlgWnd) {
				m_anglelimitcopy = m_anglelimit;
			}
			});
		m_limitpsB->setButtonListener([=, this]() {
			//paste button
			if (m_anglelimitbone && m_dlgWnd) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				m_anglelimit = m_anglelimitcopy;
				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_LIM2BONE_ONE, true);
				}
				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});



		m_limitreset180B->setButtonListener([=, this]() {
			//curboneだけ
			if (m_model && m_dlgWnd && m_anglelimitbone) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				bool excludebt = false;
				m_model->ResetAngleLimit(excludebt, 180, m_anglelimitbone);//2022/12/05 curbone引数追加

				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, true);
				}

				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});
		m_limit180to170B->setButtonListener([=, this]() {
			//curboneだけ
			if (m_model && m_dlgWnd && m_anglelimitbone) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				m_model->AngleLimitReplace180to170(m_anglelimitbone);//2022/12/05 curbone引数追加

				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, true);
				}

				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});
		m_limitreset0B->setButtonListener([=, this]() {
			//curboneだけ
			if (m_model && m_dlgWnd && m_anglelimitbone) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				bool excludebt = false;
				m_model->ResetAngleLimit(excludebt, 0, m_anglelimitbone);//2022/12/05 curbone引数追加

				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, true);
				}

				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});
		m_limitfrommotB->setButtonListener([=, this]() {
			//curboneだけ
			if (m_model && m_dlgWnd && m_anglelimitbone) {
				if (m_model->ExistCurrentMotion()) {
					bool savelimitflag = g_limitdegflag;

					////m_changelimitangleFlag = true;
					////PrepairUndo();//全フレーム変更の前に全フレーム保存

					//長いフレームの処理は数秒時間がかかることがあるので砂時計カーソルにする
					HCURSOR oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

					bool excludebt = true;
					//m_model->ResetAngleLimit(excludebt, 0);
					m_model->ResetAngleLimit(excludebt, 0, m_anglelimitbone);//2024/02/02 curboneだけリセット

					if (m_UpdateAfterEditAngleLimit) {
						(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, false);
					}

					m_model->AdditiveCurrentToAngleLimit(m_anglelimitbone);//2022/12/05 curbone引数追加

					//ChangeLimitDegFlag(m_savelimitdegflag, true, false);//updateeulはこれより後で呼ばれるUpdateAfterEditAngleLimitで

					if (m_UpdateAfterEditAngleLimit) {
						(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, false);
					}

					bool updateonlycheckeul = false;
					AngleLimit2Dlg(updateonlycheckeul);
					//UpdateWindow(m_anglelimitdlg);
					m_dlgWnd->callRewrite();

					//カーソルを元に戻す
					SetCursor(oldcursor);

					//全フレーム変更後に全フレーム保存
					if (m_PrepairUndo) {
						(this->m_PrepairUndo)();
					}

					m_changelimitangleFlag = false;

				}
			}
			});



		m_limitresetall180B->setButtonListener([=, this]() {
			if (m_model && m_dlgWnd) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				bool excludebt = false;
				m_model->ResetAngleLimit(excludebt, 180);

				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, true);
				}

				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});
		m_limitall180to170B->setButtonListener([=, this]() {
			if (m_model && m_dlgWnd) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				m_model->AngleLimitReplace180to170();

				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, true);
				}

				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});
		m_limitallreset0B->setButtonListener([=, this]() {
			if (m_model && m_dlgWnd) {
				m_changelimitangleFlag = true;
				//全フレーム変更の前に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}


				bool excludebt = false;
				m_model->ResetAngleLimit(excludebt, 0);

				if (m_UpdateAfterEditAngleLimit) {
					(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, true);
				}

				bool updateonlycheckeul = false;
				AngleLimit2Dlg(updateonlycheckeul);
				//UpdateWindow(m_anglelimitdlg);
				m_dlgWnd->callRewrite();

				//全フレーム変更後に全フレーム保存
				if (m_PrepairUndo) {
					(this->m_PrepairUndo)();
				}

				m_changelimitangleFlag = false;

			}
			});
		m_limitallfrommotB->setButtonListener([=, this]() {
			if (m_model && m_dlgWnd) {
				if (m_model->ExistCurrentMotion()) {

					m_changelimitangleFlag = true;
					//全フレーム変更の前に全フレーム保存
					if (m_PrepairUndo) {
						(this->m_PrepairUndo)();
					}


					//長いフレームの処理は数秒時間がかかることがあるので砂時計カーソルにする
					HCURSOR oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

					//m_savelimitdegflag = g_limitdegflag;
					//ChangeLimitDegFlag(false, true, true);
					//g_limitdegflag = false;
					//if (m_LimitDegCheckBox) {
					//	m_LimitDegCheckBox->SetChecked(g_limitdegflag);
					//}


					//モーションからの設定の前に　まずはゼロ初期化する
					bool excludebt = true;
					m_model->ResetAngleLimit(excludebt, 0);

					if (m_UpdateAfterEditAngleLimit) {
						(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, false);
					}



					//g_limitdegflagに関わらず　既存モーションの制限無しの姿勢を元に設定
					m_model->AdditiveCurrentToAngleLimit(0);//内部で全フレーム分処理

					//ChangeLimitDegFlag(m_savelimitdegflag, true, false);//updateeulはこれより後で呼ばれるUpdateAfterEditAngleLimitで
					//g_limitdegflag = m_savelimitdegflag;
					//if (m_LimitDegCheckBox) {
					//	m_LimitDegCheckBox->SetChecked(g_limitdegflag);
					//}

					if (m_UpdateAfterEditAngleLimit) {
						(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, false);
					}

					bool updateonlycheckeul = false;
					AngleLimit2Dlg(updateonlycheckeul);
					//UpdateWindow(m_anglelimitdlg);
					m_dlgWnd->callRewrite();

					//カーソルを元に戻す
					SetCursor(oldcursor);

					//全フレーム変更後に全フレーム保存
					if (m_PrepairUndo) {
						(this->m_PrepairUndo)();
					}

					m_changelimitangleFlag = false;

				}
			}
			});

		m_limitallboneallmotB->setButtonListener([=, this]() {
			if (m_model && m_dlgWnd) {
				if (m_model->ExistCurrentMotion()) {
					bool savelimitflag = g_limitdegflag;

					m_changelimitangleFlag = true;
					//全フレーム変更の前に全フレーム保存
					if (m_PrepairUndo) {
						(this->m_PrepairUndo)();
					}


					//長いフレームの処理は数秒時間がかかることがあるので砂時計カーソルにする
					HCURSOR oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

					//if (savelimitflag == true) {
					//	//2023/10/18 制限角度キャプチャは　limitdegflag = falseにした状態で働く
					//	m_savelimitdegflag = true;
					//	ChangeLimitDegFlag(false, true, true);
					//}

					//モーションからの設定の前に　まずはゼロ初期化する
					bool excludebt = true;
					m_model->ResetAngleLimit(excludebt, 0);
					if (m_UpdateAfterEditAngleLimit) {
						(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, false);
					}

					//g_limitdegflagに関わらず　既存モーションの制限無しの姿勢を元に設定
					m_model->AdditiveAllMotionsToAngleLimit();//内部で全モーション全フレーム分処理

					//ChangeLimitDegFlag(m_savelimitdegflag, true, false);//updateeulはこれより後で呼ばれるUpdateAfterEditAngleLimitで
					//if (savelimitflag == true) {
					//	//処理後に元に戻す
					//	m_savelimitdegflag = false;
					//	ChangeLimitDegFlag(true, true, true);
					//}

					if (m_UpdateAfterEditAngleLimit) {
						(this->m_UpdateAfterEditAngleLimit)(eLIM2BONE_BONE2LIM, false);
					}

					//m_model->SetMotionFrame(1.0);
					//ChaMatrix tmpwm = m_model->GetWorldMat();
					//m_model->UpdateMatrix(&tmpwm, &m_matVP);

					bool updateonlycheckeul = false;
					AngleLimit2Dlg(updateonlycheckeul);
					//UpdateWindow(m_anglelimitdlg);
					m_dlgWnd->callRewrite();

					//カーソルを元に戻す
					SetCursor(oldcursor);

					//全フレーム変更後に全フレーム保存
					if (m_PrepairUndo) {
						(this->m_PrepairUndo)();
					}

					m_changelimitangleFlag = false;

				}
			}
			});

		m_limitphysicsDepperB->setButtonListener([=, this]() {
			if (m_anglelimitbone && m_dlgWnd) {
				LimitRate2Bone_Req(m_anglelimitbone, 0);
			}
			//PrepairUndo();
			});
		m_limitphysicsAllB->setButtonListener([=, this]() {
			if (m_anglelimitbone && m_dlgWnd) {
				LimitRate2Bone_Req(m_model->GetTopBone(false), 1);
			}
			//PrepairUndo();
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

//int CLimitEulDlg::ParamsToDlg(CMQOMaterial* srcmat, CShaderTypeParams* srcshadertypeparams)
//{
//
//	return 0;
//}

int CLimitEulDlg::Bone2AngleLimit(int srccurboneno)
{
	if (!m_model) {
		return 0;
	}
	if (srccurboneno < 0) {
		return 0;
	}
	if (!m_model->GetTopBone()) {
		return 0;
	}

	ANGLELIMIT saveal;
	saveal = m_anglelimit;


	CBone* curbone;
	curbone = m_model->GetBoneByID(srccurboneno);

	//オイラーグラフの表示と合わせるために選択ジョイントの１階層親のジョイントを扱う //2021/11/17
	if (curbone) {
		if (curbone->GetParent(false) && curbone->GetParent(false)->IsSkeleton()) {
			m_anglelimitbone = curbone->GetParent(false);
		}
		else {
			m_anglelimitbone = curbone;
		}
	}
	else {
		m_anglelimitbone = 0;
	}

	if (m_anglelimitbone && m_model->ExistCurrentMotion()) {
		m_anglelimit = m_anglelimitbone->GetAngleLimit(g_limitdegflag, 1);

		if (g_limitdegflag) {
			m_anglelimit.chkeul[AXIS_X] = min(m_anglelimit.upper[AXIS_X], m_anglelimit.chkeul[AXIS_X]);
			m_anglelimit.chkeul[AXIS_Y] = min(m_anglelimit.upper[AXIS_Y], m_anglelimit.chkeul[AXIS_Y]);
			m_anglelimit.chkeul[AXIS_Z] = min(m_anglelimit.upper[AXIS_Z], m_anglelimit.chkeul[AXIS_Z]);

			m_anglelimit.chkeul[AXIS_X] = max(m_anglelimit.lower[AXIS_X], m_anglelimit.chkeul[AXIS_X]);
			m_anglelimit.chkeul[AXIS_Y] = max(m_anglelimit.lower[AXIS_Y], m_anglelimit.chkeul[AXIS_Y]);
			m_anglelimit.chkeul[AXIS_Z] = max(m_anglelimit.lower[AXIS_Z], m_anglelimit.chkeul[AXIS_Z]);
		}
	}
	else {
		//モーションの無いfbxに対してプレビューボタンを押すとここを通る
		_ASSERT(0);
		m_anglelimit.Init();
	}

	////setcheckflag == 0のときにはチェックボックスの状態を変えずに復元する
	//if (setcheckflag == 0) {
	//	m_anglelimit.applyeul[AXIS_X] = saveal.applyeul[AXIS_X];
	//	m_anglelimit.applyeul[AXIS_Y] = saveal.applyeul[AXIS_Y];
	//	m_anglelimit.applyeul[AXIS_Z] = saveal.applyeul[AXIS_Z];

	//	m_anglelimit.via180flag[AXIS_X] = saveal.via180flag[AXIS_X];
	//	m_anglelimit.via180flag[AXIS_Y] = saveal.via180flag[AXIS_Y];
	//	m_anglelimit.via180flag[AXIS_Z] = saveal.via180flag[AXIS_Z];
	//}


	return 0;
}

int CLimitEulDlg::AngleLimit2Dlg(bool updateonlycheckeul)
{
	if (m_anglelimitbone && (m_dlgWnd != 0)) {

		if (updateonlycheckeul == false) {
			if (m_limitnameLabel) {
				m_limitnameLabel->setName(m_anglelimitbone->GetWBoneName());
			}

			//入力フィールドを毎フレーム更新すると　入力できないので　updateonlycheckeulのときには更新しない
			WCHAR strlimit[ANGLEDLGEDITLEN] = { 0L };

			swprintf_s(strlimit, ANGLEDLGEDITLEN, L"%d", m_anglelimit.lower[AXIS_X]);
			if (m_limitxlowerEdit) {
				m_limitxlowerEdit->setName(strlimit);
			}
			swprintf_s(strlimit, ANGLEDLGEDITLEN, L"%d", m_anglelimit.upper[AXIS_X]);
			if (m_limitxupperEdit) {
				m_limitxupperEdit->setName(strlimit);
			}

			swprintf_s(strlimit, ANGLEDLGEDITLEN, L"%d", m_anglelimit.lower[AXIS_Y]);
			if (m_limitylowerEdit) {
				m_limitylowerEdit->setName(strlimit);
			}
			swprintf_s(strlimit, ANGLEDLGEDITLEN, L"%d", m_anglelimit.upper[AXIS_Y]);
			if (m_limityupperEdit) {
				m_limityupperEdit->setName(strlimit);
			}

			swprintf_s(strlimit, ANGLEDLGEDITLEN, L"%d", m_anglelimit.lower[AXIS_Z]);
			if (m_limitzlowerEdit) {
				m_limitzlowerEdit->setName(strlimit);
			}
			swprintf_s(strlimit, ANGLEDLGEDITLEN, L"%d", m_anglelimit.upper[AXIS_Z]);
			if (m_limitzupperEdit) {
				m_limitzupperEdit->setName(strlimit);
			}

			if (m_limitphysicsrateSlider) {
				m_limitphysicsrateSlider->setValue((double)m_anglelimit.limitrate, false);
			}
		}

		WCHAR strchk[ANGLEDLGEDITLEN] = { 0L };
		swprintf_s(strchk, ANGLEDLGEDITLEN, L"%d", (int)m_anglelimit.chkeul[AXIS_X]);
		if (m_limitxchkEdit) {
			m_limitxchkEdit->setName(strchk);
		}
		swprintf_s(strchk, ANGLEDLGEDITLEN, L"%d", (int)m_anglelimit.chkeul[AXIS_Y]);
		if (m_limitychkEdit) {
			m_limitychkEdit->setName(strchk);
		}
		swprintf_s(strchk, ANGLEDLGEDITLEN, L"%d", (int)m_anglelimit.chkeul[AXIS_Z]);
		if (m_limitzchkEdit) {
			m_limitzchkEdit->setName(strchk);
		}


		//if (m_anglelimit.via180flag[0] == 1){
		//	CheckDlgButton(hDlgWnd, IDC_CHECKX, BST_CHECKED);
		//}
		//else{
		//	CheckDlgButton(hDlgWnd, IDC_CHECKX, BST_UNCHECKED);
		//}
		//if (m_anglelimit.via180flag[1] == 1){
		//	CheckDlgButton(hDlgWnd, IDC_CHECKY, BST_CHECKED);
		//}
		//else{
		//	CheckDlgButton(hDlgWnd, IDC_CHECKY, BST_UNCHECKED);
		//}
		//if (m_anglelimit.via180flag[2] == 1){
		//	CheckDlgButton(hDlgWnd, IDC_CHECKZ, BST_CHECKED);
		//}
		//else{
		//	CheckDlgButton(hDlgWnd, IDC_CHECKZ, BST_UNCHECKED);
		//}


		m_dlgWnd->callRewrite();
	}
	else {
		//_ASSERT(0);
		int dbgflag1 = 1;
	}

	return 0;
}


int CLimitEulDlg::AngleDlg2AngleLimit()//2022/12/05 エラー入力通知ダイアログも出す
{
	int result_xl, result_xu;
	int result_yl, result_yu;
	int result_zl, result_zu;
	int val_xl, val_xu;
	int val_yl, val_yu;
	int val_zl, val_zu;

	result_xl = 1;
	result_xu = 1;
	result_yl = 1;
	result_yu = 1;
	result_zl = 1;
	result_zu = 1;
	val_xl = -180;
	val_xu = 180;
	val_yl = -180;
	val_yu = 180;
	val_zl = -180;
	val_zu = 180;
	bool errorflag = false;

	result_xl = GetAngleLimitEditIntOWP(m_limitxlowerEdit, &val_xl);
	if (result_xl != 0) {
		::MessageBox(g_mainhwnd, L"AngleLimitDlgのXLowerの入力値が不正です。", L"入力し直してください。", MB_OK);
		errorflag = true;
	}
	result_xu = GetAngleLimitEditIntOWP(m_limitxupperEdit, &val_xu);
	if (result_xu != 0) {
		::MessageBox(g_mainhwnd, L"AngleLimitDlgのXUpperの入力値が不正です。", L"入力し直してください。", MB_OK);
		errorflag = true;
	}


	result_yl = GetAngleLimitEditIntOWP(m_limitylowerEdit, &val_yl);
	if (result_yl != 0) {
		::MessageBox(g_mainhwnd, L"AngleLimitDlgのYLowerの入力値が不正です。", L"入力し直してください。", MB_OK);
		errorflag = true;
	}
	//result_yu = GetAngleLimitEditIntOWP(m_limitxupperEdit, &val_yu);//<--- 不具合
	result_yu = GetAngleLimitEditIntOWP(m_limityupperEdit, &val_yu);//<--- 2024/08/09修正　xupper-->yupper
	if (result_yu != 0) {
		::MessageBox(g_mainhwnd, L"AngleLimitDlgのYUpperの入力値が不正です。", L"入力し直してください。", MB_OK);
		errorflag = true;
	}


	result_zl = GetAngleLimitEditIntOWP(m_limitzlowerEdit, &val_zl);
	if (result_zl != 0) {
		::MessageBox(g_mainhwnd, L"AngleLimitDlgのZLowerの入力値が不正です。", L"入力し直してください。", MB_OK);
		errorflag = true;
	}
	result_zu = GetAngleLimitEditIntOWP(m_limitzupperEdit, &val_zu);
	if (result_zu != 0) {
		::MessageBox(g_mainhwnd, L"AngleLimitDlgのZUpperの入力値が不正です。", L"入力し直してください。", MB_OK);
		errorflag = true;
	}


	if (errorflag == false) {
		m_anglelimit.lower[AXIS_X] = val_xl;
		m_anglelimit.upper[AXIS_X] = val_xu;

		m_anglelimit.lower[AXIS_Y] = val_yl;
		m_anglelimit.upper[AXIS_Y] = val_yu;

		m_anglelimit.lower[AXIS_Z] = val_zl;
		m_anglelimit.upper[AXIS_Z] = val_zu;

		return 0;
	}
	else {
		return 1;
	}


}

int CLimitEulDlg::GetAngleLimitEditIntOWP(OWP_EditBox* srcedit, int* dstlimit)
{
	if (!srcedit || !dstlimit) {
		_ASSERT(0);
		return 1;
	}

	WCHAR stredit[ANGLEDLGEDITLEN] = { 0L };
	::ZeroMemory(stredit, sizeof(WCHAR) * ANGLEDLGEDITLEN);
	srcedit->getName(stredit, ANGLEDLGEDITLEN);

	stredit[ANGLEDLGEDITLEN - 1] = 0L;
	int result1;
	result1 = CheckStr_SInt(stredit);
	if (result1 == 0) {
		stredit[ANGLEDLGEDITLEN - 1] = 0L;

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

int CLimitEulDlg::CheckStr_SInt(const WCHAR* srcstr)
{
	if (!srcstr) {
		return 1;
	}
	size_t strleng = wcslen(srcstr);
	if ((strleng <= 0) || (strleng >= ANGLEDLGEDITLEN)) {
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

void CLimitEulDlg::LimitRate2Bone_Req(CBone* srcbone, int setbroflag)
{
	if (srcbone) {

		srcbone->SetLimitRate(m_anglelimit.limitrate);

		if (srcbone->GetChild(false)) {
			LimitRate2Bone_Req(srcbone->GetChild(false), 1);
		}
		if ((setbroflag != 0) && (srcbone->GetBrother(false) != nullptr)) {
			LimitRate2Bone_Req(srcbone->GetBrother(false), setbroflag);
		}
	}
}


int CLimitEulDlg::AngleLimit2Bone_One(CBone* srcbone)
{
	if (m_model && srcbone && (srcbone->IsSkeleton())) {
		if (m_model->ExistCurrentMotion()) {
			srcbone->SetAngleLimit(g_limitdegflag, m_anglelimit);
		}
		return 0;
	}
	else {
		return 1;
	}
}
void CLimitEulDlg::AngleLimit2Bone_Req(CBone* srcbone, int setbroflag)
{
	if (srcbone) {
		if (srcbone->IsSkeleton()) {
			AngleLimit2Bone_One(srcbone);
		}

		if (srcbone->GetChild(false)) {
			int newsetbroflag = 1;
			AngleLimit2Bone_Req(srcbone->GetChild(false), newsetbroflag);
		}
		if ((setbroflag != 0) && (srcbone->GetBrother(false) != nullptr)) {
			AngleLimit2Bone_Req(srcbone->GetBrother(false), setbroflag);
		}
	}
}

int CLimitEulDlg::AngleLimit2Bone(int limit2boneflag)
{
	if (!m_model) {
		return 0;
	}
	if (!m_anglelimitbone) {
		return 0;
	}
	if (!m_model->GetTopBone()) {
		return 0;
	}

	//if (g_previewFlag == 0) {
	if (m_anglelimitbone) {
		if (limit2boneflag == eLIM2BONE_LIM2BONE_ONE) {
			AngleLimit2Bone_One(m_anglelimitbone);
		}
		else if (limit2boneflag == eLIM2BONE_LIM2BONE_DEEPER) {
			int setbroflag = 0;
			AngleLimit2Bone_Req(m_anglelimitbone, setbroflag);
		}
		else if (limit2boneflag == eLIM2BONE_LIM2BONE_ALL) {
			int setbroflag = 0;
			AngleLimit2Bone_Req(m_model->GetTopBone(false), setbroflag);
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	//}
//}

	return 0;
}



const HWND CLimitEulDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CLimitEulDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CLimitEulDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

void CLimitEulDlg::CallRewrite()
{
	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		m_dlgWnd->callRewrite();
	}
}