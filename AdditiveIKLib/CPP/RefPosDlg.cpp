#include "stdafx.h"

#include <RefPosDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
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
// CRefPosDlg

CRefPosDlg::CRefPosDlg()
{
	InitParams();
}

CRefPosDlg::~CRefPosDlg()
{
	DestroyObjs();
}
	
int CRefPosDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_refposnumLabel) {
		delete m_refposnumLabel;
		m_refposnumLabel = nullptr;
	}
	if (m_refposnumSlider) {
		delete m_refposnumSlider;
		m_refposnumSlider = nullptr;
	}
	if (m_refposnumsp) {
		delete m_refposnumsp;
		m_refposnumsp = nullptr;
	}


	if (m_diffuseRLabel) {
		delete m_diffuseRLabel;
		m_diffuseRLabel = nullptr;
	}
	if (m_diffuseRSlider) {
		delete m_diffuseRSlider;
		m_diffuseRSlider = nullptr;
	}
	if (m_diffuseRsp) {
		delete m_diffuseRsp;
		m_diffuseRsp = nullptr;
	}

	if (m_diffuseGLabel) {
		delete m_diffuseGLabel;
		m_diffuseGLabel = nullptr;
	}
	if (m_diffuseGSlider) {
		delete m_diffuseGSlider;
		m_diffuseGSlider = nullptr;
	}
	if (m_diffuseGsp) {
		delete m_diffuseGsp;
		m_diffuseGsp = nullptr;
	}

	if (m_diffuseBLabel) {
		delete m_diffuseBLabel;
		m_diffuseBLabel = nullptr;
	}
	if (m_diffuseBSlider) {
		delete m_diffuseBSlider;
		m_diffuseBSlider = nullptr;
	}
	if (m_diffuseBsp) {
		delete m_diffuseBsp;
		m_diffuseBsp = nullptr;
	}

	if (m_diffuseALabel) {
		delete m_diffuseALabel;
		m_diffuseALabel = nullptr;
	}
	if (m_diffuseASlider) {
		delete m_diffuseASlider;
		m_diffuseASlider = nullptr;
	}
	if (m_diffuseAsp) {
		delete m_diffuseAsp;
		m_diffuseAsp = nullptr;
	}

	if (m_rainbowLabel) {
		delete m_rainbowLabel;
		m_rainbowLabel = nullptr;
	}
	if (m_rainbowcheck) {
		delete m_rainbowcheck;
		m_rainbowcheck = nullptr;
	}
	if (m_rainbowsp) {
		delete m_rainbowsp;
		m_rainbowsp = nullptr;
	}

	if (m_rainbowinvLabel) {
		delete m_rainbowinvLabel;
		m_rainbowinvLabel = nullptr;
	}
	if (m_rainbowinvcheck) {
		delete m_rainbowinvcheck;
		m_rainbowinvcheck = nullptr;
	}
	if (m_rainbowinvsp) {
		delete m_rainbowinvsp;
		m_rainbowinvsp = nullptr;
	}

	if (m_rainbowtimeLabel) {
		delete m_rainbowtimeLabel;
		m_rainbowtimeLabel = nullptr;
	}
	if (m_rainbowtimecheck) {
		delete m_rainbowtimecheck;
		m_rainbowtimecheck = nullptr;
	}
	if (m_rainbowtimesp) {
		delete m_rainbowtimesp;
		m_rainbowtimesp = nullptr;
	}


	if (m_nameLabel) {
		delete m_nameLabel;
		m_nameLabel = nullptr;
	}
	if (m_space01Label) {
		delete m_space01Label;
		m_space01Label = nullptr;
	}
	if (m_space02Label) {
		delete m_space02Label;
		m_space02Label = nullptr;
	}
	if (m_space03Label) {
		delete m_space03Label;
		m_space03Label = nullptr;
	}

	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CRefPosDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;
	m_dlgWnd = nullptr;

	//m_PrepairUndo = nullptr;
	//m_UpdateAfterEditAngleLimit = nullptr;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;
	m_refposnum = 1;
	m_diffuserate = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_rainbowmode = false;

	m_dlgWnd = nullptr;

	m_refposnumsp = nullptr;
	m_refposnumLabel = nullptr;
	m_refposnumSlider = nullptr;

	m_diffuseRsp = nullptr;
	m_diffuseRLabel = nullptr;
	m_diffuseRSlider = nullptr;

	m_diffuseGsp = nullptr;
	m_diffuseGLabel = nullptr;
	m_diffuseGSlider = nullptr;

	m_diffuseBsp = nullptr;
	m_diffuseBLabel = nullptr;
	m_diffuseBSlider = nullptr;

	m_diffuseAsp = nullptr;
	m_diffuseALabel = nullptr;
	m_diffuseASlider = nullptr;

	m_rainbowsp = nullptr;
	m_rainbowLabel = nullptr;
	m_rainbowcheck = nullptr;

	m_rainbowinvsp = nullptr;
	m_rainbowinvLabel = nullptr;
	m_rainbowinvcheck = nullptr;

	m_rainbowtimesp = nullptr;
	m_rainbowtimeLabel = nullptr;
	m_rainbowtimecheck = nullptr;

	m_nameLabel = nullptr;
	m_space01Label = nullptr;
	m_space02Label = nullptr;
	m_space03Label = nullptr;

}

int CRefPosDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CRefPosDlg::SetFunctions(
//	void (*srcPrepairUndo)(),
//	int (*srcUpdateAfterEditAngleLimit)(int limit2boneflag, bool setcursorflag)
//)
//{
//	if (!srcPrepairUndo || !srcUpdateAfterEditAngleLimit) {
//		_ASSERT(0);
//		return 1;
//	}
//	m_PrepairUndo = srcPrepairUndo;
//	m_UpdateAfterEditAngleLimit = srcUpdateAfterEditAngleLimit;
//
//	return 0;
//}

void CRefPosDlg::SetModel(CModel* srcmodel)
{
	m_model = srcmodel;
	CreateRefPosWnd();//作成済はリターン

	Params2Dlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_st_Sc) {
		//	m_st_Sc->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CRefPosDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			Params2Dlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			//if (m_st_Sc) {
			//	//############
			//	//2024/07/24
			//	//############
			//	//int showposline = m_dlgSc->getShowPosLine();
			//	//m_dlgSc->setShowPosLine(showposline);
			//	//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
			//	//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
			//	//スクロールバーを少し動かすと全て表示された
			//	//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
			//	m_st_Sc->autoResize();
			//}
			m_dlgWnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgWnd) {
			m_dlgWnd->setVisible(false);
			m_dlgWnd->setListenMouse(false);

			//m_underanglelimithscroll = 0;
		}
	}
	m_visible = srcflag;
}


