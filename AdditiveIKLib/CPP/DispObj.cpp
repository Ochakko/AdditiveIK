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

#include <DispObj.h>

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
#include <CSDeform.h>


#include "../../MiniEngine/ConstantBuffer.h"
#include "../../MiniEngine/StructuredBuffer.h"
#include "../../MiniEngine/DescriptorHeap.h"
//#include "../../MiniEngine/Material.h"



#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>


using namespace std;
/*
extern ID3D11DepthStencilState *g_pDSStateZCmp;
extern ID3D11DepthStencilState *g_pDSStateZCmpAlways;
extern ID3D11ShaderResourceView* g_presview;

extern int g_dbgflag;
extern CTexBank* g_texbank;

extern ID3D11Effect*		g_pEffect;
extern ID3DX11EffectTechnique* g_hRenderBoneL0;
extern ID3DX11EffectTechnique* g_hRenderBoneL1;
extern ID3DX11EffectTechnique* g_hRenderBoneL2;
extern ID3DX11EffectTechnique* g_hRenderBoneL3;
extern ID3DX11EffectTechnique* g_hRenderNoBoneL0;
extern ID3DX11EffectTechnique* g_hRenderNoBoneL1;
extern ID3DX11EffectTechnique* g_hRenderNoBoneL2;
extern ID3DX11EffectTechnique* g_hRenderNoBoneL3;
extern ID3DX11EffectTechnique* g_hRenderLine;
extern ID3DX11EffectTechnique* g_hRenderSprite;

extern ID3D11EffectMatrixVariable* g_hm4x4Mat;
extern ID3D11EffectMatrixVariable* g_hmWorld;
extern ID3D11EffectMatrixVariable* g_hmVP;

extern ID3D11EffectVectorVariable* g_hEyePos;
extern ID3D11EffectScalarVariable* g_hnNumLight;
extern ID3D11EffectVectorVariable* g_hLightDir;
extern ID3D11EffectVectorVariable* g_hLightDiffuse;
extern ID3D11EffectVectorVariable* g_hLightAmbient;

extern ID3D11EffectVectorVariable*g_hdiffuse;
extern ID3D11EffectVectorVariable* g_hambient;
extern ID3D11EffectVectorVariable* g_hspecular;
extern ID3D11EffectScalarVariable* g_hpower;
extern ID3D11EffectVectorVariable* g_hemissive;
extern ID3D11EffectShaderResourceVariable* g_hMeshTexture;

extern ID3D11EffectVectorVariable* g_hPm3Scale;
extern ID3D11EffectVectorVariable* g_hPm3Offset;


extern int	g_nNumActiveLights;
*/

extern bool g_zcmpalways;



CDispObj::CDispObj() 
{
	InitParams();
}
CDispObj::~CDispObj()
{
	DestroyObjs();
}
int CDispObj::InitParams()
{
	m_tmpindexLH = 0;

	ResetScaleInstancing();

	m_hasbone = 0;

	m_pdev = 0;
	m_pm3 = 0;//外部メモリ
	m_pm4 = 0;
	m_extline = 0;

	m_vertexBuffer = nullptr;		//頂点バッファ。
	ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));	//頂点バッファビュー。
	m_vertexMap = nullptr;

	m_indexBuffer = nullptr;	//インデックスバッファ。
	ZeroMemory(&m_indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));	//インデックスバッファビュー。

	m_csdeform = nullptr;

	return 0;
}
int CDispObj::DestroyObjs()
{

	if (m_tmpindexLH) {
		free(m_tmpindexLH);
		m_tmpindexLH = 0;
	}

	if (m_vertexBuffer) {
		m_vertexBuffer->Unmap(0, nullptr);
		m_vertexBuffer->Release();
	}
	if (m_indexBuffer) {
		m_indexBuffer->Unmap(0, nullptr);
		m_indexBuffer->Release();
	}

	m_InstancingBuffer.DestroyObjs();


	if (m_csdeform) {
		delete m_csdeform;
		m_csdeform = nullptr;
	}


	return 0;
}


int CDispObj::CreateDispObj(ID3D12Device* pdev, CPolyMesh3* pm3, int hasbone, int srcuvnum)
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm3 = pm3;

	CallF(CreateVBandIB(pdev), return 1);


	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,//for SV_Target0
		DXGI_FORMAT_R32G32B32A32_FLOAT,//for SV_Target1
		//DXGI_FORMAT_R8G8B8A8_UNORM,
		//DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
	};	//レンダリングするカラーバッファのフォーマット。


	//int MateiralNo = 0;//!!!!!! shaderとpipelineは１メッシュにつき１つにした
	int materialNum = m_pm3->GetOptMatNum();
	int rootindex = 0;
	int blno;
	for (blno = 0; blno < m_pm3->GetOptMatNum(); blno++) {
		MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;
		CMQOMaterial* curmat;
		curmat = currb->mqomat;
		bool withboneflag = false;
		int vertextype = 1;//pm3
		if (curmat) {
			curmat->CreateDecl(pdev, vertextype);

			int result1;
			result1 = curmat->InitShadersAndPipelines(
				srcuvnum,
				vertextype,
				"../Media/Shader/AdditiveIK_NoSkin_PBR.fx",//fx NoSkin PBR
				"../Media/Shader/AdditiveIK_NoSkin_Std.fx",//fx NoSkin Std
				"../Media/Shader/AdditiveIK_NoSkin_Std.fx",//fx NoSkin NoLight
			//###
			//VS
			//###
				//VS NoSkin PBR
				"VSMainNoSkinPBR",
				"VSMainNoSkinPBRShadowMap",
				"VSMainNoSkinPBRShadowReciever",

				//VS NoSkin Std
				"VSMainNoSkinStd",
				"VSMainNoSkinStdShadowMap",
				"VSMainNoSkinStdShadowReciever",

				//VS NoSkin NoLight
				"VSMainNoSkinStd",
				"VSMainNoSkinStdShadowMap",
				"VSMainNoSkinStdShadowReciever",

			//###
			//PS
			//###
				//PS NoSkin PBR
				"PSMainNoSkinPBR",
				"PSMainNoSkinPBRShadowMap",
				"PSMainNoSkinPBRShadowReciever",

				//PS NoSkin Std
				"PSMainNoSkinStd",
				"PSMainNoSkinStdShadowMap",
				"PSMainNoSkinStdShadowReciever",

				//PS NoSkin NoLight
				"PSMainNoSkinNoLight",
				"PSMainNoSkinStdShadowMap",
				"PSMainNoSkinNoLightShadowReciever",

				colorBufferFormat,
				curmat->NUM_SRV_ONE_MATERIAL,
				curmat->NUM_CBV_ONE_MATERIAL,
				0, //curmat->NUM_CBV_ONE_MATERIAL * rootindex,//offset
				0, //curmat->NUM_SRV_ONE_MATERIAL * rootindex,//offset
				D3D12_FILTER_MIN_MAG_MIP_LINEAR
			);
			if (result1 != 0) {
				_ASSERT(0);
				std::abort();//2024/03/17 シェーダコンパイル失敗時にはabort()する
			}

			//int result2;
			//result2 = curmat->InitZPreShadersAndPipelines(
			//	vertextype,
			//	"../Media/Shader/AdditiveIK_NoSkin_ZPrepass.fx",
			//	"VSMainZPrepass",
			//	"PSMainZPrepass",
			//	colorBufferFormat,
			//	curmat->NUM_SRV_ONE_MATERIAL,
			//	curmat->NUM_CBV_ONE_MATERIAL,
			//	0, //curmat->NUM_CBV_ONE_MATERIAL * rootindex,//offset
			//	0, //curmat->NUM_SRV_ONE_MATERIAL * rootindex,//offset
			//	D3D12_FILTER_MIN_MAG_MIP_LINEAR
			//);
			//if (result2 != 0) {
			//	_ASSERT(0);
			//	std::abort();//2024/03/17 シェーダコンパイル失敗時にはabort()する
			//}

			int result3;
			result3 = curmat->InitInstancingShadersAndPipelines(
				vertextype,
				"../Media/Shader/AdditiveIK_NoSkin_Instancing.fx",
				"VSMainNoSkinInstancing",
				"PSMainNoSkinInstancingNoLight",
				colorBufferFormat,
				curmat->NUM_SRV_ONE_MATERIAL,
				curmat->NUM_CBV_ONE_MATERIAL,
				0, //curmat->NUM_CBV_ONE_MATERIAL * rootindex,//offset
				0, //curmat->NUM_SRV_ONE_MATERIAL * rootindex,//offset
				D3D12_FILTER_MIN_MAG_MIP_LINEAR
			);
			if (result3 != 0) {
				_ASSERT(0);
				std::abort();//2024/03/17 シェーダコンパイル失敗時にはabort()する
			}

			rootindex++;
		}
	}

	m_csdeform = new CSDeform();
	if (!m_csdeform) {
		_ASSERT(0);
		return 1;
	}
	CallF(m_csdeform->CreateDispObj(pdev, pm3), return 1);


	return 0;
}
int CDispObj::CreateDispObj(ID3D12Device* pdev, CPolyMesh4* pm4, int hasbone, int srcuvnum)
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm4 = pm4;

	CallF( CreateVBandIB(pdev), return 1 );

	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,//for SV_Target0
		DXGI_FORMAT_R32G32B32A32_FLOAT,//for SV_Target1
		//DXGI_FORMAT_R8G8B8A8_UNORM,
		//DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
	};	//レンダリングするカラーバッファのフォーマット。


	int materialnum = m_pm4->GetDispMaterialNum();
	int materialcnt;
	int rootindex = 0;
	for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
		CMQOMaterial* curmat = NULL;
		int curoffset = 0;
		int curtrinum = 0;
		bool withboneflag = true;
		int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
		if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0)) {
			if (curmat) {
				int vertextype = 0;//pm4

				curmat->CreateDecl(pdev, vertextype);

				int result1;
				result1 = curmat->InitShadersAndPipelines(
					srcuvnum,
					vertextype,
					"../Media/Shader/AdditiveIK_Skin_PBR.fx",//fx Skin PBR
					"../Media/Shader/AdditiveIK_Skin_Std.fx",//fx Skin Std
					"../Media/Shader/AdditiveIK_Skin_Std.fx",//fx Skin NoLight
					//###
					//VS
					//###
						//VS Skin PBR
					"VSMainSkinPBR",
					"VSMainSkinPBRShadowMap",
					"VSMainSkinPBRShadowReciever",

					//VS Skin Std
					"VSMainSkinStd",
					"VSMainSkinStdShadowMap",
					"VSMainSkinStdShadowReciever",

					//VS Skin NoLight
					"VSMainSkinStd",
					"VSMainSkinStdShadowMap",
					"VSMainSkinStdShadowReciever",

					//###
					//PS
					//###
						//PS Skin PBR
					"PSMainSkinPBR",
					"PSMainSkinPBRShadowMap",
					"PSMainSkinPBRShadowReciever",

					//PS Skin Std
					"PSMainSkinStd",
					"PSMainSkinStdShadowMap",
					"PSMainSkinStdShadowReciever",

					//PS Skin NoLight
					"PSMainSkinNoLight",
					"PSMainSkinStdShadowMap",
					"PSMainSkinNoLightShadowReciever",

					colorBufferFormat,
					curmat->NUM_SRV_ONE_MATERIAL,
					curmat->NUM_CBV_ONE_MATERIAL,
					0, //curmat->NUM_CBV_ONE_MATERIAL * rootindex,//offset
					0, //curmat->NUM_SRV_ONE_MATERIAL * rootindex,//offset
					D3D12_FILTER_MIN_MAG_MIP_LINEAR
				);
				if (result1 != 0) {
					_ASSERT(0);
					std::abort();//2024/03/17 シェーダコンパイル失敗時にはabort()する
				}


				//int result2;
				//result2 = curmat->InitZPreShadersAndPipelines(
				//	vertextype,
				//	"../Media/Shader/AdditiveIK_Skin_ZPrepass.fx",
				//	"VSMainZPrepass",
				//	"PSMainZPrepass",
				//	colorBufferFormat,
				//	curmat->NUM_SRV_ONE_MATERIAL,
				//	curmat->NUM_CBV_ONE_MATERIAL,
				//	0, //curmat->NUM_CBV_ONE_MATERIAL * rootindex,//offset
				//	0, //curmat->NUM_SRV_ONE_MATERIAL * rootindex,//offset
				//	D3D12_FILTER_MIN_MAG_MIP_LINEAR
				//);
				//if (result2 != 0) {
				//	_ASSERT(0);
				//	std::abort();//2024/03/17 シェーダコンパイル失敗時にはabort()する
				//}


				rootindex++;
			}
		}
	}

	m_csdeform = new CSDeform();
	if (!m_csdeform) {
		_ASSERT(0);
		return 1;
	}
	CallF(m_csdeform->CreateDispObj(pdev, pm4), return 1);


	return 0;
}

