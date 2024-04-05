#include "stdafx.h"
#include "VertexBuffer.h"

static int s_vbcreatecount = 0;

VertexBuffer::~VertexBuffer()
{
	DestroyObjs();
}
void VertexBuffer::DestroyObjs()
{
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void VertexBuffer::Init(int size, int stride)
{

	if (s_vbcreatecount >= 64) {
		int dbgflag1 = 1;
	}

	DestroyObjs();

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

	WCHAR objname[1024] = { 0L };
	swprintf_s(objname, 1024, L"VertexBuffer:Init:vertexBuffer_%d", s_vbcreatecount);
	s_vbcreatecount++;
	m_vertexBuffer->SetName(objname);


	//頂点バッファのビューを作成。
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = size;
	m_vertexBufferView.StrideInBytes = stride;
}
void VertexBuffer::Copy(void* srcVertices)
{
	uint8_t* pData;
	m_vertexBuffer->Map(0, nullptr, (void**)&pData);
	if (pData) {
		memcpy(pData, srcVertices, m_vertexBufferView.SizeInBytes);
		m_vertexBuffer->Unmap(0, nullptr);
	}
}