﻿#include "stdafx.h"

#include <FootRigDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <ChaScene.h>
#include <Model.h>
#include <Bone.h>
#include <RigidElem.h>
#include <BtObject.h>
#include <OrgWindow.h>
#include <GlobalVar.h>

#include <FootRigFile.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>


using namespace std;
using namespace OrgWinGUI;

#define FOOTRIGPICKHEIGHT	1000.0f

//位置補正のトリガーを少し緩めるために使用
//#define ROUNDINGPOS	2.0f
#define ROUNDINGPOS	0.05f


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル


CFootInfo* s_leftfootinfo = nullptr;
CFootInfo* s_rightfootinfo = nullptr;


/////////////////////////////////////////////////////////////////////////////
// CFootRigDlg

CFootRigDlg::CFootRigDlg()
{
	InitParams();
}

CFootRigDlg::~CFootRigDlg()
{
	DestroyObjs();
}
	
int CFootRigDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (s_leftfootinfo) {
		delete s_leftfootinfo;
		s_leftfootinfo = nullptr;
	}
	if (s_rightfootinfo) {
		delete s_rightfootinfo;
		s_rightfootinfo = nullptr;
	}


	if (m_enableChk) {
		delete m_enableChk;
		m_enableChk = nullptr;
	}
	if (m_modellabel) {
		delete m_modellabel;
		m_modellabel = nullptr;
	}
	if (m_groundlabel) {
		delete m_groundlabel;
		m_groundlabel = nullptr;
	}
	if (m_groundCombo) {
		delete m_groundCombo;
		m_groundCombo = nullptr;
	}
	if (m_gpuChk) {
		delete m_gpuChk;
		m_gpuChk = nullptr;
	}
	if (m_leftfootlabel) {
		delete m_leftfootlabel;
		m_leftfootlabel = nullptr;
	}
	if (m_leftfootBonelabel) {
		delete m_leftfootBonelabel;
		m_leftfootBonelabel = nullptr;
	}
	if (m_leftfootBoneCombo) {
		delete m_leftfootBoneCombo;
		m_leftfootBoneCombo = nullptr;
	}
	if (m_leftoffsetLabelY1) {
		delete m_leftoffsetLabelY1;
		m_leftoffsetLabelY1 = nullptr;
	}
	if (m_leftoffsetLabelZ1) {
		delete m_leftoffsetLabelZ1;
		m_leftoffsetLabelZ1 = nullptr;
	}
	if (m_leftoffsetLabelY2) {
		delete m_leftoffsetLabelY2;
		m_leftoffsetLabelY2 = nullptr;
	}
	if (m_leftoffsetLabelZ2) {
		delete m_leftoffsetLabelZ2;
		m_leftoffsetLabelZ2 = nullptr;
	}
	if (m_leftoffsetEditY1) {
		delete m_leftoffsetEditY1;
		m_leftoffsetEditY1 = nullptr;
	}
	if (m_leftoffsetEditZ1) {
		delete m_leftoffsetEditZ1;
		m_leftoffsetEditZ1 = nullptr;
	}
	if (m_leftoffsetEditY2) {
		delete m_leftoffsetEditY2;
		m_leftoffsetEditY2 = nullptr;
	}
	if (m_leftoffsetEditZ2) {
		delete m_leftoffsetEditZ2;
		m_leftoffsetEditZ2 = nullptr;
	}
	if (m_leftriglabel) {
		delete m_leftriglabel;
		m_leftriglabel = nullptr;
	}
	if (m_leftrigCombo) {
		delete m_leftrigCombo;
		m_leftrigCombo = nullptr;
	}
	if (m_leftdirlabel) {
		delete m_leftdirlabel;
		m_leftdirlabel = nullptr;
	}
	if (m_leftdirCombo) {
		delete m_leftdirCombo;
		m_leftdirCombo = nullptr;
	}
	if (m_rightfootlabel) {
		delete m_rightfootlabel;
		m_rightfootlabel = nullptr;
	}
	if (m_rightfootBonelabel) {
		delete m_rightfootBonelabel;
		m_rightfootBonelabel = nullptr;
	}
	if (m_rightfootBoneCombo) {
		delete m_rightfootBoneCombo;
		m_rightfootBoneCombo = nullptr;
	}
	if (m_rightoffsetLabelY1) {
		delete m_rightoffsetLabelY1;
		m_rightoffsetLabelY1 = nullptr;
	}
	if (m_rightoffsetLabelZ1) {
		delete m_rightoffsetLabelZ1;
		m_rightoffsetLabelZ1 = nullptr;
	}
	if (m_rightoffsetLabelY2) {
		delete m_rightoffsetLabelY2;
		m_rightoffsetLabelY2 = nullptr;
	}
	if (m_rightoffsetLabelZ2) {
		delete m_rightoffsetLabelZ2;
		m_rightoffsetLabelZ2 = nullptr;
	}
	if (m_rightoffsetEditY1) {
		delete m_rightoffsetEditY1;
		m_rightoffsetEditY1 = nullptr;
	}
	if (m_rightoffsetEditZ1) {
		delete m_rightoffsetEditZ1;
		m_rightoffsetEditZ1 = nullptr;
	}
	if (m_rightoffsetEditY2) {
		delete m_rightoffsetEditY2;
		m_rightoffsetEditY2 = nullptr;
	}
	if (m_rightoffsetEditZ2) {
		delete m_rightoffsetEditZ2;
		m_rightoffsetEditZ2 = nullptr;
	}
	if (m_rightriglabel) {
		delete m_rightriglabel;
		m_rightriglabel = nullptr;
	}
	if (m_rightrigCombo) {
		delete m_rightrigCombo;
		m_rightrigCombo = nullptr;
	}
	if (m_rightdirlabel) {
		delete m_rightdirlabel;
		m_rightdirlabel = nullptr;
	}
	if (m_rightdirCombo) {
		delete m_rightdirCombo;
		m_rightdirCombo = nullptr;
	}
	if (m_hdiffmaxlabel) {
		delete m_hdiffmaxlabel;
		m_hdiffmaxlabel = nullptr;
	}
	if (m_hdiffmaxEdit) {
		delete m_hdiffmaxEdit;
		m_hdiffmaxEdit = nullptr;
	}
	if (m_rigsteplabel) {
		delete m_rigsteplabel;
		m_rigsteplabel = nullptr;
	}
	if (m_rigstepEdit) {
		delete m_rigstepEdit;
		m_rigstepEdit = nullptr;
	}
	if (m_maxcountlabel) {
		delete m_maxcountlabel;
		m_maxcountlabel = nullptr;
	}
	if (m_maxcountEdit) {
		delete m_maxcountEdit;
		m_maxcountEdit = nullptr;
	}
	if (m_hopypersteplabel) {
		delete m_hopypersteplabel;
		m_hopypersteplabel = nullptr;
	}
	if (m_hopyperstepEdit) {
		delete m_hopyperstepEdit;
		m_hopyperstepEdit = nullptr;
	}
	if (m_wmblendlabel) {
		delete m_wmblendlabel;
		m_wmblendlabel = nullptr;
	}
	if (m_wmblendSlider) {
		delete m_wmblendSlider;
		m_wmblendSlider = nullptr;
	}
	//if (m_applyB) {
	//	delete m_applyB;
	//	m_applyB = nullptr;
	//}

	//separator sp
	if (m_groundmeshsp) {
		delete m_groundmeshsp;
		m_groundmeshsp = nullptr;
	}
	if (m_leftfootbonesp) {
		delete m_leftfootbonesp;
		m_leftfootbonesp = nullptr;
	}
	if (m_leftoffsetspY1) {
		delete m_leftoffsetspY1;
		m_leftoffsetspY1 = nullptr;
	}
	if (m_leftoffsetspZ1) {
		delete m_leftoffsetspZ1;
		m_leftoffsetspZ1 = nullptr;
	}
	if (m_leftoffsetspY2) {
		delete m_leftoffsetspY2;
		m_leftoffsetspY2 = nullptr;
	}
	if (m_leftoffsetspZ2) {
		delete m_leftoffsetspZ2;
		m_leftoffsetspZ2 = nullptr;
	}
	if (m_leftrigsp) {
		delete m_leftrigsp;
		m_leftrigsp = nullptr;
	}
	if (m_leftdirsp) {
		delete m_leftdirsp;
		m_leftdirsp = nullptr;
	}
	if (m_rightfootbonesp) {
		delete m_rightfootbonesp;
		m_rightfootbonesp = nullptr;
	}
	if (m_rightoffsetspY1) {
		delete m_rightoffsetspY1;
		m_rightoffsetspY1 = nullptr;
	}
	if (m_rightoffsetspZ1) {
		delete m_rightoffsetspZ1;
		m_rightoffsetspZ1 = nullptr;
	}
	if (m_rightoffsetspY2) {
		delete m_rightoffsetspY2;
		m_rightoffsetspY2 = nullptr;
	}
	if (m_rightoffsetspZ2) {
		delete m_rightoffsetspZ2;
		m_rightoffsetspZ2 = nullptr;
	}
	if (m_rightrigsp) {
		delete m_rightrigsp;
		m_rightrigsp = nullptr;
	}
	if (m_rightdirsp) {
		delete m_rightdirsp;
		m_rightdirsp = nullptr;
	}
	if (m_hdiffmaxsp) {
		delete m_hdiffmaxsp;
		m_hdiffmaxsp = nullptr;
	}
	if (m_rigstepsp) {
		delete m_rigstepsp;
		m_rigstepsp = nullptr;
	}
	if (m_maxcountsp) {
		delete m_maxcountsp;
		m_maxcountsp = nullptr;
	}
	if (m_gpusp) {
		delete m_gpusp;
		m_gpusp = nullptr;
	}
	if (m_hopyperstepsp) {
		delete m_hopyperstepsp;
		m_hopyperstepsp = nullptr;
	}
	if (m_wmblendsp) {
		delete m_wmblendsp;
		m_wmblendsp = nullptr;
	}

	//spacer
	if (m_spacerlabel0) {
		delete m_spacerlabel0;
		m_spacerlabel0 = nullptr;
	}
	if (m_spacerlabel1) {
		delete m_spacerlabel1;
		m_spacerlabel1 = nullptr;
	}
	if (m_spacerlabel2) {
		delete m_spacerlabel2;
		m_spacerlabel2 = nullptr;
	}
	if (m_spacerlabel3) {
		delete m_spacerlabel3;
		m_spacerlabel3 = nullptr;
	}
	if (m_spacerlabel4) {
		delete m_spacerlabel4;
		m_spacerlabel4 = nullptr;
	}
	if (m_spacerlabel5) {
		delete m_spacerlabel5;
		m_spacerlabel5 = nullptr;
	}
	if (m_spacerlabel6) {
		delete m_spacerlabel6;
		m_spacerlabel6 = nullptr;
	}



	if (m_leftinfolabel) {
		delete m_leftinfolabel;
		m_leftinfolabel = nullptr;
	}
	if (m_rightinfolabel) {
		delete m_rightinfolabel;
		m_rightinfolabel = nullptr;
	}



	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CFootRigDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;
	m_chascene = nullptr;
	m_footrigelem.clear();
	m_savemodelwm.clear();

	m_dlgWnd = nullptr;

	m_enableChk = nullptr;
	m_modellabel = nullptr;
	m_groundlabel = nullptr;
	m_groundCombo = nullptr;
	m_gpuChk = nullptr;
	m_leftfootlabel = nullptr;
	m_leftfootBonelabel = nullptr;
	m_leftfootBoneCombo = nullptr;
	m_leftoffsetLabelY1 = nullptr;
	m_leftoffsetLabelZ1 = nullptr;
	m_leftoffsetEditY1 = nullptr;
	m_leftoffsetEditZ1 = nullptr;
	m_leftoffsetLabelY2 = nullptr;
	m_leftoffsetLabelZ2 = nullptr;
	m_leftoffsetEditY2 = nullptr;
	m_leftoffsetEditZ2 = nullptr;
	m_leftriglabel = nullptr;
	m_leftrigCombo = nullptr;
	m_leftdirlabel = nullptr;
	m_leftdirCombo = nullptr;
	m_rightfootlabel = nullptr;
	m_rightfootBonelabel = nullptr;
	m_rightfootBoneCombo = nullptr;
	m_rightoffsetLabelY1 = nullptr;
	m_rightoffsetLabelZ1 = nullptr;
	m_rightoffsetEditY1 = nullptr;
	m_rightoffsetEditZ1 = nullptr;
	m_rightoffsetLabelY2 = nullptr;
	m_rightoffsetLabelZ2 = nullptr;
	m_rightoffsetEditY2 = nullptr;
	m_rightoffsetEditZ2 = nullptr;
	m_rightriglabel = nullptr;
	m_rightrigCombo = nullptr;
	m_rightdirlabel = nullptr;
	m_rightdirCombo = nullptr;
	m_hdiffmaxlabel = nullptr;
	m_hdiffmaxEdit = nullptr;
	m_rigsteplabel = nullptr;
	m_rigstepEdit = nullptr;
	m_maxcountlabel = nullptr;
	m_maxcountEdit = nullptr;
	m_hopypersteplabel = nullptr;
	m_hopyperstepEdit = nullptr;
	m_wmblendlabel = nullptr;
	m_wmblendSlider = nullptr;
	//m_applyB = nullptr;

	m_groundmeshsp = nullptr;
	m_leftfootbonesp = nullptr;
	m_leftoffsetspY1 = nullptr;
	m_leftoffsetspZ1 = nullptr;
	m_leftoffsetspY2 = nullptr;
	m_leftoffsetspZ2 = nullptr;
	m_leftrigsp = nullptr;
	m_leftdirsp = nullptr;
	m_rightfootbonesp = nullptr;
	m_rightoffsetspY1 = nullptr;
	m_rightoffsetspZ1 = nullptr;
	m_rightoffsetspY2 = nullptr;
	m_rightoffsetspZ2 = nullptr;
	m_rightrigsp = nullptr;
	m_rightdirsp = nullptr;
	m_hdiffmaxsp = nullptr;
	m_rigstepsp = nullptr;
	m_hopyperstepsp = nullptr;
	m_wmblendsp = nullptr;
	m_maxcountsp = nullptr;
	m_gpusp = nullptr;

	m_spacerlabel0 = nullptr;
	m_spacerlabel1 = nullptr;
	m_spacerlabel2 = nullptr;
	m_spacerlabel3 = nullptr;
	m_spacerlabel4 = nullptr;
	m_spacerlabel5 = nullptr;
	m_spacerlabel6 = nullptr;

	m_leftinfolabel = nullptr;
	m_rightinfolabel = nullptr;

}




int CFootRigDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

int CFootRigDlg::SetModel(ChaScene* srcchascene, CModel* srcmodel)
{
	m_model = srcmodel;
	m_chascene = srcchascene;
	if (m_model && m_chascene) {
		int mindex = m_chascene->FindModelIndex(m_model);//削除されていないことを確認
		if (mindex >= 0) {
			std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
			itrelem = m_footrigelem.find(m_model);
			if (itrelem == m_footrigelem.end()) {
				FOOTRIGELEM newfootelem;
				newfootelem.Init();
				m_footrigelem[m_model] = newfootelem;
			}
			else {
				bool gpucollision = itrelem->second.gpucollision;
				CModel* groundmodel = itrelem->second.groundmodel;
				if (groundmodel) {
					int gindex = m_chascene->FindModelIndex(groundmodel);//削除されていないことを確認
					if (gindex >= 0) {
						int result = groundmodel->SetGPUInteraction(gpucollision);//当たり判定シェーダの準備
						if (result != 0) {
							_ASSERT(0);
						}
					}
				}
			}

			ChaMatrix newmodelwm;
			newmodelwm = m_model->GetWorldMat();
			SetSaveModelWM(m_model, newmodelwm);

			CreateFootRigWnd();//作成済の場合は０リターン
			ParamsToDlg();
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}
	return 0;
}

int CFootRigDlg::SetEditedRig(CModel* srcmodel, CBone* srcrigbone, CUSTOMRIG updatedrig)
{

	//AdditiveIK.cppでのCUSTOMRIGの編集結果をCFootRigDlgの保持データに反映する

	CBone* rigbone[2] = { nullptr, nullptr };
	CUSTOMRIG customrig[2];
	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	itrelem = m_footrigelem.find(m_model);
	if (itrelem != m_footrigelem.end()) {

		rigbone[0] = itrelem->second.leftfootbone;
		rigbone[1] = itrelem->second.rightfootbone;
		customrig[0] = itrelem->second.leftrig;
		customrig[1] = itrelem->second.rightrig;

		int rigindex;
		for (rigindex = 0; rigindex < 2; rigindex++) {
			if (srcrigbone && (srcrigbone == rigbone[rigindex])) {//FootRigDlgで選択したfootboneと編集したリグボーンが同じ場合
				CBone* chkbone = srcmodel->GetBoneByID(updatedrig.rigboneno);
				if (chkbone && (chkbone == rigbone[rigindex])) {//FootRigDlgで選択したfootboneと編集したリグボーンが同じ場合
					if (customrig[rigindex].rigname[0] && updatedrig.rigname[0] &&
						(wcscmp(customrig[rigindex].rigname, updatedrig.rigname) == 0)) {//FootRigDlgで選択したリグの名前と編集したリグの名前が同じ場合
						if (rigindex == 0) {
							itrelem->second.leftrig = updatedrig;
						}
						else if (rigindex == 1) {
							itrelem->second.rightrig = updatedrig;
						}
					}
				}
			}
		}

		//CBone* curbone = itrelem->second.leftfootbone;
		//if (curbone) {
		//	string curcombo = m_leftrigCombo->getSelectedComboStr();
		//	CUSTOMRIG setrig = curbone->GetCustomRig(curcombo);
		//	itrelem->second.leftrig = setrig;
		//}
		//else {
		//	itrelem->second.leftrig.Init();
		//}
	}




	return 0;
}

void CFootRigDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		//CreateFootRigWnd();//作成済の場合は０リターン

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


int CFootRigDlg::CreateFootRigWnd()
{
	if (!s_leftfootinfo) {
		s_leftfootinfo = new CFootInfo(this);
		if (!s_leftfootinfo) {
			_ASSERT(0);
			return 1;
		}
	}
	if (!s_rightfootinfo) {
		s_rightfootinfo = new CFootInfo(this);
		if (!s_rightfootinfo) {
			_ASSERT(0);
			return 1;
		}
	}



	if (m_dlgWnd) {
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("FootRigDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("FootRigDlg"),	//タイトル
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

		m_enableChk = new OWP_CheckBoxA(L"Eanble FootRig", false, labelheight, false);
		if (!m_enableChk) {
			_ASSERT(0);
			abort();
		}
		m_modellabel = new OWP_Label(L"ModelName : ", labelheight);
		if (!m_modellabel) {
			_ASSERT(0);
			abort();
		}
		m_groundlabel = new OWP_Label(L"GroundMesh", labelheight);
		if (!m_groundlabel) {
			_ASSERT(0);
			abort();
		}
		m_groundCombo = new OWP_ComboBoxA(L"GroundMeshCombo", labelheight);
		if (!m_groundCombo) {
			_ASSERT(0);
			abort();
		}
		m_gpuChk = new OWP_CheckBoxA(L"GPU Collision", false, labelheight, false);
		if (!m_gpuChk) {
			_ASSERT(0);
			abort();
		}


		m_leftfootlabel = new OWP_Label(L"LeftFoot", labelheight);
		if (!m_leftfootlabel) {
			_ASSERT(0);
			abort();
		}
		m_leftfootBonelabel = new OWP_Label(L"Joint", labelheight);
		if (!m_leftfootBonelabel) {
			_ASSERT(0);
			abort();
		}
		m_leftfootBoneCombo = new OWP_ComboBoxA(L"LeftFootBoneCombo", labelheight);
		if (!m_leftfootBoneCombo) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetLabelY1 = new OWP_Label(L"toebaseL OffsetY", labelheight);
		if (!m_leftoffsetLabelY1) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetLabelZ1 = new OWP_Label(L"toebaseL OffsetZ", labelheight);
		if (!m_leftoffsetLabelZ1) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetLabelY2 = new OWP_Label(L"footL OffsetY", labelheight);
		if (!m_leftoffsetLabelY2) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetLabelZ2 = new OWP_Label(L"footL OffsetZ", labelheight);
		if (!m_leftoffsetLabelZ2) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetEditY1 = new OWP_EditBox(true, L"leftoffsetEditY1", labelheight, EDIT_BUFLEN_NUM);
		if (!m_leftoffsetEditY1) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetEditZ1 = new OWP_EditBox(true, L"leftoffsetEditZ1", labelheight, EDIT_BUFLEN_NUM);
		if (!m_leftoffsetEditZ1) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetEditY2 = new OWP_EditBox(true, L"leftoffsetEditY2", labelheight, EDIT_BUFLEN_NUM);
		if (!m_leftoffsetEditY2) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetEditZ2 = new OWP_EditBox(true, L"leftoffsetEditZ2", labelheight, EDIT_BUFLEN_NUM);
		if (!m_leftoffsetEditZ2) {
			_ASSERT(0);
			abort();
		}
		m_leftriglabel = new OWP_Label(L"Rig", labelheight);
		if (!m_leftriglabel) {
			_ASSERT(0);
			abort();
		}
		m_leftrigCombo = new OWP_ComboBoxA(L"leftrigCombo", labelheight);
		if (!m_leftrigCombo) {
			_ASSERT(0);
			abort();
		}
		m_leftdirlabel = new OWP_Label(L"Rig Direction", labelheight);
		if (!m_leftdirlabel) {
			_ASSERT(0);
			abort();
		}
		m_leftdirCombo = new OWP_ComboBoxA(L"leftdirCombo", labelheight);
		if (!m_leftdirCombo) {
			_ASSERT(0);
			abort();
		}


		m_rightfootlabel = new OWP_Label(L"RightFoot", labelheight);
		if (!m_rightfootlabel) {
			_ASSERT(0);
			abort();
		}
		m_rightfootBonelabel = new OWP_Label(L"Joint", labelheight);
		if (!m_rightfootBonelabel) {
			_ASSERT(0);
			abort();
		}
		m_rightfootBoneCombo = new OWP_ComboBoxA(L"RightFootBoneCombo", labelheight);
		if (!m_rightfootBoneCombo) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetLabelY1 = new OWP_Label(L"toebaseR OffsetY", labelheight);
		if (!m_rightoffsetLabelY1) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetLabelZ1 = new OWP_Label(L"toebaseR OffsetZ", labelheight);
		if (!m_rightoffsetLabelZ1) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetLabelY2 = new OWP_Label(L"footR OffsetY", labelheight);
		if (!m_rightoffsetLabelY2) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetLabelZ2 = new OWP_Label(L"footR OffsetZ", labelheight);
		if (!m_rightoffsetLabelZ2) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetEditY1 = new OWP_EditBox(true, L"rightoffsetEditY1", labelheight, EDIT_BUFLEN_NUM);
		if (!m_rightoffsetEditY1) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetEditZ1 = new OWP_EditBox(true, L"rightoffsetEditZ1", labelheight, EDIT_BUFLEN_NUM);
		if (!m_rightoffsetEditZ1) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetEditY2 = new OWP_EditBox(true, L"rightoffsetEditY2", labelheight, EDIT_BUFLEN_NUM);
		if (!m_rightoffsetEditY2) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetEditZ2 = new OWP_EditBox(true, L"rightoffsetEditZ2", labelheight, EDIT_BUFLEN_NUM);
		if (!m_rightoffsetEditZ2) {
			_ASSERT(0);
			abort();
		}
		m_rightriglabel = new OWP_Label(L"Rig", labelheight);
		if (!m_rightriglabel) {
			_ASSERT(0);
			abort();
		}
		m_rightrigCombo = new OWP_ComboBoxA(L"rightrigCombo", labelheight);
		if (!m_rightrigCombo) {
			_ASSERT(0);
			abort();
		}
		m_rightdirlabel = new OWP_Label(L"Rig Direction", labelheight);
		if (!m_rightdirlabel) {
			_ASSERT(0);
			abort();
		}
		m_rightdirCombo = new OWP_ComboBoxA(L"rightdirCombo", labelheight);
		if (!m_rightdirCombo) {
			_ASSERT(0);
			abort();
		}


		m_hdiffmaxlabel = new OWP_Label(L"Height difference max between Hips and Ground", labelheight);
		if (!m_hdiffmaxlabel) {
			_ASSERT(0);
			abort();
		}
		m_hdiffmaxEdit = new OWP_EditBox(true, L"hdiffmaxEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_hdiffmaxEdit) {
			_ASSERT(0);
			abort();
		}
		m_rigsteplabel = new OWP_Label(L"Rig Step", labelheight);
		if (!m_rigsteplabel) {
			_ASSERT(0);
			abort();
		}
		m_rigstepEdit = new OWP_EditBox(true, L"rigstepEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_rigstepEdit) {
			_ASSERT(0);
			abort();
		}
		m_maxcountlabel = new OWP_Label(L"MaxCalcCount", labelheight);
		if (!m_maxcountlabel) {
			_ASSERT(0);
			abort();
		}
		m_maxcountEdit = new OWP_EditBox(true, L"MaxCalcCount", labelheight, EDIT_BUFLEN_NUM);
		if (!m_maxcountEdit) {
			_ASSERT(0);
			abort();
		}
		m_hopypersteplabel = new OWP_Label(L"Hop Y per Step", labelheight);
		if (!m_hopypersteplabel) {
			_ASSERT(0);
			abort();
		}
		m_hopyperstepEdit = new OWP_EditBox(true, L"hopyperstepEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_hopyperstepEdit) {
			_ASSERT(0);
			abort();
		}
		m_wmblendlabel = new OWP_Label(L"BlendRate WM", labelheight);
		if (!m_wmblendlabel) {
			_ASSERT(0);
			abort();
		}
		m_wmblendSlider = new OWP_Slider(0.0f, 1.0f, 0.0f, labelheight);
		if (!m_wmblendSlider) {
			_ASSERT(0);
			abort();
		}
		//m_applyB = new OWP_Button(L"Apply(適用)", 38);
		//if (!m_applyB) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_applyB->setTextColor(RGB(168, 129, 129));


		m_groundmeshsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_groundmeshsp) {
			_ASSERT(0);
			abort();
		}
		m_leftfootbonesp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftfootbonesp) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetspY1 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftoffsetspY1) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetspZ1 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftoffsetspZ1) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetspY2 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftoffsetspY2) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetspZ2 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftoffsetspZ2) {
			_ASSERT(0);
			abort();
		}
		m_leftrigsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftrigsp) {
			_ASSERT(0);
			abort();
		}
		m_leftdirsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftdirsp) {
			_ASSERT(0);
			abort();
		}
		m_rightfootbonesp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightfootbonesp) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetspY1 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightoffsetspY1) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetspZ1 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightoffsetspZ1) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetspY2 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightoffsetspY2) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetspZ2 = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightoffsetspZ2) {
			_ASSERT(0);
			abort();
		}
		m_rightrigsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightrigsp) {
			_ASSERT(0);
			abort();
		}
		m_rightdirsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightdirsp) {
			_ASSERT(0);
			abort();
		}
		m_hdiffmaxsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_hdiffmaxsp) {
			_ASSERT(0);
			abort();
		}
		m_rigstepsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rigstepsp) {
			_ASSERT(0);
			abort();
		}
		m_maxcountsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_maxcountsp) {
			_ASSERT(0);
			abort();
		}
		m_gpusp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_gpusp) {
			_ASSERT(0);
			abort();
		}
		m_hopyperstepsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_hopyperstepsp) {
			_ASSERT(0);
			abort();
		}
		m_wmblendsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_wmblendsp) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel0 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel0) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel1 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel1) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel2 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel2) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel3 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel3) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel4 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel4) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel5 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel5) {
			_ASSERT(0);
			abort();
		}
		m_spacerlabel6 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerlabel6) {
			_ASSERT(0);
			abort();
		}

		m_leftinfolabel = new OWP_Label(L"LeftFoot(***.**, ***.**), LeftGround ***.**", labelheight);
		if (!m_leftinfolabel) {
			_ASSERT(0);
			abort();
		}
		m_rightinfolabel = new OWP_Label(L"RightFoot(***.**, ***.**), RightGround ***.**", labelheight);
		if (!m_rightinfolabel) {
			_ASSERT(0);
			abort();
		}


		m_dlgWnd->addParts(*m_modellabel);
		m_dlgWnd->addParts(*m_enableChk);
		m_dlgWnd->addParts(*m_spacerlabel0);

		m_dlgWnd->addParts(*m_groundmeshsp);
		m_groundmeshsp->addParts1(*m_groundlabel);
		m_groundmeshsp->addParts2(*m_groundCombo);
		m_dlgWnd->addParts(*m_gpusp);
		m_gpusp->addParts2(*m_gpuChk);

		m_dlgWnd->addParts(*m_spacerlabel1);
		m_dlgWnd->addParts(*m_leftfootlabel);
		m_dlgWnd->addParts(*m_leftfootbonesp);
		m_leftfootbonesp->addParts1(*m_leftfootBonelabel);
		m_leftfootbonesp->addParts2(*m_leftfootBoneCombo);

		m_dlgWnd->addParts(*m_leftoffsetspY1);
		m_leftoffsetspY1->addParts1(*m_leftoffsetLabelY1);
		m_leftoffsetspY1->addParts2(*m_leftoffsetEditY1);
		m_dlgWnd->addParts(*m_leftoffsetspZ1);
		m_leftoffsetspZ1->addParts1(*m_leftoffsetLabelZ1);
		m_leftoffsetspZ1->addParts2(*m_leftoffsetEditZ1);

		m_dlgWnd->addParts(*m_leftoffsetspY2);
		m_leftoffsetspY2->addParts1(*m_leftoffsetLabelY2);
		m_leftoffsetspY2->addParts2(*m_leftoffsetEditY2);
		m_dlgWnd->addParts(*m_leftoffsetspZ2);
		m_leftoffsetspZ2->addParts1(*m_leftoffsetLabelZ2);
		m_leftoffsetspZ2->addParts2(*m_leftoffsetEditZ2);

		m_dlgWnd->addParts(*m_leftrigsp);
		m_leftrigsp->addParts1(*m_leftriglabel);
		m_leftrigsp->addParts2(*m_leftrigCombo);
		m_dlgWnd->addParts(*m_leftdirsp);
		m_leftdirsp->addParts1(*m_leftdirlabel);
		m_leftdirsp->addParts2(*m_leftdirCombo);

		m_dlgWnd->addParts(*m_spacerlabel2);
		m_dlgWnd->addParts(*m_rightfootlabel);
		m_dlgWnd->addParts(*m_rightfootbonesp);
		m_rightfootbonesp->addParts1(*m_rightfootBonelabel);
		m_rightfootbonesp->addParts2(*m_rightfootBoneCombo);

		m_dlgWnd->addParts(*m_rightoffsetspY1);
		m_rightoffsetspY1->addParts1(*m_rightoffsetLabelY1);
		m_rightoffsetspY1->addParts2(*m_rightoffsetEditY1);
		m_dlgWnd->addParts(*m_rightoffsetspZ1);
		m_rightoffsetspZ1->addParts1(*m_rightoffsetLabelZ1);
		m_rightoffsetspZ1->addParts2(*m_rightoffsetEditZ1);
		
		m_dlgWnd->addParts(*m_rightoffsetspY2);
		m_rightoffsetspY2->addParts1(*m_rightoffsetLabelY2);
		m_rightoffsetspY2->addParts2(*m_rightoffsetEditY2);
		m_dlgWnd->addParts(*m_rightoffsetspZ2);
		m_rightoffsetspZ2->addParts1(*m_rightoffsetLabelZ2);
		m_rightoffsetspZ2->addParts2(*m_rightoffsetEditZ2);

		m_dlgWnd->addParts(*m_rightrigsp);
		m_rightrigsp->addParts1(*m_rightriglabel);
		m_rightrigsp->addParts2(*m_rightrigCombo);
		m_dlgWnd->addParts(*m_rightdirsp);
		m_rightdirsp->addParts1(*m_rightdirlabel);
		m_rightdirsp->addParts2(*m_rightdirCombo);

		m_dlgWnd->addParts(*m_spacerlabel3);
		m_dlgWnd->addParts(*m_hdiffmaxlabel);
		m_dlgWnd->addParts(*m_hdiffmaxsp);
		m_hdiffmaxsp->addParts2(*m_hdiffmaxEdit);
		m_dlgWnd->addParts(*m_spacerlabel4);
		m_dlgWnd->addParts(*m_rigstepsp);
		m_rigstepsp->addParts1(*m_rigsteplabel);
		m_rigstepsp->addParts2(*m_rigstepEdit);
		m_dlgWnd->addParts(*m_maxcountsp);
		m_maxcountsp->addParts1(*m_maxcountlabel);
		m_maxcountsp->addParts2(*m_maxcountEdit);
		m_dlgWnd->addParts(*m_hopyperstepsp);
		m_hopyperstepsp->addParts1(*m_hopypersteplabel);
		m_hopyperstepsp->addParts2(*m_hopyperstepEdit);
		m_dlgWnd->addParts(*m_wmblendsp);
		m_wmblendsp->addParts1(*m_wmblendlabel);
		m_wmblendsp->addParts2(*m_wmblendSlider);
		m_dlgWnd->addParts(*m_spacerlabel5);

		m_dlgWnd->addParts(*m_leftinfolabel);
		m_dlgWnd->addParts(*m_rightinfolabel);

		m_dlgWnd->addParts(*m_spacerlabel6);
		//m_dlgWnd->addParts(*m_applyB);

		//int slotindex;
		//for (slotindex = 0; slotindex < 8; slotindex++) {
		//	char strslot[128] = { 0 };
		//	sprintf_s(strslot, 128, "Slot_%d", slotindex);
		//	m_lightsslotCombo->addString(strslot);
		//}
		//m_lightsslotCombo->setSelectedCombo(g_lightSlot);

		//#######
		//Button
		//#######
		m_gpuChk->setButtonListener([=, this]() {
			bool value = m_gpuChk->getValue();
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.gpucollision = value;
					CModel* groundmodel = itrelem->second.groundmodel;
					if (groundmodel && m_chascene) {
						int gindex = m_chascene->FindModelIndex(groundmodel);
						if (gindex >= 0) {
							int result = groundmodel->SetGPUInteraction(value);
							if (result != 0) {
								_ASSERT(0);
							}
						}
					}
				}
			}
		});

		//############
		//ComboBox
		//############
		m_groundCombo->setButtonListener([=, this]() {
			int comboid = m_groundCombo->trackPopUpMenu();
			if ((comboid >= 1) && m_chascene && m_model) {
				MODELELEM gmodelelem = m_chascene->GetModelElem(comboid - 1);
				if (gmodelelem.modelptr) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.groundmodel = gmodelelem.modelptr;
					}
				}
			}

			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});

		m_leftfootBoneCombo->setButtonListener([=, this]() {
			int comboid = m_leftfootBoneCombo->trackPopUpMenu();
			if ((comboid >= 1) && m_model) {
				string curcombo = m_leftfootBoneCombo->getSelectedComboStr();
				CBone* curbone = m_model->GetBoneByName(curcombo);
				if (curbone) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.leftfootbone = curbone;
					}
				}
				else {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.leftfootbone = nullptr;
					}
				}
				ParamsToDlg_LeftRig();//jointが変わったので　rigのComboをセットし直すために呼ぶ
			}

			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});
		m_leftrigCombo->setButtonListener([=, this]() {
			int comboid = m_leftrigCombo->trackPopUpMenu();
			if ((comboid >= 1) && m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					CBone* curbone = itrelem->second.leftfootbone;
					if (curbone) {
						string curcombo = m_leftrigCombo->getSelectedComboStr();
						CUSTOMRIG setrig = curbone->GetCustomRig(curcombo);
						itrelem->second.leftrig = setrig;
					}
					else {
						itrelem->second.leftrig.Init();
					}
				}
			}

			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});
		m_leftdirCombo->setButtonListener([=, this]() {
			int comboid = m_leftdirCombo->trackPopUpMenu();
			if ((comboid >= 0) && m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.leftdir = comboid;
				}
			}

			//g_lightSlot = comboid;
			//ParamsToDlg();
			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});
		

		m_rightfootBoneCombo->setButtonListener([=, this]() {
			int comboid = m_rightfootBoneCombo->trackPopUpMenu();
			if ((comboid >= 1) && m_model) {
				string curcombo = m_rightfootBoneCombo->getSelectedComboStr();
				CBone* curbone = m_model->GetBoneByName(curcombo);
				if (curbone) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rightfootbone = curbone;
					}
				}
				else {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rightfootbone = nullptr;
					}
				}
				ParamsToDlg_RightRig();//jointが変わったので　rigのComboをセットし直すために呼ぶ
			}

			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});
		m_rightrigCombo->setButtonListener([=, this]() {
			int comboid = m_rightrigCombo->trackPopUpMenu();
			if ((comboid >= 1) && m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					CBone* curbone = itrelem->second.rightfootbone;
					if (curbone) {
						string curcombo = m_rightrigCombo->getSelectedComboStr();
						CUSTOMRIG setrig = curbone->GetCustomRig(curcombo);
						itrelem->second.rightrig = setrig;
					}
					else {
						itrelem->second.rightrig.Init();
					}
				}
			}

			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});
		m_rightdirCombo->setButtonListener([=, this]() {
			int comboid = m_rightdirCombo->trackPopUpMenu();
			if ((comboid >= 0) && m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.rightdir = comboid;
				}
			}

			//g_lightSlot = comboid;
			//ParamsToDlg();
			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}
			});

		//###########
		//CheckBox
		//###########
		m_enableChk->setButtonListener([=, this]() {
			bool value = m_enableChk->getValue();
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.enablefootrig = value;
				}
			}
			});

		//m_applyB->setButtonListener([=, this]() {
		//	Dlg2Params();

		//	////SetLightDirection();
		//	//if (g_mainhwnd && IsWindow(g_mainhwnd)) {
		//	//	PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
		//	//}
		//	});

		//#######
		//Slider
		//#######
		m_wmblendSlider->setCursorListener([=, this]() {
			float value = (float)m_wmblendSlider->getValue();
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.wmblend = value;
				}
			}
			});

		//########
		//EditBox
		//########
		Dlg2ParamsListener();

		
		
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

