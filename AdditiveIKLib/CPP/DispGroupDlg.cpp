#include "stdafx.h"

#include <DispGroupDlg.h>
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
// CDispGroupDlg

CDispGroupDlg::CDispGroupDlg()
{
	InitParams();
}

CDispGroupDlg::~CDispGroupDlg()
{
	DestroyObjs();
}
	
int CDispGroupDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_groupsetB) {
		delete m_groupsetB;
		m_groupsetB = nullptr;
	}
	if (m_groupgetB) {
		delete m_groupgetB;
		m_groupgetB = nullptr;
	}
	if (m_grouponB) {
		delete m_grouponB;
		m_grouponB = nullptr;
	}
	if (m_groupoffB) {
		delete m_groupoffB;
		m_groupoffB = nullptr;
	}
	if (m_groupclearB) {
		delete m_groupclearB;
		m_groupclearB = nullptr;
	}
	if (m_grouplabel11) {
		delete m_grouplabel11;
		m_grouplabel11 = nullptr;
	}
	if (m_grouplabel12) {
		delete m_grouplabel12;
		m_grouplabel12 = nullptr;
	}
	if (m_grouplabel21) {
		delete m_grouplabel21;
		m_grouplabel21 = nullptr;
	}
	if (m_grouplabel22) {
		delete m_grouplabel22;
		m_grouplabel22 = nullptr;
	}

	int selno;
	for (selno = 0; selno < MAXDISPGROUPNUM; selno++) {
		if (m_groupselect[selno]) {
			delete m_groupselect[selno];
		}
		m_groupselect[selno] = nullptr;
	}


	int objno;
	size_t objnum = m_groupobjvec.size();
	for (objno = 0; objno < objnum; objno++) {
		if (m_groupobjvec[objno]) {
			delete m_groupobjvec[objno];
		}
	}
	m_groupobjvec.clear();

	int testno;
	size_t testnum = m_grouptestBvec.size();
	for (testno = 0; testno < testnum; testno++) {
		if (m_grouptestBvec[testno]) {
			delete m_grouptestBvec[testno];
		}
	}
	m_grouptestBvec.clear();

	m_grouplinenum = 0;


	if (m_groupsp3) {
		delete m_groupsp3;
		m_groupsp3 = nullptr;
	}
	if (m_groupsp2) {
		delete m_groupsp2;
		m_groupsp2 = nullptr;
	}
	if (m_groupsp1) {
		delete m_groupsp1;
		m_groupsp1 = nullptr;
	}
	if (m_groupsp) {
		delete m_groupsp;
		m_groupsp = nullptr;
	}
	if (m_groupsp0) {
		delete m_groupsp0;
		m_groupsp0 = nullptr;
	}
	if (m_groupSCWnd) {
		delete m_groupSCWnd;
		m_groupSCWnd = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	m_groupUnderGetting = false;//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ
	m_groupmqoobjvec.clear();
	m_grouplinenum = 0;
	m_disponlyoneobj = false;//for test button of groupWnd
	m_onlyoneobjno = -1;//for test button of groupWnd

	m_checksimilarFlag = false;
	m_checksimilarobjno = 0;


	return 0;
}

void CDispGroupDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;

	m_dlgWnd = nullptr;

	m_groupSCWnd = nullptr;
	m_groupsp0 = nullptr;
	m_groupsp = nullptr;
	m_groupsp1 = nullptr;
	m_groupsp2 = nullptr;
	m_groupsp3 = nullptr;
	int groupindex;
	for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
		m_groupselect[groupindex] = nullptr;
	}
	m_groupsetB = nullptr;
	m_groupgetB = nullptr;
	m_grouponB = nullptr;
	m_groupoffB = nullptr;
	m_groupclearB = nullptr;
	m_grouplabel11 = nullptr;
	m_grouplabel12 = nullptr;
	m_grouplabel21 = nullptr;
	m_grouplabel22 = nullptr;
	m_groupobjvec.clear();
	m_grouptestBvec.clear();


	m_groupUnderGetting = false;//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ
	m_groupmqoobjvec.clear();
	m_grouplinenum = 0;
	m_disponlyoneobj = false;//for test button of groupWnd
	m_onlyoneobjno = -1;//for test button of groupWnd

	m_checksimilarFlag = false;
	m_checksimilarobjno = 0;

}

int CDispGroupDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

void CDispGroupDlg::SetModel(CModel* srcmodel)
{
	m_model = srcmodel;
	CreateDispGroupWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		if (m_groupSCWnd) {
			m_groupSCWnd->autoResize();
		}
		m_dlgWnd->callRewrite();
	}
}


void CDispGroupDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_groupSCWnd) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_groupSCWnd->autoResize();
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


int CDispGroupDlg::CreateDispGroupWnd()
{

	DestroyObjs();

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}

	HCURSOR oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));//数秒時間がかかることがあるので砂時計カーソルにする

	m_dlgWnd = new OrgWindow(
		0,
		_T("DispGroupWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("DispGroupWindow"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		true,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true, true);					//サイズ変更の可否

	int labelheight;
	if (g_4kresolution) {
		labelheight = 24;
	}
	else {
		labelheight = 20;
	}

	if (m_dlgWnd) {

		m_dlgWnd->setSizeMin(WindowSize(150, 150));		// 最小サイズを設定


		m_groupsetB = new OWP_Button(L"Set", labelheight);
		if (!m_groupsetB) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupgetB = new OWP_Button(L"Get", labelheight);
		if (!m_groupgetB) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		//m_grouptestB = new OWP_Button(L"Test");
		//if (!m_grouptestB) {
		//	_ASSERT(0);
		//	return 1;
		//}
		m_grouponB = new OWP_Button(L"ON", labelheight);
		if (!m_grouponB) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupoffB = new OWP_Button(L"OFF", labelheight);
		if (!m_groupoffB) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupclearB = new OWP_Button(L"Clear", labelheight);
		if (!m_groupclearB) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_grouplabel11 = new OWP_Label(L"---------", labelheight);
		if (!m_grouplabel11) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_grouplabel12 = new OWP_Label(L"---------", labelheight);
		if (!m_grouplabel12) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_grouplabel21 = new OWP_Label(L"---------", labelheight);
		if (!m_grouplabel21) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_grouplabel22 = new OWP_Label(L"---------", labelheight);
		if (!m_grouplabel22) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}


		int groupindex0;
		for (groupindex0 = 0; groupindex0 < MAXDISPGROUPNUM; groupindex0++) {
			WCHAR groupname[256] = { 0L };
			swprintf_s(groupname, 256, L"%02d", groupindex0 + 1);
			m_groupselect[groupindex0] = new OWP_CheckBoxA(groupname, false, labelheight, true);
			if (!m_groupselect[groupindex0]) {
				_ASSERT(0);
				if (oldcursor != NULL) {
					SetCursor(oldcursor);
				}
				return 1;
			}
		}

		int result = m_model->SetDispGroupGUI(m_groupobjvec, m_groupmqoobjvec, labelheight);
		if (result != 0) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_grouplinenum = (int)m_groupobjvec.size();

		if (m_grouplinenum <= 0) {
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 0;
		}


		double centerrate;
		int linedatasize;
		if (g_4kresolution) {
			//centerrate = (double)12 / (double)140;
			centerrate = (double)12 / (double)140 * 1.50;
			//linedatasize = max(140, linenum + 12);
			//linedatasize = max(106, (linenum + 12));
			linedatasize = (int)((double)m_grouplinenum * 1.5);
		}
		else {
			//centerrate = (double)12 / (double)70;
			centerrate = (double)12 / (double)70 * 1.50;
			//linedatasize = max(70, linenum + 12);
			//linedatasize = max(54, (linenum + 12));
			linedatasize = (int)((double)m_grouplinenum * 1.5);
		}



		//スクロールウインドウ		
		m_groupSCWnd = new OWP_ScrollWnd(L"DispGroupScWnd", true, labelheight);
		if (!m_groupSCWnd) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupSCWnd->setLineDataSize(linedatasize);//!!!!!!!!!!!!!
		//m_dlgWnd->addParts(*m_groupSCWnd);



		//m_groupsp0 = new OWP_Separator(m_dlgWnd, false, centerrate, false);//上段と下段を格納
		m_groupsp0 = new OWP_Separator(m_dlgWnd, true, centerrate, false);//上段と下段を格納
		if (!m_groupsp0) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		//m_groupSCWnd->addParts(*m_groupsp0);
		m_dlgWnd->addParts(*m_groupsp0);


		//m_groupsp = new OWP_Separator(m_dlgWnd, false, 0.5, true);//ウインドウ上段部分用
		m_groupsp = new OWP_Separator(m_dlgWnd, true, 0.5, true);//ウインドウ上段部分用
		if (!m_groupsp) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupsp0->addParts1(*m_groupsp);


		m_groupsp0->addParts2(*m_groupSCWnd);
		//m_groupsp3 = new OWP_Separator(m_dlgWnd, false, 0.8, true, m_groupSCWnd);//parent : m_groupSCWnd　下段　objチェックボックスとtestボタン用
		m_groupsp3 = new OWP_Separator(m_dlgWnd, true, 0.8, true, m_groupSCWnd);//parent : m_groupSCWnd　下段　objチェックボックスとtestボタン用
		if (!m_groupsp3) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupSCWnd->addParts(*m_groupsp3);
		int lineno;
		for (lineno = 0; lineno < m_grouplinenum; lineno++) {
			m_groupsp3->addParts1(*(m_groupobjvec[lineno]));

			OWP_Button* testbutton = new OWP_Button(L"Test", labelheight);
			if (!testbutton) {
				_ASSERT(0);
				if (oldcursor != NULL) {
					SetCursor(oldcursor);
				}
				return 1;
			}
			m_grouptestBvec.push_back(testbutton);
			m_groupsp3->addParts2(*(m_grouptestBvec[lineno]));
		}


		//m_groupsp1 = new OWP_Separator(m_dlgWnd, false, 0.5, true);//上段　グループ番号チェックボックス用
		m_groupsp1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);//上段　グループ番号チェックボックス用
		if (!m_groupsp1) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupsp->addParts1(*m_groupsp1);

		//m_groupsp2 = new OWP_Separator(m_dlgWnd, false, 0.5, true);//上段　グループ番号チェックボックス用
		m_groupsp2 = new OWP_Separator(m_dlgWnd, true, 0.5, true);//上段　グループ番号チェックボックス用
		if (!m_groupsp2) {
			_ASSERT(0);
			if (oldcursor != NULL) {
				SetCursor(oldcursor);
			}
			return 1;
		}
		m_groupsp->addParts2(*m_groupsp2);

		for (groupindex0 = 0; groupindex0 < MAXDISPGROUPNUM; groupindex0++) {
			int colno = groupindex0 % 4;
			if (colno == 0) {
				m_groupsp1->addParts1(*m_groupselect[groupindex0]);
			}
			else if (colno == 1) {
				m_groupsp1->addParts2(*m_groupselect[groupindex0]);
			}
			else if (colno == 2) {
				m_groupsp2->addParts1(*m_groupselect[groupindex0]);
			}
			else {
				m_groupsp2->addParts2(*m_groupselect[groupindex0]);
			}
		}

		m_groupsp1->addParts1(*m_grouplabel11);
		m_groupsp1->addParts2(*m_grouplabel12);
		m_groupsp2->addParts1(*m_grouplabel21);
		m_groupsp2->addParts2(*m_grouplabel22);

		m_groupsp1->addParts1(*m_groupsetB);
		m_groupsp1->addParts2(*m_groupgetB);
		m_groupsp1->addParts1(*m_groupclearB);
		m_groupsp2->addParts1(*m_grouponB);
		m_groupsp2->addParts2(*m_groupoffB);

		//m_groupsp2->addParts2(*m_grouptestB);

		{//testボタンのラムダ関数
			int lineno1;
			for (lineno1 = 0; lineno1 < m_grouplinenum; lineno1++) {
				if (m_grouptestBvec[lineno1]) {
					m_grouptestBvec[lineno1]->setButtonListener([=, this]() {

						HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));

						//ボタンのtext色をリセット
						int lineno;
						for (lineno = 0; lineno < m_grouplinenum; lineno++) {
							COLORREF normalcol = RGB(255, 255, 255);
							m_grouptestBvec[lineno]->setTextColor(normalcol);
						}

						bool currentstate = m_disponlyoneobj;

						if (lineno1 == m_onlyoneobjno) {
							//現在表示中のobjをオフにした場合にだけ　m_disponlyoneobjをオフにする
							//他のobjのTestボタンを押した場合には　m_disponlyoneobjオンのまま　表示objを変更する

							m_disponlyoneobj = false;//!!!!!!!!
							m_onlyoneobjno = -1;//!!!!!!!!
						}
						else {
							m_disponlyoneobj = true;//!!!!!!!!
							m_onlyoneobjno = lineno1;//!!!!!!!!

							COLORREF importantcol = RGB(168, 129, 129);
							m_grouptestBvec[lineno1]->setTextColor(importantcol);
						}

						if (oldcursor1) {
							SetCursor(oldcursor1);
						}
						});
				}
			}
		}

		{//groupselectボタンのラムダ関数
			int groupindex;
			for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
				if (m_groupselect[groupindex]) {
					m_groupselect[groupindex]->setButtonListener([=, this]() {
						bool ischecked = m_groupselect[groupindex]->getValue();
						if (ischecked) {
							//チェックの場合には　他のグループセレクトボタンはチェックを外す(排他的)
							int groupindex2;
							for (groupindex2 = 0; groupindex2 < MAXDISPGROUPNUM; groupindex2++) {
								if ((groupindex2 != groupindex) && m_groupselect[groupindex2]) {
									m_groupselect[groupindex2]->setValue(false, false);
								}
							}
						}
						if (m_dlgWnd) {
							m_dlgWnd->callRewrite();
						}
						});
				}
			}
		}

		{//setボタンのラムダ関数
			if (m_groupsetB) {
				m_groupsetB->setButtonListener([=, this]() {
					if (m_model) {

						HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));

						//選択中のグループ番号を取得
						int selectedgroupno = 0;
						int groupindex;
						for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
							if (m_groupselect[groupindex] && m_groupselect[groupindex]->getValue()) {
								selectedgroupno = groupindex + 1;//groupno = groupindex + 1
								break;
							}
						}

						//チェックの付いているobjectに対してselectedgroupindexを設定
						if (selectedgroupno >= 1) {
							int lineno1;
							for (lineno1 = 0; lineno1 < m_grouplinenum; lineno1++) {
								if (m_groupobjvec[lineno1] && m_groupobjvec[lineno1]->getValue()) {
									m_model->SetDispGroup(selectedgroupno - 1, lineno1);
								}
							}
							m_model->MakeDispGroupForRender();
						}

						if (oldcursor1) {
							SetCursor(oldcursor1);
						}

					}
					});
			}
		}

		{//getボタンのラムダ関数
			if (m_groupgetB) {
				m_groupgetB->setButtonListener([=, this]() {
					if (m_model) {

						m_groupUnderGetting = true;//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ

						HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));


						//objectのチェックを全てリセット
						int lineno1;
						for (lineno1 = 0; lineno1 < m_grouplinenum; lineno1++) {
							if (m_groupobjvec[lineno1]) {
								bool calllistener = false;
								m_groupobjvec[lineno1]->setValue(false, calllistener);
							}
						}


						//選択中のグループ番号を取得
						int selectedgroupno = 0;
						int groupindex;
						for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
							if (m_groupselect[groupindex] && m_groupselect[groupindex]->getValue()) {
								selectedgroupno = groupindex + 1;//groupno = groupindex + 1
								break;
							}
						}

						//selectedgroupnoに属するgroupobjvecチェックボックスにチェックを入れる
						if (selectedgroupno >= 1) {

							//selectedgroupnoに属するobjectを取得
							vector<DISPGROUPELEM> digvec;
							digvec.clear();
							m_model->GetDispGroupForRender(selectedgroupno - 1, digvec);//groupindex = groupno - 1

							//objvecにチェックを入れる
							int digvecsize = (int)digvec.size();
							int digno;
							for (digno = 0; digno < digvecsize; digno++) {
								DISPGROUPELEM digelem = digvec[digno];
								int objno = digelem.objno;
								if ((objno >= 0) && (objno < m_grouplinenum) && m_groupobjvec[objno]) {
									bool calllistener = false;
									m_groupobjvec[objno]->setValue(true, calllistener);
								}
							}
							if (m_dlgWnd) {
								m_dlgWnd->callRewrite();
							}
						}

						if (oldcursor1) {
							SetCursor(oldcursor1);
						}

						m_groupUnderGetting = false;//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ
					}
					});
			}
		}

		{//ONボタン
			if (m_grouponB) {
				m_grouponB->setButtonListener([=, this]() {
					if (m_model) {
						HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));

						//選択中のグループ番号を取得
						int selectedgroupno = 0;
						int groupindex;
						for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
							if (m_groupselect[groupindex] && m_groupselect[groupindex]->getValue()) {
								selectedgroupno = groupindex + 1;//groupno = groupindex + 1
								break;
							}
						}

						//selectedgroupnoの表示をオン
						if (selectedgroupno >= 1) {//groupindex = groupno - 1
							m_model->SetDispGroupON(selectedgroupno - 1, true);
						}

						if (oldcursor1) {
							SetCursor(oldcursor1);
						}
					}
					});
			}
		}

		{//OFFボタン
			if (m_groupoffB) {
				m_groupoffB->setButtonListener([=, this]() {
					if (m_model) {

						HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));

						//選択中のグループ番号を取得
						int selectedgroupno = 0;
						int groupindex;
						for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
							if (m_groupselect[groupindex] && m_groupselect[groupindex]->getValue()) {
								selectedgroupno = groupindex + 1;//groupno = groupindex + 1
								break;
							}
						}

						//selectedgroupnoの表示をオフ
						if (selectedgroupno >= 1) {//groupindex = groupno - 1
							m_model->SetDispGroupON(selectedgroupno - 1, false);
						}

						if (oldcursor1) {
							SetCursor(oldcursor1);
						}

					}
					});
			}
		}

		{//Clearボタン
			if (m_groupclearB) {
				m_groupclearB->setButtonListener([=, this]() {
					HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));

					int objno1;
					for (objno1 = 0; objno1 < m_grouplinenum; objno1++) {
						if (m_groupobjvec[objno1]) {
							bool calllistener = false;
							m_groupobjvec[objno1]->setValue(false, calllistener);
						}
					}

					if (oldcursor1) {
						SetCursor(oldcursor1);
					}

					});
			}
		}


		{//objvecボタン

			//チェックを入れたobjectの子供treeも一緒にチェックを入れる
			int objno1;
			for (objno1 = 0; objno1 < m_grouplinenum; objno1++) {
				if (m_groupobjvec[objno1]) {

					//左クリックで　チェックボックスのチェックをオンオフ
					m_groupobjvec[objno1]->setButtonListener([=, this]() {
						if (m_model) {
							if (m_groupUnderGetting == false) {//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ)								
								HCURSOR oldcursor1 = SetCursor(LoadCursor(NULL, IDC_WAIT));

								vector<int> selectedobjtree;
								selectedobjtree.clear();
								m_model->GetSelectedObjTree(objno1, selectedobjtree);//objno1の子供のobjectTreeを取得


								bool newstate;
								if (m_groupobjvec[objno1] && m_groupobjvec[objno1]->getValue()) {
									newstate = true;
								}
								else {
									newstate = false;
								}


								int selectedobjnum = (int)selectedobjtree.size();
								int objindex;
								for (objindex = 0; objindex < selectedobjnum; objindex++) {
									int selectedobjno = selectedobjtree[objindex];
									if ((selectedobjno >= 0) && (selectedobjno < m_grouplinenum) && (selectedobjno != objno1)) {
										bool calllistener = false;
										m_groupobjvec[selectedobjno]->setValue(newstate, calllistener);
									}
								}

								if (oldcursor1) {
									SetCursor(oldcursor1);
								}
							}
						}
						});


					//右クリックメニュー
					m_groupobjvec[objno1]->setContextMenuListener([=, this]() {
						if (m_model && (m_groupUnderGetting == false) && (m_checksimilarFlag == false)) {//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ)								
							m_checksimilarFlag = true;
							m_checksimilarobjno = objno1;
						}
						});

				}
			}
		}



		//autoResizeしないと　チェックボックス４段目以下が反応なかった
		//m_groupSCWnd->autoResize();
		//m_groupsp3->autoResize();
		//m_groupsp1->autoResize();
		//m_groupsp2->autoResize();
		//m_groupsp->autoResize();
		//m_groupsp0->autoResize();

		m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgWnd->setPos(WindowPos(m_posx, m_posy));
		//１クリック目問題対応
		m_dlgWnd->refreshPosAndSize();
		m_dlgWnd->autoResizeAllParts();
		m_dlgWnd->setVisible(false);
	}


	if (oldcursor != NULL) {
		SetCursor(oldcursor);
	}

	return 0;
}

