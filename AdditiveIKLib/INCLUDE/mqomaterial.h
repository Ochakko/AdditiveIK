﻿#ifndef MQOMATERIALH
#define MQOMATERIALH

#include <stdio.h>
#include <stdarg.h>

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>

#define DBGH
#include <dbg.h>

#include <wchar.h>

#include "../../MiniEngine/Texture.h"
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"

class ConstantBuffer;//定数バッファ。
class RootSignature;//ルートシグネチャ。

enum {
	MQOSHADER_PBR,
	MQOSHADER_STD,
	MQOSHADER_NOLIGHT,

	MQOSHADER_PBR_SHADOWMAP,
	MQOSHADER_PBR_SHADOWRECIEVER,

	MQOSHADER_STD_SHADOWMAP,
	MQOSHADER_STD_SHADOWRECIEVER,

	MQOSHADER_NOLIGHT_SHADOWMAP,
	MQOSHADER_NOLIGHT_SHADOWRECIEVER,

	MQOSHADER_MAX
};

enum {
	SHADERFX_NOSKIN_STD,
	SHADERFX_NOSKIN_PBR,
	SHADERFX_NOSKIN_ZPRE,
	SHADERFX_SKIN_STD,
	SHADERFX_SKIN_PBR,
	SHADERFX_SKIN_ZPRE,
	SHADERFX_MAX
};

enum {//renderobj.renderkind
	RENDERKIND_NORMAL,
	RENDERKIND_SHADOWMAP,
	RENDERKIND_SHADOWRECIEVER,
	RENDERKIND_ZPREPASS,
	RENDERKIND_MAX
};

struct SConstantBuffer {
	Matrix mWorld;		//ワールド行列。
	Matrix mView;		//ビュー行列。
	Matrix mProj;		//プロジェクション行列。
	ChaVector4 diffusemult;
	ChaVector4 ambient;
	ChaVector4 emission;
	ChaVector4 metalcoef;
	ChaVector4 materialdisprate;
	ChaVector4 shadowmaxz;
	int UVs[4];
	void Init() {
		mWorld.SetIdentity();
		mView.SetIdentity();
		mProj.SetIdentity();
		diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
		ambient = ChaVector4(0.2f, 0.2f, 0.2f, 1.0f);
		emission = ChaVector4(0.0f, 0.0f, 0.0f, 1.0f);
		metalcoef = ChaVector4(0.250f, 0.250f, 0.0f, 0.0f);
		materialdisprate = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
		shadowmaxz = ChaVector4(3000.0f, 0.0010f, 0.0f, 0.0f);
		ZeroMemory(UVs, sizeof(int) * 4);
	};
};

struct DirectionalLight
{
	ChaVector4 direction; // ライトの方向
	ChaVector4 color; // ライトの色
	void Init() {
		direction.SetZeroVec4(0.0f);
		color.SetZeroVec4(1.0f);
	};
};

#define NUM_DIRECTIONAL_LIGHT 8
struct SConstantBufferLights {
	int lightsnum[4];
	DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
	ChaVector4 eyePos; // カメラの視点
	ChaVector4 specPow; // スペキュラの絞り
	ChaVector4 ambientLight; // 環境光
	void Init() {
		//ZeroMemory(&lightsnum, sizeof(int) * 4);
		lightsnum[0] = 1;
		lightsnum[1] = 1;
		lightsnum[2] = 1;
		lightsnum[3] = 1;
		int lindex;
		for (lindex = 0; lindex < NUM_DIRECTIONAL_LIGHT; lindex++) {
			directionalLight[lindex].Init();
		}
		eyePos.SetZeroVec4(1.0f);
		specPow.SetZeroVec4(0.0f);
		ambientLight.SetZeroVec4(1.0f);
	};
};

