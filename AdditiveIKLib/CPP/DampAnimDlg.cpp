#include "stdafx.h"

#include <DampAnimDlg.h>
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
// CDampAnimDlg

CDampAnimDlg::CDampAnimDlg()
{
	InitParams();
}

CDampAnimDlg::~CDampAnimDlg()
{
	DestroyObjs();
}
	
int CDampAnimDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_dmpgroupcheck) {
		delete m_dmpgroupcheck;
		m_dmpgroupcheck = nullptr;
	}
	if (m_dmpanimLlabel) {
		delete m_dmpanimLlabel;
		m_dmpanimLlabel = nullptr;
	}
	if (m_dmpanimLSlider) {
		delete m_dmpanimLSlider;
		m_dmpanimLSlider = nullptr;
	}
	if (m_dmpanimAlabel) {
		delete m_dmpanimAlabel;
		m_dmpanimAlabel = nullptr;
	}
	if (m_dmpanimASlider) {
		delete m_dmpanimASlider;
		m_dmpanimASlider = nullptr;
	}
	if (m_dmpanimB) {
		delete m_dmpanimB;
		m_dmpanimB = nullptr;
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

void CDampAnimDlg::InitParams()
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

	m_dmpgroupcheck = nullptr;
	m_dmpanimLlabel = nullptr;
	m_dmpanimLSlider = nullptr;
	m_dmpanimAlabel = nullptr;
	m_dmpanimASlider = nullptr;
	m_dmpanimB = nullptr;

}

int CDampAnimDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CDampAnimDlg::SetFunctions(
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


void CDampAnimDlg::SetModel(CModel* srcmodel, int srccurboneno, std::unordered_map<CModel*, int> srcrgdindexmap)
{
	m_model = srcmodel;
	m_curboneno = srccurboneno;
	m_rgdindexmap = srcrgdindexmap;

	CreateDampAnimWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_skyst_Sc) {
		//	m_skyst_Sc->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CDampAnimDlg::SetVisible(bool srcflag)
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


int CDampAnimDlg::CreateDampAnimWnd()
{
	if (m_dlgWnd) {//既に作成済は０リターン
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("dampAnimWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("AnimOfDumping"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		false,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否

	if (m_dlgWnd) {

		m_dmpgroupcheck = new OWP_CheckBoxA(L"SetToAllRigidsMeansToSetToSameGroup", 0, 15, false);
		if (!m_dmpgroupcheck) {
			_ASSERT(0);
			return 1;
		}
		m_dmpanimLlabel = new OWP_Label(L"posSpringDumpingPerFrame", 15);
		if (!m_dmpanimLlabel) {
			_ASSERT(0);
			return 1;
		}
		m_dmpanimLSlider = new OWP_Slider(0.0, 1.0, 0.0);
		if (!m_dmpanimLSlider) {
			_ASSERT(0);
			return 1;
		}
		m_dmpanimAlabel = new OWP_Label(L"rotSpringDumpingPerFrame", 15);
		if (!m_dmpanimAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_dmpanimASlider = new OWP_Slider(0.0, 1.0, 0.0);
		if (!m_dmpanimASlider) {
			_ASSERT(0);
			return 1;
		}
		m_dmpanimB = new OWP_Button(L"SetToAllRigids");
		if (!m_dmpanimB) {
			_ASSERT(0);
			return 1;
		}

		int slw2 = 500;
		if (m_dmpanimLSlider) {
			m_dmpanimLSlider->setSize(WindowSize(slw2, 40));
			m_dmpanimASlider->setSize(WindowSize(slw2, 40));
		}

		if (m_dlgWnd) {
			if (m_dmpgroupcheck) {
				m_dlgWnd->addParts(*m_dmpgroupcheck);
			}
			if (m_dmpanimLlabel) {
				m_dlgWnd->addParts(*m_dmpanimLlabel);
			}
			if (m_dmpanimLSlider) {
				m_dlgWnd->addParts(*m_dmpanimLSlider);
			}
			if (m_dmpanimAlabel) {
				m_dlgWnd->addParts(*m_dmpanimAlabel);
			}
			if (m_dmpanimASlider) {
				m_dlgWnd->addParts(*m_dmpanimASlider);
			}
			if (m_dmpanimB) {
				m_dlgWnd->addParts(*m_dmpanimB);
			}
		}


		//m_dsdampctrls.push_back(m_dmpgroupcheck);
		//m_dsdampctrls.push_back(m_dmpanimLlabel);
		//m_dsdampctrls.push_back(m_dmpanimLSlider);
		//m_dsdampctrls.push_back(m_dmpanimAlabel);
		//m_dsdampctrls.push_back(m_dmpanimASlider);
		//m_dsdampctrls.push_back(m_dmpanimB);

		if (m_dlgWnd) {
			m_dlgWnd->setCloseListener([=, this]() {
				m_closeFlag = true;
				});

			if (m_dmpanimLSlider) {
				m_dmpanimLSlider->setCursorListener([=, this]() {
					if (m_model) {
						CRigidElem* curre = m_model->GetRgdRigidElem(m_rgdindexmap[m_model], m_curboneno);
						if (curre) {
							float val = (float)m_dmpanimLSlider->getValue();
							curre->SetDampanimL(val);
						}
						m_dlgWnd->callRewrite();						//再描画
					}
					});
			}
			if (m_dmpanimASlider) {
				m_dmpanimASlider->setCursorListener([=, this]() {
					if (m_model) {
						CRigidElem* curre = m_model->GetRgdRigidElem(m_rgdindexmap[m_model], m_curboneno);
						if (curre) {
							float val = (float)m_dmpanimASlider->getValue();
							curre->SetDampanimA(val);
						}
						m_dlgWnd->callRewrite();						//再描画
					}
					});
			}
			if (m_dmpanimB) {
				m_dmpanimB->setButtonListener([=, this]() {
					if (m_model && (m_rgdindexmap[m_model] >= 0)) {
						float valL = (float)m_dmpanimLSlider->getValue();
						float valA = (float)m_dmpanimASlider->getValue();
						int chkg = (int)m_dmpgroupcheck->getValue();
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
						m_model->SetAllDampAnimData(gid, m_rgdindexmap[m_model], valL, valA);
					}
					});
			}

			m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
			m_dlgWnd->setPos(WindowPos(m_posx, m_posy));

			//１クリック目問題対応
			m_dlgWnd->refreshPosAndSize();//2022/09/20

			m_dlgWnd->callRewrite();
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;

}

int CDampAnimDlg::ParamsToDlg()
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
			char* filename = m_model->GetRigidElemInfo(m_rgdindexmap[m_model]).filename;
			CRigidElem* curre = parentbone->GetRigidElemOfMap(filename, curbone);
			if (curre) {
				if (m_dmpanimLSlider) {
					m_dmpanimLSlider->setValue(curre->GetDampanimL(), false);
				}
				if (m_dmpanimASlider) {
					m_dmpanimASlider->setValue(curre->GetDampanimA(), false);
				}
			}
		}
	}

	m_dlgWnd->callRewrite();

	return 0;
}

const HWND CDampAnimDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CDampAnimDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CDampAnimDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

