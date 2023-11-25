#ifndef DISPOBJH
#define DISPOBJH

#include <ChaVecCalc.h>
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"


class CMQOObject;
class CMQOMaterial;
class CPolyMesh3;
class CPolyMesh4;
class CExtLine;


class ConstantBuffer;//メッシュ共通の定数バッファ。
class StructuredBuffer;//ボーン行列の構造化バッファ。
class DescriptorHeap;//ディスクリプタヒープ。


struct SConstantBuffer {
	Matrix mWorld;		//ワールド行列。
	Matrix mView;		//ビュー行列。
	Matrix mProj;		//プロジェクション行列。
	ChaVector4 diffusemult;
	float setfl4x4[16 * MAXCLUSTERNUM];//ボーンの姿勢マトリックス
};



class CDispObj
{
public:

/**
 * @fn
 * CDispObj
 * @breaf コンストラクタ。
 * @return なし。
 */
	CDispObj();

/**
 * @fn
 * ~CDispObj
 * @breaf デストラクタ。
 * @return なし。
 */
	~CDispObj();

/**
 * @fn
 * CreateDispObj
 * @breaf メタセコイアからのデータ用の表示用オブジェクトを作成する。
 * @param (ID3D12Device* pdev) IN Direct3Dのデバイス。
 * @param (CPolyMesh3* pm3) IN 形状データへのポインタ。
 * @param (int hasbone) IN ボーン変形有の場合は１、無しの場合は０。MameBake3Dではメタセコイアデータはボーン無し。
 * @return 成功したら０。
 */
	int CreateDispObj( ID3D12Device* pdev, CPolyMesh3* pm3, int hasbone );


/**
 * @fn
 * CreateDispObj
 * @breaf FBXからのデータ用の表示用オブジェクトを作成する。
 * @param (ID3D12Device* pdev) IN Direct3Dのデバイス。
 * @param (CPolyMesh4* pm4) IN 形状データへのポインタ。
 * @param (int hasbone) IN ボーン変形有の場合は１、無しの場合は０。 * @return 戻り値の説明
 * @return 成功したら０。
 */
	int CreateDispObj( ID3D12Device* pdev, CPolyMesh4* pm4, int hasbone );

/**
 * @fn
 * CreateDispObj
 * @breaf 線分データ用の表示用オブジェクトを作成する。
 * @param (ID3D12Device* pdev) IN Direct3Dのデバイス。
 * @param (CExtLine* extline) IN 線の形状データへのポインタ。
 * @return 成功したら０。
 */
	int CreateDispObj( ID3D12Device* pdev, CExtLine* extline );


/**
 * @fn
 * RenderNormal
 * @breaf 通常のベーシックな方法で３D描画をする。
 * @param (CMQOMaterial* rmaterial) IN 材質へのポインタ。
 * @param (int lightflag) IN 照光処理するときは１、しないときは０。
 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
 * @return 成功したら０。
 */
	int RenderNormal(RenderContext& rc, myRenderer::RENDEROBJ renderobj);

	int RenderNormalMaterial(RenderContext& rc, myRenderer::RENDEROBJ renderobj, 
		bool laterflag, CMQOMaterial* rmaterial, int curoffset, int curtrinum);

	void DrawCommon(RenderContext& rc, myRenderer::RENDEROBJ renderobj,
		const Matrix& mView, const Matrix& mProj);

/**
 * @fn
 * RenderNormalPM3
 * @breaf 通常のベーシックな方法でメタセコイアデータを３D表示する。
 * @param (int lightflag) IN 照光処理するときは１、しないときは０。
 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
 * @return 成功したら０。
 * @detail FBXデータは１オブジェクトにつき１マテリアル(材質)だが、メタセコイアデータは１オブジェクトに複数マテリアルが設定されていることが多い。
 */
	int RenderNormalPM3(RenderContext& rc, myRenderer::RENDEROBJ renderobj);

	int RenderNormalPM3Material(RenderContext& rc, myRenderer::RENDEROBJ renderobj,
		bool lasterflag, CMQOMaterial* rmaterial,
		int curoffset, int curtrinum);


/**
 * @fn
 * RenderLine
 * @breaf 線分データを表示する。
 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
 * @return 成功したら０。
 */
	int RenderLine(bool withalpha,
		RenderContext* pRenderContext, 
		ChaVector4 diffusemult, ChaVector4 materialdisprate);

/**
 * @fn
 * CopyDispV
 * @breaf FBXの頂点データを表示バッファにコピーする。
 * @param (CPolyMesh4* pm4) IN FBXの形状データ。
 * @return 成功したら０。
 */
	int CopyDispV( CPolyMesh4* pm4 );

/**
 * @fn
 * CopyDispV
 * @breaf メタセコイアの頂点データを表示バッファにコピーする。
 * @param (CPolyMesh3* pm3) IN メタセコイアの形状データ。
 * @param (引数名) 引数の説明
 * @return 成功したら０。
 */
	int CopyDispV( CPolyMesh3* pm3 );


	void SetScale(ChaVector3 srcscale, ChaVector3 srcoffset)
	{
		m_scale = srcscale;
		m_scaleoffset = srcoffset;
	};

