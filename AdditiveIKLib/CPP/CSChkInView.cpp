#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <GlobalVar.h>

#include <CSChkInView.h>

#include <mqoobject.h>
//#include <mqomaterial.h>

#include <polymesh3.h>
#include <polymesh4.h>
#include <ExtLine.h>

#include <ChaVecCalc.h>
#include <Bone.h>
#include <mqoobject.h>
#include <Model.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <TexBank.h>
#include <TexElem.h>

#include <InfBone.h>

#include "../../MiniEngine/ConstantBuffer.h"
#include "../../MiniEngine/StructuredBuffer.h"
#include "../../MiniEngine/DescriptorHeap.h"
//#include "../../MiniEngine/Material.h"
#include "../../MiniEngine/IMCompute.h"



#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>


//変える場合は *_ChkInView.fxの中の　[numthreads(4, 1, 1)]の行の16の部分も変える必要有
#define CSCHKINVIEWTHREADNUM	4

using namespace std;

static void InitCSRootSignature(RootSignature& rs);
static void InitCSPipelineState(RootSignature& rs, PipelineState& pipelineState, Shader& cs);
void InitCSRootSignature(RootSignature& rs)
{
	// CSルートシグネチャの初期化
	rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}
void InitCSPipelineState(RootSignature& rs, PipelineState& pipelineState, Shader& cs)
{
	// CSパイプラインステートを作成
	D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
	psoDesc.pRootSignature = rs.Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(cs.GetCompiledBlob());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.NodeMask = 0;

	pipelineState.Init(psoDesc);
}




CSChkInView::CSChkInView() : 
	m_CSrootSignature(), m_CSPipelineState(),
	m_CSdescriptorHeap(), m_cbChkInView(),
	m_inputChkInViewSB(), m_outputChkInViewSB()
{
	InitParams();
}
CSChkInView::~CSChkInView()
{
	DestroyObjs();
}
int CSChkInView::InitParams()
{
	m_indexmap.clear();
	m_bsvec.clear();

	m_IMChkInView = nullptr;
	m_workingChkInView = false;

	m_csbsphere = nullptr;
	m_cschkinviewOutPut = nullptr;
	m_csbsnum = 0;
	m_cscreatebsnum = 0;
	m_csChkInView = nullptr;
	m_cbChkInViewCPU.Init();

	m_pdev = 0;//外部メモリ

	return 0;
}
int CSChkInView::DestroyObjs()
{
	if (m_IMChkInView) {
		delete m_IMChkInView;
		m_IMChkInView = nullptr;
	}

	if (m_csbsphere) {
		free(m_csbsphere);
		m_csbsphere = nullptr;
	}
	if (m_cschkinviewOutPut) {
		free(m_cschkinviewOutPut);
		m_cschkinviewOutPut = nullptr;
	}
	m_cbChkInView.DestroyObjs();
	m_CSrootSignature.DestroyObjs();
	m_CSPipelineState.DestroyObjs();
	m_CSdescriptorHeap.DestroyObjs();

	return 0;
}

