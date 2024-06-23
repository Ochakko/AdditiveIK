// InfluenceDlg.h : CFrameCopyDlg の宣言

#ifndef __FRAMECOPYDLG_H_
#define __FRAMECOPYDLG_H_

#include "resource.h"       // メイン シンボル
#include <atlbase.h>
#include <atlhost.h>

#include "dlgid.h"

#include <map>
#include <vector>

#define FRAMECOPYLISTLENG	1024

#define FCSLOTNUM			10
//2023/08/21 To12024 FCSLOTNUM2
#define FCSLOTNUM2			20

#define SLOTNAMELEN			32

class CModel;
class CBone;


/////////////////////////////////////////////////////////////////////////////
// CFrameCopyDlg
class CFrameCopyDlg : 
	public CAxDialogImpl<CFrameCopyDlg>
{
public:
	CFrameCopyDlg();
	~CFrameCopyDlg();

	int SetModel(CModel* srcmodel);
	int ExecuteOnOK();//OnOK

	int SaveWithProjectFile(WCHAR* srcfilename);
	bool LoadWithProjectFile(WCHAR* srcfilename);

	enum { IDD = IDD_FRAMECOPYDLG };

BEGIN_MSG_MAP(CFrameCopyDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	NOTIFY_HANDLER(IDN_TREE1, TVN_SELCHANGED, OnSelchangedTree1)
	COMMAND_ID_HANDLER(IDN_ADD, OnAdd)
	COMMAND_ID_HANDLER(IDN_DELETE, OnDelete)
	COMMAND_ID_HANDLER(IDN_ALLDELETE, OnAllDelete)

	COMMAND_ID_HANDLER(IDN_ADD2, OnAdd2)
	COMMAND_ID_HANDLER(IDN_DELETE2, OnDelete2)
	COMMAND_ID_HANDLER(IDN_ALLDELETE2, OnAllDelete2)

	COMMAND_ID_HANDLER(IDC_SLOTCOMBO, OnSelCombo)

	COMMAND_ID_HANDLER(IDC_LOADTBO, OnLoad)
	COMMAND_ID_HANDLER(IDC_SAVETBO, OnSave)

END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelchangedTree1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAllDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnAdd2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAllDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnSelCombo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTimer(UINT, WPARAM, LPARAM, BOOL&);

	LRESULT OnLoad(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	int InitParams();
	int DestroyObjs();
	int DestroyDlgCtrls();
	int SetupDlg(CModel* srcmodel);


	int FillTree();
	void AddBoneToTree( CBone* srcbone, int addbroflag, int addtolast );
	HTREEITEM TVAdd( HTREEITEM parentTI, WCHAR* srcname, int srcno, int imageno, int selectno, int addtolast );

	void CreateImageList();

	int SetTree2ListReq( int validorinvalid, int srcno, int addbroflag );

	int CreateCombo();

	int WriteTBOFile();
	int WriteTBOFile(WCHAR* srcfilename);
	int ValidateTBOFile(char* dstTBOheader, char* srcbuf, DWORD bufleng);
	bool LoadTBOFile();
	bool LoadTBOFile(WCHAR* srcfilename);



public:
	int ParamsToDlg();

	std::vector<CBone*> GetCpVec()
	{
		return m_cpvec;
	}

	void StartTimer() {
		if (!m_inittimerflag) {
			SetTimer(m_timerid, 20);
			m_inittimerflag = true;
		}
	};
	void EndTimer() {
		if (m_inittimerflag) {
			KillTimer(m_timerid);
			m_inittimerflag = false;
		}
	};

private:
	int m_samemodelflag;
	bool m_inittimerflag;
	int m_timerid;
	bool m_tboloadedflag;

	WCHAR m_tmpmqopath[MAX_PATH];

	CWindow m_dlg_wnd;
	CWindow m_tree_wnd;
	CWindow m_list_wnd;
	CWindow m_list2_wnd;
	CWindow m_combo_wnd;
	CWindow m_slotname_wnd;

	HIMAGELIST m_hImageList;
	int m_iImage;
	int m_iSelect;

	//HTREEITEM* m_TI;
	std::map<int, HTREEITEM> m_timap;
	HTREEITEM m_selecteditem;
	int m_selectedno;
	
	CModel* m_model;

	HTREEITEM m_hrootti;

	//bool m_pastecamera;

public:
// ユーザーが指定したtreeの先頭番号を格納する。
	int m_slotno;

	WCHAR m_slotname[FCSLOTNUM2][SLOTNAMELEN];

	int m_influencenum[FCSLOTNUM2];
	int m_influencelist[FCSLOTNUM2][FRAMECOPYLISTLENG];

	int m_ignorenum[FCSLOTNUM2];
	int m_ignorelist[FCSLOTNUM2][FRAMECOPYLISTLENG];

// ユーザーが指定したtree全体を格納（子供を含む。）
	std::map<int, CBone*> m_validelemmap;
	std::map<int, CBone*> m_invalidelemmap;
	std::vector<CBone*> m_cpvec;

};

#endif //__FRAMECOPYDLG_H_
