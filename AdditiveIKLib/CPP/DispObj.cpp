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
#include <mqomaterial.h>

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
#include "../../MiniEngine/Material.h"



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
	m_createdescriptorflag = false;
	m_tmpindexLH = 0;

	m_scale = ChaVector3(1.0f, 1.0f, 1.0f);
	m_scaleoffset = ChaVector3(0.0f, 0.0f, 0.0f);

	//ZeroMemory(&m_BufferDescBone, sizeof(D3D11_BUFFER_DESC));
	//ZeroMemory(&m_BufferDescNoBone, sizeof(D3D11_BUFFER_DESC));
	//ZeroMemory(&m_BufferDescInf, sizeof(D3D11_BUFFER_DESC));
	//ZeroMemory(&m_BufferDescLine, sizeof(D3D11_BUFFER_DESC));

	m_hasbone = 0;

	m_pdev = 0;
	m_pm3 = 0;//外部メモリ
	m_pm4 = 0;

	//m_layoutBoneL0 = 0;
	//m_layoutBoneL1 = 0;
	//m_layoutBoneL2 = 0;
	//m_layoutBoneL3 = 0;
	//m_layoutBoneL4 = 0;
	//m_layoutBoneL5 = 0;
	//m_layoutBoneL6 = 0;
	//m_layoutBoneL7 = 0;
	//m_layoutBoneL8 = 0;
	//m_layoutNoBoneL0 = 0;
	//m_layoutNoBoneL1 = 0;
	//m_layoutNoBoneL2 = 0;
	//m_layoutNoBoneL3 = 0;
	//m_layoutNoBoneL4 = 0;
	//m_layoutNoBoneL5 = 0;
	//m_layoutNoBoneL6 = 0;
	//m_layoutNoBoneL7 = 0;
	//m_layoutNoBoneL8 = 0;
	//m_layoutLine = 0;

 //   m_VB = 0;
	//m_InfB = 0;
	//m_IB = 0;

	m_vertexBuffer = nullptr;		//頂点バッファ。
	ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));	//頂点バッファビュー。

	m_indexBuffer = nullptr;	//インデックスバッファ。
	ZeroMemory(&m_indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));	//インデックスバッファビュー。

	return 0;
}
int CDispObj::DestroyObjs()
{
	m_createdescriptorflag = false;

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


	//if (m_layoutBoneL0) {
	//	m_layoutBoneL0->Release();
	//	m_layoutBoneL0 = 0;
	//}
	//if (m_layoutBoneL1) {
	//	m_layoutBoneL1->Release();
	//	m_layoutBoneL1 = 0;
	//}
	//if (m_layoutBoneL2) {
	//	m_layoutBoneL2->Release();
	//	m_layoutBoneL2 = 0;
	//}
	//if (m_layoutBoneL3) {
	//	m_layoutBoneL3->Release();
	//	m_layoutBoneL3 = 0;
	//}
	//if (m_layoutBoneL4) {
	//	m_layoutBoneL4->Release();
	//	m_layoutBoneL4 = 0;
	//}
	//if (m_layoutBoneL5) {
	//	m_layoutBoneL5->Release();
	//	m_layoutBoneL5 = 0;
	//}
	//if (m_layoutBoneL6) {
	//	m_layoutBoneL6->Release();
	//	m_layoutBoneL6 = 0;
	//}
	//if (m_layoutBoneL7) {
	//	m_layoutBoneL7->Release();
	//	m_layoutBoneL7 = 0;
	//}
	//if (m_layoutBoneL8) {
	//	m_layoutBoneL8->Release();
	//	m_layoutBoneL8 = 0;
	//}

	//if (m_layoutNoBoneL0) {
	//	m_layoutNoBoneL0->Release();
	//	m_layoutNoBoneL0 = 0;
	//}
	//if (m_layoutNoBoneL1) {
	//	m_layoutNoBoneL1->Release();
	//	m_layoutNoBoneL1 = 0;
	//}
	//if (m_layoutNoBoneL2) {
	//	m_layoutNoBoneL2->Release();
	//	m_layoutNoBoneL2 = 0;
	//}
	//if (m_layoutNoBoneL3) {
	//	m_layoutNoBoneL3->Release();
	//	m_layoutNoBoneL3 = 0;
	//}
	//if (m_layoutNoBoneL4) {
	//	m_layoutNoBoneL4->Release();
	//	m_layoutNoBoneL4 = 0;
	//}
	//if (m_layoutNoBoneL5) {
	//	m_layoutNoBoneL5->Release();
	//	m_layoutNoBoneL5 = 0;
	//}
	//if (m_layoutNoBoneL6) {
	//	m_layoutNoBoneL6->Release();
	//	m_layoutNoBoneL6 = 0;
	//}
	//if (m_layoutNoBoneL7) {
	//	m_layoutNoBoneL7->Release();
	//	m_layoutNoBoneL7 = 0;
	//}
	//if (m_layoutNoBoneL8) {
	//	m_layoutNoBoneL8->Release();
	//	m_layoutNoBoneL8 = 0;
	//}


	//if (m_layoutLine) {
	//	m_layoutLine->Release();
	//	m_layoutLine = 0;
	//}

	//if( m_VB ){
	//	m_VB->Release();
	//	m_VB = 0;
	//}

	//if( m_InfB ){
	//	m_InfB->Release();
	//	m_InfB = 0;
	//}

	//if( m_IB ){
	//	m_IB->Release();
	//	m_IB = 0;
	//}

	return 0;
}


