#include "stdafx.h"
#include "IMCompute.h"

static ID3D12CommandQueue* s_commandQueue = nullptr;			//コマンドキュー。


IMCompute::IMCompute()
{
	m_dev = nullptr;
	m_allocator = nullptr;
	m_commandList = nullptr;	//コマンドリスト。
	//m_commandQueue = nullptr;
	m_fenceEvent = nullptr;
	m_fence = nullptr;
	m_fenceValue = 0;

	ZeroMemory(m_descriptorHeaps, sizeof(ID3D12DescriptorHeap*) * MAX_DESCRIPTOR_HEAP);
	ZeroMemory(m_constantBuffers, sizeof(ConstantBuffer*) * MAX_CONSTANT_BUFFER);
	ZeroMemory(m_shaderResources, sizeof(Texture*) * MAX_SHADER_RESOURCE);

};

IMCompute::~IMCompute()
{
	WaitDraw();

	if (m_fence) {
		m_fence->Release();
	}
	if (m_commandList) {
		m_commandList->Release();
		m_commandList = nullptr;	//コマンドリスト。
	}
	//if (m_commandQueue) {
	//	m_commandQueue->Release();
	//	m_commandQueue = nullptr;
	//}
	if (m_allocator) {
		m_allocator->Release();
		m_allocator = nullptr;
	}
	if (m_fenceEvent) {
		CloseHandle(m_fenceEvent);
	}
	

}

int IMCompute::Init(ID3D12Device* pdev)
{
	m_dev = pdev;
	HRESULT hr = CreateCommandList(pdev);
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	return 0;
}


//staitc
int IMCompute::InitStaticMem(ID3D12Device* pdev)
{
	//for s_commandQueue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	//queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	HRESULT hr = pdev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_commandQueue));
	if (FAILED(hr)) {
		//コマンドキューの作成に失敗した。
		_ASSERT(0);
		return 1;
	}

	return 0;
}

//static 
void IMCompute::DestroyStaticMem()
{
	//for s_commandQueue
	if (s_commandQueue) {
		s_commandQueue->Release();
		s_commandQueue = nullptr;
	}

}

HRESULT IMCompute::CreateCommandList(ID3D12Device* pdev)
{

	HRESULT hr = pdev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE,
		IID_PPV_ARGS(&m_allocator));
	if (FAILED(hr)) {
		_ASSERT(0);
		return hr;
	}

	hr = pdev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_allocator, nullptr,
		IID_PPV_ARGS(&m_commandList));
	if (FAILED(hr)) {
		_ASSERT(0);
		return hr;
	}
	m_commandList->Close();//!!!!!!!!!! 最初のReset()時にClose()が呼ばれてない場合に警告が出るため


	//D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	//queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	////queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	//queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//hr = pdev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
	//if (FAILED(hr)) {
	//	//コマンドキューの作成に失敗した。
	//	_ASSERT(0);
	//	return hr;
	//}

	pdev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (!m_fence) {
		//フェンスの作成に失敗した。
		_ASSERT(0);
		return hr;
	}
	m_fenceValue = 1;
	//同期を行うときのイベントハンドラを作成する。
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		_ASSERT(0);
		return hr;
	}

	return hr;
}

void IMCompute::IMExecute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
	if (!m_commandList || !s_commandQueue || !m_allocator)
	{
		_ASSERT(0);
		return;
	}



	//#####################################################################################
	//Reset(), SetComputeRootSignature(), SetPipelineState(), SetComputeDescriptorHeap()を
	//この関数を呼び出す前に呼び出す必要有
	//#####################################################################################
	////コマンドのリセット
	//allo->Reset();
	//list->Reset(allo, nullptr);
	////それぞれのセット
	//list->SetComputeRootSignature(root);
	//list->SetPipelineState(pipe);
	//list->SetDescriptorHeaps(1, &h);
	//auto handle = heap->GetGPUDescriptorHandleForHeapStart();
	//list->SetComputeRootDescriptorTable(0, handle);




	//コンピュートシェーダーの実行(今回は256個のスレッドグループを指定)
	Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	m_commandList->Close();

	//コマンドの実行
	ID3D12CommandList* ppCommandLists[] = { (ID3D12CommandList*)m_commandList };
	s_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	WaitDraw();

}

void IMCompute::WaitDraw()
{
	if (!s_commandQueue || !m_fence) {
		_ASSERT(0);
		return;
	}

	//描画終了待ち
	// Signal and increment the fence value.
	const UINT64 fence = m_fenceValue;
	s_commandQueue->Signal(m_fence, fence);
	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < fence)
	{
		m_fence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}


void IMCompute::SetComputeDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetComputeRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetComputeRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetComputeRootDescriptorTable(2, descHeap.GetUavResourceGpuDescriptorStartHandle());
	}
}