int CFootRigDlg::ParamsToDlg()
{

	if ((m_dlgWnd != 0) && m_model && m_chascene) {

		//if ((g_lightSlot < 0) || (g_lightSlot >= LIGHTSLOTNUM)) {
		//	_ASSERT(0);
		//	return 1;
		//}

		WCHAR modelname[MAX_PATH] = { 0L };
		swprintf_s(modelname, MAX_PATH, L"ModelName : %s", m_model->GetFileName());
		if (m_modellabel) {
			m_modellabel->setName(modelname);
		}

		FOOTRIGELEM curfootrigelem;
		curfootrigelem.Init();
		std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
		itrelem = m_footrigelem.find(m_model);
		if (itrelem != m_footrigelem.end()) {
			curfootrigelem = itrelem->second;
		}
		else {
			curfootrigelem.Init();
		}

		if (m_enableChk) {
			m_enableChk->setValue(curfootrigelem.enablefootrig, false);
		}

		if (m_groundCombo) {
			m_groundCombo->ResetCombo();
			m_groundCombo->addString("   ");//先頭項目は未設定

			int findselected = -1;
			int modelnum = m_chascene->GetModelNum();
			int modelindex;
			for (modelindex = 0; modelindex < modelnum; modelindex++) {
				MODELELEM curmodelelem = m_chascene->GetModelElem(modelindex);
				if (curmodelelem.modelptr) {
					WCHAR gname[MAX_PATH] = { 0L };
					wcscpy_s(gname, MAX_PATH, curmodelelem.modelptr->GetFileName());
					char mbgname[MAX_PATH] = { 0 };
					WideCharToMultiByte(CP_ACP, 0, gname, -1, mbgname, MAX_PATH, NULL, NULL);
					m_groundCombo->addString(mbgname);

					if (curfootrigelem.groundmodel && (curfootrigelem.groundmodel == curmodelelem.modelptr)) {
						findselected = modelindex;
					}
				}
				else {
					m_groundCombo->addString("invalid name");
				}
			}

			if (findselected >= 0) {
				m_groundCombo->setSelectedCombo(findselected + 1);
			}
		}

		if (m_gpuChk) {
			bool value = curfootrigelem.gpucollision;
			m_gpuChk->setValue(value, false);
		}


		if (m_leftfootBoneCombo) {
			m_leftfootBoneCombo->ResetCombo();
			m_leftfootBoneCombo->addString("   ");//先頭項目は未設定
			if (m_model) {
				int findselectedbone = -1;
				int bonenum = m_model->GetBoneListSize();
				int boneindex;
				int addindex = 0;
				for (boneindex = 0; boneindex < bonenum; boneindex++) {
					CBone* curbone = m_model->GetBoneByZeroBaseIndex(boneindex);
					if (curbone && curbone->IsSkeleton()) {
						m_leftfootBoneCombo->addString(curbone->GetBoneName());
						if (curfootrigelem.leftfootbone == curbone) {
							findselectedbone = addindex;
						}
						addindex++;
					}
				}
				if (findselectedbone >= 0) {
					m_leftfootBoneCombo->setSelectedCombo(findselectedbone + 1);
				}
			}
		}

		if (m_leftoffsetEditY1) {
			WCHAR wleftoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wleftoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.leftoffsetY1);
			m_leftoffsetEditY1->setName(wleftoffset);
		}
		if (m_leftoffsetEditZ1) {
			WCHAR wleftoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wleftoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.leftoffsetZ1);
			m_leftoffsetEditZ1->setName(wleftoffset);
		}
		if (m_leftoffsetEditY2) {
			WCHAR wleftoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wleftoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.leftoffsetY2);
			m_leftoffsetEditY2->setName(wleftoffset);
		}
		if (m_leftoffsetEditZ2) {
			WCHAR wleftoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wleftoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.leftoffsetZ2);
			m_leftoffsetEditZ2->setName(wleftoffset);
		}

		ParamsToDlg_LeftRig();
		
		if (m_leftdirCombo) {
			m_leftdirCombo->ResetCombo();
			m_leftdirCombo->addString("Row");
			m_leftdirCombo->addString("Col");
			m_leftdirCombo->setSelectedCombo(curfootrigelem.leftdir);
		}



		if (m_rightfootBoneCombo) {
			m_rightfootBoneCombo->ResetCombo();
			m_rightfootBoneCombo->addString("   ");//先頭項目は未設定
			if (m_model) {
				int findselectedbone = -1;
				int bonenum = m_model->GetBoneListSize();
				int boneindex;
				int addindex = 0;
				for (boneindex = 0; boneindex < bonenum; boneindex++) {
					CBone* curbone = m_model->GetBoneByZeroBaseIndex(boneindex);
					if (curbone && curbone->IsSkeleton()) {
						m_rightfootBoneCombo->addString(curbone->GetBoneName());
						if (curfootrigelem.rightfootbone == curbone) {
							findselectedbone = addindex;
						}
						addindex++;
					}
				}
				if (findselectedbone >= 0) {
					m_rightfootBoneCombo->setSelectedCombo(findselectedbone + 1);
				}
			}
		}

		if (m_rightoffsetEditY1) {
			WCHAR wrightoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wrightoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.rightoffsetY1);
			m_rightoffsetEditY1->setName(wrightoffset);
		}
		if (m_rightoffsetEditZ1) {
			WCHAR wrightoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wrightoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.rightoffsetZ1);
			m_rightoffsetEditZ1->setName(wrightoffset);
		}
		if (m_rightoffsetEditY2) {
			WCHAR wrightoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wrightoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.rightoffsetY2);
			m_rightoffsetEditY2->setName(wrightoffset);
		}
		if (m_rightoffsetEditZ2) {
			WCHAR wrightoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wrightoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.rightoffsetZ2);
			m_rightoffsetEditZ2->setName(wrightoffset);
		}

		ParamsToDlg_RightRig();
		
		if (m_rightdirCombo) {
			m_rightdirCombo->ResetCombo();
			m_rightdirCombo->addString("Row");
			m_rightdirCombo->addString("Col");
			m_rightdirCombo->setSelectedCombo(curfootrigelem.rightdir);
		}


		if (m_hdiffmaxEdit) {
			WCHAR strhdiffmax[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(strhdiffmax, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.hdiffmax);
			m_hdiffmaxEdit->setName(strhdiffmax);
		}
		if (m_rigstepEdit) {
			WCHAR strrigstep[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(strrigstep, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.rigstep);
			m_rigstepEdit->setName(strrigstep);
		}
		if (m_maxcountEdit) {
			WCHAR strmaxcount[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(strmaxcount, EDIT_BUFLEN_NUM, L"%d", curfootrigelem.maxcalccount);
			m_maxcountEdit->setName(strmaxcount);
		}
		if (m_hopyperstepEdit) {
			WCHAR strhopyperstep[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(strhopyperstep, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.hopyperstep);
			m_hopyperstepEdit->setName(strhopyperstep);
		}

		if (m_wmblendSlider) {
			m_wmblendSlider->setValue(curfootrigelem.wmblend, false);
		}


		m_dlgWnd->callRewrite();
	}
	return 0;

}

int CFootRigDlg::ParamsToDlg_LeftRig()
{
	FOOTRIGELEM curfootrigelem;
	curfootrigelem.Init();
	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	itrelem = m_footrigelem.find(m_model);
	if (itrelem != m_footrigelem.end()) {
		curfootrigelem = itrelem->second;
	}
	else {
		curfootrigelem.Init();
	}

	if (m_leftrigCombo) {
		m_leftrigCombo->ResetCombo();
		m_leftrigCombo->addString("   ");//先頭項目は未設定
		if (curfootrigelem.leftfootbone) {
			int findselectedrig = -1;
			int rigno;
			int addindex = 0;
			for (rigno = 0; rigno < MAXRIGNUM; rigno++) {
				CUSTOMRIG currig = curfootrigelem.leftfootbone->GetCustomRig(rigno);
				if ((currig.useflag == 2) && (currig.rigno >= 0)) {
					WCHAR wrigname[MAX_PATH] = { 0L };
					wcscpy_s(wrigname, MAX_PATH, currig.rigname);
					char mbrigname[MAX_PATH] = { 0 };
					WideCharToMultiByte(CP_ACP, 0, wrigname, -1, mbrigname, MAX_PATH, NULL, NULL);
					m_leftrigCombo->addString(mbrigname);

					if (curfootrigelem.leftrig.rigno == currig.rigno) {
					//if (wcscmp(wrigname, curfootrigelem.leftrig.rigname) == 0) {
						findselectedrig = addindex;
					}
					addindex++;
				}
			}

			if (findselectedrig >= 0) {
				m_leftrigCombo->setSelectedCombo(findselectedrig + 1);
			}
		}
	}
	return 0;
}
int CFootRigDlg::ParamsToDlg_RightRig()
{
	FOOTRIGELEM curfootrigelem;
	curfootrigelem.Init();
	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	itrelem = m_footrigelem.find(m_model);
	if (itrelem != m_footrigelem.end()) {
		curfootrigelem = itrelem->second;
	}
	else {
		curfootrigelem.Init();
	}

	if (m_rightrigCombo) {
		m_rightrigCombo->ResetCombo();
		m_rightrigCombo->addString("   ");//先頭項目は未設定
		if (curfootrigelem.rightfootbone) {
			int findselectedrig = -1;
			int rigno;
			int addindex = 0;
			for (rigno = 0; rigno < MAXRIGNUM; rigno++) {
				CUSTOMRIG currig = curfootrigelem.rightfootbone->GetCustomRig(rigno);
				if ((currig.useflag == 2) && (currig.rigno >= 0)) {
					WCHAR wrigname[MAX_PATH] = { 0L };
					wcscpy_s(wrigname, MAX_PATH, currig.rigname);
					char mbrigname[MAX_PATH] = { 0 };
					WideCharToMultiByte(CP_ACP, 0, wrigname, -1, mbrigname, MAX_PATH, NULL, NULL);
					m_rightrigCombo->addString(mbrigname);

					if (curfootrigelem.rightrig.rigno == currig.rigno) {
					//if (wcscmp(wrigname, curfootrigelem.rightrig.rigname) == 0) {
						findselectedrig = addindex;
					}
					addindex++;
				}
			}

			if (findselectedrig >= 0) {
				m_rightrigCombo->setSelectedCombo(findselectedrig + 1);
			}
		}
	}
	return 0;
}


int CFootRigDlg::Dlg2ParamsListener()
{
	if (m_leftoffsetEditY1) {
		m_leftoffsetEditY1->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_leftoffsetEditY1->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.leftoffsetY1 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.leftoffsetY1);
						m_leftoffsetEditY1->setName(strnotchange);
					}
				}
			}
		});
	}
	if (m_leftoffsetEditZ1) {
		m_leftoffsetEditZ1->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_leftoffsetEditZ1->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.leftoffsetZ1 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.leftoffsetZ1);
						m_leftoffsetEditZ1->setName(strnotchange);
					}
				}
			}
		});
	}
	if (m_rightoffsetEditY1) {
		m_rightoffsetEditY1->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_rightoffsetEditY1->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rightoffsetY1 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.rightoffsetY1);
						m_rightoffsetEditY1->setName(strnotchange);
					}
				}
			}
		});
	}
	if (m_rightoffsetEditZ1) {
		m_rightoffsetEditZ1->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_rightoffsetEditZ1->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rightoffsetZ1 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.rightoffsetZ1);
						m_rightoffsetEditZ1->setName(strnotchange);
					}
				}
			}
		});
	}


	if (m_leftoffsetEditY2) {
		m_leftoffsetEditY2->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_leftoffsetEditY2->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.leftoffsetY2 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.leftoffsetY2);
						m_leftoffsetEditY2->setName(strnotchange);
					}
				}
			}
		});
	}
	if (m_leftoffsetEditZ2) {
		m_leftoffsetEditZ2->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_leftoffsetEditZ2->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.leftoffsetZ2 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.leftoffsetZ2);
						m_leftoffsetEditZ2->setName(strnotchange);
					}
				}
			}
		});
	}
	if (m_rightoffsetEditY2) {
		m_rightoffsetEditY2->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_rightoffsetEditY2->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rightoffsetY2 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.rightoffsetY2);
						m_rightoffsetEditY2->setName(strnotchange);
					}
				}
			}
		});
	}
	if (m_rightoffsetEditZ2) {
		m_rightoffsetEditZ2->setExitDialogListener([=, this]() {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_rightoffsetEditZ2->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if ((offsetval >= -5000.0f) && (offsetval <= 5000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rightoffsetZ2 = offsetval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.rightoffsetZ2);
						m_rightoffsetEditZ2->setName(strnotchange);
					}
				}
			}
		});
	}


	if (m_hdiffmaxEdit) {
		m_hdiffmaxEdit->setExitDialogListener([=, this]() {
			WCHAR strdiff[EDIT_BUFLEN_NUM] = { 0L };
			m_hdiffmaxEdit->getName(strdiff, EDIT_BUFLEN_NUM);
			float diffval = (float)_wtof(strdiff);
			if ((diffval >= -50000.0f) && (diffval <= 50000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.hdiffmax = diffval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.hdiffmax);
						m_hdiffmaxEdit->setName(strnotchange);
					}
				}
			}
		});
	}

	if (m_rigstepEdit) {
		m_rigstepEdit->setExitDialogListener([=, this]() {
			WCHAR strstep[EDIT_BUFLEN_NUM] = { 0L };
			m_rigstepEdit->getName(strstep, EDIT_BUFLEN_NUM);
			float stepval = (float)_wtof(strstep);
			if ((stepval >= 0.0f) && (stepval <= 400.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.rigstep = stepval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.rigstep);
						m_rigstepEdit->setName(strnotchange);
					}
				}
			}
		});
	}

	if (m_maxcountEdit) {
		m_maxcountEdit->setExitDialogListener([=, this]() {
			WCHAR strmaxcount[EDIT_BUFLEN_NUM] = { 0L };
			m_maxcountEdit->getName(strmaxcount, EDIT_BUFLEN_NUM);
			int countval = _wtoi(strmaxcount);
			if ((countval >= 0) && (countval <= 10000)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.maxcalccount = countval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%d", itrelem->second.maxcalccount);
						m_maxcountEdit->setName(strnotchange);
					}
				}
			}
		});
	}

	if (m_hopyperstepEdit) {
		m_hopyperstepEdit->setExitDialogListener([=, this]() {
			WCHAR strstep[EDIT_BUFLEN_NUM] = { 0L };
			m_hopyperstepEdit->getName(strstep, EDIT_BUFLEN_NUM);
			float stepval = (float)_wtof(strstep);
			if ((stepval >= -50000.0f) && (stepval <= 50000.0f)) {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						itrelem->second.hopyperstep = stepval;
					}
				}
			}
			else {
				if (m_model) {
					std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
					itrelem = m_footrigelem.find(m_model);
					if (itrelem != m_footrigelem.end()) {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", itrelem->second.hopyperstep);
						m_hopyperstepEdit->setName(strnotchange);
					}
				}
			}
		});
	}

	return 0;
}




