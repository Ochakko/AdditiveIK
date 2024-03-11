#include "stdafx.h"
// FrameCopyDlg.cpp : CFrameCopyDlg のインプリメンテーション
#include "FrameCopyDlg.h"
#include <Model.h>
#include <Bone.h>
#include <GlobalVar.h>

#include "SetDlgPos.h"

#define DBGH
#include <dbg.h>

using namespace std;

//extern
extern void OnDSUpdate();
//extern void OnDSMouseHereApeal();
//extern LONG g_undertrackingRMenu;
//extern LONG g_underApealingMouseHere;


static void ChangeCurDirFromMameMediaToTest();

void ChangeCurDirFromMameMediaToTest()
{
	//CurrentDirectoryがMameMediaになっていたときにはTestディレクトリに変える
	WCHAR curdir[MAX_PATH] = { 0L };
	ZeroMemory(curdir, sizeof(WCHAR) * MAX_PATH);
	GetCurrentDirectory(MAX_PATH, curdir);
	WCHAR* findpat = wcsstr(curdir, L"\\MameMedia");
	if (findpat) {
		WCHAR initialdir[MAX_PATH] = { 0L };
		wcscpy_s(initialdir, MAX_PATH, g_basedir);
		wcscat_s(initialdir, MAX_PATH, L"..\\Test\\");
		SetCurrentDirectoryW(initialdir);
	}
}



/////////////////////////////////////////////////////////////////////////////
// CFrameCopyDlg

CFrameCopyDlg::CFrameCopyDlg()
{
	m_samemodelflag = 0;
	InitParams();
}

int CFrameCopyDlg::InitParams()
{
	m_inittimerflag = false;
	m_timerid = 339;
	m_tboloadedflag = false;

	ZeroMemory(m_tmpmqopath, sizeof(WCHAR) * MAX_PATH);

	m_hImageList = 0;
	m_iImage = 0;
	m_iSelect = 0;

	m_timap.clear();
	m_selecteditem = 0;
	m_selectedno = 0;

	if( m_samemodelflag == 0 ){
		m_model = 0;
		m_slotno = 0;

		int slotno;
		for( slotno = 0; slotno < FCSLOTNUM2; slotno++ ){
			swprintf_s( &(m_slotname[slotno][0]), SLOTNAMELEN, L"Slot %d", slotno );
		}

		::ZeroMemory( m_influencenum, sizeof( int ) * FCSLOTNUM2 );
		::ZeroMemory(m_ignorenum, sizeof(int) * FCSLOTNUM2);


		//２次元を１回で初期化すると警告が出る
		//::ZeroMemory(&(m_influencelist[0][0]), sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG);
		//::ZeroMemory(&(m_ignorelist[0][0]), sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG);

		int fcslotno;
		for (fcslotno = 0; fcslotno < FCSLOTNUM2; fcslotno++) {
			::ZeroMemory(&(m_influencelist[fcslotno][0]), sizeof(int) * FRAMECOPYLISTLENG);
			::ZeroMemory(&(m_ignorelist[fcslotno][0]), sizeof(int) * FRAMECOPYLISTLENG);
		}
	}

	m_validelemmap.clear();
	m_invalidelemmap.clear();
	m_cpvec.clear();

	m_samemodelflag = 0;

	ZeroMemory(&m_hrootti, sizeof(HTREEITEM));

	return 0;
}

int CFrameCopyDlg::SetModel( CModel* srcmodel )
{
	if( m_model == srcmodel ){
		m_samemodelflag = 1;
	}else{
		m_samemodelflag = 0;
	}
	m_model = srcmodel;
	return 0;
}


CFrameCopyDlg::~CFrameCopyDlg()
{
	DestroyObjs();
}
	
int CFrameCopyDlg::DestroyObjs()
{
	//KillTimer(m_timerid);

	DestroyDlgCtrls();

	m_validelemmap.clear();
	m_invalidelemmap.clear();
	m_cpvec.clear();

	return 0;
}

int CFrameCopyDlg::DestroyDlgCtrls()
{
	if (m_hImageList) {
		ImageList_Destroy(m_hImageList);
	}
	m_timap.clear();

	return 0;
}

int CFrameCopyDlg::SetupDlg( CModel* srcmodel )
{
	int ret;

	//tboloadedflag == trueの場合には　ウインドウコントロール以外は設定済
	if (m_tboloadedflag == false) {
		DestroyObjs();
		InitParams();
	}
	else {
		DestroyDlgCtrls();
	}

	int saveslotno = m_slotno;

	m_model = srcmodel;

	m_dlg_wnd = m_hWnd;
	m_tree_wnd = GetDlgItem(IDN_TREE1);
	_ASSERT(m_tree_wnd);
	m_list_wnd = GetDlgItem(IDN_LIST1);
	_ASSERT(m_list_wnd);
	m_list2_wnd = GetDlgItem(IDN_LIST2);
	_ASSERT(m_list2_wnd);
	m_combo_wnd = GetDlgItem(IDC_SLOTCOMBO);
	_ASSERT(m_combo_wnd);
	m_slotname_wnd = GetDlgItem(IDC_SLOTNAME);
	_ASSERT(m_slotname_wnd);

	CreateImageList();

	//SetTimer(m_timerid, 20);

	if (m_model->GetTopBone()) {
		ret = FillTree();
		if (ret) {
			_ASSERT(0);
			return 1;
		}

		CreateCombo();


		m_slotno = saveslotno;
		ret = ParamsToDlg();
		_ASSERT(!ret);
	}

	return 0;
}

LRESULT CFrameCopyDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//InitCommonControls();

	SetDlgPosDesktopCenter(m_hWnd, HWND_TOPMOST);
	RECT dlgrect;
	::GetWindowRect(m_hWnd, &dlgrect);
	SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);


	SetupDlg( m_model );
	StartTimer();
	return 1;  // システムにフォーカスを設定させます
}


