#include "stdafx.h"

#include <RigidParamsDlg.h>
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
// CRigidParamsDlg

CRigidParamsDlg::CRigidParamsDlg()
{
	InitParams();
}

CRigidParamsDlg::~CRigidParamsDlg()
{
	DestroyObjs();
}
	
int CRigidParamsDlg::DestroyObjs()
{
	if (m_rigidWnd) {
		m_rigidWnd->setVisible(false);
		m_rigidWnd->setListenMouse(false);
	}


	if (m_sphrateSlider) {
		delete m_sphrateSlider;
		m_sphrateSlider = nullptr;
	}
	if (m_boxzSlider) {
		delete m_boxzSlider;
		m_boxzSlider = nullptr;
	}
	if (m_boxzlabel) {
		delete m_boxzlabel;
		m_boxzlabel = nullptr;
	}
	if (m_massSlider) {
		delete m_massSlider;
		m_massSlider = nullptr;
	}
	if (m_massSLlabel) {
		delete m_massSLlabel;
		m_massSLlabel = nullptr;
	}
	//if (m_massSeparator) {
	//	delete m_massSeparator;
	//	m_massSeparator = nullptr;
	//}
	//if (m_massSeparator1) {
	//	delete m_massSeparator1;
	//	m_massSeparator1 = nullptr;
	//}
	//if (m_massSeparator2) {
	//	delete m_massSeparator2;
	//	m_massSeparator2 = nullptr;
	//}
	if (m_massB) {
		delete m_massB;
		m_massB = nullptr;
	}
	if (m_thicknessB) {
		delete m_thicknessB;
		m_thicknessB = nullptr;
	}
	if (m_depthB) {
		delete m_depthB;
		m_depthB = nullptr;
	}
	if (m_massspacelabel) {
		delete m_massspacelabel;
		m_massspacelabel = nullptr;
	}
	if (m_namelabel) {
		delete m_namelabel;
		m_namelabel = nullptr;
	}
	if (m_lenglabel) {
		delete m_lenglabel;
		m_lenglabel = nullptr;
	}
	if (m_skipB) {
		delete m_skipB;
		m_skipB = nullptr;
	}
	if (m_rigidskip) {
		delete m_rigidskip;
		m_rigidskip = nullptr;
	}
	if (m_forbidSeparator) {
		delete m_forbidSeparator;
		m_forbidSeparator = nullptr;
	}
	if (m_forbidB) {
		delete m_forbidB;
		m_forbidB = nullptr;
	}
	if (m_forbidrot) {
		delete m_forbidrot;
		m_forbidrot = nullptr;
	}

	if (m_validSeparator) {
		delete m_validSeparator;
		m_validSeparator = nullptr;
	}
	if (m_allrigidenableB) {
		delete m_allrigidenableB;
		m_allrigidenableB = nullptr;
	}
	if (m_allrigiddisableB) {
		delete m_allrigiddisableB;
		m_allrigiddisableB = nullptr;
	}


	if (m_shplabel) {
		delete m_shplabel;
		m_shplabel = nullptr;
	}
	if (m_colSeparator) {
		delete m_colSeparator;
		m_colSeparator = nullptr;
	}
	if (m_colB) {
		delete m_colB;
		m_colB = nullptr;
	}
	if (m_colradio) {
		delete m_colradio;
		m_colradio = nullptr;
	}
	if (m_lkradio) {
		delete m_lkradio;
		m_lkradio = nullptr;
	}
	if (m_lkSlider) {
		delete m_lkSlider;
		m_lkSlider = nullptr;
	}
	if (m_lklabel) {
		delete m_lklabel;
		m_lklabel = nullptr;
	}
	if (m_akradio) {
		delete m_akradio;
		m_akradio = nullptr;
	}
	if (m_akSlider) {
		delete m_akSlider;
		m_akSlider = nullptr;
	}
	if (m_aklabel) {
		delete m_aklabel;
		m_aklabel = nullptr;
	}
	if (m_restSlider) {
		delete m_restSlider;
		m_restSlider = nullptr;
	}
	if (m_restlabel) {
		delete m_restlabel;
		m_restlabel = nullptr;
	}
	if (m_fricSlider) {
		delete m_fricSlider;
		m_fricSlider = nullptr;
	}
	if (m_friclabel) {
		delete m_friclabel;
		m_friclabel = nullptr;
	}
	if (m_ldmpSlider) {
		delete m_ldmpSlider;
		m_ldmpSlider = nullptr;
	}
	if (m_admpSlider) {
		delete m_admpSlider;
		m_admpSlider = nullptr;
	}
	if (m_kB) {
		delete m_kB;
		m_kB = nullptr;
	}
	if (m_restB) {
		delete m_restB;
		m_restB = nullptr;
	}
	if (m_ldmplabel) {
		delete m_ldmplabel;
		m_ldmplabel = nullptr;
	}
	if (m_admplabel) {
		delete m_admplabel;
		m_admplabel = nullptr;
	}
	if (m_dmpB) {
		delete m_dmpB;
		m_dmpB = nullptr;
	}
	if (m_groupB) {
		delete m_groupB;
		m_groupB = nullptr;
	}
	if (m_gcoliB) {
		delete m_gcoliB;
		m_gcoliB = nullptr;
	}
	if (m_btgSlider) {
		delete m_btgSlider;
		m_btgSlider = nullptr;
	}
	if (m_btglabel) {
		delete m_btglabel;
		m_btglabel = nullptr;
	}
	if (m_btgscSlider) {
		delete m_btgscSlider;
		m_btgscSlider = nullptr;
	}
	if (m_btgsclabel) {
		delete m_btgsclabel;
		m_btgsclabel = nullptr;
	}
	if (m_btforceSeparator) {
		delete m_btforceSeparator;
		m_btforceSeparator = nullptr;
	}
	if (m_btforce) {
		delete m_btforce;
		m_btforce = nullptr;
	}
	if (m_btforceB) {
		delete m_btforceB;
		m_btforceB = nullptr;
	}
	if (m_rigidsp0) {
		delete m_rigidsp0;
		m_rigidsp0 = nullptr;
	}
	if (m_groupcheck) {
		delete m_groupcheck;
		m_groupcheck = nullptr;
	}
	if (m_btgB) {
		delete m_btgB;
		m_btgB = nullptr;
	}
	if (m_thicknessSeparator) {
		delete m_thicknessSeparator;
		m_thicknessSeparator = nullptr;
	}
	if (m_depthSeparator) {
		delete m_depthSeparator;
		m_depthSeparator = nullptr;
	}
	if (m_massSeparator) {
		delete m_massSeparator;
		m_massSeparator = nullptr;
	}
	if (m_thicknessSeparator2) {
		delete m_thicknessSeparator2;
		m_thicknessSeparator2 = nullptr;
	}
	if (m_depthSeparator2) {
		delete m_depthSeparator2;
		m_depthSeparator2 = nullptr;
	}
	if (m_massSeparator2) {
		delete m_massSeparator2;
		m_massSeparator2 = nullptr;
	}
	if (m_validSeparator2) {
		delete m_validSeparator2;
		m_validSeparator2 = nullptr;
	}
	if (m_forbidSeparator2) {
		delete m_forbidSeparator2;
		m_forbidSeparator2 = nullptr;
	}
	if (m_colSeparator2) {
		delete m_colSeparator2;
		m_colSeparator2 = nullptr;
	}
	if (m_springSeparator) {
		delete m_springSeparator;
		m_springSeparator = nullptr;
	}
	if (m_restitutionSeparator) {
		delete m_restitutionSeparator;
		m_restitutionSeparator = nullptr;
	}
	if (m_dumpingSeparator) {
		delete m_dumpingSeparator;
		m_dumpingSeparator = nullptr;
	}
	if (m_gSeparator) {
		delete m_gSeparator;
		m_gSeparator = nullptr;
	}
	if (m_btforceSeparator2) {
		delete m_btforceSeparator2;
		m_btforceSeparator2 = nullptr;
	}
	if (m_coliidSeparator) {
		delete m_coliidSeparator;
		m_coliidSeparator = nullptr;
	}


	if (m_thicknessDeeperB) {
		delete m_thicknessDeeperB;
		m_thicknessDeeperB = nullptr;
	}
	if (m_depthDeeperB) {
		delete m_depthDeeperB;
		m_depthDeeperB = nullptr;
	}
	if (m_massDeeperB) {
		delete m_massDeeperB;
		m_massDeeperB = nullptr;
	}
	if (m_validDeeperB) {
		delete m_validDeeperB;
		m_validDeeperB = nullptr;
	}
	if (m_forbidDeeperB) {
		delete m_forbidDeeperB;
		m_forbidDeeperB = nullptr;
	}
	if (m_colDeeperB) {
		delete m_colDeeperB;
		m_colDeeperB = nullptr;
	}
	if (m_springDeeperB) {
		delete m_springDeeperB;
		m_springDeeperB = nullptr;
	}
	if (m_restitutionDeeperB) {
		delete m_restitutionDeeperB;
		m_restitutionDeeperB = nullptr;
	}
	if (m_dumpingDeeperB) {
		delete m_dumpingDeeperB;
		m_dumpingDeeperB = nullptr;
	}
	if (m_gDeeperB) {
		delete m_gDeeperB;
		m_gDeeperB = nullptr;
	}
	if (m_btforceDeeperB) {
		delete m_btforceDeeperB;
		m_btforceDeeperB = nullptr;
	}
	if (m_coliidDeeperB) {
		delete m_coliidDeeperB;
		m_coliidDeeperB = nullptr;
	}
	if (m_coliseparator) {
		delete m_coliseparator;
		m_coliseparator = nullptr;
	}
	if (m_rigidspacerLabel01) {
		delete m_rigidspacerLabel01;
		m_rigidspacerLabel01 = nullptr;
	}
	if (m_rigidspacerLabel02) {
		delete m_rigidspacerLabel02;
		m_rigidspacerLabel02 = nullptr;
	}
	if (m_rigidspacerLabel03) {
		delete m_rigidspacerLabel03;
		m_rigidspacerLabel03 = nullptr;
	}
	if (m_rigidspacerLabel04) {
		delete m_rigidspacerLabel04;
		m_rigidspacerLabel04 = nullptr;
	}
	if (m_rigidspacerLabel05) {
		delete m_rigidspacerLabel05;
		m_rigidspacerLabel05 = nullptr;
	}
	if (m_rigidspacerLabel06) {
		delete m_rigidspacerLabel06;
		m_rigidspacerLabel06 = nullptr;
	}
	if (m_rigidspacerLabel07) {
		delete m_rigidspacerLabel07;
		m_rigidspacerLabel07 = nullptr;
	}
	if (m_rigidspacerLabel08) {
		delete m_rigidspacerLabel08;
		m_rigidspacerLabel08 = nullptr;
	}

	if (m_rigidspall) {
		delete m_rigidspall;
		m_rigidspall = nullptr;
	}
	if (m_rigidSc) {
		delete m_rigidSc;
		m_rigidSc = nullptr;
	}

	if (m_rigidWnd) {
		delete m_rigidWnd;
		m_rigidWnd = nullptr;
	}

	return 0;
}

void CRigidParamsDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_model = nullptr;
	m_curboneno = -1;
	m_reindexmap.clear();
	m_rgdindexmap.clear();

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;


	m_rigidWnd = nullptr;
	m_rigidSc = nullptr;
	m_rigidspall = nullptr;
	m_sphrateSlider = nullptr;
	m_boxzSlider = nullptr;
	m_boxzlabel = nullptr;
	m_massSlider = nullptr;
	m_massSLlabel = nullptr;
	m_massB = nullptr;
	m_thicknessB = nullptr;
	m_depthB = nullptr;
	m_massspacelabel = nullptr;
	m_namelabel = nullptr;
	m_lenglabel = nullptr;
	m_skipB = nullptr;
	m_rigidskip = nullptr;
	m_forbidSeparator = nullptr;
	m_forbidB = nullptr;
	m_forbidrot = nullptr;
	m_validSeparator = nullptr;
	m_allrigidenableB = nullptr;
	m_allrigiddisableB = nullptr;
	m_shplabel = nullptr;
	m_colSeparator = nullptr;
	m_colB = nullptr;
	m_colradio = nullptr;
	m_lkradio = nullptr;
	m_lkSlider = nullptr;
	m_lklabel = nullptr;
	m_akradio = nullptr;
	m_akSlider = nullptr;
	m_aklabel = nullptr;
	m_restSlider = nullptr;
	m_restlabel = nullptr;
	m_fricSlider = nullptr;
	m_friclabel = nullptr;
	m_ldmpSlider = nullptr;
	m_admpSlider = nullptr;
	m_kB = nullptr;
	m_restB = nullptr;
	m_ldmplabel = nullptr;
	m_admplabel = nullptr;
	m_dmpB = nullptr;
	m_groupB = nullptr;
	m_gcoliB = nullptr;
	m_btgSlider = nullptr;
	m_btglabel = nullptr;
	m_btgscSlider = nullptr;
	m_btgsclabel = nullptr;
	m_btforceSeparator = nullptr;
	m_btforce = nullptr;
	m_btforceB = nullptr;
	m_rigidsp0 = nullptr;
	m_groupcheck = nullptr;
	m_btgB = nullptr;
	m_thicknessSeparator = nullptr;
	m_depthSeparator = nullptr;
	m_massSeparator = nullptr;
	m_thicknessSeparator2 = nullptr;
	m_depthSeparator2 = nullptr;
	m_massSeparator2 = nullptr;
	m_validSeparator2 = nullptr;
	m_forbidSeparator2 = nullptr;
	m_colSeparator2 = nullptr;
	m_springSeparator = nullptr;
	m_restitutionSeparator = nullptr;
	m_dumpingSeparator = nullptr;
	m_gSeparator = nullptr;
	m_btforceSeparator2 = nullptr;
	m_coliidSeparator = nullptr;
	m_thicknessDeeperB = nullptr;
	m_depthDeeperB = nullptr;
	m_massDeeperB = nullptr;
	m_validDeeperB = nullptr;
	m_forbidDeeperB = nullptr;
	m_colDeeperB = nullptr;
	m_springDeeperB = nullptr;
	m_restitutionDeeperB = nullptr;
	m_dumpingDeeperB = nullptr;
	m_gDeeperB = nullptr;
	m_btforceDeeperB = nullptr;
	m_coliidDeeperB = nullptr;
	m_coliseparator = nullptr;
	m_rigidspacerLabel01 = nullptr;
	m_rigidspacerLabel02 = nullptr;
	m_rigidspacerLabel03 = nullptr;
	m_rigidspacerLabel04 = nullptr;
	m_rigidspacerLabel05 = nullptr;
	m_rigidspacerLabel06 = nullptr;
	m_rigidspacerLabel07 = nullptr;
	m_rigidspacerLabel08 = nullptr;



}

int CRigidParamsDlg::SetModel(CModel* srcmodel, int srcboneno, 
	std::unordered_map<CModel*, int>& srcreindexmap, std::unordered_map<CModel*, int>& srcrgdindexmap)
{
	m_model = srcmodel;
	m_curboneno = srcboneno;
	m_reindexmap = srcreindexmap;
	m_rgdindexmap = srcrgdindexmap;

	CreateRigidWnd();//作成済の場合は０リターン

	if (m_rigidWnd && m_model) {
		SetRigidLeng();
		RigidElem2WndParam();
	}

	return 0;
}

