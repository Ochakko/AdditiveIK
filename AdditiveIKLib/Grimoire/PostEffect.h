#pragma once

#include "Bloom.h"
#include "Dof.h"

namespace myRenderer {
    class PostEffect
    {
    public:
        PostEffect() : m_bloom(), m_dof() {
            m_initflag = false;
        };
        ~PostEffect() {
            DestroyObjs();
        };

        void Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget);
        void Render( RenderContext* rc, RenderTarget& mainRenderTarget );
        void DestroyObjs() {
            m_bloom.DestroyObjs();
            m_dof.DestroyObjs();
            m_initflag = false;
        };
    private:
        bool m_initflag;
        Bloom m_bloom;	//ブルーム
        Dof m_dof;		//被写界深度
    };
}

