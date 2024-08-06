#include "stdafx.h"
#include <CopyHistoryDlg2.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <OrgWindow.h>
#include <GlobalVar.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>

using namespace std;
using namespace OrgWinGUI;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル



CCpHistoryOWPElem::CCpHistoryOWPElem()
{
	InitParams();
}
CCpHistoryOWPElem::~CCpHistoryOWPElem()
{
	DestroyObjs();
}
int CCpHistoryOWPElem::SetHistoryElem(
	int srccopyhistoryindex,
	OrgWinGUI::OrgWindow* parwnd, OrgWinGUI::OWP_Separator* parentsp,
	HISTORYELEM srchistory)
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

	m_copyhistoryindex = srccopyhistoryindex;

	m_namesp = new OWP_Separator(parwnd, true, 0.75, true);
	if (!m_namesp) {
		_ASSERT(0);
		return 1;
	}
	//parentsp->addParts1(*m_namesp);

	WCHAR dispname[MAX_PATH] = { 0L };
	wcscpy_s(dispname, MAX_PATH, srchistory.cpinfo.fbxname);
	wcscat_s(dispname, MAX_PATH, L"_");
	wcscat_s(dispname, MAX_PATH, srchistory.cpinfo.motionname);
	m_nameChk = new OWP_CheckBoxA(dispname, false, labelheight, true, MAX_PATH);
	if (!m_nameChk) {
		_ASSERT(0);
		return 1;
	}
	//m_namesp->addParts1(*m_nameChk);

	m_deleteB = new OWP_Button(L"delete", labelheight);
	if (!m_deleteB) {
		_ASSERT(0);
		return 1;
	}
	//m_namesp->addParts2(*m_deleteB);


	WCHAR strdesc[MAX_PATH] = { 0L };
	if (srchistory.hascpinfo == 1) {
		WCHAR textstartframe[MAX_PATH] = { 0L };
		swprintf_s(textstartframe, MAX_PATH, L"%.0lf", srchistory.cpinfo.startframe);
		WCHAR textframenum[MAX_PATH] = { 0L };
		swprintf_s(textframenum, MAX_PATH, L"%.0lf", srchistory.cpinfo.framenum);
		int curbvhtype = srchistory.cpinfo.bvhtype;
		WCHAR textbvhtype[MAX_PATH] = { 0L };
		if ((curbvhtype >= 1) && (curbvhtype <= 144)) {
			swprintf_s(textbvhtype, MAX_PATH, L"bvh_%03d", srchistory.cpinfo.bvhtype);
		}
		else {
			swprintf_s(textbvhtype, MAX_PATH, L"bvh_Undef");
		}
		int curimportance = srchistory.cpinfo.importance;
		WCHAR textimportance[INPORTANCESTRLEN] = { 0L };
		if ((curimportance >= 0) && (curimportance < IMPORTANCEKINDNUM)) {
			swprintf_s(textimportance, INPORTANCESTRLEN, m_strimportance[curimportance]);
		}
		else {
			swprintf_s(textimportance, INPORTANCESTRLEN, L"Undef.");
		}

		swprintf_s(strdesc, MAX_PATH, L"StartFrame:%s Num:%s  %s %s",
			textstartframe, textframenum, textbvhtype, textimportance);
	}
	else {
		wcscpy_s(strdesc, MAX_PATH, L"Invalid.");
	}
	m_descLabel = new OWP_Label(strdesc, labelheight, MAX_PATH);
	if (!m_descLabel) {
		_ASSERT(0);
		return 1;
	}
	//parentsp->addParts1(*m_descLabel);

	WCHAR strmemo[HISTORYCOMMENTLEN] = { 0L };
	if (srchistory.hascpinfo == 1) {
		srchistory.cpinfo.comment[HISTORYCOMMENTLEN - 1] = 0L;
		if (srchistory.cpinfo.comment[0] != 0L) {
			wcscpy_s(strmemo, HISTORYCOMMENTLEN, srchistory.cpinfo.comment);
		}
		else {
			wcscpy_s(strmemo, HISTORYCOMMENTLEN, L" ");
		}
	}
	else {
		wcscpy_s(strmemo, HISTORYCOMMENTLEN, L"Invalid.");
	}
	m_memoLabel = new OWP_Label(strmemo, labelheight);
	if (!m_memoLabel) {
		_ASSERT(0);
		return 1;
	}
	//parentsp->addParts1(*m_memoLabel);

	m_spacerLabel = new OWP_Label(L"     ", labelheight);
	if (!m_spacerLabel) {
		_ASSERT(0);
		return 1;
	}
	//parentsp->addParts1(*m_spacerLabel);


	return 0;
}
int CCpHistoryOWPElem::AddParts(OrgWinGUI::OWP_Separator* parentsp)
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
	if (m_nameChk) {
		m_namesp->addParts1(*m_nameChk);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	if (m_deleteB) {
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
int CCpHistoryOWPElem::SetEventFunc(CCopyHistoryDlg2* srcdlg)
{
	m_parentdlg = srcdlg;

	if (m_nameChk) {
		m_nameChk->setButtonListener([=, this]() {
			if (m_parentdlg) {
				m_parentdlg->OnRadio(m_copyhistoryindex);
			}
		});
	}
	if (m_deleteB) {
		m_deleteB->setButtonListener([=, this]() {
			if (m_parentdlg) {
				m_parentdlg->OnDelete(m_copyhistoryindex);
			}
		});
	}

	return 0;
}

void CCpHistoryOWPElem::InitParams()
{
	m_copyhistoryindex = -1;

	m_namesp = nullptr;
	m_nameChk = nullptr;
	m_deleteB = nullptr;
	m_descLabel = nullptr;
	m_memoLabel = nullptr;
	m_spacerLabel = nullptr;

	ZeroMemory(m_strimportance, sizeof(WCHAR) * INPORTANCESTRLEN * 7);
	wcscpy_s(m_strimportance[0], INPORTANCESTRLEN, L"Undef.");
	wcscpy_s(m_strimportance[1], INPORTANCESTRLEN, L"Tiny.");
	wcscpy_s(m_strimportance[2], INPORTANCESTRLEN, L"ALittle.");
	wcscpy_s(m_strimportance[3], INPORTANCESTRLEN, L"Normal.");
	wcscpy_s(m_strimportance[4], INPORTANCESTRLEN, L"Noticed.");
	wcscpy_s(m_strimportance[5], INPORTANCESTRLEN, L"Important.");
	wcscpy_s(m_strimportance[6], INPORTANCESTRLEN, L"VeryImportant.");

	//m_SelChangeFunc = nullptr;
	//m_DeleteFunc = nullptr;

	m_parentdlg = nullptr;
}
void CCpHistoryOWPElem::DestroyObjs()
{
	if (m_namesp) {
		delete m_namesp;
		m_namesp = nullptr;
	}
	if (m_nameChk) {
		delete m_nameChk;
		m_nameChk = nullptr;
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
// CCopyHistoryDlg2

CCopyHistoryDlg2::CCopyHistoryDlg2()
{
	InitParams();
}

int CCopyHistoryDlg2::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}


CCopyHistoryDlg2::~CCopyHistoryDlg2()
{
	DestroyObjs();
}
	
int CCopyHistoryDlg2::DestroyObjs()
{
	DestroyOWPWnd();
	InitParams();

	return 0;
}

int CCopyHistoryDlg2::DestroyOWPWnd()
{

	if (m_dlgwnd) {
		m_dlgwnd->setVisible(false);
		m_dlgwnd->setListenMouse(false);
	}

	
	DestroyOWPElem();


	if (m_dlgspall) {
		delete m_dlgspall;
		m_dlgspall = 0;
	}
	if (m_dlgspcombo) {
		delete m_dlgspcombo;
		m_dlgspcombo = 0;
	}
	if (m_dlgsplist) {
		delete m_dlgsplist;
		m_dlgsplist = 0;
	}
	if (m_combosp1) {
		delete m_combosp1;
		m_combosp1 = 0;
	}
	if (m_combosp2) {
		delete m_combosp2;
		m_combosp2 = 0;
	}
	if (m_combosp3) {
		delete m_combosp3;
		m_combosp3 = 0;
	}
	if (m_Combo1) {
		delete m_Combo1;
		m_Combo1 = 0;
	}
	if (m_Combo2) {
		delete m_Combo2;
		m_Combo2 = 0;
	}
	if (m_Combo3) {
		delete m_Combo3;
		m_Combo3 = 0;
	}
	if (m_fromsp1) {
		delete m_fromsp1;
		m_fromsp1 = 0;
	}
	if (m_fromsp2) {
		delete m_fromsp2;
		m_fromsp2 = 0;
	}
	if (m_fromsp3) {
		delete m_fromsp3;
		m_fromsp3 = 0;
	}
	if (m_fromLabel) {
		delete m_fromLabel;
		m_fromLabel = 0;
	}
	if (m_fromComboYear) {
		delete m_fromComboYear;
		m_fromComboYear = 0;
	}
	if (m_fromComboMonth) {
		delete m_fromComboMonth;
		m_fromComboMonth = 0;
	}
	if (m_fromComboDay) {
		delete m_fromComboDay;
		m_fromComboDay = 0;
	}
	if (m_tosp1) {
		delete m_tosp1;
		m_tosp1 = 0;
	}
	if (m_tosp2) {
		delete m_tosp2;
		m_tosp2 = 0;
	}
	if (m_tosp3) {
		delete m_tosp3;
		m_tosp3 = 0;
	}
	if (m_toLabel) {
		delete m_toLabel;
		m_toLabel = 0;
	}
	if (m_toComboYear) {
		delete m_toComboYear;
		m_toComboYear = 0;
	}
	if (m_toComboMonth) {
		delete m_toComboMonth;
		m_toComboMonth = 0;
	}
	if (m_toComboDay) {
		delete m_toComboDay;
		m_toComboDay = 0;
	}
	if (m_searchB) {
		delete m_searchB;
		m_searchB = 0;
	}
	if (m_spacerLabel1) {
		delete m_spacerLabel1;
		m_spacerLabel1 = 0;
	}
	if (m_recentChk) {
		delete m_recentChk;
		m_recentChk = 0;
	}
	if (m_spacerLabel2) {
		delete m_spacerLabel2;
		m_spacerLabel2 = 0;
	}


	if (m_dlgSc) {
		delete m_dlgSc;
		m_dlgSc = 0;
	}
	if (m_dlgwnd) {
		delete m_dlgwnd;
		m_dlgwnd = 0;
	}

	return 0;
}

int CCopyHistoryDlg2::DestroyOWPElem()
{
	size_t elemnum = m_owpelemvec.size();
	size_t elemindex;
	for (elemindex = 0; elemindex < elemnum; elemindex++) {
		CCpHistoryOWPElem* curowp = m_owpelemvec[elemindex];
		if (curowp) {
			delete curowp;
		}
	}
	m_owpelemvec.clear();

	return 0;
}

void CCopyHistoryDlg2::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = 0;
	m_ischeckedmostrecent = true;
	m_selectnamemap.clear();
	m_selectindexmap.clear();

	//m_namenum = 0;
	//m_selectedindex = -1;
	m_copyhistory.clear();
	m_savecopyhistory.clear();

	m_strcombo_fbxname.clear();
	m_strcombo_motionname.clear();
	m_strcombo_bvhtype.clear();

	m_initsearchcomboflag = false;

	m_owpelemvec.clear();


	m_dlgwnd = 0;
	m_dlgSc = 0;
	m_dlgspall = 0;
	m_dlgspcombo = 0;
	m_dlgsplist = 0;
	m_combosp1 = 0;
	m_combosp2 = 0;
	m_combosp3 = 0;
	m_Combo1 = 0;
	m_Combo2 = 0;
	m_Combo3 = 0;
	m_fromsp1 = 0;
	m_fromsp2 = 0;
	m_fromsp3 = 0;
	m_fromLabel = 0;
	m_fromComboYear = 0;
	m_fromComboMonth = 0;
	m_fromComboDay = 0;
	m_tosp1 = 0;
	m_tosp2 = 0;
	m_tosp3 = 0;
	m_toLabel = 0;
	m_toComboYear = 0;
	m_toComboMonth = 0;
	m_toComboDay = 0;
	m_searchB = 0;
	m_spacerLabel1 = 0;
	m_recentChk = 0;
	m_spacerLabel2 = 0;


}

void CCopyHistoryDlg2::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgwnd) {
			m_dlgwnd->setListenMouse(true);
			m_dlgwnd->setVisible(true);
			if (m_dlgSc && m_dlgspcombo) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				//m_dlgSc->autoResize();

				//選択項目が画面内に入るようにスクロール処理をすることにした
				int selectindex = m_selectindexmap[m_model];
				int showposline = selectindex * 4 + m_dlgspcombo->getPartsNum1();
				//+getPartsNum1()について 
				//今回の場合 親ウインドウの直接の子供はm_dlgspcomboとm_dlgsplist(m_dlgSc)の上下２段組で下段だけスクロールする
				//m_dlgsplist(下段)のshowPosLineの制御をする
				//スクロールウインドウのinView()は表示行数計算時に親ウインドウのsize.y/LABEL_SIZE_Yを使う
				//inView()の１行目の位置は、m_dlgspcombo(上段)の１行目の位置になっている
				//m_dlgsplistの１行目からの計算にするためにm_dlgspcomboの全行数を足す
				m_dlgSc->inView(showposline);//内部でautoResize()も呼ぶ
			}
			m_dlgwnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgwnd) {
			m_dlgwnd->setVisible(false);
			m_dlgwnd->setListenMouse(false);

			m_initsearchcomboflag = false;//!!!!!!!!!!
		}
	}
	m_visible = srcflag;
}

