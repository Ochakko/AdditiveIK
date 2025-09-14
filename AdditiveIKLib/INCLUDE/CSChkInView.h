#ifndef CSCHKINVIEWH
#define CSCHKINVIEWH

#include <ChaVecCalc.h>
#include <mqomaterial.h>
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"

#include <unordered_map>
#include <vector>

class CMQOObject;
class CMQOMaterial;
class CPolyMesh3;
class CPolyMesh4;
class CExtLine;
class IMCompute;

class ConstantBuffer;//メッシュ共通の定数バッファ。
class StructuredBuffer;//ボーン行列の構造化バッファ。
class DescriptorHeap;//ディスクリプタヒープ。

struct CSBSphere
{
	float bs[4];//[0]-[2]:centerpos, [3]:r
	float bbmin[4];//bbox min
	float bbmax[4];//bbox max
	int lodno[4];//[0]:lodnumindex (lodnum - 1), [1]:lodno, [2]-[3]:未使用
	int forceresult[4]; //[0]:forceInView, [1]:forceInViewValue, [2]:forceInShadow, [3]:forceInShadowValue
	void Init() {
		bs[0] = 0.0f;
		bs[1] = 0.0f;
		bs[2] = 0.0f;
		bs[3] = 0.1f;
		bbmin[0] = 0.0f;
		bbmin[1] = 0.0f;
		bbmin[2] = 0.0f;
		bbmin[3] = 1.0f;
		bbmax[0] = 0.0f;
		bbmax[1] = 0.0f;
		bbmax[2] = 0.0f;
		bbmax[3] = 1.0f;
		lodno[0] = -1;
		lodno[1] = -1;
		lodno[2] = -1;
		lodno[3] = -1;
		forceresult[0] = 0;
		forceresult[1] = 0;
		forceresult[2] = 0;
		forceresult[3] = 0;
	};
};

struct CSInView
{
	int inview[4];//[0]:InView, [1]:InShadow, [2]-[3]:未使用
	float dist[4]; //[0]:distFromCamera
	void Init() {
		ZeroMemory(inview, sizeof(int) * 4);
		ZeroMemory(dist, sizeof(float) * 4);
	};
};

struct CSConstantBufferChkInView {
	int mBufferSize[4];//[0]:bsnum
	Matrix mWorld;		//ワールド行列。
	float camEye[4];
	float params1[4];//[0]:BACKPOSCOEF, [1]:cos(g_fovy * 0.85), [2]:g_projfar, [3]:g_projnear
	//float lodrate2L[4];
	//float lodrate3L[4];
	//float lodrate4L[4];//2024/04/22
	float lodmindist[4][4];//2024/04/25 lodmindist[lodnum][lodno]
	float lodmaxdist[4][4];//2024/04/25 lodmaxdist[lodnum][lodno]
	float shadowPos[4];
	float shadowparams1[4];//[0]:cos(shadowfov), [1]:shadowmaxdist(g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno])
	float frustumPlanes[6][4];
	float frustumCorners[8][4];
	float shadowPlanes[6][4];
	float shadowCorners[8][4];
	void Init() {
		mBufferSize[0] = 0;
		mBufferSize[1] = 0;
		mBufferSize[2] = 0;
		mBufferSize[3] = 0;

		mWorld.SetIdentity();
		ZeroMemory(camEye, sizeof(float) * 4);
		params1[0] = 1.0f;
		params1[1] = 45.0f;
		params1[2] = 5000.0f;
		params1[3] = 10.0f;
		//lodrate2L[0] = 0.1f;
		//lodrate2L[1] = 1.0f;
		//lodrate2L[2] = 1.0f;
		//lodrate2L[3] = 1.0f;
		//lodrate3L[0] = 0.1f;
		//lodrate3L[1] = 0.15f;
		//lodrate3L[2] = 1.0f;
		//lodrate3L[3] = 1.0f;
		//lodrate4L[0] = 0.1f;
		//lodrate4L[1] = 0.15f;
		//lodrate4L[2] = 0.5f;
		//lodrate4L[3] = 1.0f;
		ZeroMemory(lodmindist, sizeof(float) * 4 * 4);
		ZeroMemory(lodmaxdist, sizeof(float) * 4 * 4);

		ZeroMemory(shadowPos, sizeof(float) * 4);
		shadowparams1[0] = 45.0f;
		shadowparams1[1] = 5000.0f;
		shadowparams1[2] = 0.0f;
		shadowparams1[3] = 0.0f;
		ZeroMemory(frustumPlanes, sizeof(float) * 6 * 4);
		ZeroMemory(frustumCorners, sizeof(float) * 8 * 4);
		ZeroMemory(shadowPlanes, sizeof(float) * 6 * 4);
		ZeroMemory(shadowCorners, sizeof(float) * 8 * 4);
	};
};

class CSChkInView
{
public:
	CSChkInView();
	~CSChkInView();

	int AddBoundary(CMQOObject* srcobj, MODELBOUND srcmb, 
		int srclodnum, int forceinview, int forceinshadow);
	int CreateDispObj(ID3D12Device* pdev);

	int ComputeChkInView(CSConstantBufferChkInView srccb);
	CSInView GetResultOfChkInView(CMQOObject* srcobj);

private:
	int InitParams();
	int DestroyObjs();
	int CreateIOBuffers();
	int CreateIMCompute(ID3D12Device* pdev);

private:
	ID3D12Device* m_pdev;//外部メモリ、Direct3Dのデバイス。

	std::unordered_map<CMQOObject*, int> m_indexmap;//mqoobject-->bufferindex
	std::vector<CSBSphere> m_bsvec;

	bool m_workingChkInView;
	IMCompute* m_IMChkInView;

	RootSignature m_CSrootSignature;
	PipelineState m_CSPipelineState;

	CSBSphere* m_csbsphere;
	CSInView* m_cschkinviewOutPut;

	int m_csbsnum;
	int m_cscreatebsnum;

	StructuredBuffer m_inputChkInViewSB;
	RWStructuredBuffer m_outputChkInViewSB;
	Shader* m_csChkInView = nullptr;

	ConstantBuffer m_cbChkInView;
	CSConstantBufferChkInView m_cbChkInViewCPU;
	DescriptorHeap m_CSdescriptorHeap;

};

#endif