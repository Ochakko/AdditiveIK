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

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <TexBank.h>
#include <TexElem.h>

#include <InfBone.h>

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

	m_scale = ChaVector3(1.0f, 1.0f, 1.0f);
	m_scaleoffset = ChaVector3(0.0f, 0.0f, 0.0f);

	ZeroMemory(&m_BufferDescBone, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_BufferDescNoBone, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_BufferDescInf, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_BufferDescLine, sizeof(D3D11_BUFFER_DESC));

	m_hasbone = 0;

	m_pdev = 0;
	m_pm3 = 0;//外部メモリ
	m_pm4 = 0;

	m_layoutBoneL0 = 0;
	m_layoutBoneL1 = 0;
	m_layoutBoneL2 = 0;
	m_layoutBoneL3 = 0;
	m_layoutBoneL4 = 0;
	m_layoutBoneL5 = 0;
	m_layoutBoneL6 = 0;
	m_layoutBoneL7 = 0;
	m_layoutBoneL8 = 0;
	m_layoutNoBoneL0 = 0;
	m_layoutNoBoneL1 = 0;
	m_layoutNoBoneL2 = 0;
	m_layoutNoBoneL3 = 0;
	m_layoutNoBoneL4 = 0;
	m_layoutNoBoneL5 = 0;
	m_layoutNoBoneL6 = 0;
	m_layoutNoBoneL7 = 0;
	m_layoutNoBoneL8 = 0;
	m_layoutLine = 0;

    m_VB = 0;
	m_InfB = 0;
	m_IB = 0;

	return 0;
}
int CDispObj::DestroyObjs()
{
	if (m_tmpindexLH) {
		free(m_tmpindexLH);
		m_tmpindexLH = 0;
	}

	if (m_layoutBoneL0) {
		m_layoutBoneL0->Release();
		m_layoutBoneL0 = 0;
	}
	if (m_layoutBoneL1) {
		m_layoutBoneL1->Release();
		m_layoutBoneL1 = 0;
	}
	if (m_layoutBoneL2) {
		m_layoutBoneL2->Release();
		m_layoutBoneL2 = 0;
	}
	if (m_layoutBoneL3) {
		m_layoutBoneL3->Release();
		m_layoutBoneL3 = 0;
	}
	if (m_layoutBoneL4) {
		m_layoutBoneL4->Release();
		m_layoutBoneL4 = 0;
	}
	if (m_layoutBoneL5) {
		m_layoutBoneL5->Release();
		m_layoutBoneL5 = 0;
	}
	if (m_layoutBoneL6) {
		m_layoutBoneL6->Release();
		m_layoutBoneL6 = 0;
	}
	if (m_layoutBoneL7) {
		m_layoutBoneL7->Release();
		m_layoutBoneL7 = 0;
	}
	if (m_layoutBoneL8) {
		m_layoutBoneL8->Release();
		m_layoutBoneL8 = 0;
	}

	if (m_layoutNoBoneL0) {
		m_layoutNoBoneL0->Release();
		m_layoutNoBoneL0 = 0;
	}
	if (m_layoutNoBoneL1) {
		m_layoutNoBoneL1->Release();
		m_layoutNoBoneL1 = 0;
	}
	if (m_layoutNoBoneL2) {
		m_layoutNoBoneL2->Release();
		m_layoutNoBoneL2 = 0;
	}
	if (m_layoutNoBoneL3) {
		m_layoutNoBoneL3->Release();
		m_layoutNoBoneL3 = 0;
	}
	if (m_layoutNoBoneL4) {
		m_layoutNoBoneL4->Release();
		m_layoutNoBoneL4 = 0;
	}
	if (m_layoutNoBoneL5) {
		m_layoutNoBoneL5->Release();
		m_layoutNoBoneL5 = 0;
	}
	if (m_layoutNoBoneL6) {
		m_layoutNoBoneL6->Release();
		m_layoutNoBoneL6 = 0;
	}
	if (m_layoutNoBoneL7) {
		m_layoutNoBoneL7->Release();
		m_layoutNoBoneL7 = 0;
	}
	if (m_layoutNoBoneL8) {
		m_layoutNoBoneL8->Release();
		m_layoutNoBoneL8 = 0;
	}


	if (m_layoutLine) {
		m_layoutLine->Release();
		m_layoutLine = 0;
	}

	if( m_VB ){
		m_VB->Release();
		m_VB = 0;
	}

	if( m_InfB ){
		m_InfB->Release();
		m_InfB = 0;
	}

	if( m_IB ){
		m_IB->Release();
		m_IB = 0;
	}

	return 0;
}

int CDispObj::CreateDispObj( ID3D11Device* pdev, CPolyMesh3* pm3, int hasbone )
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm3 = pm3;

	CallF( CreateDecl(), return 1 );
	CallF( CreateVBandIB(), return 1 );

	return 0;
}
int CDispObj::CreateDispObj( ID3D11Device* pdev, CPolyMesh4* pm4, int hasbone )
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm4 = pm4;

	CallF( CreateDecl(), return 1 );
	CallF( CreateVBandIB(), return 1 );

	return 0;
}

int CDispObj::CreateDispObj( ID3D11Device* pdev, CExtLine* extline )
{
	DestroyObjs();

	m_hasbone = 0;

	m_pdev = pdev;
	m_extline = extline;

	CallF( CreateDecl(), return 1 );
	CallF( CreateVBandIBLine(), return 1 );

	return 0;
}


