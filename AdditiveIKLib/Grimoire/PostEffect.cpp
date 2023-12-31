﻿#include "stdafx.h"
#include "PostEffect.h"
namespace myRenderer {
    void PostEffect::Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget)
    {
        m_bloom.Init(mainRenderTarget);
        m_dof.Init(mainRenderTarget, zprepassRenderTarget);
    }
    void PostEffect::Render(RenderContext* rc, RenderTarget& mainRenderTarget)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        m_bloom.Render(rc, mainRenderTarget);
        m_dof.Render(rc, mainRenderTarget);
    }
}