int CCopyHistoryDlg2::CreateOWPWnd(CModel* srcmodel)
{
	DestroyOWPWnd();

	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	WCHAR modelname[MAX_PATH] = { 0L };
	const WCHAR* pname = srcmodel->GetFileName();
	if (!pname) {
		_ASSERT(0);
		return 1;
	}
	wcscpy_s(modelname, MAX_PATH, pname);

	m_strcombo_motionname.clear();
	m_strcombo_bvhtype.clear();

	size_t srcnum = m_savecopyhistory.size();//検索候補はsavecopyhistoryから取得する
	size_t srcno;
	for (srcno = 0; srcno < srcnum; srcno++) {
		if (m_savecopyhistory[srcno].hascpinfo == 1) {
			wstring curmotionname = m_savecopyhistory[srcno].cpinfo.motionname;
			bool foundmotionname = false;
			std::vector<std::wstring>::iterator itrmotionname;
			for (itrmotionname = m_strcombo_motionname.begin(); itrmotionname != m_strcombo_motionname.end(); itrmotionname++) {
				if (curmotionname == *itrmotionname) {
					foundmotionname = true;
					break;
				}
			}
			if (foundmotionname == false) {
				m_strcombo_motionname.push_back(curmotionname);
			}

			int curbvhtype = m_savecopyhistory[srcno].cpinfo.bvhtype;
			bool foundbvhtype = false;
			std::vector<int>::iterator itrbvhtype;
			for (itrbvhtype = m_strcombo_bvhtype.begin(); itrbvhtype != m_strcombo_bvhtype.end(); itrbvhtype++) {
				if (curbvhtype == *itrbvhtype) {
					foundbvhtype = true;
					break;
				}
			}
			if (foundbvhtype == false) {
				m_strcombo_bvhtype.push_back(curbvhtype);
			}
		}
	}


	m_dlgwnd = new OrgWindow(
		0,
		_T("CopyHistoryDlg"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("CopyHistoryDlg"),	//タイトル
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

		m_dlgspcombo = new OWP_Separator(m_dlgwnd, true, 0.995, false);
		if (!m_dlgspcombo) {
			_ASSERT(0);
			abort();
		}
		m_dlgwnd->addParts(*m_dlgspcombo);

		m_dlgSc = new OWP_ScrollWnd(L"CopyHistoryScroll", true, labelheight);
		if (!m_dlgSc) {
			_ASSERT(0);
			abort();
		}
		int linedatanum = (int)((double)m_copyhistory.size() * 4.0 * 1.25);
		m_dlgSc->setLineDataSize(linedatanum);
		m_dlgwnd->addParts(*m_dlgSc);

		//m_dlgspall = new OWP_Separator(m_dlgwnd, true, 0.050, false);
		//if (!m_dlgspall) {
		//	_ASSERT(0);
		//	abort();
		//}
		//m_dlgSc->addParts(*m_dlgspall);


		m_dlgsplist = new OWP_Separator(m_dlgwnd, true, 0.995, false, m_dlgSc);
		if (!m_dlgsplist) {
			_ASSERT(0);
			abort();
		}
		m_dlgSc->addParts(*m_dlgsplist);


		m_combosp1 = new OWP_Separator(m_dlgwnd, true, 0.6667, true);
		if (!m_combosp1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspcombo->addParts1(*m_combosp1);

		m_combosp2 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_combosp2) {
			_ASSERT(0);
			abort();
		}
		m_combosp1->addParts1(*m_combosp2);

		m_combosp3 = new OWP_Separator(m_dlgwnd, true, 0.9995, true);
		if (!m_combosp3) {
			_ASSERT(0);
			abort();
		}
		m_combosp1->addParts2(*m_combosp3);

		m_Combo1 = new OWP_ComboBoxA(L"CpHistoryCombo1", labelheight);
		if (!m_Combo1) {
			_ASSERT(0);
			abort();
		}
		m_Combo1->addString("----");//index == 0 は　"----"
		size_t fbxnamenum = m_strcombo_fbxname.size();
		size_t fbxnameno;
		size_t curmodelindex = 0;
		for (fbxnameno = 0; fbxnameno < fbxnamenum; fbxnameno++) {
			WCHAR tempchar[MAX_PATH];
			ZeroMemory(tempchar, sizeof(WCHAR) * MAX_PATH);
			wcscpy_s(tempchar, MAX_PATH, m_strcombo_fbxname[fbxnameno].c_str());
			char strcombo1[MAX_PATH] = { 0L };
			WideCharToMultiByte(CP_ACP, 0, tempchar, -1, strcombo1, MAX_PATH, NULL, NULL);
			m_Combo1->addString(strcombo1);
			if (wcscmp(tempchar, modelname) == 0) {
				curmodelindex = fbxnameno;//!!!!!!!!!!!!!!!!
			}
		}
		m_Combo1->setSelectedCombo((int)curmodelindex + 1);//index == 0 は　"----"
		m_combosp2->addParts1(*m_Combo1);


		m_Combo2 = new OWP_ComboBoxA(L"CpHistoryCombo2", labelheight);
		if (!m_Combo2) {
			_ASSERT(0);
			abort();
		}
		m_Combo2->addString("----");//index == 0 は　"----"
		size_t motionnamenum = m_strcombo_motionname.size();
		size_t motionnameno;
		for (motionnameno = 0; motionnameno < motionnamenum; motionnameno++) {
			WCHAR tempchar[MAX_PATH];
			ZeroMemory(tempchar, sizeof(WCHAR) * MAX_PATH);
			wcscpy_s(tempchar, MAX_PATH, m_strcombo_motionname[motionnameno].c_str());
			char strcombo2[MAX_PATH] = { 0L };
			WideCharToMultiByte(CP_ACP, 0, tempchar, -1, strcombo2, MAX_PATH, NULL, NULL);
			m_Combo2->addString(strcombo2);
		}
		m_Combo2->setSelectedCombo(0);//index == 0 は　"----"
		m_combosp2->addParts2(*m_Combo2);



		m_Combo3 = new OWP_ComboBoxA(L"CpHistoryCombo3", labelheight);
		if (!m_Combo3) {
			_ASSERT(0);
			abort();
		}
		m_Combo3->addString("----");//index == 0 は　"----"
		size_t bvhtypenum = m_strcombo_bvhtype.size();
		size_t bvhtypeno;
		for (bvhtypeno = 0; bvhtypeno < bvhtypenum; bvhtypeno++) {
			WCHAR tempchar[MAX_PATH];
			ZeroMemory(tempchar, sizeof(WCHAR) * MAX_PATH);
			swprintf_s(tempchar, MAX_PATH, L"bvh_%03d", m_strcombo_bvhtype[bvhtypeno]);
			char strcombo3[MAX_PATH] = { 0L };
			WideCharToMultiByte(CP_ACP, 0, tempchar, -1, strcombo3, MAX_PATH, NULL, NULL);
			m_Combo3->addString(strcombo3);
		}
		m_Combo3->setSelectedCombo(0);//index == 0 は　"----"
		m_combosp3->addParts1(*m_Combo3);



		m_fromsp1 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_fromsp1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspcombo->addParts1(*m_fromsp1);

		m_fromsp2 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_fromsp2) {
			_ASSERT(0);
			abort();
		}
		m_fromsp1->addParts1(*m_fromsp2);

		m_fromsp3 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_fromsp3) {
			_ASSERT(0);
			abort();
		}
		m_fromsp1->addParts2(*m_fromsp3);

		m_fromLabel = new OWP_Label(L"From", labelheight);
		if (!m_fromLabel) {
			_ASSERT(0);
			abort();
		}
		m_fromsp2->addParts1(*m_fromLabel);

		m_fromComboYear = new OWP_ComboBoxA(L"FromComboYear", labelheight);
		if (!m_fromComboYear) {
			_ASSERT(0);
			abort();
		}
		{
			m_fromComboYear->addString("----");
			int comboyearnum = 4;
			int comboyearno;
			for (comboyearno = 0; comboyearno < comboyearnum; comboyearno++) {
				int curyear = 2021 + comboyearno;
				char stryear[MAX_PATH];
				ZeroMemory(stryear, sizeof(char) * MAX_PATH);
				sprintf_s(stryear, MAX_PATH, "%d", curyear);
				m_fromComboYear->addString(stryear);
			}
			m_fromComboYear->setSelectedCombo(0);//index == 0 は　"----"
		}
		m_fromsp2->addParts2(*m_fromComboYear);

		m_fromComboMonth = new OWP_ComboBoxA(L"FromComboMonth", labelheight);
		if (!m_fromComboMonth) {
			_ASSERT(0);
			abort();
		}
		{
			m_fromComboMonth->addString("----");
			int comboMonthnum = 12;
			int comboMonthno;
			for (comboMonthno = 0; comboMonthno < comboMonthnum; comboMonthno++) {
				int curMonth = comboMonthno + 1;
				char strMonth[MAX_PATH];
				ZeroMemory(strMonth, sizeof(char) * MAX_PATH);
				sprintf_s(strMonth, MAX_PATH, "%d", curMonth);
				m_fromComboMonth->addString(strMonth);
			}
			m_fromComboMonth->setSelectedCombo(0);//index == 0 は　"----"
		}
		m_fromsp3->addParts1(*m_fromComboMonth);

		m_fromComboDay = new OWP_ComboBoxA(L"FromComboDay", labelheight);;
		if (!m_fromComboDay) {
			_ASSERT(0);
			abort();
		}
		{
			m_fromComboDay->addString("----");
			int comboDaynum = 31;
			int comboDayno;
			for (comboDayno = 0; comboDayno < comboDaynum; comboDayno++) {
				int curDay = comboDayno + 1;
				char strDay[MAX_PATH];
				ZeroMemory(strDay, sizeof(char) * MAX_PATH);
				sprintf_s(strDay, MAX_PATH, "%d", curDay);
				m_fromComboDay->addString(strDay);
			}
			m_fromComboDay->setSelectedCombo(0);//index == 0 は　"----"
		}
		m_fromsp3->addParts2(*m_fromComboDay);

		m_tosp1 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_tosp1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspcombo->addParts1(*m_tosp1);

		m_tosp2 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_tosp2) {
			_ASSERT(0);
			abort();
		}
		m_tosp1->addParts1(*m_tosp2);

		m_tosp3 = new OWP_Separator(m_dlgwnd, true, rate50, true);
		if (!m_tosp3) {
			_ASSERT(0);
			abort();
		}
		m_tosp1->addParts2(*m_tosp3);

		m_toLabel = new OWP_Label(L"To", labelheight);
		if (!m_toLabel) {
			_ASSERT(0);
			abort();
		}
		m_tosp2->addParts1(*m_toLabel);

		m_toComboYear = new OWP_ComboBoxA(L"ToComboYear", labelheight);
		if (!m_toComboYear) {
			_ASSERT(0);
			abort();
		}
		{
			m_toComboYear->addString("----");
			int comboyearnum = 4;
			int comboyearno;
			for (comboyearno = 0; comboyearno < comboyearnum; comboyearno++) {
				int curyear = 2021 + comboyearno;
				char stryear[MAX_PATH];
				ZeroMemory(stryear, sizeof(char) * MAX_PATH);
				sprintf_s(stryear, MAX_PATH, "%d", curyear);
				m_toComboYear->addString(stryear);
			}
			m_toComboYear->setSelectedCombo(0);//index == 0 は　"----"
		}
		m_tosp2->addParts2(*m_toComboYear);

		m_toComboMonth = new OWP_ComboBoxA(L"ToComboMonth", labelheight);
		if (!m_toComboMonth) {
			_ASSERT(0);
			abort();
		}
		{
			m_toComboMonth->addString("----");
			int comboMonthnum = 12;
			int comboMonthno;
			for (comboMonthno = 0; comboMonthno < comboMonthnum; comboMonthno++) {
				int curMonth = comboMonthno + 1;
				char strMonth[MAX_PATH];
				ZeroMemory(strMonth, sizeof(char) * MAX_PATH);
				sprintf_s(strMonth, MAX_PATH, "%d", curMonth);
				m_toComboMonth->addString(strMonth);
			}
			m_toComboMonth->setSelectedCombo(0);//index == 0 は　"----"
		}
		m_tosp3->addParts1(*m_toComboMonth);

		m_toComboDay = new OWP_ComboBoxA(L"ToComboDay", labelheight);
		if (!m_toComboDay) {
			_ASSERT(0);
			abort();
		}
		{
			m_toComboDay->addString("----");
			int comboDaynum = 31;
			int comboDayno;
			for (comboDayno = 0; comboDayno < comboDaynum; comboDayno++) {
				int curDay = comboDayno + 1;
				char strDay[MAX_PATH];
				ZeroMemory(strDay, sizeof(char) * MAX_PATH);
				sprintf_s(strDay, MAX_PATH, "%d", curDay);
				m_toComboDay->addString(strDay);
			}
			m_toComboDay->setSelectedCombo(0);//index == 0 は　"----"
		}
		m_tosp3->addParts2(*m_toComboDay);

		m_searchB = new OWP_Button(L"検索実行(Search)", labelheight);
		if (!m_searchB) {
			_ASSERT(0);
			abort();
		}
		m_dlgspcombo->addParts1(*m_searchB);

		m_spacerLabel1 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerLabel1) {
			_ASSERT(0);
			abort();
		}
		m_dlgspcombo->addParts1(*m_spacerLabel1);

		m_recentChk = new OWP_CheckBoxA(L"最新を使用 UseMostRecent", m_ischeckedmostrecent, labelheight, false);
		if (!m_recentChk) {
			_ASSERT(0);
			abort();
		}
		m_recentChk->setTextColor(RGB(255, 129, 129));
		m_dlgspcombo->addParts1(*m_recentChk);

		m_spacerLabel2 = new OWP_Label(L"     ", labelheight);
		if (!m_spacerLabel2) {
			_ASSERT(0);
			abort();
		}
		m_dlgspcombo->addParts1(*m_spacerLabel2);


		size_t nameno;
		size_t namenum = m_copyhistory.size();
		for (nameno = 0; nameno < namenum; nameno++) {
			HISTORYELEM curhistory = m_copyhistory[nameno];
			CCpHistoryOWPElem* newowp = new CCpHistoryOWPElem();
			if (!newowp) {
				_ASSERT(0);
				abort();
			}
			int result1 = newowp->SetHistoryElem((int)nameno, m_dlgwnd, m_dlgsplist, curhistory);
			if (result1 != 0) {
				_ASSERT(0);
				abort();
			}
			m_owpelemvec.push_back(newowp);
		}

		size_t owpindex;
		size_t owpnum = m_owpelemvec.size();
		for (owpindex = 0; owpindex < owpnum; owpindex++) {
			CCpHistoryOWPElem* curowp = m_owpelemvec[owpindex];
			if (curowp) {
				curowp->AddParts(m_dlgsplist);
			}
		}
		size_t owpindex2;
		for (owpindex2 = 0; owpindex2 < owpnum; owpindex2++) {
			CCpHistoryOWPElem* curowp = m_owpelemvec[owpindex2];
			if (curowp) {
				curowp->SetEventFunc(this);
			}
		}


		//s_fogslotCombo->setButtonListener([]() {
		//	int comboid = s_lightsslotCombo->trackPopUpMenu();
		//	if ((comboid >= 0) && (comboid < FOGSLOTNUM)) {
		//		g_fogindex = comboid;
		//		FogParams2Dlg();
		//	}
		//});

		m_Combo1->setButtonListener([=, this]() {
			int cmoboid = m_Combo1->trackPopUpMenu();
		});
		m_Combo2->setButtonListener([=, this]() {
			int cmoboid = m_Combo2->trackPopUpMenu();
		});
		m_Combo3->setButtonListener([=, this]() {
			int cmoboid = m_Combo3->trackPopUpMenu();
		});

		m_fromComboYear->setButtonListener([=, this]() {
			int cmoboid = m_fromComboYear->trackPopUpMenu();
		});
		m_toComboYear->setButtonListener([=, this]() {
			int cmoboid = m_toComboYear->trackPopUpMenu();
		});

		m_fromComboMonth->setButtonListener([=, this]() {
			int cmoboid = m_fromComboMonth->trackPopUpMenu();
		});
		m_toComboMonth->setButtonListener([=, this]() {
			int cmoboid = m_toComboMonth->trackPopUpMenu();
		});

		m_fromComboDay->setButtonListener([=, this]() {
			int cmoboid = m_fromComboDay->trackPopUpMenu();
		});
		m_toComboDay->setButtonListener([=, this]() {
			int cmoboid = m_toComboDay->trackPopUpMenu();
		});


		m_recentChk->setButtonListener([=, this]() {
			bool value = m_recentChk->getValue();
			m_ischeckedmostrecent = value;
			EnableList(!m_ischeckedmostrecent);//2024/07/24
			if (m_dlgwnd) {
				m_dlgwnd->callRewrite();
			}
		});

		m_searchB->setButtonListener([=, this]() {
			if (g_mainhwnd && IsWindow(g_mainhwnd)) {
				//OnSearch();//OnSearch()からはParams2Dlg()が呼ばれてそこからCreateOWPWnd()が呼ばれてm_searchBは作り直されエラーになる
				::PostMessage(g_mainhwnd, WM_COMMAND, (ID_RMENU_0 + MENUOFFSET_COPYHISTORYDLG), 0);//SendMessageでも上記と同様のエラー.　PostMessage()を使う
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

int CCopyHistoryDlg2::ParamsToDlg(CModel* srcmodel, HISTORYELEM saveselectedelem)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	WCHAR currentname[MAX_PATH] = { 0L };
	int result = GetSelectedFileName(srcmodel, currentname);

//##################
//選択状態の保存をする
//##################

	//######################################################################################
	//CopyHistoryの選択状態はm_copyhistoryが更新されるより前で保存しなければならない--> 引数で渡される
	//######################################################################################
	//int saveselectedindex = GetCheckedElem();
	//HISTORYELEM saveselectedelem;
	//saveselectedelem.Init();
	//if (saveselectedindex >= 0) {
	//	saveselectedelem = m_copyhistory[saveselectedindex];
	//}

	//###############################
	//ComboBoxの選択状態はこの場所で良い
	//###############################
	int selectedfbx = -1;
	WCHAR findfbxname[MAX_PATH] = { 0L };
	if (m_Combo1) {
		int selectedindex = m_Combo1->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < (int)m_strcombo_fbxname.size())) {
			selectedfbx = selectedindex;
			wcscpy_s(findfbxname, MAX_PATH, m_strcombo_fbxname[selectedfbx].c_str());
		}
	}
	int selectedmotion = -1;
	WCHAR findmotname[MAX_PATH] = { 0L };
	if (m_Combo2) {
		int selectedindex = m_Combo2->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < (int)m_strcombo_motionname.size())) {
			selectedmotion = selectedindex;
			wcscpy_s(findmotname, MAX_PATH, m_strcombo_motionname[selectedmotion].c_str());
		}
	}
	int selectedbvh = -1;
	int findbvhtype = -1;
	if (m_Combo3) {
		int selectedindex = m_Combo3->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < (int)m_strcombo_bvhtype.size())) {
			selectedbvh = selectedindex;
			findbvhtype = m_strcombo_bvhtype[selectedbvh];
		}
	}
	int selectedfromyear = -1;
	int findfromyear = -1;
	if (m_fromComboYear) {
		int selectedindex = m_fromComboYear->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 4)) {
			selectedfromyear = selectedindex;
			findfromyear = 2021 + selectedfromyear;
		}
	}
	int selectedtoyear = -1;
	int findtoyear = -1;
	if (m_toComboYear) {
		int selectedindex = m_toComboYear->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 4)) {
			selectedtoyear = selectedindex;
			findtoyear = 2021 + selectedtoyear;
		}
	}
	int selectedfromMonth = -1;
	int findfromMonth = -1;
	if (m_fromComboMonth) {
		int selectedindex = m_fromComboMonth->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 12)) {
			selectedfromMonth = selectedindex;
			findfromMonth = selectedfromMonth + 1;
		}
	}
	int selectedtoMonth = -1;
	int findtoMonth = -1;
	if (m_toComboMonth) {
		int selectedindex = m_toComboMonth->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 12)) {
			selectedtoMonth = selectedindex;
			findtoMonth = selectedtoMonth + 1;
		}
	}
	int selectedfromDay = -1;
	int findfromDay = -1;
	if (m_fromComboDay) {
		int selectedindex = m_fromComboDay->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 31)) {
			selectedfromDay = selectedindex;
			findfromDay = selectedfromDay + 1;
		}
	}
	int selectedtoDay = -1;
	int findtoDay = -1;
	if (m_toComboDay) {
		int selectedindex = m_toComboDay->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 31)) {
			selectedtoDay = selectedindex;
			findtoDay = selectedtoDay + 1;
		}
	}