int CDispObj::CreateDecl()
{

	D3D11_INPUT_ELEMENT_DESC declbone[] = {
		//pos[4]
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[3]
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//uv
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(ChaVector4) + sizeof(ChaVector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		//weight[4]
		{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ 1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },

		//boneindex[4]
		{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ 1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC declnobone[] = {
		//pos[4]
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[3]
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//uv
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(ChaVector4) + sizeof(ChaVector3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

	};
	D3D11_INPUT_ELEMENT_DESC declline[] = {
		//pos[4]
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	};


	/*
	extern ID3DX11EffectTechnique* g_hRenderBoneL0;
	extern ID3DX11EffectTechnique* g_hRenderBoneL1;
	extern ID3DX11EffectTechnique* g_hRenderBoneL2;
	extern ID3DX11EffectTechnique* g_hRenderBoneL3;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL0;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL1;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL2;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL3;
	extern ID3DX11EffectTechnique* g_hRenderLine;

	ID3D11InputLayout* m_layoutBoneL0;
	ID3D11InputLayout* m_layoutBoneL1;
	ID3D11InputLayout* m_layoutBoneL2;
	ID3D11InputLayout* m_layoutBoneL3;
	ID3D11InputLayout* m_layoutNoBoneL0;
	ID3D11InputLayout* m_layoutNoBoneL1;
	ID3D11InputLayout* m_layoutNoBoneL2;
	ID3D11InputLayout* m_layoutNoBoneL3;
	ID3D11InputLayout* m_layoutLine;
	*/


	if (!m_pdev ||
		!g_hRenderBoneL0 || 
		!g_hRenderBoneL1 || !g_hRenderBoneL2 || !g_hRenderBoneL3 || !g_hRenderBoneL4 ||
		!g_hRenderBoneL5 || !g_hRenderBoneL6 || !g_hRenderBoneL7 || !g_hRenderBoneL8 ||
		!g_hRenderNoBoneL0 || 
		!g_hRenderNoBoneL1 || !g_hRenderNoBoneL2 || !g_hRenderNoBoneL3 || !g_hRenderNoBoneL4 ||
		!g_hRenderNoBoneL5 || !g_hRenderNoBoneL6 || !g_hRenderNoBoneL7 || !g_hRenderNoBoneL8
		) {
		_ASSERT(0);
		return 1;
	}


	// テクニックからパス情報を取得
	D3DX11_PASS_DESC PassDescBoneL0;
	g_hRenderBoneL0->GetPassByIndex(0)->GetDesc(&PassDescBoneL0);
	D3DX11_PASS_DESC PassDescBoneL1;
	g_hRenderBoneL1->GetPassByIndex(0)->GetDesc(&PassDescBoneL1);
	D3DX11_PASS_DESC PassDescBoneL2;
	g_hRenderBoneL2->GetPassByIndex(0)->GetDesc(&PassDescBoneL2);
	D3DX11_PASS_DESC PassDescBoneL3;
	g_hRenderBoneL3->GetPassByIndex(0)->GetDesc(&PassDescBoneL3);
	D3DX11_PASS_DESC PassDescBoneL4;
	g_hRenderBoneL4->GetPassByIndex(0)->GetDesc(&PassDescBoneL4);
	D3DX11_PASS_DESC PassDescBoneL5;
	g_hRenderBoneL5->GetPassByIndex(0)->GetDesc(&PassDescBoneL5);
	D3DX11_PASS_DESC PassDescBoneL6;
	g_hRenderBoneL6->GetPassByIndex(0)->GetDesc(&PassDescBoneL6);
	D3DX11_PASS_DESC PassDescBoneL7;
	g_hRenderBoneL7->GetPassByIndex(0)->GetDesc(&PassDescBoneL7);
	D3DX11_PASS_DESC PassDescBoneL8;
	g_hRenderBoneL8->GetPassByIndex(0)->GetDesc(&PassDescBoneL8);

	D3DX11_PASS_DESC PassDescNoBoneL0;
	g_hRenderNoBoneL0->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL0);
	D3DX11_PASS_DESC PassDescNoBoneL1;
	g_hRenderNoBoneL1->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL1);
	D3DX11_PASS_DESC PassDescNoBoneL2;
	g_hRenderNoBoneL2->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL2);
	D3DX11_PASS_DESC PassDescNoBoneL3;
	g_hRenderNoBoneL3->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL3);
	D3DX11_PASS_DESC PassDescNoBoneL4;
	g_hRenderNoBoneL4->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL4);
	D3DX11_PASS_DESC PassDescNoBoneL5;
	g_hRenderNoBoneL5->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL5);
	D3DX11_PASS_DESC PassDescNoBoneL6;
	g_hRenderNoBoneL6->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL6);
	D3DX11_PASS_DESC PassDescNoBoneL7;
	g_hRenderNoBoneL7->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL7);
	D3DX11_PASS_DESC PassDescNoBoneL8;
	g_hRenderNoBoneL8->GetPassByIndex(0)->GetDesc(&PassDescNoBoneL8);

	D3DX11_PASS_DESC PassDescLine;
	g_hRenderLine->GetPassByIndex(0)->GetDesc(&PassDescLine);


	// 頂点レイアウトを作成
	HRESULT hr;
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL0.pIAInputSignature, PassDescBoneL0.IAInputSignatureSize, &m_layoutBoneL0);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL1.pIAInputSignature, PassDescBoneL1.IAInputSignatureSize, &m_layoutBoneL1);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL2.pIAInputSignature, PassDescBoneL2.IAInputSignatureSize, &m_layoutBoneL2);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL3.pIAInputSignature, PassDescBoneL3.IAInputSignatureSize, &m_layoutBoneL3);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL4.pIAInputSignature, PassDescBoneL4.IAInputSignatureSize, &m_layoutBoneL4);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL5.pIAInputSignature, PassDescBoneL5.IAInputSignatureSize, &m_layoutBoneL5);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL6.pIAInputSignature, PassDescBoneL6.IAInputSignatureSize, &m_layoutBoneL6);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL7.pIAInputSignature, PassDescBoneL7.IAInputSignatureSize, &m_layoutBoneL7);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declbone, sizeof(declbone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescBoneL8.pIAInputSignature, PassDescBoneL8.IAInputSignatureSize, &m_layoutBoneL8);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}

	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL0.pIAInputSignature, PassDescNoBoneL0.IAInputSignatureSize, &m_layoutNoBoneL0);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL1.pIAInputSignature, PassDescNoBoneL1.IAInputSignatureSize, &m_layoutNoBoneL1);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL2.pIAInputSignature, PassDescNoBoneL2.IAInputSignatureSize, &m_layoutNoBoneL2);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL3.pIAInputSignature, PassDescNoBoneL3.IAInputSignatureSize, &m_layoutNoBoneL3);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL4.pIAInputSignature, PassDescNoBoneL4.IAInputSignatureSize, &m_layoutNoBoneL4);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL5.pIAInputSignature, PassDescNoBoneL5.IAInputSignatureSize, &m_layoutNoBoneL5);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL6.pIAInputSignature, PassDescNoBoneL6.IAInputSignatureSize, &m_layoutNoBoneL6);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL7.pIAInputSignature, PassDescNoBoneL7.IAInputSignatureSize, &m_layoutNoBoneL7);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	hr = m_pdev->CreateInputLayout(
		declnobone, sizeof(declnobone) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescNoBoneL8.pIAInputSignature, PassDescNoBoneL8.IAInputSignatureSize, &m_layoutNoBoneL8);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}


	hr = m_pdev->CreateInputLayout(
		declline, sizeof(declline) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		PassDescLine.pIAInputSignature, PassDescLine.IAInputSignatureSize, &m_layoutLine);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}


	return 0;
}
int CDispObj::CreateVBandIB()
{
	/*
	extern ID3DX11EffectTechnique* g_hRenderBoneL0;
	extern ID3DX11EffectTechnique* g_hRenderBoneL1;
	extern ID3DX11EffectTechnique* g_hRenderBoneL2;
	extern ID3DX11EffectTechnique* g_hRenderBoneL3;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL0;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL1;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL2;
	extern ID3DX11EffectTechnique* g_hRenderNoBoneL3;
	extern ID3DX11EffectTechnique* g_hRenderLine;

	ID3D11InputLayout* m_layoutBoneL0;
	ID3D11InputLayout* m_layoutBoneL1;
	ID3D11InputLayout* m_layoutBoneL2;
	ID3D11InputLayout* m_layoutBoneL3;
	ID3D11InputLayout* m_layoutNoBoneL0;
	ID3D11InputLayout* m_layoutNoBoneL1;
	ID3D11InputLayout* m_layoutNoBoneL2;
	ID3D11InputLayout* m_layoutNoBoneL3;
	ID3D11InputLayout* m_layoutLine;
	*/


	HRESULT hr;

	UINT elemleng, infleng;

	elemleng = sizeof( PM3DISPV );
	infleng = sizeof( PM3INF );

	int pmvleng, pmfleng;
	PM3INF* pmib = 0;
	PM3DISPV* pmv = 0;
	if( m_pm3 ){
		pmvleng = m_pm3->GetOptLeng();
		pmfleng = m_pm3->GetFaceNum();
		pmib = 0;
		pmv = m_pm3->GetDispV();
	}else if( m_pm4 ){
		pmvleng = m_pm4->GetOptLeng();
		pmfleng = m_pm4->GetFaceNum();
		pmib = m_pm4->GetPm3Inf();
		pmv = m_pm4->GetPm3Disp();
	}
	else {
		_ASSERT(0);
		return 1;
	}

	m_BufferDescBone.ByteWidth = pmvleng * sizeof(PM3DISPV);
	m_BufferDescBone.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_BufferDescBone.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDescBone.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	m_BufferDescBone.MiscFlags = 0;

	m_BufferDescInf.ByteWidth = pmvleng * sizeof(PM3INF);
	m_BufferDescInf.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_BufferDescInf.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDescInf.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
	m_BufferDescInf.MiscFlags = 0;


	m_BufferDescNoBone.ByteWidth = pmvleng * sizeof(PM3DISPV);
	m_BufferDescNoBone.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_BufferDescNoBone.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDescNoBone.CPUAccessFlags = 0;
	m_BufferDescNoBone.MiscFlags = 0;


	if (m_hasbone) {
		D3D11_SUBRESOURCE_DATA SubData;
		SubData.pSysMem = pmv;
		SubData.SysMemPitch = 0;
		SubData.SysMemSlicePitch = 0;
		hr = m_pdev->CreateBuffer(&m_BufferDescBone, &SubData, &m_VB);
		if (FAILED(hr)) {
			_ASSERT(0);
			return 1;
		}

		if (m_pm4) {
			D3D11_SUBRESOURCE_DATA SubDataInf;
			SubDataInf.pSysMem = m_pm4->GetPm3Inf();
			SubDataInf.SysMemPitch = 0;
			SubDataInf.SysMemSlicePitch = 0;
			hr = m_pdev->CreateBuffer(&m_BufferDescInf, &SubDataInf, &m_InfB);
			if (FAILED(hr)) {
				_ASSERT(0);
				return 1;
			}
		}
		else {
			_ASSERT(0);
		}
	}
	else {
		D3D11_SUBRESOURCE_DATA SubData;
		SubData.pSysMem = pmv;
		SubData.SysMemPitch = 0;
		SubData.SysMemSlicePitch = 0;
		hr = m_pdev->CreateBuffer(&m_BufferDescNoBone, &SubData, &m_VB);
		if (FAILED(hr)) {
			_ASSERT(0);
			return 1;
		}
	}

	//IndexBuffer
	D3D11_BUFFER_DESC IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA SubDataIndex;

	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.ByteWidth = pmfleng * 3 * sizeof(int);
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	SubDataIndex.SysMemPitch = 0;
	SubDataIndex.SysMemSlicePitch = 0;

	//if (m_tmpindexLH) {
	//	free(m_tmpindexLH);
	//	m_tmpindexLH = 0;
	//}
	//m_tmpindexLH = (int*)malloc(pmfleng * 3 * sizeof(int));
	//if (!m_tmpindexLH) {
	//	_ASSERT(0);
	//	return 1;
	//}
	if (m_pm3) {
		//int cptricnt;
		//for (cptricnt = 0; cptricnt < pmfleng; cptricnt++) {
		//	*(m_tmpindexLH + cptricnt * 3) = *(m_pm3->GetDispIndex() + cptricnt * 3);
		//	*(m_tmpindexLH + cptricnt * 3 + 1) = *(m_pm3->GetDispIndex() + cptricnt * 3 + 2);
		//	*(m_tmpindexLH + cptricnt * 3 + 2) = *(m_pm3->GetDispIndex() + cptricnt * 3 + 1);
		//}
		//SubDataIndex.pSysMem = m_tmpindexLH;
		SubDataIndex.pSysMem = m_pm3->GetDispIndex();
	}
	else if (m_pm4) {
		//int cptricnt;
		//for (cptricnt = 0; cptricnt < pmfleng; cptricnt++) {
		//	*(m_tmpindexLH + cptricnt * 3) = *(m_pm4->GetDispIndex() + cptricnt * 3);
		//	*(m_tmpindexLH + cptricnt * 3 + 1) = *(m_pm4->GetDispIndex() + cptricnt * 3 + 2);
		//	*(m_tmpindexLH + cptricnt * 3 + 2) = *(m_pm4->GetDispIndex() + cptricnt * 3 + 1);
		//}
		//SubDataIndex.pSysMem = m_tmpindexLH;
		SubDataIndex.pSysMem = m_pm4->GetDispIndex();
	}
	else {
		IndexBufferDesc.ByteWidth = 0;
		SubDataIndex.pSysMem = 0;
		_ASSERT(0);
	}

	hr = m_pdev->CreateBuffer(&IndexBufferDesc, &SubDataIndex, &m_IB);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CDispObj::CreateVBandIBLine()
{
	HRESULT hr;

	UINT elemleng;
	//DWORD curFVF;

	elemleng = sizeof( EXTLINEV );


	m_BufferDescLine.ByteWidth = m_extline->m_linenum * 2 * sizeof(EXTLINEV);
	m_BufferDescLine.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!
	m_BufferDescLine.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDescLine.CPUAccessFlags = 0;
	m_BufferDescLine.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA SubData;
	SubData.pSysMem = m_extline->m_linev;
	SubData.SysMemPitch = 0;
	SubData.SysMemSlicePitch = 0;
	hr = m_pdev->CreateBuffer(&m_BufferDescLine, &SubData, &m_VB);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}


	////IndexBuffer
	//D3D11_BUFFER_DESC IndexBufferDesc;
	//D3D11_SUBRESOURCE_DATA SubDataIndex;

	//IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//IndexBufferDesc.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
	//IndexBufferDesc.MiscFlags = 0;

	//SubDataIndex.SysMemPitch = 0;
	//SubDataIndex.SysMemSlicePitch = 0;

	//IndexBufferDesc.ByteWidth = m_extline->m_linenum * 2 * sizeof(int);
	//SubDataIndex.pSysMem = m_extline->m_linev;

	//hr = m_pdev->CreateBuffer(&IndexBufferDesc, &SubDataIndex, &m_IB);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}


	return 0;
}

