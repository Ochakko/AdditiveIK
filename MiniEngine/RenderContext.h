﻿#pragma once

class ConstantBuffer;
class Texture;
//class DescriptorHeap;
class RenderTarget;

#include "DescriptorHeap.h"


//#include <d3dx12.h>
//#include "../../DirectXTex/DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTex/d3dx12.h"

//namespace raytracing{
//	class PSO;
//}
/// <summary>
/// レンダリングコンテキスト。
/// </summary>
class RenderContext {
public:
	RenderContext() 
	{
		ZeroMemory(&m_currentViewport, sizeof(D3D12_VIEWPORT));				//現在のビューポート。
		m_commandList = nullptr;	//コマンドリスト。
		ZeroMemory(m_descriptorHeaps, sizeof(ID3D12DescriptorHeap*) * MAX_DESCRIPTOR_HEAP);
		ZeroMemory(m_constantBuffers, sizeof(ConstantBuffer*) * MAX_CONSTANT_BUFFER);
		ZeroMemory(m_shaderResources, sizeof(Texture*) * MAX_SHADER_RESOURCE);
		m_scratchResourceList.clear();
	};

	~RenderContext(){};//このクラス内のオブジェクトは作成していない　外部ポインタを保持しているだけなので破棄しない

	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="commandList">コマンドリスト。</param>
	void Init(ID3D12GraphicsCommandList4* commandList)
	{
		m_commandList = commandList;
	}

