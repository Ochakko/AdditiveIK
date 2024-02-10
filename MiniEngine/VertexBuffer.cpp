#include "stdafx.h"
#include "VertexBuffer.h"


VertexBuffer::~VertexBuffer()
{
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}
void VertexBuffer::Init(int size, int stride)
{
	auto d3dDevice = g_graphicsEngine->GetD3DDevice();
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	HRESULT hrvb2 = d3dDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer));
	if (FAILED(hrvb2) || !m_vertexBuffer) {
		::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
			"VertexBuffer::Init Error", MB_OK | MB_ICONERROR);
		abort();
	}


	m_vertexBuffer->SetName(L"VertexBuffer");
	//頂点バッファのビューを作成。
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = size;
	m_vertexBufferView.StrideInBytes = stride;
}
void VertexBuffer::Copy(void* srcVertices)
{
	uint8_t* pData;
	m_vertexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, srcVertices, m_vertexBufferView.SizeInBytes);
	m_vertexBuffer->Unmap(0, nullptr);
}