//##################
//ウインドウを作り直す
//##################
	DestroyOWPWnd();//まず破棄する
	int result2 = CreateOWPWnd(srcmodel);
	if (result2 != 0) {
		_ASSERT(0);
		abort();
	}

//####################
//可能であれば選択を復元
//####################
	if (saveselectedelem.hascpinfo == 1) {
		int newelemindex;
		int newelemnum = (int)m_copyhistory.size();
		for (newelemindex = 0; newelemindex < newelemnum; newelemindex++) {
			HISTORYELEM chkhistory = m_copyhistory[newelemindex];
			if ((chkhistory.hascpinfo == 1) && (saveselectedelem == chkhistory)) {
				size_t owpnum = m_owpelemvec.size();
				if (newelemindex < owpnum) {
					OnRadio(newelemindex);//copyhistoryのindex : 0から
					break;
				}
			}
		}
	}

	if ((selectedfbx >= 0) && (findfbxname[0] != 0L) && m_Combo1) {
		int combonum = (int)m_strcombo_fbxname.size();
		int comboindex;
		for (comboindex = 0; comboindex < combonum; comboindex++) {
			wstring chkname = m_strcombo_fbxname[comboindex];
			if (chkname == findfbxname) {
				m_Combo1->setSelectedCombo(comboindex + 1);//+1 : index0は"----"//2024/07/24
				break;
			}
		}
	}
	if ((selectedmotion >= 0) && (findmotname[0] != 0L) && m_Combo2) {
		int combonum = (int)m_strcombo_motionname.size();
		int comboindex;
		for (comboindex = 0; comboindex < combonum; comboindex++) {
			wstring chkname = m_strcombo_motionname[comboindex];
			if (chkname == findmotname) {
				m_Combo2->setSelectedCombo(comboindex + 1);//+1 : index0は"----"//2024/07/24
				break;
			}
		}
	}
	if ((selectedbvh >= 0) && (findbvhtype >= 0) && m_Combo3) {
		int combonum = (int)m_strcombo_bvhtype.size();
		int comboindex;
		for (comboindex = 0; comboindex < combonum; comboindex++) {
			int chktype = m_strcombo_bvhtype[comboindex];
			if (chktype == findbvhtype) {
				m_Combo3->setSelectedCombo(comboindex + 1);//+1 : index0は"----"//2024/07/24
				break;
			}
		}
	}
	if ((selectedfromyear >= 0) && m_fromComboYear) {
		m_fromComboYear->setSelectedCombo(selectedfromyear + 1);//+1 : index0は"----"//2024/07/24
	}
	if ((selectedtoyear >= 0) && m_toComboYear) {
		m_toComboYear->setSelectedCombo(selectedtoyear + 1);//+1 : index0は"----"//2024/07/24
	}
	if ((selectedfromMonth >= 0) && m_fromComboMonth) {
		m_fromComboMonth->setSelectedCombo(selectedfromMonth + 1);//+1 : index0は"----"//2024/07/24
	}
	if ((selectedtoMonth >= 0) && m_toComboMonth) {
		m_toComboMonth->setSelectedCombo(selectedtoMonth + 1);//+1 : index0は"----"//2024/07/24
	}
	if ((selectedfromDay >= 0) && m_fromComboDay) {
		m_fromComboDay->setSelectedCombo(selectedfromDay + 1);//+1 : index0は"----"//2024/07/24
	}
	if ((selectedtoDay >= 0) && m_toComboDay) {
		m_toComboDay->setSelectedCombo(selectedtoDay + 1);//+1 : index0は"----"//2024/07/24
	}

