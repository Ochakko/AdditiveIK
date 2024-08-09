#ifndef __CSkyParamsDlg_H_
#define __CSkyParamsDlg_H_


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

class CShaderTypeParams;

/////////////////////////////////////////////////////////////////////////////
// CSkyParamsDlg
class CSkyParamsDlg
{
public:
	CSkyParamsDlg();
	~CSkyParamsDlg();

	int SetPosAndSize(int srcposx, int srcposy, int srcsizex, int srcsizey);//!!!!!!!!

	bool GetCreatedFlag()
	{
		return m_createdflag;
	};

	//void SetModel(CModel* srcmodel, CShaderTypeParams* srcshadertypeparams);
	int ParamsToDlg(CModel* srcmodel, CMQOMaterial* srcmat, CShaderTypeParams* srcshadertypeparams);
	void SetVisible(bool srcflag);
	int OnFrameSkyParams();

	void InitParams();
	int DestroyObjs();

	const HWND GetHWnd();
	const bool GetVisible();
	void ListenMouse(bool srcflag);

private:
	int CreateSkyParamsWnd();

public:
	bool GetSkyCloseFlag() {
		return m_skyst_closeFlag;
	};
	void SetSkyCloseFlag(bool srcval) {
		m_skyst_closeFlag = srcval;
	};
	bool GetSkyRemakeToonTextureFlag() {
		return m_skyst_remakeToonTextureFlag;
	};
	void SetSkyRemakeToonTextureFlag(bool srcval)
	{
		m_skyst_remakeToonTextureFlag = srcval;
	}
	bool GetSkyBackFlag() {
		return m_skyst_backFlag;
	};
	void SetSkyBackFlag(bool srcval) {
		m_skyst_backFlag = srcval;
	};
	bool GetSkyStRadioFlag() {
		return m_skyst_stradioFlag;
	};
	void SetSkyStRadioFlag(bool srcval) {
		m_skyst_stradioFlag = srcval;
	};
	bool GetSkyLightChkFlag() {
		return m_skyst_lightchkFlag;
	};
	void SetSkyLightChkFlag(bool srcval) {
		m_skyst_lightchkFlag = srcval;
	}
	bool GetSkyShadowCasterChkFlag() {
		return m_skyst_shadowcasterchkFlag;
	};
	void SetSkyShadowCasterChkFlag(bool srcval) {
		m_skyst_shadowcasterchkFlag = srcval;
	};
	bool GetSkyNormalY0ChkFlag() {
		return m_skyst_normaly0chkFlag;
	};
	void SetSkyNormalY0ChkFlag(bool srcval) {
		m_skyst_normaly0chkFlag = srcval;
	};
	bool GetSkySpcCoefSliderFlag() {
		return m_skyst_spccoefsliderFlag;
	};
	void SetSkySpcCoefSliderFlag(bool srcval) {
		m_skyst_spccoefsliderFlag = srcval;
	};
	bool GetSkyEmissionChkFlag() {
		return m_skyst_emissionchkFlag;
	};
	void SetSkyEmissionChkFlag(bool srcval) {
		m_skyst_emissionchkFlag = srcval;
	};
	bool GetSkyEmissionSliderFlag() {
		return m_skyst_emissionsliderFlag;
	};
	void SetSkyEmissionSliderFlag(bool srcval) {
		m_skyst_emissionsliderFlag = srcval;
	};
	bool GetSkyMetalSliderFlag() {
		return m_skyst_metalsliderFlag;
	};
	void SetSkyMetalSliderFlag(bool srcval) {
		m_skyst_metalsliderFlag = srcval;
	};
	bool GetSkySmoothSliderFlag() {
		return m_skyst_smoothsliderFlag;
	};
	void SetSkySmoothSliderFlag(bool srcval) {
		m_skyst_smoothsliderFlag = srcval;
	};
	bool GetSkyLitScale1Flag() {
		return m_skyst_litscale1Flag;
	};
	void SetSkyLitScale1Flag(bool srcval) {
		m_skyst_litscale1Flag = srcval;
	};
	bool GetSkyLitScale2Flag() {
		return m_skyst_litscale2Flag;
	};
	void SetSkyLitScale2Flag(bool srcval) {
		m_skyst_litscale2Flag = srcval;
	};
	bool GetSkyLitScale3Flag() {
		return m_skyst_litscale3Flag;
	};
	void SetSkyLitScale3Flag(bool srcval) {
		m_skyst_litscale3Flag = srcval;
	};
	bool GetSkyLitScale4Flag() {
		return m_skyst_litscale4Flag;
	};
	void SetSkyLitScale4Flag(bool srcval) {
		m_skyst_litscale4Flag = srcval;
	};
	bool GetSkyLitScale5Flag() {
		return m_skyst_litscale5Flag;
	};
	void SetSkyLitScale5Flag(bool srcval) {
		m_skyst_litscale5Flag = srcval;
	};
	bool GetSkyLitScale6Flag() {
		return m_skyst_litscale6Flag;
	};
	void SetSkyLitScale6Flag(bool srcval) {
		m_skyst_litscale6Flag = srcval;
	};
	bool GetSkyLitScale7Flag() {
		return m_skyst_litscale7Flag;
	};
	void SetSkyLitScale7Flag(bool srcval) {
		m_skyst_litscale7Flag = srcval;
	};
	bool GetSkyLitScale8Flag() {
		return m_skyst_litscale8Flag;
	};
	void SetSkyLitScale8Flag(bool srcval) {
		m_skyst_litscale8Flag = srcval;
	};
	bool GetSkyToonLitRadioFlag() {
		return m_skyst_toonlitradioFlag;
	};
	void SetSkyToonLitRadioFlag(bool srcval) {
		m_skyst_toonlitradioFlag = srcval;
	};
	bool GetSkyToonHiAddrSliderFlag() {
		return m_skyst_toonhiaddrsliderFlag;
	};
	void SetSkyToonHiAddrSliderFlag(bool srcval) {
		m_skyst_toonhiaddrsliderFlag = srcval;
	};
	bool GetSkyToonLowAddrSliderFlag() {
		return m_skyst_toonlowaddrsliderFlag;
	};
	void SetSkyToonLowAddrSliderFlag(bool srcval) {
		m_skyst_toonlowaddrsliderFlag = srcval;
	};
	bool GetSkyGradationChkFlag() {
		return m_skyst_gradationchkFlag;
	};
	void SetSkyGradationChkFlag(bool srcval) {
		m_skyst_gradationchkFlag = srcval;
	};
	bool GetSkyPowerToonChkFlag() {
		return m_skyst_powertoonchkFlag;
	};
	void SetSkyPowerToonChkFlag(bool srcval) {
		m_skyst_powertoonchkFlag = srcval;
	};
	bool GetSkyToonBaseHSliderFlag() {
		return m_skyst_toonbaseHsliderFlag;
	};
	void SetSkyToonBaseHSliderFlag(bool srcval) {
		m_skyst_toonbaseHsliderFlag = srcval;
	};
	bool GetSkyToonBaseSSliderFlag() {
		return m_skyst_toonbaseSsliderFlag;
	};
	void SetSkyToonBaseSSliderFlag(bool srcval) {
		m_skyst_toonbaseSsliderFlag = srcval;
	};
	bool GetSkyToonBaseVSliderFlag() {
		return m_skyst_toonbaseVsliderFlag;
	};
	void SetSkyToonBaseVSliderFlag(bool srcval) {
		m_skyst_toonbaseVsliderFlag = srcval;
	};
	bool GetSkyToonBaseASliderFlag() {
		return m_skyst_toonbaseAsliderFlag;
	};
	void SetSkyToonBaseASliderFlag(bool srcval) {
		m_skyst_toonbaseAsliderFlag = srcval;
	};
	bool GetSkyToonHiHSliderFlag() {
		return m_skyst_toonhiHsliderFlag;
	};
	void SetSkyToonHiHSliderFlag(bool srcval) {
		m_skyst_toonhiHsliderFlag = srcval;
	};
	bool GetSkyToonHiSSliderFlag() {
		return m_skyst_toonhiSsliderFlag;
	};
	void SetSkyToonHiSSliderFlag(bool srcval) {
		m_skyst_toonhiSsliderFlag = srcval;
	};
	bool GetSkyToonHiVSliderFlag() {
		return m_skyst_toonhiVsliderFlag;
	};
	void SetSkyToonHiVSliderFlag(bool srcval) {
		m_skyst_toonhiVsliderFlag = srcval;
	};
	bool GetSkyToonHiASliderFlag() {
		return m_skyst_toonhiAsliderFlag;
	};
	void SetSkyToonHiASliderFlag(bool srcval) {
		m_skyst_toonhiAsliderFlag = srcval;
	};
	bool GetSkyToonLowHSliderFlag() {
		return m_skyst_toonlowHsliderFlag;
	};
	void SetSkyToonLowHSliderFlag(bool srcval) {
		m_skyst_toonlowHsliderFlag = srcval;
	};
	bool GetSkyToonLowSSliderFlag() {
		return m_skyst_toonlowSsliderFlag;
	};
	void SetSkyToonLowSSliderFlag(bool srcval) {
		m_skyst_toonlowSsliderFlag = srcval;
	};
	bool GetSkyToonLowVSliderFlag() {
		return m_skyst_toonlowVsliderFlag;
	};
	void SetSkyToonLowVSliderFlag(bool srcval) {
		m_skyst_toonlowVsliderFlag = srcval;
	};
	bool GetSkyToonLowASliderFlag() {
		return m_skyst_toonlowAsliderFlag;
	};
	void SetSkyToonLowASliderFlag(bool srcval) {
		m_skyst_toonlowAsliderFlag = srcval;
	};
	bool GetSkyTilingUSliderFlag() {
		return m_skyst_tilingUsliderFlag;
	};
	void SetSkyTilingUSliderFlag(bool srcval) {
		m_skyst_tilingUsliderFlag = srcval;
	};
	bool GetSkyTilingVSliderFlag() {
		return m_skyst_tilingVsliderFlag;
	};
	void SetSkyTilingVSliderFlag(bool srcval) {
		m_skyst_tilingVsliderFlag = srcval;
	};
	bool GetSkyTilingUSliderUpFlag() {
		return m_skyst_tilingUsliderUpFlag;
	};
	void SetSkyTilingUSliderUpFlag(bool srcval) {
		m_skyst_tilingUsliderUpFlag = srcval;
	};
	bool GetSkyTilingVSliderUpFlag() {
		return m_skyst_tilingVsliderUpFlag;
	};
	void SetSkyTilingVSliderUpFlag(bool srcval) {
		m_skyst_tilingVsliderUpFlag = srcval;
	};
	bool GetSkyAlphaTestSliderFlag() {
		return m_skyst_alphatestesliderFlag;
	};
	void SetSkyAlphaTestSliderFlag(bool srcval) {
		m_skyst_alphatestesliderFlag = srcval;
	};
	bool GetSkyDistortionChkFlag() {
		return m_skyst_distortionchkFlag;
	};
	void SetSkyDistortionChkFlag(bool srcval) {
		m_skyst_distortionchkFlag = srcval;
	};
	bool GetSkyDistortionScaleSliderFlag() {
		return m_skyst_distortionscalesliderFlag;
	};
	void SetSkyDistortionScaleSliderFlag(bool srcval) {
		m_skyst_distortionscalesliderFlag = srcval;
	};
	bool GetSkyRiverRadioFlag() {
		return m_skyst_riverradioFlag;
	};
	void SetSkyRiverRadioFlag(bool srcval) {
		m_skyst_riverradioFlag = srcval;
	};
	bool GetSkySeaCenterUSliderFlag() {
		return m_skyst_seacenterUsliderFlag;
	};
	void SetSkySeaCenterUSliderFlag(bool srcval) {
		m_skyst_seacenterUsliderFlag = srcval;
	};
	bool GetSkySeaCenterVSliderFlag() {
		return m_skyst_seacenterVsliderFlag;
	};
	void SetSkySeaCenterVSliderFlag(bool srcval) {
		m_skyst_seacenterVsliderFlag = srcval;
	};
	bool GetSkyRiverDirUSliderFlag() {
		return m_skyst_riverdirUsliderFlag;
	};
	void SetSkyRiverDirUSliderFlag(bool srcval) {
		m_skyst_riverdirUsliderFlag = srcval;
	};
	bool GetSkyRiverDirVSliderFlag() {
		return m_skyst_riverdirVsliderFlag;
	};
	void SetSkyRiverDirVSliderFlag(bool srcval) {
		m_skyst_riverdirVsliderFlag = srcval;
	};
	bool GetSkyFlowRateSliderFlag() {
		return m_skyst_flowratesliderFlag;
	};
	void SetSkyFlowRateSliderFlag(bool srcval) {
		m_skyst_flowratesliderFlag = srcval;
	};
	bool GetSkyDistortionMapRadioFlag() {
		return m_skyst_distortionmapradioFlag;
	};
	void SetSkyDistortionMapRadioFlag(bool srcval) {
		m_skyst_distortionmapradioFlag = srcval;
	};


