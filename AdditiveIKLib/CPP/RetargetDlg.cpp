#include "stdafx.h"

#include <RetargetDlg.h>
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
// CRetargetDlg

CRetargetDlg::CRetargetDlg()
{
	InitParams();
}

CRetargetDlg::~CRetargetDlg()
{
	DestroyObjs();
}
	
int CRetargetDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}


	if (m_convboneSCWnd) {
		delete m_convboneSCWnd;
		m_convboneSCWnd = 0;
	}

	int cbno;
	for (cbno = 0; cbno < MAXBONENUM; cbno++) {
		if (m_modelbone[cbno]) {
			delete m_modelbone[cbno];
			m_modelbone[cbno] = 0;
		}
		if (m_bvhbone[cbno]) {
			delete m_bvhbone[cbno];
			m_bvhbone[cbno] = 0;
		}
		m_modelbone_bone[cbno] = 0;
		m_bvhbone_bone[cbno] = 0;
	}

	if (m_cbselmodel) {
		delete m_cbselmodel;
		m_cbselmodel = 0;
	}
	if (m_cbselbvh) {
		delete m_cbselbvh;
		m_cbselbvh = 0;
	}

	if (m_convboneconvert) {
		delete m_convboneconvert;
		m_convboneconvert = 0;
	}
	if (m_convbonespace1) {
		delete m_convbonespace1;
		m_convbonespace1 = 0;
	}
	if (m_convbonespace2) {
		delete m_convbonespace2;
		m_convbonespace2 = 0;
	}
	if (m_convbonespace3) {
		delete m_convbonespace3;
		m_convbonespace3 = 0;
	}
	if (m_convbonespace4) {
		delete m_convbonespace4;
		m_convbonespace4 = 0;
	}
	if (m_convbonespace5) {
		delete m_convbonespace5;
		m_convbonespace5 = 0;
	}
	if (m_rtgfilesave) {
		delete m_rtgfilesave;
		m_rtgfilesave = 0;
	}
	if (m_rtgfileload) {
		delete m_rtgfileload;
		m_rtgfileload = 0;
	}

	if (m_convbonesp) {
		delete m_convbonesp;
		m_convbonesp = 0;
	}

	if (m_convbonemidashi[0]) {
		delete m_convbonemidashi[0];
		m_convbonemidashi[0] = 0;
	}
	if (m_convbonemidashi[1]) {
		delete m_convbonemidashi[1];
		m_convbonemidashi[1] = 0;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	m_convbonenum = 0;
	
	//SetModel()-->CreateRetargetWnd()-->DestroyObjs()と呼び出す場合にm_convbone_modelには有効な値がセットされている
	//ここではm_convbone_modelは初期化しない
	//m_convbone_model = 0;

	m_convbone_bvh = nullptr;
	m_convbonemap.clear();
	m_bvhbone_bonenomap.clear();

	m_selectboneindex = 0;
	m_bvhbone_cbno = 0;

	return 0;
}

void CRetargetDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;
	m_closeconvboneFlag = false;
	m_retargetguiFlag = false;
	m_selectboneindex = 0;
	//m_maxboneno = 0;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_convbonenum = 0;
	m_convbone_model = nullptr;
	m_convbone_bvh = nullptr;
	m_convbonemap.clear();
	m_bvhbone_bonenomap.clear();
	m_bvhbone_cbno = 0;

	m_dlgWnd = nullptr;

	m_convboneSCWnd = nullptr;

	int cbno;
	for (cbno = 0; cbno < MAXBONENUM; cbno++) {
		if (m_modelbone[cbno]) {
			m_modelbone[cbno] = nullptr;
		}
		if (m_bvhbone[cbno]) {
			m_bvhbone[cbno] = nullptr;
		}
		m_modelbone_bone[cbno] = nullptr;
		m_bvhbone_bone[cbno] = nullptr;
	}

	m_cbselmodel = nullptr;
	m_cbselbvh = nullptr;
	m_convboneconvert = nullptr;
	m_convbonespace1 = nullptr;
	m_convbonespace2 = nullptr;
	m_convbonespace3 = nullptr;
	m_convbonespace4 = nullptr;
	m_convbonespace5 = nullptr;
	m_rtgfilesave = nullptr;
	m_rtgfileload = nullptr;
	m_convbonesp = nullptr;
	m_convbonemidashi[0] = nullptr;
	m_convbonemidashi[1] = nullptr;


}

int CRetargetDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

