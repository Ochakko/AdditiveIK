﻿#include "stdafx.h"
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

#include "../../MiniEngine/MiniEngine.h"
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"


extern GraphicsEngine* g_graphicsEngine;	//グラフィックスエンジン


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
		//return 0;
		delete m_texture;
		m_texture = nullptr;
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


	HRESULT hr0, hr1, hr2, hrmip;// hr3, hr4;
	WCHAR patdds[256] = { 0L };
	wcscpy_s(patdds, 256, L".dds");
	WCHAR pattga[256] = { 0L };
	wcscpy_s(pattga, 256, L".tga");
	WCHAR* finddds = wcsstr(m_name, patdds);
	WCHAR* findtga = wcsstr(m_name, pattga);
	if (findtga) {
		DirectX::TexMetadata metadata;
		//DirectX::ScratchImage scratchImg;
		std::unique_ptr<DirectX::ScratchImage> scratchImg(new DirectX::ScratchImage);
		hr0 = DirectX::LoadFromTGAFile(m_name, DirectX::TGA_FLAGS_NONE, &metadata, *scratchImg);
		if (FAILED(hr0)) {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
		auto img = scratchImg->GetImage(0, 0, 0);//生データ抽出


		//2023/12/31 MIPMAP作成
		std::unique_ptr<DirectX::ScratchImage> mipChain(new DirectX::ScratchImage);
		hrmip = DirectX::GenerateMipMaps(
			*img,
			DirectX::TEX_FILTER_DEFAULT,
			0,
			*mipChain);
		if (FAILED(hrmip)) {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
		//auto mipimg = mipChain->GetImage(0, 0, 0);
		auto mipimg = mipChain->GetImages();


		//WriteToSubresourceで転送する用のヒープ設定
		D3D12_HEAP_PROPERTIES texHeapProp = {};
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//特殊な設定なのでdefaultでもuploadでもなく
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//ライトバックで
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//転送がL0つまりCPU側から直で
		texHeapProp.CreationNodeMask = 0;//単一アダプタのため0
		texHeapProp.VisibleNodeMask = 0;//単一アダプタのため0

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Format = mipChain->GetMetadata().format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBAフォーマット
		//resDesc.Width = static_cast<UINT>(mipChain->GetMetadata().width);//幅
		//resDesc.Width = static_cast<UINT>(AlignmentSize(mipChain->GetMetadata().width, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));//幅
		resDesc.Width = static_cast<UINT>(mipChain->GetMetadata().width);//幅
		resDesc.Height = static_cast<UINT>(mipChain->GetMetadata().height);//高さ
		resDesc.DepthOrArraySize = static_cast<uint16_t>(mipChain->GetMetadata().arraySize);//2Dで配列でもないので１
		resDesc.SampleDesc.Count = 1;//通常テクスチャなのでアンチェリしない
		resDesc.SampleDesc.Quality = 0;//
		resDesc.MipLevels = static_cast<uint16_t>(mipChain->GetMetadata().mipLevels);//ミップマップ 1
		resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(mipChain->GetMetadata().dimension);//2Dテクスチャ用
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//レイアウトについては決定しない
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//とくにフラグなし

		ID3D12Resource* texbuff = nullptr;
		hr1 = pdev->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,//特に指定なし
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//テクスチャ用(ピクセルシェーダから見る用)
			//D3D12_RESOURCE_STATE_COPY_DEST,
			//D3D12_RESOURCE_STATE_GENERIC_READ,
			//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&texbuff)
		);
		if (FAILED(hr1)) {
			_ASSERT(0);
			SetNullTexture();
			scratchImg.reset();
			return -1;
		}


		int subresno;
		for (subresno = 0; subresno < mipChain->GetMetadata().mipLevels; subresno++) {
		//for (subresno = 0; subresno < mipChain->GetImageCount(); subresno++) {
			hr2 = texbuff->WriteToSubresource(
				subresno,
				nullptr,//全領域へコピー
				//img->pixels,//元データアドレス
				//static_cast<UINT>(img->rowPitch),//1ラインサイズ
				//static_cast<UINT>(img->slicePitch)//全サイズ
				//mipimg->pixels,//2023/12/31 mipmap
				//static_cast<UINT>(mipimg->rowPitch),//2023/12/31 mipmap//1ラインサイズ
				//static_cast<UINT>(mipimg->slicePitch)//2023/12/31 mipmap//全サイズ
				mipChain->GetImage(subresno, 0, 0)->pixels,
				(UINT)mipChain->GetImage(subresno, 0, 0)->rowPitch,
				(UINT)mipChain->GetImage(subresno, 0, 0)->slicePitch
			);
			if (FAILED(hr2)) {
				_ASSERT(0);
				SetNullTexture();
				scratchImg.reset();
				mipChain.reset();
				return -1;
			}
		}

		m_texture = new Texture();
		if (m_texture) {
			m_texture->InitFromD3DResource(texbuff);
		}
		else {
			_ASSERT(0);
			SetNullTexture();
			scratchImg.reset();
			return -1;
		}

		scratchImg.reset();


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
		//WICテクスチャのロード
		DirectX::TexMetadata metadata = {};
		std::unique_ptr<DirectX::ScratchImage> scratchImg(new DirectX::ScratchImage);
		hr0 = DirectX::LoadFromWICFile(m_name, DirectX::WIC_FLAGS_NONE, &metadata, *scratchImg);
		if (FAILED(hr0)) {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
		auto img = scratchImg->GetImage(0, 0, 0);//生データ抽出


		//2023/12/31 MIPMAP作成
		std::unique_ptr<DirectX::ScratchImage> mipChain(new DirectX::ScratchImage);
		hrmip = DirectX::GenerateMipMaps(
			*img,
			DirectX::TEX_FILTER_DEFAULT,
			0, 
			*mipChain);
		if (FAILED(hrmip)) {
			_ASSERT(0);
			SetNullTexture();
			return -1;
		}
		//auto mipimg = mipChain->GetImage(0, 0, 0);
		auto mipimg = mipChain->GetImages();


		//WriteToSubresourceで転送する用のヒープ設定
		D3D12_HEAP_PROPERTIES texHeapProp = {};
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//特殊な設定なのでdefaultでもuploadでもなく
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//ライトバックで
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//転送がL0つまりCPU側から直で
		texHeapProp.CreationNodeMask = 0;//単一アダプタのため0
		texHeapProp.VisibleNodeMask = 0;//単一アダプタのため0

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Format = mipChain->GetMetadata().format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBAフォーマット
		//resDesc.Width = static_cast<UINT>(AlignmentSize(mipChain->GetMetadata().width, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));//幅
		resDesc.Width = static_cast<UINT>(mipChain->GetMetadata().width);//幅
		resDesc.Height = static_cast<UINT>(mipChain->GetMetadata().height);//高さ
		resDesc.DepthOrArraySize = static_cast<uint16_t>(mipChain->GetMetadata().arraySize);//2Dで配列でもないので１
		resDesc.SampleDesc.Count = 1;//通常テクスチャなのでアンチェリしない
		resDesc.SampleDesc.Quality = 0;//
		resDesc.MipLevels = static_cast<uint16_t>(mipChain->GetMetadata().mipLevels);//ミップマップ 1
		resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(mipChain->GetMetadata().dimension);//2Dテクスチャ用
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//レイアウトについては決定しない
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//とくにフラグなし

		ID3D12Resource* texbuff = nullptr;
		hr1 = pdev->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,//特に指定なし
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//テクスチャ用(ピクセルシェーダから見る用)
			//D3D12_RESOURCE_STATE_COPY_DEST,
			//D3D12_RESOURCE_STATE_GENERIC_READ,
			//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&texbuff)
		);
		if (FAILED(hr1)) {
			_ASSERT(0);
			SetNullTexture();
			scratchImg.reset();
			return -1;
		}


		int subresno;
		for (subresno = 0; subresno < mipChain->GetMetadata().mipLevels; subresno++) {
		//for (subresno = 0; subresno < mipChain->GetImageCount(); subresno++) {
			hr2 = texbuff->WriteToSubresource(
				subresno,
				nullptr,//全領域へコピー
				//img->pixels,//元データアドレス
				//static_cast<UINT>(img->rowPitch),//1ラインサイズ
				//static_cast<UINT>(img->slicePitch)//全サイズ
				//mipimg->pixels,//2023/12/31 mipmap
				//static_cast<UINT>(mipimg->rowPitch),//2023/12/31 mipmap//1ラインサイズ
				//static_cast<UINT>(mipimg->slicePitch)//2023/12/31 mipmap//全サイズ
				mipChain->GetImage(subresno, 0, 0)->pixels,
				(UINT)mipChain->GetImage(subresno, 0, 0)->rowPitch,
				(UINT)mipChain->GetImage(subresno, 0, 0)->slicePitch
				//(UINT)(AlignmentSize(mipChain->GetImage(subresno, 0, 0)->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)),
				//(UINT)(AlignmentSize(mipChain->GetImage(subresno, 0, 0)->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * 
				//	mipChain->GetImage(subresno, 0, 0)->height)
			);
			if (FAILED(hr2)) {
				_ASSERT(0);
				SetNullTexture();
				scratchImg.reset();
				mipChain.reset();
				return -1;
			}
		}


		m_texture = new Texture();
		if (m_texture) {
			m_texture->InitFromD3DResource(texbuff);
		}
		else {
			_ASSERT(0);
			SetNullTexture();
			scratchImg.reset();
			return -1;
		}

		scratchImg.reset();

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

