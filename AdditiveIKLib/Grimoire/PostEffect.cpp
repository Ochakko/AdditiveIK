#include "stdafx.h"
#include "PostEffect.h"
namespace myRenderer {
    void PostEffect::Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget)
    {
        DestroyObjs();

        m_bloom.Init(mainRenderTarget);
        m_dof.Init(mainRenderTarget, zprepassRenderTarget);

        m_initflag = true;
    }
    void PostEffect::Render(RenderContext* rc, RenderTarget& mainRenderTarget)
    {
        if (!rc || !m_initflag) {
            _ASSERT(0);
            return;
        }

        m_bloom.Render(rc, mainRenderTarget);
        m_dof.Render(rc, mainRenderTarget);
    }
}