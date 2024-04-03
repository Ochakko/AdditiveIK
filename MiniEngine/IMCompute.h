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
	void SetComputeRootSignature(ID3D12RootSignature* rootSignature);
	void SetComputeRootSignature(RootSignature& rootSignature);
	/// <summary>
	/// パイプラインステートを設定。
	/// </summary>
	void SetPipelineState(ID3D12PipelineState* pipelineState);
	void SetPipelineState(PipelineState& pipelineState);
	/// <summary>
	/// ディスクリプタヒープを設定。
	/// </summary>
	void SetComputeDescriptorHeap(DescriptorHeap& descHeap);
	/// <summary>
	/// コマンドリストを閉じる
	/// </summary>
	void Close();
	/// <summary>
	/// コマンドリストをリセット。
	/// </summary>
	/// <param name="commandAllocator"></param>
	/// <param name="pipelineState"></param>
	void Reset(ID3D12PipelineState* pipelineState);


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
		UINT ThreadGroupCountZ);
	

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
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);

private:
	enum { MAX_DESCRIPTOR_HEAP = 8 };	//ディスクリプタヒープの最大数。
	enum { MAX_CONSTANT_BUFFER = 8 };	//定数バッファの最大数。足りなくなったら増やしてね。
	enum { MAX_SHADER_RESOURCE = 8 };	//シェーダーリソースの最大数。足りなくなったら増やしてね。


	ID3D12Device* m_dev;

	ID3D12DescriptorHeap* m_descriptorHeaps[MAX_DESCRIPTOR_HEAP];			//ディスクリプタヒープの配列。
	ConstantBuffer* m_constantBuffers[MAX_CONSTANT_BUFFER] = { nullptr };	//定数バッファの配列。
	Texture* m_shaderResources[MAX_SHADER_RESOURCE] = { nullptr };			//シェーダーリソースの配列。
};