int CDispObj::CreateDispObj( ID3D12Device* pdev, CExtLine* extline )
{
	DestroyObjs();

	m_hasbone = 0;

	m_pdev = pdev;
	m_extline = extline;

	//CallF( CreateVBandIBLine(pdev), return 1 );
	CallF(CreateVBandIB(pdev), return 1);


	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,//for SV_Target0
		DXGI_FORMAT_R32G32B32A32_FLOAT,//for SV_Target1
		//DXGI_FORMAT_R8G8B8A8_UNORM,
		//DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
	};	//レンダリングするカラーバッファのフォーマット。


	int materialNum = 1;
	int rootindex = 0;

	CMQOMaterial* curmat = m_extline->GetMaterial();
	if (curmat) {
		int vertextype = 2;//extline

		curmat->CreateDecl(pdev, vertextype);

		int result1;
		result1 = curmat->InitShadersAndPipelines(
			0,
			vertextype,
			"../Media/Shader/AdditiveIK_NoSkin_Std.fx",
			"../Media/Shader/AdditiveIK_NoSkin_Std.fx",
			"../Media/Shader/AdditiveIK_NoSkin_Std.fx",

			"VSMainExtLine",
			"VSMainExtLine",
			"VSMainExtLine",

			"VSMainExtLine",
			"VSMainExtLine",
			"VSMainExtLine",

			"VSMainExtLine",
			"VSMainExtLine",
			"VSMainExtLine",


			"PSMainExtLine",
			"PSMainExtLine",
			"PSMainExtLine",

			"PSMainExtLine",
			"PSMainExtLine",
			"PSMainExtLine",

			"PSMainExtLine",
			"PSMainExtLine",
			"PSMainExtLine",

			colorBufferFormat,
			curmat->NUM_SRV_ONE_MATERIAL,
			curmat->NUM_CBV_ONE_MATERIAL,
			0, //curmat->NUM_CBV_ONE_MATERIAL * rootindex,//offset
			0, //curmat->NUM_SRV_ONE_MATERIAL * rootindex,//offset
			D3D12_FILTER_MIN_MAG_MIP_LINEAR
		);
		if (result1 != 0) {
			_ASSERT(0);
			std::abort();//2024/03/17 シェーダコンパイル失敗時にはabort()する
		}

		rootindex++;
	}

	return 0;
}

//void MeshParts::CreateMeshFromTkmMesh(
//	const TkmFile::SMesh& tkmMesh,
//	int meshNo,
//	int& materialNum,
//	const char* fxFilePath,
//	const char* vsEntryPointFunc,
//	const char* vsSkinEntryPointFunc,
//	const char* psEntryPointFunc,
//	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
//	D3D12_FILTER samplerFilter
//)
//{
//	//1. 頂点バッファを作成。
//	int numVertex = (int)tkmMesh.vertexBuffer.size();
//	int vertexStride = sizeof(TkmFile::SVertex);
//	auto mesh = new SMesh;
//	mesh->skinFlags.reserve(tkmMesh.materials.size());
//	mesh->m_vertexBuffer.Init(vertexStride * numVertex, vertexStride);
//	mesh->m_vertexBuffer.Copy((void*)&tkmMesh.vertexBuffer[0]);
//
//	auto SetSkinFlag = [&](int index) {
//		if (tkmMesh.vertexBuffer[index].skinWeights.x > 0.0f) {
//			//スキンがある。
//			mesh->skinFlags.push_back(1);
//		}
//		else {
//			//スキンなし。
//			mesh->skinFlags.push_back(0);
//		}
//		};
//	//2. インデックスバッファを作成。
//	if (!tkmMesh.indexBuffer16Array.empty()) {
//		//インデックスのサイズが2byte
//		mesh->m_indexBufferArray.reserve(tkmMesh.indexBuffer16Array.size());
//		for (auto& tkIb : tkmMesh.indexBuffer16Array) {
//			auto ib = new IndexBuffer;
//			ib->Init(static_cast<int>(tkIb.indices.size()) * 2, 2);
//			ib->Copy((uint16_t*)&tkIb.indices.at(0));
//
//			//スキンがあるかどうかを設定する。
//			SetSkinFlag(tkIb.indices[0]);
//
//			mesh->m_indexBufferArray.push_back(ib);
//		}
//	}
//	else {
//		//インデックスのサイズが4byte
//		mesh->m_indexBufferArray.reserve(tkmMesh.indexBuffer32Array.size());
//		for (auto& tkIb : tkmMesh.indexBuffer32Array) {
//			auto ib = new IndexBuffer;
//			ib->Init(static_cast<int>(tkIb.indices.size()) * 4, 4);
//			ib->Copy((uint32_t*)&tkIb.indices.at(0));
//
//			//スキンがあるかどうかを設定する。
//			SetSkinFlag(tkIb.indices[0]);
//
//			mesh->m_indexBufferArray.push_back(ib);
//		}
//	}
//	//3. マテリアルを作成。
//	mesh->m_materials.reserve(tkmMesh.materials.size());
//	for (auto& tkmMat : tkmMesh.materials) {
//		auto mat = new Material;
//		mat->InitFromTkmMaterila(
//			tkmMat,
//			fxFilePath,
//			vsEntryPointFunc,
//			vsSkinEntryPointFunc,
//			psEntryPointFunc,
//			colorBufferFormat,
//			NUM_SRV_ONE_MATERIAL,
//			NUM_CBV_ONE_MATERIAL,
//			NUM_CBV_ONE_MATERIAL * materialNum,
//			NUM_SRV_ONE_MATERIAL * materialNum,
//			samplerFilter
//		);
//		//作成したマテリアル数をカウントする。
//		materialNum++;
//		mesh->m_materials.push_back(mat);
//	}
//
//	m_meshs[meshNo] = mesh;
//
//}

