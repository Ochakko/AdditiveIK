#include "stdafx.h"
#include "DollyHistoryDlg.h"
#include "GetDlgParams.h"
#include "SetDlgPos.h"

#include <Model.h>

#include <GlobalVar.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDollyHistoryDlg

CDollyHistoryDlg::CDollyHistoryDlg()
{
	InitParams();
}

CDollyHistoryDlg::~CDollyHistoryDlg()
{
	DestroyObjs();
}
	
int CDollyHistoryDlg::DestroyObjs()
{
	return 0;
}

void CDollyHistoryDlg::InitParams()
{
	m_createdflag = false;

	m_pagenum = 0;
	m_currentpage = 0;
	m_startno = 0;

	m_namenum = 0;
	m_dollyhistory.clear();

	m_camerapos = ChaVector3(0.0f, 0.0f, 10.0f);
	m_targetpos = ChaVector3(0.0f, 0.0f, 0.0f);
	ZeroMemory(m_comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);


	m_ctrlid[0] = IDC_RADIO1;
	m_ctrlid[1] = IDC_RADIO2;
	m_ctrlid[2] = IDC_RADIO3;
	m_ctrlid[3] = IDC_RADIO4;
	m_ctrlid[4] = IDC_RADIO5;
	m_ctrlid[5] = IDC_RADIO6;
	m_ctrlid[6] = IDC_RADIO7;
	m_ctrlid[7] = IDC_RADIO8;
	m_ctrlid[8] = IDC_RADIO9;
	m_ctrlid[9] = IDC_RADIO10;

	m_targetid[0] = IDC_TARGET1;
	m_targetid[1] = IDC_TARGET2;
	m_targetid[2] = IDC_TARGET3;
	m_targetid[3] = IDC_TARGET4;
	m_targetid[4] = IDC_TARGET5;
	m_targetid[5] = IDC_TARGET6;
	m_targetid[6] = IDC_TARGET7;
	m_targetid[7] = IDC_TARGET8;
	m_targetid[8] = IDC_TARGET9;
	m_targetid[9] = IDC_TARGET10;

	m_commentid[0] = IDC_TEXT_COMMENT1;
	m_commentid[1] = IDC_TEXT_COMMENT11;
	m_commentid[2] = IDC_TEXT_COMMENT12;
	m_commentid[3] = IDC_TEXT_COMMENT13;
	m_commentid[4] = IDC_TEXT_COMMENT14;
	m_commentid[5] = IDC_TEXT_COMMENT15;
	m_commentid[6] = IDC_TEXT_COMMENT16;
	m_commentid[7] = IDC_TEXT_COMMENT17;
	m_commentid[8] = IDC_TEXT_COMMENT18;
	m_commentid[9] = IDC_TEXT_COMMENT19;


	m_UpdateFunc = nullptr;
}

void CDollyHistoryDlg::SetUpdateFunc(int (*UpdateFunc)())
{
	m_UpdateFunc = UpdateFunc;
}



//LRESULT CDollyHistoryDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	int ret;
//
//	InitCommonControls();
//
//	m_dlg_wnd = m_hWnd;
//
//	ret = ParamsToDlg();
//	_ASSERT( !ret );
//
//
//	return 1;  // システムにフォーカスを設定させます
//}

LRESULT CDollyHistoryDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//int ret;
	
	InitCommonControls();
	

	//このダイアログは　右ペイン埋め込みなので　デスクトップ中央には動かさない
	//SetDlgPosDesktopCenter(m_hWnd, HWND_TOPMOST);
	//RECT dlgrect;
	//::GetWindowRect(m_hWnd, &dlgrect);
	//SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);


	//m_dlg_wnd = m_hWnd;
	
	//ret = ParamsToDlg();
	//_ASSERT( !ret );
	
	return 0;
}
LRESULT CDollyHistoryDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}



//LRESULT CDollyHistoryDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
//LRESULT CDollyHistoryDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	m_initsearchcomboflag = false;
//	//EndDialog(wID);
//	//DestroyWindow();
//	ShowWindow(SW_HIDE);
//	return 0;
//}

