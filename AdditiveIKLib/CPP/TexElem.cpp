#include "stdafx.h"
//#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>
#include <search.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>
#include <crtdbg.h>


#include <TexElem.h>

////#####################################
////####### harayuu9さん　参照　ここから
////#####################################
////参照　：　https://github.com/harayuu9/DirectX11DefferdQiita/blob/Part1/DeferredRenderer/DeferredRenderer/DirectX11Manager.h
//#include <windows.h>
//#include <d3d11.h>
//#include <DirectXMath.h>
//#include <d3dcompiler.h>
////#include <DirectXTex.h>
//#include <wrl/client.h>
//
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <codecvt>
//#include <vector>
////#pragma comment(lib, "d3d11.lib")
////#pragma comment(lib, "d3dcompiler.lib")
////#pragma comment(lib, "dxgi.lib")
//using Microsoft::WRL::ComPtr;
//using namespace DirectX;
//using std::string;
//using std::cout;
//using std::endl;
//using std::ifstream;
//using std::ofstream;
//using std::vector;
//
////書きやすいようにtypedef色々
//typedef ComPtr<ID3D11Buffer> ConstantBuffer, VertexBuffer, IndexBuffer, StructuredBuffer;
//typedef ComPtr<ID3D11InputLayout> InputLayout;
//typedef ComPtr<ID3D11VertexShader> VertexShader;
//typedef ComPtr<ID3D11GeometryShader> GeometryShader;
//typedef ComPtr<ID3D11PixelShader> PixelShader;
//typedef ComPtr<ID3D11ComputeShader> ComputeShader;
//typedef ComPtr<ID3D11Texture2D> Texture2D;
//typedef ComPtr<ID3D11ShaderResourceView> ShaderTexture;
//typedef ComPtr<ID3D11UnorderedAccessView> ComputeOutputView;
////#####################################
////####### harayuu9さん　参照　ここまで
////#####################################




static int s_alloccnt = 0;

CTexElem::CTexElem()
{
	InitParams();
	s_alloccnt++;
	m_id = s_alloccnt;
}

CTexElem::~CTexElem()
{
	DestroyObjs();
}

int CTexElem::InitParams()
{
	m_id = -1;
	m_validflag = false;
	ZeroMemory( m_name, sizeof( WCHAR ) * MAX_PATH );
	ZeroMemory( m_path, sizeof( WCHAR ) * MAX_PATH );
	m_transparent = 0;
	//m_pool = 0;
	m_pool = 0;
	m_orgheight = 0;
	m_orgwidth = 0;
	m_height = 0;
	m_width = 0;
	////m_transcol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	//m_ptex = 0;
	//m_ResView = 0;
	return 0;
}
int CTexElem::DestroyObjs()
{
	//if (m_ptex) {
	//	m_ptex->Release();
	//	m_ptex = 0;
	//}
	//if (m_ResView) {
	//	m_ResView->Release();
	//	m_ResView = 0;
	//}

	InitParams();

	return 0;
}