//###############
//リスト選択の可否
//###############
	EnableList(!m_ischeckedmostrecent);//2024/07/24
	m_dlgwnd->callRewrite();

	m_initsearchcomboflag = true;


	return 0;
}
//LRESULT CCopyHistoryDlg2::OnCheckMostRecent(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	m_ischeckedmostrecent = m_dlg_wnd.IsDlgButtonChecked(IDC_CHECK1);
//
//	if (m_ischeckedmostrecent && !m_copyhistory.empty()) {
//		m_selectnamemap[m_model] = m_copyhistory[m_startno + 0].wfilename;
//	}
//
//	SetEnableCtrls();
//	return 0;
//}


int CCopyHistoryDlg2::SetNames(CModel* srcmodel, std::vector<HISTORYELEM>& copyhistory)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

//##################
//選択状態の保存をする
//##################
	HISTORYELEM saveselectedelem = GetCheckedElem();

//##################
//ウインドウを作り直す
//##################
	m_model = srcmodel;
	m_copyhistory = copyhistory;
	m_savecopyhistory = copyhistory;


	WCHAR modelname[MAX_PATH] = { 0L };
	const WCHAR* pname = srcmodel->GetFileName();
	if (!pname) {
		_ASSERT(0);
		return 1;
	}
	wcscpy_s(modelname, MAX_PATH, pname);

	{
		//srcmodelで検索して　結果をm_copyhisotryにセット
		//search fbxname
		std::vector<HISTORYELEM> copyhistory1 = m_savecopyhistory;
		std::vector<HISTORYELEM> copyhistory2;
		copyhistory2.clear();
		{
			size_t namenum1 = copyhistory1.size();
			int nameno1;
			for (nameno1 = 0; nameno1 < namenum1; nameno1++) {
				if (copyhistory1[nameno1].hascpinfo == 1) {
					if (wcscmp(copyhistory1[nameno1].cpinfo.fbxname, modelname) == 0) {
						copyhistory2.push_back(copyhistory1[nameno1]);
					}
				}
			}
		}


		m_copyhistory = copyhistory2;
	}

	{
		m_strcombo_fbxname.clear();

		size_t srcnum = m_savecopyhistory.size();//検索候補はsavecopyhistoryから取得する
		size_t srcno;
		for (srcno = 0; srcno < srcnum; srcno++) {
			if (m_savecopyhistory[srcno].hascpinfo == 1) {

				wstring curfbxname = m_savecopyhistory[srcno].cpinfo.fbxname;
				bool foundfbxname = false;
				std::vector<std::wstring>::iterator itrfbxname;
				for (itrfbxname = m_strcombo_fbxname.begin(); itrfbxname != m_strcombo_fbxname.end(); itrfbxname++) {
					if (curfbxname == *itrfbxname) {
						foundfbxname = true;
						break;
					}
				}
				if (foundfbxname == false) {
					m_strcombo_fbxname.push_back(curfbxname);
				}
			}
		}
	}

	ParamsToDlg(srcmodel, saveselectedelem);


	m_createdflag = true;


	return 0;


}