int CDispObj::CreateVBandIB(ID3D12Device* pdev)
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
	}
	else {
		_ASSERT(0);
		return 1;
	}


//###########
//頂点バッファ
//###########
	{
		//auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(vbsize);
		HRESULT hrvb0 = pdev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&rDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer));
		if (FAILED(hrvb0) || !m_vertexBuffer) {
			::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
				"CreateVertexBuffer Error", MB_OK | MB_ICONERROR);
			abort();
		}

		m_vertexBuffer->SetName(L"VertexBuffer");
		//頂点バッファのビューを作成。
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.SizeInBytes = vbsize;
		m_vertexBufferView.StrideInBytes = stride;


		//インスタンシングバッファ
		m_InstancingBuffer.Init((sizeof(INSTANCINGPARAMS) * RIGMULTINDEXMAX), sizeof(INSTANCINGPARAMS));




		//頂点バッファをコピー.
		//uint8_t* pData;
		m_vertexBuffer->Map(0, nullptr, (void**)&m_vertexMap);
		if (m_pm3) {
			memcpy(m_vertexMap, pm3v, m_vertexBufferView.SizeInBytes);
		}
		else if (m_pm4) {
			DWORD vno;
			for (vno = 0; vno < (DWORD)pmvleng; vno++) {
				uint8_t* pdest = m_vertexMap + vno * (sizeof(BINORMALDISPV) + sizeof(PM3INF));
				
				BINORMALDISPV* curv = pm4v + vno;
				PM3INF* curinf = pmib + vno;


				memcpy(pdest, curv, sizeof(BINORMALDISPV));
				memcpy(pdest + sizeof(BINORMALDISPV), curinf, sizeof(PM3INF));
			}
			//memcpy(pData, pmv, m_vertexBufferView.SizeInBytes);

		}
		else if (m_extline) {
			memcpy(m_vertexMap, plinev, m_vertexBufferView.SizeInBytes);
		}
		else {
			_ASSERT(0);
			//m_vertexBuffer->Unmap(0, nullptr);
			return 1;
		}
		//m_vertexBuffer->Unmap(0, nullptr);
	}

//###################
//インデックスバッファ
//###################
	{
		if (m_pm3 || m_pm4) {
			//auto d3dDevice = g_graphicsEngine->GetD3DDevice();
			DWORD ibsize = pmfleng * 3 * sizeof(int);
			auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(ibsize);
			HRESULT hrib0 = pdev->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&rDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_indexBuffer));
			if (FAILED(hrib0) || !m_indexBuffer) {
				::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
					"CreateIndexBuffer Error", MB_OK | MB_ICONERROR);
				abort();
			}

			m_indexBuffer->SetName(L"DispOjb:indexBuffer");


			//インデックスバッファのビューを作成。
			m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_indexBufferView.SizeInBytes = ibsize;

			//インデックスバッファをコピー。
			uint32_t* pData;
			//DWORD triangleno;
			m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
			if (m_pm3) {
				//for (triangleno = 0; triangleno < (DWORD)pmfleng; triangleno++) {
				//	*(pData + triangleno * 3) = *(m_pm3->GetDispIndex() + triangleno * 3);
				//	*(pData + triangleno * 3 + 1) = *(m_pm3->GetDispIndex() + triangleno * 3 + 1);
				//	*(pData + triangleno * 3 + 2) = *(m_pm3->GetDispIndex() + triangleno * 3 + 2);
				//}
				MoveMemory(pData, m_pm3->GetDispIndex(), pmfleng * 3 * sizeof(int));
			}
			else if (m_pm4) {
				//for (triangleno = 0; triangleno < (DWORD)pmfleng; triangleno++) {
				//	*(pData + triangleno * 3) = *(m_pm4->GetDispIndex() + triangleno * 3);
				//	*(pData + triangleno * 3 + 1) = *(m_pm4->GetDispIndex() + triangleno * 3 + 1);
				//	*(pData + triangleno * 3 + 2) = *(m_pm4->GetDispIndex() + triangleno * 3 + 2);
				//}
				MoveMemory(pData, m_pm4->GetDispIndex(), pmfleng * 3 * sizeof(int));
			}
			else {
				_ASSERT(0);
				//m_indexBuffer->Unmap(0, nullptr);
				return 1;
			}
			//m_indexBuffer->Unmap(0, nullptr);
		}
		else if (m_extline) {
			DWORD ibsize = pmfleng * 2 * sizeof(int);
			auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(ibsize);
			HRESULT hrib1 = pdev->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&rDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_indexBuffer));
			if (FAILED(hrib1) || !m_indexBuffer) {
				::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
					"CreateIndexBuffer Error", MB_OK | MB_ICONERROR);
				abort();
			}

			m_indexBuffer->SetName(L"DispOjb:indexBuffer");

			//インデックスバッファのビューを作成。
			m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_indexBufferView.SizeInBytes = ibsize;

			//インデックスバッファをコピー。
			uint32_t* pData;
			//DWORD triangleno;
			m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));

			DWORD lineno;
			for (lineno = 0; lineno < (DWORD)pmfleng; lineno++) {
				*(pData + lineno * 2) = lineno * 2;
				*(pData + lineno * 2 + 1) = lineno * 2 + 1;
			}
			//m_indexBuffer->Unmap(0, nullptr);
		}
	}




	//m_BufferDescBone.ByteWidth = pmvleng * sizeof(PM3DISPV);
	//m_BufferDescBone.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//m_BufferDescBone.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_BufferDescBone.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	//m_BufferDescBone.MiscFlags = 0;

	//m_BufferDescInf.ByteWidth = pmvleng * sizeof(PM3INF);
	//m_BufferDescInf.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//m_BufferDescInf.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_BufferDescInf.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
	//m_BufferDescInf.MiscFlags = 0;


	//m_BufferDescNoBone.ByteWidth = pmvleng * sizeof(PM3DISPV);
	//m_BufferDescNoBone.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//m_BufferDescNoBone.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_BufferDescNoBone.CPUAccessFlags = 0;
	//m_BufferDescNoBone.MiscFlags = 0;


	//if (m_hasbone) {
	//	D3D11_SUBRESOURCE_DATA SubData;
	//	SubData.pSysMem = pmv;
	//	SubData.SysMemPitch = 0;
	//	SubData.SysMemSlicePitch = 0;
	//	hr = m_pdev->CreateBuffer(&m_BufferDescBone, &SubData, &m_VB);
	//	if (FAILED(hr)) {
	//		_ASSERT(0);
	//		return 1;
	//	}

	//	if (m_pm4) {
	//		D3D11_SUBRESOURCE_DATA SubDataInf;
	//		SubDataInf.pSysMem = m_pm4->GetPm3Inf();
	//		SubDataInf.SysMemPitch = 0;
	//		SubDataInf.SysMemSlicePitch = 0;
	//		hr = m_pdev->CreateBuffer(&m_BufferDescInf, &SubDataInf, &m_InfB);
	//		if (FAILED(hr)) {
	//			_ASSERT(0);
	//			return 1;
	//		}
	//	}
	//	else {
	//		_ASSERT(0);
	//	}
	//}
	//else {
	//	D3D11_SUBRESOURCE_DATA SubData;
	//	SubData.pSysMem = pmv;
	//	SubData.SysMemPitch = 0;
	//	SubData.SysMemSlicePitch = 0;
	//	hr = m_pdev->CreateBuffer(&m_BufferDescNoBone, &SubData, &m_VB);
	//	if (FAILED(hr)) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//}

	////IndexBuffer
	//D3D11_BUFFER_DESC IndexBufferDesc;
	//D3D11_SUBRESOURCE_DATA SubDataIndex;

	//IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//IndexBufferDesc.ByteWidth = pmfleng * 3 * sizeof(int);
	//IndexBufferDesc.CPUAccessFlags = 0;
	//IndexBufferDesc.MiscFlags = 0;

	//SubDataIndex.SysMemPitch = 0;
	//SubDataIndex.SysMemSlicePitch = 0;

	////if (m_tmpindexLH) {
	////	free(m_tmpindexLH);
	////	m_tmpindexLH = 0;
	////}
	////m_tmpindexLH = (int*)malloc(pmfleng * 3 * sizeof(int));
	////if (!m_tmpindexLH) {
	////	_ASSERT(0);
	////	return 1;
	////}
	//if (m_pm3) {
	//	//int cptricnt;
	//	//for (cptricnt = 0; cptricnt < pmfleng; cptricnt++) {
	//	//	*(m_tmpindexLH + cptricnt * 3) = *(m_pm3->GetDispIndex() + cptricnt * 3);
	//	//	*(m_tmpindexLH + cptricnt * 3 + 1) = *(m_pm3->GetDispIndex() + cptricnt * 3 + 2);
	//	//	*(m_tmpindexLH + cptricnt * 3 + 2) = *(m_pm3->GetDispIndex() + cptricnt * 3 + 1);
	//	//}
	//	//SubDataIndex.pSysMem = m_tmpindexLH;
	//	SubDataIndex.pSysMem = m_pm3->GetDispIndex();
	//}
	//else if (m_pm4) {
	//	//int cptricnt;
	//	//for (cptricnt = 0; cptricnt < pmfleng; cptricnt++) {
	//	//	*(m_tmpindexLH + cptricnt * 3) = *(m_pm4->GetDispIndex() + cptricnt * 3);
	//	//	*(m_tmpindexLH + cptricnt * 3 + 1) = *(m_pm4->GetDispIndex() + cptricnt * 3 + 2);
	//	//	*(m_tmpindexLH + cptricnt * 3 + 2) = *(m_pm4->GetDispIndex() + cptricnt * 3 + 1);
	//	//}
	//	//SubDataIndex.pSysMem = m_tmpindexLH;
	//	SubDataIndex.pSysMem = m_pm4->GetDispIndex();
	//}
	//else {
	//	IndexBufferDesc.ByteWidth = 0;
	//	SubDataIndex.pSysMem = 0;
	//	_ASSERT(0);
	//}

	//hr = m_pdev->CreateBuffer(&IndexBufferDesc, &SubDataIndex, &m_IB);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}

	return 0;
}

