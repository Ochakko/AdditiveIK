#include "stdafx.h"

#include <LaterTransparentDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <mqoobject.h>
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
// CLaterTransparentDlg

CLaterTransparentDlg::CLaterTransparentDlg()
{
	InitParams();
}

CLaterTransparentDlg::~CLaterTransparentDlg()
{
	DestroyObjs();
}
	
int CLaterTransparentDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_laterlistLabel) {
		delete m_laterlistLabel;
		m_laterlistLabel = nullptr;
	}
	if (m_laterlist1Sc) {
		delete m_laterlist1Sc;
		m_laterlist1Sc = nullptr;
	}
	if (m_laterlist1List) {
		delete m_laterlist1List;
		m_laterlist1List = nullptr;
	}
	if (m_laterspacerLabel1) {
		delete m_laterspacerLabel1;
		m_laterspacerLabel1 = nullptr;
	}
	if (m_lateraddB) {
		delete m_lateraddB;
		m_lateraddB = nullptr;
	}
	if (m_laterspacerLabel2) {
		delete m_laterspacerLabel2;
		m_laterspacerLabel2 = nullptr;
	}
	if (m_laterlist2Sc) {
		delete m_laterlist2Sc;
		m_laterlist2Sc = nullptr;
	}
	if (m_laterlist2List) {
		delete m_laterlist2List;
		m_laterlist2List = nullptr;
	}
	if (m_laterspacerLabel3) {
		delete m_laterspacerLabel3;
		m_laterspacerLabel3 = nullptr;
	}
	if (m_latersp1) {
		delete m_latersp1;
		m_latersp1 = nullptr;
	}
	if (m_latersp2) {
		delete m_latersp2;
		m_latersp2 = nullptr;
	}
	if (m_latersp3) {
		delete m_latersp3;
		m_latersp3 = nullptr;
	}
	if (m_laterdelallB) {
		delete m_laterdelallB;
		m_laterdelallB = nullptr;
	}
	if (m_laterdelB) {
		delete m_laterdelB;
		m_laterdelB = nullptr;
	}
	if (m_laterupB) {
		delete m_laterupB;
		m_laterupB = nullptr;
	}
	if (m_laterdownB) {
		delete m_laterdownB;
		m_laterdownB = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}



	return 0;
}

void CLaterTransparentDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;

	m_dlgWnd = nullptr;

	m_laterlistLabel = nullptr;
	m_laterlist1Sc = nullptr;
	m_laterlist1List = nullptr;
	m_laterspacerLabel1 = nullptr;
	m_lateraddB = nullptr;
	m_laterspacerLabel2 = nullptr;
	m_laterlist2Sc = nullptr;
	m_laterlist2List = nullptr;
	m_laterspacerLabel3 = nullptr;
	m_latersp1 = nullptr;
	m_latersp2 = nullptr;
	m_latersp3 = nullptr;
	m_laterdelallB = nullptr;
	m_laterdelB = nullptr;
	m_laterupB = nullptr;
	m_laterdownB = nullptr;

}

int CLaterTransparentDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

void CLaterTransparentDlg::SetModel(CModel* srcmodel)
{
	m_model = srcmodel;
	CreateLaterTransparentWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		//if (m_groupSCWnd) {
		//	m_groupSCWnd->autoResize();
		//}
		m_dlgWnd->callRewrite();
	}
}


void CLaterTransparentDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			//if (m_groupSCWnd) {
			//	//############
			//	//2024/07/24
			//	//############
			//	//int showposline = m_dlgSc->getShowPosLine();
			//	//m_dlgSc->setShowPosLine(showposline);
			//	//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
			//	//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
			//	//スクロールバーを少し動かすと全て表示された
			//	//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
			//	m_groupSCWnd->autoResize();
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


