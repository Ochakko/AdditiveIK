#include "stdafx.h"

#include <DollyMemoDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <StrMisc.h>

#include <Model.h>
#include <OrgWindow.h>
#include <GlobalVar.h>

#include <ChaCamera.h>

#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>

using namespace std;
using namespace OrgWinGUI;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル
extern CModel* g_cameragmodel;
extern ChaCamera g_chacamera;

static WCHAR s_checkfilename[MAX_PATH] = L"MB3DOpenProjDollyInitialized10085.dol";


CDollyMemoOWPElem::CDollyMemoOWPElem()
{
	InitParams();
}
CDollyMemoOWPElem::~CDollyMemoOWPElem()
{
	DestroyObjs();
}
int CDollyMemoOWPElem::SetDollyElem(
	int srcdollyelemindex, OrgWinGUI::OrgWindow* parwnd, OrgWinGUI::OWP_Separator* parentsp,
	DOLLYELEM2 srcdollyelem)
{
	DestroyObjs();

	if (!parwnd || !parentsp) {
		_ASSERT(0);
		return 1;
	}

	int labelheight;
	if (g_4kresolution) {
		labelheight = 28;
	}
	else {
		labelheight = 20;
	}

	double rate50 = 0.50;

	m_dollyelemindex = srcdollyelemindex;
	m_dollyelem = srcdollyelem;

	m_namesp = new OWP_Separator(parwnd, true, 0.75, true);
	if (!m_namesp) {
		_ASSERT(0);
		return 1;
	}
	//m_namesp2 = new OWP_Separator(parwnd, true, rate50, true);
	//if (!m_namesp2) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//m_pasteB = new OWP_Button(L"paste", labelheight);
	//if (!m_pasteB) {
	//	_ASSERT(0);
	//	return 1;
	//}
	m_deleteB = new OWP_Button(L"del", labelheight);
	if (!m_deleteB) {
		_ASSERT(0);
		return 1;
	}
	m_spacerLabel = new OWP_Label(L"     ", labelheight);
	if (!m_spacerLabel) {
		_ASSERT(0);
		return 1;
	}


	m_descsp = new OWP_Separator(parwnd, true, 0.75, true);
	if (!m_descsp) {
		_ASSERT(0);
		return 1;
	}
	m_descsp2 = new OWP_Separator(parwnd, true, 0.50, true);
	if (!m_descsp2) {
		_ASSERT(0);
		return 1;
	}
	m_upB = new OWP_Button(L"↑", labelheight);
	if (!m_upB) {
		_ASSERT(0);
		return 1;
	}
	m_downB = new OWP_Button(L"↓", labelheight);
	if (!m_downB) {
		_ASSERT(0);
		return 1;
	}


	if (srcdollyelem.elem1.validflag) {
		WCHAR dispname[MAX_PATH] = { 0L };
		ChaVector3 currentpos = srcdollyelem.elem1.camerapos;
		swprintf_s(dispname, MAX_PATH, L"%.2f, %.2f, %.2f", currentpos.x, currentpos.y, currentpos.z);
		m_nameChk = new OWP_CheckBoxA(dispname, false, labelheight, true, MAX_PATH);
		if (!m_nameChk) {
			_ASSERT(0);
			return 1;
		}

		WCHAR strdesc[MAX_PATH] = { 0L };
		ChaVector3 currenttarget = srcdollyelem.elem1.cameratarget;
		swprintf_s(strdesc, MAX_PATH, L"%.2f, %.2f, %.2f", currenttarget.x, currenttarget.y, currenttarget.z);
		m_descLabel = new OWP_Label(strdesc, labelheight, MAX_PATH);
		if (!m_descLabel) {
			_ASSERT(0);
			return 1;
		}

		WCHAR strmemo[HISTORYCOMMENTLEN] = { 0L };
		swprintf_s(strmemo, HISTORYCOMMENTLEN, L"%s", srcdollyelem.elem1.comment);
		m_memoLabel = new OWP_Label(strmemo, labelheight, HISTORYCOMMENTLEN);
		if (!m_memoLabel) {
			_ASSERT(0);
			return 1;
		}


	}
	else {
		WCHAR strinvalid[MAX_PATH] = { 0L };
		wcscpy_s(strinvalid, MAX_PATH, L"Invalid.");
		m_nameChk = new OWP_CheckBoxA(strinvalid, false, labelheight, true, MAX_PATH);
		if (!m_nameChk) {
			_ASSERT(0);
			return 1;
		}

		m_descLabel = new OWP_Label(strinvalid, labelheight, MAX_PATH);
		if (!m_descLabel) {
			_ASSERT(0);
			return 1;
		}

		m_memoLabel = new OWP_Label(strinvalid, labelheight, HISTORYCOMMENTLEN);
		if (!m_memoLabel) {
			_ASSERT(0);
			return 1;
		}
	}

	return 0;
}
int CDollyMemoOWPElem::AddParts(OrgWinGUI::OWP_Separator* parentsp)
{
	if (!parentsp) {
		_ASSERT(0);
		return 1;
	}

	if (m_namesp) {
		parentsp->addParts1(*m_namesp);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	//if (m_namesp2) {
	//	parentsp->addParts2(*m_namesp2);
	//}
	//else {
	//	_ASSERT(0);
	//	return 1;
	//}
	if (m_nameChk) {
		m_namesp->addParts1(*m_nameChk);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	//if (m_pasteB) {
	//	m_namesp2->addParts1(*m_pasteB);
	//}
	//else {
	//	_ASSERT(0);
	//	return 1;
	//}
	if (m_deleteB) {
		//m_namesp2->addParts2(*m_deleteB);
		m_namesp->addParts2(*m_deleteB);
	}
	else {
		_ASSERT(0);
		return 1;
	}

	if (m_descsp) {
		parentsp->addParts1(*m_descsp);

		if (m_descLabel) {
			m_descsp->addParts1(*m_descLabel);
		}
		if (m_descsp2) {
			m_descsp->addParts2(*m_descsp2);
		}
		if (m_upB) {
			m_descsp2->addParts1(*m_upB);
		}
		if (m_downB) {
			m_descsp2->addParts2(*m_downB);
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_memoLabel) {
		parentsp->addParts1(*m_memoLabel);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_spacerLabel) {
		parentsp->addParts1(*m_spacerLabel);
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}
int CDollyMemoOWPElem::SetEventFunc(CDollyMemoDlg* srcdlg)
{
	m_parentdlg = srcdlg;

	if (m_nameChk) {
		m_nameChk->setButtonListener([=, this]() {
			if (m_parentdlg) {
				bool pasteflag = true;//カメラアニメスイッチオンの場合にはカメラアニメにペーストする
				bool callFromKey = false;
				m_parentdlg->OnRadio(m_dollyelemindex, pasteflag, callFromKey);
			}
		});
	}
	if (m_deleteB) {
		m_deleteB->setButtonListener([=, this]() {
			if (m_parentdlg) {
				m_parentdlg->OnDelete(m_dollyelemindex);
			}
		});
	}
	//if (m_pasteB) {
	//	m_pasteB->setButtonListener([=, this]() {
	//		if (m_parentdlg) {
	//			bool pasteflag = true;
	//			m_parentdlg->OnRadio(m_dollyelemindex, pasteflag);
	//		}
	//	});
	//}

	if (m_upB) {
		m_upB->setButtonListener([=, this]() {
			if (m_parentdlg) {
				m_parentdlg->OnUp(m_dollyelemindex);
			}
			});
	}
	if (m_downB) {
		m_downB->setButtonListener([=, this]() {
			if (m_parentdlg) {
				m_parentdlg->OnDown(m_dollyelemindex);
			}
			});
	}

	return 0;
}

void CDollyMemoOWPElem::InitParams()
{
	m_dollyelemindex = -1;

	m_namesp = nullptr;
	m_namesp2 = nullptr;
	m_nameChk = nullptr;
	m_pasteB = nullptr;
	m_deleteB = nullptr;
	m_descLabel = nullptr;
	m_memoLabel = nullptr;
	m_spacerLabel = nullptr;

	m_descsp = nullptr;
	m_descsp2 = nullptr;
	m_upB = nullptr;
	m_downB = nullptr;

	m_parentdlg = nullptr;
}
void CDollyMemoOWPElem::DestroyObjs()
{
	if (m_namesp) {
		delete m_namesp;
		m_namesp = nullptr;
	}
	if (m_namesp2) {
		delete m_namesp2;
		m_namesp2 = nullptr;
	}
	if (m_nameChk) {
		delete m_nameChk;
		m_nameChk = nullptr;
	}
	if (m_pasteB) {
		delete m_pasteB;
		m_pasteB = nullptr;
	}
	if (m_deleteB) {
		delete m_deleteB;
		m_deleteB = nullptr;
	}
	if (m_descLabel) {
		delete m_descLabel;
		m_descLabel = nullptr;
	}
	if (m_memoLabel) {
		delete m_memoLabel;
		m_memoLabel = nullptr;
	}
	if (m_spacerLabel) {
		delete m_spacerLabel;
		m_spacerLabel = nullptr;
	}

	if (m_descsp) {
		delete m_descsp;
		m_descsp = nullptr;
	}
	if (m_descsp2) {
		delete m_descsp2;
		m_descsp2 = nullptr;
	}
	if (m_upB) {
		delete m_upB;
		m_upB = nullptr;
	}
	if (m_downB) {
		delete m_downB;
		m_downB = nullptr;
	}
}




/////////////////////////////////////////////////////////////////////////////
// CDollyMemoDlg

CDollyMemoDlg::CDollyMemoDlg()
{
	InitParams();
}

CDollyMemoDlg::~CDollyMemoDlg()
{
	DestroyObjs();
}
	
int CDollyMemoDlg::DestroyObjs()
{
	DestroyOWPWnd();
	InitParams();

	return 0;
}

void CDollyMemoDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;
	m_onshow = false;
	m_envdir[0] = 0L;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_dollymemo.clear();
	m_owpelemvec.clear();

	m_camerapos.SetParams(0.0f, 0.0f, 10.0f);
	m_targetpos.SetParams(0.0f, 0.0f, 0.0f);
	m_upvec.SetParams(0.0f, 1.0f, 0.0f);
	ZeroMemory(m_comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);

	m_UpdateFunc = nullptr;
	m_PasteFunc = nullptr;

	m_dlgwnd = nullptr;
	m_dlgSc = nullptr;
	m_dlgspheader = nullptr;
	m_dlgsplist = nullptr;
	m_possp1 = nullptr;
	m_possp2 = nullptr;
	m_possp3 = nullptr;
	m_posLabel = nullptr;
	m_posxEdit = nullptr;
	m_posyEdit = nullptr;
	m_poszEdit = nullptr;
	m_tarsp1 = nullptr;
	m_tarsp2 = nullptr;
	m_tarsp3 = nullptr;
	m_tarLabel = nullptr;
	m_tarxEdit = nullptr;
	m_taryEdit = nullptr;
	m_tarzEdit = nullptr;
	m_memosp = nullptr;
	m_memoLabel = nullptr;
	m_memoEdit = nullptr;
	m_btnsp1 = nullptr;
	m_btnsp2 = nullptr;
	m_btnsp3 = nullptr;
	m_getB = nullptr;
	m_setB = nullptr;
	m_saveB = nullptr;
	m_spacer1Label = nullptr;
	m_spacer2Label = nullptr;
}

void CDollyMemoDlg::SetUpdateFunc(int (*UpdateFunc)(), int (*PasteFunc)())
{
	m_UpdateFunc = UpdateFunc;
	m_PasteFunc = PasteFunc;
}

int CDollyMemoDlg::DestroyOWPWnd()
{
	if (m_dlgwnd) {
		m_dlgwnd->setVisible(false);
		m_dlgwnd->setListenMouse(false);
	}


	DestroyOWPElem();


	if (m_dlgSc) {
		delete m_dlgSc;
		m_dlgSc = nullptr;
	}
	if (m_dlgspheader) {
		delete m_dlgspheader;
		m_dlgspheader = nullptr;
	}
	if (m_dlgsplist) {
		delete m_dlgsplist;
		m_dlgsplist = nullptr;
	}
	if (m_possp1) {
		delete m_possp1;
		m_possp1 = nullptr;
	}
	if (m_possp2) {
		delete m_possp2;
		m_possp2 = nullptr;
	}
	if (m_possp3) {
		delete m_possp3;
		m_possp3 = nullptr;
	}
	if (m_posLabel) {
		delete m_posLabel;
		m_posLabel = nullptr;
	}
	if (m_posxEdit) {
		delete m_posxEdit;
		m_posxEdit = nullptr;
	}
	if (m_posyEdit) {
		delete m_posyEdit;
		m_posyEdit = nullptr;
	}
	if (m_poszEdit) {
		delete m_poszEdit;
		m_poszEdit = nullptr;
	}
	if (m_tarsp1) {
		delete m_tarsp1;
		m_tarsp1 = nullptr;
	}
	if (m_tarsp2) {
		delete m_tarsp2;
		m_tarsp2 = nullptr;
	}
	if (m_tarsp3) {
		delete m_tarsp3;
		m_tarsp3 = nullptr;
	}
	if (m_tarLabel) {
		delete m_tarLabel;
		m_tarLabel = nullptr;
	}
	if (m_tarxEdit) {
		delete m_tarxEdit;
		m_tarxEdit = nullptr;
	}
	if (m_taryEdit) {
		delete m_taryEdit;
		m_taryEdit = nullptr;
	}
	if (m_tarzEdit) {
		delete m_tarzEdit;
		m_tarzEdit = nullptr;
	}
	if (m_memosp) {
		delete m_memosp;
		m_memosp = nullptr;
	}
	if (m_memoLabel) {
		delete m_memoLabel;
		m_memoLabel = nullptr;
	}
	if (m_memoEdit) {
		delete m_memoEdit;
		m_memoEdit = nullptr;
	}
	if (m_btnsp1) {
		delete m_btnsp1;
		m_btnsp1 = nullptr;
	}
	if (m_btnsp2) {
		delete m_btnsp2;
		m_btnsp2 = nullptr;
	}
	if (m_btnsp3) {
		delete m_btnsp3;
		m_btnsp3 = nullptr;
	}
	if (m_getB) {
		delete m_getB;
		m_getB = nullptr;
	}
	if (m_setB) {
		delete m_setB;
		m_setB = nullptr;
	}
	if (m_saveB) {
		delete m_saveB;
		m_saveB = nullptr;
	}
	if (m_spacer1Label) {
		delete m_spacer1Label;
		m_spacer1Label = nullptr;
	}
	if (m_spacer2Label) {
		delete m_spacer2Label;
		m_spacer2Label = nullptr;
	}
	if (m_dlgwnd) {
		delete m_dlgwnd;
		m_dlgwnd = nullptr;
	}

	return 0;
}
int CDollyMemoDlg::DestroyOWPElem()
{
	size_t elemnum = m_owpelemvec.size();
	size_t elemindex;
	for (elemindex = 0; elemindex < elemnum; elemindex++) {
		CDollyMemoOWPElem* curowp = m_owpelemvec[elemindex];
		if (curowp) {
			delete curowp;
		}
	}
	m_owpelemvec.clear();

	return 0;
}

int CDollyMemoDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}


//LRESULT CDollyMemoDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	//size_t selectedno = 0;//チェックされていない場合(あり得ないが)、一番最初
//	//size_t nameno;
//	//for (nameno = 0; nameno < m_namenum; nameno++) {
//	//	if (m_dlg_wnd.IsDlgButtonChecked(m_ctrlid[nameno]) && (m_copyhistory[m_startno + nameno].hascpinfo == 1)) {
//	//		selectedno = nameno;
//	//		break;
//	//	}
//	//}
//
//	//if ((selectedno >= 0) && (selectedno < m_namenum)) {
//	//	m_selectname[MAX_PATH - 1] = 0L;
//	//	wcscpy_s(m_selectname, MAX_PATH, m_copyhistory[m_startno + selectedno].wfilename);
//	//	m_selectname[MAX_PATH - 1] = 0L;
//	//}
//
//	m_ischeckedmostrecent = m_dlg_wnd.IsDlgButtonChecked(IDC_CHECK1);
//
//
//	m_initsearchcomboflag = false;
//	//EndDialog(wID);
//	//DestroyWindow();
//	ShowWindow(SW_HIDE);
//	return 0;
//}
//
//LRESULT CDollyMemoDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	m_initsearchcomboflag = false;
//	//EndDialog(wID);
//	//DestroyWindow();
//	ShowWindow(SW_HIDE);
//	return 0;
//}

int CDollyMemoDlg::EditParamsToDlg()
{
	if (m_posxEdit) {
		WCHAR dispname[EDIT_BUFLEN_NUM] = { 0L };
		swprintf_s(dispname, EDIT_BUFLEN_NUM, L"%.2f", m_camerapos.x);
		m_posxEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_posyEdit) {
		WCHAR dispname[EDIT_BUFLEN_NUM] = { 0L };
		swprintf_s(dispname, EDIT_BUFLEN_NUM, L"%.2f", m_camerapos.y);
		m_posyEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_poszEdit) {
		WCHAR dispname[EDIT_BUFLEN_NUM] = { 0L };
		swprintf_s(dispname, EDIT_BUFLEN_NUM, L"%.2f", m_camerapos.z);
		m_poszEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}


	if (m_tarxEdit) {
		WCHAR dispname[EDIT_BUFLEN_NUM] = { 0L };
		swprintf_s(dispname, EDIT_BUFLEN_NUM, L"%.2f", m_targetpos.x);
		m_tarxEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_taryEdit) {
		WCHAR dispname[EDIT_BUFLEN_NUM] = { 0L };
		swprintf_s(dispname, EDIT_BUFLEN_NUM, L"%.2f", m_targetpos.y);
		m_taryEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_tarzEdit) {
		WCHAR dispname[EDIT_BUFLEN_MEMO] = { 0L };
		swprintf_s(dispname, EDIT_BUFLEN_MEMO, L"%.2f", m_targetpos.z);
		m_tarzEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}



	if (m_memoEdit) {
		WCHAR dispname[HISTORYCOMMENTLEN] = { 0L };
		swprintf_s(dispname, HISTORYCOMMENTLEN, L"%s", m_comment);
		m_memoEdit->setName(dispname);
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}


int CDollyMemoDlg::ParamsToDlg()
{

	//##################
	//ウインドウを作り直す
	//##################
	DestroyOWPWnd();//まず破棄する
	int result2 = CreateOWPWnd();
	if (result2 != 0) {
		_ASSERT(0);
		abort();
	}


	EditParamsToDlg();

	return 0;
}

int CDollyMemoDlg::CreateOWPWnd()
{

	DestroyOWPWnd();

	m_dlgwnd = new OrgWindow(
		0,
		_T("DollyMemoDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("DollyMemoDlg"),	//タイトル
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

	if (m_dlgwnd) {
		m_dlgwnd->setListenMouse(true);

		double rate50 = 0.50;

		m_dlgspheader = new OWP_Separator(m_dlgwnd, true, 0.995, false);
		if (!m_dlgspheader) {
			_ASSERT(0);
			abort();
		}
		m_dlgwnd->addParts(*m_dlgspheader);

		m_dlgSc = new OWP_ScrollWnd(L"DollyMemoScroll", true, labelheight);
		if (!m_dlgSc) {
			_ASSERT(0);
			abort();
		}
		int linedatanum = (int)((double)m_dollymemo.size() * 4.0 * 1.25);
		m_dlgSc->setLineDataSize(linedatanum);
		m_dlgwnd->addParts(*m_dlgSc);

		m_dlgsplist = new OWP_Separator(m_dlgwnd, true, 0.995, false, m_dlgSc);
		if (!m_dlgsplist) {
			_ASSERT(0);
			abort();
		}
		m_dlgSc->addParts(*m_dlgsplist);

		m_possp1 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_possp1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspheader->addParts1(*m_possp1);
		m_possp2 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_possp2) {
			_ASSERT(0);
			abort();
		}
		m_possp1->addParts1(*m_possp2);
		m_possp3 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_possp3) {
			_ASSERT(0);
			abort();
		}
		m_possp1->addParts2(*m_possp3);

		m_posLabel = new OWP_Label(L"Position", labelheight, MAX_PATH);
		if (!m_posLabel) {
			_ASSERT(0);
			abort();
		}
		m_possp2->addParts1(*m_posLabel);

		m_posxEdit = new OWP_EditBox(true, L"posx", labelheight, EDIT_BUFLEN_NUM);
		if (!m_posxEdit) {
			_ASSERT(0);
			abort();
		}
		m_possp2->addParts2(*m_posxEdit);
		m_posyEdit = new OWP_EditBox(true, L"posy", labelheight, EDIT_BUFLEN_NUM);
		if (!m_posyEdit) {
			_ASSERT(0);
			abort();
		}
		m_possp3->addParts1(*m_posyEdit);
		m_poszEdit = new OWP_EditBox(true, L"posz", labelheight, EDIT_BUFLEN_NUM);
		if (!m_poszEdit) {
			_ASSERT(0);
			abort();
		}
		m_possp3->addParts2(*m_poszEdit);

		m_tarsp1 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_tarsp1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspheader->addParts1(*m_tarsp1);

		m_tarsp2 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_tarsp2) {
			_ASSERT(0);
			abort();
		}
		m_tarsp1->addParts1(*m_tarsp2);

		m_tarsp3 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_tarsp3) {
			_ASSERT(0);
			abort();
		}
		m_tarsp1->addParts2(*m_tarsp3);

		m_tarLabel = new OWP_Label(L"Target", labelheight, MAX_PATH);
		if (!m_tarLabel) {
			_ASSERT(0);
			abort();
		}
		m_tarsp2->addParts1(*m_tarLabel);
		m_tarxEdit = new OWP_EditBox(true, L"targetx", labelheight, EDIT_BUFLEN_NUM);
		if (!m_tarxEdit) {
			_ASSERT(0);
			abort();
		}
		m_tarsp2->addParts2(*m_tarxEdit);

		m_taryEdit = new OWP_EditBox(true, L"targety", labelheight, EDIT_BUFLEN_NUM);
		if (!m_taryEdit) {
			_ASSERT(0);
			abort();
		}
		m_tarsp3->addParts1(*m_taryEdit);
		m_tarzEdit = new OWP_EditBox(true, L"targetz", labelheight, EDIT_BUFLEN_NUM);
		if (!m_tarzEdit) {
			_ASSERT(0);
			abort();
		}
		m_tarsp3->addParts2(*m_tarzEdit);

		m_memosp = new OWP_Separator(m_dlgwnd, true, 0.25, true);
		if (!m_memosp) {
			_ASSERT(0);
			abort();
		}
		m_dlgspheader->addParts1(*m_memosp);

		m_memoLabel = new OWP_Label(L"Comment", labelheight, MAX_PATH);
		if (!m_memoLabel) {
			_ASSERT(0);
			abort();
		}
		m_memosp->addParts1(*m_memoLabel);
		m_memoEdit = new OWP_EditBox(false, L"memo", labelheight, EDIT_BUFLEN_MEMO);//onlynum=false, BUFLEM_MEMO
		if (!m_memoEdit) {
			_ASSERT(0);
			abort();
		}
		m_memosp->addParts2(*m_memoEdit);

		m_spacer1Label = new OWP_Label(L"     ", labelheight, MAX_PATH);
		if (!m_spacer1Label) {
			_ASSERT(0);
			abort();
		}
		m_dlgspheader->addParts1(*m_spacer1Label);

		m_btnsp1 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_btnsp1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspheader->addParts1(*m_btnsp1);

		m_btnsp2 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_btnsp2) {
			_ASSERT(0);
			abort();
		}
		m_btnsp1->addParts1(*m_btnsp2);

		m_btnsp3 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_btnsp3) {
			_ASSERT(0);
			abort();
		}
		m_btnsp1->addParts2(*m_btnsp3);

		m_getB = new OWP_Button(L"Get", labelheight);
		if (!m_getB) {
			_ASSERT(0);
			abort();
		}
		m_btnsp2->addParts2(*m_getB);
		m_setB = new OWP_Button(L"Set", labelheight);
		if (!m_setB) {
			_ASSERT(0);
			abort();
		}
		m_btnsp3->addParts1(*m_setB);
		m_saveB = new OWP_Button(L"Save", labelheight);
		if (!m_saveB) {
			_ASSERT(0);
			abort();
		}
		m_btnsp3->addParts2(*m_saveB);

		m_spacer2Label = new OWP_Label(L"     ", labelheight, MAX_PATH);
		if (!m_spacer2Label) {
			_ASSERT(0);
			abort();
		}
		m_dlgspheader->addParts1(*m_spacer2Label);


		size_t nameno;
		size_t namenum = m_dollymemo.size();
		for (nameno = 0; nameno < namenum; nameno++) {
			DOLLYELEM2 curhistory = m_dollymemo[nameno];
			CDollyMemoOWPElem* newowp = new CDollyMemoOWPElem();
			if (!newowp) {
				_ASSERT(0);
				abort();
			}
			int result1 = newowp->SetDollyElem((int)nameno, m_dlgwnd, m_dlgsplist, curhistory);
			if (result1 != 0) {
				_ASSERT(0);
				abort();
			}
			m_owpelemvec.push_back(newowp);
		}

		size_t owpindex;
		size_t owpnum = m_owpelemvec.size();
		for (owpindex = 0; owpindex < owpnum; owpindex++) {
			CDollyMemoOWPElem* curowp = m_owpelemvec[owpindex];
			if (curowp) {
				curowp->AddParts(m_dlgsplist);
			}
		}
		size_t owpindex2;
		for (owpindex2 = 0; owpindex2 < owpnum; owpindex2++) {
			CDollyMemoOWPElem* curowp = m_owpelemvec[owpindex2];
			if (curowp) {
				curowp->SetEventFunc(this);
			}
		}

		m_getB->setButtonListener([=, this]() {
			OnGetDolly();
		});
		m_setB->setButtonListener([=, this]() {
			bool pasteflag = true;//カメラアニメスイッチオンの場合にはカメラアニメにペーストする
			OnSetDolly(pasteflag);
		});
		m_saveB->setButtonListener([=, this]() {
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				//OnSaveDolly();//OnSaveDolly()からはParams2Dlg()が呼ばれてそこからCreateOWPWnd()が呼ばれてm_saveBは作り直されエラーになる
				int opekind = 0;
				::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_DOLLYMEMODLG), opekind);//SendMessageでも上記と同様のエラー.　PostMessage()を使う
			}
		});


		m_dlgwnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgwnd->setPos(WindowPos(m_posx, m_posy));

		//１クリック目問題対応
		m_dlgwnd->refreshPosAndSize();

		m_dlgwnd->callRewrite();

		SetVisible(m_visible);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	return 0;
}