int CDispObj::CreateDispObj(ID3D12Device* pdev, CPolyMesh3* pm3, int hasbone)
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm3 = pm3;

	CallF( CreateDecl(pdev), return 1 );
	CallF( CreateVBandIB(pdev), return 1 );


	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		//DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_UNKNOWN,
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
		if (curmat) {
			curmat->InitShadersAndPipelines(
				withboneflag,
				"../Media/Shader/AdditiveIK.fx",
				"VSMainWithoutBone",
				"VSMainWithBone",
				"PSMain",
				colorBufferFormat,
				NUM_SRV_ONE_MATERIAL,
				NUM_CBV_ONE_MATERIAL,
				NUM_CBV_ONE_MATERIAL * rootindex,//offset
				NUM_SRV_ONE_MATERIAL * rootindex,//offset
				D3D12_FILTER_MIN_MAG_MIP_LINEAR
			);

			rootindex++;
		}
	}



	return 0;
}
int CDispObj::CreateDispObj( ID3D12Device* pdev, CPolyMesh4* pm4, int hasbone )
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm4 = pm4;

	CallF( CreateDecl(pdev), return 1 );
	CallF( CreateVBandIB(pdev), return 1 );

	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormat = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		//DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_UNKNOWN,
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
				curmat->InitShadersAndPipelines(
					withboneflag,
					"../Media/Shader/AdditiveIK.fx",
					"VSMainWithoutBone",
					"VSMainWithBone",
					"PSMain",
					colorBufferFormat,
					NUM_SRV_ONE_MATERIAL,
					NUM_CBV_ONE_MATERIAL,
					NUM_CBV_ONE_MATERIAL * rootindex,//offset
					NUM_SRV_ONE_MATERIAL * rootindex,//offset
					D3D12_FILTER_MIN_MAG_MIP_LINEAR
				);
				rootindex++;
			}
		}
	}

	return 0;
}

int CDispObj::CreateDispObj( ID3D12Device* pdev, CExtLine* extline )
{
	DestroyObjs();

	m_hasbone = 0;

	m_pdev = pdev;
	m_extline = extline;

	CallF( CreateDecl(pdev), return 1 );
	CallF( CreateVBandIBLine(pdev), return 1 );

	return 0;
}

