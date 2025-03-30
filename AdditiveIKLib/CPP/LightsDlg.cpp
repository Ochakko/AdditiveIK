#include "stdafx.h"

#include <LightsDlg.h>
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
// CLightsDlg

CLightsDlg::CLightsDlg()
{
	InitParams();
}

CLightsDlg::~CLightsDlg()
{
	DestroyObjs();
}
	
int CLightsDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_lightsslotsp) {
		delete m_lightsslotsp;
		m_lightsslotsp = nullptr;
	}
	if (m_lightsslotCombo) {
		delete m_lightsslotCombo;
		m_lightsslotCombo = nullptr;
	}
	if (m_lightsslotLabel) {
		delete m_lightsslotLabel;
		m_lightsslotLabel = nullptr;
	}
	int lightindex;
	for (lightindex = 0; lightindex < 8; lightindex++) {
		if (m_lightsgroupLabel[lightindex]) {
			delete m_lightsgroupLabel[lightindex];
			m_lightsgroupLabel[lightindex] = nullptr;
		}
		if (m_lightschecksp[lightindex]) {
			delete m_lightschecksp[lightindex];
			m_lightschecksp[lightindex] = nullptr;
		}
		if (m_lightsenableChk[lightindex]) {
			delete m_lightsenableChk[lightindex];
			m_lightsenableChk[lightindex] = nullptr;
		}
		if (m_lightsviewrotChk[lightindex]) {
			delete m_lightsviewrotChk[lightindex];
			m_lightsviewrotChk[lightindex] = nullptr;
		}
		if (m_lightscolorsp[lightindex]) {
			delete m_lightscolorsp[lightindex];
			m_lightscolorsp[lightindex] = nullptr;
		}
		if (m_lightscolorBox[lightindex]) {
			delete m_lightscolorBox[lightindex];
			m_lightscolorBox[lightindex] = nullptr;
		}
		if (m_lightsmultSlider[lightindex]) {
			delete m_lightsmultSlider[lightindex];
			m_lightsmultSlider[lightindex] = nullptr;
		}
		if (m_polarsp0[lightindex]) {
			delete m_polarsp0[lightindex];
			m_polarsp0[lightindex] = nullptr;
		}
		if (m_polarsp1[lightindex]) {
			delete m_polarsp1[lightindex];
			m_polarsp1[lightindex] = nullptr;
		}
		if (m_polarsp2[lightindex]) {
			delete m_polarsp2[lightindex];
			m_polarsp2[lightindex] = nullptr;
		}
		if (m_polarxzLabel[lightindex]) {
			delete m_polarxzLabel[lightindex];
			m_polarxzLabel[lightindex] = nullptr;
		}
		if (m_polarxzEdit[lightindex]) {
			delete m_polarxzEdit[lightindex];
			m_polarxzEdit[lightindex] = nullptr;
		}
		if (m_polaryLabel[lightindex]) {
			delete m_polaryLabel[lightindex];
			m_polaryLabel[lightindex] = nullptr;
		}
		if (m_polaryEdit[lightindex]) {
			delete m_polaryEdit[lightindex];
			m_polaryEdit[lightindex] = nullptr;
		}
	}
	//if (m_lightsapplysp) {
	//	delete m_lightsapplysp;
	//	m_lightsapplysp = nullptr;
	//}
	//if (m_lightsspace1Label) {
	//	delete m_lightsspace1Label;
	//	m_lightsspace1Label = nullptr;
	//}
	//if (m_lightsapplyB) {
	//	delete m_lightsapplyB;
	//	m_lightsapplyB = nullptr;
	//}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	return 0;
}

void CLightsDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;


	m_dlgWnd = nullptr;

	m_lightsslotsp = nullptr;
	m_lightsslotCombo = nullptr;
	m_lightsslotLabel = nullptr;
	int lightindex;
	for (lightindex = 0; lightindex < 8; lightindex++) {
		m_lightsgroupLabel[lightindex] = nullptr;
		m_lightschecksp[lightindex] = nullptr;
		m_lightsenableChk[lightindex] = nullptr;
		m_lightsviewrotChk[lightindex] = nullptr;
		m_lightscolorsp[lightindex] = nullptr;
		m_lightscolorBox[lightindex] = nullptr;
		m_lightsmultSlider[lightindex] = nullptr;
		m_polarsp0[lightindex] = nullptr;
		m_polarsp1[lightindex] = nullptr;
		m_polarsp2[lightindex] = nullptr;
		m_polarxzLabel[lightindex] = nullptr;
		m_polarxzEdit[lightindex] = nullptr;
		m_polaryLabel[lightindex] = nullptr;
		m_polaryEdit[lightindex] = nullptr;
	}
	//m_lightsapplysp = nullptr;
	//m_lightsspace1Label = nullptr;
	//m_lightsapplyB = nullptr;

}




int CLightsDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}




void CLightsDlg::SetVisible(bool srcflag)
{
	if (srcflag) {

		CreateLightsWnd();//作成済の場合は０リターン

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


int CLightsDlg::CreateLightsWnd()
{
	if (m_dlgWnd) {
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("LightsForEditDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("LightsForEditDlg"),	//タイトル
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

		m_lightsslotsp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_lightsslotsp) {
			_ASSERT(0);
			abort();
		}
		m_lightsslotCombo = new OWP_ComboBoxA(L"LightsSlot", labelheight);
		if (!m_lightsslotCombo) {
			_ASSERT(0);
			abort();
		}
		int slotindex;
		for (slotindex = 0; slotindex < 8; slotindex++) {
			char strslot[128] = { 0 };
			sprintf_s(strslot, 128, "Slot_%d", slotindex);
			m_lightsslotCombo->addString(strslot);
		}
		m_lightsslotCombo->setSelectedCombo(g_lightSlot);


		m_lightsslotLabel = new OWP_Label(L"PolarCoordDeg(極座標)", labelheight);
		if (!m_lightsslotLabel) {
			_ASSERT(0);
			abort();
		}

		int lightindex;
		for (lightindex = 0; lightindex < 8; lightindex++) {
			WCHAR groupname[256] = { 0L };
			swprintf_s(groupname, 256, L"Light_%d", lightindex + 1);
			m_lightsgroupLabel[lightindex] = new OWP_Label(groupname, labelheight);
			if (!m_lightsgroupLabel[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_lightschecksp[lightindex] = new OWP_Separator(m_dlgWnd, true, rate50, true);
			if (!m_lightschecksp[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_lightsenableChk[lightindex] = new OWP_CheckBoxA(L"Enable", g_lightEnable[g_lightSlot][lightindex], labelheight, true);
			if (!m_lightsenableChk[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_lightsviewrotChk[lightindex] = new OWP_CheckBoxA(L"RotWithView", g_lightDirWithView[g_lightSlot][lightindex], labelheight, false);
			if (!m_lightsviewrotChk[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_lightscolorsp[lightindex] = new OWP_Separator(m_dlgWnd, true, rate50, true);
			if (!m_lightscolorsp[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_lightscolorBox[lightindex] = new OWP_ColorBox(g_lightDiffuse[g_lightSlot][lightindex].ColorRef(), labelheight);//g_lightDiffuse[g_lightSlot][lightindex].SetParams(fr, fg, fb);
			if (!m_lightscolorBox[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_lightsmultSlider[lightindex] = new OWP_Slider(g_lightScale[g_lightSlot][lightindex], 3.0, 0.0, labelheight);
			if (!m_lightsmultSlider[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polarsp0[lightindex] = new OWP_Separator(m_dlgWnd, true, rate50, true);
			if (!m_polarsp0[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polarsp1[lightindex] = new OWP_Separator(m_dlgWnd, true, rate50, true);
			if (!m_polarsp1[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polarsp2[lightindex] = new OWP_Separator(m_dlgWnd, true, rate50, true);
			if (!m_polarsp2[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polarxzLabel[lightindex] = new OWP_Label(L"XZ(-180,180]", labelheight);
			if (!m_polarxzLabel[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polarxzEdit[lightindex] = new OWP_EditBox(true, L"XZ Edit", labelheight, EDIT_BUFLEN_NUM);
			if (!m_polarxzEdit[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polaryLabel[lightindex] = new OWP_Label(L"Y(-90,90]", labelheight);
			if (!m_polaryLabel[lightindex]) {
				_ASSERT(0);
				abort();
			}
			m_polaryEdit[lightindex] = new OWP_EditBox(true, L"Y Edit", labelheight, EDIT_BUFLEN_NUM);
			if (!m_polaryEdit[lightindex]) {
				_ASSERT(0);
				abort();
			}
		}
		//m_lightsapplysp = new OWP_Separator(m_dlgWnd, true, rate50, true);
		//if (!m_lightsapplysp) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_lightsspace1Label = new OWP_Label(L"     ", 24);
		//if (!m_lightsspace1Label) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_lightsapplyB = new OWP_Button(L"Apply(適用)", 38);
		//if (!m_lightsapplyB) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_lightsapplyB->setTextColor(RGB(168, 129, 129));


		m_dlgWnd->addParts(*m_lightsslotsp);
		m_lightsslotsp->addParts1(*m_lightsslotCombo);
		m_lightsslotsp->addParts2(*m_lightsslotLabel);
		int lightindex2;
		for (lightindex2 = 0; lightindex2 < 8; lightindex2++) {
			m_dlgWnd->addParts(*m_lightsgroupLabel[lightindex2]);
			m_dlgWnd->addParts(*m_lightschecksp[lightindex2]);
			m_lightschecksp[lightindex2]->addParts1(*m_lightsenableChk[lightindex2]);
			m_lightschecksp[lightindex2]->addParts2(*m_lightsviewrotChk[lightindex2]);
			m_dlgWnd->addParts(*m_lightscolorsp[lightindex2]);
			m_lightscolorsp[lightindex2]->addParts1(*m_lightscolorBox[lightindex2]);
			m_lightscolorsp[lightindex2]->addParts2(*m_lightsmultSlider[lightindex2]);
			m_dlgWnd->addParts(*m_polarsp0[lightindex2]);
			m_polarsp0[lightindex2]->addParts1(*m_polarsp1[lightindex2]);
			m_polarsp0[lightindex2]->addParts2(*m_polarsp2[lightindex2]);
			m_polarsp1[lightindex2]->addParts1(*m_polarxzLabel[lightindex2]);
			m_polarsp1[lightindex2]->addParts2(*m_polarxzEdit[lightindex2]);
			m_polarsp2[lightindex2]->addParts1(*m_polaryLabel[lightindex2]);
			m_polarsp2[lightindex2]->addParts2(*m_polaryEdit[lightindex2]);
		}
		//m_dlgWnd->addParts(*m_lightsspace1Label);
		//m_dlgWnd->addParts(*m_lightsapplysp);
		//m_lightsapplysp->addParts2(*m_lightsapplyB);


		//############
		//ComboBox
		//############
		m_lightsslotCombo->setButtonListener([=, this]() {
			int comboid = m_lightsslotCombo->trackPopUpMenu();
			g_lightSlot = comboid;

			ParamsToDlg();
			if (m_dlgWnd) {
				m_dlgWnd->callRewrite();
			}

			//SetLightDirection();
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
			}
			});

		int lightindex3;
		for (lightindex3 = 0; lightindex3 < 8; lightindex3++) {
			//###########
			//CheckBox
			//###########
			m_lightsenableChk[lightindex3]->setButtonListener([=, this]() {
				bool value = m_lightsenableChk[lightindex3]->getValue();
				g_lightEnable[g_lightSlot][lightindex3] = value;
				
				//SetLightDirection();
				if (g_mainhwnd && IsWindow(g_mainhwnd)) {
					PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
				}
				});

			m_lightsviewrotChk[lightindex3]->setButtonListener([=, this]() {
				bool value = m_lightsviewrotChk[lightindex3]->getValue();
				g_lightDirWithView[g_lightSlot][lightindex3] = value;
				
				//SetLightDirection();
				if (g_mainhwnd && IsWindow(g_mainhwnd)) {
					PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
				}
				});


			//##########
			//Slider
			//##########
			m_lightsmultSlider[lightindex3]->setCursorListener([=, this]() {
				double value = m_lightsmultSlider[lightindex3]->getValue();
				g_lightScale[g_lightSlot][lightindex3] = (float)value;
				
				//SetLightDirection();
				if (g_mainhwnd && IsWindow(g_mainhwnd)) {
					PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
				}
			});



			//##########
			//ColorBox
			//##########
			m_lightscolorBox[lightindex3]->setButtonListener([=, this]() {
				COLORREF choosedcolor = m_lightscolorBox[lightindex3]->getColor();
				float fr, fg, fb;
				fr = (float)((double)GetRValue(choosedcolor) / 255.0);
				fr = (float)fmin(1.0f, fr);
				fr = (float)fmax(0.0f, fr);
				fg = (float)((double)GetGValue(choosedcolor) / 255.0);
				fg = (float)fmin(1.0f, fg);
				fg = (float)fmax(0.0f, fg);
				fb = (float)((double)GetBValue(choosedcolor) / 255.0);
				fb = (float)fmin(1.0f, fb);
				fb = (float)fmax(0.0f, fb);

				g_lightDiffuse[g_lightSlot][lightindex3].SetParams(fr, fg, fb);

				m_lightscolorBox[lightindex3]->callRewrite();

				//SetLightDirection();
				if (g_mainhwnd && IsWindow(g_mainhwnd)) {
					PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
				}

				});
		}

		//########
		//EditBox
		//########
		Dlg2LightsListener();


		//m_lightsapplyB->setButtonListener([=, this]() {
		//	Dlg2Lights();

		//	//SetLightDirection();
		//	if (g_mainhwnd && IsWindow(g_mainhwnd)) {
		//		PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_LIGHTSDLG), (LPARAM)0);
		//	}
		//	});



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

int CLightsDlg::ParamsToDlg()
{

	if (m_dlgWnd != 0) {

		if ((g_lightSlot < 0) || (g_lightSlot >= LIGHTSLOTNUM)) {
			_ASSERT(0);
			return 1;
		}

		if (m_lightsslotCombo) {
			m_lightsslotCombo->setSelectedCombo(g_lightSlot);
		}

		int lightindex;
		for (lightindex = 0; lightindex < 8; lightindex++) {
			if (m_lightsenableChk[lightindex]) {
				m_lightsenableChk[lightindex]->setValue(g_lightEnable[g_lightSlot][lightindex], false);
			}

			if (m_lightsviewrotChk[lightindex]) {
				m_lightsviewrotChk[lightindex]->setValue(g_lightDirWithView[g_lightSlot][lightindex], false);
			}

			if (m_lightsmultSlider[lightindex]) {
				m_lightsmultSlider[lightindex]->setValue((double)g_lightScale[g_lightSlot][lightindex], false);
			}


			float degxz = 0.0f;
			float degy = 0.0f;
			ConvDir2PolarCoord(g_lightDir[g_lightSlot][lightindex].x, g_lightDir[g_lightSlot][lightindex].y, g_lightDir[g_lightSlot][lightindex].z,
				&degxz, &degy);

			WCHAR strdirx[256] = { 0L };
			swprintf_s(strdirx, 256, L"%.4f", degxz);
			if (m_polarxzEdit[lightindex]) {
				m_polarxzEdit[lightindex]->setName(strdirx);
			}
			WCHAR strdiry[256] = { 0L };
			swprintf_s(strdiry, 256, L"%.4f", degy);
			if (m_polaryEdit[lightindex]) {
				m_polaryEdit[lightindex]->setName(strdiry);
			}


			COLORREF col = g_lightDiffuse[g_lightSlot][lightindex].ColorRef();
			if (m_lightscolorBox[lightindex]) {
				m_lightscolorBox[lightindex]->setColor(col);
			}

		}

		m_dlgWnd->callRewrite();
	}
	return 0;

}

int CLightsDlg::Dlg2LightsListener()
{
	if (m_dlgWnd != 0) {
		if ((g_lightSlot < 0) || (g_lightSlot >= LIGHTSLOTNUM)) {
			_ASSERT(0);
			return 1;
		}


		int lightindex;
		for (lightindex = 0; lightindex < 8; lightindex++) {

			if (m_polarxzEdit[lightindex]) {
				m_polarxzEdit[lightindex]->setExitDialogListener([=, this]() {
					ChaVector3 dir0 = g_lightDir[g_lightSlot][lightindex];
					float degxz0 = 0.0f;
					float degy0 = 0.0f;
					ConvDir2PolarCoord(dir0.x, dir0.y, dir0.z, &degxz0, &degy0);

					WCHAR strdegxz[256] = { 0L };
					m_polarxzEdit[lightindex]->getName(strdegxz, 256);
					int chkvalue;
					chkvalue = CheckStr_float(strdegxz);
					if (chkvalue == 0) {
						float degxz;
						degxz = (float)_wtof(strdegxz);

						float dirx = 0.0f;
						float diry = 0.0f;
						float dirz = 0.0f;
						ConvPolarCoord2Dir(degxz, degy0, &dirx, &diry, &dirz);
						g_lightDir[g_lightSlot][lightindex].SetParams(dirx, diry, dirz);
					}
					else {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", degxz0);
						m_polarxzEdit[lightindex]->setName(strnotchange);
					}
				});
			}

			if (m_polaryEdit[lightindex]) {
				m_polaryEdit[lightindex]->setExitDialogListener([=, this]() {
					ChaVector3 dir0 = g_lightDir[g_lightSlot][lightindex];
					float degxz0 = 0.0f;
					float degy0 = 0.0f;
					ConvDir2PolarCoord(dir0.x, dir0.y, dir0.z, &degxz0, &degy0);

					WCHAR strdegy[256] = { 0L };
					m_polaryEdit[lightindex]->getName(strdegy, 256);
					int chkvalue;
					chkvalue = CheckStr_float(strdegy);
					if (chkvalue == 0) {
						float degy;
						degy = (float)_wtof(strdegy);

						float dirx = 0.0f;
						float diry = 0.0f;
						float dirz = 0.0f;
						ConvPolarCoord2Dir(degxz0, degy, &dirx, &diry, &dirz);
						g_lightDir[g_lightSlot][lightindex].SetParams(dirx, diry, dirz);
					}
					else {
						WCHAR strnotchange[256] = { 0L };
						swprintf_s(strnotchange, 256, L"%f", degy0);
						m_polaryEdit[lightindex]->setName(strnotchange);
					}
				});

			}

			//if (m_lightsenableChk[lightindex]) {
			//	g_lightEnable[g_lightSlot][lightindex] = m_lightsenableChk[lightindex]->getValue();
			//}
			//if (m_lightsviewrotChk[lightindex]) {
			//	g_lightDirWithView[g_lightSlot][lightindex] = m_lightsviewrotChk[lightindex]->getValue();
			//}
			//if (m_lightsmultSlider[lightindex]) {
			//	g_lightScale[g_lightSlot][lightindex] = (float)m_lightsmultSlider[lightindex]->getValue();
			//}
		}


	}

	return 0;
}



int CLightsDlg::ConvDir2PolarCoord(float srcdirx, float srcdiry, float srcdirz, float* dstxzdeg, float* dstydeg)
{
	if (!dstxzdeg || !dstydeg) {
		_ASSERT(0);
		return 1;
	}
	ChaVector3 srcdir;
	srcdir.SetParams(srcdirx, srcdiry, srcdirz);
	ChaVector3 ndir;
	ndir.SetZeroVec3();
	ChaVector3Normalize(&ndir, &srcdir);

	//float degy = (float)(-asin(ndir.y) * PAI2DEG);//srcdiry
	float degy = (float)(asin(ndir.y) * PAI2DEG);//srcdiry
	float degxz = (float)(-atan2(ndir.x, ndir.z) * PAI2DEG - 180.0f);//本来はX軸が０度だが、Z軸が０度になるように計算。左回りに。
	float setdegxz = degxz;
	if (degxz > 180.0f) {
		setdegxz -= 360.0f;
	}
	else if (degxz < -180.0f) {
		setdegxz += 360.0f;
	}
	*dstxzdeg = setdegxz;
	*dstydeg = degy;

	return 0;
}

int CLightsDlg::ConvPolarCoord2Dir(float srcxzdeg, float srcydeg, float* dstdirx, float* dstdiry, float* dstdirz)
{
	if (!dstdirx || !dstdiry || !dstdirz) {
		_ASSERT(0);
		return 1;
	}

	float diry = (float)sin(srcydeg * DEG2PAI);
	float dirycos = (float)cos(srcydeg * DEG2PAI);
	float dirx = (float)sin(srcxzdeg * DEG2PAI) * dirycos;//本来はX軸が０度だが、Z軸が０度になるように計算。左回りに。
	float dirz = (float)-cos(srcxzdeg * DEG2PAI) * dirycos;//本来はX軸が０度だが、Z軸が０度になるように計算。左回りに。

	ChaVector3 calcdir;
	calcdir.SetParams(dirx, diry, dirz);
	ChaVector3 ndir;
	ndir.SetZeroVec3();
	ChaVector3Normalize(&ndir, &calcdir);

	*dstdirx = ndir.x;
	*dstdiry = ndir.y;
	*dstdirz = ndir.z;

	return 0;
}

int CLightsDlg::CheckStr_float(const WCHAR* srcstr)
{
	if (!srcstr) {
		return 1;
	}
	size_t strleng = wcslen(srcstr);
	if ((strleng <= 0) || (strleng >= 256)) {
		_ASSERT(0);
		return 1;
	}

	bool errorflag = false;
	size_t strindex;
	for (strindex = 0; strindex < strleng; strindex++) {
		WCHAR curwc = *(srcstr + strindex);
		if ((curwc >= TEXT('0')) && (curwc <= TEXT('9')) || (curwc == TEXT('+')) || (curwc == TEXT('-')) ||
			(curwc == TEXT('.'))
			) {

		}
		else {
			errorflag = true;
			break;
		}
	}

	if (errorflag == false) {
		return 0;
	}
	else {
		return 1;
	}
}

const HWND CLightsDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CLightsDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CLightsDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