int CDollyMemoDlg::SetNames(std::vector<DOLLYELEM2>& copyhistory)
{

	m_dollymemo = copyhistory;

	ParamsToDlg();

	m_createdflag = true;
	return 0;
}
int CDollyMemoDlg::SetNames()
{
	//m_dollymemo = copyhistory;

	vector<DOLLYELEM2> newhistory;
	size_t owpnum = m_owpelemvec.size();
	if (owpnum <= 0) {
		return 0;
	}
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		newhistory.push_back(m_owpelemvec[owpindex]->GetDollyElem());
	}
	m_dollymemo = newhistory;

	ParamsToDlg();

	m_createdflag = true;
	return 0;
}


int CDollyMemoDlg::OnDelete(int delid)
{
	if (m_dollymemo.empty()) {
		return 0;
	}
	if ((delid < 0) || (delid >= (int)m_dollymemo.size())) {
		return 0;
	}

	WCHAR delcpt[MAX_PATH] = { 0L };
	wcscpy_s(delcpt, MAX_PATH, m_dollymemo[delid].elem1.wfilename);
	delcpt[MAX_PATH - 1] = 0L;
	if (delcpt[0] != 0L) {
		BOOL bexist;
		bexist = PathFileExists(delcpt);
		if (bexist) {
			DeleteFileW(delcpt);
		}
	}

	wcscpy_s(m_dollymemo[delid].elem1.wfilename, MAX_PATH, L"deleted.");
	m_dollymemo[delid].elem1.validflag = false;


	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CDollyMemoOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && (curowp->GetDollyMemoIndex() == delid)) {
			if (curowp->GetDescLabel()) {
				curowp->GetDescLabel()->setName(L"deleted.");
			}
			if (curowp->GetMemoLabel()) {
				curowp->GetMemoLabel()->setName(L"deleted.");
			}
			if (curowp->GetNameCheckBox()) {
				curowp->GetNameCheckBox()->setActive(false);
			}
			if (curowp->GetDelButton()) {
				curowp->GetDelButton()->setActive(false);
			}
		}
	}

	if (m_dlgwnd) {
		m_dlgwnd->callRewrite();
	}
	return 0;
}

