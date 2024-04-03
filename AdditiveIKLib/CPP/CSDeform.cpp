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

#include <CSDeform.h>

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



//変える場合は *_Deform.fx, *_Pick.fxの中の　[numthreads(16, 1, 1)]の行の16の部分も変える必要有
//#define CSTHREADNUM	16
#define CSTHREADNUM	4



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




CSDeform::CSDeform() : 
	m_CSrootSignature(), m_CSPipelineState(),
	m_CSdescriptorHeap(), m_cbWithoutBone(), m_cbWithBone(),
	m_inputSB(), m_outputSB(),
	m_CSPickrootSignature(), m_CSPickPipelineState(),
	m_CSPickdescriptorHeap(), m_cbPick(),
	m_inputIndicesSB(), m_outputPickSB1()
{
	InitParams();
}
CSDeform::~CSDeform()
{
	DestroyObjs();
}
int CSDeform::InitParams()
{
	m_IMDeform = nullptr;
	m_IMPick = nullptr;
	m_workingDeform = false;
	m_workingPick = false;

	m_csvertexwithbone = nullptr;
	m_csvertexwithoutbone = nullptr;
	m_csvertexwithboneOutPut = nullptr;
	m_csvertexwithoutboneOutPut = nullptr;
	m_csvertexnum = 0;
	m_cscreatevertexnum = 0;
	m_csModel = nullptr;
	m_cbWithoutBoneCPU.Init();
	m_cbWithBoneCPU.Init();

	//#########
	//for pick
	//#########
	m_pickstate = 0;//0:inital, 1:DispatchCS, 2:GetResult
	m_csindices = nullptr;
	m_cspickOutPut1.Init();
	m_cspickOutPut1_save.Init();
	m_csfacenum = 0;
	m_cscreatefacenum = 0;
	m_csPick = nullptr;
	m_cbPickCPU.Init();


	m_pdev = 0;//外部メモリ
	m_pm3 = 0;//外部メモリ
	m_pm4 = 0;//外部メモリ
	m_extline = 0;//外部メモリ

	return 0;
}
int CSDeform::DestroyObjs()
{
	if (m_IMDeform) {
		delete m_IMDeform;
		m_IMDeform = nullptr;
	}
	if (m_IMPick) {
		delete m_IMPick;
		m_IMPick = nullptr;
	}


	if (m_csvertexwithbone) {
		free(m_csvertexwithbone);
		m_csvertexwithbone = nullptr;
	}
	if (m_csvertexwithoutbone) {
		free(m_csvertexwithoutbone);
		m_csvertexwithoutbone = nullptr;
	}
	if (m_csvertexwithboneOutPut) {
		free(m_csvertexwithboneOutPut);
		m_csvertexwithboneOutPut = nullptr;
	}
	if (m_csvertexwithoutboneOutPut) {
		free(m_csvertexwithoutboneOutPut);
		m_csvertexwithoutboneOutPut = nullptr;
	}
	m_cbWithoutBone.DestroyObjs();
	m_cbWithBone.DestroyObjs();
	m_CSrootSignature.DestroyObjs();
	m_CSPipelineState.DestroyObjs();
	m_CSdescriptorHeap.DestroyObjs();


	//#########
	//for pick
	//#########
	if (m_csindices) {
		free(m_csindices);
		m_csindices = nullptr;
	}
	m_cbPick.DestroyObjs();
	m_CSPickrootSignature.DestroyObjs();
	m_CSPickPipelineState.DestroyObjs();
	m_CSPickdescriptorHeap.DestroyObjs();

	return 0;
}