int CFrameCopyDlg::ExecuteOnOK()
{

	m_validelemmap.clear();
	m_invalidelemmap.clear();
	m_cpvec.clear();

	int i, validno;
	for (i = 0; i < m_influencenum[m_slotno]; i++) {
		validno = m_influencelist[m_slotno][i];
		SetTree2ListReq(0, validno, 0);
	}

	int j, invalidno;
	for (j = 0; j < m_ignorenum[m_slotno]; j++) {
		invalidno = m_ignorelist[m_slotno][j];
		SetTree2ListReq(1, invalidno, 0);
	}

	map<int, CBone*>::iterator itrbone;
	for (itrbone = m_model->GetBoneListBegin(); itrbone != m_model->GetBoneListEnd(); itrbone++) {
		int chkboneno = itrbone->first;
		CBone* chkbone = itrbone->second;
		if (chkbone && chkbone->IsSkeleton() && (chkbone->GetBoneNo() != 0)) {//2023/09/28 exlucde RootNode (boneno == 0)
			CBone* valbone = m_validelemmap[chkboneno];
			CBone* invalbone = m_invalidelemmap[chkboneno];

			if (valbone && !invalbone) {
				m_cpvec.push_back(chkbone);
			}
		}
	}



	UINT chkenable1 = IsDlgButtonChecked(IDC_CHECK_PASTESCALE);
	if (chkenable1 == BST_CHECKED) {
		g_pasteScale = true;
	}
	else {
		g_pasteScale = false;
	}
	UINT chkenable2 = IsDlgButtonChecked(IDC_CHECK_PASTEROTATION);
	if (chkenable2 == BST_CHECKED) {
		g_pasteRotation = true;
	}
	else {
		g_pasteRotation = false;
	}
	UINT chkenable3 = IsDlgButtonChecked(IDC_CHECK_PASTETRANSLATION);
	if (chkenable3 == BST_CHECKED) {
		g_pasteTranslation = true;
	}
	else {
		g_pasteTranslation = false;
	}



	return 0;
}


LRESULT CFrameCopyDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	//外部クラスから、wNotifyCode = 999 で呼び出されることがある。（ダイアログは表示されていない状態）
	
	EndTimer();

	GetDlgItemTextW( IDC_SLOTNAME, &(m_slotname[m_slotno][0]), SLOTNAMELEN );


	ExecuteOnOK();


	EndDialog(wID);

	return 0;
}

LRESULT CFrameCopyDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndTimer();

	EndDialog(wID);
	return 0;
}
LRESULT CFrameCopyDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndTimer();

	EndDialog(IDCANCEL);
	return 0;
}



int CFrameCopyDlg::FillTree()
{
	//m_selecteditem = hRoot;

	//int shdnum;
	//shdnum = m_model->GetBoneListSize();//eNull含む

	m_timap.clear();

	m_selectedno = 0;
	m_selecteditem = 0;
	m_hrootti = TreeView_GetRoot(m_tree_wnd);
	AddBoneToTree( m_model->GetTopBone(false), 1, 1 );


	map<int, HTREEITEM>::iterator itrtree;
	for( itrtree = m_timap.begin(); itrtree != m_timap.end(); itrtree++ ){
		HTREEITEM* curti = &(itrtree->second);
		TreeView_Expand( m_tree_wnd, *curti, TVE_EXPAND );
	}


	//TreeView_SelectSetFirstVisible( m_tree_wnd, m_selecteditem );
	TreeView_SelectSetFirstVisible(m_tree_wnd, TreeView_GetRoot(m_tree_wnd));


	return 0;
}
void CFrameCopyDlg::AddBoneToTree( CBone* srcbone, int addbroflag, int addtolast )
{
	//addtolastは初回のTVAddのみを制御する。
	//( CShdElemのTree構造と同期するため。)

	HTREEITEM parTI;
	CBone* parentbone = srcbone->GetParent(false);
	if( parentbone && parentbone->IsSkeleton()){
		parTI = m_timap[ parentbone->GetBoneNo() ];
	}else{
		//parTI = TreeView_GetRoot( m_tree_wnd );//１つTIを作ると　以降　それがRootとして取得されてしまう
		parTI = m_hrootti;
	}

	if (srcbone->IsSkeleton()) {
		HTREEITEM newTI;
		newTI = TVAdd(parTI, (WCHAR*)srcbone->GetWBoneName(), srcbone->GetBoneNo(), m_iImage, m_iSelect, addtolast);
		if (!newTI) {
			_ASSERT(0);
			return;
		}
		m_timap[srcbone->GetBoneNo()] = newTI;
		if (m_selectedno == 0) {
			m_selecteditem = newTI;
			m_selectedno = srcbone->GetBoneNo();
			TreeView_SelectItem(m_tree_wnd, m_selecteditem);
		}
	}


///////////
	if( srcbone->GetChild(false) ){
		AddBoneToTree( srcbone->GetChild(false), 1, 1 );
	}

	///////////
	if( addbroflag ){
		if( srcbone->GetBrother(false) ){
			AddBoneToTree( srcbone->GetBrother(false), 1, 1 );
		}
	}

}
HTREEITEM CFrameCopyDlg::TVAdd( HTREEITEM parentTI, WCHAR* srcname, int srcno, int imageno, int selectno, int addtolast )
{
	//ここのDbg文は取り除かない。
//	DbgOut( "FrameCopyDlg : TVAdd : srcname %s, srcno %d, imageno %d, selectno %d, addtolast %d\n",
//		srcname, srcno, imageno, selectno, addtolast );

	TVITEM tvi;
	tvi.mask = TVIF_TEXT;
	tvi.pszText = srcname;

	tvi.mask |= TVIF_PARAM;
	tvi.lParam = (LPARAM)srcno;

	if( imageno != -1 ){
		tvi.mask |= TVIF_IMAGE;
		tvi.iImage = imageno;
	}

	if( selectno != -1 ){
		tvi.mask |= TVIF_SELECTEDIMAGE;
		tvi.iSelectedImage = selectno;
	}

	TVINSERTSTRUCT tvins;
	tvins.item = tvi;

	if( addtolast )
		tvins.hInsertAfter = TVI_LAST;
	else
		tvins.hInsertAfter = TVI_FIRST;

	tvins.hParent = parentTI;

	return TreeView_InsertItem( m_tree_wnd, &tvins );

}

