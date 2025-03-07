﻿#include "stdafx.h"

#include <ShaderParamsDlg.h>
#include "../../AdditiveIK/SetDlgPos.h"

#include <Model.h>
#include <mqoobject.h>
//#include <mqomaterial.h>
#include <Bone.h>
#include <RigidElem.h>
#include <BtObject.h>
#include <OrgWindow.h>
#include <GlobalVar.h>

#include <ShaderTypeDlg.h>

#define DBGH
#include <dbg.h>
#include <crtdbg.h>
#include <algorithm>


using namespace std;
using namespace OrgWinGUI;


extern HWND g_mainhwnd;//アプリケーションウインドウハンドル


/////////////////////////////////////////////////////////////////////////////
// CShaderParamsDlg

CShaderParamsDlg::CShaderParamsDlg()
{
	InitParams();
}

CShaderParamsDlg::~CShaderParamsDlg()
{
	DestroyObjs();
}
	
int CShaderParamsDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_st_Sc) {
		delete m_st_Sc;
		m_st_Sc = nullptr;
	}
	if (m_st_spall) {
		delete m_st_spall;
		m_st_spall = nullptr;
	}
	if (m_st_spacerLabel01) {
		delete m_st_spacerLabel01;
		m_st_spacerLabel01 = nullptr;
	}
	if (m_st_spacerLabel02) {
		delete m_st_spacerLabel02;
		m_st_spacerLabel02 = nullptr;
	}
	if (m_st_spacerLabel03) {
		delete m_st_spacerLabel03;
		m_st_spacerLabel03 = nullptr;
	}
	if (m_st_spacerLabel04) {
		delete m_st_spacerLabel04;
		m_st_spacerLabel04 = nullptr;
	}
	if (m_st_spacerLabel05) {
		delete m_st_spacerLabel05;
		m_st_spacerLabel05 = nullptr;
	}
	if (m_st_spacerLabel06) {
		delete m_st_spacerLabel06;
		m_st_spacerLabel06 = nullptr;
	}
	if (m_st_namesp) {
		delete m_st_namesp;
		m_st_namesp = nullptr;
	}
	if (m_st_backB) {
		delete m_st_backB;
		m_st_backB = nullptr;
	}
	if (m_st_namelabel) {
		delete m_st_namelabel;
		m_st_namelabel = nullptr;
	}
	if (m_st_shadertyperadio) {
		delete m_st_shadertyperadio;
		m_st_shadertyperadio = nullptr;
	}
	if (m_st_litflagsp) {
		delete m_st_litflagsp;
		m_st_litflagsp = nullptr;
	}
	if (m_st_lightflagchk) {
		delete m_st_lightflagchk;
		m_st_lightflagchk = nullptr;
	}
	if (m_st_shadowcasterchk) {
		delete m_st_shadowcasterchk;
		m_st_shadowcasterchk = nullptr;
	}
	if (m_st_normaly0chk) {
		delete m_st_normaly0chk;
		m_st_normaly0chk = nullptr;
	}
	if (m_st_spccoefsp0) {
		delete m_st_spccoefsp0;
		m_st_spccoefsp0 = nullptr;
	}
	if (m_st_spccoefsp1) {
		delete m_st_spccoefsp1;
		m_st_spccoefsp1 = nullptr;
	}
	if (m_st_spccoefsp2) {
		delete m_st_spccoefsp2;
		m_st_spccoefsp2 = nullptr;
	}
	if (m_st_spccoeflabel) {
		delete m_st_spccoeflabel;
		m_st_spccoeflabel = nullptr;
	}
	if (m_st_spccoefslider) {
		delete m_st_spccoefslider;
		m_st_spccoefslider = nullptr;
	}
	if (m_st_emissionchk) {
		delete m_st_emissionchk;
		m_st_emissionchk = nullptr;
	}
	if (m_st_emissionslider) {
		delete m_st_emissionslider;
		m_st_emissionslider = nullptr;
	}
	if (m_st_metalsp0) {
		delete m_st_metalsp0;
		m_st_metalsp0 = nullptr;
	}
	if (m_st_metalsp1) {
		delete m_st_metalsp1;
		m_st_metalsp1 = nullptr;
	}
	if (m_st_metalsp2) {
		delete m_st_metalsp2;
		m_st_metalsp2 = nullptr;
	}
	if (m_st_metallabel) {
		delete m_st_metallabel;
		m_st_metallabel = nullptr;
	}
	if (m_st_metalslider) {
		delete m_st_metalslider;
		m_st_metalslider = nullptr;
	}
	if (m_st_smoothlabel) {
		delete m_st_smoothlabel;
		m_st_smoothlabel = nullptr;
	}
	if (m_st_smoothslider) {
		delete m_st_smoothslider;
		m_st_smoothslider = nullptr;
	}

	if (m_st_litscalesp1_0) {
		delete m_st_litscalesp1_0;
		m_st_litscalesp1_0 = nullptr;
	}
	if (m_st_litscalesp1_1) {
		delete m_st_litscalesp1_1;
		m_st_litscalesp1_1 = nullptr;
	}
	if (m_st_litscalesp1_2) {
		delete m_st_litscalesp1_2;
		m_st_litscalesp1_2 = nullptr;
	}
	if (m_st_litscalelabel1) {
		delete m_st_litscalelabel1;
		m_st_litscalelabel1 = nullptr;
	}
	if (m_st_litscaleslider1) {
		delete m_st_litscaleslider1;
		m_st_litscaleslider1 = nullptr;
	}
	if (m_st_litscalelabel2) {
		delete m_st_litscalelabel2;
		m_st_litscalelabel2 = nullptr;
	}
	if (m_st_litscaleslider2) {
		delete m_st_litscaleslider2;
		m_st_litscaleslider2 = nullptr;
	}

	if (m_st_litscalesp3_0) {
		delete m_st_litscalesp3_0;
		m_st_litscalesp3_0 = nullptr;
	}
	if (m_st_litscalesp3_1) {
		delete m_st_litscalesp3_1;
		m_st_litscalesp3_1 = nullptr;
	}
	if (m_st_litscalesp3_2) {
		delete m_st_litscalesp3_2;
		m_st_litscalesp3_2 = nullptr;
	}
	if (m_st_litscalelabel3) {
		delete m_st_litscalelabel3;
		m_st_litscalelabel3 = nullptr;
	}
	if (m_st_litscaleslider3) {
		delete m_st_litscaleslider3;
		m_st_litscaleslider3 = nullptr;
	}
	if (m_st_litscalelabel4) {
		delete m_st_litscalelabel4;
		m_st_litscalelabel4 = nullptr;
	}
	if (m_st_litscaleslider4) {
		delete m_st_litscaleslider4;
		m_st_litscaleslider4 = nullptr;
	}

	if (m_st_litscalesp5_0) {
		delete m_st_litscalesp5_0;
		m_st_litscalesp5_0 = nullptr;
	}
	if (m_st_litscalesp5_1) {
		delete m_st_litscalesp5_1;
		m_st_litscalesp5_1 = nullptr;
	}
	if (m_st_litscalesp5_2) {
		delete m_st_litscalesp5_2;
		m_st_litscalesp5_2 = nullptr;
	}
	if (m_st_litscalelabel5) {
		delete m_st_litscalelabel5;
		m_st_litscalelabel5 = nullptr;
	}
	if (m_st_litscaleslider5) {
		delete m_st_litscaleslider5;
		m_st_litscaleslider5 = nullptr;
	}
	if (m_st_litscalelabel6) {
		delete m_st_litscalelabel6;
		m_st_litscalelabel6 = nullptr;
	}
	if (m_st_litscaleslider6) {
		delete m_st_litscaleslider6;
		m_st_litscaleslider6 = nullptr;
	}

	if (m_st_litscalesp7_0) {
		delete m_st_litscalesp7_0;
		m_st_litscalesp7_0 = nullptr;
	}
	if (m_st_litscalesp7_1) {
		delete m_st_litscalesp7_1;
		m_st_litscalesp7_1 = nullptr;
	}
	if (m_st_litscalesp7_2) {
		delete m_st_litscalesp7_2;
		m_st_litscalesp7_2 = nullptr;
	}
	if (m_st_litscalelabel7) {
		delete m_st_litscalelabel7;
		m_st_litscalelabel7 = nullptr;
	}
	if (m_st_litscaleslider7) {
		delete m_st_litscaleslider7;
		m_st_litscaleslider7 = nullptr;
	}
	if (m_st_litscalelabel8) {
		delete m_st_litscalelabel8;
		m_st_litscalelabel8 = nullptr;
	}
	if (m_st_litscaleslider8) {
		delete m_st_litscaleslider8;
		m_st_litscaleslider8 = nullptr;
	}

	if (m_st_toonlitradio) {
		delete m_st_toonlitradio;
		m_st_toonlitradio = nullptr;
	}

	if (m_st_toonaddrsp0) {
		delete m_st_toonaddrsp0;
		m_st_toonaddrsp0 = nullptr;
	}
	if (m_st_toonaddrsp1) {
		delete m_st_toonaddrsp1;
		m_st_toonaddrsp1 = nullptr;
	}
	if (m_st_toonaddrsp2) {
		delete m_st_toonaddrsp2;
		m_st_toonaddrsp2 = nullptr;
	}
	if (m_st_toonhiaddrlabel) {
		delete m_st_toonhiaddrlabel;
		m_st_toonhiaddrlabel = nullptr;
	}
	if (m_st_toonhiaddrslider) {
		delete m_st_toonhiaddrslider;
		m_st_toonhiaddrslider = nullptr;
	}
	if (m_st_toonlowaddrlabel) {
		delete m_st_toonlowaddrlabel;
		m_st_toonlowaddrlabel = nullptr;
	}
	if (m_st_toonlowaddrslider) {
		delete m_st_toonlowaddrslider;
		m_st_toonlowaddrslider = nullptr;
	}

	if (m_st_gradationsp0) {
		delete m_st_gradationsp0;
		m_st_gradationsp0 = nullptr;
	}
	if (m_st_gradationchk) {
		delete m_st_gradationchk;
		m_st_gradationchk = nullptr;
	}
	if (m_st_powertoonchk) {
		delete m_st_powertoonchk;
		m_st_powertoonchk = nullptr;
	}

	if (m_st_toonbasesp1_0) {
		delete m_st_toonbasesp1_0;
		m_st_toonbasesp1_0 = nullptr;
	}
	if (m_st_toonbasesp1_1) {
		delete m_st_toonbasesp1_1;
		m_st_toonbasesp1_1 = nullptr;
	}
	if (m_st_toonbasesp1_2) {
		delete m_st_toonbasesp1_2;
		m_st_toonbasesp1_2 = nullptr;
	}
	if (m_st_toonbaseHlabel) {
		delete m_st_toonbaseHlabel;
		m_st_toonbaseHlabel = nullptr;
	}
	if (m_st_toonbaseHslider) {
		delete m_st_toonbaseHslider;
		m_st_toonbaseHslider = nullptr;
	}
	if (m_st_toonbaseSlabel) {
		delete m_st_toonbaseSlabel;
		m_st_toonbaseSlabel = nullptr;
	}
	if (m_st_toonbaseSslider) {
		delete m_st_toonbaseSslider;
		m_st_toonbaseSslider = nullptr;
	}
	if (m_st_toonbasesp2_0) {
		delete m_st_toonbasesp2_0;
		m_st_toonbasesp2_0 = nullptr;
	}
	if (m_st_toonbasesp2_1) {
		delete m_st_toonbasesp2_1;
		m_st_toonbasesp2_1 = nullptr;
	}
	if (m_st_toonbasesp2_2) {
		delete m_st_toonbasesp2_2;
		m_st_toonbasesp2_2 = nullptr;
	}
	if (m_st_toonbaseVlabel) {
		delete m_st_toonbaseVlabel;
		m_st_toonbaseVlabel = nullptr;
	}
	if (m_st_toonbaseVslider) {
		delete m_st_toonbaseVslider;
		m_st_toonbaseVslider = nullptr;
	}
	if (m_st_toonbaseAlabel) {
		delete m_st_toonbaseAlabel;
		m_st_toonbaseAlabel = nullptr;
	}
	if (m_st_toonbaseAslider) {
		delete m_st_toonbaseAslider;
		m_st_toonbaseAslider = nullptr;
	}

	if (m_st_toonhisp1_0) {
		delete m_st_toonhisp1_0;
		m_st_toonhisp1_0 = nullptr;
	}
	if (m_st_toonhisp1_1) {
		delete m_st_toonhisp1_1;
		m_st_toonhisp1_1 = nullptr;
	}
	if (m_st_toonhisp1_2) {
		delete m_st_toonhisp1_2;
		m_st_toonhisp1_2 = nullptr;
	}
	if (m_st_toonhiHlabel) {
		delete m_st_toonhiHlabel;
		m_st_toonhiHlabel = nullptr;
	}
	if (m_st_toonhiHslider) {
		delete m_st_toonhiHslider;
		m_st_toonhiHslider = nullptr;
	}
	if (m_st_toonhiSlabel) {
		delete m_st_toonhiSlabel;
		m_st_toonhiSlabel = nullptr;
	}
	if (m_st_toonhiSslider) {
		delete m_st_toonhiSslider;
		m_st_toonhiSslider = nullptr;
	}
	if (m_st_toonhisp2_0) {
		delete m_st_toonhisp2_0;
		m_st_toonhisp2_0 = nullptr;
	}
	if (m_st_toonhisp2_1) {
		delete m_st_toonhisp2_1;
		m_st_toonhisp2_1 = nullptr;
	}
	if (m_st_toonhisp2_2) {
		delete m_st_toonhisp2_2;
		m_st_toonhisp2_2 = nullptr;
	}
	if (m_st_toonhiVlabel) {
		delete m_st_toonhiVlabel;
		m_st_toonhiVlabel = nullptr;
	}
	if (m_st_toonhiVslider) {
		delete m_st_toonhiVslider;
		m_st_toonhiVslider = nullptr;
	}
	if (m_st_toonhiAlabel) {
		delete m_st_toonhiAlabel;
		m_st_toonhiAlabel = nullptr;
	}
	if (m_st_toonhiAslider) {
		delete m_st_toonhiAslider;
		m_st_toonhiAslider = nullptr;
	}

	if (m_st_toonlowsp1_0) {
		delete m_st_toonlowsp1_0;
		m_st_toonlowsp1_0 = nullptr;
	}
	if (m_st_toonlowsp1_1) {
		delete m_st_toonlowsp1_1;
		m_st_toonlowsp1_1 = nullptr;
	}
	if (m_st_toonlowsp1_2) {
		delete m_st_toonlowsp1_2;

		m_st_toonlowsp1_2 = nullptr;
	}
	if (m_st_toonlowHlabel) {
		delete m_st_toonlowHlabel;
		m_st_toonlowHlabel = nullptr;
	}
	if (m_st_toonlowHslider) {
		delete m_st_toonlowHslider;
		m_st_toonlowHslider = nullptr;
	}
	if (m_st_toonlowSlabel) {
		delete m_st_toonlowSlabel;
		m_st_toonlowSlabel = nullptr;
	}
	if (m_st_toonlowSslider) {
		delete m_st_toonlowSslider;
		m_st_toonlowSslider = nullptr;
	}
	if (m_st_toonlowsp2_0) {
		delete m_st_toonlowsp2_0;
		m_st_toonlowsp2_0 = nullptr;
	}
	if (m_st_toonlowsp2_1) {
		delete m_st_toonlowsp2_1;
		m_st_toonlowsp2_1 = nullptr;
	}
	if (m_st_toonlowsp2_2) {
		delete m_st_toonlowsp2_2;
		m_st_toonlowsp2_2 = nullptr;
	}
	if (m_st_toonlowVlabel) {
		delete m_st_toonlowVlabel;
		m_st_toonlowVlabel = nullptr;
	}
	if (m_st_toonlowVslider) {
		delete m_st_toonlowVslider;
		m_st_toonlowVslider = nullptr;
	}
	if (m_st_toonlowAlabel) {
		delete m_st_toonlowAlabel;
		m_st_toonlowAlabel = nullptr;
	}
	if (m_st_toonlowAslider) {
		delete m_st_toonlowAslider;
		m_st_toonlowAslider = nullptr;
	}

	if (m_st_tilingsp0) {
		delete m_st_tilingsp0;
		m_st_tilingsp0 = nullptr;
	}
	if (m_st_tilingsp1) {
		delete m_st_tilingsp1;
		m_st_tilingsp1 = nullptr;
	}
	if (m_st_tilingsp2) {
		delete m_st_tilingsp2;
		m_st_tilingsp2 = nullptr;
	}
	if (m_st_tilingUlabel) {
		delete m_st_tilingUlabel;
		m_st_tilingUlabel = nullptr;
	}
	if (m_st_tilingUslider) {
		delete m_st_tilingUslider;
		m_st_tilingUslider = nullptr;
	}
	if (m_st_tilingVlabel) {
		delete m_st_tilingVlabel;
		m_st_tilingVlabel = nullptr;
	}
	if (m_st_tilingVslider) {
		delete m_st_tilingVslider;
		m_st_tilingVslider = nullptr;
	}

	if (m_st_alphatestsp0) {
		delete m_st_alphatestsp0;
		m_st_alphatestsp0 = nullptr;
	}
	if (m_st_alphatestsp1) {
		delete m_st_alphatestsp1;
		m_st_alphatestsp1 = nullptr;
	}
	if (m_st_alphatestlabel) {
		delete m_st_alphatestlabel;
		m_st_alphatestlabel = nullptr;
	}
	if (m_st_alphatestslider) {
		delete m_st_alphatestslider;
		m_st_alphatestslider = nullptr;
	}

	if (m_st_distortionsp0) {
		delete m_st_distortionsp0;
		m_st_distortionsp0 = nullptr;
	}
	if (m_st_distortionsp1) {
		delete m_st_distortionsp1;
		m_st_distortionsp1 = nullptr;
	}
	if (m_st_distortionchk) {
		delete m_st_distortionchk;
		m_st_distortionchk = nullptr;
	}
	if (m_st_distortionscalelabel) {
		delete m_st_distortionscalelabel;
		m_st_distortionscalelabel = nullptr;
	}
	if (m_st_distortionscaleslider) {
		delete m_st_distortionscaleslider;
		m_st_distortionscaleslider = nullptr;
	}

	if (m_st_riverradio) {
		delete m_st_riverradio;
		m_st_riverradio = nullptr;
	}

	if (m_st_seacentersp0) {
		delete m_st_seacentersp0;
		m_st_seacentersp0 = nullptr;
	}
	if (m_st_seacentersp1) {
		delete m_st_seacentersp1;
		m_st_seacentersp1 = nullptr;
	}
	if (m_st_seacenterlabel) {
		delete m_st_seacenterlabel;
		m_st_seacenterlabel = nullptr;
	}
	if (m_st_seacenterUslider) {
		delete m_st_seacenterUslider;
		m_st_seacenterUslider = nullptr;
	}
	if (m_st_seacenterVslider) {
		delete m_st_seacenterVslider;
		m_st_seacenterVslider = nullptr;
	}

	if (m_st_riverdirsp0) {
		delete m_st_riverdirsp0;
		m_st_riverdirsp0 = nullptr;
	}
	if (m_st_riverdirsp1) {
		delete m_st_riverdirsp1;
		m_st_riverdirsp1 = nullptr;
	}
	if (m_st_riverdirlabel) {
		delete m_st_riverdirlabel;
		m_st_riverdirlabel = nullptr;
	}
	if (m_st_riverdirUslider) {
		delete m_st_riverdirUslider;
		m_st_riverdirUslider = nullptr;
	}
	if (m_st_riverdirVslider) {
		delete m_st_riverdirVslider;
		m_st_riverdirVslider = nullptr;
	}

	if (m_st_flowratesp0) {
		delete m_st_flowratesp0;
		m_st_flowratesp0 = nullptr;
	}
	if (m_st_flowratelabel) {
		delete m_st_flowratelabel;
		m_st_flowratelabel = nullptr;
	}
	if (m_st_flowrateslider) {
		delete m_st_flowrateslider;
		m_st_flowrateslider = nullptr;
	}

	if (m_st_distortionmapradio) {
		delete m_st_distortionmapradio;
		m_st_distortionmapradio = nullptr;
	}


	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CShaderParamsDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;

	m_toonmqomaterial = nullptr;//toonスライダーを離した後の処理用
	m_toonparamchange = false;//toonスライダーを離した後の処理用
	m_hsvtoonforall.Init();
	m_shadertypeparams.InitParams(m_hsvtoonforall);


	m_st_closeFlag = false;
	m_st_remakeToonTextureFlag = false;
	m_st_backFlag = false;
	m_st_stradioFlag = false;
	m_st_lightchkFlag = false;
	m_st_shadowcasterchkFlag = false;
	m_st_normaly0chkFlag = false;
	m_st_spccoefsliderFlag = false;
	m_st_emissionchkFlag = false;
	m_st_emissionsliderFlag = false;
	m_st_metalsliderFlag = false;
	m_st_smoothsliderFlag = false;
	m_st_litscale1Flag = false;
	m_st_litscale2Flag = false;
	m_st_litscale3Flag = false;
	m_st_litscale4Flag = false;
	m_st_litscale5Flag = false;
	m_st_litscale6Flag = false;
	m_st_litscale7Flag = false;
	m_st_litscale8Flag = false;
	m_st_toonlitradioFlag = false;
	m_st_toonhiaddrsliderFlag = false;
	m_st_toonlowaddrsliderFlag = false;
	m_st_gradationchkFlag = false;
	m_st_powertoonchkFlag = false;
	m_st_toonbaseHsliderFlag = false;
	m_st_toonbaseSsliderFlag = false;
	m_st_toonbaseVsliderFlag = false;
	m_st_toonbaseAsliderFlag = false;
	m_st_toonhiHsliderFlag = false;
	m_st_toonhiSsliderFlag = false;
	m_st_toonhiVsliderFlag = false;
	m_st_toonhiAsliderFlag = false;
	m_st_toonlowHsliderFlag = false;
	m_st_toonlowSsliderFlag = false;
	m_st_toonlowVsliderFlag = false;
	m_st_toonlowAsliderFlag = false;
	m_st_tilingUsliderFlag = false;
	m_st_tilingVsliderFlag = false;
	m_st_tilingUsliderUpFlag = false;
	m_st_tilingVsliderUpFlag = false;
	m_st_alphatestesliderFlag = false;
	m_st_distortionchkFlag = false;
	m_st_distortionscalesliderFlag = false;
	m_st_riverradioFlag = false;
	m_st_seacenterUsliderFlag = false;
	m_st_seacenterVsliderFlag = false;
	m_st_riverdirUsliderFlag = false;
	m_st_riverdirVsliderFlag = false;
	m_st_flowratesliderFlag = false;
	m_st_distortionmapradioFlag = false;


	m_dlgWnd = nullptr;
	m_st_Sc = nullptr;
	m_st_spall = nullptr;
	m_st_spacerLabel01 = nullptr;
	m_st_spacerLabel02 = nullptr;
	m_st_spacerLabel03 = nullptr;
	m_st_spacerLabel04 = nullptr;
	m_st_spacerLabel05 = nullptr;
	m_st_spacerLabel06 = nullptr;
	m_st_namesp = nullptr;
	m_st_backB = nullptr;
	m_st_namelabel = nullptr;
	m_st_shadertyperadio = nullptr;
	m_st_litflagsp = nullptr;
	m_st_lightflagchk = nullptr;
	m_st_shadowcasterchk = nullptr;
	m_st_normaly0chk = nullptr;
	m_st_spccoefsp0 = nullptr;
	m_st_spccoefsp1 = nullptr;
	m_st_spccoefsp2 = nullptr;
	m_st_spccoeflabel = nullptr;
	m_st_spccoefslider = nullptr;
	m_st_emissionchk = nullptr;
	m_st_emissionslider = nullptr;
	m_st_metalsp0 = nullptr;
	m_st_metalsp1 = nullptr;
	m_st_metalsp2 = nullptr;
	m_st_metallabel = nullptr;
	m_st_metalslider = nullptr;
	m_st_smoothlabel = nullptr;
	m_st_smoothslider = nullptr;

	m_st_litscalesp1_0 = nullptr;
	m_st_litscalesp1_1 = nullptr;
	m_st_litscalesp1_2 = nullptr;
	m_st_litscalelabel1 = nullptr;
	m_st_litscaleslider1 = nullptr;
	m_st_litscalelabel2 = nullptr;
	m_st_litscaleslider2 = nullptr;

	m_st_litscalesp3_0 = nullptr;
	m_st_litscalesp3_1 = nullptr;
	m_st_litscalesp3_2 = nullptr;
	m_st_litscalelabel3 = nullptr;
	m_st_litscaleslider3 = nullptr;
	m_st_litscalelabel4 = nullptr;
	m_st_litscaleslider4 = nullptr;

	m_st_litscalesp5_0 = nullptr;
	m_st_litscalesp5_1 = nullptr;
	m_st_litscalesp5_2 = nullptr;
	m_st_litscalelabel5 = nullptr;
	m_st_litscaleslider5 = nullptr;
	m_st_litscalelabel6 = nullptr;
	m_st_litscaleslider6 = nullptr;

	m_st_litscalesp7_0 = nullptr;
	m_st_litscalesp7_1 = nullptr;
	m_st_litscalesp7_2 = nullptr;
	m_st_litscalelabel7 = nullptr;
	m_st_litscaleslider7 = nullptr;
	m_st_litscalelabel8 = nullptr;
	m_st_litscaleslider8 = nullptr;

	m_st_toonlitradio = nullptr;

	m_st_toonaddrsp0 = nullptr;
	m_st_toonaddrsp1 = nullptr;
	m_st_toonaddrsp2 = nullptr;
	m_st_toonhiaddrlabel = nullptr;
	m_st_toonhiaddrslider = nullptr;
	m_st_toonlowaddrlabel = nullptr;
	m_st_toonlowaddrslider = nullptr;

	m_st_gradationsp0 = nullptr;
	m_st_gradationchk = nullptr;
	m_st_powertoonchk = nullptr;

	m_st_toonbasesp1_0 = nullptr;
	m_st_toonbasesp1_1 = nullptr;
	m_st_toonbasesp1_2 = nullptr;
	m_st_toonbaseHlabel = nullptr;
	m_st_toonbaseHslider = nullptr;
	m_st_toonbaseSlabel = nullptr;
	m_st_toonbaseSslider = nullptr;
	m_st_toonbasesp2_0 = nullptr;
	m_st_toonbasesp2_1 = nullptr;
	m_st_toonbasesp2_2 = nullptr;
	m_st_toonbaseVlabel = nullptr;
	m_st_toonbaseVslider = nullptr;
	m_st_toonbaseAlabel = nullptr;
	m_st_toonbaseAslider = nullptr;

	m_st_toonhisp1_0 = nullptr;
	m_st_toonhisp1_1 = nullptr;
	m_st_toonhisp1_2 = nullptr;
	m_st_toonhiHlabel = nullptr;
	m_st_toonhiHslider = nullptr;
	m_st_toonhiSlabel = nullptr;
	m_st_toonhiSslider = nullptr;
	m_st_toonhisp2_0 = nullptr;
	m_st_toonhisp2_1 = nullptr;
	m_st_toonhisp2_2 = nullptr;
	m_st_toonhiVlabel = nullptr;
	m_st_toonhiVslider = nullptr;
	m_st_toonhiAlabel = nullptr;
	m_st_toonhiAslider = nullptr;

	m_st_toonlowsp1_0 = nullptr;
	m_st_toonlowsp1_1 = nullptr;
	m_st_toonlowsp1_2 = nullptr;
	m_st_toonlowHlabel = nullptr;
	m_st_toonlowHslider = nullptr;
	m_st_toonlowSlabel = nullptr;
	m_st_toonlowSslider = nullptr;
	m_st_toonlowsp2_0 = nullptr;
	m_st_toonlowsp2_1 = nullptr;
	m_st_toonlowsp2_2 = nullptr;
	m_st_toonlowVlabel = nullptr;
	m_st_toonlowVslider = nullptr;
	m_st_toonlowAlabel = nullptr;
	m_st_toonlowAslider = nullptr;

	m_st_tilingsp0 = nullptr;
	m_st_tilingsp1 = nullptr;
	m_st_tilingsp2 = nullptr;
	m_st_tilingUlabel = nullptr;
	m_st_tilingUslider = nullptr;
	m_st_tilingVlabel = nullptr;
	m_st_tilingVslider = nullptr;

	m_st_alphatestsp0 = nullptr;
	m_st_alphatestsp1 = nullptr;
	m_st_alphatestlabel = nullptr;
	m_st_alphatestslider = nullptr;

	m_st_distortionsp0 = nullptr;
	m_st_distortionsp1 = nullptr;
	m_st_distortionchk = nullptr;
	m_st_distortionscalelabel = nullptr;
	m_st_distortionscaleslider = nullptr;

	m_st_riverradio = nullptr;

	m_st_seacentersp0 = nullptr;
	m_st_seacentersp1 = nullptr;
	m_st_seacenterlabel = nullptr;
	m_st_seacenterUslider = nullptr;
	m_st_seacenterVslider = nullptr;

	m_st_riverdirsp0 = nullptr;
	m_st_riverdirsp1 = nullptr;
	m_st_riverdirlabel = nullptr;
	m_st_riverdirUslider = nullptr;
	m_st_riverdirVslider = nullptr;

	m_st_flowratesp0 = nullptr;
	m_st_flowratelabel = nullptr;
	m_st_flowrateslider = nullptr;

	m_st_distortionmapradio = nullptr;

}

int CShaderParamsDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

void CShaderParamsDlg::SetModel(CModel* srcmodel)
{
	m_model = srcmodel;
	CreateShaderParamsWnd();
	ParamsToDlg(nullptr);

	if (m_dlgWnd && m_dlgWnd->getVisible()) {
		if (m_st_Sc) {
			m_st_Sc->autoResize();
		}
		m_dlgWnd->callRewrite();
	}
}


void CShaderParamsDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_st_Sc) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_st_Sc->autoResize();
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


int CShaderParamsDlg::CreateShaderParamsWnd()
{

	DestroyObjs();

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("ShaderTypeParamsWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("ShaderTypeParamsWindow"),	//タイトル
		g_mainhwnd,	//親ウィンドウハンドル
		false,					//表示・非表示状態
		//70, 50, 70,				//カラー
		0, 0, 0,				//カラー
		true, true);					//サイズ変更の可否

	int labelheight;
	if (g_4kresolution) {
		labelheight = 28;
	}
	else {
		labelheight = 20;
	}

	if (m_dlgWnd) {
		bool limitradionamelen = true;
		double separaterate = 0.26;
		double separateratelit = 0.35;

		m_st_Sc = new OWP_ScrollWnd(L"ShaderParamsScroll", false, labelheight);//wheelフラグOFF：ホイールでスライダーを動かしたいから
		if (!m_st_Sc) {
			_ASSERT(0);
			return 1;
		}
		int linedatasize = (int)(46.0 * 1.25);
		m_st_Sc->setLineDataSize(linedatasize);//!!!!!!!!!!!!
		m_dlgWnd->addParts(*m_st_Sc);


		//スクロールするGUI全てを束ねるセパレータは必要
		m_st_spall = new OWP_Separator(m_dlgWnd, true, 0.995, false, m_st_Sc);
		if (!m_st_spall) {
			_ASSERT(0);
			return 1;
		}
		m_st_Sc->addParts(*m_st_spall);

		m_st_spacerLabel01 = new OWP_Label(L"     ", labelheight);
		if (!m_st_spacerLabel01) {
			_ASSERT(0);
			return 1;
		}
		m_st_spacerLabel02 = new OWP_Label(L"     ", labelheight);
		if (!m_st_spacerLabel02) {
			_ASSERT(0);
			return 1;
		}
		m_st_spacerLabel03 = new OWP_Label(L"     ", labelheight);
		if (!m_st_spacerLabel03) {
			_ASSERT(0);
			return 1;
		}
		m_st_spacerLabel04 = new OWP_Label(L"     ", labelheight);
		if (!m_st_spacerLabel04) {
			_ASSERT(0);
			return 1;
		}
		m_st_spacerLabel05 = new OWP_Label(L"     ", labelheight);
		if (!m_st_spacerLabel05) {
			_ASSERT(0);
			return 1;
		}
		m_st_spacerLabel06 = new OWP_Label(L"     ", labelheight);
		if (!m_st_spacerLabel06) {
			_ASSERT(0);
			return 1;
		}
		m_st_namesp = new OWP_Separator(m_dlgWnd, true, 0.2, true);
		if (!m_st_namesp) {
			_ASSERT(0);
			return 1;
		}
		m_st_backB = new OWP_Button(L"<-Back", labelheight);
		if (!m_st_backB) {
			_ASSERT(0);
			return 1;
		}
		m_st_namelabel = new OWP_Label(L"MaterialName", labelheight);
		if (!m_st_namelabel) {
			_ASSERT(0);
			return 1;
		}
		//COLORREF colorToAll = RGB(64, 128 + 32, 128 + 32);
		//COLORREF colorValidInvalid = RGB(168, 129, 129);
		//COLORREF colorToDeeper = RGB(24, 126, 176);
		COLORREF colorCaution = RGB(168, 129, 129);
		m_st_namelabel->setTextColor(colorCaution);


		m_st_shadertyperadio = new OWP_RadioButton(L"Auto", limitradionamelen, labelheight);
		if (!m_st_shadertyperadio) {
			_ASSERT(0);
			return 1;
		}
		m_st_shadertyperadio->addLine(L"PBR");
		m_st_shadertyperadio->addLine(L"Std.");
		m_st_shadertyperadio->addLine(L"HSV Toon");


		m_st_litflagsp = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_litflagsp) {
			_ASSERT(0);
			return 1;
		}
		m_st_lightflagchk = new OWP_CheckBoxA(L"Lighting", 0, labelheight, false);
		if (!m_st_lightflagchk) {
			_ASSERT(0);
			return 1;
		}
		m_st_shadowcasterchk = new OWP_CheckBoxA(L"ShadowCaster", 0, labelheight, false);
		if (!m_st_shadowcasterchk) {
			_ASSERT(0);
			return 1;
		}
		m_st_normaly0chk = new OWP_CheckBoxA(L"NormalY0", 0, labelheight, false);
		if (!m_st_normaly0chk) {
			_ASSERT(0);
			return 1;
		}



		m_st_spccoefsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_spccoefsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_spccoefsp1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_spccoefsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_spccoefsp2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_spccoefsp2) {
			_ASSERT(0);
			return 1;
		}
		m_st_spccoeflabel = new OWP_Label(L"Specular", labelheight);
		if (!m_st_spccoeflabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_spccoefslider = new OWP_Slider(0.0, 2.0, 0.0, labelheight);
		if (!m_st_spccoefslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_emissionchk = new OWP_CheckBoxA(L"Emi", 0, labelheight, false);
		if (!m_st_emissionchk) {
			_ASSERT(0);
			return 1;
		}
		//m_st_emissionslider = new OWP_Slider(0.0, 1.0, 0.0);
		m_st_emissionslider = new OWP_Slider(0.0, 10.0, 0.0, labelheight);//2024/06/23 max 10.0
		if (!m_st_emissionslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_metalsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_metalsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_metalsp1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_metalsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_metalsp2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_metalsp2) {
			_ASSERT(0);
			return 1;
		}
		m_st_metallabel = new OWP_Label(L"Metal", labelheight);
		if (!m_st_metallabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_metalslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_metalslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_smoothlabel = new OWP_Label(L"Smooth", labelheight);
		if (!m_st_smoothlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_smoothslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_smoothslider) {
			_ASSERT(0);
			return 1;
		}



		m_st_litscalesp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_litscalesp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp1_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp1_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel1 = new OWP_Label(L"Lit1", labelheight);
		if (!m_st_litscalelabel1) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider1 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider1) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel2 = new OWP_Label(L"Lit2", labelheight);
		if (!m_st_litscalelabel2) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider2 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider2) {
			_ASSERT(0);
			return 1;
		}


		m_st_litscalesp3_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_litscalesp3_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp3_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp3_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp3_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp3_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel3 = new OWP_Label(L"Lit3", labelheight);
		if (!m_st_litscalelabel3) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider3 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider3) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel4 = new OWP_Label(L"Lit4", labelheight);
		if (!m_st_litscalelabel4) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider4 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider4) {
			_ASSERT(0);
			return 1;
		}



		m_st_litscalesp5_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_litscalesp5_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp5_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp5_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp5_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp5_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel5 = new OWP_Label(L"Lit5", labelheight);
		if (!m_st_litscalelabel5) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider5 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider5) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel6 = new OWP_Label(L"Lit6", labelheight);
		if (!m_st_litscalelabel6) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider6 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider6) {
			_ASSERT(0);
			return 1;
		}



		m_st_litscalesp7_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_litscalesp7_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp7_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp7_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalesp7_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_st_litscalesp7_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel7 = new OWP_Label(L"Lit7", labelheight);
		if (!m_st_litscalelabel7) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider7 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider7) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscalelabel8 = new OWP_Label(L"Lit8", labelheight);
		if (!m_st_litscalelabel8) {
			_ASSERT(0);
			return 1;
		}
		m_st_litscaleslider8 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_st_litscaleslider8) {
			_ASSERT(0);
			return 1;
		}


		m_st_toonlitradio = new OWP_RadioButton(L"ToonLit1", limitradionamelen, labelheight);
		if (!m_st_toonlitradio) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlitradio->addLine(L"ToonLit2");
		m_st_toonlitradio->addLine(L"ToonLit3");
		m_st_toonlitradio->addLine(L"ToonLit4");
		m_st_toonlitradio->addLine(L"ToonLit5");
		m_st_toonlitradio->addLine(L"ToonLit6");
		m_st_toonlitradio->addLine(L"ToonLit7");
		m_st_toonlitradio->addLine(L"ToonLit8");


		m_st_toonaddrsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonaddrsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonaddrsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonaddrsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonaddrsp2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonaddrsp2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiaddrlabel = new OWP_Label(L"HiAddr", labelheight);
		if (!m_st_toonhiaddrlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiaddrslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_toonhiaddrslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowaddrlabel = new OWP_Label(L"LowAddr", labelheight);
		if (!m_st_toonlowaddrlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowaddrslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_litscaleslider8) {
			_ASSERT(0);
			return 1;
		}


		m_st_gradationsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_gradationsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_gradationchk = new OWP_CheckBoxA(L"Gradation", 0, labelheight, false);
		if (!m_st_gradationchk) {
			_ASSERT(0);
			return 1;
		}
		m_st_powertoonchk = new OWP_CheckBoxA(L"PowerToon", 0, labelheight, false);
		if (!m_st_powertoonchk) {
			_ASSERT(0);
			return 1;
		}



		m_st_toonbasesp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonbasesp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbasesp1_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonbasesp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbasesp1_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonbasesp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseHlabel = new OWP_Label(L"BaseH", labelheight);
		if (!m_st_toonbaseHlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseHslider = new OWP_Slider(0.0, 360.0, 0.0, labelheight);
		if (!m_st_toonbaseHslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseSlabel = new OWP_Label(L"BaseS", labelheight);
		if (!m_st_toonbaseSlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseSslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_toonbaseSslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbasesp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonbasesp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbasesp2_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonbasesp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbasesp2_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonbasesp2_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseVlabel = new OWP_Label(L"BaseV", labelheight);
		if (!m_st_toonbaseVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseVslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_toonbaseVslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseAlabel = new OWP_Label(L"BaseA", labelheight);
		if (!m_st_toonbaseAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonbaseAslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_toonbaseAslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_toonhisp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonhisp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhisp1_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonhisp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhisp1_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonhisp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiHlabel = new OWP_Label(L"HighH", labelheight);
		if (!m_st_toonhiHlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiHslider = new OWP_Slider(0.0, 360.0, -360.0, labelheight);
		if (!m_st_toonhiHslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiSlabel = new OWP_Label(L"HighS", labelheight);
		if (!m_st_toonhiSlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiSslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_st_toonhiSslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhisp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonhisp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhisp2_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonhisp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhisp2_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonhisp2_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiVlabel = new OWP_Label(L"HighV", labelheight);
		if (!m_st_toonhiVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiVslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_st_toonhiVslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiAlabel = new OWP_Label(L"HighA", labelheight);
		if (!m_st_toonhiAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonhiAslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_st_toonhiAslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_toonlowsp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonlowsp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowsp1_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonlowsp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowsp1_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonlowsp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowHlabel = new OWP_Label(L"LowH", labelheight);
		if (!m_st_toonlowHlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowHslider = new OWP_Slider(0.0, 360.0, -360.0, labelheight);
		if (!m_st_toonlowHslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowSlabel = new OWP_Label(L"LowS", labelheight);
		if (!m_st_toonlowSlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowSslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_st_toonlowSslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowsp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_toonlowsp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowsp2_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonlowsp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowsp2_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_toonlowsp2_2) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowVlabel = new OWP_Label(L"LowV", labelheight);
		if (!m_st_toonlowVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowVslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_st_toonlowVslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowAlabel = new OWP_Label(L"LowA", labelheight);
		if (!m_st_toonlowAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_toonlowAslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_st_toonlowAslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_tilingsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_tilingsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_tilingsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_tilingsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_tilingsp2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_tilingsp2) {
			_ASSERT(0);
			return 1;
		}
		m_st_tilingUlabel = new OWP_Label(L"TileU", labelheight);
		if (!m_st_tilingUlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_tilingUslider = new OWP_Slider(0.0, 100.0, 0.0, labelheight);
		if (!m_st_tilingUslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_tilingVlabel = new OWP_Label(L"TileV", labelheight);
		if (!m_st_tilingVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_tilingVslider = new OWP_Slider(0.0, 100.0, 0.0, labelheight);
		if (!m_st_tilingVslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_alphatestsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_alphatestsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_alphatestsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_alphatestsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_alphatestlabel = new OWP_Label(L"ATest", labelheight);
		if (!m_st_alphatestlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_alphatestslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_alphatestslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_distortionsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_distortionsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_distortionsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_distortionsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_distortionchk = new OWP_CheckBoxA(L"SimpleWater(PBR,NoSkin)", 0, labelheight, false);
		if (!m_st_distortionchk) {
			_ASSERT(0);
			return 1;
		}
		m_st_distortionscalelabel = new OWP_Label(L"Scale", labelheight);
		if (!m_st_distortionscalelabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_distortionscaleslider = new OWP_Slider(1.0, 10.0, 0.0, labelheight);
		if (!m_st_distortionscaleslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_riverradio = new OWP_RadioButton(L"river", limitradionamelen, labelheight);
		if (!m_st_riverradio) {
			_ASSERT(0);
			return 1;
		}
		m_st_riverradio->addLine(L"sea");


		m_st_seacentersp0 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_seacentersp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_seacentersp1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_seacentersp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_seacenterlabel = new OWP_Label(L"SeaCenterUV", labelheight);
		if (!m_st_seacenterlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_seacenterUslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_seacenterUslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_seacenterVslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_seacenterVslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_riverdirsp0 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_st_riverdirsp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_riverdirsp1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_riverdirsp1) {
			_ASSERT(0);
			return 1;
		}
		m_st_riverdirlabel = new OWP_Label(L"FlowDirUV", labelheight);
		if (!m_st_riverdirlabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_riverdirUslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_riverdirUslider) {
			_ASSERT(0);
			return 1;
		}
		m_st_riverdirVslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_st_riverdirVslider) {
			_ASSERT(0);
			return 1;
		}


		m_st_flowratesp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_st_flowratesp0) {
			_ASSERT(0);
			return 1;
		}
		m_st_flowratelabel = new OWP_Label(L"FlowRate", labelheight);
		if (!m_st_flowratelabel) {
			_ASSERT(0);
			return 1;
		}
		m_st_flowrateslider = new OWP_Slider(1.0, 4.0, -4.0, labelheight);
		if (!m_st_flowrateslider) {
			_ASSERT(0);
			return 1;
		}



		m_st_distortionmapradio = new OWP_RadioButton(L"RG", limitradionamelen, labelheight);
		if (!m_st_distortionmapradio) {
			_ASSERT(0);
			return 1;
		}
		m_st_distortionmapradio->addLine(L"GB");
		m_st_distortionmapradio->addLine(L"RB");


		m_st_spall->addParts1(*m_st_namesp);
		m_st_namesp->addParts1(*m_st_backB);
		m_st_namesp->addParts2(*m_st_namelabel);
		m_st_spall->addParts1(*m_st_spacerLabel01);

		m_st_spall->addParts1(*m_st_shadertyperadio);
		m_st_spall->addParts1(*m_st_spacerLabel02);

		m_st_spall->addParts1(*m_st_litflagsp);
		m_st_litflagsp->addParts1(*m_st_lightflagchk);
		m_st_litflagsp->addParts2(*m_st_shadowcasterchk);
		m_st_spall->addParts1(*m_st_normaly0chk);
		m_st_spall->addParts1(*m_st_spacerLabel03);

		m_st_spall->addParts1(*m_st_spccoefsp0);
		m_st_spccoefsp0->addParts1(*m_st_spccoefsp1);
		m_st_spccoefsp1->addParts1(*m_st_spccoeflabel);
		m_st_spccoefsp1->addParts2(*m_st_spccoefslider);
		m_st_spccoefsp0->addParts2(*m_st_spccoefsp2);
		m_st_spccoefsp2->addParts1(*m_st_emissionchk);
		m_st_spccoefsp2->addParts2(*m_st_emissionslider);

		m_st_spall->addParts1(*m_st_metalsp0);
		m_st_metalsp0->addParts1(*m_st_metalsp1);
		m_st_metalsp1->addParts1(*m_st_metallabel);
		m_st_metalsp1->addParts2(*m_st_metalslider);
		m_st_metalsp0->addParts2(*m_st_metalsp2);
		m_st_metalsp2->addParts1(*m_st_smoothlabel);
		m_st_metalsp2->addParts2(*m_st_smoothslider);

		m_st_spall->addParts1(*m_st_litscalesp1_0);
		m_st_litscalesp1_0->addParts1(*m_st_litscalesp1_1);
		m_st_litscalesp1_1->addParts1(*m_st_litscalelabel1);
		m_st_litscalesp1_1->addParts2(*m_st_litscaleslider1);
		m_st_litscalesp1_0->addParts2(*m_st_litscalesp1_2);
		m_st_litscalesp1_2->addParts1(*m_st_litscalelabel2);
		m_st_litscalesp1_2->addParts2(*m_st_litscaleslider2);

		m_st_spall->addParts1(*m_st_litscalesp3_0);
		m_st_litscalesp3_0->addParts1(*m_st_litscalesp3_1);
		m_st_litscalesp3_1->addParts1(*m_st_litscalelabel3);
		m_st_litscalesp3_1->addParts2(*m_st_litscaleslider3);
		m_st_litscalesp3_0->addParts2(*m_st_litscalesp3_2);
		m_st_litscalesp3_2->addParts1(*m_st_litscalelabel4);
		m_st_litscalesp3_2->addParts2(*m_st_litscaleslider4);


		m_st_spall->addParts1(*m_st_litscalesp5_0);
		m_st_litscalesp5_0->addParts1(*m_st_litscalesp5_1);
		m_st_litscalesp5_1->addParts1(*m_st_litscalelabel5);
		m_st_litscalesp5_1->addParts2(*m_st_litscaleslider5);
		m_st_litscalesp5_0->addParts2(*m_st_litscalesp5_2);
		m_st_litscalesp5_2->addParts1(*m_st_litscalelabel6);
		m_st_litscalesp5_2->addParts2(*m_st_litscaleslider6);


		m_st_spall->addParts1(*m_st_litscalesp7_0);
		m_st_litscalesp7_0->addParts1(*m_st_litscalesp7_1);
		m_st_litscalesp7_1->addParts1(*m_st_litscalelabel7);
		m_st_litscalesp7_1->addParts2(*m_st_litscaleslider7);
		m_st_litscalesp7_0->addParts2(*m_st_litscalesp7_2);
		m_st_litscalesp7_2->addParts1(*m_st_litscalelabel8);
		m_st_litscalesp7_2->addParts2(*m_st_litscaleslider8);
		m_st_spall->addParts1(*m_st_spacerLabel04);

		m_st_spall->addParts1(*m_st_toonlitradio);
		m_st_spall->addParts1(*m_st_spacerLabel05);

		m_st_spall->addParts1(*m_st_toonaddrsp0);
		m_st_toonaddrsp0->addParts1(*m_st_toonaddrsp1);
		m_st_toonaddrsp1->addParts1(*m_st_toonhiaddrlabel);
		m_st_toonaddrsp1->addParts2(*m_st_toonhiaddrslider);
		m_st_toonaddrsp0->addParts2(*m_st_toonaddrsp2);
		m_st_toonaddrsp2->addParts1(*m_st_toonlowaddrlabel);
		m_st_toonaddrsp2->addParts2(*m_st_toonlowaddrslider);


		m_st_spall->addParts1(*m_st_gradationsp0);
		m_st_gradationsp0->addParts1(*m_st_gradationchk);
		m_st_gradationsp0->addParts2(*m_st_powertoonchk);


		m_st_spall->addParts1(*m_st_toonbasesp1_0);
		m_st_toonbasesp1_0->addParts1(*m_st_toonbasesp1_1);
		m_st_toonbasesp1_1->addParts1(*m_st_toonbaseHlabel);
		m_st_toonbasesp1_1->addParts2(*m_st_toonbaseHslider);
		m_st_toonbasesp1_0->addParts2(*m_st_toonbasesp1_2);
		m_st_toonbasesp1_2->addParts1(*m_st_toonbaseSlabel);
		m_st_toonbasesp1_2->addParts2(*m_st_toonbaseSslider);
		m_st_spall->addParts1(*m_st_toonbasesp2_0);
		m_st_toonbasesp2_0->addParts1(*m_st_toonbasesp2_1);
		m_st_toonbasesp2_1->addParts1(*m_st_toonbaseVlabel);
		m_st_toonbasesp2_1->addParts2(*m_st_toonbaseVslider);
		m_st_toonbasesp2_0->addParts2(*m_st_toonbasesp2_2);
		m_st_toonbasesp2_2->addParts1(*m_st_toonbaseAlabel);
		m_st_toonbasesp2_2->addParts2(*m_st_toonbaseAslider);


		m_st_spall->addParts1(*m_st_toonhisp1_0);
		m_st_toonhisp1_0->addParts1(*m_st_toonhisp1_1);
		m_st_toonhisp1_1->addParts1(*m_st_toonhiHlabel);
		m_st_toonhisp1_1->addParts2(*m_st_toonhiHslider);
		m_st_toonhisp1_0->addParts2(*m_st_toonhisp1_2);
		m_st_toonhisp1_2->addParts1(*m_st_toonhiSlabel);
		m_st_toonhisp1_2->addParts2(*m_st_toonhiSslider);
		m_st_spall->addParts1(*m_st_toonhisp2_0);
		m_st_toonhisp2_0->addParts1(*m_st_toonhisp2_1);
		m_st_toonhisp2_1->addParts1(*m_st_toonhiVlabel);
		m_st_toonhisp2_1->addParts2(*m_st_toonhiVslider);
		m_st_toonhisp2_0->addParts2(*m_st_toonhisp2_2);
		m_st_toonhisp2_2->addParts1(*m_st_toonhiAlabel);
		m_st_toonhisp2_2->addParts2(*m_st_toonhiAslider);


		m_st_spall->addParts1(*m_st_toonlowsp1_0);
		m_st_toonlowsp1_0->addParts1(*m_st_toonlowsp1_1);
		m_st_toonlowsp1_1->addParts1(*m_st_toonlowHlabel);
		m_st_toonlowsp1_1->addParts2(*m_st_toonlowHslider);
		m_st_toonlowsp1_0->addParts2(*m_st_toonlowsp1_2);
		m_st_toonlowsp1_2->addParts1(*m_st_toonlowSlabel);
		m_st_toonlowsp1_2->addParts2(*m_st_toonlowSslider);
		m_st_spall->addParts1(*m_st_toonlowsp2_0);
		m_st_toonlowsp2_0->addParts1(*m_st_toonlowsp2_1);
		m_st_toonlowsp2_1->addParts1(*m_st_toonlowVlabel);
		m_st_toonlowsp2_1->addParts2(*m_st_toonlowVslider);
		m_st_toonlowsp2_0->addParts2(*m_st_toonlowsp2_2);
		m_st_toonlowsp2_2->addParts1(*m_st_toonlowAlabel);
		m_st_toonlowsp2_2->addParts2(*m_st_toonlowAslider);


		m_st_spall->addParts1(*m_st_tilingsp0);
		m_st_tilingsp0->addParts1(*m_st_tilingsp1);
		m_st_tilingsp1->addParts1(*m_st_tilingUlabel);
		m_st_tilingsp1->addParts2(*m_st_tilingUslider);
		m_st_tilingsp0->addParts2(*m_st_tilingsp2);
		m_st_tilingsp2->addParts1(*m_st_tilingVlabel);
		m_st_tilingsp2->addParts2(*m_st_tilingVslider);

		m_st_spall->addParts1(*m_st_alphatestsp0);
		m_st_alphatestsp0->addParts1(*m_st_alphatestsp1);
		m_st_alphatestsp1->addParts1(*m_st_alphatestlabel);
		m_st_alphatestsp1->addParts2(*m_st_alphatestslider);
		m_st_spall->addParts1(*m_st_spacerLabel06);

		m_st_spall->addParts1(*m_st_distortionsp0);
		m_st_distortionsp0->addParts1(*m_st_distortionchk);
		m_st_distortionsp0->addParts2(*m_st_distortionsp1);
		m_st_distortionsp1->addParts1(*m_st_distortionscalelabel);
		m_st_distortionsp1->addParts2(*m_st_distortionscaleslider);

		m_st_spall->addParts1(*m_st_riverradio);

		//m_st_spall->addParts1(*m_st_seacentersp0);
		//m_st_seacentersp0->addParts1(*m_st_seacenterlabel);
		//m_st_seacentersp0->addParts2(*m_st_seacentersp1);
		//m_st_seacentersp1->addParts1(*m_st_seacenterUslider);
		//m_st_seacentersp1->addParts2(*m_st_seacenterVslider);

		m_st_spall->addParts1(*m_st_riverdirsp0);
		m_st_riverdirsp0->addParts1(*m_st_riverdirlabel);
		m_st_riverdirsp0->addParts2(*m_st_riverdirsp1);
		m_st_riverdirsp1->addParts1(*m_st_riverdirUslider);
		m_st_riverdirsp1->addParts2(*m_st_riverdirVslider);


		m_st_spall->addParts1(*m_st_flowratesp0);
		m_st_flowratesp0->addParts1(*m_st_flowratelabel);
		m_st_flowratesp0->addParts2(*m_st_flowrateslider);

		m_st_spall->addParts1(*m_st_distortionmapradio);





		if (m_dlgWnd) {
			m_dlgWnd->setCloseListener([=, this]() {
				if (m_model) {
					m_st_closeFlag = true;
				}
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_backB) {
			m_st_backB->setButtonListener([=, this]() {
				m_st_backFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_shadertyperadio) {
			m_st_shadertyperadio->setSelectListener([=, this]() {
				m_st_stradioFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_lightflagchk) {
			m_st_lightflagchk->setButtonListener([=, this]() {
				m_st_lightchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_shadowcasterchk) {
			m_st_shadowcasterchk->setButtonListener([=, this]() {
				m_st_shadowcasterchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_normaly0chk) {
			m_st_normaly0chk->setButtonListener([=, this]() {
				m_st_normaly0chkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_spccoefslider) {
			m_st_spccoefslider->setCursorListener([=, this]() {
				m_st_spccoefsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_emissionchk) {
			m_st_emissionchk->setButtonListener([=, this]() {
				m_st_emissionchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_emissionslider) {
			m_st_emissionslider->setCursorListener([=, this]() {
				m_st_emissionsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_metalslider) {
			m_st_metalslider->setCursorListener([=, this]() {
				m_st_metalsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_smoothslider) {
			m_st_smoothslider->setCursorListener([=, this]() {
				m_st_smoothsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider1) {
			m_st_litscaleslider1->setCursorListener([=, this]() {
				m_st_litscale1Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider2) {
			m_st_litscaleslider2->setCursorListener([=, this]() {
				m_st_litscale2Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider3) {
			m_st_litscaleslider3->setCursorListener([=, this]() {
				m_st_litscale3Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider4) {
			m_st_litscaleslider4->setCursorListener([=, this]() {
				m_st_litscale4Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider5) {
			m_st_litscaleslider5->setCursorListener([=, this]() {
				m_st_litscale5Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider6) {
			m_st_litscaleslider6->setCursorListener([=, this]() {
				m_st_litscale6Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider7) {
			m_st_litscaleslider7->setCursorListener([=, this]() {
				m_st_litscale7Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_litscaleslider8) {
			m_st_litscaleslider8->setCursorListener([=, this]() {
				m_st_litscale8Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlitradio) {
			m_st_toonlitradio->setSelectListener([=, this]() {
				m_st_toonlitradioFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiaddrslider) {
			m_st_toonhiaddrslider->setCursorListener([=, this]() {
				m_st_toonhiaddrsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_st_toonhiaddrslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowaddrslider) {
			m_st_toonlowaddrslider->setCursorListener([=, this]() {
				m_st_toonlowaddrsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_st_toonlowaddrslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_gradationchk) {
			m_st_gradationchk->setButtonListener([=, this]() {
				m_st_gradationchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_powertoonchk) {
			m_st_powertoonchk->setButtonListener([=, this]() {
				m_st_powertoonchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseHslider) {
			m_st_toonbaseHslider->setCursorListener([=, this]() {
				m_st_toonbaseHsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseSslider) {
			m_st_toonbaseSslider->setCursorListener([=, this]() {
				m_st_toonbaseSsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseVslider) {
			m_st_toonbaseVslider->setCursorListener([=, this]() {
				m_st_toonbaseVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseAslider) {
			m_st_toonbaseAslider->setCursorListener([=, this]() {
				m_st_toonbaseAsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseHslider) {
			m_st_toonbaseHslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseSslider) {
			m_st_toonbaseSslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseVslider) {
			m_st_toonbaseVslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonbaseAslider) {
			m_st_toonbaseAslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_toonhiHslider) {
			m_st_toonhiHslider->setCursorListener([=, this]() {
				m_st_toonhiHsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiSslider) {
			m_st_toonhiSslider->setCursorListener([=, this]() {
				m_st_toonhiSsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiVslider) {
			m_st_toonhiVslider->setCursorListener([=, this]() {
				m_st_toonhiVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiAslider) {
			m_st_toonhiAslider->setCursorListener([=, this]() {
				m_st_toonhiAsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiHslider) {
			m_st_toonhiHslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiSslider) {
			m_st_toonhiSslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiVslider) {
			m_st_toonhiVslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonhiAslider) {
			m_st_toonhiAslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_toonlowHslider) {
			m_st_toonlowHslider->setCursorListener([=, this]() {
				m_st_toonlowHsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowSslider) {
			m_st_toonlowSslider->setCursorListener([=, this]() {
				m_st_toonlowSsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowVslider) {
			m_st_toonlowVslider->setCursorListener([=, this]() {
				m_st_toonlowVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowAslider) {
			m_st_toonlowAslider->setCursorListener([=, this]() {
				m_st_toonlowAsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowHslider) {
			m_st_toonlowHslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowSslider) {
			m_st_toonlowSslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowVslider) {
			m_st_toonlowVslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_toonlowAslider) {
			m_st_toonlowAslider->setLUpListener([=, this]() {
				m_st_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_tilingUslider) {
			m_st_tilingUslider->setCursorListener([=, this]() {
				m_st_tilingUsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_st_tilingUslider->setLUpListener([=, this]() {
				m_st_tilingUsliderUpFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_tilingVslider) {
			m_st_tilingVslider->setCursorListener([=, this]() {
				m_st_tilingVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_st_tilingVslider->setLUpListener([=, this]() {
				m_st_tilingVsliderUpFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_alphatestslider) {
			m_st_alphatestslider->setCursorListener([=, this]() {
				m_st_alphatestesliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_distortionchk) {
			m_st_distortionchk->setButtonListener([=, this]() {
				m_st_distortionchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_distortionscaleslider) {
			m_st_distortionscaleslider->setCursorListener([=, this]() {
				m_st_distortionscalesliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_riverradio) {
			m_st_riverradio->setSelectListener([=, this]() {
				m_st_riverradioFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_seacenterUslider) {
			m_st_seacenterUslider->setCursorListener([=, this]() {
				m_st_seacenterUsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_seacenterVslider) {
			m_st_seacenterVslider->setCursorListener([=, this]() {
				m_st_seacenterVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_riverdirUslider) {
			m_st_riverdirUslider->setCursorListener([=, this]() {
				m_st_riverdirUsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_st_riverdirVslider) {
			m_st_riverdirVslider->setCursorListener([=, this]() {
				m_st_riverdirVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_flowrateslider) {
			m_st_flowrateslider->setCursorListener([=, this]() {
				m_st_flowratesliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_st_distortionmapradio) {
			m_st_distortionmapradio->setSelectListener([=, this]() {
				m_st_distortionmapradioFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}



		m_dlgWnd->setSize(WindowSize(m_sizex, m_sizey));
		m_dlgWnd->setPos(WindowPos(m_posx, m_posy));

		//１クリック目問題対応
		m_dlgWnd->refreshPosAndSize();//2022/09/20


		m_dlgWnd->callRewrite();						//再描画
		//m_dlgWnd->setVisible(false);


		//m_rcshadertypeparamswnd.top = m_sidemenuheight;
		//m_rcshadertypeparamswnd.left = 0;
		//m_rcshadertypeparamswnd.bottom = m_sideheight;
		//m_rcshadertypeparamswnd.right = m_sidewidth;


	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CShaderParamsDlg::ParamsToDlg(CMQOMaterial* srcmat)
{
	//############################################################
	//srcmat == nullptrの場合にはm_modelの最初のマテリアルシェーダを表示
	//############################################################

	if (!m_model || (m_dlgWnd == NULL)) {
		_ASSERT(0);
		return 1;
	}


	int materialnum = m_model->GetMQOMaterialSize();
	if (materialnum == 0) {
		return 0;
	}
	if ((materialnum < 0) || (materialnum >= MAXMATERIALNUM)) {
		//2024/03/03
		::MessageBoxW(NULL, L"ERROR : MaterialNum Overflow.", L"Can't open dialog for settings.",
			MB_OK | MB_ICONERROR);

		m_dlgWnd->setListenMouse(false);
		m_dlgWnd->setVisible(false);

		return 1;
	}

	m_shadertypeparams.SetMaterial(srcmat);

	//######
	//Text
	//######
	if (m_st_namelabel) {
		if (srcmat) {
			WCHAR wmatname[MAX_PATH] = { 0L };
			char matname[MAX_PATH] = { 0 };
			strcpy_s(matname, MAX_PATH, srcmat->GetName());
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, matname, MAX_PATH, wmatname, MAX_PATH);
			m_st_namelabel->setName(wmatname);
		}
		else {
			m_st_namelabel->setName(L"All");
		}
	}

	//#######
	//Button
	//#######
	if (m_st_shadertyperadio) {
		switch (m_shadertypeparams.shadertype) {
		case -1:
			m_st_shadertyperadio->setSelectIndex(0, false);
			break;
		case MQOSHADER_PBR://case 0
			m_st_shadertyperadio->setSelectIndex(1, false);
			break;
		case MQOSHADER_STD://case 1
			m_st_shadertyperadio->setSelectIndex(2, false);
			break;
		case MQOSHADER_TOON://case 2
			m_st_shadertyperadio->setSelectIndex(3, false);
			break;
		default:
			_ASSERT(0);
			m_st_shadertyperadio->setSelectIndex(0, false);
			break;
		}
	}

	if (m_st_toonlitradio) {
		if ((m_shadertypeparams.hsvtoon.lightindex >= 0) && (m_shadertypeparams.hsvtoon.lightindex <= 7)) {
			m_st_toonlitradio->setSelectIndex(m_shadertypeparams.hsvtoon.lightindex, false);
		}
		else {
			_ASSERT(0);
			m_st_toonlitradio->setSelectIndex(0, false);
		}
	}

	if (m_st_riverradio) {
		if ((m_shadertypeparams.riverorsea >= 0) && (m_shadertypeparams.riverorsea <= 1)) {
			m_st_riverradio->setSelectIndex(m_shadertypeparams.riverorsea, false);
		}
		else {
			_ASSERT(0);
			m_st_riverradio->setSelectIndex(0, false);
		}
	}

	if (m_st_distortionmapradio) {
		if ((m_shadertypeparams.distortionmaptype >= 0) && (m_shadertypeparams.distortionmaptype <= 2)) {
			m_st_distortionmapradio->setSelectIndex(m_shadertypeparams.distortionmaptype, false);
		}
		else {
			_ASSERT(0);
			m_st_distortionmapradio->setSelectIndex(0, false);
		}
	}


	//#######
	//Slider
	//#######
	if (m_st_metalslider) {
		m_st_metalslider->setValue(m_shadertypeparams.metalcoef, false);
	}
	if (m_st_smoothslider) {
		m_st_smoothslider->setValue(m_shadertypeparams.smoothcoef, false);
	}


	if (m_st_litscaleslider1) {
		m_st_litscaleslider1->setValue(m_shadertypeparams.lightscale[0], false);
	}
	if (m_st_litscaleslider2) {
		m_st_litscaleslider2->setValue(m_shadertypeparams.lightscale[1], false);
	}
	if (m_st_litscaleslider3) {
		m_st_litscaleslider3->setValue(m_shadertypeparams.lightscale[2], false);
	}
	if (m_st_litscaleslider4) {
		m_st_litscaleslider4->setValue(m_shadertypeparams.lightscale[3], false);
	}
	if (m_st_litscaleslider5) {
		m_st_litscaleslider5->setValue(m_shadertypeparams.lightscale[4], false);
	}
	if (m_st_litscaleslider6) {
		m_st_litscaleslider6->setValue(m_shadertypeparams.lightscale[5], false);
	}
	if (m_st_litscaleslider7) {
		m_st_litscaleslider7->setValue(m_shadertypeparams.lightscale[6], false);
	}
	if (m_st_litscaleslider8) {
		m_st_litscaleslider8->setValue(m_shadertypeparams.lightscale[7], false);
	}

	if (m_st_emissionslider) {
		m_st_emissionslider->setValue(m_shadertypeparams.emissiveScale, false);
	}

	if (m_st_toonhiaddrslider) {
		m_st_toonhiaddrslider->setValue(m_shadertypeparams.hsvtoon.hicolorh, false);
	}
	if (m_st_toonlowaddrslider) {
		m_st_toonlowaddrslider->setValue(m_shadertypeparams.hsvtoon.lowcolorh, false);
	}


	if (m_st_toonbaseHslider) {
		m_st_toonbaseHslider->setValue(m_shadertypeparams.hsvtoon.basehsv.x, false);
	}
	if (m_st_toonbaseSslider) {
		m_st_toonbaseSslider->setValue(m_shadertypeparams.hsvtoon.basehsv.y, false);
	}
	if (m_st_toonbaseVslider) {
		m_st_toonbaseVslider->setValue(m_shadertypeparams.hsvtoon.basehsv.z, false);
	}
	if (m_st_toonbaseAslider) {
		m_st_toonbaseAslider->setValue(m_shadertypeparams.hsvtoon.basehsv.w, false);
	}

	if (m_st_toonhiHslider) {
		m_st_toonhiHslider->setValue(m_shadertypeparams.hsvtoon.hiaddhsv.x, false);
	}
	if (m_st_toonhiSslider) {
		m_st_toonhiSslider->setValue(m_shadertypeparams.hsvtoon.hiaddhsv.y, false);
	}
	if (m_st_toonhiVslider) {
		m_st_toonhiVslider->setValue(m_shadertypeparams.hsvtoon.hiaddhsv.z, false);
	}
	if (m_st_toonhiAslider) {
		m_st_toonhiAslider->setValue(m_shadertypeparams.hsvtoon.hiaddhsv.w, false);
	}

	if (m_st_toonlowHslider) {
		m_st_toonlowHslider->setValue(m_shadertypeparams.hsvtoon.lowaddhsv.x, false);
	}
	if (m_st_toonlowSslider) {
		m_st_toonlowSslider->setValue(m_shadertypeparams.hsvtoon.lowaddhsv.y, false);
	}
	if (m_st_toonlowVslider) {
		m_st_toonlowVslider->setValue(m_shadertypeparams.hsvtoon.lowaddhsv.z, false);
	}
	if (m_st_toonlowAslider) {
		m_st_toonlowAslider->setValue(m_shadertypeparams.hsvtoon.lowaddhsv.w, false);
	}

	if (m_st_spccoefslider) {
		m_st_spccoefslider->setValue(m_shadertypeparams.specularcoef, false);
	}

	if (m_st_tilingUslider) {
		m_st_tilingUslider->setValue((int)(m_shadertypeparams.uvscale.x + 0.0001), false);
	}
	if (m_st_tilingVslider) {
		m_st_tilingVslider->setValue((int)(m_shadertypeparams.uvscale.y + 0.0001), false);
	}

	if (m_st_alphatestslider) {
		m_st_alphatestslider->setValue(m_shadertypeparams.alphatest, false);
	}

	if (m_st_distortionscaleslider) {
		m_st_distortionscaleslider->setValue(m_shadertypeparams.distortionscale, false);
	}
	if (m_st_seacenterUslider) {
		m_st_seacenterUslider->setValue(m_shadertypeparams.seacenter.x, false);
	}
	if (m_st_seacenterVslider) {
		m_st_seacenterVslider->setValue(m_shadertypeparams.seacenter.y, false);
	}
	if (m_st_riverdirUslider) {
		m_st_riverdirUslider->setValue(m_shadertypeparams.riverdir.x, false);
	}
	if (m_st_riverdirVslider) {
		m_st_riverdirVslider->setValue(m_shadertypeparams.riverdir.y, false);
	}
	if (m_st_flowrateslider) {
		m_st_flowrateslider->setValue(m_shadertypeparams.riverflowrate, false);
	}


	//#########
	//CheckBox
	//#########
	if (m_st_emissionchk) {
		if ((bool)m_shadertypeparams.enableEmission == true) {
			m_st_emissionchk->setValue(true, false);
		}
		else {
			m_st_emissionchk->setValue(false, false);
		}
	}

	if (m_st_gradationchk) {
		if (m_shadertypeparams.hsvtoon.gradationflag == true) {
			m_st_gradationchk->setValue(true, false);

			if (m_st_powertoonchk) {
				m_st_powertoonchk->setActive(true);
			}
		}
		else {
			m_st_gradationchk->setValue(false, false);

			if (m_st_powertoonchk) {
				m_st_powertoonchk->setActive(false);
			}
		}
	}

	if (m_st_powertoonchk) {
		if (m_shadertypeparams.hsvtoon.powertoon == true) {
			m_st_powertoonchk->setValue(true, false);
		}
		else {
			m_st_powertoonchk->setValue(false, false);
		}
	}

	if (m_st_normaly0chk) {
		if (m_shadertypeparams.normaly0flag == true) {
			m_st_normaly0chk->setValue(true, false);
		}
		else {
			m_st_normaly0chk->setValue(false, false);
		}
	}

	if (m_st_shadowcasterchk) {
		if (m_shadertypeparams.shadowcasterflag == true) {
			m_st_shadowcasterchk->setValue(true, false);
		}
		else {
			m_st_shadowcasterchk->setValue(false, false);
		}
	}

	if (m_st_lightflagchk) {
		if (m_shadertypeparams.lightingmat == true) {
			m_st_lightflagchk->setValue(true, false);
		}
		else {
			m_st_lightflagchk->setValue(false, false);
		}
	}

	if (m_st_distortionchk) {
		if (m_shadertypeparams.distortionflag == true) {
			m_st_distortionchk->setValue(true, false);
		}
		else {
			m_st_distortionchk->setValue(false, false);
		}
	}

	return 0;
}

int CShaderParamsDlg::OnFrameShaperTypeParamsDlg(CShaderTypeDlg* pshadertypedlg)
{
	//if (!pshadertypedlg || !pshadertypeparams || !phsvtoonforall || !pptoonmqomaterial) {
	//	_ASSERT(0);
	//	return 1;
	//}
	if (!pshadertypedlg) {
		_ASSERT(0);
		return 1;
	}

	if (GetShaderStRadioFlag()) {
		SetShaderStRadioFlag(false);
		if (m_model) {
			int shadertype = GetShaderTypeRadio();
			switch (shadertype) {
			case 0:
				m_shadertypeparams.shadertype = -1;
				break;
			case 1:
				m_shadertypeparams.shadertype = MQOSHADER_PBR;
				break;
			case 2:
				m_shadertypeparams.shadertype = MQOSHADER_STD;
				break;
			case 3:
				m_shadertypeparams.shadertype = MQOSHADER_TOON;
				break;
			default:
				_ASSERT(0);
				break;
			}
			pshadertypedlg->SetShaderType(shadertype);
		}
	}
	if (GetShaderLightChkFlag()) {
		SetShaderLightChkFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShaderLightFlagChk();
			if (ischecked) {
				m_shadertypeparams.lightingmat = true;
			}
			else {
				m_shadertypeparams.lightingmat = false;
			}

			if (curmqomat) {
				curmqomat->SetLightingFlag(m_shadertypeparams.lightingmat);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetLightingFlag(m_shadertypeparams.lightingmat);
					}
				}
			}
		}
	}
	if (GetShaderShadowCasterChkFlag()) {
		SetShaderShadowCasterChkFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShaderShadowCasterChk();
			if (ischecked) {
				m_shadertypeparams.shadowcasterflag = true;
			}
			else {
				m_shadertypeparams.shadowcasterflag = false;
			}

			if (curmqomat) {
				curmqomat->SetShadowCasterFlag(m_shadertypeparams.shadowcasterflag);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetShadowCasterFlag(m_shadertypeparams.shadowcasterflag);
					}
				}
			}
		}
	}
	if (GetShaderNormalY0ChkFlag()) {
		SetShaderNormalY0ChkFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShadowNormalY0Chk();
			if (ischecked) {
				m_shadertypeparams.normaly0flag = true;
			}
			else {
				m_shadertypeparams.normaly0flag = false;
			}

			if (curmqomat) {
				curmqomat->SetNormalY0Flag(m_shadertypeparams.normaly0flag);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetNormalY0Flag(m_shadertypeparams.normaly0flag);
					}
				}
			}
		}
	}
	if (GetShaderSpcCoefSliderFlag()) {
		SetShaderSpcCoefSliderFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newspcscale = GetShaderSpcCoefSlider();
			m_shadertypeparams.specularcoef = newspcscale;

			if (curmqomat) {
				curmqomat->SetSpecularCoef(newspcscale);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetSpecularCoef(newspcscale);
					}
				}
			}
		}
	}
	if (GetShaderEmissionChkFlag()) {
		SetShaderEmissionChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShaderEmissionChk();
			if (ischecked) {
				m_shadertypeparams.enableEmission = true;
			}
			else {
				m_shadertypeparams.enableEmission = false;
			}

			if (curmqomat) {
				curmqomat->SetEnableEmission(m_shadertypeparams.enableEmission);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetEnableEmission(m_shadertypeparams.enableEmission);
					}
				}
			}
		}
	}
	if (GetShaderEmissionSliderFlag()) {
		SetShaderEmissionSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newemiscale = GetShaderEmissionSlider();
			m_shadertypeparams.emissiveScale = newemiscale;

			if (curmqomat) {
				curmqomat->SetEmissiveScale(newemiscale);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetEmissiveScale(newemiscale);
					}
				}
			}
		}
	}
	if (GetShaderMetalSliderFlag()) {
		SetShaderMetalSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newmetalcoef = GetShaderMetalSlider();
			m_shadertypeparams.metalcoef = newmetalcoef;

			if (curmqomat) {
				curmqomat->SetMetalAdd(newmetalcoef);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetMetalAdd(newmetalcoef);
					}
				}
			}
		}
	}
	if (GetShaderSmoothSliderFlag()) {
		SetShaderSmoothSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newsmoothcoef = GetShaderSmoothSlider();
			m_shadertypeparams.smoothcoef = newsmoothcoef;

			if (curmqomat) {
				curmqomat->SetSmoothCoef(newsmoothcoef);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetSmoothCoef(newsmoothcoef);
					}
				}
			}
		}
	}
	if (GetShaderLitScale1Flag()) {
		SetShaderLitScale1Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 0;//!!!
			float newlitscale = GetShaderLitScaleSlider1();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale2Flag()) {
		SetShaderLitScale2Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 1;//!!!
			float newlitscale = GetShaderLitScaleSlider2();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale3Flag()) {
		SetShaderLitScale3Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 2;//!!!
			float newlitscale = GetShaderLitScaleSlider3();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale4Flag()) {
		SetShaderLitScale4Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 3;//!!!
			float newlitscale = GetShaderLitScaleSlider4();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale5Flag()) {
		SetShaderLitScale5Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 4;//!!!
			float newlitscale = GetShaderLitScaleSlider5();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale6Flag()) {
		SetShaderLitScale6Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 5;//!!!
			float newlitscale = GetShaderLitScaleSlider6();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale7Flag()) {
		SetShaderLitScale7Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 6;//!!!
			float newlitscale = GetShaderLitScaleSlider7();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderLitScale8Flag()) {
		SetShaderLitScale8Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 7;//!!!
			float newlitscale = GetShaderLitScaleSlider8();
			m_shadertypeparams.lightscale[litno4] = newlitscale;

			if (curmqomat) {
				curmqomat->SetLightScale(litno4, newlitscale);
			}
			else {
				int materialindex4;
				for (materialindex4 = 0; materialindex4 < materialnum; materialindex4++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex4);
					if (setmqomat) {
						setmqomat->SetLightScale(litno4, newlitscale);
					}
				}
			}
		}
	}
	if (GetShaderToonLitRadioFlag()) {
		SetShaderToonLitRadioFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int lightindex = GetShaderToonLitRadio();
			m_shadertypeparams.hsvtoon.lightindex = lightindex;

			if (curmqomat) {
				curmqomat->SetToonLightIndex(lightindex);
			}
			else {
				int materialindex8;
				for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
					if (setmqomat) {
						setmqomat->SetToonLightIndex(lightindex);
					}
				}
			}
		}
	}
	if (GetShaderToonHiAddrSliderFlag()) {
		SetShaderToonHiAddrSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float hicolorh = GetShaderToonHiAddrSlider();
			m_shadertypeparams.hsvtoon.hicolorh = hicolorh;

			if (curmqomat) {
				curmqomat->SetToonHiAddrH(m_shadertypeparams.hsvtoon.hicolorh);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddrH(m_shadertypeparams.hsvtoon.hicolorh);
						m_hsvtoonforall.hicolorh = m_shadertypeparams.hsvtoon.hicolorh;
					}
				}
			}
		}
	}
	if (GetShaderRemakeToonTextureFlag()) {
		SetShaderRemakeToonTextureFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			if (curmqomat) {
				m_toonmqomaterial = curmqomat;
			}
			else {
				m_toonmqomaterial = nullptr;
			}
			m_toonparamchange = true;
		}
	}
	if (GetShaderToonLowAddrSliderFlag()) {
		SetShaderToonLowAddrSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float lowcolorh = GetShaderToonLowAddrSlider();
			m_shadertypeparams.hsvtoon.lowcolorh = lowcolorh;

			if (curmqomat) {
				curmqomat->SetToonLowAddrH(m_shadertypeparams.hsvtoon.lowcolorh);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddrH(m_shadertypeparams.hsvtoon.lowcolorh);
						m_hsvtoonforall.lowcolorh = m_shadertypeparams.hsvtoon.lowcolorh;
					}
				}
			}
		}
	}
	if (GetShaderGradationChkFlag()) {
		SetShaderGradationChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShaderGradationChk();
			if (ischecked) {
				m_shadertypeparams.hsvtoon.gradationflag = true;
			}
			else {
				m_shadertypeparams.hsvtoon.gradationflag = false;
			}

			if (curmqomat) {
				curmqomat->SetToonGradationFlag(m_shadertypeparams.hsvtoon.gradationflag);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonGradationFlag(m_shadertypeparams.hsvtoon.gradationflag);
						m_hsvtoonforall.gradationflag = m_shadertypeparams.hsvtoon.gradationflag;
					}
				}
			}

			if (curmqomat) {
				m_toonmqomaterial = curmqomat;
			}
			else {
				m_toonmqomaterial = nullptr;
			}
			m_toonparamchange = true;
		}
	}
	if (GetShaderPowerToonChkFlag()) {
		SetShaderPowerToonChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShaderPowerToonChk();
			if (ischecked) {
				m_shadertypeparams.hsvtoon.powertoon = true;
			}
			else {
				m_shadertypeparams.hsvtoon.powertoon = false;
			}

			if (curmqomat) {
				curmqomat->SetToonPowerToon(m_shadertypeparams.hsvtoon.powertoon);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonPowerToon(m_shadertypeparams.hsvtoon.powertoon);
						m_hsvtoonforall.powertoon = m_shadertypeparams.hsvtoon.powertoon;
					}
				}
			}

			if (curmqomat) {
				m_toonmqomaterial = curmqomat;
			}
			else {
				m_toonmqomaterial = nullptr;
			}
			m_toonparamchange = true;
		}
	}
	if (GetShaderToonBaseHSliderFlag()) {
		SetShaderToonBaseHSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_shadertypeparams.hsvtoon.basehsv.x = GetShaderToonBaseHSlider();

			if (curmqomat) {
				curmqomat->SetToonBaseH(m_shadertypeparams.hsvtoon.basehsv.x);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseH(m_shadertypeparams.hsvtoon.basehsv.x);
						m_hsvtoonforall.basehsv.x = m_shadertypeparams.hsvtoon.basehsv.x;
					}
				}
			}
		}
	}
	if (GetShaderToonBaseSSliderFlag()) {
		SetShaderToonBaseSSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_shadertypeparams.hsvtoon.basehsv.y = GetShaderToonBaseSSlider();

			if (curmqomat) {
				curmqomat->SetToonBaseS(m_shadertypeparams.hsvtoon.basehsv.y);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseS(m_shadertypeparams.hsvtoon.basehsv.y);
						m_hsvtoonforall.basehsv.y = m_shadertypeparams.hsvtoon.basehsv.y;
					}
				}
			}
		}
	}
	if (GetShaderToonBaseVSliderFlag()) {
		SetShaderToonBaseVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_shadertypeparams.hsvtoon.basehsv.z = GetShaderToonBaseVSlider();

			if (curmqomat) {
				curmqomat->SetToonBaseV(m_shadertypeparams.hsvtoon.basehsv.z);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseV(m_shadertypeparams.hsvtoon.basehsv.z);
						m_hsvtoonforall.basehsv.z = m_shadertypeparams.hsvtoon.basehsv.z;
					}
				}
			}
		}
	}
	if (GetShaderToonBaseASliderFlag()) {
		SetShaderToonBaseASliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_shadertypeparams.hsvtoon.basehsv.w = GetShaderToonBaseASlider();

			if (curmqomat) {
				curmqomat->SetToonBaseA(m_shadertypeparams.hsvtoon.basehsv.w);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseA(m_shadertypeparams.hsvtoon.basehsv.w);
						m_hsvtoonforall.basehsv.w = m_shadertypeparams.hsvtoon.basehsv.w;
					}
				}
			}
		}
	}
	if (GetShaderToonHiHSliderFlag()) {
		SetShaderToonHiHSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetShaderToonHiHSlider();
			m_shadertypeparams.hsvtoon.hiaddhsv.x = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddH(m_shadertypeparams.hsvtoon.hiaddhsv.x);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddH(m_shadertypeparams.hsvtoon.hiaddhsv.x);
						m_hsvtoonforall.hiaddhsv.x = m_shadertypeparams.hsvtoon.hiaddhsv.x;
					}
				}
			}
		}
	}
	if (GetShaderToonHiSSliderFlag()) {
		SetShaderToonHiSSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetShaderToonHiSSlider();
			m_shadertypeparams.hsvtoon.hiaddhsv.y = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddS(m_shadertypeparams.hsvtoon.hiaddhsv.y);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddS(m_shadertypeparams.hsvtoon.hiaddhsv.y);
						m_hsvtoonforall.hiaddhsv.y = m_shadertypeparams.hsvtoon.hiaddhsv.y;
					}
				}
			}
		}
	}
	if (GetShaderToonHiVSliderFlag()) {
		SetShaderToonHiVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetShaderToonHiVSlider();
			m_shadertypeparams.hsvtoon.hiaddhsv.z = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddV(m_shadertypeparams.hsvtoon.hiaddhsv.z);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddV(m_shadertypeparams.hsvtoon.hiaddhsv.z);
						m_hsvtoonforall.hiaddhsv.z = m_shadertypeparams.hsvtoon.hiaddhsv.z;
					}
				}
			}
		}
	}
	if (GetShaderToonHiASliderFlag()) {
		SetShaderToonHiASliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetShaderToonHiASlider();
			m_shadertypeparams.hsvtoon.hiaddhsv.w = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddA(m_shadertypeparams.hsvtoon.hiaddhsv.w);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddA(m_shadertypeparams.hsvtoon.hiaddhsv.w);
						m_hsvtoonforall.hiaddhsv.w = m_shadertypeparams.hsvtoon.hiaddhsv.w;
					}
				}
			}
		}
	}
	if (GetShaderToonLowHSliderFlag()) {
		SetShaderToonLowHSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetShaderToonLowHSlider();
			m_shadertypeparams.hsvtoon.lowaddhsv.x = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddH(m_shadertypeparams.hsvtoon.lowaddhsv.x);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddH(m_shadertypeparams.hsvtoon.lowaddhsv.x);
						m_hsvtoonforall.lowaddhsv.x = m_shadertypeparams.hsvtoon.lowaddhsv.x;
					}
				}
			}
		}
	}
	if (GetShaderToonLowSSliderFlag()) {
		SetShaderToonLowSSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetShaderToonLowSSlider();
			m_shadertypeparams.hsvtoon.lowaddhsv.y = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddS(m_shadertypeparams.hsvtoon.lowaddhsv.y);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddS(m_shadertypeparams.hsvtoon.lowaddhsv.y);
						m_hsvtoonforall.lowaddhsv.y = m_shadertypeparams.hsvtoon.lowaddhsv.y;
					}
				}
			}
		}
	}
	if (GetShaderToonLowVSliderFlag()) {
		SetShaderToonLowVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetShaderToonLowVSlider();
			m_shadertypeparams.hsvtoon.lowaddhsv.z = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddV(m_shadertypeparams.hsvtoon.lowaddhsv.z);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddV(m_shadertypeparams.hsvtoon.lowaddhsv.z);
						m_hsvtoonforall.lowaddhsv.z = m_shadertypeparams.hsvtoon.lowaddhsv.z;
					}
				}
			}
		}
	}
	if (GetShaderToonLowASliderFlag()) {
		SetShaderToonLowASliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetShaderToonLowASlider();
			m_shadertypeparams.hsvtoon.lowaddhsv.w = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddA(m_shadertypeparams.hsvtoon.lowaddhsv.w);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddA(m_shadertypeparams.hsvtoon.lowaddhsv.w);
						m_hsvtoonforall.lowaddhsv.w = m_shadertypeparams.hsvtoon.lowaddhsv.w;
					}
				}
			}
		}
	}
	if (GetShaderTilingUSliderFlag()) {
		SetShaderTilingUSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int newvalue = GetShaderTilingUSlider();
			m_shadertypeparams.uvscale.x = (double)newvalue;

			if (curmqomat) {
				curmqomat->SetUVScale(m_shadertypeparams.uvscale);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetUVScale(m_shadertypeparams.uvscale);
					}
				}
			}
		}
	}
	if (GetShaderTilingUSliderUpFlag()) {
		SetShaderTilingUSliderUpFlag(false);

		if (m_model) {
			int newvalue = GetShaderTilingUSlider();
			m_shadertypeparams.uvscale.x = (double)newvalue;
			SetShaderTilingUSlider(newvalue);
			//m_st_tilingUslider->setValue(m_shadertypeparams.uvscale.x, false);//!!! マウスを離したときにintに丸めた値をセットし直す
		}
	}
	if (GetShaderTilingVSliderFlag()) {
		SetShaderTilingVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int newvalue = GetShaderTilingVSlider();
			m_shadertypeparams.uvscale.y = (double)newvalue;

			if (curmqomat) {
				curmqomat->SetUVScale(m_shadertypeparams.uvscale);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetUVScale(m_shadertypeparams.uvscale);
					}
				}
			}
		}
	}
	if (GetShaderTilingVSliderUpFlag()) {
		SetShaderTilingVSliderUpFlag(false);

		if (m_model) {
			int newvalue = GetShaderTilingVSlider();
			m_shadertypeparams.uvscale.y = (double)newvalue;

			//m_st_tilingVslider->setValue(m_shadertypeparams.uvscale.y, false);//!!! マウスを離したときにintに丸めた値をセットし直す
			SetShaderTilingVSlider(newvalue);
		}
	}
	if (GetShaderAlphaTestSliderFlag()) {
		SetShaderAlphaTestSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_shadertypeparams.alphatest = GetShaderAlphaTestSlider();

			if (curmqomat) {
				curmqomat->SetAlphaTestClipVal(m_shadertypeparams.alphatest);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetAlphaTestClipVal(m_shadertypeparams.alphatest);
					}
				}
			}
		}
	}
	if (GetShaderDistortionChkFlag()) {
		SetShaderDistortionChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShaderDistortionChk();
			if (ischecked) {
				m_shadertypeparams.distortionflag = true;
			}
			else {
				m_shadertypeparams.distortionflag = false;
			}

			if (curmqomat) {
				curmqomat->SetDistortionFlag(m_shadertypeparams.distortionflag);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetDistortionFlag(m_shadertypeparams.distortionflag);
					}
				}
			}
		}
	}
	if (GetShaderDistortionScaleSliderFlag()) {
		SetShaderDistortionScaleSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			double newvalue = GetShaderDistortonScaleSlider();
			m_shadertypeparams.distortionscale = newvalue;

			if (curmqomat) {
				curmqomat->SetDistortionScale(m_shadertypeparams.distortionscale);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetDistortionScale(m_shadertypeparams.distortionscale);
					}
				}
			}
		}
	}
	if (GetShaderRiverRadioFlag()) {
		SetShaderRiverRadioFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int riverorsea = GetShaderRiverRadio();
			m_shadertypeparams.riverorsea = riverorsea;

			if (curmqomat) {
				curmqomat->SetRiverOrSea(riverorsea);
			}
			else {
				int materialindex8;
				for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
					if (setmqomat) {
						setmqomat->SetRiverOrSea(riverorsea);
					}
				}
			}
		}
	}
	if (GetShaderSeaCenterUSliderFlag()) {
		SetShaderSeaCenterUSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalueU = GetShaderSeaCenterUSlider();
			float newvalueV = GetShaderSeaCenterVSlider();
			m_shadertypeparams.seacenter.SetParams(newvalueU, newvalueV);

			if (curmqomat) {
				curmqomat->SetSeaCenter(m_shadertypeparams.seacenter);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetSeaCenter(m_shadertypeparams.seacenter);
					}
				}
			}
		}
	}
	if (GetShaderSeaCenterVSliderFlag()) {
		SetShaderSeaCenterVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalueU = GetShaderSeaCenterUSlider();
			float newvalueV = GetShaderSeaCenterVSlider();
			m_shadertypeparams.seacenter.SetParams(newvalueU, newvalueV);

			if (curmqomat) {
				curmqomat->SetSeaCenter(m_shadertypeparams.seacenter);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetSeaCenter(m_shadertypeparams.seacenter);
					}
				}
			}
		}
	}
	if (GetShaderRiverDirUSliderFlag()) {
		SetShaderRiverDirUSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalueU = GetShaderRiverDirUSlider();
			float newvalueV = GetShaderRiverDirVSlider();
			m_shadertypeparams.riverdir.SetParams(newvalueU, newvalueV);

			if (curmqomat) {
				curmqomat->SetRiverDir(m_shadertypeparams.riverdir);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetRiverDir(m_shadertypeparams.riverdir);
					}
				}
			}
		}
	}
	if (GetShaderRiverDirVSliderFlag()) {
		SetShaderRiverDirVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalueU = GetShaderRiverDirUSlider();
			float newvalueV = GetShaderRiverDirVSlider();
			m_shadertypeparams.riverdir.SetParams(newvalueU, newvalueV);

			if (curmqomat) {
				curmqomat->SetRiverDir(m_shadertypeparams.riverdir);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetRiverDir(m_shadertypeparams.riverdir);
					}
				}
			}
		}
	}
	if (GetShaderFlowRateSliderFlag()) {
		SetShaderFlowRateSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			double newvalue = GetShaderFlowRateSlider();
			m_shadertypeparams.riverflowrate = newvalue;

			if (curmqomat) {
				curmqomat->SetRiverFlowRate(m_shadertypeparams.riverflowrate);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetRiverFlowRate(m_shadertypeparams.riverflowrate);
					}
				}
			}
		}

	}
	if (GetShaderDistortionMapRadioFlag()) {
		SetShaderDistortionMapRadioFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = pshadertypedlg->GetShaderTypeParamsIndex() - 1;
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int maptype = GetShaderDistortionMapRadio();
			m_shadertypeparams.distortionmaptype = maptype;

			if (curmqomat) {
				curmqomat->SetDistortionMapType(maptype);
			}
			else {
				int materialindex8;
				for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
					if (setmqomat) {
						setmqomat->SetDistortionMapType(maptype);
					}
				}
			}
		}
	}




	if (m_toonparamchange) {
		m_toonparamchange = false;
		if (m_model) {
			if (m_toonmqomaterial) {
				m_toonmqomaterial->RemakeDiffuseTexture();
			}
			else {
				int materialnum = m_model->GetMQOMaterialSize();
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->RemakeDiffuseTexture();
					}
				}
			}
		}
	}


	return 0;
}

const HWND CShaderParamsDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CShaderParamsDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CShaderParamsDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