//int CDispObj::CreateVBandIBLine(ID3D12Device* pdev)
//{
//	//HRESULT hr;
//
//	UINT elemleng;
//	//DWORD curFVF;
//
//	elemleng = sizeof( EXTLINEV );
//
//
//	//m_BufferDescLine.ByteWidth = m_extline->m_linenum * 2 * sizeof(EXTLINEV);
//	//m_BufferDescLine.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!
//	//m_BufferDescLine.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	//m_BufferDescLine.CPUAccessFlags = 0;
//	//m_BufferDescLine.MiscFlags = 0;
//
//
//	//D3D11_SUBRESOURCE_DATA SubData;
//	//SubData.pSysMem = m_extline->m_linev;
//	//SubData.SysMemPitch = 0;
//	//SubData.SysMemSlicePitch = 0;
//	//hr = m_pdev->CreateBuffer(&m_BufferDescLine, &SubData, &m_VB);
//	//if (FAILED(hr)) {
//	//	_ASSERT(0);
//	//	return 1;
//	//}
//
//
//	//////IndexBuffer
//	////D3D11_BUFFER_DESC IndexBufferDesc;
//	////D3D11_SUBRESOURCE_DATA SubDataIndex;
//
//	////IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	////IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	////IndexBufferDesc.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
//	////IndexBufferDesc.MiscFlags = 0;
//
//	////SubDataIndex.SysMemPitch = 0;
//	////SubDataIndex.SysMemSlicePitch = 0;
//
//	////IndexBufferDesc.ByteWidth = m_extline->m_linenum * 2 * sizeof(int);
//	////SubDataIndex.pSysMem = m_extline->m_linev;
//
//	////hr = m_pdev->CreateBuffer(&IndexBufferDesc, &SubDataIndex, &m_IB);
//	////if (FAILED(hr)) {
//	////	_ASSERT(0);
//	////	return 1;
//	////}
//
//
//	return 0;
//}


int CDispObj::RenderShadowMap(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc) {
		_ASSERT(0);
		return 1;
	}
	renderobj.renderkind = RENDERKIND_SHADOWMAP;
	return RenderNormal(rc, renderobj);



	//renderobj.renderkind = RENDERKIND_SHADOWMAP;
	//renderobj.shadertype = MQOSHADER_TOON;
	//renderobj.forcewithalpha = false;
	//renderobj.withalpha = false;
	//renderobj.lightflag = false;

	//if (!rc) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//
	//if (!m_pm4) {
	//	_ASSERT(0);
	//	return 0;
	//}
	//
	//int materialnum = m_pm4->GetDispMaterialNum();
	//if (materialnum <= 0) {
	//	_ASSERT(0);
	//	return 0;
	//}
	//
	//int totaltrinum = m_pm4->GetFaceNum();
	//if (totaltrinum > 0) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//	//1. 頂点バッファを設定。
	//	rc->SetVertexBuffer(m_vertexBufferView);
	//	//3. インデックスバッファを設定。
	//	rc->SetIndexBuffer(m_indexBufferView);
	//
	//
	//	//###############################
	//	//### 最初のマテリアルで全部描画する
	//	//###############################
	//	int materialcnt = 0;
	//	CMQOMaterial* curmat = NULL;
	//	int curoffset = 0;
	//	int curtrinum = 0;
	//	int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
	//	if ((result0 == 0) && (curmat != NULL) && curmat->GetShadowCasterFlag()) {
	//
	//		Matrix mView, mProj;
	//		//for shadow
	//		mView = g_cameraShadow->GetViewMatrix(false);
	//		mProj = g_cameraShadow->GetProjectionMatrix();
	//
	//		//定数バッファの設定、更新など描画の共通処理を実行する。
	//		int refposindex = 0;//!!!!!!!!!
	//		curmat->DrawCommon(rc, renderobj, mView, mProj, refposindex);
	//		curmat->BeginRender(rc, renderobj, refposindex);
	//		//4. ドローコールを実行。
	//		//rc->DrawIndexed(curtrinum * 3, curoffset);
	//		rc->DrawIndexed(totaltrinum * 3, 0);
	//	}
	//}

	return 0;
}
int CDispObj::RenderShadowMapPM3(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc) {
		_ASSERT(0);
		return 1;
	}
	renderobj.renderkind = RENDERKIND_SHADOWMAP;
	return RenderNormalPM3(rc, renderobj);

	
	//renderobj.renderkind = RENDERKIND_SHADOWMAP;
	//renderobj.shadertype = MQOSHADER_TOON;
	//renderobj.forcewithalpha = false;
	//renderobj.withalpha = false;
	//renderobj.lightflag = false;
	//
	//if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
	//	_ASSERT(0);
	//	return 0;
	//}
	//
	//if (!m_pm3) {
	//	return 0;
	//}
	//if (m_pm3->GetCreateOptFlag() == 0) {
	//	return 0;
	//}
	//
	//int totaltrinum = m_pm3->GetFaceNum();
	//if (totaltrinum > 0) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//	//1. 頂点バッファを設定。
	//	rc->SetVertexBuffer(m_vertexBufferView);
	//	//3. インデックスバッファを設定。
	//	rc->SetIndexBuffer(m_indexBufferView);
	//
	//
	////###############################
	////### 最初のマテリアルで全部描画する
	////###############################
	//	int blno = 0;
	//	MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;
	//	CMQOMaterial* curmat;
	//	curmat = currb->mqomat;
	//	if (!curmat) {
	//		//_ASSERT(0);
	//		return 1;
	//	}
	//
	//	if (curmat->GetShadowCasterFlag()) {
	//		Matrix mView, mProj;
	//		//for shadow
	//		mView = g_cameraShadow->GetViewMatrix(false);
	//		mProj = g_cameraShadow->GetProjectionMatrix();
	//
	//		//定数バッファの設定、更新など描画の共通処理を実行する。
	//		int refposindex = 0;//!!!!!!!!!
	//		curmat->DrawCommon(rc, renderobj, mView, mProj, refposindex);
	//		curmat->BeginRender(rc, renderobj, refposindex);
	//
	//		//4. ドローコールを実行。
	//		rc->DrawIndexed(totaltrinum * 3, 0);
	//	}
	//}
	//return 0;
}
int CDispObj::RenderShadowReciever(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc) {
		_ASSERT(0);
		return 1;
	}
	renderobj.renderkind = RENDERKIND_SHADOWRECIEVER;
	return RenderNormal(rc, renderobj);
}
int CDispObj::RenderShadowRecieverPM3(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc) {
		_ASSERT(0);
		return 1;
	}
	renderobj.renderkind = RENDERKIND_SHADOWRECIEVER;
	return RenderNormalPM3(rc, renderobj);
}

int CDispObj::ComputeDeform(myRenderer::RENDEROBJ renderobj)
{
	if (!m_csdeform || !renderobj.pmodel || !renderobj.mqoobj) {
		_ASSERT(0);
		return 0;
	}

	return m_csdeform->ComputeDeform(renderobj);
}

int CDispObj::CopyCSDeform()
{
	if (!m_csdeform) {
		_ASSERT(0);
		return 0;
	}

	//return m_csdeform->CopyCSDeform();
	return 0;
}

int CDispObj::PickRay(ChaVector3 startglobal, ChaVector3 dirglobal,
	bool excludeinvface, int* hitfaceindex, ChaVector3* dsthitpos)
{
	if (!m_csdeform || !hitfaceindex || !dsthitpos) {
		_ASSERT(0);
		return 0;
	}

	return m_csdeform->PickRay(startglobal, dirglobal, excludeinvface, hitfaceindex, dsthitpos);
}