void CFrameCopyDlg::CreateImageList()
{
	m_tree_wnd.SetWindowLong(GWL_STYLE,
		WS_CHILD | WS_VISIBLE | WS_BORDER |
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS
	);

	HBITMAP hBitmap;
	m_hImageList = ImageList_Create(16, 16, ILC_COLOR, 2, 10);

	hBitmap = LoadBitmap((HINSTANCE)GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BONE_IMAGE));
	if (hBitmap != NULL) {
		m_iImage = ImageList_Add(m_hImageList, hBitmap, (HBITMAP)0);
		if (m_iImage != -1) {
			DeleteObject(hBitmap);

			HBITMAP hBitmap2;
			hBitmap2 = LoadBitmap((HINSTANCE)GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDB_BONE_SELECT));
			if (hBitmap2 != NULL) {
				m_iSelect = ImageList_Add(m_hImageList, hBitmap2, (HBITMAP)0);
				if (m_iSelect != -1) {
					DeleteObject(hBitmap2);

					HIMAGELIST retIL;
					retIL = TreeView_SetImageList(m_tree_wnd,
						m_hImageList, TVSIL_NORMAL);
				}
				else {
					_ASSERT(0);
				}
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}
	}
	else {
		_ASSERT(0);
	}

}


int CFrameCopyDlg::ParamsToDlg()
{

	m_combo_wnd.SendMessage(CB_SETCURSEL, (WPARAM)m_slotno, 0);

	m_slotname_wnd.SetWindowTextW( &(m_slotname[ m_slotno ][0]) );

	m_list_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );
	
	int listno;
	for( listno = 0; listno < m_influencenum[m_slotno]; listno++ ){
		LRESULT lres;

		const WCHAR* bonename = m_model->GetBoneByID( m_influencelist[m_slotno][listno] )->GetWBoneName();
		WCHAR addstr[2048];
		ZeroMemory( addstr, sizeof( WCHAR ) * 2048 );
		swprintf_s( addstr, 2048, L"%d : %s", m_influencelist[m_slotno][listno], bonename );

		lres = m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)addstr );
		if( (lres == LB_ERR) || (lres == LB_ERRSPACE) ){
			_ASSERT( 0 );
			return 1;
		}
	}

	///////////////
	
	m_list2_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );
	
	int listno2;
	for( listno2 = 0; listno2 < m_ignorenum[m_slotno]; listno2++ ){
		LRESULT lres;

		const WCHAR* bonename = m_model->GetBoneByID( m_ignorelist[m_slotno][listno2] )->GetWBoneName();
		WCHAR addstr[2048];
		ZeroMemory( addstr, sizeof( WCHAR ) * 2048 );
		//swprintf_s( addstr, 2048, L"%d : %s", m_influencelist[m_slotno][listno2], bonename );
		swprintf_s(addstr, 2048, L"%d : %s", m_ignorelist[m_slotno][listno2], bonename);

		lres = m_list2_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)addstr );
		if( (lres == LB_ERR) || (lres == LB_ERRSPACE) ){
			_ASSERT( 0 );
			return 1;
		}
	}


	if (g_pasteScale == true) {
		CheckDlgButton(IDC_CHECK_PASTESCALE, true);
	}
	else {
		CheckDlgButton(IDC_CHECK_PASTESCALE, false);
	}
	if (g_pasteRotation == true) {
		CheckDlgButton(IDC_CHECK_PASTEROTATION, true);
	}
	else {
		CheckDlgButton(IDC_CHECK_PASTEROTATION, false);
	}
	if (g_pasteTranslation == true) {
		CheckDlgButton(IDC_CHECK_PASTETRANSLATION, true);
	}
	else {
		CheckDlgButton(IDC_CHECK_PASTETRANSLATION, false);
	}



	return 0;
}

LRESULT CFrameCopyDlg::OnSelchangedTree1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	// TODO : ｺﾝﾄﾛｰﾙの通知ﾊﾝﾄﾞﾗ用のｺｰﾄﾞを追加してください。
	NMTREEVIEW* pnmtv = (NMTREEVIEW*)pnmh;
	TVITEM tvi = pnmtv->itemNew;

	m_selecteditem = tvi.hItem;

	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	if( !TreeView_GetItem( m_tree_wnd, &tvi ) )
		return 0;

	m_selectedno = (int)(tvi.lParam);

	return 0;
}

LRESULT CFrameCopyDlg::OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( m_selectedno == 0 )
		return 0;

	if( m_influencenum[m_slotno] >= FRAMECOPYLISTLENG ){
		MessageBox( L"これ以上、指定できません。", L"バッファ不足エラー", MB_OK );
		return 0;
	}

	int listno;
	int findflag = 0;
	for( listno = 0; listno < m_influencenum[m_slotno]; listno++ ){
		if( m_influencelist[m_slotno][listno] == m_selectedno ){
			findflag = 1;
			break;
		}
	}
	if( findflag == 1 )
		return 0;

	m_influencelist[m_slotno][m_influencenum[m_slotno]] = m_selectedno;
	(m_influencenum[m_slotno])++;

	LRESULT lres;

	const WCHAR* bonename = m_model->GetBoneByID( m_selectedno )->GetWBoneName();
	WCHAR addstr[2048];
	ZeroMemory( addstr, sizeof( WCHAR ) * 2048 );
	swprintf_s( addstr, 2048, L"%d : %s", m_selectedno, bonename );

	lres = m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)addstr );
	if( lres == LB_ERR ){
		_ASSERT( 0 );
		return -1;
	}

	return 0;
}

