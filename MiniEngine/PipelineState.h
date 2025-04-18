﻿#pragma once


/// <summary>
/// パイプラインステート。
/// </summary>
class PipelineState {
public:
	PipelineState() {
		InitParams();
	};

	void InitParams() {
		m_pipelineState = nullptr;
	};

	/// <summary>
	/// デストラクタ。
	/// </summary>
	~PipelineState()
	{
		DestroyObjs();
	}

	void DestroyObjs()
	{
		if (m_pipelineState) {
			m_pipelineState->Release();
			m_pipelineState = nullptr;
		}
	}

	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="desc"></param>
	void Init(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc);
	void Init(D3D12_COMPUTE_PIPELINE_STATE_DESC desc);
	ID3D12PipelineState* Get()
	{
		return m_pipelineState;
	}
private:
	ID3D12PipelineState* m_pipelineState = nullptr;	//パイプラインステート。
};