	//##########
	//GUI state
	//##########
	int GetSkyTypeRadio()
	{
		if (m_skyst_shadertyperadio && m_model) {
			int shadertype = m_skyst_shadertyperadio->getSelectIndex();
			return shadertype;
		}
		else {
			return 0;
		}
	};
	bool GetSkyLightFlagChk()
	{
		if (m_skyst_lightflagchk && m_model) {
			return m_skyst_lightflagchk->getValue();
		}
		else {
			return true;
		}
	};
	bool GetSkyShadowCasterChk()
	{
		if (m_skyst_shadowcasterchk && m_model) {
			bool ischecked = m_skyst_shadowcasterchk->getValue();
			return ischecked;
		}
		else {
			return true;
		}
	};
	bool GetShadowNormalY0Chk() {
		if (m_skyst_normaly0chk && m_model) {
			bool ischecked = m_skyst_normaly0chk->getValue();
			return ischecked;
		}
		else {
			return false;
		}
	};
	float GetSkySpcCoefSlider() {
		if (m_skyst_spccoefslider && m_model) {
			return (float)m_skyst_spccoefslider->getValue();
		}
		else {
			return 0.1250f;
		}
	};
	bool GetSkyEmissionChk()
	{
		if (m_skyst_emissionchk && m_model) {
			bool ischecked = m_skyst_emissionchk->getValue();
			return ischecked;
		}
		else {
			return false;
		}
	};
	float GetSkyEmissionSlider()
	{
		if (m_skyst_emissionslider && m_model) {
			float newemiscale = (float)m_skyst_emissionslider->getValue();
			return newemiscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyMetalSlider()
	{
		if (m_skyst_metalslider && m_model) {
			float newmetalcoef = (float)m_skyst_metalslider->getValue();
			return newmetalcoef;
		}
		else {
			return 0.0f;
		}
	};
	float GetSkySmoothSlider()
	{
		if (m_skyst_smoothslider && m_model) {
			float newsmoothcoef = (float)m_skyst_smoothslider->getValue();
			return newsmoothcoef;
		}
		else {
			return 0.250f;
		}
	};
	float GetSkyLitScaleSlider1() {
		if (m_skyst_litscaleslider1 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider1->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider2() {
		if (m_skyst_litscaleslider2 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider2->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider3() {
		if (m_skyst_litscaleslider3 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider3->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider4() {
		if (m_skyst_litscaleslider4 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider4->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider5() {
		if (m_skyst_litscaleslider5 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider5->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider6() {
		if (m_skyst_litscaleslider6 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider6->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider7() {
		if (m_skyst_litscaleslider7 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider7->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyLitScaleSlider8() {
		if (m_skyst_litscaleslider8 && m_model) {
			float newlitscale = (float)m_skyst_litscaleslider8->getValue();
			return newlitscale;
		}
		else {
			return 1.0f;
		}
	};
	int GetSkyToonLitRadio()
	{
		if (m_skyst_toonlitradio && m_model) {
			int lightindex = m_skyst_toonlitradio->getSelectIndex();
			return lightindex;
		}
		else {
			return 0;
		}
	};
	float GetSkyToonHiAddrSlider()
	{
		if (m_skyst_toonhiaddrslider && m_model) {
			float hicolorh = (float)m_skyst_toonhiaddrslider->getValue();
			return hicolorh;
		}
		else {
			float hicolorh = 200.0f / 255.0f;
			return hicolorh;
		}
	};
	float GetSkyToonLowAddrSlider()
	{
		if (m_skyst_toonlowaddrslider && m_model) {
			float lowcolorh = (float)m_skyst_toonlowaddrslider->getValue();
			return lowcolorh;
		}
		else {
			float lowcolorh = 138.0f / 255.0f;
			return lowcolorh;
		}
	};
	bool GetSkyGradationChk()
	{
		if (m_skyst_gradationchk && m_model) {
			bool ischecked = m_skyst_gradationchk->getValue();
			return ischecked;
		}
		else {
			return true;
		}
	};
	bool GetSkyPowerToonChk()
	{
		if (m_skyst_powertoonchk && m_model) {
			bool ischecked = m_skyst_powertoonchk->getValue();
			return ischecked;
		}
		else {
			return true;
		}
	};
	float GetSkyToonBaseHSlider()
	{
		if (m_skyst_toonbaseHslider && m_model) {
			return (float)m_skyst_toonbaseHslider->getValue();
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonBaseSSlider()
	{
		if (m_skyst_toonbaseSslider && m_model) {
			return (float)m_skyst_toonbaseSslider->getValue();
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonBaseVSlider()
	{
		if (m_skyst_toonbaseVslider && m_model) {
			return (float)m_skyst_toonbaseVslider->getValue();
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyToonBaseASlider()
	{
		if (m_skyst_toonbaseAslider && m_model) {
			return (float)m_skyst_toonbaseAslider->getValue();
		}
		else {
			return 1.0f;
		}
	};
	float GetSkyToonHiHSlider()
	{
		if (m_skyst_toonhiHslider && m_model) {
			float newvalue = (float)m_skyst_toonhiHslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonHiSSlider()
	{
		if (m_skyst_toonhiSslider && m_model) {
			float newvalue = (float)m_skyst_toonhiSslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonHiVSlider()
	{
		if (m_skyst_toonhiVslider && m_model) {
			float newvalue = (float)m_skyst_toonhiVslider->getValue();
			return newvalue;
		}
		else {
			return 0.20f;
		}
	};
	float GetSkyToonHiASlider()
	{
		if (m_skyst_toonhiAslider && m_model) {
			float newvalue = (float)m_skyst_toonhiAslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonLowHSlider()
	{
		if (m_skyst_toonlowHslider && m_model) {
			float newvalue = (float)m_skyst_toonlowHslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonLowSSlider()
	{
		if (m_skyst_toonlowSslider && m_model) {
			float newvalue = (float)m_skyst_toonlowSslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyToonLowVSlider()
	{
		if (m_skyst_toonlowVslider && m_model) {
			float newvalue = (float)m_skyst_toonlowVslider->getValue();
			return newvalue;
		}
		else {
			return -0.20f;
		}
	};
	float GetSkyToonLowASlider()
	{
		if (m_skyst_toonlowAslider && m_model) {
			float newvalue = (float)m_skyst_toonlowAslider->getValue();
			return newvalue;
		}
		else {
			return 0.0f;
		}
	};
	int GetSkyTilingUSlider() {
		if (m_skyst_tilingUslider && m_model) {
			int newvalue = (int)(m_skyst_tilingUslider->getValue() + 0.0001);
			return newvalue;
		}
		else {
			return 1;
		}
	};
	int GetSkyTilingVSlider() {
		if (m_skyst_tilingVslider && m_model) {
			int newvalue = (int)(m_skyst_tilingVslider->getValue() + 0.0001);
			return newvalue;
		}
		else {
			return 1;
		}
	};
	void SetSkyTilingUSlider(int newvalue) {
		if (m_skyst_tilingUslider) {
			m_skyst_tilingUslider->setValue((double)newvalue, false);
		}
	};
	void SetSkyTilingVSlider(int newvalue) {
		if (m_skyst_tilingVslider) {
			m_skyst_tilingVslider->setValue((double)newvalue, false);
		}
	};
	double GetSkyAlphaTestSlider()
	{
		if (m_skyst_alphatestslider && m_model) {
			return m_skyst_alphatestslider->getValue();
		}
		else {
			return (8.0 / 255.0);
		}
	};
	bool GetSkyDistortionChk()
	{
		if (m_skyst_distortionchk && m_model) {
			bool ischecked = m_skyst_distortionchk->getValue();
			return ischecked;
		}
		else {
			return false;
		}
	};
	double GetSkyDistortonScaleSlider()
	{
		if (m_skyst_distortionscaleslider && m_model) {
			double newvalue = m_skyst_distortionscaleslider->getValue();
			return newvalue;
		}
		else {
			return 1.0;
		}
	};
	int GetSkyRiverRadio()
	{
		if (m_skyst_riverradio && m_model) {
			int riverorsea = m_skyst_riverradio->getSelectIndex();
			return riverorsea;
		}
		else {
			return 0;
		}
	};
	float GetSkySeaCenterUSlider()
	{
		if (m_skyst_seacenterUslider && m_model) {
			return (float)m_skyst_seacenterUslider->getValue();
		}
		else {
			return 0.5f;
		}
	};
	float GetSkySeaCenterVSlider()
	{
		if (m_skyst_seacenterVslider && m_model) {
			return (float)m_skyst_seacenterVslider->getValue();
		}
		else {
			return 0.5f;
		}
	};
	float GetSkyRiverDirUSlider()
	{
		if (m_skyst_riverdirUslider && m_model) {
			return (float)m_skyst_riverdirUslider->getValue();
		}
		else {
			return 0.0f;
		}
	};
	float GetSkyRiverDirVSlider()
	{
		if (m_skyst_riverdirVslider && m_model) {
			return (float)m_skyst_riverdirVslider->getValue();
		}
		else {
			return 1.0f;
		}
	};
	double GetSkyFlowRateSlider()
	{
		if (m_skyst_flowrateslider && m_model) {
			double newvalue = m_skyst_flowrateslider->getValue();
			return newvalue;
		}
		else {
			return 1.0;
		}
	};
	int GetSkyDistortionMapRadio()
	{
		if (m_skyst_distortionmapradio && m_model) {
			int maptype = m_skyst_distortionmapradio->getSelectIndex();
			return maptype;
		}
		else {
			return 1;
		}
	};

	//###########
	//Slot
	//###########
	int GetSkySlotIndex() {
		return m_skyst_slotindex;
	};
	bool GetSkySlotFlag() {
		return m_skyst_slotFlag;
	};
	void SetSkySlotFlag(bool srcval) {
		m_skyst_slotFlag = srcval;
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

	OrgWinGUI::OWP_ScrollWnd* m_skyst_Sc;
	OrgWinGUI::OWP_Separator* m_skyst_spall;
	OrgWinGUI::OWP_Label* m_skyst_spacerLabel01;
	OrgWinGUI::OWP_Label* m_skyst_spacerLabel02;
	OrgWinGUI::OWP_Label* m_skyst_spacerLabel03;
	OrgWinGUI::OWP_Label* m_skyst_spacerLabel04;
	OrgWinGUI::OWP_Label* m_skyst_spacerLabel05;
	OrgWinGUI::OWP_Label* m_skyst_spacerLabel06;
	OrgWinGUI::OWP_Separator* m_skyst_namesp;
	OrgWinGUI::OWP_Button* m_skyst_backB;
	OrgWinGUI::OWP_Label* m_skyst_namelabel;
	OrgWinGUI::OWP_RadioButton* m_skyst_shadertyperadio;
	OrgWinGUI::OWP_Separator* m_skyst_litflagsp;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_lightflagchk;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_shadowcasterchk;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_normaly0chk;
	OrgWinGUI::OWP_Separator* m_skyst_spccoefsp0;
	OrgWinGUI::OWP_Separator* m_skyst_spccoefsp1;
	OrgWinGUI::OWP_Separator* m_skyst_spccoefsp2;
	OrgWinGUI::OWP_Label* m_skyst_spccoeflabel;
	OrgWinGUI::OWP_Slider* m_skyst_spccoefslider;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_emissionchk;
	OrgWinGUI::OWP_Slider* m_skyst_emissionslider;
	OrgWinGUI::OWP_Separator* m_skyst_metalsp0;
	OrgWinGUI::OWP_Separator* m_skyst_metalsp1;
	OrgWinGUI::OWP_Separator* m_skyst_metalsp2;
	OrgWinGUI::OWP_Label* m_skyst_metallabel;
	OrgWinGUI::OWP_Slider* m_skyst_metalslider;
	OrgWinGUI::OWP_Label* m_skyst_smoothlabel;
	OrgWinGUI::OWP_Slider* m_skyst_smoothslider;

	OrgWinGUI::OWP_Separator* m_skyst_litscalesp1_0;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp1_1;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp1_2;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel1;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider1;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel2;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider2;

	OrgWinGUI::OWP_Separator* m_skyst_litscalesp3_0;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp3_1;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp3_2;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel3;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider3;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel4;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider4;

	OrgWinGUI::OWP_Separator* m_skyst_litscalesp5_0;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp5_1;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp5_2;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel5;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider5;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel6;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider6;

	OrgWinGUI::OWP_Separator* m_skyst_litscalesp7_0;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp7_1;
	OrgWinGUI::OWP_Separator* m_skyst_litscalesp7_2;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel7;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider7;
	OrgWinGUI::OWP_Label* m_skyst_litscalelabel8;
	OrgWinGUI::OWP_Slider* m_skyst_litscaleslider8;

	OrgWinGUI::OWP_RadioButton* m_skyst_toonlitradio;

	OrgWinGUI::OWP_Separator* m_skyst_toonaddrsp0;
	OrgWinGUI::OWP_Separator* m_skyst_toonaddrsp1;
	OrgWinGUI::OWP_Separator* m_skyst_toonaddrsp2;
	OrgWinGUI::OWP_Label* m_skyst_toonhiaddrlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonhiaddrslider;
	OrgWinGUI::OWP_Label* m_skyst_toonlowaddrlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonlowaddrslider;

	OrgWinGUI::OWP_Separator* m_skyst_gradationsp0;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_gradationchk;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_powertoonchk;

	OrgWinGUI::OWP_Separator* m_skyst_toonbasesp1_0;
	OrgWinGUI::OWP_Separator* m_skyst_toonbasesp1_1;
	OrgWinGUI::OWP_Separator* m_skyst_toonbasesp1_2;
	OrgWinGUI::OWP_Label* m_skyst_toonbaseHlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonbaseHslider;
	OrgWinGUI::OWP_Label* m_skyst_toonbaseSlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonbaseSslider;
	OrgWinGUI::OWP_Separator* m_skyst_toonbasesp2_0;
	OrgWinGUI::OWP_Separator* m_skyst_toonbasesp2_1;
	OrgWinGUI::OWP_Separator* m_skyst_toonbasesp2_2;
	OrgWinGUI::OWP_Label* m_skyst_toonbaseVlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonbaseVslider;
	OrgWinGUI::OWP_Label* m_skyst_toonbaseAlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonbaseAslider;

	OrgWinGUI::OWP_Separator* m_skyst_toonhisp1_0;
	OrgWinGUI::OWP_Separator* m_skyst_toonhisp1_1;
	OrgWinGUI::OWP_Separator* m_skyst_toonhisp1_2;
	OrgWinGUI::OWP_Label* m_skyst_toonhiHlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonhiHslider;
	OrgWinGUI::OWP_Label* m_skyst_toonhiSlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonhiSslider;
	OrgWinGUI::OWP_Separator* m_skyst_toonhisp2_0;
	OrgWinGUI::OWP_Separator* m_skyst_toonhisp2_1;
	OrgWinGUI::OWP_Separator* m_skyst_toonhisp2_2;
	OrgWinGUI::OWP_Label* m_skyst_toonhiVlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonhiVslider;
	OrgWinGUI::OWP_Label* m_skyst_toonhiAlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonhiAslider;

	OrgWinGUI::OWP_Separator* m_skyst_toonlowsp1_0;
	OrgWinGUI::OWP_Separator* m_skyst_toonlowsp1_1;
	OrgWinGUI::OWP_Separator* m_skyst_toonlowsp1_2;
	OrgWinGUI::OWP_Label* m_skyst_toonlowHlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonlowHslider;
	OrgWinGUI::OWP_Label* m_skyst_toonlowSlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonlowSslider;
	OrgWinGUI::OWP_Separator* m_skyst_toonlowsp2_0;
	OrgWinGUI::OWP_Separator* m_skyst_toonlowsp2_1;
	OrgWinGUI::OWP_Separator* m_skyst_toonlowsp2_2;
	OrgWinGUI::OWP_Label* m_skyst_toonlowVlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonlowVslider;
	OrgWinGUI::OWP_Label* m_skyst_toonlowAlabel;
	OrgWinGUI::OWP_Slider* m_skyst_toonlowAslider;

	OrgWinGUI::OWP_Separator* m_skyst_tilingsp0;
	OrgWinGUI::OWP_Separator* m_skyst_tilingsp1;
	OrgWinGUI::OWP_Separator* m_skyst_tilingsp2;
	OrgWinGUI::OWP_Label* m_skyst_tilingUlabel;
	OrgWinGUI::OWP_Slider* m_skyst_tilingUslider;
	OrgWinGUI::OWP_Label* m_skyst_tilingVlabel;
	OrgWinGUI::OWP_Slider* m_skyst_tilingVslider;

	OrgWinGUI::OWP_Separator* m_skyst_alphatestsp0;
	OrgWinGUI::OWP_Separator* m_skyst_alphatestsp1;
	OrgWinGUI::OWP_Label* m_skyst_alphatestlabel;
	OrgWinGUI::OWP_Slider* m_skyst_alphatestslider;

	OrgWinGUI::OWP_Separator* m_skyst_distortionsp0;
	OrgWinGUI::OWP_Separator* m_skyst_distortionsp1;
	OrgWinGUI::OWP_CheckBoxA* m_skyst_distortionchk;
	OrgWinGUI::OWP_Label* m_skyst_distortionscalelabel;
	OrgWinGUI::OWP_Slider* m_skyst_distortionscaleslider;

	OrgWinGUI::OWP_RadioButton* m_skyst_riverradio;

	OrgWinGUI::OWP_Separator* m_skyst_seacentersp0;
	OrgWinGUI::OWP_Separator* m_skyst_seacentersp1;
	OrgWinGUI::OWP_Label* m_skyst_seacenterlabel;
	OrgWinGUI::OWP_Slider* m_skyst_seacenterUslider;
	OrgWinGUI::OWP_Slider* m_skyst_seacenterVslider;

	OrgWinGUI::OWP_Separator* m_skyst_riverdirsp0;
	OrgWinGUI::OWP_Separator* m_skyst_riverdirsp1;
	OrgWinGUI::OWP_Label* m_skyst_riverdirlabel;
	OrgWinGUI::OWP_Slider* m_skyst_riverdirUslider;
	OrgWinGUI::OWP_Slider* m_skyst_riverdirVslider;

	OrgWinGUI::OWP_Separator* m_skyst_flowratesp0;
	OrgWinGUI::OWP_Label* m_skyst_flowratelabel;
	OrgWinGUI::OWP_Slider* m_skyst_flowrateslider;

	OrgWinGUI::OWP_RadioButton* m_skyst_distortionmapradio;

	bool m_skyst_closeFlag;
	bool m_skyst_remakeToonTextureFlag;
	bool m_skyst_backFlag;
	bool m_skyst_stradioFlag;
	bool m_skyst_lightchkFlag;
	bool m_skyst_shadowcasterchkFlag;
	bool m_skyst_normaly0chkFlag;
	bool m_skyst_spccoefsliderFlag;
	bool m_skyst_emissionchkFlag;
	bool m_skyst_emissionsliderFlag;
	bool m_skyst_metalsliderFlag;
	bool m_skyst_smoothsliderFlag;
	bool m_skyst_litscale1Flag;
	bool m_skyst_litscale2Flag;
	bool m_skyst_litscale3Flag;
	bool m_skyst_litscale4Flag;
	bool m_skyst_litscale5Flag;
	bool m_skyst_litscale6Flag;
	bool m_skyst_litscale7Flag;
	bool m_skyst_litscale8Flag;
	bool m_skyst_toonlitradioFlag;
	bool m_skyst_toonhiaddrsliderFlag;
	bool m_skyst_toonlowaddrsliderFlag;
	bool m_skyst_gradationchkFlag;
	bool m_skyst_powertoonchkFlag;
	bool m_skyst_toonbaseHsliderFlag;
	bool m_skyst_toonbaseSsliderFlag;
	bool m_skyst_toonbaseVsliderFlag;
	bool m_skyst_toonbaseAsliderFlag;
	bool m_skyst_toonhiHsliderFlag;
	bool m_skyst_toonhiSsliderFlag;
	bool m_skyst_toonhiVsliderFlag;
	bool m_skyst_toonhiAsliderFlag;
	bool m_skyst_toonlowHsliderFlag;
	bool m_skyst_toonlowSsliderFlag;
	bool m_skyst_toonlowVsliderFlag;
	bool m_skyst_toonlowAsliderFlag;
	bool m_skyst_tilingUsliderFlag;
	bool m_skyst_tilingVsliderFlag;
	bool m_skyst_tilingUsliderUpFlag;
	bool m_skyst_tilingVsliderUpFlag;
	bool m_skyst_alphatestesliderFlag;
	bool m_skyst_distortionchkFlag;
	bool m_skyst_distortionscalesliderFlag;
	bool m_skyst_riverradioFlag;
	bool m_skyst_seacenterUsliderFlag;
	bool m_skyst_seacenterVsliderFlag;
	bool m_skyst_riverdirUsliderFlag;
	bool m_skyst_riverdirVsliderFlag;
	bool m_skyst_flowratesliderFlag;
	bool m_skyst_distortionmapradioFlag;



	OrgWinGUI::OWP_Separator* m_skyst_slotsp1_0;
	OrgWinGUI::OWP_Separator* m_skyst_slotsp1_1;
	OrgWinGUI::OWP_Separator* m_skyst_slotsp1_2;
	OrgWinGUI::OWP_Separator* m_skyst_slotsp2_0;
	OrgWinGUI::OWP_Separator* m_skyst_slotsp2_1;
	OrgWinGUI::OWP_Separator* m_skyst_slotsp2_2;
	OrgWinGUI::OWP_Button* m_skyst_slotB[8];

	int m_skyst_slotindex;
	bool m_skyst_slotFlag;

};

#endif //__ColiIDDlg_H_