int CDispObj::GetResultOfPickRay(int* hitfaceindex, ChaVector3* dsthitpos)
{
	if (!m_csdeform || !hitfaceindex || !dsthitpos) {
		_ASSERT(0);
		return 0;
	}

	return m_csdeform->GetResultOfPickRay(hitfaceindex, dsthitpos);
}



int CDispObj::RenderNormal(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{

	if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
		_ASSERT(0);
		return 0;
	}

	// Only PM4
	if (!m_pm4) {
		_ASSERT(0);
		return 0;
	}

	//#################################################
	//DescriptorHeapが作成されてない場合は　すぐにリターン
	//#################################################
	//if (m_createdescriptorflag == false) {
	//	return 0;
	//}
	//if (m_descriptorHeap.Get() == nullptr) {
	//	int dbgflag1 = 1;
	//	return 0;
	//}

	int materialnum = m_pm4->GetDispMaterialNum();
	if (materialnum <= 0) {
		_ASSERT(0);
		return 0;
	}

	//Matrix mView, mProj;
	//mView = g_camera3D->GetViewMatrix(false);
	//mProj = g_camera3D->GetProjectionMatrix();
	////定数バッファの設定、更新など描画の共通処理を実行する。
	//DrawCommon(rc, renderobj, mView, mProj);
	
	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//##################################################################################
	//2023/12/04　メモ
	//SetVertexBufferとSetIndexBufferの呼び出し位置について
	//マテリアルループに入るここで呼び出せば良いようだ
	//マテリアルが思い通りに設定されない場合などに
	//原因を探しながら　度々マテリアルループの中に移動するが
	//SetVertexBufferとSetIndexBufferの呼び出し位置の問題で無いことが分かるとこの場所に戻している
	//##################################################################################
	//1. 頂点バッファを設定。
	rc->SetVertexBuffer(m_vertexBufferView);
	//3. インデックスバッファを設定。
	rc->SetIndexBuffer(m_indexBufferView);


	if (renderobj.renderkind == -1) {
		renderobj.renderkind = RENDERKIND_NORMAL;//2023/12/11
	}
	

	bool isfirstmaterial = true;
	int materialcnt;
	for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
		CMQOMaterial* curmat = NULL;
		int curoffset = 0;
		int curtrinum = 0;
		int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
		if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0) &&
			((renderobj.renderkind != RENDERKIND_SHADOWMAP) || (curmat->GetShadowCasterFlag()))) {
			bool laterflag = renderobj.mqoobj->ExistInLaterMaterial(curmat);

			if (laterflag == false) {
				bool laterflag2 = false;
				RenderNormalMaterial(rc, renderobj, laterflag2,
					curmat, curoffset, curtrinum, isfirstmaterial);
				isfirstmaterial = false;
			}
		}
	}

	int latermatnum = renderobj.mqoobj->GetLaterMaterialNum();
	if ((renderobj.forcewithalpha || renderobj.withalpha) && (latermatnum > 0)) {
		//VRoid VRM 裾(すそ)の透過の順番のため　最後に描画
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {
			LATERMATERIAL latermaterial = renderobj.mqoobj->GetLaterMaterial(laterindex);
			if (latermaterial.pmaterial &&
				((renderobj.renderkind != RENDERKIND_SHADOWMAP) || (latermaterial.pmaterial->GetShadowCasterFlag()))) {
				bool laterflag2 = true;
				RenderNormalMaterial(rc, renderobj, 
					laterflag2,
					latermaterial.pmaterial, latermaterial.offset, latermaterial.trinum,
					isfirstmaterial);
				isfirstmaterial = false;
			}
		}
	}
	
	return 0;
}


int CDispObj::RenderNormalMaterial(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
	bool laterflag, CMQOMaterial* curmat, int curoffset, int curtrinum, bool isfirstmaterial)
{
	if (!rc || !curmat) {
		_ASSERT(0);
		return 1;
	}

	if (!m_pm4) {
		_ASSERT(0);
		return 0;
	}

	//HRESULT hr;


	ChaVector4 diffuse;
	ChaVector4 curdif4f = curmat->GetDif4F();
	diffuse.w = curdif4f.w * renderobj.diffusemult.w;
	diffuse.x = curdif4f.x * renderobj.diffusemult.x * renderobj.materialdisprate.x;
	diffuse.y = curdif4f.y * renderobj.diffusemult.y * renderobj.materialdisprate.x;
	diffuse.z = curdif4f.z * renderobj.diffusemult.z * renderobj.materialdisprate.x;
	//diffuse.Clamp(0.0f, 1.0f);

	bool opeflag = false;
	bool withalpha = renderobj.withalpha || renderobj.forcewithalpha;
	if (laterflag) {
		//laterflag == trueのときは　withalpha == trueのときだけ描画
		if (withalpha) {
			opeflag = true;
		}
		else {
			opeflag = false;
		}
	}
	else {
		if (withalpha == false) {
			//不透明だけを描画
			if (diffuse.w > 0.99999f) {
				opeflag = true;
			}
			else {
				opeflag = false;
			}
		}
		else {
			//ここを通るのは　(renderobj.withalpha == true) || (renderobj.forcewithalpha == true)
			if (renderobj.forcewithalpha) {
				//renderobj.forcewithalpha == trueのときは　1passでオブジェクト全部を描画するのでopeflag = true
				opeflag = true;
			}
			else {
				//renderobj.withalpha == trueのときは　2passの内の1passで半透明だけを描画
				if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
					opeflag = true;
				}
				else {
					opeflag = false;
				}
			}
		}
	}

	if (opeflag == false) {
		return 0;
	}


//
////diffuse = ChaVector4( 0.6f, 0.6f, 0.6f, 1.0f );
//
//	hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
//	_ASSERT(SUCCEEDED(hr));
//
//	ChaVector3 tmpamb = curmat->GetAmb3F() * materialdisprate.w;
//	//tmpamb.Clamp(0.0f, 1.0f);
//	hr = g_hambient->SetRawValue(&tmpamb, 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//
//	ChaVector3 tmpspc = curmat->GetSpc3F() * materialdisprate.y;
//	//tmpspc.Clamp(0.0f, 1.0f);
//	hr = g_hspecular->SetRawValue(&tmpspc, 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//
//	hr = g_hpower->SetFloat(curmat->GetPower());
//	_ASSERT(SUCCEEDED(hr));
//
//	ChaVector3 tmpemi = curmat->GetEmi3F() * materialdisprate.z;
//	//tmpemi.Clamp(0.0f, 1.0f);
//	hr = g_hemissive->SetRawValue(&tmpemi, 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//
//	hr = g_hPm3Scale->SetRawValue(&m_scale, 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//
//
//	pRenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//// no lighting test
		//curtech = g_hRenderBoneL0;
		//pRenderContext->IASetInputLayout(m_layoutBoneL0);

		//ID3D11Buffer* pVBset[2] = { m_VB, m_InfB };
		//UINT strideset[2] = { sizeof(PM3DISPV), sizeof(PM3INF) };
		//UINT offsetset[2] = { 0, 0 };
		//pRenderContext->IASetVertexBuffers(0, 2, &pVBset[0], &strideset[0], &offsetset[0]);

		//UINT vbstride1 = sizeof(PM3DISPV);
		//UINT offset = 0;
		//pRenderContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);

	//pRenderContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);


	////UINT indexoffset = (UINT)curoffset;
	////pRenderContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, indexoffset);//########## 2022/07/05

	////FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	//FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	//pRenderContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);


	Matrix mView, mProj;
	if ((renderobj.renderkind != RENDERKIND_SHADOWMAP)) {
		//mView = g_camera3D->GetViewMatrix(false);
		//mProj = g_camera3D->GetProjectionMatrix();
		mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
		mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
	}
	else {
		//for shadow
		mView = g_cameraShadow->GetViewMatrix(false);
		mProj = g_cameraShadow->GetProjectionMatrix();
	}
	//定数バッファの設定、更新など描画の共通処理を実行する。


	curmat->DrawCommon(rc, renderobj, mView, mProj, renderobj.refposindex);
	int hasskin = 1;
	bool isline = false;
	curmat->BeginRender(rc, renderobj, renderobj.refposindex);
	//4. ドローコールを実行。
	rc->DrawIndexed(curtrinum * 3, curoffset);
	//rc.DrawIndexed(m_pm4->GetFaceNum() * 3);

	return 0;


}


