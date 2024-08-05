#ifndef __CDollyHistoryDlg2_H_
#define __CDollyHistoryDlg2_H_


#include <windows.h>

#include <coef.h>
#include "../../AdditiveIK/StructHistory.h"
#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>

class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;

class CDollyHistoryDlg2;


class CDollyHistoryOWPElem
{
public:
	CDollyHistoryOWPElem();
	~CDollyHistoryOWPElem();

	int SetDollyElem(
		int srcdollyelemindex, OrgWinGUI::OrgWindow* parwnd, OrgWinGUI::OWP_Separator* parentsp,
		DOLLYELEM2 srcdollyelem);
	int AddParts(OrgWinGUI::OWP_Separator* parentsp);
	int SetEventFunc(CDollyHistoryDlg2* srcdlg);

private:
	void InitParams();
	void DestroyObjs();

public:
	int GetDollyHistoryIndex() {
		return m_dollyelemindex;
	};
	OrgWinGUI::OWP_CheckBoxA* GetNameCheckBox() {
		return m_nameChk;
	};
	OrgWinGUI::OWP_Button* GetDelButton() {
		return m_deleteB;
	};
	OrgWinGUI::OWP_Label* GetDescLabel() {
		return m_descLabel;
	};
	OrgWinGUI::OWP_Label* GetMemoLabel() {
		return m_memoLabel;
	};
private:
	CDollyHistoryDlg2* m_parentdlg;
	int m_dollyelemindex;
	OrgWinGUI::OWP_Separator* m_namesp;
	OrgWinGUI::OWP_Separator* m_namesp2;
	OrgWinGUI::OWP_CheckBoxA* m_nameChk;
	OrgWinGUI::OWP_Button* m_pasteB;
	OrgWinGUI::OWP_Button* m_deleteB;
	OrgWinGUI::OWP_Label* m_descLabel;
	OrgWinGUI::OWP_Label* m_memoLabel;
	OrgWinGUI::OWP_Label* m_spacerLabel;

};

/////////////////////////////////////////////////////////////////////////////
// CDollyHistoryDlg2
class CDollyHistoryDlg2
{
public:
	CDollyHistoryDlg2();
	~CDollyHistoryDlg2();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!
	void SetUpdateFunc(int (*UpdateFunc)(), int (*PasteFunc)());

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	int LoadDollyHistory(std::vector<DOLLYELEM2>& vecdolly);
	int LoadDollyHistory_ver1(std::vector<DOLLYELEM2>& vecdolly);
	int LoadDollyHistory_ver2(std::vector<DOLLYELEM2>& vecdolly);
	int SetNames(std::vector<DOLLYELEM2>& copyhistory);

	int OnDelete(int delid);
	int OnRadio(int radioid, bool pasteflag);
	int OnSaveDolly();

	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();
private:

	int DestroyOWPWnd();
	int DestroyOWPElem();
	int CreateOWPWnd();


	int ParamsToDlg();
	int EditParamsToDlg();


	DOLLYELEM2 GetFirstValidElem();
	DOLLYELEM2 GetCheckedElem();

	int SetDollyElem2Camera(DOLLYELEM2 srcelem, bool pasteflag);

	int OnGetDolly();
	int OnSetDolly(bool pasteflag);


public:
	void SetOnShow(bool srcflag)
	{
		m_onshow = srcflag;
	}
	bool GetOnShow()
	{
		return m_onshow;
	}


private:
	bool m_createdflag;
	bool m_visible;
	//size_t m_namenum;//m_dollyhistory.size()を使う
	std::vector<DOLLYELEM2> m_dollyhistory;
	std::vector<CDollyHistoryOWPElem*> m_owpelemvec;

	ChaVector3 m_camerapos;
	ChaVector3 m_targetpos;
	WCHAR m_comment[HISTORYCOMMENTLEN];


	ChaVector3 m_upvec;
	//#########################################################################
	//m_upvecはエディットボックス編集しない
	//LoadDollyHistory_ver2()で読み込まれる
	//OnGet()と　OnRadio()から呼ばれるSetDollyElem2Camera()で取得した値をそのまま使う
	//#########################################################################


	int (*m_UpdateFunc)();
	int (*m_PasteFunc)();

	//2024/02/27
	//ShowWindow()を呼び出したときにOnRadio*()内でカメラが動いてしまわないようにフラグで回避
	//ユーザーは　保存したいカメラ位置を決めてからカメラドリーダイアログを出す場合がある　そのときにカメラが動いてしまうと困る
	//m_onshowのセットはAdditiveIK.cppのShowCameraDollyDlg()で行い　m_onshowのリセットは　OnPaintで行う
	bool m_onshow;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	OrgWinGUI::OrgWindow* m_dlgwnd;
	OrgWinGUI::OWP_ScrollWnd* m_dlgSc;
	OrgWinGUI::OWP_Separator* m_dlgspheader;
	OrgWinGUI::OWP_Separator* m_dlgsplist;

	OrgWinGUI::OWP_Separator* m_possp1;
	OrgWinGUI::OWP_Separator* m_possp2;
	OrgWinGUI::OWP_Separator* m_possp3;
	OrgWinGUI::OWP_Label* m_posLabel;
	OrgWinGUI::OWP_EditBox* m_posxEdit;
	OrgWinGUI::OWP_EditBox* m_posyEdit;
	OrgWinGUI::OWP_EditBox* m_poszEdit;

	OrgWinGUI::OWP_Separator* m_tarsp1;
	OrgWinGUI::OWP_Separator* m_tarsp2;
	OrgWinGUI::OWP_Separator* m_tarsp3;
	OrgWinGUI::OWP_Label* m_tarLabel;
	OrgWinGUI::OWP_EditBox* m_tarxEdit;
	OrgWinGUI::OWP_EditBox* m_taryEdit;
	OrgWinGUI::OWP_EditBox* m_tarzEdit;

	OrgWinGUI::OWP_Separator* m_memosp;
	OrgWinGUI::OWP_Label* m_memoLabel;
	OrgWinGUI::OWP_EditBox* m_memoEdit;

	OrgWinGUI::OWP_Separator* m_btnsp1;
	OrgWinGUI::OWP_Separator* m_btnsp2;
	OrgWinGUI::OWP_Separator* m_btnsp3;
	OrgWinGUI::OWP_Button* m_getB;
	OrgWinGUI::OWP_Button* m_setB;
	OrgWinGUI::OWP_Button* m_saveB;

	OrgWinGUI::OWP_Label* m_spacer1Label;
	OrgWinGUI::OWP_Label* m_spacer2Label;


};

#endif //__ColiIDDlg_H_
