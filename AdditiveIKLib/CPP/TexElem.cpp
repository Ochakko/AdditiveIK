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
#include "../../DirectXTex/DirectXTex/DirectXTex.h"


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

	//m_ptex = nullptr;//テクスチャ。
	//ZeroMemory(&m_textureDesc, sizeof(D3D12_RESOURCE_DESC));//テクスチャ情報
	m_texture = nullptr;

	return 0;
}
int CTexElem::DestroyObjs()
{
	//if (m_ptex) {
	//	m_ptex->Release();
	//	m_ptex = nullptr;
	//}
	//if (m_ResView) {
	//	m_ResView->Release();
	//	m_ResView = 0;
	//}

	if (m_texture) {
		delete m_texture;
		m_texture = nullptr;
	}

	InitParams();

	return 0;
}


int CTexElem::CreateTexData(ID3D12Device* pdev)
{
	if (m_texture) {
		return 0;
	}


	//###############################################
	//ファイル読み込みに失敗した場合にも
	//必ず　m_textureはnewしてnulltextureをセットする
	//GetPTexが *m_texture を返すから
	//###############################################

	int ret;
	if (m_path[0] != 0) {
		ret = SetCurrentDirectory(m_path);
		//_ASSERT( ret );
	}

	//int miplevels = 0;
	//int mipfilter = D3DX_FILTER_TRIANGLE;

	int miplevels = 1;
	size_t resizew = 512;
	size_t resizeh = 512;


	HRESULT hr0, hr1;// , hr2, hr3;// , hr4;
	WCHAR patdds[256] = { 0L };
	wcscpy_s(patdds, 256, L".dds");
	WCHAR pattga[256] = { 0L };
	wcscpy_s(pattga, 256, L".tga");
	WCHAR* finddds = wcsstr(m_name, patdds);
	WCHAR* findtga = wcsstr(m_name, pattga);
	if (findtga) {
		DirectX::TexMetadata meta;
		ZeroMemory(&meta, sizeof(DirectX::TexMetadata));
		hr0 = GetMetadataFromTGAFile(m_name, meta);
		if (FAILED(hr0)) {
			_ASSERT(0);
			DbgOut(L"TexElem : CreateTexData : GetMetadataFromTGAFile error!!! %x, path : %s, name : %s\r\n",
				hr0, m_path, m_name);
			SetNullTexture();
			return -1;
		}
		ID3D12Resource* texture = nullptr;
		hr1 = DirectX::CreateTexture(pdev, meta, &texture);
		if (FAILED(hr1)) {
			_ASSERT(0);
			DbgOut(L"TexElem : CreateTexData : CreateTexture error!!! %x, path : %s, name : %s\r\n",
				hr1, m_path, m_name);
			SetNullTexture();
			return -1;
		}
		m_texture = new Texture();
		if (m_texture) {
			m_texture->InitFromD3DResource(texture);
		}
		else {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
	}
	else if (finddds) {
		auto device = g_graphicsEngine->GetD3DDevice();
		DirectX::ResourceUploadBatch re(device);
		re.Begin();
		ID3D12Resource* texture = nullptr;
		//auto hr = DirectX::CreateDDSTextureFromMemoryEx(
		auto hr = DirectX::CreateDDSTextureFromFile(
			device,
			re,
			m_name,
			&texture
		);
		re.End(g_graphicsEngine->GetCommandQueue());

		if (FAILED(hr)) {
			//テクスチャの作成に失敗しました。
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
		m_texture = new Texture();
		if (m_texture) {
			m_texture->InitFromD3DResource(texture);
		}
		else {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
	}
	else {
		DirectX::TexMetadata meta;
		ZeroMemory(&meta, sizeof(DirectX::TexMetadata));
		hr0 = GetMetadataFromWICFile(m_name, DirectX::WIC_FLAGS_NONE, meta, nullptr);
		if (FAILED(hr0)) {
			_ASSERT(0);
			DbgOut(L"TexElem : CreateTexData : GetMetadataFromWICFile error!!! %x, path : %s, name : %s\r\n",
				hr0, m_path, m_name);
			SetNullTexture();
			return -1;
		}
		ID3D12Resource* texture = nullptr;
		hr1 = DirectX::CreateTexture(pdev, meta, &texture);
		if (FAILED(hr1)) {
			_ASSERT(0);
			DbgOut(L"TexElem : CreateTexData : CreateTexture error!!! %x, path : %s, name : %s\r\n",
				hr1, m_path, m_name);
			SetNullTexture();
			return -1;
		}
		m_texture = new Texture();
		if (m_texture) {
			m_texture->InitFromD3DResource(texture);
		}
		else {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
	}

	m_orgwidth = m_texture->GetWidth();
	m_orgheight = m_texture->GetHeight();


	m_width = m_orgwidth;
	m_height = m_orgheight;

	return 0;

}

int CTexElem::SetNullTexture()
{
	const auto& nullTextureMaps = g_graphicsEngine->GetNullTextureMaps();
	//const char* filePath = nullptr;
	char* map = nullptr;
	unsigned int mapSize;

	map = nullTextureMaps.GetAlbedoMap().get();
	mapSize = nullTextureMaps.GetAlbedoMapSize();
	Texture* texture = new Texture();
	if (!texture) {
		_ASSERT(0);
		return 1;
	}
	texture->InitFromMemory(map, mapSize);
	m_texture = texture;
	return 0;
}

int CTexElem::InvalidateTexData()
{
	if (m_texture) {
		delete m_texture;
		m_texture = nullptr;
	}

	//if (m_ResView) {
	//	m_ResView->Release();
	//	m_ResView = 0;
	//}

	return 0;
}