int CDollyMemoDlg::OnUp(int srcindex)
{
	if (m_dollymemo.empty()) {
		return 0;
	}
	if ((srcindex < 0) || (srcindex >= (int)m_dollymemo.size())) {
		return 0;
	}

	size_t owpnum = m_owpelemvec.size();
	if ((srcindex < 1) || (owpnum <= 0)) {
		return 0;
	}

	vector<CDollyMemoOWPElem*> newvec;
	CDollyMemoOWPElem* saveelem = m_owpelemvec[srcindex - 1];

	
	//before
	int owpindex;
	for (owpindex = 0; owpindex < (srcindex - 1); owpindex++) {
		newvec.push_back(m_owpelemvec[owpindex]);
	}

	//swap 
	m_owpelemvec[srcindex]->ChangeDollyElemIndex(srcindex - 1);
	newvec.push_back(m_owpelemvec[srcindex]);
	m_owpelemvec[srcindex - 1]->ChangeDollyElemIndex(srcindex);
	newvec.push_back(m_owpelemvec[srcindex - 1]);

	//after
	for (owpindex = srcindex + 1; owpindex < owpnum; owpindex++) {
		newvec.push_back(m_owpelemvec[owpindex]);
	}

	//copy
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		m_owpelemvec[owpindex] = newvec[owpindex];
	}

	//if (m_dlgwnd) {
	//	m_dlgwnd->callRewrite();
	//}

	if (g_mainhwnd && IsWindow(g_mainhwnd)) {
		//OnUp();//OnUp()からはParams2Dlg()が呼ばれてそこからCreateOWPWnd()が呼ばれてm_saveBは作り直されエラーになる
		int opekind = 1;
		::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_DOLLYMEMODLG), opekind);//SendMessageでも上記と同様のエラー.　PostMessage()を使う
	}

	return 0;

}
int CDollyMemoDlg::OnDown(int srcindex)
{
	if (m_dollymemo.empty()) {
		return 0;
	}
	if ((srcindex < 0) || (srcindex >= (int)m_dollymemo.size())) {
		return 0;
	}

	size_t owpnum = m_owpelemvec.size();
	if ((srcindex >= (owpnum - 1)) || (owpnum <= 0)) {
		return 0;
	}

	vector<CDollyMemoOWPElem*> newvec;
	CDollyMemoOWPElem* saveelem = m_owpelemvec[srcindex + 1];


	//before
	int owpindex;
	for (owpindex = 0; owpindex < srcindex; owpindex++) {
		newvec.push_back(m_owpelemvec[owpindex]);
	}

	//swap 
	m_owpelemvec[srcindex + 1]->ChangeDollyElemIndex(srcindex);
	newvec.push_back(m_owpelemvec[srcindex + 1]);
	m_owpelemvec[srcindex]->ChangeDollyElemIndex(srcindex + 1);
	newvec.push_back(m_owpelemvec[srcindex]);

	//after
	for (owpindex = srcindex + 2; owpindex < owpnum; owpindex++) {
		newvec.push_back(m_owpelemvec[owpindex]);
	}

	//copy
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		m_owpelemvec[owpindex] = newvec[owpindex];
	}

	//if (m_dlgwnd) {
	//	m_dlgwnd->callRewrite();
	//}

	if (g_mainhwnd && IsWindow(g_mainhwnd)) {
		//OnDown();//OnDown()からはParams2Dlg()が呼ばれてそこからCreateOWPWnd()が呼ばれてm_saveBは作り直されエラーになる
		int opekind = 1;
		::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_DOLLYMEMODLG), opekind);//SendMessageでも上記と同様のエラー.　PostMessage()を使う
	}

	return 0;
}



