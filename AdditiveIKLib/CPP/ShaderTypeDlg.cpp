#include "stdafx.h"

#include <ShaderTypeDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <mqoobject.h>
#include <mqomaterial.h>
#include <Bone.h>
#include <RigidElem.h>
#include <BtObject.h>
#include <OrgWindow.h>
#include <GlobalVar.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>


using namespace std;
using namespace OrgWinGUI;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル


/////////////////////////////////////////////////////////////////////////////
// CShaderTypeDlg

CShaderTypeDlg::CShaderTypeDlg()
{
	InitParams();
}

CShaderTypeDlg::~CShaderTypeDlg()
{
	DestroyObjs();
}
	
int CShaderTypeDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_modelnamelabel) {
		delete m_modelnamelabel;
		m_modelnamelabel = 0;
	}

	int index;
	for (index = 0; index < (MAXMATERIALNUM + 1); index++) {
		if (m_materialnameB[index]) {
			delete m_materialnameB[index];
			m_materialnameB[index] = 0;
		}
		if (m_shadertypelabel[index]) {
			delete m_shadertypelabel[index];
			m_shadertypelabel[index] = 0;
		}
	}

	if (m_shadersp2) {
		delete m_shadersp2;
		m_shadersp2 = 0;
	}
	if (m_shadersp3) {
		delete m_shadersp3;
		m_shadersp3 = 0;
	}
	if (m_shadersp1) {
		delete m_shadersp1;
		m_shadersp1 = 0;
	}
	if (m_SCshadertype) {
		delete m_SCshadertype;
		m_SCshadertype = 0;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}

	m_shadertypeparamsFlag = false;
	m_shadertypeparamsindex = -1;//index==0は全てのマテリアルに設定. それ以外はindex - 1のマテリアルに設定

	return 0;
}

void CShaderTypeDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;
	m_shadertypeparamsFlag = false;
	m_shadertypeparamsindex = -1;//index==0は全てのマテリアルに設定. それ以外はindex - 1のマテリアルに設定

	m_dlgWnd = nullptr;

	m_SCshadertype = nullptr;
	m_shadersp1 = nullptr;
	m_shadersp2 = nullptr;
	m_shadersp3 = nullptr;
	m_modelnamelabel = nullptr;

	//+1は見出しの分
	int objno;
	for (objno = 0; objno < (MAXMATERIALNUM + 1); objno++) {
		m_materialnameB[objno] = nullptr;//+1は見出しの分
		m_shadertypelabel[objno] = nullptr;//+1は見出しの分
	}

}

int CShaderTypeDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

void CShaderTypeDlg::SetModel(CModel* srcmodel)
{
	m_model = srcmodel;
	CreateShaderTypeWnd();
	ParamsToDlg();

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		if (m_SCshadertype) {
			m_SCshadertype->autoResize();
		}
		m_dlgWnd->callRewrite();
	}
}


void CShaderTypeDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_SCshadertype) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_SCshadertype->autoResize();
			}
			m_dlgWnd->callRewrite();//2024/07/24
		}
	}
	else {
		if (m_dlgWnd) {
			m_dlgWnd->setVisible(false);
			//m_dlgWnd->setListenMouse(false);//<--- CloseAllRightPainWindow()を呼んだ時にwindowが反応しなくなるのはこの呼び出しが原因だった
		}
	}
	m_visible = srcflag;
}