void CDispObj::CreateDescriptorHeaps()
{
	if (m_pm4) {
		//ディスクリプタヒープを構築していく。
		int srvNo = 0;
		int cbNo = 0;
		//for (auto& mesh : m_meshs) {
		int materialnum = m_pm4->GetDispMaterialNum();
		int materialcnt;
		for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
			CMQOMaterial* curmat = NULL;
			int curoffset = 0;
			int curtrinum = 0;
			int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
			if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0)) {

				//ディスクリプタヒープにディスクリプタを登録していく。
				m_descriptorHeap.RegistShaderResource(srvNo, curmat->GetDiffuseMap());			//アルベドに乗算するテクスチャ。
				m_descriptorHeap.RegistShaderResource(srvNo + 1, curmat->GetAlbedoMap());			//アルベドマップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 2, curmat->GetNormalMap());		//法線マップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 3, curmat->GetSpecularMap());		//スペキュラマップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
				for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
					if (m_expandShaderResourceView[i]) {
						m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
					}
				}
				srvNo += NUM_SRV_ONE_MATERIAL;
				m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
				if (m_expandConstantBuffer.IsValid()) {
					m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
				}
				cbNo += NUM_CBV_ONE_MATERIAL;

				m_createdescriptorflag = true;
			}
		}
		//}
		if (m_createdescriptorflag) {
			m_descriptorHeap.Commit();
		}
	}
	else if (m_pm3) {
		//ディスクリプタヒープを構築していく。
		int srvNo = 0;
		int cbNo = 0;
		//for (auto& mesh : m_meshs) {
		int blno;
		for (blno = 0; blno < m_pm3->GetOptMatNum(); blno++) {
			MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;
			CMQOMaterial* curmat;
			curmat = currb->mqomat;
			if (curmat) {
				//ディスクリプタヒープにディスクリプタを登録していく。
				m_descriptorHeap.RegistShaderResource(srvNo, curmat->GetDiffuseMap());			//アルベドに乗算するテクスチャ。
				m_descriptorHeap.RegistShaderResource(srvNo + 1, curmat->GetAlbedoMap());			//アルベドマップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 2, curmat->GetNormalMap());		//法線マップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 3, curmat->GetSpecularMap());		//スペキュラマップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
				for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
					if (m_expandShaderResourceView[i]) {
						m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
					}
				}
				srvNo += NUM_SRV_ONE_MATERIAL;
				m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
				if (m_expandConstantBuffer.IsValid()) {
					m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
				}
				cbNo += NUM_CBV_ONE_MATERIAL;
				m_createdescriptorflag = true;
			}
		}
		//}
		if (m_createdescriptorflag) {
			m_descriptorHeap.Commit();
		}
	}
	else {

	}

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