int CDollyMemoDlg::SetDollyElem2Camera(DOLLYELEM2 srcelem, bool pasteflag)
{
	if (!m_UpdateFunc) {
		_ASSERT(0);
		return 1;
	}

	if (srcelem.elem1.validflag) {
		m_camerapos = srcelem.elem1.camerapos;
		m_targetpos = srcelem.elem1.cameratarget;
		m_upvec = srcelem.upvec;
		wcscpy_s(m_comment, HISTORYCOMMENTLEN, srcelem.elem1.comment);

		EditParamsToDlg();

		ChaVector3 savepos = g_chacamera.GetCamEye();
		ChaVector3 savetarget = g_chacamera.GetCamTargetPos();
		ChaVector3 saveupvec = g_chacamera.GetCamUpDir();
		float savedist = g_chacamera.GetCamDist();

		g_chacamera.SetCamEye(srcelem.elem1.camerapos);
		g_chacamera.SetCamTargetPos(srcelem.elem1.cameratarget);
		g_chacamera.SetCamUpDir(srcelem.upvec);

		int result;
		if (m_UpdateFunc != nullptr) {
			result = (this->m_UpdateFunc)();//!!!!!!!!!! Main.cpp : int UpdateCameraPosAndTarget()
			if (result == 0) {
				//正常

				if (pasteflag && (m_PasteFunc != nullptr)) {
					(this->m_PasteFunc)();
				}

				return 0;
			}
			else {
				//エラーのため　巻き戻し
				g_chacamera.SetCamEye(savepos);
				g_chacamera.SetCamTargetPos(savetarget);
				g_chacamera.SetCamUpDir(saveupvec);
				g_chacamera.SetCamDist(savedist);

				::MessageBox(g_mainhwnd, L"パラメータ不正のため適用できませんでした。", L"入力エラー", MB_OK);
				return 1;
			}
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

	return 0;
}

int CDollyMemoDlg::OnRadio(int radioid, bool pasteflag, bool callFromKey)
{

	if (m_dollymemo.empty()) {
		return 0;
	}
	if ((radioid < 0) || (radioid >= (int)m_dollymemo.size())) {
		return 0;
	}

	if (!callFromKey && m_onshow) {
		//2024/02/27
		//ShowWindow()を呼び出したときにOnRadio*()内でカメラが動いてしまわないようにフラグで回避
		//ユーザーは　保存したいカメラ位置を決めてからカメラドリーダイアログを出す場合がある　そのときにカメラが動いてしまうと困る
		//m_onshowのセットはAdditiveIK.cppのShowCameraDollyDlg()で行い　m_onshowのリセットは　OnPaintで行う
		return 0;
	}

	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CDollyMemoOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && curowp->GetNameCheckBox()) {
			if (curowp->GetDollyMemoIndex() == radioid) {
				curowp->GetNameCheckBox()->setValue(true, false);
			}
			else {
				curowp->GetNameCheckBox()->setValue(false, false);
			}
		}
	}

	if (m_dlgSc && m_dlgspheader) {
		//2024/07/24 2024/07/25
		//選択項目が画面内に入るようにスクロール処理をすることにした
		int showposline = radioid * 4 + m_dlgspheader->getPartsNum1();
		//+getPartsNum1()について 
		//今回の場合 親ウインドウの直接の子供はm_dlgspheaderとm_dlgsplist(m_dlgSc)の上下２段組で下段だけスクロールする
		//m_dlgsplist(下段)のshowPosLineの制御をする
		//スクロールウインドウのinView()は表示行数計算時に親ウインドウのsize.y/LABEL_SIZE_Yを使う
		//inView()の１行目の位置は、m_dlgspheader(上段)の１行目の位置になっている
		//m_dlgsplistの１行目からの計算にするためにm_dlgspheaderの全行数を足す
		m_dlgSc->inView(showposline);//2024/07/24 inView()は視野内にある場合には何もしない
	}


	//OKボタンを押さないでも反映されるように
	int result = 0;
	if (!m_dollymemo.empty()) {
		DOLLYELEM2 curelem = m_dollymemo[radioid];
		result = SetDollyElem2Camera(curelem, pasteflag);
	}

	if (m_dlgwnd) {
		m_dlgwnd->callRewrite();
	}

	return result;
}


