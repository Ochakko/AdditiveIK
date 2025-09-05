// InfluenceDlg.h : CMotChangeDlg の宣言

#ifndef __MOTCHANGEDLG_H_
#define __MOTCHANGEDLG_H_

#include "../AdditiveIK/resource.h"       // メイン シンボル
#include <atlhost.h>

class CMCHandler;
class CMCRMenu;
class CEventKey;

class ChaScene;
class CModel;

#include <coef.h>

/////////////////////////////////////////////////////////////////////////////
// CMotChangeDlg
class CMotChangeDlg : 
	public CAxDialogImpl<CMotChangeDlg>
{
public:
	CMotChangeDlg();
	~CMotChangeDlg();

	void InitParams();
	void DestroyObjs();

	//int CreateLists();

	enum { IDD = IDD_MOTCHANGEDLG };

BEGIN_MSG_MAP(CMotChangeDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(ID_MCRMENU_ADD, OnAdd)
	COMMAND_ID_HANDLER(ID_MCRMENU_DEL, OnDel)
	COMMAND_ID_HANDLER(ID_MCRMENU_PROP, OnProp)
	COMMAND_ID_HANDLER(ID_MCRMENU_COPY, OnCopy)
	COMMAND_ID_HANDLER(ID_MCRMENU_PASTE, OnPaste)

	NOTIFY_HANDLER(IDC_TREE1, TVN_SELCHANGED, OnSelchangedTree1)
	NOTIFY_HANDLER(IDC_TREE1, TVN_ITEMEXPANDED, OnExpandedTree1)

	COMMAND_ID_HANDLER(IDC_ADD, OnAddList)
	COMMAND_ID_HANDLER(IDC_DEFAULT10, OnDefault10)
	COMMAND_ID_HANDLER(IDC_DEL, OnDelList)
	COMMAND_ID_HANDLER(IDC_ALLDEL, OnAllDelList)

	COMMAND_ID_HANDLER(IDC_ADD2, OnAddList2)
	COMMAND_ID_HANDLER(IDC_DEFAULTPAD, OnDefaultPAD)
	COMMAND_ID_HANDLER(IDC_DEL2, OnDelList2)
	COMMAND_ID_HANDLER(IDC_ALLDEL2, OnAllDelList2)

	COMMAND_ID_HANDLER(IDC_APPLY, OnApplyFULeng)
	COMMAND_ID_HANDLER(IDC_PLAY, OnPlay)
	COMMAND_ID_HANDLER(IDC_PLAYWITHBT, OnPlayWithBt)
	COMMAND_ID_HANDLER(IDC_STOP, OnStop)
	COMMAND_ID_HANDLER(IDC_COMBOIDLE, OnChangeIdle)
	

END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnProp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCopy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnSelchangedTree1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnExpandedTree1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnAddList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDefault10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAllDelList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnAddList2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDefaultPAD(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelList2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAllDelList2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnApplyFULeng(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPlayWithBt(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnChangeIdle(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);



public:
	int SetChaScene(ChaScene* srcchascene);
	int SetVisible(bool srcflag);
	bool GetVisible() {
		if (m_cmdshow != SW_HIDE) {
			return true;
		}
		else {
			return false;
		}
	};

	int DestroyMCElem( int delcookie );
	int InitMC();

	int AddParentMC( int addcookie, int srcidling, int srcev0idle, int srccommonid, int srcforbidnum, int* srcforbidid, int srcnotfu );
	int AddChildMC( int parentcookie, MCELEM childmc );


	int SaveMAFile( char* srcdirname, char* srcmafname, int overwriteflag );

	bool GetFreezeOption();
	bool GetPlusRandMoa();
private:
	int FillTree();
	//void AddBoneToTree( int srcseri, int addbroflag, int addtolast );
	int AddMCToTree( HTREEITEM hRoot );
	HTREEITEM TVAdd( HTREEITEM parentTI, char* srcname, int srcno, int imageno, int selectno, int addtolast );

	void CreateImageList();
	int ParamsToDlg();

	int OnUserSelchange( int selno );

	int WriteQuaFile( char* srcfilename, int srccookie, int srcstart, int srcend, int overwriteflag );

	int InitComboKey();
	int InitList();

	int InitComboKeyPad();
	int InitListPad();

	int InitComboIdle();

	CModel* GetCurrentModel();

private:
	int m_firstmake;
//	int m_idlingid;
	char m_idlingname[ MAX_PATH ];
	//CMCHandler* m_mch;

private:
	int m_cmdshow;
	CWindow m_dlg_wnd;
	CWindow m_tree_wnd;
	CWindow m_list_wnd;
	CWindow m_listpad_wnd;
	CWindow m_event_wnd;
	CWindow m_eventpad_wnd;
	CWindow m_combokey_wnd;
	CWindow m_combokeypad_wnd;
	CWindow m_comboidle_wnd;
	CWindow m_fuleng_wnd;

	int m_eventno;
	int m_combono;
	int m_singleevent;

	int m_eventnopad;
	int m_combonopad;
	int m_singleeventpad;

	int m_fuleng;

	HIMAGELIST m_hImageList;
	int m_iImage;
	int m_iSelect;

	HTREEITEM* m_TI;
	HTREEITEM m_selecteditem;
	int m_selectedno;
	
	//CMyD3DApplication* m_papp;
	//HWND	m_apphwnd;
	ChaScene* m_chascene;

	//int* m_serial2cookie;
	CMCRMenu* m_rmenu;

	int m_undertreeedit;

	int m_overwritekind;

	int m_cpelemnum;
	MCELEM m_cpelem[MAXMCCOPYNUM];

};

#endif //__MotChangeDlg_H_