int CDispObj::CreateDecl(ID3D12Device* pdev)
{
	//共通定数バッファの作成。
	m_commonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
	//ユーザー拡張用の定数バッファを作成。
	//if (expandData) {
	//	m_expandConstantBuffer.Init(expandDataSize, nullptr);
	//	m_expandData = expandData;
	//}
	//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
	//	m_expandShaderResourceView[i] = expandShaderResourceView[i];
	//}
	int expandDataSize = 0;
	m_expandConstantBuffer.Init(expandDataSize, nullptr);
	m_expandData = nullptr;
	for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
		m_expandShaderResourceView[i] = nullptr;
	}

	//2023/11/17
	//とりあえず　ボーン無しで表示テスト
	int datanum = 1;
	ChaMatrix dummymat;
	dummymat.SetIdentity();
	m_boneMatricesStructureBuffer.Init(
		sizeof(ChaMatrix),
		datanum,
		&dummymat
	);


	//ディスクリプタヒープを作成。
	CreateDescriptorHeaps();




	//D3D11_INPUT_ELEMENT_DESC declbone[] = {
	//	//pos[4]
	//	{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

	//	//normal[3]
	//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

	//	//uv
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(ChaVector4) + sizeof(ChaVector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

	//	//weight[4]
	//	{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },

	//	//boneindex[4]
	//	{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//	//{ 1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
	//};
	//D3D11_INPUT_ELEMENT_DESC declnobone[] = {
	//	//pos[4]
	//	{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

	//	//normal[3]
	//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

	//	//uv
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(ChaVector4) + sizeof(ChaVector3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//	//{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

	//};
	//D3D11_INPUT_ELEMENT_DESC declline[] = {
	//	//pos[4]
	//	{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//	//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//};


	///*
	//extern ID3DX11EffectTechnique* g_hRenderBoneL0;
	//extern ID3DX11EffectTechnique* g_hRenderBoneL1;
	//extern ID3DX11EffectTechnique* g_hRenderBoneL2;
	//extern ID3DX11EffectTechnique* g_hRenderBoneL3;
	//extern ID3DX11EffectTechnique* g_hRenderNoBoneL0;
	//extern ID3DX11EffectTechnique* g_hRenderNoBoneL1;
	//extern ID3DX11EffectTechnique* g_hRenderNoBoneL2;
	//extern ID3DX11EffectTechnique* g_hRenderNoBoneL3;
	//extern ID3DX11EffectTechnique* g_hRenderLine;

	//ID3D11InputLayout* m_layoutBoneL0;
	//ID3D11InputLayout* m_layoutBoneL1;
	//ID3D11InputLayout* m_layoutBoneL2;
	//ID3D11InputLayout* m_layoutBoneL3;
	//ID3D11InputLayout* m_layoutNoBoneL0;
	//ID3D11InputLayout* m_layoutNoBoneL1;
	//ID3D11InputLayout* m_layoutNoBoneL2;
	//ID3D11InputLayout* m_layoutNoBoneL3;
	//ID3D11InputLayout* m_layoutLine;
	//*/


	//if (!m_pdev ||
	//	!g_hRenderBoneL0 || 
	//	!g_hRenderBoneL1 || !g_hRenderBoneL2 || !g_hRenderBoneL3 || !g_hRenderBoneL4 ||
	//	!g_hRenderBoneL5 || !g_hRenderBoneL6 || !g_hRenderBoneL7 || !g_hRenderBoneL8 ||
	//	!g_hRenderNoBoneL0 || 
	//	!g_hRenderNoBoneL1 || !g_hRenderNoBoneL2 || !g_hRenderNoBoneL3 || !g_hRenderNoBoneL4 ||
	//	!g_hRenderNoBoneL5 || !g_hRenderNoBoneL6 || !g_hRenderNoBoneL7 || !g_hRenderNoBoneL8
	//	) {
	//	_ASSERT(0);
	//	return 1;
	//}


	//// テクニックからパス情報を取得
	//D3DX11_PASS_DESC PassDescBoneL0;
	//g_hRenderBoneL0->GetPassByIndex(0)->GetDesc(&PassDescBoneL0);
	//D3DX11_PASS_DESC PassDescBoneL1;
	//g_hRenderBoneL1->GetPassByIndex(0)->GetDesc(&PassDescBoneL1);
	//D3DX11_PASS_DESC PassDescBoneL2;
	//g_hRenderBoneL2->GetPassByIndex(0)->GetDesc(&PassDescBoneL2);
	//D3DX11_PASS_DESC PassDescBoneL3;
	//g_hRenderBoneL3->GetPassByIndex(0)->GetDesc(&PassDescBoneL3);
	//D3DX11_PASS_DESC PassDescBoneL4;
	//g_hRenderBoneL4->GetPassByIndex(0)->GetDesc(&PassDescBoneL4);
	//D3DX11_PASS_DESC PassDescBoneL5;
	//g_hRenderBoneL5->GetPassByIndex(0)->GetDesc(&PassDescBoneL5);
	//D3DX11_PASS_DESC PassDescBoneL6;
	//g_hRenderBoneL6->GetPassByIndex(0)->GetDesc(&PassDescBoneL6);
	//D3DX11_PASS_DESC PassDescBoneL7;
	//g_hRenderBoneL7->GetPassByIndex(0)->GetDesc(&PassDescBoneL7);
	//D3DX11_PASS_DESC PassDescBoneL8;
	//g_hRenderBoneL8->GetPassByIndex(0)->GetDesc(&PassDescBoneL8);

	//D3DX11_PASS_DESC PassDescNoBoneL0;
	//g_hRenderNoBoneL0->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL0);
	//D3DX11_PASS_DESC PassDescNoBoneL1;
	//g_hRenderNoBoneL1->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL1);
	//D3DX11_PASS_DESC PassDescNoBoneL2;
	//g_hRenderNoBoneL2->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL2);
	//D3DX11_PASS_DESC PassDescNoBoneL3;
	//g_hRenderNoBoneL3->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL3);
	//D3DX11_PASS_DESC PassDescNoBoneL4;
	//g_hRenderNoBoneL4->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL4);
	//D3DX11_PASS_DESC PassDescNoBoneL5;
	//g_hRenderNoBoneL5->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL5);
	//D3DX11_PASS_DESC PassDescNoBoneL6;
	//g_hRenderNoBoneL6->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL6);
	//D3DX11_PASS_DESC PassDescNoBoneL7;
	//g_hRenderNoBoneL7->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL7);
	//D3DX11_PASS_DESC PassDescNoBoneL8;
	//g_hRenderNoBoneL8->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL8);

	//D3DX11_PASS_DESC PassDescLine;
	//g_hRenderLine->GetPassByIndex(0)->GetDesc(&PassDescLine);


	//// 頂点レイアウトを作成
	//HRESULT hr;
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL0.pIAInputSignature, PassDescBoneL0.IAInputSignatureSize, &m_layoutBoneL0);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL1.pIAInputSignature, PassDescBoneL1.IAInputSignatureSize, &m_layoutBoneL1);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL2.pIAInputSignature, PassDescBoneL2.IAInputSignatureSize, &m_layoutBoneL2);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL3.pIAInputSignature, PassDescBoneL3.IAInputSignatureSize, &m_layoutBoneL3);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL4.pIAInputSignature, PassDescBoneL4.IAInputSignatureSize, &m_layoutBoneL4);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL5.pIAInputSignature, PassDescBoneL5.IAInputSignatureSize, &m_layoutBoneL5);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL6.pIAInputSignature, PassDescBoneL6.IAInputSignatureSize, &m_layoutBoneL6);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL7.pIAInputSignature, PassDescBoneL7.IAInputSignatureSize, &m_layoutBoneL7);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescBoneL8.pIAInputSignature, PassDescBoneL8.IAInputSignatureSize, &m_layoutBoneL8);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}

	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL0.pIAInputSignature, PassDescNoBoneL0.IAInputSignatureSize, &m_layoutNoBoneL0);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL1.pIAInputSignature, PassDescNoBoneL1.IAInputSignatureSize, &m_layoutNoBoneL1);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL2.pIAInputSignature, PassDescNoBoneL2.IAInputSignatureSize, &m_layoutNoBoneL2);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL3.pIAInputSignature, PassDescNoBoneL3.IAInputSignatureSize, &m_layoutNoBoneL3);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL4.pIAInputSignature, PassDescNoBoneL4.IAInputSignatureSize, &m_layoutNoBoneL4);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL5.pIAInputSignature, PassDescNoBoneL5.IAInputSignatureSize, &m_layoutNoBoneL5);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL6.pIAInputSignature, PassDescNoBoneL6.IAInputSignatureSize, &m_layoutNoBoneL6);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL7.pIAInputSignature, PassDescNoBoneL7.IAInputSignatureSize, &m_layoutNoBoneL7);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//hr = m_pdev->CreateInputLayout(
	//	declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescNoBoneL8.pIAInputSignature, PassDescNoBoneL8.IAInputSignatureSize, &m_layoutNoBoneL8);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}


	//hr = m_pdev->CreateInputLayout(
	//	declline, sizeof(declline) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDescLine.pIAInputSignature, PassDescLine.IAInputSignatureSize, &m_layoutLine);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}

	return 0;
}
int CDispObj::CreateVBandIB(ID3D12Device* pdev)
{
	//HRESULT hr;

	UINT elemleng, infleng;

	elemleng = sizeof( PM3DISPV );
	infleng = sizeof( PM3INF );

	int pmvleng, pmfleng;
	PM3INF* pmib = 0;
	PM3DISPV* pmv = 0;
	DWORD vbsize;
	DWORD stride;
	if( m_pm3 ){
		pmvleng = m_pm3->GetOptLeng();
		pmfleng = m_pm3->GetFaceNum();
		pmib = 0;
		pmv = m_pm3->GetDispV();

		stride = sizeof(PM3DISPV);
		vbsize = pmvleng * stride;
	}else if( m_pm4 ){
		pmvleng = m_pm4->GetOptLeng();
		pmfleng = m_pm4->GetFaceNum();
		pmib = m_pm4->GetPm3Inf();
		pmv = m_pm4->GetPm3Disp();

		//stride = sizeof(PM3DISPV);
		stride = sizeof(PM3DISPV) + sizeof(PM3INF);


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
		pdev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&rDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer));

		m_vertexBuffer->SetName(L"VertexBuffer");
		//頂点バッファのビューを作成。
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.SizeInBytes = vbsize;
		m_vertexBufferView.StrideInBytes = stride;

		//頂点バッファをコピー.
		uint8_t* pData;
		m_vertexBuffer->Map(0, nullptr, (void**)&pData);
		if (m_pm3) {
			memcpy(pData, pmv, m_vertexBufferView.SizeInBytes);
		}
		else if (m_pm4) {
			DWORD vno;
			for (vno = 0; vno < (DWORD)pmvleng; vno++) {
				uint8_t* pdest = pData + vno * stride;
				PM3DISPV* curv = pmv + vno;
				PM3INF* curinf = pmib + vno;

				memcpy(pdest, curv, sizeof(PM3DISPV));
				memcpy(pdest + sizeof(PM3DISPV), curinf, sizeof(PM3INF));
			}
			//memcpy(pData, pmv, m_vertexBufferView.SizeInBytes);

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
		//auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		DWORD ibsize = pmfleng * 3 * sizeof(int);
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(ibsize);
		auto hr = pdev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&rDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_indexBuffer));

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