	//void UpdateBoneMatrix(int srcdatanum, void* srcdata);

private:

/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
 * @return ０。
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０。
 */
	int DestroyObjs();

/**
 * @fn
 * CreateDecl
 * @breaf 表示用頂点データのフォーマット(宣言)を作成する。
 * @return 成功したら０。
 */
	int CreateDecl(ID3D12Device* pdev);

/**
 * @fn
 * CreateVBandIB
 * @breaf ３D表示用の頂点バッファとインデックスバッファを作成する。
 * @return 成功したら０。
 */
	int CreateVBandIB(ID3D12Device* pdev);

/**
 * @fn
 * CreateVBandIBLine
 * @breaf 線分用の頂点バッファとインデックスバッファを作成する。
 * @return 成功したら０。
 */
	int CreateVBandIBLine(ID3D12Device* pdev);


	void CreateDescriptorHeaps();

private:
	//拡張SRVが設定されるレジスタの開始番号。
	//const int EXPAND_SRV_REG__START_NO = 10;
	const int EXPAND_SRV_REG__START_NO = 6;
	//１つのマテリアルで使用されるSRVの数。
	const int NUM_SRV_ONE_MATERIAL = EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV;
	//１つのマテリアルで使用されるCBVの数。
	const int NUM_CBV_ONE_MATERIAL = 2;

	int m_hasbone;//ボーン変形用のオブジェクトであるとき１、それ以外の時は０。

	ID3D12Device* m_pdev;//外部メモリ、Direct3Dのデバイス。
	CPolyMesh3* m_pm3;//外部メモリ、メタセコイアファイルから作成した３Dデータ。
	CPolyMesh4* m_pm4;//外部メモリ、FBXファイルから作成した３Dデータ。
	CExtLine* m_extline;//外部メモリ、線データ。

	//D3D11_BUFFER_DESC m_BufferDescBone;
	//D3D11_BUFFER_DESC m_BufferDescNoBone;
	//D3D11_BUFFER_DESC m_BufferDescInf;
	//D3D11_BUFFER_DESC m_BufferDescLine;

	//ID3D11InputLayout* m_layoutBoneL0;
	//ID3D11InputLayout* m_layoutBoneL1;
	//ID3D11InputLayout* m_layoutBoneL2;
	//ID3D11InputLayout* m_layoutBoneL3;
	//ID3D11InputLayout* m_layoutBoneL4;
	//ID3D11InputLayout* m_layoutBoneL5;
	//ID3D11InputLayout* m_layoutBoneL6;
	//ID3D11InputLayout* m_layoutBoneL7;
	//ID3D11InputLayout* m_layoutBoneL8;
	//ID3D11InputLayout* m_layoutNoBoneL0;
	//ID3D11InputLayout* m_layoutNoBoneL1;
	//ID3D11InputLayout* m_layoutNoBoneL2;
	//ID3D11InputLayout* m_layoutNoBoneL3;
	//ID3D11InputLayout* m_layoutNoBoneL4;
	//ID3D11InputLayout* m_layoutNoBoneL5;
	//ID3D11InputLayout* m_layoutNoBoneL6;
	//ID3D11InputLayout* m_layoutNoBoneL7;
	//ID3D11InputLayout* m_layoutNoBoneL8;
	//ID3D11InputLayout* m_layoutLine;

 //   ID3D11Buffer* m_VB;//表示用頂点バッファ。
	//ID3D11Buffer* m_InfB;//表示用ボーン影響度バッファ。
	//ID3D11Buffer* m_IB;//表示用三角のインデックスバッファ。


	ID3D12Resource* m_vertexBuffer;		//頂点バッファ。
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;	//頂点バッファビュー。

	ID3D12Resource* m_indexBuffer;	//インデックスバッファ。
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//インデックスバッファビュー。


	ConstantBuffer m_commonConstantBuffer;					//メッシュ共通の定数バッファ。
	ConstantBuffer m_expandConstantBuffer;					//ユーザー拡張用の定数バッファ
	std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV> m_expandShaderResourceView = { nullptr };	//ユーザー拡張シェーダーリソースビュー。
	void* m_expandData = nullptr;
	//StructuredBuffer m_boneMatricesStructureBuffer;	//ボーン行列の構造化バッファ。
	//std::vector< SMesh* > m_meshs;						//メッシュ。
	bool m_createdescriptorflag;
	//////std::vector< DescriptorHeap > m_descriptorHeap;	//ディスクリプタヒープ。
	DescriptorHeap m_descriptorHeap;					//ディスクリプタヒープ。
	//Skeleton* m_skeleton = nullptr;						//スケルトン。
	//void* m_expandData = nullptr;						//ユーザー拡張データ。


	////Shaderのポイントはnewした場合もShaderBankに格納する
	////Shaderの破棄はShaderBankが行うので　CDispObjのデストラクタでは破棄しない
	//Shader* m_vsNonSkinModel;//スキンなしモデル用の頂点シェーダー。
	//Shader* m_vsSkinModel;//スキンありモデル用の頂点シェーダー。
	//Shader* m_psModel;//モデル用のピクセルシェーダー。




	ChaVector3 m_scale;
	ChaVector3 m_scaleoffset;

	int* m_tmpindexLH;
};

#endif