void CRetargetDlg::SetModel(CModel* srcmodel)
{
	m_convbone_model = srcmodel;
	CreateRetargetWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		if (m_convboneSCWnd) {
			m_convboneSCWnd->autoResize();
		}
		m_dlgWnd->callRewrite();
	}
}


void CRetargetDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_convboneSCWnd) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_convboneSCWnd->autoResize();
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


int CRetargetDlg::CreateRetargetWnd()
{

	DestroyObjs();

	if (!m_convbone_model) {
		_ASSERT(0);
		::MessageBox(g_mainhwnd, L"modelメニューでmodelを選択して下さい", L"model not selected !!!", MB_OK);
		return 0;
	}
	//if (m_convbone_model->GetBoneListSize() <= 1) {
	if (m_convbone_model->GetBoneForMotionSize() <= 1) {
		return 0;
	}

	//m_convbonenum = m_convbone_model->GetBoneListSize();
	m_convbonenum = m_convbone_model->GetBoneForMotionSize();
	if (m_convbonenum >= MAXBONENUM) {
		_ASSERT(0);
		return 1;
	}

	m_dlgWnd = new OrgWindow(
		0,
		L"convbone0",		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),		//位置
		WindowSize(m_sizex, m_sizey),	//サイズ
		L"ConvBoneWnd",	//タイトル
		g_mainhwnd,					//親ウィンドウハンドル
		false,					//表示・非表示状態
		//true,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true,					//閉じられるか否か
		true);					//サイズ変更の可否


	if (m_dlgWnd) {
		m_dlgWnd->setSizeMin(WindowSize(150, 150));		// 最小サイズを設定

		//スクロールウインドウ
		m_convboneSCWnd = new OWP_ScrollWnd(L"ConvBoneScWnd", true, 20);
		if (!m_convboneSCWnd) {
			_ASSERT(0);
			return 1;
		}

		//m_convboneSCWnd->setLineDataSize(m_convbonenum + 4);
		//2023/02/14
		//要素数が変わったときには指定し忘れないように！！！
		m_convboneSCWnd->setLineDataSize(m_convbonenum + 8);
		m_dlgWnd->addParts(*m_convboneSCWnd);


		WCHAR bvhbonename[MAX_PATH];
		int listno = 0;
		int cbno = 0;
		unordered_map<int, CBone*>::iterator itrbone;
		for (itrbone = m_convbone_model->GetBoneListBegin(); itrbone != m_convbone_model->GetBoneListEnd(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone && (curbone->IsSkeleton())) {
				const WCHAR* wbonename = curbone->GetWBoneName();
				_ASSERT(wbonename);
				m_modelbone[cbno] = new OWP_Label(wbonename, 20);
				if (m_modelbone[cbno]) {
					m_modelbone_bone[cbno] = curbone;

					swprintf_s(bvhbonename, MAX_PATH, L"NotSet_%03d", cbno);
					m_bvhbone[cbno] = new OWP_Button(bvhbonename, 20);
					if (m_bvhbone[cbno]) {
						m_bvhbone_bone[cbno] = 0;
						m_convbonemap[curbone] = 0;
#ifndef NDEBUG
						DbgOut(L"convbone %d : (%s,  %s)\n", cbno, wbonename, bvhbonename);
#endif
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

				cbno++;
			}
			listno++;
		}
#ifndef NDEBUG
		DbgOut(L"\n\n");
#endif
		if (cbno != m_convbonenum) {
			_ASSERT(0);
			return 1;
		}

		//m_convbonesp = new OWP_Separator(m_dlgWnd, false, 0.5, true, m_convboneSCWnd);// セパレータ1（境界線による横方向2分割）
		m_convbonesp = new OWP_Separator(m_dlgWnd, true, 0.5, true, m_convboneSCWnd);// セパレータ1（境界線による横方向2分割）
		if (!m_convbonesp) {
			_ASSERT(0);
			return 1;
		}

		//m_cbselmodel = new OWP_Button(L"SelectShapeModel");
		WCHAR strtext[256] = { 0L };
		//swprintf_s(strtext, 256, L"Model: %s", m_convbone_model->GetFileName());
		ShortenNameW(m_convbone_model->GetFileName(), strtext, 256, 21);//2024/07/13 21文字まで
		m_cbselmodel = new OWP_Label(strtext, 20);
		if (!m_cbselmodel) {
			_ASSERT(0);
			return 1;
		}

		m_cbselbvh = new OWP_Button(L"SelectMotionModel", 20);
		if (!m_cbselbvh) {
			_ASSERT(0);
			return 1;
		}
		m_convboneconvert = new OWP_Button(L"ConvertButton", 20);
		if (!m_convboneconvert) {
			_ASSERT(0);
			return 1;
		}
		m_convbonespace1 = new OWP_Label(L"--------------", 20);
		if (!m_convbonespace1) {
			_ASSERT(0);
			return 1;
		}
		m_convbonespace2 = new OWP_Label(L"--------------", 20);
		if (!m_convbonespace2) {
			_ASSERT(0);
			return 1;
		}
		m_convbonespace3 = new OWP_Label(L"--------------", 20);
		if (!m_convbonespace3) {
			_ASSERT(0);
			return 1;
		}
		m_convbonespace4 = new OWP_Label(L"--------------", 20);
		if (!m_convbonespace4) {
			_ASSERT(0);
			return 1;
		}
		m_convbonespace5 = new OWP_Label(L"              ", 20);
		if (!m_convbonespace5) {
			_ASSERT(0);
			return 1;
		}
		m_rtgfilesave = new OWP_Button(L"Save RtgFile", 20);
		if (!m_rtgfilesave) {
			_ASSERT(0);
			return 1;
		}
		m_rtgfileload = new OWP_Button(L"Load RtgFile", 20);
		if (!m_rtgfileload) {
			_ASSERT(0);
			return 1;
		}
		m_convbonemidashi[0] = new OWP_Label(L"ShapeSide", 20);
		if (!m_convbonemidashi) {
			_ASSERT(0);
			return 1;
		}
		m_convbonemidashi[1] = new OWP_Label(L"MotionSide", 20);
		if (!m_convbonemidashi) {
			_ASSERT(0);
			return 1;
		}

		COLORREF importantcolR = RGB(168, 129, 129);
		COLORREF importantcolG = RGB(0, 240, 0);
		COLORREF importantcolW = RGB(240, 240, 240);
		if (m_convboneconvert) {
			m_convboneconvert->setTextColor(importantcolG);
		}
		if (m_rtgfilesave) {
			m_rtgfilesave->setTextColor(importantcolG);
		}
		if (m_rtgfileload) {
			m_rtgfileload->setTextColor(importantcolG);
		}
		if (m_cbselbvh) {
			m_cbselbvh->setTextColor(importantcolR);
		}
		if (m_cbselmodel) {
			m_cbselmodel->setTextColor(importantcolR);
		}
		if (m_convbonespace1) {
			m_convbonespace1->setTextColor(importantcolW);
		}
		if (m_convbonespace2) {
			m_convbonespace2->setTextColor(importantcolW);
		}
		if (m_convbonespace3) {
			m_convbonespace3->setTextColor(importantcolW);
		}

		if (m_convboneSCWnd && m_convbonesp) {
			m_convboneSCWnd->addParts(*m_convbonesp);
		}
		if (m_convbonesp && m_convbonemidashi[0]) {
			m_convbonesp->addParts1(*m_convbonemidashi[0]);
		}
		if (m_convbonesp && m_cbselmodel) {
			m_convbonesp->addParts1(*m_cbselmodel);
		}
		if (m_convbonesp && m_convbonemidashi[1]) {
			m_convbonesp->addParts2(*m_convbonemidashi[1]);
		}
		if (m_convbonesp && m_cbselbvh) {
			m_convbonesp->addParts2(*m_cbselbvh);
		}
		//if (m_cbselmodel) {
		//	m_dsretargetctrls.push_back(m_cbselmodel);
		//}
		//if (m_cbselbvh) {
		//	m_dsretargetctrls.push_back(m_cbselbvh);
		//}


		//2023/02/14
		//convert実行、rtgファイル読み込みボタンは
		//ボーン名対応表よりも　上に配置
		//一番下までスクロールしなくても　操作できることが多くなるように
		if (m_convbonesp && m_convboneconvert) {
			m_convbonesp->addParts1(*m_convboneconvert);
		}
		//if (m_convboneconvert) {
		//	m_dsretargetctrls.push_back(m_convboneconvert);
		//}
		if (m_convbonesp && m_rtgfileload) {
			m_convbonesp->addParts2(*m_rtgfileload);
		}
		//if (m_rtgfileload) {
		//	m_dsretargetctrls.push_back(m_rtgfileload);
		//}

		//2023/02/14
		//境目に　空白
		if (m_convbonesp && m_convbonespace1) {
			m_convbonesp->addParts1(*m_convbonespace1);
		}
		if (m_convbonesp && m_convbonespace2) {
			m_convbonesp->addParts2(*m_convbonespace2);
		}

		for (cbno = 0; cbno < m_convbonenum; cbno++) {
			if (m_convbonesp && m_modelbone[cbno]) {
				m_convbonesp->addParts1(*m_modelbone[cbno]);
			}
			if (m_convbonesp && m_bvhbone[cbno]) {
				m_convbonesp->addParts2(*m_bvhbone[cbno]);
			}

			////m_dsretargetctrls.push_back(m_modelbone[cbno]);
			//if (m_bvhbone[cbno]) {
			//	m_dsretargetctrls.push_back(m_bvhbone[cbno]);
			//}
		}

		//2023/02/14
		//境目に　空白
		if (m_convbonesp && m_convbonespace3) {
			m_convbonesp->addParts1(*m_convbonespace3);
		}
		if (m_convbonesp && m_convbonespace4) {
			m_convbonesp->addParts2(*m_convbonespace4);
		}

		//Rtgファイル保存ボタンは　設定し終わってから押すので　一番下のまま
		if (m_convbonesp && m_rtgfilesave) {
			m_convbonesp->addParts1(*m_rtgfilesave);
			//m_dsretargetctrls.push_back(m_rtgfilesave);
		}
		if (m_convbonesp && m_convbonespace5) {
			m_convbonesp->addParts2(*m_convbonespace5);
		}

		if (m_dlgWnd) {
			m_dlgWnd->setListenMouse(false);
			m_dlgWnd->setVisible(0);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		////////////
		if (m_dlgWnd) {
			m_dlgWnd->setCloseListener([=, this]() {
				if (m_convbone_model) {
					m_closeconvboneFlag = true;
				}
				});
		}

		//m_cbselmodel->setButtonListener([=, this](){
		//	if (m_convbone_model) {
		//		SetConvBoneModel();
		//		m_dlgWnd->callRewrite();
		//	}
		//});

		//RETARGETDLG_OPE_SELECTBVH,
		//RETARGETDLG_OPE_SELECTBONE,
		//RETARGETDLG_OPE_RETARGETGUI,
		//RETARGETDLG_OPE_SAVEFILE,
		//RETARGETDLG_OPE_LOADFILE,

		if (m_cbselbvh) {
			m_cbselbvh->setButtonListener([=, this]() {
				if (m_convbone_model) {
					if (!m_convbone_model || (m_convbone_model != m_convbone_model)) {
						::MessageBoxW(NULL, L"Retry after selecting ShapeModel using ModelMenu Of MainWindow.", L"error!!!", MB_OK);
					}
					else {
						//SetConvBoneBvh();
						if (g_mainhwnd && IsWindow(g_mainhwnd)) {
							::PostMessage(g_mainhwnd, WM_COMMAND, 
								(ID_RMENU_0 + MENUOFFSET_RETARGETDLG), (LPARAM)RETARGETDLG_OPE_SELECTBVH);
						}
					}
					m_dlgWnd->callRewrite();
				}
				});
		}

		for (cbno = 0; cbno < m_convbonenum; cbno++) {
			if (m_bvhbone[cbno]) {
				m_bvhbone[cbno]->setButtonListener([=, this]() {
					if (m_convbone_model) {

						//SetConvBone(cbno);
						m_selectboneindex = cbno;
						if (g_mainhwnd && IsWindow(g_mainhwnd)) {
							::PostMessage(g_mainhwnd, WM_COMMAND,
								(ID_RMENU_0 + MENUOFFSET_RETARGETDLG), (LPARAM)RETARGETDLG_OPE_SELECTBONE);
						}

						m_dlgWnd->callRewrite();
					}
					});
			}
		}

		if (m_convboneconvert) {
			m_convboneconvert->setButtonListener([=, this]() {
				if (m_convbone_model) {
					if (m_retargetguiFlag == false) {
						m_retargetguiFlag = true;
					}
				}
				});
		}

		if (m_rtgfilesave) {
			m_rtgfilesave->setButtonListener([=, this]() {
				if (m_convbone_model) {
					//SaveRetargetFile();
					if (g_mainhwnd && IsWindow(g_mainhwnd)) {
						::PostMessage(g_mainhwnd, WM_COMMAND,
							(ID_RMENU_0 + MENUOFFSET_RETARGETDLG), (LPARAM)RETARGETDLG_OPE_SAVEFILE);
					}
				}
				});
		}
		if (m_rtgfileload) {
			m_rtgfileload->setButtonListener([=, this]() {
				if (m_convbone_model) {
					//LoadRetargetFile(0);
					if (g_mainhwnd && IsWindow(g_mainhwnd)) {
						::PostMessage(g_mainhwnd, WM_COMMAND,
							(ID_RMENU_0 + MENUOFFSET_RETARGETDLG), (LPARAM)RETARGETDLG_OPE_LOADFILE);
					}
				}
				});
		}
		if (m_dlgWnd) {
			m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
			m_dlgWnd->setPos(WindowPos(m_posx, m_posy));
			//１クリック目問題対応
			m_dlgWnd->refreshPosAndSize();//2022/09/20

			m_dlgWnd->setVisible(false);
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CRetargetDlg::ParamsToDlg()
{
	if (!m_dlgWnd) {
		_ASSERT(0);
		return 1;
	}

	if (m_dlgWnd) {
		m_dlgWnd->callRewrite();
	}

	return 0;
}

int CRetargetDlg::SetConvBone_NoSet()
{
	m_bvhbone_bone[m_bvhbone_cbno] = 0;
	CBone* modelbone = m_modelbone_bone[m_bvhbone_cbno];
	_ASSERT(modelbone);
	if (modelbone) {
		m_convbonemap[modelbone] = 0;
	}
	m_bvhbone[m_bvhbone_cbno]->setName(L"NotSet");
	m_bvhbone[m_bvhbone_cbno]->callRewrite();

	return 0;
}
int CRetargetDlg::SetConvBone_Set(int bvhcbno)
{
	int boneno = m_bvhbone_bonenomap[bvhcbno];//2024/07/07 変換表
	CBone* curbone = m_convbone_bvh->GetBoneByID(boneno);
	WCHAR strmes[1024];
	if (!curbone) {
		m_bvhbone_bone[m_bvhbone_cbno] = 0;
		CBone* modelbone = m_modelbone_bone[m_bvhbone_cbno];
		_ASSERT(modelbone);
		if (modelbone) {
			m_convbonemap[modelbone] = 0;
		}
		m_bvhbone[m_bvhbone_cbno]->setName(L"NotSet");
		m_bvhbone[m_bvhbone_cbno]->callRewrite();

		swprintf_s(strmes, 1024, L"convbone : sel bvh bone : curbone NULL !!!");
		::MessageBox(NULL, strmes, L"check!!!", MB_OK);
	}
	else {
		swprintf_s(strmes, 1024, L"%s", curbone->GetWBoneName());
		m_bvhbone[m_bvhbone_cbno]->setName(strmes);
		m_bvhbone[m_bvhbone_cbno]->callRewrite();
		m_bvhbone_bone[m_bvhbone_cbno] = curbone;

		CBone* modelbone = m_modelbone_bone[m_bvhbone_cbno];
		if (modelbone) {
			m_convbonemap[modelbone] = curbone;
		}
	}
	return 0;
}

int CRetargetDlg::InitJointPair2ConvBoneWnd()
{
	m_convbonemap.clear();

	WCHAR bvhbonename[MAX_PATH];
	int cbno = 0;
	unordered_map<int, CBone*>::iterator itrbone;
	for (itrbone = m_convbone_model->GetBoneListBegin(); itrbone != m_convbone_model->GetBoneListEnd(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {
			swprintf_s(bvhbonename, MAX_PATH, L"NotSet_%03d", cbno);
			(m_bvhbone[cbno])->setName(bvhbonename);
			m_bvhbone_bone[cbno] = 0;

			cbno++;
		}
	}
	return 0;
}

int CRetargetDlg::SetJointPair2ConvBoneWnd()
{
	if (m_convbonemap.empty()) {
		return 0;
	}
	if (!m_convbone_bvh) {
		return 0;
	}

	WCHAR bvhbonename[MAX_PATH];
	int cbno = 0;
	unordered_map<int, CBone*>::iterator itrbone;
	for (itrbone = m_convbone_model->GetBoneListBegin(); itrbone != m_convbone_model->GetBoneListEnd(); itrbone++) {
		CBone* curbone = itrbone->second;
		if (curbone && (curbone->IsSkeleton())) {
			CBone* bvhbone = m_convbonemap[curbone];
			if (bvhbone) {
				swprintf_s(bvhbonename, MAX_PATH, bvhbone->GetWBoneName());
				(m_bvhbone[cbno])->setName(bvhbonename);
				m_bvhbone_bone[cbno] = bvhbone;
			}
			else {
				swprintf_s(bvhbonename, MAX_PATH, L"NotSet_%03d", cbno);
				(m_bvhbone[cbno])->setName(bvhbonename);
				m_bvhbone_bone[cbno] = 0;
			}

			cbno++;
		}
	}

	_ASSERT(cbno == m_convbonenum);

	return 0;
}

const HWND CRetargetDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CRetargetDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CRetargetDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

