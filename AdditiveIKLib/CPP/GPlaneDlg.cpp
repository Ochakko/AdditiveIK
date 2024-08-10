#include "stdafx.h"

#include <GPlaneDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <mqoobject.h>
#include <mqomaterial.h>
#include <Bone.h>
#include <RigidElem.h>
#include <BtObject.h>
#include <OrgWindow.h>
#include <GlobalVar.h>

#include <BPWorld.h>

#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>


using namespace std;
using namespace OrgWinGUI;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル


/////////////////////////////////////////////////////////////////////////////
// CGPlaneDlg

CGPlaneDlg::CGPlaneDlg()
{
	InitParams();
}

CGPlaneDlg::~CGPlaneDlg()
{
	DestroyObjs();
}
	
int CGPlaneDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_ghSlider) {
		delete m_ghSlider;
		m_ghSlider = nullptr;
	}
	if (m_gsizexSlider) {
		delete m_gsizexSlider;
		m_gsizexSlider = nullptr;
	}
	if (m_gsizezSlider) {
		delete m_gsizezSlider;
		m_gsizezSlider = nullptr;
	}
	if (m_ghlabel) {
		delete m_ghlabel;
		m_ghlabel = nullptr;
	}
	if (m_gsizexlabel) {
		delete m_gsizexlabel;
		m_gsizexlabel = nullptr;
	}
	if (m_gsizezlabel) {
		delete m_gsizezlabel;
		m_gsizezlabel = nullptr;
	}
	if (m_gpdisp) {
		delete m_gpdisp;
		m_gpdisp = nullptr;
	}
	if (m_grestSlider) {
		delete m_grestSlider;
		m_grestSlider = nullptr;
	}
	if (m_grestlabel) {
		delete m_grestlabel;
		m_grestlabel = nullptr;
	}
	if (m_gfricSlider) {
		delete m_gfricSlider;
		m_gfricSlider = nullptr;
	}
	if (m_gfriclabel) {
		delete m_gfriclabel;
		m_gfriclabel = nullptr;
	}



	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	m_gplane = nullptr;

	return 0;
}

void CGPlaneDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_gplane = nullptr;
	m_closeFlag = false;


	m_dlgWnd = nullptr;

	m_ghSlider = nullptr;
	m_gsizexSlider = nullptr;
	m_gsizezSlider = nullptr;
	m_ghlabel = nullptr;
	m_gsizexlabel = nullptr;
	m_gsizezlabel = nullptr;
	m_gpdisp = nullptr;
	m_grestSlider = nullptr;
	m_grestlabel = nullptr;
	m_gfricSlider = nullptr;
	m_gfriclabel = nullptr;

}

int CGPlaneDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CGPlaneDlg::SetFunctions(
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


void CGPlaneDlg::SetModel(CModel* srcmodel, BPWorld* srcbpworld)
{
	m_gplane = srcmodel;
	m_bpWorld = srcbpworld;

	CreateGPlaneWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_skyst_Sc) {
		//	m_skyst_Sc->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CGPlaneDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		//CreateImpulseWnd();//作成済は０リターン

		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			
			//ParamsToDlg();

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


int CGPlaneDlg::CreateGPlaneWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	//////////
	m_dlgWnd = new OrgWindow(
		0,
		_T("GPlaneWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),		//位置
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("GroudOfPhysics"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		false,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否

	if (m_dlgWnd) {
		m_ghSlider = new OWP_Slider(-1.5, 5.0, -15.0);
		if (!m_ghSlider) {
			_ASSERT(0);
			return 1;
		}
		m_gsizexSlider = new OWP_Slider(5.0, 50.0, -50.0);
		if (!m_gsizexSlider) {
			_ASSERT(0);
			return 1;
		}
		m_gsizezSlider = new OWP_Slider(5.0, 50.0, -50.0);
		if (!m_gsizezSlider) {
			_ASSERT(0);
			return 1;
		}
		m_ghlabel = new OWP_Label(L"Height", 15);
		if (!m_ghlabel) {
			_ASSERT(0);
			return 1;
		}
		m_gsizexlabel = new OWP_Label(L"SizeOfX", 15);
		if (!m_gsizexlabel) {
			_ASSERT(0);
			return 1;
		}
		m_gsizezlabel = new OWP_Label(L"SizeOfZ", 15);
		if (!m_gsizezlabel) {
			_ASSERT(0);
			return 1;
		}
		m_gpdisp = new OWP_CheckBoxA(L"Display", 1, 15, false);
		if (!m_gpdisp) {
			_ASSERT(0);
			return 1;
		}

		m_grestSlider = new OWP_Slider(0.5, 1.0, 0.0);
		if (!m_grestSlider) {
			_ASSERT(0);
			return 1;
		}
		m_gfricSlider = new OWP_Slider(0.5, 1.0, 0.0);
		if (!m_gfricSlider) {
			_ASSERT(0);
			return 1;
		}
		m_grestlabel = new OWP_Label(L"Restitution", 15);
		if (!m_grestlabel) {
			_ASSERT(0);
			return 1;
		}
		m_gfriclabel = new OWP_Label(L"Friction", 15);
		if (!m_gfriclabel) {
			_ASSERT(0);
			return 1;
		}


		int slw = 350;

		m_ghSlider->setSize(WindowSize(slw, 40));
		m_gsizexSlider->setSize(WindowSize(slw, 40));
		m_gsizezSlider->setSize(WindowSize(slw, 40));
		m_grestSlider->setSize(WindowSize(slw, 40));
		m_gfricSlider->setSize(WindowSize(slw, 40));

		m_dlgWnd->addParts(*m_ghlabel);
		m_dlgWnd->addParts(*m_ghSlider);
		m_dlgWnd->addParts(*m_gsizexlabel);
		m_dlgWnd->addParts(*m_gsizexSlider);
		m_dlgWnd->addParts(*m_gsizezlabel);
		m_dlgWnd->addParts(*m_gsizezSlider);
		m_dlgWnd->addParts(*m_gpdisp);

		m_dlgWnd->addParts(*m_grestlabel);
		m_dlgWnd->addParts(*m_grestSlider);
		m_dlgWnd->addParts(*m_gfriclabel);
		m_dlgWnd->addParts(*m_gfricSlider);
		/////////
		//m_dsgpctrls.push_back(m_ghlabel);
		//m_dsgpctrls.push_back(m_ghSlider);
		//m_dsgpctrls.push_back(m_gsizexlabel);
		//m_dsgpctrls.push_back(m_gsizexSlider);
		//m_dsgpctrls.push_back(m_gsizezlabel);
		//m_dsgpctrls.push_back(m_gsizezSlider);
		//m_dsgpctrls.push_back(m_gpdisp);

		//m_dsgpctrls.push_back(m_grestlabel);
		//m_dsgpctrls.push_back(m_grestSlider);
		//m_dsgpctrls.push_back(m_gfriclabel);
		//m_dsgpctrls.push_back(m_gfricSlider);

		m_dlgWnd->setCloseListener([=, this]() {
			m_closeFlag = true;
			});

		m_ghSlider->setCursorListener([=, this]() {
			if (m_bpWorld && m_dlgWnd && m_ghSlider && m_gplane) {
				m_bpWorld->m_gplaneh = (float)m_ghSlider->getValue();
				m_bpWorld->RemakeG();

				ChaVector3 tra(0.0f, 0.0f, 0.0f);
				ChaVector3 mult(m_bpWorld->m_gplanesize.x, 1.0f, m_bpWorld->m_gplanesize.y);
				CallF(m_gplane->MultDispObj(mult, tra), return);

				m_dlgWnd->callRewrite();						//再描画
			}
			});
		m_gsizexSlider->setCursorListener([=, this]() {
			if (m_dlgWnd && m_gsizexSlider) {
				if (m_bpWorld && m_gplane) {
					m_bpWorld->m_gplanesize.x = (float)m_gsizexSlider->getValue();

					ChaVector3 tra(0.0f, 0.0f, 0.0f);
					ChaVector3 mult(m_bpWorld->m_gplanesize.x, 1.0f, m_bpWorld->m_gplanesize.y);
					CallF(m_gplane->MultDispObj(mult, tra), return);
					m_dlgWnd->callRewrite();						//再描画
				}
			}
			});
		m_gsizezSlider->setCursorListener([=, this]() {
			if (m_dlgWnd && m_gsizezSlider) {
				if (m_bpWorld && m_gplane) {
					m_bpWorld->m_gplanesize.y = (float)m_gsizezSlider->getValue();

					ChaVector3 tra(0.0f, 0.0f, 0.0f);
					ChaVector3 mult(m_bpWorld->m_gplanesize.x, 1.0f, m_bpWorld->m_gplanesize.y);
					CallF(m_gplane->MultDispObj(mult, tra), return);
					m_dlgWnd->callRewrite();						//再描画
				}
			}
			});
		m_gpdisp->setButtonListener([=, this]() {
			if (m_dlgWnd && m_gpdisp) {
				if (m_bpWorld) {
					bool dispflag = m_gpdisp->getValue();
					m_bpWorld->m_gplanedisp = (int)dispflag;
					m_dlgWnd->callRewrite();						//再描画
				}
			}
			});
		m_grestSlider->setCursorListener([=, this]() {
			if (m_dlgWnd && m_grestSlider) {
				if (m_bpWorld && m_gplane) {
					m_bpWorld->m_restitution = (float)m_grestSlider->getValue();
					m_bpWorld->RemakeG();

					m_dlgWnd->callRewrite();						//再描画
				}
			}
			});
		m_gfricSlider->setCursorListener([=, this]() {
			if (m_dlgWnd && m_gfricSlider) {
				if (m_bpWorld && m_gplane) {
					m_bpWorld->m_friction = (float)m_gfricSlider->getValue();
					m_bpWorld->RemakeG();

					m_dlgWnd->callRewrite();						//再描画
				}
			}
			});


		m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgWnd->setPos(WindowPos(m_posx, m_posy));

		//１クリック目問題対応
		m_dlgWnd->refreshPosAndSize();//2022/09/20

		m_dlgWnd->callRewrite();
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;


}

int CGPlaneDlg::ParamsToDlg()
{
	if (!m_bpWorld) {
		return 0;
	}

	if (m_ghSlider) {
		m_ghSlider->setValue(m_bpWorld->m_gplaneh, false);
	}
	if (m_gsizexSlider) {
		m_gsizexSlider->setValue(m_bpWorld->m_gplanesize.x, false);
	}
	if (m_gsizezSlider) {
		m_gsizezSlider->setValue(m_bpWorld->m_gplanesize.y, false);
	}
	if (m_gpdisp) {
		m_gpdisp->setValue((bool)m_bpWorld->m_gplanedisp, false);
	}

	if (m_grestSlider) {
		m_grestSlider->setValue(m_bpWorld->m_restitution, false);
	}
	if (m_gfricSlider) {
		m_gfricSlider->setValue(m_bpWorld->m_friction, false);
	}

	m_dlgWnd->callRewrite();

	return 0;
}

const HWND CGPlaneDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CGPlaneDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CGPlaneDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