LRESULT CFrameCopyDlg::OnAdd2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( m_selectedno == 0 )
		return 0;

	if( m_ignorenum[m_slotno] >= FRAMECOPYLISTLENG ){
		MessageBox( L"これ以上、指定できません。", L"バッファ不足エラー", MB_OK );
		return 0;
	}

	int listno;
	int findflag = 0;
	for( listno = 0; listno < m_ignorenum[m_slotno]; listno++ ){
		if( m_ignorelist[m_slotno][listno] == m_selectedno ){
			findflag = 1;
			break;
		}
	}
	if( findflag == 1 )
		return 0;

	m_ignorelist[m_slotno][m_ignorenum[m_slotno]] = m_selectedno;
	(m_ignorenum[m_slotno])++;

	int ret;

	const WCHAR* bonename = m_model->GetBoneByID( m_selectedno )->GetWBoneName();
	WCHAR addstr[2048];
	ZeroMemory( addstr, sizeof( WCHAR ) * 2048 );
	swprintf_s( addstr, 2048, L"%d : %s", m_selectedno, bonename );

	ret = (int)m_list2_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)addstr );
	if( ret == LB_ERR ){
		_ASSERT( 0 );
		return -1;
	}

	return 0;
}

LRESULT CFrameCopyDlg::OnDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int selindex;
	selindex = (int)m_list_wnd.SendMessage( LB_GETCURSEL, 0, 0 );
	if( selindex == LB_ERR ){
		//_ASSERT( 0 );
		//return -1;
		
		//何も選択していないときもLB_ERRが返る
		return 0;
	}
	if( (selindex < 0) || (selindex >= m_influencenum[m_slotno]) ){
		_ASSERT( 0 );
		return -1;
	}

	int ret;
	ret = (int)m_list_wnd.SendMessage( LB_DELETESTRING, (WPARAM)selindex, 0 );
	if( ret == LB_ERR ){
		_ASSERT( 0 );
		return 0;
	}

	int listno;
	for( listno = selindex; listno < (m_influencenum[m_slotno] - 1); listno++ ){
		m_influencelist[m_slotno][listno] = m_influencelist[m_slotno][listno + 1];
	}
	(m_influencenum[m_slotno])--;


	return 0;
}

LRESULT CFrameCopyDlg::OnDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int selindex;
	selindex = (int)m_list2_wnd.SendMessage( LB_GETCURSEL, 0, 0 );
	if( selindex == LB_ERR ){
		//_ASSERT( 0 );
		//return -1;
		
		//何も選択していないときもLB_ERRが返る
		return 0;
	}
	if( (selindex < 0) || (selindex >= m_ignorenum[m_slotno]) ){
		_ASSERT( 0 );
		return -1;
	}

	int ret;
	ret = (int)m_list2_wnd.SendMessage( LB_DELETESTRING, (WPARAM)selindex, 0 );
	if( ret == LB_ERR ){
		_ASSERT( 0 );
		return 0;
	}

	int listno;
	for( listno = selindex; listno < (m_ignorenum[m_slotno] - 1); listno++ ){
		m_ignorelist[m_slotno][listno] = m_ignorelist[m_slotno][listno + 1];
	}
	(m_ignorenum[m_slotno])--;


	return 0;
}

LRESULT CFrameCopyDlg::OnAllDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_list_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );
	m_influencenum[m_slotno] = 0;
	////ZeroMemory( &(m_influencelist[0][0]), sizeof( int ) * FCSLOTNUM2 * FRAMECOPYLISTLENG );
	//int fcslotno;
	//for (fcslotno = 0; fcslotno < FCSLOTNUM2; fcslotno++) {
	//	::ZeroMemory(&(m_influencelist[fcslotno][0]), sizeof(int) * FRAMECOPYLISTLENG);
	//}


	//カレントスロットだけ
	::ZeroMemory(&(m_influencelist[m_slotno][0]), sizeof(int) * FRAMECOPYLISTLENG);


	return 0;
}

LRESULT CFrameCopyDlg::OnAllDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_list2_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );
	m_ignorenum[m_slotno] = 0;
	////ZeroMemory( &(m_ignorelist[0][0]), sizeof( int ) * FCSLOTNUM2 * FRAMECOPYLISTLENG );
	//int fcslotno;
	//for (fcslotno = 0; fcslotno < FCSLOTNUM2; fcslotno++) {
	//	::ZeroMemory(&(m_ignorelist[fcslotno][0]), sizeof(int) * FRAMECOPYLISTLENG);
	//}


	//カレントスロットだけ
	::ZeroMemory(&(m_ignorelist[m_slotno][0]), sizeof(int) * FRAMECOPYLISTLENG);


	return 0;
}

int CFrameCopyDlg::SetTree2ListReq( int validorinvalid, int srcno, int addbroflag )
{

	CBone* curbone = 0;
	int bonenum = m_model->GetBoneListSize();//eNull含む

	if( (srcno >= 0) && (srcno < bonenum) ){
		curbone = m_model->GetBoneByID( srcno );
		if (curbone && curbone->IsSkeleton()) {
			if( validorinvalid == 0 ){
				m_validelemmap[ srcno ] = curbone;//[boneno]
			}else{
				m_invalidelemmap[ srcno ] = curbone;//[boneno]
			}
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( curbone );

	if( curbone->GetChild(false) ){
		SetTree2ListReq( validorinvalid, curbone->GetChild(false)->GetBoneNo(), 1 );
	}

	if( addbroflag == 1 ){
		if( curbone->GetBrother(false) ){
			SetTree2ListReq( validorinvalid, curbone->GetBrother(false)->GetBoneNo(), 1 );
		}
	}

	return 0;
}

int CFrameCopyDlg::CreateCombo()
{
	m_combo_wnd.SendMessage( CB_RESETCONTENT, 0, 0 );

	int selno;
	for( selno = 0; selno < FCSLOTNUM2; selno++ ){
		m_combo_wnd.SendMessage( CB_ADDSTRING, 0, (LPARAM)&(m_slotname[selno]) );
	}
	m_slotno = 0;
	m_combo_wnd.SendMessage( CB_SETCURSEL, 0, 0 );

	return 0;
}


LRESULT CFrameCopyDlg::OnSelCombo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	GetDlgItemTextW( IDC_SLOTNAME, &(m_slotname[m_slotno][0]), SLOTNAMELEN );

	int combono;
	combono = (int)m_combo_wnd.SendMessage( CB_GETCURSEL, 0, 0 );
	if( combono == CB_ERR )
		return 0;

	CreateCombo();

	m_slotno = combono;
	m_combo_wnd.SendMessage( CB_SETCURSEL, m_slotno, 0 );

	ParamsToDlg();

	return 0;
}

LRESULT CFrameCopyDlg::OnTimer(UINT, WPARAM, LPARAM, BOOL&)
{
	OnDSUpdate();
	return TRUE;
}


LRESULT CFrameCopyDlg::OnLoad(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LoadTBOFile();
	return 0;
}
LRESULT CFrameCopyDlg::OnSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	WriteTBOFile();
	return 0;
}