int CTexElem::CreateTexData(ID3D12Device* pdev)
{
	//if (m_ptex) {
	//	return 0;
	//}
	//if (m_ResView) {
	//	return 0;
	//}

	//int ret;
	//if (m_path[0] != 0) {
	//	ret = SetCurrentDirectory(m_path);
	//	//_ASSERT( ret );
	//}

	////int miplevels = 0;
	////int mipfilter = D3DX_FILTER_TRIANGLE;

	//int miplevels = 1;
	//size_t resizew = 512;
	//size_t resizeh = 512;


	//HRESULT hr0, hr1, hr2, hr3;// , hr4;
	//WCHAR patdds[256] = { 0L };
	//wcscpy_s(patdds, 256, L".dds");
	//WCHAR pattga[256] = { 0L };
	//wcscpy_s(pattga, 256, L".tga");
	//WCHAR* finddds = wcsstr(m_name, patdds);
	//WCHAR* findtga = wcsstr(m_name, pattga);
	//if (findtga) {
	//	TexMetadata meta;
	//	ZeroMemory(&meta, sizeof(TexMetadata));
	//	hr0 = GetMetadataFromTGAFile(m_name, meta);
	//	if (FAILED(hr0)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : GetMetadataFromTGAFile error!!! %x, path : %s, name : %s\r\n",
	//			hr0, m_path, m_name);
	//		return -1;
	//	}

	//	std::unique_ptr<ScratchImage> image(new ScratchImage);
	//	hr1 = LoadFromTGAFile(m_name, &meta, *image);
	//	if (FAILED(hr1)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : LoadFromTGAFile error!!! %x, path : %s, name : %s\r\n",
	//			hr1, m_path, m_name);
	//		image.reset();
	//		return -1;
	//	}

	//	hr2 = CreateShaderResourceView(pdev, image->GetImages(), image->GetImageCount(), meta, &m_ResView);
	//	if (FAILED(hr2)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : CreateShaderResourceView error!!! %x, path : %s, name : %s\r\n",
	//			hr2, m_path, m_name);
	//		image.reset();
	//		return -1;
	//	}
	//	image.reset();


	//	//if (meta.width >= 512) {
	//	//	resizew = 512;
	//	//}
	//	//if (meta.height >= 512) {
	//	//	resizeh = 512;
	//	//}
	//	//std::unique_ptr<ScratchImage> image2(new ScratchImage);
	//	//hr4 = Resize(image->GetImages(), image->GetImageCount(),
	//	//	meta,
	//	//	resizew, resizeh, TEX_FILTER_DEFAULT,
	//	//	*image2);
	//	//image.reset();
	//	//if (FAILED(hr4)) {
	//	//	_ASSERT(0);
	//	//	DbgOut(L"TexElem : CreateTexData : Resize error!!! %x, path : %s, name : %s\r\n",
	//	//		hr1, m_path, m_name);
	//	//	return -1;
	//	//}
	//	//TexMetadata meta2 = meta;
	//	//meta2.width = resizew;
	//	//meta2.height = resizeh;
	//	//meta2.mipLevels = 1;
	//	//hr2 = CreateShaderResourceView(pdev, image2->GetImages(), image2->GetImageCount(), meta2, &m_ResView);
	//	//if (FAILED(hr2)) {
	//	//	_ASSERT(0);
	//	//	DbgOut(L"TexElem : CreateTexData : CreateShaderResourceView error!!! %x, path : %s, name : %s\r\n",
	//	//		hr2, m_path, m_name);
	//	//	image2.reset();
	//	//	return -1;
	//	//}
	//	//image2.reset();
	//}
	//else if (finddds) {
	//	TexMetadata meta;
	//	ZeroMemory(&meta, sizeof(TexMetadata));
	//	hr0 = GetMetadataFromDDSFile(m_name, DDS_FLAGS_NONE, meta);
	//	if (FAILED(hr0)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : GetMetadataFromDDSFile error!!! %x, path : %s, name : %s\r\n",
	//			hr0, m_path, m_name);
	//		return -1;
	//	}

	//	std::unique_ptr<ScratchImage> image(new ScratchImage);
	//	hr1 = LoadFromDDSFile(m_name, DDS_FLAGS_NONE, &meta, *image);
	//	if (FAILED(hr1)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : LoadFromTGAFile error!!! %x, path : %s, name : %s\r\n", 
	//			hr1, m_path, m_name);
	//		image.reset();
	//		return -1;
	//	}

	//	hr2 = CreateShaderResourceView(pdev, image->GetImages(), image->GetImageCount(), meta, &m_ResView);
	//	//hr2 = CreateShaderResourceView(pdev, image->GetImages(), 1, meta, &m_ResView);
	//	if (FAILED(hr2)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : CreateShaderResourceView error!!! %x, path : %s, name : %s\r\n",
	//			hr2, m_path, m_name);
	//		image.reset();
	//		return -1;
	//	}
	//	image.reset();

	//	//if (meta.width >= 512) {
	//	//	resizew = 512;
	//	//}
	//	//if (meta.height >= 512) {
	//	//	resizeh = 512;
	//	//}
	//	//std::unique_ptr<ScratchImage> image2(new ScratchImage);
	//	//hr4 = Resize(image->GetImages(), image->GetImageCount(),
	//	//	meta,
	//	//	resizew, resizeh, TEX_FILTER_DEFAULT,
	//	//	*image2);
	//	//image.reset();
	//	//if (FAILED(hr4)) {
	//	//	_ASSERT(0);
	//	//	DbgOut(L"TexElem : CreateTexData : Resize error!!! %x, path : %s, name : %s\r\n",
	//	//		hr1, m_path, m_name);
	//	//	return -1;
	//	//}
	//	//TexMetadata meta2 = meta;
	//	//meta2.width = resizew;
	//	//meta2.height = resizeh;
	//	//meta2.mipLevels = 1;
	//	//hr2 = CreateShaderResourceView(pdev, image2->GetImages(), image2->GetImageCount(), meta2, &m_ResView);
	//	////hr2 = CreateShaderResourceView(pdev, image->GetImages(), 1, meta, &m_ResView);
	//	//if (FAILED(hr2)) {
	//	//	_ASSERT(0);
	//	//	DbgOut(L"TexElem : CreateTexData : CreateShaderResourceView error!!! %x, path : %s, name : %s\r\n", 
	//	//		hr2, m_path, m_name);
	//	//	image2.reset();
	//	//	return -1;
	//	//}
	//	//image2.reset();
	//}
	//else {
	//	TexMetadata meta;
	//	ZeroMemory(&meta, sizeof(TexMetadata));
	//	hr0 = GetMetadataFromWICFile(m_name, WIC_FLAGS_NONE, meta, nullptr);
	//	if (FAILED(hr0)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : GetMetadataFromWICFile error!!! %x, path : %s, name : %s\r\n",
	//			hr0, m_path, m_name);
	//		return -1;
	//	}

	//	std::unique_ptr<ScratchImage> image(new ScratchImage);
	//	hr1 = LoadFromWICFile(m_name, 
	//		//WIC_FLAGS_ALL_FRAMES, 
	//		WIC_FLAGS_NONE, &meta, *image, nullptr);
	//	if (FAILED(hr1)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : LoadFromTGAFile error!!! %x, path : %s, name : %s\r\n", 
	//			hr1, m_path, m_name);
	//		image.reset();
	//		return -1;
	//	}

	//	hr2 = CreateShaderResourceView(pdev, image->GetImages(), image->GetImageCount(), meta, &m_ResView);
	//	//hr2 = CreateShaderResourceView(pdev, image->GetImages(), 1, meta, &m_ResView);
	//	if (FAILED(hr2)) {
	//		_ASSERT(0);
	//		DbgOut(L"TexElem : CreateTexData : CreateShaderResourceView error!!! %x, path : %s, name : %s\r\n",
	//			hr2, m_path, m_name);
	//		image.reset();
	//		return -1;
	//	}
	//	image.reset();


	//	//if (meta.width >= 512) {
	//	//	resizew = 512;
	//	//}
	//	//if (meta.height >= 512) {
	//	//	resizeh = 512;
	//	//}
	//	//std::unique_ptr<ScratchImage> image2(new ScratchImage);
	//	//hr4 = Resize(image->GetImages(), image->GetImageCount(),
	//	//	meta,
	//	//	resizew, resizeh, TEX_FILTER_DEFAULT,
	//	//	*image2);
	//	//image.reset();
	//	//if (FAILED(hr4)) {
	//	//	_ASSERT(0);
	//	//	DbgOut(L"TexElem : CreateTexData : Resize error!!! %x, path : %s, name : %s\r\n",
	//	//		hr1, m_path, m_name);
	//	//	return -1;
	//	//}
	//	//TexMetadata meta2 = meta;
	//	//meta2.width = resizew;
	//	//meta2.height = resizeh;
	//	//meta2.mipLevels = 1;
	//	//hr2 = CreateShaderResourceView(pdev, image2->GetImages(), image2->GetImageCount(), meta2, &m_ResView);
	//	////hr2 = CreateShaderResourceView(pdev, image->GetImages(), 1, meta, &m_ResView);
	//	//if (FAILED(hr2)) {
	//	//	_ASSERT(0);
	//	//	DbgOut(L"TexElem : CreateTexData : CreateShaderResourceView error!!! %x, path : %s, name : %s\r\n", 
	//	//		hr2, m_path, m_name);
	//	//	image2.reset();
	//	//	return -1;
	//	//}
	//	//image2.reset();
	//}


	////WCHAR patdds[256] = { 0L };
	////wcscpy_s(patdds, 256, L".dds");
	////WCHAR* findptr = wcsstr(m_name, patdds);
	////if (findptr) {
	////	hr = DirectX::CreateDDSTextureFromFile(
	////		pdev,
	////		pRenderContext,
	////		m_name,
	////		&m_ptex,
	////		&m_ResView
	////		);
	////}else{
	////	hr = DirectX::CreateWICTextureFromFile(
	////		pdev,
	////		pRenderContext,
	////		m_name,
	////		&m_ptex,
	////		&m_ResView
	////	);
	////}
	////if (FAILED(hr)) {
	////	//_ASSERT( 0 );
	////	DbgOut(L"TexElem : CreateTexData : CreateTextureFromFileEx error!!! %x, path : %s, name : %s\r\n", hr, m_path, m_name);
	////	return -1;
	////}


	//if (m_ResView) {
	//	ID3D12Resource* res = nullptr;
	//	m_ResView->GetResource(&res);
	//	if (res) {
	//		ID3D11Texture2D* texture2d = nullptr;
	//		hr3 = res->QueryInterface(&texture2d);
	//		if (SUCCEEDED(hr3))
	//		{
	//			D3D11_TEXTURE2D_DESC desc;
	//			texture2d->GetDesc(&desc);
	//			m_orgwidth = (int)desc.Width;
	//			m_orgheight = (int)desc.Height;

	//			if (texture2d) {
	//				texture2d->Release();
	//			}
	//			if (res) {
	//				res->Release();
	//			}

	//		}
	//		else {
	//			DbgOut(L"texelem : CreateTexData : QueryInterface tex2d error !!! skip size setting %s\r\n", 
	//				m_name);
	//			_ASSERT(0);
	//			if (texture2d) {
	//				texture2d->Release();
	//			}
	//			if (res) {
	//				res->Release();
	//			}
	//			return -1;
	//		}
	//	}
	//	else {
	//		DbgOut(L"texelem : CreateTexData : GetResource error !!! skip size setting %s\r\n", 
	//			m_name);
	//		_ASSERT(0);
	//		return -1;
	//	}
	//	
	//}
	//else {
	//	_ASSERT(0);
	//	return -1;
	//}

	//m_width = m_orgwidth;
	//m_height = m_orgheight;

	return 0;

}
int CTexElem::InvalidateTexData()
{
	//if (m_ptex) {
	//	m_ptex->Release();
	//	m_ptex = 0;
	//}
	//if (m_ResView) {
	//	m_ResView->Release();
	//	m_ResView = 0;
	//}

	return 0;
}


