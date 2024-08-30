#include "stdafx.h"

#include <FootRigDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <ChaScene.h>
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
	if (m_leftoffsetLabel) {
		delete m_leftoffsetLabel;
		m_leftoffsetLabel = nullptr;
	}
	if (m_leftoffsetEdit) {
		delete m_leftoffsetEdit;
		m_leftoffsetEdit = nullptr;
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
	if (m_rightoffsetLabel) {
		delete m_rightoffsetLabel;
		m_rightoffsetLabel = nullptr;
	}
	if (m_rightoffsetEdit) {
		delete m_rightoffsetEdit;
		m_rightoffsetEdit = nullptr;
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
	if (m_applyB) {
		delete m_applyB;
		m_applyB = nullptr;
	}


	if (m_groundmeshsp) {
		delete m_groundmeshsp;
		m_groundmeshsp = nullptr;
	}
	if (m_leftfootbonesp) {
		delete m_leftfootbonesp;
		m_leftfootbonesp = nullptr;
	}
	if (m_leftoffsetsp) {
		delete m_leftoffsetsp;
		m_leftoffsetsp = nullptr;
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
	if (m_rightoffsetsp) {
		delete m_rightoffsetsp;
		m_rightoffsetsp = nullptr;
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

	m_enablefootrig = false;
	m_model = nullptr;
	m_chascene = nullptr;
	m_footrigelem.clear();


	m_dlgWnd = nullptr;

	m_enableChk = nullptr;
	m_modellabel = nullptr;
	m_groundlabel = nullptr;
	m_groundCombo = nullptr;
	m_leftfootlabel = nullptr;
	m_leftfootBonelabel = nullptr;
	m_leftfootBoneCombo = nullptr;
	m_leftoffsetLabel = nullptr;
	m_leftoffsetEdit = nullptr;
	m_leftriglabel = nullptr;
	m_leftrigCombo = nullptr;
	m_leftdirlabel = nullptr;
	m_leftdirCombo = nullptr;
	m_rightfootlabel = nullptr;
	m_rightfootBonelabel = nullptr;
	m_rightfootBoneCombo = nullptr;
	m_rightoffsetLabel = nullptr;
	m_rightoffsetEdit = nullptr;
	m_rightriglabel = nullptr;
	m_rightrigCombo = nullptr;
	m_rightdirlabel = nullptr;
	m_rightdirCombo = nullptr;
	m_hdiffmaxlabel = nullptr;
	m_hdiffmaxEdit = nullptr;
	m_rigsteplabel = nullptr;
	m_rigstepEdit = nullptr;
	m_applyB = nullptr;

	m_groundmeshsp = nullptr;
	m_leftfootbonesp = nullptr;
	m_leftoffsetsp = nullptr;
	m_leftrigsp = nullptr;
	m_leftdirsp = nullptr;
	m_rightfootbonesp = nullptr;
	m_rightoffsetsp = nullptr;
	m_rightrigsp = nullptr;
	m_rightdirsp = nullptr;
	m_hdiffmaxsp = nullptr;
	m_rigstepsp = nullptr;
	m_spacerlabel0 = nullptr;
	m_spacerlabel1 = nullptr;
	m_spacerlabel2 = nullptr;
	m_spacerlabel3 = nullptr;
	m_spacerlabel4 = nullptr;
	m_spacerlabel5 = nullptr;
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

		std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
		itrelem = m_footrigelem.find(m_model);
		if (itrelem == m_footrigelem.end()) {
			FOOTRIGELEM newfootelem;
			newfootelem.Init();
			m_footrigelem[m_model] = newfootelem;
		}

		CreateFootRigWnd();//作成済の場合は０リターン
		ParamsToDlg();
	}
	else {
		_ASSERT(0);
		return 1;
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

		m_enableChk = new OWP_CheckBoxA(L"Eanble FootRig", m_enablefootrig, labelheight, false);
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
		m_leftoffsetLabel = new OWP_Label(L"Height Offset", labelheight);
		if (!m_leftoffsetLabel) {
			_ASSERT(0);
			abort();
		}
		m_leftoffsetEdit = new OWP_EditBox(true, L"leftoffsetEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_leftoffsetEdit) {
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
		m_rightoffsetLabel = new OWP_Label(L"Height Offset", labelheight);
		if (!m_rightoffsetLabel) {
			_ASSERT(0);
			abort();
		}
		m_rightoffsetEdit = new OWP_EditBox(true, L"rightoffsetEdit", labelheight, EDIT_BUFLEN_NUM);
		if (!m_rightoffsetEdit) {
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
		m_applyB = new OWP_Button(L"Apply(適用)", 38);
		if (!m_applyB) {
			_ASSERT(0);
			abort();
		}
		m_applyB->setTextColor(RGB(168, 129, 129));


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
		m_leftoffsetsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_leftoffsetsp) {
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
		m_rightoffsetsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rightoffsetsp) {
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

		m_dlgWnd->addParts(*m_modellabel);
		m_dlgWnd->addParts(*m_enableChk);
		m_dlgWnd->addParts(*m_spacerlabel0);

		m_dlgWnd->addParts(*m_groundmeshsp);
		m_groundmeshsp->addParts1(*m_groundlabel);
		m_groundmeshsp->addParts2(*m_groundCombo);

		m_dlgWnd->addParts(*m_spacerlabel1);
		m_dlgWnd->addParts(*m_leftfootlabel);
		m_dlgWnd->addParts(*m_leftfootbonesp);
		m_leftfootbonesp->addParts1(*m_leftfootBonelabel);
		m_leftfootbonesp->addParts2(*m_leftfootBoneCombo);
		m_dlgWnd->addParts(*m_leftoffsetsp);
		m_leftoffsetsp->addParts1(*m_leftoffsetLabel);
		m_leftoffsetsp->addParts2(*m_leftoffsetEdit);
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
		m_dlgWnd->addParts(*m_rightoffsetsp);
		m_rightoffsetsp->addParts1(*m_rightoffsetLabel);
		m_rightoffsetsp->addParts2(*m_rightoffsetEdit);
		m_dlgWnd->addParts(*m_rightrigsp);
		m_rightrigsp->addParts1(*m_rightriglabel);
		m_rightrigsp->addParts2(*m_rightrigCombo);
		m_dlgWnd->addParts(*m_rightdirsp);
		m_rightdirsp->addParts1(*m_rightdirlabel);
		m_rightdirsp->addParts2(*m_rightdirCombo);

		m_dlgWnd->addParts(*m_spacerlabel3);
		m_dlgWnd->addParts(*m_hdiffmaxsp);
		m_hdiffmaxsp->addParts1(*m_hdiffmaxlabel);
		m_hdiffmaxsp->addParts2(*m_hdiffmaxEdit);
		m_dlgWnd->addParts(*m_spacerlabel4);
		m_dlgWnd->addParts(*m_rigstepsp);
		m_rigstepsp->addParts1(*m_rigsteplabel);
		m_rigstepsp->addParts2(*m_rigstepEdit);
		m_dlgWnd->addParts(*m_spacerlabel5);

		m_dlgWnd->addParts(*m_applyB);

		//int slotindex;
		//for (slotindex = 0; slotindex < 8; slotindex++) {
		//	char strslot[128] = { 0 };
		//	sprintf_s(strslot, 128, "Slot_%d", slotindex);
		//	m_lightsslotCombo->addString(strslot);
		//}
		//m_lightsslotCombo->setSelectedCombo(g_lightSlot);


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
			m_enablefootrig = value;
			});

		m_applyB->setButtonListener([=, this]() {
			Dlg2Params();

			////SetLightDirection();
			//if (g_mainhwnd && IsWindow(g_mainhwnd)) {
			//	PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
			//}
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

		if (m_enableChk) {
			m_enableChk->setValue(m_enablefootrig, false);
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

		if (m_leftoffsetEdit) {
			WCHAR wleftoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wleftoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.leftoffset);
			m_leftoffsetEdit->setName(wleftoffset);
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

		if (m_rightoffsetEdit) {
			WCHAR wrightoffset[EDIT_BUFLEN_NUM] = { 0L };
			swprintf_s(wrightoffset, EDIT_BUFLEN_NUM, L"%.2f", curfootrigelem.rightoffset);
			m_rightoffsetEdit->setName(wrightoffset);
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


int CFootRigDlg::Dlg2Params()
{
	if (m_dlgWnd != 0) {
		//if ((g_lightSlot < 0) || (g_lightSlot >= LIGHTSLOTNUM)) {
		//	_ASSERT(0);
		//	return 1;
		//}

		if (m_leftoffsetEdit) {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_leftoffsetEdit->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.leftoffset = offsetval;
				}
			}
		}
		if (m_rightoffsetEdit) {
			WCHAR stroffset[EDIT_BUFLEN_NUM] = { 0L };
			m_rightoffsetEdit->getName(stroffset, EDIT_BUFLEN_NUM);
			float offsetval = (float)_wtof(stroffset);
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.rightoffset = offsetval;
				}
			}
		}


		if (m_hdiffmaxEdit) {
			WCHAR strdiff[EDIT_BUFLEN_NUM] = { 0L };
			m_hdiffmaxEdit->getName(strdiff, EDIT_BUFLEN_NUM);
			float diffval = (float)_wtof(strdiff);
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.hdiffmax = diffval;
				}
			}
		}

		if (m_rigstepEdit) {
			WCHAR strstep[EDIT_BUFLEN_NUM] = { 0L };
			m_rigstepEdit->getName(strstep, EDIT_BUFLEN_NUM);
			float stepval = (float)_wtof(strstep);
			if (m_model) {
				std::map<CModel*, FOOTRIGELEM>::iterator itrelem;
				itrelem = m_footrigelem.find(m_model);
				if (itrelem != m_footrigelem.end()) {
					itrelem->second.rigstep = stepval;
				}
			}
		}

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