int CDispGroupDlg::CheckSimilarGroup(int opetype)
{
	if (!m_model) {
		return 0;
	}

	if ((m_grouplinenum > 0) &&
		(opetype >= 0) && (opetype <= 7) &&
		(m_checksimilarobjno >= 0) && (m_checksimilarobjno < m_grouplinenum)) {

		OWP_CheckBoxA* srccheckbox = m_groupobjvec[m_checksimilarobjno];
		if (srccheckbox) {
			WCHAR objname[512] = { 0L };
			int result = srccheckbox->getName(objname, 512);
			if ((result == 0) && (objname[0] != 0L)) {
				bool secondtokenflag;
				if (opetype <= 3) {
					secondtokenflag = false;
				}
				else {
					secondtokenflag = true;
				}
				WCHAR similarpattern[512] = { 0L };
				int result0 = TrimLeadingAlnum(secondtokenflag, objname, 512, similarpattern, 512, false);
				if ((result0 == 0) && (similarpattern[0] != 0L)) {

					int pattern0len = (int)wcslen(similarpattern);

					if ((opetype == 0) || (opetype == 1) ||
						(opetype == 4) || (opetype == 5)) {
						//#####################
						//pattern include num
						//#####################

						int objno1;
						for (objno1 = 0; objno1 < m_grouplinenum; objno1++) {
							if (m_groupobjvec[objno1]) {
								WCHAR chkname[512] = { 0L };
								int result1 = m_groupobjvec[objno1]->getName(chkname, 512);
								if ((result == 0) && (chkname[0] != 0L)) {
									WCHAR* findptr = wcsstr(chkname, similarpattern);//check if pattern is included
									if (findptr) {
										bool calllistener = false;
										if ((opetype == 0) || (opetype == 4)) {
											m_groupobjvec[objno1]->setValue(true, calllistener);
										}
										else if ((opetype == 1) || (opetype == 5)) {
											m_groupobjvec[objno1]->setValue(false, calllistener);
										}
										else {
											_ASSERT(0);
											return 1;
										}
									}
								}
							}
						}
					}
					else if ((opetype == 2) || (opetype == 3) ||
						(opetype == 6) || (opetype == 7)) {
						//#####################
						//pattern exclude num
						//#####################

						bool numflag = true;
						int findpos = pattern0len - 1;
						while (numflag && (findpos > 1)) {
							WCHAR chkwc = similarpattern[findpos];
							if ((chkwc == TEXT('0')) || (chkwc == TEXT('1')) || (chkwc == TEXT('2')) || (chkwc == TEXT('3')) || (chkwc == TEXT('4')) ||
								(chkwc == TEXT('5')) || (chkwc == TEXT('6')) || (chkwc == TEXT('7')) || (chkwc == TEXT('8')) || (chkwc == TEXT('9'))) {
								findpos--;
							}
							else {
								numflag = false;
								break;
							}
						}
						if ((findpos >= 0) && (findpos < pattern0len)) {
							similarpattern[findpos + 1] = 0L;

							int objno1;
							for (objno1 = 0; objno1 < m_grouplinenum; objno1++) {
								if (m_groupobjvec[objno1]) {
									WCHAR chkname[512] = { 0L };
									int result1 = m_groupobjvec[objno1]->getName(chkname, 512);
									if ((result == 0) && (chkname[0] != 0L)) {
										WCHAR* findptr = wcsstr(chkname, similarpattern);//check if pattern is included
										if (findptr) {
											bool calllistener = false;
											if ((opetype == 2) || (opetype == 6)) {
												m_groupobjvec[objno1]->setValue(true, calllistener);
											}
											else if ((opetype == 3) || (opetype == 7)) {
												m_groupobjvec[objno1]->setValue(false, calllistener);
											}
											else {
												_ASSERT(0);
												return 1;
											}
										}
									}
								}
							}
						}
					}
					else {
						_ASSERT(0);
						return 1;
					}
				}
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}
	else {
		return 1;
	}

	return 0;
}

int CDispGroupDlg::TrimLeadingAlnum(bool secondtokenflag, WCHAR* srcstr, int srclen, WCHAR* dststr, int dstlen, bool secondcallflag)
{
	if (!srcstr || !dststr) {
		_ASSERT(0);
		return 1;
	}
	if ((srclen < 0) || (srclen >= 2048) || (dstlen < 0) || (dstlen >= 2048)) {
		_ASSERT(0);
		return 1;
	}


	*dststr = 0L;

	WCHAR trimstr0[2048];
	int tokenloopcount;
	if (secondtokenflag) {
		tokenloopcount = 2;
	}
	else {
		tokenloopcount = 1;
	}
	int index0;
	int srcstartindex = 0;
	int dstindex = 0;
	int loopindex;
	for (loopindex = 0; loopindex < tokenloopcount; loopindex++) {
		ZeroMemory(trimstr0, sizeof(WCHAR) * 2048);
		dstindex = 0;

		bool starttrim = false;
		for (index0 = srcstartindex; index0 < srclen; index0++) {
			WCHAR* pchkstr = srcstr + index0;
			if (starttrim == false) {
				if (iswalnum(*pchkstr) == 0) {
					continue;
				}
				else {
					starttrim = true;
				}
			}
			if (starttrim == true) {
				if (*pchkstr == TEXT('_')) {
					srcstartindex = index0 + 1;
					break;
				}
				if (dstindex < (dstlen - 1)) {
					trimstr0[dstindex] = *pchkstr;
					trimstr0[dstindex + 1] = 0L;
					dstindex++;
				}
				else {
					srcstartindex = index0 + 1;
					break;
				}
			}
		}
		srcstartindex = index0 + 1;
	}

	trimstr0[2048 - 1] = 0L;

	if (secondcallflag == false) {//再帰呼び出しは１回まで

		//パターンの先頭がPrefabだった場合には、次のアンダーバーまでをパターンとする

		WCHAR* nextptr = 0L;
		if (wcscmp(trimstr0, L"Prefab") == 0) {
			nextptr = wcsstr(srcstr, L"Prefab");
			if (nextptr) {
				nextptr += (wcslen(L"Prefab") + 1);//+1 : '_'の分
			}
		}
		else if (wcscmp(trimstr0, L"prefab") == 0) {
			nextptr = wcsstr(srcstr, L"prefab");
			if (nextptr) {
				nextptr += (wcslen(L"prefab") + 1);//+1 : '_'の分
			}
		}

		if (nextptr && (*nextptr != 0L)) {
			bool prefab2ndwordflag = false;
			WCHAR trimstr1[2048];
			ZeroMemory(trimstr1, sizeof(WCHAR) * 2048);
			int result2 = TrimLeadingAlnum(prefab2ndwordflag, nextptr, (int)wcslen(nextptr), trimstr1, 2048, true);
			if ((result2 == 0) && (trimstr1[0] != 0L)) {
				wcscat_s(trimstr0, 2048, L"_");//!!!!!
				wcscat_s(trimstr0, 2048, trimstr1);
			}
		}
	}

	trimstr0[dstlen - 1] = 0L;
	wcscpy_s(dststr, dstlen, trimstr0);

	return 0;
}

int CDispGroupDlg::ShowPosLineMqoObject(CModel* srcpickmodel, CMQOObject* srcpickmqoobj)
{
	if (!srcpickmodel) {
		_ASSERT(0);
		return 1;
	}
	if (srcpickmodel != m_model) {
		_ASSERT(0);
		return 1;
	}

	int selectlineindex = -1;
	int lineno;
	for (lineno = 0; lineno < m_grouplinenum; lineno++) {
		if (m_groupmqoobjvec[lineno]) {
			if (m_groupmqoobjvec[lineno] == srcpickmqoobj) {
				selectlineindex = lineno;
				break;
			}
		}
	}
	if (m_dlgWnd && (selectlineindex >= 0)) {
		m_groupSCWnd->setShowPosLine(selectlineindex);
	}

	return 0;
}



int CDispGroupDlg::ParamsToDlg()
{
	if (m_dlgWnd) {
		m_dlgWnd->callRewrite();
	}
	
	return 0;
}

const HWND CDispGroupDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CDispGroupDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CDispGroupDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

