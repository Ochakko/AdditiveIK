#include "stdafx.h"

#include <ShadowDlg.h>
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
// CShadowDlg

CShadowDlg::CShadowDlg()
{
	InitParams();
}

CShadowDlg::~CShadowDlg()
{
	DestroyObjs();
}
	
int CShadowDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_shadowslotsp) {
		delete m_shadowslotsp;
		m_shadowslotsp = nullptr;
	}
	if (m_shadowenableChk) {
		delete m_shadowenableChk;
		m_shadowenableChk = nullptr;
	}
	if (m_shadowslotCombo) {
		delete m_shadowslotCombo;
		m_shadowslotCombo = nullptr;
	}
	if (m_shadowvsmsp) {
		delete m_shadowvsmsp;
		m_shadowvsmsp = nullptr;
	}
	if (m_shadowvsmChk) {
		delete m_shadowvsmChk;
		m_shadowvsmChk = nullptr;
	}
	if (m_shadowvsmblurChk) {
		delete m_shadowvsmblurChk;
		m_shadowvsmblurChk = nullptr;
	}
	if (m_shadowcamposLabel) {
		delete m_shadowcamposLabel;
		m_shadowcamposLabel = nullptr;
	}
	if (m_shadowcamposupsp0) {
		delete m_shadowcamposupsp0;
		m_shadowcamposupsp0 = nullptr;
	}
	if (m_shadowcamposupsp1) {
		delete m_shadowcamposupsp1;
		m_shadowcamposupsp1 = nullptr;
	}
	if (m_shadowcamposupsp2) {
		delete m_shadowcamposupsp2;
		m_shadowcamposupsp2 = nullptr;
	}
	if (m_shadowcamposupLabel) {
		delete m_shadowcamposupLabel;
		m_shadowcamposupLabel = nullptr;
	}
	if (m_shadowcamposupEdit) {
		delete m_shadowcamposupEdit;
		m_shadowcamposupEdit = nullptr;
	}
	if (m_shadowcamposdistsp0) {
		delete m_shadowcamposdistsp0;
		m_shadowcamposdistsp0 = nullptr;
	}
	if (m_shadowcamposdistsp1) {
		delete m_shadowcamposdistsp1;
		m_shadowcamposdistsp1 = nullptr;
	}
	if (m_shadowcamposdistsp2) {
		delete m_shadowcamposdistsp2;
		m_shadowcamposdistsp2 = nullptr;
	}
	if (m_shadowcamposdistLabel) {
		delete m_shadowcamposdistLabel;
		m_shadowcamposdistLabel = nullptr;
	}
	if (m_shadowcamposdistEdit) {
		delete m_shadowcamposdistEdit;
		m_shadowcamposdistEdit = nullptr;
	}
	if (m_shadowcamposdistxLabel) {
		delete m_shadowcamposdistxLabel;
		m_shadowcamposdistxLabel = nullptr;
	}
	if (m_shadowcamdirLabel) {
		delete m_shadowcamdirLabel;
		m_shadowcamdirLabel = nullptr;
	}
	if (m_shadowcamdirsp0) {
		delete m_shadowcamdirsp0;
		m_shadowcamdirsp0 = nullptr;
	}
	if (m_shadowcamdirsp1) {
		delete m_shadowcamdirsp1;
		m_shadowcamdirsp1 = nullptr;
	}
	if (m_shadowcamdirsp2) {
		delete m_shadowcamdirsp2;
		m_shadowcamdirsp2 = nullptr;
	}
	if (m_shadowcamdirsp3) {
		delete m_shadowcamdirsp3;
		m_shadowcamdirsp3 = nullptr;
	}
	if (m_shadowcamdirsp4) {
		delete m_shadowcamdirsp4;
		m_shadowcamdirsp4 = nullptr;
	}
	if (m_shadowcamdirsp5) {
		delete m_shadowcamdirsp5;
		m_shadowcamdirsp5 = nullptr;
	}
	if (m_shadowcamdirsp6) {
		delete m_shadowcamdirsp6;
		m_shadowcamdirsp6 = nullptr;
	}
	int dirindex;
	for (dirindex = 0; dirindex < 8; dirindex++) {
		if (m_shadowcamdirChk[dirindex]) {
			delete m_shadowcamdirChk[dirindex];
			m_shadowcamdirChk[dirindex] = nullptr;
		}
	}
	if (m_shadowprojLabel) {
		delete m_shadowprojLabel;
		m_shadowprojLabel = nullptr;
	}
	if (m_shadowprojfovsp) {
		delete m_shadowprojfovsp;
		m_shadowprojfovsp = nullptr;
	}
	if (m_shadowprojfovLabel) {
		delete m_shadowprojfovLabel;
		m_shadowprojfovLabel = nullptr;
	}
	if (m_shadowprojfovSlider) {
		delete m_shadowprojfovSlider;
		m_shadowprojfovSlider = nullptr;
	}
	if (m_shadowprojnearfarsp0) {
		delete m_shadowprojnearfarsp0;
		m_shadowprojnearfarsp0 = nullptr;
	}
	if (m_shadowprojnearfarsp1) {
		delete m_shadowprojnearfarsp1;
		m_shadowprojnearfarsp1 = nullptr;
	}
	if (m_shadowprojnearfarsp2) {
		delete m_shadowprojnearfarsp2;
		m_shadowprojnearfarsp2 = nullptr;
	}
	if (m_shadowprojnearLabel) {
		delete m_shadowprojnearLabel;
		m_shadowprojnearLabel = nullptr;
	}
	if (m_shadowprojnearEdit) {
		delete m_shadowprojnearEdit;
		m_shadowprojnearEdit = nullptr;
	}
	if (m_shadowprojfarLabel) {
		delete m_shadowprojfarLabel;
		m_shadowprojfarLabel = nullptr;
	}
	if (m_shadowprojfarEdit) {
		delete m_shadowprojfarEdit;
		m_shadowprojfarEdit = nullptr;
	}
	if (m_shadowotherLabel) {
		delete m_shadowotherLabel;
		m_shadowotherLabel = nullptr;
	}
	if (m_shadowothersp1) {
		delete m_shadowothersp1;
		m_shadowothersp1 = nullptr;
	}
	if (m_shadowothercolorLabel) {
		delete m_shadowothercolorLabel;
		m_shadowothercolorLabel = nullptr;
	}
	if (m_shadowothercolorSlider) {
		delete m_shadowothercolorSlider;
		m_shadowothercolorSlider = nullptr;
	}
	if (m_shadowothersp2) {
		delete m_shadowothersp2;
		m_shadowothersp2 = nullptr;
	}
	if (m_shadowotherbiasLabel) {
		delete m_shadowotherbiasLabel;
		m_shadowotherbiasLabel = nullptr;
	}
	if (m_shadowotherbiasSlider) {
		delete m_shadowotherbiasSlider;
		m_shadowotherbiasSlider = nullptr;
	}
	if (m_shadowothersp3) {
		delete m_shadowothersp3;
		m_shadowothersp3 = nullptr;
	}
	if (m_shadowothermultLabel) {
		delete m_shadowothermultLabel;
		m_shadowothermultLabel = nullptr;
	}
	if (m_shadowothermultSlider) {
		delete m_shadowothermultSlider;
		m_shadowothermultSlider = nullptr;
	}
	if (m_shadowspacerLabel) {
		delete m_shadowspacerLabel;
		m_shadowspacerLabel = nullptr;
	}
	if (m_shadowapplysp) {
		delete m_shadowapplysp;
		m_shadowapplysp = nullptr;
	}
	if (m_shadowapplysp) {
		delete m_shadowapplysp;
		m_shadowapplysp = nullptr;
	}
	if (m_shadowapplyB) {
		delete m_shadowapplyB;
		m_shadowapplyB = nullptr;
	}
	if (m_shadowinitB) {
		delete m_shadowinitB;
		m_shadowinitB = nullptr;
	}
	if (m_shadowspacerLabel001) {
		delete m_shadowspacerLabel001;
		m_shadowspacerLabel001 = nullptr;
	}
	if (m_shadowspacerLabel002) {
		delete m_shadowspacerLabel002;
		m_shadowspacerLabel002 = nullptr;
	}
	if (m_shadowspacerLabel003) {
		delete m_shadowspacerLabel003;
		m_shadowspacerLabel003 = nullptr;
	}



	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CShadowDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_dlgWnd = nullptr;

	m_shadowslotsp = nullptr;
	m_shadowenableChk = nullptr;
	m_shadowslotCombo = nullptr;
	m_shadowvsmsp = nullptr;
	m_shadowvsmChk = nullptr;
	m_shadowvsmblurChk = nullptr;
	m_shadowcamposLabel = nullptr;
	m_shadowcamposupsp0 = nullptr;
	m_shadowcamposupsp1 = nullptr;
	m_shadowcamposupsp2 = nullptr;
	m_shadowcamposupLabel = nullptr;
	m_shadowcamposupEdit = nullptr;
	m_shadowcamposdistsp0 = nullptr;
	m_shadowcamposdistsp1 = nullptr;
	m_shadowcamposdistsp2 = nullptr;
	m_shadowcamposdistLabel = nullptr;
	m_shadowcamposdistEdit = nullptr;
	m_shadowcamposdistxLabel = nullptr;
	m_shadowcamdirLabel = nullptr;
	m_shadowcamdirsp0 = nullptr;
	m_shadowcamdirsp1 = nullptr;
	m_shadowcamdirsp2 = nullptr;
	m_shadowcamdirsp3 = nullptr;
	m_shadowcamdirsp4 = nullptr;
	m_shadowcamdirsp5 = nullptr;
	m_shadowcamdirsp6 = nullptr;
	int dirindex;
	for (dirindex = 0; dirindex < 8; dirindex++) {
		m_shadowcamdirChk[dirindex] = nullptr;
	}
	m_shadowprojLabel = nullptr;
	m_shadowprojfovsp = nullptr;
	m_shadowprojfovLabel = nullptr;
	m_shadowprojfovSlider = nullptr;
	m_shadowprojnearfarsp0 = nullptr;
	m_shadowprojnearfarsp1 = nullptr;
	m_shadowprojnearfarsp2 = nullptr;
	m_shadowprojnearLabel = nullptr;
	m_shadowprojnearEdit = nullptr;
	m_shadowprojfarLabel = nullptr;
	m_shadowprojfarEdit = nullptr;
	m_shadowotherLabel = nullptr;
	m_shadowothersp1 = nullptr;
	m_shadowothercolorLabel = nullptr;
	m_shadowothercolorSlider = nullptr;
	m_shadowothersp2 = nullptr;
	m_shadowotherbiasLabel = nullptr;
	m_shadowotherbiasSlider = nullptr;
	m_shadowothersp3 = nullptr;
	m_shadowothermultLabel = nullptr;
	m_shadowothermultSlider = nullptr;
	m_shadowspacerLabel = nullptr;
	m_shadowapplysp = nullptr;
	m_shadowapplyB = nullptr;
	m_shadowinitB = nullptr;

	m_shadowspacerLabel001 = nullptr;
	m_shadowspacerLabel002 = nullptr;
	m_shadowspacerLabel003 = nullptr;

	m_SetCamera3DFromEyePos = nullptr;
}

int CShadowDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

int CShadowDlg::SetFunctions(
	void (*srcSetCamera3DFromEyePos)()
)
{
	if (!srcSetCamera3DFromEyePos) {
		_ASSERT(0);
		return 1;
	}

	m_SetCamera3DFromEyePos = srcSetCamera3DFromEyePos;

	return 0;
}


//void CShadowDlg::SetModel(CModel* srcmodel, CShaderTypeParams* srcshadertypeparams)
//{
//	m_model = srcmodel;
//	CreateSkyParamsWnd();
//	if (srcshadertypeparams) {
//		ParamsToDlg(nullptr, srcshadertypeparams);
//	}
//
//	if (m_dlgWnd && m_dlgWnd->getVisible()) {
//		if (m_skyst_Sc) {
//			m_skyst_Sc->autoResize();
//		}
//		m_dlgWnd->callRewrite();
//	}
//}


void CShadowDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateShadowWnd();//作成済は０リターン

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


int CShadowDlg::CreateShadowWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("ShadowDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("ShadowDlg"),	//タイトル
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

		m_shadowslotsp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowslotsp) {
			_ASSERT(0);
			abort();
		}
		m_shadowenableChk = new OWP_CheckBoxA(L"Enable Shadow", g_enableshadow, labelheight, false);
		if (!m_shadowenableChk) {
			_ASSERT(0);
			abort();
		}
		m_shadowslotCombo = new OWP_ComboBoxA(L"ShadowSlotCombo", labelheight);
		if (!m_shadowslotCombo) {
			_ASSERT(0);
			abort();
		}
		int slotindex;
		for (slotindex = 0; slotindex < 8; slotindex++) {
			char strslot[128] = { 0 };
			sprintf_s(strslot, 128, "Slot_%d", slotindex);
			m_shadowslotCombo->addString(strslot);
		}
		m_shadowslotCombo->setSelectedCombo(g_shadowmap_slotno);

		m_shadowvsmsp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowvsmsp) {
			_ASSERT(0);
			abort();
		}
		m_shadowvsmChk = new OWP_CheckBoxA(L"Variance Shadow Maps", g_VSMflag, labelheight, false);
		if (!m_shadowvsmChk) {
			_ASSERT(0);
			abort();
		}
		m_shadowvsmblurChk = new OWP_CheckBoxA(L"Blur Shadow", g_blurShadow, labelheight, false);
		if (!m_shadowvsmblurChk) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposLabel = new OWP_Label(L"Shadow Camera Pos(CameraPos +)", labelheight);
		if (!m_shadowcamposLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposupsp0 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamposupsp0) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposupsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamposupsp1) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposupsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamposupsp2) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposupLabel = new OWP_Label(L"plus UP", labelheight);
		if (!m_shadowcamposupLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposupEdit = new OWP_EditBox(true, L"plus Up Edit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_shadowcamposupEdit) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposdistsp0 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamposdistsp0) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposdistsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamposdistsp1) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposdistsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamposdistsp2) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposdistLabel = new OWP_Label(L"dist Scale", labelheight);
		if (!m_shadowcamposdistLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposdistEdit = new OWP_EditBox(true, L"dist Scale Edit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_shadowcamposdistEdit) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamposdistxLabel = new OWP_Label(L"X CameraDist", labelheight);
		if (!m_shadowcamposdistxLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirLabel = new OWP_Label(L"Shadow Camera Direction", labelheight);
		if (!m_shadowcamdirLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp0 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp0) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp1) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp2) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp3) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp4 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp4) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp5 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp5) {
			_ASSERT(0);
			abort();
		}
		m_shadowcamdirsp6 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowcamdirsp6) {
			_ASSERT(0);
			abort();
		}
		int dirindex;
		for (dirindex = 0; dirindex < 8; dirindex++) {
			WCHAR strdir[32] = { 0L };
			swprintf_s(strdir, 32, L"%d", (dirindex + 1));
			m_shadowcamdirChk[dirindex] = new OWP_CheckBoxA(strdir, false, labelheight, true);
			if (!m_shadowcamdirChk[dirindex]) {
				_ASSERT(0);
				abort();
			}
		}
		m_shadowprojLabel = new OWP_Label(L"Projection of ShadowCamera", labelheight);
		if (!m_shadowprojLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojfovsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shadowprojfovsp) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojfovLabel = new OWP_Label(L"FOV(deg)", labelheight);
		if (!m_shadowprojfovLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojfovSlider = new OWP_Slider(g_shadowmap_fov[g_shadowmap_slotno], 60.0, 10.0, labelheight);
		if (!m_shadowprojfovSlider) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojnearfarsp0 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowprojnearfarsp0) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojnearfarsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowprojnearfarsp1) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojnearfarsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowprojnearfarsp2) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojnearLabel = new OWP_Label(L"near", labelheight);
		if (!m_shadowprojnearLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojnearEdit = new OWP_EditBox(true, L"near Edit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_shadowprojnearEdit) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojfarLabel = new OWP_Label(L"far", labelheight);
		if (!m_shadowprojfarLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowprojfarEdit = new OWP_EditBox(true, L"far Edit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_shadowprojfarEdit) {
			_ASSERT(0);
			abort();
		}
		m_shadowotherLabel = new OWP_Label(L"Other", labelheight);
		if (!m_shadowotherLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowothersp1 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shadowothersp1) {
			_ASSERT(0);
			abort();
		}
		m_shadowothercolorLabel = new OWP_Label(L"Color", labelheight);
		if (!m_shadowothercolorLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowothercolorSlider = new OWP_Slider(g_shadowmap_color[g_shadowmap_slotno], 2.0, 0.0, labelheight);
		if (!m_shadowothercolorSlider) {
			_ASSERT(0);
			abort();
		}
		m_shadowothersp2 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shadowothersp2) {
			_ASSERT(0);
			abort();
		}
		m_shadowotherbiasLabel = new OWP_Label(L"Bias", labelheight);
		if (!m_shadowotherbiasLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowotherbiasSlider = new OWP_Slider(g_shadowmap_bias[g_shadowmap_slotno], 0.06, 0.0);
		if (!m_shadowotherbiasSlider) {
			_ASSERT(0);
			abort();
		}
		m_shadowothersp3 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shadowothersp3) {
			_ASSERT(0);
			abort();
		}
		m_shadowothermultLabel = new OWP_Label(L"SceneMult", labelheight);
		if (!m_shadowothermultLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowothermultSlider = new OWP_Slider(g_shadowmap_projscale[g_shadowmap_slotno], 10.0, 0.1);
		if (!m_shadowothermultSlider) {
			_ASSERT(0);
			abort();
		}
		m_shadowspacerLabel = new OWP_Label(L"     ", 24);
		if (!m_shadowspacerLabel) {
			_ASSERT(0);
			abort();
		}
		m_shadowapplysp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_shadowapplysp) {
			_ASSERT(0);
			abort();
		}
		m_shadowapplyB = new OWP_Button(L"Apply(適用)", 38);
		if (!m_shadowapplyB) {
			_ASSERT(0);
			abort();
		}
		m_shadowapplyB->setTextColor(RGB(168, 129, 129));
		m_shadowinitB = new OWP_Button(L"InitParams", 38);
		if (!m_shadowinitB) {
			_ASSERT(0);
			abort();
		}
		m_shadowspacerLabel001 = new OWP_Label(L"     ", 32);
		if (!m_shadowspacerLabel001) {
			_ASSERT(0);
			abort();
		}
		m_shadowspacerLabel002 = new OWP_Label(L"     ", 32);
		if (!m_shadowspacerLabel002) {
			_ASSERT(0);
			abort();
		}
		m_shadowspacerLabel003 = new OWP_Label(L"     ", 32);
		if (!m_shadowspacerLabel003) {
			_ASSERT(0);
			abort();
		}


		m_dlgWnd->addParts(*m_shadowslotsp);
		m_shadowslotsp->addParts1(*m_shadowenableChk);
		m_shadowslotsp->addParts2(*m_shadowslotCombo);
		m_dlgWnd->addParts(*m_shadowvsmsp);
		m_shadowvsmsp->addParts1(*m_shadowvsmChk);
		m_shadowvsmsp->addParts2(*m_shadowvsmblurChk);
		m_dlgWnd->addParts(*m_shadowcamposLabel);
		m_dlgWnd->addParts(*m_shadowcamposupsp0);
		m_shadowcamposupsp0->addParts1(*m_shadowcamposupsp1);
		m_shadowcamposupsp0->addParts2(*m_shadowcamposupsp2);
		m_shadowcamposupsp1->addParts1(*m_shadowcamposupLabel);
		m_shadowcamposupsp1->addParts2(*m_shadowcamposupEdit);
		m_dlgWnd->addParts(*m_shadowcamposdistsp0);
		m_shadowcamposdistsp0->addParts1(*m_shadowcamposdistsp1);
		m_shadowcamposdistsp0->addParts2(*m_shadowcamposdistsp2);
		m_shadowcamposdistsp1->addParts1(*m_shadowcamposdistLabel);
		m_shadowcamposdistsp1->addParts2(*m_shadowcamposdistEdit);
		m_shadowcamposdistsp2->addParts1(*m_shadowcamposdistxLabel);
		m_dlgWnd->addParts(*m_shadowspacerLabel001);//
		m_dlgWnd->addParts(*m_shadowcamdirLabel);
		m_dlgWnd->addParts(*m_shadowcamdirsp0);
		m_shadowcamdirsp0->addParts1(*m_shadowcamdirsp1);
		m_shadowcamdirsp0->addParts2(*m_shadowcamdirsp2);
		m_shadowcamdirsp1->addParts1(*m_shadowcamdirsp3);
		m_shadowcamdirsp1->addParts2(*m_shadowcamdirsp4);
		m_shadowcamdirsp2->addParts1(*m_shadowcamdirsp5);
		m_shadowcamdirsp2->addParts2(*m_shadowcamdirsp6);
		m_shadowcamdirsp3->addParts1(*m_shadowcamdirChk[0]);
		m_shadowcamdirsp3->addParts2(*m_shadowcamdirChk[1]);
		m_shadowcamdirsp4->addParts1(*m_shadowcamdirChk[2]);
		m_shadowcamdirsp4->addParts2(*m_shadowcamdirChk[3]);
		m_shadowcamdirsp5->addParts1(*m_shadowcamdirChk[4]);
		m_shadowcamdirsp5->addParts2(*m_shadowcamdirChk[5]);
		m_shadowcamdirsp6->addParts1(*m_shadowcamdirChk[6]);
		m_shadowcamdirsp6->addParts2(*m_shadowcamdirChk[7]);
		m_dlgWnd->addParts(*m_shadowspacerLabel002);//
		m_dlgWnd->addParts(*m_shadowprojLabel);
		m_dlgWnd->addParts(*m_shadowprojfovsp);
		m_shadowprojfovsp->addParts1(*m_shadowprojfovLabel);
		m_shadowprojfovsp->addParts2(*m_shadowprojfovSlider);
		m_dlgWnd->addParts(*m_shadowprojnearfarsp0);
		m_shadowprojnearfarsp0->addParts1(*m_shadowprojnearfarsp1);
		m_shadowprojnearfarsp0->addParts2(*m_shadowprojnearfarsp2);
		m_shadowprojnearfarsp1->addParts1(*m_shadowprojnearLabel);
		m_shadowprojnearfarsp1->addParts2(*m_shadowprojnearEdit);
		m_shadowprojnearfarsp2->addParts1(*m_shadowprojfarLabel);
		m_shadowprojnearfarsp2->addParts2(*m_shadowprojfarEdit);
		m_dlgWnd->addParts(*m_shadowspacerLabel003);//
		m_dlgWnd->addParts(*m_shadowotherLabel);
		m_dlgWnd->addParts(*m_shadowothersp1);
		m_shadowothersp1->addParts1(*m_shadowothercolorLabel);
		m_shadowothersp1->addParts2(*m_shadowothercolorSlider);
		m_dlgWnd->addParts(*m_shadowothersp2);
		m_shadowothersp2->addParts1(*m_shadowotherbiasLabel);
		m_shadowothersp2->addParts2(*m_shadowotherbiasSlider);
		m_dlgWnd->addParts(*m_shadowothersp3);
		m_shadowothersp3->addParts1(*m_shadowothermultLabel);
		m_shadowothersp3->addParts2(*m_shadowothermultSlider);
		m_dlgWnd->addParts(*m_shadowspacerLabel);
		m_dlgWnd->addParts(*m_shadowapplysp);
		m_shadowapplysp->addParts1(*m_shadowapplyB);
		m_shadowapplysp->addParts2(*m_shadowinitB);





		//############
		//ComboBox
		//############
		m_shadowslotCombo->setButtonListener([=, this]() {
			int comboid = m_shadowslotCombo->trackPopUpMenu();
			g_shadowmap_slotno = comboid;

			ParamsToDlg();
			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
			}
			});

		//###########
		//CheckBox
		//###########
		m_shadowenableChk->setButtonListener([=, this]() {
			bool value = m_shadowenableChk->getValue();
			g_enableshadow = value;
			});
		m_shadowvsmChk->setButtonListener([=, this]() {
			bool value = m_shadowvsmChk->getValue();
			g_VSMflag = value;
			});
		m_shadowvsmblurChk->setButtonListener([=, this]() {
			bool value = m_shadowvsmblurChk->getValue();
			g_blurShadow = value;
			});

		int dirindex2;
		for (dirindex2 = 0; dirindex2 < 8; dirindex2++) {
			m_shadowcamdirChk[dirindex2]->setButtonListener([=, this]() {
				bool value = m_shadowcamdirChk[dirindex2]->getValue();
				g_shadowmap_lightdir[g_shadowmap_slotno] = (dirindex2 + 1);

				CheckShadowDirectionButton(g_shadowmap_lightdir[g_shadowmap_slotno]);
				if (m_SetCamera3DFromEyePos) {
					(this->m_SetCamera3DFromEyePos)();
				}
				});
		}

		//##########
		//Slider
		//##########
		m_shadowprojfovSlider->setCursorListener([=, this]() {
			double value = m_shadowprojfovSlider->getValue();
			g_shadowmap_fov[g_shadowmap_slotno] = (float)value;
			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
			}
			});
		m_shadowothercolorSlider->setCursorListener([=, this]() {
			double value = m_shadowothercolorSlider->getValue();
			g_shadowmap_color[g_shadowmap_slotno] = (float)value;
			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
			}
			});
		m_shadowotherbiasSlider->setCursorListener([=, this]() {
			double value = m_shadowotherbiasSlider->getValue();
			g_shadowmap_bias[g_shadowmap_slotno] = (float)value;
			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
			}
			});
		m_shadowothermultSlider->setCursorListener([=, this]() {
			double value = m_shadowothermultSlider->getValue();
			g_shadowmap_projscale[g_shadowmap_slotno] = (float)value;
			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
			}
			});



		m_shadowapplyB->setButtonListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			m_shadowcamposupEdit->getName(streditbox, 256);
			float tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= -50000.0f) && (tempeditvalue <= 50000.0f)) {
				g_shadowmap_plusup[g_shadowmap_slotno] = tempeditvalue;
			}
			else {
				::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : plusup", L"Invalid Value", MB_OK);
			}

			m_shadowcamposdistEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= -50000.0f) && (tempeditvalue <= 50000.0f)) {
				g_shadowmap_distscale[g_shadowmap_slotno] = tempeditvalue;
			}
			else {
				::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : plusright", L"Invalid Value", MB_OK);
			}

			m_shadowprojnearEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_shadowmap_near[g_shadowmap_slotno] = tempeditvalue;
			}
			else {
				::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : near", L"Invalid Value", MB_OK);
			}

			m_shadowprojfarEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_shadowmap_far[g_shadowmap_slotno] = tempeditvalue;
			}
			else {
				::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : near", L"Invalid Value", MB_OK);
			}

			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
			}
			});
		m_shadowinitB->setButtonListener([=, this]() {
			g_shadowmap_fov[g_shadowmap_slotno] = 60.0f;
			g_shadowmap_projscale[g_shadowmap_slotno] = 1.0f;
			g_shadowmap_near[g_shadowmap_slotno] = 50.0f;
			g_shadowmap_far[g_shadowmap_slotno] = 2000.0f;
			g_shadowmap_color[g_shadowmap_slotno] = 0.5f;
			g_shadowmap_bias[g_shadowmap_slotno] = 0.0010f;
			g_shadowmap_plusup[g_shadowmap_slotno] = 300.0f;
			g_shadowmap_distscale[g_shadowmap_slotno] = 1.0f;
			g_shadowmap_lightdir[g_shadowmap_slotno] = 1;

			ParamsToDlg();
			if (m_SetCamera3DFromEyePos) {
				(this->m_SetCamera3DFromEyePos)();
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

int CShadowDlg::ParamsToDlg()
{
	WCHAR strdlg[256] = { 0L };


	if ((g_shadowmap_slotno < 0) || (g_shadowmap_slotno >= SHADOWSLOTNUM)) {
		_ASSERT(0);
		g_shadowmap_slotno = 0;
	}


	if (m_dlgWnd) {

		//#########
		//ComboBox
		//#########
		if (m_shadowslotCombo) {
			m_shadowslotCombo->setSelectedCombo(g_shadowmap_slotno);
		}

		//#########
		//CheckBox
		//#########
		if (m_shadowenableChk) {
			m_shadowenableChk->setValue(g_enableshadow, false);
		}
		if (m_shadowvsmChk) {
			m_shadowvsmChk->setValue(g_VSMflag, false);
		}
		if (m_shadowvsmblurChk) {
			m_shadowvsmblurChk->setValue(g_blurShadow, false);
		}

		CheckShadowDirectionButton(g_shadowmap_lightdir[g_shadowmap_slotno]);

		//########
		//EditBox
		//########
		if (m_shadowcamposupEdit) {
			swprintf_s(strdlg, 256, L"%.1f", g_shadowmap_plusup[g_shadowmap_slotno]);
			m_shadowcamposupEdit->setNameString(strdlg);
		}
		if (m_shadowcamposdistEdit) {
			swprintf_s(strdlg, 256, L"%.1f", g_shadowmap_distscale[g_shadowmap_slotno]);
			m_shadowcamposdistEdit->setNameString(strdlg);
		}
		if (m_shadowprojnearEdit) {
			swprintf_s(strdlg, 256, L"%.1f", g_shadowmap_near[g_shadowmap_slotno]);
			m_shadowprojnearEdit->setNameString(strdlg);
		}
		if (m_shadowprojfarEdit) {
			swprintf_s(strdlg, 256, L"%.1f", g_shadowmap_far[g_shadowmap_slotno]);
			m_shadowprojfarEdit->setNameString(strdlg);
		}

		//#######
		//Slider
		//#######
		if (m_shadowprojfovSlider) {
			m_shadowprojfovSlider->setValue(g_shadowmap_fov[g_shadowmap_slotno], false);
		}
		if (m_shadowothercolorSlider) {
			m_shadowothercolorSlider->setValue(g_shadowmap_color[g_shadowmap_slotno], false);
		}
		if (m_shadowotherbiasSlider) {
			m_shadowotherbiasSlider->setValue(g_shadowmap_bias[g_shadowmap_slotno], false);
		}
		if (m_shadowothermultSlider) {
			m_shadowothermultSlider->setValue(g_shadowmap_projscale[g_shadowmap_slotno], false);
		}

		m_dlgWnd->callRewrite();
	}
	return 0;

}

void CShadowDlg::CheckShadowDirectionButton(int srcshadowdir)
{
	//####################################################
	//Shadowプレートメニューから呼び出すShadowParamsダイアログ用
	//####################################################

	int dirindex;
	for (dirindex = 0; dirindex < 8; dirindex++) {
		if (!m_shadowcamdirChk[dirindex]) {
			_ASSERT(0);
			return;
		}
	}

	switch (srcshadowdir) {
	case 1:
		m_shadowcamdirChk[0]->setValue(true, false);

		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 2:
		m_shadowcamdirChk[1]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 3:
		m_shadowcamdirChk[2]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 4:
		m_shadowcamdirChk[3]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 5:
		m_shadowcamdirChk[4]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 6:
		m_shadowcamdirChk[5]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 7:
		m_shadowcamdirChk[6]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	case 8:
		m_shadowcamdirChk[7]->setValue(true, false);

		m_shadowcamdirChk[0]->setValue(false, false);
		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		break;
	default:
		m_shadowcamdirChk[0]->setValue(true, false);

		m_shadowcamdirChk[1]->setValue(false, false);
		m_shadowcamdirChk[2]->setValue(false, false);
		m_shadowcamdirChk[3]->setValue(false, false);
		m_shadowcamdirChk[4]->setValue(false, false);
		m_shadowcamdirChk[5]->setValue(false, false);
		m_shadowcamdirChk[6]->setValue(false, false);
		m_shadowcamdirChk[7]->setValue(false, false);
		break;
	}
}

const HWND CShadowDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CShadowDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CShadowDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