DOLLYELEM2 CDollyMemoDlg::GetFirstValidElem()
{
	int elemnum = (int)m_dollymemo.size();
	int elemno;
	for (elemno = 0; elemno < elemnum; elemno++) {
		DOLLYELEM2 helem = m_dollymemo[elemno];
		if (helem.elem1.validflag) {
			return helem;
		}
		else {
			//有効な履歴が見つかるまでループを続ける
		}
	}

	//有効な履歴がみつからなかった場合
	DOLLYELEM2 inielem;
	inielem.Init();
	return inielem;
}

DOLLYELEM2 CDollyMemoDlg::GetCheckedElem()
{
	int selectedno = -1;//チェックされていない場合

	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CDollyMemoOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && curowp->GetNameCheckBox() && curowp->GetNameCheckBox()->getValue()) {
			//selectedno = owpindex;
			selectedno = curowp->GetDollyMemoIndex();
			break;
		}
	}

	if ((selectedno >= 0) && (selectedno < (int)m_dollymemo.size())) {
		DOLLYELEM2 checkedelem = m_dollymemo[selectedno];
		if (checkedelem.elem1.validflag) {
			return checkedelem;
			//return (int)selectedno;
		}
	}

	//有効な履歴がみつからなかった場合
	DOLLYELEM2 inielem;
	inielem.Init();
	return inielem;

}