	/// <summary>
	/// 頂点バッファを設定。
	/// </summary>
	/// <param name="vb">頂点バッファ。</param>
	void SetVertexBuffer(VertexBuffer& vb)
	{
		m_commandList->IASetVertexBuffers(0, 1, &vb.GetView());
	}
	void SetVertexBuffer(int srcslotno, VertexBuffer& vb)
	{
		m_commandList->IASetVertexBuffers(srcslotno, 1, &vb.GetView());
	}
	void SetVertexBuffer(D3D12_VERTEX_BUFFER_VIEW& vbview)//CDispObjからの呼び出しに使用
	{
		m_commandList->IASetVertexBuffers(0, 1, &vbview);
	}
	/// <summary>
	/// インデックスバッファを設定。
	/// </summary>
	/// <param name="ib"></param>
	void SetIndexBuffer(IndexBuffer& ib)
	{
		m_commandList->IASetIndexBuffer(&ib.GetView());
	}
	void SetIndexBuffer(D3D12_INDEX_BUFFER_VIEW& ibview)//CDispObjからの呼び出しに使用
	{
		m_commandList->IASetIndexBuffer(&ibview);
	}
	/// <summary>
	/// プリミティブのトポロジーを設定。
	/// </summary>
	/// <remarks>
	/// ID3D12GraphicsCommandList::のIASetPrimitiveTopologyのラッパー関数。
	/// 詳細はMicrosoftのヘルプを参照。
	/// </remarks>
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		m_commandList->IASetPrimitiveTopology(topology);
	}
	/// <summary>
	/// コマンドリストを設定。
	/// </summary>
	/// <param name="commandList">コマンドリスト。</param>
	void SetCommandList(ID3D12GraphicsCommandList4* commandList)
	{
		m_commandList = commandList;
	}
	/// <summary>
	/// ビューポートとシザリング矩形をセットで設定
	/// </summary>
	/// <param name="viewport">ビューポート</param>
	void SetViewportAndScissor(D3D12_VIEWPORT& viewport)
	{
		//シザリング矩形も設定する。
		D3D12_RECT scissorRect;
		scissorRect.bottom = static_cast<LONG>(viewport.Height);
		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = static_cast<LONG>(viewport.Width);
		SetScissorRect(scissorRect);

		m_commandList->RSSetViewports(1, &viewport);
		m_currentViewport = viewport;
	}
	void SetViewportAndScissor2(D3D12_VIEWPORT& viewport)
	{
		//###########################################
		//2024/04/09
		//MultiRenderTarget用
		//同じビューポートとScissorRectを２つセットする
		//###########################################

		//シザリング矩形も設定する。
		D3D12_RECT scissorRect;
		scissorRect.bottom = static_cast<LONG>(viewport.Height);
		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = static_cast<LONG>(viewport.Width);
		SetScissorRect2(scissorRect);

		D3D12_VIEWPORT setvps[2] = { viewport, viewport };
		m_commandList->RSSetViewports(2, setvps);//2, 
		m_currentViewport = viewport;
	}

	/// <summary>
	/// ビューポートを取得。
	/// </summary>
	/// <returns></returns>
	D3D12_VIEWPORT GetViewport() const
	{
		return m_currentViewport;
	}
	/// <summary>
	/// シザリング矩形を設定
	/// </summary>
	/// <param name="rect"></param>
	void SetScissorRect(D3D12_RECT& rect)
	{
		m_commandList->RSSetScissorRects(1, &rect);
	}
	void SetScissorRect2(D3D12_RECT& rect)
	{
		//##############################
		//2024/04/09
		//マルチレンダーターゲット用
		//同じScissorRectを２つセットする
		//##############################
		D3D12_RECT setsrs[2] = { rect, rect };
		m_commandList->RSSetScissorRects(2, setsrs);
	}

	/// <summary>
	/// ルートシグネチャを設定。
	/// </summary>
	void SetRootSignature(ID3D12RootSignature* rootSignature)
	{
		m_commandList->SetGraphicsRootSignature(rootSignature);
	}
	void SetRootSignature(RootSignature& rootSignature)
	{
		m_commandList->SetGraphicsRootSignature(rootSignature.Get());
	}
	void SetComputeRootSignature(ID3D12RootSignature* rootSignature)
	{
		m_commandList->SetComputeRootSignature(rootSignature);
	}
	void SetComputeRootSignature(RootSignature& rootSignature)
	{
		m_commandList->SetComputeRootSignature(rootSignature.Get());
	}
	/// <summary>
	/// パイプラインステートを設定。
	/// </summary>
	void SetPipelineState(ID3D12PipelineState* pipelineState)
	{
		m_commandList->SetPipelineState(pipelineState);
	}
	void SetPipelineState(PipelineState& pipelineState)
	{
		m_commandList->SetPipelineState(pipelineState.Get());
	}
	/// <summary>
	/// レイトレ用のパイプラインステートオブジェクトを設定。
	/// </summary>
	/// <param name="pso"></param>
	//void SetPipelineState(raytracing::PSO& pso);

	/// <summary>
	/// ディスクリプタヒープを設定。
	/// </summary>
	void SetDescriptorHeap(ID3D12DescriptorHeap* descHeap)
	{
		m_descriptorHeaps[0] = descHeap;
		m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);
	}
	
	void SetDescriptorHeap(DescriptorHeap& descHeap);
	void SetComputeDescriptorHeap(DescriptorHeap& descHeap);
	/// <summary>
	/// 複数のディスクリプタヒープを登録。
	/// </summary>
	/// <param name="numDescriptorHeap">ディスクリプタヒープの数。</param>
	/// <param name="descHeaps">ディスクリプタヒープの配列</param>
	void SetDescriptorHeaps(int numDescriptorHeap, const DescriptorHeap* descHeaps[])
	{
		for (int i = 0; i < numDescriptorHeap; i++) {
			m_descriptorHeaps[i] = descHeaps[i]->Get();
		}
		m_commandList->SetDescriptorHeaps(numDescriptorHeap, m_descriptorHeaps);
	}
	/// <summary>
	/// 定数バッファを設定。
	/// </summary>
	/// <param name="registerNo">設定するレジスタの番号。</param>
	/// <param name="cb">定数バッファ。</param>
	void SetConstantBuffer(int registerNo, ConstantBuffer& cb)
	{
		if (registerNo < MAX_CONSTANT_BUFFER) {
			m_constantBuffers[registerNo] = &cb;
		}
		else {
			//範囲外アクセス。
			std::abort();
		}
	}
	/// <summary>
	/// シェーダーリソースを設定。
	/// </summary>
	/// <param name="registerNo">設定するレジスタの番号。</param>
	/// <param name="texture">テクスチャ。</param>
	void SetShaderResource(int registerNo, Texture& texture)
	{
		if (registerNo < MAX_SHADER_RESOURCE) {
			m_shaderResources[registerNo] = &texture;
		}
		else {
			//範囲外アクセス。
			std::abort();
		}
	}
	/// <summary>
	/// 複数枚のレンダリングターゲットを設定する。
	/// </summary>
	/// <remarks>
	/// MRTを利用したレンダリングを行いたい場合に利用してください。
	/// </remarks>
	/// <param name="numRT">レンダリングターゲットの数</param>
	/// <param name="renderTarget">レンダリングターゲットの配列。</param>
	void SetRenderTargets(UINT numRT, RenderTarget* renderTargets[]);
	/// <summary>
	/// レンダリングターゲットを設定する。
	/// </summary>
	/// <param name="renderTarget"></param>
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
	{
		m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	}
	
	/// <summary>
	/// レンダリングターゲットをスロット0に設定する。
	/// </summary>
	/// <remarks>
	/// 本関数はビューポートの設定を行いません。
	/// ユーザー側で適切なビューポートを指定する必要があります。
	/// </remarks>
	/// <param name="renderTarget">レンダリングターゲット</param>
	void SetRenderTarget(RenderTarget& renderTarget) 
	{
		RenderTarget* rtArray[] = { &renderTarget };
		SetRenderTargets(1, rtArray);
	}

	/// <summary>
	/// レンダリングターゲットとビューポートを同時に設定する。
	/// </summary>
	/// <remarks>
	/// この関数を利用するとレンダリングターゲットと同じ幅と高さのビューポートが設定されます。
	/// </remarks>
	/// <param name="renderTarget">レンダリングターゲット</param>
	void SetRenderTargetAndViewport(RenderTarget& renderTarget);
	/// <summary>
	/// 複数枚のレンダリングターゲットとビューポートを同時に設定する。
	/// </summary>
	/// /// <remarks>
	/// この関数を利用するとレンダリングターゲットと同じ幅と高さのビューポートが設定されます。
	/// </remarks>
	/// <param name="numRT">設定するレンダリングターゲットの枚数</param>
	/// <param name="renderTargets">レンダリングターゲットの配列。</param>
	void SetRenderTargetsAndViewport(UINT numRT, RenderTarget* renderTargets[]);
	
	/// <summary>
	/// 複数枚のレンダリングターゲットをクリア。
	/// </summary>
	/// <remarks>
	/// クリアカラーはレンダリングターゲットの初期化時に指定したカラーです。
	/// </remarks>
	/// <param name="numRt">レンダリングターゲットの数</param>
	/// <param name="renderTargets">レンダリングターゲットの数</param>
	void ClearRenderTargetViews(
		int numRt, 
		RenderTarget* renderTargets[]
	);
	/// <summary>
	/// レンダリングターゲットのクリア。
	/// </summary>
	/// <param name="rtvHandle">CPUのレンダリングターゲットビューのディスクリプタハンドル</param>
	/// <param name="clearColor">クリアカラー</param>
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor)
	{
		m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}
	/// <summary>
	/// レンダリングターゲットのクリア。
	/// </summary>
	/// <param name="renderTarget"></param>
	void ClearRenderTargetView(RenderTarget& renderTarget)
	{
		RenderTarget* rtArray[] = { &renderTarget };
		ClearRenderTargetViews(1, rtArray);
	}
	/// <summary>
	/// デプスステンシルビューをクリア
	/// </summary>
	/// <param name="renderTarget">レンダリングターゲット</param>
	/// <param name="clearValue">クリア値</param>
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue)
	{
		m_commandList->ClearDepthStencilView(
			dsvHandle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			clearValue,
			0,
			0,
			nullptr);
	}
	/// <summary>
	/// レンダリングターゲットへの描き込み待ち。
	/// </summary>
	/// <remarks>
	/// レンダリングターゲットとして使われているテクスチャをシェーダーリソースビューとして
	/// 使用したい場合は、この関数を使って描き込み完了待ちを行う必要があります。
	/// </remarks>
	/// <param name="renderTarget">レンダリングターゲット</param>
	void WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[]);
	void WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget);
	void WaitUntilFinishDrawingToRenderTarget( ID3D12Resource* renderTarget )
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1,&barrier);
	}
	/// <summary>
	/// レンダリングターゲットとして使用可能になるまで待つ。
	/// </summary>
	/// <remarks>
	/// レンダリングターゲットとして設定したい場合は、
	/// 本関数を使って使用可能になるまで待機する必要があります。
	/// </remarks>
	void WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[]);
	void WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget);
	void WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		//D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE  2023/11/24 How to use???? Present(0, 0)でも可変周波数にならない　後で調査
		//auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE);
		//auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1,&barrier);
	}
	/// <summary>
	/// リソースバリア。
	/// </summary>
	/// <param name="barrier"></param>
	void ResourceBarrier(D3D12_RESOURCE_BARRIER& barrier)
	{
		m_commandList->ResourceBarrier(1, &barrier);
	}
	/// <summary>
	/// リソースステートを遷移する。
	/// </summary>
	/// <param name="resrouce"></param>
	/// <param name="beforeState"></param>
	/// <param name="afterState"></param>
	void TransitionResourceState(ID3D12Resource* resrouce , D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resrouce, beforeState, afterState);
		m_commandList->ResourceBarrier(1,&barrier);
	}
	/// <summary>
	/// コマンドリストを閉じる
	/// </summary>
	void Close()
	{
		m_commandList->Close();
	}
	/// <summary>
	/// コマンドリストをリセット。
	/// </summary>
	/// <param name="commandAllocator"></param>
	/// <param name="pipelineState"></param>
	void Reset( ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState)
	{
		m_commandList->Reset(commandAllocator, pipelineState);
		//スクラッチリソースをクリア。
		m_scratchResourceList.clear();
	}
	/// <summary>
	/// インデックスつきプリミティブを描画。
	/// </summary>
	/// <param name="indexCount">インデックスの数。</param>
	void DrawIndexed(UINT indexCount)
	{
		m_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}
	void DrawIndexed(UINT indexCount, UINT startindex)//CDispObjからの呼び出しに使用
	{
		m_commandList->DrawIndexedInstanced(indexCount, 1, startindex, 0, 0);
		//m_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, startindex);
	}
	/// <summary>
	/// インスタンシング描画
	/// </summary>
	/// <param name="indexCount">インデックス数</param>
	/// <param name="numInstance">インスタンス数</param>
	void DrawIndexedInstanced(UINT indexCount, UINT numInstance)
	{
		m_commandList->DrawIndexedInstanced(indexCount, numInstance, 0, 0, 0);
	}
	/// <summary>
	/// コンピュートシェーダーをディスパッチ。
	/// </summary>
	/// <param name="ThreadGroupCountX"></param>
	/// <param name="ThreadGroupCountY"></param>
	/// <param name="ThreadGroupCountZ"></param>
	void Dispatch( 
		UINT ThreadGroupCountX,
		UINT ThreadGroupCountY,
		UINT ThreadGroupCountZ )
	{
		m_commandList->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}
	/// <summary>
	/// GPUでレイトレーシングアクセラレーション構造のビルドを行います。
	/// </summary>
	//void BuildRaytracingAccelerationStructure(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc)
	//{
	//	m_commandList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
	//}
	/// <summary>
	/// レイをディスパッチ
	/// </summary>
	/// <param name="rayDesc"></param>
	//void DispatchRays(D3D12_DISPATCH_RAYS_DESC& rayDesc)
	//{
	//	m_commandList->DispatchRays(&rayDesc);
	//}
	/// <summary>
	/// リソースをコピー。
	/// </summary>
	/// <param name="pDst">コピー先のリソース</param>
	/// <param name="pSrc">コピー元のリソース</param>
	void CopyResource(ID3D12Resource* pDst, ID3D12Resource* pSrc)
	{
		m_commandList->CopyResource(pDst, pSrc);
	
	}
	
	UINT64 UpdateSubresourcesRC(
		//_In_ ID3D12GraphicsCommandList* pCmdList,
		ID3D12Resource* pDestinationResource,
		ID3D12Resource* pIntermediate,
		UINT64 IntermediateOffset,
		UINT FirstSubresource,
		UINT NumSubresources,
		D3D12_SUBRESOURCE_DATA* pSrcData)
	{
			return UpdateSubresources(
				m_commandList,
				pDestinationResource, 
				pIntermediate, IntermediateOffset,
				FirstSubresource, NumSubresources,
				pSrcData);
	
		//texbuff.Get(), nullptr,
		//	(UINT64)mipChain->GetImages(),
		//	mipChain->GetImageCount(),
		//	mipChain->GetMetadata().width,//  .bytesPerRow, 
		//	mipChain->GetMetadata().width* mipChain->GetMetadata().height,  //.totalBytes, 
		//	nullptr
	
	}


	//void CopyTextureRegionRC(D3D12_TEXTURE_COPY_LOCATION* dst, D3D12_TEXTURE_COPY_LOCATION* src)
	//{
	//	m_commandList->CopyTextureRegion(dst, 0, 0, 0, src, nullptr);
	//	
	//}