int CShaderTypeDlg::CreateShaderTypeWnd()
{

	DestroyObjs();

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("ShaderTypeWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("ShderTypeWindow"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		true,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true, true);					//サイズ変更の可否

	if (!m_model) {
		//モデルが無い場合には　ウインドウ枠だけ作成してリターン
		return 0;
	}

	int materialnum = m_model->GetMQOMaterialSize();
	if (materialnum == 0) {
		return 0;
	}
	if ((materialnum < 0) || (materialnum >= MAXMATERIALNUM)) {
		//2024/03/03
		MessageBoxW(NULL, L"ERROR : MaterialNum Overflow.", L"Can't open dialog for settings.", MB_OK | MB_ICONERROR);
		return 1;
	}



	if (m_dlgWnd) {
		int linedatasize;
		linedatasize = (int)((double)(materialnum + 1) * 1.2);


		m_SCshadertype = new OWP_ScrollWnd(L"ShaderTypeScWnd", true, 20);
		if (!m_SCshadertype) {
			_ASSERT(0);
			return 1;
		}
		m_SCshadertype->setLineDataSize(linedatasize);//!!!!!!!!!!!!!
		m_dlgWnd->addParts(*m_SCshadertype);


		//m_shadersp1 = new OWP_Separator(m_dlgWnd, false, 0.75, true);
		//m_shadersp1 = new OWP_Separator(m_dlgWnd, false, 0.75, true, m_SCshadertype);//2023/12/22
		m_shadersp1 = new OWP_Separator(m_dlgWnd, true, 0.75, true, m_SCshadertype);//2024/05/25 contentsSize true
		if (!m_shadersp1) {
			_ASSERT(0);
			return 1;
		}
		m_SCshadertype->addParts(*m_shadersp1);

		//m_shadersp2 = new OWP_Separator(m_dlgWnd, false, 0.80, true, m_SCshadertype);
		//if (!m_shadersp2) {
		//	_ASSERT(0);
		//	return 1;
		//}
		//m_shadersp1->addParts1(*m_shadersp2);

		//m_shadersp3 = new OWP_Separator(m_dlgWnd, false, 0.5, true, m_SCshadertype);
		//if (!m_shadersp3) {
		//	_ASSERT(0);
		//	return 1;
		//}
		//m_shadersp1->addParts2(*m_shadersp3);

		//見出し行　見出し行のm_materialnameBは全てのマテリアルに適用するためのボタン
		//OrgWinGUI::OrgWindowParts::color_tag colorforindex;//RGB(168, 129, 129)
		//colorforindex.r = 168;
		//colorforindex.g = 129;
		//colorforindex.b = 129;
		COLORREF indexcolor = RGB(168, 129, 129);
		m_materialnameB[0] = new OWP_Button(L"MaterialName", 25);
		if (!m_materialnameB[0]) {
			_ASSERT(0);
			return 1;
		}
		m_materialnameB[0]->setTextColor(indexcolor);
		m_shadertypelabel[0] = new OWP_Label(L"ShaderType", 25);
		if (!m_shadertypelabel[0]) {
			_ASSERT(0);
			return 1;
		}
		//m_shadertypelabel[0]->setTextColor(indexcolor);
		//m_metalcoeflabel[0] = new OWP_Label(L"MetalCoef");
		//if (!m_metalcoeflabel[0]) {
		//	_ASSERT(0);
		//	return 1;
		//}
		//m_metalcoeflabel[0]->setTextColor(indexcolor);
		//m_lightscalelabel[0] = new OWP_Label(L"LightScale");
		//if (!m_lightscalelabel[0]) {
		//	_ASSERT(0);
		//	return 1;
		//}
		//m_lightscalelabel[0]->setTextColor(indexcolor);

		//m_shadersp2->addParts1(*(m_materialnameB[0]));
		//m_shadersp2->addParts2(*(m_shadertypelabel[0]));
		//m_shadersp3->addParts1(*(m_metalcoeflabel[0]));
		//m_shadersp3->addParts2(*(m_lightscalelabel[0]));

		m_shadersp1->addParts1(*(m_materialnameB[0]));
		m_shadersp1->addParts2(*(m_shadertypelabel[0]));



		int setindex;
		for (setindex = 1; setindex < (materialnum + 1); setindex++) {

			if (setindex == materialnum) {
				int dbgflag2 = 1;
			}

			int materialindex = setindex - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);
			if (!curmqomat) {
				_ASSERT(0);
				return 1;
			}


			char name[256] = { 0 };
			strcpy_s(name, 256, curmqomat->GetName());
			WCHAR wname[256] = { 0L };
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, 256, wname, 256);
			m_materialnameB[setindex] = new OWP_Button(wname, 20);
			if (!m_materialnameB[setindex]) {
				_ASSERT(0);
				return 1;
			}


			int shadertype = curmqomat->GetShaderType();
			WCHAR strshadertype[256] = { 0L };
			switch (shadertype) {
			case MQOSHADER_PBR:
				wcscpy_s(strshadertype, 256, L"PBR");
				break;
			case MQOSHADER_STD:
				wcscpy_s(strshadertype, 256, L"STD");
				break;
			case MQOSHADER_TOON:
				wcscpy_s(strshadertype, 256, L"TOON");
				break;
			case -1:
			case -2:
				wcscpy_s(strshadertype, 256, L"AUTO");
				break;
			default:
				_ASSERT(0);
				wcscpy_s(strshadertype, 256, L"Unknown");
				break;
			}
			m_shadertypelabel[setindex] = new OWP_Label(strshadertype, 20);
			if (!m_shadertypelabel[setindex]) {
				_ASSERT(0);
				return 1;
			}

			m_shadersp1->addParts1(*(m_materialnameB[setindex]));
			m_shadersp1->addParts2(*(m_shadertypelabel[setindex]));

		}


		//ボタンのラムダ関数
		int setindex2;
		for (setindex2 = 0; setindex2 < (materialnum + 1); setindex2++) {

			if (setindex2 == materialnum) {
				int dbgflag3 = 1;
			}

			//int materialindex = setindex2 - 1;	
			if (m_materialnameB[setindex2]) {
				m_materialnameB[setindex2]->setButtonListener([=, this]() {
					if (!m_shadertypeparamsFlag) {
						m_shadertypeparamsindex = setindex2;//index==0は全てのマテリアルに設定. それ以外はindex - 1のマテリアルに設定
						m_shadertypeparamsFlag = true;
					}
					if (m_dlgWnd) {
						m_dlgWnd->callRewrite();//再描画
					}
					});
			}
		}


		//autoResizeしないと　チェックボックス４段目以下が反応なかった
		//m_SCshadertype->autoResize();
		//m_shadersp3->autoResize();
		//m_shadersp2->autoResize();
		//m_shadersp1->autoResize();

		m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgWnd->setPos(WindowPos(m_posx, m_posy));
		//１クリック目問題対応
		m_dlgWnd->refreshPosAndSize();
		m_dlgWnd->autoResizeAllParts();
		m_dlgWnd->setVisible(false);


		//m_rcshadertypewnd.bottom = m_sideheight;
		//m_rcshadertypewnd.right = m_sidewidth;
	}
	else {
		_ASSERT(0);
		return 1;
	}


	return 0;


}