int CFrameCopyDlg::WriteTBOFile()
{
	if (!m_model) {
		return 0;
	}
	if (!m_model->GetCurMotInfo()) {
		return 0;
	}

	ChangeCurDirFromMameMediaToTest();

	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	//ofn.hwndOwner = hDlgWnd;
	ofn.hwndOwner = m_dlg_wnd;
	ofn.hInstance = 0;
	ofn.lpstrFilter = L"TargetBone(*.tbo)\0*.tbo\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = m_tmpmqopath;
	ofn.nMaxFile = MULTIPATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"GetFileNameDlg";
	//ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.Flags = OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_ENABLESIZING | OFN_ALLOWMULTISELECT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = NULL;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	//s_getfilenamehwnd = 0;
	//s_getfilenametreeview = 0;
	//HWINEVENTHOOK hhook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 0,
	//	WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	//InterlockedExchange(&g_undertrackingRMenu, 1);

	int result = 0;

	if (GetOpenFileNameW(&ofn) == IDOK) {
		result = WriteTBOFile(m_tmpmqopath);
	}

	//InterlockedExchange(&g_undertrackingRMenu, 0);
	//UnhookWinEvent(hhook);
	//s_getfilenamehwnd = 0;
	//s_getfilenametreeview = 0;

	return result;
}


int CFrameCopyDlg::ValidateTBOFile(char* dstTBOheader, char* srcbuf, DWORD bufleng)
{
	if (!dstTBOheader || !srcbuf || (bufleng <= 0)) {
		_ASSERT(0);
		return 0;
	}

	if (bufleng <= (sizeof(char) * 256)) {
		_ASSERT(0);
		return 0;
	}

	MoveMemory(dstTBOheader, srcbuf, sizeof(char) * 256);

	//typedef struct tag_CPTheader
	//{
	//	char magicstr[32];//EvaluateGlobalPosition
	//	char version[16];
	//	char fbxdate[256];
	//	int animno;
	//	int jointnum;
	//	int framenum;
	//	int reserved;
	//}CPTHEADER;

	size_t magicstrlen;
	magicstrlen = strlen(dstTBOheader);
	if ((magicstrlen <= 0) || (magicstrlen >= 256)) {
		_ASSERT(0);
		return 0;
	}
	int cmp7;
	int cmp8;
	int cmp9;
	int cmp10;
	int cmp11;
	int cmp12;
	cmp7 = strcmp(dstTBOheader, "MB3DTargetBoneFile ver1.0.0.7");//本体ではない
	cmp8 = strcmp(dstTBOheader, "MB3DTargetBoneFile ver1.0.0.8");//本体ではない
	cmp9 = strcmp(dstTBOheader, "MB3DTargetBoneFile ver1.0.0.9");//本体ではない
	cmp10 = strcmp(dstTBOheader, "MB3DTargetBoneFile ver1.0.0.10");//本体ではない 2023/08/21 To12024  FCSLOTNUM-->FCSLOTNUM2
	cmp11 = strcmp(dstTBOheader, "MB3DTargetBoneFile ver1.0.0.11");//本体ではない 2023/09/11 To12025  ボーン情報を名前ベースに変更
	cmp12 = strcmp(dstTBOheader, "MB3DTargetBoneFile ver1.0.0.12");//本体ではない 2023/09/25 To12025 RC4  設定されている分だけ書き出し

	if (cmp12 == 0) {
		DWORD datasize;
		datasize = (bufleng - sizeof(char) * 256);

		DWORD chksize1;
		chksize1 = (sizeof(char) * 256) +
			(sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(int) * FCSLOTNUM2);
		if (bufleng < chksize1) {
			_ASSERT(0);
			return 0;
		}


		DWORD totalinfluencenum = 0;
		DWORD pcurrent = (sizeof(char) * 256) + 
			(sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN);
		int slotno;
		for (slotno = 0; slotno < FCSLOTNUM2; slotno++) {
			if (pcurrent >= bufleng) {
				_ASSERT(0);
				return 0;
			}

			DWORD addnum = *((int*)(srcbuf + pcurrent));
			if (addnum > FRAMECOPYLISTLENG) {
				_ASSERT(0);
				return 0;
			}

			totalinfluencenum += addnum;
			pcurrent += sizeof(int);
		}
		if (totalinfluencenum > (FCSLOTNUM2 * FRAMECOPYLISTLENG)) {
			_ASSERT(0);
			return 0;
		}

		DWORD totalignorenum = 0;
		DWORD pcurrent2 = (sizeof(char) * 256) + 
			(sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(WCHAR) * MAX_PATH * totalinfluencenum);
		int slotno2;
		for (slotno2 = 0; slotno2 < FCSLOTNUM2; slotno2++) {
			if (pcurrent2 >= bufleng) {
				_ASSERT(0);
				return 0;
			}

			DWORD addnum2 = *((int*)(srcbuf + pcurrent2));
			if (addnum2 > FRAMECOPYLISTLENG) {
				_ASSERT(0);
				return 0;
			}

			totalignorenum += addnum2;
			pcurrent2 += sizeof(int);
		}
		if (totalignorenum > (FCSLOTNUM2 * FRAMECOPYLISTLENG)) {
			_ASSERT(0);
			return 0;
		}


		DWORD chkdatasize = (sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(WCHAR) * MAX_PATH * totalinfluencenum) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(WCHAR) * MAX_PATH * totalignorenum);

		if (datasize == chkdatasize) {
			return 4;//MB3DTargetBoneFile ver1.0.0.12
		}
		else {
			_ASSERT(0);
			return 0;
		}


	}
	else {
		if ((cmp7 != 0) && (cmp8 != 0) && (cmp9 != 0) && (cmp10 != 0) && (cmp11 != 0)) {
			_ASSERT(0);
			return 0;
		}

		DWORD datasize;
		datasize = (bufleng - sizeof(char) * 256);

		DWORD chksize1, chksize2, chksize3;
		chksize1 = (sizeof(WCHAR) * FCSLOTNUM * SLOTNAMELEN) +
			(sizeof(int) * FCSLOTNUM) +
			(sizeof(int) * FCSLOTNUM * FRAMECOPYLISTLENG) +
			(sizeof(int) * FCSLOTNUM) +
			(sizeof(int) * FCSLOTNUM * FRAMECOPYLISTLENG);

		chksize2 = (sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG);

		chksize3 = (sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(WCHAR) * MAX_PATH * FCSLOTNUM2 * FRAMECOPYLISTLENG) +
			(sizeof(int) * FCSLOTNUM2) +
			(sizeof(WCHAR) * MAX_PATH * FCSLOTNUM2 * FRAMECOPYLISTLENG);


		if (datasize == chksize1) {
			return 1;
		}
		else if (datasize == chksize2) {
			return 2;
		}
		else if (datasize == chksize3) {
			return 3;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}

	return 0;
}


