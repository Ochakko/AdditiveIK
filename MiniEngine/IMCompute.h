#pragma once

class ConstantBuffer;
class Texture;
//class DescriptorHeap;
class RenderTarget;

#include "DescriptorHeap.h"


//#include <d3dx12.h>
//#include "../../DirectXTex/DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTex/d3dx12.h"


/// <summary>
/// コンピュートシェーダ即時実行用クラス
/// </summary>
class IMCompute {
public:
	IMCompute();
	~IMCompute();

	int Init(ID3D12Device* pdev);

	static int InitStaticMem(ID3D12Device* pdev);//for s_commandQueue
	static void DestroyStaticMem();//for s_commandQueue



	///// <summary>
	///// ルートシグネチャを設定。
	///// </summary>
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
	/// ディスクリプタヒープを設定。
	/// </summary>
	void SetComputeDescriptorHeap(DescriptorHeap& descHeap);
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
	void Reset(ID3D12PipelineState* pipelineState)
	{
		if (m_allocator && m_commandList) {
			m_allocator->Reset();
			m_commandList->Reset(m_allocator, pipelineState);
		}
		else {
			_ASSERT(0);
		}
	}



	void IMExecute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);




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
	

private:
	HRESULT CreateCommandList(ID3D12Device* pdev);
	void WaitDraw();


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


	ID3D12Device* m_dev;
	ID3D12CommandAllocator* m_allocator;
	ID3D12GraphicsCommandList4* m_commandList;	//コマンドリスト。
	//ID3D12CommandQueue* m_commandQueue;			//コマンドキュー。
	HANDLE m_fenceEvent = nullptr;
	ID3D12Fence* m_fence = nullptr;
	UINT64 m_fenceValue = 0;



	ID3D12DescriptorHeap* m_descriptorHeaps[MAX_DESCRIPTOR_HEAP];			//ディスクリプタヒープの配列。
	ConstantBuffer* m_constantBuffers[MAX_CONSTANT_BUFFER] = { nullptr };	//定数バッファの配列。
	Texture* m_shaderResources[MAX_SHADER_RESOURCE] = { nullptr };			//シェーダーリソースの配列。
};

