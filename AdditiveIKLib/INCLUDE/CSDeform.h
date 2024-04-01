#ifndef CSDEFORMH
#define CSDEFORMH

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






class CSDeform
{
public:

	CSDeform();
	~CSDeform();

	int CreateDispObj(ID3D12Device* pdev, CPolyMesh3* pm3);
	int CreateDispObj(ID3D12Device* pdev, CPolyMesh4* pm4);
	int CreateDispObj(ID3D12Device* pdev, CExtLine* extline);
	int CreateIOBuffers(ID3D12Device* pdev);

	int ComputeDeform(RenderContext* rc, myRenderer::RENDEROBJ renderobj);
	int CopyCSDeform();


	int GetDeformedDispV(int srcvertindex, BINORMALDISPV* dstv);

private:

	int InitParams();
	int DestroyObjs();


private:
	ID3D12Device* m_pdev;//外部メモリ、Direct3Dのデバイス。
	CPolyMesh3* m_pm3;//外部メモリ、メタセコイアファイルから作成した３Dデータ。
	CPolyMesh4* m_pm4;//外部メモリ、FBXファイルから作成した３Dデータ。
	CExtLine* m_extline;//外部メモリ、線データ。


	CSVertexWithBone* m_csvertexwithbone;
	CSVertexWithoutBone* m_csvertexwithoutbone;
	CSVertexWithBone* m_csvertexwithboneOutPut;
	CSVertexWithoutBone* m_csvertexwithoutboneOutPut;
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


};

#endif