int CShaderTypeDlg::ParamsToDlg()
{
	if (!m_dlgWnd) {
		_ASSERT(0);
		return 1;
	}


	if (m_dlgWnd) {
		m_dlgWnd->callRewrite();
	}

	return 0;
}

int CShaderTypeDlg::SelectMaterial(CModel* srcpickmodel, CMQOMaterial* srcpickmaterial)
{
	if (!srcpickmodel || !srcpickmaterial) {
		return 0;
	}
	if (srcpickmodel != m_model) {
		_ASSERT(0);
		return 1;
	}

	int materialnum = m_model->GetMQOMaterialSize();
	int materialindex;
	for (materialindex = 0; materialindex < materialnum; materialindex++) {
		CMQOMaterial* chkmaterial = m_model->GetMQOMaterialByIndex(materialindex);
		if (chkmaterial && (chkmaterial == srcpickmaterial)) {
			if (m_SCshadertype) {
				m_SCshadertype->setShowPosLine(materialindex + 1);
			}

			m_shadertypeparamsindex = materialindex + 1;//index==0は全てのマテリアルに設定. それ以外はindex - 1のマテリアルに設定
			m_shadertypeparamsFlag = true;
			break;
		}
	}

	return 0;
}

int CShaderTypeDlg::SetShaderType(int srcshadertype)
{
	if (!m_model) {
		return 0;
	}

	int materialnum = m_model->GetMQOMaterialSize();
	materialnum = min(materialnum, MAXMATERIALNUM);
	int materialindex = m_shadertypeparamsindex - 1;
	CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	switch (srcshadertype) {
	case 0:
		if (curmqomat) {
			curmqomat->SetShaderType(-1);
			//s_shadertypeparams.shadertype = -1;

			if ((m_shadertypeparamsindex >= 1) && (m_shadertypeparamsindex < (materialnum + 1))) {
				if (m_shadertypelabel[m_shadertypeparamsindex]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"AUTO");
					m_shadertypelabel[m_shadertypeparamsindex]->setName(strdlg2);
				}
			}
		}
		else {
			int materialindex5;
			for (materialindex5 = 0; materialindex5 < materialnum; materialindex5++) {
				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex5);
				if (setmqomat) {
					setmqomat->SetShaderType(-1);
				}

				if (m_shadertypelabel[materialindex5 + 1]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"AUTO");
					m_shadertypelabel[materialindex5 + 1]->setName(strdlg2);
				}
			}
		}
		break;
	case 1:
		//s_shadertypeparams.shadertype = MQOSHADER_PBR;

		if (curmqomat) {
			curmqomat->SetShaderType(MQOSHADER_PBR);

			if ((m_shadertypeparamsindex >= 1) && (m_shadertypeparamsindex < (materialnum + 1))) {
				if (m_shadertypelabel[m_shadertypeparamsindex]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"PBR");
					m_shadertypelabel[m_shadertypeparamsindex]->setName(strdlg2);
				}
			}
		}
		else {
			int materialindex6;
			for (materialindex6 = 0; materialindex6 < materialnum; materialindex6++) {
				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex6);
				if (setmqomat) {
					setmqomat->SetShaderType(MQOSHADER_PBR);
				}

				if (m_shadertypelabel[materialindex6 + 1]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"PBR");
					m_shadertypelabel[materialindex6 + 1]->setName(strdlg2);
				}
			}
		}
		break;
	case 2:
		//s_shadertypeparams.shadertype = MQOSHADER_STD;

		if (curmqomat) {
			curmqomat->SetShaderType(MQOSHADER_STD);

			if ((m_shadertypeparamsindex >= 1) && (m_shadertypeparamsindex < (materialnum + 1))) {
				if (m_shadertypelabel[m_shadertypeparamsindex]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"STD");
					m_shadertypelabel[m_shadertypeparamsindex]->setName(strdlg2);
				}
			}
		}
		else {
			int materialindex7;
			for (materialindex7 = 0; materialindex7 < materialnum; materialindex7++) {
				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex7);
				if (setmqomat) {
					setmqomat->SetShaderType(MQOSHADER_STD);
				}

				if (m_shadertypelabel[materialindex7 + 1]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"STD");
					m_shadertypelabel[materialindex7 + 1]->setName(strdlg2);
				}
			}
		}
		break;
	case 3:
		//s_shadertypeparams.shadertype = MQOSHADER_TOON;

		if (curmqomat) {
			curmqomat->SetShaderType(MQOSHADER_TOON);

			if ((m_shadertypeparamsindex >= 1) && (m_shadertypeparamsindex < (materialnum + 1))) {
				if (m_shadertypelabel[m_shadertypeparamsindex]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"TOON");
					m_shadertypelabel[m_shadertypeparamsindex]->setName(strdlg2);
				}
			}
		}
		else {
			int materialindex8;
			for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
				if (setmqomat) {
					setmqomat->SetShaderType(MQOSHADER_TOON);
				}

				if (m_shadertypelabel[materialindex8 + 1]) {
					WCHAR strdlg2[256] = { 0L };
					wcscpy_s(strdlg2, 256, L"TOON");
					m_shadertypelabel[materialindex8 + 1]->setName(strdlg2);
				}
			}
		}
		break;
	default:
		_ASSERT(0);
		break;
	}

	return 0;
}

const HWND CShaderTypeDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CShaderTypeDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CShaderTypeDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

