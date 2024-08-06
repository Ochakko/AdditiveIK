#include "stdafx.h"

#include <DispLimitsDlg.h>
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
// CDispLimitsDlg

CDispLimitsDlg::CDispLimitsDlg()
{
	InitParams();
}

CDispLimitsDlg::~CDispLimitsDlg()
{
	DestroyObjs();
}
	
int CDispLimitsDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_lightsChk) {
		delete m_lightsChk;
		m_lightsChk = nullptr;
	}
	if (m_lightssp) {
		delete m_lightssp;
		m_lightssp = nullptr;
	}
	if (m_lightsSlider) {
		delete m_lightsSlider;
		m_lightsSlider = nullptr;
	}
	if (m_threadssp) {
		delete m_threadssp;
		m_threadssp = nullptr;
	}
	if (m_threadsLabel) {
		delete m_threadsLabel;
		m_threadsLabel = nullptr;
	}
	if (m_threadsSlider) {
		delete m_threadsSlider;
		m_threadsSlider = nullptr;
	}
	if (m_highRpmChk) {
		delete m_highRpmChk;
		m_highRpmChk = nullptr;
	}
	if (m_bonemarksp) {
		delete m_bonemarksp;
		m_bonemarksp = nullptr;
	}
	if (m_bonemarkChk) {
		delete m_bonemarkChk;
		m_bonemarkChk = nullptr;
	}
	if (m_bonemarkSlider) {
		delete m_bonemarkSlider;
		m_bonemarkSlider = nullptr;
	}
	if (m_rigidmarksp) {
		delete m_rigidmarksp;
		m_rigidmarksp = nullptr;
	}
	if (m_rigidmarkChk) {
		delete m_rigidmarkChk;
		m_rigidmarkChk = nullptr;
	}
	if (m_rigidmarkSlider) {
		delete m_rigidmarkSlider;
		m_rigidmarkSlider = nullptr;
	}
	if (m_rigmarksp) {
		delete m_rigmarksp;
		m_rigmarksp = nullptr;
	}
	if (m_rigmarkLabel) {
		delete m_rigmarkLabel;
		m_rigmarkLabel = nullptr;
	}
	if (m_rigmarkSlider) {
		delete m_rigmarkSlider;
		m_rigmarkSlider = nullptr;
	}
	if (m_refpossp) {
		delete m_refpossp;
		m_refpossp = nullptr;
	}
	if (m_refposLabel) {
		delete m_refposLabel;
		m_refposLabel = nullptr;
	}
	if (m_refposSlider) {
		delete m_refposSlider;
		m_refposSlider = nullptr;
	}
	if (m_iklevelssp) {
		delete m_iklevelssp;
		m_iklevelssp = nullptr;
	}
	if (m_iklevelsLabel) {
		delete m_iklevelsLabel;
		m_iklevelsLabel = nullptr;
	}
	if (m_iklevelsCombo) {
		delete m_iklevelsCombo;
		m_iklevelsCombo = nullptr;
	}
	if (m_axiskindsp) {
		delete m_axiskindsp;
		m_axiskindsp = nullptr;
	}
	if (m_axiskindLabel) {
		delete m_axiskindLabel;
		m_axiskindLabel = nullptr;
	}
	if (m_axiskindCombo) {
		delete m_axiskindCombo;
		m_axiskindCombo = nullptr;
	}
	if (m_uvsetsp) {
		delete m_uvsetsp;
		m_uvsetsp = nullptr;
	}
	if (m_uvsetLabel) {
		delete m_uvsetLabel;
		m_uvsetLabel = nullptr;
	}
	if (m_uvsetCombo) {
		delete m_uvsetCombo;
		m_uvsetCombo = nullptr;
	}
	if (m_dispsp1) {
		delete m_dispsp1;
		m_dispsp1 = nullptr;
	}
	if (m_x180Chk) {
		delete m_x180Chk;
		m_x180Chk = nullptr;
	}
	if (m_rottraChk) {
		delete m_rottraChk;
		m_rottraChk = nullptr;
	}
	if (m_dispsp2) {
		delete m_dispsp2;
		m_dispsp2 = nullptr;
	}
	if (m_dofChk) {
		delete m_dofChk;
		m_dofChk = nullptr;
	}
	if (m_bloomChk) {
		delete m_bloomChk;
		m_bloomChk = nullptr;
	}
	if (m_dispsp3) {
		delete m_dispsp3;
		m_dispsp3 = nullptr;
	}
	if (m_alphaChk) {
		delete m_alphaChk;
		m_alphaChk = nullptr;
	}
	if (m_zcmpChk) {
		delete m_zcmpChk;
		m_zcmpChk = nullptr;
	}
	if (m_dispsp4) {
		delete m_dispsp4;
		m_dispsp4 = nullptr;
	}
	if (m_freefpsChk) {
		delete m_freefpsChk;
		m_freefpsChk = nullptr;
	}
	if (m_skydispChk) {
		delete m_skydispChk;
		m_skydispChk = nullptr;
	}
	if (m_dispspacerLabel001) {
		delete m_dispspacerLabel001;
		m_dispspacerLabel001 = nullptr;
	}
	if (m_dispspacerLabel002) {
		delete m_dispspacerLabel002;
		m_dispspacerLabel002 = nullptr;
	}
	if (m_dispspacerLabel003) {
		delete m_dispspacerLabel003;
		m_dispspacerLabel003 = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CDispLimitsDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;


	m_dlgWnd = nullptr;

	m_lightsChk = nullptr;
	m_lightssp = nullptr;
	m_lightsSlider = nullptr;
	m_threadssp = nullptr;
	m_threadsLabel = nullptr;
	m_threadsSlider = nullptr;
	m_highRpmChk = nullptr;
	m_bonemarksp = nullptr;
	m_bonemarkChk = nullptr;
	m_bonemarkSlider = nullptr;
	m_rigidmarksp = nullptr;
	m_rigidmarkChk = nullptr;
	m_rigidmarkSlider = nullptr;
	m_rigmarksp = nullptr;
	m_rigmarkLabel = nullptr;
	m_rigmarkSlider = nullptr;
	m_refpossp = nullptr;
	m_refposLabel = nullptr;
	m_refposSlider = nullptr;
	m_iklevelssp = nullptr;
	m_iklevelsLabel = nullptr;
	m_iklevelsCombo = nullptr;
	m_axiskindsp = nullptr;
	m_axiskindLabel = nullptr;
	m_axiskindCombo = nullptr;
	m_uvsetsp = nullptr;
	m_uvsetLabel = nullptr;
	m_uvsetCombo = nullptr;
	m_dispsp1 = nullptr;
	m_x180Chk = nullptr;
	m_rottraChk = nullptr;
	m_dispsp2 = nullptr;
	m_dofChk = nullptr;
	m_bloomChk = nullptr;
	m_dispsp3 = nullptr;
	m_alphaChk = nullptr;
	m_zcmpChk = nullptr;
	m_dispsp4 = nullptr;
	m_freefpsChk = nullptr;
	m_skydispChk = nullptr;
	m_dispspacerLabel001 = nullptr;
	m_dispspacerLabel002 = nullptr;
	m_dispspacerLabel003 = nullptr;


}




int CDispLimitsDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}




void CDispLimitsDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateDispLimitsWnd();//作成済の場合は０リターン

		if (m_dlgWnd) {
			ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_lightsChk) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_lightsChk->autoResize();
			}
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


int CDispLimitsDlg::CreateDispLimitsWnd()
{
	if (m_dlgWnd) {
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("DispAndLimitsDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("DispAndLimitsDlg"),	//タイトル
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

		m_lightssp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_lightssp) {
			_ASSERT(0);
			abort();
		}
		m_lightsChk = new OWP_CheckBoxA(L"Enable Lights", (g_lightflag != 0), labelheight, false);
		if (!m_lightsChk) {
			_ASSERT(0);
			abort();
		}
		m_lightsSlider = new OWP_Slider((double)g_fLightScale, 10.0, 0.0, labelheight);
		if (!m_lightsSlider) {
			_ASSERT(0);
			abort();
		}
		m_threadssp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_threadssp) {
			_ASSERT(0);
			abort();
		}
		m_threadsLabel = new OWP_Label(L"Update Threads", labelheight);
		if (!m_threadsLabel) {
			_ASSERT(0);
			abort();
		}
		m_threadsSlider = new OWP_Slider((double)g_UpdateMatrixThreads, 8.0, 1.0, labelheight);
		if (!m_threadsSlider) {
			_ASSERT(0);
			abort();
		}

		m_highRpmChk = new OWP_CheckBoxA(L"Hight RPM", g_HighRpmMode, labelheight, false);
		if (!m_highRpmChk) {
			_ASSERT(0);
			abort();
		}

		m_bonemarksp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_bonemarksp) {
			_ASSERT(0);
			abort();
		}
		m_bonemarkChk = new OWP_CheckBoxA(L"BoneMark", (g_bonemarkflag != 0), labelheight, false);
		if (!m_bonemarkChk) {
			_ASSERT(0);
			abort();
		}
		m_bonemarkSlider = new OWP_Slider((double)g_bonemark_bright, 1.0, 0.0, labelheight);
		if (!m_bonemarkSlider) {
			_ASSERT(0);
			abort();
		}

		m_rigidmarksp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rigidmarksp) {
			_ASSERT(0);
			abort();
		}
		m_rigidmarkChk = new OWP_CheckBoxA(L"RigidMark", (g_rigidmarkflag != 0), labelheight, false);
		if (!m_rigidmarkChk) {
			_ASSERT(0);
			abort();
		}
		m_rigidmarkSlider = new OWP_Slider((double)g_rigidmark_alpha, 1.0, 0.0, labelheight);
		if (!m_rigidmarkSlider) {
			_ASSERT(0);
			abort();
		}

		m_rigmarksp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rigmarksp) {
			_ASSERT(0);
			abort();
		}
		m_rigmarkLabel = new OWP_Label(L"RigMark", labelheight);
		if (!m_rigmarkLabel) {
			_ASSERT(0);
			abort();
		}
		m_rigmarkSlider = new OWP_Slider((double)g_rigmark_alpha, 1.0, 0.0, labelheight);
		if (!m_rigmarkSlider) {
			_ASSERT(0);
			abort();
		}

		m_refpossp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_refpossp) {
			_ASSERT(0);
			abort();
		}
		m_refposLabel = new OWP_Label(L"RefPosAlpha", labelheight);
		if (!m_refposLabel) {
			_ASSERT(0);
			abort();
		}
		m_refposSlider = new OWP_Slider((double)g_refalpha, 100.0, 0.0, labelheight);
		if (!m_refposSlider) {
			_ASSERT(0);
			abort();
		}

		m_iklevelssp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_iklevelssp) {
			_ASSERT(0);
			abort();
		}
		m_iklevelsLabel = new OWP_Label(L"IK Levels", labelheight);
		if (!m_iklevelsLabel) {
			_ASSERT(0);
			abort();
		}
		m_iklevelsCombo = new OWP_ComboBoxA(L"IKLEVELS", labelheight);//g_iklevel:1,15
		if (!m_iklevelsCombo) {
			_ASSERT(0);
			abort();
		}
		int levelnum = 15;
		int levelno;
		for (levelno = 1; levelno <= levelnum; levelno++) {
			char combostr[256] = { 0 };
			sprintf_s(combostr, 256, "%02d", levelno);
			m_iklevelsCombo->addString(combostr);
		}


		m_axiskindsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_axiskindsp) {
			_ASSERT(0);
			abort();
		}
		m_axiskindLabel = new OWP_Label(L"Axis Kind", labelheight);
		if (!m_axiskindLabel) {
			_ASSERT(0);
			abort();
		}
		m_axiskindCombo = new OWP_ComboBoxA(L"AXISKIND", labelheight);//g_boneaxis:CURRENT,PARENT,GLOBAL,BINDPOSE
		if (!m_axiskindCombo) {
			_ASSERT(0);
			abort();
		}
		m_axiskindCombo->addString("Current");
		m_axiskindCombo->addString("Parent");
		m_axiskindCombo->addString("Global");
		m_axiskindCombo->addString("BindPose");


		m_uvsetsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_uvsetsp) {
			_ASSERT(0);
			abort();
		}
		m_uvsetLabel = new OWP_Label(L"UV Set", labelheight);
		if (!m_uvsetLabel) {
			_ASSERT(0);
			abort();
		}
		m_uvsetCombo = new OWP_ComboBoxA(L"UVSET", labelheight);//g_uvset:UVSet0, UVSet1
		if (!m_uvsetCombo) {
			_ASSERT(0);
			abort();
		}
		m_uvsetCombo->addString("UV Set0");
		m_uvsetCombo->addString("UV Set1");

		m_dispsp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dispsp1) {
			_ASSERT(0);
			abort();
		}
		m_x180Chk = new OWP_CheckBoxA(L"Modify Euler X180", g_x180flag, labelheight, false);
		if (!m_x180Chk) {
			_ASSERT(0);
			abort();
		}
		m_rottraChk = new OWP_CheckBoxA(L"Rotate Translation", g_rotatetanim, labelheight, false);
		if (!m_rottraChk) {
			_ASSERT(0);
			abort();
		}

		m_dispsp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dispsp2) {
			_ASSERT(0);
			abort();
		}
		m_dofChk = new OWP_CheckBoxA(L"DOF(DepthOfField)", g_zpreflag, labelheight, false);
		if (!m_dofChk) {
			_ASSERT(0);
			abort();
		}
		m_bloomChk = new OWP_CheckBoxA(L"HDRP Bloom", g_hdrpbloom, labelheight, false);
		if (!m_bloomChk) {
			_ASSERT(0);
			abort();
		}

		m_dispsp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dispsp3) {
			_ASSERT(0);
			abort();
		}
		m_alphaChk = new OWP_CheckBoxA(L"Alpha Blending", g_alphablending, labelheight, false);
		if (!m_alphaChk) {
			_ASSERT(0);
			abort();
		}
		m_zcmpChk = new OWP_CheckBoxA(L"ZCmpAlways", g_zalways, labelheight, false);
		if (!m_zcmpChk) {
			_ASSERT(0);
			abort();
		}

		m_dispsp4 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_dispsp4) {
			_ASSERT(0);
			abort();
		}
		m_freefpsChk = new OWP_CheckBoxA(L"Free fps", g_freefps, labelheight, false);
		if (!m_freefpsChk) {
			_ASSERT(0);
			abort();
		}
		m_skydispChk = new OWP_CheckBoxA(L"Sky Disp", g_skydispflag, labelheight, false);
		if (!m_skydispChk) {
			_ASSERT(0);
			abort();
		}
		m_dispspacerLabel001 = new OWP_Label(L"     ", 32);
		if (!m_dispspacerLabel001) {
			_ASSERT(0);
			abort();
		}
		m_dispspacerLabel002 = new OWP_Label(L"     ", 32);
		if (!m_dispspacerLabel002) {
			_ASSERT(0);
			abort();
		}
		m_dispspacerLabel003 = new OWP_Label(L"     ", 32);
		if (!m_dispspacerLabel003) {
			_ASSERT(0);
			abort();
		}


		m_dlgWnd->addParts(*m_lightssp);
		m_lightssp->addParts1(*m_lightsChk);
		m_lightssp->addParts2(*m_lightsSlider);
		m_dlgWnd->addParts(*m_threadssp);
		m_threadssp->addParts1(*m_threadsLabel);
		m_threadssp->addParts2(*m_threadsSlider);
		m_dlgWnd->addParts(*m_highRpmChk);
		m_dlgWnd->addParts(*m_dispspacerLabel001);//
		m_dlgWnd->addParts(*m_bonemarksp);
		m_bonemarksp->addParts1(*m_bonemarkChk);
		m_bonemarksp->addParts2(*m_bonemarkSlider);
		m_dlgWnd->addParts(*m_rigidmarksp);
		m_rigidmarksp->addParts1(*m_rigidmarkChk);
		m_rigidmarksp->addParts2(*m_rigidmarkSlider);
		m_dlgWnd->addParts(*m_rigmarksp);
		m_rigmarksp->addParts1(*m_rigmarkLabel);
		m_rigmarksp->addParts2(*m_rigmarkSlider);
		m_dlgWnd->addParts(*m_refpossp);
		m_refpossp->addParts1(*m_refposLabel);
		m_refpossp->addParts2(*m_refposSlider);
		m_dlgWnd->addParts(*m_dispspacerLabel002);//
		m_dlgWnd->addParts(*m_iklevelssp);
		m_iklevelssp->addParts1(*m_iklevelsLabel);
		m_iklevelssp->addParts2(*m_iklevelsCombo);
		m_dlgWnd->addParts(*m_axiskindsp);
		m_axiskindsp->addParts1(*m_axiskindLabel);
		m_axiskindsp->addParts2(*m_axiskindCombo);
		m_dlgWnd->addParts(*m_uvsetsp);
		m_uvsetsp->addParts1(*m_uvsetLabel);
		m_uvsetsp->addParts2(*m_uvsetCombo);
		m_dlgWnd->addParts(*m_dispspacerLabel003);//
		m_dlgWnd->addParts(*m_dispsp1);
		m_dispsp1->addParts1(*m_x180Chk);
		m_dispsp1->addParts2(*m_rottraChk);
		m_dlgWnd->addParts(*m_dispsp2);
		m_dispsp2->addParts1(*m_dofChk);
		m_dispsp2->addParts2(*m_bloomChk);
		m_dlgWnd->addParts(*m_dispsp3);
		m_dispsp3->addParts1(*m_alphaChk);
		m_dispsp3->addParts2(*m_zcmpChk);
		m_dlgWnd->addParts(*m_dispsp4);
		m_dispsp4->addParts1(*m_freefpsChk);
		m_dispsp4->addParts2(*m_skydispChk);

		//###########
		//CheckBox
		//###########
		m_lightsChk->setButtonListener([=, this]() {
			bool value = m_lightsChk->getValue();
			if (value) {
				g_lightflag = 1;
			}
			else {
				g_lightflag = 0;
			}
			});
		m_highRpmChk->setButtonListener([=, this]() {
			bool value = m_highRpmChk->getValue();
			g_HighRpmMode = value;
			});
		m_bonemarkChk->setButtonListener([=, this]() {
			bool value = m_bonemarkChk->getValue();
			if (value) {
				g_bonemarkflag = 1;
			}
			else {
				g_bonemarkflag = 0;
			}
			});
		m_rigidmarkChk->setButtonListener([=, this]() {
			bool value = m_rigidmarkChk->getValue();
			if (value) {
				g_rigidmarkflag = 1;
			}
			else {
				g_rigidmarkflag = 0;
			}
			});
		m_x180Chk->setButtonListener([=, this]() {
			bool value = m_x180Chk->getValue();
			g_x180flag = value;
			});
		m_rottraChk->setButtonListener([=, this]() {
			bool value = m_rottraChk->getValue();
			g_rotatetanim = value;
			});
		m_dofChk->setButtonListener([=, this]() {
			bool value = m_dofChk->getValue();
			g_zpreflag = value;
			});
		m_bloomChk->setButtonListener([=, this]() {
			bool value = m_bloomChk->getValue();
			g_hdrpbloom = value;
			});
		m_alphaChk->setButtonListener([=, this]() {
			bool value = m_alphaChk->getValue();
			g_alphablending = value;
			});
		m_zcmpChk->setButtonListener([=, this]() {
			bool value = m_zcmpChk->getValue();
			g_zalways = value;
			});
		m_freefpsChk->setButtonListener([=, this]() {
			bool value = m_freefpsChk->getValue();
			g_freefps = value;
			});
		m_skydispChk->setButtonListener([=, this]() {
			bool value = m_skydispChk->getValue();
			g_skydispflag = value;
			});

		//##########
		//Slider
		//##########
		m_lightsSlider->setCursorListener([=, this]() {
			double value = m_lightsSlider->getValue();
			g_fLightScale = (float)value;
			});
		m_bonemarkSlider->setCursorListener([=, this]() {
			double value = m_bonemarkSlider->getValue();
			g_bonemark_bright = (float)value;
			});
		m_rigidmarkSlider->setCursorListener([=, this]() {
			double value = m_rigidmarkSlider->getValue();
			g_rigidmark_alpha = (float)value;
			});
		m_rigmarkSlider->setCursorListener([=, this]() {
			double value = m_rigmarkSlider->getValue();
			g_rigmark_alpha = (float)value;
			});
		m_refposSlider->setCursorListener([=, this]() {
			double value = m_refposSlider->getValue();
			g_refalpha = (int)(value + 0.0001);
			});

		m_threadsSlider->setCursorListener([=, this]() {
			int value = (int)(m_threadsSlider->getValue() + 0.5);
			g_UpdateMatrixThreads = max(1, min(8, value));
			m_threadsSlider->setValue((double)value, false);//intに丸めた値をセットし直し
			});
		m_threadsSlider->setLUpListener([=, this]() {
			int value = (int)(m_threadsSlider->getValue() + 0.5);
			g_UpdateMatrixThreads = max(1, min(8, value));
			m_threadsSlider->setValue((double)value, false);//intに丸めた値をセットし直し

			//#################################################
			//ReleasedCaptureのときに　PrepairUndo用のフラグを立てる
			//#################################################
			//m_changeupdatethreadsFlag = true;
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_DISPLIMITSDLG), (LPARAM)0);
			}
		});


		//############
		//ComboBox
		//############
		m_iklevelsCombo->setButtonListener([=, this]() {
			int comboid = m_iklevelsCombo->trackPopUpMenu();
			//char strchk[256] = { 0 };
			//sprintf_s(strchk, 256, "select combo %d", comboid);
			//MessageBoxA(m_dlgWnd->getHWnd(), strchk, "Check", MB_OK);
			g_iklevel = comboid + 1;
			});
		m_axiskindCombo->setButtonListener([=, this]() {
			int comboid = m_axiskindCombo->trackPopUpMenu();
			//char strchk[256] = { 0 };
			//sprintf_s(strchk, 256, "select combo %d", comboid);
			//MessageBoxA(m_dlgWnd->getHWnd(), strchk, "Check", MB_OK);
			g_boneaxis = comboid;
			});
		m_uvsetCombo->setButtonListener([=, this]() {
			int comboid = m_uvsetCombo->trackPopUpMenu();
			//char strchk[256] = { 0 };
			//sprintf_s(strchk, 256, "select combo %d", comboid);
			//MessageBoxA(m_dlgWnd->getHWnd(), strchk, "Check", MB_OK);
			g_uvset = comboid;
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

int CDispLimitsDlg::ParamsToDlg()
{
	if (m_dlgWnd) {

		//#######
		//Slider
		//#######
		if (m_lightsSlider) {
			m_lightsSlider->setValue((double)g_fLightScale, false);
		}
		if (m_threadsSlider) {
			m_threadsSlider->setValue((double)g_UpdateMatrixThreads, false);
		}
		if (m_bonemarkSlider) {
			m_bonemarkSlider->setValue((double)g_bonemark_bright, false);
		}
		if (m_rigidmarkSlider) {
			m_rigidmarkSlider->setValue((double)g_rigidmark_alpha, false);
		}
		if (m_rigmarkSlider) {
			m_rigmarkSlider->setValue((double)g_rigmark_alpha, false);
		}
		if (m_refposSlider) {
			m_refposSlider->setValue((double)g_refalpha, false);
		}


		//#########
		//ComboBox
		//#########
		if (m_iklevelsCombo) {
			m_iklevelsCombo->setSelectedCombo(g_iklevel - 1);
		}
		if (m_axiskindCombo) {
			m_axiskindCombo->setSelectedCombo(g_boneaxis);
		}
		if (m_uvsetCombo) {
			m_uvsetCombo->setSelectedCombo(g_uvset);
		}


		//#########
		//CheckBox
		//#########
		if (m_bloomChk) {
			m_bloomChk->setValue(g_hdrpbloom, false);
		}
		if (m_freefpsChk) {
			m_freefpsChk->setValue(g_freefps, false);
		}
		if (m_lightsChk) {
			m_lightsChk->setValue((g_lightflag != 0), false);
		}
		if (m_highRpmChk) {
			m_highRpmChk->setValue(g_HighRpmMode, false);
		}
		if (m_bonemarkChk) {
			m_bonemarkChk->setValue((g_bonemarkflag != 0), false);
		}
		if (m_rigidmarkChk) {
			m_rigidmarkChk->setValue((g_rigidmarkflag != 0), false);
		}
		if (m_x180Chk) {
			m_x180Chk->setValue(g_x180flag, false);
		}
		if (m_rottraChk) {
			m_rottraChk->setValue(g_rotatetanim, false);
		}
		if (m_dofChk) {
			m_dofChk->setValue(g_zpreflag, false);
		}
		if (m_zcmpChk) {
			m_zcmpChk->setValue(g_zalways, false);
		}
		if (m_skydispChk) {
			m_skydispChk->setValue(g_skydispflag, false);
		}
		if (m_alphaChk) {
			m_alphaChk->setValue(g_alphablending, false);
		}

		m_dlgWnd->callRewrite();
	}

	return 0;
}

const HWND CDispLimitsDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CDispLimitsDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CDispLimitsDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

