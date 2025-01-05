// MotChangeDlg.cpp : CMotChangeDlg のインプリメンテーション
#include "stdafx.h"
#include "MotChangeDlg.h"

#include <MCHandler.h>

#include "MCRMenu.h"

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#include "MCParentDlg.h"
#include "MCChildDlg.h"
#include <MAFile.h>

#include "ChkOWDlg.h"

#include <forbidid.h>
#include <EventKey.h>

#include <ChaScene.h>

#include <GetDlgParams.h>
#include <GlobalVar.h>

static WCHAR s_strkey[40][3] = {
	L"←", L"→", L"↑", L"↓", L"1", L"2", L"3", L"4", L"5", L"6",
	L"7", L"8", L"9", L"0", L"A", L"B", L"C", L"D", L"E", L"F",
	L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P",
	L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z"
};
static int s_vkkey[40] = {
	VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', 'A', 'B', 'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

/////////////////////////////////////////////////////////////////////////////


static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


/////////////////////////////////////////////////////////////////////////////
// CMotChangeDlg

CMotChangeDlg::CMotChangeDlg()
{
	InitParams();
}

void CMotChangeDlg::InitParams()
{
	m_hImageList = 0;
	m_iImage = 0;
	m_iSelect = 0;

	m_TI = 0;
	m_selecteditem = 0;
	m_selectedno = 0;


	//m_papp = papp;
////	m_thandler = papp->m_thandler;
////	m_shandler = papp->m_shandler;
	//m_apphwnd = papp->m_hWnd;
	m_chascene = nullptr;


	m_cmdshow = SW_HIDE;

	m_rmenu = nullptr;
	m_undertreeedit = 0;

	m_firstmake = 0;

	ZeroMemory(m_idlingname, sizeof(char) * MAX_PATH);

	m_overwritekind = OW_YES;

	m_eventno = 1;
	m_combono = 0;
	//if( papp->m_mhandler ){
	//	m_fuleng = papp->m_mhandler->m_fuleng;
	//}else{
	m_fuleng = g_defaultFillUpMOA;
	//}

	m_cpelemnum = 0;
	ZeroMemory(m_cpelem, sizeof(MCELEM) * MAXMCCOPYNUM);

	m_singleevent = 1;
}
void CMotChangeDlg::DestroyObjs()
{
	if (m_hImageList) {
		ImageList_Destroy(m_hImageList);
		m_hImageList = nullptr;
	}

	if (m_TI) {
		free(m_TI);
		m_TI = 0;
	}

	if (m_rmenu) {
		m_rmenu->Destroy();
		delete m_rmenu;
		m_rmenu = 0;
	}
}

CMotChangeDlg::~CMotChangeDlg()
{
	DestroyObjs();
}

int CMotChangeDlg::SetChaScene(ChaScene* srcchascene)
{
	m_chascene = srcchascene;
	return 0;
}
int CMotChangeDlg::SetVisible(bool srcflag)
{
	int ret;

	BOOL dummy;
	OnStop( 0, 0, 0, dummy );

	if (srcflag) {
		m_cmdshow = SW_SHOW;
	}
	else {
		m_cmdshow = SW_HIDE;
	}
	ShowWindow(m_cmdshow);

	if( m_cmdshow != SW_HIDE ){
		////InitTree();

		////if( m_papp->m_mhandler ){
		////	m_fuleng = m_papp->m_mhandler->m_fuleng;
		////}else{
			m_fuleng = g_defaultFillUpMOA;
		////}

		m_undertreeedit = 0;

		m_cpelemnum = 0;
		ZeroMemory( m_cpelem, sizeof( MCELEM ) * MAXMCCOPYNUM );

		ret = InitList();
		_ASSERT( !ret );

		ret = InitMC();
		if( ret ){
			DbgOut( L"motchangedlg : SetVisible : InitMC error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}

	return 0;
}

int CMotChangeDlg::InitMC()
{
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : InitMC : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}


	//int motnum;
	//motnum = currentmodel->GetMotInfoSize();
	//MOTID* motidarray;
	//motidarray = g_motdlg->GetMotIDArray();
	
	int initidling = -1;
	initidling = InitComboIdle();

	//ret = m_mch->InitMCArray( motnum, motidarray, initidling );
	//if( ret ){
	//	DbgOut( "motchangedlg : InitMC : mch InitMCArray error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}


	ret = FillTree();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = ParamsToDlg();
	_ASSERT( !ret );

	m_firstmake = 1;

	return 0;
}



LRESULT CMotChangeDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int ret;

	if (!m_chascene) {
		_ASSERT(0);
		return 0;
	}
	//CModel* currentmodel = GetCurrentModel();
	//if (!currentmodel) {
	//	_ASSERT(0);
	//	return 0;
	//}
	//ret = currentmodel->CreateMotChangeHandlerIfNot();
	//if (ret) {
	//	DbgOut(L"motchangedlg : OnInitDialog : CreateMotChangeHandlerIfNot error !!!\n");
	//	_ASSERT(0);
	//	return 0;
	//}

//	_ASSERT( g_motdlg );
//	_ASSERT( g_motdlg->m_motparamdlg );
//	g_motdlg->m_motparamdlg->m_mch = m_mch;

	if( !m_rmenu ){
		m_rmenu = new CMCRMenu();
		if( !m_rmenu ){
			DbgOut( L"motchangedlg : OnInitDialog : rmenu alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = m_rmenu->Create( m_hWnd );
		if( ret ){
			DbgOut( L"motchangedlg : OnInitDialog : rmenu create error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	//InitCommonControls();

	m_dlg_wnd = m_hWnd;
	m_tree_wnd = GetDlgItem( IDC_TREE1 );
	_ASSERT(m_tree_wnd);
	m_list_wnd = GetDlgItem( IDC_LIST1 );
	_ASSERT(m_list_wnd);
	m_event_wnd = GetDlgItem( IDC_EVENT );
	_ASSERT(m_event_wnd);
	m_combokey_wnd = GetDlgItem( IDC_COMBOKEY );
	_ASSERT(m_combokey_wnd);
	m_comboidle_wnd = GetDlgItem( IDC_COMBOIDLE );
	_ASSERT(m_comboidle_wnd);
	m_fuleng_wnd = GetDlgItem( IDC_FULENG );
	_ASSERT(m_fuleng_wnd);

	InitComboKey();
	CreateImageList();

//	ret = FillTree();
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}
//
//	ret = ParamsToDlg();
//	_ASSERT( !ret );


	return 1;  // システムにフォーカスを設定させます
}

LRESULT CMotChangeDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	//外部クラスから、wNotifyCode = 999 で呼び出されることがある。（ダイアログは表示されていない状態）

//	EndDialog(wID);

	SetVisible(SW_HIDE);

	return 0;
}

LRESULT CMotChangeDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
//	EndDialog(wID);

	SetVisible(SW_HIDE);

	return 0;
}

int CMotChangeDlg::FillTree()
{

	TreeView_DeleteAllItems( m_tree_wnd );


	HTREEITEM hRoot = TreeView_GetRoot( m_tree_wnd );
	//m_selecteditem = hRoot;

	if( m_TI ){
		free( m_TI );
		m_TI = 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 0;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 0;
	}
	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : FillTree : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int totalmcnum = mch->GetTotalMCNum();

	if( totalmcnum == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	m_TI = (HTREEITEM*)malloc(sizeof(HTREEITEM) * totalmcnum);
	if( !m_TI ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory(m_TI, sizeof(HTREEITEM) * totalmcnum);
	

	*m_TI = hRoot;

	m_selectedno = 0;
	m_selecteditem = 0;

	AddMCToTree( hRoot );

	int serino;
	for( serino = 0; serino < totalmcnum; serino++ ){
		HTREEITEM* curitem;
		curitem = m_TI + serino;
		if( curitem ){

			MCELEM* curmce;
			MCELEM* parmce;
			curmce = mch->Setno2MCElem( serino, &parmce );
			if( !parmce && curmce && (curmce->closetree == 0) ){
				TreeView_Expand( m_tree_wnd, *curitem, TVE_EXPAND );
			}
		}
	}
	TreeView_SelectSetFirstVisible( m_tree_wnd, m_selecteditem );

	return 0;
}


int CMotChangeDlg::AddMCToTree( HTREEITEM hRoot )
{
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : AddMCToTree : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}


	int mcno;
	int childno;
	int addtolast = 1;
	int setno;

	for( mcno = 0; mcno < mch->GetMCNum(); mcno++ ){
		MCELEM* curmce = mch->GetMCElem(mcno);
		if (curmce && (curmce->id > 0)) {
			setno = curmce->setno;//!!!!!!!!!!!!

			MOTINFO currentmi = currentmodel->GetMotInfo(curmce->id);
			char* nameptr;
			nameptr = currentmi.motname;

			char parname[1024];
			//sprintf_s( parname, 1024, "%s (共%d,禁%d)", nameptr, curmce->commonid, curmce->forbidcommonid );
			sprintf_s(parname, 1024, "%s (共%d)", nameptr, curmce->commonid);


			HTREEITEM newTI;
			newTI = TVAdd(hRoot, parname, setno, m_iImage, m_iSelect, addtolast);
			if (!newTI) {
				DbgOut(L"MotChangeDlg : AddMCToTree : TVAdd error %d %s!!!\n", curmce->id, nameptr);
				_ASSERT(0);
				return 1;
			}
			*(m_TI + setno) = newTI;
			if (setno == 0) {
				m_selecteditem = newTI;
				m_selectedno = setno;
				TreeView_SelectItem(m_tree_wnd, m_selecteditem);
			}

			/////////

			for (childno = 0; childno < curmce->childnum; childno++) {
				MCELEM* curchildmc;
				curchildmc = curmce->childmc + childno;
				setno = curchildmc->setno;//!!!!!!!!!!!

				MOTINFO childmi = currentmodel->GetMotInfo(curchildmc->id);
				char* childname;
				childname = childmi.motname;

				char setname[1024];
				sprintf_s(setname, 1024, "%s (%d)", childname, curchildmc->eventno1);

				HTREEITEM newchildTI;
				newchildTI = TVAdd(newTI, setname, setno, m_iImage, m_iSelect, addtolast);
				if (!newchildTI) {
					DbgOut(L"MotChangeDlg : AddMCToTree : TVAdd child error %d %s!!!\n", curchildmc->id, childname);
					_ASSERT(0);
					return 1;
				}
				*(m_TI + setno) = newchildTI;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	return 0;
}



HTREEITEM CMotChangeDlg::TVAdd( HTREEITEM parentTI, char* srcname, int srcno, int imageno, int selectno, int addtolast )
{
	//ここのDbg文は取り除かない。
	DbgOut( L"MotChangeDlg : TVAdd : srcname %s, srcno %d, imageno %d, selectno %d, addtolast %d\n",
		srcname, srcno, imageno, selectno, addtolast );

	WCHAR wtvname[1024] = { 0L };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, srcname, -1, wtvname, 1024);


	TVITEM tvi;
	tvi.mask = TVIF_TEXT;
	tvi.pszText = wtvname;

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

void CMotChangeDlg::CreateImageList()
{
	m_tree_wnd.SetWindowLong(
		GWL_STYLE,
		WS_CHILD | WS_VISIBLE | WS_BORDER | 
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS
		);

	HBITMAP hBitmap;
	m_hImageList = ImageList_Create( 16, 16, ILC_COLOR, 2, 10 );
	
	hBitmap = LoadBitmap( (HINSTANCE)GetModuleHandle(NULL),
		MAKEINTRESOURCE( IDB_BONE_IMAGE ) );
	if( hBitmap == NULL ){
		DbgOut( L"MotChangeDlg : CreateImageList : LoadBitmap error !!!\n" );
		_ASSERT(0);
		return;
	}
	m_iImage = ImageList_Add( m_hImageList, hBitmap, (HBITMAP)0 );
	if( m_iImage == -1 ){
		DbgOut( L"MotChangeDlg : CreateImageList : ImageList_Add error !!!\n" );	
		_ASSERT(0);
		return;
	}
	DeleteObject( hBitmap );

	hBitmap = LoadBitmap( (HINSTANCE)GetModuleHandle(NULL),
		MAKEINTRESOURCE( IDB_BONE_SELECT ) );
	if( hBitmap == NULL ){
		DbgOut( L"MotChangeDlg : CreateImageList : LoadBitmap error !!!\n" );		
		_ASSERT(0);
		return;
	}
	m_iSelect = ImageList_Add( m_hImageList, hBitmap, (HBITMAP)0 );
	if( m_iSelect == -1 ){
		DbgOut( L"MotChangeDlg : CreateImageList : ImageList_Add error !!!\n" );	
		_ASSERT(0);
		return;
	}
	DeleteObject( hBitmap );

	HIMAGELIST retIL;
	retIL = TreeView_SetImageList( m_tree_wnd,
		m_hImageList, TVSIL_NORMAL );

	
}
int CMotChangeDlg::ParamsToDlg()
{
	WCHAR mes[256];

	swprintf_s( mes, 256, L"%d", m_fuleng );
	m_fuleng_wnd.SetWindowText(mes);

	swprintf_s( mes, 256, L"%d", m_eventno );
	m_event_wnd.SetWindowText( mes );

	m_combokey_wnd.SendMessage( CB_SETCURSEL, m_combono, 0 );

	if( m_singleevent ){
		::CheckDlgButton( m_dlg_wnd, IDC_SINGLEEVENT, BST_CHECKED );
	}else{
		::CheckDlgButton( m_dlg_wnd, IDC_SINGLEEVENT, BST_UNCHECKED );
	}

	return 0;
}

LRESULT CMotChangeDlg::OnExpandedTree1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	if(g_previewFlag != 0){
		return 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnExpandedTree1 : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}




	// TODO : ｺﾝﾄﾛｰﾙの通知ﾊﾝﾄﾞﾗ用のｺｰﾄﾞを追加してください。
	NMTREEVIEW* pnmtv = (NMTREEVIEW*)pnmh;
	TVITEM tvi = pnmtv->itemNew;

	int closetree = 0;
	if( pnmtv->action == 1 ){
		closetree = 1;
	}else if( pnmtv->action == 2 ){
		closetree = 0;
	}

//	m_selecteditem = tvi.hItem;
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	if( !TreeView_GetItem( m_tree_wnd, &tvi ) )
		return 0;

	int cursetno;
	cursetno = (int)(tvi.lParam);

	MCELEM* curmce;
	MCELEM* parmce = 0;
	curmce = mch->Setno2MCElem( cursetno, &parmce );
	
	if( curmce && !parmce ){
		curmce->closetree = closetree;
	}

	return 0;
}


LRESULT CMotChangeDlg::OnSelchangedTree1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	if( m_undertreeedit == 1 )
		return 0;
	if (g_previewFlag != 0) {
		return 0;
	}


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

LRESULT CMotChangeDlg::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnContectMenu : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}


	POINT screenpos, clickpos;
	clickpos.x = GET_X_LPARAM(lParam); 
	clickpos.y = GET_Y_LPARAM(lParam);
	screenpos = clickpos;
	m_tree_wnd.ScreenToClient( &clickpos );
	
	TVHITTESTINFO hittestinfo;
	hittestinfo.pt = clickpos;
	hittestinfo.flags = TVHT_ONITEMLABEL;
	hittestinfo.hItem = NULL;

	HTREEITEM hititem = NULL;
	hititem = TreeView_HitTest( m_tree_wnd, &hittestinfo );

	if( hititem != NULL ){
		
		TVITEM tvi;
		tvi.hItem = hititem;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		ret = TreeView_GetItem( m_tree_wnd, &tvi );
		int setno;
		setno = (int)tvi.lParam;
	
		if( m_selectedno != setno ){
			OnUserSelchange( setno );
		}

		MCELEM* curmce;
		MCELEM* parmce = 0;
		curmce = mch->Setno2MCElem( m_selectedno, &parmce );
		int isparent;
		if( parmce ){
			isparent = 0;
		}else{
			isparent = 1;
		}

		m_rmenu->TrackPopupMenu( isparent, m_cpelemnum, m_hWnd, screenpos );

	}

	return 0;
}

int CMotChangeDlg::OnUserSelchange( int selno )
{
	
	HTREEITEM selitem;
	selitem = *(m_TI + (int)selno);
	if( selitem ){
		m_selectedno = selno;
		m_selecteditem = selitem;

		TreeView_SelectItem( m_tree_wnd,
			m_selecteditem );
	}
	return S_OK;
}

LRESULT CMotChangeDlg::OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnAdd : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}



	CMCChildDlg dlg(currentmodel, 0);
	int dlgret;
	dlgret = (int)dlg.DoModal( m_hWnd );
	if( dlgret == IDOK ){
		ret = mch->AddChild( m_selectedno, dlg.m_cookie, dlg.m_always, dlg.m_frameno1, dlg.m_frameno2, dlg.m_eventno1, dlg.m_notfu, dlg.m_nottoidle );
		if( ret ){
			DbgOut( L"motchangedlg : OnAdd : mch AddChild error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		currentmodel->SetMotInfoLoopFlagByID(dlg.m_cookie, 0);


		///////////////////////////

		int saveselectedno = m_selectedno;

		ret = FillTree();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		OnUserSelchange( saveselectedno );


		ret = ParamsToDlg();
		_ASSERT( !ret );
		

	}

	return 0;
}
LRESULT CMotChangeDlg::OnDel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnDel : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}


	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}


	int delmotid;
	delmotid = mch->Setno2MotID( m_selectedno );
	if( delmotid < 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!
	}
	
	MOTINFO currentmi = currentmodel->GetMotInfo(delmotid);
	if (currentmi.motid <= 0) {
		_ASSERT(0);
		return 1;
	}

	char* nameptr;
	nameptr = currentmi.motname;

	char messtr[256];
	sprintf_s( messtr, 256, "%sを、ツリーから削除します。よろしいでしょうか。", nameptr );

	int dlgret;
	dlgret = ::MessageBoxA( m_hWnd, messtr, "確認", MB_OKCANCEL );
	if( dlgret != IDOK ){
		return 0;//!!!!!!!!!!!!!!!
	}

	ret = mch->DeleteMCElem( m_selectedno );
	if( ret ){
		DbgOut( L"motchangedlg : OnDel : mch DeleteMCElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = FillTree();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = ParamsToDlg();
	_ASSERT( !ret );
	
	return 0;
}

LRESULT CMotChangeDlg::OnProp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnDel : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}


	//::MessageBox( m_hWnd, "OnProp", "rmenu", MB_OK );
	HTREEITEM parti;
	parti = TreeView_GetParent( m_tree_wnd, m_selecteditem );

	int selcookie;
	selcookie = mch->Setno2MotID( m_selectedno );
	if( selcookie < 0 ){
		DbgOut( L"motchangedlg : OnProp : Setno2MotID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	MCELEM* curmce;
	MCELEM* parmce;
	curmce = mch->Setno2MCElem( m_selectedno, &parmce );
	if( !curmce ){
		DbgOut( L"motchangedlg : OnProp : mch Setno2MCElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int isparent;
	if( parti ){
		isparent = 0;


		int befcookie;
		befcookie = curmce->id;

		CMCChildDlg dlg( currentmodel, curmce );
		int dlgret;
		dlgret = (int)dlg.DoModal( m_hWnd );
		if( dlgret == IDOK ){
			
			ret = mch->SetMCElem( m_selectedno, dlg.m_cookie, dlg.m_always, dlg.m_frameno1, dlg.m_frameno2, dlg.m_eventno1, dlg.m_notfu, curmce->idling, dlg.m_nottoidle );
			if( ret ){
				DbgOut( L"motchangedlg : OnProp : mch SetMCElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			
			int saveselectedno = m_selectedno;
			
			ret = FillTree();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			ret = ParamsToDlg();
			_ASSERT( !ret );

			OnUserSelchange( saveselectedno );
		}

	}else{


		isparent = 1;

		MOTINFO currentmi = currentmodel->GetMotInfo(selcookie);
		char* mname;
		mname = currentmi.motname;
		WCHAR* wfname;
		wfname = currentmi.wfilename;
		int totalframe;
		totalframe = IntTime(currentmi.frameleng);

		CMCParentDlg dlg( mname, wfname, totalframe, curmce->ev0idle, curmce->commonid, curmce->forbidnum, curmce->forbidid, curmce->notfu );

		int dlgret;
		dlgret = (int)dlg.DoModal( m_hWnd );
		if( dlgret == IDOK ){

			int changename = 0;

			//idling設定の更新
			if( curmce->idling ){
				//strcpy_s( m_idlingname, MAX_PATH, fname );
				strcpy_s(m_idlingname, MAX_PATH, mname);//2025/01/02
			}
			curmce->ev0idle = dlg.GetEv0Idle();
			if( curmce->commonid != dlg.GetCommonId() ){
				changename = 1;
			}
			curmce->commonid = dlg.GetCommonId();


			int issame = 0;
			issame = IsSameForbidID( curmce, dlg.GetForbidNum(), dlg.GetForbidId() );
			if( issame == 0 ){
				changename = 1;
			
				ret = SetMCEForbidID( curmce, dlg.GetForbidNum(), dlg.GetForbidId());
				if( ret ){
					DbgOut( L"mcdlg : OnProp : parent : SetMCEForbidID error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			curmce->notfu = dlg.GetNotFU();//!!!!!!!!!
			curmce->nottoidle = 0;

			//MOTID* curmotid;
			//curmotid = motidarray + selcookie;
			//if( curmotid->id != selcookie ){
			//	_ASSERT( 0 );
			//	return 1;
			//}
			//curmotid->ev0idle = curmce->ev0idle;
			//curmotid->commonid = curmce->commonid;

			//if( curmotid->forbidid ){
			//	free( curmotid->forbidid );
			//	curmotid->forbidid = 0;
			//}
			//curmotid->forbidnum = 0;
			//if( dlg.forbidnum > 0 ){
			//	curmotid->forbidid = (int*)malloc( sizeof( int ) * dlg.forbidnum );
			//	if( !curmotid->forbidid ){
			//		DbgOut( "mcdlg : OnProp : parent ; motid : forbidid alloc error !!!\n" );
			//		_ASSERT( 0 );
			//		return 1;
			//	}
			//	memcpy( curmotid->forbidid, dlg.forbidid, sizeof( int ) * dlg.forbidnum );
			//	curmotid->forbidnum = dlg.forbidnum;
			//}
			//curmotid->notfu = curmce->notfu;


			////名前のチェックと変更
			//int cmp;
			//cmp = strcmp( mname, dlg.GetName() );
			//if( (cmp != 0) || changename ){
			//	////strcpy_s( m_motionname, 256, dlg.m_name );
			//
			//	ret = m_papp->m_mhandler->SetMotionName( selcookie, dlg.name );
			//	if( ret ){
			//		_ASSERT( 0 );
			//		DbgOut( "motchangedlg : OnProp : SetMotionName error !!!\n" );
			//		m_undertreeedit = 0;
			//		return 1;
			//	}
			//
			//	::SendMessage( m_apphwnd, WM_USER_ANIMMENU, selcookie, 0 );
			//
			//	ret = FillTree();
			//	if( ret ){
			//		_ASSERT( 0 );
			//		return 1;
			//	}
			//
			//	InitComboIdle();
			//
			//	ret = ParamsToDlg();
			//	_ASSERT( !ret );
			//}
			
		}

		//m_undertreeedit = 0;

	}

	return 0;
}

LRESULT CMotChangeDlg::OnCopy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnCopy : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}
	//int ret;

	HTREEITEM parti;
	parti = TreeView_GetParent( m_tree_wnd, m_selecteditem );

	MCELEM* curmce;
	MCELEM* parmce;
	curmce = mch->Setno2MCElem( m_selectedno, &parmce );
	if( !curmce ){
		DbgOut( L"motchangedlg : OnProp : mch Setno2MCElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( parti ){
		//子供１個コピー
		m_cpelemnum = 1;
		m_cpelem[0] = *curmce;
	}else{
		//子供全部をコピー
		if( curmce->childnum < MAXMCCOPYNUM ){
			m_cpelemnum = curmce->childnum;
		}else{
			m_cpelemnum = MAXMCCOPYNUM;
		}
		MoveMemory( m_cpelem, curmce->childmc, sizeof( MCELEM ) * m_cpelemnum );
	}

	return 0;
}
LRESULT CMotChangeDlg::OnPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnCopy : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}

	HTREEITEM parti;
	parti = TreeView_GetParent( m_tree_wnd, m_selecteditem );

	MCELEM* curmce;
	MCELEM* parmce;
	curmce = mch->Setno2MCElem( m_selectedno, &parmce );
	if( !curmce ){
		DbgOut( L"motchangedlg : OnPaste : mch Setno2MCElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( parti ){
		::MessageBox( NULL, L"子供要素にはペーストできません。親を選んで再試行してください。", L"エラー", MB_OK );
		return 0;
	}else{
		int cpno;
		for( cpno = 0; cpno < m_cpelemnum; cpno++ ){
			MCELEM* curchild = m_cpelem + cpno;

			ret = mch->AddChild( curmce->setno, curchild->id, 0, curchild->frameno1, curchild->frameno2, curchild->eventno1, curchild->notfu, curchild->nottoidle );
			if( ret ){
				DbgOut( L"motchangedlg : OnPaste : mch AddChild error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			currentmodel->SetMotInfoLoopFlagByID(curchild->id, 0);
		}

		if( m_cpelemnum > 0 ){
			ret = FillTree();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			ret = ParamsToDlg();
			_ASSERT( !ret );
		}

	}

	return 0;
}


int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return ( (int)lParam1 - (int)lParam2 );	
}

int CMotChangeDlg::DestroyMCElem( int delcookie )
{
	int ret;

	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnCopy : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	if( delcookie < 0 ){

		ret = mch->DestroyObjs();
		if( ret ){
			DbgOut( L"motchangedlg : DestroyMCElem : mch DestroyObjs error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{

		int delsetno;
		delsetno = mch->MotID2Setno( delcookie );
		if( delsetno < 0 ){
			_ASSERT( 0 );
			return 0;//!!!!!!!!!!!!!
		}

//DbgOut( "check !!! : motchangedlg : DestroyMCElem : delcookie %d, delsetno %d\r\n", delcookie, delsetno );

		ret = mch->DeleteMCElem( delsetno );
		if( ret ){
			DbgOut( L"motchangedlg : DestroyMCElem : mch DeleteMCElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	ret = FillTree();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	InitComboIdle();

	ret = ParamsToDlg();
	_ASSERT( !ret );

	return 0;
}

int CMotChangeDlg::AddParentMC( int addcookie, int srcidling, int srcev0idle, int srccommonid, int srcforbidnum, int* srcforbidid, int srcnotfu )
{
	int ret;

/***
	if( m_firstmake == 0 ){
		ret = InitMC();
		if( ret ){
			DbgOut( "motchangedlg : AddParentMC :  error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
***/

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : AddParentMC : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}

	MOTINFO currentmi = currentmodel->GetMotInfo(addcookie);

	if (currentmi.motid > 0) {
		if (srcidling != 0) {
			//m_idlingid = addcookie;
			strcpy_s(m_idlingname, MAX_PATH, currentmi.motname);
		}

		ret = mch->AddParentMC(addcookie, srcidling, srcev0idle, srccommonid, srcforbidnum, srcforbidid, srcnotfu);
		if (ret) {
			DbgOut(L"motchangeldg : AddParentMC : mch AddParentMC error !!!\n");
			_ASSERT(0);
			return 1;
		}
		currentmodel->SetMotInfoLoopFlagByID(addcookie, 0);

		ret = FillTree();
		if (ret) {
			_ASSERT(0);
			return 1;
		}

		InitComboIdle();

		ret = ParamsToDlg();
		_ASSERT(!ret);
	}



	return 0;
}

int CMotChangeDlg::SaveMAFile( char* srcdirname, char* srcmafname, int overwriteflag )
{

	_ASSERT(0);

	//int ret;

	//m_overwritekind = OW_YES;//!!!!!!!!!!!!!

	//char maffilename[MAX_PATH];
	//sprintf_s( maffilename, MAX_PATH, "%s\\%s.moa", srcdirname, srcmafname );

	//CMAFile* mafile;
	//mafile = new CMAFile();
	//if( !mafile ){
	//	DbgOut( L"motchangedlg : SaveMAFile : mafile alloc error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	//ret = mafile->SaveMAFile( maffilename, m_papp->m_mhandler, m_mch, m_hWnd, overwriteflag );
	//if( ret == -1 ){
	//	::MessageBox( m_hWnd, L"同じモーション名が複数あると、moaを書き出せません。\n名前を変えて、再試行してください。", L"名前エラー", MB_OK );
	//	delete mafile;
	//	mafile = 0;
	//	return 0;
	//}else if( ret == -2 ){
	//	::MessageBox( m_hWnd, L"アイドリング設定のモーションが、１つだけ存在する必要があります。\n設定を直して、再試行してください。", L"アイドリングエラー", MB_OK );
	//	delete mafile;
	//	mafile = 0;
	//	return 0;
	//}else if( ret != 0 ){
	//	DbgOut(L"motchangedlg : SaveMAFile : mafile SaveMAFile error !!!\n" );
	//	_ASSERT( 0 );
	//	delete mafile;
	//	mafile = 0;
	//	return 1;
	//}

	//delete mafile;
	//mafile = 0;


	/////////////////// quaの保存

	//if( !m_papp->m_mhandler || !m_papp->m_shandler ){
	//	return 0;//!!!!!!!!!!!!!!!!!!
	//}

	//if( m_papp->m_mhandler->s2mot_leng <= 1 ){
	//	return 0;//!!!!!!!!!!!!!!!!!!
	//}

	//CMotionCtrl* mcptr = (*m_papp->m_mhandler)( 1 );
	//if( !mcptr ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	//CMotionInfo* miptr = mcptr->motinfo;
	//if( !miptr ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	//if( !(miptr->motname) ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	////szName = *(miptr->motname + cookie);

	//int mcno;
	//for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
	//	MCELEM* curmce;
	//	curmce = m_mch->m_mcarray + mcno;

	//	char* curname;
	//	curname = *( miptr->motname + curmce->id );

	//	if( m_overwritekind != OW_ALLNO ){

	//		char newquaname[MAX_PATH];
	//		int newqualeng;

	//		newqualeng = (int)strlen( srcdirname ) + (int)strlen( curname ) + 5;
	//		if( newqualeng >= MAX_PATH ){
	//			::MessageBox( m_hWnd, L"quaファイル名が長すぎて保存できません。\nモーション名を短くするか、もしくは、\nディレクトリの階層を浅く指定して、再試行してください。", L"失敗", MB_OK );
	//			return 1;//!!!!!!!!!!!
	//		}

	//		sprintf_s( newquaname, MAX_PATH, "%s\\%s.qua", srcdirname, curname );

	//		int endframe = *(miptr->motnum + curmce->id) - 1; //(framenum - 1)

	//		ret = WriteQuaFile( newquaname, curmce->id, 0, endframe, overwriteflag );
	//		if( ret ){
	//			DbgOut(L"motchangedlg : SaveMAFile : WriteQuaFile error !!!\n" );
	//			_ASSERT( 0 );
	//			return 1;
	//		}
	//	}

	//}

	return 0;
}


int CMotChangeDlg::WriteQuaFile( char* srcfilename, int srccookie, int srcstart, int srcend, int overwriteflag )
{

	_ASSERT(0);

	//if( (overwriteflag == 0) && (m_overwritekind != OW_ALLYES) && (m_overwritekind != OW_ALLNO) ){
	//	DWORD fattr;
	//	fattr = GetFileAttributes( srcfilename );
	//	if( (fattr & FILE_ATTRIBUTE_DIRECTORY) == 0 ){//ファイルが見つかった場合

	//		/***
	//		char messtr[1024];
	//		sprintf_s( messtr, 1024, "%sは、既に存在します。\n上書きしますか？", srcfilename );

	//		int dlgret;
	//		dlgret = (int)::MessageBox( m_hWnd, messtr, "上書き確認", MB_OKCANCEL );
	//		if( dlgret != IDOK ){
	//			return 0;//!!!!!!!!!!!!!!!!!!
	//		}
	//		***/

	//		CChkOWDlg dlg( srcfilename );
	//		dlg.DoModal();

	//		m_overwritekind = dlg.m_owkind;

	//	}
	//}
	//
	//if( (m_overwritekind == OW_YES) || (m_overwritekind == OW_ALLYES) ){
	//	int ret;	
	//	CQuaFile quafile;
	//	ret = quafile.SaveQuaFile( g_inidata->opt_namequa, m_papp->m_thandler, m_papp->m_shandler, m_papp->m_mhandler, srcfilename, srccookie, srcstart, srcend );
	//	if( ret ){
	//		DbgOut( "motchangedlg : WriteQuaFile quafile SaveQuaFile error !!!\n" );
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}

	return 0;

}

int CMotChangeDlg::AddChildMC( int parentcookie, MCELEM childmc )
{

	int ret;

	if( m_firstmake == 0 ){
		ret = InitMC();
		if( ret ){
			DbgOut( L"motchangedlg : AddChildMC : InitMC error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : AddChildMC : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}



	int parentsetno;
	parentsetno = mch->MotID2Setno( parentcookie );
	if( parentsetno < 0 ){
		DbgOut(L"motchangedlg : AddChildMC : mch MotID2Setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int always;
	if( childmc.frameno1 >= 0 ){
		always = 0;
	}else{
		always = 1;
	}

	ret = mch->AddChild( parentsetno, childmc.id, always, childmc.frameno1, childmc.frameno2, childmc.eventno1, childmc.notfu, childmc.nottoidle );
	if( ret ){
		DbgOut(L"motchangedlg : AddChildMC : mch AddChild error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	currentmodel->SetMotInfoLoopFlagByID(childmc.id, 0);



	///////////////////////////

	ret = FillTree();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = ParamsToDlg();
	_ASSERT( !ret );

	return 0;
}

int CMotChangeDlg::InitComboKey()
{
	m_combokey_wnd.SendMessage( CB_RESETCONTENT, 0, 0 );

	int kno;
	for( kno = 0; kno < 40; kno++ ){
		m_combokey_wnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)&(s_strkey[kno][0]));
	}
	m_combokey_wnd.SendMessage(CB_SETCURSEL, 0, 0);



	return 0;
}

int CMotChangeDlg::InitList()
{
	m_list_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : InitList : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}
	CEventKey* ekptr = currentmodel->GetEventKey();
	if (!ekptr) {
		_ASSERT(0);
		return 1;
	}

	LRESULT lres;
	int kno;
	for( kno = 0; kno < ekptr->GetKeyNum(); kno++ ){
		WCHAR mes[256];
		int eventno = ekptr->GetEventNo(kno);
		int combono = ekptr->GetComboNo(kno);
		if( (combono < 0) || (combono >= 40) ){
			DbgOut( L"mcdlg : InitList : combono out of range error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		WCHAR singlestr[256];
		if( ekptr->GetSingleEvent(kno) == 1 ){
			swprintf_s( singlestr, 256, L"単発：" );
		}else{
			swprintf_s( singlestr, 256, L"長押：" );
		}

		swprintf_s( mes, 256, L"%s eventno : %d, key : %s", singlestr, eventno, &( s_strkey[combono][0] ) );

		lres = m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)mes );
		if( (lres == LB_ERR) || (lres == LB_ERRSPACE) ){
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

LRESULT CMotChangeDlg::OnDefault10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnDefault10 : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}
	CEventKey* ekptr = currentmodel->GetEventKey();
	if (!ekptr) {
		_ASSERT(0);
		return 1;
	}

	int combono;
	for( combono = 4; combono <= 13; combono++ ){
		EKEY ek;
		ZeroMemory( &ek, sizeof( EKEY ) );
		ek.eventno = combono - 3;
		m_combono = combono;
		ek.combono = combono;
		ek.key = s_vkkey[combono];
		ek.singleevent = 1;


		if( ekptr->GetKeyNum() >= 255) {
			::MessageBox( m_hWnd, L"イベントキーは２５６個までしか作成できません。", L"エラー", MB_OK );
			return 0;
		}

		int kindex = -1;
		ekptr->CheckSameKey( ek.key, &kindex );
		if( kindex < 0 ){
			ret = ekptr->AddEKey( ek );	
			if( ret ){
				DbgOut( L"mcldg : OnDefault10: ek AddEKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			LRESULT lres;

			WCHAR singlestr[256];
			if( ek.singleevent == 1 ){
				swprintf_s( singlestr, 256, L"単発：" );
			}else{
				swprintf_s( singlestr, 256, L"長押：" );
			}

			WCHAR mes[256];
			swprintf_s( mes, 256, L"%s eventno : %d, key : %s", singlestr, ek.eventno, &( s_strkey[ek.combono][0] ) );
			lres = m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)mes );
			if( (lres == LB_ERR) || (lres == LB_ERRSPACE) ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


LRESULT CMotChangeDlg::OnAddList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnAddList : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}
	CEventKey* ekptr = currentmodel->GetEventKey();
	if (!ekptr) {
		_ASSERT(0);
		return 1;
	}

	int tmpint;
	EKEY ek;
	ZeroMemory( &ek, sizeof( EKEY ) );

	ret = GetInt( m_event_wnd, &tmpint );
	if( ret || (tmpint <= 0) ){
		::MessageBox( m_hWnd, L"イベント番号が不正です。０より大きい整数を書いてください。", L"入力エラー", MB_OK );
		return 0;
	}
	ek.eventno = tmpint;

	int combono;
	combono = (int)m_combokey_wnd.SendMessage( CB_GETCURSEL, 0, 0 );
	if( combono == CB_ERR ){
		_ASSERT( 0 );
		return 0;
	}
	if( (combono < 0) || (combono >= 40) ){
		_ASSERT( 0 );
		return 0;
	}
	m_combono = combono;
	ek.combono = combono;
	ek.key = s_vkkey[combono];

	UINT ischecked;
	ischecked = m_dlg_wnd.IsDlgButtonChecked( IDC_SINGLEEVENT );
	if( ischecked == BST_CHECKED ){
		ek.singleevent = 1;
	}else{
		ek.singleevent = 0;
	}

	if( ekptr->GetKeyNum() >= 255) {
		::MessageBox( m_hWnd, L"イベントキーは２５６個までしか作成できません。", L"エラー", MB_OK );
		return 0;
	}

	int kindex = -1;
	ekptr->CheckSameKey( ek.key, &kindex );
	
	int doadd = 1;

	if( kindex >= 0 ){
		int dlgret;
		dlgret = (int)::MessageBox( m_hWnd, L"同じキーが既に使われています。削除してから新規に作成しますか？", L"確認", MB_OKCANCEL );
		if( dlgret == IDOK ){

			LRESULT lret;
			lret = m_list_wnd.SendMessage( LB_DELETESTRING, (WPARAM)kindex, 0 );
			if( lret == LB_ERR ){
				_ASSERT( 0 );
				return 1;
			}

			ret = ekptr->DelEKeyByIndex( kindex );
			if( ret ){
				DbgOut( L"mcdlg : OnAddList : ek DelEKeyByIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			doadd = 0;
		}
	}

	if( doadd == 1 ){
		ret = ekptr->AddEKey( ek );	
		if( ret ){
			DbgOut( L"mcldg : OnAddList : ek AddEKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		LRESULT lres;

		WCHAR singlestr[256];
		if( ek.singleevent == 1 ){
			swprintf_s( singlestr, 256, L"単発：" );
		}else{
			swprintf_s( singlestr, 256, L"長押：" );
		}

		WCHAR mes[256];
		swprintf_s( mes, 256, L"%s eventno : %d, key : %s", singlestr, ek.eventno, &( s_strkey[ek.combono][0] ) );
		lres = m_list_wnd.SendMessage( LB_ADDSTRING, 0, (LPARAM)mes );
		if( (lres == LB_ERR) || (lres == LB_ERRSPACE) ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
LRESULT CMotChangeDlg::OnDelList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnDelList : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}
	CEventKey* ekptr = currentmodel->GetEventKey();
	if (!ekptr) {
		_ASSERT(0);
		return 1;
	}


	int selindex;
	selindex = (int)m_list_wnd.SendMessage( LB_GETCURSEL, 0, 0 );
	if( selindex == LB_ERR ){
		//何も選択していないときもLB_ERRが返る
		::MessageBox( m_hWnd, L"リストの要素を選択してから削除ボタンを押してください。", L"警告", MB_OK );
		return 0;
	}
	if( (selindex < 0) || (selindex >= 40) ){
		_ASSERT( 0 );
		return -1;
	}

	ret = (int)m_list_wnd.SendMessage( LB_DELETESTRING, (WPARAM)selindex, 0 );
	if( ret == LB_ERR ){
		_ASSERT( 0 );
		return 0;
	}

	ret = ekptr->DelEKeyByIndex( selindex );
	if( ret ){
		DbgOut( L"mcdlg : OnDelList : ek DelEKeyByIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
LRESULT CMotChangeDlg::OnAllDelList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (g_previewFlag != 0) {
		return 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnDelList : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}
	CEventKey* ekptr = currentmodel->GetEventKey();
	if (!ekptr) {
		_ASSERT(0);
		return 1;
	}


	m_list_wnd.SendMessage( LB_RESETCONTENT, 0, 0 );

	ret = ekptr->DelEKeyByIndex( -1 );
	if( ret ){
		DbgOut( L"mcdlg : OnAllDelete : ek DelEKeyByIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
LRESULT CMotChangeDlg::OnApplyFULeng(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	
	if (g_previewFlag != 0) {
		return 0;
	}

	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnApplyFULeng : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}

	int tmpint;
	ret = GetInt( m_fuleng_wnd, &tmpint );
	if( ret || (tmpint <= 0) ){
		::MessageBox( m_hWnd, L"イベント番号が不正です。０より大きい整数を書いてください。", L"入力エラー", MB_OK );
		return 0;
	}
	
	mch->SetFillUpLeng(tmpint);
	m_fuleng = tmpint;

	return 0;
}
LRESULT CMotChangeDlg::OnPlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(g_previewFlag != 0){
		return 0;
	}
	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnChangeIdle : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}


	CMAFile* mafile;
	mafile = new CMAFile();
	if( !mafile ){
		DbgOut( L"mcdlg : OnPlay : mafile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = mafile->CheckIdlingMotion( mch );
	if( ret ){
		::MessageBox( m_hWnd, L"アイドリング設定のモーションが、１つだけ存在する必要があります。\n設定を直して、再試行してください。", L"アイドリングエラー", MB_OK );
		delete mafile;
		return 0;
	}

	int idlingmotid = mch->GetIdlingMotID(nullptr, 0);
	if (idlingmotid > 0) {
		currentmodel->SetCurrentMotion(idlingmotid);
		currentmodel->SetUnderBlending(false);
		g_previewMOA = 1;
		g_previewFlag = 1;
	}


	//BOOL dummy;
	//g_motdlg->m_motparamdlg->m_mch = m_mch;
	//ret = g_motdlg->m_motparamdlg->OnPreview( 0, 0, 0, dummy );
	//if( ret ){
	//	_ASSERT( 0 );
	//	delete mafile;
	//	return 1;
	//}

	delete mafile;

	return 0;
}
LRESULT CMotChangeDlg::OnStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{

	//if( g_previewFlag == 0 ){
	//	return 0;
	//}

	g_previewFlag = 0;
	g_previewMOA = 0;

	//int ret;
	//BOOL dummy;
	//ret = g_motdlg->m_motparamdlg->OnStop( 0, 0, 0, dummy );
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	return 0;
}

int CMotChangeDlg::InitComboIdle()
{

	m_comboidle_wnd.SendMessage( CB_RESETCONTENT, 0, 0 );

	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}
	CMCHandler* mch = GetCurrentModel()->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum;
	motnum = currentmodel->GetMotInfoSize();

	int doneidle = 0;//すでにmchにidlingが設定されていれば１
	int initidling = -1;
	int tmpidling;
	tmpidling = mch->GetIdlingMotID(m_idlingname, MAX_PATH);
	if( tmpidling > 0 ){
		initidling = tmpidling;
		//strcpy_s( m_idlingname, MAX_PATH, (motidarray + initidling)->filename );
		doneidle = 1;
	}else{
		if( m_idlingname[0] != 0 ){

			MOTINFO idlingmi = currentmodel->GetMotInfoByNameMOA(m_idlingname);
			if (idlingmi.motid > 0) {
				initidling = idlingmi.motid;
			}

			//for( mno = 0; mno < motnum; mno++ ){
			//	int cmp0;
			//	MOTID* curmotid = motidarray + mno;
			//	cmp0 = strcmp( curmotid->filename, m_idlingname );
			//	if( cmp0 == 0 ){
			//		initidling = curmotid->id;
			//		break;
			//	}
			//}
		}
	}

	int ret;
	int setno = 0;
	int idlesetno = -1;
	int mno;
	WCHAR tmpmotname[MAX_PATH];
	for( mno = 0; mno < motnum; mno++ ){
		MOTINFO currentmi = currentmodel->GetMotInfoByIndex(mno);
		if(currentmi.motid > 0){
			//strcpy_s(tmpmotname, MAX_PATH, currentmi.motname);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, currentmi.motname, -1, tmpmotname, MAX_PATH);

			m_comboidle_wnd.SendMessage(CB_ADDSTRING, 0, (LPARAM)tmpmotname);
			m_comboidle_wnd.SendMessage(CB_SETITEMDATA, setno, (LPARAM)(DWORD)currentmi.motid);

			if(currentmi.motid == initidling){
				idlesetno = setno;
			}
			if((initidling < 0) && (setno == 0)){
				idlesetno = 0;
				initidling = currentmi.motid;
				strcpy_s(m_idlingname, MAX_PATH, currentmi.motname);
			}
			setno++;
		}
	}

	m_comboidle_wnd.SendMessage( CB_SETCURSEL, (WPARAM)idlesetno, 0 );


	if( (doneidle == 0) && (initidling >= 0) ){
		ret = currentmodel->ChangeIdlingMotion(initidling);
		//_ASSERT( !ret );
	}


	return initidling;
}



LRESULT CMotChangeDlg::OnChangeIdle(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int ret;
	int combono;
	combono = (int)m_comboidle_wnd.SendMessage( CB_GETCURSEL, 0, 0 );
	if( combono == CB_ERR ){
		_ASSERT( 0 );
		return 0;
	}

	int idlemotid;
	idlemotid = (int)m_comboidle_wnd.SendMessage( CB_GETITEMDATA, (WPARAM)combono, 0 );
	if( idlemotid < 0 ){
		_ASSERT( 0 );
		return 0;
	}


	if (!m_chascene) {
		_ASSERT(0);
		return 1;
	}
	CModel* currentmodel = GetCurrentModel();
	if (!currentmodel) {
		_ASSERT(0);
		return 1;
	}

	ret = currentmodel->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"motchangedlg : OnChangeIdle : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}

	CMCHandler* mch = currentmodel->GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}

	int motnum = currentmodel->GetMotInfoSize();
	if (motnum <= 0) {
		return 0;
	}

	MOTINFO currentmi = currentmodel->GetMotInfo(idlemotid);
	if (currentmi.motid > 0) {
		strcpy_s(m_idlingname, MAX_PATH, currentmi.motname);/// 

		ret = mch->ChangeIdlingMotion(currentmodel, idlemotid);
		_ASSERT(!ret);
	}

	return 0;
}


CModel* CMotChangeDlg::GetCurrentModel()
{
	if (m_chascene) {
		return m_chascene->GetCurrentModel();
	}
	else {
		return nullptr;
	}
}

bool CMotChangeDlg::GetFreezeOption()
{
	UINT ischecked;
	ischecked = m_dlg_wnd.IsDlgButtonChecked(IDC_FREEZE);
	if (ischecked == BST_CHECKED) {
		return true;
	}
	else {
		return false;
	}
}