int CRefPosDlg::CreateRefPosWnd()
{
	//m_underanglelimithscroll = 0;

	if (m_dlgWnd) {
		return 0;//既に作成済　０リターン
	}

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}


	m_dlgWnd = new OrgWindow(
		0,
		_T("RefPosDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("RefPosDlg"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		false,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否

	int labelheight;
	int labelheightL;
	if (g_4kresolution) {
		labelheight = 28;
		labelheightL = 36;
	}
	else {
		labelheight = 20;
		labelheightL = 28;
	}



	if (m_dlgWnd) {
		m_dlgWnd->setListenMouse(true);

		double rate1 = 0.350;
		double rate50 = 0.50;
		double diffuseratemax = 8.0;

		m_nameLabel = new OWP_Label(m_model->GetFileName(), labelheightL);
		if (!m_nameLabel) {
			_ASSERT(0);
			abort();
		}
		m_space01Label = new OWP_Label(L"  ", labelheightL);
		if (!m_space01Label) {
			_ASSERT(0);
			abort();
		}
		m_space02Label = new OWP_Label(L"  ", labelheightL);
		if (!m_space02Label) {
			_ASSERT(0);
			abort();
		}
		m_space03Label = new OWP_Label(L"  ", labelheightL);
		if (!m_space03Label) {
			_ASSERT(0);
			abort();
		}


		m_refposnumLabel = new OWP_Label(L"RefPos Num", labelheight);
		if (!m_refposnumLabel) {
			_ASSERT(0);
			abort();
		}
		m_refposnumsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_refposnumsp) {
			_ASSERT(0);
			abort();
		}
		m_refposnumSlider = new OWP_Slider((double)m_model->GetRefPosNum(), REFPOSMAXNUM, 1.0);
		if (!m_refposnumSlider) {
			_ASSERT(0);
			abort();
		}

		m_diffuseRLabel = new OWP_Label(L"Red rate", labelheight);
		if (!m_diffuseRLabel) {
			_ASSERT(0);
			abort();
		}
		m_diffuseRsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_diffuseRsp) {
			_ASSERT(0);
			abort();
		}
		m_diffuseRSlider = new OWP_Slider((double)m_model->GetRefPosDiffuseRate().x, diffuseratemax, 0.0);
		if (!m_diffuseRSlider) {
			_ASSERT(0);
			abort();
		}

		m_diffuseGLabel = new OWP_Label(L"Green rate", labelheight);
		if (!m_diffuseGLabel) {
			_ASSERT(0);
			abort();
		}
		m_diffuseGsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_diffuseGsp) {
			_ASSERT(0);
			abort();
		}
		m_diffuseGSlider = new OWP_Slider((double)m_model->GetRefPosDiffuseRate().y, diffuseratemax, 0.0);
		if (!m_diffuseGSlider) {
			_ASSERT(0);
			abort();
		}

		m_diffuseBLabel = new OWP_Label(L"Blue rate", labelheight);
		if (!m_diffuseBLabel) {
			_ASSERT(0);
			abort();
		}
		m_diffuseBsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_diffuseBsp) {
			_ASSERT(0);
			abort();
		}
		m_diffuseBSlider = new OWP_Slider((double)m_model->GetRefPosDiffuseRate().z, diffuseratemax, 0.0);
		if (!m_diffuseBSlider) {
			_ASSERT(0);
			abort();
		}

		m_diffuseALabel = new OWP_Label(L"Alpha rate", labelheight);
		if (!m_diffuseALabel) {
			_ASSERT(0);
			abort();
		}
		m_diffuseAsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_diffuseAsp) {
			_ASSERT(0);
			abort();
		}
		m_diffuseASlider = new OWP_Slider((double)m_model->GetRefPosDiffuseRate().w, 1.0, 0.0);
		if (!m_diffuseASlider) {
			_ASSERT(0);
			abort();
		}

		m_rainbowLabel = new OWP_Label(L"RainbowMode", labelheight);
		if (!m_rainbowLabel) {
			_ASSERT(0);
			abort();
		}
		m_rainbowsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rainbowsp) {
			_ASSERT(0);
			abort();
		}
		m_rainbowcheck = new OWP_CheckBoxA(L"RainbowMode", m_model->GetRefPosRainbowMode(), labelheight, false);
		if (!m_rainbowcheck) {
			_ASSERT(0);
			abort();
		}

		m_rainbowinvLabel = new OWP_Label(L"Rainbow Inverse", labelheight);
		if (!m_rainbowinvLabel) {
			_ASSERT(0);
			abort();
		}
		m_rainbowinvsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rainbowinvsp) {
			_ASSERT(0);
			abort();
		}
		m_rainbowinvcheck = new OWP_CheckBoxA(L"Rainbow Inverse", m_model->GetRefPosRainbowInv(), labelheight, false);
		if (!m_rainbowinvcheck) {
			_ASSERT(0);
			abort();
		}


		m_rainbowtimeLabel = new OWP_Label(L"Rainbow TimeProc", labelheight);
		if (!m_rainbowtimeLabel) {
			_ASSERT(0);
			abort();
		}
		m_rainbowtimesp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_rainbowtimesp) {
			_ASSERT(0);
			abort();
		}
		m_rainbowtimecheck = new OWP_CheckBoxA(L"Rainbow TimeProc", m_model->GetRefPosRainbowTime(), labelheight, false);
		if (!m_rainbowtimecheck) {
			_ASSERT(0);
			abort();
		}

		m_dlgWnd->addParts(*m_nameLabel);
		m_dlgWnd->addParts(*m_space01Label);

		m_dlgWnd->addParts(*m_refposnumsp);
		m_refposnumsp->addParts1(*m_refposnumLabel);
		m_refposnumsp->addParts2(*m_refposnumSlider);
		m_dlgWnd->addParts(*m_space02Label);

		m_dlgWnd->addParts(*m_diffuseRsp);
		m_diffuseRsp->addParts1(*m_diffuseRLabel);
		m_diffuseRsp->addParts2(*m_diffuseRSlider);

		m_dlgWnd->addParts(*m_diffuseGsp);
		m_diffuseGsp->addParts1(*m_diffuseGLabel);
		m_diffuseGsp->addParts2(*m_diffuseGSlider);

		m_dlgWnd->addParts(*m_diffuseBsp);
		m_diffuseBsp->addParts1(*m_diffuseBLabel);
		m_diffuseBsp->addParts2(*m_diffuseBSlider);

		m_dlgWnd->addParts(*m_diffuseAsp);
		m_diffuseAsp->addParts1(*m_diffuseALabel);
		m_diffuseAsp->addParts2(*m_diffuseASlider);

		m_dlgWnd->addParts(*m_space03Label);

		m_dlgWnd->addParts(*m_rainbowsp);
		m_rainbowsp->addParts1(*m_rainbowLabel);
		m_rainbowsp->addParts2(*m_rainbowcheck);

		m_dlgWnd->addParts(*m_rainbowinvsp);
		m_rainbowsp->addParts1(*m_rainbowinvLabel);
		m_rainbowsp->addParts2(*m_rainbowinvcheck);

		m_dlgWnd->addParts(*m_rainbowtimesp);
		m_rainbowsp->addParts1(*m_rainbowtimeLabel);
		m_rainbowsp->addParts2(*m_rainbowtimecheck);


		//##########
		//Slider
		//##########
		m_refposnumSlider->setCursorListener([=, this]() {
			double value = m_refposnumSlider->getValue();
			int setvalue = (int)(value + 0.0001);
			if (m_model != nullptr) {
				m_model->SetRefPosNum(setvalue);
			}
			m_refposnumSlider->setValue((double)setvalue, false);//intに丸めてセットし直し
			});

		m_diffuseRSlider->setCursorListener([=, this]() {
			double value = m_diffuseRSlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentrate = m_model->GetRefPosDiffuseRate();
				ChaVector4 newrate = currentrate;
				newrate.x = (float)value;
				m_model->SetRefPosDiffuseRate(newrate);
			}
			});
		m_diffuseGSlider->setCursorListener([=, this]() {
			double value = m_diffuseGSlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentrate = m_model->GetRefPosDiffuseRate();
				ChaVector4 newrate = currentrate;
				newrate.y = (float)value;
				m_model->SetRefPosDiffuseRate(newrate);
			}
			});
		m_diffuseBSlider->setCursorListener([=, this]() {
			double value = m_diffuseBSlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentrate = m_model->GetRefPosDiffuseRate();
				ChaVector4 newrate = currentrate;
				newrate.z = (float)value;
				m_model->SetRefPosDiffuseRate(newrate);
			}
			});
		m_diffuseASlider->setCursorListener([=, this]() {
			double value = m_diffuseASlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentrate = m_model->GetRefPosDiffuseRate();
				ChaVector4 newrate = currentrate;
				newrate.w = (float)value;
				m_model->SetRefPosDiffuseRate(newrate);
			}
			});

		//########
		//Check
		//########
		m_rainbowcheck->setButtonListener([=, this]() {
			bool value = m_rainbowcheck->getValue();
			if (m_model != nullptr) {
				m_model->SetRefPosRainbowMode(value);
			}
			});
		m_rainbowinvcheck->setButtonListener([=, this]() {
			bool value = m_rainbowinvcheck->getValue();
			if (m_model != nullptr) {
				m_model->SetRefPosRainbowInv(value);
			}
			});
		m_rainbowtimecheck->setButtonListener([=, this]() {
			bool value = m_rainbowtimecheck->getValue();
			if (m_model != nullptr) {
				m_model->SetRefPosRainbowTime(value);
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

int CRefPosDlg::Params2Dlg()
{
	if ((m_dlgWnd != nullptr) && (m_model != nullptr)) {
		if (m_nameLabel) {
			m_nameLabel->setName(m_model->GetFileName());
		}

		int refposnum = m_model->GetRefPosNum();
		if (m_refposnumSlider != nullptr) {
			int setvalue = (int)(refposnum + 0.0001);
			setvalue = min(REFPOSMAXNUM, setvalue);
			setvalue = max(1, setvalue);
			m_refposnumSlider->setValue((double)setvalue, false);//intに丸めてセットし直し
		}

		ChaVector4 diffuserate = m_model->GetRefPosDiffuseRate();
		if (m_diffuseRSlider != nullptr) {
			m_diffuseRSlider->setValue((double)diffuserate.x, false);
		}
		if (m_diffuseGSlider != nullptr) {
			m_diffuseGSlider->setValue((double)diffuserate.y, false);
		}
		if (m_diffuseBSlider != nullptr) {
			m_diffuseBSlider->setValue((double)diffuserate.z, false);
		}
		if (m_diffuseASlider != nullptr) {
			m_diffuseASlider->setValue((double)diffuserate.w, false);
		}

		bool rainbowmode = m_model->GetRefPosRainbowMode();
		if (m_rainbowcheck != nullptr) {
			m_rainbowcheck->setValue(rainbowmode, false);
		}
		bool rainbowinv = m_model->GetRefPosRainbowInv();
		if (m_rainbowinvcheck != nullptr) {
			m_rainbowinvcheck->setValue(rainbowinv, false);
		}
		bool rainbowtime = m_model->GetRefPosRainbowTime();
		if (m_rainbowtimecheck != nullptr) {
			m_rainbowtimecheck->setValue(rainbowtime, false);
		}

		m_dlgWnd->callRewrite();
	}
	else {
		//_ASSERT(0);
		int dbgflag1 = 1;
	}

	return 0;
}



const HWND CRefPosDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CRefPosDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CRefPosDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

void CRefPosDlg::CallRewrite()
{
	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		m_dlgWnd->callRewrite();
	}
}
