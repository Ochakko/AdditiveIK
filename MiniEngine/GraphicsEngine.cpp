#include "stdafx.h"
#include "GraphicsEngine.h"

#include <GlobalVar.h>
#include <ChaScene.h>

GraphicsEngine* g_graphicsEngine = nullptr;	//グラフィックスエンジン
Camera* g_camera2D = nullptr;				//2Dカメラ。
Camera* g_camera3D = nullptr;				//3Dカメラ。
Camera* g_cameraShadow = nullptr;			//ShadowMapカメラ


//2024/02/10 コンストラクタを書くことにより　ConstantBuffer::CopyToVRAMで落ちなくなった
GraphicsEngine::GraphicsEngine() :
m_renderContext(), m_nullTextureMaps(), m_fontEngine(),
m_camera2D(), m_camera3D(), m_cameraShadow()
{
	m_d3dDevice = nullptr;					//D3Dデバイス。
	m_commandQueue = nullptr;			//コマンドキュー。
	m_swapChain = nullptr;					//スワップチェイン。
	m_rtvHeap = nullptr;				//レンダリングターゲットビューのディスクリプタヒープ。
	m_dsvHeap = nullptr;				//深度ステンシルビューのディスクリプタヒープ。
	m_commandAllocator = nullptr;	//コマンドアロケータ。
	m_commandList = nullptr;		//コマンドリスト。
	m_pipelineState = nullptr;			//パイプラインステート。
	m_currentBackBufferIndex = 0;						//現在のバックバッファの番号。
	m_rtvDescriptorSize = 0;							//フレームバッファのディスクリプタのサイズ。
	m_dsvDescriptorSize = 0;							//深度ステンシルバッファのディスクリプタのサイズ。
	m_cbrSrvDescriptorSize = 0;						//CBR_SRVのディスクリプタのサイズ。
	m_samplerDescriptorSize = 0;					//サンプラのディスクリプタのサイズ。			
	m_renderTargets[0] = nullptr;	//フレームバッファ用のレンダリングターゲット。
	m_renderTargets[1] = nullptr;	//フレームバッファ用のレンダリングターゲット。
	m_depthStencilBuffer = nullptr;	//深度ステンシルバッファ。
	ZeroMemory(&m_viewport, sizeof(D3D12_VIEWPORT));			//ビューポート。
	ZeroMemory(&m_scissorRect, sizeof(D3D12_RECT));			//シザリング矩形。
	//RenderContext m_renderContext;		//レンダリングコンテキスト。
	m_currentFrameBufferRTVHandle.ptr = 0;		//現在書き込み中のフレームバッファのレンダリングターゲットビューのハンドル。
	m_currentFrameBufferDSVHandle.ptr = 0;		//現在書き込み中のフレームバッファの深度ステンシルビューの
	// GPUとの同期で使用する変数。
	m_frameIndex = 0;
	m_fenceEvent = nullptr;
	m_fence = nullptr;
	m_fenceValue = 0;
	m_frameBufferWidth = 0;				//フレームバッファの幅。
	m_frameBufferHeight = 0;				//フレームバッファの高さ。
	//Camera m_camera2D;							//2Dカメラ。
	//Camera m_camera3D;							//3Dカメラ。
	//Camera m_cameraShadow;						//ShadowMapカメラ。
	////raytracing::Engine m_raytracingEngine;		//レイトレエンジン。
	//NullTextureMaps m_nullTextureMaps;			//ヌルテクスチャマップ。
	//FontEngine m_fontEngine;					//フォントエンジン。
	m_directXTKGfxMemroy = nullptr;	//DirectXTKのグラフィックメモリシステム。
}

GraphicsEngine::~GraphicsEngine()
{
	WaitDraw();
	//後始末。
	if (m_commandQueue) {
		m_commandQueue->Release();
	}
	if (m_swapChain) {
		m_swapChain->Release();
	}
	if (m_rtvHeap) {
		m_rtvHeap->Release();
	}
	if (m_dsvHeap) {
		m_dsvHeap->Release();
	}
	if (m_commandAllocator) {
		m_commandAllocator->Release();
	}
	if (m_commandList) {
		m_commandList->Release();
	}
	if (m_pipelineState) {
		m_pipelineState->Release();
	}

	int targetindex;
	for (targetindex = 0; targetindex < FRAME_BUFFER_COUNT; targetindex++) {
		ID3D12Resource* curtarget = m_renderTargets[targetindex];
		if (curtarget) {
			curtarget->Release();
		}
		m_renderTargets[targetindex] = nullptr;
	}

	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
	}
	if (m_fence) {
		m_fence->Release();
	}

	m_directXTKGfxMemroy.reset();
	m_directXTKGfxMemroy = nullptr;

	if (m_d3dDevice) {
		m_d3dDevice->Release();
	}

	CloseHandle(m_fenceEvent);

}
void GraphicsEngine::WaitDraw()
{
	//描画終了待ち
	// Signal and increment the fence value.
	const UINT64 fence = m_fenceValue;
	m_commandQueue->Signal(m_fence, fence);
	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < fence)
	{
		m_fence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}
