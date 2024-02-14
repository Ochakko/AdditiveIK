#include "stdafx.h"
#include "Texture.h"

#include <ChaVecCalc.h>
#include <mqomaterial.h>
#include "../../DirectXTex/DirectXTex/DirectXTex.h"

Texture::Texture(const wchar_t* filePath)
{
	InitFromDDSFile(filePath);
}
Texture::~Texture()
{
	////if (!IsRenderTarget() && m_texture) {//2023/11/25
	//if (m_texture) {//2024/02/07 RenderTargetの場合InitFromD3DResourceで元のテクスチャをreleaseしてm_textureをAddrefしたので、ここでm_textureをreleaseしても良い
	//	m_texture->Release();
	//	m_texture = nullptr;
	//}
	ReleaseTexture();

	IShaderResource::~IShaderResource();
}

void Texture::ReleaseTexture()
{
	if (m_texture) {//2024/02/07 RenderTargetの場合InitFromD3DResourceで元のテクスチャをreleaseしてm_textureをAddrefしたので、ここでm_textureをreleaseしても良い
		m_texture->Release();
		m_texture = nullptr;
	}
}

int Texture::InitFromCustomColor(ChaVector4 srccol)
{
	if (!g_graphicsEngine) {
		_ASSERT(0);
		return 1;
	}
	if (!g_graphicsEngine->GetD3DDevice()) {
		_ASSERT(0);
		return 1;
	}
	ReleaseTexture();

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
		if (FAILED(hr0) || !texbuff) {
			::MessageBoxA(NULL, "CreateTexture error. App must exit.",
				"Texture::InitFromCustomColor Error", MB_OK | MB_ICONERROR);
			_ASSERT(0);
			abort();
			free(texturedata);
			//return 1;
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
			::MessageBoxA(NULL, "Write To VideoMemory error. App must exit.",
				"Texture::InitFromCustomColor Error", MB_OK | MB_ICONERROR);
			abort();
			free(texturedata);
			//return 1;
		}
		
		InitFromD3DResource(texbuff);

		free(texturedata);
	}
	else {
		_ASSERT(0);
		::MessageBoxA(NULL, "memory alloc error. App must exit.",
			"Texture::InitFromCustomColor Error", MB_OK | MB_ICONERROR);
		abort();
	}
	
	return 0;

}

//ToonTexture
int Texture::InitToonFromCustomColor(tag_hsvtoon* phsvtoon)
{
	if (!g_graphicsEngine) {
		_ASSERT(0);
		return 1;
	}
	if (!g_graphicsEngine->GetD3DDevice()) {
		_ASSERT(0);
		return 1;
	}
	if (!phsvtoon) {
		_ASSERT(0);
		return 1;
	}
	ReleaseTexture();

	UINT texW, texH;
	texW = 256;
	texH = 256;

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
	if (FAILED(hr0) || !texbuff) {
		::MessageBoxA(NULL, "CreateTexture error. App must exit.",
			"Texture::InitToonFromCustomColor Error", MB_OK | MB_ICONERROR);
		_ASSERT(0);
		abort();
		//return 1;
	}


	int result1 = WriteToonToSubResource(phsvtoon, texbuff);
	if (result1 != 0) {
		_ASSERT(0);
		::MessageBoxA(NULL, "WriteToonToSubResource error. App must exit.",
			"Texture::InitToonFromCustomColor Error", MB_OK | MB_ICONERROR);
		abort();
		//return 1;
	}


	//InitFromD3DResource(texbuff);//WriteToonToSubResource()内でInitFromD3DResource()を呼んでいる


	return 0;

}