//int CDispObj::RenderZPrePm4(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
//{
//
//	if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
//		_ASSERT(0);
//		return 0;
//	}
//
//	// Only PM4
//	if (!m_pm4) {
//		_ASSERT(0);
//		return 0;
//	}
//
//	//#################################################
//	//DescriptorHeapが作成されてない場合は　すぐにリターン
//	//#################################################
//	//if (m_createdescriptorflag == false) {
//	//	return 0;
//	//}
//	//if (m_descriptorHeap.Get() == nullptr) {
//	//	int dbgflag1 = 1;
//	//	return 0;
//	//}
//
//	int materialnum = m_pm4->GetDispMaterialNum();
//	if (materialnum <= 0) {
//		_ASSERT(0);
//		return 0;
//	}
//
//	//Matrix mView, mProj;
//	//mView = g_camera3D->GetViewMatrix(false);
//	//mProj = g_camera3D->GetProjectionMatrix();
//	////定数バッファの設定、更新など描画の共通処理を実行する。
//	//DrawCommon(rc, renderobj, mView, mProj);
//
//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	//##################################################################################
//	//2023/12/04　メモ
//	//SetVertexBufferとSetIndexBufferの呼び出し位置について
//	//マテリアルループに入るここで呼び出せば良いようだ
//	//マテリアルが思い通りに設定されない場合などに
//	//原因を探しながら　度々マテリアルループの中に移動するが
//	//SetVertexBufferとSetIndexBufferの呼び出し位置の問題で無いことが分かるとこの場所に戻している
//	//##################################################################################
//	//1. 頂点バッファを設定。
//	rc->SetVertexBuffer(m_vertexBufferView);
//	//3. インデックスバッファを設定。
//	rc->SetIndexBuffer(m_indexBufferView);
//
//	renderobj.renderkind = RENDERKIND_ZPREPASS;//2023/12/11
//
//	bool isfirstmaterial = true;
//	//int materialcnt;
//	//for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
//	int materialcnt = 0;
//
//		CMQOMaterial* curmat = NULL;
//		int curoffset = 0;
//		int curtrinum = 0;
//		int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
//		if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0)) {
//			ChaVector4 diffuse;
//			ChaVector4 curdif4f = curmat->GetDif4F();
//			diffuse.w = curdif4f.w * renderobj.diffusemult.w;
//			diffuse.x = curdif4f.x * renderobj.diffusemult.x * renderobj.materialdisprate.x;
//			diffuse.y = curdif4f.y * renderobj.diffusemult.y * renderobj.materialdisprate.x;
//			diffuse.z = curdif4f.z * renderobj.diffusemult.z * renderobj.materialdisprate.x;
//			//diffuse.Clamp(0.0f, 1.0f);
//
//
//			//#####################################################
//			//半透明はZバッファに書き込まないように　ZPrepass処理をスキップ
//			//#####################################################
//			//#####################################################################################################
//			//2024/03/15 GroupNo == 1以外は全部半透明扱いなのでスキップするとZPreが機能しない　よって半透明時も描画することにした
//			//#####################################################################################################
//			//if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
//			//	continue;
//			//}
//			//if (renderobj.withalpha || renderobj.forcewithalpha) {
//			//	continue;
//			//}
//
//			Matrix mView, mProj;
//			//mView = g_camera3D->GetViewMatrix(false);
//			//mProj = g_camera3D->GetProjectionMatrix();
//			mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
//			mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
//			//定数バッファの設定、更新など描画の共通処理を実行する。
//			//curmat->ZPreDrawCommon(rc, renderobj, mView, mProj, renderobj.refposindex);
//			curmat->DrawCommon(rc, renderobj, mView, mProj, renderobj.refposindex);
//			curmat->ZPreBeginRender(rc, renderobj, renderobj.refposindex);
//
//			//4. ドローコールを実行。
//			//rc->DrawIndexed(curtrinum * 3, curoffset);
//
//			int totaltrinum = renderobj.mqoobj->GetPm4()->GetFaceNum();
//			rc->DrawIndexed(totaltrinum * 3, 0);
//		}
//	//}
//
//	return 0;
//}



int CDispObj::RenderNormalPM3(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
		_ASSERT(0);
		return 0;
	}


	if( !m_pm3 ){
		return 0;
	}
	if( m_pm3->GetCreateOptFlag() == 0 ){
		return 0;
	}

	//#################################################
	//DescriptorHeapが作成されてない場合は　すぐにリターン
	//#################################################
	//if (m_createdescriptorflag == false) {
	//	return 0;
	//}
	//if (m_descriptorHeap.Get() == nullptr) {
	//	return 0;
	//}

	//Matrix mView, mProj;
	//mView = g_camera3D->GetViewMatrix(false);
	//mProj = g_camera3D->GetProjectionMatrix();
	////定数バッファの設定、更新など描画の共通処理を実行する。
	//DrawCommon(rc, renderobj, mView, mProj);
	//rc.SetDescriptorHeap(m_descriptorHeap);//BeginRender()より後で呼ばないとエラー

	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	//##################################################################################
	//2023/12/04　メモ
	//SetVertexBufferとSetIndexBufferの呼び出し位置について
	//マテリアルループに入るここで呼び出せば良いようだ
	//マテリアルが思い通りに設定されない場合などに
	//原因を探しながら　度々マテリアルループの中に移動するが
	//SetVertexBufferとSetIndexBufferの呼び出し位置の問題で無いことが分かるとこの場所に戻している
	//##################################################################################
	//1. 頂点バッファを設定。
	rc->SetVertexBuffer(m_vertexBufferView);
	//3. インデックスバッファを設定。
	rc->SetIndexBuffer(m_indexBufferView);

	if (renderobj.renderkind == -1) {
		renderobj.renderkind = RENDERKIND_NORMAL;//2023/12/11
	}

	//マテリアルごとにドロー。

	//HRESULT hr;
	int blno;
	for (blno = 0; blno < m_pm3->GetOptMatNum(); blno++) {
		MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;

		CMQOMaterial* curmat;
		curmat = currb->mqomat;
		if (!curmat) {
			//_ASSERT(0);
			return 1;
		}

		if ((renderobj.renderkind != RENDERKIND_SHADOWMAP) || (curmat->GetShadowCasterFlag())) {
			int curnumprim;
			curnumprim = currb->endface - currb->startface + 1;

			bool laterflag = renderobj.mqoobj->ExistInLaterMaterial(curmat);
			if (laterflag == false) {
				bool laterflag2 = false;
				int result = RenderNormalPM3Material(
					rc, renderobj,
					laterflag2, curmat, currb->startface * 3, curnumprim);
			}
		}
	}



	int latermatnum = renderobj.mqoobj->GetLaterMaterialNum();
	if ((renderobj.forcewithalpha || renderobj.withalpha) && (latermatnum > 0)) {
		//VRoid VRM 裾(すそ)の透過の順番のため　最後に描画
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {
			LATERMATERIAL latermaterial = renderobj.mqoobj->GetLaterMaterial(laterindex);
			if (latermaterial.pmaterial && 
				((renderobj.renderkind != RENDERKIND_SHADOWMAP) || (latermaterial.pmaterial->GetShadowCasterFlag()))) {
				bool laterflag2 = true;
				RenderNormalPM3Material(
					rc, renderobj,
					laterflag2,
					latermaterial.pmaterial, latermaterial.offset, latermaterial.trinum);
			}
		}
	}

	return 0;
}

int CDispObj::RenderNormalPM3Material(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
	bool laterflag, CMQOMaterial* curmat,
	int curoffset, int curtrinum)
{
	if (!rc || !curmat) {
		_ASSERT(0);
		return 1;
	}

	ChaVector4 diffuse;
	ChaVector4 curdif4f = curmat->GetDif4F();
	diffuse.w = curdif4f.w * renderobj.diffusemult.w;
	diffuse.x = curdif4f.x * renderobj.diffusemult.x * renderobj.materialdisprate.x;
	diffuse.y = curdif4f.y * renderobj.diffusemult.y * renderobj.materialdisprate.x;
	diffuse.z = curdif4f.z * renderobj.diffusemult.z * renderobj.materialdisprate.x;
	//diffuse.Clamp(0.0f, 1.0f);


	bool opeflag = false;
	bool withalpha = renderobj.withalpha || renderobj.forcewithalpha;
	if (laterflag) {
		//laterflag == trueのときは　withalpha == trueのときだけ描画
		if (withalpha) {
			opeflag = true;
		}
		else {
			opeflag = false;
		}
	}
	else {
		if (withalpha == false) {
			//不透明だけを描画
			if (diffuse.w > 0.99999f) {
				opeflag = true;
			}
			else {
				opeflag = false;
			}
		}
		else {
			//ここを通るのは　(renderobj.withalpha == true) || (renderobj.forcewithalpha == true)
			if (renderobj.forcewithalpha) {
				//renderobj.forcewithalpha == trueのときは　1passでオブジェクト全部を描画するのでopeflag = true
				opeflag = true;
			}
			else {
				//renderobj.withalpha == trueのときは　2passの内の1passで半透明だけを描画
				if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
					opeflag = true;
				}
				else {
					opeflag = false;
				}
			}
		}
	}

	if (opeflag == false) {
		return 0;
	}

	//HRESULT hr;
	//hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
	//_ASSERT(SUCCEEDED(hr));
	//ChaVector3 tmpamb = curmat->GetAmb3F() * materialdisprate.w;
	////tmpamb.Clamp(0.0f, 1.0f);
	//hr = g_hambient->SetRawValue(&tmpamb, 0, sizeof(ChaVector3));
	//_ASSERT(SUCCEEDED(hr));
	//ChaVector3 tmpspc = curmat->GetSpc3F() * materialdisprate.y;
	////tmpspc.Clamp(0.0f, 1.0f);
	//hr = g_hspecular->SetRawValue(&tmpspc, 0, sizeof(ChaVector3));
	//_ASSERT(SUCCEEDED(hr));
	//hr = g_hpower->SetFloat(curmat->GetPower());
	//_ASSERT(SUCCEEDED(hr));
	//ChaVector3 tmpemi = curmat->GetEmi3F() * materialdisprate.z;
	////tmpemi.Clamp(0.0f, 1.0f);
	//hr = g_hemissive->SetRawValue(&tmpemi, 0, sizeof(ChaVector3));
	//_ASSERT(SUCCEEDED(hr));
	//hr = g_hPm3Scale->SetRawValue(&m_scale, 0, sizeof(ChaVector3));
	//_ASSERT(SUCCEEDED(hr));
	//hr = g_hPm3Offset->SetRawValue(&m_scaleoffset, 0, sizeof(ChaVector3));
	//_ASSERT(SUCCEEDED(hr));


	//pRenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//UINT vbstride1 = sizeof(PM3DISPV);
	//UINT offset = 0;
	//pRenderContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);

	//pRenderContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);


	//FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	//pRenderContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);


	/////////////
		//HRESULT hres;

		//D3D11_TECHNIQUE_DESC techDesc;
		//curtech->GetDesc(&techDesc);
		//UINT p = 0;
		//for (UINT p = 0; p < techDesc.Passes; ++p)
		//{
			//pはテクスチャの有無によるパスの数字
	//curtech->GetPassByIndex(p)->Apply(0, pRenderContext);
	//pRenderContext->DrawIndexed(curtrinum * 3, curoffset, 0);
	//}


	Matrix mView, mProj;
	if ((renderobj.renderkind != RENDERKIND_SHADOWMAP)) {
		//mView = g_camera3D->GetViewMatrix(false);
		//mProj = g_camera3D->GetProjectionMatrix();
		mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
		mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
	}
	else {
		//for shadow
		mView = g_cameraShadow->GetViewMatrix(false);
		mProj = g_cameraShadow->GetProjectionMatrix();
	}
	//定数バッファの設定、更新など描画の共通処理を実行する。
	int refposindex = 0;//!!!!!!!!!
	curmat->DrawCommon(rc, renderobj, mView, mProj, refposindex);


	int hasskin = 0;
	bool isline = false;
	curmat->BeginRender(rc, renderobj, refposindex);

	//rc.SetDescriptorHeap(m_descriptorHeap);

	////1. 頂点バッファを設定。
	//rc.SetVertexBuffer(m_vertexBufferView);
	////3. インデックスバッファを設定。
	//rc.SetIndexBuffer(m_indexBufferView);

	//4. ドローコールを実行。
	rc->DrawIndexed(curtrinum * 3, curoffset);
	//rc.DrawIndexed(m_pm3->GetFaceNum() * 3);

	//descriptorHeapNo += NUM_SRV_ONE_MATERIAL;

	return 0;
}