int CCopyHistoryDlg2::OnSearch()
{
	//##################
	//選択状態の保存をする
	//##################
	HISTORYELEM saveselectedelem = GetCheckedElem();



	//##########
	//Searching
	//##########
	int selectedfbx = -1;
	WCHAR findfbxname[MAX_PATH] = { 0L };
	if (m_Combo1) {
		int selectedindex = m_Combo1->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < (int)m_strcombo_fbxname.size())) {
			selectedfbx = selectedindex;
			wcscpy_s(findfbxname, MAX_PATH, m_strcombo_fbxname[selectedfbx].c_str());
		}
	}

	int selectedmotion = -1;
	WCHAR findmotname[MAX_PATH] = { 0L };
	if (m_Combo2) {
		int selectedindex = m_Combo2->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < (int)m_strcombo_motionname.size())) {
			selectedmotion = selectedindex;
			wcscpy_s(findmotname, MAX_PATH, m_strcombo_motionname[selectedmotion].c_str());
		}
	}

	int selectedbvh = -1;
	int findbvhtype = -1;
	if (m_Combo3) {
		int selectedindex = m_Combo3->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < (int)m_strcombo_bvhtype.size())) {
			selectedbvh = selectedindex;
			findbvhtype = m_strcombo_bvhtype[selectedbvh];
		}
	}

	int selectedfromyear = -1;
	int findfromyear = -1;
	if (m_fromComboYear) {
		int selectedindex = m_fromComboYear->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 4)) {
			selectedfromyear = selectedindex;
			findfromyear = 2021 + selectedfromyear;
		}
	}
	int selectedtoyear = -1;
	int findtoyear = -1;
	if (m_toComboYear) {
		int selectedindex = m_toComboYear->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 4)) {
			selectedtoyear = selectedindex;
			findtoyear = 2021 + selectedtoyear;
		}
	}

	int selectedfromMonth = -1;
	int findfromMonth = -1;
	if (m_fromComboMonth) {
		int selectedindex = m_fromComboMonth->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 12)) {
			selectedfromMonth = selectedindex;
			findfromMonth = selectedfromMonth + 1;
		}
	}
	int selectedtoMonth = -1;
	int findtoMonth = -1;
	if (m_toComboMonth) {
		int selectedindex = m_toComboMonth->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 12)) {
			selectedtoMonth = selectedindex;
			findtoMonth = selectedtoMonth + 1;
		}
	}

	int selectedfromDay = -1;
	int findfromDay = -1;
	if (m_fromComboDay) {
		int selectedindex = m_fromComboDay->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 31)) {
			selectedfromDay = selectedindex;
			findfromDay = selectedfromDay + 1;
		}
	}
	int selectedtoDay = -1;
	int findtoDay = -1;
	if (m_toComboDay) {
		int selectedindex = m_toComboDay->getSelectedCombo() - 1;
		if ((selectedindex >= 0) && (selectedindex < 31)) {
			selectedtoDay = selectedindex;
			findtoDay = selectedtoDay + 1;
		}
	}



	bool finddateflag;
	if ((selectedfromyear >= 0) && (selectedtoyear >= 0) &&
		(selectedfromMonth >= 0) && (selectedtoMonth >= 0) &&
		(selectedfromDay >= 0) && (selectedtoDay >= 0)
		) {
		finddateflag = true;
	}
	else {
		finddateflag = false;
	}


	//search fbxname
	std::vector<HISTORYELEM> copyhistory1 = m_savecopyhistory;
	std::vector<HISTORYELEM> copyhistory2;
	copyhistory2.clear();
	{
		size_t namenum1 = copyhistory1.size();
		if ((selectedfbx >= 0) && (findfbxname[0] != 0L)) {
			int nameno1;
			for (nameno1 = 0; nameno1 < namenum1; nameno1++) {
				if (copyhistory1[nameno1].hascpinfo == 1) {
					if (wcscmp(copyhistory1[nameno1].cpinfo.fbxname, findfbxname) == 0) {
						copyhistory2.push_back(copyhistory1[nameno1]);
					}
				}
			}
		}
		else {
			copyhistory2 = copyhistory1;
		}
	}

	//search motionname
	std::vector<HISTORYELEM> copyhistory3;
	copyhistory3.clear();
	{
		size_t namenum1 = copyhistory2.size();
		if ((selectedmotion >= 0) && (findmotname[0] != 0L)) {
			int nameno1;
			for (nameno1 = 0; nameno1 < namenum1; nameno1++) {
				if (copyhistory2[nameno1].hascpinfo == 1) {
					if (wcscmp(copyhistory2[nameno1].cpinfo.motionname, findmotname) == 0) {
						copyhistory3.push_back(copyhistory2[nameno1]);
					}
				}
			}
		}
		else {
			copyhistory3 = copyhistory2;
		}
	}

	//search bvhtype
	std::vector<HISTORYELEM> copyhistory4;
	copyhistory4.clear();
	{
		size_t namenum1 = copyhistory3.size();
		if ((selectedbvh >= 0) && (findbvhtype >= 0)) {
			size_t nameno1;
			for (nameno1 = 0; nameno1 < namenum1; nameno1++) {
				if (copyhistory3[nameno1].hascpinfo == 1) {
					if (copyhistory3[nameno1].cpinfo.bvhtype == findbvhtype) {
						copyhistory4.push_back(copyhistory3[nameno1]);
					}
				}
			}
		}
		else {
			copyhistory4 = copyhistory3;
		}
	}


	//search date
	std::vector<HISTORYELEM> copyhistory5;
	copyhistory5.clear();
	{
		size_t namenum1 = copyhistory4.size();
		if (finddateflag) {
			size_t nameno1;
			for (nameno1 = 0; nameno1 < namenum1; nameno1++) {
				if (copyhistory4[nameno1].hascpinfo == 1) {
					FILETIME srcfiletime = copyhistory4[nameno1].filetime;
					FILETIME localfiletime;
					FILETIME startfiletime;
					FILETIME endfiletime;
					SYSTEMTIME ststart;
					SYSTEMTIME stend;
					ZeroMemory(&ststart, sizeof(SYSTEMTIME));
					ZeroMemory(&stend, sizeof(SYSTEMTIME));
					ststart.wYear = (WORD)findfromyear;
					ststart.wMonth = (WORD)findfromMonth;
					ststart.wDay = (WORD)findfromDay;
					stend.wYear = (WORD)findtoyear;
					stend.wMonth = (WORD)findtoMonth;
					stend.wDay = (WORD)findtoDay;
					stend.wHour = 23;
					stend.wMinute = 59;
					stend.wSecond = 59;
					stend.wMilliseconds = 999;
					SystemTimeToFileTime(&ststart, &startfiletime);
					SystemTimeToFileTime(&stend, &endfiletime);

					FileTimeToLocalFileTime(&srcfiletime, &localfiletime);

					//if ((CompareFileTime(&srcfiletime, &startfiletime) >= 0) && (CompareFileTime(&srcfiletime, &endfiletime) <= 0)) {
					if ((CompareFileTime(&localfiletime, &startfiletime) >= 0) && (CompareFileTime(&localfiletime, &endfiletime) <= 0)) {
						copyhistory5.push_back(copyhistory4[nameno1]);
					}
				}
			}
		}
		else {
			copyhistory5 = copyhistory4;
		}
	}




	m_copyhistory = copyhistory5;


	ParamsToDlg(m_model, saveselectedelem);

	return 0;
}