int CDispObj::RenderNormal(bool withalpha,
	ID3D11DeviceContext* pd3d11DeviceContext, int lightflag, 
	ChaVector4 diffusemult, ChaVector4 materialdisprate, CMQOObject* pmqoobj)
{

	if (!pd3d11DeviceContext || !pmqoobj) {
		_ASSERT(0);
		return 0;
	}

	// Only PM4
	if (!m_pm4) {
		_ASSERT(0);
		return 0;
	}

	int materialnum = m_pm4->GetDispMaterialNum();
	if (materialnum <= 0) {
		_ASSERT(0);
		return 0;
	}

	int materialcnt;
	for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
		CMQOMaterial* curmat = NULL;
		int curoffset = 0;
		int curtrinum = 0;
		int result0 = m_pm4->GetDispMaterial(materialcnt, &curmat, &curoffset, &curtrinum);
		if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0)) {

			bool laterflag = pmqoobj->ExistInLaterMaterial(curmat);

			if (laterflag == false) {
				bool laterflag2 = false;
				RenderNormalMaterial(laterflag2, withalpha,
					pd3d11DeviceContext,
					curmat, curoffset, curtrinum,
					lightflag, diffusemult, materialdisprate);
			}
		}
	}

	int latermatnum = pmqoobj->GetLaterMaterialNum();
	if (withalpha && (latermatnum > 0)) {
		//VRoid VRM 裾(すそ)の透過の順番のため　最後に描画
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {
			LATERMATERIAL latermaterial = pmqoobj->GetLaterMaterial(laterindex);
			if (latermaterial.pmaterial) {
				bool laterflag2 = true;
				RenderNormalMaterial(laterflag2, withalpha,
					pd3d11DeviceContext,
					latermaterial.pmaterial, latermaterial.offset, latermaterial.trinum,
					lightflag, diffusemult, materialdisprate);
			}
		}
	}



	return 0;
}


