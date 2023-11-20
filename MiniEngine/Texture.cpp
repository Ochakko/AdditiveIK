#include "stdafx.h"
#include "Texture.h"

#include <ChaVecCalc.h>

Texture::Texture(const wchar_t* filePath)
{
	InitFromDDSFile(filePath);
}
Texture::~Texture()
{
	if (m_texture) {
		m_texture->Release();
	}
}

void Texture::InitFromCustomColor(ChaVector4 srccol)
{
	if (!g_graphicsEngine) {
		_ASSERT(0);
		return;
	}
	if (!g_graphicsEngine->GetD3DDevice()) {
		_ASSERT(0);
		return;
	}


	UINT texW, texH;
	texW = 16;
	texH = 16;

	struct TexRGBA {
		unsigned char R, G, B, A;
	};
	double colR = (double)srccol.x * 255.0;
	double colG = (double)srccol.y * 255.0;
	double colB = (double)srccol.z * 255.0;
	double colA = (double)srccol.w * 255.0;
	TexRGBA srcrgba;
	srcrgba.R = (unsigned char)(max(0.0, min(255.0, colR)));
	srcrgba.G = (unsigned char)(max(0.0, min(255.0, colG)));
	srcrgba.B = (unsigned char)(max(0.0, min(255.0, colB)));
	srcrgba.A = (unsigned char)(max(0.0, min(255.0, colA)));

	
	unsigned char* texturedata = (unsigned char*)malloc(sizeof(unsigned char) * texW * texH * 4);


	if (texturedata) {
		UINT dataindex;
		for (dataindex = 0; dataindex < (texW * texH); dataindex++) {
			unsigned char* ppix = texturedata + dataindex * 4;
			*(ppix) = srcrgba.R;
			*(ppix + 1) = srcrgba.G;
			*(ppix + 2) = srcrgba.B;
			*(ppix + 3) = srcrgba.A;
		}

		//WriteToSubresourceで転送する用のヒープ設定
		D3D12_HEAP_PROPERTIES texHeapProp = {};
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//特殊な設定なのでdefaultでもuploadでもなく
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//ライトバックで
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//転送がL0つまりCPU側から直で
		texHeapProp.CreationNodeMask = 0;//単一アダプタのため0
		texHeapProp.VisibleNodeMask = 0;//単一アダプタのため0

		D3D12_RESOURCE_DESC resDesc = {};
		//resDesc.Format = metadata.format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBAフォーマット
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//RGBAフォーマット
		//resDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//RGBAフォーマット
		//resDesc.Width = static_cast<UINT>(metadata.width);//幅
		//resDesc.Height = static_cast<UINT>(metadata.height);//高さ
		//resDesc.DepthOrArraySize = static_cast<uint16_t>(metadata.arraySize);//2Dで配列でもないので１
		resDesc.Width = texW;//幅
		resDesc.Height = texH;//高さ
		resDesc.DepthOrArraySize = 1;//2Dで配列でもないので１
		resDesc.SampleDesc.Count = 1;//通常テクスチャなのでアンチェリしない
		resDesc.SampleDesc.Quality = 0;//
		//resDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);//ミップマップしないのでミップ数は１つ
		resDesc.MipLevels = 1;//ミップマップしないのでミップ数は１つ
		//resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);//2Dテクスチャ用
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2Dテクスチャ用
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//レイアウトについては決定しない
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//とくにフラグなし

		ID3D12Resource* texbuff = nullptr;
		HRESULT hr0 = g_graphicsEngine->GetD3DDevice()->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,//特に指定なし
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//テクスチャ用(ピクセルシェーダから見る用)
			nullptr,
			IID_PPV_ARGS(&texbuff)
		);
		if (FAILED(hr0)) {
			_ASSERT(0);
			free(texturedata);
			return;
		}
		HRESULT hr1 = texbuff->WriteToSubresource(0,
			nullptr,//全領域へコピー
			//img->pixels,//元データアドレス
			(const void*)texturedata,
			texW * 4,//1ラインサイズ
			texW * texH * 4//全サイズ
		);
		if (FAILED(hr1)) {
			_ASSERT(0);
			free(texturedata);
			return;
		}
		
		InitFromD3DResource(texbuff);

		free(texturedata);
	}
	

}

void Texture::InitFromDDSFile(const wchar_t* filePath)
{
	//DDSファイルからテクスチャをロード。
	LoadTextureFromDDSFile(filePath);
	
}
void Texture::InitFromD3DResource(ID3D12Resource* texture)
{
	if (m_texture) {
		m_texture->Release();
	}
	m_texture = texture;
	m_texture->AddRef();
	m_textureDesc = m_texture->GetDesc();
}
void Texture::InitFromMemory(const char* memory, unsigned int size)
{
	//DDSファイルからテクスチャをロード。
	LoadTextureFromMemory(memory, size);

}
void Texture::LoadTextureFromMemory(const char* memory, unsigned int size
)
{
	auto device = g_graphicsEngine->GetD3DDevice();
	DirectX::ResourceUploadBatch re(device);
	re.Begin();
	ID3D12Resource* texture;
	auto hr = DirectX::CreateDDSTextureFromMemoryEx(
		device,
		re,
		(const uint8_t*)memory,
		size,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		(DirectX::DX12::DDS_LOADER_FLAGS)0,
		&texture
	);
	re.End(g_graphicsEngine->GetCommandQueue());

	if (FAILED(hr)) {
		//テクスチャの作成に失敗しました。
		return;
	}

	m_texture = texture;
	m_textureDesc = m_texture->GetDesc();
}
void Texture::LoadTextureFromDDSFile(const wchar_t* filePath)
{
	auto device = g_graphicsEngine->GetD3DDevice();
	DirectX::ResourceUploadBatch re(device);
	re.Begin();
	ID3D12Resource* texture;
	auto hr = DirectX::CreateDDSTextureFromFileEx(
		device,
		re,
		filePath,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		(DirectX::DX12::DDS_LOADER_FLAGS)0,
		&texture
	);
	re.End(g_graphicsEngine->GetCommandQueue());

	if (FAILED(hr)) {
		//テクスチャの作成に失敗しました。
		return;
	}

	m_texture = texture;
	m_textureDesc = m_texture->GetDesc();
}
	


//DescriptorHeapのCommitから呼ばれる
void Texture::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
{
	if (m_texture) {
		auto device = g_graphicsEngine->GetD3DDevice();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;
		device->CreateShaderResourceView(m_texture, &srvDesc, descriptorHandle);
	}
}
