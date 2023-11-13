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


#include <MySprite.h>

#define DBGH
#include <dbg.h>

#include <TexBank.h>
#include <TexElem.h>

#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>

#include "../../MiniEngine/MiniEngine.h"

using namespace std;


CMySprite::CMySprite( ID3D12Device* pdev )
{
	InitParams();
	m_pdev = pdev;
}
CMySprite::~CMySprite()
{
	DestroyObjs();
}

int CMySprite::InitParams()
{
	m_pdev = 0;
	m_texid = -1;

	//m_BufferDesc = 0;
	//m_layout = 0;
	//m_VB = 0;

	m_pos = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_size = ChaVector2( 1.0f, 1.0f );
	m_col = ChaVector4( 1.0f, 1.0f, 1.0f, 1.0f );
	//m_col = ChaVector4(0.5f, 0.5f, 0.5f, 1.0f);
	m_spriteoffset = ChaVector3(0.0f, 0.0f, 0.0f);
	m_spritescale = ChaVector2(1.0f, 1.0f);

	m_v[0].pos = ChaVector4( -1.0f, +1.0f, 0.0f, 1.0f );
	m_v[1].pos = ChaVector4( +1.0f, +1.0f, 0.0f, 1.0f );
	m_v[2].pos = ChaVector4( +1.0f, -1.0f, 0.0f, 1.0f );
	m_v[3].pos = ChaVector4(-1.0f, +1.0f, 0.0f, 1.0f);
	m_v[4].pos = ChaVector4(+1.0f, -1.0f, 0.0f, 1.0f);
	m_v[5].pos = ChaVector4(-1.0f, -1.0f, 0.0f, 1.0f);


	m_v[0].uv = ChaVector2( 0.0f, 0.0f );
	m_v[1].uv = ChaVector2( 1.0f, 0.0f );
	m_v[2].uv = ChaVector2( 1.0f, 1.0f );
	m_v[3].uv = ChaVector2(0.0f, 0.0f);
	m_v[4].uv = ChaVector2(1.0f, 1.0f);
	m_v[5].uv = ChaVector2(0.0f, 1.0f);

	return 0;
}




int CMySprite::DestroyObjs()
{
	//if (m_VB) {
	//	m_VB->Release();
	//	m_VB = 0;
	//}
	//if (m_layout) {
	//	m_layout->Release();
	//	m_layout = 0;
	//}
	//if (m_BufferDesc) {
	//	free(m_BufferDesc);
	//	m_BufferDesc = 0;
	//}

	return 0;
}

int CMySprite::CreateDecl()
{

	//m_BufferDesc = (D3D11_BUFFER_DESC*)malloc(sizeof(D3D11_BUFFER_DESC));
	//if (!m_BufferDesc) {
	//	_ASSERT(0);
	//	return 1;
	//}
	//if (m_BufferDesc) {
	//	ZeroMemory(m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	//}


	//D3D11_INPUT_ELEMENT_DESC decl[] = {
	//	//pos[4]
	//	{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	//{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

	//	//uv
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(ChaVector4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//	//{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

	//	//D3DDECL_END()
	//};

	//D3DX11_PASS_DESC PassDesc;
	//g_hRenderSprite->GetPassByIndex(0)->GetDesc(&PassDesc);
	//// 頂点レイアウトを作成
	//HRESULT hr;
	//hr = m_pdev->CreateInputLayout(
	//	decl, sizeof(decl) / sizeof(D3D11_INPUT_ELEMENT_DESC),
	//	PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_layout);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}


	//m_BufferDesc->ByteWidth = 6 * sizeof(SPRITEV);
	//m_BufferDesc->Usage = D3D11_USAGE_DYNAMIC;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//m_BufferDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_BufferDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//m_BufferDesc->MiscFlags = 0;


	//D3D11_SUBRESOURCE_DATA SubData;
	//SubData.pSysMem = m_v;
	//SubData.SysMemPitch = 0;
	//SubData.SysMemSlicePitch = 0;
	//hr = m_pdev->CreateBuffer(m_BufferDesc, &SubData, &m_VB);
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return 1;
	//}


	return 0;
}

//int CMySprite::Create( WCHAR* srcpath, WCHAR* srcname, int srctransparent, int srcpool, D3DXCOLOR* srccol )
int CMySprite::Create(const WCHAR* srcpath, const WCHAR* srcname, int srctransparent, int srcpool)
{
	DestroyObjs();//2023/08/07

	CallF( CreateDecl(), return 1 );

	if( !g_texbank ){
		g_texbank = new CTexBank( m_pdev );
		_ASSERT( g_texbank );
	}

	//g_texbank->AddTex( srcpath, srcname, srctransparent, srcpool, srccol, &m_texid );
	g_texbank->AddTex(srcpath, srcname, srctransparent, srcpool, &m_texid);
	if( m_texid < 0 ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CMySprite::SetPos( ChaVector3 srcpos )
{
	m_pos = srcpos;
	m_spriteoffset = m_pos;

	return 0;
}
int CMySprite::SetSize( ChaVector2 srcsize )
{
	m_size = srcsize;
	m_spritescale = m_size / 2.0f;

	return 0;
}
int CMySprite::SetColor( ChaVector4 srccol )
{
	m_col = srccol;
	return 0;
}
int CMySprite::OnRender(RenderContext* pRenderContext, ID3D12Resource* ptex )
{
	//HRESULT hr;

	//hr = g_hdiffuse->SetRawValue(&m_col, 0, sizeof(ChaVector4));
	//_ASSERT( SUCCEEDED(hr) );
	//hr = g_hSpriteOffset->SetRawValue(&m_spriteoffset, 0, sizeof(ChaVector3));
	//_ASSERT(SUCCEEDED(hr));
	//hr = g_hSpriteScale->SetRawValue(&m_spritescale, 0, sizeof(ChaVector2));
	//_ASSERT(SUCCEEDED(hr));


	//pRenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pRenderContext->IASetInputLayout(m_layout);


	//ID3D11ShaderResourceView* texresview = 0;
	//CTexElem* findtex = g_texbank->GetTexElem(m_texid);
	//if (findtex && findtex->IsValid()) {
	//	texresview = findtex->GetPTex();
	//	_ASSERT(texresview);
	//}
	//else {
	//	texresview = 0;
	//}
	//if (texresview && (texresview != g_presview)) {
	//	g_hMeshTexture->SetResource(texresview);
	//	g_presview = texresview;
	//}
	//else {
	//	//g_hMeshTexture->SetResource(NULL);
	//}


	//UINT vbstride = sizeof(SPRITEV);
	//UINT offset = 0;
	//pRenderContext->IASetVertexBuffers(0, 1, &m_VB, &vbstride, &offset);


	////D3D11_TECHNIQUE_DESC techDesc;
	////g_hRenderSprite->GetDesc(&techDesc);
	//UINT p = 0;
	////for (UINT p = 0; p < techDesc.Passes; ++p)
	////{
	//	g_hRenderSprite->GetPassByIndex(p)->Apply(0, pRenderContext);
	//	pRenderContext->Draw(6, 0);
	////}

	return 0;
}

