﻿#include "stdafx.h"
#include "GPUBuffer.h"

void GPUBuffer::Init(D3D12_RESOURCE_DESC desc)
{
	m_desc = desc;
	//todo まとめた方がええな。
	const D3D12_HEAP_PROPERTIES kDefaultHeapProps =
	{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	};

	auto d3dDevice = g_graphicsEngine->GetD3DDevice();
	HRESULT hrgpu0 = d3dDevice->CreateCommittedResource(
		&kDefaultHeapProps, 
		D3D12_HEAP_FLAG_NONE, 
		&desc,
		D3D12_RESOURCE_STATE_COPY_SOURCE, 
		nullptr, 
		IID_PPV_ARGS(&m_buffer)
	); 
	if (FAILED(hrgpu0) || !m_buffer) {
		::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
			"GPUBuffer::Init Error", MB_OK | MB_ICONERROR);
		abort();
	}

	m_buffer->SetName(L"GPUBuffer:Init:buffer");

}
void GPUBuffer::RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	if (m_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}
	else {
		//未対応。
		std::abort();
	}
	auto d3dDevice = g_graphicsEngine->GetD3DDevice();
	d3dDevice->CreateUnorderedAccessView(
		m_buffer, 
		nullptr, 
		&uavDesc, 
		cpuHandle
	);

	
}
void GPUBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
}