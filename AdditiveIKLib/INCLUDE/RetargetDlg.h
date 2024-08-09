#ifndef __CRetargetDlg_H_
#define __CRetargetDlg_H_


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
// CRetargetDlg
class CRetargetDlg
{
public:
	CRetargetDlg();
	~CRetargetDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	void SetVisible(bool srcflag);

	int SetConvBone_NoSet();
	int SetConvBone_Set(int bvhcbno);
	int InitJointPair2ConvBoneWnd();
	int SetJointPair2ConvBoneWnd();

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateRetargetWnd();
	int ParamsToDlg();

public:
	bool GetRetargetCloseFlag()
	{
		return m_closeconvboneFlag;
	};
	void SetRetargetCloseFlag(bool srcval)
	{
		m_closeconvboneFlag = srcval;
	};
	bool GetRetargetRetargetGUIFlag()
	{
		return m_retargetguiFlag;
	};
	void SetRetargetRetargetGUIFlag(bool srcval)
	{
		m_retargetguiFlag = srcval;
	};
	int GetRetargetSelectBoneIndex() {
		return m_selectboneindex;
	};
	void SetRetargetSelectBoneIndex(int srcval)
	{
		m_selectboneindex = srcval;
	};
	CModel* GetRetargetModel()
	{
		return m_convbone_model;
	};
	CModel* GetRetargetBvh()
	{
		return m_convbone_bvh;
	};
	void SetRetargetBvh(CModel* srcval)
	{
		m_convbone_bvh = srcval;
	};
	void SetBvhName(const WCHAR* strmes, COLORREF txtcol)
	{
		if (m_cbselbvh) {
			m_cbselbvh->setName(strmes);
			m_cbselbvh->setTextColor(txtcol);
		}
	};
	int GetRetargetBvhBoneCBNo()
	{
		return m_bvhbone_cbno;
	};
	void SetRetargetBvhBoneCBNo(int srcval)
	{
		m_bvhbone_cbno = srcval;
	};
	void SetRetargetBvhBoneCBNo(int bvhcbno, int boneno)
	{
		m_bvhbone_bonenomap[bvhcbno] = boneno;//2024/07/07 メニューのインデックス-->ボーン番号　変換表
	};
	void GetRetargetConvBoneMap(std::map<CBone*, CBone*>& dstmap)
	{
		dstmap = m_convbonemap;
	};
	void SetRetargetConvBoneMap(std::map<CBone*, CBone*> srcmap)
	{
		m_convbonemap = srcmap;
	};

private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_convbone_model;
	CModel* m_convbone_bvh;
	int m_convbonenum;
	//int m_maxboneno;
	CBone* m_modelbone_bone[MAXBONENUM];
	CBone* m_bvhbone_bone[MAXBONENUM];
	std::map<CBone*, CBone*> m_convbonemap;
	std::map<int, int> m_bvhbone_bonenomap;//<メニューインデックス, ボーン番号>  eNullなどは除外してm_bvhbone_boneにセットする
	int m_bvhbone_cbno;
	bool m_closeconvboneFlag;
	bool m_retargetguiFlag;
	int m_selectboneindex;

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_ScrollWnd* m_convboneSCWnd;
	OrgWinGUI::OWP_Label* m_cbselmodel;
	OrgWinGUI::OWP_Button* m_cbselbvh;
	OrgWinGUI::OWP_Label* m_convbonemidashi[2];
	OrgWinGUI::OWP_Label* m_modelbone[MAXBONENUM];
	OrgWinGUI::OWP_Button* m_bvhbone[MAXBONENUM];
	OrgWinGUI::OWP_Separator* m_convbonesp;
	OrgWinGUI::OWP_Button* m_convboneconvert;
	OrgWinGUI::OWP_Label* m_convbonespace1;
	OrgWinGUI::OWP_Label* m_convbonespace2;
	OrgWinGUI::OWP_Label* m_convbonespace3;
	OrgWinGUI::OWP_Label* m_convbonespace4;
	OrgWinGUI::OWP_Label* m_convbonespace5;
	OrgWinGUI::OWP_Button* m_rtgfilesave;
	OrgWinGUI::OWP_Button* m_rtgfileload;

};

#endif //__ColiIDDlg_H_