bool CFrameCopyDlg::LoadTBOFile()
{
	if (!m_model) {
		_ASSERT(0);
		return false;
	}
	if (!m_model->GetCurMotInfo()) {
		_ASSERT(0);
		return false;
	}

	ChangeCurDirFromMameMediaToTest();

	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	//ofn.hwndOwner = hDlgWnd;
	ofn.hwndOwner = m_dlg_wnd;
	ofn.hInstance = 0;
	ofn.lpstrFilter = L"TargetBone(*.tbo)\0*.tbo\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = m_tmpmqopath;
	ofn.nMaxFile = MULTIPATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"GetFileNameDlg";
	//ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_ENABLESIZING | OFN_ALLOWMULTISELECT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = NULL;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	//s_getfilenamehwnd = 0;
	//s_getfilenametreeview = 0;
	//HWINEVENTHOOK hhook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 0,
	//	WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	//InterlockedExchange(&g_undertrackingRMenu, 1);

	bool result = true;

	if (GetOpenFileNameW(&ofn) == IDOK) {
		result = LoadTBOFile(m_tmpmqopath);
		if (result) {
			ParamsToDlg();
		}
	}

	//InterlockedExchange(&g_undertrackingRMenu, 0);
	//UnhookWinEvent(hhook);
	//s_getfilenamehwnd = 0;
	//s_getfilenametreeview = 0;

	return result;
}

