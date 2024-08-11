#ifndef __CShaderParamsDlg_H_
#define __CShaderParamsDlg_H_


#include <windows.h>

#include <ChaVecCalc.h>
#include <mqomaterial.h>

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

class CShaderTypeDlg;
//class CShaderTypeParams;
//struct tag_hsvtoon;

/////////////////////////////////////////////////////////////////////////////
// CShaderParamsDlg
class CShaderParamsDlg
{
public:
	CShaderParamsDlg();
	~CShaderParamsDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	void SetModel(CModel* srcmodel);
	int ParamsToDlg(CMQOMaterial* srcmat);
	void SetVisible(bool srcflag);
	int OnFrameShaperTypeParamsDlg(CShaderTypeDlg* pshadertypedlg);

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateShaderParamsWnd();

public:
	bool GetShaderCloseFlag() {
		return m_st_closeFlag;
	};
	void SetShaderCloseFlag(bool srcval) {
		m_st_closeFlag = srcval;
	};
	bool GetShaderRemakeToonTextureFlag() {
		return m_st_remakeToonTextureFlag;
	};
	void SetShaderRemakeToonTextureFlag(bool srcval)
	{
		m_st_remakeToonTextureFlag = srcval;
	}
	bool GetShaderBackFlag() {
		return m_st_backFlag;
	};
	void SetShaderBackFlag(bool srcval) {
		m_st_backFlag = srcval;
	};
	bool GetShaderStRadioFlag() {
		return m_st_stradioFlag;
	};
	void SetShaderStRadioFlag(bool srcval) {
		m_st_stradioFlag = srcval;
	};
	bool GetShaderLightChkFlag() {
		return m_st_lightchkFlag;
	};
	void SetShaderLightChkFlag(bool srcval) {
		m_st_lightchkFlag = srcval;
	}
	bool GetShaderShadowCasterChkFlag() {
		return m_st_shadowcasterchkFlag;
	};
	void SetShaderShadowCasterChkFlag(bool srcval) {
		m_st_shadowcasterchkFlag = srcval;
	};
	bool GetShaderNormalY0ChkFlag() {
		return m_st_normaly0chkFlag;
	};
	void SetShaderNormalY0ChkFlag(bool srcval) {
		m_st_normaly0chkFlag = srcval;
	};
	bool GetShaderSpcCoefSliderFlag() {
		return m_st_spccoefsliderFlag;
	};
	void SetShaderSpcCoefSliderFlag(bool srcval) {
		m_st_spccoefsliderFlag = srcval;
	};
	bool GetShaderEmissionChkFlag() {
		return m_st_emissionchkFlag;
	};
	void SetShaderEmissionChkFlag(bool srcval) {
		m_st_emissionchkFlag = srcval;
	};
	bool GetShaderEmissionSliderFlag() {
		return m_st_emissionsliderFlag;
	};
	void SetShaderEmissionSliderFlag(bool srcval) {
		m_st_emissionsliderFlag = srcval;
	};
	bool GetShaderMetalSliderFlag() {
		return m_st_metalsliderFlag;
	};
	void SetShaderMetalSliderFlag(bool srcval) {
		m_st_metalsliderFlag = srcval;
	};
	bool GetShaderSmoothSliderFlag() {
		return m_st_smoothsliderFlag;
	};
	void SetShaderSmoothSliderFlag(bool srcval) {
		m_st_smoothsliderFlag = srcval;
	};
	bool GetShaderLitScale1Flag() {
		return m_st_litscale1Flag;
	};
	void SetShaderLitScale1Flag(bool srcval) {
		m_st_litscale1Flag = srcval;
	};
	bool GetShaderLitScale2Flag() {
		return m_st_litscale2Flag;
	};
	void SetShaderLitScale2Flag(bool srcval) {
		m_st_litscale2Flag = srcval;
	};
	bool GetShaderLitScale3Flag() {
		return m_st_litscale3Flag;
	};
	void SetShaderLitScale3Flag(bool srcval) {
		m_st_litscale3Flag = srcval;
	};
	bool GetShaderLitScale4Flag() {
		return m_st_litscale4Flag;
	};
	void SetShaderLitScale4Flag(bool srcval) {
		m_st_litscale4Flag = srcval;
	};
	bool GetShaderLitScale5Flag() {
		return m_st_litscale5Flag;
	};
	void SetShaderLitScale5Flag(bool srcval) {
		m_st_litscale5Flag = srcval;
	};
	bool GetShaderLitScale6Flag() {
		return m_st_litscale6Flag;
	};
	void SetShaderLitScale6Flag(bool srcval) {
		m_st_litscale6Flag = srcval;
	};
	bool GetShaderLitScale7Flag() {
		return m_st_litscale7Flag;
	};
	void SetShaderLitScale7Flag(bool srcval) {
		m_st_litscale7Flag = srcval;
	};
	bool GetShaderLitScale8Flag() {
		return m_st_litscale8Flag;
	};
	void SetShaderLitScale8Flag(bool srcval) {
		m_st_litscale8Flag = srcval;
	};
	bool GetShaderToonLitRadioFlag() {
		return m_st_toonlitradioFlag;
	};
	void SetShaderToonLitRadioFlag(bool srcval) {
		m_st_toonlitradioFlag = srcval;
	};
	bool GetShaderToonHiAddrSliderFlag() {
		return m_st_toonhiaddrsliderFlag;
	};
	void SetShaderToonHiAddrSliderFlag(bool srcval) {
		m_st_toonhiaddrsliderFlag = srcval;
	};
	bool GetShaderToonLowAddrSliderFlag() {
		return m_st_toonlowaddrsliderFlag;
	};
	void SetShaderToonLowAddrSliderFlag(bool srcval) {
		m_st_toonlowaddrsliderFlag = srcval;
	};
	bool GetShaderGradationChkFlag() {
		return m_st_gradationchkFlag;
	};
	void SetShaderGradationChkFlag(bool srcval) {
		m_st_gradationchkFlag = srcval;
	};
	bool GetShaderPowerToonChkFlag() {
		return m_st_powertoonchkFlag;
	};
	void SetShaderPowerToonChkFlag(bool srcval) {
		m_st_powertoonchkFlag = srcval;
	};
	bool GetShaderToonBaseHSliderFlag() {
		return m_st_toonbaseHsliderFlag;
	};
	void SetShaderToonBaseHSliderFlag(bool srcval) {
		m_st_toonbaseHsliderFlag = srcval;
	};
	bool GetShaderToonBaseSSliderFlag() {
		return m_st_toonbaseSsliderFlag;
	};
	void SetShaderToonBaseSSliderFlag(bool srcval) {
		m_st_toonbaseSsliderFlag = srcval;
	};
	bool GetShaderToonBaseVSliderFlag() {
		return m_st_toonbaseVsliderFlag;
	};
	void SetShaderToonBaseVSliderFlag(bool srcval) {
		m_st_toonbaseVsliderFlag = srcval;
	};
	bool GetShaderToonBaseASliderFlag() {
		return m_st_toonbaseAsliderFlag;
	};
	void SetShaderToonBaseASliderFlag(bool srcval) {
		m_st_toonbaseAsliderFlag = srcval;
	};
	bool GetShaderToonHiHSliderFlag() {
		return m_st_toonhiHsliderFlag;
	};
	void SetShaderToonHiHSliderFlag(bool srcval) {
		m_st_toonhiHsliderFlag = srcval;
	};
	bool GetShaderToonHiSSliderFlag() {
		return m_st_toonhiSsliderFlag;
	};
	void SetShaderToonHiSSliderFlag(bool srcval) {
		m_st_toonhiSsliderFlag = srcval;
	};
	bool GetShaderToonHiVSliderFlag() {
		return m_st_toonhiVsliderFlag;
	};
	void SetShaderToonHiVSliderFlag(bool srcval) {
		m_st_toonhiVsliderFlag = srcval;
	};
	bool GetShaderToonHiASliderFlag() {
		return m_st_toonhiAsliderFlag;
	};
	void SetShaderToonHiASliderFlag(bool srcval) {
		m_st_toonhiAsliderFlag = srcval;
	};
	bool GetShaderToonLowHSliderFlag() {
		return m_st_toonlowHsliderFlag;
	};
	void SetShaderToonLowHSliderFlag(bool srcval) {
		m_st_toonlowHsliderFlag = srcval;
	};
	bool GetShaderToonLowSSliderFlag() {
		return m_st_toonlowSsliderFlag;
	};
	void SetShaderToonLowSSliderFlag(bool srcval) {
		m_st_toonlowSsliderFlag = srcval;
	};
	bool GetShaderToonLowVSliderFlag() {
		return m_st_toonlowVsliderFlag;
	};
	void SetShaderToonLowVSliderFlag(bool srcval) {
		m_st_toonlowVsliderFlag = srcval;
	};
	bool GetShaderToonLowASliderFlag() {
		return m_st_toonlowAsliderFlag;
	};
	void SetShaderToonLowASliderFlag(bool srcval) {
		m_st_toonlowAsliderFlag = srcval;
	};
	bool GetShaderTilingUSliderFlag() {
		return m_st_tilingUsliderFlag;
	};
	void SetShaderTilingUSliderFlag(bool srcval) {
		m_st_tilingUsliderFlag = srcval;
	};
	bool GetShaderTilingVSliderFlag() {
		return m_st_tilingVsliderFlag;
	};
	void SetShaderTilingVSliderFlag(bool srcval) {
		m_st_tilingVsliderFlag = srcval;
	};
	bool GetShaderTilingUSliderUpFlag() {
		return m_st_tilingUsliderUpFlag;
	};
	void SetShaderTilingUSliderUpFlag(bool srcval) {
		m_st_tilingUsliderUpFlag = srcval;
	};
	bool GetShaderTilingVSliderUpFlag() {
		return m_st_tilingVsliderUpFlag;
	};
	void SetShaderTilingVSliderUpFlag(bool srcval) {
		m_st_tilingVsliderUpFlag = srcval;
	};
	bool GetShaderAlphaTestSliderFlag() {
		return m_st_alphatestesliderFlag;
	};
	void SetShaderAlphaTestSliderFlag(bool srcval) {
		m_st_alphatestesliderFlag = srcval;
	};
	bool GetShaderDistortionChkFlag() {
		return m_st_distortionchkFlag;
	};
	void SetShaderDistortionChkFlag(bool srcval) {
		m_st_distortionchkFlag = srcval;
	};
	bool GetShaderDistortionScaleSliderFlag() {
		return m_st_distortionscalesliderFlag;
	};
	void SetShaderDistortionScaleSliderFlag(bool srcval) {
		m_st_distortionscalesliderFlag = srcval;
	};
	bool GetShaderRiverRadioFlag() {
		return m_st_riverradioFlag;
	};
	void SetShaderRiverRadioFlag(bool srcval) {
		m_st_riverradioFlag = srcval;
	};
	bool GetShaderSeaCenterUSliderFlag() {
		return m_st_seacenterUsliderFlag;
	};
	void SetShaderSeaCenterUSliderFlag(bool srcval) {
		m_st_seacenterUsliderFlag = srcval;
	};
	bool GetShaderSeaCenterVSliderFlag() {
		return m_st_seacenterVsliderFlag;
	};
	void SetShaderSeaCenterVSliderFlag(bool srcval) {
		m_st_seacenterVsliderFlag = srcval;
	};
	bool GetShaderRiverDirUSliderFlag() {
		return m_st_riverdirUsliderFlag;
	};
	void SetShaderRiverDirUSliderFlag(bool srcval) {
		m_st_riverdirUsliderFlag = srcval;
	};
	bool GetShaderRiverDirVSliderFlag() {
		return m_st_riverdirVsliderFlag;
	};
	void SetShaderRiverDirVSliderFlag(bool srcval) {
		m_st_riverdirVsliderFlag = srcval;
	};
	bool GetShaderFlowRateSliderFlag() {
		return m_st_flowratesliderFlag;
	};
	void SetShaderFlowRateSliderFlag(bool srcval) {
		m_st_flowratesliderFlag = srcval;
	};
	bool GetShaderDistortionMapRadioFlag() {
		return m_st_distortionmapradioFlag;
	};
	void SetShaderDistortionMapRadioFlag(bool srcval) {
		m_st_distortionmapradioFlag = srcval;
	};