//int CDispObj::RenderZPrePm3(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
//{
//	if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
//		_ASSERT(0);
//		return 0;
//	}
//
//
//	if (!m_pm3) {
//		return 0;
//	}
//	if (m_pm3->GetCreateOptFlag() == 0) {
//		return 0;
//	}
//
//	//#################################################
//	//DescriptorHeapが作成されてない場合は　すぐにリターン
//	//#################################################
//	//if (m_createdescriptorflag == false) {
//	//	return 0;
//	//}
//	//if (m_descriptorHeap.Get() == nullptr) {
//	//	return 0;
//	//}
//
//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	//##################################################################################
//	//2023/12/04　メモ
//	//SetVertexBufferとSetIndexBufferの呼び出し位置について
//	//マテリアルループに入るここで呼び出せば良いようだ
//	//マテリアルが思い通りに設定されない場合などに
//	//原因を探しながら　度々マテリアルループの中に移動するが
//	//SetVertexBufferとSetIndexBufferの呼び出し位置の問題で無いことが分かるとこの場所に戻している
//	//##################################################################################
//	//1. 頂点バッファを設定。
//	rc->SetVertexBuffer(m_vertexBufferView);
//	//3. インデックスバッファを設定。
//	rc->SetIndexBuffer(m_indexBufferView);
//
//
//	renderobj.renderkind = RENDERKIND_ZPREPASS;//2023/12/11
//
//	//マテリアルごとにドロー。
//
//	//HRESULT hr;
//	//int blno;
//	//for (blno = 0; blno < m_pm3->GetOptMatNum(); blno++) {
//	int blno = 0;
//		MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;
//
//		CMQOMaterial* curmat;
//		curmat = currb->mqomat;
//		if (!curmat) {
//			_ASSERT(0);
//			return 1;
//		}
//
//		//int curtrinum;
//		//curtrinum = currb->endface - currb->startface + 1;
//		//int curoffset = currb->startface * 3;
//
//		ChaVector4 diffuse;
//		ChaVector4 curdif4f = curmat->GetDif4F();
//		diffuse.w = curdif4f.w * renderobj.diffusemult.w;
//		diffuse.x = curdif4f.x * renderobj.diffusemult.x * renderobj.materialdisprate.x;
//		diffuse.y = curdif4f.y * renderobj.diffusemult.y * renderobj.materialdisprate.x;
//		diffuse.z = curdif4f.z * renderobj.diffusemult.z * renderobj.materialdisprate.x;
//		//diffuse.Clamp(0.0f, 1.0f);
//
//
//
//		//#####################################################
//		//半透明はZバッファに書き込まないように　ZPrepass処理をスキップ
//		//#####################################################
//		//#####################################################################################################
//		//2024/03/15 GroupNo == 1以外は全部半透明扱いなのでスキップするとZPreが機能しない　よって半透明時も描画することにした
//		//#####################################################################################################
//		//if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
//		//	continue;
//		//}
//		//if (renderobj.withalpha || renderobj.forcewithalpha) {
//		//	continue;
//		//}
//
//
//		Matrix mView, mProj;
//		//mView = g_camera3D->GetViewMatrix(false);
//		//mProj = g_camera3D->GetProjectionMatrix();
//		mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
//		mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
//		//定数バッファの設定、更新など描画の共通処理を実行する。
//		//curmat->ZPreDrawCommon(rc, renderobj, mView, mProj, renderobj.refposindex);
//		curmat->DrawCommon(rc, renderobj, mView, mProj, renderobj.refposindex);
//		curmat->ZPreBeginRender(rc, renderobj, renderobj.refposindex);
//
//		//rc.SetDescriptorHeap(m_descriptorHeap);
//
//		////1. 頂点バッファを設定。
//		//rc.SetVertexBuffer(m_vertexBufferView);
//		////3. インデックスバッファを設定。
//		//rc.SetIndexBuffer(m_indexBufferView);
//
//		//4. ドローコールを実行。
//		//rc->DrawIndexed(curtrinum * 3, curoffset);
//		int totaltrinum = renderobj.mqoobj->GetPm3()->GetFaceNum();
//		rc->DrawIndexed(totaltrinum * 3, 0);
//
//	//}
//
//	return 0;
//}