private:

	/// <summary>
	/// ディスクリプタテーブルを設定。
	/// </summary>
	/// <param name="RootParameterIndex"></param>
	/// <param name="BaseDescriptor"></param>
	void SetGraphicsRootDescriptorTable(
		UINT RootParameterIndex,
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
	{
		m_commandList->SetGraphicsRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}
	/// <summary>
	/// ディスクリプタテーブルを設定。
	/// </summary>
	/// <param name="RootParameterIndex"></param>
	/// <param name="BaseDescriptor"></param>
	void SetComputeRootDescriptorTable(
		UINT RootParameterIndex,
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
	{
		m_commandList->SetComputeRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}




private:
	//enum { MAX_DESCRIPTOR_HEAP = 4 };	//ディスクリプタヒープの最大数。
	//enum { MAX_CONSTANT_BUFFER = 8 };	//定数バッファの最大数。足りなくなったら増やしてね。
	//enum { MAX_SHADER_RESOURCE = 16 };	//シェーダーリソースの最大数。足りなくなったら増やしてね。
	//enum { MAX_DESCRIPTOR_HEAP = (4096 * 8) };	//ディスクリプタヒープの最大数。
	//enum { MAX_CONSTANT_BUFFER = (4096 * 16) };	//定数バッファの最大数。足りなくなったら増やしてね。
	//enum { MAX_SHADER_RESOURCE = (4096 * 32) };	//シェーダーリソースの最大数。足りなくなったら増やしてね。
	enum { MAX_DESCRIPTOR_HEAP = 1024 };	//ディスクリプタヒープの最大数。
	enum { MAX_CONSTANT_BUFFER = 1024 };	//定数バッファの最大数。足りなくなったら増やしてね。
	enum { MAX_SHADER_RESOURCE = 1024 };	//シェーダーリソースの最大数。足りなくなったら増やしてね。


	D3D12_VIEWPORT m_currentViewport;				//現在のビューポート。
	ID3D12GraphicsCommandList4* m_commandList;	//コマンドリスト。
	ID3D12DescriptorHeap* m_descriptorHeaps[MAX_DESCRIPTOR_HEAP];			//ディスクリプタヒープの配列。
	ConstantBuffer* m_constantBuffers[MAX_CONSTANT_BUFFER] = { nullptr };	//定数バッファの配列。
	Texture* m_shaderResources[MAX_SHADER_RESOURCE] = { nullptr };			//シェーダーリソースの配列。
	std::vector< ComPtr<ID3D12Resource> > m_scratchResourceList;				//スクラッチリソースのリスト。
};

