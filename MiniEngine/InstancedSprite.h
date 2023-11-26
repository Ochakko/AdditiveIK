#pragma once

#include "Indexbuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

#include <ChaVecCalc.h>

#include "Sprite.h"

class Texture;

//スプライトに設定できる最大テクスチャ数。
//const int MAX_TEXTURE = 32;
//const int MAX_TEXTURE = 4;

////拡張SRVが設定されるレジスタの開始番号。
//const int EXPAND_SRV_REG__START_NO = 10;

class IShaderResource;

///// <summary>
///// アルファブレンディングモード
///// </summary>
//enum AlphaBlendMode {
//	AlphaBlendMode_None,	//アルファブレンディングなし(上書き)。
//	AlphaBlendMode_Trans,	//半透明合成
//	AlphaBlendMode_Add,		//加算合成
//};
///// <summary>
///// スプライトの初期化データ。
///// </summary>
//struct SpriteInitData {
//	std::array<const char*, MAX_TEXTURE> m_ddsFilePath= {nullptr};	// DDSファイルのファイルパス。
//	std::array<const char*, MAX_TEXTURE> m_wicFilePath = { nullptr };	// WICファイルのファイルパス。
//	std::array<Texture*, MAX_TEXTURE> m_textures = { nullptr };		// 使用するテクスチャ。DDSファイルのパスが指定されている場合は、このパラメータは無視されます。
//	const char* m_vsEntryPointFunc = "VSMain";						// 頂点シェーダーのエントリーポイント。
//	const char* m_psEntryPoinFunc = "PSMain";						// ピクセルシェーダーのエントリーポイント。
//	const char* m_fxFilePath = nullptr;								// .fxファイルのファイルパス。
//	UINT m_width = 0;												// スプライトの幅。
//	UINT m_height = 0;												// スプライトの高さ。
//	void* m_expandConstantBuffer = nullptr;							// ユーザー拡張の定数バッファ
//	int m_expandConstantBufferSize = 0;								// ユーザー拡張の定数バッファのサイズ。
//	IShaderResource* m_expandShaderResoruceView = nullptr;			// ユーザー拡張のシェーダーリソース。
//	AlphaBlendMode m_alphaBlendMode = AlphaBlendMode_None;			// アルファブレンディングモード。
//	D3D12_FILTER m_samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// サンプラのフィルター。
//	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> m_colorBufferFormat = { 
//		DXGI_FORMAT_R8G8B8A8_UNORM,
//		DXGI_FORMAT_B8G8R8A8_UNORM,
//		DXGI_FORMAT_B8G8R8X8_UNORM,
//		DXGI_FORMAT_UNKNOWN,
//		DXGI_FORMAT_UNKNOWN,
//		DXGI_FORMAT_UNKNOWN,
//		DXGI_FORMAT_UNKNOWN,
//		DXGI_FORMAT_UNKNOWN,
//	};	//レンダリングするカラーバッファのフォーマット。
//};


#define INSTANCEDSPMAX	MAXBONENUM

typedef struct tag_sppossizecol {
	ChaVector4 pos;
	ChaVector2 size;
	ChaVector2 WH;
	ChaVector4 colmult;
	void Init()
	{
		pos = ChaVector4(0.0f, 0.0f, 0.0f, 1.0f);
		size = ChaVector2(1.0f, 1.0f);
		WH.x = 800.0f;
		WH.y = 600.0f;
		colmult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
	};
	tag_sppossizecol() {
		Init();
	};
}SPPOSSIZECOL;


/// <summary>
/// スプライトクラス。
/// </summary>
class InstancedSprite  {
public:
	static const Vector2	DEFAULT_PIVOT;					//!<ピボット。
	virtual ~InstancedSprite();
	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="initData">初期化データ</param>
	void Init(const SpriteInitData& initData);
	/// <summary>
	/// 更新。
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率</param>
	/// <param name="pivot">
	/// ピボット
	/// 0.5, 0.5で画像の中心が基点。
	/// 0.0, 0.0で画像の左下。
	/// 1.0, 1.0で画像の右上。
	/// UnityのuGUIに準拠。
	/// </param>
	//void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot = DEFAULT_PIVOT);
	

