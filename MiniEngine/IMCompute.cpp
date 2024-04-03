#include "stdafx.h"
#include "IMCompute.h"

static ID3D12CommandQueue* s_commandQueue = nullptr;			//コマンドキュー。
static ID3D12CommandAllocator* s_allocator = nullptr;
static ID3D12GraphicsCommandList4* s_commandList = nullptr;	//コマンドリスト。
static ID3D12Fence* s_fence = nullptr;
static HANDLE s_fenceEvent = nullptr;
static UINT64 s_fenceValue = 0;


IMCompute::IMCompute()
{
	m_dev = nullptr;
	//m_allocator = nullptr;
	//m_commandList = nullptr;	//コマンドリスト。
	//m_commandQueue = nullptr;
	//m_fenceEvent = nullptr;
	//m_fence = nullptr;
	//m_fenceValue = 0;

	ZeroMemory(m_descriptorHeaps, sizeof(ID3D12DescriptorHeap*) * MAX_DESCRIPTOR_HEAP);
	ZeroMemory(m_constantBuffers, sizeof(ConstantBuffer*) * MAX_CONSTANT_BUFFER);
	ZeroMemory(m_shaderResources, sizeof(Texture*) * MAX_SHADER_RESOURCE);

};

IMCompute::~IMCompute()
{
	WaitDraw();
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

	hr = pdev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE,
		IID_PPV_ARGS(&s_allocator));
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}

	hr = pdev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, s_allocator, nullptr,
		IID_PPV_ARGS(&s_commandList));
	if (FAILED(hr)) {
		_ASSERT(0);
		return 1;
	}
	s_commandList->Close();//!!!!!!!!!! 最初のReset()時にClose()が呼ばれてない場合に警告が出るため


	pdev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&s_fence));
	if (!s_fence) {
		//フェンスの作成に失敗した。
		_ASSERT(0);
		return 1;
	}
	s_fenceValue = 1;
	//同期を行うときのイベントハンドラを作成する。
	s_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (s_fenceEvent == nullptr) {
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

	if (s_fence) {
		s_fence->Release();
	}
	if (s_commandList) {
		s_commandList->Release();
		s_commandList = nullptr;	//コマンドリスト。
	}
	if (s_allocator) {
		s_allocator->Release();
		s_allocator = nullptr;
	}

	if (s_fenceEvent) {
		CloseHandle(s_fenceEvent);
	}


}

HRESULT IMCompute::CreateCommandList(ID3D12Device* pdev)
{
	//HRESULT hr = pdev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE,
	//	IID_PPV_ARGS(&m_allocator));
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return hr;
	//}

	//hr = pdev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_allocator, nullptr,
	//	IID_PPV_ARGS(&m_commandList));
	//if (FAILED(hr)) {
	//	_ASSERT(0);
	//	return hr;
	//}
	//m_commandList->Close();//!!!!!!!!!! 最初のReset()時にClose()が呼ばれてない場合に警告が出るため


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

	//pdev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	//if (!m_fence) {
	//	//フェンスの作成に失敗した。
	//	_ASSERT(0);
	//	return hr;
	//}
	//m_fenceValue = 1;
	////同期を行うときのイベントハンドラを作成する。
	//m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	//if (m_fenceEvent == nullptr) {
	//	_ASSERT(0);
	//	return hr;
	//}

	return 0;
}

void IMCompute::IMExecute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
	if (!s_commandList || !s_commandQueue || !s_allocator)
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
	s_commandList->Close();

	//コマンドの実行
	ID3D12CommandList* ppCommandLists[] = { (ID3D12CommandList*)s_commandList };
	s_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	WaitDraw();

}

void IMCompute::WaitDraw()
{
	if (!s_commandQueue || !s_fence) {
		_ASSERT(0);
		return;
	}

	//描画終了待ち
	// Signal and increment the fence value.
	const UINT64 fence = s_fenceValue;
	s_commandQueue->Signal(s_fence, fence);
	s_fenceValue++;

	// Wait until the previous frame is finished.
	if (s_fence->GetCompletedValue() < fence)
	{
		s_fence->SetEventOnCompletion(fence, s_fenceEvent);
		WaitForSingleObject(s_fenceEvent, INFINITE);
	}
}


void IMCompute::SetComputeDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	s_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);

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



void IMCompute::SetComputeRootSignature(ID3D12RootSignature* rootSignature)
{
	if (s_commandList) {
		s_commandList->SetComputeRootSignature(rootSignature);
	}
	else {
		_ASSERT(0);
	}
}
void IMCompute::SetComputeRootSignature(RootSignature& rootSignature)
{
	if (s_commandList) {
		s_commandList->SetComputeRootSignature(rootSignature.Get());
	}
	else {
		_ASSERT(0);
	}
}

void IMCompute::SetPipelineState(ID3D12PipelineState* pipelineState)
{
	if (s_commandList) {
		s_commandList->SetPipelineState(pipelineState);
	}
	else {
		_ASSERT(0);
	}
}
void IMCompute::SetPipelineState(PipelineState& pipelineState)
{
	if (s_commandList) {
		s_commandList->SetPipelineState(pipelineState.Get());
	}
	else {
		_ASSERT(0);
	}
}

void IMCompute::Close()
{
	if (s_commandList) {
		s_commandList->Close();
	}
	else {
		_ASSERT(0);
	}
}

void IMCompute::Reset(ID3D12PipelineState* pipelineState)
{
	if (s_allocator && s_commandList) {
		s_allocator->Reset();
		s_commandList->Reset(s_allocator, pipelineState);
	}
	else {
		_ASSERT(0);
	}
}

void IMCompute::Dispatch(
	UINT ThreadGroupCountX,
	UINT ThreadGroupCountY,
	UINT ThreadGroupCountZ)
{
	if (s_commandList) {
		s_commandList->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}
	else {
		_ASSERT(0);
	}
}

void IMCompute::SetComputeRootDescriptorTable(
	UINT RootParameterIndex,
	D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
{
	if (s_commandList) {
		s_commandList->SetComputeRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}
	else {
		_ASSERT(0);
	}
}