int CCopyHistoryDlg2::OnDelete(int delid)
{
	if ((delid < 0) || (delid >= (int)m_copyhistory.size())) {
		return 0;
	}


	WCHAR delcpt[MAX_PATH] = { 0L };
	wcscpy_s(delcpt, MAX_PATH, m_copyhistory[delid].wfilename);
	delcpt[MAX_PATH - 1] = 0L;
	if (delcpt[0] != 0L) {
		BOOL bexist;
		bexist = PathFileExists(delcpt);
		if (bexist) {
			DeleteFileW(delcpt);
		}
	}

	if (m_copyhistory[delid].hascpinfo == 1) {
		WCHAR delcpi[MAX_PATH] = { 0L };
		wcscpy_s(delcpi, MAX_PATH, delcpt);
		delcpi[MAX_PATH - 1] = 0L;
		WCHAR* pdot = wcsrchr(delcpi, TEXT('.'));
		if (pdot) {
			*pdot = 0L;
			wcscat_s(delcpi, MAX_PATH, L".cpi");
			BOOL bexist;
			bexist = PathFileExists(delcpi);
			if (bexist) {
				DeleteFileW(delcpi);
			}
		}
	}

	wcscpy_s(m_copyhistory[delid].wfilename, MAX_PATH, L"deleted.");
	m_copyhistory[delid].hascpinfo = 0;
	wcscpy_s(m_copyhistory[delid].cpinfo.fbxname, MAX_PATH, L"deleted.");
	wcscpy_s(m_copyhistory[delid].cpinfo.motionname, MAX_PATH, L"deleted.");


	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CCpHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && (curowp->GetCopyHistoryIndex() == delid)) {
			if (curowp->GetDescLabel()) {
				curowp->GetDescLabel()->setName(L"deleted.");
				curowp->GetDescLabel()->setActive(false);
			}
			if (curowp->GetMemoLabel()) {
				curowp->GetMemoLabel()->setName(L"deleted.");
				curowp->GetMemoLabel()->setActive(false);
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

int CCopyHistoryDlg2::OnRadio(int radioid)
{
	if ((radioid < 0) || (radioid >= (int)m_copyhistory.size())) {
		return 0;
	}

	//OKボタンを押さないでも反映されるように
	if (!m_copyhistory.empty()) {
		m_selectnamemap[m_model] = m_copyhistory[radioid].wfilename;
		m_selectindexmap[m_model] = radioid;
	}

	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CCpHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && curowp->GetNameCheckBox()) {
			if (curowp->GetCopyHistoryIndex() == radioid) {
				curowp->GetNameCheckBox()->setValue(true, false);
			}
			else {
				curowp->GetNameCheckBox()->setValue(false, false);
			}
		}
	}

	if (m_dlgSc && m_dlgspcombo) {
		//2024/07/24 2024/07/25
		//選択項目が画面内に入るようにスクロール処理をすることにした
		int showposline = radioid * 4 + m_dlgspcombo->getPartsNum1();
		//+getPartsNum1()について 
		//今回の場合 親ウインドウの直接の子供はm_dlgspcomboとm_dlgsplist(m_dlgSc)の上下２段組で下段だけスクロールする
		//m_dlgsplist(下段)のshowPosLineの制御をする
		//スクロールウインドウのinView()は表示行数計算時に親ウインドウのsize.y/LABEL_SIZE_Yを使う
		//inView()の１行目の位置は、m_dlgspcombo(上段)の１行目の位置になっている
		//m_dlgsplistの１行目からの計算にするためにm_dlgspcomboの全行数を足す
		m_dlgSc->inView(showposline);//2024/07/24 inView()は視野内にある場合には何もしない
	}

	if (m_dlgwnd) {
		m_dlgwnd->callRewrite();
	}

	return 0;
}