int CRigidParamsDlg::SetRigidLeng()
{
	if (m_curboneno < 0) {
		return 0;
	}
	if (!m_model) {
		return 0;
	}
	if (!m_model->GetTopBt()) {
		return 0;
	}

	CBtObject* curbto = m_model->FindBtObject(m_curboneno);
	if (curbto) {
		WCHAR curlabel[512];
		swprintf_s(curlabel, 512, L"BonaName：%s", curbto->GetEndBone()->GetWBoneName());
		if (m_namelabel) {
			m_namelabel->setName(curlabel);
		}

		WCHAR curlabel2[256];
		swprintf_s(curlabel2, 256, L"BoneLength:%.3f[m]", curbto->GetBoneLeng());
		if (m_lenglabel) {
			m_lenglabel->setName(curlabel2);
		}
	}
	else {
		WCHAR curlabel[512];
		wcscpy_s(curlabel, 512, L"BoneName：not selected");
		if (m_namelabel) {
			m_namelabel->setName(curlabel);
		}

		WCHAR curlabel2[256];
		wcscpy_s(curlabel2, 256, L"BoneLength：not selected");
		if (m_lenglabel) {
			m_lenglabel->setName(curlabel2);
		}
	}
	return 0;
}

int CRigidParamsDlg::RigidElem2WndParam()
{
	if (!m_model) {
		return 0;
	}
	if (m_curboneno < 0) {
		return 0;
	}


	//ダイアログの数値はメニューで選択中のもの
	m_model->SetCurrentRigidElem(m_reindexmap[m_model]);


	CBone* curbone = m_model->GetBoneByID(m_curboneno);
	if (curbone) {
		//int kinflag = curbone->m_btforce;

		CBone* parentbone = curbone->GetParent(false);
		if (parentbone && parentbone->IsSkeleton()) {
			int kinflag = parentbone->GetBtForce();
			m_btforce->setValue((bool)kinflag, false);

			CRigidElem* curre = parentbone->GetRigidElem(curbone);
			if (curre) {
				float rate = (float)curre->GetSphrate();
				float boxz = (float)curre->GetBoxzrate();
				float mass = (float)curre->GetMass();
				bool skipflag = !((bool)curre->GetSkipflag());
				int colindex = curre->GetColtype();
				int lkindex = curre->GetLKindex();
				int akindex = curre->GetAKindex();
				float ldmp = curre->GetLDamping();
				float admp = curre->GetADamping();
				float cuslk = curre->GetCusLk();
				float cusak = curre->GetCusAk();
				float rest = curre->GetRestitution();
				float fric = curre->GetFriction();
				float btg = curre->GetBtg();
				int forbidrot = curre->GetForbidRotFlag();

				if (m_sphrateSlider) {
					m_sphrateSlider->setValue(rate, false);
				}
				if (m_boxzSlider) {
					m_boxzSlider->setValue(boxz, false);
				}
				if (m_massSlider) {
					m_massSlider->setValue(mass, false);
				}
				if (m_rigidskip) {
					m_rigidskip->setValue(skipflag, false);
				}
				if (m_forbidrot) {
					m_forbidrot->setValue(forbidrot, false);
				}
				if (m_colradio) {
					m_colradio->setSelectIndex(colindex, false);
				}
				if (m_lkradio) {
					m_lkradio->setSelectIndex(lkindex, false);
				}
				if (m_akradio) {
					m_akradio->setSelectIndex(akindex, false);
				}
				if (m_ldmpSlider) {
					m_ldmpSlider->setValue(ldmp, false);
				}
				if (m_admpSlider) {
					m_admpSlider->setValue(admp, false);
				}
				if (m_lkSlider) {
					m_lkSlider->setValue(cuslk, false);
				}
				if (m_akSlider) {
					m_akSlider->setValue(cusak, false);
				}
				if (m_restSlider) {
					m_restSlider->setValue(rest, false);
				}
				if (m_fricSlider) {
					m_fricSlider->setValue(fric, false);
				}
				if (m_btgSlider) {
					m_btgSlider->setValue(btg, false);
				}
			}
			else {
				//rigid elemが作成されていないとき
				if (m_sphrateSlider) {
					m_sphrateSlider->setValue(1.0, false);
				}
				if (m_boxzSlider) {
					m_boxzSlider->setValue(1.0, false);
				}
				if (m_massSlider) {
					m_massSlider->setValue(1.0, false);
				}
				if (m_rigidskip) {
					m_rigidskip->setValue(0, false);
				}
				if (m_forbidrot) {
					m_forbidrot->setValue(0, false);
				}
				if (m_colradio) {
					m_colradio->setSelectIndex(0, false);
				}
				if (m_lkradio) {
					m_lkradio->setSelectIndex(0, false);
				}
				if (m_akradio) {
					m_akradio->setSelectIndex(0, false);
				}
				if (m_ldmpSlider) {
					m_ldmpSlider->setValue(g_l_dmp, false);
				}
				if (m_admpSlider) {
					m_admpSlider->setValue(g_a_dmp, false);
				}
				if (m_lkSlider) {
					m_lkSlider->setValue(g_initcuslk, false);
				}
				if (m_akSlider) {
					m_akSlider->setValue(g_initcusak, false);
				}
				if (m_restSlider) {
					m_restSlider->setValue(0.0, false);
				}
				if (m_fricSlider) {
					m_fricSlider->setValue(0.0, false);
				}
				if (m_btgSlider) {
					m_btgSlider->setValue(9.0, false);
				}
			}
		}
		else {
			//rigid elemが作成されていないとき
			if (m_sphrateSlider) {
				m_sphrateSlider->setValue(1.0, false);
			}
			if (m_boxzSlider) {
				m_boxzSlider->setValue(1.0, false);
			}
			if (m_massSlider) {
				m_massSlider->setValue(1.0, false);
			}
			if (m_rigidskip) {
				m_rigidskip->setValue(0, false);
			}
			if (m_forbidrot) {
				m_forbidrot->setValue(0, false);
			}
			if (m_colradio) {
				m_colradio->setSelectIndex(0, false);
			}
			if (m_lkradio) {
				m_lkradio->setSelectIndex(0, false);
			}
			if (m_akradio) {
				m_akradio->setSelectIndex(0, false);
			}
			if (m_ldmpSlider) {
				m_ldmpSlider->setValue(g_l_dmp, false);
			}
			if (m_admpSlider) {
				m_admpSlider->setValue(g_a_dmp, false);
			}
			if (m_lkSlider) {
				m_lkSlider->setValue(g_initcuslk, false);
			}
			if (m_akSlider) {
				m_akSlider->setValue(g_initcusak, false);
			}
			if (m_restSlider) {
				m_restSlider->setValue(0.0, false);
			}
			if (m_fricSlider) {
				m_fricSlider->setValue(0.0, false);
			}
			if (m_btgSlider) {
				m_btgSlider->setValue(9.0, false);
			}
		}
		if (m_namelabel) {
			m_namelabel->setName((WCHAR*)curbone->GetWBoneName());
		}
	}
	else {
		if (m_namelabel) {
			WCHAR noname[256];
			wcscpy_s(noname, 256, L"BoneName：not selected");
			m_namelabel->setName(noname);
		}
	}
	if (m_model && (m_model->GetNoBoneFlag() == false) && m_btgscSlider) {
		m_btgscSlider->setValue(m_model->GetBtGScale(m_model->GetCurReIndex()), false);
	}


	if (m_rigidWnd) {
		m_rigidWnd->callRewrite();
	}


	//再生中、シミュレーション中への対応。元の状態に戻す。
	if (g_previewFlag != 5) {
		m_model->SetCurrentRigidElem(m_reindexmap[m_model]);
	}
	else {
		m_model->SetCurrentRigidElem(m_rgdindexmap[m_model]);
	}


	return 0;
}



int CRigidParamsDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}




void CRigidParamsDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_rigidWnd) {
			m_rigidWnd->setListenMouse(true);
			m_rigidWnd->setVisible(true);
			if (m_rigidSc) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_rigidSc->autoResize();
			}
			m_rigidWnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_rigidWnd) {
			m_rigidWnd->setVisible(false);
			m_rigidWnd->setListenMouse(false);
		}
	}
	m_visible = srcflag;
}


int CRigidParamsDlg::CreateRigidWnd()
{
	if (m_rigidWnd) {
		return 0;
	}

	m_rigidWnd = new OrgWindow(
		0,
		_T("RigidWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("RigidWindow"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		true,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true, true);					//サイズ変更の可否

	int labelheight;
	if (g_4kresolution) {
		labelheight = 28;
	}
	else {
		labelheight = 20;
	}

	if (m_rigidWnd) {
		bool limitradionamelen = false;

		m_rigidSc = new OWP_ScrollWnd(L"RigidScroll", true, labelheight);
		if (!m_rigidSc) {
			_ASSERT(0);
			return 1;
		}
		int linedatasize = (int)(51.0 * 1.25);
		m_rigidSc->setLineDataSize(linedatasize);//!!!!!!!!!!!!!
		m_rigidWnd->addParts(*m_rigidSc);

		m_rigidspall = new OWP_Separator(m_rigidWnd, true, 0.995, false, m_rigidSc);//スクロールの子供全体をまとめるセパレータ　縦分割
		if (!m_rigidspall) {
			_ASSERT(0);
			return 1;
		}
		m_rigidSc->addParts(*m_rigidspall);


		m_rigidsp0 = new OWP_Separator(m_rigidWnd, true, 0.7, true);
		if (!m_rigidsp0) {
			_ASSERT(0);
			return 1;
		}
		//m_rigidWnd->addParts(*m_rigidsp0);

		m_coliseparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_coliseparator) {
			_ASSERT(0);
			return 1;
		}

		m_thicknessSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_thicknessSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_depthSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_depthSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_massSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_massSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_thicknessSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.20, true);
		if (!m_thicknessSeparator2) {
			_ASSERT(0);
			return 1;
		}
		m_depthSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.20, true);
		if (!m_depthSeparator2) {
			_ASSERT(0);
			return 1;
		}
		m_massSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.20, true);
		if (!m_massSeparator2) {
			_ASSERT(0);
			return 1;
		}


		m_validSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.50, true);
		if (!m_validSeparator2) {
			_ASSERT(0);
			return 1;
		}
		m_forbidSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_forbidSeparator2) {
			_ASSERT(0);
			return 1;
		}
		m_colSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_colSeparator2) {
			_ASSERT(0);
			return 1;
		}
		m_springSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_springSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_restitutionSeparator = new OWP_Separator(m_rigidWnd, true, 0.65, true);
		if (!m_restitutionSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_dumpingSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_dumpingSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_gSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_gSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_btforceSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_btforceSeparator2) {
			_ASSERT(0);
			return 1;
		}
		m_coliidSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_coliidSeparator) {
			_ASSERT(0);
			return 1;
		}

		m_thicknessDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_thicknessDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_depthDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_depthDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_massDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_massDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_validDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_validDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_forbidDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_forbidDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_colDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_colDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_springDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_springDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_restitutionDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_restitutionDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_dumpingDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_dumpingDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_gDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_gDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_btforceDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_btforceDeeperB) {
			_ASSERT(0);
			return 1;
		}
		m_coliidDeeperB = new OWP_Button(L"ToDeeper", labelheight);
		if (!m_coliidDeeperB) {
			_ASSERT(0);
			return 1;
		}




		m_groupcheck = new OWP_CheckBoxA(L"ToAll=ToGroup", 0, labelheight, false);
		if (!m_groupcheck) {
			_ASSERT(0);
			return 1;
		}

		m_sphrateSlider = new OWP_Slider(0.6, 20.0, 0.0, labelheight);
		if (!m_sphrateSlider) {
			_ASSERT(0);
			return 1;
		}
		m_boxzSlider = new OWP_Slider(0.6, 20.0, 0.0, labelheight);
		if (!m_boxzSlider) {
			_ASSERT(0);
			return 1;
		}
		m_massSlider = new OWP_Slider(g_initmass, 30.0, 0.0, labelheight);
		if (!m_massSlider) {
			_ASSERT(0);
			return 1;
		}
		//m_massSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		//m_massSeparator1 = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		//m_massSeparator2 = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		m_massB = new OWP_Button(L"MassToAll", labelheight);
		if (!m_massB) {
			_ASSERT(0);
			return 1;
		}
		m_thicknessB = new OWP_Button(L"ThicknessToAll", labelheight);
		if (!m_thicknessB) {
			_ASSERT(0);
			return 1;
		}
		m_depthB = new OWP_Button(L"DepthToAll", labelheight);
		if (!m_depthB) {
			_ASSERT(0);
			return 1;
		}
		m_massspacelabel = new OWP_Label(L"(Space4)", labelheight);
		if (!m_massspacelabel) {
			_ASSERT(0);
			return 1;
		}
		m_validSeparator = new OWP_Separator(m_rigidWnd, true, 0.60, true);
		if (!m_validSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_skipB = new OWP_Button(L"ToAll", labelheight);
		if (!m_skipB) {
			_ASSERT(0);
			return 1;
		}
		m_rigidskip = new OWP_CheckBoxA(L"Valid/Invalid (有効/無効)", 1, labelheight, false);
		if (!m_rigidskip) {
			_ASSERT(0);
			return 1;
		}
		m_forbidSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_forbidSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_forbidB = new OWP_Button(L"ToAll", labelheight);
		if (!m_forbidB) {
			_ASSERT(0);
			return 1;
		}
		m_forbidrot = new OWP_CheckBoxA(L"ForbidRot", 0, labelheight, false);
		if (!m_forbidrot) {
			_ASSERT(0);
			return 1;
		}
		//m_allrigidenableB = new OWP_Button(L"ValidateAllRigids");
		//m_allrigiddisableB = new OWP_Button(L"InvalidateAllRigids");
		m_btgSlider = new OWP_Slider(-1.0, 1.0, -1.0, labelheight);
		if (!m_btgSlider) {
			_ASSERT(0);
			return 1;
		}
		//m_btgscSlider = new OWP_Slider(10.0, 100.0, 0.0);
		m_btgscSlider = new OWP_Slider(10.0, 200.0, 0.0, labelheight);//2024/04/15 200.0
		if (!m_btgscSlider) {
			_ASSERT(0);
			return 1;
		}
		m_btgB = new OWP_Button(L"GToAll", labelheight);
		if (!m_btgB) {
			_ASSERT(0);
			return 1;
		}
		m_btforceSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_btforceSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_btforce = new OWP_CheckBoxA(L"RigidBodySimulation", 0, labelheight, false);
		if (!m_btforce) {
			_ASSERT(0);
			return 1;
		}
		m_btforceB = new OWP_Button(L"ToAll", labelheight);
		if (!m_btforceB) {
			_ASSERT(0);
			return 1;
		}

		m_shplabel = new OWP_Label(L"Thickness", labelheight);
		if (!m_shplabel) {
			_ASSERT(0);
			return 1;
		}
		m_boxzlabel = new OWP_Label(L"Depth", labelheight);
		if (!m_boxzlabel) {
			_ASSERT(0);
			return 1;
		}
		m_massSLlabel = new OWP_Label(L"Mass", labelheight);
		if (!m_massSLlabel) {
			_ASSERT(0);
			return 1;
		}
		m_btglabel = new OWP_Label(L"Gravity", labelheight);
		if (!m_btglabel) {
			_ASSERT(0);
			return 1;
		}
		m_btgsclabel = new OWP_Label(L"ScaleOfGravity", labelheight);
		if (!m_btgsclabel) {
			_ASSERT(0);
			return 1;
		}

		m_namelabel = new OWP_Label(L"BonaName:????", labelheight);
		if (!m_namelabel) {
			_ASSERT(0);
			return 1;
		}

		m_lenglabel = new OWP_Label(L"BoneLength:*****[m]", labelheight);
		if (!m_lenglabel) {
			_ASSERT(0);
			return 1;
		}


		m_kB = new OWP_Button(L"SpringParamsToAll", labelheight);
		if (!m_kB) {
			_ASSERT(0);
			return 1;
		}
		m_restB = new OWP_Button(L"RestitutionAndFrictionToAll", labelheight);
		if (!m_restB) {
			_ASSERT(0);
			return 1;
		}

		m_colSeparator = new OWP_Separator(m_rigidWnd, true, 0.5, true);
		if (!m_colSeparator) {
			_ASSERT(0);
			return 1;
		}
		m_colB = new OWP_Button(L"ToAll", labelheight);
		if (!m_colB) {
			_ASSERT(0);
			return 1;
		}
		m_colradio = new OWP_RadioButton(L"Cone", limitradionamelen, labelheight);
		if (!m_colradio) {
			_ASSERT(0);
			return 1;
		}
		m_colradio->addLine(L"Capsule");
		m_colradio->addLine(L"Sphere");
		m_colradio->addLine(L"Rectangular");

		m_lkradio = new OWP_RadioButton(L"[posSpring]very weak", limitradionamelen, labelheight);
		if (!m_lkradio) {
			_ASSERT(0);
			return 1;
		}
		m_lkradio->addLine(L"[posSpring]weak");
		m_lkradio->addLine(L"[posSpring]regular");
		m_lkradio->addLine(L"[posSpring]custom");

		//m_lkSlider = new OWP_Slider(g_initcuslk, 1e6, 1e4);//60000
		//m_lkSlider = new OWP_Slider(g_initcuslk, 1e10, 1e8);//60000
		m_lkSlider = new OWP_Slider(g_initcuslk, 1e4, 1e2, labelheight);//60000
		if (!m_lkSlider) {
			_ASSERT(0);
			return 1;
		}
		m_lklabel = new OWP_Label(L"posSpring customValue", labelheight);
		if (!m_lklabel) {
			_ASSERT(0);
			return 1;
		}

		m_akradio = new OWP_RadioButton(L"[rotSpring]very weak", limitradionamelen, labelheight);
		if (!m_akradio) {
			_ASSERT(0);
			return 1;
		}
		m_akradio->addLine(L"[rotSpring]weak");
		m_akradio->addLine(L"[rotSpring]regular");
		m_akradio->addLine(L"[rotSpring]custom");

		//m_akSlider = new OWP_Slider(g_initcusak, 6000.0f, 0.0f);//300
		//m_akSlider = new OWP_Slider(g_initcusak, 30.0f, 0.0f);//300
		//m_akSlider = new OWP_Slider(g_initcusak, 3000.0f, 30.0f);//300
		//m_akSlider = new OWP_Slider(g_initcusak, 3000.0f, 10.0f);//300 ver10024
		//m_akSlider = new OWP_Slider(g_initcusak, 3000.0f, 2.0f);//2022/07/19
		m_akSlider = new OWP_Slider(g_initcusak, 500.0f, 0.0f, labelheight);//2024/04/15 柔らかい設定へシフト　硬い設定が必要になった場合にはScaleSliderを追加して対応予定
		if (!m_akSlider) {
			_ASSERT(0);
			return 1;
		}
		//m_akSlider = new OWP_Slider(g_initcusak, 1.0f, 0.0f);//2023/01/18
		m_aklabel = new OWP_Label(L"rotSpring customValue", labelheight);
		if (!m_aklabel) {
			_ASSERT(0);
			return 1;
		}

		m_restSlider = new OWP_Slider(0.5f, 1.0f, 0.0f, labelheight);
		if (!m_restSlider) {
			_ASSERT(0);
			return 1;
		}
		m_restlabel = new OWP_Label(L"RigidRestitution", labelheight);
		if (!m_restlabel) {
			_ASSERT(0);
			return 1;
		}
		m_fricSlider = new OWP_Slider(0.5f, 1.0f, 0.0f, labelheight);
		if (!m_fricSlider) {
			_ASSERT(0);
			return 1;
		}
		m_friclabel = new OWP_Label(L"RigidFriction", labelheight);
		if (!m_friclabel) {
			_ASSERT(0);
			return 1;
		}


		m_ldmplabel = new OWP_Label(L"[posSpring]rateOfDumping", labelheight);
		if (!m_ldmplabel) {
			_ASSERT(0);
			return 1;
		}
		m_admplabel = new OWP_Label(L"[rotSpring]rateOfDumping", labelheight);
		if (!m_admplabel) {
			_ASSERT(0);
			return 1;
		}
		m_ldmpSlider = new OWP_Slider(g_l_dmp, 1.0, 0.0, labelheight);
		if (!m_ldmpSlider) {
			_ASSERT(0);
			return 1;
		}
		m_admpSlider = new OWP_Slider(g_a_dmp, 1.0, 0.0, labelheight);
		if (!m_admpSlider) {
			_ASSERT(0);
			return 1;
		}
		m_dmpB = new OWP_Button(L"DumpingToAll", labelheight);
		if (!m_dmpB) {
			_ASSERT(0);
			return 1;
		}
		m_groupB = new OWP_Button(L"Conflict", labelheight);
		if (!m_groupB) {
			_ASSERT(0);
			return 1;
		}
		m_gcoliB = new OWP_Button(L"GConflict", labelheight);
		if (!m_gcoliB) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel01 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel01) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel02 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel02) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel03 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel03) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel04 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel04) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel05 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel05) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel06 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel06) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel07 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel07) {
			_ASSERT(0);
			return 1;
		}
		m_rigidspacerLabel08 = new OWP_Label(L"     ", labelheight);
		if (!m_rigidspacerLabel08) {
			_ASSERT(0);
			return 1;
		}




		COLORREF colorToAll = RGB(64, 128 + 32, 128 + 32);
		COLORREF colorValidInvalid = RGB(168, 129, 129);
		COLORREF colorToDeeper = RGB(24, 126, 176);

		m_groupcheck->setTextColor(colorToAll);
		m_massB->setTextColor(colorToAll);
		m_thicknessB->setTextColor(colorToAll);
		m_depthB->setTextColor(colorToAll);
		m_skipB->setTextColor(colorToAll);
		m_forbidB->setTextColor(colorToAll);
		m_btgB->setTextColor(colorToAll);
		m_btforceB->setTextColor(colorToAll);
		m_colB->setTextColor(colorToAll);
		m_dmpB->setTextColor(colorToAll);
		m_kB->setTextColor(colorToAll);
		m_restB->setTextColor(colorToAll);

		m_rigidskip->setTextColor(colorValidInvalid);
		m_btforce->setTextColor(colorValidInvalid);

		m_thicknessDeeperB->setTextColor(colorToDeeper);
		m_depthDeeperB->setTextColor(colorToDeeper);
		m_massDeeperB->setTextColor(colorToDeeper);
		m_validDeeperB->setTextColor(colorToDeeper);
		m_forbidDeeperB->setTextColor(colorToDeeper);
		m_colDeeperB->setTextColor(colorToDeeper);
		m_springDeeperB->setTextColor(colorToDeeper);
		m_restitutionDeeperB->setTextColor(colorToDeeper);
		m_dumpingDeeperB->setTextColor(colorToDeeper);
		m_gDeeperB->setTextColor(colorToDeeper);
		m_btforceDeeperB->setTextColor(colorToDeeper);
		m_coliidDeeperB->setTextColor(colorToDeeper);


		int slw = 350;

		m_sphrateSlider->setSize(WindowSize(slw, labelheight));
		m_boxzSlider->setSize(WindowSize(slw, labelheight));
		m_massSlider->setSize(WindowSize(slw, labelheight));
		m_btgSlider->setSize(WindowSize(slw, labelheight));
		m_btgscSlider->setSize(WindowSize(slw, labelheight));
		m_ldmpSlider->setSize(WindowSize(slw, labelheight));
		m_admpSlider->setSize(WindowSize(slw, labelheight));
		m_lkSlider->setSize(WindowSize(slw, labelheight));
		m_akSlider->setSize(WindowSize(slw, labelheight));
		m_restSlider->setSize(WindowSize(slw, labelheight));
		m_fricSlider->setSize(WindowSize(slw, labelheight));



		m_rigidspall->addParts1(*m_rigidsp0);
		m_rigidsp0->addParts1(*m_namelabel);
		m_rigidsp0->addParts2(*m_groupcheck);
		//m_rigidWnd->addParts(*m_namelabel);
		//m_rigidWnd->addParts(*m_groupcheck);
		m_rigidspall->addParts1(*m_rigidspacerLabel01);

		m_rigidspall->addParts1(*m_thicknessSeparator2);
		m_thicknessSeparator2->addParts1(*m_shplabel);
		m_thicknessSeparator2->addParts2(*m_sphrateSlider);
		m_rigidspall->addParts1(*m_thicknessSeparator);
		m_thicknessSeparator->addParts1(*m_thicknessB);
		m_thicknessSeparator->addParts2(*m_thicknessDeeperB);
		//m_rigidspall->addParts1(*m_thicknessB);

		m_rigidspall->addParts1(*m_depthSeparator2);
		m_depthSeparator2->addParts1(*m_boxzlabel);
		m_depthSeparator2->addParts2(*m_boxzSlider);
		m_rigidspall->addParts1(*m_depthSeparator);
		m_depthSeparator->addParts1(*m_depthB);
		m_depthSeparator->addParts2(*m_depthDeeperB);
		//m_rigidspall->addParts1(*m_depthB);

		m_rigidspall->addParts1(*m_massSeparator2);
		m_massSeparator2->addParts1(*m_massSLlabel);
		m_massSeparator2->addParts2(*m_massSlider);
		m_rigidspall->addParts1(*m_massSeparator);
		m_massSeparator->addParts1(*m_massB);
		m_massSeparator->addParts2(*m_massDeeperB);
		//m_rigidspall->addParts1(*m_massB);
		m_rigidspall->addParts1(*m_rigidspacerLabel02);

		////m_rigidspall->addParts1(*m_massB);
		////m_massB->setColor(64, 128, 128);
		////m_thicknessB->setColor(64, 128, 128);
		////m_depthB->setColor(64, 128, 128);
		//m_rigidspall->addParts1(*m_massSeparator);
		//m_massSeparator->addParts1(*m_massSeparator1);
		//m_massSeparator->addParts2(*m_massSeparator2);
		//m_massSeparator1->addParts1(*m_massB);
		//m_massSeparator1->addParts2(*m_thicknessB);
		//m_massSeparator2->addParts1(*m_depthB);
		////m_massSeparator2->addParts2(*m_massspacelabel);

		m_rigidspall->addParts1(*m_lenglabel);

		m_rigidspall->addParts1(*m_validSeparator);
		m_validSeparator->addParts1(*m_rigidskip);
		m_validSeparator->addParts2(*m_validSeparator2);
		m_validSeparator2->addParts1(*m_skipB);
		m_validSeparator2->addParts2(*m_validDeeperB);
		//m_validSeparator->addParts2(*m_skipB);
		////m_rigidspall->addParts1(*m_rigidskip);

		m_rigidspall->addParts1(*m_forbidSeparator);
		m_forbidSeparator->addParts1(*m_forbidrot);
		m_forbidSeparator->addParts2(*m_forbidSeparator2);
		m_forbidSeparator2->addParts1(*m_forbidB);
		m_forbidSeparator2->addParts2(*m_forbidDeeperB);
		//m_forbidSeparator->addParts2(*m_forbidB);
		////m_rigidspall->addParts1(*m_forbidrot);
		////m_rigidspall->addParts1(*m_allrigidenableB);
		////m_rigidspall->addParts1(*m_allrigiddisableB);
		m_rigidspall->addParts1(*m_rigidspacerLabel03);

		m_rigidspall->addParts1(*m_colSeparator);
		m_colSeparator->addParts1(*m_colradio);
		m_colSeparator->addParts2(*m_colSeparator2);
		m_colSeparator2->addParts1(*m_colB);
		m_colSeparator2->addParts2(*m_colDeeperB);
		//m_colSeparator->addParts2(*m_colB);
		////m_rigidspall->addParts1(*m_colradio);
		m_rigidspall->addParts1(*m_rigidspacerLabel04);

		m_rigidspall->addParts1(*m_lkradio);
		m_rigidspall->addParts1(*m_lklabel);
		m_rigidspall->addParts1(*m_lkSlider);
		m_rigidspall->addParts1(*m_akradio);
		m_rigidspall->addParts1(*m_aklabel);
		m_rigidspall->addParts1(*m_akSlider);
		m_rigidspall->addParts1(*m_springSeparator);
		m_springSeparator->addParts1(*m_kB);
		m_springSeparator->addParts2(*m_springDeeperB);
		//m_rigidspall->addParts1(*m_kB);
		m_rigidspall->addParts1(*m_rigidspacerLabel05);

		m_rigidspall->addParts1(*m_restlabel);
		m_rigidspall->addParts1(*m_restSlider);
		m_rigidspall->addParts1(*m_friclabel);
		m_rigidspall->addParts1(*m_fricSlider);
		m_rigidspall->addParts1(*m_restitutionSeparator);
		m_restitutionSeparator->addParts1(*m_restB);
		m_restitutionSeparator->addParts2(*m_restitutionDeeperB);
		//m_rigidspall->addParts1(*m_restB);
		m_rigidspall->addParts1(*m_rigidspacerLabel06);

		m_rigidspall->addParts1(*m_ldmplabel);
		m_rigidspall->addParts1(*m_ldmpSlider);
		m_rigidspall->addParts1(*m_admplabel);
		m_rigidspall->addParts1(*m_admpSlider);
		m_rigidspall->addParts1(*m_dumpingSeparator);
		m_dumpingSeparator->addParts1(*m_dmpB);
		m_dumpingSeparator->addParts2(*m_dumpingDeeperB);
		//m_rigidspall->addParts1(*m_dmpB);
		m_rigidspall->addParts1(*m_rigidspacerLabel07);

		m_rigidspall->addParts1(*m_btglabel);
		m_rigidspall->addParts1(*m_btgSlider);
		m_rigidspall->addParts1(*m_btgsclabel);
		m_rigidspall->addParts1(*m_btgscSlider);

		m_rigidspall->addParts1(*m_gSeparator);
		m_gSeparator->addParts1(*m_btgB);
		m_gSeparator->addParts2(*m_gDeeperB);
		//m_rigidspall->addParts1(*m_btgB);
		m_rigidspall->addParts1(*m_rigidspacerLabel08);

		//m_rigidspall->addParts1(*m_btforce);
		m_rigidspall->addParts1(*m_btforceSeparator);
		m_btforceSeparator->addParts1(*m_btforce);
		m_btforceSeparator->addParts2(*m_btforceSeparator2);
		m_btforceSeparator2->addParts1(*m_btforceB);
		m_btforceSeparator2->addParts2(*m_btforceDeeperB);
		//m_btforceSeparator->addParts2(*m_btforceB);

		m_rigidspall->addParts1(*m_coliidSeparator);
		//m_coliidSeparator->addParts1(*m_groupB);
		m_coliidSeparator->addParts1(*m_coliseparator);
		m_coliidSeparator->addParts2(*m_coliidDeeperB);
		m_coliseparator->addParts1(*m_groupB);
		m_coliseparator->addParts2(*m_gcoliB);
		//m_rigidspall->addParts1(*m_groupB);
		//m_rigidspall->addParts1(*m_gcoliB);
		/////////



		//m_dsrigidctrls.push_back(m_namelabel);
		//m_dsrigidctrls.push_back(m_groupcheck);
		//m_dsrigidctrls.push_back(m_shplabel);
		//m_dsrigidctrls.push_back(m_sphrateSlider);
		//m_dsrigidctrls.push_back(m_boxzlabel);
		//m_dsrigidctrls.push_back(m_boxzSlider);
		//m_dsrigidctrls.push_back(m_massSLlabel);
		//m_dsrigidctrls.push_back(m_massSlider);
		//m_dsrigidctrls.push_back(m_massB);
		//m_dsrigidctrls.push_back(m_thicknessB);
		//m_dsrigidctrls.push_back(m_depthB);
		//m_dsrigidctrls.push_back(m_lenglabel);
		//m_dsrigidctrls.push_back(m_rigidskip);
		//m_dsrigidctrls.push_back(m_skipB);
		//m_dsrigidctrls.push_back(m_forbidrot);
		//m_dsrigidctrls.push_back(m_forbidB);
		////m_dsrigidctrls.push_back(m_allrigidenableB);
		////m_dsrigidctrls.push_back(m_allrigiddisableB);
		//m_dsrigidctrls.push_back(m_colradio);
		//m_dsrigidctrls.push_back(m_colB);
		//m_dsrigidctrls.push_back(m_lkradio);
		//m_dsrigidctrls.push_back(m_lklabel);
		//m_dsrigidctrls.push_back(m_lkSlider);
		//m_dsrigidctrls.push_back(m_akradio);
		//m_dsrigidctrls.push_back(m_aklabel);
		//m_dsrigidctrls.push_back(m_akSlider);
		//m_dsrigidctrls.push_back(m_kB);
		//m_dsrigidctrls.push_back(m_restlabel);
		//m_dsrigidctrls.push_back(m_restSlider);
		//m_dsrigidctrls.push_back(m_friclabel);
		//m_dsrigidctrls.push_back(m_fricSlider);
		//m_dsrigidctrls.push_back(m_restB);
		//m_dsrigidctrls.push_back(m_ldmplabel);
		//m_dsrigidctrls.push_back(m_ldmpSlider);
		//m_dsrigidctrls.push_back(m_admplabel);
		//m_dsrigidctrls.push_back(m_admpSlider);
		//m_dsrigidctrls.push_back(m_dmpB);
		//m_dsrigidctrls.push_back(m_btglabel);
		//m_dsrigidctrls.push_back(m_btgSlider);
		//m_dsrigidctrls.push_back(m_btgsclabel);
		//m_dsrigidctrls.push_back(m_btgscSlider);
		//m_dsrigidctrls.push_back(m_btgB);
		//m_dsrigidctrls.push_back(m_btforce);
		//m_dsrigidctrls.push_back(m_btforceB);
		//m_dsrigidctrls.push_back(m_groupB);
		//m_dsrigidctrls.push_back(m_gcoliB);


		m_rigidWnd->setCloseListener([=, this]() {
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_RIGIDPARAMSDLG), (LPARAM)RIGIDPARAMSDLG_OPE_CLOSE);
			}
			});

		m_sphrateSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_sphrateSlider) {
						float val = (float)m_sphrateSlider->getValue();
						curre->SetSphrate(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_thicknessDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_sphrateSlider) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						float val = (float)m_sphrateSlider->getValue();
						int gid = -1;
						m_model->SetSphrateDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_boxzSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_boxzSlider) {
						float val = (float)m_boxzSlider->getValue();
						curre->SetBoxzrate(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_depthDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_boxzSlider) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						float val = (float)m_boxzSlider->getValue();
						int gid = -1;
						m_model->SetBoxzrateDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_massSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_massSlider) {
						float val = (float)m_massSlider->getValue();
						curre->SetMass(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_massDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_massSlider) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						float val = (float)m_massSlider->getValue();
						int gid = -1;
						m_model->SetMassDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});

		m_ldmpSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_ldmpSlider) {
						float val = (float)m_ldmpSlider->getValue();
						curre->SetLDamping(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_admpSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_admpSlider) {
						float val = (float)m_admpSlider->getValue();
						curre->SetADamping(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_dumpingDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						int gid = -1;
						if (m_ldmpSlider && m_admpSlider) {
							float ldmp = (float)m_ldmpSlider->getValue();
							float admp = (float)m_admpSlider->getValue();
							m_model->SetDmpDataReq(gid, m_reindexmap[m_model], curbone, ldmp, admp);
						}
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_lkSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_lkSlider) {
						float val = (float)m_lkSlider->getValue();
						curre->SetCusLk(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_akSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_akSlider) {
						float val = (float)m_akSlider->getValue();
						curre->SetCusAk(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});


		m_restSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_restSlider) {
						float val = (float)m_restSlider->getValue();
						curre->SetRestitution(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_restitutionDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd &&
				m_restSlider && m_fricSlider) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						float rest = (float)m_restSlider->getValue();
						float fric = (float)m_fricSlider->getValue();
						int gid = -1;
						m_model->SetRestDataReq(gid, m_reindexmap[m_model], curbone, rest, fric);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_fricSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_fricSlider) {
						float val = (float)m_fricSlider->getValue();
						curre->SetFriction(val);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_rigidskip->setButtonListener([=, this]() {
			if (m_model && m_rigidWnd) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					if (m_rigidskip) {
						bool validflag = m_rigidskip->getValue();
						if (validflag == false) {//!!!!!!!!GUIはValidでデータとしてはSkip
							curre->SetSkipflag(1);
						}
						else {
							curre->SetSkipflag(0);
						}
						m_model->SetBtObjectVec();//2024/06/16
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_validDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_rigidskip) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						//int val = m_rigidskip->getValue() ? 1 : 0;
						int val = m_rigidskip->getValue() ? 0 : 1;//!!!!!!!!GUIはValidでデータとしてはSkip
						int gid = -1;
						m_model->SetSkipflagDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_model->SetBtObjectVec();//2024/06/16
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_skipB->setButtonListener([=, this]() {
			if (m_model && m_rigidskip && m_groupcheck) {
				bool validflag = m_rigidskip->getValue();
				int skipflag;
				if (validflag == false) {
					skipflag = 1;
				}
				else {
					skipflag = 0;
				}
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllSkipflagData(gid, m_reindexmap[m_model], skipflag);
			}
			});

		m_forbidrot->setButtonListener([=, this]() {
			if (m_model && m_rigidWnd && m_forbidrot) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					bool validflag = m_forbidrot->getValue();
					if (validflag == false) {
						curre->SetForbidRotFlag(0);
					}
					else {
						curre->SetForbidRotFlag(1);
					}
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_forbidDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_forbidrot) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						int val = m_forbidrot->getValue() ? 1 : 0;
						int gid = -1;
						m_model->SetForbidrotDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_forbidB->setButtonListener([=, this]() {
			if (m_model && m_forbidrot && m_groupcheck) {
				bool validflag = m_forbidrot->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllForbidrotData(gid, m_reindexmap[m_model], validflag);
			}
			});

		//m_allrigidenableB->setButtonListener([=, this](){
		//	if (m_model){
		//		m_model->EnableAllRigidElem(m_reindexmap[m_model]);
		//	}
		//	m_rigidWnd->callRewrite();						//再描画
		//});
		//m_allrigiddisableB->setButtonListener([=, this](){
		//	if (m_model){
		//		m_model->DisableAllRigidElem(m_reindexmap[m_model]);
		//	}
		//	m_rigidWnd->callRewrite();						//再描画
		//});


		m_btforce->setButtonListener([=, this]() {
			if (m_model && (m_curboneno >= 0) && m_btforce) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						bool kinflag = m_btforce->getValue();
						if (kinflag == false) {
							parentbone->SetBtForce(0);
						}
						else {
							parentbone->SetBtForce(1);
						}
					}
				}
			}
			m_rigidWnd->callRewrite();						//再描画
			});
		m_btforceDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_btforce) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						int val = m_btforce->getValue() ? 1 : 0;
						int gid = -1;
						m_model->SetBtforceDataReq(m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_btgSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd && m_btgSlider) {
				float btg = (float)m_btgSlider->getValue();
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					curre->SetBtg(btg);
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_btgscSlider->setCursorListener([=, this]() {
			if (m_model && m_rigidWnd && m_btgscSlider) {
				float btgsc = (float)m_btgscSlider->getValue();
				if (m_model && (m_reindexmap[m_model] >= 0)) {
					REINFO tmpinfo = m_model->GetRigidElemInfo(m_reindexmap[m_model]);
					tmpinfo.btgscale = btgsc;
					m_model->SetRigidElemInfo(m_reindexmap[m_model], tmpinfo);
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_gDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_btgSlider) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						float val = (float)m_btgSlider->getValue();
						int gid = -1;
						m_model->SetBtgDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});

		m_colradio->setSelectListener([=, this]() {
			if (m_model && m_rigidWnd && m_colradio) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					int val = m_colradio->getSelectIndex();
					curre->SetColtype(val);
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_colDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd && m_colradio) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						int val = m_colradio->getSelectIndex();
						int gid = -1;
						m_model->SetColtypeDataReq(gid, m_reindexmap[m_model], curbone, val);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_colB->setButtonListener([=, this]() {
			if (m_model && m_colradio && m_groupcheck) {
				int val = m_colradio->getSelectIndex();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllColtypeData(gid, m_reindexmap[m_model], val);
			}
			});

		m_lkradio->setSelectListener([=, this]() {
			if (m_model && m_rigidWnd && m_lkradio) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					int val = m_lkradio->getSelectIndex();
					curre->SetLKindex(val);
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});
		m_springDeeperB->setButtonListener([=, this]() {
			if (m_model && (m_reindexmap[m_model] >= 0) && (m_curboneno >= 0) && m_rigidWnd &&
				m_lkradio && m_akradio && m_lkSlider && m_akSlider) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						int lindex = m_lkradio->getSelectIndex();
						int aindex = m_akradio->getSelectIndex();
						float cuslk = (float)m_lkSlider->getValue();
						float cusak = (float)m_akSlider->getValue();
						int gid = -1;
						m_model->SetKDataReq(gid, m_reindexmap[m_model], curbone, lindex, aindex, cuslk, cusak);
						m_rigidWnd->callRewrite();						//再描画
					}
				}
			}
			});
		m_akradio->setSelectListener([=, this]() {
			if (m_model && m_rigidWnd && m_akradio) {
				CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
				if (curre) {
					int val = m_akradio->getSelectIndex();
					curre->SetAKindex(val);
				}
				m_rigidWnd->callRewrite();						//再描画
			}
			});

		m_kB->setButtonListener([=, this]() {
			if (m_model && m_lkradio && m_akradio && m_lkSlider && m_akSlider && m_groupcheck) {
				int lindex = m_lkradio->getSelectIndex();
				int aindex = m_akradio->getSelectIndex();
				float cuslk = (float)m_lkSlider->getValue();
				float cusak = (float)m_akSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllKData(gid, m_reindexmap[m_model], lindex, aindex, cuslk, cusak);
			}
			});
		m_restB->setButtonListener([=, this]() {
			if (m_model && m_restSlider && m_fricSlider && m_groupcheck) {
				float rest = (float)m_restSlider->getValue();
				float fric = (float)m_fricSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllRestData(gid, m_reindexmap[m_model], rest, fric);
			}
			});
		m_dmpB->setButtonListener([=, this]() {
			if (m_model && m_ldmpSlider && m_admpSlider && m_groupcheck) {
				float ldmp = (float)m_ldmpSlider->getValue();
				float admp = (float)m_admpSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllDmpData(gid, m_reindexmap[m_model], ldmp, admp);
			}
			});

		m_groupB->setButtonListener([=, this]() {
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_RIGIDPARAMSDLG), (LPARAM)RIGIDPARAMSDLG_OPE_COLIDLG);
			}
			});
		m_coliidDeeperB->setButtonListener([=, this]() {
			if (m_model) {
				CBone* curbone = m_model->GetBoneByID(m_curboneno);
				if (curbone) {
					CBone* parentbone = curbone->GetParent(false);
					if (parentbone && parentbone->IsSkeleton()) {
						CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
						if (curre) {
							int groupid = curre->GetGroupid();
							int coliidsize = curre->GetColiidsSize();
							vector<int> coliids;
							int index1;
							for (index1 = 0; index1 < coliidsize; index1++) {
								coliids.push_back(curre->GetColiids(index1));
							}
							//int myselfflag = curre->GetMyselfflag();
							m_model->SetColiidDataReq(m_reindexmap[m_model], curbone, groupid, coliids);// , myselfflag);
						}
					}
				}
			}
			});
		m_gcoliB->setButtonListener([=, this]() {
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_RIGIDPARAMSDLG), (LPARAM)RIGIDPARAMSDLG_OPE_GCOLIDLG);
			}
		});

		m_massB->setButtonListener([=, this]() {
			if (m_model && m_massSlider && m_groupcheck) {
				float mass = (float)m_massSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllMassData(gid, m_reindexmap[m_model], mass);
			}
			//		_ASSERT( 0 );
			});
		m_thicknessB->setButtonListener([=, this]() {
			if (m_model && m_sphrateSlider && m_groupcheck) {
				float sphrate = (float)m_sphrateSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllSphrateData(gid, m_reindexmap[m_model], sphrate);
			}
			//		_ASSERT( 0 );
			});
		m_depthB->setButtonListener([=, this]() {
			if (m_model && m_boxzSlider && m_groupcheck) {
				float boxzrate = (float)m_boxzSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllBoxzrateData(gid, m_reindexmap[m_model], boxzrate);
			}
			//		_ASSERT( 0 );
			});
		m_btgB->setButtonListener([=, this]() {
			if (m_model && m_btgSlider && m_groupcheck) {
				float btg = (float)m_btgSlider->getValue();
				int chkg = (int)m_groupcheck->getValue();
				int gid = -1;
				if (chkg) {
					CRigidElem* curre = m_model->GetRigidElem(m_curboneno);
					if (curre) {
						gid = curre->GetGroupid();
					}
					else {
						gid = -1;
					}
				}
				m_model->SetAllBtgData(gid, m_reindexmap[m_model], btg);
			}
			});
		m_btforceB->setButtonListener([=, this]() {
			if (m_model && m_btforce) {
				bool kinflag = m_btforce->getValue();
				m_model->SetAllBtforceData(m_reindexmap[m_model], kinflag);
			}
			});



		//m_rigidSc->autoResize();

		m_rigidWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_rigidWnd->setPos(WindowPos(m_posx, m_posy));

		//１クリック目問題対応
		m_rigidWnd->refreshPosAndSize();//2022/09/20


		m_rigidWnd->callRewrite();						//再描画
		m_rigidWnd->setVisible(false);

		//m_rcrigidwnd.top = m_sidemenuheight;
		//m_rcrigidwnd.left = 0;

		//m_rcrigidwnd.bottom = m_sideheight;
		//m_rcrigidwnd.right = m_sidewidth;

	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

const HWND CRigidParamsDlg::GetHWnd()
{
	if (!m_rigidWnd) {
		return NULL;
	}
	else {
		return m_rigidWnd->getHWnd();
	}
}

const bool CRigidParamsDlg::GetVisible()
{
	if (!m_rigidWnd) {
		return false;
	}
	else {
		return m_rigidWnd->getVisible();
	}
}
void CRigidParamsDlg::ListenMouse(bool srcflag)
{
	if (!m_rigidWnd) {
		return;
	}
	else {
		m_rigidWnd->setListenMouse(srcflag);
	}
}