int CDispObj::RenderNormalMaterial(bool laterflag, bool withalpha,
	ID3D11DeviceContext* pd3d11DeviceContext, 
	CMQOMaterial* curmat, int curoffset, int curtrinum, 
	int lightflag, ChaVector4 diffusemult, ChaVector4 materialdisprate)
{
	if (!curmat) {
		_ASSERT(0);
		return 1;
	}

	if (!m_pm4) {
		_ASSERT(0);
		return 0;
	}

	HRESULT hr;


	ChaVector4 diffuse;
	ChaVector4 curdif4f = curmat->GetDif4F();
	diffuse.w = curdif4f.w * diffusemult.w;
	diffuse.x = curdif4f.x * diffusemult.x * materialdisprate.x;
	diffuse.y = curdif4f.y * diffusemult.y * materialdisprate.x;
	diffuse.z = curdif4f.z * diffusemult.z * materialdisprate.x;
	//diffuse.Clamp(0.0f, 1.0f);

	//if ((withalpha == false) && (diffuse.w <= 0.99999f)) {
	//	continue;
	//}
	//if ((withalpha == true) && (diffuse.w > 0.99999f)) {
	//	continue;
	//}
	bool opeflag = false;
	if (laterflag && withalpha) {
		opeflag = true;
	}
	else {
		if (withalpha == false) {//2023/09/24
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


	if (diffuse.w <= 0.99999f) {
		//m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		if (g_zcmpalways == false) {
			pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmpAlways, 1);
		}
		g_zcmpalways = true;
	}
	else {
		//m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		if (g_zcmpalways == true) {
			pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmp, 1);
		}
		g_zcmpalways = false;
	}

	//pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmp, 1);