int CDispObj::RenderInstancingPm3(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
		_ASSERT(0);
		return 0;
	}


	if (!m_pm3) {
		return 0;
	}
	if (m_pm3->GetCreateOptFlag() == 0) {
		return 0;
	}

	//#################################################
	//DescriptorHeapが作成されてない場合は　すぐにリターン
	//#################################################
	//if (m_createdescriptorflag == false) {
	//	return 0;
	//}
	//if (m_descriptorHeap.Get() == nullptr) {
	//	return 0;
	//}

	//##################################################################################
	//2023/12/04　メモ
	//SetVertexBufferとSetIndexBufferの呼び出し位置について
	//マテリアルループに入るここで呼び出せば良いようだ
	//マテリアルが思い通りに設定されない場合などに
	//原因を探しながら　度々マテリアルループの中に移動するが
	//SetVertexBufferとSetIndexBufferの呼び出し位置の問題で無いことが分かるとこの場所に戻している
	//##################################################################################
	//1. 頂点バッファを設定。
	rc->SetVertexBuffer(m_vertexBufferView);


	//#####################################################
	//インスタンシングはモデル単位。剛体のスケール情報はメッシュ単位。
	//m_InstancingBufferはメッシュと同じ単位。
	//スケール情報にメッシュのスケールを格納する。
	//#####################################################
	INSTANCINGPARAMS* pinstancingparams = renderobj.pmodel->GetInstancingParams();
	SCALEINSTANCING* pscale = GetScaleInstancing();
	if (pinstancingparams && pscale) {
		int instanceno;
		for (instanceno = 0; instanceno < RIGMULTINDEXMAX; instanceno++) {
			INSTANCINGPARAMS* curparams = renderobj.pmodel->GetInstancingParams() + instanceno;
			SCALEINSTANCING* curscale = GetScaleInstancing() + instanceno;
			curparams->scale = curscale->scale;
			curparams->scaleoffset = curscale->offset;
		}
		m_InstancingBuffer.Copy(pinstancingparams);
		rc->SetVertexBuffer(1, m_InstancingBuffer);//!!!!!!! InstancingBuffer !!!!!!
	}
	else {
		_ASSERT(0);
		abort();
	}

	//3. インデックスバッファを設定。
	rc->SetIndexBuffer(m_indexBufferView);


	//renderobj.renderkind = RENDERKIND_INSTANCING;//2024/01/11
	//renderobj.renderkind = RENDERKIND_INSTANCING_TRIANGLE;//for debug

	//マテリアルごとにドロー。

	//HRESULT hr;
	int blno;
	//for (blno = 0; blno < m_pm3->GetOptMatNum(); blno++) {


		blno = 0;//先頭マテリアルで一括描画 //!!!!!!!!!!!!!!!


		MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;

		CMQOMaterial* curmat;
		curmat = currb->mqomat;
		if (!curmat) {
			_ASSERT(0);
			return 1;
		}

		int curtrinum;
		//curtrinum = currb->endface - currb->startface + 1;
		//int curoffset = currb->startface * 3;

		curtrinum = m_pm3->GetFaceNum() * 3;//!!!!!!!!!!!!!!!

		ChaVector4 diffuse;
		ChaVector4 curdif4f = curmat->GetDif4F();
		diffuse.w = curdif4f.w * renderobj.diffusemult.w;
		diffuse.x = curdif4f.x * renderobj.diffusemult.x * renderobj.materialdisprate.x;
		diffuse.y = curdif4f.y * renderobj.diffusemult.y * renderobj.materialdisprate.x;
		diffuse.z = curdif4f.z * renderobj.diffusemult.z * renderobj.materialdisprate.x;
		//diffuse.Clamp(0.0f, 1.0f);



		////#####################################################
		////半透明はZバッファに書き込まないように　ZPrepass処理をスキップ
		////#####################################################
		//if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
		//	continue;
		//}
		//if (renderobj.withalpha || renderobj.forcewithalpha) {
		//	continue;
		//}


		//ChaMatrix objscalemat;
		//ChaMatrix beftramat, afttramat, scalemat;
		//beftramat.SetIdentity();
		//afttramat.SetIdentity();
		//scalemat.SetIdentity();
		//beftramat.SetTranslation(-GetScaleOffset());
		//afttramat.SetTranslation(GetScaleOffset());
		//scalemat.SetScale(GetScale());
		//objscalemat = beftramat * scalemat * afttramat;



		Matrix mWorld, mView, mProj;
		//mView = g_camera3D->GetViewMatrix(false);
		//mProj = g_camera3D->GetProjectionMatrix();
		mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
		mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
		//定数バッファの設定、更新など描画の共通処理を実行する。

		curmat->InstancingDrawCommon(rc, renderobj, mView, mProj, GetScaleInstancing());
		curmat->InstancingBeginRender(rc, renderobj);

		//rc.SetDescriptorHeap(m_descriptorHeap);

		////1. 頂点バッファを設定。
		//rc.SetVertexBuffer(m_vertexBufferView);
		////3. インデックスバッファを設定。
		//rc.SetIndexBuffer(m_indexBufferView);

		//4. ドローコールを実行。
		//rc->DrawIndexedInstanced(curtrinum * 3, renderobj.pmodel->GetInstancingDrawNum());
		rc->DrawIndexedInstanced(curtrinum, renderobj.pmodel->GetInstancingDrawNum());//2024/03/07 curtrinumには既に3を掛けてある


	//}

	return 0;
}



int CDispObj::RenderLine(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if(!rc || !m_extline){
		return 0;
	}
	if( m_extline->GetLineNum() <= 0 ){
		return 0;
	}

	//HRESULT hr;

	ChaVector4 diffuse;
	diffuse.w = m_extline->GetColor().w * renderobj.diffusemult.w;
	diffuse.x = m_extline->GetColor().x * renderobj.diffusemult.x;// *materialdisprate.x;
	diffuse.y = m_extline->GetColor().y * renderobj.diffusemult.y;// *materialdisprate.x;
	diffuse.z = m_extline->GetColor().z * renderobj.diffusemult.z;// *materialdisprate.x;
	//diffuse.Clamp(0.0f, 1.0f);

	if ((renderobj.withalpha == false) && (diffuse.w <= 0.99999f)) {
		return 0;
	}
	if ((renderobj.withalpha == true) && (diffuse.w > 0.99999f)) {
		return 0;
	}


	
	//Matrix mView, mProj;
	//mView = g_camera3D->GetViewMatrix(false);
	//mProj = g_camera3D->GetProjectionMatrix();
	////定数バッファの設定、更新など描画の共通処理を実行する。
	//DrawCommon(rc, renderobj, mView, mProj);
	//rc.SetDescriptorHeap(m_descriptorHeap);//BeginRender()より後で呼ばないとエラー

	////1. 頂点バッファを設定。
	//rc.SetVertexBuffer(m_vertexBufferView);
	////3. インデックスバッファを設定。
	//rc.SetIndexBuffer(m_indexBufferView);

	renderobj.renderkind = RENDERKIND_NORMAL;//2023/12/11
	renderobj.shadertype = MQOSHADER_TOON;//2024/03/24

	int curnumprim;
	curnumprim = m_extline->GetLineNum();

	int hasskin = 0;
	bool isline = true;
	CMQOMaterial* curmat = m_extline->GetMaterial();//m_rootsignatureのためのMaterial. 色はextline::m_colorにある
	if (curmat) {

		rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		Matrix mView, mProj;
		//mView = g_camera3D->GetViewMatrix(false);
		//mProj = g_camera3D->GetProjectionMatrix();
		mView = renderobj.pmodel->GetViewMat().TKMatrix();//2024/03/02
		mProj = renderobj.pmodel->GetProjMat().TKMatrix();//2024/03/02
		//定数バッファの設定、更新など描画の共通処理を実行する。
		curmat->DrawCommon(rc, renderobj, mView, mProj, 0);

		curmat->BeginRender(rc, renderobj, 0);
		//rc.SetDescriptorHeap(m_descriptorHeap);

		//1. 頂点バッファを設定。
		rc->SetVertexBuffer(m_vertexBufferView);
		//3. インデックスバッファを設定。
		rc->SetIndexBuffer(m_indexBufferView);


		rc->DrawIndexed(curnumprim * 2);
	}

	return 0;
}

int CDispObj::CopyDispV( CPolyMesh4* pm4 )
{
	m_pm4 = pm4;

	//if( !m_VB || !pm4->GetPm3Disp() ){
	//	_ASSERT( 0 );
	//	return 1;
	//}


	////HRESULT hr;
	////PM3DISPV* pv;
	////hr = m_VB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&pv);
	////if (FAILED(hr)) {
	////	_ASSERT(0);
	////	return 1;
	////}
	////memcpy( pv, m_pm4->GetPm3Disp(), sizeof( PM3DISPV ) * m_pm4->GetOptLeng() );
	////m_VB->Unmap();


	////PM3INF* pinf;
	////hr = m_InfB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&pinf);
	////if (FAILED(hr)) {
	////	_ASSERT(0);
	////	return 1;
	////}
	////memcpy(pinf, m_pm4->GetPm3Inf(), sizeof(PM3INF) * m_pm4->GetOptLeng());
	////m_InfB->Unmap();


	return 0;
}

int CDispObj::CopyDispV( CPolyMesh3* pm3 )
{

	m_pm3 = pm3;

	//if( !m_VB || !pm3->GetDispV() ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	////HRESULT hr;
	////PM3DISPV* pv;
	////hr = m_VB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&pv);
	////if (FAILED(hr)) {
	////	_ASSERT(0);
	////	return 1;
	////}

	////memcpy( pv, m_pm3->GetDispV(), sizeof( PM3DISPV ) * m_pm3->GetOptLeng() );

	////m_VB->Unmap();

	return 0;
}

//void CDispObj::UpdateBoneMatrix(int srcdatanum, void* srcdata)
//{
//	if ((srcdatanum <= 0) || !srcdata) {
//		_ASSERT(0);
//		return;
//	}
//
//	m_boneMatricesStructureBuffer.Update(sizeof(ChaMatrix), srcdatanum, srcdata);
//
//}

//void Material::InitFromTkmMaterila(


void CDispObj::ResetScaleInstancing()
{
	int instanceno;
	for (instanceno = 0; instanceno < RIGMULTINDEXMAX; instanceno++) {
		m_scaleInstancing[instanceno].Init();
	}
	m_scaleinstancenum = 0;
};
void CDispObj::SetScale(ChaVector3 srcscale, ChaVector3 srcoffset)
{
	if ((m_scaleinstancenum >= 0) && (m_scaleinstancenum < RIGMULTINDEXMAX)) {
		m_scaleInstancing[m_scaleinstancenum].scale = ChaVector4(srcscale, 0.0f);
		m_scaleInstancing[m_scaleinstancenum].offset = ChaVector4(srcoffset, 0.0f);
		m_scaleinstancenum++;
	}
	else {
		_ASSERT(0);
	}
};

SCALEINSTANCING* CDispObj::GetScaleInstancing()
{
	return m_scaleInstancing;
}
int CDispObj::GetScaleInstancingNum()
{
	return m_scaleinstancenum;
}

int CDispObj::GetDeformedDispV(int srcvertindex, BINORMALDISPV* dstv)
{
	if (!m_csdeform || !dstv) {
		_ASSERT(0);
		return 1;//error
	}

	return m_csdeform->GetDeformedDispV(srcvertindex, dstv);
}