int Texture::WriteToonToSubResource(tag_hsvtoon* phsvtoon, ID3D12Resource* srctexbuff)
{
	if (!phsvtoon) {
		_ASSERT(0);
		return 1;
	}

	ID3D12Resource* texbuff = nullptr;
	UINT texW, texH;
	if (srctexbuff != nullptr) {
		texbuff = srctexbuff;
	}
	else {
		texbuff = m_texture;
	}
	if (!texbuff) {
		_ASSERT(0);
		return 1;
	}
	D3D12_RESOURCE_DESC textureDesc;
	textureDesc = texbuff->GetDesc();
	texW = (UINT)textureDesc.Width;
	texH = (UINT)textureDesc.Height;

	ChaVector4 basecolor = phsvtoon->basehsv.HSV2RGB();

	struct TexRGBA {
		unsigned char R, G, B, A;
	};

	TexRGBA basergba;
	{
		double colR = (double)basecolor.x * 255.0;
		double colG = (double)basecolor.y * 255.0;
		double colB = (double)basecolor.z * 255.0;
		double colA = (double)basecolor.w * 255.0;
		basergba.R = (unsigned char)(fmax(0.0, fmin(255.0, colR)));
		basergba.G = (unsigned char)(fmax(0.0, fmin(255.0, colG)));
		basergba.B = (unsigned char)(fmax(0.0, fmin(255.0, colB)));
		basergba.A = (unsigned char)(fmax(0.0, fmin(255.0, colA)));
	}

	TexRGBA hirgba;
	{
		ChaVector4 hicol = basecolor;
		hicol.HSV_Add(phsvtoon->hiaddhsv);

		double colR = (double)hicol.x * 255.0;
		double colG = (double)hicol.y * 255.0;
		double colB = (double)hicol.z * 255.0;
		double colA = (double)hicol.w * 255.0;
		hirgba.R = (unsigned char)(fmax(0.0, fmin(255.0, colR)));
		hirgba.G = (unsigned char)(fmax(0.0, fmin(255.0, colG)));
		hirgba.B = (unsigned char)(fmax(0.0, fmin(255.0, colB)));
		hirgba.A = (unsigned char)(fmax(0.0, fmin(255.0, colA)));
	}

	TexRGBA lowrgba;
	{
		ChaVector4 lowcol = basecolor;
		lowcol.HSV_Add(phsvtoon->lowaddhsv);

		double colR = (double)lowcol.x * 255.0;
		double colG = (double)lowcol.y * 255.0;
		double colB = (double)lowcol.z * 255.0;
		double colA = (double)lowcol.w * 255.0;
		lowrgba.R = (unsigned char)(fmax(0.0, fmin(255.0, colR)));
		lowrgba.G = (unsigned char)(fmax(0.0, fmin(255.0, colG)));
		lowrgba.B = (unsigned char)(fmax(0.0, fmin(255.0, colB)));
		lowrgba.A = (unsigned char)(fmax(0.0, fmin(255.0, colA)));
	}


	unsigned char* texturedata = (unsigned char*)malloc(sizeof(unsigned char) * texW * texH * 4);


	if (texturedata) {
		UINT thhi = (UINT)(phsvtoon->hicolorh * (float)(texH - 1) + 0.01f);
		UINT thlow = (UINT)(phsvtoon->lowcolorh * (float)(texH - 1) + 0.01f);
		thhi = min((texH - 1), thhi);
		thhi = max(0, thhi);
		thlow = min((texH - 1), thlow);
		thlow = max(0, thlow);

		UINT indexw, indexh;
		for (indexh = 0; indexh < texH; indexh++) {
			for (indexw = 0; indexw < texW; indexw++) {
				unsigned char* ppix = texturedata + (indexh * texW + indexw) * 4;
				
				if ((indexh >= thlow) && (indexh <= thhi)) {
					*(ppix) = basergba.R;
					*(ppix + 1) = basergba.G;
					*(ppix + 2) = basergba.B;
					*(ppix + 3) = basergba.A;
				}
				else if (indexh < thlow) {
					*(ppix) = lowrgba.R;
					*(ppix + 1) = lowrgba.G;
					*(ppix + 2) = lowrgba.B;
					*(ppix + 3) = lowrgba.A;
				}
				else {
					*(ppix) = hirgba.R;
					*(ppix + 1) = hirgba.G;
					*(ppix + 2) = hirgba.B;
					*(ppix + 3) = hirgba.A;
				}

				//if (indexh >= thhi) {
				//	*(ppix) = hirgba.R;
				//	*(ppix + 1) = hirgba.G;
				//	*(ppix + 2) = hirgba.B;
				//	*(ppix + 3) = hirgba.A;
				//}
				//else if (indexh <= thlow) {
				//	*(ppix) = lowrgba.R;
				//	*(ppix + 1) = lowrgba.G;
				//	*(ppix + 2) = lowrgba.B;
				//	*(ppix + 3) = lowrgba.A;
				//}
				//else {
				//	*(ppix) = basergba.R;
				//	*(ppix + 1) = basergba.G;
				//	*(ppix + 2) = basergba.B;
				//	*(ppix + 3) = basergba.A;
				//}
			}
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
			::MessageBoxA(NULL, "Write To VideoMemory error. App must exit.",
				"Texture::WriteToonToSubResource Error", MB_OK | MB_ICONERROR);
			abort();
			free(texturedata);
			//return 1;
		}

		free(texturedata);
	}
	else {
		_ASSERT(0);
		::MessageBoxA(NULL, "memory alloc error. App must exit.",
			"Texture::WriteToonToSubResource Error", MB_OK | MB_ICONERROR);
		abort();
	}

	InitFromD3DResource(texbuff);


	return 0;
}