const HWND CFootRigDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CFootRigDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CFootRigDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

int CFootRigDlg::SaveFootRigFile(WCHAR* srcprojectdir, WCHAR* srcprojectname, ChaScene* srcchascene)
{
	if (!srcprojectdir || !srcprojectname || !srcchascene) {
		_ASSERT(0);
		return 1;
	}

	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	for (itrelem = m_footrigelem.begin(); itrelem != m_footrigelem.end(); itrelem++) {
		CModel* curmodel = itrelem->first;
		FOOTRIGELEM curelem = itrelem->second;

		//curmodelが削除済でないことを確認する
		int chkmodelindex = srcchascene->FindModelIndex(curmodel);
		
		if (curmodel && (chkmodelindex >= 0)) {//curmodelが削除済でないことを確認する
			WCHAR friname[MAX_PATH] = { 0L };
			swprintf_s(friname, MAX_PATH, L"%s\\%s\\%s.fri", 
				srcprojectdir, srcprojectname, curmodel->GetFileName());
			CFootRigFile footrigfile;
			int result = footrigfile.WriteFootRigFile(friname, itrelem->second);
			if (result != 0) {
				_ASSERT(0);
			}		
		}
	}

	return 0;
}
int CFootRigDlg::LoadFootRigFile(WCHAR* savechadir, WCHAR* saveprojname)
{
	if (!m_chascene || !savechadir || !saveprojname) {
		_ASSERT(0);
		return 1;
	}

	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	for (itrelem = m_footrigelem.begin(); itrelem != m_footrigelem.end(); itrelem++) {
		CModel* curmodel = itrelem->first;
		if (curmodel) {
			FOOTRIGELEM tmpelem;
			tmpelem.Init();

			WCHAR friname[MAX_PATH] = { 0L };
			swprintf_s(friname, MAX_PATH, L"%s\\%s\\%s.fri", 
				savechadir, saveprojname, curmodel->GetFileName());
			CFootRigFile footrigfile;
			int result = footrigfile.LoadFootRigFile(friname, curmodel, m_chascene, &tmpelem);
			if (result == 0) {
				itrelem->second = tmpelem;
			}
			else {
				//_ASSERT(0);
				int dbgflag1 = 1;
			}
		}
	}

	return 0;
}

