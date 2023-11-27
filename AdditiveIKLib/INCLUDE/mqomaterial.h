#ifndef MQOMATERIALH
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

class ConstantBuffer;//定数バッファ。
class RootSignature;//ルートシグネチャ。


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

	void InitShadersAndPipelines(
		int vertextype,
		const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* vsSkinEntryPointFunc,
		const char* psEntryPointFunc,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		int numSrv,
		int numCbv,
		UINT offsetInDescriptorsFromTableStartCB,
		UINT offsetInDescriptorsFromTableStartSRV,
		D3D12_FILTER samplerFilter);
	void InitPipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat);
	void InitShaders(const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* vsSkinEntriyPointFunc,
		const char* psEntryPointFunc
	);
	void BeginRender(RenderContext& rc, int hasSkin, bool isline);


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
		return m_shader;
	};
	void SetShader( int srcval ){
		m_shader = srcval;
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

	int GetTexID(){
		return m_texid;
	};
	void SetTexID( int srcval ){
		m_texid = srcval;
	};

	Texture& GetDiffuseMap();
	Texture& GetAlbedoMap();
	Texture& GetNormalMap();
	Texture& GetSpecularMap();

private:
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

	int m_vcolflag;
////

	int m_shader;

////
	ChaVector4 m_dif4f;
	ChaVector3 m_amb3f;
	ChaVector3 m_emi3f;
	ChaVector3 m_spc3f;
	ChaVector4 m_sceneamb;


	int m_transparent;
	int m_texrule;
	int m_blendmode;

	float m_uanime;
	float m_vanime;

	int m_texid;
	Texture m_whitetex;
	Texture m_blacktex;
	Texture m_diffuseMap;
	Texture* m_albedoMap;//bank管理の外部ポインタ
	Texture* m_normalMap;//とりあえずnulltexture このクラスで作成するポインタ
	Texture* m_specularMap;//とりあえずnulltexture このクラスで作成するポインタ

	ConstantBuffer m_constantBuffer;				//定数バッファ。
	RootSignature m_rootSignature;					//ルートシグネチャ。
	PipelineState m_nonSkinModelPipelineState;		//スキンなしモデル用のパイプラインステート。
	PipelineState m_skinModelPipelineState;			//スキンありモデル用のパイプラインステート。
	PipelineState m_transSkinModelPipelineState;	//スキンありモデル用のパイプラインステート(半透明マテリアル)。
	PipelineState m_transNonSkinModelPipelineState;	//スキンなしモデル用のパイプラインステート(半透明マテリアル)。
	Shader* m_vsNonSkinModel = nullptr;				//スキンなしモデル用の頂点シェーダー。
	Shader* m_vsSkinModel = nullptr;				//スキンありモデル用の頂点シェーダー。
	Shader* m_psModel = nullptr;					//モデル用のピクセルシェーダー。


//以下、クラス外からアクセスしないのでアクセッサー無し。
	char* m_curtexname;

	char m_alphatest;
	unsigned char m_alphaval;

	float m_glowmult[3];
	float m_orgalpha;


	int m_convnamenum;
	char** m_ppconvname;

};


#endif