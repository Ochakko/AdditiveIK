#ifndef __CShaderTypeDlg_H_
#define __CShaderTypeDlg_H_


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
// CShaderTypeDlg
class CShaderTypeDlg
{
public:
	CShaderTypeDlg();
	~CShaderTypeDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	void SetVisible(bool srcflag);
	int SelectMaterial(CModel* srcpickmodel, CMQOMaterial* srcpickmaterial);
	int SetShaderType(int srcshadertype);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateShaderTypeWnd();
	int ParamsToDlg();

public:
	void SetShaderTypeParamsFlag(bool srcval)
	{
		m_shadertypeparamsFlag = srcval;
	};
	bool GetShaderTypeParamsFlag()
	{
		return m_shadertypeparamsFlag;
	};
	void SetShaderTypeParamsIndex(int srcval)
	{
		m_shadertypeparamsindex = srcval;
	};
	int GetShaderTypeParamsIndex()
	{
		return m_shadertypeparamsindex;
	};


private:
	bool m_createdflag;
	bool m_visible;

	int m_posx;
	int m_posy;
	int m_sizex;
	int m_sizey;

	CModel* m_model;
	bool m_shadertypeparamsFlag;
	int m_shadertypeparamsindex;//index==0は全てのマテリアルに設定. それ以外はindex - 1のマテリアルに設定

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_ScrollWnd* m_SCshadertype;
	OrgWinGUI::OWP_Separator* m_shadersp1;
	OrgWinGUI::OWP_Separator* m_shadersp2;
	OrgWinGUI::OWP_Separator* m_shadersp3;
	OrgWinGUI::OWP_Label* m_modelnamelabel;
	OrgWinGUI::OWP_Button* m_materialnameB[MAXMATERIALNUM + 1];//+1は見出しの分
	OrgWinGUI::OWP_Label* m_shadertypelabel[MAXMATERIALNUM + 1];//+1は見出しの分

};

#endif //__ColiIDDlg_H_