int Texture::InitFromWICFile(const wchar_t* filePath)
{
	if (!g_graphicsEngine) {
		_ASSERT(0);
		return 1;
	}
	if (!g_graphicsEngine->GetD3DDevice()) {
		_ASSERT(0);
		return 1;
	}
	ReleaseTexture();


	//WICテクスチャのロード
	DirectX::TexMetadata metadata = {};
	std::unique_ptr<DirectX::ScratchImage> scratchImg(new DirectX::ScratchImage);
	HRESULT hr0 = DirectX::LoadFromWICFile(filePath, DirectX::WIC_FLAGS_NONE, &metadata, *scratchImg);
	if (FAILED(hr0)) {
		_ASSERT(0);
		::MessageBoxA(NULL, "TextureFile not found error. App must exit.",
			"Texture::InitFromWICFile Error", MB_OK | MB_ICONERROR);
		abort();
		//return 1;
	}
	auto img = scratchImg->GetImage(0, 0, 0);//生データ抽出

	//WriteToSubresourceで転送する用のヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//特殊な設定なのでdefaultでもuploadでもなく
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//ライトバックで
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//転送がL0つまりCPU側から直で
	texHeapProp.CreationNodeMask = 0;//単一アダプタのため0
	texHeapProp.VisibleNodeMask = 0;//単一アダプタのため0

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = metadata.format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBAフォーマット
	//resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBAフォーマット
	resDesc.Width = static_cast<UINT>(metadata.width);//幅
	resDesc.Height = static_cast<UINT>(metadata.height);//高さ
	resDesc.DepthOrArraySize = static_cast<uint16_t>(metadata.arraySize);//2Dで配列でもないので１
	resDesc.SampleDesc.Count = 1;//通常テクスチャなのでアンチェリしない
	resDesc.SampleDesc.Quality = 0;//
	resDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);//ミップマップしないのでミップ数は１つ
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);//2Dテクスチャ用
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//レイアウトについては決定しない
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//とくにフラグなし

	ID3D12Resource* texbuff = nullptr;
	HRESULT hrwic1 = g_graphicsEngine->GetD3DDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,//特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//テクスチャ用(ピクセルシェーダから見る用)
		nullptr,
		IID_PPV_ARGS(&texbuff)
	);
	if (FAILED(hrwic1) || !texbuff) {
		::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
			"Texture::InitFromWICFile Error", MB_OK | MB_ICONERROR);
		abort();

		//_ASSERT(0);
		//scratchImg.reset();
		//return;
	}

	HRESULT hr2 = texbuff->WriteToSubresource(0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		static_cast<UINT>(img->rowPitch),//1ラインサイズ
		static_cast<UINT>(img->slicePitch)//全サイズ
	);
	if (FAILED(hr2)) {
		_ASSERT(0);
		::MessageBoxA(NULL, "Write To videomemory Error. App must exit.",
			"Texture::InitFromWICFile Error", MB_OK | MB_ICONERROR);
		abort();

		scratchImg.reset();
		//return;
	}

	InitFromD3DResource(texbuff);
	scratchImg.reset();

	return 0;
}

void Texture::InitFromDDSFile(const wchar_t* filePath)
{
	//DDSファイルからテクスチャをロード。
	LoadTextureFromDDSFile(filePath);
	
}
void Texture::InitFromD3DResource(ID3D12Resource* texture)
{
	if (m_texture != texture) {//srcとdstが同じ時に実行するとm_textureが解放されてしまう
		ReleaseTexture();

		m_texture = texture;
		m_texture->AddRef();
		texture->Release();//2023/11/23

		m_textureDesc = m_texture->GetDesc();
	}
}
void Texture::InitFromMemory(const char* memory, unsigned int size)
{
	//DDSファイルからテクスチャをロード。
	LoadTextureFromMemory(memory, size);

}
void Texture::LoadTextureFromMemory(const char* memory, unsigned int size
)
{
	ReleaseTexture();


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
	ReleaseTexture();

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
