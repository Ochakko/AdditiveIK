#include "stdafx.h"

#include <BulletDlg.h>
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
extern double g_erp;//AdditiveIK.cpp


/////////////////////////////////////////////////////////////////////////////
// CBulletDlg

CBulletDlg::CBulletDlg()
{
	InitParams();
}

CBulletDlg::~CBulletDlg()
{
	DestroyObjs();
}
	
int CBulletDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_bulletcalccntLabel) {
		delete m_bulletcalccntLabel;
		m_bulletcalccntLabel = nullptr;
	}
	if (m_bulletcalccntSlider) {
		delete m_bulletcalccntSlider;
		m_bulletcalccntSlider = nullptr;
	}
	if (m_bulletERPLabel) {
		delete m_bulletERPLabel;
		m_bulletERPLabel = nullptr;
	}
	if (m_bulletERPSlider) {
		delete m_bulletERPSlider;
		m_bulletERPSlider = nullptr;
	}
	if (m_bulletlimitrateLabel) {
		delete m_bulletlimitrateLabel;
		m_bulletlimitrateLabel = nullptr;
	}
	if (m_bulletlimitrateSlider) {
		delete m_bulletlimitrateSlider;
		m_bulletlimitrateSlider = nullptr;
	}
	if (m_bulletmvrateLabel) {
		delete m_bulletmvrateLabel;
		m_bulletmvrateLabel = nullptr;
	}
	if (m_bulletmvrateSlider) {
		delete m_bulletmvrateSlider;
		m_bulletmvrateSlider = nullptr;
	}
	if (m_bulletspringscaleLabel) {
		delete m_bulletspringscaleLabel;
		m_bulletspringscaleLabel = nullptr;
	}
	if (m_bulletspringscaleSlider) {
		delete m_bulletspringscaleSlider;
		m_bulletspringscaleSlider = nullptr;
	}
	if (m_bulletrigidspeedLabel) {
		delete m_bulletrigidspeedLabel;
		m_bulletrigidspeedLabel = nullptr;
	}
	if (m_bulletrigidspeedSlider) {
		delete m_bulletrigidspeedSlider;
		m_bulletrigidspeedSlider = nullptr;
	}
	if (m_bulletcommentLabel) {
		delete m_bulletcommentLabel;
		m_bulletcommentLabel = nullptr;
	}
	if (m_bulletspacer1Label) {
		delete m_bulletspacer1Label;
		m_bulletspacer1Label = nullptr;
	}
	if (m_bulletspacer2Label) {
		delete m_bulletspacer2Label;
		m_bulletspacer2Label = nullptr;
	}
	if (m_bulletspacer3Label) {
		delete m_bulletspacer3Label;
		m_bulletspacer3Label = nullptr;
	}
	if (m_bulletspacer4Label) {
		delete m_bulletspacer4Label;
		m_bulletspacer4Label = nullptr;
	}
	if (m_bulletspacer5Label) {
		delete m_bulletspacer5Label;
		m_bulletspacer5Label = nullptr;
	}
	if (m_bulletspacer6Label) {
		delete m_bulletspacer6Label;
		m_bulletspacer6Label = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CBulletDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;


	m_dlgWnd = nullptr;

	m_bulletcalccntLabel = nullptr;
	m_bulletcalccntSlider = nullptr;
	m_bulletERPLabel = nullptr;
	m_bulletERPSlider = nullptr;
	m_bulletlimitrateLabel = nullptr;
	m_bulletlimitrateSlider = nullptr;
	m_bulletmvrateLabel = nullptr;
	m_bulletmvrateSlider = nullptr;
	m_bulletspringscaleLabel = nullptr;
	m_bulletspringscaleSlider = nullptr;
	m_bulletrigidspeedLabel = nullptr;
	m_bulletrigidspeedSlider = nullptr;
	m_bulletcommentLabel = nullptr;
	m_bulletspacer1Label = nullptr;
	m_bulletspacer2Label = nullptr;
	m_bulletspacer3Label = nullptr;
	m_bulletspacer4Label = nullptr;
	m_bulletspacer5Label = nullptr;
	m_bulletspacer6Label = nullptr;

}




int CBulletDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}




void CBulletDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateBulletWnd();//作成済の場合は０リターン

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


int CBulletDlg::CreateBulletWnd()
{
	if (m_dlgWnd) {
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("BulletDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("BulletDlg"),	//タイトル
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
		m_bulletcalccntLabel = new OWP_Label(L"BulletPhysics Calclation Count", labelheight);
		if (!m_bulletcalccntLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletcalccntSlider = new OWP_Slider((double)((int)g_btcalccnt), 10.0, 1.0, labelheight);
		if (!m_bulletcalccntSlider) {
			_ASSERT(0);
			abort();
		}
		m_bulletERPLabel = new OWP_Label(L"ERP Position Correcting Rate", labelheight);
		if (!m_bulletERPLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletERPSlider = new OWP_Slider(g_erp, 1.0, 0.0);
		if (!m_bulletERPSlider) {
			_ASSERT(0);
			abort();
		}
		m_bulletlimitrateLabel = new OWP_Label(L"Scale of PhysicalLimitRate", labelheight);
		if (!m_bulletlimitrateLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletlimitrateSlider = new OWP_Slider(g_physicalLimitScale, 1.0, 0.0, labelheight);
		if (!m_bulletlimitrateSlider) {
			_ASSERT(0);
			abort();
		}
		m_bulletmvrateLabel = new OWP_Label(L"Movable Rate on MovableRange", labelheight);
		if (!m_bulletmvrateLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletmvrateSlider = new OWP_Slider((double)g_physicalMovableRate, 100.0, 0.0, labelheight);
		if (!m_bulletmvrateSlider) {
			_ASSERT(0);
			abort();
		}
		m_bulletspringscaleLabel = new OWP_Label(L"Scale of PhysicalSpringCoef", labelheight);
		if (!m_bulletspringscaleLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletspringscaleSlider = new OWP_Slider((double)g_akscale, 10.0, 0.0, labelheight);
		if (!m_bulletspringscaleSlider) {
			_ASSERT(0);
			abort();
		}
		m_bulletrigidspeedLabel = new OWP_Label(L"Scale of RigidSpeed on MovalbeRange", labelheight);
		if (!m_bulletrigidspeedLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletrigidspeedSlider = new OWP_Slider(g_physicalVeloScale, 1.0, 0.0, labelheight);
		if (!m_bulletrigidspeedSlider) {
			_ASSERT(0);
			abort();
		}
		m_bulletcommentLabel = new OWP_Label(L"(SpeedScale on UnmovableRange is 0.1 fixed)", labelheight);
		if (!m_bulletcommentLabel) {
			_ASSERT(0);
			abort();
		}
		m_bulletspacer1Label = new OWP_Label(L"      ", labelheight);
		if (!m_bulletspacer1Label) {
			_ASSERT(0);
			abort();
		}
		m_bulletspacer2Label = new OWP_Label(L"      ", labelheight);
		if (!m_bulletspacer2Label) {
			_ASSERT(0);
			abort();
		}
		m_bulletspacer3Label = new OWP_Label(L"      ", labelheight);
		if (!m_bulletspacer3Label) {
			_ASSERT(0);
			abort();
		}
		m_bulletspacer4Label = new OWP_Label(L"      ", labelheight);
		if (!m_bulletspacer4Label) {
			_ASSERT(0);
			abort();
		}
		m_bulletspacer5Label = new OWP_Label(L"      ", labelheight);
		if (!m_bulletspacer5Label) {
			_ASSERT(0);
			abort();
		}
		m_bulletspacer6Label = new OWP_Label(L"      ", labelheight);
		if (!m_bulletspacer6Label) {
			_ASSERT(0);
			abort();
		}


		m_dlgWnd->addParts(*m_bulletcalccntLabel);
		m_dlgWnd->addParts(*m_bulletcalccntSlider);
		m_dlgWnd->addParts(*m_bulletspacer1Label);

		m_dlgWnd->addParts(*m_bulletERPLabel);
		m_dlgWnd->addParts(*m_bulletERPSlider);
		m_dlgWnd->addParts(*m_bulletspacer2Label);

		m_dlgWnd->addParts(*m_bulletlimitrateLabel);
		m_dlgWnd->addParts(*m_bulletlimitrateSlider);//g_physicalLimitScale: 0,1
		m_dlgWnd->addParts(*m_bulletspacer3Label);

		m_dlgWnd->addParts(*m_bulletmvrateLabel);
		m_dlgWnd->addParts(*m_bulletmvrateSlider);//g_physicalMovableRate: 0, 100
		m_dlgWnd->addParts(*m_bulletspacer4Label);

		m_dlgWnd->addParts(*m_bulletspringscaleLabel);
		m_dlgWnd->addParts(*m_bulletspringscaleSlider);//g_akscale: 0, 10
		m_dlgWnd->addParts(*m_bulletspacer5Label);

		m_dlgWnd->addParts(*m_bulletrigidspeedLabel);
		m_dlgWnd->addParts(*m_bulletrigidspeedSlider);//g_physicalVeloScale: 0, 1
		m_dlgWnd->addParts(*m_bulletspacer6Label);

		m_dlgWnd->addParts(*m_bulletcommentLabel);


		//##########
		//Slider
		//##########
		m_bulletcalccntSlider->setCursorListener([=, this]() {
			double value = m_bulletcalccntSlider->getValue();
			g_btcalccnt = (int)(value + 0.0001);
			m_bulletcalccntSlider->setValue((double)g_btcalccnt, false);//intに丸めてセットし直し
			});

		m_bulletERPSlider->setCursorListener([=, this]() {
			double value = m_bulletERPSlider->getValue();
			g_erp = value;
			});

		m_bulletlimitrateSlider->setCursorListener([=, this]() {
			double value = m_bulletlimitrateSlider->getValue();
			g_physicalLimitScale = value;
			});

		m_bulletmvrateSlider->setCursorListener([=, this]() {
			double value = m_bulletmvrateSlider->getValue();
			g_physicalMovableRate = (int)(value + 0.0001);
			m_bulletmvrateSlider->setValue((double)g_physicalMovableRate, false);//intに丸めてセットし直し
			});

		m_bulletspringscaleSlider->setCursorListener([=, this]() {
			double value = m_bulletspringscaleSlider->getValue();
			g_akscale = (float)value;
			});
		m_bulletspringscaleSlider->setLUpListener([=, this]() {
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_BULLETDLG), (LPARAM)0);
			}
			});

		m_bulletrigidspeedSlider->setCursorListener([=, this]() {
			double value = m_bulletrigidspeedSlider->getValue();
			g_physicalVeloScale = value;
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

int CBulletDlg::ParamsToDlg()
{
	if (m_dlgWnd) {
		if (m_bulletlimitrateSlider) {
			m_bulletlimitrateSlider->setValue(g_physicalLimitScale, false);
		}
		if (m_bulletmvrateSlider) {
			m_bulletmvrateSlider->setValue((double)g_physicalMovableRate, false);
		}
		if (m_bulletspringscaleSlider) {
			m_bulletspringscaleSlider->setValue((double)g_akscale, false);
		}
		if (m_bulletrigidspeedSlider) {
			m_bulletrigidspeedSlider->setValue(g_physicalVeloScale, false);
		}

		m_dlgWnd->callRewrite();
	}

	return 0;
}

const HWND CBulletDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CBulletDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CBulletDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