int CDollyHistoryDlg::EditParamsToDlg()
{
	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%.2f", m_camerapos.x);
		m_dlg_wnd.SetDlgItemTextW(IDC_POSITION_X, dispname);
	}
	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%.2f", m_camerapos.y);
		m_dlg_wnd.SetDlgItemTextW(IDC_POSITION_Y, dispname);
	}
	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%.2f", m_camerapos.z);
		m_dlg_wnd.SetDlgItemTextW(IDC_POSITION_Z, dispname);
	}


	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%.2f", m_targetpos.x);
		m_dlg_wnd.SetDlgItemTextW(IDC_TARGET_X, dispname);
	}
	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%.2f", m_targetpos.y);
		m_dlg_wnd.SetDlgItemTextW(IDC_TARGET_Y, dispname);
	}
	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%.2f", m_targetpos.z);
		m_dlg_wnd.SetDlgItemTextW(IDC_TARGET_Z, dispname);
	}



	{
		WCHAR dispname[MAX_PATH] = { 0L };
		swprintf_s(dispname, MAX_PATH, L"%s", m_comment);
		m_dlg_wnd.SetDlgItemTextW(IDC_COMMENT, dispname);
	}

	return 0;
}


int CDollyHistoryDlg::ParamsToDlg()
{
	//m_dlg_wnd.CheckRadioButton(IDC_RADIO1, IDC_RADIO5, (g_ClearColorIndex + IDC_RADIO1));

	m_dlg_wnd = m_hWnd;

	EditParamsToDlg();


	bool ischeck = false;
	size_t nameno;
	for (nameno = 0; nameno < m_namenum; nameno++) {
		if (m_dollyhistory[m_startno + nameno].validflag) {
			{
				WCHAR dispname[MAX_PATH] = { 0L };
				ChaVector3 currentpos = m_dollyhistory[m_startno + nameno].camerapos;
				swprintf_s(dispname, MAX_PATH, L"%.2f, %.2f, %.2f", currentpos.x, currentpos.y, currentpos.z);
				m_dlg_wnd.SetDlgItemTextW(m_ctrlid[nameno], dispname);
			}

			{
				WCHAR dispname[MAX_PATH] = { 0L };
				ChaVector3 currenttarget = m_dollyhistory[m_startno + nameno].cameratarget;
				swprintf_s(dispname, MAX_PATH, L"%.2f, %.2f, %.2f", currenttarget.x, currenttarget.y, currenttarget.z);
				m_dlg_wnd.SetDlgItemTextW(m_targetid[nameno], dispname);
			}

			{
				WCHAR dispname[MAX_PATH] = { 0L };
				swprintf_s(dispname, MAX_PATH, L"%s", m_dollyhistory[m_startno + nameno].comment);
				m_dlg_wnd.SetDlgItemTextW(m_commentid[nameno], dispname);
			}
		}
		else {
			m_dlg_wnd.SetDlgItemTextW(m_ctrlid[nameno], L"Invalid.");
			m_dlg_wnd.SetDlgItemTextW(m_targetid[nameno], L"Invalid.");
			m_dlg_wnd.SetDlgItemTextW(m_commentid[nameno], L"Invalid.");
		}
	}
	size_t nameno2;
	for (nameno2 = m_namenum; nameno2 < DOLLYNUMFORDISP; nameno2++) {
		m_dlg_wnd.SetDlgItemTextW(m_ctrlid[nameno2], L"no more.");
		m_dlg_wnd.SetDlgItemTextW(m_targetid[nameno2], L"no more.");
		m_dlg_wnd.SetDlgItemTextW(m_commentid[nameno2], L"no more.");
	}

	
	m_dlg_wnd.CheckRadioButton(IDC_RADIO1, IDC_RADIO10, IDC_RADIO1);


	if (m_pagenum > 0) {
		int disppage;
		disppage = m_currentpage % m_pagenum + 1;
		WCHAR strpages[256] = { 0L };
		swprintf_s(strpages, 256, L"%03d / %03d Pages", disppage, m_pagenum);
		m_dlg_wnd.SetDlgItemTextW(IDC_PAGES, strpages);
	}
	else {
		WCHAR strpages[256] = { 0L };
		swprintf_s(strpages, 256, L"0 / 0 NoPages");
		m_dlg_wnd.SetDlgItemTextW(IDC_PAGES, strpages);
	}


	return 0;
}


int CDollyHistoryDlg::SetNames(std::vector<DOLLYELEM>& copyhistory)
{
	m_dollyhistory = copyhistory;


	int numhistory2 = (int)m_dollyhistory.size();
	int fullpagenum = numhistory2 / DOLLYNUMFORDISP;//満たされているページの数
	m_pagenum = fullpagenum;//端数込みのページ数
	if ((numhistory2 - fullpagenum * DOLLYNUMFORDISP) > 0) {
		m_pagenum++;
	}
	m_currentpage = 0;

	m_startno = m_currentpage * DOLLYNUMFORDISP;//!!!!!!!!!!!!!!!!!!!
	int restnum = numhistory2 - m_startno;
	m_namenum = min(restnum, DOLLYNUMFORDISP);//!!!!!!!!!!!!!!!!!!!

	ParamsToDlg();//表示されていないときにはm_hWndがNULLの場合があるのでShowWindowの後でSetNamesを呼ぶ

	m_createdflag = true;

	return 0;


}