//diffuse = ChaVector4( 0.6f, 0.6f, 0.6f, 1.0f );

	hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
	_ASSERT(SUCCEEDED(hr));

	ChaVector3 tmpamb = curmat->GetAmb3F() * materialdisprate.w;
	//tmpamb.Clamp(0.0f, 1.0f);
	hr = g_hambient->SetRawValue(&tmpamb, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));

	ChaVector3 tmpspc = curmat->GetSpc3F() * materialdisprate.y;
	//tmpspc.Clamp(0.0f, 1.0f);
	hr = g_hspecular->SetRawValue(&tmpspc, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));

	hr = g_hpower->SetFloat(curmat->GetPower());
	_ASSERT(SUCCEEDED(hr));

	ChaVector3 tmpemi = curmat->GetEmi3F() * materialdisprate.z;
	//tmpemi.Clamp(0.0f, 1.0f);
	hr = g_hemissive->SetRawValue(&tmpemi, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));

	hr = g_hPm3Scale->SetRawValue(&m_scale, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));


	pd3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3DX11EffectTechnique* curtech = 0;
	if (m_hasbone) {
		if (lightflag != 0) {
			switch (g_nNumActiveLights) {
			case 1:
				curtech = g_hRenderBoneL1;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL1);
				break;
			case 2:
				curtech = g_hRenderBoneL2;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL2);
				break;
			case 3:
				curtech = g_hRenderBoneL3;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL3);
				break;
			case 4:
				curtech = g_hRenderBoneL4;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL4);
				break;
			case 5:
				curtech = g_hRenderBoneL5;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL5);
				break;
			case 6:
				curtech = g_hRenderBoneL6;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL6);
				break;
			case 7:
				curtech = g_hRenderBoneL7;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL7);
				break;
			case 8:
				curtech = g_hRenderBoneL8;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL8);
				break;

			case 0:
				curtech = g_hRenderBoneL0;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL0);
				break;

			default:
				_ASSERT(0);
				curtech = g_hRenderBoneL1;
				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL1);
				break;
			}
		}
		else {
			curtech = g_hRenderBoneL0;
			pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL0);
			//_ASSERT(0);
		}

		//// no lighting test
		//curtech = g_hRenderBoneL0;
		//pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL0);


		ID3D11Buffer* pVBset[2] = { m_VB, m_InfB };
		UINT strideset[2] = { sizeof(PM3DISPV), sizeof(PM3INF) };
		UINT offsetset[2] = { 0, 0 };
		pd3d11DeviceContext->IASetVertexBuffers(0, 2, &pVBset[0], &strideset[0], &offsetset[0]);

	}
	else {

		if (lightflag != 0) {
			switch (g_nNumActiveLights) {
			case 1:
				curtech = g_hRenderNoBoneL1;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL1);
				break;
			case 2:
				curtech = g_hRenderNoBoneL2;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL2);
				break;
			case 3:
				curtech = g_hRenderNoBoneL3;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL3);
				break;
			case 4:
				curtech = g_hRenderNoBoneL4;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL4);
				break;
			case 5:
				curtech = g_hRenderNoBoneL5;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL5);
				break;
			case 6:
				curtech = g_hRenderNoBoneL6;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL6);
				break;
			case 7:
				curtech = g_hRenderNoBoneL7;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL7);
				break;
			case 8:
				curtech = g_hRenderNoBoneL8;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL8);
				break;

			case 0:
				curtech = g_hRenderNoBoneL0;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
				break;

			default:
				_ASSERT(0);
				curtech = g_hRenderNoBoneL1;
				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL1);
				break;
			}
		}
		else {
			curtech = g_hRenderNoBoneL0;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
			_ASSERT(0);
		}


		//// no lighting test
		//curtech = g_hRenderNoBoneL0;
		//pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);


		UINT vbstride1 = sizeof(PM3DISPV);
		UINT offset = 0;
		pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);
	}

	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//// no bone and no lighting test
	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//curtech = g_hRenderNoBoneL0;
	//pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
	//UINT vbstride1 = sizeof(PM3DISPV);
	//UINT offset = 0;
	//pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);


	pd3d11DeviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);


	//UINT indexoffset = (UINT)curoffset;
	//pd3d11DeviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, indexoffset);//########## 2022/07/05


	//ID3D11Resource* disptex = 0;
	//if( curmat->GetTexID() >= 0 ){
	//	CTexElem* findtex = g_texbank->GetTexElem( curmat->GetTexID() );
	//	if( findtex ){
	//		disptex = findtex->GetPTex();
	//		_ASSERT( disptex );
	//	}else{
	//		disptex = 0;
	//	}
	//}else{
	//	disptex = 0;
	//}
	//int passno;
	//if( disptex ){
	//	passno = 0;
	//}else{
	//	passno = 1;
	//}
	//hr = g_pEffect->SetTexture( g_hMeshTexture, disptex );
	//_ASSERT( !hr );

	ID3D11ShaderResourceView* texresview = 0;
	if (curmat->GetTexID() >= 0) {
		CTexElem* findtex = g_texbank->GetTexElem(curmat->GetTexID());
		if (findtex && findtex->IsValid()) {
			texresview = findtex->GetPTex();
			_ASSERT(texresview);
		}
		else {
			texresview = 0;
		}
	}
	else {
		texresview = 0;
	}

	if (texresview && (texresview != g_presview)) {
		hr = g_hMeshTexture->SetResource(texresview);
		_ASSERT(SUCCEEDED(hr));
		g_presview = texresview;
	}
	else {
		//g_hMeshTexture->SetResource(NULL);
	}

	UINT p;
	if (texresview) {
		p = 0;
	}
	else {
		p = 1;
	}

	//// no texture test
	//g_hMeshTexture->SetResource(NULL);
	//p = 1;///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! no texture test

	//FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	pd3d11DeviceContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);


	/////////////
		//HRESULT hres;

	int rendervnum;
	//if (m_pm3) {
	//	rendervnum = m_pm3->GetOptLeng();
	//}
	//else 
	if (m_pm4) {
		rendervnum = m_pm4->GetOptLeng();
	}
	int curnumprim;
	//if (m_pm3) {
	//	curnumprim = m_pm3->GetFaceNum();
	//}
	//else 
	if (m_pm4) {
		//curnumprim = m_pm4->GetFaceNum();
		curnumprim = curtrinum;
	}
	else {
		_ASSERT(0);
		return 1;
	}

	//D3D11_TECHNIQUE_DESC techDesc;
	//curtech->GetDesc(&techDesc);
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
		//pはテクスチャの有無によるパスの数字
	hr = curtech->GetPassByIndex(p)->Apply(0, pd3d11DeviceContext);
	_ASSERT(SUCCEEDED(hr));
	//pd3d11DeviceContext->DrawIndexed(curnumprim * 3, 0, 0);
	pd3d11DeviceContext->DrawIndexed(curnumprim * 3, curoffset, 0);

	//pd3d11DeviceContext->Draw(rendervnum, 0);