int CDispObj::CreateVBandIBLine(ID3D12Device* pdev)
{
	//HRESULT hr;

	UINT elemleng;
	//DWORD curFVF;

	elemleng = sizeof( EXTLINEV );


	//m_BufferDescLine.ByteWidth = m_extline->m_linenum * 2 * sizeof(EXTLINEV);
	//m_BufferDescLine.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!
	//m_BufferDescLine.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_BufferDescLine.CPUAccessFlags = 0;
	//m_BufferDescLine.MiscFlags = 0;


	//D3D11_SUBRESOURCE_DATA SubData;
	//SubData.pSysMem = m_extline->m_linev;
	//SubData.SysMemPitch = 0;
	//SubData.SysMemSlicePitch = 0;
	//hr = m_pdev->CreateBuffer(&m_BufferDescLine, &SubData, &m_VB);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}


	//////IndexBuffer
	////D3D11_BUFFER_DESC IndexBufferDesc;
	////D3D11_SUBRESOURCE_DATA SubDataIndex;

	////IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	////IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	////IndexBufferDesc.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
	////IndexBufferDesc.MiscFlags = 0;

	////SubDataIndex.SysMemPitch = 0;
	////SubDataIndex.SysMemSlicePitch = 0;

	////IndexBufferDesc.ByteWidth = m_extline->m_linenum * 2 * sizeof(int);
	////SubDataIndex.pSysMem = m_extline->m_linev;

	////hr = m_pdev->CreateBuffer(&IndexBufferDesc, &SubDataIndex, &m_IB);
	////if (FAILED(hr)) {
	////	_ASSERT(0);
	////	return 1;
	////}


	return 0;
}