	//2023/11/20
	/// <summary>
	/// 更新。m_screenvertexflag == trueの場合用のUpdate
	/// </summary>
	void UpdateScreen(int instanceno, 
		ChaVector3 srcpos, ChaVector2 srcdispsize, ChaVector4 srccolmult);

	
	/// <summary>
	/// 描画。
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト/param>
	//void Draw(RenderContext& renderContext);

	/// <summary>
	/// 描画。m_screenvertexflag == trueの場合用の描画
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト/param>
	void DrawScreen(RenderContext& renderContext);

private:
	/// <summary>
	/// テクスチャを初期化。
	/// </summary>
	/// <param name="initData"></param>
	void InitTextures(const SpriteInitData& initData);
	/// <summary>
	/// シェーダーを初期化。
	/// </summary>
	/// <param name="initData"></param>
	void InitShader( const SpriteInitData& initData );
	/// <summary>
	/// ディスクリプタヒープを初期化。
	/// </summary>
	/// <param name="initData"></param>
	void InitDescriptorHeap(const SpriteInitData& initData);
	/// <summary>
	/// 頂点バッファとインデックスバッファを初期化。
	/// </summary>
	/// <param name="initData"></param>
	void InitVertexBufferAndIndexBuffer(const SpriteInitData& initData);
	/// <summary>
	/// パイプラインステートを初期化する。
	/// </summary>
	void InitPipelineState(const SpriteInitData& initData);
	/// <summary>
	/// 定数バッファを初期化。
	/// </summary>
	/// <param name="initData"></param>
	void InitConstantBuffer(const SpriteInitData& initData);
private:
	int m_instancenum = 0;

	IndexBuffer m_indexBuffer;			//インデックスバッファ。
	VertexBuffer m_vertexBuffer;		//頂点バッファ。
	VertexBuffer m_possizeBuffer;			//インスタンシング頂点バッファ　スプライト変換行列
	//int m_numTexture = 0;				//テクスチャの枚数。
	//Texture m_textures[MAX_TEXTURE];	//テクスチャ。
	//Texture* m_textureExternal[MAX_TEXTURE] = {nullptr};	//外部から指定されたテクスチャ
	Texture* m_textureExternal = nullptr;	//外部から指定されたテクスチャ
	//Vector3 m_position[INSTANCEDSPMAX];				//座標。
	Vector2 m_size;						//読込画像サイズ。
	SPPOSSIZECOL m_possize[INSTANCEDSPMAX]; //表示位置と表示サイズ
	Quaternion m_rotation[INSTANCEDSPMAX];			//回転。
	Matrix m_world;					//ワールド行列。

	struct LocalConstantBuffer {
		Matrix mvp;
		Vector4 mulColor;
		Vector4 screenParam;
	};
	LocalConstantBuffer m_constantBufferCPU;	//CPU側の定数バッファ。
	ConstantBuffer		m_constantBufferGPU;	//GPU側の定数バッファ。
	ConstantBuffer		m_userExpandConstantBufferGPU;	//ユーザー拡張の定数バッファ(GPU側)
	void* m_userExpandConstantBufferCPU = nullptr;		//ユーザー拡張の定数バッファ(CPU側)
	DescriptorHeap		m_descriptorHeap;		//ディスクリプタヒープ。
	RootSignature		m_rootSignature;		//ルートシグネチャ。
	PipelineState		m_pipelineState;		//パイプラインステート。
	Shader				m_vs;					//頂点シェーダー。
	Shader				m_ps;					//ピクセルシェーダー。

	//bool m_screenvertexflag;//頂点バッファの座標値を-1.0から1.0にする. m_sizeを掛けない. GUI用のテクスチャ表示に使用

};