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
class IMCompute;

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

struct CSIndices
{
	int index[4];//[0]-[2]:IndexOfTriangle, [3]:not use
};

struct CSPickResult
{
	int result[4];//[0]:hitflag, [1]:justflag, [2]:hitfaceno, [3]:未使用0
	int dbginfo[4];
	float hitpos[4];
	void Init() {
		result[0] = 0;
		result[1] = 0;
		result[2] = 0;
		result[3] = 0;
		dbginfo[0] = 0;
		dbginfo[1] = 0;
		dbginfo[2] = 0;
		dbginfo[3] = 0;

		hitpos[0] = 0.0f;
		hitpos[1] = 0.0f;
		hitpos[2] = 0.0f;
		hitpos[3] = FLT_MAX;//2024/09/15 mStartからの距離
	};
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


struct CSConstantBufferPick {
	int mBufferSize[4];//0:vertexnum, 1:facenum, 2:indicesnum, 3:未使用0
	float mStartglobal[4];//start point of PickRay
	float mDirglobal[4];//direction of PickRay
	int mFlags[4];//0:excludeinvface, 1-3:未使用0
	void Init() {
		ZeroMemory(mBufferSize, sizeof(int) * 4);
		ZeroMemory(mStartglobal, sizeof(float) * 4);
		ZeroMemory(mDirglobal, sizeof(float) * 4);
		ZeroMemory(mFlags, sizeof(int) * 4);
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
	int CreateIOBuffers(ID3D12Device* pdev, int vertextype);
	int DestroyObjs();

	int ComputeDeform(myRenderer::RENDEROBJ renderobj);
	int CopyCSDeform();


	int PickRay(ChaVector3 startglobal, ChaVector3 dirglobal,
		bool excludeinvface, int* hitfaceindex, ChaVector3* dsthitpos, float* dstdist);
	int GetResultOfPickRay(int* hitfaceindex, ChaVector3* dsthitpos, float* dstdist);

	int GetDeformedDispV(int srcvertindex, BINORMALDISPV* dstv);

private:

	int InitParams();
	int CreateIMCompute(ID3D12Device* pdev);

private:
	ID3D12Device* m_pdev;//外部メモリ、Direct3Dのデバイス。
	CPolyMesh3* m_pm3;//外部メモリ、メタセコイアファイルから作成した３Dデータ。
	CPolyMesh4* m_pm4;//外部メモリ、FBXファイルから作成した３Dデータ。
	CExtLine* m_extline;//外部メモリ、線データ。


	bool m_workingDeform;
	bool m_workingPick;
	IMCompute* m_IMDeform;
	IMCompute* m_IMPick;


	//###########
	//for Deform
	//###########
	RootSignature m_CSrootSignature;
	PipelineState m_CSPipelineState;

	CSVertexWithBone* m_csvertexwithbone;
	CSVertexWithoutBone* m_csvertexwithoutbone;
	CSVertexWithBone* m_csvertexwithboneOutPut;
	CSVertexWithoutBone* m_csvertexwithoutboneOutPut;

	int m_csvertexnum;
	int m_cscreatevertexnum;

	StructuredBuffer m_inputSB;
	RWStructuredBuffer m_outputSB;
	Shader* m_csModel = nullptr;

	ConstantBuffer m_cbWithoutBone;
	ConstantBuffer m_cbWithBone;
	CSConstantBufferWithoutBone m_cbWithoutBoneCPU;
	CSConstantBufferWithBone m_cbWithBoneCPU;
	DescriptorHeap m_CSdescriptorHeap;

	//#########
	//for pick
	//#########
	int m_pickstate;//0:inital, 1:DispatchCS, 2:GetResult
	RootSignature m_CSPickrootSignature;//for pick
	PipelineState m_CSPickPipelineState;//for pick

	CSIndices* m_csindices;//for pick length:createfacenum
	CSPickResult m_cspickOutPut1;//for pick 要素数1
	CSPickResult m_cspickOutPut1_save;//for pick 要素数1
	int m_csfacenum;//for pick
	int m_cscreatefacenum;//for pick

	StructuredBuffer m_inputIndicesSB;//for pick
	RWStructuredBuffer m_outputPickSB1;//for pick length:1 要素数1
	Shader* m_csPick = nullptr;//for pick

	ConstantBuffer m_cbPick;//for pick
	CSConstantBufferPick m_cbPickCPU;//for pick
	DescriptorHeap m_CSPickdescriptorHeap;//for pick

};

#endif