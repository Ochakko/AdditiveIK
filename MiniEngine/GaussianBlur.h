﻿#pragma once

/// <summary>
/// ガウシアンブラークラス。
/// </summary>
class GaussianBlur {
public:
	GaussianBlur() : 
	m_xBlurRenderTarget(),
	m_yBlurRenderTarget(),
	m_xBlurSprite(),
	m_yBlurSprite()
	{
		m_originalTexture = nullptr;	//オリジナルテクスチャ。

		int windex;
		for (windex = 0; windex < NUM_WEIGHTS; windex++) {
			m_weights[windex] = 0.0f;
		}
	}
	~GaussianBlur() {
		DestroyObjs();
	}
	void DestroyObjs() {
		m_xBlurRenderTarget.DestroyObjs();
		m_yBlurRenderTarget.DestroyObjs();

		m_xBlurSprite.DestroyObjs();
		m_yBlurSprite.DestroyObjs();
	}

	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="originalTexture">ガウシアンブラーをかけるオリジナルテクスチャ。</param>
	void Init(Texture* originalTexture);
	/// <summary>
	/// ガウシアンブラーをGPU上で実行。
	/// </summary>
	/// <remarks>
	/// 本関数の呼び出しは、DirectX12を利用した描画コマンド生成中に呼び出す必要があります。
	/// </remarks>
	/// <param name="rc">レンダリングターゲット</param>
	/// <param name="blurPower">ブラーの強さ。値が大きいほどボケが強くなる。</param>
	void ExecuteOnGPU(RenderContext* rc, float blurPower, bool blurflag);
	/// <summary>
	/// ボケテクスチャを取得。
	/// </summary>
	/// <returns></returns>
	Texture* GetBokeTexture()
	{
		return m_yBlurRenderTarget.GetRenderTargetTexture();
	}
private:
	/// <summary>
	/// レンダリングターゲットを初期化。
	/// </summary>
	void InitRenderTargets();
	/// <summary>
	/// スプライトを初期化。
	/// </summary>
	void InitSprites();
	/// <summary>
	/// 重みテーブルを更新する。
	/// </summary>
	void UpdateWeightsTable(float blurPower, bool blurflag);
private:
	//enum { NUM_WEIGHTS = 8 };				//重みの数。
	enum { NUM_WEIGHTS = 4 };				//重みの数。 2024/04/11表示の重み的に8から4へ変更

	float m_weights[NUM_WEIGHTS];			//重みテーブル。
	Texture* m_originalTexture = nullptr;	//オリジナルテクスチャ。
	RenderTarget m_xBlurRenderTarget;		//横ボケ画像を描画するレンダリングターゲット。
	RenderTarget m_yBlurRenderTarget;		//縦ボケ画像を描画するレンダリングターゲット。
	Sprite m_xBlurSprite;					//横ボケ画像を描画するためのスプライト。
	Sprite m_yBlurSprite;					//縦ボケ画像を描画するためのスプライト。
};
