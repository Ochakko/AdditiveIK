#include "stdafx.h"

#include <SkyParamsDlg.h>
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
// CSkyParamsDlg

CSkyParamsDlg::CSkyParamsDlg()
{
	InitParams();
}

CSkyParamsDlg::~CSkyParamsDlg()
{
	DestroyObjs();
}
	
int CSkyParamsDlg::DestroyObjs()
{
	if (m_dlgWnd) {
		m_dlgWnd->setVisible(false);
		m_dlgWnd->setListenMouse(false);
	}

	if (m_skyst_Sc) {
		delete m_skyst_Sc;
		m_skyst_Sc = nullptr;
	}
	if (m_skyst_spall) {
		delete m_skyst_spall;
		m_skyst_spall = nullptr;
	}
	if (m_skyst_spacerLabel01) {
		delete m_skyst_spacerLabel01;
		m_skyst_spacerLabel01 = nullptr;
	}
	if (m_skyst_spacerLabel02) {
		delete m_skyst_spacerLabel02;
		m_skyst_spacerLabel02 = nullptr;
	}
	if (m_skyst_spacerLabel03) {
		delete m_skyst_spacerLabel03;
		m_skyst_spacerLabel03 = nullptr;
	}
	if (m_skyst_spacerLabel04) {
		delete m_skyst_spacerLabel04;
		m_skyst_spacerLabel04 = nullptr;
	}
	if (m_skyst_spacerLabel05) {
		delete m_skyst_spacerLabel05;
		m_skyst_spacerLabel05 = nullptr;
	}
	if (m_skyst_spacerLabel06) {
		delete m_skyst_spacerLabel06;
		m_skyst_spacerLabel06 = nullptr;
	}
	if (m_skyst_namesp) {
		delete m_skyst_namesp;
		m_skyst_namesp = nullptr;
	}
	if (m_skyst_backB) {
		delete m_skyst_backB;
		m_skyst_backB = nullptr;
	}
	if (m_skyst_namelabel) {
		delete m_skyst_namelabel;
		m_skyst_namelabel = nullptr;
	}
	if (m_skyst_shadertyperadio) {
		delete m_skyst_shadertyperadio;
		m_skyst_shadertyperadio = nullptr;
	}
	if (m_skyst_litflagsp) {
		delete m_skyst_litflagsp;
		m_skyst_litflagsp = nullptr;
	}
	if (m_skyst_lightflagchk) {
		delete m_skyst_lightflagchk;
		m_skyst_lightflagchk = nullptr;
	}
	if (m_skyst_shadowcasterchk) {
		delete m_skyst_shadowcasterchk;
		m_skyst_shadowcasterchk = nullptr;
	}
	if (m_skyst_normaly0chk) {
		delete m_skyst_normaly0chk;
		m_skyst_normaly0chk = nullptr;
	}
	if (m_skyst_spccoefsp0) {
		delete m_skyst_spccoefsp0;
		m_skyst_spccoefsp0 = nullptr;
	}
	if (m_skyst_spccoefsp1) {
		delete m_skyst_spccoefsp1;
		m_skyst_spccoefsp1 = nullptr;
	}
	if (m_skyst_spccoefsp2) {
		delete m_skyst_spccoefsp2;
		m_skyst_spccoefsp2 = nullptr;
	}
	if (m_skyst_spccoeflabel) {
		delete m_skyst_spccoeflabel;
		m_skyst_spccoeflabel = nullptr;
	}
	if (m_skyst_spccoefslider) {
		delete m_skyst_spccoefslider;
		m_skyst_spccoefslider = nullptr;
	}
	if (m_skyst_emissionchk) {
		delete m_skyst_emissionchk;
		m_skyst_emissionchk = nullptr;
	}
	if (m_skyst_emissionslider) {
		delete m_skyst_emissionslider;
		m_skyst_emissionslider = nullptr;
	}
	if (m_skyst_metalsp0) {
		delete m_skyst_metalsp0;
		m_skyst_metalsp0 = nullptr;
	}
	if (m_skyst_metalsp1) {
		delete m_skyst_metalsp1;
		m_skyst_metalsp1 = nullptr;
	}
	if (m_skyst_metalsp2) {
		delete m_skyst_metalsp2;
		m_skyst_metalsp2 = nullptr;
	}
	if (m_skyst_metallabel) {
		delete m_skyst_metallabel;
		m_skyst_metallabel = nullptr;
	}
	if (m_skyst_metalslider) {
		delete m_skyst_metalslider;
		m_skyst_metalslider = nullptr;
	}
	if (m_skyst_smoothlabel) {
		delete m_skyst_smoothlabel;
		m_skyst_smoothlabel = nullptr;
	}
	if (m_skyst_smoothslider) {
		delete m_skyst_smoothslider;
		m_skyst_smoothslider = nullptr;
	}

	if (m_skyst_litscalesp1_0) {
		delete m_skyst_litscalesp1_0;
		m_skyst_litscalesp1_0 = nullptr;
	}
	if (m_skyst_litscalesp1_1) {
		delete m_skyst_litscalesp1_1;
		m_skyst_litscalesp1_1 = nullptr;
	}
	if (m_skyst_litscalesp1_2) {
		delete m_skyst_litscalesp1_2;
		m_skyst_litscalesp1_2 = nullptr;
	}
	if (m_skyst_litscalelabel1) {
		delete m_skyst_litscalelabel1;
		m_skyst_litscalelabel1 = nullptr;
	}
	if (m_skyst_litscaleslider1) {
		delete m_skyst_litscaleslider1;
		m_skyst_litscaleslider1 = nullptr;
	}
	if (m_skyst_litscalelabel2) {
		delete m_skyst_litscalelabel2;
		m_skyst_litscalelabel2 = nullptr;
	}
	if (m_skyst_litscaleslider2) {
		delete m_skyst_litscaleslider2;
		m_skyst_litscaleslider2 = nullptr;
	}

	if (m_skyst_litscalesp3_0) {
		delete m_skyst_litscalesp3_0;
		m_skyst_litscalesp3_0 = nullptr;
	}
	if (m_skyst_litscalesp3_1) {
		delete m_skyst_litscalesp3_1;
		m_skyst_litscalesp3_1 = nullptr;
	}
	if (m_skyst_litscalesp3_2) {
		delete m_skyst_litscalesp3_2;
		m_skyst_litscalesp3_2 = nullptr;
	}
	if (m_skyst_litscalelabel3) {
		delete m_skyst_litscalelabel3;
		m_skyst_litscalelabel3 = nullptr;
	}
	if (m_skyst_litscaleslider3) {
		delete m_skyst_litscaleslider3;
		m_skyst_litscaleslider3 = nullptr;
	}
	if (m_skyst_litscalelabel4) {
		delete m_skyst_litscalelabel4;
		m_skyst_litscalelabel4 = nullptr;
	}
	if (m_skyst_litscaleslider4) {
		delete m_skyst_litscaleslider4;
		m_skyst_litscaleslider4 = nullptr;
	}

	if (m_skyst_litscalesp5_0) {
		delete m_skyst_litscalesp5_0;
		m_skyst_litscalesp5_0 = nullptr;
	}
	if (m_skyst_litscalesp5_1) {
		delete m_skyst_litscalesp5_1;
		m_skyst_litscalesp5_1 = nullptr;
	}
	if (m_skyst_litscalesp5_2) {
		delete m_skyst_litscalesp5_2;
		m_skyst_litscalesp5_2 = nullptr;
	}
	if (m_skyst_litscalelabel5) {
		delete m_skyst_litscalelabel5;
		m_skyst_litscalelabel5 = nullptr;
	}
	if (m_skyst_litscaleslider5) {
		delete m_skyst_litscaleslider5;
		m_skyst_litscaleslider5 = nullptr;
	}
	if (m_skyst_litscalelabel6) {
		delete m_skyst_litscalelabel6;
		m_skyst_litscalelabel6 = nullptr;
	}
	if (m_skyst_litscaleslider6) {
		delete m_skyst_litscaleslider6;
		m_skyst_litscaleslider6 = nullptr;
	}

	if (m_skyst_litscalesp7_0) {
		delete m_skyst_litscalesp7_0;
		m_skyst_litscalesp7_0 = nullptr;
	}
	if (m_skyst_litscalesp7_1) {
		delete m_skyst_litscalesp7_1;
		m_skyst_litscalesp7_1 = nullptr;
	}
	if (m_skyst_litscalesp7_2) {
		delete m_skyst_litscalesp7_2;
		m_skyst_litscalesp7_2 = nullptr;
	}
	if (m_skyst_litscalelabel7) {
		delete m_skyst_litscalelabel7;
		m_skyst_litscalelabel7 = nullptr;
	}
	if (m_skyst_litscaleslider7) {
		delete m_skyst_litscaleslider7;
		m_skyst_litscaleslider7 = nullptr;
	}
	if (m_skyst_litscalelabel8) {
		delete m_skyst_litscalelabel8;
		m_skyst_litscalelabel8 = nullptr;
	}
	if (m_skyst_litscaleslider8) {
		delete m_skyst_litscaleslider8;
		m_skyst_litscaleslider8 = nullptr;
	}

	if (m_skyst_toonlitradio) {
		delete m_skyst_toonlitradio;
		m_skyst_toonlitradio = nullptr;
	}

	if (m_skyst_toonaddrsp0) {
		delete m_skyst_toonaddrsp0;
		m_skyst_toonaddrsp0 = nullptr;
	}
	if (m_skyst_toonaddrsp1) {
		delete m_skyst_toonaddrsp1;
		m_skyst_toonaddrsp1 = nullptr;
	}
	if (m_skyst_toonaddrsp2) {
		delete m_skyst_toonaddrsp2;
		m_skyst_toonaddrsp2 = nullptr;
	}
	if (m_skyst_toonhiaddrlabel) {
		delete m_skyst_toonhiaddrlabel;
		m_skyst_toonhiaddrlabel = nullptr;
	}
	if (m_skyst_toonhiaddrslider) {
		delete m_skyst_toonhiaddrslider;
		m_skyst_toonhiaddrslider = nullptr;
	}
	if (m_skyst_toonlowaddrlabel) {
		delete m_skyst_toonlowaddrlabel;
		m_skyst_toonlowaddrlabel = nullptr;
	}
	if (m_skyst_toonlowaddrslider) {
		delete m_skyst_toonlowaddrslider;
		m_skyst_toonlowaddrslider = nullptr;
	}

	if (m_skyst_gradationsp0) {
		delete m_skyst_gradationsp0;
		m_skyst_gradationsp0 = nullptr;
	}
	if (m_skyst_gradationchk) {
		delete m_skyst_gradationchk;
		m_skyst_gradationchk = nullptr;
	}
	if (m_skyst_powertoonchk) {
		delete m_skyst_powertoonchk;
		m_skyst_powertoonchk = nullptr;
	}

	if (m_skyst_toonbasesp1_0) {
		delete m_skyst_toonbasesp1_0;
		m_skyst_toonbasesp1_0 = nullptr;
	}
	if (m_skyst_toonbasesp1_1) {
		delete m_skyst_toonbasesp1_1;
		m_skyst_toonbasesp1_1 = nullptr;
	}
	if (m_skyst_toonbasesp1_2) {
		delete m_skyst_toonbasesp1_2;
		m_skyst_toonbasesp1_2 = nullptr;
	}
	if (m_skyst_toonbaseHlabel) {
		delete m_skyst_toonbaseHlabel;
		m_skyst_toonbaseHlabel = nullptr;
	}
	if (m_skyst_toonbaseHslider) {
		delete m_skyst_toonbaseHslider;
		m_skyst_toonbaseHslider = nullptr;
	}
	if (m_skyst_toonbaseSlabel) {
		delete m_skyst_toonbaseSlabel;
		m_skyst_toonbaseSlabel = nullptr;
	}
	if (m_skyst_toonbaseSslider) {
		delete m_skyst_toonbaseSslider;
		m_skyst_toonbaseSslider = nullptr;
	}
	if (m_skyst_toonbasesp2_0) {
		delete m_skyst_toonbasesp2_0;
		m_skyst_toonbasesp2_0 = nullptr;
	}
	if (m_skyst_toonbasesp2_1) {
		delete m_skyst_toonbasesp2_1;
		m_skyst_toonbasesp2_1 = nullptr;
	}
	if (m_skyst_toonbasesp2_2) {
		delete m_skyst_toonbasesp2_2;
		m_skyst_toonbasesp2_2 = nullptr;
	}
	if (m_skyst_toonbaseVlabel) {
		delete m_skyst_toonbaseVlabel;
		m_skyst_toonbaseVlabel = nullptr;
	}
	if (m_skyst_toonbaseVslider) {
		delete m_skyst_toonbaseVslider;
		m_skyst_toonbaseVslider = nullptr;
	}
	if (m_skyst_toonbaseAlabel) {
		delete m_skyst_toonbaseAlabel;
		m_skyst_toonbaseAlabel = nullptr;
	}
	if (m_skyst_toonbaseAslider) {
		delete m_skyst_toonbaseAslider;
		m_skyst_toonbaseAslider = nullptr;
	}

	if (m_skyst_toonhisp1_0) {
		delete m_skyst_toonhisp1_0;
		m_skyst_toonhisp1_0 = nullptr;
	}
	if (m_skyst_toonhisp1_1) {
		delete m_skyst_toonhisp1_1;
		m_skyst_toonhisp1_1 = nullptr;
	}
	if (m_skyst_toonhisp1_2) {
		delete m_skyst_toonhisp1_2;
		m_skyst_toonhisp1_2 = nullptr;
	}
	if (m_skyst_toonhiHlabel) {
		delete m_skyst_toonhiHlabel;
		m_skyst_toonhiHlabel = nullptr;
	}
	if (m_skyst_toonhiHslider) {
		delete m_skyst_toonhiHslider;
		m_skyst_toonhiHslider = nullptr;
	}
	if (m_skyst_toonhiSlabel) {
		delete m_skyst_toonhiSlabel;
		m_skyst_toonhiSlabel = nullptr;
	}
	if (m_skyst_toonhiSslider) {
		delete m_skyst_toonhiSslider;
		m_skyst_toonhiSslider = nullptr;
	}
	if (m_skyst_toonhisp2_0) {
		delete m_skyst_toonhisp2_0;
		m_skyst_toonhisp2_0 = nullptr;
	}
	if (m_skyst_toonhisp2_1) {
		delete m_skyst_toonhisp2_1;
		m_skyst_toonhisp2_1 = nullptr;
	}
	if (m_skyst_toonhisp2_2) {
		delete m_skyst_toonhisp2_2;
		m_skyst_toonhisp2_2 = nullptr;
	}
	if (m_skyst_toonhiVlabel) {
		delete m_skyst_toonhiVlabel;
		m_skyst_toonhiVlabel = nullptr;
	}
	if (m_skyst_toonhiVslider) {
		delete m_skyst_toonhiVslider;
		m_skyst_toonhiVslider = nullptr;
	}
	if (m_skyst_toonhiAlabel) {
		delete m_skyst_toonhiAlabel;
		m_skyst_toonhiAlabel = nullptr;
	}
	if (m_skyst_toonhiAslider) {
		delete m_skyst_toonhiAslider;
		m_skyst_toonhiAslider = nullptr;
	}

	if (m_skyst_toonlowsp1_0) {
		delete m_skyst_toonlowsp1_0;
		m_skyst_toonlowsp1_0 = nullptr;
	}
	if (m_skyst_toonlowsp1_1) {
		delete m_skyst_toonlowsp1_1;
		m_skyst_toonlowsp1_1 = nullptr;
	}
	if (m_skyst_toonlowsp1_2) {
		delete m_skyst_toonlowsp1_2;

		m_skyst_toonlowsp1_2 = nullptr;
	}
	if (m_skyst_toonlowHlabel) {
		delete m_skyst_toonlowHlabel;
		m_skyst_toonlowHlabel = nullptr;
	}
	if (m_skyst_toonlowHslider) {
		delete m_skyst_toonlowHslider;
		m_skyst_toonlowHslider = nullptr;
	}
	if (m_skyst_toonlowSlabel) {
		delete m_skyst_toonlowSlabel;
		m_skyst_toonlowSlabel = nullptr;
	}
	if (m_skyst_toonlowSslider) {
		delete m_skyst_toonlowSslider;
		m_skyst_toonlowSslider = nullptr;
	}
	if (m_skyst_toonlowsp2_0) {
		delete m_skyst_toonlowsp2_0;
		m_skyst_toonlowsp2_0 = nullptr;
	}
	if (m_skyst_toonlowsp2_1) {
		delete m_skyst_toonlowsp2_1;
		m_skyst_toonlowsp2_1 = nullptr;
	}
	if (m_skyst_toonlowsp2_2) {
		delete m_skyst_toonlowsp2_2;
		m_skyst_toonlowsp2_2 = nullptr;
	}
	if (m_skyst_toonlowVlabel) {
		delete m_skyst_toonlowVlabel;
		m_skyst_toonlowVlabel = nullptr;
	}
	if (m_skyst_toonlowVslider) {
		delete m_skyst_toonlowVslider;
		m_skyst_toonlowVslider = nullptr;
	}
	if (m_skyst_toonlowAlabel) {
		delete m_skyst_toonlowAlabel;
		m_skyst_toonlowAlabel = nullptr;
	}
	if (m_skyst_toonlowAslider) {
		delete m_skyst_toonlowAslider;
		m_skyst_toonlowAslider = nullptr;
	}

	if (m_skyst_tilingsp0) {
		delete m_skyst_tilingsp0;
		m_skyst_tilingsp0 = nullptr;
	}
	if (m_skyst_tilingsp1) {
		delete m_skyst_tilingsp1;
		m_skyst_tilingsp1 = nullptr;
	}
	if (m_skyst_tilingsp2) {
		delete m_skyst_tilingsp2;
		m_skyst_tilingsp2 = nullptr;
	}
	if (m_skyst_tilingUlabel) {
		delete m_skyst_tilingUlabel;
		m_skyst_tilingUlabel = nullptr;
	}
	if (m_skyst_tilingUslider) {
		delete m_skyst_tilingUslider;
		m_skyst_tilingUslider = nullptr;
	}
	if (m_skyst_tilingVlabel) {
		delete m_skyst_tilingVlabel;
		m_skyst_tilingVlabel = nullptr;
	}
	if (m_skyst_tilingVslider) {
		delete m_skyst_tilingVslider;
		m_skyst_tilingVslider = nullptr;
	}

	if (m_skyst_alphatestsp0) {
		delete m_skyst_alphatestsp0;
		m_skyst_alphatestsp0 = nullptr;
	}
	if (m_skyst_alphatestsp1) {
		delete m_skyst_alphatestsp1;
		m_skyst_alphatestsp1 = nullptr;
	}
	if (m_skyst_alphatestlabel) {
		delete m_skyst_alphatestlabel;
		m_skyst_alphatestlabel = nullptr;
	}
	if (m_skyst_alphatestslider) {
		delete m_skyst_alphatestslider;
		m_skyst_alphatestslider = nullptr;
	}

	if (m_skyst_distortionsp0) {
		delete m_skyst_distortionsp0;
		m_skyst_distortionsp0 = nullptr;
	}
	if (m_skyst_distortionsp1) {
		delete m_skyst_distortionsp1;
		m_skyst_distortionsp1 = nullptr;
	}
	if (m_skyst_distortionchk) {
		delete m_skyst_distortionchk;
		m_skyst_distortionchk = nullptr;
	}
	if (m_skyst_distortionscalelabel) {
		delete m_skyst_distortionscalelabel;
		m_skyst_distortionscalelabel = nullptr;
	}
	if (m_skyst_distortionscaleslider) {
		delete m_skyst_distortionscaleslider;
		m_skyst_distortionscaleslider = nullptr;
	}

	if (m_skyst_riverradio) {
		delete m_skyst_riverradio;
		m_skyst_riverradio = nullptr;
	}

	if (m_skyst_seacentersp0) {
		delete m_skyst_seacentersp0;
		m_skyst_seacentersp0 = nullptr;
	}
	if (m_skyst_seacentersp1) {
		delete m_skyst_seacentersp1;
		m_skyst_seacentersp1 = nullptr;
	}
	if (m_skyst_seacenterlabel) {
		delete m_skyst_seacenterlabel;
		m_skyst_seacenterlabel = nullptr;
	}
	if (m_skyst_seacenterUslider) {
		delete m_skyst_seacenterUslider;
		m_skyst_seacenterUslider = nullptr;
	}
	if (m_skyst_seacenterVslider) {
		delete m_skyst_seacenterVslider;
		m_skyst_seacenterVslider = nullptr;
	}

	if (m_skyst_riverdirsp0) {
		delete m_skyst_riverdirsp0;
		m_skyst_riverdirsp0 = nullptr;
	}
	if (m_skyst_riverdirsp1) {
		delete m_skyst_riverdirsp1;
		m_skyst_riverdirsp1 = nullptr;
	}
	if (m_skyst_riverdirlabel) {
		delete m_skyst_riverdirlabel;
		m_skyst_riverdirlabel = nullptr;
	}
	if (m_skyst_riverdirUslider) {
		delete m_skyst_riverdirUslider;
		m_skyst_riverdirUslider = nullptr;
	}
	if (m_skyst_riverdirVslider) {
		delete m_skyst_riverdirVslider;
		m_skyst_riverdirVslider = nullptr;
	}

	if (m_skyst_flowratesp0) {
		delete m_skyst_flowratesp0;
		m_skyst_flowratesp0 = nullptr;
	}
	if (m_skyst_flowratelabel) {
		delete m_skyst_flowratelabel;
		m_skyst_flowratelabel = nullptr;
	}
	if (m_skyst_flowrateslider) {
		delete m_skyst_flowrateslider;
		m_skyst_flowrateslider = nullptr;
	}

	if (m_skyst_distortionmapradio) {
		delete m_skyst_distortionmapradio;
		m_skyst_distortionmapradio = nullptr;
	}




	if (m_skyst_slotsp1_0) {
		delete m_skyst_slotsp1_0;
		m_skyst_slotsp1_0 = 0;
	}
	if (m_skyst_slotsp1_1) {
		delete m_skyst_slotsp1_1;
		m_skyst_slotsp1_1 = 0;
	}
	if (m_skyst_slotsp1_2) {
		delete m_skyst_slotsp1_2;
		m_skyst_slotsp1_2 = 0;
	}
	if (m_skyst_slotsp2_0) {
		delete m_skyst_slotsp2_0;
		m_skyst_slotsp2_0 = 0;
	}
	if (m_skyst_slotsp2_1) {
		delete m_skyst_slotsp2_1;
		m_skyst_slotsp2_1 = 0;
	}
	if (m_skyst_slotsp2_2) {
		delete m_skyst_slotsp2_2;
		m_skyst_slotsp2_2 = 0;
	}
	int slotindex;
	for (slotindex = 0; slotindex < 8; slotindex++) {
		if (m_skyst_slotB[slotindex]) {
			delete m_skyst_slotB[slotindex];
			m_skyst_slotB[slotindex] = nullptr;
		}
	}




	if (m_dlgWnd) {
		delete m_dlgWnd;
		m_dlgWnd = nullptr;
	}


	return 0;
}