HISTORYELEM CCopyHistoryDlg2::GetFirstValidElem(int* pindex)
{
	if (!pindex) {
		_ASSERT(0);
		HISTORYELEM inielem;
		inielem.Init();
		return inielem;
	}

	int elemnum = (int)m_copyhistory.size();
	int elemno;
	for (elemno = 0; elemno < elemnum; elemno++) {
		HISTORYELEM helem = m_copyhistory[elemno];
		if (helem.hascpinfo == 1) {
			*pindex = elemno;
			return helem;
		}
		else {
			//有効な履歴が見つかるまでループを続ける
		}
	}

	//有効な履歴がみつからなかった場合
	HISTORYELEM inielem;
	inielem.Init();
	*pindex = -1;
	return inielem;
}

HISTORYELEM CCopyHistoryDlg2::GetCheckedElem()
{
	int selectedno = -1;//チェックされていない場合

	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CCpHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp && curowp->GetNameCheckBox() && curowp->GetNameCheckBox()->getValue()) {
			//selectedno = owpindex;
			selectedno = curowp->GetCopyHistoryIndex();//2024/07/24
			break;
		}
	}

	if ((selectedno >= 0) && (selectedno < (int)m_copyhistory.size())) {
		HISTORYELEM checkedelem = m_copyhistory[selectedno];
		if (checkedelem.hascpinfo == 1) {
			return checkedelem;
			//return (int)selectedno;
		}
	}

	//有効な履歴がみつからなかった場合
	HISTORYELEM inielem;
	inielem.Init();
	return inielem;

	//return -1;
}

