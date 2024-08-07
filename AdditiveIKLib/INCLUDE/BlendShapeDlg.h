#ifndef __CBlendShapeDlg_H_
#define __CBlendShapeDlg_H_


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
// CBlendShapeDlg
class CBlendShapeDlg
{
public:
	CBlendShapeDlg();
	~CBlendShapeDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	void SetVisible(bool srcflag);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);
	int ParamsToDlg();

	int CreateBlendShapeWnd();

public:
	void SetBlendShapeOpeIndex(int srcval)
	{
		m_blendshapeOpeIndex = srcval;
	};
	int GetBlendShapeOpeIndex()
	{
		return m_blendshapeOpeIndex;
	};
	void SetBlendShapeBefore(float srcval)
	{
		m_blendshapeBefore = srcval;
	};
	float GetBlendShapeBefore()
	{
		return m_blendshapeBefore;
	};
	void SetBlendShapeAfter(float srcval)
	{
		m_blendshapeAfter = srcval;
	};
	float GetBlendShapeAfter()
	{
		return m_blendshapeAfter;
	};
	void SetBlendShapeUndoOpeIndex(int srcval)
	{
		m_blendshapeUndoOpeIndex = srcval;
	};
	int GetBlendShapeUndoOpeIndex()
	{
		return m_blendshapeUndoOpeIndex;
	};
	void SetBlendShapeUnderEdit(bool srcval)
	{
		m_blendshapeUnderEdit = srcval;
	};
	bool GetBlendShapeUnderEdit()
	{
		return m_blendshapeUnderEdit;
	};
	void SetBlendShapePostEdit(bool srcval)
	{
		m_blendshapePostEdit = srcval;
	};
	bool GetBlendShapePostEdit()
	{
		return m_blendshapePostEdit;
	};
	void SetBlendShapeUnderSelect(bool srcval)
	{
		m_blendshapeUnderSelect = srcval;
	};
	bool GetBlendShapeUnderSelect()
	{
		return m_blendshapeUnderSelect;
	};
	void SetBlendShapeUnderSelectFromUndo(bool srcval)
	{
		m_blendshapeUnderSelectFromUndo = srcval;
	};
	bool GetBlendShapeUnderSelectFromUndo()
	{
		return m_blendshapeUnderSelectFromUndo;
	};
	void SetBlendShapeUnderSelectFromRefresh(bool srcval)
	{
		m_blendshapeUnderSelectFromRefresh = srcval;
	};
	bool GetBlendShapeUnderSelectFromRefresh()
	{
		return 	m_blendshapeUnderSelectFromRefresh;
	};
	int GetBlendShapeLineNum()
	{
		int linenum = (int)m_blendshapeelemvec.size();
		if (linenum == m_blendshapelinenum) {
			return linenum;
		}
		else {
			_ASSERT(0);
			return linenum;
		}
	};
	CBlendShapeElem GetBlendShapeElem(int srcindex)
	{
		if ((srcindex >= 0) && (srcindex < (int)m_blendshapeelemvec.size())) {
			return m_blendshapeelemvec[srcindex];
		}
		else {
			CBlendShapeElem initelem;
			initelem.Init();
			return initelem;
		}
	};


private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;


	int m_blendshapeOpeIndex;
	float m_blendshapeBefore = 0.0f;
	float m_blendshapeAfter = 0.0f;
	int m_blendshapeUndoOpeIndex;
	bool m_blendshapeUnderEdit;
	bool m_blendshapePostEdit;
	bool m_blendshapeUnderSelect;
	bool m_blendshapeUnderSelectFromUndo;
	bool m_blendshapeUnderSelectFromRefresh;

	int m_blendshapelinenum;
	std::vector<CBlendShapeElem> m_blendshapeelemvec;


	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_ScrollWnd* m_blendshapeSCWnd;
	OrgWinGUI::OWP_Separator* m_blendshapesp0;
	OrgWinGUI::OWP_Separator* m_blendshapesp1;
	OrgWinGUI::OWP_CheckBoxA* m_blendshapeadditive;
	OrgWinGUI::OWP_Label* m_blendshapemodelname;
	OrgWinGUI::OWP_Label* m_blendshapeemptyLabel;
	OrgWinGUI::OWP_Separator* m_blendshapedistsp;
	OrgWinGUI::OWP_Label* m_blendshapedistLabel;
	OrgWinGUI::OWP_Slider* m_blendshapedistSlider;
	std::vector<OrgWinGUI::OWP_Button*> m_blendshapeButton;
	std::vector<OrgWinGUI::OWP_Slider*> m_blendshapeSlider;


};

#endif //__ColiIDDlg_H_