void CSkyParamsDlg::InitParams()
{
	m_createdflag = false;
	m_visible = false;

	m_posx = 0;
	m_posy = 0;
	m_sizex = 150;
	m_sizey = 150;

	m_model = nullptr;

	m_skytoonmqomaterial = nullptr;//toonスライダーを離した後の処理用
	m_skytoonparamchange = false;//toonスライダーを離した後の処理用
	m_skyhsvtoonforall.Init();
	int skyindex;
	for (skyindex = 0; skyindex < SKYSLOTNUM; skyindex++) {
		m_skyparams[skyindex].InitParams(m_skyhsvtoonforall);
	}


	m_skyst_closeFlag = false;
	m_skyst_remakeToonTextureFlag = false;
	m_skyst_backFlag = false;
	m_skyst_stradioFlag = false;
	m_skyst_lightchkFlag = false;
	m_skyst_shadowcasterchkFlag = false;
	m_skyst_normaly0chkFlag = false;
	m_skyst_spccoefsliderFlag = false;
	m_skyst_emissionchkFlag = false;
	m_skyst_emissionsliderFlag = false;
	m_skyst_metalsliderFlag = false;
	m_skyst_smoothsliderFlag = false;
	m_skyst_litscale1Flag = false;
	m_skyst_litscale2Flag = false;
	m_skyst_litscale3Flag = false;
	m_skyst_litscale4Flag = false;
	m_skyst_litscale5Flag = false;
	m_skyst_litscale6Flag = false;
	m_skyst_litscale7Flag = false;
	m_skyst_litscale8Flag = false;
	m_skyst_toonlitradioFlag = false;
	m_skyst_toonhiaddrsliderFlag = false;
	m_skyst_toonlowaddrsliderFlag = false;
	m_skyst_gradationchkFlag = false;
	m_skyst_powertoonchkFlag = false;
	m_skyst_toonbaseHsliderFlag = false;
	m_skyst_toonbaseSsliderFlag = false;
	m_skyst_toonbaseVsliderFlag = false;
	m_skyst_toonbaseAsliderFlag = false;
	m_skyst_toonhiHsliderFlag = false;
	m_skyst_toonhiSsliderFlag = false;
	m_skyst_toonhiVsliderFlag = false;
	m_skyst_toonhiAsliderFlag = false;
	m_skyst_toonlowHsliderFlag = false;
	m_skyst_toonlowSsliderFlag = false;
	m_skyst_toonlowVsliderFlag = false;
	m_skyst_toonlowAsliderFlag = false;
	m_skyst_tilingUsliderFlag = false;
	m_skyst_tilingVsliderFlag = false;
	m_skyst_tilingUsliderUpFlag = false;
	m_skyst_tilingVsliderUpFlag = false;
	m_skyst_alphatestesliderFlag = false;
	m_skyst_distortionchkFlag = false;
	m_skyst_distortionscalesliderFlag = false;
	m_skyst_riverradioFlag = false;
	m_skyst_seacenterUsliderFlag = false;
	m_skyst_seacenterVsliderFlag = false;
	m_skyst_riverdirUsliderFlag = false;
	m_skyst_riverdirVsliderFlag = false;
	m_skyst_flowratesliderFlag = false;
	m_skyst_distortionmapradioFlag = false;


	m_dlgWnd = nullptr;
	m_skyst_Sc = nullptr;
	m_skyst_spall = nullptr;
	m_skyst_spacerLabel01 = nullptr;
	m_skyst_spacerLabel02 = nullptr;
	m_skyst_spacerLabel03 = nullptr;
	m_skyst_spacerLabel04 = nullptr;
	m_skyst_spacerLabel05 = nullptr;
	m_skyst_spacerLabel06 = nullptr;
	m_skyst_namesp = nullptr;
	m_skyst_backB = nullptr;
	m_skyst_namelabel = nullptr;
	m_skyst_shadertyperadio = nullptr;
	m_skyst_litflagsp = nullptr;
	m_skyst_lightflagchk = nullptr;
	m_skyst_shadowcasterchk = nullptr;
	m_skyst_normaly0chk = nullptr;
	m_skyst_spccoefsp0 = nullptr;
	m_skyst_spccoefsp1 = nullptr;
	m_skyst_spccoefsp2 = nullptr;
	m_skyst_spccoeflabel = nullptr;
	m_skyst_spccoefslider = nullptr;
	m_skyst_emissionchk = nullptr;
	m_skyst_emissionslider = nullptr;
	m_skyst_metalsp0 = nullptr;
	m_skyst_metalsp1 = nullptr;
	m_skyst_metalsp2 = nullptr;
	m_skyst_metallabel = nullptr;
	m_skyst_metalslider = nullptr;
	m_skyst_smoothlabel = nullptr;
	m_skyst_smoothslider = nullptr;

	m_skyst_litscalesp1_0 = nullptr;
	m_skyst_litscalesp1_1 = nullptr;
	m_skyst_litscalesp1_2 = nullptr;
	m_skyst_litscalelabel1 = nullptr;
	m_skyst_litscaleslider1 = nullptr;
	m_skyst_litscalelabel2 = nullptr;
	m_skyst_litscaleslider2 = nullptr;

	m_skyst_litscalesp3_0 = nullptr;
	m_skyst_litscalesp3_1 = nullptr;
	m_skyst_litscalesp3_2 = nullptr;
	m_skyst_litscalelabel3 = nullptr;
	m_skyst_litscaleslider3 = nullptr;
	m_skyst_litscalelabel4 = nullptr;
	m_skyst_litscaleslider4 = nullptr;

	m_skyst_litscalesp5_0 = nullptr;
	m_skyst_litscalesp5_1 = nullptr;
	m_skyst_litscalesp5_2 = nullptr;
	m_skyst_litscalelabel5 = nullptr;
	m_skyst_litscaleslider5 = nullptr;
	m_skyst_litscalelabel6 = nullptr;
	m_skyst_litscaleslider6 = nullptr;

	m_skyst_litscalesp7_0 = nullptr;
	m_skyst_litscalesp7_1 = nullptr;
	m_skyst_litscalesp7_2 = nullptr;
	m_skyst_litscalelabel7 = nullptr;
	m_skyst_litscaleslider7 = nullptr;
	m_skyst_litscalelabel8 = nullptr;
	m_skyst_litscaleslider8 = nullptr;

	m_skyst_toonlitradio = nullptr;

	m_skyst_toonaddrsp0 = nullptr;
	m_skyst_toonaddrsp1 = nullptr;
	m_skyst_toonaddrsp2 = nullptr;
	m_skyst_toonhiaddrlabel = nullptr;
	m_skyst_toonhiaddrslider = nullptr;
	m_skyst_toonlowaddrlabel = nullptr;
	m_skyst_toonlowaddrslider = nullptr;

	m_skyst_gradationsp0 = nullptr;
	m_skyst_gradationchk = nullptr;
	m_skyst_powertoonchk = nullptr;

	m_skyst_toonbasesp1_0 = nullptr;
	m_skyst_toonbasesp1_1 = nullptr;
	m_skyst_toonbasesp1_2 = nullptr;
	m_skyst_toonbaseHlabel = nullptr;
	m_skyst_toonbaseHslider = nullptr;
	m_skyst_toonbaseSlabel = nullptr;
	m_skyst_toonbaseSslider = nullptr;
	m_skyst_toonbasesp2_0 = nullptr;
	m_skyst_toonbasesp2_1 = nullptr;
	m_skyst_toonbasesp2_2 = nullptr;
	m_skyst_toonbaseVlabel = nullptr;
	m_skyst_toonbaseVslider = nullptr;
	m_skyst_toonbaseAlabel = nullptr;
	m_skyst_toonbaseAslider = nullptr;

	m_skyst_toonhisp1_0 = nullptr;
	m_skyst_toonhisp1_1 = nullptr;
	m_skyst_toonhisp1_2 = nullptr;
	m_skyst_toonhiHlabel = nullptr;
	m_skyst_toonhiHslider = nullptr;
	m_skyst_toonhiSlabel = nullptr;
	m_skyst_toonhiSslider = nullptr;
	m_skyst_toonhisp2_0 = nullptr;
	m_skyst_toonhisp2_1 = nullptr;
	m_skyst_toonhisp2_2 = nullptr;
	m_skyst_toonhiVlabel = nullptr;
	m_skyst_toonhiVslider = nullptr;
	m_skyst_toonhiAlabel = nullptr;
	m_skyst_toonhiAslider = nullptr;

	m_skyst_toonlowsp1_0 = nullptr;
	m_skyst_toonlowsp1_1 = nullptr;
	m_skyst_toonlowsp1_2 = nullptr;
	m_skyst_toonlowHlabel = nullptr;
	m_skyst_toonlowHslider = nullptr;
	m_skyst_toonlowSlabel = nullptr;
	m_skyst_toonlowSslider = nullptr;
	m_skyst_toonlowsp2_0 = nullptr;
	m_skyst_toonlowsp2_1 = nullptr;
	m_skyst_toonlowsp2_2 = nullptr;
	m_skyst_toonlowVlabel = nullptr;
	m_skyst_toonlowVslider = nullptr;
	m_skyst_toonlowAlabel = nullptr;
	m_skyst_toonlowAslider = nullptr;

	m_skyst_tilingsp0 = nullptr;
	m_skyst_tilingsp1 = nullptr;
	m_skyst_tilingsp2 = nullptr;
	m_skyst_tilingUlabel = nullptr;
	m_skyst_tilingUslider = nullptr;
	m_skyst_tilingVlabel = nullptr;
	m_skyst_tilingVslider = nullptr;

	m_skyst_alphatestsp0 = nullptr;
	m_skyst_alphatestsp1 = nullptr;
	m_skyst_alphatestlabel = nullptr;
	m_skyst_alphatestslider = nullptr;

	m_skyst_distortionsp0 = nullptr;
	m_skyst_distortionsp1 = nullptr;
	m_skyst_distortionchk = nullptr;
	m_skyst_distortionscalelabel = nullptr;
	m_skyst_distortionscaleslider = nullptr;

	m_skyst_riverradio = nullptr;

	m_skyst_seacentersp0 = nullptr;
	m_skyst_seacentersp1 = nullptr;
	m_skyst_seacenterlabel = nullptr;
	m_skyst_seacenterUslider = nullptr;
	m_skyst_seacenterVslider = nullptr;

	m_skyst_riverdirsp0 = nullptr;
	m_skyst_riverdirsp1 = nullptr;
	m_skyst_riverdirlabel = nullptr;
	m_skyst_riverdirUslider = nullptr;
	m_skyst_riverdirVslider = nullptr;

	m_skyst_flowratesp0 = nullptr;
	m_skyst_flowratelabel = nullptr;
	m_skyst_flowrateslider = nullptr;

	m_skyst_distortionmapradio = nullptr;


	//#########
	m_skyst_slotsp1_0 = nullptr;
	m_skyst_slotsp1_1 = nullptr;
	m_skyst_slotsp1_2 = nullptr;
	m_skyst_slotsp2_0 = nullptr;
	m_skyst_slotsp2_1 = nullptr;
	m_skyst_slotsp2_2 = nullptr;
	int slotindex;
	for (slotindex = 0; slotindex < 8; slotindex++) {
		m_skyst_slotB[slotindex] = nullptr;
	}
	
	m_skyst_slotindex = 0;
	m_skyst_slotFlag = false;

}