LRESULT CDollyHistoryDlg::OnDelete1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(0);
}
LRESULT CDollyHistoryDlg::OnDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(1);
}
LRESULT CDollyHistoryDlg::OnDelete3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(2);
}
LRESULT CDollyHistoryDlg::OnDelete4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(3);
}
LRESULT CDollyHistoryDlg::OnDelete5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(4);
}
LRESULT CDollyHistoryDlg::OnDelete6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(5);
}
LRESULT CDollyHistoryDlg::OnDelete7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(6);
}
LRESULT CDollyHistoryDlg::OnDelete8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(7);
}
LRESULT CDollyHistoryDlg::OnDelete9(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(8);
}
LRESULT CDollyHistoryDlg::OnDelete10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnDelete(9);
}

LRESULT CDollyHistoryDlg::OnDelete(size_t delid)
{
	if (delid >= m_namenum) {
		return 0;
	}

	WCHAR delcpt[MAX_PATH] = { 0L };
	wcscpy_s(delcpt, MAX_PATH, m_dollyhistory[m_startno + delid].wfilename);
	delcpt[MAX_PATH - 1] = 0L;
	if (delcpt[0] != 0L) {
		BOOL bexist;
		bexist = PathFileExists(delcpt);
		if (bexist) {
			DeleteFileW(delcpt);
		}
	}

	wcscpy_s(m_dollyhistory[m_startno + delid].wfilename, MAX_PATH, L"deleted.");
	m_dollyhistory[m_startno + delid].validflag = false;


	//#######################
	//削除中は　pageそのまま
	//#######################
	//{
	//	int numhistory2 = (int)m_copyhistory.size();
	//	int fullpagenum = numhistory2 / COPYNUMFORDISP;//満たされているページの数
	//	m_pagenum = fullpagenum;//端数込みのページ数
	//	if ((numhistory2 - fullpagenum * COPYNUMFORDISP) > 0) {
	//		m_pagenum++;
	//	}

	//	m_currentpage = 0;

	//	m_startno = m_currentpage * COPYNUMFORDISP;//!!!!!!!!!!!!!!!!!!!
	//	int restnum = numhistory2 - m_startno;
	//	m_namenum = min(restnum, COPYNUMFORDISP);//!!!!!!!!!!!!!!!!!!!
	//}

	ParamsToDlg();

	return 0;
}


LRESULT CDollyHistoryDlg::OnRadio1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(0);
}
LRESULT CDollyHistoryDlg::OnRadio2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(1);
}
LRESULT CDollyHistoryDlg::OnRadio3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(2);
}
LRESULT CDollyHistoryDlg::OnRadio4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(3);
}
LRESULT CDollyHistoryDlg::OnRadio5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(4);
}
LRESULT CDollyHistoryDlg::OnRadio6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(5);
}
LRESULT CDollyHistoryDlg::OnRadio7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(6);
}
LRESULT CDollyHistoryDlg::OnRadio8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(7);
}
LRESULT CDollyHistoryDlg::OnRadio9(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(8);
}
LRESULT CDollyHistoryDlg::OnRadio10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return OnRadio(9);
}


