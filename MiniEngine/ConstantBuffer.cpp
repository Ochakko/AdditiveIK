#include "stdafx.h"
#include <tchar.h>
#include <stdexcept>
#include "ConstantBuffer.h"


class com_exception : public std::exception
{
public:
	com_exception(HRESULT hr) noexcept : result(hr) {}

	const char* what() const noexcept override
	{
		static char s_str[64] = {};
		sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
		return s_str;
	}

	HRESULT get_result() const noexcept { return result; }

private:
	HRESULT result;
};


ConstantBuffer::ConstantBuffer()
{
	m_constantBuffer = nullptr;
	m_constBufferCPU = nullptr;
	m_size = 0;
	m_allocSize = 0;
	m_isValid = false;
}

ConstantBuffer::~ConstantBuffer()
{
	//アンマーップ
	CD3DX12_RANGE readRange(0, 0);
	if (m_constantBuffer != nullptr) {
		m_constantBuffer->Unmap(0, &readRange);
		m_constantBuffer->Release();
	}
}
void ConstantBuffer::Init(int size, void* srcData)
{
	m_size = size;

	//D3Dデバイスを取得。
	auto device = g_graphicsEngine->GetD3DDevice();


	//定数バッファは256バイトアライメントが要求されるので、256の倍数に切り上げる。
	m_allocSize = (size + 256) & 0xFFFFFF00;

	//定数バッファの作成。
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_allocSize);
	HRESULT hrcb0 = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_constantBuffer)
	);

	if (FAILED(hrcb0) || !m_constantBuffer) {
		::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
			"ConstantBuffer::Init Error", MB_OK | MB_ICONERROR);
		abort();
	}


	//定数バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
	//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
	{
		CD3DX12_RANGE readRange(0, 0);        //     intend to read from this resource on the CPU.
		HRESULT hrmap = m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_constBufferCPU));
		if (FAILED(hrmap) || !m_constBufferCPU) {
			::MessageBoxA(NULL, "may not have enough memory? App must exit.",
				"ConstantBuffer::Init : Map Error", MB_OK | MB_ICONERROR);
			abort();
		}
	}
	if (srcData != nullptr) {
		memcpy(m_constBufferCPU, srcData, m_size);
	}

	//利用可能にする。
	m_isValid = true;
}
//void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
//{
//	//D3Dデバイスを取得。
//	auto device = g_graphicsEngine->GetD3DDevice();
//	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
//	desc.BufferLocation = m_constantBuffer[bufferNo]->GetGPUVirtualAddress();
//	desc.SizeInBytes = m_allocSize;
//	device->CreateConstantBufferView(&desc, descriptorHandle);
//}
void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	//auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
	//RegistConstantBufferView(descriptorHandle, backBufferIndex);

	//D3Dデバイスを取得。
	auto device = g_graphicsEngine->GetD3DDevice();
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	desc.SizeInBytes = m_allocSize;
	device->CreateConstantBufferView(&desc, descriptorHandle);
}
void ConstantBuffer::CopyToVRAM(void* data)
{

	//2024/02/10
	//リリースビルドがこの関数で頻繁に落ちたので
	//エラー処理を付けまくった
	//結局は　GraphicsEngineクラスのコンストラクタを書いたらエラーは解決した？

	if (!m_isValid) {
		_ASSERT(0);
		OutputDebugStringA("ConstantBuffer::CopyToVRAM : isValid error");
		::MessageBoxA(NULL, "isValid error", "ConstantBuffer::CopyToVRAM exeptionA0", MB_OK | MB_ICONERROR);
		abort();
	}
	if (!data) {
		_ASSERT(0);
		OutputDebugStringA("ConstantBuffer::CopyToVRAM : data null error");
		::MessageBoxA(NULL, "data null error", "ConstantBuffer::CopyToVRAM exeptionA", MB_OK | MB_ICONERROR);
		abort();
	}
	if (!g_graphicsEngine) {
		_ASSERT(0);
		OutputDebugStringA("ConstantBuffer::CopyToVRAM : graphicsEngine null error");
		::MessageBoxA(NULL, "graphicsEngine null error", "ConstantBuffer::CopyToVRAM exeptionB", MB_OK | MB_ICONERROR);
		abort();
	}

	try {
		if (!m_constBufferCPU) {
			_ASSERT(0);
			OutputDebugStringA("ConstantBuffer::CopyToVRAM : m_constBufferCPU error");
			::MessageBoxA(NULL, "m_constBufferCPU null error", "ConstantBuffer::CopyToVRAM exeptionD", MB_OK | MB_ICONERROR);
			abort();
		}
		if (m_size <= 0) {
			_ASSERT(0);
			OutputDebugStringA("ConstantBuffer::CopyToVRAM : m_size error");
			::MessageBoxA(NULL, "m_size zero error", "ConstantBuffer::CopyToVRAM exeptionE", MB_OK | MB_ICONERROR);
			abort();
		}

		memcpy(m_constBufferCPU, data, m_size);

	}
	//catch (const com_exception& exc)
	//{
	//	_ASSERT(0);
	//	OutputDebugStringA(exc.what());
	//	::MessageBoxA(NULL, exc.what(), "ConstantBuffer::CopyToVRAM exeption0", MB_OK | MB_ICONERROR);
	//	abort();
	//}
	//catch (const std::exception& exc)
	//{
	//	_ASSERT(0);
	//	OutputDebugStringA(exc.what());
	//	::MessageBoxA(NULL, exc.what(), "ConstantBuffer::CopyToVRAM exeption1", MB_OK | MB_ICONERROR);
	//	abort();
	//}
	catch (...) {
		_ASSERT(0);
		OutputDebugStringA("memcpy error : app must exit.");
		::MessageBox(NULL, L"memcpy error : app must exit.", L"ConstantBuffer::CopyToVRAM exeption3", MB_OK | MB_ICONERROR);
		abort();
	}
}
D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress()
{
	//auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
	//return m_constantBuffer[backBufferIndex]->GetGPUVirtualAddress();

	return m_constantBuffer->GetGPUVirtualAddress();
}