int CFootRigDlg::Update(bool limitdegflag)
{
	if (g_underRetargetFlag) {
		//2025/02/09 Retarget中はFootRigしない
		return 0;
	}


	//###########################################################################
	//2024/09/13 LimitEul角度制限のオンオフに対応　制限オンの時には強制的に壁すりIKもオン
	//しかし、limiteulオフで計算した方が綺麗に動く
	//limiteulオンで足を上げると足を戻す動作の変化率が大きくなり(速くなり)綺麗にはみえなかった
	//Rigの動作範囲は確認可能であり、FootRigの設定で制限した方が綺麗に動くのでそうすることに
	//よって、FootRigに関しては、Update関数内でlimitdegflag = falseに上書きして使用する
	//###########################################################################
	limitdegflag = false;

	int result = 0;
	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	for (itrelem = m_footrigelem.begin(); itrelem != m_footrigelem.end(); itrelem++) {
		CModel* curmodel = itrelem->first;
		if (curmodel) {
			result += Update(limitdegflag, curmodel);
			if (result != 0) {
				_ASSERT(0);
			}
		}
	}

	return result;
}


int CFootRigDlg::Update(bool limitdegflag, CModel* srcmodel)
{
	if (g_underRetargetFlag) {
		//2025/02/09 Retarget中はFootRigしない
		return 0;
	}

	//#####################################################################################
	//2024/09/13 LimitEul角度制限のオンオフに対応　制限オンの時には強制的に壁すりIKもオン
	//しかし、limiteulオフで計算した方が綺麗に動く
	//limiteulオンで足を上げると足を戻す動作の変化率が大きくなり(速くなり)綺麗にはみえなかった
	//Rigの動作範囲は確認可能であり、FootRigの設定で制限した方が綺麗に動くのでそうすることに
	//角度制限に対応したが、FootRigに関してはUpdate関数内でlimitdegflag = falseに上書きして使用する
	//#####################################################################################
	limitdegflag = false;


	if (!srcmodel) {
		return 0;
	}

	if (!IsValidModel(srcmodel)) {
		//srcmodelは削除済
		return 0;
	}

	if (!srcmodel->GetLoadedFlag())
	{
		return 0;
	}

	if (!s_leftfootinfo || !s_rightfootinfo) {
		_ASSERT(0);
		return 0;
	}


	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	itrelem = m_footrigelem.find(srcmodel);
	if (itrelem != m_footrigelem.end()) {

		FOOTRIGELEM curelem = itrelem->second;
		if (curelem.IsEnable()) {


			CModel* chkgroundmodel = curelem.groundmodel;
			if (!IsValidModel(chkgroundmodel)) {
				return 0;//groundmodelは削除済
			}

			s_leftfootinfo->SetFootInfo(FOOTRIG_LR_LEFT, curelem);
			s_rightfootinfo->SetFootInfo(FOOTRIG_LR_RIGHT, curelem);

			if (curelem.leftfootbone) {
				s_leftfootinfo->CalcPos(limitdegflag);
			}
			if (curelem.rightfootbone) {
				s_rightfootinfo->CalcPos(limitdegflag);
			}

			{
				CFootInfo* lowerfootinfo = nullptr;
				CFootInfo* higherfootinfo = nullptr;
				//if (leftjointpos.y <= rightjointpos.y) {
				if (s_leftfootinfo->IsHigherGPos(s_rightfootinfo)) {
					lowerfootinfo = s_rightfootinfo;
					higherfootinfo = s_leftfootinfo;
				}
				else {
					lowerfootinfo = s_leftfootinfo;
					higherfootinfo = s_rightfootinfo;
				}
				FootRig(false,
					limitdegflag, srcmodel,
					curelem,
					lowerfootinfo, higherfootinfo);

			}


			//if ((srcmodel == m_model) && GetVisible()) {
			//	if (m_leftinfolabel) {
			//		ChaVector3 leftoffset;
			//		leftoffset.SetParams(0.0f, curelem.leftoffsetY, curelem.leftoffsetZ);
			//		ChaVector3 newleftjointpos = GetJointPos(limitdegflag, srcmodel, curelem.leftfootbone, leftoffset);

			//		WCHAR strlabel[MAX_PATH] = { 0L };
			//		swprintf_s(strlabel, MAX_PATH, L"LeftFoot(%.2f, %.2f), LeftGround %.2f",
			//			leftjointpos.y, newleftjointpos.y, leftgpos.y);
			//		m_leftinfolabel->setName(strlabel);
			//	}
			//	if (m_rightinfolabel) {
			//		ChaVector3 rightoffset;
			//		rightoffset.SetParams(0.0f, curelem.rightoffsetY, curelem.rightoffsetZ);
			//		ChaVector3 newrightjointpos = GetJointPos(limitdegflag, srcmodel, curelem.rightfootbone, rightoffset);

			//		WCHAR strlabel[MAX_PATH] = { 0L };
			//		swprintf_s(strlabel, MAX_PATH, L"RightFoot(%.2f, %.2f), RightGround %.2f",
			//			rightjointpos.y, newrightjointpos.y, rightgpos.y);
			//		m_rightinfolabel->setName(strlabel);
			//	}
			//}
		}
		else {

			if ((srcmodel == m_model) && GetVisible()) {
				//設定不足のためFootRigを機能させない

				if (m_leftinfolabel) {
					WCHAR strlabel[MAX_PATH] = { 0L };
					swprintf_s(strlabel, MAX_PATH, L"LeftFoot(***.**, ***.**), LeftGround ***.**");
					m_leftinfolabel->setName(strlabel);
				}
				if (m_rightinfolabel) {
					WCHAR strlabel[MAX_PATH] = { 0L };
					swprintf_s(strlabel, MAX_PATH, L"RightFoot(***.**, ***.**), RightGround ***.**");
					m_rightinfolabel->setName(strlabel);
				}
			}
		}
	}

	return 0;
}


