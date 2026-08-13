#include "stdafx.h"

#include <GrassDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <ChaScene.h>
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
extern GRASSMOVER g_grassmover;

/////////////////////////////////////////////////////////////////////////////
// CGrassDlg

CGrassDlg::CGrassDlg()
{
	InitParams();
}

CGrassDlg::~CGrassDlg()
{
	DestroyObjs();
}
	
int CGrassDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_diffuseLabel) {
		delete m_diffuseLabel;
		m_diffuseLabel = nullptr;
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



	if (m_shapescaleLabel) {
		delete m_shapescaleLabel;
		m_shapescaleLabel = nullptr;
	}
	if (m_shapescaleXLabel) {
		delete m_shapescaleXLabel;
		m_shapescaleXLabel = nullptr;
	}
	if (m_shapescaleXSlider) {
		delete m_shapescaleXSlider;
		m_shapescaleXSlider = nullptr;
	}
	if (m_shapescaleXsp) {
		delete m_shapescaleXsp;
		m_shapescaleXsp = nullptr;
	}
	if (m_shapescaleYLabel) {
		delete m_shapescaleYLabel;
		m_shapescaleYLabel = nullptr;
	}
	if (m_shapescaleYSlider) {
		delete m_shapescaleYSlider;
		m_shapescaleYSlider = nullptr;
	}
	if (m_shapescaleYsp) {
		delete m_shapescaleYsp;
		m_shapescaleYsp = nullptr;
	}
	if (m_shapescaleZLabel) {
		delete m_shapescaleZLabel;
		m_shapescaleZLabel = nullptr;
	}
	if (m_shapescaleZSlider) {
		delete m_shapescaleZSlider;
		m_shapescaleZSlider = nullptr;
	}
	if (m_shapescaleZsp) {
		delete m_shapescaleZsp;
		m_shapescaleZsp = nullptr;
	}


	if (m_bendscaleLabel) {
		delete m_bendscaleLabel;
		m_bendscaleLabel = nullptr;
	}
	if (m_bendscaleLabel) {
		delete m_bendscaleLabel;
		m_bendscaleLabel = nullptr;
	}
	if (m_bendscaleSlider) {
		delete m_bendscaleSlider;
		m_bendscaleSlider = nullptr;
	}
	if (m_bendscalesp) {
		delete m_bendscalesp;
		m_bendscalesp = nullptr;
	}


	if (m_moverLabel) {
		delete m_moverLabel;
		m_moverLabel = nullptr;
	}
	if (m_moverCombo) {
		delete m_moverCombo;
		m_moverCombo = nullptr;
	}
	if (m_moversp) {
		delete m_moversp;
		m_moversp = nullptr;
	}
	if (m_moverRLabel) {
		delete m_moverRLabel;
		m_moverRLabel = nullptr;
	}
	if (m_moverRSlider) {
		delete m_moverRSlider;
		m_moverRSlider = nullptr;
	}
	if (m_moverRsp) {
		delete m_moverRsp;
		m_moverRsp = nullptr;
	}



	if (m_nameLabel) {
		delete m_nameLabel;
		m_nameLabel = nullptr;
	}
	if (m_grassLabel) {
		delete m_grassLabel;
		m_grassLabel = nullptr;
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
	if (m_space04Label) {
		delete m_space04Label;
		m_space04Label = nullptr;
	}
	if (m_space05Label) {
		delete m_space05Label;
		m_space05Label = nullptr;
	}
	if (m_space06Label) {
		delete m_space06Label;
		m_space06Label = nullptr;
	}
	if (m_space07Label) {
		delete m_space07Label;
		m_space07Label = nullptr;
	}

	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CGrassDlg::InitParams()
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
	m_diffuserate = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_shapescale = ChaVector3(1.0f, 1.0f, 1.0f);
	m_bendscale = 1.0f;

	m_dlgWnd = nullptr;


	m_diffuseLabel = nullptr;
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

	m_shapescaleLabel = nullptr;
	m_shapescaleXsp = nullptr;
	m_shapescaleXLabel = nullptr;
	m_shapescaleXSlider = nullptr;
	m_shapescaleYsp = nullptr;
	m_shapescaleYLabel = nullptr;
	m_shapescaleYSlider = nullptr;
	m_shapescaleZsp = nullptr;
	m_shapescaleZLabel = nullptr;
	m_shapescaleZSlider = nullptr;

	m_bendscaleLabel = nullptr;
	m_bendscalesp = nullptr;
	m_bendscaleLabel = nullptr;
	m_bendscaleSlider = nullptr;


	m_moversp = nullptr;
	m_moverLabel = nullptr;
	m_moverCombo = nullptr;
	m_moverRsp = nullptr;
	m_moverRLabel = nullptr;
	m_moverRSlider = nullptr;


	m_nameLabel = nullptr;
	m_grassLabel = nullptr;
	m_space01Label = nullptr;
	m_space02Label = nullptr;
	m_space03Label = nullptr;
	m_space04Label = nullptr;
	m_space05Label = nullptr;
	m_space06Label = nullptr;
	m_space07Label = nullptr;

}

int CGrassDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//int CGrassDlg::SetFunctions(
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

void CGrassDlg::SetModel(ChaScene* srcchascene, CModel* srcmodel)
{
	m_chascene = srcchascene;
	m_model = srcmodel;
	CreateGrassWnd();//作成済はリターン

	Params2Dlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_st_Sc) {
		//	m_st_Sc->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CGrassDlg::SetVisible(bool srcflag)
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


int CGrassDlg::CreateGrassWnd()
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
		_T("GrassDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("GrassDlg"),	//タイトル
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
		labelheightL = 22;
	}



	if (m_dlgWnd) {
		m_dlgWnd->setListenMouse(true);

		double rate1 = 0.350;
		double rate50 = 0.50;
		double diffuseratemax = 20.0;
		double shapescalemax = 20.0;
		double bendscalemax = 20.0;


		m_nameLabel = new OWP_Label(m_model->GetFileName(), labelheightL);
		if (!m_nameLabel) {
			_ASSERT(0);
			abort();
		}
		m_grassLabel = new OWP_Label(L"   ", labelheightL);
		if (!m_grassLabel) {
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
		m_space04Label = new OWP_Label(L"  ", labelheightL);
		if (!m_space04Label) {
			_ASSERT(0);
			abort();
		}
		m_space05Label = new OWP_Label(L"--- Common Settings Below ---", labelheightL);
		if (!m_space05Label) {
			_ASSERT(0);
			abort();
		}
		m_space06Label = new OWP_Label(L"  ", labelheightL);
		if (!m_space06Label) {
			_ASSERT(0);
			abort();
		}
		m_space07Label = new OWP_Label(L"  ", labelheightL);
		if (!m_space07Label) {
			_ASSERT(0);
			abort();
		}


		m_diffuseLabel = new OWP_Label(L"DiffuseRate of Grass", labelheightL);
		if (!m_diffuseLabel) {
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
		m_diffuseRSlider = new OWP_Slider((double)m_model->GetGrassDiffuseRate().x, diffuseratemax, 0.0);
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
		m_diffuseGSlider = new OWP_Slider((double)m_model->GetGrassDiffuseRate().y, diffuseratemax, 0.0);
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
		m_diffuseBSlider = new OWP_Slider((double)m_model->GetGrassDiffuseRate().z, diffuseratemax, 0.0);
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
		m_diffuseASlider = new OWP_Slider((double)m_model->GetGrassDiffuseRate().w, 1.0, 0.0);
		if (!m_diffuseASlider) {
			_ASSERT(0);
			abort();
		}



		m_shapescaleLabel = new OWP_Label(L"ShapeScale of Grass", labelheightL);
		if (!m_shapescaleLabel) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleXLabel = new OWP_Label(L"X Scale", labelheight);
		if (!m_shapescaleXLabel) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleXsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shapescaleXsp) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleXSlider = new OWP_Slider((double)m_model->GetGrassShapeScale().x, shapescalemax, 0.0);
		if (!m_shapescaleXSlider) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleYLabel = new OWP_Label(L"Y Scale", labelheight);
		if (!m_shapescaleYLabel) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleYsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shapescaleYsp) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleYSlider = new OWP_Slider((double)m_model->GetGrassShapeScale().y, shapescalemax, 0.0);
		if (!m_shapescaleYSlider) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleZLabel = new OWP_Label(L"Z Scale", labelheight);
		if (!m_shapescaleZLabel) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleZsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_shapescaleZsp) {
			_ASSERT(0);
			abort();
		}
		m_shapescaleZSlider = new OWP_Slider((double)m_model->GetGrassShapeScale().z, shapescalemax, 0.0);
		if (!m_shapescaleZSlider) {
			_ASSERT(0);
			abort();
		}


		m_bendscaleLabel = new OWP_Label(L"BendScale of Grass", labelheightL);
		if (!m_bendscaleLabel) {
			_ASSERT(0);
			abort();
		}
		m_bendscaleLabel = new OWP_Label(L"Bend Scale", labelheight);
		if (!m_bendscaleLabel) {
			_ASSERT(0);
			abort();
		}
		m_bendscalesp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_bendscalesp) {
			_ASSERT(0);
			abort();
		}
		m_bendscaleSlider = new OWP_Slider((double)m_model->GetGrassBendScale(), bendscalemax, 0.0);
		if (!m_bendscaleSlider) {
			_ASSERT(0);
			abort();
		}


		m_moverLabel = new OWP_Label(L"MoverModel", labelheight);
		if (!m_moverLabel) {
			_ASSERT(0);
			abort();
		}
		m_moversp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_moversp) {
			_ASSERT(0);
			abort();
		}
		m_moverCombo = new OWP_ComboBoxA(L"GrassMoverCombo", labelheight);
		if (!m_moverCombo) {
			_ASSERT(0);
			abort();
		}
		m_moverRLabel = new OWP_Label(L"Mover Radius", labelheight);
		if (!m_moverRLabel) {
			_ASSERT(0);
			abort();
		}
		m_moverRsp = new OWP_Separator(m_dlgWnd, true, rate1, true);
		if (!m_moverRsp) {
			_ASSERT(0);
			abort();
		}
		m_moverRSlider = new OWP_Slider((double)g_grassmover.mover_r, 500.0, 0.0);
		if (!m_moverRSlider) {
			_ASSERT(0);
			abort();
		}

		m_dlgWnd->addParts(*m_nameLabel);
		m_dlgWnd->addParts(*m_grassLabel);
		m_dlgWnd->addParts(*m_space01Label);

		m_dlgWnd->addParts(*m_diffuseLabel);
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

		m_dlgWnd->addParts(*m_space06Label);


		m_dlgWnd->addParts(*m_shapescaleLabel);
		m_dlgWnd->addParts(*m_shapescaleXsp);
		m_shapescaleXsp->addParts1(*m_shapescaleXLabel);
		m_shapescaleXsp->addParts2(*m_shapescaleXSlider);
		m_dlgWnd->addParts(*m_shapescaleYsp);
		m_shapescaleYsp->addParts1(*m_shapescaleYLabel);
		m_shapescaleYsp->addParts2(*m_shapescaleYSlider);
		m_dlgWnd->addParts(*m_shapescaleZsp);
		m_shapescaleZsp->addParts1(*m_shapescaleZLabel);
		m_shapescaleZsp->addParts2(*m_shapescaleZSlider);


		m_dlgWnd->addParts(*m_space07Label);


		m_dlgWnd->addParts(*m_bendscaleLabel);
		m_dlgWnd->addParts(*m_bendscalesp);
		m_bendscalesp->addParts1(*m_bendscaleLabel);
		m_bendscalesp->addParts2(*m_bendscaleSlider);

		m_dlgWnd->addParts(*m_space03Label);
		m_dlgWnd->addParts(*m_space05Label);//common settings
		m_dlgWnd->addParts(*m_space02Label);

		m_dlgWnd->addParts(*m_moversp);
		m_moversp->addParts1(*m_moverLabel);
		m_moversp->addParts2(*m_moverCombo);
		m_dlgWnd->addParts(*m_space01Label);
		m_dlgWnd->addParts(*m_moverRsp);
		m_moverRsp->addParts1(*m_moverRLabel);
		m_moverRsp->addParts2(*m_moverRSlider);


		//##########
		//Slider
		//##########
		m_diffuseRSlider->setCursorListener([=, this]() {
			double value = m_diffuseRSlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentval = m_model->GetGrassDiffuseRate();
				currentval.x = (float)value;
				m_model->SetGrassDiffuseRate(currentval);
			}
			});
		m_diffuseGSlider->setCursorListener([=, this]() {
			double value = m_diffuseGSlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentval = m_model->GetGrassDiffuseRate();
				currentval.y = (float)value;
				m_model->SetGrassDiffuseRate(currentval);
			}
			});
		m_diffuseBSlider->setCursorListener([=, this]() {
			double value = m_diffuseBSlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentval = m_model->GetGrassDiffuseRate();
				currentval.z = (float)value;
				m_model->SetGrassDiffuseRate(currentval);
			}
			});
		m_diffuseASlider->setCursorListener([=, this]() {
			double value = m_diffuseASlider->getValue();
			if (m_model != nullptr) {
				ChaVector4 currentval = m_model->GetGrassDiffuseRate();
				currentval.w = (float)value;
				m_model->SetGrassDiffuseRate(currentval);
			}
			});



		m_shapescaleXSlider->setCursorListener([=, this]() {
			double value = m_shapescaleXSlider->getValue();
			if (m_model != nullptr) {
				ChaVector3 currentval = m_model->GetGrassShapeScale();
				currentval.x = (float)value;
				m_model->SetGrassShapeScale(currentval);
			}
			});
		m_shapescaleYSlider->setCursorListener([=, this]() {
			double value = m_shapescaleYSlider->getValue();
			if (m_model != nullptr) {
				ChaVector3 currentval = m_model->GetGrassShapeScale();
				currentval.y = (float)value;
				m_model->SetGrassShapeScale(currentval);
			}
			});
		m_shapescaleZSlider->setCursorListener([=, this]() {
			double value = m_shapescaleZSlider->getValue();
			if (m_model != nullptr) {
				ChaVector3 currentval = m_model->GetGrassShapeScale();
				currentval.z = (float)value;
				m_model->SetGrassShapeScale(currentval);
			}
			});
		m_bendscaleSlider->setCursorListener([=, this]() {
			double value = m_bendscaleSlider->getValue();
			if (m_model != nullptr) {
				m_model->SetGrassBendScale((float)value);
			}
			});
		m_moverRSlider->setCursorListener([=, this]() {
			double value = m_moverRSlider->getValue();
			g_grassmover.mover_r = (float)value;
			});

		///////////////////
		// Combo
		///////////////////
		m_moverCombo->setButtonListener([=, this]() {
			int comboid = m_moverCombo->trackPopUpMenu();
			if ((comboid >= 1) && m_chascene && m_model) {
				MODELELEM modelelem = m_chascene->GetModelElem(comboid - 1);
				if (modelelem.modelptr) {
					g_grassmover.mover_model = modelelem.modelptr;
				}
				else {
					g_grassmover.mover_model = nullptr;
				}
			}
			else {
				g_grassmover.mover_model = nullptr;
			}

			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
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

int CGrassDlg::Params2Dlg()
{
	if ((m_dlgWnd != nullptr) && (m_model != nullptr)) {
		if (m_nameLabel) {
			m_nameLabel->setName(m_model->GetFileName());
		}
		if (m_grassLabel) {
			if (m_model->GetGrassFlag()) {
				m_grassLabel->setName(L"this model is grass.");
			}
			else {
				m_grassLabel->setName(L"this model is NOT grass.");
			}
		}

		ChaVector4 diffuserate = m_model->GetGrassDiffuseRate();
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

		ChaVector3 shapescale = m_model->GetGrassShapeScale();
		if (m_shapescaleXSlider != nullptr) {
			m_shapescaleXSlider->setValue((double)shapescale.x, false);
		}
		if (m_shapescaleYSlider != nullptr) {
			m_shapescaleYSlider->setValue((double)shapescale.y, false);
		}
		if (m_shapescaleZSlider != nullptr) {
			m_shapescaleZSlider->setValue((double)shapescale.z, false);
		}

		float bendscale = m_model->GetGrassBendScale();
		if (m_bendscaleSlider != nullptr) {
			m_bendscaleSlider->setValue((double)bendscale, false);
		}

		if (m_moverRSlider != nullptr) {
			m_moverRSlider->setValue((double)g_grassmover.mover_r, false);
		}

		if (m_moverCombo) {
			m_moverCombo->ResetCombo();
			m_moverCombo->addString("   ");//先頭項目は未設定

			int findselected = -1;
			int modelnum = m_chascene->GetModelNum();
			int modelindex;
			for (modelindex = 0; modelindex < modelnum; modelindex++) {
				MODELELEM curmodelelem = m_chascene->GetModelElem(modelindex);
				if (curmodelelem.modelptr) {
					WCHAR movername[MAX_PATH] = { 0L };
					wcscpy_s(movername, MAX_PATH, curmodelelem.modelptr->GetFileName());
					char mbmovername[MAX_PATH] = { 0 };
					WideCharToMultiByte(CP_ACP, 0, movername, -1, mbmovername, MAX_PATH, NULL, NULL);
					m_moverCombo->addString(mbmovername);

					if ((g_grassmover.mover_model != nullptr) && (g_grassmover.mover_model == curmodelelem.modelptr)) {
						findselected = modelindex;
					}
				}
				else {
					m_moverCombo->addString("invalid name");
				}
			}

			if (findselected >= 0) {
				m_moverCombo->setSelectedCombo(findselected + 1);
			}
		}

		m_dlgWnd->callRewrite();
	}
	else {
		//_ASSERT(0);
		int dbgflag1 = 1;
	}

	return 0;
}



const HWND CGrassDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CGrassDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CGrassDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

void CGrassDlg::CallRewrite()
{
	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		m_dlgWnd->callRewrite();
	}
}
