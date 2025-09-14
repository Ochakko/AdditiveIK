#include "stdafx.h"

#include <ImpulseDlg.h>
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
// CImpulseDlg

CImpulseDlg::CImpulseDlg()
{
	InitParams();
}

CImpulseDlg::~CImpulseDlg()
{
	DestroyObjs();
}
	
int CImpulseDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_impgroupcheck) {
		delete m_impgroupcheck;
		m_impgroupcheck = nullptr;
	}
	if (m_impzSlider) {
		delete m_impzSlider;
		m_impzSlider = nullptr;
	}
	if (m_impySlider) {
		delete m_impySlider;
		m_impySlider = nullptr;
	}
	if (m_impxSlider) {
		delete m_impxSlider;
		m_impxSlider = nullptr;
	}
	if (m_impzlabel) {
		delete m_impzlabel;
		m_impzlabel = nullptr;
	}
	if (m_impylabel) {
		delete m_impylabel;
		m_impylabel = nullptr;
	}
	if (m_impxlabel) {
		delete m_impxlabel;
		m_impxlabel = nullptr;
	}
	if (m_impscaleSlider) {
		delete m_impscaleSlider;
		m_impscaleSlider = nullptr;
	}
	if (m_impscalelabel) {
		delete m_impscalelabel;
		m_impscalelabel = nullptr;
	}
	if (m_impallB) {
		delete m_impallB;
		m_impallB = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	m_model = nullptr;
	m_curboneno = -1;
	m_rgdindexmap.clear();


	return 0;
}

void CImpulseDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;
	m_curboneno = -1;
	m_rgdindexmap.clear();
	m_closeFlag = false;


	m_dlgWnd = nullptr;

	m_impgroupcheck = nullptr;
	m_impzSlider = nullptr;
	m_impySlider = nullptr;
	m_impxSlider = nullptr;
	m_impzlabel = nullptr;
	m_impylabel = nullptr;
	m_impxlabel = nullptr;
	m_impscaleSlider = nullptr;
	m_impscalelabel = nullptr;
	m_impallB = nullptr;

}

int CImpulseDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CImpulseDlg::SetFunctions(
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


void CImpulseDlg::SetModel(CModel* srcmodel, int srccurboneno, std::unordered_map<CModel*, int> srcrgdindexmap)
{
	m_model = srcmodel;
	m_curboneno = srccurboneno;
	m_rgdindexmap = srcrgdindexmap;

	CreateImpulseWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_skyst_Sc) {
		//	m_skyst_Sc->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CImpulseDlg::SetVisible(bool srcflag)
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


int CImpulseDlg::CreateImpulseWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	//////////
	m_dlgWnd = new OrgWindow(
		0,
		_T("ImpulseWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("ImpulseWindow"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		false,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否

	if (m_dlgWnd) {
		m_impgroupcheck = new OWP_CheckBoxA(L"SetToAllRigidSMeansToSetSameGroup", 0, 15, false);
		if (!m_impgroupcheck) {
			_ASSERT(0);
			return 1;
		}
		m_impxSlider = new OWP_Slider(0.0, 50.0, -50.0);
		if (!m_impxSlider) {
			_ASSERT(0);
			return 1;
		}
		m_impySlider = new OWP_Slider(0.0, 50.0, -50.0);
		if (!m_impySlider) {
			_ASSERT(0);
			return 1;
		}
		m_impzSlider = new OWP_Slider(0.0, 50.0, -50.0);
		if (!m_impzSlider) {
			_ASSERT(0);
			return 1;
		}
		m_impscaleSlider = new OWP_Slider(1.0, 10.0, 0.0);
		if (!m_impscaleSlider) {
			_ASSERT(0);
			return 1;
		}
		m_impxlabel = new OWP_Label(L"Impulse X", 15);
		if (!m_impxlabel) {
			_ASSERT(0);
			return 1;
		}
		m_impylabel = new OWP_Label(L"Impulse Y", 15);
		if (!m_impylabel) {
			_ASSERT(0);
			return 1;
		}
		m_impzlabel = new OWP_Label(L"Impulse Z", 15);
		if (!m_impzlabel) {
			_ASSERT(0);
			return 1;
		}
		m_impscalelabel = new OWP_Label(L"ScaleOfImpulse ", 15);
		if (!m_impscalelabel) {
			_ASSERT(0);
			return 1;
		}
		m_impallB = new OWP_Button(L"SetImpulseToAllRigies");
		if (!m_impallB) {
			_ASSERT(0);
			return 1;
		}

		int slw = 350;

		m_impzSlider->setSize(WindowSize(slw, 40));
		m_impySlider->setSize(WindowSize(slw, 40));

		m_dlgWnd->addParts(*m_impgroupcheck);
		m_dlgWnd->addParts(*m_impxlabel);
		m_dlgWnd->addParts(*m_impxSlider);
		m_dlgWnd->addParts(*m_impylabel);
		m_dlgWnd->addParts(*m_impySlider);
		m_dlgWnd->addParts(*m_impzlabel);
		m_dlgWnd->addParts(*m_impzSlider);
		m_dlgWnd->addParts(*m_impscalelabel);
		m_dlgWnd->addParts(*m_impscaleSlider);
		m_dlgWnd->addParts(*m_impallB);
		///////////
		//m_dsimpulsectrls.push_back(m_impgroupcheck);
		//m_dsimpulsectrls.push_back(m_impxlabel);
		//m_dsimpulsectrls.push_back(m_impxSlider);
		//m_dsimpulsectrls.push_back(m_impylabel);
		//m_dsimpulsectrls.push_back(m_impySlider);
		//m_dsimpulsectrls.push_back(m_impzlabel);
		//m_dsimpulsectrls.push_back(m_impzSlider);
		//m_dsimpulsectrls.push_back(m_impscalelabel);
		//m_dsimpulsectrls.push_back(m_impscaleSlider);
		//m_dsimpulsectrls.push_back(m_impallB);


		m_dlgWnd->setCloseListener([=, this]() {
			m_closeFlag = true;
			});

		m_impzSlider->setCursorListener([=, this]() {
			if (m_model && m_dlgWnd && m_impzSlider) {
				float val = (float)m_impzSlider->getValue();
				if (m_model) {
					m_model->SetImp(m_curboneno, 2, val);
				}
				m_dlgWnd->callRewrite();						//再描画
			}
			});
		m_impySlider->setCursorListener([=, this]() {
			if (m_model && m_dlgWnd && m_impySlider) {
				float val = (float)m_impySlider->getValue();
				if (m_model) {
					m_model->SetImp(m_curboneno, 1, val);
				}
				m_dlgWnd->callRewrite();						//再描画
			}
			});
		m_impxSlider->setCursorListener([=, this]() {
			if (m_model && m_dlgWnd && m_impxSlider) {
				float val = (float)m_impxSlider->getValue();
				if (m_model) {
					m_model->SetImp(m_curboneno, 0, val);
				}
				m_dlgWnd->callRewrite();						//再描画
			}
			});
		m_impscaleSlider->setCursorListener([=, this]() {
			if (m_model && m_dlgWnd && m_impscaleSlider) {
				float scale = (float)m_impscaleSlider->getValue();
				g_impscale = scale;
				m_dlgWnd->callRewrite();						//再描画
			}
			});
		m_impallB->setButtonListener([=, this]() {
			if (m_model && m_impxSlider && m_impySlider && m_impzSlider && m_impgroupcheck) {
				float impx = (float)m_impxSlider->getValue();
				float impy = (float)m_impySlider->getValue();
				float impz = (float)m_impzSlider->getValue();
				int chkg = (int)m_impgroupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRgdRigidElem(m_rgdindexmap[m_model], m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllImpulseData(gid, impx, impy, impz);
			}
			});
		//////////


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

int CImpulseDlg::ParamsToDlg()
{
	if (m_curboneno < 0) {
		return 0;
	}
	if (!m_model) {
		return 0;
	}
	if (m_rgdindexmap[m_model] < 0) {
		return 0;
	}


	CBone* curbone = m_model->GetBoneByID(m_curboneno);
	if (curbone) {
		CBone* parentbone = curbone->GetParent(false);
		if (parentbone && parentbone->IsSkeleton()) {
			ChaVector3 setimp(0.0f, 0.0f, 0.0f);


			int impnum = parentbone->GetImpMapSize();
			if ((m_model->GetCurImpIndex() >= 0) && (m_model->GetCurImpIndex() < impnum)) {
				string curimpname = m_model->GetImpInfo(m_model->GetCurImpIndex());
				setimp = parentbone->GetImpMap(curimpname, curbone);
			}
			else {
				//_ASSERT(0);
			}

			if (m_impzSlider) {
				m_impzSlider->setValue(setimp.z, false);
			}
			if (m_impySlider) {
				m_impySlider->setValue(setimp.y, false);
			}
			if (m_impxSlider) {
				m_impxSlider->setValue(setimp.x, false);
			}
			if (m_impscaleSlider) {
				m_impscaleSlider->setValue(g_impscale, false);
			}
		}
	}

	m_dlgWnd->callRewrite();

	return 0;
}

const HWND CImpulseDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CImpulseDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CImpulseDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

