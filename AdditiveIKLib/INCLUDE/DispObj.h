#ifndef DISPOBJH
#define DISPOBJH

#include <ChaVecCalc.h>
#include <mqomaterial.h>
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"


class CMQOObject;
class CMQOMaterial;
class CPolyMesh3;
class CPolyMesh4;
class CExtLine;


class ConstantBuffer;//メッシュ共通の定数バッファ。
class StructuredBuffer;//ボーン行列の構造化バッファ。
class DescriptorHeap;//ディスクリプタヒープ。

struct CSVertexWithBone
{
	float pos[4];
	float bweight[4];
	int bindices[4];
};

struct CSVertexWithoutBone
{
	float pos[4];
};


struct CSConstantBufferWithoutBone {
	int mVertexNum[4];
	Matrix mWorld;		//ワールド行列。
	Matrix mView;		//ビュー行列。
	Matrix mProj;		//プロジェクション行列。
	void Init() {
		mVertexNum[0] = 0;
		mVertexNum[1] = 0;
		mVertexNum[2] = 0;
		mVertexNum[3] = 0;

		mWorld.SetIdentity();
		mView.SetIdentity();
		mProj.SetIdentity();
	};
};


struct CSConstantBufferWithBone {
	int mVertexNum[4];
	Matrix mWorld;		//ワールド行列。
	Matrix mView;		//ビュー行列。
	Matrix mProj;		//プロジェクション行列。
	float setfl4x4[16 * MAXBONENUM];//ボーンの姿勢マトリックス
	void Init() {
		mVertexNum[0] = 0;
		mVertexNum[1] = 0;
		mVertexNum[2] = 0;
		mVertexNum[3] = 0;

		mWorld.SetIdentity();
		mView.SetIdentity();
		mProj.SetIdentity();
		ZeroMemory(setfl4x4, sizeof(float) * 16 * MAXBONENUM);
	};
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
	 * @param (int hasbone) IN ボーン変形有の場合は１、無しの場合は０。AdditiveIKではメタセコイアデータはボーン無し。
	 * @return 成功したら０。
	 */
	int CreateDispObj(ID3D12Device* pdev, CPolyMesh3* pm3, int hasbone, int srcuvnum);


	/**
	 * @fn
	 * CreateDispObj
	 * @breaf FBXからのデータ用の表示用オブジェクトを作成する。
	 * @param (ID3D12Device* pdev) IN Direct3Dのデバイス。
	 * @param (CPolyMesh4* pm4) IN 形状データへのポインタ。
	 * @param (int hasbone) IN ボーン変形有の場合は１、無しの場合は０。 * @return 戻り値の説明
	 * @return 成功したら０。
	 */
	int CreateDispObj(ID3D12Device* pdev, CPolyMesh4* pm4, int hasbone, int srcuvnum);

	/**
	 * @fn
	 * CreateDispObj
	 * @breaf 線分データ用の表示用オブジェクトを作成する。
	 * @param (ID3D12Device* pdev) IN Direct3Dのデバイス。
	 * @param (CExtLine* extline) IN 線の形状データへのポインタ。
	 * @return 成功したら０。
	 */
	int CreateDispObj(ID3D12Device* pdev, CExtLine* extline);