int CLaterTransparentDlg::CreateLaterTransparentWnd()
{

	DestroyObjs();

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}


	m_dlgWnd = new OrgWindow(
		0,
		_T("LaterTransparentDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("LaterTransparentDlg"),	//タイトル
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
		double rate50 = 0.50;
		int initlinenum = 10;

		m_laterlistLabel = new OWP_Label(L"Texture Files", labelheight);
		if (!m_laterlistLabel) {
			_ASSERT(0);
			abort();
		}
		//m_laterlist1Sc = new OWP_ScrollWnd(L"List1Scroll", false, labelheight);
		//if (!m_laterlist1Sc) {
		//	_ASSERT(0);
		//	abort();
		//}
		m_laterlist1List = new OWP_ListBox(L"List1", initlinenum, labelheight);
		if (!m_laterlist1List) {
			_ASSERT(0);
			abort();
		}
		m_laterspacerLabel1 = new OWP_Label(L"     ", labelheight);
		if (!m_laterspacerLabel1) {
			_ASSERT(0);
			abort();
		}
		m_lateraddB = new OWP_Button(L"↓Add to LaterTransparent", 32);
		if (!m_lateraddB) {
			_ASSERT(0);
			abort();
		}
		m_laterspacerLabel2 = new OWP_Label(L"     ", labelheight);
		if (!m_laterspacerLabel2) {
			_ASSERT(0);
			abort();
		}
		//m_laterlist2Sc = new OWP_ScrollWnd(L"List2Scroll", false, labelheight);
		//if (!m_laterlist2Sc) {
		//	_ASSERT(0);
		//	abort();
		//}
		m_laterlist2List = new OWP_ListBox(L"List2", initlinenum, labelheight);
		if (!m_laterlist2List) {
			_ASSERT(0);
			abort();
		}
		m_laterspacerLabel3 = new OWP_Label(L"     ", labelheight);
		if (!m_laterspacerLabel3) {
			_ASSERT(0);
			abort();
		}
		m_latersp1 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_latersp1) {
			_ASSERT(0);
			abort();
		}
		m_latersp2 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_latersp2) {
			_ASSERT(0);
			abort();
		}
		m_latersp3 = new OWP_Separator(m_dlgWnd, true, rate50, true);
		if (!m_latersp3) {
			_ASSERT(0);
			abort();
		}
		m_laterdelallB = new OWP_Button(L"Delete All", labelheight);
		if (!m_laterdelallB) {
			_ASSERT(0);
			abort();
		}
		m_laterdelB = new OWP_Button(L"Delete", labelheight);
		if (!m_laterdelB) {
			_ASSERT(0);
			abort();
		}
		m_laterupB = new OWP_Button(L"↑ Up", labelheight);;
		if (!m_laterupB) {
			_ASSERT(0);
			abort();
		}
		m_laterdownB = new OWP_Button(L"↓ Down", labelheight);
		if (!m_laterdownB) {
			_ASSERT(0);
			abort();
		}



		m_dlgWnd->addParts(*m_laterlistLabel);
		//m_dlgWnd->addParts(*m_laterlist1Sc);
		//m_laterlist1Sc->addParts(*m_laterlist1List);
		m_dlgWnd->addParts(*m_laterlist1List);
		m_dlgWnd->addParts(*m_laterspacerLabel1);
		m_dlgWnd->addParts(*m_lateraddB);
		m_dlgWnd->addParts(*m_laterspacerLabel2);
		//m_dlgWnd->addParts(*m_laterlist2Sc);
		//m_laterlist2Sc->addParts(*m_laterlist2List);
		m_dlgWnd->addParts(*m_laterlist2List);
		m_dlgWnd->addParts(*m_laterspacerLabel3);
		m_dlgWnd->addParts(*m_latersp1);
		m_latersp1->addParts1(*m_latersp2);
		m_latersp1->addParts2(*m_latersp3);
		m_latersp2->addParts1(*m_laterdelallB);
		m_latersp2->addParts2(*m_laterdelB);
		m_latersp3->addParts1(*m_laterupB);
		m_latersp3->addParts2(*m_laterdownB);

		m_lateraddB->setButtonListener([=, this]() {
			if (m_laterlist1List && m_laterlist2List) {
				wstring strlist1 = m_laterlist1List->getCurrentLineName();
				if (!m_laterlist2List->existName(strlist1)) {
					m_laterlist2List->newLine(strlist1);
					m_laterlist2List->setCurrentLineName(strlist1);
					Dlg2LaterTransparent();
				}
			}
			});
		m_laterdelallB->setButtonListener([=, this]() {
			if (m_laterlist2List) {
				m_laterlist2List->deleteLine();
			}
			});
		m_laterdelB->setButtonListener([=, this]() {
			if (m_laterlist2List) {
				int currentline = m_laterlist2List->getCurrentLine();
				if (currentline >= 0) {
					m_laterlist2List->deleteLine(currentline);
					Dlg2LaterTransparent();
				}
			}
			});
		m_laterupB->setButtonListener([=, this]() {
			if (m_laterlist2List) {
				int currentline = m_laterlist2List->getCurrentLine();
				if (currentline >= 0) {
					m_laterlist2List->upLine(currentline);
					Dlg2LaterTransparent();
				}
			}
			});
		m_laterdownB->setButtonListener([=, this]() {
			if (m_laterlist2List) {
				int currentline = m_laterlist2List->getCurrentLine();
				if (currentline >= 0) {
					m_laterlist2List->downLine(currentline);
					Dlg2LaterTransparent();
				}
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

int CLaterTransparentDlg::ParamsToDlg()
{
	if (!m_dlgWnd) {
		_ASSERT(0);
		return 1;
	}
	if (!m_laterlist1List || !m_laterlist2List) {
		_ASSERT(0);
		return 1;
	}
	m_laterlist1List->deleteLine();
	m_laterlist2List->deleteLine();

	if (!m_model) {
		return 0;
	}

	vector<string> modelstexturevec;
	int result = m_model->GetTextureNameVec(modelstexturevec);
	if (result == 0) {
		int texturenum = (int)modelstexturevec.size();

		int listno;
		for (listno = 0; listno < texturenum; listno++) {
			string curtexname = modelstexturevec[listno];
			if (curtexname.c_str() && (*curtexname.c_str() != 0)) {
				char mbtexname[512] = { 0 };
				strcpy_s(mbtexname, 512, curtexname.c_str());
				WCHAR wctexname[512] = { 0L };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbtexname, 512, wctexname, 512);

				m_laterlist1List->newLine(wctexname);
			}
		}


		int laternum = m_model->GetLaterTransparentNum();
		int listno2;
		for (listno2 = 0; listno2 < laternum; listno2++) {
			string curlatername = m_model->GetLaterTransparent(listno2);
			if (curlatername.c_str() && (*curlatername.c_str() != 0)) {
				char mblastername[512] = { 0 };
				strcpy_s(mblastername, 512, curlatername.c_str());
				WCHAR wclatername[512] = { 0L };
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mblastername, 512, wclatername, 512);

				m_laterlist2List->newLine(wclatername);
			}
		}

	}
	else {
		_ASSERT(0);
		return 1;
	}

	if (m_dlgWnd) {
		m_dlgWnd->callRewrite();
	}

	return 0;
}

int CLaterTransparentDlg::Dlg2LaterTransparent()
{
	if (!m_model) {
		return 0;
	}

	if (!m_dlgWnd || !m_laterlist2List) {
		_ASSERT(0);
		return 1;
	}


	int elemnum = m_laterlist2List->getLineNum();

	//リスト２をsavelist2に格納
	vector<wstring> savelist2;
	int elemno;
	for (elemno = 0; elemno < elemnum; elemno++) {
		wstring currentname = m_laterlist2List->getName(elemno);
		if (currentname.c_str() && (*currentname.c_str() != 0L) && (currentname != L"NoData")) {
			savelist2.push_back(currentname);
		}
	}

	int result2 = m_model->SetLaterTransparentVec(savelist2);//丸ごと設定
	if (result2 != 0) {
		_ASSERT(0);
		return 1;
	}

	return 0;
}


const HWND CLaterTransparentDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CLaterTransparentDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CLaterTransparentDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