void CDispObj::DrawCommon(RenderContext& rc, myRenderer::RENDEROBJ renderobj,
	const Matrix& mView, const Matrix& mProj)
{
	//メッシュごとにドロー
	//プリミティブのトポロジーはトライアングルリストのみ。
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	////定数バッファを更新する。
	SConstantBuffer cb;
	cb.mWorld = renderobj.mWorld;
	cb.mView = mView;
	cb.mProj = mProj;

	float setfl4x4[16 * MAXCLUSTERNUM];
	ZeroMemory(setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);


	if(renderobj.pmodel && renderobj.mqoobj)
	{
		MOTINFO* curmi = 0;
		int curmotid;
		double curframe;
		curmi = renderobj.pmodel->GetCurMotInfo();

		if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && curmi) {
			curmotid = curmi->motid;
			curframe = RoundingTime(curmi->curframe);

			if (curmotid > 0) {
				int setclcnt = 0;
				int clcnt;
				int clusternum = (int)renderobj.mqoobj->GetClusterSize();
				if ((clusternum > 0) && (clusternum < MAXCLUSTERNUM)) {
					for (clcnt = 0; clcnt < clusternum; clcnt++) {
						CBone* curbone = renderobj.mqoobj->GetCluster(clcnt);
						if (curbone) {
							bool currentlimitdegflag = g_limitdegflag;
							CMotionPoint curmp = curbone->GetCurMp(renderobj.calcslotflag);



							ChaMatrix clustermat;
							clustermat.SetIdentity();

							//CMotionPoint tmpmp = curbone->GetCurMp();
							if (renderobj.btflag == 0) {
								//set4x4[clcnt] = tmpmp.GetWorldMat();
								clustermat = curbone->GetWorldMat(currentlimitdegflag, curmotid, curframe, &curmp);
								MoveMemory(&(setfl4x4[16 * clcnt]),
									clustermat.GetDataPtr(), sizeof(float) * 16);
							}
							else if (renderobj.btflag == 1) {
								//物理シミュ
								//set4x4[clcnt] = curbone->GetBtMat();
								clustermat = curbone->GetBtMat(renderobj.calcslotflag);
								MoveMemory(&(setfl4x4[16 * clcnt]),
									clustermat.GetDataPtr(), sizeof(float) * 16);
							}
							else if (renderobj.btflag == 2) {
								//物理IK
								//set4x4[clcnt] = curbone->GetBtMat();
								clustermat = curbone->GetBtMat(renderobj.calcslotflag);
								MoveMemory(&(setfl4x4[16 * clcnt]),
									curbone->GetBtMat().GetDataPtr(), sizeof(float) * 16);
							}
							else {
								//set4x4[clcnt] = tmpmp.GetWorldMat();
								clustermat = curbone->GetWorldMat(currentlimitdegflag, curmotid, curframe, &curmp);
								MoveMemory(&(setfl4x4[16 * clcnt]),
									clustermat.GetDataPtr(), sizeof(float) * 16);
							}

							setclcnt++;
						}
					}


					if (setclcnt > 0) {
						_ASSERT(setclcnt <= MAXCLUSTERNUM);
						MoveMemory(&(cb.setfl4x4[0]), &(setfl4x4[0]), sizeof(float) * 16 * setclcnt);
					}
				}
			}
		}
	}
	
	m_commonConstantBuffer.CopyToVRAM(cb);

	if (m_expandData) {
		m_expandConstantBuffer.CopyToVRAM(m_expandData);
	}
	if (m_boneMatricesStructureBuffer.IsInited()) {
		//ボーン行列を更新する。

		ChaMatrix dummymat;
		//m_boneMatricesStructureBuffer.Update(m_skeleton->GetBoneMatricesTopAddress());
		m_boneMatricesStructureBuffer.Update(&dummymat);
	}
}


