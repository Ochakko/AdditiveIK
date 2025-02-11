﻿#pragma once

#include "../../MiniEngine/GaussianBlur.h"

namespace myRenderer {
    class Bloom
    {
    public:
        Bloom();
        ~Bloom();
        void Init(RenderTarget& mainRenderTarget);
        void Render(RenderContext* rc, RenderTarget& mainRenderTarget);
        void DestroyObjs();
    private:
        RenderTarget m_luminanceRenderTarget;	//輝度抽出用のレンダリングターゲット
        Sprite m_luminanceSprite;				//輝度抽出用のスプライト
        GaussianBlur m_gaussianBlur[4];			//ガウシアンブラー
        Sprite m_finalSprite;					//最終合成用のスプライト
    };
}

