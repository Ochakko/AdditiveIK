#ifndef __CDollyHistoryDlg_H_
#define __CDollyHistoryDlg_H_

//#include "usealt.h"

#include <coef.h>
#include "StructHistory.h"

#include <atlbase.h>
#include <atlhost.h>



#include "resource.h"       // メイン シンボル

#include <vector>
#include <string>


#define DOLLYNUMFORDISP	10
//#define IMPORTANCEKINDNUM	7
//#define INPORTANCESTRLEN	32



/////////////////////////////////////////////////////////////////////////////
// CDollyHistoryDlg
class CDollyHistoryDlg : 
	public CAxDialogImpl<CDollyHistoryDlg>
{
public:
	CDollyHistoryDlg();
	~CDollyHistoryDlg();

	enum { IDD = IDD_DOLLYHISTORYDLG };

	void SetUpdateFunc(int (*UpdateFunc)());

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	int LoadDollyHistory(std::vector<DOLLYELEM>& vecdolly);


private:

BEGIN_MSG_MAP(CDollyHistoryDlg)
	//MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	//COMMAND_ID_HANDLER(IDOK, OnOK)
	//COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	//COMMAND_ID_HANDLER(IDC_CHECK1, OnCheckMostRecent)
	//COMMAND_ID_HANDLER(IDC_BUTTON3, OnSearch)
	COMMAND_ID_HANDLER(IDC_GETDOLLY, OnGetDolly)
	COMMAND_ID_HANDLER(IDC_SETDOLLY, OnSetDolly)
	COMMAND_ID_HANDLER(IDC_SAVEDOLLY, OnSaveDolly)
	COMMAND_ID_HANDLER(IDC_BUTTON6, OnDelete1)
	COMMAND_ID_HANDLER(IDC_BUTTON16, OnDelete2)
	COMMAND_ID_HANDLER(IDC_BUTTON17, OnDelete3)
	COMMAND_ID_HANDLER(IDC_BUTTON18, OnDelete4)
	COMMAND_ID_HANDLER(IDC_BUTTON19, OnDelete5)
	COMMAND_ID_HANDLER(IDC_BUTTON20, OnDelete6)
	COMMAND_ID_HANDLER(IDC_BUTTON21, OnDelete7)
	COMMAND_ID_HANDLER(IDC_BUTTON22, OnDelete8)
	COMMAND_ID_HANDLER(IDC_BUTTON23, OnDelete9)
	COMMAND_ID_HANDLER(IDC_BUTTON24, OnDelete10)
	COMMAND_ID_HANDLER(IDC_RADIO1, OnRadio1)
	COMMAND_ID_HANDLER(IDC_RADIO2, OnRadio2)
	COMMAND_ID_HANDLER(IDC_RADIO3, OnRadio3)
	COMMAND_ID_HANDLER(IDC_RADIO4, OnRadio4)
	COMMAND_ID_HANDLER(IDC_RADIO5, OnRadio5)
	COMMAND_ID_HANDLER(IDC_RADIO6, OnRadio6)
	COMMAND_ID_HANDLER(IDC_RADIO7, OnRadio7)
	COMMAND_ID_HANDLER(IDC_RADIO8, OnRadio8)
	COMMAND_ID_HANDLER(IDC_RADIO9, OnRadio9)
	COMMAND_ID_HANDLER(IDC_RADIO10, OnRadio10)
	//COMMAND_ID_HANDLER(IDC_CHECK1, OnChkRecent)
	COMMAND_ID_HANDLER(IDC_PREVPAGE, OnPrevPage)
	COMMAND_ID_HANDLER(IDC_NEXTPAGE, OnNextPage)
END_MSG_MAP()
// ハンドラのプロトタイプ:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	//LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnGetDolly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSetDolly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSaveDolly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnDelete1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete9(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelete10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnRadio1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio9(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadio10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);


	LRESULT OnPrevPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNextPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void InitParams();
	int DestroyObjs();

	int SetNames(std::vector<DOLLYELEM>& copyhistory);
	//int GetSelectedFileName(WCHAR* dstfilename);
	int ParamsToDlg();
	int EditParamsToDlg();

	LRESULT OnDelete(size_t delid);
	LRESULT OnRadio(size_t radioid);

	DOLLYELEM GetFirstValidElem();
	DOLLYELEM GetCheckedElem();

	int SetDollyElem2Camera(DOLLYELEM srcelem);

private:

/*
#define DOLLYNUMFORDISP	10
#define IMPORTANCEKINDNUM	7
#define INPORTANCESTRLEN	32
*/

	bool m_createdflag;
	CWindow m_dlg_wnd;

	size_t m_namenum;

	int m_pagenum;
	int m_currentpage;
	int m_startno;
	std::vector<DOLLYELEM> m_dollyhistory;

	UINT m_ctrlid[DOLLYNUMFORDISP];
	UINT m_targetid[DOLLYNUMFORDISP];
	UINT m_commentid[DOLLYNUMFORDISP];

	ChaVector3 m_camerapos;
	ChaVector3 m_targetpos;
	WCHAR m_comment[HISTORYCOMMENTLEN];


	int (*m_UpdateFunc)();

};

#endif //__ColiIDDlg_H_