int CCopyHistoryDlg2::GetSelectedFileName(CModel* srcmodel, WCHAR* dstfilename) 
{
	if (!srcmodel || !dstfilename) {
		_ASSERT(0);
		return 1;
	}

	*dstfilename = 0L;
	//m_selectname[MAX_PATH - 1] = 0L;



	if (GetCreatedFlag() == false) {

		//まだウインドウが作成されていない場合

		//最新の有効なコピーを選択
		int selectindex = -1;
		HISTORYELEM firstelem = GetFirstValidElem(&selectindex);
		if (firstelem.hascpinfo == 1) {
			m_selectnamemap[srcmodel] = firstelem.wfilename;
			m_selectindexmap[srcmodel] = selectindex;
			wcscpy_s(dstfilename, MAX_PATH, firstelem.wfilename);
			return 0;
		}
		else {
			return 1;//有効な履歴無しの場合
		}
	}
	else {

		//ウインドウが作成された後

		//m_ischeckedmostrecent = m_dlg_wnd.IsDlgButtonChecked(IDC_CHECK1);

		if (m_ischeckedmostrecent == true) {
			//最新の有効なコピーを選択
			int selectindex = -1;
			HISTORYELEM firstelem = GetFirstValidElem(&selectindex);
			if (firstelem.hascpinfo == 1) {
				m_selectnamemap[srcmodel] = firstelem.wfilename;
				m_selectindexmap[srcmodel] = selectindex;
				wcscpy_s(dstfilename, MAX_PATH, firstelem.wfilename);
				return 0;
			}
			else {
				return 1;//有効な履歴無しの場合
			}
		}
		else {
			////チェックした履歴を選択
			//HISTORYELEM checkedelem = GetCheckedElem();
			//if (checkedelem.hascpinfo == 1) {
			//	m_selectnamemap[srcmodel] = checkedelem.wfilename;
			//	wcscpy_s(dstfilename, MAX_PATH, checkedelem.wfilename);
			//	return 0;
			//}
			//else {
			//	return 1;//有効な履歴無しの場合
			//}

			map<CModel*, wstring>::iterator itrfindname;
			itrfindname = m_selectnamemap.find(srcmodel);
			if (itrfindname != m_selectnamemap.end()) {
				wcscpy_s(dstfilename, MAX_PATH, itrfindname->second.c_str());
				return 0;
			}
			else {
				int selectindex = -1;
				HISTORYELEM firstelem = GetFirstValidElem(&selectindex);
				if (firstelem.hascpinfo == 1) {
					m_selectnamemap[srcmodel] = firstelem.wfilename;
					m_selectindexmap[srcmodel] = selectindex;
					wcscpy_s(dstfilename, MAX_PATH, firstelem.wfilename);
					return 0;
				}
				else {
					return 1;//有効な履歴無しの場合
				}
			}
		}
	}
};

void CCopyHistoryDlg2::EnableList(bool srcflag)
{
	size_t owpnum = m_owpelemvec.size();
	int owpindex;
	for (owpindex = 0; owpindex < owpnum; owpindex++) {
		CCpHistoryOWPElem* curowp = m_owpelemvec[owpindex];
		if (curowp){
			int historyindex = curowp->GetCopyHistoryIndex();
			if ((historyindex >= 0) && (historyindex < m_copyhistory.size())) {
				bool setflag = srcflag;
				HISTORYELEM curhistory = m_copyhistory[historyindex];
				if (curhistory.hascpinfo == 1) {
					setflag = srcflag;
				}
				else {
					setflag = false;//2024/07/25 deletedなどは非アクティブのままに
				}
				if (curowp->GetNameCheckBox() && (curowp->GetNameCheckBox() != m_recentChk)) {
					curowp->GetNameCheckBox()->setActive(setflag);
				}
				if (curowp->GetDescLabel()) {
					curowp->GetDescLabel()->setActive(setflag);
				}
				if (curowp->GetMemoLabel()) {
					curowp->GetMemoLabel()->setActive(setflag);
				}
				if (curowp->GetDelButton()) {
					curowp->GetDelButton()->setActive(setflag);
				}

			}
		}
	}
}
