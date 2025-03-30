#include "stdafx.h"

#include <FogDlg.h>
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
// CFogDlg

CFogDlg::CFogDlg()
{
	InitParams();
}

CFogDlg::~CFogDlg()
{
	DestroyObjs();
}
	
int CFogDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_fogslotCombo) {
		delete m_fogslotCombo;
		m_fogslotCombo = nullptr;
	}
	if (m_fogkindsp1) {
		delete m_fogkindsp1;
		m_fogkindsp1 = nullptr;
	}
	if (m_fogkindsp2) {
		delete m_fogkindsp2;
		m_fogkindsp2 = nullptr;
	}
	if (m_fogkindsp3) {
		delete m_fogkindsp3;
		m_fogkindsp3 = nullptr;
	}
	if (m_fogkindnoChk) {
		delete m_fogkindnoChk;
		m_fogkindnoChk = nullptr;
	}
	if (m_fogkinddistChk) {
		delete m_fogkinddistChk;
		m_fogkinddistChk = nullptr;
	}
	if (m_fogkindheightChk) {
		delete m_fogkindheightChk;
		m_fogkindheightChk = nullptr;
	}
	if (m_fogspacerLabel1) {
		delete m_fogspacerLabel1;
		m_fogspacerLabel1 = nullptr;
	}
	if (m_fogdistLabel) {
		delete m_fogdistLabel;
		m_fogdistLabel = nullptr;
	}
	if (m_fogdistsp1) {
		delete m_fogdistsp1;
		m_fogdistsp1 = nullptr;
	}
	if (m_fogdistsp2) {
		delete m_fogdistsp2;
		m_fogdistsp2 = nullptr;
	}
	if (m_fogdistsp3) {
		delete m_fogdistsp3;
		m_fogdistsp3 = nullptr;
	}
	if (m_fogdistnearLabel) {
		delete m_fogdistnearLabel;
		m_fogdistnearLabel = nullptr;
	}
	if (m_fogdistnearEdit) {
		delete m_fogdistnearEdit;
		m_fogdistnearEdit = nullptr;
	}
	if (m_fogdistfarLabel) {
		delete m_fogdistfarLabel;
		m_fogdistfarLabel = nullptr;
	}
	if (m_fogdistfarEdit) {
		delete m_fogdistfarEdit;
		m_fogdistfarEdit = nullptr;
	}
	if (m_fogdistsp4) {
		delete m_fogdistsp4;
		m_fogdistsp4 = nullptr;
	}
	if (m_fogdistColor) {
		delete m_fogdistColor;
		m_fogdistColor = nullptr;
	}
	if (m_fogdistSlider) {
		delete m_fogdistSlider;
		m_fogdistSlider = nullptr;
	}
	if (m_fogspacerLabel2) {
		delete m_fogspacerLabel2;
		m_fogspacerLabel2 = nullptr;
	}
	if (m_fogheightsp1) {
		delete m_fogheightsp1;
		m_fogheightsp1 = nullptr;
	}
	if (m_fogheightsp2) {
		delete m_fogheightsp2;
		m_fogheightsp2 = nullptr;
	}
	if (m_fogheightsp3) {
		delete m_fogheightsp3;
		m_fogheightsp3 = nullptr;
	}
	if (m_fogheightminLabel) {
		delete m_fogheightminLabel;
		m_fogheightminLabel = nullptr;
	}
	if (m_fogheightminEdit) {
		delete m_fogheightminEdit;
		m_fogheightminEdit = nullptr;
	}
	if (m_fogheightmaxLabel) {
		delete m_fogheightmaxLabel;
		m_fogheightmaxLabel = nullptr;
	}
	if (m_fogheightmaxEdit) {
		delete m_fogheightmaxEdit;
		m_fogheightmaxEdit = nullptr;
	}
	if (m_fogheightsp4) {
		delete m_fogheightsp4;
		m_fogheightsp4 = nullptr;
	}
	if (m_fogheightColor) {
		delete m_fogheightColor;
		m_fogheightColor = nullptr;
	}
	if (m_fogheightSlider) {
		delete m_fogheightSlider;
		m_fogheightSlider = nullptr;
	}
	//if (m_fogspacerLabel3) {
	//	delete m_fogspacerLabel3;
	//	m_fogspacerLabel3 = nullptr;
	//}
	//if (m_fogapplysp) {
	//	delete m_fogapplysp;
	//	m_fogapplysp = nullptr;
	//}
	//if (m_fogapplyB) {
	//	delete m_fogapplyB;
	//	m_fogapplyB = nullptr;
	//}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CFogDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_dlgWnd = nullptr;

	m_fogslotCombo = nullptr;
	m_fogkindsp1 = nullptr;
	m_fogkindsp2 = nullptr;
	m_fogkindsp3 = nullptr;
	m_fogkindnoChk = nullptr;
	m_fogkinddistChk = nullptr;
	m_fogkindheightChk = nullptr;
	m_fogspacerLabel1 = nullptr;
	m_fogdistLabel = nullptr;
	m_fogdistsp1 = nullptr;
	m_fogdistsp2 = nullptr;
	m_fogdistsp3 = nullptr;
	m_fogdistnearLabel = nullptr;
	m_fogdistnearEdit = nullptr;
	m_fogdistfarLabel = nullptr;
	m_fogdistfarEdit = nullptr;
	m_fogdistsp4 = nullptr;
	m_fogdistColor = nullptr;
	m_fogdistSlider = nullptr;
	m_fogspacerLabel2 = nullptr;
	m_fogheightsp1 = nullptr;
	m_fogheightsp2 = nullptr;
	m_fogheightsp3 = nullptr;
	m_fogheightminLabel = nullptr;
	m_fogheightminEdit = nullptr;
	m_fogheightmaxLabel = nullptr;
	m_fogheightmaxEdit = nullptr;
	m_fogheightsp4 = nullptr;
	m_fogheightColor = nullptr;
	m_fogheightSlider = nullptr;
	//m_fogspacerLabel3 = nullptr;
	//m_fogapplysp = nullptr;
	//m_fogapplyB = nullptr;

}

int CFogDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CFogDlg::SetFunctions(
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


//void CFogDlg::SetModel(CModel* srcmodel, int srccurboneno, std::map<CModel*, int> srcrgdindexmap)
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


void CFogDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateFogWnd();//作成済は０リターン

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


int CFogDlg::CreateFogWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("FogDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("FogDlg"),	//タイトル
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

		m_fogslotCombo = new OWP_ComboBoxA(L"FogSlot", labelheight);
		if (!m_fogslotCombo) {
			_ASSERT(0);
			abort();
		}
		int slotindex;
		for (slotindex = 0; slotindex < 8; slotindex++) {
			char strslot[128] = { 0 };
			sprintf_s(strslot, 128, "Slot_%d", slotindex);
			m_fogslotCombo->addString(strslot);
		}
		m_fogslotCombo->setSelectedCombo(g_lightSlot);

		m_fogkindsp1 = new OWP_Separator(m_dlgWnd, true, 0.667, true);
		if (!m_fogkindsp1) {
			_ASSERT(0);
			abort();
		}
		m_fogkindsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogkindsp2) {
			_ASSERT(0);
			abort();
		}
		m_fogkindsp3 = new OWP_Separator(m_dlgWnd, true, 0.95, true);
		if (!m_fogkindsp3) {
			_ASSERT(0);
			abort();
		}
		m_fogkindnoChk = new OWP_CheckBoxA(L"NoFog", false, labelheight, true);
		if (!m_fogkindnoChk) {
			_ASSERT(0);
			abort();
		}
		m_fogkinddistChk = new OWP_CheckBoxA(L"DistFog", false, labelheight, true);
		if (!m_fogkinddistChk) {
			_ASSERT(0);
			abort();
		}
		m_fogkindheightChk = new OWP_CheckBoxA(L"HeightFog", false, labelheight, true);
		if (!m_fogkindheightChk) {
			_ASSERT(0);
			abort();
		}
		m_fogspacerLabel1 = new OWP_Label(L"     ", 34);
		if (!m_fogspacerLabel1) {
			_ASSERT(0);
			abort();
		}
		m_fogdistLabel = new OWP_Label(L"Dist Fog Params", labelheight);
		if (!m_fogdistLabel) {
			_ASSERT(0);
			abort();
		}
		m_fogdistsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogdistsp1) {
			_ASSERT(0);
			abort();
		}
		m_fogdistsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogdistsp2) {
			_ASSERT(0);
			abort();
		}
		m_fogdistsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogdistsp3) {
			_ASSERT(0);
			abort();
		}
		m_fogdistnearLabel = new OWP_Label(L"near", labelheight);
		if (!m_fogdistnearLabel) {
			_ASSERT(0);
			abort();
		}
		m_fogdistnearEdit = new OWP_EditBox(true, L"nearEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_fogdistnearEdit) {
			_ASSERT(0);
			abort();
		}
		m_fogdistfarLabel = new OWP_Label(L"far", labelheight);
		if (!m_fogdistfarLabel) {
			_ASSERT(0);
			abort();
		}
		m_fogdistfarEdit = new OWP_EditBox(true, L"nearEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_fogdistfarEdit) {
			_ASSERT(0);
			abort();
		}
		m_fogdistsp4 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogdistsp4) {
			_ASSERT(0);
			abort();
		}
		m_fogdistColor = new OWP_ColorBox(RGB(0, 0, 0), labelheight);
		if (!m_fogdistColor) {
			_ASSERT(0);
			abort();
		}
		m_fogdistSlider = new OWP_Slider(g_fogparams[g_fogindex].GetDistRate(), 1.0, 0.0);
		if (!m_fogdistSlider) {
			_ASSERT(0);
			abort();
		}
		m_fogspacerLabel2 = new OWP_Label(L"     ", 34);
		if (!m_fogspacerLabel2) {
			_ASSERT(0);
			abort();
		}
		m_fogheightsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogheightsp1) {
			_ASSERT(0);
			abort();
		}
		m_fogheightsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogheightsp2) {
			_ASSERT(0);
			abort();
		}
		m_fogheightsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogheightsp3) {
			_ASSERT(0);
			abort();
		}
		m_fogheightminLabel = new OWP_Label(L"min", labelheight);
		if (!m_fogheightminLabel) {
			_ASSERT(0);
			abort();
		}
		m_fogheightminEdit = new OWP_EditBox(true, L"minEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_fogheightminEdit) {
			_ASSERT(0);
			abort();
		}
		m_fogheightmaxLabel = new OWP_Label(L"max", labelheight);
		if (!m_fogheightmaxLabel) {
			_ASSERT(0);
			abort();
		}
		m_fogheightmaxEdit = new OWP_EditBox(true, L"maxEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_fogheightmaxEdit) {
			_ASSERT(0);
			abort();
		}
		m_fogheightsp4 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_fogheightsp4) {
			_ASSERT(0);
			abort();
		}
		m_fogheightColor = new OWP_ColorBox(RGB(0, 0, 0), labelheight);
		if (!m_fogheightColor) {
			_ASSERT(0);
			abort();
		}
		m_fogheightSlider = new OWP_Slider(g_fogparams[g_fogindex].GetHeightRate(), 1.0, 0.0);
		if (!m_fogheightSlider) {
			_ASSERT(0);
			abort();
		}
		//m_fogspacerLabel3 = new OWP_Label(L"     ", 34);
		//if (!m_fogspacerLabel3) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_fogapplysp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		//if (!m_fogapplysp) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_fogapplyB = new OWP_Button(L"Apply(適用)", labelheight);
		//if (!m_fogapplyB) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_fogapplyB->setTextColor(RGB(168, 129, 129));


		m_dlgWnd->addParts(*m_fogslotCombo);
		m_dlgWnd->addParts(*m_fogkindsp1);
		m_fogkindsp1->addParts1(*m_fogkindsp2);
		m_fogkindsp1->addParts2(*m_fogkindsp3);
		m_fogkindsp2->addParts1(*m_fogkindnoChk);
		m_fogkindsp2->addParts2(*m_fogkinddistChk);
		m_fogkindsp3->addParts1(*m_fogkindheightChk);
		m_dlgWnd->addParts(*m_fogspacerLabel1);
		m_dlgWnd->addParts(*m_fogdistLabel);
		m_dlgWnd->addParts(*m_fogdistsp1);
		m_fogdistsp1->addParts1(*m_fogdistsp2);
		m_fogdistsp1->addParts2(*m_fogdistsp3);
		m_fogdistsp2->addParts1(*m_fogdistnearLabel);
		m_fogdistsp2->addParts2(*m_fogdistnearEdit);
		m_fogdistsp3->addParts1(*m_fogdistfarLabel);
		m_fogdistsp3->addParts2(*m_fogdistfarEdit);
		m_dlgWnd->addParts(*m_fogdistsp4);
		m_fogdistsp4->addParts1(*m_fogdistColor);
		m_fogdistsp4->addParts2(*m_fogdistSlider);
		m_dlgWnd->addParts(*m_fogspacerLabel2);
		m_dlgWnd->addParts(*m_fogheightsp1);
		m_fogheightsp1->addParts1(*m_fogheightsp2);
		m_fogheightsp1->addParts2(*m_fogheightsp3);
		m_fogheightsp2->addParts1(*m_fogheightminLabel);
		m_fogheightsp2->addParts2(*m_fogheightminEdit);
		m_fogheightsp3->addParts1(*m_fogheightmaxLabel);
		m_fogheightsp3->addParts2(*m_fogheightmaxEdit);
		m_dlgWnd->addParts(*m_fogheightsp4);
		m_fogheightsp4->addParts1(*m_fogheightColor);//g_fogparams[g_fogindex].GetHeightColor()
		m_fogheightsp4->addParts2(*m_fogheightSlider);//g_fogparams[g_fogindex].GetHeightRate()
		//m_dlgWnd->addParts(*m_fogspacerLabel3);
		//m_dlgWnd->addParts(*m_fogapplysp);
		//m_fogapplysp->addParts2(*m_fogapplyB);



		m_fogslotCombo->setButtonListener([=, this]() {
			int comboid = m_fogslotCombo->trackPopUpMenu();
			if ((comboid >= 0) && (comboid < FOGSLOTNUM)) {
				g_fogindex = comboid;
				ParamsToDlg();
			}
			});
		m_fogkindnoChk->setButtonListener([=, this]() {
			CheckFogKindParamsButton(0);
			g_fogparams[g_fogindex].SetFogKind(0);
			});
		m_fogkinddistChk->setButtonListener([=, this]() {
			CheckFogKindParamsButton(1);
			g_fogparams[g_fogindex].SetFogKind(1);
			});
		m_fogkindheightChk->setButtonListener([=, this]() {
			CheckFogKindParamsButton(2);
			g_fogparams[g_fogindex].SetFogKind(2);
			});

		m_fogdistColor->setButtonListener([=, this]() {
			COLORREF choosedcolor = m_fogdistColor->getColor();
			float fr, fg, fb;
			fr = (float)((double)GetRValue(choosedcolor) / 255.0);
			fr = (float)fmin(1.0f, fr);
			fr = (float)fmax(0.0f, fr);
			fg = (float)((double)GetGValue(choosedcolor) / 255.0);
			fg = (float)fmin(1.0f, fg);
			fg = (float)fmax(0.0f, fg);
			fb = (float)((double)GetBValue(choosedcolor) / 255.0);
			fb = (float)fmin(1.0f, fb);
			fb = (float)fmax(0.0f, fb);

			ChaVector4 setcol;
			setcol.SetParams(fr, fg, fb, 1.0);
			g_fogparams[g_fogindex].SetDistColor(setcol);
			m_fogdistColor->callRewrite();
			});
		m_fogheightColor->setButtonListener([=, this]() {
			COLORREF choosedcolor = m_fogheightColor->getColor();
			float fr, fg, fb;
			fr = (float)((double)GetRValue(choosedcolor) / 255.0);
			fr = (float)fmin(1.0f, fr);
			fr = (float)fmax(0.0f, fr);
			fg = (float)((double)GetGValue(choosedcolor) / 255.0);
			fg = (float)fmin(1.0f, fg);
			fg = (float)fmax(0.0f, fg);
			fb = (float)((double)GetBValue(choosedcolor) / 255.0);
			fb = (float)fmin(1.0f, fb);
			fb = (float)fmax(0.0f, fb);

			ChaVector4 setcol;
			setcol.SetParams(fr, fg, fb, 1.0);
			g_fogparams[g_fogindex].SetHeightColor(setcol);
			m_fogheightColor->callRewrite();
			});

		m_fogdistSlider->setCursorListener([=, this]() {
			double value = m_fogdistSlider->getValue();
			g_fogparams[g_fogindex].SetDistRate((float)value);
			});
		m_fogheightSlider->setCursorListener([=, this]() {
			double value = m_fogheightSlider->getValue();
			g_fogparams[g_fogindex].SetHeightRate((float)value);
			});

		//########
		//EditBox
		//########
		m_fogdistnearEdit->setExitDialogListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			float tempeditvalue;
			m_fogdistnearEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_fogparams[g_fogindex].SetDistNear(tempeditvalue);
			}
			else {
				//if (m_dlgWnd) {
				//	::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : Near", L"Invalid Value", MB_OK);
				//}
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_fogparams[g_fogindex].GetDistNear());
				m_fogdistnearEdit->setName(strnotchange);
			}
		});
		
		m_fogdistfarEdit->setExitDialogListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			float tempeditvalue;
			m_fogdistfarEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_fogparams[g_fogindex].SetDistFar(tempeditvalue);
			}
			else {
				//if (m_dlgWnd) {
				//	::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : Far", L"Invalid Value", MB_OK);
				//}
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_fogparams[g_fogindex].GetDistFar());
				m_fogdistfarEdit->setName(strnotchange);
			}
		});


		m_fogheightminEdit->setExitDialogListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			float tempeditvalue;
			m_fogheightminEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= -500000.0f) && (tempeditvalue <= 500000.0f)) {
				g_fogparams[g_fogindex].SetHeightLow(tempeditvalue);
			}
			else {
				//if (m_dlgWnd) {
				//	::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : Min Height", L"Invalid Value", MB_OK);
				//}
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_fogparams[g_fogindex].GetHeightLow());
				m_fogheightminEdit->setName(strnotchange);
			}
		});

		m_fogheightmaxEdit->setExitDialogListener([=, this]() {
			WCHAR streditbox[256] = { 0L };
			float tempeditvalue;
			m_fogheightmaxEdit->getName(streditbox, 256);
			tempeditvalue = (float)_wtof(streditbox);
			if ((tempeditvalue >= -500000.0f) && (tempeditvalue <= 500000.0f)) {
				g_fogparams[g_fogindex].SetHeightHigh(tempeditvalue);
			}
			else {
				//if (m_dlgWnd) {
				//	::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : Max Height", L"Invalid Value", MB_OK);
				//}
				WCHAR strnotchange[256] = { 0L };
				swprintf_s(strnotchange, 256, L"%f", g_fogparams[g_fogindex].GetHeightHigh());
				m_fogheightmaxEdit->setName(strnotchange);
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

int CFogDlg::ParamsToDlg()
{
	if (!m_dlgWnd) {
		_ASSERT(0);
		return 1;
	}

	if ((g_fogindex < 0) || (g_fogindex >= FOGSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}

	//#########
	//ComboBox
	//#########
	if (m_fogslotCombo) {
		m_fogslotCombo->setSelectedCombo(g_fogindex);
	}

	//#######
	//Button
	//#######
	CheckFogKindParamsButton(g_fogparams[g_fogindex].GetFogKind());

	//#######
	//Slider
	//#######
	if (m_fogdistSlider) {
		m_fogdistSlider->setValue((double)g_fogparams[g_fogindex].GetDistRate(), false);
	}
	if (m_fogheightSlider) {
		m_fogheightSlider->setValue((double)g_fogparams[g_fogindex].GetHeightRate(), false);
	}

	//#####
	//Text
	//#####
	WCHAR strdlg[256] = { 0L };
	swprintf_s(strdlg, 256, L"%.1f", g_fogparams[g_fogindex].GetDistNear());
	if (m_fogdistnearEdit) {
		m_fogdistnearEdit->setName(strdlg);
	}
	swprintf_s(strdlg, 256, L"%.1f", g_fogparams[g_fogindex].GetDistFar());
	if (m_fogdistfarEdit) {
		m_fogdistfarEdit->setName(strdlg);
	}

	swprintf_s(strdlg, 256, L"0.0");
	if (m_fogheightminEdit) {
		m_fogheightminEdit->setName(strdlg);
		//m_fogheightminEdit->setActive(false);//!!!!!
	}
	swprintf_s(strdlg, 256, L"%.1f", g_fogparams[g_fogindex].GetHeightHigh());
	if (m_fogheightmaxEdit) {
		m_fogheightmaxEdit->setName(strdlg);
	}

	//##########
	//ColorBar
	//##########
	{
		ChaVector4 fogcolor;
		fogcolor = g_fogparams[g_fogindex].GetDistColor();
		unsigned char ur, ug, ub;
		ur = (unsigned char)(fogcolor.x * 255.0f + 0.0001f);
		ur = min(255, max(0, ur));
		ug = (unsigned char)(fogcolor.y * 255.0f + 0.0001f);
		ug = min(255, max(0, ug));
		ub = (unsigned char)(fogcolor.z * 255.0f + 0.0001f);
		ub = min(255, max(0, ub));
		COLORREF col;
		col = RGB(ur, ug, ub);
		if (m_fogdistColor) {
			m_fogdistColor->setColor(col);
			m_fogdistColor->callRewrite();
		}
	}

	{
		ChaVector4 fogcolor = g_fogparams[g_fogindex].GetHeightColor();
		unsigned char ur, ug, ub;
		ur = (unsigned char)(fogcolor.x * 255.0f + 0.0001f);
		ur = min(255, max(0, ur));
		ug = (unsigned char)(fogcolor.y * 255.0f + 0.0001f);
		ug = min(255, max(0, ug));
		ub = (unsigned char)(fogcolor.z * 255.0f + 0.0001f);
		ub = min(255, max(0, ub));
		COLORREF col;
		col = RGB(ur, ug, ub);
		if (m_fogheightColor) {
			m_fogheightColor->setColor(col);
			m_fogheightColor->callRewrite();
		}
	}

	m_dlgWnd->callRewrite();

	return 0;
}

//int CFogDlg::DlgToParams()
//{
//}

void CFogDlg::CheckFogKindParamsButton(int srckind)
{
	if (!m_fogkindnoChk || !m_fogkinddistChk || !m_fogkindheightChk) {
		_ASSERT(0);
		return;
	}

	switch (srckind) {
	case 0:
		m_fogkindnoChk->setValue(true, false);
		m_fogkinddistChk->setValue(false, false);
		m_fogkindheightChk->setValue(false, false);
		break;
	case 1:
		m_fogkindnoChk->setValue(false, false);
		m_fogkinddistChk->setValue(true, false);
		m_fogkindheightChk->setValue(false, false);
		break;
	case 2:
		m_fogkindnoChk->setValue(false, false);
		m_fogkinddistChk->setValue(false, false);
		m_fogkindheightChk->setValue(true, false);
		break;
	default:
		_ASSERT(0);
		m_fogkindnoChk->setValue(true, false);
		m_fogkinddistChk->setValue(false, false);
		m_fogkindheightChk->setValue(false, false);
		break;
	}
}


const HWND CFogDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CFogDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CFogDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