//}

	return 0;


}


//int CDispObj::RenderNormal(ID3D11DeviceContext* pd3d11DeviceContext, CMQOMaterial* rmaterial, int lightflag, ChaVector4 diffusemult )
//{
//	if( !m_pm3 && !m_pm4 ){
//		_ASSERT(0);
//		return 0;
//	}
//	
//	HRESULT hr;
//	
//	CMQOMaterial* curmat = rmaterial;
//	_ASSERT( curmat );
//
//	ChaVector4 diffuse;
//	ChaVector4 curdif4f = curmat->GetDif4F();
//	
//	diffuse.w = curdif4f.w * diffusemult.w;
//	diffuse.x = curdif4f.x * diffusemult.x;
//	diffuse.y = curdif4f.y * diffusemult.y;
//	diffuse.z = curdif4f.z * diffusemult.z;
//
//
//
//	if( diffuse.w <= 0.99999f ){
//		//m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
//		pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmpAlways, 1);
//	}else{
//		//m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//		pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmp, 1);
//	}
//
//	//pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmp, 1);
//
//
////diffuse = ChaVector4( 0.6f, 0.6f, 0.6f, 1.0f );
//
//	hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
//	_ASSERT(SUCCEEDED(hr));
//	hr = g_hambient->SetRawValue(&(curmat->GetAmb3F()), 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//	hr = g_hspecular->SetRawValue(&(curmat->GetSpc3F()), 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//	hr = g_hpower->SetFloat(curmat->GetPower());
//	_ASSERT(SUCCEEDED(hr));
//	hr = g_hemissive->SetRawValue(&(curmat->GetEmi3F()), 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//	hr = g_hPm3Scale->SetRawValue(&m_scale, 0, sizeof(ChaVector3));
//	_ASSERT(SUCCEEDED(hr));
//
//
//	pd3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	ID3DX11EffectTechnique* curtech = 0;
//	if( m_hasbone ){
//		if( lightflag != 0 ){
//			switch( g_nNumActiveLights ){
//			case 1:
//				curtech = g_hRenderBoneL1;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL1);
//				break;
//			case 2:
//				curtech = g_hRenderBoneL2;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL2);
//				break;
//			case 3:
//				curtech = g_hRenderBoneL3;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL3);
//				break;
//			default:
//				_ASSERT( 0 );
//				curtech = g_hRenderBoneL1;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL1);
//				break;
//			}
//		}else{
//			curtech = g_hRenderBoneL0;
//			pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL0);
//			//_ASSERT(0);
//		}
//
//		//// no lighting test
//		//curtech = g_hRenderBoneL0;
//		//pd3d11DeviceContext->IASetInputLayout(m_layoutBoneL0);
//
//
//		ID3D11Buffer* pVBset[2] = { m_VB, m_InfB };
//		UINT strideset[2] = { sizeof(PM3DISPV), sizeof(PM3INF) };
//		UINT offsetset[2] = { 0, 0 };
//		pd3d11DeviceContext->IASetVertexBuffers(0, 2, &pVBset[0], &strideset[0], &offsetset[0]);
//
//	}else{
//
//		if( lightflag != 0 ){
//			switch( g_nNumActiveLights ){
//			case 1:
//				curtech = g_hRenderNoBoneL1;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL1);
//				break;
//			case 2:
//				curtech = g_hRenderNoBoneL2;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL2);
//				break;
//			case 3:
//				curtech = g_hRenderNoBoneL3;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL3);
//				break;
//			default:
//				_ASSERT( 0 );
//				curtech = g_hRenderNoBoneL1;
//				pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL1);
//				break;
//			}
//		}else{
//			curtech = g_hRenderNoBoneL0;
//			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
//			_ASSERT(0);
//		}
//
//
//		//// no lighting test
//		//curtech = g_hRenderNoBoneL0;
//		//pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
//
//
//		UINT vbstride1 = sizeof(PM3DISPV);
//		UINT offset = 0;
//		pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);
//
//	}
//
//	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	//// no bone and no lighting test
//	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	//curtech = g_hRenderNoBoneL0;
//	//pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
//	//UINT vbstride1 = sizeof(PM3DISPV);
//	//UINT offset = 0;
//	//pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);
//
//
//	pd3d11DeviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
//
//
//	//ID3D11Resource* disptex = 0;
//	//if( curmat->GetTexID() >= 0 ){
//	//	CTexElem* findtex = g_texbank->GetTexElem( curmat->GetTexID() );
//	//	if( findtex ){
//	//		disptex = findtex->GetPTex();
//	//		_ASSERT( disptex );
//	//	}else{
//	//		disptex = 0;
//	//	}
//	//}else{
//	//	disptex = 0;
//	//}
//	//int passno;
//	//if( disptex ){
//	//	passno = 0;
//	//}else{
//	//	passno = 1;
//	//}
//	//hr = g_pEffect->SetTexture( g_hMeshTexture, disptex );
//	//_ASSERT( !hr );
//		
//	ID3D11ShaderResourceView* texresview = 0;
//	if( curmat->GetTexID() >= 0 ){
//		CTexElem* findtex = g_texbank->GetTexElem( curmat->GetTexID() );
//		if( findtex ){
//			texresview = findtex->GetPTex();
//			_ASSERT(texresview);
//		}else{
//			texresview = 0;
//		}
//	}else{
//		texresview = 0;
//	}
//
//	if(texresview && (texresview != g_presview)){
//		hr = g_hMeshTexture->SetResource(texresview);
//		_ASSERT(SUCCEEDED(hr));
//		g_presview = texresview;
//	}else{
//		//g_hMeshTexture->SetResource(NULL);
//	}
//
//	UINT p;
//	if(texresview){
//		p = 0;
//	}else{
//		p = 1;
//	}
//
//	//// no texture test
//	//g_hMeshTexture->SetResource(NULL);
//	//p = 1;///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! no texture test
//
//	FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
//	pd3d11DeviceContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);
//
//
///////////////
//	//HRESULT hres;
//
//	int rendervnum;
//	if (m_pm3) {
//		rendervnum = m_pm3->GetOptLeng();
//	}
//	else if (m_pm4) {
//		rendervnum = m_pm4->GetOptLeng();
//	}
//	int curnumprim;
//	if( m_pm3 ){
//		curnumprim = m_pm3->GetFaceNum();
//	}else if( m_pm4 ){
//		curnumprim = m_pm4->GetFaceNum();
//	}else{
//		_ASSERT( 0 );
//		return 1;
//	}
//
//	//D3D11_TECHNIQUE_DESC techDesc;
//	//curtech->GetDesc(&techDesc);
//	//for (UINT p = 0; p < techDesc.Passes; ++p)
//	//{
//		//pはテクスチャの有無によるパスの数字
//		hr = curtech->GetPassByIndex(p)->Apply(0, pd3d11DeviceContext);
//		_ASSERT(SUCCEEDED(hr));
//		pd3d11DeviceContext->DrawIndexed(curnumprim * 3, 0, 0);
//		//pd3d11DeviceContext->Draw(rendervnum, 0);
//	//}
//
//	return 0;
//}