	/**
	 * @fn
	 * RenderNormal
	 * @breaf 通常のベーシックな方法で３D描画をする。
	 * @param (CMQOMaterial* rmaterial) IN 材質へのポインタ。
	 * @param (int lightflag) IN 照光処理するときは１、しないときは０。
	 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
	 * @return 成功したら０。
	 */
	int ComputeDeform(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	int CopyCSDeform();

	int RenderNormal(RenderContext* rc, myRenderer::RENDEROBJ renderobj);

	int RenderNormalMaterial(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
		bool laterflag, CMQOMaterial* rmaterial, int curoffset, int curtrinum, bool isfirstmaterial);


	/**
	 * @fn
	 * RenderNormalPM3
	 * @breaf 通常のベーシックな方法でメタセコイアデータを３D表示する。
	 * @param (int lightflag) IN 照光処理するときは１、しないときは０。
	 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
	 * @return 成功したら０。
	 * @detail FBXデータは１オブジェクトにつき１マテリアル(材質)だが、メタセコイアデータは１オブジェクトに複数マテリアルが設定されていることが多い。
	 */
	int RenderNormalPM3(RenderContext* rc, myRenderer::RENDEROBJ renderobj);

	int RenderNormalPM3Material(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
		bool lasterflag, CMQOMaterial* rmaterial,
		int curoffset, int curtrinum);

	int RenderZPrePm4(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	int RenderZPrePm3(RenderContext* rc, myRenderer::RENDEROBJ renderobj);

	int RenderInstancingPm3(RenderContext* rc, myRenderer::RENDEROBJ renderobj);

	int RenderShadowMap(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	int RenderShadowMapPM3(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	int RenderShadowReciever(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	int RenderShadowRecieverPM3(RenderContext* rc, myRenderer::RENDEROBJ renderobj);

	/**
	 * @fn
	 * RenderLine
	 * @breaf 線分データを表示する。
	 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
	 * @return 成功したら０。
	 */
	int RenderLine(RenderContext* rc, myRenderer::RENDEROBJ renderobj);

	/**
	 * @fn
	 * CopyDispV
	 * @breaf FBXの頂点データを表示バッファにコピーする。
	 * @param (CPolyMesh4* pm4) IN FBXの形状データ。
	 * @return 成功したら０。
	 */
	int CopyDispV(CPolyMesh4* pm4);

	/**
	 * @fn
	 * CopyDispV
	 * @breaf メタセコイアの頂点データを表示バッファにコピーする。
	 * @param (CPolyMesh3* pm3) IN メタセコイアの形状データ。
	 * @param (引数名) 引数の説明
	 * @return 成功したら０。
	 */
	int CopyDispV(CPolyMesh3* pm3);

	void ResetScaleInstancing();
	void SetScale(ChaVector3 srcscale, ChaVector3 srcoffset);
	SCALEINSTANCING* GetScaleInstancing();
	int GetScaleInstancingNum();

	//void UpdateBoneMatrix(int srcdatanum, void* srcdata);

	ID3D12Resource* GetVertexBuffer()
	{
		return m_vertexBuffer;
	}
	ID3D12Resource* GetIndexBuffer()
	{
		return m_indexBuffer;
	}


	//####################
	//位置以外のコピーは省略
	//####################
	int GetDeformedDispV(int srcvertindex, BINORMALDISPV* dstv);

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
	 //int CreateVBandIBLine(ID3D12Device* pdev);//extlineもCreateVBandIBで処理


	void CreateDescriptorHeaps();


private:


	int m_hasbone;//ボーン変形用のオブジェクトであるとき１、それ以外の時は０。

	ID3D12Device* m_pdev;//外部メモリ、Direct3Dのデバイス。
	CPolyMesh3* m_pm3;//外部メモリ、メタセコイアファイルから作成した３Dデータ。
	CPolyMesh4* m_pm4;//外部メモリ、FBXファイルから作成した３Dデータ。
	CExtLine* m_extline;//外部メモリ、線データ。


	ID3D12Resource* m_vertexBuffer;		//頂点バッファ。
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;	//頂点バッファビュー。
	VertexBuffer  m_InstancingBuffer;		//インスタンシング頂点バッファ。
	uint8_t* m_vertexMap;

	ID3D12Resource* m_indexBuffer;	//インデックスバッファ。
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//インデックスバッファビュー。


	CSVertexWithBone* m_csvertexwithbone;
	CSVertexWithoutBone* m_csvertexwithoutbone;
	CSVertexWithBone* m_csvertexwithboneOutPut;
	CSVertexWithoutBone* m_csvertexwithoutboneOutPut;
	//CSVertexOutput* m_csvertexoutput;
	int m_csvertexnum;
	int m_cscreatevertexnum;
	StructuredBuffer m_inputSB;
	RWStructuredBuffer m_outputSB;
	RootSignature m_CSrootSignature;					//CSルートシグネチャ。
	PipelineState m_CSPipelineState;		//CSモデル用のパイプラインステート。
	Shader* m_csModel = nullptr;				//CSモデル用の頂点シェーダー。
	DescriptorHeap m_CSdescriptorHeap;
	ConstantBuffer m_cbWithoutBone;
	ConstantBuffer m_cbWithBone;
	CSConstantBufferWithoutBone m_cbWithoutBoneCPU;
	CSConstantBufferWithBone m_cbWithBoneCPU;


	////Shaderのポイントはnewした場合もShaderBankに格納する
	////Shaderの破棄はShaderBankが行うので　CDispObjのデストラクタでは破棄しない
	//Shader* m_vsNonSkinModel;//スキンなしモデル用の頂点シェーダー。
	//Shader* m_vsSkinModel;//スキンありモデル用の頂点シェーダー。
	//Shader* m_psModel;//モデル用のピクセルシェーダー。

	CDispObj* m_shadowdispobj;

	int m_scaleinstancenum;
	SCALEINSTANCING m_scaleInstancing[RIGMULTINDEXMAX];

	int* m_tmpindexLH;


};

#endif