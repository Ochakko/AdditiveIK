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

	ZeroMemory(&m_currentViewport, sizeof(D3D12_VIEWPORT));				//現在のビューポート。
	ZeroMemory(m_descriptorHeaps, sizeof(ID3D12DescriptorHeap*) * MAX_DESCRIPTOR_HEAP);
	ZeroMemory(m_constantBuffers, sizeof(ConstantBuffer*) * MAX_CONSTANT_BUFFER);
	ZeroMemory(m_shaderResources, sizeof(Texture*) * MAX_SHADER_RESOURCE);
	m_scratchResourceList.clear();

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


void IMCompute::SetDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);
	
	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetGraphicsRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetGraphicsRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetGraphicsRootDescriptorTable(2, descHeap.GetUavResourceGpuDescriptorStartHandle());
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
void IMCompute::WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget)
{
	WaitUntilFinishDrawingToRenderTarget(renderTarget.GetRenderTargetTexture()->Get());
}
void IMCompute::WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget)
{
	WaitUntilToPossibleSetRenderTarget(renderTarget.GetRenderTargetTexture()->Get());
}
void IMCompute::WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[])
{
	for (int i = 0; i < numRt; i++) {
		WaitUntilToPossibleSetRenderTarget(*renderTargets[i]);
	}
}
void IMCompute::WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[])
{
	for (int i = 0; i < numRt; i++) {
		WaitUntilFinishDrawingToRenderTarget(*renderTargets[i]);
	}
}
void IMCompute::SetRenderTargets(UINT numRT, RenderTarget* renderTargets[])
{
	//d
	D3D12_CPU_DESCRIPTOR_HANDLE rtDSHandleTbl[32];
	::ZeroMemory(&(rtDSHandleTbl[0]), sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * 32);//2023/11/13

	//int rtNo = 0;//2023/11/13 comment out
	for( UINT rtNo = 0; rtNo < numRT; rtNo++){
		rtDSHandleTbl[rtNo] = renderTargets[rtNo]->GetRTVCpuDescriptorHandle();
	}
	if (renderTargets[0]->IsExsitDepthStencilBuffer()) {
		//深度バッファがある。
		D3D12_CPU_DESCRIPTOR_HANDLE dsDS = renderTargets[0]->GetDSVCpuDescriptorHandle();
		m_commandList->OMSetRenderTargets(numRT, rtDSHandleTbl, FALSE, &dsDS);
	}
	else {
		//深度バッファがない。
		m_commandList->OMSetRenderTargets(numRT, rtDSHandleTbl, FALSE, nullptr);
	}

}
void IMCompute::SetRenderTargetAndViewport(RenderTarget& renderTarget)
{
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(renderTarget.GetWidth());
	viewport.Height = static_cast<float>(renderTarget.GetHeight());
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;
	SetViewportAndScissor(viewport);
	
	SetRenderTarget(renderTarget);
}
void IMCompute::SetRenderTargetsAndViewport(UINT numRT, RenderTarget* renderTargets[])
{
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(renderTargets[0]->GetWidth());
	viewport.Height = static_cast<float>(renderTargets[0]->GetHeight());
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;
	SetViewportAndScissor(viewport);
	SetRenderTargets(numRT, renderTargets);
}
void IMCompute::ClearRenderTargetViews(int numRt, RenderTarget* renderTargets[])
{
	if (renderTargets[0]->IsExsitDepthStencilBuffer()) {
		//深度バッファがある。
		ClearDepthStencilView(renderTargets[0]->GetDSVCpuDescriptorHandle(), renderTargets[0]->GetDSVClearValue());
	}
	for (int i = 0; i < numRt; i++) {
		ClearRenderTargetView(renderTargets[i]->GetRTVCpuDescriptorHandle(), renderTargets[i]->GetRTVClearColor());
	}
}
//void IMCompute::SetPipelineState(raytracing::PSO& pso)
//{
//	m_commandList->SetPipelineState1(pso.Get());
//}