int CDollyMemoDlg::OnGetDolly()
{
	m_camerapos = g_chacamera.GetCamEye();
	m_targetpos = g_chacamera.GetCamTargetPos();
	m_upvec = g_chacamera.GetCamUpDir();

	ZeroMemory(m_comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);

	EditParamsToDlg();

	return 0;
}
int CDollyMemoDlg::OnSetDolly(bool pasteflag)
{
	WCHAR strpos[256] = { 0L };
	float posvalue = 0.0f;

	ChaVector3 savepos = g_chacamera.GetCamEye();
	ChaVector3 savetarget = g_chacamera.GetCamTargetPos();
	ChaVector3 saveupvec = g_chacamera.GetCamUpDir();
	float savedist = g_chacamera.GetCamDist();

	g_chacamera.SetBefCamTargetPos(savetarget);

	ChaVector3 newcameye = savepos;
	ChaVector3 newcamtarget = savetarget;

	if (m_posxEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_posxEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			newcameye.x = posvalue;
			g_chacamera.SetCamEye(newcameye);
		}
	}
	if (m_posyEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_posyEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			newcameye.y = posvalue;
			g_chacamera.SetCamEye(newcameye);
		}
	}
	if (m_poszEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_poszEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			newcameye.z = posvalue;
			g_chacamera.SetCamEye(newcameye);
		}
	}


	if (m_tarxEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_tarxEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			newcamtarget.x = posvalue;
			g_chacamera.SetCamTargetPos(newcamtarget);
		}
	}
	if (m_taryEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_taryEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			newcamtarget.y = posvalue;
			g_chacamera.SetCamTargetPos(newcamtarget);
		}
	}
	if (m_tarzEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_tarzEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			newcamtarget.z = posvalue;
			g_chacamera.SetCamTargetPos(newcamtarget);
		}
	}

	if (m_memoEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_memoEdit->getName(strpos, 256);
		size_t commentlen = wcslen(strpos);
		if (commentlen > 0) {
			strpos[HISTORYCOMMENTLEN - 1] = 0L;
			wcscpy_s(m_comment, HISTORYCOMMENTLEN, strpos);
		}
		else {
			m_comment[0] = 0L;
		}
	}

	g_chacamera.SetCamUpDir(m_upvec);
	//#########################################################################
	//m_upvecはエディットボックス編集しない
	//LoadDollyMemo_ver2()で読み込まれる
	//OnGet()と　OnRadio()から呼ばれるSetDollyElem2Camera()で取得した値をそのまま使う
	//#########################################################################


	m_camerapos = g_chacamera.GetCamEye();
	m_targetpos = g_chacamera.GetCamTargetPos();

	g_chacamera.UpdateCameraDist();

	if (g_chacamera.GetCamDist() <= 1e-4) {
		//#########
		//rollback
		//#########

		g_chacamera.SetCamEye(savepos);
		g_chacamera.SetCamTargetPos(savetarget);
		g_chacamera.SetCamUpDir(saveupvec);
		g_chacamera.SetCamDist(savedist);
	}
	else {
		//####################
		// Apply Dlg EditValue
		//####################

		int result;
		if (m_UpdateFunc != nullptr) {
			result = (this->m_UpdateFunc)();//!!!!!!!!!! Main.cpp : int UpdateCameraPosAndTarget()
			if (result == 0) {
				//正常

				if (pasteflag && (m_PasteFunc != nullptr)) {
					(this->m_PasteFunc)();
				}

				return 0;
			}
			else {
				//エラーのため　巻き戻し
				g_chacamera.SetCamEye(savepos);
				g_chacamera.SetCamTargetPos(savetarget);
				g_chacamera.SetCamUpDir(saveupvec);
				g_chacamera.SetCamDist(savedist);

				::MessageBox(g_mainhwnd, L"パラメータ不正のため適用できませんでした。", L"入力エラー", MB_OK);
				return 1;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}


	return 0;
}



int CDollyMemoDlg::OnSaveDolly(bool reloadflag)
{
	BOOL bhandled = true;
	bool pasteflag = true;//カメラアニメスイッチオンの場合にはカメラアニメにペーストする
	LRESULT lresult = OnSetDolly(pasteflag);
	if (lresult == 0) {
		DOLLYELEM2 dollyelem;
		dollyelem.Init();

		dollyelem.elem1.camerapos = m_camerapos;
		dollyelem.elem1.cameratarget = m_targetpos;
		dollyelem.upvec = m_upvec;
		wcscpy_s(dollyelem.elem1.comment, HISTORYCOMMENTLEN, m_comment);
		dollyelem.elem1.validflag = true;



		vector<CDollyMemoOWPElem*> newvec;


		CDollyMemoOWPElem* newowp = new CDollyMemoOWPElem();
		if (!newowp) {
			_ASSERT(0);
			abort();
		}
		int result1 = newowp->SetDollyElem((int)0, m_dlgwnd, m_dlgsplist, dollyelem);
		if (result1 != 0) {
			_ASSERT(0);
			abort();
		}
		newvec.push_back(newowp);


		int orgsize = (int)m_owpelemvec.size();
		int owpindex;
		for (owpindex = 0; owpindex < orgsize; owpindex++) {
			CDollyMemoOWPElem* currentowp = m_owpelemvec[owpindex];
			if (currentowp != nullptr) {
				currentowp->ChangeDollyElemIndex(owpindex + 1);//一番上に１つ足したので、１つ下にずらす
				newvec.push_back(currentowp);
			}
		}

		//copy
		m_owpelemvec.clear();
		int newsize = (int)newvec.size();
		for (owpindex = 0; owpindex < newsize; owpindex++) {
			m_owpelemvec.push_back(newvec[owpindex]);
		}

		SaveDollyFiles(m_envdir);

		if (reloadflag) {
			std::vector<DOLLYELEM2> vecdolly;
			vecdolly.clear();
			
			//LoadDollyMemo(vecdolly, m_envdir);
			LoadDollyMemo_ver3(vecdolly);//ver3:upvec有り, elemindex名前付き　の履歴を読込
			SetNames(vecdolly);
		}

		return 0;
	}
	else {
		return 1;
	}
}

bool CDollyMemoDlg::CheckInitializedVer3(WCHAR* envdir)
{
	//ver3へ移行した印のファイルがあるかどうかを調べる
	WCHAR chkfilename[MAX_PATH] = { 0L };
	swprintf_s(chkfilename, MAX_PATH, L"%s\\%s", envdir, s_checkfilename);

	HANDLE hfile;
	hfile = CreateFile(chkfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hfile != INVALID_HANDLE_VALUE) {
		DWORD sizehigh;
		DWORD bufleng;
		bufleng = GetFileSize(hfile, &sizehigh);
		if (bufleng <= 0) {
			_ASSERT(0);
			CloseHandle(hfile);
			hfile = INVALID_HANDLE_VALUE;
			return false;
		}
		if (sizehigh != 0) {
			_ASSERT(0);
			CloseHandle(hfile);
			hfile = INVALID_HANDLE_VALUE;
			return false;
		}

		DOLLYELEM2 dollyelem;
		dollyelem.Init();

		DWORD rleng, readleng;
		rleng = sizeof(DOLLYELEM2);
		BOOL bsuccess;
		bsuccess = ReadFile(hfile, (void*)&dollyelem, rleng, &readleng, NULL);
		if (!bsuccess || (rleng != readleng)) {
			_ASSERT(0);
			CloseHandle(hfile);
			hfile = INVALID_HANDLE_VALUE;
			return false;
		}
		CloseHandle(hfile);
		hfile = INVALID_HANDLE_VALUE;

		if (dollyelem.elem1.validflag) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

int CDollyMemoDlg::LoadDollyMemo(std::vector<DOLLYELEM2>& vecdolly, WCHAR* envdir)
{
	vecdolly.clear();
	if (envdir == nullptr) {
		_ASSERT(0);
		return 1;
	}
	wcscpy_s(m_envdir, MAX_PATH, envdir);


	bool initialized_ver3 = CheckInitializedVer3(envdir);
	if (!initialized_ver3) {
		//2026/11/01
		//ヒストリーに順番をつけて並び変えることを可能にした
		//変更前と順番が同じ状態から始められるように　２回読み込む

		//1回目読み込み　ファイルタイムでソートする
		LoadDollyMemo_ver1(vecdolly);//ver1:upvec無しの履歴を読込
		LoadDollyMemo_ver2(vecdolly);//ver2:upvec有りの履歴を読込
		//ソート
		if (!vecdolly.empty()) {
			std::sort(vecdolly.begin(), vecdolly.end());
			std::reverse(vecdolly.begin(), vecdolly.end());
			SetNames(vecdolly);
		}

		//ver.3で書き出し　ファイル名にelemindexを付加
		vecdolly.clear();
		SaveDollyFiles(envdir);
	}
	
	//２回目読み込み　ファイル名で並んでいれば　ソートは不要
	LoadDollyMemo_ver3(vecdolly);//ver3:upvec有り, elemindex名前付き　の履歴を読込
	SetNames(vecdolly);

	return 0;
}


bool CDollyMemoDlg::LoadDollyMemo_ver1(std::vector<DOLLYELEM2>& vecdolly)
{
	//###########
	//version 1
	//###########

	//vecdolly.clear();

	WCHAR temppath[MAX_PATH] = { 0L };
	wcscpy_s(temppath, MAX_PATH, m_envdir);
	bool copy2envdir = false;
	//::GetTempPathW(MAX_PATH, temppath);
	//bool resgetappfolder = GetAppFolderPathOchakkoLAB(temppath, MAX_PATH);
	if (temppath[0] != 0L) {
		WCHAR searchfilename[MAX_PATH] = { 0L };
		searchfilename[0] = { 0L };
		//#### ver1 ####
		swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly_*.dol", temppath);
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		hFind = FindFirstFileW(searchfilename, &win32fd);

		if ((hFind == INVALID_HANDLE_VALUE) || (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)) {
			//Projectフォルダに設定ファイルが無い場合には　昔の保存場所を参照する
			temppath[0] = 0L;
			bool resgetappfolder = GetAppFolderPathOchakkoLAB(temppath, MAX_PATH);
			if (resgetappfolder && (temppath[0] != 0L)) {
				copy2envdir = true;
				searchfilename[0] = 0L;
				//#### ver1 ####
				swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly_*.dol", temppath);
				hFind = FindFirstFileW(searchfilename, &win32fd);
			}
		}

		//std::vector<DOLLYELEM2> vechistory;//!!!!!!!!! tmpファイル名
		//vechistory.clear();

		bool notfoundfirst = true;
		if (hFind != INVALID_HANDLE_VALUE) {
			notfoundfirst = false;
			do {
				if ((win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
					DOLLYELEM curelem;
					curelem.Init();
					curelem.filetime = win32fd.ftCreationTime;

					//printf("%s\n", win32fd.cFileName);
					curelem.wfilename[MAX_PATH - 1] = { 0L };
					curelem.wfilename[0] = { 0L };
					swprintf_s(curelem.wfilename, MAX_PATH, L"%s%s", temppath, win32fd.cFileName);


					HANDLE hfile;
					hfile = CreateFile(curelem.wfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN, NULL);
					if (hfile != INVALID_HANDLE_VALUE) {
						DWORD sizehigh;
						DWORD bufleng;
						bufleng = GetFileSize(hfile, &sizehigh);
						if (bufleng <= 0) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}
						if (sizehigh != 0) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}

						DOLLYELEM dollyelem;
						dollyelem.Init();

						DWORD rleng, readleng;
						rleng = sizeof(DOLLYELEM);//#### ver1 ####
						BOOL bsuccess;
						bsuccess = ReadFile(hfile, (void*)&dollyelem, rleng, &readleng, NULL);
						if (!bsuccess || (rleng != readleng)) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}
						CloseHandle(hfile);
						hfile = INVALID_HANDLE_VALUE;


						curelem.camerapos = dollyelem.camerapos;
						curelem.cameratarget = dollyelem.cameratarget;
						wcscpy_s(curelem.comment, HISTORYCOMMENTLEN, dollyelem.comment);
						curelem.validflag = true;//!!!!!!!!!!!!!!

						DOLLYELEM2 curelem2;
						curelem2.Init();
						curelem2.elem1 = curelem;
						curelem2.upvec = ChaVector3(0.0f, 1.0f, 0.0f);//ver1のupvecは (0, 1, 0)
						curelem2.noupvecflag = true;//#### ver1 ####

						vecdolly.push_back(curelem2);
					}
					else {
						_ASSERT(0);
						continue;
					}
				}
			} while (FindNextFile(hFind, &win32fd));
			FindClose(hFind);
		}
	}
	else {
		_ASSERT(0);
	}

	return copy2envdir;
}

bool CDollyMemoDlg::LoadDollyMemo_ver2(std::vector<DOLLYELEM2>& vecdolly)
{
	//###########
	//version 2
	//###########
		
	//vecdolly.clear();

	WCHAR temppath[MAX_PATH] = { 0L };
	wcscpy_s(temppath, MAX_PATH, m_envdir);
	bool copy2envdir = false;
	//bool resgetappfolder = GetAppFolderPathOchakkoLAB(temppath, MAX_PATH);
	if (temppath[0] != 0L) {
		WCHAR searchfilename[MAX_PATH] = { 0L };
		searchfilename[0] = { 0L };
		//#### ver2 ####
		swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly2_*.dol", temppath);
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		hFind = FindFirstFileW(searchfilename, &win32fd);

		if ((hFind == INVALID_HANDLE_VALUE) || (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)) {
			//Projectフォルダに設定ファイルが無い場合には　昔の保存場所を参照する
			copy2envdir = true;
			temppath[0] = 0L;
			bool resgetappfolder = GetAppFolderPathOchakkoLAB(temppath, MAX_PATH);
			if (resgetappfolder && (temppath[0] != 0L)) {
				searchfilename[0] = 0L;
				//#### ver2 ####
				swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly2_*.dol", temppath);
				hFind = FindFirstFileW(searchfilename, &win32fd);
			}
		}

		//std::vector<DOLLYELEM2> vechistory;//!!!!!!!!! tmpファイル名
		//vechistory.clear();

		bool notfoundfirst = true;
		if (hFind != INVALID_HANDLE_VALUE) {
			notfoundfirst = false;
			do {
				if ((win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
					DOLLYELEM2 curelem;
					curelem.Init();
					curelem.elem1.filetime = win32fd.ftCreationTime;

					//printf("%s\n", win32fd.cFileName);
					curelem.elem1.wfilename[MAX_PATH - 1] = { 0L };
					curelem.elem1.wfilename[0] = { 0L };
					swprintf_s(curelem.elem1.wfilename, MAX_PATH, L"%s%s", temppath, win32fd.cFileName);


					HANDLE hfile;
					hfile = CreateFile(curelem.elem1.wfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN, NULL);
					if (hfile != INVALID_HANDLE_VALUE) {
						DWORD sizehigh;
						DWORD bufleng;
						bufleng = GetFileSize(hfile, &sizehigh);
						if (bufleng <= 0) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}
						if (sizehigh != 0) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}

						DOLLYELEM2 dollyelem;
						dollyelem.Init();

						DWORD rleng, readleng;
						rleng = sizeof(DOLLYELEM2);//#### ver2 ####
						BOOL bsuccess;
						bsuccess = ReadFile(hfile, (void*)&dollyelem, rleng, &readleng, NULL);
						if (!bsuccess || (rleng != readleng)) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}
						CloseHandle(hfile);
						hfile = INVALID_HANDLE_VALUE;


						curelem.elem1.camerapos = dollyelem.elem1.camerapos;
						curelem.elem1.cameratarget = dollyelem.elem1.cameratarget;
						curelem.upvec = dollyelem.upvec;//#### ver2 ####
						curelem.noupvecflag = false;//#### ver2 ####
						wcscpy_s(curelem.elem1.comment, HISTORYCOMMENTLEN, dollyelem.elem1.comment);
						curelem.elem1.validflag = true;//!!!!!!!!!!!!!!

						vecdolly.push_back(curelem);
					}
					else {
						_ASSERT(0);
						continue;
					}
				}
			} while (FindNextFile(hFind, &win32fd));
			FindClose(hFind);
		}
	}
	else {
		_ASSERT(0);
	}

	return copy2envdir;
}