int CDispObj::RenderNormalPM3(bool withalpha,
	ID3D11DeviceContext* pd3d11DeviceContext, int lightflag, 
	ChaVector4 diffusemult, ChaVector4 materialdisprate, CMQOObject* pmqoobj)
{
	if( !m_pm3 ){
		return 0;
	}
	if( m_pm3->GetCreateOptFlag() == 0 ){
		return 0;
	}


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

		bool laterflag = pmqoobj->ExistInLaterMaterial(curmat);
		if (laterflag == false) {
			bool laterflag2 = false;
			int result = RenderNormalPM3Material(laterflag2, withalpha,
				pd3d11DeviceContext,
				curmat, currb->startface * 3, curnumprim,
				lightflag, diffusemult, materialdisprate);
		}
	}



	int latermatnum = pmqoobj->GetLaterMaterialNum();
	if (withalpha && (latermatnum > 0)) {
		//VRoid VRM 裾(すそ)の透過の順番のため　最後に描画
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {
			LATERMATERIAL latermaterial = pmqoobj->GetLaterMaterial(laterindex);
			if (latermaterial.pmaterial) {
				bool laterflag2 = true;
				RenderNormalPM3Material(laterflag2, withalpha,
					pd3d11DeviceContext,
					latermaterial.pmaterial, latermaterial.offset, latermaterial.trinum,
					lightflag, diffusemult, materialdisprate);
			}
		}
	}

	return 0;
}