int CDispObj::RenderNormal(RenderContext& rc, myRenderer::RENDEROBJ renderobj)
{

	if (!renderobj.pmodel || !renderobj.mqoobj) {
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
	if (m_createdescriptorflag == false) {
		return 0;
	}
	if (m_descriptorHeap.Get() == nullptr) {
		int dbgflag1 = 1;
		return 0;
	}

	int materialnum = m_pm4->GetDispMaterialNum();
	if (materialnum <= 0) {
		_ASSERT(0);
		return 0;
	}

	Matrix mView, mProj;
	mView = g_camera3D->GetViewMatrix();
	mProj = g_camera3D->GetProjectionMatrix();
	//定数バッファの設定、更新など描画の共通処理を実行する。
	DrawCommon(rc, renderobj, mView, mProj);


	//rc.SetDescriptorHeap(m_descriptorHeap);//BeginRender()より後で呼ばないとエラー

	//int descriptorHeapNo = 0;
	//1. 頂点バッファを設定。
	//rc.SetVertexBuffer(m_vertexBufferView);

	int materialcnt;
	for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
		CMQOMaterial* curmat = NULL;
		int curoffset = 0;
		int curtrinum = 0;
		int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
		if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0)) {

			bool laterflag = renderobj.mqoobj->ExistInLaterMaterial(curmat);

			if (laterflag == false) {
				bool laterflag2 = false;
				RenderNormalMaterial(rc, renderobj, laterflag2,
					curmat, curoffset, curtrinum);
			}
		}
	}

	int latermatnum = renderobj.mqoobj->GetLaterMaterialNum();
	if (renderobj.withalpha && (latermatnum > 0)) {
		//VRoid VRM 裾(すそ)の透過の順番のため　最後に描画
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {
			LATERMATERIAL latermaterial = renderobj.mqoobj->GetLaterMaterial(laterindex);
			if (latermaterial.pmaterial) {
				bool laterflag2 = true;
				RenderNormalMaterial(rc, renderobj, 
					laterflag2,
					latermaterial.pmaterial, latermaterial.offset, latermaterial.trinum);
			}
		}
	}
	
	return 0;
}