void CFootRigDlg::FootRig(bool secondcalling,
	bool limitdegflag, CModel* srcmodel,
	FOOTRIGELEM curelem,
	CFootInfo* lowerfootinfo, CFootInfo* higherfootinfo
)
{
	if (g_underRetargetFlag) {
		//2025/02/09 Retarget中はFootRigしない
		return;
	}


	//##############################################################################################
	//2024/09/30
	//#### 注意 ####
	//詳細な地面データに対しては、ローポリゴンの当たり判定用の地面データを用意して
	//それを非表示にしつつFootRig用の地面として選択する方法が良い
	//(ローポリゴンがバレないようにキーになる地形で当たり判定用地面のポリゴンを多くするのも良いかもしれない)
	//
	//#### 理由 ####
	//ディテールがあるようなレベルの地面データ(例えば階段データ)は
	//リアリティを増すために欠けている部分が存在することがある
	//そのような地面を当たり判定地面として選んでこの関数で歩くと
	//欠けている部分から頻繁に下に落ちて　動きがカクついてしまう
	//
	//落下距離の最大値を決めて試したりしたが、落ちたときには無視できないくらいカクカクした動きになった
	//
	//地面の高さの測定点を複数取る方法(足の裏の大きさ分サンプリングする方法)も考えられた
	//しかし、測定点を２倍にすると２倍重くなり、大きい隙間に対応すると踏んでいないものを踏んだことになることは
	//試さないでも予測できる
	// 
	//##############################################################################################



	if (!srcmodel || !lowerfootinfo || !higherfootinfo) {
		//_ASSERT(0);
		int dbgflag1 = 1;
		return;
	}
	if (!lowerfootinfo->IsValid() || !higherfootinfo->IsValid()) {
		int dbgflag1 = 1;
		return;
	}


	float hdiffoffset;
	if (!secondcalling) {
		hdiffoffset = 0.0f;
	}
	else {
		//どちらかの足が地面の下に潜っていた場合にsecondcallingでもう一度呼ばれる
		//その際には地面の上に足を出すためのRigControlFootRig()呼び出しの確率が上がるように　hdiffmaxを大きくする
		hdiffoffset = 10.0f;
	}
	float hdiffmax = curelem.hdiffmax + hdiffoffset;
	float higherhdiffoffset = 10.0f;

	ChaMatrix modelwm = srcmodel->GetWorldMat();
	ChaMatrix matView = srcmodel->GetViewMat();
	ChaMatrix matProj = srcmodel->GetProjMat();

	int curmotid = srcmodel->GetCurrentMotID();
	double curframe = RoundingTime(srcmodel->GetCurrentFrame());//!!! RoundingTime

	CBone* hipsjoint = nullptr;
	srcmodel->GetHipsBoneReq(srcmodel->GetTopBone(), &hipsjoint);
	if (hipsjoint) {
		ChaVector3 hipsoffset;
		hipsoffset.SetZeroVec3();
		ChaVector3 hipspos = GetJointPos(limitdegflag, srcmodel, hipsjoint, hipsoffset, false);

		bool lowerdoneflag = false;
		bool higherdoneflag = false;
		bool forcehigherfootrig = false;

		bool jumpflag = GetJumpFlag(srcmodel);

		if (jumpflag == false) {//2025/02/16 jump対応

			if (!secondcalling) {//### Hop Y per step ###
				//2024/09/16 上り坂の傾斜を上る際に足が曲がり過ぎないように　毎回ホップする
				float diffy = curelem.hopyperstep;
				ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, 0.0f, false);//wmをブレンドしない　保存しない
				float diffy2 = modelwm3.data[MATI_42] - modelwm.data[MATI_42];
				modelwm = modelwm3;
				hipspos.y += diffy2;
				higherfootinfo->CalcPos(limitdegflag);
				lowerfootinfo->CalcPos(limitdegflag);
			}

			if (//!secondcalling &&
				(hipspos.y - higherfootinfo->GetLowerGPos().y) > (hdiffmax + ROUNDINGPOS)) {

				//float diffy = highergpos.y - (higherjointpos.y + higheroffset);
				//float diffy = -(hipspos.y - higherfootinfo->GetLowerGPos().y - hdiffmax);//2024/09/16 hdiffmax設定で足の曲がり方が調整できるように修正　地面とhipsの高さがhdiffmax以下になるようにShiftする
				float diffy = -(hipspos.y - higherfootinfo->GetLowerGPos().y - hdiffmax) - higherfootinfo->GetLowerFootOffset().y;//2024/10/17
				ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, curelem.wmblend, true);//wmをブレンドする　この処理後に地面に潜っていても　後処理で地面位置まで上げる

				float diffy2 = modelwm3.data[MATI_42] - modelwm.data[MATI_42];
				modelwm = modelwm3;
				hipspos.y += diffy2;
				higherfootinfo->CalcPos(limitdegflag);
				lowerfootinfo->CalcPos(limitdegflag);

				//higherdoneflag = true;
			}

			if (//!secondcalling &&
				(hipspos.y - lowerfootinfo->GetLowerGPos().y) > (hdiffmax + ROUNDINGPOS)) {

				//float diffy = lowergpos.y - (lowerjointpos.y + loweroffset);
				//float diffy = -(hipspos.y - lowerfootinfo->GetLowerGPos().y - hdiffmax);//2024/09/16 hdiffmax設定で足の曲がり方が調整できるように修正　地面とhipsの高さがhdiffmax以下になるようにShiftする
				float diffy = -(hipspos.y - lowerfootinfo->GetLowerGPos().y - hdiffmax) - lowerfootinfo->GetLowerFootOffset().y;//2024/10/17
				ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, curelem.wmblend, true);//wmをブレンドする　この処理後に地面に潜っていても　後処理で地面位置まで上げる

				float diffy2 = modelwm3.data[MATI_42] - modelwm.data[MATI_42];
				modelwm = modelwm3;
				hipspos.y += diffy2;
				higherfootinfo->CalcPos(limitdegflag);
				lowerfootinfo->CalcPos(limitdegflag);

				//lowerdoneflag = true;
				//if (higherdoneflag) {
				//	//低い方の地面に接地した場合には　強制的に高い方の足を曲げる処理をする
				//	forcehigherfootrig = true;
				//}
			}

			if (!lowerdoneflag &&
				//((hipspos.y - lowerfootinfo->GetHigherGPos().y) <= hdiffmax) &&
				!lowerfootinfo->IsHigherFootThanGround(ROUNDINGPOS)
				) {

				//低い方の足をFootRigで曲げて接地
				lowerfootinfo->RigControlFootRig(limitdegflag, srcmodel, curframe,
					curelem.rigstep, curelem.maxcalccount,
					modelwm, matView, matProj);

				lowerdoneflag = true;
			}

			if (forcehigherfootrig ||
				(
					!higherdoneflag &&
					//((hipspos.y - higherfootinfo->GetHigherGPos().y) <= (hdiffmax + higherhdiffoffset)) &&
					!higherfootinfo->IsHigherFootThanGround(0.0f)
					)
				) {

				//高い方の足をFootRigで曲げて接地
				higherfootinfo->RigControlFootRig(limitdegflag, srcmodel, curframe,
					curelem.rigstep, curelem.maxcalccount,
					modelwm, matView, matProj);

				higherdoneflag = true;
			}
		}

		//#####################################
		//check and secondcalling. jump中も必要
		//#####################################
		if (!lowerfootinfo->IsHigherFootThanGround(ROUNDINGPOS) ||
			!higherfootinfo->IsHigherFootThanGround(ROUNDINGPOS)) {

			//if (!secondcalling) {
			//	//足が潜っていた場合　２回目の呼び出しをする
			//	if (higherfootinfo->IsHigherGPos(lowerfootinfo)) {
			//		FootRig(true,//secondcalling !!!!
			//			limitdegflag, srcmodel,
			//			curelem,
			//			lowerfootinfo, higherfootinfo);
			//	}
			//	else {
			//		//高低入れ替えて呼び出し
			//		FootRig(true,//secondcalling !!!!
			//			limitdegflag, srcmodel,
			//			curelem,
			//			higherfootinfo, lowerfootinfo);
			//	}
			//}
			//else {
				//２回の実行でも足が地面に潜っている場合
				//高い方の地面の位置に合わせる
				if (higherfootinfo->IsHigherGPos(lowerfootinfo)) {
					float diffy = higherfootinfo->GetDiffBetweenHigherGPosAndFoot();
					//ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, false);//wmをブレンドしない　この処理後に地面に潜らないように. ブレンド無しは階段でガクガクし過ぎる
					ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, curelem.wmblend, true);//遅めの環境でもカクカクしないために　やっぱりブレンドフラグtrueに
					modelwm = modelwm3;

				}
				else {
					float diffy = lowerfootinfo->GetDiffBetweenHigherGPosAndFoot();
					//ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, false);//wmをブレンドしない　この処理後に地面に潜らないように. ブレンド無しは階段でガクガクし過ぎる
					ChaMatrix modelwm3 = ModelShiftY(srcmodel, modelwm, diffy, curelem.wmblend, true);//遅めの環境でもカクカクしないために　やっぱりブレンドフラグtrueに
					modelwm = modelwm3;
				}

				higherfootinfo->CalcPos(limitdegflag);
				lowerfootinfo->CalcPos(limitdegflag);

			//}
		}

	}

}