bool CDollyMemoDlg::LoadDollyMemo_ver3(std::vector<DOLLYELEM2>& vecdolly)
{
	//###########
	//version 3
	//###########

	//vecdolly.clear();

	WCHAR temppath[MAX_PATH] = { 0L };
	wcscpy_s(temppath, MAX_PATH, m_envdir);
	bool copy2envdir = false;
	//bool resgetappfolder = GetAppFolderPathOchakkoLAB(temppath, MAX_PATH);
	if (temppath[0] != 0L) {
		WCHAR searchfilename[MAX_PATH] = { 0L };
		searchfilename[0] = { 0L };
		//#### ver3 ####
		swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly3_*.dol", temppath);
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		hFind = FindFirstFileW(searchfilename, &win32fd);

		int elemindex = 0;
		bool notfoundfirst = true;
		if (hFind != INVALID_HANDLE_VALUE) {
			notfoundfirst = false;
			do {
				if ((win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
					DOLLYELEM2 curelem;
					curelem.Init();
					curelem.elem1.filetime = win32fd.ftCreationTime;

					//printf("%s\n", win32fd.cFileName);
					curelem.elem1.wfilename[MAX_PATH - 1] = { 0L };
					curelem.elem1.wfilename[0] = { 0L };
					swprintf_s(curelem.elem1.wfilename, MAX_PATH, L"%s%s", temppath, win32fd.cFileName);


					HANDLE hfile;
					hfile = CreateFile(curelem.elem1.wfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN, NULL);
					if (hfile != INVALID_HANDLE_VALUE) {
						DWORD sizehigh;
						DWORD bufleng;
						bufleng = GetFileSize(hfile, &sizehigh);
						if (bufleng <= 0) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}
						if (sizehigh != 0) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}

						DOLLYELEM2 dollyelem;
						dollyelem.Init();

						DWORD rleng, readleng;
						rleng = sizeof(DOLLYELEM2);//#### ver2 ####
						BOOL bsuccess;
						bsuccess = ReadFile(hfile, (void*)&dollyelem, rleng, &readleng, NULL);
						if (!bsuccess || (rleng != readleng)) {
							_ASSERT(0);
							CloseHandle(hfile);
							hfile = INVALID_HANDLE_VALUE;
							continue;
						}
						CloseHandle(hfile);
						hfile = INVALID_HANDLE_VALUE;


						curelem.elem1.camerapos = dollyelem.elem1.camerapos;
						curelem.elem1.cameratarget = dollyelem.elem1.cameratarget;
						curelem.upvec = dollyelem.upvec;//#### ver2 ####
						curelem.noupvecflag = false;//#### ver2 ####
						wcscpy_s(curelem.elem1.comment, HISTORYCOMMENTLEN, dollyelem.elem1.comment);
						curelem.elem1.validflag = true;//!!!!!!!!!!!!!!

						vecdolly.push_back(curelem);
					}
					else {
						_ASSERT(0);
						continue;
					}
				}
			} while (FindNextFile(hFind, &win32fd));
			FindClose(hFind);
		}
	}
	else {
		_ASSERT(0);
	}

	return copy2envdir;
}