int CSkyParamsDlg::SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey)
{
	m_posx = srcposx;
	m_posy = srcposy;
	m_sizex = srcsizex;
	m_sizey = srcsizey;

	return 0;
}

//void CSkyParamsDlg::SetModel(CModel* srcmodel, CShaderTypeParams* srcshadertypeparams)
//{
//	m_model = srcmodel;
//	CreateSkyParamsWnd();
//	if (srcshadertypeparams) {
//		ParamsToDlg(nullptr, srcshadertypeparams);
//	}
//
//	if (m_dlgWnd && m_dlgWnd->getVisible()) {
//		if (m_skyst_Sc) {
//			m_skyst_Sc->autoResize();
//		}
//		m_dlgWnd->callRewrite();
//	}
//}


void CSkyParamsDlg::SetVisible(bool srcflag)
{
	if (srcflag) {
		if (m_dlgWnd) {//ウインドウ作成はSetModel()にて行う
			//ParamsToDlg();

			m_dlgWnd->setListenMouse(true);
			m_dlgWnd->setVisible(true);
			if (m_skyst_Sc) {
				//############
				//2024/07/24
				//############
				//int showposline = m_dlgSc->getShowPosLine();
				//m_dlgSc->setShowPosLine(showposline);
				//コピー履歴をスクロールしてチェック-->他の右ペインウインドウを表示-->再びコピー履歴表示としたときに
				//ラベルは表示されたがセパレータの中にあるチェックボックスとボタンが表示されなかった
				//スクロールバーを少し動かすと全て表示された
				//スクロール処理のsetShowPosLine()から呼び出していたautoResize()が必要だった
				m_skyst_Sc->autoResize();
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


int CSkyParamsDlg::CreateSkyParamsWnd()
{

	DestroyObjs();

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}

	m_dlgWnd = new OrgWindow(
		0,
		_T("ShaderSkyParamsWindow"),		//ウィンドウクラス名
		GetModuleHandle(NULL),	//インスタンスハンドル
		WindowPos(m_posx, m_posy),
		WindowSize(m_sizex, m_sizey),		//サイズ
		_T("ShaderSkyParamsWindow"),	//タイトル
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

		m_skyst_Sc = new OWP_ScrollWnd(L"SkyParamsScroll", false, labelheight);//wheelフラグOFF：ホイールでスライダーを動かしたいから
		if (!m_skyst_Sc) {
			_ASSERT(0);
			return 1;
		}
		int linedatasize = (int)(46.0 * 1.25);
		m_skyst_Sc->setLineDataSize(linedatasize);//!!!!!!!!!!!!
		m_dlgWnd->addParts(*m_skyst_Sc);


		//スクロールするGUI全てを束ねるセパレータは必要
		m_skyst_spall = new OWP_Separator(m_dlgWnd, true, 0.995, false, m_skyst_Sc);
		if (!m_skyst_spall) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_Sc->addParts(*m_skyst_spall);

		m_skyst_spacerLabel01 = new OWP_Label(L"     ", labelheight);
		if (!m_skyst_spacerLabel01) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spacerLabel02 = new OWP_Label(L"     ", labelheight);
		if (!m_skyst_spacerLabel02) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spacerLabel03 = new OWP_Label(L"     ", labelheight);
		if (!m_skyst_spacerLabel03) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spacerLabel04 = new OWP_Label(L"     ", labelheight);
		if (!m_skyst_spacerLabel04) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spacerLabel05 = new OWP_Label(L"     ", labelheight);
		if (!m_skyst_spacerLabel05) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spacerLabel06 = new OWP_Label(L"     ", labelheight);
		if (!m_skyst_spacerLabel06) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_namesp = new OWP_Separator(m_dlgWnd, true, 0.2, true);
		if (!m_skyst_namesp) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_backB = new OWP_Button(L"<-Back", labelheight);
		if (!m_skyst_backB) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_namelabel = new OWP_Label(L"MaterialName", labelheight);
		if (!m_skyst_namelabel) {
			_ASSERT(0);
			return 1;
		}
		//COLORREF colorToAll = RGB(64, 128 + 32, 128 + 32);
		//COLORREF colorValidInvalid = RGB(168, 129, 129);
		//COLORREF colorToDeeper = RGB(24, 126, 176);
		COLORREF colorCaution = RGB(168, 129, 129);
		m_skyst_namelabel->setTextColor(colorCaution);


		m_skyst_slotsp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_slotsp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_slotsp1_1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_slotsp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_slotsp1_2 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_slotsp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_slotsp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_slotsp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_slotsp2_1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_slotsp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_slotsp2_2 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_slotsp2_2) {
			_ASSERT(0);
			return 1;
		}

		int slotindex;
		for (slotindex = 0; slotindex < 8; slotindex++) {
			WCHAR strslotno[256] = { 0L };
			swprintf_s(strslotno, 256, L"Slot%d", (slotindex + 1));
			m_skyst_slotB[slotindex] = new OWP_Button(strslotno, labelheight);
			if (!m_skyst_slotB[slotindex]) {
				_ASSERT(0);
				return 1;
			}
		}



		m_skyst_shadertyperadio = new OWP_RadioButton(L"Auto", limitradionamelen, labelheight);
		if (!m_skyst_shadertyperadio) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_shadertyperadio->addLine(L"PBR");
		m_skyst_shadertyperadio->addLine(L"Std.");
		m_skyst_shadertyperadio->addLine(L"HSV Toon");


		m_skyst_litflagsp = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_litflagsp) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_lightflagchk = new OWP_CheckBoxA(L"Lighting", 0, labelheight, false);
		if (!m_skyst_lightflagchk) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_shadowcasterchk = new OWP_CheckBoxA(L"ShadowCaster", 0, labelheight, false);
		if (!m_skyst_shadowcasterchk) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_normaly0chk = new OWP_CheckBoxA(L"NormalY0", 0, labelheight, false);
		if (!m_skyst_normaly0chk) {
			_ASSERT(0);
			return 1;
		}



		m_skyst_spccoefsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_spccoefsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spccoefsp1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_spccoefsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spccoefsp2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_spccoefsp2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spccoeflabel = new OWP_Label(L"Specular", labelheight);
		if (!m_skyst_spccoeflabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_spccoefslider = new OWP_Slider(0.0, 2.0, 0.0, labelheight);
		if (!m_skyst_spccoefslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_emissionchk = new OWP_CheckBoxA(L"Emi", 0, labelheight, false);
		if (!m_skyst_emissionchk) {
			_ASSERT(0);
			return 1;
		}
		//m_skyst_emissionslider = new OWP_Slider(0.0, 1.0, 0.0);
		m_skyst_emissionslider = new OWP_Slider(0.0, 10.0, 0.0, labelheight);//2024/06/23 max 10.0
		if (!m_skyst_emissionslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_metalsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_metalsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_metalsp1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_metalsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_metalsp2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_metalsp2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_metallabel = new OWP_Label(L"Metal", labelheight);
		if (!m_skyst_metallabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_metalslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_metalslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_smoothlabel = new OWP_Label(L"Smooth", labelheight);
		if (!m_skyst_smoothlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_smoothslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_smoothslider) {
			_ASSERT(0);
			return 1;
		}



		m_skyst_litscalesp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_litscalesp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp1_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp1_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel1 = new OWP_Label(L"Lit1", labelheight);
		if (!m_skyst_litscalelabel1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider1 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel2 = new OWP_Label(L"Lit2", labelheight);
		if (!m_skyst_litscalelabel2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider2 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider2) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_litscalesp3_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_litscalesp3_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp3_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp3_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp3_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp3_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel3 = new OWP_Label(L"Lit3", labelheight);
		if (!m_skyst_litscalelabel3) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider3 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider3) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel4 = new OWP_Label(L"Lit4", labelheight);
		if (!m_skyst_litscalelabel4) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider4 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider4) {
			_ASSERT(0);
			return 1;
		}



		m_skyst_litscalesp5_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_litscalesp5_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp5_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp5_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp5_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp5_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel5 = new OWP_Label(L"Lit5", labelheight);
		if (!m_skyst_litscalelabel5) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider5 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider5) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel6 = new OWP_Label(L"Lit6", labelheight);
		if (!m_skyst_litscalelabel6) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider6 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider6) {
			_ASSERT(0);
			return 1;
		}



		m_skyst_litscalesp7_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_litscalesp7_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp7_1 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp7_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalesp7_2 = new OWP_Separator(m_dlgWnd, true, separateratelit, true);
		if (!m_skyst_litscalesp7_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel7 = new OWP_Label(L"Lit7", labelheight);
		if (!m_skyst_litscalelabel7) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider7 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider7) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscalelabel8 = new OWP_Label(L"Lit8", labelheight);
		if (!m_skyst_litscalelabel8) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_litscaleslider8 = new OWP_Slider(0.0, 5.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider8) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_toonlitradio = new OWP_RadioButton(L"ToonLit1", limitradionamelen, labelheight);
		if (!m_skyst_toonlitradio) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlitradio->addLine(L"ToonLit2");
		m_skyst_toonlitradio->addLine(L"ToonLit3");
		m_skyst_toonlitradio->addLine(L"ToonLit4");
		m_skyst_toonlitradio->addLine(L"ToonLit5");
		m_skyst_toonlitradio->addLine(L"ToonLit6");
		m_skyst_toonlitradio->addLine(L"ToonLit7");
		m_skyst_toonlitradio->addLine(L"ToonLit8");


		m_skyst_toonaddrsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonaddrsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonaddrsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonaddrsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonaddrsp2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonaddrsp2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiaddrlabel = new OWP_Label(L"HiAddr", labelheight);
		if (!m_skyst_toonhiaddrlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiaddrslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_toonhiaddrslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowaddrlabel = new OWP_Label(L"LowAddr", labelheight);
		if (!m_skyst_toonlowaddrlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowaddrslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_litscaleslider8) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_gradationsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_gradationsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_gradationchk = new OWP_CheckBoxA(L"Gradation", 0, labelheight, false);
		if (!m_skyst_gradationchk) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_powertoonchk = new OWP_CheckBoxA(L"PowerToon", 0, labelheight, false);
		if (!m_skyst_powertoonchk) {
			_ASSERT(0);
			return 1;
		}



		m_skyst_toonbasesp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonbasesp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbasesp1_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonbasesp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbasesp1_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonbasesp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseHlabel = new OWP_Label(L"BaseH", labelheight);
		if (!m_skyst_toonbaseHlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseHslider = new OWP_Slider(0.0, 360.0, 0.0, labelheight);
		if (!m_skyst_toonbaseHslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseSlabel = new OWP_Label(L"BaseS", labelheight);
		if (!m_skyst_toonbaseSlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseSslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_toonbaseSslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbasesp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonbasesp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbasesp2_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonbasesp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbasesp2_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonbasesp2_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseVlabel = new OWP_Label(L"BaseV", labelheight);
		if (!m_skyst_toonbaseVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseVslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_toonbaseVslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseAlabel = new OWP_Label(L"BaseA", labelheight);
		if (!m_skyst_toonbaseAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonbaseAslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_toonbaseAslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_toonhisp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonhisp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhisp1_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonhisp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhisp1_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonhisp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiHlabel = new OWP_Label(L"HighH", labelheight);
		if (!m_skyst_toonhiHlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiHslider = new OWP_Slider(0.0, 360.0, -360.0, labelheight);
		if (!m_skyst_toonhiHslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiSlabel = new OWP_Label(L"HighS", labelheight);
		if (!m_skyst_toonhiSlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiSslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_skyst_toonhiSslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhisp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonhisp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhisp2_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonhisp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhisp2_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonhisp2_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiVlabel = new OWP_Label(L"HighV", labelheight);
		if (!m_skyst_toonhiVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiVslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_skyst_toonhiVslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiAlabel = new OWP_Label(L"HighA", labelheight);
		if (!m_skyst_toonhiAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonhiAslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_skyst_toonhiAslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_toonlowsp1_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonlowsp1_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowsp1_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonlowsp1_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowsp1_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonlowsp1_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowHlabel = new OWP_Label(L"LowH", labelheight);
		if (!m_skyst_toonlowHlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowHslider = new OWP_Slider(0.0, 360.0, -360.0, labelheight);
		if (!m_skyst_toonlowHslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowSlabel = new OWP_Label(L"LowS", labelheight);
		if (!m_skyst_toonlowSlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowSslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_skyst_toonlowSslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowsp2_0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_toonlowsp2_0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowsp2_1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonlowsp2_1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowsp2_2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_toonlowsp2_2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowVlabel = new OWP_Label(L"LowV", labelheight);
		if (!m_skyst_toonlowVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowVslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_skyst_toonlowVslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowAlabel = new OWP_Label(L"LowA", labelheight);
		if (!m_skyst_toonlowAlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_toonlowAslider = new OWP_Slider(0.0, 1.0, -1.0, labelheight);
		if (!m_skyst_toonlowAslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_tilingsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_tilingsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_tilingsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_tilingsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_tilingsp2 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_tilingsp2) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_tilingUlabel = new OWP_Label(L"TileU", labelheight);
		if (!m_skyst_tilingUlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_tilingUslider = new OWP_Slider(0.0, 100.0, 0.0, labelheight);
		if (!m_skyst_tilingUslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_tilingVlabel = new OWP_Label(L"TileV", labelheight);
		if (!m_skyst_tilingVlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_tilingVslider = new OWP_Slider(0.0, 100.0, 0.0, labelheight);
		if (!m_skyst_tilingVslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_alphatestsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_alphatestsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_alphatestsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_alphatestsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_alphatestlabel = new OWP_Label(L"ATest", labelheight);
		if (!m_skyst_alphatestlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_alphatestslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_alphatestslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_distortionsp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_distortionsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_distortionsp1 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_distortionsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_distortionchk = new OWP_CheckBoxA(L"SimpleWater(PBR,NoSkin)", 0, labelheight, false);
		if (!m_skyst_distortionchk) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_distortionscalelabel = new OWP_Label(L"Scale", labelheight);
		if (!m_skyst_distortionscalelabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_distortionscaleslider = new OWP_Slider(1.0, 10.0, 0.0, labelheight);
		if (!m_skyst_distortionscaleslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_riverradio = new OWP_RadioButton(L"river", limitradionamelen, labelheight);
		if (!m_skyst_riverradio) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_riverradio->addLine(L"sea");


		m_skyst_seacentersp0 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_seacentersp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_seacentersp1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_seacentersp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_seacenterlabel = new OWP_Label(L"SeaCenterUV", labelheight);
		if (!m_skyst_seacenterlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_seacenterUslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_seacenterUslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_seacenterVslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_seacenterVslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_riverdirsp0 = new OWP_Separator(m_dlgWnd, true, separaterate, true);
		if (!m_skyst_riverdirsp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_riverdirsp1 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_riverdirsp1) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_riverdirlabel = new OWP_Label(L"FlowDirUV", labelheight);
		if (!m_skyst_riverdirlabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_riverdirUslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_riverdirUslider) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_riverdirVslider = new OWP_Slider(0.0, 1.0, 0.0, labelheight);
		if (!m_skyst_riverdirVslider) {
			_ASSERT(0);
			return 1;
		}


		m_skyst_flowratesp0 = new OWP_Separator(m_dlgWnd, true, 0.5, true);
		if (!m_skyst_flowratesp0) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_flowratelabel = new OWP_Label(L"FlowRate", labelheight);
		if (!m_skyst_flowratelabel) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_flowrateslider = new OWP_Slider(1.0, 4.0, -4.0, labelheight);
		if (!m_skyst_flowrateslider) {
			_ASSERT(0);
			return 1;
		}



		m_skyst_distortionmapradio = new OWP_RadioButton(L"RG", limitradionamelen, labelheight);
		if (!m_skyst_distortionmapradio) {
			_ASSERT(0);
			return 1;
		}
		m_skyst_distortionmapradio->addLine(L"GB");
		m_skyst_distortionmapradio->addLine(L"RB");


		m_skyst_spall->addParts1(*m_skyst_namesp);
		m_skyst_namesp->addParts1(*m_skyst_backB);
		m_skyst_namesp->addParts2(*m_skyst_namelabel);

		m_skyst_spall->addParts1(*m_skyst_slotsp1_0);
		m_skyst_slotsp1_0->addParts1(*m_skyst_slotsp1_1);
		m_skyst_slotsp1_0->addParts2(*m_skyst_slotsp1_2);
		m_skyst_slotsp1_1->addParts1(*m_skyst_slotB[0]);
		m_skyst_slotsp1_1->addParts2(*m_skyst_slotB[1]);
		m_skyst_slotsp1_2->addParts1(*m_skyst_slotB[2]);
		m_skyst_slotsp1_2->addParts2(*m_skyst_slotB[3]);

		m_skyst_spall->addParts1(*m_skyst_slotsp2_0);
		m_skyst_slotsp2_0->addParts1(*m_skyst_slotsp2_1);
		m_skyst_slotsp2_0->addParts2(*m_skyst_slotsp2_2);
		m_skyst_slotsp2_1->addParts1(*m_skyst_slotB[4]);
		m_skyst_slotsp2_1->addParts2(*m_skyst_slotB[5]);
		m_skyst_slotsp2_2->addParts1(*m_skyst_slotB[6]);
		m_skyst_slotsp2_2->addParts2(*m_skyst_slotB[7]);
		m_skyst_spall->addParts1(*m_skyst_spacerLabel01);

		m_skyst_spall->addParts1(*m_skyst_shadertyperadio);
		m_skyst_spall->addParts1(*m_skyst_spacerLabel02);

		m_skyst_spall->addParts1(*m_skyst_litflagsp);
		m_skyst_litflagsp->addParts1(*m_skyst_lightflagchk);
		m_skyst_litflagsp->addParts2(*m_skyst_shadowcasterchk);
		m_skyst_spall->addParts1(*m_skyst_normaly0chk);
		m_skyst_spall->addParts1(*m_skyst_spacerLabel03);

		m_skyst_spall->addParts1(*m_skyst_spccoefsp0);
		m_skyst_spccoefsp0->addParts1(*m_skyst_spccoefsp1);
		m_skyst_spccoefsp1->addParts1(*m_skyst_spccoeflabel);
		m_skyst_spccoefsp1->addParts2(*m_skyst_spccoefslider);
		m_skyst_spccoefsp0->addParts2(*m_skyst_spccoefsp2);
		m_skyst_spccoefsp2->addParts1(*m_skyst_emissionchk);
		m_skyst_spccoefsp2->addParts2(*m_skyst_emissionslider);

		m_skyst_spall->addParts1(*m_skyst_metalsp0);
		m_skyst_metalsp0->addParts1(*m_skyst_metalsp1);
		m_skyst_metalsp1->addParts1(*m_skyst_metallabel);
		m_skyst_metalsp1->addParts2(*m_skyst_metalslider);
		m_skyst_metalsp0->addParts2(*m_skyst_metalsp2);
		m_skyst_metalsp2->addParts1(*m_skyst_smoothlabel);
		m_skyst_metalsp2->addParts2(*m_skyst_smoothslider);

		m_skyst_spall->addParts1(*m_skyst_litscalesp1_0);
		m_skyst_litscalesp1_0->addParts1(*m_skyst_litscalesp1_1);
		m_skyst_litscalesp1_1->addParts1(*m_skyst_litscalelabel1);
		m_skyst_litscalesp1_1->addParts2(*m_skyst_litscaleslider1);
		m_skyst_litscalesp1_0->addParts2(*m_skyst_litscalesp1_2);
		m_skyst_litscalesp1_2->addParts1(*m_skyst_litscalelabel2);
		m_skyst_litscalesp1_2->addParts2(*m_skyst_litscaleslider2);

		m_skyst_spall->addParts1(*m_skyst_litscalesp3_0);
		m_skyst_litscalesp3_0->addParts1(*m_skyst_litscalesp3_1);
		m_skyst_litscalesp3_1->addParts1(*m_skyst_litscalelabel3);
		m_skyst_litscalesp3_1->addParts2(*m_skyst_litscaleslider3);
		m_skyst_litscalesp3_0->addParts2(*m_skyst_litscalesp3_2);
		m_skyst_litscalesp3_2->addParts1(*m_skyst_litscalelabel4);
		m_skyst_litscalesp3_2->addParts2(*m_skyst_litscaleslider4);


		m_skyst_spall->addParts1(*m_skyst_litscalesp5_0);
		m_skyst_litscalesp5_0->addParts1(*m_skyst_litscalesp5_1);
		m_skyst_litscalesp5_1->addParts1(*m_skyst_litscalelabel5);
		m_skyst_litscalesp5_1->addParts2(*m_skyst_litscaleslider5);
		m_skyst_litscalesp5_0->addParts2(*m_skyst_litscalesp5_2);
		m_skyst_litscalesp5_2->addParts1(*m_skyst_litscalelabel6);
		m_skyst_litscalesp5_2->addParts2(*m_skyst_litscaleslider6);


		m_skyst_spall->addParts1(*m_skyst_litscalesp7_0);
		m_skyst_litscalesp7_0->addParts1(*m_skyst_litscalesp7_1);
		m_skyst_litscalesp7_1->addParts1(*m_skyst_litscalelabel7);
		m_skyst_litscalesp7_1->addParts2(*m_skyst_litscaleslider7);
		m_skyst_litscalesp7_0->addParts2(*m_skyst_litscalesp7_2);
		m_skyst_litscalesp7_2->addParts1(*m_skyst_litscalelabel8);
		m_skyst_litscalesp7_2->addParts2(*m_skyst_litscaleslider8);
		m_skyst_spall->addParts1(*m_skyst_spacerLabel04);

		m_skyst_spall->addParts1(*m_skyst_toonlitradio);
		m_skyst_spall->addParts1(*m_skyst_spacerLabel05);

		m_skyst_spall->addParts1(*m_skyst_toonaddrsp0);
		m_skyst_toonaddrsp0->addParts1(*m_skyst_toonaddrsp1);
		m_skyst_toonaddrsp1->addParts1(*m_skyst_toonhiaddrlabel);
		m_skyst_toonaddrsp1->addParts2(*m_skyst_toonhiaddrslider);
		m_skyst_toonaddrsp0->addParts2(*m_skyst_toonaddrsp2);
		m_skyst_toonaddrsp2->addParts1(*m_skyst_toonlowaddrlabel);
		m_skyst_toonaddrsp2->addParts2(*m_skyst_toonlowaddrslider);


		m_skyst_spall->addParts1(*m_skyst_gradationsp0);
		m_skyst_gradationsp0->addParts1(*m_skyst_gradationchk);
		m_skyst_gradationsp0->addParts2(*m_skyst_powertoonchk);


		m_skyst_spall->addParts1(*m_skyst_toonbasesp1_0);
		m_skyst_toonbasesp1_0->addParts1(*m_skyst_toonbasesp1_1);
		m_skyst_toonbasesp1_1->addParts1(*m_skyst_toonbaseHlabel);
		m_skyst_toonbasesp1_1->addParts2(*m_skyst_toonbaseHslider);
		m_skyst_toonbasesp1_0->addParts2(*m_skyst_toonbasesp1_2);
		m_skyst_toonbasesp1_2->addParts1(*m_skyst_toonbaseSlabel);
		m_skyst_toonbasesp1_2->addParts2(*m_skyst_toonbaseSslider);
		m_skyst_spall->addParts1(*m_skyst_toonbasesp2_0);
		m_skyst_toonbasesp2_0->addParts1(*m_skyst_toonbasesp2_1);
		m_skyst_toonbasesp2_1->addParts1(*m_skyst_toonbaseVlabel);
		m_skyst_toonbasesp2_1->addParts2(*m_skyst_toonbaseVslider);
		m_skyst_toonbasesp2_0->addParts2(*m_skyst_toonbasesp2_2);
		m_skyst_toonbasesp2_2->addParts1(*m_skyst_toonbaseAlabel);
		m_skyst_toonbasesp2_2->addParts2(*m_skyst_toonbaseAslider);


		m_skyst_spall->addParts1(*m_skyst_toonhisp1_0);
		m_skyst_toonhisp1_0->addParts1(*m_skyst_toonhisp1_1);
		m_skyst_toonhisp1_1->addParts1(*m_skyst_toonhiHlabel);
		m_skyst_toonhisp1_1->addParts2(*m_skyst_toonhiHslider);
		m_skyst_toonhisp1_0->addParts2(*m_skyst_toonhisp1_2);
		m_skyst_toonhisp1_2->addParts1(*m_skyst_toonhiSlabel);
		m_skyst_toonhisp1_2->addParts2(*m_skyst_toonhiSslider);
		m_skyst_spall->addParts1(*m_skyst_toonhisp2_0);
		m_skyst_toonhisp2_0->addParts1(*m_skyst_toonhisp2_1);
		m_skyst_toonhisp2_1->addParts1(*m_skyst_toonhiVlabel);
		m_skyst_toonhisp2_1->addParts2(*m_skyst_toonhiVslider);
		m_skyst_toonhisp2_0->addParts2(*m_skyst_toonhisp2_2);
		m_skyst_toonhisp2_2->addParts1(*m_skyst_toonhiAlabel);
		m_skyst_toonhisp2_2->addParts2(*m_skyst_toonhiAslider);


		m_skyst_spall->addParts1(*m_skyst_toonlowsp1_0);
		m_skyst_toonlowsp1_0->addParts1(*m_skyst_toonlowsp1_1);
		m_skyst_toonlowsp1_1->addParts1(*m_skyst_toonlowHlabel);
		m_skyst_toonlowsp1_1->addParts2(*m_skyst_toonlowHslider);
		m_skyst_toonlowsp1_0->addParts2(*m_skyst_toonlowsp1_2);
		m_skyst_toonlowsp1_2->addParts1(*m_skyst_toonlowSlabel);
		m_skyst_toonlowsp1_2->addParts2(*m_skyst_toonlowSslider);
		m_skyst_spall->addParts1(*m_skyst_toonlowsp2_0);
		m_skyst_toonlowsp2_0->addParts1(*m_skyst_toonlowsp2_1);
		m_skyst_toonlowsp2_1->addParts1(*m_skyst_toonlowVlabel);
		m_skyst_toonlowsp2_1->addParts2(*m_skyst_toonlowVslider);
		m_skyst_toonlowsp2_0->addParts2(*m_skyst_toonlowsp2_2);
		m_skyst_toonlowsp2_2->addParts1(*m_skyst_toonlowAlabel);
		m_skyst_toonlowsp2_2->addParts2(*m_skyst_toonlowAslider);


		m_skyst_spall->addParts1(*m_skyst_tilingsp0);
		m_skyst_tilingsp0->addParts1(*m_skyst_tilingsp1);
		m_skyst_tilingsp1->addParts1(*m_skyst_tilingUlabel);
		m_skyst_tilingsp1->addParts2(*m_skyst_tilingUslider);
		m_skyst_tilingsp0->addParts2(*m_skyst_tilingsp2);
		m_skyst_tilingsp2->addParts1(*m_skyst_tilingVlabel);
		m_skyst_tilingsp2->addParts2(*m_skyst_tilingVslider);

		m_skyst_spall->addParts1(*m_skyst_alphatestsp0);
		m_skyst_alphatestsp0->addParts1(*m_skyst_alphatestsp1);
		m_skyst_alphatestsp1->addParts1(*m_skyst_alphatestlabel);
		m_skyst_alphatestsp1->addParts2(*m_skyst_alphatestslider);
		m_skyst_spall->addParts1(*m_skyst_spacerLabel06);

		m_skyst_spall->addParts1(*m_skyst_distortionsp0);
		m_skyst_distortionsp0->addParts1(*m_skyst_distortionchk);
		m_skyst_distortionsp0->addParts2(*m_skyst_distortionsp1);
		m_skyst_distortionsp1->addParts1(*m_skyst_distortionscalelabel);
		m_skyst_distortionsp1->addParts2(*m_skyst_distortionscaleslider);

		//m_skyst_spall->addParts1(*m_skyst_riverradio);

		////m_skyst_spall->addParts1(*m_skyst_seacentersp0);
		////m_skyst_seacentersp0->addParts1(*m_skyst_seacenterlabel);
		////m_skyst_seacentersp0->addParts2(*m_skyst_seacentersp1);
		////m_skyst_seacentersp1->addParts1(*m_skyst_seacenterUslider);
		////m_skyst_seacentersp1->addParts2(*m_skyst_seacenterVslider);

		//m_skyst_spall->addParts1(*m_skyst_riverdirsp0);
		//m_skyst_riverdirsp0->addParts1(*m_skyst_riverdirlabel);
		//m_skyst_riverdirsp0->addParts2(*m_skyst_riverdirsp1);
		//m_skyst_riverdirsp1->addParts1(*m_skyst_riverdirUslider);
		//m_skyst_riverdirsp1->addParts2(*m_skyst_riverdirVslider);


		//m_skyst_spall->addParts1(*m_skyst_flowratesp0);
		//m_skyst_flowratesp0->addParts1(*m_skyst_flowratelabel);
		//m_skyst_flowratesp0->addParts2(*m_skyst_flowrateslider);

		//m_skyst_spall->addParts1(*m_skyst_distortionmapradio);


		if (m_dlgWnd) {
			m_dlgWnd->setCloseListener([=, this]() {
				if (m_model) {
					m_skyst_closeFlag = true;
				}
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_backB) {
			m_skyst_backB->setButtonListener([=, this]() {
				m_skyst_backFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		int setindex2;
		for (setindex2 = 0; setindex2 < 8; setindex2++) {
			if (m_skyst_slotB[setindex2]) {
				m_skyst_slotB[setindex2]->setButtonListener([=, this]() {
					if (!m_skyst_slotFlag) {
						m_skyst_slotindex = setindex2;
						m_skyst_slotFlag = true;
					}
					if (m_dlgWnd) {
						m_dlgWnd->callRewrite();//再描画
					}
					});
			}
		}


		if (m_skyst_shadertyperadio) {
			m_skyst_shadertyperadio->setSelectListener([=, this]() {
				m_skyst_stradioFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_lightflagchk) {
			m_skyst_lightflagchk->setButtonListener([=, this]() {
				m_skyst_lightchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_shadowcasterchk) {
			m_skyst_shadowcasterchk->setButtonListener([=, this]() {
				m_skyst_shadowcasterchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_normaly0chk) {
			m_skyst_normaly0chk->setButtonListener([=, this]() {
				m_skyst_normaly0chkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_spccoefslider) {
			m_skyst_spccoefslider->setCursorListener([=, this]() {
				m_skyst_spccoefsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_emissionchk) {
			m_skyst_emissionchk->setButtonListener([=, this]() {
				m_skyst_emissionchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_emissionslider) {
			m_skyst_emissionslider->setCursorListener([=, this]() {
				m_skyst_emissionsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_metalslider) {
			m_skyst_metalslider->setCursorListener([=, this]() {
				m_skyst_metalsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_smoothslider) {
			m_skyst_smoothslider->setCursorListener([=, this]() {
				m_skyst_smoothsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider1) {
			m_skyst_litscaleslider1->setCursorListener([=, this]() {
				m_skyst_litscale1Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider2) {
			m_skyst_litscaleslider2->setCursorListener([=, this]() {
				m_skyst_litscale2Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider3) {
			m_skyst_litscaleslider3->setCursorListener([=, this]() {
				m_skyst_litscale3Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider4) {
			m_skyst_litscaleslider4->setCursorListener([=, this]() {
				m_skyst_litscale4Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider5) {
			m_skyst_litscaleslider5->setCursorListener([=, this]() {
				m_skyst_litscale5Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider6) {
			m_skyst_litscaleslider6->setCursorListener([=, this]() {
				m_skyst_litscale6Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider7) {
			m_skyst_litscaleslider7->setCursorListener([=, this]() {
				m_skyst_litscale7Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_litscaleslider8) {
			m_skyst_litscaleslider8->setCursorListener([=, this]() {
				m_skyst_litscale8Flag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlitradio) {
			m_skyst_toonlitradio->setSelectListener([=, this]() {
				m_skyst_toonlitradioFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiaddrslider) {
			m_skyst_toonhiaddrslider->setCursorListener([=, this]() {
				m_skyst_toonhiaddrsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_skyst_toonhiaddrslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowaddrslider) {
			m_skyst_toonlowaddrslider->setCursorListener([=, this]() {
				m_skyst_toonlowaddrsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_skyst_toonlowaddrslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_gradationchk) {
			m_skyst_gradationchk->setButtonListener([=, this]() {
				m_skyst_gradationchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_powertoonchk) {
			m_skyst_powertoonchk->setButtonListener([=, this]() {
				m_skyst_powertoonchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseHslider) {
			m_skyst_toonbaseHslider->setCursorListener([=, this]() {
				m_skyst_toonbaseHsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseSslider) {
			m_skyst_toonbaseSslider->setCursorListener([=, this]() {
				m_skyst_toonbaseSsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseVslider) {
			m_skyst_toonbaseVslider->setCursorListener([=, this]() {
				m_skyst_toonbaseVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseAslider) {
			m_skyst_toonbaseAslider->setCursorListener([=, this]() {
				m_skyst_toonbaseAsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseHslider) {
			m_skyst_toonbaseHslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseSslider) {
			m_skyst_toonbaseSslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseVslider) {
			m_skyst_toonbaseVslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonbaseAslider) {
			m_skyst_toonbaseAslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_skyst_toonhiHslider) {
			m_skyst_toonhiHslider->setCursorListener([=, this]() {
				m_skyst_toonhiHsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiSslider) {
			m_skyst_toonhiSslider->setCursorListener([=, this]() {
				m_skyst_toonhiSsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiVslider) {
			m_skyst_toonhiVslider->setCursorListener([=, this]() {
				m_skyst_toonhiVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiAslider) {
			m_skyst_toonhiAslider->setCursorListener([=, this]() {
				m_skyst_toonhiAsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiHslider) {
			m_skyst_toonhiHslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiSslider) {
			m_skyst_toonhiSslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiVslider) {
			m_skyst_toonhiVslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonhiAslider) {
			m_skyst_toonhiAslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_skyst_toonlowHslider) {
			m_skyst_toonlowHslider->setCursorListener([=, this]() {
				m_skyst_toonlowHsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowSslider) {
			m_skyst_toonlowSslider->setCursorListener([=, this]() {
				m_skyst_toonlowSsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowVslider) {
			m_skyst_toonlowVslider->setCursorListener([=, this]() {
				m_skyst_toonlowVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowAslider) {
			m_skyst_toonlowAslider->setCursorListener([=, this]() {
				m_skyst_toonlowAsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowHslider) {
			m_skyst_toonlowHslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowSslider) {
			m_skyst_toonlowSslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowVslider) {
			m_skyst_toonlowVslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_toonlowAslider) {
			m_skyst_toonlowAslider->setLUpListener([=, this]() {
				m_skyst_remakeToonTextureFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_skyst_tilingUslider) {
			m_skyst_tilingUslider->setCursorListener([=, this]() {
				m_skyst_tilingUsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_skyst_tilingUslider->setLUpListener([=, this]() {
				m_skyst_tilingUsliderUpFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_tilingVslider) {
			m_skyst_tilingVslider->setCursorListener([=, this]() {
				m_skyst_tilingVsliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
			m_skyst_tilingVslider->setLUpListener([=, this]() {
				m_skyst_tilingVsliderUpFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_skyst_alphatestslider) {
			m_skyst_alphatestslider->setCursorListener([=, this]() {
				m_skyst_alphatestesliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		if (m_skyst_distortionchk) {
			m_skyst_distortionchk->setButtonListener([=, this]() {
				m_skyst_distortionchkFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}
		if (m_skyst_distortionscaleslider) {
			m_skyst_distortionscaleslider->setCursorListener([=, this]() {
				m_skyst_distortionscalesliderFlag = true;
				if (m_dlgWnd) {
					m_dlgWnd->callRewrite();//再描画
				}
				});
		}

		//if (m_skyst_riverradio) {
		//	m_skyst_riverradio->setSelectListener([=, this]() {
		//		m_skyst_riverradioFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}

		//if (m_skyst_seacenterUslider) {
		//	m_skyst_seacenterUslider->setCursorListener([=, this]() {
		//		m_skyst_seacenterUsliderFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}
		//if (m_skyst_seacenterVslider) {
		//	m_skyst_seacenterVslider->setCursorListener([=, this]() {
		//		m_skyst_seacenterVsliderFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}

		//if (m_skyst_riverdirUslider) {
		//	m_skyst_riverdirUslider->setCursorListener([=, this]() {
		//		m_skyst_riverdirUsliderFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}
		//if (m_skyst_riverdirVslider) {
		//	m_skyst_riverdirVslider->setCursorListener([=, this]() {
		//		m_skyst_riverdirVsliderFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}

		//if (m_skyst_flowrateslider) {
		//	m_skyst_flowrateslider->setCursorListener([=, this]() {
		//		m_skyst_flowratesliderFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}

		//if (m_skyst_distortionmapradio) {
		//	m_skyst_distortionmapradio->setSelectListener([=, this]() {
		//		m_skyst_distortionmapradioFlag = true;
		//		if (m_dlgWnd) {
		//			m_dlgWnd->callRewrite();//再描画
		//		}
		//		});
		//}



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

int CSkyParamsDlg::ParamsToDlg(CModel* srcmodel, CMQOMaterial* srcmat)
{
	//############################################################
	//srcmat == nullptrの場合にはm_modelの最初のマテリアルシェーダを表示
	//############################################################
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	m_model = srcmodel;
	CreateSkyParamsWnd();

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

	m_skyparams[g_skyindex].SetMaterial(srcmat);

	//######
	//Text
	//######
	if (m_skyst_namelabel) {
		if (srcmat) {
			WCHAR wmatname[MAX_PATH] = { 0L };
			char matname[MAX_PATH] = { 0 };
			strcpy_s(matname, MAX_PATH, srcmat->GetName());
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, matname, MAX_PATH, wmatname, MAX_PATH);
			m_skyst_namelabel->setName(wmatname);
		}
		else {
			m_skyst_namelabel->setName(L"All");
		}
	}

	//#######
	//Button
	//#######
	if (m_skyst_shadertyperadio) {
		switch (m_skyparams[g_skyindex].shadertype) {
		case -1:
			m_skyst_shadertyperadio->setSelectIndex(0, false);
			break;
		case MQOSHADER_PBR://case 0
			m_skyst_shadertyperadio->setSelectIndex(1, false);
			break;
		case MQOSHADER_STD://case 1
			m_skyst_shadertyperadio->setSelectIndex(2, false);
			break;
		case MQOSHADER_TOON://case 2
			m_skyst_shadertyperadio->setSelectIndex(3, false);
			break;
		default:
			_ASSERT(0);
			m_skyst_shadertyperadio->setSelectIndex(0, false);
			break;
		}
	}

	if (m_skyst_toonlitradio) {
		if ((m_skyparams[g_skyindex].hsvtoon.lightindex >= 0) && (m_skyparams[g_skyindex].hsvtoon.lightindex <= 7)) {
			m_skyst_toonlitradio->setSelectIndex(m_skyparams[g_skyindex].hsvtoon.lightindex, false);
		}
		else {
			_ASSERT(0);
			m_skyst_toonlitradio->setSelectIndex(0, false);
		}
	}

	if (m_skyst_riverradio) {
		if ((m_skyparams[g_skyindex].riverorsea >= 0) && (m_skyparams[g_skyindex].riverorsea <= 1)) {
			m_skyst_riverradio->setSelectIndex(m_skyparams[g_skyindex].riverorsea, false);
		}
		else {
			_ASSERT(0);
			m_skyst_riverradio->setSelectIndex(0, false);
		}
	}

	if (m_skyst_distortionmapradio) {
		if ((m_skyparams[g_skyindex].distortionmaptype >= 0) && (m_skyparams[g_skyindex].distortionmaptype <= 2)) {
			m_skyst_distortionmapradio->setSelectIndex(m_skyparams[g_skyindex].distortionmaptype, false);
		}
		else {
			_ASSERT(0);
			m_skyst_distortionmapradio->setSelectIndex(0, false);
		}
	}


	//#######
	//Slider
	//#######
	if (m_skyst_metalslider) {
		m_skyst_metalslider->setValue(m_skyparams[g_skyindex].metalcoef, false);
	}
	if (m_skyst_smoothslider) {
		m_skyst_smoothslider->setValue(m_skyparams[g_skyindex].smoothcoef, false);
	}


	if (m_skyst_litscaleslider1) {
		m_skyst_litscaleslider1->setValue(m_skyparams[g_skyindex].lightscale[0], false);
	}
	if (m_skyst_litscaleslider2) {
		m_skyst_litscaleslider2->setValue(m_skyparams[g_skyindex].lightscale[1], false);
	}
	if (m_skyst_litscaleslider3) {
		m_skyst_litscaleslider3->setValue(m_skyparams[g_skyindex].lightscale[2], false);
	}
	if (m_skyst_litscaleslider4) {
		m_skyst_litscaleslider4->setValue(m_skyparams[g_skyindex].lightscale[3], false);
	}
	if (m_skyst_litscaleslider5) {
		m_skyst_litscaleslider5->setValue(m_skyparams[g_skyindex].lightscale[4], false);
	}
	if (m_skyst_litscaleslider6) {
		m_skyst_litscaleslider6->setValue(m_skyparams[g_skyindex].lightscale[5], false);
	}
	if (m_skyst_litscaleslider7) {
		m_skyst_litscaleslider7->setValue(m_skyparams[g_skyindex].lightscale[6], false);
	}
	if (m_skyst_litscaleslider8) {
		m_skyst_litscaleslider8->setValue(m_skyparams[g_skyindex].lightscale[7], false);
	}

	if (m_skyst_emissionslider) {
		m_skyst_emissionslider->setValue(m_skyparams[g_skyindex].emissiveScale, false);
	}

	if (m_skyst_toonhiaddrslider) {
		m_skyst_toonhiaddrslider->setValue(m_skyparams[g_skyindex].hsvtoon.hicolorh, false);
	}
	if (m_skyst_toonlowaddrslider) {
		m_skyst_toonlowaddrslider->setValue(m_skyparams[g_skyindex].hsvtoon.lowcolorh, false);
	}


	if (m_skyst_toonbaseHslider) {
		m_skyst_toonbaseHslider->setValue(m_skyparams[g_skyindex].hsvtoon.basehsv.x, false);
	}
	if (m_skyst_toonbaseSslider) {
		m_skyst_toonbaseSslider->setValue(m_skyparams[g_skyindex].hsvtoon.basehsv.y, false);
	}
	if (m_skyst_toonbaseVslider) {
		m_skyst_toonbaseVslider->setValue(m_skyparams[g_skyindex].hsvtoon.basehsv.z, false);
	}
	if (m_skyst_toonbaseAslider) {
		m_skyst_toonbaseAslider->setValue(m_skyparams[g_skyindex].hsvtoon.basehsv.w, false);
	}

	if (m_skyst_toonhiHslider) {
		m_skyst_toonhiHslider->setValue(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.x, false);
	}
	if (m_skyst_toonhiSslider) {
		m_skyst_toonhiSslider->setValue(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.y, false);
	}
	if (m_skyst_toonhiVslider) {
		m_skyst_toonhiVslider->setValue(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.z, false);
	}
	if (m_skyst_toonhiAslider) {
		m_skyst_toonhiAslider->setValue(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.w, false);
	}

	if (m_skyst_toonlowHslider) {
		m_skyst_toonlowHslider->setValue(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.x, false);
	}
	if (m_skyst_toonlowSslider) {
		m_skyst_toonlowSslider->setValue(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.y, false);
	}
	if (m_skyst_toonlowVslider) {
		m_skyst_toonlowVslider->setValue(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.z, false);
	}
	if (m_skyst_toonlowAslider) {
		m_skyst_toonlowAslider->setValue(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.w, false);
	}

	if (m_skyst_spccoefslider) {
		m_skyst_spccoefslider->setValue(m_skyparams[g_skyindex].specularcoef, false);
	}

	if (m_skyst_tilingUslider) {
		m_skyst_tilingUslider->setValue((int)(m_skyparams[g_skyindex].uvscale.x + 0.0001), false);
	}
	if (m_skyst_tilingVslider) {
		m_skyst_tilingVslider->setValue((int)(m_skyparams[g_skyindex].uvscale.y + 0.0001), false);
	}

	if (m_skyst_alphatestslider) {
		m_skyst_alphatestslider->setValue(m_skyparams[g_skyindex].alphatest, false);
	}

	if (m_skyst_distortionscaleslider) {
		m_skyst_distortionscaleslider->setValue(m_skyparams[g_skyindex].distortionscale, false);
	}
	if (m_skyst_seacenterUslider) {
		m_skyst_seacenterUslider->setValue(m_skyparams[g_skyindex].seacenter.x, false);
	}
	if (m_skyst_seacenterVslider) {
		m_skyst_seacenterVslider->setValue(m_skyparams[g_skyindex].seacenter.y, false);
	}
	if (m_skyst_riverdirUslider) {
		m_skyst_riverdirUslider->setValue(m_skyparams[g_skyindex].riverdir.x, false);
	}
	if (m_skyst_riverdirVslider) {
		m_skyst_riverdirVslider->setValue(m_skyparams[g_skyindex].riverdir.y, false);
	}
	if (m_skyst_flowrateslider) {
		m_skyst_flowrateslider->setValue(m_skyparams[g_skyindex].riverflowrate, false);
	}


	//#########
	//CheckBox
	//#########
	if (m_skyst_emissionchk) {
		if ((bool)m_skyparams[g_skyindex].enableEmission == true) {
			m_skyst_emissionchk->setValue(true, false);
		}
		else {
			m_skyst_emissionchk->setValue(false, false);
		}
	}

	if (m_skyst_gradationchk) {
		if (m_skyparams[g_skyindex].hsvtoon.gradationflag == true) {
			m_skyst_gradationchk->setValue(true, false);

			if (m_skyst_powertoonchk) {
				m_skyst_powertoonchk->setActive(true);
			}
		}
		else {
			m_skyst_gradationchk->setValue(false, false);

			if (m_skyst_powertoonchk) {
				m_skyst_powertoonchk->setActive(false);
			}
		}
	}

	if (m_skyst_powertoonchk) {
		if (m_skyparams[g_skyindex].hsvtoon.powertoon == true) {
			m_skyst_powertoonchk->setValue(true, false);
		}
		else {
			m_skyst_powertoonchk->setValue(false, false);
		}
	}

	if (m_skyst_normaly0chk) {
		if (m_skyparams[g_skyindex].normaly0flag == true) {
			m_skyst_normaly0chk->setValue(true, false);
		}
		else {
			m_skyst_normaly0chk->setValue(false, false);
		}
	}

	if (m_skyst_shadowcasterchk) {
		if (m_skyparams[g_skyindex].shadowcasterflag == true) {
			m_skyst_shadowcasterchk->setValue(true, false);
		}
		else {
			m_skyst_shadowcasterchk->setValue(false, false);
		}
	}

	if (m_skyst_lightflagchk) {
		if (m_skyparams[g_skyindex].lightingmat == true) {
			m_skyst_lightflagchk->setValue(true, false);
		}
		else {
			m_skyst_lightflagchk->setValue(false, false);
		}
	}

	if (m_skyst_distortionchk) {
		if (m_skyparams[g_skyindex].distortionflag == true) {
			m_skyst_distortionchk->setValue(true, false);
		}
		else {
			m_skyst_distortionchk->setValue(false, false);
		}
	}

	return 0;
}

int CSkyParamsDlg::OnFrameSkyParamsDlg()
{
	if (GetSkyStRadioFlag()) {
		SetSkyStRadioFlag(false);
		if (m_model) {

			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int shadertype = GetSkyTypeRadio();
			switch (shadertype) {
			case 0:
				if (curmqomat) {
					curmqomat->SetShaderType(-1);
					m_skyparams[g_skyindex].shadertype = -1;
				}
				else {
					int materialindex5;
					for (materialindex5 = 0; materialindex5 < materialnum; materialindex5++) {
						CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex5);
						if (setmqomat) {
							setmqomat->SetShaderType(-1);
						}
					}
				}
				break;
			case 1:
				m_skyparams[g_skyindex].shadertype = MQOSHADER_PBR;

				if (curmqomat) {
					curmqomat->SetShaderType(MQOSHADER_PBR);
				}
				else {
					int materialindex6;
					for (materialindex6 = 0; materialindex6 < materialnum; materialindex6++) {
						CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex6);
						if (setmqomat) {
							setmqomat->SetShaderType(MQOSHADER_PBR);
						}
					}
				}
				break;
			case 2:
				m_skyparams[g_skyindex].shadertype = MQOSHADER_STD;

				if (curmqomat) {
					curmqomat->SetShaderType(MQOSHADER_STD);
				}
				else {
					int materialindex7;
					for (materialindex7 = 0; materialindex7 < materialnum; materialindex7++) {
						CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex7);
						if (setmqomat) {
							setmqomat->SetShaderType(MQOSHADER_STD);
						}
					}
				}
				break;
			case 3:
				m_skyparams[g_skyindex].shadertype = MQOSHADER_TOON;

				if (curmqomat) {
					curmqomat->SetShaderType(MQOSHADER_TOON);
				}
				else {
					int materialindex8;
					for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
						CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
						if (setmqomat) {
							setmqomat->SetShaderType(MQOSHADER_TOON);
						}
					}
				}
				break;
			default:
				_ASSERT(0);
				break;
			}

		}
	}
	if (GetSkyLightChkFlag()) {
		SetSkyLightChkFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetSkyLightFlagChk();
			if (ischecked) {
				m_skyparams[g_skyindex].lightingmat = true;
			}
			else {
				m_skyparams[g_skyindex].lightingmat = false;
			}

			if (curmqomat) {
				curmqomat->SetLightingFlag(m_skyparams[g_skyindex].lightingmat);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetLightingFlag(m_skyparams[g_skyindex].lightingmat);
					}
				}
			}
		}
	}
	if (GetSkyShadowCasterChkFlag()) {
		SetSkyShadowCasterChkFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetSkyShadowCasterChk();
			if (ischecked) {
				m_skyparams[g_skyindex].shadowcasterflag = true;
			}
			else {
				m_skyparams[g_skyindex].shadowcasterflag = false;
			}

			if (curmqomat) {
				curmqomat->SetShadowCasterFlag(m_skyparams[g_skyindex].shadowcasterflag);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetShadowCasterFlag(m_skyparams[g_skyindex].shadowcasterflag);
					}
				}
			}
		}
	}
	if (GetSkyNormalY0ChkFlag()) {
		SetSkyNormalY0ChkFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetShadowNormalY0Chk();
			if (ischecked) {
				m_skyparams[g_skyindex].normaly0flag = true;
			}
			else {
				m_skyparams[g_skyindex].normaly0flag = false;
			}

			if (curmqomat) {
				curmqomat->SetNormalY0Flag(m_skyparams[g_skyindex].normaly0flag);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetNormalY0Flag(m_skyparams[g_skyindex].normaly0flag);
					}
				}
			}
		}
	}
	if (GetSkySpcCoefSliderFlag()) {
		SetSkySpcCoefSliderFlag(false);
		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newspcscale = GetSkySpcCoefSlider();
			m_skyparams[g_skyindex].specularcoef = newspcscale;

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
	if (GetSkyEmissionChkFlag()) {
		SetSkyEmissionChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetSkyEmissionChk();
			if (ischecked) {
				m_skyparams[g_skyindex].enableEmission = true;
			}
			else {
				m_skyparams[g_skyindex].enableEmission = false;
			}

			if (curmqomat) {
				curmqomat->SetEnableEmission(m_skyparams[g_skyindex].enableEmission);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetEnableEmission(m_skyparams[g_skyindex].enableEmission);
					}
				}
			}
		}
	}
	if (GetSkyEmissionSliderFlag()) {
		SetSkyEmissionSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newemiscale = GetSkyEmissionSlider();
			m_skyparams[g_skyindex].emissiveScale = newemiscale;

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
	if (GetSkyMetalSliderFlag()) {
		SetSkyMetalSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newmetalcoef = GetSkyMetalSlider();
			m_skyparams[g_skyindex].metalcoef = newmetalcoef;

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
	if (GetSkySmoothSliderFlag()) {
		SetSkySmoothSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newsmoothcoef = GetSkySmoothSlider();
			m_skyparams[g_skyindex].smoothcoef = newsmoothcoef;

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
	if (GetSkyLitScale1Flag()) {
		SetSkyLitScale1Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 0;//!!!
			float newlitscale = GetSkyLitScaleSlider1();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale2Flag()) {
		SetSkyLitScale2Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 1;//!!!
			float newlitscale = GetSkyLitScaleSlider2();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale3Flag()) {
		SetSkyLitScale3Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 2;//!!!
			float newlitscale = GetSkyLitScaleSlider3();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale4Flag()) {
		SetSkyLitScale4Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 3;//!!!
			float newlitscale = GetSkyLitScaleSlider4();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale5Flag()) {
		SetSkyLitScale5Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 4;//!!!
			float newlitscale = GetSkyLitScaleSlider5();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale6Flag()) {
		SetSkyLitScale6Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 5;//!!!
			float newlitscale = GetSkyLitScaleSlider6();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale7Flag()) {
		SetSkyLitScale7Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 6;//!!!
			float newlitscale = GetSkyLitScaleSlider7();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyLitScale8Flag()) {
		SetSkyLitScale8Flag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int litno4 = 7;//!!!
			float newlitscale = GetSkyLitScaleSlider8();
			m_skyparams[g_skyindex].lightscale[litno4] = newlitscale;

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
	if (GetSkyToonLitRadioFlag()) {
		SetSkyToonLitRadioFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int lightindex = GetSkyToonLitRadio();
			m_skyparams[g_skyindex].hsvtoon.lightindex = lightindex;

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
	if (GetSkyToonHiAddrSliderFlag()) {
		SetSkyToonHiAddrSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float hicolorh = GetSkyToonHiAddrSlider();
			m_skyparams[g_skyindex].hsvtoon.hicolorh = hicolorh;

			if (curmqomat) {
				curmqomat->SetToonHiAddrH(m_skyparams[g_skyindex].hsvtoon.hicolorh);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddrH(m_skyparams[g_skyindex].hsvtoon.hicolorh);
						m_skyhsvtoonforall.hicolorh = m_skyparams[g_skyindex].hsvtoon.hicolorh;
					}
				}
			}
		}
	}
	if (GetSkyRemakeToonTextureFlag()) {
		SetSkyRemakeToonTextureFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			if (curmqomat) {
				m_skytoonmqomaterial = curmqomat;
			}
			else {
				m_skytoonmqomaterial = nullptr;
			}
			m_skytoonparamchange = true;
		}
	}
	if (GetSkyToonLowAddrSliderFlag()) {
		SetSkyToonLowAddrSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float lowcolorh = GetSkyToonLowAddrSlider();
			m_skyparams[g_skyindex].hsvtoon.lowcolorh = lowcolorh;

			if (curmqomat) {
				curmqomat->SetToonLowAddrH(m_skyparams[g_skyindex].hsvtoon.lowcolorh);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddrH(m_skyparams[g_skyindex].hsvtoon.lowcolorh);
						m_skyhsvtoonforall.lowcolorh = m_skyparams[g_skyindex].hsvtoon.lowcolorh;
					}
				}
			}
		}
	}
	if (GetSkyGradationChkFlag()) {
		SetSkyGradationChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetSkyGradationChk();
			if (ischecked) {
				m_skyparams[g_skyindex].hsvtoon.gradationflag = true;
			}
			else {
				m_skyparams[g_skyindex].hsvtoon.gradationflag = false;
			}

			if (curmqomat) {
				curmqomat->SetToonGradationFlag(m_skyparams[g_skyindex].hsvtoon.gradationflag);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonGradationFlag(m_skyparams[g_skyindex].hsvtoon.gradationflag);
						m_skyhsvtoonforall.gradationflag = m_skyparams[g_skyindex].hsvtoon.gradationflag;
					}
				}
			}

			if (curmqomat) {
				m_skytoonmqomaterial = curmqomat;
			}
			else {
				m_skytoonmqomaterial = nullptr;
			}
			m_skytoonparamchange = true;
		}
	}
	if (GetSkyPowerToonChkFlag()) {
		SetSkyPowerToonChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetSkyPowerToonChk();
			if (ischecked) {
				m_skyparams[g_skyindex].hsvtoon.powertoon = true;
			}
			else {
				m_skyparams[g_skyindex].hsvtoon.powertoon = false;
			}

			if (curmqomat) {
				curmqomat->SetToonPowerToon(m_skyparams[g_skyindex].hsvtoon.powertoon);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonPowerToon(m_skyparams[g_skyindex].hsvtoon.powertoon);
						m_skyhsvtoonforall.powertoon = m_skyparams[g_skyindex].hsvtoon.powertoon;
					}
				}
			}

			if (curmqomat) {
				m_skytoonmqomaterial = curmqomat;
			}
			else {
				m_skytoonmqomaterial = nullptr;
			}
			m_skytoonparamchange = true;
		}
	}
	if (GetSkyToonBaseHSliderFlag()) {
		SetSkyToonBaseHSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_skyparams[g_skyindex].hsvtoon.basehsv.x = GetSkyToonBaseHSlider();

			if (curmqomat) {
				curmqomat->SetToonBaseH(m_skyparams[g_skyindex].hsvtoon.basehsv.x);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseH(m_skyparams[g_skyindex].hsvtoon.basehsv.x);
						m_skyhsvtoonforall.basehsv.x = m_skyparams[g_skyindex].hsvtoon.basehsv.x;
					}
				}
			}
		}
	}
	if (GetSkyToonBaseSSliderFlag()) {
		SetSkyToonBaseSSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_skyparams[g_skyindex].hsvtoon.basehsv.y = GetSkyToonBaseSSlider();

			if (curmqomat) {
				curmqomat->SetToonBaseS(m_skyparams[g_skyindex].hsvtoon.basehsv.y);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseS(m_skyparams[g_skyindex].hsvtoon.basehsv.y);
						m_skyhsvtoonforall.basehsv.y = m_skyparams[g_skyindex].hsvtoon.basehsv.y;
					}
				}
			}
		}
	}
	if (GetSkyToonBaseVSliderFlag()) {
		SetSkyToonBaseVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_skyparams[g_skyindex].hsvtoon.basehsv.z = GetSkyToonBaseVSlider();

			if (curmqomat) {
				curmqomat->SetToonBaseV(m_skyparams[g_skyindex].hsvtoon.basehsv.z);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseV(m_skyparams[g_skyindex].hsvtoon.basehsv.z);
						m_skyhsvtoonforall.basehsv.z = m_skyparams[g_skyindex].hsvtoon.basehsv.z;
					}
				}
			}
		}
	}
	if (GetSkyToonBaseASliderFlag()) {
		SetSkyToonBaseASliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_skyparams[g_skyindex].hsvtoon.basehsv.w = GetSkyToonBaseASlider();

			if (curmqomat) {
				curmqomat->SetToonBaseA(m_skyparams[g_skyindex].hsvtoon.basehsv.w);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonBaseA(m_skyparams[g_skyindex].hsvtoon.basehsv.w);
						m_skyhsvtoonforall.basehsv.w = m_skyparams[g_skyindex].hsvtoon.basehsv.w;
					}
				}
			}
		}
	}
	if (GetSkyToonHiHSliderFlag()) {
		SetSkyToonHiHSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetSkyToonHiHSlider();
			m_skyparams[g_skyindex].hsvtoon.hiaddhsv.x = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddH(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.x);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddH(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.x);
						m_skyhsvtoonforall.hiaddhsv.x = m_skyparams[g_skyindex].hsvtoon.hiaddhsv.x;
					}
				}
			}
		}
	}
	if (GetSkyToonHiSSliderFlag()) {
		SetSkyToonHiSSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetSkyToonHiSSlider();
			m_skyparams[g_skyindex].hsvtoon.hiaddhsv.y = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddS(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.y);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddS(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.y);
						m_skyhsvtoonforall.hiaddhsv.y = m_skyparams[g_skyindex].hsvtoon.hiaddhsv.y;
					}
				}
			}
		}
	}
	if (GetSkyToonHiVSliderFlag()) {
		SetSkyToonHiVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetSkyToonHiVSlider();
			m_skyparams[g_skyindex].hsvtoon.hiaddhsv.z = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddV(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.z);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddV(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.z);
						m_skyhsvtoonforall.hiaddhsv.z = m_skyparams[g_skyindex].hsvtoon.hiaddhsv.z;
					}
				}
			}
		}
	}
	if (GetSkyToonHiASliderFlag()) {
		SetSkyToonHiASliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			float newvalue = GetSkyToonHiASlider();
			m_skyparams[g_skyindex].hsvtoon.hiaddhsv.w = newvalue;

			if (curmqomat) {
				curmqomat->SetToonHiAddA(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.w);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonHiAddA(m_skyparams[g_skyindex].hsvtoon.hiaddhsv.w);
						m_skyhsvtoonforall.hiaddhsv.w = m_skyparams[g_skyindex].hsvtoon.hiaddhsv.w;
					}
				}
			}
		}
	}
	if (GetSkyToonLowHSliderFlag()) {
		SetSkyToonLowHSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetSkyToonLowHSlider();
			m_skyparams[g_skyindex].hsvtoon.lowaddhsv.x = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddH(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.x);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddH(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.x);
						m_skyhsvtoonforall.lowaddhsv.x = m_skyparams[g_skyindex].hsvtoon.lowaddhsv.x;
					}
				}
			}
		}
	}
	if (GetSkyToonLowSSliderFlag()) {
		SetSkyToonLowSSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetSkyToonLowSSlider();
			m_skyparams[g_skyindex].hsvtoon.lowaddhsv.y = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddS(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.y);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddS(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.y);
						m_skyhsvtoonforall.lowaddhsv.y = m_skyparams[g_skyindex].hsvtoon.lowaddhsv.y;
					}
				}
			}
		}
	}
	if (GetSkyToonLowVSliderFlag()) {
		SetSkyToonLowVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetSkyToonLowVSlider();
			m_skyparams[g_skyindex].hsvtoon.lowaddhsv.z = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddV(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.z);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddV(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.z);
						m_skyhsvtoonforall.lowaddhsv.z = m_skyparams[g_skyindex].hsvtoon.lowaddhsv.z;
					}
				}
			}
		}
	}
	if (GetSkyToonLowASliderFlag()) {
		SetSkyToonLowASliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);


			float newvalue = GetSkyToonLowASlider();
			m_skyparams[g_skyindex].hsvtoon.lowaddhsv.w = newvalue;

			if (curmqomat) {
				curmqomat->SetToonLowAddA(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.w);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetToonLowAddA(m_skyparams[g_skyindex].hsvtoon.lowaddhsv.w);
						m_skyhsvtoonforall.lowaddhsv.w = m_skyparams[g_skyindex].hsvtoon.lowaddhsv.w;
					}
				}
			}
		}
	}
	if (GetSkyTilingUSliderFlag()) {
		SetSkyTilingUSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int newvalue = GetSkyTilingUSlider();
			m_skyparams[g_skyindex].uvscale.x = (double)newvalue;

			if (curmqomat) {
				curmqomat->SetUVScale(m_skyparams[g_skyindex].uvscale);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetUVScale(m_skyparams[g_skyindex].uvscale);
					}
				}
			}
		}
	}
	if (GetSkyTilingUSliderUpFlag()) {
		SetSkyTilingUSliderUpFlag(false);

		if (m_model) {
			int newvalue = GetSkyTilingUSlider();
			m_skyparams[g_skyindex].uvscale.x = (double)newvalue;
			SetSkyTilingUSlider(newvalue);
			//m_st_tilingUslider->setValue(m_skyparams[g_skyindex].uvscale.x, false);//!!! マウスを離したときにintに丸めた値をセットし直す
		}
	}
	if (GetSkyTilingVSliderFlag()) {
		SetSkyTilingVSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			int newvalue = GetSkyTilingVSlider();
			m_skyparams[g_skyindex].uvscale.y = (double)newvalue;

			if (curmqomat) {
				curmqomat->SetUVScale(m_skyparams[g_skyindex].uvscale);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetUVScale(m_skyparams[g_skyindex].uvscale);
					}
				}
			}
		}
	}
	if (GetSkyTilingVSliderUpFlag()) {
		SetSkyTilingVSliderUpFlag(false);

		if (m_model) {
			int newvalue = GetSkyTilingVSlider();
			m_skyparams[g_skyindex].uvscale.y = (double)newvalue;

			//m_st_tilingVslider->setValue(m_skyparams[g_skyindex].uvscale.y, false);//!!! マウスを離したときにintに丸めた値をセットし直す
			SetSkyTilingVSlider(newvalue);
		}
	}
	if (GetSkyAlphaTestSliderFlag()) {
		SetSkyAlphaTestSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			m_skyparams[g_skyindex].alphatest = GetSkyAlphaTestSlider();

			if (curmqomat) {
				curmqomat->SetAlphaTestClipVal(m_skyparams[g_skyindex].alphatest);
			}
			else {
				int materialindex2;
				for (materialindex2 = 0; materialindex2 < materialnum; materialindex2++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex2);
					if (setmqomat) {
						setmqomat->SetAlphaTestClipVal(m_skyparams[g_skyindex].alphatest);
					}
				}
			}
		}
	}
	if (GetSkyDistortionChkFlag()) {
		SetSkyDistortionChkFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			bool ischecked = GetSkyDistortionChk();
			if (ischecked) {
				m_skyparams[g_skyindex].distortionflag = true;
			}
			else {
				m_skyparams[g_skyindex].distortionflag = false;
			}

			if (curmqomat) {
				curmqomat->SetDistortionFlag(m_skyparams[g_skyindex].distortionflag);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetDistortionFlag(m_skyparams[g_skyindex].distortionflag);
					}
				}
			}
		}
	}
	if (GetSkyDistortionScaleSliderFlag()) {
		SetSkyDistortionScaleSliderFlag(false);

		if (m_model) {
			int materialnum = m_model->GetMQOMaterialSize();
			materialnum = min(materialnum, MAXMATERIALNUM);
			int materialindex = 0;//m_skyの最初のマテリアル
			CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

			double newvalue = GetSkyDistortonScaleSlider();
			m_skyparams[g_skyindex].distortionscale = newvalue;

			if (curmqomat) {
				curmqomat->SetDistortionScale(m_skyparams[g_skyindex].distortionscale);
			}
			else {
				int materialindex9;
				for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
					CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
					if (setmqomat) {
						setmqomat->SetDistortionScale(m_skyparams[g_skyindex].distortionscale);
					}
				}
			}
		}
	}



	if (m_skytoonparamchange) {
		m_skytoonparamchange = false;
		if (m_model) {
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




	//if (GetSkyRiverRadioFlag()) {
	//	SetSkyRiverRadioFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		int riverorsea = GetSkyRiverRadio();
	//		m_skyparams[g_skyindex].riverorsea = riverorsea;

	//		if (curmqomat) {
	//			curmqomat->SetRiverOrSea(riverorsea);
	//		}
	//		else {
	//			int materialindex8;
	//			for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
	//				if (setmqomat) {
	//					setmqomat->SetRiverOrSea(riverorsea);
	//				}
	//			}
	//		}
	//	}
	//}
	//if (GetSkySeaCenterUSliderFlag()) {
	//	SetSkySeaCenterUSliderFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		float newvalueU = GetSkySeaCenterUSlider();
	//		float newvalueV = GetSkySeaCenterVSlider();
	//		m_skyparams[g_skyindex].seacenter.SetParams(newvalueU, newvalueV);

	//		if (curmqomat) {
	//			curmqomat->SetSeaCenter(m_skyparams[g_skyindex].seacenter);
	//		}
	//		else {
	//			int materialindex9;
	//			for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
	//				if (setmqomat) {
	//					setmqomat->SetSeaCenter(m_skyparams[g_skyindex].seacenter);
	//				}
	//			}
	//		}
	//	}
	//}
	//if (GetSkySeaCenterVSliderFlag()) {
	//	SetSkySeaCenterVSliderFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		float newvalueU = GetSkySeaCenterUSlider();
	//		float newvalueV = GetSkySeaCenterVSlider();
	//		m_skyparams[g_skyindex].seacenter.SetParams(newvalueU, newvalueV);

	//		if (curmqomat) {
	//			curmqomat->SetSeaCenter(m_skyparams[g_skyindex].seacenter);
	//		}
	//		else {
	//			int materialindex9;
	//			for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
	//				if (setmqomat) {
	//					setmqomat->SetSeaCenter(m_skyparams[g_skyindex].seacenter);
	//				}
	//			}
	//		}
	//	}
	//}
	//if (GetSkyRiverDirUSliderFlag()) {
	//	SetSkyRiverDirUSliderFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		float newvalueU = GetSkyRiverDirUSlider();
	//		float newvalueV = GetSkyRiverDirVSlider();
	//		m_skyparams[g_skyindex].riverdir.SetParams(newvalueU, newvalueV);

	//		if (curmqomat) {
	//			curmqomat->SetRiverDir(m_skyparams[g_skyindex].riverdir);
	//		}
	//		else {
	//			int materialindex9;
	//			for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
	//				if (setmqomat) {
	//					setmqomat->SetRiverDir(m_skyparams[g_skyindex].riverdir);
	//				}
	//			}
	//		}
	//	}
	//}
	//if (GetSkyRiverDirVSliderFlag()) {
	//	SetSkyRiverDirVSliderFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		float newvalueU = GetSkyRiverDirUSlider();
	//		float newvalueV = GetSkyRiverDirVSlider();
	//		m_skyparams[g_skyindex].riverdir.SetParams(newvalueU, newvalueV);

	//		if (curmqomat) {
	//			curmqomat->SetRiverDir(m_skyparams[g_skyindex].riverdir);
	//		}
	//		else {
	//			int materialindex9;
	//			for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
	//				if (setmqomat) {
	//					setmqomat->SetRiverDir(m_skyparams[g_skyindex].riverdir);
	//				}
	//			}
	//		}
	//	}
	//}
	//if (GetSkyFlowRateSliderFlag()) {
	//	SetSkyFlowRateSliderFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		double newvalue = GetSkyFlowRateSlider();
	//		m_skyparams[g_skyindex].riverflowrate = newvalue;

	//		if (curmqomat) {
	//			curmqomat->SetRiverFlowRate(m_skyparams[g_skyindex].riverflowrate);
	//		}
	//		else {
	//			int materialindex9;
	//			for (materialindex9 = 0; materialindex9 < materialnum; materialindex9++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex9);
	//				if (setmqomat) {
	//					setmqomat->SetRiverFlowRate(m_skyparams[g_skyindex].riverflowrate);
	//				}
	//			}
	//		}
	//	}

	//}
	//if (GetSkyDistortionMapRadioFlag()) {
	//	SetSkyDistortionMapRadioFlag(false);

	//	if (m_model) {
	//		int materialnum = m_model->GetMQOMaterialSize();
	//		materialnum = min(materialnum, MAXMATERIALNUM);
	//		int materialindex = 0;//m_skyの最初のマテリアル
	//		CMQOMaterial* curmqomat = m_model->GetMQOMaterialByIndex(materialindex);

	//		int maptype = GetSkyDistortionMapRadio();
	//		m_skyparams[g_skyindex].distortionmaptype = maptype;

	//		if (curmqomat) {
	//			curmqomat->SetDistortionMapType(maptype);
	//		}
	//		else {
	//			int materialindex8;
	//			for (materialindex8 = 0; materialindex8 < materialnum; materialindex8++) {
	//				CMQOMaterial* setmqomat = m_model->GetMQOMaterialByIndex(materialindex8);
	//				if (setmqomat) {
	//					setmqomat->SetDistortionMapType(maptype);
	//				}
	//			}
	//		}
	//	}
	//}

	//if (m_skyst_distortionscalesliderFlag) {
	//	m_skyst_distortionscalesliderFlag = false;
	//}
	//if (m_skyst_riverradioFlag) {
	//	m_skyst_riverradioFlag = false;
	//}
	//if (m_skyst_seacenterUsliderFlag) {
	//	m_skyst_seacenterUsliderFlag = false;
	//}
	//if (m_skyst_seacenterVsliderFlag) {
	//	m_skyst_seacenterVsliderFlag = false;
	//}
	//if (m_skyst_riverdirUsliderFlag) {
	//	m_skyst_riverdirUsliderFlag = false;
	//}
	//if (m_skyst_riverdirVsliderFlag) {
	//	m_skyst_riverdirVsliderFlag = false;
	//}
	//if (m_skyst_flowratesliderFlag) {
	//	m_skyst_flowratesliderFlag = false;
	//}
	//if (m_skyst_distortionmapradioFlag) {
	//	m_skyst_distortionmapradioFlag = false;
	//}


	return 0;
}


const HWND CSkyParamsDlg::GetHWnd()
{
	if (!m_dlgWnd) {
		return NULL;
	}
	else {
		return m_dlgWnd->getHWnd();
	}
}

const bool CSkyParamsDlg::GetVisible()
{
	if (!m_dlgWnd) {
		return false;
	}
	else {
		return m_dlgWnd->getVisible();
	}
}
void CSkyParamsDlg::ListenMouse(bool srcflag)
{
	if (!m_dlgWnd) {
		return;
	}
	else {
		m_dlgWnd->setListenMouse(srcflag);
	}
}