int CFrameCopyDlg::WriteTBOFile(WCHAR* srcfilename)
{
	if (!m_model) {
		_ASSERT(0);
		return 1;
	}


	HANDLE hfile = CreateFile(srcfilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		DbgOut(L"TBOFile : WriteTBOFile : file open error !!!\n");
		_ASSERT(0);
		return 1;
	}

	char TBOheader[256];
	::ZeroMemory(TBOheader, sizeof(char) * 256);
	//strcpy_s(TBOheader, 256, "MB3DTargetBoneFile ver1.0.0.9");//本体ではない
	//strcpy_s(TBOheader, 256, "MB3DTargetBoneFile ver1.0.0.10");//本体ではない 2023/08/21
	//strcpy_s(TBOheader, 256, "MB3DTargetBoneFile ver1.0.0.11");//本体ではない 2023/09/11
	strcpy_s(TBOheader, 256, "MB3DTargetBoneFile ver1.0.0.12");//本体ではない 2023/09/25 for ver1.2.0.25 RC4
	DWORD wleng = 0;
	WriteFile(hfile, TBOheader, sizeof(char) * 256, &wleng, NULL);
	if (wleng != (sizeof(char) * 256)) {
		_ASSERT(0);
		return 1;
	}

	//WCHAR m_slotname[FCSLOTNUM][SLOTNAMELEN];
	wleng = 0;
	WriteFile(hfile, m_slotname, sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN, &wleng, NULL);
	if (wleng != (sizeof(WCHAR) * FCSLOTNUM2 * SLOTNAMELEN)) {
		_ASSERT(0);
		return 1;
	}

	//int m_influencenum[FCSLOTNUM2];
	wleng = 0;
	WriteFile(hfile, m_influencenum, sizeof(int) * FCSLOTNUM2, &wleng, NULL);
	if (wleng != (sizeof(int) * FCSLOTNUM2)) {
		_ASSERT(0);
		return 1;
	}


	////int m_influencelist[FCSLOTNUM2][FRAMECOPYLISTLENG];
	//wleng = 0;
	//WriteFile(hfile, m_influencelist, sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG, &wleng, NULL);
	//if (wleng != (sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	wleng = 0;
	int slotno;
	int influenceno;
	for (slotno = 0; slotno < FCSLOTNUM2; slotno++) {
		//for (influenceno = 0; influenceno < FRAMECOPYLISTLENG; influenceno++) {
		int influencenum = m_influencenum[slotno];
		for(influenceno = 0; influenceno < influencenum; influenceno++){//2023/09/25 セットされている分だけ書き出し MB3DTargetBoneFile ver1.0.0.12 RC4
			int writeboneno = m_influencelist[slotno][influenceno];
			if (writeboneno > 0) {
				CBone* writebone = m_model->GetBoneByID(writeboneno);
				const WCHAR* writebonename = writebone->GetWBoneName();
				WCHAR tmpbuf[MAX_PATH] = { 0L };
				wcscpy_s(tmpbuf, MAX_PATH, writebonename);
				WriteFile(hfile, tmpbuf, sizeof(WCHAR) * MAX_PATH, &wleng, NULL);
				if (wleng != (sizeof(WCHAR) * MAX_PATH)) {
					_ASSERT(0);
					return 1;
				}
			}
			else {
				WCHAR tmpbuf[MAX_PATH] = { 0L };
				wcscpy_s(tmpbuf, MAX_PATH, L"---unused---");
				WriteFile(hfile, tmpbuf, sizeof(WCHAR) * MAX_PATH, &wleng, NULL);
				if (wleng != (sizeof(WCHAR) * MAX_PATH)) {
					_ASSERT(0);
					return 1;
				}
			}
		}
	}




	//int m_ignorenum[FCSLOTNUM2];
	wleng = 0;
	WriteFile(hfile, m_ignorenum, sizeof(int) * FCSLOTNUM2, &wleng, NULL);
	if (wleng != (sizeof(int) * FCSLOTNUM2)) {
		_ASSERT(0);
		return 1;
	}
	////int m_ignorelist[FCSLOTNUM2][FRAMECOPYLISTLENG];
	//wleng = 0;
	//WriteFile(hfile, m_ignorelist, sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG, &wleng, NULL);
	//if (wleng != (sizeof(int) * FCSLOTNUM2 * FRAMECOPYLISTLENG)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	int ignoreno;
	for (slotno = 0; slotno < FCSLOTNUM2; slotno++) {
		//for (ignoreno = 0; ignoreno < FRAMECOPYLISTLENG; ignoreno++) {
		int ignorenum = m_ignorenum[slotno];
		for (ignoreno = 0; ignoreno < ignorenum; ignoreno++) {//2023/09/25 セットされている分だけ書き出し MB3DTargetBoneFile ver1.0.0.12 RC4
			int writeboneno = m_ignorelist[slotno][ignoreno];
			if (writeboneno > 0) {
				CBone* writebone = m_model->GetBoneByID(writeboneno);
				const WCHAR* writebonename = writebone->GetWBoneName();
				WCHAR tmpbuf[MAX_PATH] = { 0L };
				wcscpy_s(tmpbuf, MAX_PATH, writebonename);
				WriteFile(hfile, tmpbuf, sizeof(WCHAR) * MAX_PATH, &wleng, NULL);
				if (wleng != (sizeof(WCHAR) * MAX_PATH)) {
					_ASSERT(0);
					return 1;
				}
			}
			else {
				WCHAR tmpbuf[MAX_PATH] = { 0L };
				wcscpy_s(tmpbuf, MAX_PATH, L"---unused---");
				WriteFile(hfile, tmpbuf, sizeof(WCHAR) * MAX_PATH, &wleng, NULL);
				if (wleng != (sizeof(WCHAR) * MAX_PATH)) {
					_ASSERT(0);
					return 1;
				}
			}
		}
	}



	FlushFileBuffers(hfile);
	SetEndOfFile(hfile);
	CloseHandle(hfile);

	return 0;
}
bool CFrameCopyDlg::LoadTBOFile(WCHAR* srcfilename)
{
	if (!m_model) {
		_ASSERT(0);
		return false;
	}



	HANDLE hfile;
	hfile = CreateFile(srcfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		//_ASSERT(0);
		return false;
	}

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize(hfile, &sizehigh);
	if (bufleng <= 0) {
		_ASSERT(0);
		return false;
	}
	if (sizehigh != 0) {
		_ASSERT(0);
		return false;
	}
	char* newbuf;
	newbuf = (char*)malloc(sizeof(char) * bufleng);//bufleng + 1
	if (!newbuf) {
		_ASSERT(0);
		return false;
	}
	ZeroMemory(newbuf, sizeof(char) * bufleng);
	DWORD rleng, readleng;
	rleng = bufleng;
	BOOL bsuccess;
	bsuccess = ReadFile(hfile, (void*)newbuf, rleng, &readleng, NULL);
	if (!bsuccess || (rleng != readleng)) {
		_ASSERT(0);
		CloseHandle(hfile);
		if (newbuf) {
			free(newbuf);
			newbuf = 0;
		}
		return false;
	}


	char TBOheader[256];
	ZeroMemory(TBOheader, sizeof(char) * 256);
	int filetype = 0;
	filetype = ValidateTBOFile(TBOheader, newbuf, bufleng);

	int slotNumForLoad = 0;
	if (filetype == 1) {
		slotNumForLoad = FCSLOTNUM;
	}
	else if ((filetype == 2) || (filetype == 3) || (filetype == 4)) {
		slotNumForLoad = FCSLOTNUM2;
	}
	else {
		_ASSERT(0);
		if (newbuf) {
			free(newbuf);
			newbuf = 0;
		}
		CloseHandle(hfile);
		return false;
	}


	DWORD curpos;
	curpos = sizeof(char) * 256;

	//WCHAR m_slotname[slotNumForLoad][SLOTNAMELEN];
	if ((curpos + sizeof(WCHAR) * slotNumForLoad * SLOTNAMELEN) > bufleng) {
		_ASSERT(0);
		if (newbuf) {
			free(newbuf);
			newbuf = 0;
		}
		CloseHandle(hfile);
		return false;
	}
	MoveMemory(m_slotname, newbuf + curpos, sizeof(WCHAR) * slotNumForLoad * SLOTNAMELEN);
	curpos += sizeof(WCHAR) * slotNumForLoad * SLOTNAMELEN;


	//int m_influencenum[slotNumForLoad];
	if ((curpos + sizeof(int) * slotNumForLoad) > bufleng) {
		_ASSERT(0);
		if (newbuf) {
			free(newbuf);
			newbuf = 0;
		}
		CloseHandle(hfile);
		return false;
	}
	MoveMemory(m_influencenum, newbuf + curpos, sizeof(int) * slotNumForLoad);
	curpos += sizeof(int) * slotNumForLoad;

	if ((filetype == 1) || (filetype == 2)) {
		//int m_influencelist[slotNumForLoad][FRAMECOPYLISTLENG];
		if ((curpos + sizeof(int) * slotNumForLoad * FRAMECOPYLISTLENG) > bufleng) {
			_ASSERT(0);
			if (newbuf) {
				free(newbuf);
				newbuf = 0;
			}
			CloseHandle(hfile);
			return false;
		}
		MoveMemory(m_influencelist, newbuf + curpos, sizeof(int) * slotNumForLoad * FRAMECOPYLISTLENG);
		curpos += sizeof(int) * slotNumForLoad * FRAMECOPYLISTLENG;
	}
	else if ((filetype == 3) || (filetype == 4)) {
		int slotno;
		int influenceno;
		for (slotno = 0; slotno < slotNumForLoad; slotno++) {
			
			int numinfluence = min(m_influencenum[slotno], FRAMECOPYLISTLENG);//有効データ数


			int validnum = 0;//2023/10/01
			for (influenceno = 0; influenceno < numinfluence; influenceno++) {
				if ((curpos + sizeof(WCHAR) * MAX_PATH) > bufleng) {
					_ASSERT(0);
					if (newbuf) {
						free(newbuf);
						newbuf = 0;
					}
					CloseHandle(hfile);
					return false;
				}
				WCHAR tmpbuf[MAX_PATH] = { 0L };
				MoveMemory(tmpbuf, newbuf + curpos, sizeof(WCHAR)* MAX_PATH);
				int cmp1 = wcscmp(tmpbuf, L"---unused---");
				if (cmp1 != 0) {
					CBone* loadbone = m_model->GetBoneByWName(tmpbuf);
					if (loadbone) {
						//m_influencelist[slotno][influenceno] = loadbone->GetBoneNo();
						
						int boneno = loadbone->GetBoneNo();
						if (boneno > 0) {
							m_influencelist[slotno][validnum] = boneno;//2023/10/01
							validnum++;
						}
						else {
							//RootNodeエントリ boneno == 0 は読み込まない
						}
					}
				}
				curpos += (sizeof(WCHAR) * MAX_PATH);
			}

			m_influencenum[slotno] = validnum;//2023/10/01

			for (influenceno = validnum; influenceno < FRAMECOPYLISTLENG; influenceno++) {//未設定部分 validnum-->FRAMECOPYLISTLENG
				m_influencelist[slotno][influenceno] = 0;
			}

			if (filetype == 3) {//2023/09/25 filetype4には未設定エントリーは書き出されない
				for (influenceno = numinfluence; influenceno < FRAMECOPYLISTLENG; influenceno++) {//未読込部分 numinfluence-->FRAMECOPYLISTLENG
					curpos += (sizeof(WCHAR) * MAX_PATH);
				}
			}
		}
	}
	else {
		_ASSERT(0);
		return false;
	}

	//int m_ignorenum[slotNumForLoad];
	if ((curpos + sizeof(int) * slotNumForLoad) > bufleng) {
		_ASSERT(0);
		if (newbuf) {
			free(newbuf);
			newbuf = 0;
		}
		CloseHandle(hfile);
		return false;
	}
	MoveMemory(m_ignorenum, newbuf + curpos, sizeof(int) * slotNumForLoad);
	curpos += sizeof(int) * slotNumForLoad;


	if ((filetype == 1) || (filetype == 2)) {
		//int m_ignorelist[slotNumForLoad][FRAMECOPYLISTLENG];
		if ((curpos + sizeof(int) * slotNumForLoad * FRAMECOPYLISTLENG) > bufleng) {
			_ASSERT(0);
			if (newbuf) {
				free(newbuf);
				newbuf = 0;
			}
			CloseHandle(hfile);
			return false;
		}
		MoveMemory(m_ignorelist, newbuf + curpos, sizeof(int) * slotNumForLoad * FRAMECOPYLISTLENG);
		curpos += sizeof(int) * slotNumForLoad * FRAMECOPYLISTLENG;
	}
	else if ((filetype == 3) || (filetype == 4)) {
		int slotno;
		int ignoreno;
		for (slotno = 0; slotno < slotNumForLoad; slotno++) {

			int numignore = min(m_ignorenum[slotno], FRAMECOPYLISTLENG);//有効データ数

			int validnum = 0;//2023/10/01
			for (ignoreno = 0; ignoreno < numignore; ignoreno++) {
				if ((curpos + sizeof(WCHAR) * MAX_PATH) > bufleng) {
					_ASSERT(0);
					if (newbuf) {
						free(newbuf);
						newbuf = 0;
					}
					CloseHandle(hfile);
					return false;
				}
				WCHAR tmpbuf[MAX_PATH] = { 0L };
				MoveMemory(tmpbuf, newbuf + curpos, sizeof(WCHAR) * MAX_PATH);
				int cmp1 = wcscmp(tmpbuf, L"---unused---");
				if (cmp1 != 0) {
					CBone* loadbone = m_model->GetBoneByWName(tmpbuf);
					if (loadbone) {
						//m_ignorelist[slotno][ignoreno] = loadbone->GetBoneNo();

						int boneno = loadbone->GetBoneNo();
						if (boneno > 0) {
							m_ignorelist[slotno][validnum] = boneno;//2023/10/01
							validnum++;
						}
						else {
							//RootNodeエントリ boneno == 0 は読み込まない
						}
					}
				}
				curpos += (sizeof(WCHAR) * MAX_PATH);
			}

			m_ignorenum[slotno] = validnum;//2023/10/01

			for (ignoreno = validnum; ignoreno < FRAMECOPYLISTLENG; ignoreno++) {//未設定部分 validnum-->FRAMECOPYLISTLENG
				m_ignorelist[slotno][ignoreno] = 0;
			}

			if (filetype == 3) {//2023/09/25 filetype4には未設定エントリーは書き出されない
				for (ignoreno = numignore; ignoreno < FRAMECOPYLISTLENG; ignoreno++) {//未読込部分 numignore-->FRAMECOPYLISTLENG
					curpos += (sizeof(WCHAR) * MAX_PATH);
				}
			}
		}
	}
	else {
		_ASSERT(0);
		return false;
	}


	if (newbuf) {
		free(newbuf);
		newbuf = 0;
	}
	CloseHandle(hfile);

	return true;
}

int CFrameCopyDlg::SaveWithProjectFile(WCHAR* srcfilename)
{
	if (!srcfilename) {
		_ASSERT(0);
		return 1;
	}
	int result = WriteTBOFile(srcfilename);
	return result;
}
bool CFrameCopyDlg::LoadWithProjectFile(WCHAR* srcfilename)
{
	bool result = LoadTBOFile(srcfilename);
	if (result) {
		//ParamsToDlg();
		m_tboloadedflag = true;
	}
	return result;
}