int CDollyHistoryDlg::SetDollyElem2Camera(DOLLYELEM srcelem)
{
	if (!m_UpdateFunc) {
		_ASSERT(0);
		return 1;
	}

	if (srcelem.validflag) {
		m_camerapos = srcelem.camerapos;
		m_targetpos = srcelem.cameratarget;
		wcscpy_s(m_comment, HISTORYCOMMENTLEN, srcelem.comment);

		EditParamsToDlg();

		ChaVector3 savepos = g_camEye;
		ChaVector3 savetarget = g_camtargetpos;
		float savedist = g_camdist;

		g_camEye = srcelem.camerapos;
		g_camtargetpos = srcelem.cameratarget;

		int result;
		if (m_UpdateFunc != nullptr) {
			result = (this->m_UpdateFunc)();//!!!!!!!!!! Main.cpp : int UpdateCameraPosAndTarget()
			if (result == 0) {
				//正常
				return 0;
			}
			else {
				//エラーのため　巻き戻し
				g_camEye = savepos;
				g_camtargetpos = savetarget;
				g_camdist = savedist;

				::MessageBox(m_hWnd, L"パラメータ不正のため適用できませんでした。", L"入力エラー", MB_OK);
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

LRESULT CDollyHistoryDlg::OnRadio(size_t radioid)
{

	if (radioid >= m_namenum) {
		return 0;
	}

	//OKボタンを押さないでも反映されるように
	int result = 0;
	if (!m_dollyhistory.empty()) {
		DOLLYELEM curelem = m_dollyhistory[m_startno + radioid];
		result = SetDollyElem2Camera(curelem);
	}


	return result;
}

LRESULT CDollyHistoryDlg::OnPrevPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	{
		int numhistory2 = (int)m_dollyhistory.size();
		int fullpagenum = numhistory2 / DOLLYNUMFORDISP;//満たされているページの数
		m_pagenum = fullpagenum;//端数込みのページ数
		if ((numhistory2 - fullpagenum * DOLLYNUMFORDISP) > 0) {
			m_pagenum++;
		}

		if (m_pagenum >= 1) {
			if (m_currentpage >= 1) {
				m_currentpage--;
			}
			else {
				m_currentpage = m_pagenum - 1;
			}
		}
		else {
			m_currentpage = 0;
		}

		m_startno = m_currentpage * DOLLYNUMFORDISP;//!!!!!!!!!!!!!!!!!!!
		int restnum = numhistory2 - m_startno;
		m_namenum = min(restnum, DOLLYNUMFORDISP);//!!!!!!!!!!!!!!!!!!!
	}

	ParamsToDlg();

	return 0;

}
LRESULT CDollyHistoryDlg::OnNextPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	{
		int numhistory2 = (int)m_dollyhistory.size();
		int fullpagenum = numhistory2 / DOLLYNUMFORDISP;//満たされているページの数
		m_pagenum = fullpagenum;//端数込みのページ数
		if ((numhistory2 - fullpagenum * DOLLYNUMFORDISP) > 0) {
			m_pagenum++;
		}

		if (m_pagenum >= 1) {
			if (m_currentpage < (m_pagenum - 1)) {
				m_currentpage++;
			}
			else {
				m_currentpage = 0;
			}
		}
		else {
			m_currentpage = 0;
		}

		m_startno = m_currentpage * DOLLYNUMFORDISP;//!!!!!!!!!!!!!!!!!!!
		int restnum = numhistory2 - m_startno;
		m_namenum = min(restnum, DOLLYNUMFORDISP);//!!!!!!!!!!!!!!!!!!!
	}

	ParamsToDlg();

	return 0;

}

DOLLYELEM CDollyHistoryDlg::GetFirstValidElem()
{
	int elemnum = (int)m_dollyhistory.size();
	int elemno;
	for (elemno = 0; elemno < elemnum; elemno++) {
		DOLLYELEM helem = m_dollyhistory[elemno];
		if (helem.validflag) {
			return helem;
		}
		else {
			//有効な履歴が見つかるまでループを続ける
		}
	}

	//有効な履歴がみつからなかった場合
	DOLLYELEM inielem;
	inielem.Init();
	return inielem;
}

DOLLYELEM CDollyHistoryDlg::GetCheckedElem()
{
	size_t selectedno = 0;//チェックされていない場合(あり得ないが)、一番最初
	size_t nameno;
	for (nameno = 0; nameno < m_namenum; nameno++) {
		if (m_dlg_wnd.IsDlgButtonChecked(m_ctrlid[nameno])) {
			selectedno = nameno;
			break;
		}
	}

	if ((selectedno >= 0) && (selectedno < m_namenum) &&
		(m_startno + selectedno >= 0) && (m_startno + selectedno < m_dollyhistory.size())) {
		DOLLYELEM checkedelem = m_dollyhistory[m_startno + selectedno];
		if (checkedelem.validflag) {
			return checkedelem;
		}
	}

	//有効な履歴がみつからなかった場合
	DOLLYELEM inielem;
	inielem.Init();
	return inielem;

}


LRESULT CDollyHistoryDlg::OnGetDolly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_camerapos = g_camEye;
	m_targetpos = g_camtargetpos;
	ZeroMemory(m_comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);

	EditParamsToDlg();

	return 0;
}
LRESULT CDollyHistoryDlg::OnSetDolly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	WCHAR strpos[256] = { 0L };
	float posvalue = 0.0f;

	ChaVector3 savecameye = g_camEye;
	ChaVector3 savetarget = g_camtargetpos;
	float savedist = g_camdist;

	g_befcamtargetpos = g_camtargetpos;

	GetDlgItemTextW(IDC_POSITION_X, strpos, 256);
	posvalue = (float)_wtof(strpos);
	if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
		g_camEye.x = posvalue;
	}
	GetDlgItemTextW(IDC_POSITION_Y, strpos, 256);
	posvalue = (float)_wtof(strpos);
	if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
		g_camEye.y = posvalue;
	}
	GetDlgItemTextW(IDC_POSITION_Z, strpos, 256);
	posvalue = (float)_wtof(strpos);
	if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
		g_camEye.z = posvalue;
	}


	GetDlgItemTextW(IDC_TARGET_X, strpos, 256);
	posvalue = (float)_wtof(strpos);
	if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
		g_camtargetpos.x = posvalue;
	}
	GetDlgItemTextW(IDC_TARGET_Y, strpos, 256);
	posvalue = (float)_wtof(strpos);
	if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
		g_camtargetpos.y = posvalue;
	}
	GetDlgItemTextW(IDC_TARGET_Z, strpos, 256);
	posvalue = (float)_wtof(strpos);
	if ((posvalue >= -FLT_MAX) && (posvalue <= FLT_MAX)) {
		g_camtargetpos.z = posvalue;
	}



	strpos[0] = 0L;
	GetDlgItemTextW(IDC_COMMENT, strpos, 256);
	size_t commentlen = wcslen(strpos);
	strpos[HISTORYCOMMENTLEN - 1] = 0L;
	wcscpy_s(m_comment, HISTORYCOMMENTLEN, strpos);


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
				return 0;
			}
			else {
				//エラーのため　巻き戻し
				g_camEye = savecameye;
				g_camtargetpos = savetarget;
				g_camdist = savedist;

				::MessageBox(m_hWnd, L"パラメータ不正のため適用できませんでした。", L"入力エラー", MB_OK);
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



LRESULT CDollyHistoryDlg::OnSaveDolly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL bhandled = true;
	LRESULT lresult = OnSetDolly(0, 0, 0, bhandled);
	if (lresult == 0) {
		DOLLYELEM dollyelem;
		dollyelem.Init();


		dollyelem.camerapos = m_camerapos;
		dollyelem.cameratarget = m_targetpos;
		wcscpy_s(dollyelem.comment, HISTORYCOMMENTLEN, m_comment);
		dollyelem.validflag = true;


		WCHAR temppath[MAX_PATH] = { 0L };
		::GetTempPathW(MAX_PATH, temppath);
		if (temppath[0] != 0L) {
			SYSTEMTIME localtime;
			GetLocalTime(&localtime);
			WCHAR dollyfilepath[MAX_PATH] = { 0L };
			swprintf_s(dollyfilepath, MAX_PATH, L"%s\\MB3DOpenProjDolly_%04u%02u%02u%02u%02u%02u.dol",
				temppath,
				localtime.wYear, localtime.wMonth, localtime.wDay, localtime.wHour, localtime.wMinute, localtime.wSecond);
			HANDLE hfile;
			hfile = CreateFile(dollyfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
				FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hfile != INVALID_HANDLE_VALUE) {
				DWORD writelen = 0;
				WriteFile(hfile, &dollyelem, (DWORD)(sizeof(DOLLYELEM)), &writelen, NULL);
				_ASSERT((DWORD)sizeof(DOLLYELEM) == writelen);
				CloseHandle(hfile);

				vector<DOLLYELEM> updatehistory;
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


int CDollyHistoryDlg::LoadDollyHistory(std::vector<DOLLYELEM>& vecdolly)
{
	vecdolly.clear();

	WCHAR temppath[MAX_PATH] = { 0L };
	::GetTempPathW(MAX_PATH, temppath);
	if (temppath[0] != 0L) {
		WCHAR searchfilename[MAX_PATH] = { 0L };
		searchfilename[0] = { 0L };
		swprintf_s(searchfilename, MAX_PATH, L"%s\\MB3DOpenProjDolly_*.dol", temppath);
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		hFind = FindFirstFileW(searchfilename, &win32fd);

		std::vector<DOLLYELEM> vechistory;//!!!!!!!!! tmpファイル名

		vechistory.clear();
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
						rleng = sizeof(DOLLYELEM);
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

						vechistory.push_back(curelem);
					}
					else {
						_ASSERT(0);
						continue;
					}
				}
			} while (FindNextFile(hFind, &win32fd));
			FindClose(hFind);
		}


		if (!vechistory.empty()) {

			std::sort(vechistory.begin(), vechistory.end());
			std::reverse(vechistory.begin(), vechistory.end());

			vecdolly = vechistory;
		}
		else {
			vecdolly.clear();
		}

		return 0;
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}
