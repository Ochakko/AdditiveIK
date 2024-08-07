#ifndef __CDispGroupDlg_H_
#define __CDispGroupDlg_H_


#include <windows.h>

#include <ChaVecCalc.h>

#include "../../AdditiveIK/resource.h"       // メイン シンボル

#include <vector>
#include <string>
#include <map>

class OrgWinGUI::OrgWindow;
class OrgWinGUI::OWP_ComboBoxA;
class OrgWinGUI::OWP_Button;
class OrgWinGUI::OWP_CheckBoxA;
class OrgWinGUI::OWP_Label;
class OrgWinGUI::OWP_EditBox;
class OrgWinGUI::OWP_Separator;
class OrgWinGUI::OWP_ScrollWnd;


/////////////////////////////////////////////////////////////////////////////
// CDispGroupDlg
class CDispGroupDlg
{
public:
	CDispGroupDlg();
	~CDispGroupDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	void SetVisible(bool srcflag);
	int CheckSimilarGroup(int opetype);
	int ShowPosLineMqoObject(CModel* srcpickmodel, CMQOObject* srcpickmqoobj);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateDispGroupWnd();
	int ParamsToDlg();
	int TrimLeadingAlnum(bool secondtokenflag, WCHAR* srcstr, int srclen, WCHAR* dststr, int dstlen, bool secondcallflag);

public:
	int GetDispGroupLineNum()
	{
		int linenum = (int)m_groupmqoobjvec.size();
		if (linenum == m_grouplinenum) {
			return linenum;
		}
		else {
			_ASSERT(0);
			return linenum;
		}
	};
	CMQOObject* GetDispGroupMqoObj(int srcindex)
	{
		int linenum = (int)m_groupmqoobjvec.size();
		if ((srcindex >= 0) && (srcindex < linenum)) {
			return m_groupmqoobjvec[srcindex];
		}
		else {
			_ASSERT(0);
			return nullptr;
		}
	};
	void SetDispGroupOnlyOneObjFlag(bool srcflag)
	{
		m_disponlyoneobj = srcflag;
	};
	bool GetDispGroupOnlyOneObjFlag()
	{
		return m_disponlyoneobj;
	};
	int GetDispGroupOnlyOneObjNo()
	{
		return m_onlyoneobjno;
	};
	bool GetDispGroupUnderGetting()
	{
		return m_groupUnderGetting;
	};
	void SetDispGroupCheckSimilarFlag(bool srcflag)
	{
		m_checksimilarFlag = srcflag;
	};
	bool GetDispGroupCheckSimilarFlag()
	{
		return m_checksimilarFlag;
	};
	void SetDispGroupCheckSimilarObjNo(bool srcflag)
	{
		m_checksimilarobjno = srcflag;
	};
	bool GetDispGroupCheckSimilarObjNo()
	{
		return m_checksimilarobjno;
	};


private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;
	std::vector<CMQOObject*> m_groupmqoobjvec;
	int m_grouplinenum;
	bool m_disponlyoneobj;//for test button of groupWnd
	int m_onlyoneobjno;//for test button of groupWnd
	bool m_groupUnderGetting;//m_groupgetBボタンの処理中は　groupobjvecのチェック処理をスキップ
	bool m_checksimilarFlag;
	int m_checksimilarobjno;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	//#define MAXDISPOBJNUM	4098 //vector<>に変更したため不要に
	//#define MAXDISPGROUPNUM	20 //coef.hに移動
	OrgWinGUI::OWP_ScrollWnd* m_groupSCWnd;
	OrgWinGUI::OWP_Separator* m_groupsp0;
	OrgWinGUI::OWP_Separator* m_groupsp;
	OrgWinGUI::OWP_Separator* m_groupsp1;
	OrgWinGUI::OWP_Separator* m_groupsp2;
	OrgWinGUI::OWP_Separator* m_groupsp3;
	OrgWinGUI::OWP_CheckBoxA* m_groupselect[MAXDISPGROUPNUM];
	OrgWinGUI::OWP_Button* m_groupsetB;
	OrgWinGUI::OWP_Button* m_groupgetB;
	OrgWinGUI::OWP_Button* m_grouponB;
	OrgWinGUI::OWP_Button* m_groupoffB;
	OrgWinGUI::OWP_Button* m_groupclearB;
	OrgWinGUI::OWP_Label* m_grouplabel11;
	OrgWinGUI::OWP_Label* m_grouplabel12;
	OrgWinGUI::OWP_Label* m_grouplabel21;
	OrgWinGUI::OWP_Label* m_grouplabel22;
	std::vector<OrgWinGUI::OWP_CheckBoxA*> m_groupobjvec;
	std::vector<OrgWinGUI::OWP_Button*> m_grouptestBvec;

};

#endif //__ColiIDDlg_H_