int CSChkInView::AddBoundary(CMQOObject* srcobj, MODELBOUND srcmb, 
	int srclodnum, int forceinview, int forceinshadow)
{
	if (!srcobj) {
		_ASSERT(0);
		return 1;
	}
	
	if (srcmb.IsValid() == false) {
		//バウンダリー未設定については　計算用データに加えない
		return 0;
	}


	CSBSphere csbs;
	csbs.Init();

	//int lodno = -1;
	int lodno = CHKINVIEW_LOD0;//2024/04/25 lodmindist, lodmaxdistを配列参照することにしたので、LODが無い場合にもLOD0を使用
	const char* pname = srcobj->GetName();
	if (pname && (*pname != 0)) {
		if (strstr(pname, "LOD0") != 0) {
			lodno = CHKINVIEW_LOD0;
		}
		else if (strstr(pname, "LOD1") != 0) {
			lodno = CHKINVIEW_LOD1;
		}
		else if (strstr(pname, "LOD2") != 0) {
			lodno = CHKINVIEW_LOD2;
		}
		else if (strstr(pname, "LOD3") != 0) {
			lodno = CHKINVIEW_LOD3;//2024/04/20 ４段階LODは現時点では未対応　シェーダで非表示にする
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	

	csbs.bs[0] = srcmb.center.x;
	csbs.bs[1] = srcmb.center.y;
	csbs.bs[2] = srcmb.center.z;
	csbs.bs[3] = srcmb.r;

	csbs.bbmin[0] = srcmb.min.x;
	csbs.bbmin[1] = srcmb.min.y;
	csbs.bbmin[2] = srcmb.min.z;
	csbs.bbmin[3] = 1.0f;

	csbs.bbmax[0] = srcmb.max.x;
	csbs.bbmax[1] = srcmb.max.y;
	csbs.bbmax[2] = srcmb.max.z;
	csbs.bbmax[3] = 1.0f;

	int lodnumindex = srclodnum - 1;//!!!! index !!!!
	lodnumindex = max(0, lodnumindex);
	lodnumindex = min(3, lodnumindex);
	csbs.lodno[0] = lodnumindex;//!!!! index !!!!

	int setlodno = lodno;
	setlodno = max(0, setlodno);
	setlodno = min(3, setlodno);
	csbs.lodno[1] = lodno;

	if (forceinview != -1) {//validflag==falseのMODELBOUNDを渡してforceinview指定することがあるのでmb.IsValidより外側のif
		csbs.forceresult[0] = 1;
		csbs.forceresult[1] = forceinview;
	}
	if (forceinshadow != -1) {//validflag==falseのMODELBOUNDを渡してforceinshadow指定することがあるのでmb.IsValidより外側のif
		csbs.forceresult[2] = 1;
		csbs.forceresult[3] = forceinshadow;
	}

	m_bsvec.push_back(csbs);
	
	int bsvecsize = (int)m_bsvec.size();
	int bsindex = bsvecsize - 1;
	m_indexmap[srcobj] = bsindex;


	return 0;
}


int CSChkInView::CreateIMCompute(ID3D12Device* pdev)
{
	m_IMChkInView = new IMCompute();
	if (!m_IMChkInView) {
		_ASSERT(0);
		return 1;
	}

	int result1 = m_IMChkInView->Init(pdev);
	if (result1 != 0) {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CSChkInView::CreateDispObj(ID3D12Device* pdev)
{
	DestroyObjs();

	m_pdev = pdev;


	if (m_bsvec.size() == 0) {
		//2024/04/07
		return 0;
	}


	CallF(CreateIOBuffers(), return 1);
	CallF(CreateIMCompute(pdev), return 1);

	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		//DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
	};	//レンダリングするカラーバッファのフォーマット。



	m_csChkInView = g_engine->GetShaderFromBank("../Media/Shader/AdditiveIK_ChkInView.fx", "CSMain");
	if (m_csChkInView == nullptr) {
		m_csChkInView = new Shader;
		if (!m_csChkInView) {
			_ASSERT(0);
			return 1;
		}
		int result = m_csChkInView->LoadCS("../Media/Shader/AdditiveIK_ChkInView.fx", "CSMain");
		if (result != 0) {
			_ASSERT(0);
			return 1;
		}
		g_engine->RegistShaderToBank("../Media/Shader/AdditiveIK_ChkInView.fx", "CSMain",
			m_csChkInView);
	}
	InitCSRootSignature(m_CSrootSignature);
	InitCSPipelineState(m_CSrootSignature, m_CSPipelineState, *m_csChkInView);
	m_inputChkInViewSB.Init(sizeof(CSBSphere), m_csbsnum, m_csbsphere);
	m_outputChkInViewSB.Init(sizeof(CSInView), m_cscreatebsnum, m_cschkinviewOutPut);
	m_CSdescriptorHeap.RegistShaderResource(0, m_inputChkInViewSB);
	m_CSdescriptorHeap.RegistUnorderAccessResource(0, m_outputChkInViewSB);
	m_cbChkInView.Init(sizeof(CSConstantBufferChkInView), nullptr);
	m_CSdescriptorHeap.RegistConstantBuffer(0, m_cbChkInView);
	m_CSdescriptorHeap.Commit();

	return 0;
}


int CSChkInView::CreateIOBuffers()
{
	m_csbsnum = (int)m_bsvec.size();
	m_cscreatebsnum = (m_csbsnum + (CSCHKINVIEWTHREADNUM - 1)) & ~(CSCHKINVIEWTHREADNUM - 1);

	m_csbsphere = (CSBSphere*)malloc(sizeof(CSBSphere) * m_cscreatebsnum);
	if (!m_csbsphere) {
		_ASSERT(0);
		return 1;
	}
	ZeroMemory(m_csbsphere, sizeof(CSBSphere) * m_cscreatebsnum);

	m_cschkinviewOutPut = (CSInView*)malloc(sizeof(CSInView) * m_cscreatebsnum);
	if (!m_cschkinviewOutPut) {
		_ASSERT(0);
		return 1;
	}
	ZeroMemory(m_cschkinviewOutPut, sizeof(CSInView) * m_cscreatebsnum);

	int bsindex;
	for (bsindex = 0; bsindex < m_csbsnum; bsindex++) {
		CSBSphere curbs = m_bsvec[bsindex];
		*(m_csbsphere + bsindex) = curbs;
	}

	return 0;
}


int CSChkInView::ComputeChkInView(CSConstantBufferChkInView srccb)
{
	if (m_workingChkInView) {
		return 0;
	}
	m_workingChkInView = true;

	//if (!m_IMChkInView) {
	if (!m_IMChkInView || (m_csbsnum <= 0) || (m_cscreatebsnum <= 0)) {
		//Meshが１つも無いモデルの場合にここを通る可能性有
		//_ASSERT(0);
		m_workingChkInView = false;
		return 0;
	}

	m_cbChkInViewCPU = srccb;
	m_cbChkInViewCPU.mBufferSize[0] = m_csbsnum;//!!!!!!!!
	m_cbChkInView.CopyToVRAM(&m_cbChkInViewCPU);

	m_IMChkInView->Reset(m_CSPipelineState.Get());
	m_IMChkInView->SetComputeRootSignature(m_CSrootSignature);
	m_IMChkInView->SetPipelineState(m_CSPipelineState);
	m_IMChkInView->SetComputeDescriptorHeap(m_CSdescriptorHeap);
	m_IMChkInView->IMExecute((m_cscreatebsnum / CSCHKINVIEWTHREADNUM), 1, 1);

	m_workingChkInView = false;

	return 0;
}

CSInView CSChkInView::GetResultOfChkInView(CMQOObject* srcobj)
{
	CSInView retresult;
	retresult.Init();
	if (!m_IMChkInView || (m_csbsnum <= 0) || (m_cscreatebsnum <= 0)) {
		//Meshが１つも無いモデルの場合にここを通る可能性有
		retresult.Init();
	}
	else {
		if (srcobj) {
			CSInView* outputData = (CSInView*)m_outputChkInViewSB.GetResourceOnCPU();
			if (outputData) {
				map<CMQOObject*, int>::iterator itrindex;
				itrindex = m_indexmap.find(srcobj);
				if (itrindex != m_indexmap.end()) {
					int dataIndex = itrindex->second;
					if ((dataIndex >= 0) && (dataIndex < m_csbsnum)) {
						retresult = *(outputData + dataIndex);
					}
					else {
						_ASSERT(0);
						retresult.Init();
					}
				}
				else {
					//boundaryのIsValid==falseのときには　エントリーは無い
					retresult.Init();
				}
			}
			else {
				_ASSERT(0);
				retresult.Init();
			}
		}
		else {
			_ASSERT(0);
			retresult.Init();
		}
	}

	return retresult;
}