struct SConstantBufferBoneMatrix {
	SConstantBufferLights lights;
	float setfl4x4[16 * MAXBONENUM];//ボーンの姿勢マトリックス
	void Init() {
		lights.Init();
		ZeroMemory(setfl4x4, sizeof(float) * 16 * MAXBONENUM);
	};
};

struct SConstantBufferShadow {
	float mLVP[16]; // ライトビュープロジェクション行列
	ChaVector4 lightPos; // ライトの座標
	void Init() {
		//mLVP.SetIdentity();
		ZeroMemory(mLVP, sizeof(float) * 16);
		lightPos.SetZeroVec4(1.0f);
	};
};


class CMQOMaterial
{
public:
	CMQOMaterial();
	~CMQOMaterial();

	int SetParams( int srcno, ChaVector4 srcsceneamb, char* srcchar, int srcleng );


	int SetName( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetCol( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetDif( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetAmb( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetEmi( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetSpc( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetPower( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetTex( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetAlpha( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetBump( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetShader( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetVcolFlag( char* srcchar, int pos, int srcleng, int* stepnum );

	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );


	int Dump();

	int IsSame( CMQOMaterial* compmat, int compnameflag, int cmplevel = 0 );//compnameflag == 0 の時は、名前は比較しない。

	//int GetName( int srcmatno, char** nameptr );
	int AddConvName( char** ppname );

	//int SetCurTexname( int srcenabletex );

	//int SetExtTex( int srctexnum, char** srctexname );
	//int SetConvName( int srcconvnum, char** srcconvname );

	int GetColorrefDiffuse( COLORREF* dstdiffuse );
	int GetColorrefSpecular( COLORREF* dstspecular );
	int GetColorrefAmbient( COLORREF* dstambient );
	int GetColorrefEmissive( COLORREF* dstemissive );

	//int CreateTexture( WCHAR* dirname, int texpool = 0 );
	int CreateTexture(WCHAR* dirname, int texpool = 0);//!!!!!!!!!!!!!!!!!!!!!!!!

	int CreateDecl(ID3D12Device* pdev, int objecttype);
	void CreateDescriptorHeaps(int objecttype);

	void InitShadersAndPipelines(
		int srcuvnum,
		int vertextype,
		const char* fxPBRPath,
		const char* fxStdPath,
		const char* fxNoLightPath,
	
		const char* vsPBRFunc,
		const char* vsPBRShadowMapFunc,
		const char* vsPBRShadowRecieverFunc,

		const char* vsStdFunc,
		const char* vsStdShadowMapFunc,
		const char* vsStdShadowRecieverFunc,

		const char* vsNoLightFunc,
		const char* vsNoLightShadowMapFunc,
		const char* vsNoLightShadowRecieverFunc,

		const char* psPBRFunc,
		const char* psPBRShadowMapFunc,
		const char* psPBRShadowRecieverFunc,

		const char* psStdFunc,
		const char* psStdShadowMapFunc,
		const char* psStdShadowRecieverFunc,

		const char* psNoLightFunc,
		const char* psNoLightShadowMapFunc,
		const char* psNoLightShadowRecieverFunc,
		
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		int numSrv,
		int numCbv,
		UINT offsetInDescriptorsFromTableStartCB,
		UINT offsetInDescriptorsFromTableStartSRV,
		D3D12_FILTER samplerFilter);

	void InitPipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat);
	void InitShaders(
		const char* fxPBRPath,
		const char* fxStdPath,
		const char* fxNoLightPath,

		const char* vsPBRFunc,
		const char* vsPBRShadowMapFunc,
		const char* vsPBRShadowRecieverFunc,

		const char* vsStdFunc,
		const char* vsStdShadowMapFunc,
		const char* vsStdShadowRecieverFunc,

		const char* vsNoLightFunc,
		const char* vsNoLightShadowMapFunc,
		const char* vsNoLightShadowRecieverFunc,

		const char* psPBRFunc,
		const char* psPBRShadowMapFunc,
		const char* psPBRShadowRecieverFunc,

		const char* psStdFunc,
		const char* psStdShadowMapFunc,
		const char* psStdShadowRecieverFunc,

		const char* psNoLightFunc,
		const char* psNoLightShadowMapFunc,
		const char* psNoLightShadowRecieverFunc
	);


	void InitZPreShadersAndPipelines(
		int vertextype,
		const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* psEntryPointFunc,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		int numSrv,
		int numCbv,
		UINT offsetInDescriptorsFromTableStartCB,
		UINT offsetInDescriptorsFromTableStartSRV,
		D3D12_FILTER samplerFilter);
	void InitZPrePipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat);
	void InitZPreShaders(const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* psEntryPointFunc
	);


	void SetFl4x4(myRenderer::RENDEROBJ renderobj);
	void SetConstLights(SConstantBufferLights* pcbLights);
	void SetConstShadow(SConstantBufferShadow* pcbShadow);
	void DrawCommon(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
		const Matrix& mView, const Matrix& mProj,
		bool isfirstmaterial  = false);
	void BeginRender(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	void ZPreDrawCommon(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
		const Matrix& mView, const Matrix& mProj,
		bool isfirstmaterial = false);
	void ZPreBeginRender(RenderContext* rc);



	void SetBoneMatrix(myRenderer::RENDEROBJ renderobj);
	//void SetBoneMatrixReq(CBone* srcbone, myRenderer::RENDEROBJ renderobj);

private:
	int InitParams();
	int DestroyObjs();

	int ConvParamsTo3F();
	int SetWhiteTexture();
	int SetBlackTexture();
	int SetDiffuseTexture();

public:
	//accesser
	int GetMaterialNo(){
		return m_materialno;
	};
	void SetMaterialNo( int srcval ){
		m_materialno = srcval;
	};

	const char* GetName(){
		return m_name;
	};
	void SetName( const char* srcname ){
		strcpy_s( m_name, 256, srcname );
	};

	ChaVector4 GetCol(){
		return m_col;
	};
	void SetCol( ChaVector4 srcval ){
		m_col = srcval;
	};

	float GetDif(){
		return m_dif;
	};
	void SetDif( float srcval ){
		m_dif = srcval;
	};

	float GetAmb(){
		return m_amb;
	};
	void SetAmb( float srcval ){
		m_amb = srcval;
	};

	float GetEmi(){
		return m_emi;
	};
	void SetEmi( float srcval ){
		m_emi = srcval;
	};

	float GetSpc(){
		return m_spc;
	};
	void SetSpc( float srcval ){
		m_spc = srcval;
	};

	float GetPower(){
		return m_power;
	};
	void SetPower( float srcval ){
		m_power = srcval;
	};

	int GetTexNum(){
		return m_texnum;
	};
	void SetTexNum( int srcval ){
		m_texnum = srcval;
	};

	const char* GetTex(){
		return m_tex;
	};
	void SetTex(const char* srcname ){
		strcpy_s( m_tex, 256, srcname );
	};
	void Add2Tex(const char* srcname ){
		strcat_s( m_tex, 256, srcname );
	};

	const char* GetAlpha(){
		return m_alpha;
	};
	void SetAlpha(const char* srcname ){
		strcpy_s( m_alpha, 256, srcname );
	};

	const char* GetBump(){
		return m_bump;
	};
	void SetBump(const char* srcname ){
		strcpy_s( m_bump, 256, srcname );
	};




	int GetVcolFlag(){
		return m_vcolflag;
	};
	void SetVcolFlag( int srcval ){
		m_vcolflag = srcval;
	};

	int GetShader(){
		return m_shader;//mqofileのshader
	};
	void SetShader( int srcval ){
		m_shader = srcval;//mqofileのshader
	};

	int GetShaderType() {
		return m_shadertype;//DirectX12描画用のshader
	};
	void SetShaderType(int srcval) {
		m_shadertype = srcval;//DirectX12描画用のshader
	};
	float GetMetalCoef() {
		return m_metalcoef;
	};
	void SetMetalCoef(float srcval) {
		m_metalcoef = srcval;
	};
	float GetSmoothCoef() {
		return m_smoothcoef;
	};
	void SetSmoothCoef(float srcval) {
		m_smoothcoef = srcval;
	};
	float GetLightScale(int srcindex) {
		if ((srcindex >= 0) && (srcindex < LIGHTNUMMAX)) {
			return m_lightscale[srcindex];
		}
		else {
			_ASSERT(0);
			return 0.0f;
		}
	};
	void SetLightScale(int srcindex, float srcval) {
		if ((srcindex >= 0) && (srcindex < LIGHTNUMMAX)) {
			m_lightscale[srcindex] = srcval;
		}
		else {
			_ASSERT(0);
		}
	};


	ChaVector4 GetDif4F(){
		return m_dif4f;
	};
	void SetDif4F( ChaVector4 srcval ){
		m_dif4f = srcval;
	};
	void SetDif4FW( float srcval ){
		m_dif4f.w = srcval;
	};

	ChaVector3 GetAmb3F(){
		return m_amb3f;
	};
	void SetAmb3F( ChaVector3 srcval ){
		m_amb3f = srcval;
	};

	ChaVector3 GetEmi3F(){
		return m_emi3f;
	};
	void SetEmi3F( ChaVector3 srcval ){
		m_emi3f = srcval;
	};

	ChaVector3 GetSpc3F(){
		return m_spc3f;
	};
	void SetSpc3F( ChaVector3 srcval ){
		m_spc3f = srcval;
	};

	ChaVector4 GetSceneAmb(){
		return m_sceneamb;
	};
	void SetSceneAmb( ChaVector4 srcval ){
		m_sceneamb = srcval;
	};
	
	int GetTransparent(){
		return m_transparent;
	};
	void SetTransparent( int srcval ){
		m_transparent = srcval;
	};

	int GetTexRule(){
		return m_texrule;
	};
	void SetTexRule( int srcval ){
		m_texrule = srcval;
	};

	int GetBlendMode(){
		return m_blendmode;
	};
	void SetBlendMode( int srcval ){
		m_blendmode = srcval;
	};

	float GetUAnime(){
		return m_uanime;
	};
	void SetUAnime( float srcval ){
		m_uanime = srcval;
	};

	float GetVAnime(){
		return m_vanime;
	};
	void SetVAnime( float srcval ){
		m_vanime = srcval;
	};

	const char* GetCurTexName(){
		return m_curtexname;
	};

	//#######
	//albedo
	//#######
	const char* GetAlbedoTex() {
		return m_albedotex;
	};
	void SetAlbedoTex(const char* srcname) {
		if (srcname && srcname[0]) {
			strcpy_s(m_albedotex, 256, srcname);
		}
		else {
			_ASSERT(0);
			m_albedotex[0] = 0;
		}
	};
	int GetAlbedoTexID(){
		return m_albedotexid;
	};
	void SetAlbedoTexID( int srcval ){
		m_albedotexid = srcval;
	};

	//#######
	//normal
	//#######
	const char* GetNormalTex() {
		return m_normaltex;
	};
	void SetNormalTex(const char* srcname) {
		if (srcname && srcname[0]) {
			strcpy_s(m_normaltex, 256, srcname);
		}
		else {
			_ASSERT(0);
			m_normaltex[0] = 0;
		}
	};
	int GetNormalTexID() {
		return m_normaltexid;
	};
	void SetNormalTexID(int srcval) {
		m_normaltexid = srcval;
	};

	//#######
	//metal
	//#######
	const char* GetMetalTex() {
		return m_metaltex;
	};
	void SetMetalTex(const char* srcname) {
		if (srcname && srcname[0]) {
			strcpy_s(m_metaltex, 256, srcname);
		}
		else {
			_ASSERT(0);
			m_metaltex[0] = 0;
		}
	};
	int GetMetalTexID() {
		return m_metaltexid;
	};
	void SetMetalTexID(int srcval) {
		m_metaltexid = srcval;
	};


	Texture& GetDiffuseMap();
	Texture& GetAlbedoMap();
	Texture& GetNormalMap();
	Texture& GetMetalMap();

	void ResetUpdateFl4x4Flag()//パイプライン毎
	{
		int fxno;
		for (fxno = 0; fxno < (MQOSHADER_MAX * 3); fxno++) {
			m_updatefl4x4flag[fxno] = false;
		}
	}
	void SetUpdateFl4x4Flag(int srcindex)//パイプライン毎
	{
		if ((srcindex >= 0) && (srcindex < (MQOSHADER_MAX * 3))) {
			m_updatefl4x4flag[srcindex] = true;
		}
		else {
			_ASSERT(0);
		}
	}
	bool GetUpdateFl4x4Flag(int srcindex)//パイプライン毎
	{
		if ((srcindex >= 0) && (srcindex < (MQOSHADER_MAX * 3))) {
			return m_updatefl4x4flag[srcindex];
		}
		else {
			_ASSERT(0);
			return false;
		}
	}
	void ResetUpdateLightsFlag()//パイプライン毎
	{
		int fxno;
		for (fxno = 0; fxno < (MQOSHADER_MAX * 3); fxno++) {
			m_updatelightsflag[fxno] = false;
		}
	}
	void SetUpdateLightsFlag(int srcindex)//パイプライン毎
	{
		if ((srcindex >= 0) && (srcindex < (MQOSHADER_MAX * 3))) {
			m_updatelightsflag[srcindex] = true;
		}
		else {
			_ASSERT(0);
		}
	}
	bool GetUpdateLightsFlag(int srcindex)//パイプライン毎
	{
		if ((srcindex >= 0) && (srcindex < (MQOSHADER_MAX * 3))) {
			return m_updatelightsflag[srcindex];
		}
		else {
			_ASSERT(0);
			return false;
		}
	}


	void SetTempDiffuseMult(ChaVector4 srcmult)
	{
		m_tempdiffusemult = srcmult;
	}
	ChaVector4 GetTempDiffuseMult()
	{
		return m_tempdiffusemult;
	}
	void SetTempDiffuseMultFlag(bool srcflag)
	{
		m_settempdiffusemult = srcflag;
	}
	bool GetTempDiffuseMultFlag()
	{
		return m_settempdiffusemult;
	}

	void SetEnableEmission(bool srcflag) 
	{
		m_enableEmission = srcflag;
	}
	bool GetEnableEmission()
	{
		return m_enableEmission;
	}


	void SetAddressU_albedo(FbxTexture::EWrapMode srcmode) {//eClamp or eRepeat
		if (srcmode == FbxTexture::EWrapMode::eClamp) {
			m_addressU_albedo = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
		else {
			m_addressU_albedo = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}
	D3D12_TEXTURE_ADDRESS_MODE GetAddressU_albedo()
	{
		return m_addressU_albedo;
	}
	void SetAddressV_albedo(FbxTexture::EWrapMode srcmode) {//eClamp or eRepeat
		if (srcmode == FbxTexture::EWrapMode::eClamp) {
			m_addressV_albedo = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
		else {
			m_addressV_albedo = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}
	D3D12_TEXTURE_ADDRESS_MODE GetAddressV_albedo()
	{
		return m_addressV_albedo;
	}


	void SetAddressU_normal(FbxTexture::EWrapMode srcmode) {//eClamp or eRepeat
		if (srcmode == FbxTexture::EWrapMode::eClamp) {
			m_addressU_normal = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
		else {
			m_addressU_normal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}
	D3D12_TEXTURE_ADDRESS_MODE GetAddressU_normal()
	{
		return m_addressU_normal;
	}
	void SetAddressV_normal(FbxTexture::EWrapMode srcmode) {//eClamp or eRepeat
		if (srcmode == FbxTexture::EWrapMode::eClamp) {
			m_addressV_normal = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
		else {
			m_addressV_normal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}
	D3D12_TEXTURE_ADDRESS_MODE GetAddressV_normal()
	{
		return m_addressV_normal;
	}


	void SetAddressU_metal(FbxTexture::EWrapMode srcmode) {//eClamp or eRepeat

		if (srcmode == FbxTexture::EWrapMode::eClamp) {
			m_addressU_metal = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
		else {
			m_addressU_metal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}
	D3D12_TEXTURE_ADDRESS_MODE GetAddressU_metal()
	{
		return m_addressU_metal;
	}
	void SetAddressV_metal(FbxTexture::EWrapMode srcmode) {//eClamp or eRepeat
		if (srcmode == FbxTexture::EWrapMode::eClamp) {
			m_addressV_metal = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
		else {
			m_addressV_metal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}
	D3D12_TEXTURE_ADDRESS_MODE GetAddressV_metal()
	{
		return m_addressV_metal;
	}

public:
	//###################################################
	//CreateDecl()内で　頂点フォーマットによって　定数を設定する
	//###################################################
	//拡張SRVが設定されるレジスタの開始番号。
	//const int EXPAND_SRV_REG__START_NO = 10;
	//const int EXPAND_SRV_REG__START_NO = 6;
	int EXPAND_SRV_REG__START_NO = 5;
	//１つのマテリアルで使用されるSRVの数。
	int NUM_SRV_ONE_MATERIAL = (EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV);
	//１つのマテリアルで使用されるCBVの数。
	//const int NUM_CBV_ONE_MATERIAL = 2;
	int NUM_CBV_ONE_MATERIAL = 1;

private:

	ConstantBuffer m_commonConstantBuffer;					//メッシュ共通の定数バッファ。
	ConstantBuffer m_expandConstantBuffer;					//ユーザー拡張用の定数バッファ
	ConstantBuffer m_expandConstantBuffer2;					//ユーザー拡張用の定数バッファ
	std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV> m_expandShaderResourceView = { nullptr };	//ユーザー拡張シェーダーリソースビュー。
	void* m_expandData = nullptr;
	bool m_createdescriptorflag;
	DescriptorHeap m_descriptorHeap;					//ディスクリプタヒープ。
	
	
	ConstantBuffer m_shadowcommonConstantBuffer;					//メッシュ共通の定数バッファ。
	ConstantBuffer m_shadowexpandConstantBuffer;					//ユーザー拡張用の定数バッファ
	ConstantBuffer m_shadowexpandConstantBuffer2;					//ユーザー拡張用の定数バッファ
	DescriptorHeap m_shadowdescriptorHeap;					//ディスクリプタヒープ。



	int m_materialno;
	char m_name[256];

	ChaVector4 m_col;
	float m_dif;
	float m_amb;
	float m_emi;
	float m_spc;
	float m_power;
	int m_texnum;
	char m_tex[256];
	char m_alpha[256];
	char m_bump[256];

	char m_albedotex[256];
	char m_normaltex[256];
	char m_metaltex[256];


	int m_vcolflag;
////

	int m_shader;//mqofile記述のshader
	int m_shadertype;//DirectX12描画用のshader　//Shaderプレートメニュー用
	float m_metalcoef;//Shaderプレートメニュー用
	float m_smoothcoef;//Shaderプレートメニュー用
	float m_lightscale[LIGHTNUMMAX];//Shaderプレートメニュー用

////
	ChaVector4 m_dif4f;
	ChaVector3 m_amb3f;
	ChaVector3 m_emi3f;
	ChaVector3 m_spc3f;
	ChaVector4 m_sceneamb;
	bool m_enableEmission;

	int m_transparent;
	int m_texrule;
	int m_blendmode;

	float m_uanime;
	float m_vanime;

	int m_albedotexid;
	int m_normaltexid;
	int m_metaltexid;

	Texture m_whitetex;
	Texture m_blacktex;
	Texture m_diffuseMap;
	Texture* m_albedoMap;//bank管理の外部ポインタ
	Texture* m_normalMap;//bank管理の外部ポインタ
	Texture* m_metalMap;//bank管理の外部ポインタ

	D3D12_TEXTURE_ADDRESS_MODE m_addressU_albedo;
	D3D12_TEXTURE_ADDRESS_MODE m_addressV_albedo;
	D3D12_TEXTURE_ADDRESS_MODE m_addressU_normal;
	D3D12_TEXTURE_ADDRESS_MODE m_addressV_normal;
	D3D12_TEXTURE_ADDRESS_MODE m_addressU_metal;
	D3D12_TEXTURE_ADDRESS_MODE m_addressV_metal;



	ConstantBuffer m_constantBuffer;				//定数バッファ。
	RootSignature m_rootSignature;					//ルートシグネチャ。
	RootSignature m_shadowrootSignature;					//ルートシグネチャ。
	PipelineState m_opaquePipelineState[MQOSHADER_MAX];
	PipelineState m_transPipelineState[MQOSHADER_MAX];
	PipelineState m_zalwaysPipelineState[MQOSHADER_MAX];
	Shader* m_vsMQOShader[MQOSHADER_MAX];
	Shader* m_psMQOShader[MQOSHADER_MAX];

	//PipelineState m_nonSkinModelPipelineState;		//スキンなしモデル用のパイプラインステート。
	//PipelineState m_skinModelPipelineState;			//スキンありモデル用のパイプラインステート。
	//PipelineState m_transSkinModelPipelineState;	//スキンありモデル用のパイプラインステート(半透明マテリアル)。
	//PipelineState m_transSkinAlwaysModelPipelineState;	//スキンありモデル用のパイプラインステート(半透明ALWAYSマテリアル)。
	//PipelineState m_transNonSkinModelPipelineState;	//スキンなしモデル用のパイプラインステート(半透明マテリアル)。
	//PipelineState m_transNonSkinAlwaysModelPipelineState;//スキンなしモデル用のパイプラインステート(半透明ALWAYSマテリアル)。
	//Shader* m_vsNonSkinModel = nullptr;				//スキンなしモデル用の頂点シェーダー。
	//Shader* m_vsSkinModel = nullptr;				//スキンありモデル用の頂点シェーダー。
	//Shader* m_psModel = nullptr;					//モデル用のピクセルシェーダー。


	RootSignature m_ZPrerootSignature;					//ZPreルートシグネチャ。
	PipelineState m_ZPreModelPipelineState;		//ZPreモデル用のパイプラインステート。
	Shader* m_vsZPreModel = nullptr;				//ZPreモデル用の頂点シェーダー。
	Shader* m_psZPreModel = nullptr;					//ZPreモデル用のピクセルシェーダー。


	SConstantBuffer m_cb;
	SConstantBufferBoneMatrix m_cbMatrix;
	SConstantBufferLights m_cbLights;
	SConstantBufferShadow m_cbShadow;


	bool m_initpipelineflag = false;
	bool m_initprezpipelineflag = false;

	int m_shaderfx;
	bool m_updatefl4x4flag[MQOSHADER_MAX * 3];
	bool m_updatelightsflag[MQOSHADER_MAX * 3];


//以下、クラス外からアクセスしないのでアクセッサー無し。
	char* m_curtexname;

	char m_alphatest;
	unsigned char m_alphaval;

	float m_glowmult[3];
	float m_orgalpha;


	int m_convnamenum;
	char** m_ppconvname;

	bool m_settempdiffusemult;
	ChaVector4 m_tempdiffusemult;

};


#endif