int CDollyMemoDlg::SaveDollyFiles(WCHAR* newenvdir)
{
	if (newenvdir == nullptr) {
		_ASSERT(0);
		return 1;
	}


	{
		//ver3へ移行した印として　ファイルを保存する(次回以降の読み込み時に必要)
		WCHAR savefilename[MAX_PATH] = { 0L };
		swprintf_s(savefilename, MAX_PATH, L"%s\\%s", newenvdir, s_checkfilename);

		HANDLE hfile;
		hfile = CreateFile(savefilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (hfile != INVALID_HANDLE_VALUE) {
			SetEndOfFile(hfile);
			DOLLYELEM2 dollyelem;
			dollyelem.Init();
			dollyelem.elem1.camerapos = m_camerapos;
			dollyelem.elem1.cameratarget = m_targetpos;
			dollyelem.upvec = m_upvec;
			wcscpy_s(dollyelem.elem1.comment, HISTORYCOMMENTLEN, m_comment);
			dollyelem.elem1.validflag = true;

			DWORD writelen = 0;
			WriteFile(hfile, &dollyelem, (DWORD)(sizeof(DOLLYELEM2)), &writelen, NULL);
			_ASSERT((DWORD)sizeof(DOLLYELEM2) == writelen);
			FlushFileBuffers(hfile);
			SetEndOfFile(hfile);
			CloseHandle(hfile);
		}
		else {
			_ASSERT(0);
		}
	}


	int elemnum = (int)m_owpelemvec.size();
	int elemindex;
	for (elemindex = 0; elemindex < elemnum; elemindex++) {
		CDollyMemoOWPElem* curowpelem = m_owpelemvec[elemindex];
		if (curowpelem != nullptr) {
			WCHAR savefilename[MAX_PATH] = { 0L };
			swprintf_s(savefilename, MAX_PATH, L"%s\\MB3DOpenProjDolly3_%04d.dol", newenvdir, elemindex);// curowpelem->GetDollyMemoIndex());

			HANDLE hfile;
			hfile = CreateFile(savefilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
				FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hfile != INVALID_HANDLE_VALUE) {
				SetEndOfFile(hfile);
				DOLLYELEM2 dollyelem = curowpelem->GetDollyElem();
				DWORD writelen = 0;
				WriteFile(hfile, &dollyelem, (DWORD)(sizeof(DOLLYELEM2)), &writelen, NULL);
				_ASSERT((DWORD)sizeof(DOLLYELEM2) == writelen);
				FlushFileBuffers(hfile);
				SetEndOfFile(hfile);
				CloseHandle(hfile);
			}
			else {
				_ASSERT(0);
			}
		}
	}

	return 0;
}

//LRESULT CDollyMemoDlg::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	//LRESULT result = DefWindowProcW();
//	bHandled = TRUE;
//	LRESULT result = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
//
//
//	//2024/02/27
//	//ShowWindow()を呼び出したときにOnRadio*()内でカメラが動いてしまわないようにフラグで回避
//	//ユーザーは　保存したいカメラ位置を決めてからカメラドリーダイアログを出す場合がある　そのときにカメラが動いてしまうと困る
//	//m_onshowのセットはAdditiveIK.cppのShowCameraDollyDlg()で行い　m_onshowのリセットは　OnPaintで行う
//	SetOnShow(false);
//
//
//	return result;
//}


void CDollyMemoDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgwnd) {
			m_dlgwnd->setListenMouse(true);
			m_dlgwnd->setVisible(true);
			if (m_dlgSc && m_dlgspheader) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_dlgSc->autoResize();

				////選択項目が画面内に入るようにスクロール処理をすることにした
				//int selectindex = m_selectindexmap[m_model];
				//int showposline = selectindex * 4 + m_dlgspcombo->getPartsNum1();
				////+getPartsNum1()について 
				////今回の場合 親ウインドウの直接の子供はm_dlgspcomboとm_dlgsplist(m_dlgSc)の上下２段組で下段だけスクロールする
				////m_dlgsplist(下段)のshowPosLineの制御をする
				////スクロールウインドウのinView()は表示行数計算時に親ウインドウのsize.y/LABEL_SIZE_Yを使う
				////inView()の１行目の位置は、m_dlgspcombo(上段)の１行目の位置になっている
				////m_dlgsplistの１行目からの計算にするためにm_dlgspcomboの全行数を足す
				//m_dlgSc->inView(showposline);//内部でautoResize()も呼ぶ
			}
			m_dlgwnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgwnd) {
			m_dlgwnd->setVisible(false);
			m_dlgwnd->setListenMouse(false);
		}
	}
	m_visible = srcflag;
}