int CSDeform::CreateIMCompute(ID3D12Device* pdev)
{
	m_IMDeform = new IMCompute();
	if (!m_IMDeform) {
		_ASSERT(0);
		return 1;
	}
	m_IMPick = new IMCompute();
	if (!m_IMPick) {
		_ASSERT(0);
		return 1;
	}



	int result1 = m_IMDeform->Init(pdev);
	if (result1 != 0) {
		_ASSERT(0);
		return 1;
	}
	int result2 = m_IMPick->Init(pdev);
	if (result2 != 0) {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CSDeform::CreateDispObj(ID3D12Device* pdev, CPolyMesh3* pm3)
{
	DestroyObjs();

	m_pdev = pdev;
	m_pm3 = pm3;


	//CallF(CreateIMCompute(pdev), return 1);
	//int vertextype = 1;//pm3
	//CallF(CreateIOBuffers(pdev, vertextype), return 1);


	//std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
	//	DXGI_FORMAT_R32G32B32A32_FLOAT,
	//	//DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//};	//レンダリングするカラーバッファのフォーマット。

	//
	//m_csModel = g_engine->GetShaderFromBank("../Media/Shader/AdditiveIK_NoSkin_Deform.fx", "CSMain");
	//if (m_csModel == nullptr) {
	//	m_csModel = new Shader;
	//	if (!m_csModel) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//	int result = m_csModel->LoadCS("../Media/Shader/AdditiveIK_NoSkin_Deform.fx", "CSMain");
	//	if (result != 0) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//	g_engine->RegistShaderToBank("../Media/Shader/AdditiveIK_NoSkin_Deform.fx", "CSMain", 
	//		m_csModel);
	//}
	////InitCSRootSignature(m_CSrootSignature);
	////InitCSPipelineState(m_CSrootSignature, m_CSPipelineState, *m_csModel);
	////m_inputSB.Init(sizeof(CSVertexWithoutBone), m_cscreatevertexnum, m_csvertexwithoutbone);
	//m_outputSB.Init(sizeof(CSVertexWithoutBone), m_cscreatevertexnum, m_csvertexwithoutboneOutPut);
	////m_CSdescriptorHeap.RegistShaderResource(0, m_inputSB);
	////m_CSdescriptorHeap.RegistUnorderAccessResource(0, m_outputSB);
	////m_cbWithoutBone.Init(sizeof(CSConstantBufferWithoutBone), nullptr);
	////m_CSdescriptorHeap.RegistConstantBuffer(0, m_cbWithoutBone);
	////m_CSdescriptorHeap.Commit();


	////##########
	////for pick
	////##########
	//m_csPick = g_engine->GetShaderFromBank("../Media/Shader/AdditiveIK_NoSkin_Pick.fx", "CSMain");
	//if (m_csPick == nullptr) {
	//	m_csPick = new Shader;
	//	if (!m_csPick) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//	int result = m_csPick->LoadCS("../Media/Shader/AdditiveIK_NoSkin_Pick.fx", "CSMain");
	//	if (result != 0) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//	g_engine->RegistShaderToBank("../Media/Shader/AdditiveIK_NoSkin_Pick.fx", "CSMain",
	//		m_csPick);
	//}
	//InitCSRootSignature(m_CSPickrootSignature);
	//InitCSPipelineState(m_CSPickrootSignature, m_CSPickPipelineState, *m_csPick);
	//m_inputIndicesSB.Init(sizeof(CSIndices), m_cscreatefacenum, m_csindices);
	//m_outputPickSB1.Init(sizeof(CSPickResult), 1, &m_cspickOutPut1);
	//m_CSPickdescriptorHeap.RegistShaderResource(0, m_inputIndicesSB);
	//m_CSPickdescriptorHeap.RegistUnorderAccessResource(0, m_outputSB);//<-- pm3 : 変換無しの頂点をそのままセット
	//m_CSPickdescriptorHeap.RegistUnorderAccessResource(1, m_outputPickSB1);
	//m_cbPick.Init(sizeof(CSConstantBufferPick), nullptr);
	//m_CSPickdescriptorHeap.RegistConstantBuffer(0, m_cbPick);
	//m_CSPickdescriptorHeap.Commit();

	return 0;
}
int CSDeform::CreateDispObj(ID3D12Device* pdev, CPolyMesh4* pm4)
{
	DestroyObjs();

	m_pdev = pdev;
	m_pm4 = pm4;

	CallF(CreateIMCompute(pdev), return 1);
	int vertextype = 0;//pm4
	CallF(CreateIOBuffers(pdev, vertextype), return 1);

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



	m_csModel = g_engine->GetShaderFromBank("../Media/Shader/AdditiveIK_Skin_Deform.fx", "CSMain");
	if (m_csModel == nullptr) {
		m_csModel = new Shader;
		if (!m_csModel) {
			_ASSERT(0);
			return 1;
		}
		int result = m_csModel->LoadCS("../Media/Shader/AdditiveIK_Skin_Deform.fx", "CSMain");
		if (result != 0) {
			_ASSERT(0);
			return 1;
		}
		g_engine->RegistShaderToBank("../Media/Shader/AdditiveIK_Skin_Deform.fx", "CSMain",
			m_csModel);
	}
	InitCSRootSignature(m_CSrootSignature);
	InitCSPipelineState(m_CSrootSignature, m_CSPipelineState, *m_csModel);
	m_inputSB.Init(sizeof(CSVertexWithBone), m_cscreatevertexnum, m_csvertexwithbone);
	m_outputSB.Init(sizeof(CSVertexWithBone), m_cscreatevertexnum, m_csvertexwithboneOutPut);
	//m_outputSB.Init(sizeof(CSVertexOutput), m_csvertexnum, m_csvertexoutput);
	m_CSdescriptorHeap.RegistShaderResource(0, m_inputSB);
	m_CSdescriptorHeap.RegistUnorderAccessResource(0, m_outputSB);
	m_cbWithBone.Init(sizeof(CSConstantBufferWithBone), nullptr);
	m_CSdescriptorHeap.RegistConstantBuffer(0, m_cbWithBone);
	m_CSdescriptorHeap.Commit();


	//##########
	//for pick
	//##########
	m_csPick = g_engine->GetShaderFromBank("../Media/Shader/AdditiveIK_Skin_Pick.fx", "CSMain");
	if (m_csPick == nullptr) {
		m_csPick = new Shader;
		if (!m_csPick) {
			_ASSERT(0);
			return 1;
		}
		int result = m_csPick->LoadCS("../Media/Shader/AdditiveIK_Skin_Pick.fx", "CSMain");
		if (result != 0) {
			_ASSERT(0);
			return 1;
		}
		g_engine->RegistShaderToBank("../Media/Shader/AdditiveIK_Skin_Pick.fx", "CSMain",
			m_csPick);
	}
	InitCSRootSignature(m_CSPickrootSignature);
	InitCSPipelineState(m_CSPickrootSignature, m_CSPickPipelineState, *m_csPick);
	m_inputIndicesSB.Init(sizeof(CSIndices), m_cscreatefacenum, m_csindices);
	m_outputPickSB1.Init(sizeof(CSPickResult), 1, &m_cspickOutPut1);
	m_CSPickdescriptorHeap.RegistShaderResource(0, m_inputIndicesSB);
	m_CSPickdescriptorHeap.RegistUnorderAccessResource(0, m_outputSB);//<--IMDeformの実行結果を入力
	m_CSPickdescriptorHeap.RegistUnorderAccessResource(1, m_outputPickSB1);
	m_cbPick.Init(sizeof(CSConstantBufferPick), nullptr);
	m_CSPickdescriptorHeap.RegistConstantBuffer(0, m_cbPick);
	m_CSPickdescriptorHeap.Commit();

	return 0;
}

int CSDeform::CreateDispObj( ID3D12Device* pdev, CExtLine* extline )
{
	DestroyObjs();

	m_pdev = pdev;
	m_extline = extline;

	//CallF(CreateIOBuffers(pdev), return 1);
	//
	//std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
	//	DXGI_FORMAT_R32G32B32A32_FLOAT,
	//	//DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//};	//レンダリングするカラーバッファのフォーマット。


	return 0;
}


int CSDeform::CreateIOBuffers(ID3D12Device* pdev, int vertextype)
{
	//HRESULT hr;

	UINT elemleng, infleng;

	infleng = sizeof(PM3INF);

	int pmvleng, pmfleng;
	PM3INF* pmib = 0;
	BINORMALDISPV* pm3v = 0;
	BINORMALDISPV* pm4v = 0;
	EXTLINEV* plinev = 0;
	DWORD vbsize;
	DWORD stride;
	if (m_pm3) {
		elemleng = sizeof(BINORMALDISPV);
		pmvleng = m_pm3->GetOptLeng();
		pmfleng = m_pm3->GetFaceNum();
		pmib = 0;
		pm3v = m_pm3->GetDispV();

		stride = sizeof(BINORMALDISPV);
		vbsize = pmvleng * stride;

		//m_csvertexnum = (pmvleng + 3) & ~3;
		//m_csvertexnum = (pmvleng + 7) & ~7;
		//m_csvertexnum = (pmvleng + 15) & ~15;
		m_csvertexnum = pmvleng;
		m_cscreatevertexnum = (pmvleng + (CSTHREADNUM - 1)) & ~(CSTHREADNUM - 1);
		m_csfacenum = pmfleng;
		m_cscreatefacenum = (pmfleng + (CSTHREADNUM - 1)) & ~(CSTHREADNUM - 1);
	}
	else if (m_pm4) {
		elemleng = sizeof(BINORMALDISPV);
		pmvleng = m_pm4->GetOptLeng();
		pmfleng = m_pm4->GetFaceNum();
		pmib = m_pm4->GetPm3Inf();
		pm4v = m_pm4->GetPm3Disp();

		//stride = sizeof(BINORMALDISPV);
		stride = sizeof(BINORMALDISPV) + sizeof(PM3INF);


		vbsize = pmvleng * stride;

		//m_csvertexnum = (pmvleng + 3) & ~3;
		//m_csvertexnum = (pmvleng + 7) & ~7;
		//m_csvertexnum = (pmvleng + 15) & ~15;
		m_csvertexnum = pmvleng;
		m_cscreatevertexnum = (pmvleng + (CSTHREADNUM - 1)) & ~(CSTHREADNUM - 1);
		m_csfacenum = pmfleng;
		m_cscreatefacenum = (pmfleng + (CSTHREADNUM - 1)) & ~(CSTHREADNUM - 1);
	}
	else if (m_extline) {
		pmvleng = m_extline->GetLineNum() * 2;
		pmfleng = m_extline->GetLineNum();
		pmib = 0;
		pm3v = 0;
		pm4v = 0;
		plinev = m_extline->GetExtLineV();

		stride = sizeof(EXTLINEV);
		vbsize = pmvleng * stride;

		//m_csvertexnum = (pmvleng + 3) & ~3;
		//m_csvertexnum = (pmvleng + 7) & ~7;
		//m_csvertexnum = (pmvleng + 15) & ~15;
		m_csvertexnum = pmvleng;
		m_cscreatevertexnum = (pmvleng + (CSTHREADNUM - 1)) & ~(CSTHREADNUM - 1);
		m_csfacenum = pmfleng;
		m_cscreatefacenum = (pmfleng + (CSTHREADNUM - 1)) & ~(CSTHREADNUM - 1);
	}
	else {
		_ASSERT(0);
		return 1;
	}

	if (m_pm3) {
		//m_csvertexwithoutbone = (CSVertexWithoutBone*)malloc(sizeof(CSVertexWithoutBone) * m_cscreatevertexnum);
		//if (!m_csvertexwithoutbone) {
		//	_ASSERT(0);
		//	return 1;
		//}
		//ZeroMemory(m_csvertexwithoutbone, sizeof(CSVertexWithoutBone) * m_cscreatevertexnum);

		//##############################################
		//pm3の場合にはDeform無しでそのまま元の座標を使用する
		//##############################################

		//m_csvertexwithoutboneOutPut = (CSVertexWithoutBone*)malloc(sizeof(CSVertexWithoutBone) * m_cscreatevertexnum);
		//if (!m_csvertexwithoutboneOutPut) {
		//	_ASSERT(0);
		//	return 1;
		//}
		//ZeroMemory(m_csvertexwithoutboneOutPut, sizeof(CSVertexWithoutBone) * m_cscreatevertexnum);
	}
	else if (m_pm4) {
		m_csvertexwithbone = (CSVertexWithBone*)malloc(sizeof(CSVertexWithBone) * m_cscreatevertexnum);
		if (!m_csvertexwithbone) {
			_ASSERT(0);
			return 1;
		}
		ZeroMemory(m_csvertexwithbone, sizeof(CSVertexWithBone) * m_cscreatevertexnum);

		m_csvertexwithboneOutPut = (CSVertexWithBone*)malloc(sizeof(CSVertexWithBone) * m_cscreatevertexnum);
		if (!m_csvertexwithboneOutPut) {
			_ASSERT(0);
			return 1;
		}
		ZeroMemory(m_csvertexwithboneOutPut, sizeof(CSVertexWithBone) * m_cscreatevertexnum);
	}

	//if (m_pm3 || m_pm4) {
	if (m_pm4) {
		m_csindices = (CSIndices*)malloc(sizeof(CSIndices) * m_cscreatefacenum);
		if (!m_csindices) {
			_ASSERT(0);
			return 1;
		}
		ZeroMemory(m_csindices, sizeof(CSIndices) * m_cscreatefacenum);

		m_cspickOutPut1.Init();
		m_cspickOutPut1_save.Init();
	}

//################
//頂点バッファコピー
//################
	{
		if (m_pm3) {
			//##############################################
			//pm3の場合にはDeform無しでそのまま元の座標を使用する
			//##############################################

			//DWORD vno;
			//for (vno = 0; vno < (DWORD)pmvleng; vno++) {
			//	uint8_t* pcsdest = (uint8_t*)m_csvertexwithoutboneOutPut + vno * sizeof(CSVertexWithoutBone);//Outputにセット
			//	BINORMALDISPV* curv = pm3v + vno;
			//	memcpy(pcsdest, curv, sizeof(ChaVector4));//posだけコピー
			//}
		}
		else if (m_pm4) {
			DWORD vno;
			for (vno = 0; vno < (DWORD)pmvleng; vno++) {
				uint8_t* pcsdest = (uint8_t*)m_csvertexwithbone + vno * sizeof(CSVertexWithBone);//Inputにセット
				
				BINORMALDISPV* curv = pm4v + vno;
				PM3INF* curinf = pmib + vno;

				memcpy(pcsdest, curv, sizeof(ChaVector4));//posをコピー
				memcpy(pcsdest + sizeof(ChaVector4), curinf, sizeof(PM3INF));//bweightとbindicesをコピー
			}
		}
		else if (m_extline) {

		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

//#######################
//インデックスバッファコピー
//#######################

	int* pindex = 0;
	if (m_pm3) {
		//pindex = m_pm3->GetDispIndex();
	}
	else if (m_pm4) {
		pindex = m_pm4->GetDispIndex();
	}
	if (pindex) {
		int faceindex;
		for (faceindex = 0; faceindex < m_csfacenum; faceindex++) {
			(m_csindices + faceindex)->index[0] = *(pindex + faceindex * 3);
			(m_csindices + faceindex)->index[1] = *(pindex + faceindex * 3 + 1);
			(m_csindices + faceindex)->index[2] = *(pindex + faceindex * 3 + 2);
			(m_csindices + faceindex)->index[3] = 0;
		}
	}

	return 0;
}


int CSDeform::ComputeDeform(myRenderer::RENDEROBJ renderobj)
{
	if (m_workingDeform) {
		return 0;
	}
	m_workingDeform = true;

	if (!m_IMDeform || !renderobj.pmodel || !renderobj.mqoobj) {
		_ASSERT(0);
		m_workingDeform = false;
		return 0;
	}

	Matrix mWorld, mView, mProj;
	mWorld = renderobj.mWorld;
	//if ((renderobj.renderkind != RENDERKIND_SHADOWMAP)) {
		mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
		mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
	//}
	//else {
	//	//for shadow
	//	mView = g_cameraShadow->GetViewMatrix(false);
	//	mProj = g_cameraShadow->GetProjectionMatrix();
	//}


	if (m_pm3) {
		//m_cbWithoutBoneCPU.Init();
		//m_cbWithoutBoneCPU.mVertexNum[0] = m_csvertexnum;
		//m_cbWithoutBoneCPU.mWorld = mWorld;
		//m_cbWithoutBoneCPU.mView = mView;
		//m_cbWithoutBoneCPU.mProj = mProj;
		//m_cbWithoutBone.CopyToVRAM(&m_cbWithoutBoneCPU);
		//
		//m_IMDeform->Reset(m_CSPipelineState.Get());
		//m_IMDeform->SetComputeRootSignature(m_CSrootSignature);
		//m_IMDeform->SetPipelineState(m_CSPipelineState);
		//m_IMDeform->SetComputeDescriptorHeap(m_CSdescriptorHeap);
		//m_IMDeform->IMExecute((m_cscreatevertexnum / CSTHREADNUM), 1, 1);
	}
	else if (m_pm4) {
		m_cbWithBoneCPU.Init();
		m_cbWithBoneCPU.mVertexNum[0] = m_csvertexnum;
		m_cbWithBoneCPU.mWorld = mWorld;
		m_cbWithBoneCPU.mView = mView;
		m_cbWithBoneCPU.mProj = mProj;
		if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) &&
			renderobj.pmodel->ExistCurrentMotion()) { // && renderobj.pmodel->GetCSFirstDispatchFlag()) {
			//CModel::SetShaderConst()でセットしたマトリックス配列をコピーするだけ
			renderobj.pmodel->GetBoneMatrix(m_cbWithBoneCPU.setfl4x4, MAXBONENUM);
		}
		m_cbWithBone.CopyToVRAM(&m_cbWithBoneCPU);

		m_IMDeform->Reset(m_CSPipelineState.Get());
		m_IMDeform->SetComputeRootSignature(m_CSrootSignature);
		m_IMDeform->SetPipelineState(m_CSPipelineState);
		m_IMDeform->SetComputeDescriptorHeap(m_CSdescriptorHeap);
		m_IMDeform->IMExecute((m_cscreatevertexnum / CSTHREADNUM), 1, 1);

		//renderobj.pmodel->SetCSFirstDispatchFlag(false);
	}

	m_workingDeform = false;

	return 0;
}

int CSDeform::CopyCSDeform()
{
	//if (m_pm3) {
	//	if (m_vertexMap) {
	//		CSVertexWithoutBone* outputData = (CSVertexWithoutBone*)m_outputSB.GetResourceOnCPU();
	//		if (outputData) {
	//			int optvleng = m_pm3->GetOptLeng();
	//			memcpy(m_vertexMap, outputData, (sizeof(BINORMALDISPV) * optvleng));
	//		}
	//	}
	//}
	//else if (m_pm4) {
	//	if (m_vertexMap) {
	//		CSVertexWithBone* outputData = (CSVertexWithBone*)m_outputSB.GetResourceOnCPU();
	//		if (outputData) {
	//			int optvleng = m_pm4->GetOptLeng();
	//			memcpy(m_vertexMap, outputData, ((sizeof(BINORMALDISPV) + sizeof(PM3INF)) * optvleng));
	//		}
	//	}
	//}

	return 0;
}

int CSDeform::GetDeformedDispV(int srcvertindex, BINORMALDISPV* dstv)
{
	if (!dstv) {
		_ASSERT(0);
		return 1;//error
	}

	if (m_pm4) {
		if ((srcvertindex < 0) || (srcvertindex >= m_csvertexnum)) {
			_ASSERT(0);
			return 1;//error
		}

		CSVertexWithBone* outputData = (CSVertexWithBone*)m_outputSB.GetResourceOnCPU();
		if (outputData) {
			CSVertexWithBone* currentoutput = outputData + srcvertindex;
			dstv->pos = ChaVector4(currentoutput->pos[0], currentoutput->pos[1], currentoutput->pos[2], currentoutput->pos[3]);
		}
		else {
			_ASSERT(0);
			return 1;//error
		}
	}
	else {
		return 1;//error
	}

	return 0;//success
}


int CSDeform::PickRay(ChaVector3 startglobal, ChaVector3 dirglobal,
	bool excludeinvface, int* hitfaceindex)
{
	if (m_workingPick) {
		return GetResultOfPickRay(hitfaceindex);
	}
	m_workingPick = true;

	if (!m_IMPick || !hitfaceindex) {
		_ASSERT(0);
		m_workingPick = false;
		return 0;
	}

	*hitfaceindex = -1;

	//if (m_pm3 || m_pm4) {
	if (m_pm4) {
		m_cbPickCPU.Init();
		//int mBufferSize[4];//0:vertexnum, 1:facenum, 2:indicesnum, 3:未使用0
		//float mStartglobal[4];//start point of PickRay
		//float mDirglobal[4];//direction of PickRay
		//int mFlags[4];//0:excludeinvface, 1-3:未使用0

		m_cbPickCPU.mBufferSize[0] = m_csvertexnum;
		m_cbPickCPU.mBufferSize[1] = m_csfacenum;
		m_cbPickCPU.mBufferSize[2] = m_csfacenum * 3;
		m_cbPickCPU.mBufferSize[3] = 0;
		m_cbPickCPU.mStartglobal[0] = startglobal.x;
		m_cbPickCPU.mStartglobal[1] = startglobal.y;
		m_cbPickCPU.mStartglobal[2] = startglobal.z;
		m_cbPickCPU.mStartglobal[3] = 0.0f;
		m_cbPickCPU.mDirglobal[0] = dirglobal.x;
		m_cbPickCPU.mDirglobal[1] = dirglobal.y;
		m_cbPickCPU.mDirglobal[2] = dirglobal.z;
		m_cbPickCPU.mDirglobal[3] = 0.0f;
		m_cbPickCPU.mFlags[0] = excludeinvface ? 0 : 1;//exclusiveinv --> allowinv
		m_cbPickCPU.mFlags[1] = 0;
		m_cbPickCPU.mFlags[2] = 0;
		m_cbPickCPU.mFlags[3] = 0;
		m_cbPick.CopyToVRAM(&m_cbPickCPU);


		//結果を初期化
		CSPickResult* outputData = (CSPickResult*)m_outputPickSB1.GetResourceOnCPU();//要素数1
		if (outputData) {
			outputData->result[0] = 0;
			outputData->result[1] = 0;
			outputData->result[2] = 0;
			outputData->result[3] = 0;
			outputData->dbginfo[0] = 0;
			outputData->dbginfo[1] = 0;
			outputData->dbginfo[2] = 0;
			outputData->dbginfo[3] = 0;
		}

		m_IMPick->Reset(m_CSPickPipelineState.Get());
		m_IMPick->SetComputeRootSignature(m_CSPickrootSignature);
		m_IMPick->SetPipelineState(m_CSPickPipelineState);
		m_IMPick->SetComputeDescriptorHeap(m_CSPickdescriptorHeap);
		m_IMPick->IMExecute((m_cscreatefacenum / CSTHREADNUM), 1, 1);

		//#############################################
		//結果はrc->EndFrame()より後にならないと分からない
		// EndFrame()より後でGetResultOfPickRay()を呼ぶ
		//#############################################

		m_pickstate = 1;
	}

	m_workingPick = false;

	//計算結果はすぐにコピーしておく
	CSPickResult* outputData1 = (CSPickResult*)m_outputPickSB1.GetResourceOnCPU();//要素数1
	if (outputData1) {
		m_cspickOutPut1_save = *outputData1;
	}


	return GetResultOfPickRay(hitfaceindex);
}

int CSDeform::GetResultOfPickRay(int* hitfaceindex)
{
	if (!hitfaceindex) {
		_ASSERT(0);
		return 0;
	}
	*hitfaceindex = -1;

	if (m_pickstate != 1) {
		return 0;
	}

	int hitflag = m_cspickOutPut1_save.result[0];//要素数1
	int justflag = m_cspickOutPut1_save.result[1];//要素数1
	int hitface = m_cspickOutPut1_save.result[2];//要素数1
	if ((hitflag == 1) && (hitface >= 0)) {
		//当たりをみつけた場合
		*hitfaceindex = hitface;
		m_pickstate = 2;
		return 1;//!!!!!!!!!!!!!!
	}

	m_pickstate = 2;
	return 0;
}