int CDispObj::RenderNormalMaterial(RenderContext& rc, myRenderer::RENDEROBJ renderobj,
	bool laterflag, CMQOMaterial* curmat, int curoffset, int curtrinum)
{
	if (!curmat) {
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
	if (laterflag && renderobj.withalpha) {
		opeflag = true;
	}
	else {
		if (renderobj.withalpha == false) {//2023/09/24
			if ((curmat->GetTransparent() == 0) && (diffuse.w > 0.99999f)) {
				opeflag = true;
			}
			else {
				opeflag = false;
			}
		}
		else {
			if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
				opeflag = true;
			}
			else {
				opeflag = false;
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


	int hasskin = 1;
	curmat->BeginRender(rc, hasskin);
	rc.SetDescriptorHeap(m_descriptorHeap);

	rc.SetVertexBuffer(m_vertexBufferView);

	//3. インデックスバッファを設定。
	rc.SetIndexBuffer(m_indexBufferView);

	//4. ドローコールを実行。
	rc.DrawIndexed(curtrinum * 3, curoffset);
	//rc.DrawIndexed(m_pm4->GetFaceNum() * 3);

	//descriptorHeapNo += NUM_SRV_ONE_MATERIAL;

	return 0;


}

int CDispObj::RenderNormalPM3(RenderContext& rc, myRenderer::RENDEROBJ renderobj)
{
	if (!renderobj.pmodel || !renderobj.mqoobj) {
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
	if (m_createdescriptorflag == false) {
		return 0;
	}
	if (m_descriptorHeap.Get() == nullptr) {
		return 0;
	}

	Matrix mView, mProj;
	mView = g_camera3D->GetViewMatrix();
	mProj = g_camera3D->GetProjectionMatrix();

	//定数バッファの設定、更新など描画の共通処理を実行する。
	DrawCommon(rc, renderobj, mView, mProj);
	//rc.SetDescriptorHeap(m_descriptorHeap);//BeginRender()より後で呼ばないとエラー

	//int descriptorHeapNo = 0;
	//1. 頂点バッファを設定。
	//rc.SetVertexBuffer(m_vertexBufferView);
	//マテリアルごとにドロー。

	//HRESULT hr;
	int blno;
	for (blno = 0; blno < m_pm3->GetOptMatNum(); blno++) {
		MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno;

		CMQOMaterial* curmat;
		curmat = currb->mqomat;
		if (!curmat) {
			_ASSERT(0);
			return 1;
		}

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



	int latermatnum = renderobj.mqoobj->GetLaterMaterialNum();
	if (renderobj.withalpha && (latermatnum > 0)) {
		//VRoid VRM 裾(すそ)の透過の順番のため　最後に描画
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {
			LATERMATERIAL latermaterial = renderobj.mqoobj->GetLaterMaterial(laterindex);
			if (latermaterial.pmaterial) {
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

int CDispObj::RenderNormalPM3Material(RenderContext& rc, myRenderer::RENDEROBJ renderobj,
	bool laterflag, CMQOMaterial* curmat,
	int curoffset, int curtrinum)
{
	if (!curmat) {
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
	if (renderobj.withalpha && laterflag) {
		opeflag = true;
	}
	else {
		if (renderobj.withalpha == false) {//2023/09/24
			if ((curmat->GetTransparent() == 0) && (diffuse.w > 0.99999f)) {
				opeflag = true;
			}
			else {
				opeflag = false;
			}
		}
		else {
			if ((curmat->GetTransparent() == 1) || (diffuse.w <= 0.99999f)) {
				opeflag = true;
			}
			else {
				opeflag = false;
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


	int hasskin = 0;
	curmat->BeginRender(rc, hasskin);
	rc.SetDescriptorHeap(m_descriptorHeap);

	//1. 頂点バッファを設定。
	rc.SetVertexBuffer(m_vertexBufferView);

	//3. インデックスバッファを設定。
	rc.SetIndexBuffer(m_indexBufferView);

	//4. ドローコールを実行。
	rc.DrawIndexed(curtrinum * 3, curoffset);
	//rc.DrawIndexed(m_pm3->GetFaceNum() * 3);

	//descriptorHeapNo += NUM_SRV_ONE_MATERIAL;

	return 0;
}


int CDispObj::RenderLine(bool withalpha,
	RenderContext* pRenderContext, 
	ChaVector4 diffusemult, ChaVector4 materialdisprate)
{
	//if( !m_extline ){
	//	return 0;
	//}
	//if( m_extline->m_linenum <= 0 ){
	//	return 0;
	//}

	//HRESULT hr;

	//ChaVector4 diffuse;
	//diffuse.w = m_extline->m_color.w * diffusemult.w;
	//diffuse.x = m_extline->m_color.x * diffusemult.x * materialdisprate.x;
	//diffuse.y = m_extline->m_color.y * diffusemult.y * materialdisprate.x;
	//diffuse.z = m_extline->m_color.z * diffusemult.z * materialdisprate.x;
	////diffuse.Clamp(0.0f, 1.0f);

	//if ((withalpha == false) && (diffuse.w <= 0.99999f)) {
	//	return 0;
	//}
	//if ((withalpha == true) && (diffuse.w > 0.99999f)) {
	//	return 0;
	//}



	//hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
	//_ASSERT(SUCCEEDED(hr));

	//pRenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//ID3DX11EffectTechnique* curtech = g_hRenderLine;
	//pRenderContext->IASetInputLayout(m_layoutLine);

	//UINT vbstride = sizeof(EXTLINEV);
	//UINT offset = 0;
	//pRenderContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride, &offset);
	////m_pdev->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);

	//int curnumprim;
	//curnumprim = m_extline->m_linenum;


	////D3D11_TECHNIQUE_DESC techDesc;
	////curtech->GetDesc(&techDesc);
	//UINT p = 0;
	////for (UINT p = 0; p < techDesc.Passes; ++p)
	////{
	//	curtech->GetPassByIndex(p)->Apply(0, pRenderContext);
	//	//m_pdev->DrawIndexed(curnumprim * 2, 0, 0);
	//	pRenderContext->Draw(curnumprim * 2, 0);
	////}


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

void CDispObj::UpdateBoneMatrix(int srcdatanum, void* srcdata)
{
	if ((srcdatanum <= 0) || !srcdata) {
		_ASSERT(0);
		return;
	}

	m_boneMatricesStructureBuffer.Update(sizeof(ChaMatrix), srcdatanum, srcdata);

}

//void Material::InitFromTkmMaterila(