int CDispObj::RenderNormalPM3Material(bool laterflag, bool withalpha,
	ID3D11DeviceContext* pd3d11DeviceContext,
	CMQOMaterial* curmat, int curoffset, int curtrinum,
	int lightflag, ChaVector4 diffusemult, ChaVector4 materialdisprate)
{
	ChaVector4 diffuse;
	ChaVector4 curdif4f = curmat->GetDif4F();
	diffuse.w = curdif4f.w * diffusemult.w;
	diffuse.x = curdif4f.x * diffusemult.x * materialdisprate.x;
	diffuse.y = curdif4f.y * diffusemult.y * materialdisprate.x;
	diffuse.z = curdif4f.z * diffusemult.z * materialdisprate.x;
	//diffuse.Clamp(0.0f, 1.0f);

	//if ((withalpha == false) && ((curmat->GetTransparent() == 0) && (diffuse.w <= 0.99999f))) {
	//	continue;
	//}
	//if ((withalpha == true) && (((curmat->GetTransparent() == 1) || (diffuse.w > 0.99999f))) {
	//	continue;
	//}
	bool opeflag = false;
	if (withalpha && laterflag) {
		opeflag = true;
	}
	else {
		if (withalpha == false) {//2023/09/24
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



	HRESULT hr;
	hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
	_ASSERT(SUCCEEDED(hr));
	ChaVector3 tmpamb = curmat->GetAmb3F() * materialdisprate.w;
	//tmpamb.Clamp(0.0f, 1.0f);
	hr = g_hambient->SetRawValue(&tmpamb, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));
	ChaVector3 tmpspc = curmat->GetSpc3F() * materialdisprate.y;
	//tmpspc.Clamp(0.0f, 1.0f);
	hr = g_hspecular->SetRawValue(&tmpspc, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));
	hr = g_hpower->SetFloat(curmat->GetPower());
	_ASSERT(SUCCEEDED(hr));
	ChaVector3 tmpemi = curmat->GetEmi3F() * materialdisprate.z;
	//tmpemi.Clamp(0.0f, 1.0f);
	hr = g_hemissive->SetRawValue(&tmpemi, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));
	hr = g_hPm3Scale->SetRawValue(&m_scale, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));
	hr = g_hPm3Offset->SetRawValue(&m_scaleoffset, 0, sizeof(ChaVector3));
	_ASSERT(SUCCEEDED(hr));

	if (diffuse.w <= 0.99999f) {
		//m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		if (g_zcmpalways == false) {
			pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmpAlways, 1);
		}
		g_zcmpalways = true;
	}
	else {
		//m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		if (g_zcmpalways == true) {
			pd3d11DeviceContext->OMSetDepthStencilState(g_pDSStateZCmp, 1);
		}
		g_zcmpalways = false;
	}



	pd3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3DX11EffectTechnique* curtech = 0;


	if (lightflag != 0) {
		switch (g_nNumActiveLights) {
		case 1:
			curtech = g_hRenderNoBoneL1;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL1);
			break;
		case 2:
			curtech = g_hRenderNoBoneL2;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL2);
			break;
		case 3:
			curtech = g_hRenderNoBoneL3;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL3);
			break;
		case 4:
			curtech = g_hRenderNoBoneL4;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL4);
			break;
		case 5:
			curtech = g_hRenderNoBoneL5;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL5);
			break;
		case 6:
			curtech = g_hRenderNoBoneL6;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL6);
			break;
		case 7:
			curtech = g_hRenderNoBoneL7;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL7);
			break;
		case 8:
			curtech = g_hRenderNoBoneL8;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL8);
			break;

		case 0:
			curtech = g_hRenderNoBoneL0;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
			break;

		default:
			_ASSERT(0);
			curtech = g_hRenderNoBoneL1;
			pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL1);
			break;
		}
	}
	else {
		curtech = g_hRenderNoBoneL0;
		pd3d11DeviceContext->IASetInputLayout(m_layoutNoBoneL0);
		//_ASSERT(0);
	}

	UINT vbstride1 = sizeof(PM3DISPV);
	UINT offset = 0;
	pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride1, &offset);

	pd3d11DeviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);


	ID3D11ShaderResourceView* texresview = 0;
	if (curmat->GetTexID() >= 0) {
		CTexElem* findtex = g_texbank->GetTexElem(curmat->GetTexID());
		if (findtex && findtex->IsValid()) {
			texresview = findtex->GetPTex();
			_ASSERT(texresview);
		}
		else {
			texresview = 0;
		}
	}
	else {
		texresview = 0;
	}

	if (texresview && (texresview != g_presview)) {
		hr = g_hMeshTexture->SetResource(texresview);
		_ASSERT(SUCCEEDED(hr));
		g_presview = texresview;
	}
	else {
		//g_hMeshTexture->SetResource(NULL);
	}

	UINT p;
	if (texresview) {
		p = 0;
	}
	else {
		p = 1;
	}


	FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	pd3d11DeviceContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);


	/////////////
		//HRESULT hres;

		//D3D11_TECHNIQUE_DESC techDesc;
		//curtech->GetDesc(&techDesc);
		//UINT p = 0;
		//for (UINT p = 0; p < techDesc.Passes; ++p)
		//{
			//pはテクスチャの有無によるパスの数字
	curtech->GetPassByIndex(p)->Apply(0, pd3d11DeviceContext);
	pd3d11DeviceContext->DrawIndexed(curtrinum * 3, curoffset, 0);
	//}


	return 0;
}


int CDispObj::RenderLine(bool withalpha,
	ID3D11DeviceContext* pd3d11DeviceContext, 
	ChaVector4 diffusemult, ChaVector4 materialdisprate)
{
	if( !m_extline ){
		return 0;
	}
	if( m_extline->m_linenum <= 0 ){
		return 0;
	}

	HRESULT hr;

	ChaVector4 diffuse;
	diffuse.w = m_extline->m_color.w * diffusemult.w;
	diffuse.x = m_extline->m_color.x * diffusemult.x * materialdisprate.x;
	diffuse.y = m_extline->m_color.y * diffusemult.y * materialdisprate.x;
	diffuse.z = m_extline->m_color.z * diffusemult.z * materialdisprate.x;
	//diffuse.Clamp(0.0f, 1.0f);

	if ((withalpha == false) && (diffuse.w <= 0.99999f)) {
		return 0;
	}
	if ((withalpha == true) && (diffuse.w > 0.99999f)) {
		return 0;
	}



	hr = g_hdiffuse->SetRawValue(&diffuse, 0, sizeof(ChaVector4));
	_ASSERT(SUCCEEDED(hr));

	pd3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	ID3DX11EffectTechnique* curtech = g_hRenderLine;
	pd3d11DeviceContext->IASetInputLayout(m_layoutLine);

	UINT vbstride = sizeof(EXTLINEV);
	UINT offset = 0;
	pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride, &offset);
	//m_pdev->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);

	int curnumprim;
	curnumprim = m_extline->m_linenum;


	//D3D11_TECHNIQUE_DESC techDesc;
	//curtech->GetDesc(&techDesc);
	UINT p = 0;
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
		curtech->GetPassByIndex(p)->Apply(0, pd3d11DeviceContext);
		//m_pdev->DrawIndexed(curnumprim * 2, 0, 0);
		pd3d11DeviceContext->Draw(curnumprim * 2, 0);
	//}


	return 0;
}

int CDispObj::CopyDispV( CPolyMesh4* pm4 )
{
	m_pm4 = pm4;

	if( !m_VB || !pm4->GetPm3Disp() ){
		_ASSERT( 0 );
		return 1;
	}


	//HRESULT hr;
	//PM3DISPV* pv;
	//hr = m_VB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&pv);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//memcpy( pv, m_pm4->GetPm3Disp(), sizeof( PM3DISPV ) * m_pm4->GetOptLeng() );
	//m_VB->Unmap();


	//PM3INF* pinf;
	//hr = m_InfB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&pinf);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//memcpy(pinf, m_pm4->GetPm3Inf(), sizeof(PM3INF) * m_pm4->GetOptLeng());
	//m_InfB->Unmap();


	return 0;
}

int CDispObj::CopyDispV( CPolyMesh3* pm3 )
{

	m_pm3 = pm3;

	if( !m_VB || !pm3->GetDispV() ){
		_ASSERT( 0 );
		return 1;
	}

	//HRESULT hr;
	//PM3DISPV* pv;
	//hr = m_VB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&pv);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}

	//memcpy( pv, m_pm3->GetDispV(), sizeof( PM3DISPV ) * m_pm3->GetOptLeng() );

	//m_VB->Unmap();

	return 0;
}

