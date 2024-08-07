#include "stdafx.h"

#include <ProjLodDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
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
// CProjLodDlg

CProjLodDlg::CProjLodDlg()
{
	InitParams();
}

CProjLodDlg::~CProjLodDlg()
{
	DestroyObjs();
}
	
int CProjLodDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_lodprojLabel) {
		delete m_lodprojLabel;
		m_lodprojLabel = nullptr;
	}
	if (m_lodprojfovsp0) {
		delete m_lodprojfovsp0;
		m_lodprojfovsp0 = nullptr;
	}
	if (m_lodprojfovLabel) {
		delete m_lodprojfovLabel;
		m_lodprojfovLabel = nullptr;
	}
	if (m_lodprojfovSlider) {
		delete m_lodprojfovSlider;
		m_lodprojfovSlider = nullptr;
	}
	if (m_lodnearfarsp0) {
		delete m_lodnearfarsp0;
		m_lodnearfarsp0 = nullptr;
	}
	if (m_lodnearfarsp1) {
		delete m_lodnearfarsp1;
		m_lodnearfarsp1 = nullptr;
	}
	if (m_lodnearfarsp2) {
		delete m_lodnearfarsp2;
		m_lodnearfarsp2 = nullptr;
	}
	if (m_lodnearLabel) {
		delete m_lodnearLabel;
		m_lodnearLabel = nullptr;
	}
	if (m_lodnearEdit) {
		delete m_lodnearEdit;
		m_lodnearEdit = nullptr;
	}
	if (m_lodfarLabel) {
		delete m_lodfarLabel;
		m_lodfarLabel = nullptr;
	}
	if (m_lodfarEdit) {
		delete m_lodfarEdit;
		m_lodfarEdit = nullptr;
	}
	if (m_lodpickdistsp) {
		delete m_lodpickdistsp;
		m_lodpickdistsp = nullptr;
	}
	if (m_lodpickdistLabel) {
		delete m_lodpickdistLabel;
		m_lodpickdistLabel = nullptr;
	}
	if (m_lodpickdistSlider) {
		delete m_lodpickdistSlider;
		m_lodpickdistSlider = nullptr;
	}
	if (m_lodlevel2Label) {
		delete m_lodlevel2Label;
		m_lodlevel2Label = nullptr;
	}
	if (m_lodlevel2lod0sp) {
		delete m_lodlevel2lod0sp;
		m_lodlevel2lod0sp = nullptr;
	}
	if (m_lodlevel2lod0Label) {
		delete m_lodlevel2lod0Label;
		m_lodlevel2lod0Label = nullptr;
	}
	if (m_lodlevel2lod0Slider) {
		delete m_lodlevel2lod0Slider;
		m_lodlevel2lod0Slider = nullptr;
	}
	if (m_lodlevel2lod1sp) {
		delete m_lodlevel2lod1sp;
		m_lodlevel2lod1sp = nullptr;
	}
	if (m_lodlevel2lod1Label) {
		delete m_lodlevel2lod1Label;
		m_lodlevel2lod1Label = nullptr;
	}
	if (m_lodlevel2lod1Slider) {
		delete m_lodlevel2lod1Slider;
		m_lodlevel2lod1Slider = nullptr;
	}
	if (m_lodlevel3Label) {
		delete m_lodlevel3Label;
		m_lodlevel3Label = nullptr;
	}
	if (m_lodlevel3lod0sp) {
		delete m_lodlevel3lod0sp;
		m_lodlevel3lod0sp = nullptr;
	}
	if (m_lodlevel3lod0Label) {
		delete m_lodlevel3lod0Label;
		m_lodlevel3lod0Label = nullptr;
	}
	if (m_lodlevel3lod0Slider) {
		delete m_lodlevel3lod0Slider;
		m_lodlevel3lod0Slider = nullptr;
	}
	if (m_lodlevel3lod1sp) {
		delete m_lodlevel3lod1sp;
		m_lodlevel3lod1sp = nullptr;
	}
	if (m_lodlevel3lod1Label) {
		delete m_lodlevel3lod1Label;
		m_lodlevel3lod1Label = nullptr;
	}
	if (m_lodlevel3lod1Slider) {
		delete m_lodlevel3lod1Slider;
		m_lodlevel3lod1Slider = nullptr;
	}
	if (m_lodlevel3lod2sp) {
		delete m_lodlevel3lod2sp;
		m_lodlevel3lod2sp = nullptr;
	}
	if (m_lodlevel3lod2Label) {
		delete m_lodlevel3lod2Label;
		m_lodlevel3lod2Label = nullptr;
	}
	if (m_lodlevel3lod2Slider) {
		delete m_lodlevel3lod2Slider;
		m_lodlevel3lod2Slider = nullptr;
	}
	if (m_lodlevel4Label) {
		delete m_lodlevel4Label;
		m_lodlevel4Label = nullptr;
	}
	if (m_lodlevel4lod0sp) {
		delete m_lodlevel4lod0sp;
		m_lodlevel4lod0sp = nullptr;
	}
	if (m_lodlevel4lod0Label) {
		delete m_lodlevel4lod0Label;
		m_lodlevel4lod0Label = nullptr;
	}
	if (m_lodlevel4lod0Slider) {
		delete m_lodlevel4lod0Slider;
		m_lodlevel4lod0Slider = nullptr;
	}
	if (m_lodlevel4lod1sp) {
		delete m_lodlevel4lod1sp;
		m_lodlevel4lod1sp = nullptr;
	}
	if (m_lodlevel4lod1Label) {
		delete m_lodlevel4lod1Label;
		m_lodlevel4lod1Label = nullptr;
	}
	if (m_lodlevel4lod1Slider) {
		delete m_lodlevel4lod1Slider;
		m_lodlevel4lod1Slider = nullptr;
	}
	if (m_lodlevel4lod2sp) {
		delete m_lodlevel4lod2sp;
		m_lodlevel4lod2sp = nullptr;
	}
	if (m_lodlevel4lod2Label) {
		delete m_lodlevel4lod2Label;
		m_lodlevel4lod2Label = nullptr;
	}
	if (m_lodlevel4lod2Slider) {
		delete m_lodlevel4lod2Slider;
		m_lodlevel4lod2Slider = nullptr;
	}
	if (m_lodlevel4lod3sp) {
		delete m_lodlevel4lod3sp;
		m_lodlevel4lod3sp = nullptr;
	}
	if (m_lodlevel4lod3Label) {
		delete m_lodlevel4lod3Label;
		m_lodlevel4lod3Label = nullptr;
	}
	if (m_lodlevel4lod3Slider) {
		delete m_lodlevel4lod3Slider;
		m_lodlevel4lod3Slider = nullptr;
	}
	if (m_lodspacer1Label) {
		delete m_lodspacer1Label;
		m_lodspacer1Label = nullptr;
	}
	if (m_lodapplysp) {
		delete m_lodapplysp;
		m_lodapplysp = nullptr;
	}
	if (m_lodapplyB) {
		delete m_lodapplyB;
		m_lodapplyB = nullptr;
	}
	if (m_lodspacerLabel001) {
		delete m_lodspacerLabel001;
		m_lodspacerLabel001 = nullptr;
	}
	if (m_lodspacerLabel002) {
		delete m_lodspacerLabel002;
		m_lodspacerLabel002 = nullptr;
	}
	if (m_lodspacerLabel003) {
		delete m_lodspacerLabel003;
		m_lodspacerLabel003 = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CProjLodDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;


	m_dlgWnd = nullptr;

	m_lodprojLabel = nullptr;
	m_lodprojfovsp0 = nullptr;
	m_lodprojfovLabel = nullptr;
	m_lodprojfovSlider = nullptr;
	m_lodnearfarsp0 = nullptr;
	m_lodnearfarsp1 = nullptr;
	m_lodnearfarsp2 = nullptr;
	m_lodnearLabel = nullptr;
	m_lodnearEdit = nullptr;
	m_lodfarLabel = nullptr;
	m_lodfarEdit = nullptr;
	m_lodpickdistsp = nullptr;
	m_lodpickdistLabel = nullptr;
	m_lodpickdistSlider = nullptr;
	m_lodlevel2Label = nullptr;
	m_lodlevel2lod0sp = nullptr;
	m_lodlevel2lod0Label = nullptr;
	m_lodlevel2lod0Slider = nullptr;
	m_lodlevel2lod1sp = nullptr;
	m_lodlevel2lod1Label = nullptr;
	m_lodlevel2lod1Slider = nullptr;
	m_lodlevel3Label = nullptr;
	m_lodlevel3lod0sp = nullptr;
	m_lodlevel3lod0Label = nullptr;
	m_lodlevel3lod0Slider = nullptr;
	m_lodlevel3lod1sp = nullptr;
	m_lodlevel3lod1Label = nullptr;
	m_lodlevel3lod1Slider = nullptr;
	m_lodlevel3lod2sp = nullptr;
	m_lodlevel3lod2Label = nullptr;
	m_lodlevel3lod2Slider = nullptr;
	m_lodlevel4Label = nullptr;
	m_lodlevel4lod0sp = nullptr;
	m_lodlevel4lod0Label = nullptr;
	m_lodlevel4lod0Slider = nullptr;
	m_lodlevel4lod1sp = nullptr;
	m_lodlevel4lod1Label = nullptr;
	m_lodlevel4lod1Slider = nullptr;
	m_lodlevel4lod2sp = nullptr;
	m_lodlevel4lod2Label = nullptr;
	m_lodlevel4lod2Slider = nullptr;
	m_lodlevel4lod3sp = nullptr;
	m_lodlevel4lod3Label = nullptr;
	m_lodlevel4lod3Slider = nullptr;
	m_lodspacer1Label = nullptr;
	m_lodapplysp = nullptr;
	m_lodapplyB = nullptr;
	m_lodspacerLabel001 = nullptr;
	m_lodspacerLabel002 = nullptr;
	m_lodspacerLabel003 = nullptr;

}




int CProjLodDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}




void CProjLodDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateProjLodWnd();//作成済の場合は０リターン

		if (m_dlgWnd) {
			ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			//if (m_lightsSc) {
			//	//############
			//	//2024/07/24
			//	//############
			//	//int showposline = m_dlgSc->getShowPosLine();
			//	//m_dlgSc->setShowPosLine(showposline);
			//	//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
			//	//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
			//	//スクロールバーを少し動かすと全て表示された
			//	//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
			//	m_lightsSc->autoResize();
			//}
			m_dlgWnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgWnd) {
			m_dlgWnd->setVisible(false);
			m_dlgWnd->setListenMouse(false);
		}
	}
	m_visible = srcflag;
}


int CProjLodDlg::CreateProjLodWnd()
{
	if (m_dlgWnd) {
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("LODDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("LODDlg"),	//タイトル
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
		double ratesllabel = 0.3;
		double rate50 = 0.5;

		m_lodprojLabel = new OWP_Label(L"Projection Settings", labelheight);
		if (!m_lodprojLabel) {
			_ASSERT(0);
			abort();
		}
		m_lodprojfovsp0 = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodprojfovsp0) {
			_ASSERT(0);
			abort();
		}
		m_lodprojfovLabel = new OWP_Label(L"fov", labelheight);
		if (!m_lodprojfovLabel) {
			_ASSERT(0);
			abort();
		}
		int fovvalue = (int)(g_fovy * 180.0f / (float)PI + 0.0001f);
		m_lodprojfovSlider = new OWP_Slider((double)fovvalue, 180.0, 10.0, labelheight);
		if (!m_lodprojfovSlider) {
			_ASSERT(0);
			abort();
		}
		m_lodnearfarsp0 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_lodnearfarsp0) {
			_ASSERT(0);
			abort();
		}
		m_lodnearfarsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_lodnearfarsp1) {
			_ASSERT(0);
			abort();
		}
		m_lodnearfarsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_lodnearfarsp2) {
			_ASSERT(0);
			abort();
		}
		m_lodnearLabel = new OWP_Label(L"ProjNear", labelheight);
		if (!m_lodnearLabel) {
			_ASSERT(0);
			abort();
		}
		WCHAR strnear[256] = { 0L };
		swprintf_s(strnear, 256, L"%.2f", g_projnear);
		m_lodnearEdit = new OWP_EditBox(true, strnear, labelheight, EDIT_BUFLEN_NUM);
		if (!m_lodnearEdit) {
			_ASSERT(0);
			abort();
		}
		m_lodfarLabel = new OWP_Label(L"ProjFar", labelheight);
		if (!m_lodfarLabel) {
			_ASSERT(0);
			abort();
		}
		WCHAR strfar[256] = { 0L };
		swprintf_s(strfar, 256, L"%.1f", g_projfar);
		m_lodfarEdit = new OWP_EditBox(true, strfar, labelheight, EDIT_BUFLEN_NUM);
		if (!m_lodfarEdit) {
			_ASSERT(0);
			abort();
		}
		m_lodpickdistsp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodpickdistsp) {
			_ASSERT(0);
			abort();
		}
		m_lodpickdistLabel = new OWP_Label(L"PickDist", labelheight);
		if (!m_lodpickdistLabel) {
			_ASSERT(0);
			abort();
		}
		m_lodpickdistSlider = new OWP_Slider(g_pickdistrate, 1.0, 0.0, labelheight);
		if (!m_lodpickdistSlider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2Label = new OWP_Label(L"Tow Levels LOD : clipping distance", labelheight);
		if (!m_lodlevel2Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2lod0sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel2lod0sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2lod0Label = new OWP_Label(L"LOD0", labelheight);
		if (!m_lodlevel2lod0Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2lod0Slider = new OWP_Slider((double)g_lodrate2L[CHKINVIEW_LOD0], 1.0, 0.0, labelheight);
		if (!m_lodlevel2lod0Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2lod1sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel2lod1sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2lod1Label = new OWP_Label(L"LOD1", labelheight);
		if (!m_lodlevel2lod1Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel2lod1Slider = new OWP_Slider((double)g_lodrate2L[CHKINVIEW_LOD1], 1.0, 0.0, labelheight);
		if (!m_lodlevel2lod1Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3Label = new OWP_Label(L"Three Levels LOD : clipping distance", labelheight);
		if (!m_lodlevel3Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod0sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel3lod0sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod0Label = new OWP_Label(L"LOD0", labelheight);
		if (!m_lodlevel3lod0Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod0Slider = new OWP_Slider((double)g_lodrate3L[CHKINVIEW_LOD0], 1.0, 0.0, labelheight);
		if (!m_lodlevel3lod0Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod1sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel3lod1sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod1Label = new OWP_Label(L"LOD1", labelheight);
		if (!m_lodlevel3lod1Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod1Slider = new OWP_Slider((double)g_lodrate3L[CHKINVIEW_LOD1], 1.0, 0.0, labelheight);
		if (!m_lodlevel3lod1Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod2sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel3lod2sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod2Label = new OWP_Label(L"LOD2", labelheight);
		if (!m_lodlevel3lod2Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel3lod2Slider = new OWP_Slider((double)g_lodrate3L[CHKINVIEW_LOD2], 1.0, 0.0, labelheight);
		if (!m_lodlevel3lod2Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4Label = new OWP_Label(L"Four Levels LOD : clipping distance", labelheight);
		if (!m_lodlevel4Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod0sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel4lod0sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod0Label = new OWP_Label(L"LOD0", labelheight);
		if (!m_lodlevel4lod0Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod0Slider = new OWP_Slider((double)g_lodrate4L[CHKINVIEW_LOD0], 1.0, 0.0, labelheight);
		if (!m_lodlevel4lod0Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod1sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel4lod1sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod1Label = new OWP_Label(L"LOD1", labelheight);
		if (!m_lodlevel4lod1Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod1Slider = new OWP_Slider((double)g_lodrate4L[CHKINVIEW_LOD1], 1.0, 0.0, labelheight);
		if (!m_lodlevel4lod1Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod2sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel4lod2sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod2Label = new OWP_Label(L"LOD2", labelheight);
		if (!m_lodlevel4lod2Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod2Slider = new OWP_Slider((double)g_lodrate4L[CHKINVIEW_LOD2], 1.0, 0.0, labelheight);
		if (!m_lodlevel4lod2Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod3sp = new OWP_Separator(m_dlgWnd, true, ratesllabel, true);
		if (!m_lodlevel4lod3sp) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod3Label = new OWP_Label(L"LOD3", labelheight);
		if (!m_lodlevel4lod3Label) {
			_ASSERT(0);
			abort();
		}
		m_lodlevel4lod3Slider = new OWP_Slider((double)g_lodrate4L[CHKINVIEW_LOD3], 1.0, 0.0, labelheight);
		if (!m_lodlevel4lod3Slider) {
			_ASSERT(0);
			abort();
		}
		m_lodspacer1Label = new OWP_Label(L"     ", 24);
		if (!m_lodspacer1Label) {
			_ASSERT(0);
			abort();
		}
		m_lodapplysp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_lodapplysp) {
			_ASSERT(0);
			abort();
		}
		m_lodapplyB = new OWP_Button(L"Apply(適用)", 38);
		if (!m_lodapplyB) {
			_ASSERT(0);
			abort();
		}
		m_lodapplyB->setTextColor(RGB(168, 129, 129));
		m_lodspacerLabel001 = new OWP_Label(L"     ", 32);
		if (!m_lodspacerLabel001) {
			_ASSERT(0);
			abort();
		}
		m_lodspacerLabel002 = new OWP_Label(L"     ", 32);
		if (!m_lodspacerLabel002) {
			_ASSERT(0);
			abort();
		}
		m_lodspacerLabel003 = new OWP_Label(L"     ", 32);
		if (!m_lodspacerLabel003) {
			_ASSERT(0);
			abort();
		}




		m_dlgWnd->addParts(*m_lodprojLabel);
		m_dlgWnd->addParts(*m_lodprojfovsp0);
		m_lodprojfovsp0->addParts1(*m_lodprojfovLabel);
		m_lodprojfovsp0->addParts2(*m_lodprojfovSlider);
		m_dlgWnd->addParts(*m_lodnearfarsp0);
		m_lodnearfarsp0->addParts1(*m_lodnearfarsp1);
		m_lodnearfarsp0->addParts2(*m_lodnearfarsp2);
		m_lodnearfarsp1->addParts1(*m_lodnearLabel);
		m_lodnearfarsp1->addParts2(*m_lodnearEdit);
		m_lodnearfarsp2->addParts1(*m_lodfarLabel);
		m_lodnearfarsp2->addParts2(*m_lodfarEdit);
		m_dlgWnd->addParts(*m_lodpickdistsp);
		m_lodpickdistsp->addParts1(*m_lodpickdistLabel);
		m_lodpickdistsp->addParts2(*m_lodpickdistSlider);
		m_dlgWnd->addParts(*m_lodspacerLabel001);//
		m_dlgWnd->addParts(*m_lodlevel2Label);
		m_dlgWnd->addParts(*m_lodlevel2lod0sp);
		m_lodlevel2lod0sp->addParts1(*m_lodlevel2lod0Label);
		m_lodlevel2lod0sp->addParts2(*m_lodlevel2lod0Slider);
		m_dlgWnd->addParts(*m_lodlevel2lod1sp);
		m_lodlevel2lod1sp->addParts1(*m_lodlevel2lod1Label);
		m_lodlevel2lod1sp->addParts2(*m_lodlevel2lod1Slider);
		m_dlgWnd->addParts(*m_lodspacerLabel002);//
		m_dlgWnd->addParts(*m_lodlevel3Label);
		m_dlgWnd->addParts(*m_lodlevel3lod0sp);
		m_lodlevel3lod0sp->addParts1(*m_lodlevel3lod0Label);
		m_lodlevel3lod0sp->addParts2(*m_lodlevel3lod0Slider);
		m_dlgWnd->addParts(*m_lodlevel3lod1sp);
		m_lodlevel3lod1sp->addParts1(*m_lodlevel3lod1Label);
		m_lodlevel3lod1sp->addParts2(*m_lodlevel3lod1Slider);
		m_dlgWnd->addParts(*m_lodlevel3lod2sp);
		m_lodlevel3lod2sp->addParts1(*m_lodlevel3lod2Label);
		m_lodlevel3lod2sp->addParts2(*m_lodlevel3lod2Slider);
		m_dlgWnd->addParts(*m_lodspacerLabel003);//
		m_dlgWnd->addParts(*m_lodlevel4Label);
		m_dlgWnd->addParts(*m_lodlevel4lod0sp);
		m_lodlevel4lod0sp->addParts1(*m_lodlevel4lod0Label);
		m_lodlevel4lod0sp->addParts2(*m_lodlevel4lod0Slider);
		m_dlgWnd->addParts(*m_lodlevel4lod1sp);
		m_lodlevel4lod1sp->addParts1(*m_lodlevel4lod1Label);
		m_lodlevel4lod1sp->addParts2(*m_lodlevel4lod1Slider);
		m_dlgWnd->addParts(*m_lodlevel4lod2sp);
		m_lodlevel4lod2sp->addParts1(*m_lodlevel4lod2Label);
		m_lodlevel4lod2sp->addParts2(*m_lodlevel4lod2Slider);
		m_dlgWnd->addParts(*m_lodlevel4lod3sp);
		m_lodlevel4lod3sp->addParts1(*m_lodlevel4lod3Label);
		m_lodlevel4lod3sp->addParts2(*m_lodlevel4lod3Slider);
		m_dlgWnd->addParts(*m_lodspacer1Label);
		m_dlgWnd->addParts(*m_lodapplysp);
		m_lodapplysp->addParts2(*m_lodapplyB);



		//##########
		//Slider
		//##########
		m_lodprojfovSlider->setCursorListener([=, this]() {
			double value = m_lodprojfovSlider->getValue();
			g_fovy = (float)(value * PI / 180.0);
			
			//SetCamera3DFromEyePos();//2023/12/30
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_PROJLODDLG), (LPARAM)0);
			}
			});
		m_lodpickdistSlider->setCursorListener([=, this]() {
			double value = m_lodpickdistSlider->getValue();
			g_pickdistrate = value;
			});

		m_lodlevel2lod0Slider->setCursorListener([=, this]() {
			double value = m_lodlevel2lod0Slider->getValue();
			g_lodrate2L[CHKINVIEW_LOD0] = (float)value;
			});
		m_lodlevel2lod1Slider->setCursorListener([=, this]() {
			double value = m_lodlevel2lod1Slider->getValue();
			g_lodrate2L[CHKINVIEW_LOD1] = (float)value;
			});

		m_lodlevel3lod0Slider->setCursorListener([=, this]() {
			double value = m_lodlevel3lod0Slider->getValue();
			g_lodrate3L[CHKINVIEW_LOD0] = (float)value;
			});
		m_lodlevel3lod1Slider->setCursorListener([=, this]() {
			double value = m_lodlevel3lod1Slider->getValue();
			g_lodrate3L[CHKINVIEW_LOD1] = (float)value;
			});
		m_lodlevel3lod2Slider->setCursorListener([=, this]() {
			double value = m_lodlevel3lod2Slider->getValue();
			g_lodrate3L[CHKINVIEW_LOD2] = (float)value;
			});

		m_lodlevel4lod0Slider->setCursorListener([=, this]() {
			double value = m_lodlevel4lod0Slider->getValue();
			g_lodrate4L[CHKINVIEW_LOD0] = (float)value;
			});
		m_lodlevel4lod1Slider->setCursorListener([=, this]() {
			double value = m_lodlevel4lod1Slider->getValue();
			g_lodrate4L[CHKINVIEW_LOD1] = (float)value;
			});
		m_lodlevel4lod2Slider->setCursorListener([=, this]() {
			double value = m_lodlevel4lod2Slider->getValue();
			g_lodrate4L[CHKINVIEW_LOD2] = (float)value;
			});
		m_lodlevel4lod3Slider->setCursorListener([=, this]() {
			double value = m_lodlevel4lod3Slider->getValue();
			g_lodrate4L[CHKINVIEW_LOD3] = (float)value;
			});


		m_lodapplyB->setButtonListener([=, this]() {
			WCHAR strnear[256] = { 0L };
			if (m_lodnearEdit) {
				m_lodnearEdit->getName(strnear, 256);
			}
			float tempeditvalue = (float)_wtof(strnear);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_projnear = tempeditvalue;
			}
			else {
				if (m_dlgWnd && m_dlgWnd->getHWnd()) {
					::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : near", L"Invalid Value", MB_OK);
				}
			}

			WCHAR strfar[256] = { 0L };
			if (m_lodfarEdit) {
				m_lodfarEdit->getName(strfar, 256);
			}
			tempeditvalue = (float)_wtof(strfar);
			if ((tempeditvalue >= 0.000010f) && (tempeditvalue <= 500000.0f)) {
				g_projfar = tempeditvalue;
			}
			else {
				if (m_dlgWnd && m_dlgWnd->getHWnd()) {
					::MessageBox(m_dlgWnd->getHWnd(), L"invalid editbox value : far", L"Invalid Value", MB_OK);
				}
			}

			//SetCamera3DFromEyePos();//2023/12/30			
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_PROJLODDLG), (LPARAM)0);
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

int CProjLodDlg::ParamsToDlg()
{
	if (m_dlgWnd) {

		//#######
		//Slider
		//#######
		if (m_lodprojfovSlider) {
			double value = (double)g_fovy * 180.0 / PI;
			m_lodprojfovSlider->setValue(value, false);
		}
		if (m_lodpickdistSlider) {
			m_lodpickdistSlider->setValue(g_pickdistrate, false);
		}

		if (m_lodlevel2lod0Slider) {
			m_lodlevel2lod0Slider->setValue((double)g_lodrate2L[CHKINVIEW_LOD0], false);
		}
		if (m_lodlevel2lod1Slider) {
			m_lodlevel2lod1Slider->setValue((double)g_lodrate2L[CHKINVIEW_LOD1], false);
		}

		if (m_lodlevel3lod0Slider) {
			m_lodlevel3lod0Slider->setValue((double)g_lodrate3L[CHKINVIEW_LOD0], false);
		}
		if (m_lodlevel3lod1Slider) {
			m_lodlevel3lod1Slider->setValue((double)g_lodrate3L[CHKINVIEW_LOD1], false);
		}
		if (m_lodlevel3lod2Slider) {
			m_lodlevel3lod2Slider->setValue((double)g_lodrate3L[CHKINVIEW_LOD2], false);
		}

		if (m_lodlevel4lod0Slider) {
			m_lodlevel4lod0Slider->setValue((double)g_lodrate4L[CHKINVIEW_LOD0], false);
		}
		if (m_lodlevel4lod1Slider) {
			m_lodlevel4lod1Slider->setValue((double)g_lodrate4L[CHKINVIEW_LOD1], false);
		}
		if (m_lodlevel4lod2Slider) {
			m_lodlevel4lod2Slider->setValue((double)g_lodrate4L[CHKINVIEW_LOD2], false);
		}
		if (m_lodlevel4lod3Slider) {
			m_lodlevel4lod3Slider->setValue((double)g_lodrate4L[CHKINVIEW_LOD3], false);
		}

		//#####
		//EditBox
		//#####
		WCHAR strdlg[256] = { 0L };
		swprintf_s(strdlg, 256, L"%.2f", g_projnear);
		if (m_lodnearEdit) {
			m_lodnearEdit->setNameString(strdlg);
		}

		swprintf_s(strdlg, 256, L"%.1f", g_projfar);
		if (m_lodfarEdit) {
			m_lodfarEdit->setNameString(strdlg);
		}

		m_dlgWnd->callRewrite();
	}
	return 0;
}

const HWND CProjLodDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CProjLodDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CProjLodDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