	bool GetShaderToonParamChangeFlag() {
		return m_toonparamchange;
	};
	void SetShaderToonParamChangeFlag(bool srcval) {
		m_toonparamchange = srcval;
	};


	//##########
	//GUI state
	//##########
	int GetShaderTypeRadio()
	{
		if (m_st_shadertyperadio && m_model) {
			int shadertype = m_st_shadertyperadio->getSelectIndex();
			return shadertype;
		}
		else {
			return 0;
		}
	};
	bool GetShaderLightFlagChk()
	{
		if (m_st_lightflagchk && m_model) {
			return m_st_lightflagchk->getValue();
		}
		else {
			return true;
		}
	};
	bool GetShaderShadowCasterChk()
	{
		if (m_st_shadowcasterchk && m_model) {
			bool ischecked = m_st_shadowcasterchk->getValue();
			return ischecked;
		}
		else {
			return true;
		}
	};
	bool GetShadowNormalY0Chk() {
		if (m_st_normaly0chk && m_model) {
			bool ischecked = m_st_normaly0chk->getValue();
			return ischecked;
		}
		else {
			return false;
		}
	};
	float GetShaderSpcCoefSlider() {
		if (m_st_spccoefslider && m_model) {
			return (float)m_st_spccoefslider->getValue();
		}
		else {
			return 0.1250f;
		}
	};
	bool GetShaderEmissionChk()
	{
		if (m_st_emissionchk && m_model) {
			bool ischecked = m_st_emissionchk->getValue();
			return ischecked;
		}
		else {
			return false;
		}
	};
	float GetShaderEmissionSlider()
	{
		if (m_st_emissionslider && m_model) {
			float newemiscale = (float)m_st_emissionslider->getValue();
			return newemiscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderMetalSlider()
	{
		if (m_st_metalslider && m_model) {
			float newmetalcoef = (float)m_st_metalslider->getValue();
			return newmetalcoef;
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderSmoothSlider()
	{
		if (m_st_smoothslider && m_model) {
			float newsmoothcoef = (float)m_st_smoothslider->getValue();
			return newsmoothcoef;
		}
		else {
			return 0.250f;
		}
	};
	float GetShaderLitScaleSlider1() {
		if (m_st_litscaleslider1 && m_model) {
			float newlitscale = (float)m_st_litscaleslider1->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider2() {
		if (m_st_litscaleslider2 && m_model) {
			float newlitscale = (float)m_st_litscaleslider2->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider3() {
		if (m_st_litscaleslider3 && m_model) {
			float newlitscale = (float)m_st_litscaleslider3->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider4() {
		if (m_st_litscaleslider4 && m_model) {
			float newlitscale = (float)m_st_litscaleslider4->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider5() {
		if (m_st_litscaleslider5 && m_model) {
			float newlitscale = (float)m_st_litscaleslider5->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider6() {
		if (m_st_litscaleslider6 && m_model) {
			float newlitscale = (float)m_st_litscaleslider6->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider7() {
		if (m_st_litscaleslider7 && m_model) {
			float newlitscale = (float)m_st_litscaleslider7->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderLitScaleSlider8() {
		if (m_st_litscaleslider8 && m_model) {
			float newlitscale = (float)m_st_litscaleslider8->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	int GetShaderToonLitRadio()
	{
		if (m_st_toonlitradio && m_model) {
			int lightindex = m_st_toonlitradio->getSelectIndex();
			return lightindex;
		}
		else {
			return 0;
		}
	};
	float GetShaderToonHiAddrSlider()
	{
		if (m_st_toonhiaddrslider && m_model) {
			float hicolorh = (float)m_st_toonhiaddrslider->getValue();
			return hicolorh;
		}
		else {
			float hicolorh = 200.0f / 255.0f;
			return hicolorh;
		}
	};
	float GetShaderToonLowAddrSlider()
	{
		if (m_st_toonlowaddrslider && m_model) {
			float lowcolorh = (float)m_st_toonlowaddrslider->getValue();
			return lowcolorh;
		}
		else {
			float lowcolorh = 138.0f / 255.0f;
			return lowcolorh;
		}
	};
	bool GetShaderGradationChk()
	{
		if (m_st_gradationchk && m_model) {
			bool ischecked = m_st_gradationchk->getValue();
			return ischecked;
		}
		else {
			return true;
		}
	};
	bool GetShaderPowerToonChk()
	{
		if (m_st_powertoonchk && m_model) {
			bool ischecked = m_st_powertoonchk->getValue();
			return ischecked;
		}
		else {
			return true;
		}
	};
	float GetShaderToonBaseHSlider()
	{
		if (m_st_toonbaseHslider && m_model) {
			return (float)m_st_toonbaseHslider->getValue();
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonBaseSSlider()
	{
		if (m_st_toonbaseSslider && m_model) {
			return (float)m_st_toonbaseSslider->getValue();
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonBaseVSlider()
	{
		if (m_st_toonbaseVslider && m_model) {
			return (float)m_st_toonbaseVslider->getValue();
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderToonBaseASlider()
	{
		if (m_st_toonbaseAslider && m_model) {
			return (float)m_st_toonbaseAslider->getValue();
		}
		else {
			return 1.0f;
		}
	};
	float GetShaderToonHiHSlider()
	{
		if (m_st_toonhiHslider && m_model) {
			float newvalue = (float)m_st_toonhiHslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonHiSSlider()
	{
		if (m_st_toonhiSslider && m_model) {
			float newvalue = (float)m_st_toonhiSslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonHiVSlider()
	{
		if (m_st_toonhiVslider && m_model) {
			float newvalue = (float)m_st_toonhiVslider->getValue();
			return newvalue;
		}
		else {
			return 0.20f;
		}
	};
	float GetShaderToonHiASlider()
	{
		if (m_st_toonhiAslider && m_model) {
			float newvalue = (float)m_st_toonhiAslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonLowHSlider()
	{
		if (m_st_toonlowHslider && m_model) {
			float newvalue = (float)m_st_toonlowHslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonLowSSlider()
	{
		if (m_st_toonlowSslider && m_model) {
			float newvalue = (float)m_st_toonlowSslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderToonLowVSlider()
	{
		if (m_st_toonlowVslider && m_model) {
			float newvalue = (float)m_st_toonlowVslider->getValue();
			return newvalue;
		}
		else {
			return -0.20f;
		}
	};
	float GetShaderToonLowASlider()
	{
		if (m_st_toonlowAslider && m_model) {
			float newvalue = (float)m_st_toonlowAslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	int GetShaderTilingUSlider() {
		if (m_st_tilingUslider && m_model) {
			int newvalue = (int)(m_st_tilingUslider->getValue() + 0.0001);
			return newvalue;
		}
		else {
			return 1;
		}
	};
	int GetShaderTilingVSlider() {
		if (m_st_tilingVslider && m_model) {
			int newvalue = (int)(m_st_tilingVslider->getValue() + 0.0001);
			return newvalue;
		}
		else {
			return 1;
		}
	};
	void SetShaderTilingUSlider(int newvalue) {
		if (m_st_tilingUslider) {
			m_st_tilingUslider->setValue((double)newvalue, false);
		}
	};
	void SetShaderTilingVSlider(int newvalue) {
		if (m_st_tilingVslider) {
			m_st_tilingVslider->setValue((double)newvalue, false);
		}
	};
	double GetShaderAlphaTestSlider()
	{
		if (m_st_alphatestslider && m_model) {
			return m_st_alphatestslider->getValue();
		}
		else {
			return (8.0 / 255.0);
		}
	};
	bool GetShaderDistortionChk()
	{
		if (m_st_distortionchk && m_model) {
			bool ischecked = m_st_distortionchk->getValue();
			return ischecked;
		}
		else {
			return false;
		}
	};
	double GetShaderDistortonScaleSlider()
	{
		if (m_st_distortionscaleslider && m_model) {
			double newvalue = m_st_distortionscaleslider->getValue();
			return newvalue;
		}
		else {
			return 1.0;
		}
	};
	int GetShaderRiverRadio()
	{
		if (m_st_riverradio && m_model) {
			int riverorsea = m_st_riverradio->getSelectIndex();
			return riverorsea;
		}
		else {
			return 0;
		}
	};
	float GetShaderSeaCenterUSlider()
	{
		if (m_st_seacenterUslider && m_model) {
			return (float)m_st_seacenterUslider->getValue();
		}
		else {
			return 0.5f;
		}
	};
	float GetShaderSeaCenterVSlider()
	{
		if (m_st_seacenterVslider && m_model) {
			return (float)m_st_seacenterVslider->getValue();
		}
		else {
			return 0.5f;
		}
	};
	float GetShaderRiverDirUSlider()
	{
		if (m_st_riverdirUslider && m_model) {
			return (float)m_st_riverdirUslider->getValue();
		}
		else {
			return 0.0f;
		}
	};
	float GetShaderRiverDirVSlider()
	{
		if (m_st_riverdirVslider && m_model) {
			return (float)m_st_riverdirVslider->getValue();
		}
		else {
			return 1.0f;
		}
	};
	double GetShaderFlowRateSlider()
	{
		if (m_st_flowrateslider && m_model) {
			double newvalue = m_st_flowrateslider->getValue();
			return newvalue;
		}
		else {
			return 1.0;
		}
	};
	int GetShaderDistortionMapRadio()
	{
		if (m_st_distortionmapradio && m_model) {
			int maptype = m_st_distortionmapradio->getSelectIndex();
			return maptype;
		}
		else {
			return 1;
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

	OrgWinGUI::OrgWindow* m_dlgWnd;

	OrgWinGUI::OWP_ScrollWnd* m_st_Sc;
	OrgWinGUI::OWP_Separator* m_st_spall;
	OrgWinGUI::OWP_Label* m_st_spacerLabel01;
	OrgWinGUI::OWP_Label* m_st_spacerLabel02;
	OrgWinGUI::OWP_Label* m_st_spacerLabel03;
	OrgWinGUI::OWP_Label* m_st_spacerLabel04;
	OrgWinGUI::OWP_Label* m_st_spacerLabel05;
	OrgWinGUI::OWP_Label* m_st_spacerLabel06;
	OrgWinGUI::OWP_Separator* m_st_namesp;
	OrgWinGUI::OWP_Button* m_st_backB;
	OrgWinGUI::OWP_Label* m_st_namelabel;
	OrgWinGUI::OWP_RadioButton* m_st_shadertyperadio;
	OrgWinGUI::OWP_Separator* m_st_litflagsp;
	OrgWinGUI::OWP_CheckBoxA* m_st_lightflagchk;
	OrgWinGUI::OWP_CheckBoxA* m_st_shadowcasterchk;
	OrgWinGUI::OWP_CheckBoxA* m_st_normaly0chk;
	OrgWinGUI::OWP_Separator* m_st_spccoefsp0;
	OrgWinGUI::OWP_Separator* m_st_spccoefsp1;
	OrgWinGUI::OWP_Separator* m_st_spccoefsp2;
	OrgWinGUI::OWP_Label* m_st_spccoeflabel;
	OrgWinGUI::OWP_Slider* m_st_spccoefslider;
	OrgWinGUI::OWP_CheckBoxA* m_st_emissionchk;
	OrgWinGUI::OWP_Slider* m_st_emissionslider;
	OrgWinGUI::OWP_Separator* m_st_metalsp0;
	OrgWinGUI::OWP_Separator* m_st_metalsp1;
	OrgWinGUI::OWP_Separator* m_st_metalsp2;
	OrgWinGUI::OWP_Label* m_st_metallabel;
	OrgWinGUI::OWP_Slider* m_st_metalslider;
	OrgWinGUI::OWP_Label* m_st_smoothlabel;
	OrgWinGUI::OWP_Slider* m_st_smoothslider;

	OrgWinGUI::OWP_Separator* m_st_litscalesp1_0;
	OrgWinGUI::OWP_Separator* m_st_litscalesp1_1;
	OrgWinGUI::OWP_Separator* m_st_litscalesp1_2;
	OrgWinGUI::OWP_Label* m_st_litscalelabel1;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider1;
	OrgWinGUI::OWP_Label* m_st_litscalelabel2;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider2;

	OrgWinGUI::OWP_Separator* m_st_litscalesp3_0;
	OrgWinGUI::OWP_Separator* m_st_litscalesp3_1;
	OrgWinGUI::OWP_Separator* m_st_litscalesp3_2;
	OrgWinGUI::OWP_Label* m_st_litscalelabel3;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider3;
	OrgWinGUI::OWP_Label* m_st_litscalelabel4;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider4;

	OrgWinGUI::OWP_Separator* m_st_litscalesp5_0;
	OrgWinGUI::OWP_Separator* m_st_litscalesp5_1;
	OrgWinGUI::OWP_Separator* m_st_litscalesp5_2;
	OrgWinGUI::OWP_Label* m_st_litscalelabel5;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider5;
	OrgWinGUI::OWP_Label* m_st_litscalelabel6;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider6;

	OrgWinGUI::OWP_Separator* m_st_litscalesp7_0;
	OrgWinGUI::OWP_Separator* m_st_litscalesp7_1;
	OrgWinGUI::OWP_Separator* m_st_litscalesp7_2;
	OrgWinGUI::OWP_Label* m_st_litscalelabel7;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider7;
	OrgWinGUI::OWP_Label* m_st_litscalelabel8;
	OrgWinGUI::OWP_Slider* m_st_litscaleslider8;

	OrgWinGUI::OWP_RadioButton* m_st_toonlitradio;

	OrgWinGUI::OWP_Separator* m_st_toonaddrsp0;
	OrgWinGUI::OWP_Separator* m_st_toonaddrsp1;
	OrgWinGUI::OWP_Separator* m_st_toonaddrsp2;
	OrgWinGUI::OWP_Label* m_st_toonhiaddrlabel;
	OrgWinGUI::OWP_Slider* m_st_toonhiaddrslider;
	OrgWinGUI::OWP_Label* m_st_toonlowaddrlabel;
	OrgWinGUI::OWP_Slider* m_st_toonlowaddrslider;

	OrgWinGUI::OWP_Separator* m_st_gradationsp0;
	OrgWinGUI::OWP_CheckBoxA* m_st_gradationchk;
	OrgWinGUI::OWP_CheckBoxA* m_st_powertoonchk;

	OrgWinGUI::OWP_Separator* m_st_toonbasesp1_0;
	OrgWinGUI::OWP_Separator* m_st_toonbasesp1_1;
	OrgWinGUI::OWP_Separator* m_st_toonbasesp1_2;
	OrgWinGUI::OWP_Label* m_st_toonbaseHlabel;
	OrgWinGUI::OWP_Slider* m_st_toonbaseHslider;
	OrgWinGUI::OWP_Label* m_st_toonbaseSlabel;
	OrgWinGUI::OWP_Slider* m_st_toonbaseSslider;
	OrgWinGUI::OWP_Separator* m_st_toonbasesp2_0;
	OrgWinGUI::OWP_Separator* m_st_toonbasesp2_1;
	OrgWinGUI::OWP_Separator* m_st_toonbasesp2_2;
	OrgWinGUI::OWP_Label* m_st_toonbaseVlabel;
	OrgWinGUI::OWP_Slider* m_st_toonbaseVslider;
	OrgWinGUI::OWP_Label* m_st_toonbaseAlabel;
	OrgWinGUI::OWP_Slider* m_st_toonbaseAslider;

	OrgWinGUI::OWP_Separator* m_st_toonhisp1_0;
	OrgWinGUI::OWP_Separator* m_st_toonhisp1_1;
	OrgWinGUI::OWP_Separator* m_st_toonhisp1_2;
	OrgWinGUI::OWP_Label* m_st_toonhiHlabel;
	OrgWinGUI::OWP_Slider* m_st_toonhiHslider;
	OrgWinGUI::OWP_Label* m_st_toonhiSlabel;
	OrgWinGUI::OWP_Slider* m_st_toonhiSslider;
	OrgWinGUI::OWP_Separator* m_st_toonhisp2_0;
	OrgWinGUI::OWP_Separator* m_st_toonhisp2_1;
	OrgWinGUI::OWP_Separator* m_st_toonhisp2_2;
	OrgWinGUI::OWP_Label* m_st_toonhiVlabel;
	OrgWinGUI::OWP_Slider* m_st_toonhiVslider;
	OrgWinGUI::OWP_Label* m_st_toonhiAlabel;
	OrgWinGUI::OWP_Slider* m_st_toonhiAslider;

	OrgWinGUI::OWP_Separator* m_st_toonlowsp1_0;
	OrgWinGUI::OWP_Separator* m_st_toonlowsp1_1;
	OrgWinGUI::OWP_Separator* m_st_toonlowsp1_2;
	OrgWinGUI::OWP_Label* m_st_toonlowHlabel;
	OrgWinGUI::OWP_Slider* m_st_toonlowHslider;
	OrgWinGUI::OWP_Label* m_st_toonlowSlabel;
	OrgWinGUI::OWP_Slider* m_st_toonlowSslider;
	OrgWinGUI::OWP_Separator* m_st_toonlowsp2_0;
	OrgWinGUI::OWP_Separator* m_st_toonlowsp2_1;
	OrgWinGUI::OWP_Separator* m_st_toonlowsp2_2;
	OrgWinGUI::OWP_Label* m_st_toonlowVlabel;
	OrgWinGUI::OWP_Slider* m_st_toonlowVslider;
	OrgWinGUI::OWP_Label* m_st_toonlowAlabel;
	OrgWinGUI::OWP_Slider* m_st_toonlowAslider;

	OrgWinGUI::OWP_Separator* m_st_tilingsp0;
	OrgWinGUI::OWP_Separator* m_st_tilingsp1;
	OrgWinGUI::OWP_Separator* m_st_tilingsp2;
	OrgWinGUI::OWP_Label* m_st_tilingUlabel;
	OrgWinGUI::OWP_Slider* m_st_tilingUslider;
	OrgWinGUI::OWP_Label* m_st_tilingVlabel;
	OrgWinGUI::OWP_Slider* m_st_tilingVslider;

	OrgWinGUI::OWP_Separator* m_st_alphatestsp0;
	OrgWinGUI::OWP_Separator* m_st_alphatestsp1;
	OrgWinGUI::OWP_Label* m_st_alphatestlabel;
	OrgWinGUI::OWP_Slider* m_st_alphatestslider;

	OrgWinGUI::OWP_Separator* m_st_distortionsp0;
	OrgWinGUI::OWP_Separator* m_st_distortionsp1;
	OrgWinGUI::OWP_CheckBoxA* m_st_distortionchk;
	OrgWinGUI::OWP_Label* m_st_distortionscalelabel;
	OrgWinGUI::OWP_Slider* m_st_distortionscaleslider;

	OrgWinGUI::OWP_RadioButton* m_st_riverradio;

	OrgWinGUI::OWP_Separator* m_st_seacentersp0;
	OrgWinGUI::OWP_Separator* m_st_seacentersp1;
	OrgWinGUI::OWP_Label* m_st_seacenterlabel;
	OrgWinGUI::OWP_Slider* m_st_seacenterUslider;
	OrgWinGUI::OWP_Slider* m_st_seacenterVslider;

	OrgWinGUI::OWP_Separator* m_st_riverdirsp0;
	OrgWinGUI::OWP_Separator* m_st_riverdirsp1;
	OrgWinGUI::OWP_Label* m_st_riverdirlabel;
	OrgWinGUI::OWP_Slider* m_st_riverdirUslider;
	OrgWinGUI::OWP_Slider* m_st_riverdirVslider;

	OrgWinGUI::OWP_Separator* m_st_flowratesp0;
	OrgWinGUI::OWP_Label* m_st_flowratelabel;
	OrgWinGUI::OWP_Slider* m_st_flowrateslider;

	OrgWinGUI::OWP_RadioButton* m_st_distortionmapradio;


	CMQOMaterial* m_toonmqomaterial;//toonスライダーを離した後の処理用
	bool m_toonparamchange;//toonスライダーを離した後の処理用
	HSVTOON m_hsvtoonforall;//mqomaterial指定が無い場合の設定内容を保存
	CShaderTypeParams m_shadertypeparams;


	bool m_st_closeFlag;
	bool m_st_remakeToonTextureFlag;
	bool m_st_backFlag;
	bool m_st_stradioFlag;
	bool m_st_lightchkFlag;
	bool m_st_shadowcasterchkFlag;
	bool m_st_normaly0chkFlag;
	bool m_st_spccoefsliderFlag;
	bool m_st_emissionchkFlag;
	bool m_st_emissionsliderFlag;
	bool m_st_metalsliderFlag;
	bool m_st_smoothsliderFlag;
	bool m_st_litscale1Flag;
	bool m_st_litscale2Flag;
	bool m_st_litscale3Flag;
	bool m_st_litscale4Flag;
	bool m_st_litscale5Flag;
	bool m_st_litscale6Flag;
	bool m_st_litscale7Flag;
	bool m_st_litscale8Flag;
	bool m_st_toonlitradioFlag;
	bool m_st_toonhiaddrsliderFlag;
	bool m_st_toonlowaddrsliderFlag;
	bool m_st_gradationchkFlag;
	bool m_st_powertoonchkFlag;
	bool m_st_toonbaseHsliderFlag;
	bool m_st_toonbaseSsliderFlag;
	bool m_st_toonbaseVsliderFlag;
	bool m_st_toonbaseAsliderFlag;
	bool m_st_toonhiHsliderFlag;
	bool m_st_toonhiSsliderFlag;
	bool m_st_toonhiVsliderFlag;
	bool m_st_toonhiAsliderFlag;
	bool m_st_toonlowHsliderFlag;
	bool m_st_toonlowSsliderFlag;
	bool m_st_toonlowVsliderFlag;
	bool m_st_toonlowAsliderFlag;
	bool m_st_tilingUsliderFlag;
	bool m_st_tilingVsliderFlag;
	bool m_st_tilingUsliderUpFlag;
	bool m_st_tilingVsliderUpFlag;
	bool m_st_alphatestesliderFlag;
	bool m_st_distortionchkFlag;
	bool m_st_distortionscalesliderFlag;
	bool m_st_riverradioFlag;
	bool m_st_seacenterUsliderFlag;
	bool m_st_seacenterVsliderFlag;
	bool m_st_riverdirUsliderFlag;
	bool m_st_riverdirVsliderFlag;
	bool m_st_flowratesliderFlag;
	bool m_st_distortionmapradioFlag;
};

#endif //__ColiIDDlg_H_
