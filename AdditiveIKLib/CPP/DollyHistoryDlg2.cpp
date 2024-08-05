#include "stdafx.h"

#include <DollyHistoryDlg2.h>
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


CDollyHistoryOWPElem::CDollyHistoryOWPElem()
{
	InitParams();
}
CDollyHistoryOWPElem::~CDollyHistoryOWPElem()
{
	DestroyObjs();
}
int CDollyHistoryOWPElem::SetDollyElem(
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
int CDollyHistoryOWPElem::AddParts(OrgWinGUI::OWP_Separator* parentsp)
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
	if (m_descLabel) {
		parentsp->addParts1(*m_descLabel);
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
int CDollyHistoryOWPElem::SetEventFunc(CDollyHistoryDlg2* srcdlg)
{
	m_parentdlg = srcdlg;

	if (m_nameChk) {
		m_nameChk->setButtonListener([=, this]() {
			if (m_parentdlg) {
				bool pasteflag = true;//カメラアニメスイッチオンの場合にはカメラアニメにペーストする
				m_parentdlg->OnRadio(m_dollyelemindex, pasteflag);
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

	return 0;
}

void CDollyHistoryOWPElem::InitParams()
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

	m_parentdlg = nullptr;
}
void CDollyHistoryOWPElem::DestroyObjs()
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
}




/////////////////////////////////////////////////////////////////////////////
// CDollyHistoryDlg2

CDollyHistoryDlg2::CDollyHistoryDlg2()
{
	InitParams();
}

CDollyHistoryDlg2::~CDollyHistoryDlg2()
{
	DestroyObjs();
}
	
int CDollyHistoryDlg2::DestroyObjs()
{
	DestroyOWPWnd();
	InitParams();

	return 0;
}

void CDollyHistoryDlg2::InitParams()
{
	m_createdflag = false;
	m_visible = false;
	m_onshow = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_dollyhistory.clear();
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

void CDollyHistoryDlg2::SetUpdateFunc(int (*UpdateFunc)(), int (*PasteFunc)())
{
	m_UpdateFunc = UpdateFunc;
	m_PasteFunc = PasteFunc;
}

int CDollyHistoryDlg2::DestroyOWPWnd()
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
int CDollyHistoryDlg2::DestroyOWPElem()
{
	size_t elemnum = m_owpelemvec.size();
	size_t elemindex;
	for (elemindex = 0; elemindex < elemnum; elemindex++) {
		CDollyHistoryOWPElem* curowp = m_owpelemvec[elemindex];
		if (curowp) {
			delete curowp;
		}
	}
	m_owpelemvec.clear();

	return 0;
}

int CDollyHistoryDlg2::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}


//LRESULT CDollyHistoryDlg2::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
//LRESULT CDollyHistoryDlg2::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	m_initsearchcomboflag = false;
//	//EndDialog(wID);
//	//DestroyWindow();
//	ShowWindow(SW_HIDE);
//	return 0;
//}

int CDollyHistoryDlg2::EditParamsToDlg()
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


int CDollyHistoryDlg2::ParamsToDlg()
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

int CDollyHistoryDlg2::CreateOWPWnd()
{

	DestroyOWPWnd();

	m_dlgwnd = new OrgWindow(
		0,
		_T("DollyHistoryDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("DollyHistoryDlg"),	//タイトル
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
		double rate50 = 0.50;

		m_dlgspheader = new OWP_Separator(m_dlgwnd, true, 0.995, false);
		if (!m_dlgspheader) {
			_ASSERT(0);
			abort();
		}
		m_dlgwnd->addParts(*m_dlgspheader);

		m_dlgSc = new OWP_ScrollWnd(L"DollyHistoryScroll", true, labelheight);
		if (!m_dlgSc) {
			_ASSERT(0);
			abort();
		}
		int linedatanum = (int)((double)m_dollyhistory.size() * 4.0 * 1.25);
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
		size_t namenum = m_dollyhistory.size();
		for (nameno = 0; nameno < namenum; nameno++) {
			DOLLYELEM2 curhistory = m_dollyhistory[nameno];
			CDollyHistoryOWPElem* newowp = new CDollyHistoryOWPElem();
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
			CDollyHistoryOWPElem* curowp = m_owpelemvec[owpindex];
			if (curowp) {
				curowp->AddParts(m_dlgsplist);
			}
		}
		size_t owpindex2;
		for (owpindex2 = 0; owpindex2 < owpnum; owpindex2++) {
			CDollyHistoryOWPElem* curowp = m_owpelemvec[owpindex2];
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
			//OnSaveDolly();//OnSaveDolly()からはParams2Dlg()が呼ばれてそこからCreateOWPWnd()が呼ばれてm_saveBは作り直されエラーになる
			::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + 98), 0);//SendMessageでも上記と同様のエラー.　PostMessage()を使う
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

int CDollyHistoryDlg2::SetNames(std::vector<DOLLYELEM2>& copyhistory)
{

	m_dollyhistory = copyhistory;

	ParamsToDlg();

	m_createdflag = true;
	return 0;
}


int CDollyHistoryDlg2::OnDelete(int delid)
{
	if (m_dollyhistory.empty()) {
		return 0;
	}
	if ((delid < 0) || (delid >= (int)m_dollyhistory.size())) {
		return 0;
	}

	WCHAR delcpt[MAX_PATH] = { 0L };
	wcscpy_s(delcpt, MAX_PATH, m_dollyhistory[delid].elem1.wfilename);
	delcpt[MAX_PATH - 1] = 0L;
	if (delcpt[0] != 0L) {
		BOOL bexist;
		bexist = PathFileExists(delcpt);
		if (bexist) {
			DeleteFileW(delcpt);
		}
	}

	wcscpy_s(m_dollyhistory[delid].elem1.wfilename, MAX_PATH, L"deleted.");
	m_dollyhistory[delid].elem1.validflag = false;


	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CDollyHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && (curowp->GetDollyHistoryIndex() == delid)) {
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


int CDollyHistoryDlg2::SetDollyElem2Camera(DOLLYELEM2 srcelem, bool pasteflag)
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

		ChaVector3 savepos = g_camEye;
		ChaVector3 savetarget = g_camtargetpos;
		ChaVector3 saveupvec = g_cameraupdir;
		float savedist = g_camdist;

		g_camEye = srcelem.elem1.camerapos;
		g_camtargetpos = srcelem.elem1.cameratarget;
		g_cameraupdir = srcelem.upvec;

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
				g_camEye = savepos;
				g_camtargetpos = savetarget;
				g_cameraupdir = saveupvec;
				g_camdist = savedist;

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

int CDollyHistoryDlg2::OnRadio(int radioid, bool pasteflag)
{

	if (m_dollyhistory.empty()) {
		return 0;
	}
	if ((radioid < 0) || (radioid >= (int)m_dollyhistory.size())) {
		return 0;
	}

	if (m_onshow == true) {
		//2024/02/27
		//ShowWindow()を呼び出したときにOnRadio*()内でカメラが動いてしまわないようにフラグで回避
		//ユーザーは　保存したいカメラ位置を決めてからカメラドリーダイアログを出す場合がある　そのときにカメラが動いてしまうと困る
		//m_onshowのセットはAdditiveIK.cppのShowCameraDollyDlg()で行い　m_onshowのリセットは　OnPaintで行う
		return 0;
	}

	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CDollyHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && curowp->GetNameCheckBox()) {
			if (curowp->GetDollyHistoryIndex() == radioid) {
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
	if (!m_dollyhistory.empty()) {
		DOLLYELEM2 curelem = m_dollyhistory[radioid];
		result = SetDollyElem2Camera(curelem, pasteflag);
	}

	if (m_dlgwnd) {
		m_dlgwnd->callRewrite();
	}

	return result;
}


DOLLYELEM2 CDollyHistoryDlg2::GetFirstValidElem()
{
	int elemnum = (int)m_dollyhistory.size();
	int elemno;
	for (elemno = 0; elemno < elemnum; elemno++) {
		DOLLYELEM2 helem = m_dollyhistory[elemno];
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

DOLLYELEM2 CDollyHistoryDlg2::GetCheckedElem()
{
	int selectedno = -1;//チェックされていない場合

	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CDollyHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && curowp->GetNameCheckBox() && curowp->GetNameCheckBox()->getValue()) {
			//selectedno = owpindex;
			selectedno = curowp->GetDollyHistoryIndex();
			break;
		}
	}

	if ((selectedno >= 0) && (selectedno < (int)m_dollyhistory.size())) {
		DOLLYELEM2 checkedelem = m_dollyhistory[selectedno];
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


int CDollyHistoryDlg2::OnGetDolly()
{
	m_camerapos = g_camEye;
	m_targetpos = g_camtargetpos;
	m_upvec = g_cameraupdir;
	ZeroMemory(m_comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);

	EditParamsToDlg();

	return 0;
}
int CDollyHistoryDlg2::OnSetDolly(bool pasteflag)
{
	WCHAR strpos[256] = { 0L };
	float posvalue = 0.0f;

	ChaVector3 savecameye = g_camEye;
	ChaVector3 savetarget = g_camtargetpos;
	ChaVector3 saveupvec = g_cameraupdir;
	float savedist = g_camdist;

	g_befcamtargetpos = g_camtargetpos;

	if (m_posxEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_posxEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			g_camEye.x = posvalue;
		}
	}
	if (m_posyEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_posyEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			g_camEye.y = posvalue;
		}
	}
	if (m_poszEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_poszEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			g_camEye.z = posvalue;
		}
	}


	if (m_tarxEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_tarxEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			g_camtargetpos.x = posvalue;
		}
	}
	if (m_taryEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_taryEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			g_camtargetpos.y = posvalue;
		}
	}
	if (m_tarzEdit) {
		ZeroMemory(strpos, sizeof(WCHAR) * 256);
		m_tarzEdit->getName(strpos, 256);
		posvalue = (float)_wtof(strpos);
		if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
			g_camtargetpos.z = posvalue;
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

	g_cameraupdir = m_upvec;
	//#########################################################################
	//m_upvecはエディットボックス編集しない
	//LoadDollyHistory_ver2()で読み込まれる
	//OnGet()と　OnRadio()から呼ばれるSetDollyElem2Camera()で取得した値をそのまま使う
	//#########################################################################


	m_camerapos = g_camEye;
	m_targetpos = g_camtargetpos;

	ChaVector3 diffv;
	diffv = g_camEye - g_camtargetpos;
	g_camdist = (float)ChaVector3LengthDbl(&diffv);
	if (g_camdist <= 1e-4) {
		//#########
		//rollback
		//#########

		g_camEye = savecameye;
		g_camtargetpos = savetarget;
		g_cameraupdir = saveupvec;
		g_camdist = savedist;
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
				g_camEye = savecameye;
				g_camtargetpos = savetarget;
				g_cameraupdir = saveupvec;
				g_camdist = savedist;

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



int CDollyHistoryDlg2::OnSaveDolly()
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


		WCHAR temppath[MAX_PATH] = { 0L };
		::GetTempPathW(MAX_PATH, temppath);
		if (temppath[0] != 0L) {
			SYSTEMTIME localtime;
			GetLocalTime(&localtime);
			WCHAR dollyfilepath[MAX_PATH] = { 0L };
			//swprintf_s(dollyfilepath, MAX_PATH, L"%s\\MB3DOpenProjDolly_%04u%02u%02u%02u%02u%02u.dol",
			swprintf_s(dollyfilepath, MAX_PATH, L"%s\\MB3DOpenProjDolly2_%04u%02u%02u%02u%02u%02u.dol",
				temppath,
				localtime.wYear, localtime.wMonth, localtime.wDay, localtime.wHour, localtime.wMinute, localtime.wSecond);
			HANDLE hfile;
			hfile = CreateFile(dollyfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
				FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hfile != INVALID_HANDLE_VALUE) {
				DWORD writelen = 0;
				WriteFile(hfile, &dollyelem, (DWORD)(sizeof(DOLLYELEM2)), &writelen, NULL);
				_ASSERT((DWORD)sizeof(DOLLYELEM2) == writelen);
				CloseHandle(hfile);

				vector<DOLLYELEM2> updatehistory;
				updatehistory.clear();
				LoadDollyHistory(updatehistory);

				SetNames(updatehistory);

				return 0;
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
	}
	else {
		return 1;
	}
}

int CDollyHistoryDlg2::LoadDollyHistory(std::vector<DOLLYELEM2>& vecdolly)
{
	vecdolly.clear();

	LoadDollyHistory_ver1(vecdolly);//ver1:upvec無しの履歴を読込
	LoadDollyHistory_ver2(vecdolly);//ver2:upvec有りの履歴を読込

	//ソート
	if (!vecdolly.empty()) {
		std::sort(vecdolly.begin(), vecdolly.end());
		std::reverse(vecdolly.begin(), vecdolly.end());
	}
	else {
		vecdolly.clear();
	}

	return 0;
}


int CDollyHistoryDlg2::LoadDollyHistory_ver1(std::vector<DOLLYELEM2>& vecdolly)
{
	//###########
	//version 1
	//###########

	//vecdolly.clear();

	WCHAR temppath[MAX_PATH] = { 0L };
	::GetTempPathW(MAX_PATH, temppath);
	if (temppath[0] != 0L) {
		WCHAR searchfilename[MAX_PATH] = { 0L };
		searchfilename[0] = { 0L };
		//#### ver1 ####
		swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly_*.dol", temppath);
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		hFind = FindFirstFileW(searchfilename, &win32fd);

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


		//if (!vechistory.empty()) {
		//	std::sort(vechistory.begin(), vechistory.end());
		//	std::reverse(vechistory.begin(), vechistory.end());
		//	vecdolly = vechistory;
		//}
		//else {
		//	vecdolly.clear();
		//}

		return 0;
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CDollyHistoryDlg2::LoadDollyHistory_ver2(std::vector<DOLLYELEM2>& vecdolly)
{
	//###########
	//version 2
	//###########
		
	//vecdolly.clear();

	WCHAR temppath[MAX_PATH] = { 0L };
	::GetTempPathW(MAX_PATH, temppath);
	if (temppath[0] != 0L) {
		WCHAR searchfilename[MAX_PATH] = { 0L };
		searchfilename[0] = { 0L };
		//#### ver2 ####
		swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly2_*.dol", temppath);
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		hFind = FindFirstFileW(searchfilename, &win32fd);

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


		//if (!vechistory.empty()) {
		//	std::sort(vechistory.begin(), vechistory.end());
		//	std::reverse(vechistory.begin(), vechistory.end());
		//	vecdolly = vechistory;
		//}
		//else {
		//	vecdolly.clear();
		//}

		return 0;
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

//LRESULT CDollyHistoryDlg2::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


void CDollyHistoryDlg2::SetVisible(bool srcflag)
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
