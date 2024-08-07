#include "stdafx.h"

#include <BlendShapeDlg.h>
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
// CBlendShapeDlg

CBlendShapeDlg::CBlendShapeDlg()
{
	InitParams();
}

CBlendShapeDlg::~CBlendShapeDlg()
{
	DestroyObjs();
}
	
int CBlendShapeDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_blendshapeSCWnd) {
		delete m_blendshapeSCWnd;
		m_blendshapeSCWnd = nullptr;
	}
	if (m_blendshapesp0) {
		delete m_blendshapesp0;
		m_blendshapesp0 = nullptr;
	}
	if (m_blendshapesp1) {
		delete m_blendshapesp1;
		m_blendshapesp1 = nullptr;
	}
	if (m_blendshapeadditive) {
		delete m_blendshapeadditive;
		m_blendshapeadditive = nullptr;
	}
	if (m_blendshapemodelname) {
		delete m_blendshapemodelname;
		m_blendshapemodelname = nullptr;
	}
	if (m_blendshapeemptyLabel) {
		delete m_blendshapeemptyLabel;
		m_blendshapeemptyLabel = nullptr;
	}
	if (m_blendshapedistSlider) {
		delete m_blendshapedistSlider;
		m_blendshapedistSlider = nullptr;
	}
	if (m_blendshapedistLabel) {
		delete m_blendshapedistLabel;
		m_blendshapedistLabel = nullptr;
	}
	if (m_blendshapedistsp) {
		delete m_blendshapedistsp;
		m_blendshapedistsp = nullptr;
	}


	int labelnum = (int)m_blendshapeButton.size();
	int labelindex;
	for (labelindex = 0; labelindex < labelnum; labelindex++) {
		OWP_Button* delbutton = m_blendshapeButton[labelindex];
		if (delbutton) {
			delete delbutton;
		}
	}
	m_blendshapeButton.clear();

	int slidernum = (int)m_blendshapeSlider.size();
	int sliderindex;
	for (sliderindex = 0; sliderindex < slidernum; sliderindex++) {
		OWP_Slider* delslider = m_blendshapeSlider[sliderindex];
		if (delslider) {
			delete delslider;
		}
	}
	m_blendshapeSlider.clear();

	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	m_blendshapeelemvec.clear();
	m_blendshapelinenum = 0;
	m_blendshapeOpeIndex = 0;
	m_blendshapeBefore = 0.0f;
	m_blendshapeAfter = 0.0f;
	m_blendshapeUnderEdit = false;
	m_blendshapePostEdit = false;

	return 0;
}

void CBlendShapeDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;

	m_dlgWnd = nullptr;

	m_blendshapeSCWnd = nullptr;
	m_blendshapesp0 = nullptr;
	m_blendshapesp1 = nullptr;
	m_blendshapeadditive = nullptr;
	m_blendshapemodelname = nullptr;
	m_blendshapeemptyLabel = nullptr;
	m_blendshapedistsp = nullptr;
	m_blendshapedistLabel = nullptr;
	m_blendshapedistSlider = nullptr;

	m_blendshapeButton.clear();
	m_blendshapeSlider.clear();
	m_blendshapeelemvec.clear();
	m_blendshapelinenum = 0;

	g_blendshapeAddtiveMode = false;
	g_blendshapedist = 1.0f;
	m_blendshapeBefore = 0.0f;
	m_blendshapeAfter = 0.0f;
	m_blendshapeOpeIndex = 0;
	m_blendshapeUnderEdit = false;
	m_blendshapePostEdit = false;
	m_blendshapeUnderSelect = false;
	m_blendshapeUnderSelectFromUndo = false;
	m_blendshapeUnderSelectFromRefresh = false;
	m_blendshapeUndoOpeIndex = 0;

}




int CBlendShapeDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

void CBlendShapeDlg::SetModel(CModel* srcmodel)
{
	m_model = srcmodel;
	CreateBlendShapeWnd();
	ParamsToDlg();
}


void CBlendShapeDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_blendshapeSCWnd) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_blendshapeSCWnd->autoResize();
			}
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


int CBlendShapeDlg::CreateBlendShapeWnd()
{

	DestroyObjs();


	if (!m_model) {
		_ASSERT(0);
		return 0;
	}


	m_dlgWnd = new OrgWindow(
		0,
		_T("BlendShapeWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("BlendShapeWindow"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		true,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true, true);					//サイズ変更の可否

	if (m_dlgWnd) {

		m_dlgWnd->setSizeMin(WindowSize(150, 150));		// 最小サイズを設定

		/*
		static OrgWindow* m_dlgWnd = 0;
		static OWP_ScrollWnd* m_blendshapeSCWnd = 0;
		static OWP_Separator* m_blendshapesp0 = 0;
		static std::vector<OWP_Button*> m_blendshapeButton;
		static std::vector<OWP_Slider*> m_blendshapeSlider;
		static std::vector<CBlendShapeElem> m_blendshapeelemvec;
		static int m_blendshapelinenum = 0;
		*/

		m_blendshapeButton.clear();
		m_blendshapeSlider.clear();
		m_blendshapeelemvec.clear();

		int result = m_model->SetBlendShapeGUI(m_blendshapeelemvec);
		if (result != 0) {
			_ASSERT(0);
			return 1;
		}
		m_blendshapelinenum = (int)m_blendshapeelemvec.size();

		if (m_blendshapelinenum <= 0) {
			return 0;
		}

		double centerrate;
		int linedatasize;
		if (g_4kresolution) {
			//centerrate = (double)3 / (double)140;
			centerrate = (double)7 / (double)140;
			//linedatasize = max(140, linenum + 12);
			//linedatasize = max(106, (linenum + 12));
			linedatasize = (int)((double)m_blendshapelinenum * 1.50);
		}
		else {
			//centerrate = (double)3 / (double)70;
			centerrate = (double)7 / (double)70;
			//linedatasize = max(70, linenum + 12);
			//linedatasize = max(54, (linenum + 12));
			linedatasize = (int)((double)m_blendshapelinenum * 1.50);
		}
		//centerrate = 1.0 / (double)linedatasize;


		//スクロールウインドウ		
		m_blendshapeSCWnd = new OWP_ScrollWnd(L"BlendShapeScWnd", true, 20);
		if (!m_blendshapeSCWnd) {
			_ASSERT(0);
			return 1;
		}
		m_blendshapeSCWnd->setLineDataSize(linedatasize);//!!!!!!!!!!!!!
		//m_dlgWnd->addParts(*m_blendshapeSCWnd);


		//m_blendshapesp0 = new OWP_Separator(m_dlgWnd, false, centerrate, false, m_blendshapeSCWnd);
		m_blendshapesp0 = new OWP_Separator(m_dlgWnd, true, centerrate, false);
		if (!m_blendshapesp0) {
			_ASSERT(0);
			return 1;
		}
		//m_blendshapeSCWnd->addParts(*m_blendshapesp0);

		//m_blendshapesp1 = new OWP_Separator(m_dlgWnd, true, 0.3, true, m_blendshapeSCWnd);
		//m_blendshapesp1 = new OWP_Separator(m_dlgWnd, true, 0.38, true, m_blendshapeSCWnd);
		m_blendshapesp1 = new OWP_Separator(m_dlgWnd, true, 0.50, true, m_blendshapeSCWnd);
		if (!m_blendshapesp0) {
			_ASSERT(0);
			return 1;
		}


		m_blendshapemodelname = new OWP_Label(m_model->GetFileName(), 20);
		if (!m_blendshapemodelname) {
			_ASSERT(0);
			return 1;
		}

		m_blendshapedistsp = new OWP_Separator(m_dlgWnd, true, 0.3, true);
		if (!m_blendshapedistsp) {
			_ASSERT(0);
			return 1;
		}

		m_blendshapeemptyLabel = new OWP_Label(L"          ", 20);
		if (!m_blendshapeemptyLabel) {
			_ASSERT(0);
			return 0;
		}

		m_blendshapedistLabel = new OWP_Label(L"MorphDist", 20);
		if (!m_blendshapedistLabel) {
			_ASSERT(0);
			return 1;
		}

		m_blendshapedistSlider = new OWP_Slider(g_blendshapedist, 1.0, 0.0);
		if (!m_blendshapedistSlider) {
			_ASSERT(0);
			return 1;
		}

		m_blendshapeadditive = new OWP_CheckBoxA(L"AdditiveMode", g_blendshapeAddtiveMode, 20, false);
		if (!m_blendshapeadditive) {
			_ASSERT(0);
			return 1;
		}
		m_blendshapeadditive->setActive(false);//!!!!!!!!!!!!!!!!!!



		m_dlgWnd->addParts(*m_blendshapesp0);

		m_blendshapesp0->addParts1(*m_blendshapemodelname);
		m_blendshapesp0->addParts1(*m_blendshapeemptyLabel);
		m_blendshapesp0->addParts1(*m_blendshapedistsp);
		m_blendshapedistsp->addParts1(*m_blendshapedistLabel);
		m_blendshapedistsp->addParts2(*m_blendshapedistSlider);
		//m_blendshapesp0->addParts1(*m_blendshapeadditive);

		m_blendshapesp0->addParts2(*m_blendshapeSCWnd);
		m_blendshapeSCWnd->addParts(*m_blendshapesp1);



		int lineno;
		for (lineno = 0; lineno < m_blendshapelinenum; lineno++) {
			CBlendShapeElem curblendshape = m_blendshapeelemvec[lineno];
			if (curblendshape.validflag) {
				OWP_Slider* newslider = new OWP_Slider(0.0, 100.0, 0.0);
				if (!newslider) {
					_ASSERT(0);
					return 1;
				}
				m_blendshapeSlider.push_back(newslider);

				//WindowSize slidersize = newslider->getSize();//autoResize()が走るまでsizeは(0,0)

				WCHAR strlabel[512] = { 0 };
				ZeroMemory(strlabel, sizeof(WCHAR) * 512);
				if (curblendshape.mqoobj && curblendshape.mqoobj->GetName() && curblendshape.targetname) {
					char mqoobjname[256] = { 0 };
					WCHAR wmqoobjname[256] = { 0L };
					strcpy_s(mqoobjname, 256, curblendshape.mqoobj->GetName());
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
						mqoobjname, -1, wmqoobjname, 256);
					WCHAR wtargetname[256] = { 0L };
					wcscpy_s(wtargetname, 256, curblendshape.targetname);

					//2024/06/08
					//obj名:ターゲット名の長さが長すぎてスライダーに被ることがあったので文字数制限
					//obj名は12文字まで、ターゲット名は最後の .の後から表示
					int objnameleng = (int)wcsnlen_s(wmqoobjname, 256);
					int cp_objnameleng = max(0, min(12, objnameleng));//!!!!!!!!!!
					wmqoobjname[cp_objnameleng] = 0L;//!!!!!!!!!

					//WCHAR printwtargetname[256] = { 0L };
					//ZeroMemory(printwtargetname, sizeof(WCHAR) * 256);
					//int targetnameleng = (int)wcsnlen_s(wtargetname, 256);
					//int cp_targetnameleng = max(0, min(28, targetnameleng));//!!!!!!!!
					//if (cp_targetnameleng > 28) {
					//	int offsetpos = cp_targetnameleng - 28;
					//	wcscpy_s(printwtargetname, 256, (wtargetname + offsetpos));
					//}
					//else {
					//	wcscpy_s(printwtargetname, 256, wtargetname);
					//}

					WCHAR printwtargetname[256] = { 0L };
					ZeroMemory(printwtargetname, sizeof(WCHAR) * 256);
					WCHAR wch[2] = L".";
					WCHAR* lastdot = wcsrchr(wtargetname, wch[0]);
					int offsetpos = 0;
					if (lastdot) {
						offsetpos = (int)(lastdot - wtargetname);
						offsetpos = max(0, min(254, offsetpos));
					}
					else {
						offsetpos = 0;
					}
					wcscpy_s(printwtargetname, 256, (wtargetname + offsetpos));


					ZeroMemory(strlabel, sizeof(WCHAR) * 512);
					wcscpy_s(strlabel, 512, wmqoobjname);
					wcscat_s(strlabel, 512, L":");
					//wcscat_s(strlabel, 512, wtargetname);
					wcscat_s(strlabel, 512, printwtargetname);


					//swprintf_s(strlabel, 512, L"%s:%s",
					//	wmqoobjname, curblendshape.targetname);

				}
				else {
					wcscpy_s(strlabel, 256, L"Unknown Name");
				}

				int sliderheight = 20;
				OWP_Button* newbutton = new OWP_Button(strlabel, sliderheight);//左右カラムの高さ合わせ
				if (!newbutton) {
					_ASSERT(0);
					return 1;
				}
				m_blendshapeButton.push_back(newbutton);

			}
			else {
				_ASSERT(0);
				return 1;
			}
		}

		{
			int labelnum = (int)m_blendshapeButton.size();
			int slidernum = (int)m_blendshapeSlider.size();
			if (labelnum == slidernum) {
				int guino;
				for (guino = 0; guino < labelnum; guino++) {
					OWP_Button* addbutton = m_blendshapeButton[guino];
					OWP_Slider* addslider = m_blendshapeSlider[guino];
					if (!addbutton || !addslider) {
						_ASSERT(0);
						return 1;
					}
					m_blendshapesp1->addParts1(*addbutton);
					m_blendshapesp1->addParts2(*addslider);
				}
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}

		//m_dlgWnd->setCloseListener([=, this]() {
		//	m_closeblendshapeFlag = true;
		//	});


		if (m_blendshapedistSlider) {
			m_blendshapedistSlider->setCursorListener([=, this]() {
				if (m_model && m_dlgWnd && m_blendshapedistSlider) {
					g_blendshapedist = m_blendshapedistSlider->getValue();

					m_dlgWnd->callRewrite();						//再描画
				}
				});
		}

		{
			int lineno2;
			for (lineno2 = 0; lineno2 < m_blendshapelinenum; lineno2++) {
				CBlendShapeElem curblendshape = m_blendshapeelemvec[lineno2];
				if (curblendshape.validflag) {
					if (m_blendshapeSlider[lineno2]) {
						m_blendshapeSlider[lineno2]->setCursorListener([=, this]() {
							OWP_Slider* thisslider = m_blendshapeSlider[lineno2];
							if (m_model && m_dlgWnd && thisslider) {
								CBlendShapeElem curblendshape = m_blendshapeelemvec[lineno2];
								if (curblendshape.validflag && curblendshape.mqoobj) {
									float value = (float)thisslider->getValue();
									//int curmotid = m_model->GetCurrentMotID();
									//double curframe = m_model->GetCurrentFrame();
									//curblendshape.mqoobj->SetShapeAnimWeight(curblendshape.channelindex, 
									//	curmotid, IntTime(curframe), value);

									if (m_blendshapeOpeIndex != lineno2) {
										//選択が切り替わったときには　PrepairUndo_BlendShape()有りのSelChangeイベント
										m_blendshapeUnderSelect = true;
									}

									m_blendshapeOpeIndex = lineno2;
									m_blendshapeAfter = value;
									m_blendshapeUnderEdit = true;
								}
								m_dlgWnd->callRewrite();						//再描画
							}
							});

						m_blendshapeSlider[lineno2]->setLDownListener([=, this]() {
							OWP_Slider* thisslider = m_blendshapeSlider[lineno2];
							if (m_model && m_dlgWnd && thisslider) {
								CBlendShapeElem curblendshape = m_blendshapeelemvec[lineno2];
								if (curblendshape.validflag && curblendshape.mqoobj) {
									float value = (float)thisslider->getValue();
									m_blendshapeBefore = value;//!!!!!!!!
								}
								m_dlgWnd->callRewrite();						//再描画
							}
							});


						m_blendshapeSlider[lineno2]->setLUpListener([=, this]() {
							OWP_Slider* thisslider = m_blendshapeSlider[lineno2];
							if (m_model && m_dlgWnd && thisslider) {
								CBlendShapeElem curblendshape = m_blendshapeelemvec[lineno2];
								if (curblendshape.validflag && curblendshape.mqoobj) {
									float value = (float)thisslider->getValue();
									m_blendshapeOpeIndex = lineno2;
									m_blendshapeAfter = value;
									m_blendshapePostEdit = true;
								}
								m_dlgWnd->callRewrite();						//再描画
							}
							});


						//m_blendshapeBefore = 0.0f;
					}


					if (m_blendshapeButton[lineno2]) {
						m_blendshapeButton[lineno2]->setButtonListener([=, this]() {
							OWP_Button* thisbutton = m_blendshapeButton[lineno2];
							if (m_model && m_dlgWnd && thisbutton) {
								if (g_mainhwnd && IsWindow(g_mainhwnd)) {
									PostMessage(g_mainhwnd, WM_COMMAND, 
										(ID_RMENU_0 + MENUOFFSET_BLENDSHAPEDLG), (LPARAM)lineno2);
								}

								//CBlendShapeElem curblendshape = m_blendshapeelemvec[lineno2];
								//if (curblendshape.validflag && curblendshape.mqoobj &&
								//	!m_undoFlag && !m_redoFlag &&
								//	!m_blendshapeUnderSelectFromUndo && !m_blendshapeUnderSelectFromRefresh) {

								//	m_blendshapeOpeIndex = lineno2;
								//	m_blendshapeUnderSelect = true;//2024/06/30
								//}
								m_dlgWnd->callRewrite();						//再描画
							}
							});
					}

				}
				else {
					_ASSERT(0);
					return 1;
				}
			}
		}



		////autoResizeしないと　チェックボックス４段目以下が反応なかった
		m_blendshapeSCWnd->autoResize();
		m_blendshapedistsp->autoResize();
		m_blendshapesp1->autoResize();
		m_blendshapesp0->autoResize();

		m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgWnd->setPos(WindowPos(m_posx, m_posy));
		//１クリック目問題対応
		m_dlgWnd->refreshPosAndSize();
		m_dlgWnd->autoResizeAllParts();
		m_dlgWnd->setVisible(false);

		//#######
		//for dbg
		//#######
		//WindowSize dbgsize = m_blendshapeSlider[0]->getSize();
		//int dbgflag1 = 1;
	}


	return 0;
}

int CBlendShapeDlg::ParamsToDlg()
{
	if (!m_model) {
		return 0;
	}
	if (!m_dlgWnd) {
		return 0;
	}
	if (m_dlgWnd->getVisible() != true) {
		return 0;
	}
	if (m_model->ExistCurrentMotion() != true) {
		return 0;
	}

	int blendelemnum = (int)m_blendshapeelemvec.size();
	int blendelemindex;
	for (blendelemindex = 0; blendelemindex < blendelemnum; blendelemindex++) {
		CBlendShapeElem blendelem = m_blendshapeelemvec[blendelemindex];
		if (blendelem.validflag && blendelem.model && blendelem.mqoobj) {
			int curmotid = blendelem.model->GetCurrentMotID();
			double curframe = blendelem.model->GetCurrentFrame();
			float value = blendelem.mqoobj->GetShapeAnimWeight(curmotid, IntTime(curframe), blendelem.channelindex);

			int slidernum = (int)m_blendshapeSlider.size();
			if ((blendelemindex >= 0) && (blendelemindex < slidernum) && m_blendshapeSlider[blendelemindex]) {
				m_blendshapeSlider[blendelemindex]->setValue(value, false);
			}
		}
	}

	m_dlgWnd->callRewrite();

	return 0;
}

const HWND CBlendShapeDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CBlendShapeDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CBlendShapeDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