void CFootRigDlg::SetSaveModelWM(CModel* srcmodel, ChaMatrix srcmat)
{
	if (srcmodel) {
		std::map<CModel*, ChaMatrix>::iterator itrsavewm;
		itrsavewm = m_savemodelwm.find(srcmodel);
		if (itrsavewm != m_savemodelwm.end()) {
			itrsavewm->second = srcmat;
		}
		else {
			m_savemodelwm[srcmodel] = srcmat;
		}
	}
	else {
		_ASSERT(0);
	}

}

ChaMatrix CFootRigDlg::GetSaveModelWM(CModel* srcmodel)
{
	if (srcmodel) {
		std::map<CModel*, ChaMatrix>::iterator itrsavewm;
		itrsavewm = m_savemodelwm.find(srcmodel);
		if (itrsavewm != m_savemodelwm.end()) {
			//１回前のFootRig計算によるmodelworldmatの変更を元に戻す
			ChaMatrix savewm = itrsavewm->second;
			return savewm;
		}
		else {
			ChaMatrix curwm = srcmodel->GetWorldMat();
			return curwm;
		}
	}
	else {
		_ASSERT(0);
		ChaMatrix iniwm;
		iniwm.SetIdentity();
		return iniwm;
	}
}

bool CFootRigDlg::IsEnableFootRig(CModel* srcmodel)
{
	if (!IsValidModel(srcmodel)) {
		//srcmodelは削除済
		return false;
	}

	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	itrelem = m_footrigelem.find(m_model);
	if (itrelem != m_footrigelem.end()) {

		FOOTRIGELEM curelem = itrelem->second;
		if (curelem.IsEnable()) {
			CModel* chkground = curelem.groundmodel;
			if (!IsValidModel(chkground)) {
				//groundmodelは削除済
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

ChaMatrix CFootRigDlg::BlendSaveModelWM(CModel* srcmodel, ChaMatrix srcmat, float blendrate)
{
	if (srcmodel) {
		ChaMatrix savemat = GetSaveModelWM(srcmodel);

		//ChaMatrix blendmat = srcmat * blendrate + savemat * (1.0f - blendrate);

		//2024/10/07 blendrateをGUIから指定可能にし0.0をブレンド無しとした
		ChaMatrix blendmat = srcmat * (1.0f - blendrate) + savemat * blendrate;
		return blendmat;
	}
	else {
		_ASSERT(0);
		return srcmat;
	}
}
ChaMatrix CFootRigDlg::GetJointWM(bool limitdegflag, CModel* srcmodel, CBone* srcbone, bool multmodelwm)
{
	bool calcslotflag = true;
	ChaMatrix retmat;
	retmat.SetIdentity();
	if (!srcmodel || !srcbone) {
		_ASSERT(0);
		return retmat;
	}

	int curmotid = srcmodel->GetCurrentMotID();
	double curframe = RoundingTime(srcmodel->GetCurrentFrame());
	ChaMatrix curwm = srcbone->GetWorldMat(limitdegflag, curmotid, curframe, 0);
	ChaMatrix modelwm = srcmodel->GetWorldMat();
	if (multmodelwm) {
		retmat = curwm * modelwm;
	}
	else {
		retmat = curwm;
	}

	return retmat;
}

ChaVector3 CFootRigDlg::GetJointPos(bool limitdegflag, CModel* srcmodel, CBone* srcbone, ChaVector3 srcoffset, bool istoebase)
{
	bool calcslotflag = true;
	ChaVector3 retpos;
	retpos.SetZeroVec3();
	if (!srcmodel || !srcbone) {
		_ASSERT(0);
		return retpos;
	}


	//ChaVector3 offsetZ;
	//offsetZ.SetParams(0.0f, 0.0f, srcoffset.z);
	//ChaVector3 jointfpos = srcbone->GetJointFPos() + offsetZ;//2024/10/16 +offsetZ : offsetZは回転に依存させる
	ChaVector3 jointfpos = srcbone->GetJointFPos() + srcoffset;//2024/10/17 つま先とカカトの２点で計算するようにした　回転前に足さないとつま先の角度でオフセット効果が違い過ぎるため
	ChaMatrix transmat = GetJointWM(limitdegflag, srcmodel, srcbone, true);
	ChaVector3TransformCoord(&retpos, &jointfpos, &transmat);
	//retpos.y += srcoffset.y;//offsetYは回転に依存させない　：　階段を上る際など、足首回転でつま先が持ち上がっても高さのオフセットはそのままの方が都合が良い

	return retpos;
}

ChaVector3 CFootRigDlg::GetGroundPos(CModel* groundmodel, ChaVector3 basepos, bool gpuflag)
{
	int hitflag = 0;
	ChaVector3 retpos;
	retpos.SetZeroVec3();
	if (!groundmodel) {
		_ASSERT(0);
		return retpos;
	}

	ChaVector3 startglobal = basepos + ChaVector3(0.0f, FOOTRIGPICKHEIGHT, 0.0f);
	ChaVector3 endglobal = basepos - ChaVector3(0.0f, FOOTRIGPICKHEIGHT, 0.0f);

	hitflag = groundmodel->CollisionPolyMesh3_Ray(
		gpuflag,
		startglobal, endglobal, &retpos);

	return retpos;
}

ChaMatrix CFootRigDlg::ModelShiftY(CModel* srcmodel, ChaMatrix befwm, float diffy, 
	float blendrate, bool savewmflag)
{
	//モデルworldmatのブレンド率　このブレンドをしないと上下に小刻みに揺れる
	//float MODELWMBLEND = 0.30f;
	//float MODELWMBLEND = 0.50f;

	ChaMatrix retmat;
	retmat.SetIdentity();
	if (!srcmodel) {
		_ASSERT(0);
		return retmat;
	}

	ChaMatrix modelwm2 = befwm;
	modelwm2.data[MATI_42] = modelwm2.data[MATI_42] + diffy;
	ChaMatrix modelwm3;
	if (blendrate != 0.0f) {
		//フラグしていのときのみブレンドする
		modelwm3 = BlendSaveModelWM(srcmodel, modelwm2, blendrate);//プルプル震えるのを軽減するために１回前とブレンドする
	}
	else {
		modelwm3 = modelwm2;
	}
	srcmodel->UpdateModelWMFootRig(modelwm3);
	retmat = modelwm3;

	if (savewmflag) {
		SetSaveModelWM(srcmodel, retmat);
	}

	return retmat;
}


bool CFootRigDlg::IsValidModel(CModel* srcmodel)
{
	//modelが削除されている場合はfalseを返す
	if (!m_chascene || !srcmodel) {
		return false;
	}

	int modelindex = m_chascene->FindModelIndex(srcmodel);
	if (modelindex >= 0) {
		return true;
	}
	else {
		//srcmodelは削除されていた
		return false;
	}
}

int CFootRigDlg::OnDelModel(CModel* srcmodel)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
	itrelem = m_footrigelem.find(srcmodel);
	if (itrelem != m_footrigelem.end()) {
		m_footrigelem.erase(itrelem);
	}

	std::map<CModel*, ChaMatrix>::iterator itrwm;
	itrwm = m_savemodelwm.find(srcmodel);
	if (itrwm != m_savemodelwm.end()) {
		m_savemodelwm.erase(itrwm);
	}



	return 0;
}
int CFootRigDlg::OnDellAllModel()
{
	m_footrigelem.clear();
	m_savemodelwm.clear();

	return 0;
}

bool CFootRigDlg::GetJumpFlag(CModel* srcmodel)
{
	if (srcmodel) {
		MOTINFO currentmi = srcmodel->GetCurMotInfo();
		if (currentmi.motid > 0) {
			return currentmi.GetJumpFlag();
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

/////////////////////////
/////////////////////////

void CFootInfo::SetFootInfo(int footrigLR, FOOTRIGELEM srcelem) {
	if (footrigLR == FOOTRIG_LR_LEFT) {
		m_toebasejoint = srcelem.leftfootbone;
		if (m_toebasejoint) {
			m_footjoint = m_toebasejoint->GetParent(false);
		}
		m_offset1.SetParams(0.0f, srcelem.leftoffsetY1, srcelem.leftoffsetZ1);
		m_offset2.SetParams(0.0f, srcelem.leftoffsetY2, srcelem.leftoffsetZ2);
		m_rig = srcelem.leftrig;
		m_rigdir = srcelem.leftdir;

		m_rignum = 0;
		m_updatebone = GetUpdateBone(m_toebasejoint->GetParModel(),
			m_toebasejoint, m_rig, m_rigdir, &m_rignum);

	}
	else if (footrigLR == FOOTRIG_LR_RIGHT) {
		m_toebasejoint = srcelem.rightfootbone;
		if (m_toebasejoint) {
			m_footjoint = m_toebasejoint->GetParent(false);
		}
		m_offset1.SetParams(0.0f, srcelem.rightoffsetY1, srcelem.rightoffsetZ1);
		m_offset2.SetParams(0.0f, srcelem.rightoffsetY2, srcelem.rightoffsetZ2);
		m_rig = srcelem.rightrig;
		m_rigdir = srcelem.rightdir;

		m_rignum = 0;
		m_updatebone = GetUpdateBone(m_footjoint->GetParModel(),
			m_footjoint, m_rig, m_rigdir, &m_rignum);
	}
	else {
		_ASSERT(0);
	}
	m_groundmodel = srcelem.groundmodel;
	m_gpucollision = srcelem.gpucollision;
};


int CFootInfo::CalcPos(int limitdegflag)
{
	if (m_footrigdlg && m_toebasejoint && m_footjoint) {
		{
			ChaVector3 jointpos = m_footrigdlg->GetJointPos(limitdegflag, m_toebasejoint->GetParModel(), m_toebasejoint, m_offset1, true);
			m_toebasepos = jointpos;
			if (m_groundmodel) {
				ChaVector3 gpos = m_footrigdlg->GetGroundPos(m_groundmodel, m_toebasepos, m_gpucollision);
				m_toebaseGpos = gpos;
			}
		}

		{
			ChaVector3 jointpos = m_footrigdlg->GetJointPos(limitdegflag, m_footjoint->GetParModel(), m_footjoint, m_offset2, false);
			m_footpos = jointpos;
			if (m_groundmodel) {
				ChaVector3 gpos = m_footrigdlg->GetGroundPos(m_groundmodel, m_footpos, m_gpucollision);
				m_footGpos = gpos;
			}
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

bool CFootInfo::IsHigherGPos(CFootInfo* cmpinfo)
{
	if (cmpinfo) {
		ChaVector3 highergpos = GetHigherGPos();
		ChaVector3 cmpgpos = cmpinfo->GetHigherGPos();
		if (highergpos.y >= cmpgpos.y) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		_ASSERT(0);
		return true;
	}
}

CBone* CFootInfo::GetUpdateBone(CModel* srcmodel, CBone* footbone, CUSTOMRIG footrig, int rigdir, int* prignum)
{
	//########################################
	//Rigで回転するボーンの内の一番親のボーンを返す
	//########################################

	if (!srcmodel || !footbone || !prignum ||
		(footrig.useflag != 2) || ((rigdir) != 0) && (rigdir != 1)) {
		return nullptr;
	}

	*prignum = 0;
	CBone* retbone = nullptr;

	int rignum = 0;
	int elemno;
	for (elemno = 0; elemno < footrig.elemnum; elemno++) {
		//elemnoが１つ増えるとボーンは１階層親になる
		RIGELEM currigelem = footrig.rigelem[elemno];

		if ((currigelem.rigrigboneno < 0) && //FootRigではrigのrigは想定していない.　通常のリグの場合に対応
			(currigelem.boneno >= 0) &&
			(currigelem.transuv[rigdir].enable == 1) &&
			(fabs(currigelem.transuv[rigdir].applyrate) >= 1e-4)) {
			CBone* curbone = srcmodel->GetBoneByID(currigelem.boneno);
			if (curbone) {
				retbone = curbone;//上書き
				rignum++;
			}
		}
	}

	*prignum = rignum;
	return retbone;

	//lowerupdatebone = lowerfoot;
	//int levelcnt = 0;
	//while ((levelcnt < 2) && lowerupdatebone->GetParent(false)) {//!!!!!!!! 注意：2階層上までの決め打ち
	//	lowerupdatebone = lowerupdatebone->GetParent(false);
	//	levelcnt++;
	//}

}

int CFootInfo::RigControlFootRig(bool limitdegflag, CModel* srcmodel, double curframe,
	float rigstep, int maxcalccount,
	ChaMatrix modelwm, ChaMatrix matView, ChaMatrix matProj)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	CalcPos(limitdegflag);

	RigControlFootRigFunc(false, 
		limitdegflag, srcmodel, curframe,
		rigstep, maxcalccount,
		modelwm, matView, matProj);

	return 0;
}

int CFootInfo::RigControlFootRigFunc(bool istoebase,
	bool limitdegflag, CModel* srcmodel, double curframe,
	float rigstep, int maxcalccount,
	ChaMatrix modelwm, ChaMatrix matView, ChaMatrix matProj)
{

	if (!srcmodel || !IsValid()) {
		_ASSERT(0);
		return 1;
	}

	int maxcount = min(100, max(0, maxcalccount));//2024/09/08 計算回数の最大数もGUIから指定するように変更した


	int calccount = 0;
	while ((!IsHigherFootThanGround(ROUNDINGPOS)) && (calccount < maxcount)) {//円を描くように下がってから上がることが多い　回数は多めに

		int wallscrapingikflag = 1;

		//#####################################################################################
		//2024/09/13 LimitEul角度制限のオンオフに対応　制限オンの時には強制的に壁すりIKもオン
		//しかし、limiteulオフで計算した方が綺麗に動く
		//limiteulオンで足を上げると足を戻す動作の変化率が大きくなり(速くなり)綺麗にはみえなかった
		//Rigの動作範囲は確認可能であり、FootRigの設定で制限した方が綺麗に動くのでそうすることに
		//角度制限に対応したが、FootRigに関してはUpdate関数内でlimitdegflag = falseに上書きして使用する
		//#####################################################################################
		int notmovecount = srcmodel->RigControlFootRig(
			limitdegflag,
			wallscrapingikflag,
			0, curframe,
			//footbone->GetBoneNo(),
			m_toebasejoint->GetBoneNo(),//!!!!!!!!! 高さ比較は引数のbone. rigboneはtoebase
			m_rigdir,
			rigstep,
			m_rig, 0);

		////if ((g_previewFlag == 4) || (g_previewFlag == 5)) {
		//if (g_limitdegflag) {
		//	srcmodel->BlendSaveBoneMotionReq(updatebone, BONEMOTIONBLEND);//プルプル震え防止のための１回前とのブレンド
		//}

		srcmodel->UpdateMatrixFootRigReq(istoebase, limitdegflag, m_updatebone, &modelwm, &matView, &matProj);//角度制限あり無し両方に　現状の姿勢を格納

		CalcPos(limitdegflag);
		//newbonepos = m_footrigdlg->GetJointPos(limitdegflag, srcmodel, footbone, posoffset, istoebase);
		//*pgroundpos = m_footrigdlg->GetGroundPos(groundmodel, newbonepos, gpuflag);


		calccount++;

		//2024/09/13 壁すりをオンにしたので次のif文はコメントアウト
		//if ((notmovecount < 0) || (notmovecount >= rignum)) {
		//	//エラーが起きた場合　または　角度制限機能により１つのジョイントも回転しなかった場合は処理を抜ける
		//	break;
		//}
	}

	return 0;
}