bool GraphicsEngine::Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight)
{
	//
	g_graphicsEngine = this;

	


	m_frameBufferWidth = frameBufferWidth;
	m_frameBufferHeight = frameBufferHeight;

	//デバイスにアクセスするためのインターフェースを作成。
	auto dxgiFactory = CreateDXGIFactory();
	//D3Dデバイスの作成。
	if (!CreateD3DDevice( dxgiFactory ) ) {
		//D3Dデバイスの作成に失敗した。
		MessageBox(hwnd, TEXT("D3Dデバイスの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}


	//コマンドキューの作成。
	if (!CreateCommandQueue()) {
		//コマンドキューの作成に失敗した。
		MessageBox(hwnd, TEXT("コマンドキューの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}
	//スワップチェインを作成。
	if (!CreateSwapChain(hwnd, frameBufferWidth, frameBufferHeight, dxgiFactory)) {
		//スワップチェインの作成に失敗。
		MessageBox(hwnd, TEXT("スワップチェインの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}
	
	//フレームバッファ用のディスクリプタヒープを作成する。
	if (!CreateDescriptorHeapForFrameBuffer()) {
		MessageBox(hwnd, TEXT("フレームバッファ用のディスクリプタヒープの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}

	//フレームバッファ用のRTVを作成する。
	if (!CreateRTVForFameBuffer()) {
		MessageBox(hwnd, TEXT("フレームバッファ用のRTVの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;

	}

	//フレームバッファ用のDSVを作成する。
	if (!CreateDSVForFrameBuffer(frameBufferWidth, frameBufferHeight)) {
		MessageBox(hwnd, TEXT("フレームバッファ用のDSVの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}

	//コマンドアロケータの作成。
	m_d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_commandAllocator));

	if (!m_commandAllocator) {
		MessageBox(hwnd, TEXT("コマンドアロケータの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}

	//コマンドリストの作成。
	if (!CreateCommandList()) {
		MessageBox(hwnd, TEXT("コマンドリストの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}

	//GPUと同期をとるためのオブジェクトを作成する。
	if (!CreateSynchronizationWithGPUObject()) {
		MessageBox(hwnd, TEXT("GPUと同期をとるためのオブジェクトの作成に失敗しました。"), TEXT("エラー"), MB_OK);
		return false;
	}
	
	//レンダリングコンテキストの作成。
	m_renderContext.Init(m_commandList);

	//ビューポートを初期化。
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<FLOAT>(frameBufferWidth);
	m_viewport.Height = static_cast<FLOAT>(frameBufferHeight);
	m_viewport.MinDepth = D3D12_MIN_DEPTH;
	m_viewport.MaxDepth = D3D12_MAX_DEPTH;

	//シザリング矩形を初期化。
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = frameBufferWidth;
	m_scissorRect.bottom = frameBufferHeight;

	//CBR_SVRのディスクリプタのサイズを取得。
	m_cbrSrvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//Samplerのディスクリプタのサイズを取得。
	m_samplerDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	//初期化が終わったのでDXGIを破棄。
	dxgiFactory->Release();

	//ヌルテクスチャを初期化
	m_nullTextureMaps.Init();

	//カメラを初期化する。
	m_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
	m_camera2D.SetWidth( static_cast<float>(m_frameBufferWidth) );
	m_camera2D.SetHeight( static_cast<float>(m_frameBufferHeight) );
	m_camera2D.SetPosition({0.0f, 0.0f, -1.0f});
	m_camera2D.SetTarget({ 0.0f, 0.0f, 0.0f });

	m_camera3D.SetPosition({0.0f, 50.0f, 200.0f} );
	m_camera3D.SetTarget({ 0.0f, 50.0f, 0.0f });

	m_cameraShadow.SetPosition({ 0.0f, 50.0f, 200.0f });
	m_cameraShadow.SetTarget({ 0.0f, 50.0f, 0.0f });

	g_camera2D = &m_camera2D;
	g_camera3D = &m_camera3D;
	g_cameraShadow = &m_cameraShadow;

	//DirectXTK用のグラフィックメモリ管理クラスのインスタンスを作成する。
	m_directXTKGfxMemroy = std::make_unique<DirectX::GraphicsMemory>(m_d3dDevice);
	//フォント描画エンジンを初期化。
	m_fontEngine.Init();
	
	return true;
}

IDXGIFactory4* GraphicsEngine::CreateDXGIFactory()
{
	UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
	//デバッグコントローラーがあれば、デバッグレイヤーがあるDXGIを作成する。
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();

		// Enable additional debug layers.
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

		//2023/11/23 ここでリリースすると　リークして無くてもダンプ情報が出るのでコメントアウト
		// 2023/11/27 補足　
		// 解釈が合っているかどうかについては自信が無いが
		// Microsoftのドキュメントに書いてあったことによると
		// ID3D12DebugがDeviceを削除しますと書いてあった
		// それはつまり　アプリ終了時にID3D12DebugがDeviceを削除してからメモリリークをチェックする　ということだと思う
		// なぜなら　デバイスを削除しないとリークしているかどうかをチェック出来ないからである
		// よって　ID3D12Debugをリリースしない
		
		//2024/02/07
		//上記のようにdebuginterfaceをリリースしないことによりメモリリークはダンプされていなかった
		//しかしシステムのアップデートによりそのようにしていてもLiveObjectのダンプが出るようになった
		//debugController->Release();を呼ぶように修正
		//LiveObjectのダンプが出るが　少なくとも本クラスにおいてunique_ptrのメンバを使っておりその解放は本クラスのデストラクタよりも後になる
		//よって正確にLiveObjectのダンプを調べることが難しい状態
		debugController->Release();

		//2024/02/15
		//AdditiveIK.cppにおいてID3D12DebugDeviceを使用してリークの詳細ダンプをした
		//DirectXのCreate*関数で作成したオブジェクトにアロケート番号付きの名前をSetNameして調査
		//ダンプにおいてdebugdevice以外のRefcountが0になった(IntRefには何か書いてあるが)
		//メモリリークは解決したようだ

	}
#endif
	IDXGIFactory4* factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	return factory;
}

bool GraphicsEngine::CreateD3DDevice( IDXGIFactory4* dxgiFactory )
{
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,	//Direct3D 12.1の機能を使う。
		D3D_FEATURE_LEVEL_12_0	//Direct3D 12.0の機能を使う。
	};
	IDXGIAdapter* adapterTmp = nullptr;
	IDXGIAdapter* adapterVender[Num_GPUVender] = { nullptr };	//各ベンダーのアダプター。
	IDXGIAdapter* adapterMaxVideoMemory = nullptr;				//最大ビデオメモリのアダプタ。
	IDXGIAdapter* useAdapter = nullptr;							//最終的に使用するアダプタ。
	SIZE_T videoMemorySize = 0;
	for (int i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC desc;
		adapterTmp->GetDesc(&desc);

		if (desc.DedicatedVideoMemory > videoMemorySize) {
			//こちらのビデオメモリの方が多いので、こちらを使う。
			if (adapterMaxVideoMemory != nullptr) {
				adapterMaxVideoMemory->Release();
			}
			adapterMaxVideoMemory = adapterTmp;
			adapterMaxVideoMemory->AddRef();
			videoMemorySize = desc.DedicatedVideoMemory;
		}
		if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {
			//NVIDIA製
			if (adapterVender[GPU_VenderNvidia]) {
				adapterVender[GPU_VenderNvidia]->Release();
			}
			adapterVender[GPU_VenderNvidia] = adapterTmp;
			adapterVender[GPU_VenderNvidia]->AddRef();
		}
		else if (wcsstr(desc.Description, L"AMD") != nullptr) {
			//AMD製
			if (adapterVender[GPU_VenderAMD]) {
				adapterVender[GPU_VenderAMD]->Release();
			}
			adapterVender[GPU_VenderAMD] = adapterTmp;
			adapterVender[GPU_VenderAMD]->AddRef();
		}
		else if (wcsstr(desc.Description, L"Intel") != nullptr) {
			//Intel製
			if (adapterVender[GPU_VenderIntel]) {
				adapterVender[GPU_VenderIntel]->Release();
			}
			adapterVender[GPU_VenderIntel] = adapterTmp;
			adapterVender[GPU_VenderIntel]->AddRef();
		}
		adapterTmp->Release();
	}
	//使用するアダプターを決める。
	if (adapterVender[GPU_VenderNvidia] != nullptr) {
		//NVIDIA製が最優先
		useAdapter = adapterVender[GPU_VenderNvidia];
	}
	else if (adapterVender[GPU_VenderAMD] != nullptr) {
		//次はAMDが優先。
		useAdapter = adapterVender[GPU_VenderAMD];
	}
	else {
		//NVIDIAとAMDのGPUがなければビデオメモリが一番多いやつを使う。
		useAdapter = adapterMaxVideoMemory;
	}
	for (auto featureLevel : featureLevels) {
		auto hr = D3D12CreateDevice(
			useAdapter,
			featureLevel,
			IID_PPV_ARGS(&m_d3dDevice)
		);
		if (SUCCEEDED(hr)) {
			//D3Dデバイスの作成に成功した。
			break;
		}
	}
	for (auto& adapter : adapterVender) {
		if (adapter) {
			adapter->Release();
		}
	}
	if (adapterMaxVideoMemory) {
		adapterMaxVideoMemory->Release();
	}

	return m_d3dDevice != nullptr;
}
bool GraphicsEngine::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	auto hr = m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(hr)) {
		//コマンドキューの作成に失敗した。
		return false;
	}
	return true;
}
bool GraphicsEngine::CreateSwapChain(
	HWND hwnd,
	UINT frameBufferWidth,
	UINT frameBufferHeight,
	IDXGIFactory4* dxgiFactory
)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FRAME_BUFFER_COUNT;
	swapChainDesc.Width = frameBufferWidth;
	swapChainDesc.Height = frameBufferHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	//2024/01/03 アンチエイリアシング Device作成時にも指定必要　ハードウェア対応の必要有
	//ビデオカードのコントロールパネルからアプリのアンチエイリアス設定を変更可能なので　その方法をトラブルシューティングに書く
	//swapChainDesc.SampleDesc.Count = 4;


	//2023/12/15 PresentのflagsでもALLOW_TEARINGする必要有
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;


	IDXGISwapChain1* swapChain;
	dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue,
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);
	//IDXGISwapChain3のインターフェースを取得。
	swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain));
	swapChain->Release();
	//IDXGISwapChain3のインターフェースを取得。
	m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	return true;
}
bool GraphicsEngine::CreateDescriptorHeapForFrameBuffer()
{
	//RTV用のディスクリプタヒープを作成する。
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FRAME_BUFFER_COUNT;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto hr = m_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap));
	if (FAILED(hr)) {
		//RTV用のディスクリプタヒープの作成に失敗した。
		return false;
	}
	//ディスクリプタのサイズを取得。
	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//DSV用のディスクリプタヒープを作成する。
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hr = m_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsvHeap));
	if (FAILED(hr)) {
		//DSV用のディスクリプタヒープの作成に失敗した。
		return false;
	}
	//ディスクリプタのサイズを取得。
	m_dsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	return true;
}
bool GraphicsEngine::CreateRTVForFameBuffer()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	//フロントバッファをバックバッファ用のRTVを作成。
	for (UINT n = 0; n < FRAME_BUFFER_COUNT; n++) {
		m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
		m_d3dDevice->CreateRenderTargetView(
			m_renderTargets[n], nullptr, rtvHandle
		);
		rtvHandle.ptr += m_rtvDescriptorSize;
	}
	return true;
}
bool GraphicsEngine::CreateDSVForFrameBuffer( UINT frameBufferWidth, UINT frameBufferHeight )
{
	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		frameBufferWidth,
		frameBufferHeight,
		1,
		1,
		DXGI_FORMAT_D32_FLOAT,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HRESULT hrdepth0 = m_d3dDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(&m_depthStencilBuffer)
	);
	if (FAILED(hrdepth0) || !m_depthStencilBuffer) {
		::MessageBoxA(NULL, "may not have enough videomemory? App must exit.",
			"GraphicsEngine::CreateDSVForFrameBuffer Error", MB_OK | MB_ICONERROR);
		abort();

		//深度ステンシルバッファの作成に失敗。
		//return false;
	}

	m_depthStencilBuffer->SetName(L"GraphicsEngine:CreateDSVForFrameBuffer:depthbuf");

	//ディスクリプタを作成
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

	m_d3dDevice->CreateDepthStencilView(
		m_depthStencilBuffer, nullptr, dsvHandle
	);

	return true;
}
bool GraphicsEngine::CreateCommandList()
{
	//コマンドリストの作成。
	m_d3dDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList)
	);
	if (!m_commandList) {
		return false;
	}
	//コマンドリストは開かれている状態で作成されるので、いったん閉じる。
	m_commandList->Close();

	return true;
}
bool GraphicsEngine::CreateSynchronizationWithGPUObject()
{
	m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (!m_fence) {
		//フェンスの作成に失敗した。
		return false;
	}
	m_fenceValue = 1;
	//同期を行うときのイベントハンドラを作成する。
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		return false;
	}
	return true;
}
void GraphicsEngine::BeginRender()
{
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//カメラを更新する。
	m_camera2D.Update();
	m_camera3D.Update();
	m_cameraShadow.Update();

	//コマンドアロケータををリセット。
	m_commandAllocator->Reset();
	//レンダリングコンテキストもリセット。
	m_renderContext.Reset(m_commandAllocator, m_pipelineState);
	//ビューポートを設定。
	//ビューポートを設定。
	m_renderContext.SetViewportAndScissor(m_viewport);

	m_currentFrameBufferRTVHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	m_currentFrameBufferRTVHandle.ptr += m_frameIndex * m_rtvDescriptorSize;
	//深度ステンシルバッファのディスクリプタヒープの開始アドレスを取得。
	m_currentFrameBufferDSVHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	//バックバッファがレンダリングターゲットとして設定可能になるまで待つ。
	m_renderContext.WaitUntilToPossibleSetRenderTarget(m_renderTargets[m_frameIndex]);

	//レンダリングターゲットを設定。
	m_renderContext.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_renderContext.ClearRenderTargetView(m_currentFrameBufferRTVHandle, clearColor);
	m_renderContext.ClearDepthStencilView(m_currentFrameBufferDSVHandle, 1.0f);

}
void GraphicsEngine::ChangeRenderTargetToFrameBuffer(RenderContext& rc)
{
	rc.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);
}
void GraphicsEngine::EndRender(ChaScene* srcchascene)
{
	// レンダリングターゲットへの描き込み完了待ち
	m_renderContext.WaitUntilFinishDrawingToRenderTarget(m_renderTargets[m_frameIndex]);


	m_directXTKGfxMemroy->Commit(m_commandQueue);

	//レンダリングコンテキストを閉じる。
	m_renderContext.Close();

	//コマンドを実行。
	ID3D12CommandList* ppCommandLists[] = { m_commandList };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
//#if defined( SAMPE_16_04 ) || defined(APPEND_04)
//	// Present the frame.
//	m_swapChain->Present(0, 0);
//#else
//	// Present the frame.
//	m_swapChain->Present(1, 0);
//#endif



	//m_swapChain->Present(0, 0);
	//m_swapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);

	if (g_freefps) {
		//##########
		//Free fps
		//##########
		//2023/12/15 CreateSwapChainのFlagsでもALLOW_TEARINGする必要有
		m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
	else {
		//######
		//VSync
		//######
		m_swapChain->Present(1, 0);
	}

	m_directXTKGfxMemroy->GarbageCollect();

	if (srcchascene) {
		//UpdateMatrixスレッド終了待ち
		//描画の終了待ちの前で実行する(描画の間もUpdateMatrixを計算する)のが一番速いようだ.FootRigをオフにして呼び出し場所による時間比較をすると顕著に差が出る.
		srcchascene->WaitForUpdateMatrixModels();
	}

	//描画完了待ち。
	WaitDraw();


}

void GraphicsEngine::CopyTextureRegionGE(ID3D12Resource* pres, D3D12_TEXTURE_COPY_LOCATION* dst, D3D12_TEXTURE_COPY_LOCATION* src)
{
	if (!pres || !dst || !src) {
		_ASSERT(0);
		return;
	}

	m_commandList->CopyTextureRegion(dst, 0, 0, 0, src, nullptr);

	//D3D12_RESOURCE_BARRIER BarrierDesc = {};
	//BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//BarrierDesc.Transition.pResource = pres;
	//BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//m_commandList->ResourceBarrier(1, &BarrierDesc);
	//m_commandList->Close();

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		pres,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_commandList->ResourceBarrier(1, &barrier);


	ID3D12CommandList* ppCommandLists[] = { m_commandList };
	m_commandQueue->ExecuteCommandLists(1, ppCommandLists);


	const UINT64 fence = m_fenceValue;
	m_commandQueue->Signal(m_fence, fence);
	m_fenceValue++;
	if (m_fence->GetCompletedValue() < fence)
	{
		m_fence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

}

