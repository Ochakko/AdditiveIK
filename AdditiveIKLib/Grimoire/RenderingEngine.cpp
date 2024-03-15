#include "stdafx.h"
#include "RenderingEngine.h"

#include <mqoobject.h>
#include <DispObj.h>
#include <GlobalVar.h>

#include "../../MiniEngine/InstancedSprite.h"
#include "../../AdditiveIK/FpsSprite.h"
#include "../../AdditiveIK/UndoSprite.h"


extern bool g_4kresolution;//AdditiveIk.cpp
extern IShaderResource* g_shadowmapforshader;


namespace myRenderer
{


    RenderingEngine::RenderingEngine() : 
    m_shadowMapRenderTarget(), 
    //m_shadowBlur(),
    m_deferredLightingCB(),
    m_copyMainRtToFrameBufferSprite(),
    m_diferredLightingSprite(),
    m_zprepassRenderTarget(),
    m_mainRenderTarget(),
    //m_mainRTSnapshots(),
    //m_gBuffer(),
    m_postEffect()
    {
        m_initflag = false;
        //m_zprepassModels.clear();                         // ZPrepassの描画パスで描画されるモデルのリスト
        //m_shadowmapModels.clear();
        //m_renderToGBufferModels.clear();                  // Gバッファへの描画パスで描画するモデルのリスト
        m_forwardRenderModels.clear();                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
        m_instancingRenderModels.clear();                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
        m_forwardRenderSprites.clear();
        m_forwardRenderFont.clear();
        m_forwardModelsReserveSize = 0;
    }

    RenderingEngine::~RenderingEngine()
    {
        DestroyObjs();
    }
    void RenderingEngine::Init()
    {
        if (m_initflag) {
            _ASSERT(0);
            return;
        }


        //m_zprepassModels.clear();                         // ZPrepassの描画パスで描画されるモデルのリスト
        //m_shadowmapModels.clear();
        m_renderToGBufferModels.clear();                  // Gバッファへの描画パスで描画するモデルのリスト
        m_forwardRenderModels.clear();                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
        m_instancingRenderModels.clear();                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
        m_forwardRenderSprites.clear();
        m_forwardRenderFont.clear();
        m_forwardModelsReserveSize = 0;


        InitZPrepassRenderTarget();
        InitMainRenderTarget();
        //InitGBuffer();
        //InitMainRTSnapshotRenderTarget();
        InitCopyMainRenderTargetToFrameBufferSprite();
        InitShadowMapRender();
        InitDeferredLighting();
        m_postEffect.Init(m_mainRenderTarget, m_zprepassRenderTarget);

        m_initflag = true;
    }


    void RenderingEngine::DestroyObjs()
    {
        m_shadowMapRenderTarget.DestroyObjs();
        m_zprepassRenderTarget.DestroyObjs();
        m_mainRenderTarget.DestroyObjs();

        m_copyMainRtToFrameBufferSprite.DestroyObjs();
        m_diferredLightingSprite.DestroyObjs();

        m_postEffect.DestroyObjs();

        //int rtindex;
        //for (rtindex = 0; rtindex < (int)EnMainRTSnapshot::enNum; rtindex++) {
        //    m_mainRTSnapshots[rtindex].DestroyObjs();
        //}
        //int gbufindex;
        //for (gbufindex = 0; gbufindex < enGBufferNum; gbufindex++) {
        //    m_gBuffer[gbufindex].DestroyObjs();
        //}
    }




    void RenderingEngine::InitShadowMapRender()
    {
        // シャドウマップの描画処理の初期化
        //for (auto& shadowMapRender : m_shadowMapRenders)
        //{
        //    shadowMapRender.Init();
        //}


    // step-1 シャドウマップ描画用のレンダリングターゲットを作成する
        float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_shadowMapRenderTarget.Create(
            SHADOWMAP_SIZE,//このサイズを変える場合には　g_cameraShadowのWとHも変えること
            SHADOWMAP_SIZE,//このサイズを変える場合には　g_cameraShadowのWとHも変えること
            //1024,//このサイズを変える場合には　g_cameraShadowのWとHも変えること
            //1024,//このサイズを変える場合には　g_cameraShadowのWとHも変えること
            //g_graphicsEngine->GetFrameBufferWidth(),
            //g_graphicsEngine->GetFrameBufferHeight(),
            1,
            1,
            // 【注目】シャドウマップのカラーバッファーのフォーマットを変更している
            //DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            DXGI_FORMAT_D32_FLOAT,
            clearColor
        );

        g_shadowmapforshader = m_shadowMapRenderTarget.GetRenderTargetTexture();


        //m_shadowBlur.Init(
        //    &m_shadowMapRenderTarget.GetRenderTargetTexture() // ぼかすテクスチャはシャドウマップのテクスチャ
        //);
        //g_shadowmapforshader = &m_shadowBlur.GetBokeTexture();

    }

    void RenderingEngine::InitZPrepassRenderTarget()
    {
        //float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_zprepassRenderTarget.Create(
            g_graphicsEngine->GetFrameBufferWidth(),
            g_graphicsEngine->GetFrameBufferHeight(),
            1,
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            //DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_D32_FLOAT,
            clearColor
        );

    }
    //void RenderingEngine::InitMainRTSnapshotRenderTarget()
    //{
    //    for (auto& snapshotRt : m_mainRTSnapshots)
    //    {
    //        snapshotRt.Create(
    //            g_graphicsEngine->GetFrameBufferWidth(),
    //            g_graphicsEngine->GetFrameBufferHeight(),
    //            1,
    //            1,
    //            DXGI_FORMAT_R8G8B8A8_UNORM,
    //            DXGI_FORMAT_UNKNOWN
    //        );
    //    }
    //}

    void RenderingEngine::InitMainRenderTarget()
    {
        m_mainRenderTarget.Create(
            g_graphicsEngine->GetFrameBufferWidth(),
            g_graphicsEngine->GetFrameBufferHeight(),
            1,
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            //DXGI_FORMAT_R8G8B8A8_UNORM,//2023/11/18
            DXGI_FORMAT_UNKNOWN
        );
    }

    //void RenderingEngine::InitGBuffer()
    //{
    //    int frameBuffer_w = g_graphicsEngine->GetFrameBufferWidth();
    //    int frameBuffer_h = g_graphicsEngine->GetFrameBufferHeight();

    //    // アルベドカラーを出力用のレンダリングターゲットを初期化する
    //    m_gBuffer[enGBufferAlbedo].Create(
    //        frameBuffer_w,
    //        frameBuffer_h,
    //        1,
    //        1,
    //        DXGI_FORMAT_R32G32B32A32_FLOAT,
    //        //DXGI_FORMAT_R8G8B8A8_UNORM,//2023/11/18
    //        DXGI_FORMAT_D32_FLOAT
    //    );

    //    // 法線出力用のレンダリングターゲットを初期化する
    //    m_gBuffer[enGBufferNormal].Create(
    //        frameBuffer_w,
    //        frameBuffer_h,
    //        1,
    //        1,
    //        DXGI_FORMAT_R8G8B8A8_UNORM, //メモリ使用量とメモリ書き込み速度優先で、8bitの符号なし整数バッファを使用する。
    //        DXGI_FORMAT_UNKNOWN
    //    );

    //    // 金属度と滑らかさマップ出力用のレンダリングターゲットを初期化する
    //    m_gBuffer[enGBufferMetalSmooth].Create(
    //        frameBuffer_w,
    //        frameBuffer_h,
    //        1,
    //        1,
    //        DXGI_FORMAT_R8G8B8A8_UNORM, //メモリ使用量メモリ書き込み速度優先で、8bitの符号なし整数バッファを使用する。。
    //        DXGI_FORMAT_UNKNOWN
    //    );

    //    // ワールド座標出力用のレンダリングターゲットを初期化する
    //    m_gBuffer[enGBufferWorldPos].Create(
    //        frameBuffer_w,
    //        frameBuffer_h,
    //        1,
    //        1,
    //        DXGI_FORMAT_R32G32B32A32_FLOAT,
    //        //DXGI_FORMAT_R8G8B8A8_UNORM,//2023/11/18
    //        DXGI_FORMAT_UNKNOWN
    //    );
    //    m_gBuffer[enGBUfferShadowParam].Create(
    //        frameBuffer_w,
    //        frameBuffer_h,
    //        1,
    //        1,
    //        DXGI_FORMAT_R8G8B8A8_UNORM,
    //        DXGI_FORMAT_UNKNOWN
    //    );
    //}

    void RenderingEngine::InitCopyMainRenderTargetToFrameBufferSprite()
    {
        SpriteInitData spriteInitData;
        // テクスチャはyBlurRenderTargetのカラーバッファー
        spriteInitData.m_textures[0] = m_mainRenderTarget.GetRenderTargetTexture();

        // レンダリング先がフレームバッファーなので、解像度はフレームバッファーと同じ
        spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
        spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

        // ボケ画像をそのまま貼り付けるだけなので、通常の2D描画のシェーダーを指定する
        spriteInitData.m_fxFilePath = "../Media/shader/preset/sprite.fx";
        spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        // 初期化オブジェクトを使って、スプライトを初期化する
        m_copyMainRtToFrameBufferSprite.Init(spriteInitData);
    }

    void RenderingEngine::InitDeferredLighting()
    {
        //int frameBuffer_w = g_graphicsEngine->GetFrameBufferWidth();
        //int frameBuffer_h = g_graphicsEngine->GetFrameBufferHeight();

        //// 太陽光
        //m_deferredLightingCB.m_light.directionalLight[0].color.x = 4.8f;
        //m_deferredLightingCB.m_light.directionalLight[0].color.y = 4.8f;
        //m_deferredLightingCB.m_light.directionalLight[0].color.z = 4.8f;

        //m_deferredLightingCB.m_light.directionalLight[0].direction.x = 1.0f;
        //m_deferredLightingCB.m_light.directionalLight[0].direction.y = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[0].direction.z = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[0].direction.Normalize();
        //m_deferredLightingCB.m_light.directionalLight[0].castShadow = true;

        ////
        //m_deferredLightingCB.m_light.directionalLight[1].color.x = 4.8f;
        //m_deferredLightingCB.m_light.directionalLight[1].color.y = 4.8f;
        //m_deferredLightingCB.m_light.directionalLight[1].color.z = 4.8f;

        //m_deferredLightingCB.m_light.directionalLight[1].direction.x = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[1].direction.y = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[1].direction.z = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[1].direction.Normalize();
        //m_deferredLightingCB.m_light.directionalLight[1].castShadow = true;

        ////地面からの照り返し
        //m_deferredLightingCB.m_light.directionalLight[2].color.x = 0.8f;
        //m_deferredLightingCB.m_light.directionalLight[2].color.y = 0.8f;
        //m_deferredLightingCB.m_light.directionalLight[2].color.z = 0.8f;

        //m_deferredLightingCB.m_light.directionalLight[2].direction.x = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[2].direction.y = 1.0f;
        //m_deferredLightingCB.m_light.directionalLight[2].direction.z = -1.0f;
        //m_deferredLightingCB.m_light.directionalLight[2].direction.Normalize();

        //m_deferredLightingCB.m_light.ambinetLight.x = 0.2f;
        //m_deferredLightingCB.m_light.ambinetLight.y = 0.2f;
        //m_deferredLightingCB.m_light.ambinetLight.z = 0.2f;
        //m_deferredLightingCB.m_light.eyePos = g_camera3D->GetPosition();
        //m_deferredLightingCB.m_light.specPow = 5.0f;

        //// ポストエフェクト的にディファードライティングを行うためのスプライトを初期化
        //SpriteInitData spriteInitData;

        //// 画面全体にレンダリングするので幅と高さはフレームバッファーの幅と高さと同じ
        //spriteInitData.m_width = frameBuffer_w;
        //spriteInitData.m_height = frameBuffer_h;

        //// ディファードライティングで使用するテクスチャを設定
        //int texNo = 0;
        //for (auto& gBuffer : m_gBuffer)
        //{
        //    spriteInitData.m_textures[texNo++] = &gBuffer.GetRenderTargetTexture();
        //}

        //spriteInitData.m_fxFilePath = "../Media/shader/preset/DeferredLighting.fx";
        //spriteInitData.m_expandConstantBuffer = &m_deferredLightingCB;
        //spriteInitData.m_expandConstantBufferSize = sizeof(m_deferredLightingCB);
        //for (int i = 0; i < NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT; i++)
        //{
        //    for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        //    {
        //        spriteInitData.m_textures[texNo++] = &m_shadowMapRenders[i].GetShadowMap(areaNo);
        //    }
        //}
        //spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        ////spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        //// 初期化データを使ってスプライトを作成
        //m_diferredLightingSprite.Init(spriteInitData);
    }

    void RenderingEngine::Execute(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        //const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        //rc.ClearRenderTargetView(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), clearColor);
        ////rc.ClearDepthStencilView(m_mainRenderTarget.GetDSVCpuDescriptorHandle(), 1.0f);//mainRenderTargetにはDepthBufferは無い　NULL
        //rc.ClearDepthStencilView(m_zprepassRenderTarget.GetDSVCpuDescriptorHandle(), 1.0f);



        // シャドウマップへの描画
        if (g_enableshadow) {
            RenderToShadowMap(rc);
        }
        

        // ZPrepass
        //2023/12/05
        //4Kモードでは重いシーンで効果があった
        //2Kモードでは遮蔽面積が小さいために　ZPrepassのコストの方が大きくなり遅くなる
        //ZPrepassは4Kモードの場合だけ呼び出すことに.
        //if (g_4kresolution) {
        //if (g_zpreflag) {//2023/12/09 DispAndLimitsメニューのオプションに.
        //2024/03/15 ZPrepassオフの場合にもクリアが必要
            ZPrepass(rc);
        //}
        

        // G-Bufferへのレンダリング
        //RenderToGBuffer(rc);

        // ディファードライティング
        //DeferredLighting(rc);

        // ディファードライティングが終わった時点でスナップショットを撮影する
        //SnapshotMainRenderTarget(rc, EnMainRTSnapshot::enDrawnOpacity);

        // フォワードレンダリング
        ForwardRendering(rc);

        if (g_hdrpbloom) {
            // ポストエフェクトを実行
            m_postEffect.Render(rc, m_mainRenderTarget);
        }


        SpriteRendering(rc);//m_postEffectよりも後で



        // メインレンダリングターゲットの内容をフレームバッファにコピー
        CopyMainRenderTargetToFrameBuffer(rc);

        // 登録されている3Dモデルをクリア
        //m_shadowmapModels.clear();
        m_renderToGBufferModels.clear();
        m_forwardRenderModels.clear();
        m_instancingRenderModels.clear();
        //m_zprepassModels.clear();
        m_forwardRenderSprites.clear();
        m_forwardRenderFont.clear();
    }

    void RenderingEngine::RenderToShadowMap(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        //int ligNo = 0;
        //for (auto& shadowMapRender : m_shadowMapRenders)
        //{
        //    shadowMapRender.Render(
        //        rc,
        //        m_deferredLightingCB.m_light.directionalLight[ligNo].direction
        //    );
        //    ligNo++;
        //}

        rc->WaitUntilToPossibleSetRenderTarget(m_shadowMapRenderTarget);
        rc->SetRenderTargetAndViewport(m_shadowMapRenderTarget);
        rc->ClearRenderTargetView(m_shadowMapRenderTarget);

        // 影モデルを描画
        for (auto& currenderobj : m_forwardRenderModels)
        {
            if (g_enableshadow) {
                if (currenderobj.pmodel && (currenderobj.pmodel->GetSkyFlag() == false) && 
                    (currenderobj.renderkind == RENDERKIND_SHADOWMAP)) {
                    RenderPolyMeshShadowMap(rc, currenderobj);
                }
            }
        }


        // 書き込み完了待ち
        rc->WaitUntilFinishDrawingToRenderTarget(m_shadowMapRenderTarget);

        // step-7 シャドウマップをぼかすためのガウシアンブラーを実行する
        //m_shadowBlur.ExecuteOnGPU(rc, 5.0f);

    }

    void RenderingEngine::ZPrepass(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        // まず、レンダリングターゲットとして設定できるようになるまで待つ
        rc->WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget);

        rc->SetRenderTarget(
            m_zprepassRenderTarget.GetRTVCpuDescriptorHandle(),
            m_zprepassRenderTarget.GetDSVCpuDescriptorHandle()
        );
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        rc->ClearRenderTargetView(m_zprepassRenderTarget.GetRTVCpuDescriptorHandle(), clearColor);

        ////前のパスで異なるviewportを設定した場合にはviewportの設定し直しが必要
        rc->SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());
        rc->ClearDepthStencilView(m_zprepassRenderTarget.GetDSVCpuDescriptorHandle(), 1.0f);


        if (g_zpreflag) {
            //for (auto& currenderobj : m_zprepassModels)
            for (auto& currenderobj : m_forwardRenderModels)
            {
                //2024/03/15
                //天球をぼかすと　背景の無いシーンで　カメラ距離に関わらずキャラクターがボケてしまう
                //背景の無いシーンではg_skydofflagをfalseにし、　背景のあるシーンでtrueにする
                if ((g_skydofflag == true) || (currenderobj.pmodel->GetSkyFlag() == false)) {
                    RenderPolyMeshZPre(rc, currenderobj);
                }
            }
        }

        rc->WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget);
    }

    void RenderingEngine::ForwardRendering(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }
        rc->WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        rc->SetRenderTarget(
            m_mainRenderTarget.GetRTVCpuDescriptorHandle(),
            //m_gBuffer[enGBufferAlbedo].GetDSVCpuDescriptorHandle()
            m_zprepassRenderTarget.GetDSVCpuDescriptorHandle()
        );
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        rc->ClearRenderTargetView(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), clearColor);

        //////前のパスで異なるviewportを設定した場合にはviewportの設定し直しが必要
        rc->SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());


        for (auto& currenderobj : m_forwardRenderModels)
        {
            if (g_enableshadow) {
                if (currenderobj.pmodel && (currenderobj.pmodel->GetSkyFlag() == false) && 
                    (currenderobj.renderkind == RENDERKIND_SHADOWMAP)) {
                    RenderPolyMeshShadowReciever(rc, currenderobj);
                }
                else {
                    RenderPolyMesh(rc, currenderobj);
                }
            }
            else {
                RenderPolyMesh(rc, currenderobj);
            }
        }

        for (auto& currenderobjinsta : m_instancingRenderModels)
        {
            RenderPolyMeshInstancing(rc, currenderobjinsta);
        }


        // メインレンダリングターゲットへの書き込み終了待ち
       rc->WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    }


    void RenderingEngine::SpriteRendering(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }
        rc->WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        rc->SetRenderTarget(
            m_mainRenderTarget.GetRTVCpuDescriptorHandle(),
            //m_gBuffer[enGBufferAlbedo].GetDSVCpuDescriptorHandle()
            m_zprepassRenderTarget.GetDSVCpuDescriptorHandle()
        );
        //////前のパスで異なるviewportを設定した場合にはviewportの設定し直しが必要
        rc->SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());


        //Sprite (ScreenVertexMode)
        for (auto& currendersprite : m_forwardRenderSprites)
        {
            if (currendersprite.psprite) {
                currendersprite.psprite->DrawScreen(rc);
            }
            else if (currendersprite.pinstancedsprite) {
                currendersprite.pinstancedsprite->DrawScreen(rc);
            }
            else if (currendersprite.pfpssprite) {
                currendersprite.pfpssprite->DrawScreen(rc, currendersprite.userint1);
            }
            else if (currendersprite.pundosprite) {
                currendersprite.pundosprite->DrawScreen(rc, currendersprite.userint1, currendersprite.userint2);
            }
        }

        //Font : MiniEngine
        for (auto& currenderfont : m_forwardRenderFont)
        {
            if (currenderfont.pfont && (currenderfont.strfont[0] != 0L)) {
                currenderfont.pfont->Begin(rc);
                currenderfont.pfont->Draw(currenderfont.strfont,
                    currenderfont.disppos, currenderfont.color,
                    currenderfont.rotation, currenderfont.scale, currenderfont.pivot);
                currenderfont.pfont->End(rc);
            }
        }

        // メインレンダリングターゲットへの書き込み終了待ち
        rc->WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    }


    //void RenderingEngine::RenderToGBuffer(RenderContext* rc)
    //{
    //    if (!rc) {
    //        _ASSERT(0);
    //        return;
    //    }
    //    // レンダリングターゲットをG-Bufferに変更
    //    RenderTarget* rts[enGBufferNum] = {
    //        &m_gBuffer[enGBufferAlbedo],        // 0番目のレンダリングターゲット
    //        &m_gBuffer[enGBufferNormal],        // 1番目のレンダリングターゲット
    //        &m_gBuffer[enGBufferWorldPos],      // 2番目のレンダリングターゲット
    //        &m_gBuffer[enGBufferMetalSmooth],   // 3番目のレンダリングターゲット
    //        &m_gBuffer[enGBUfferShadowParam],   // 4番目のレンダリングターゲット
    //    };
    //
    //    // まず、レンダリングターゲットとして設定できるようになるまで待つ
    //    rc->WaitUntilToPossibleSetRenderTargets(ARRAYSIZE(rts), rts);
    //
    //    // レンダリングターゲットを設定
    //    rc->SetRenderTargets(ARRAYSIZE(rts), rts);
    //
    //    // レンダリングターゲットをクリア
    //    rc->ClearRenderTargetViews(ARRAYSIZE(rts), rts);
    //    for (auto& currenderobj : m_renderToGBufferModels)
    //    {
    //        //model->Draw(rc);
    //        RenderPolyMesh(rc, currenderobj);
    //    }
    //
    //    // レンダリングターゲットへの書き込み待ち
    //    rc->WaitUntilFinishDrawingToRenderTargets(ARRAYSIZE(rts), rts);
    //}

    //void RenderingEngine::SnapshotMainRenderTarget(RenderContext* rc, EnMainRTSnapshot enSnapshot)
    //{
    //    if (!rc) {
    //        _ASSERT(0);
    //        return;
    //    }
    //    // メインレンダリングターゲットの内容をスナップショット
    //    rc->WaitUntilToPossibleSetRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
    //    rc->SetRenderTargetAndViewport(m_mainRTSnapshots[(int)enSnapshot]);
    //    m_copyMainRtToFrameBufferSprite.Draw(rc);
    //    rc->WaitUntilFinishDrawingToRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
    //}

    void RenderingEngine::DeferredLighting(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }
        //// ディファードライティングに必要なライト情報を更新する
        //m_deferredLightingCB.m_light.eyePos = g_camera3D->GetPosition();
        //for (int i = 0; i < NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT; i++)
        //{
        //    for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        //    {
        //        m_deferredLightingCB.mlvp[i][areaNo] = m_shadowMapRenders[i].GetLVPMatrix(areaNo);
        //    }
        //}

        //// レンダリング先をメインレンダリングターゲットにする
        //// メインレンダリングターゲットを設定
        //rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        //rc.SetRenderTargetAndViewport(m_mainRenderTarget);

        //// G-Bufferの内容を元にしてディファードライティング
        //m_diferredLightingSprite.Draw(rc);

        //// メインレンダリングターゲットへの書き込み終了待ち
        //rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    }

    void RenderingEngine::CopyMainRenderTargetToFrameBuffer(RenderContext* rc)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }
        rc->WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        //g_graphicsEngine->BeginRender();
        //rc->WaitUntilToPossibleSetRenderTarget(g_graphicsEngine->GetRenderTarget());//2023/12/19

        // メインレンダリングターゲットの絵をフレームバッファーにコピー
        rc->SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );
        //rc->ClearDepthStencilView(g_graphicsEngine->GetCurrentFrameBuffuerDSV(), 1.0f);

        // ビューポートを指定する
        //D3D12_VIEWPORT viewport;
        //viewport.TopLeftX = 0;
        //viewport.TopLeftY = 0;
        ////viewport.Width = 1280;
        ////viewport.Height = 720;
        ////2023/11/17 SamllWindow時の値決め打ちでテスト
        //viewport.Width = 736;
        //viewport.Height = 488;
        //viewport.MinDepth = 0.0f;
        //viewport.MaxDepth = 1.0f;

        D3D12_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = (float)g_graphicsEngine->GetFrameBufferWidth();
        viewport.Height = (float)g_graphicsEngine->GetFrameBufferHeight();
        viewport.MinDepth = D3D12_MIN_DEPTH;
        viewport.MaxDepth = D3D12_MAX_DEPTH;


        rc->SetViewportAndScissor(viewport);
        m_copyMainRtToFrameBufferSprite.Draw(rc);
        //m_copyMainRtToFrameBufferSprite.UpdateScreen(ChaVector3(viewport.Width * 0.5f, viewport.Height * 0.5f, 0.0f), ChaVector2(viewport.Width, viewport.Height));
        //m_copyMainRtToFrameBufferSprite.DrawScreen(rc);

        // 書き込み完了待ち
        rc->WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
        //rc->WaitUntilFinishDrawingToRenderTarget(g_graphicsEngine->GetRenderTarget());//2023/12/19
    }

    void RenderingEngine::RenderPolyMesh(RenderContext* rc, RENDEROBJ currenderobj)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        if (currenderobj.mqoobj) {
            if (currenderobj.mqoobj->GetDispObj()) {
                if (currenderobj.mqoobj->GetPm3()) {

                    if (currenderobj.pmodel && (currenderobj.pmodel->GetSkyFlag())) {
                        currenderobj.renderkind = RENDERKIND_NORMAL;
                        //currenderobj.lightflag = 0;
                    }

                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderNormalPM3(rc, currenderobj);
                }
                else if (currenderobj.mqoobj->GetPm4()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderNormal(rc, currenderobj);
                }
            }
            if (currenderobj.mqoobj->GetDispLine() && currenderobj.mqoobj->GetExtLine()) {
                //################################
                //GetDispObj()ではなくGetDispLine()
                //################################
                currenderobj.mqoobj->GetDispLine()->RenderLine(rc, currenderobj);
            }
        }
    }

    void RenderingEngine::RenderPolyMeshInstancing(RenderContext* rc, RENDEROBJ currenderobj)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        if (currenderobj.mqoobj) {
            if (currenderobj.mqoobj->GetDispObj()) {
                if (currenderobj.mqoobj->GetPm3()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderInstancingPm3(rc, currenderobj);
                }
                else if (currenderobj.mqoobj->GetPm4()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    //currenderobj.mqoobj->GetDispObj()->RenderNormal(rc, currenderobj);
                    _ASSERT(0);//not support
                }
            }
            if (currenderobj.mqoobj->GetDispLine() && currenderobj.mqoobj->GetExtLine()) {
                //################################
                //GetDispObj()ではなくGetDispLine()
                //################################
                //currenderobj.mqoobj->GetDispLine()->RenderLine(rc, currenderobj);
                _ASSERT(0);//not support
            }
        }
    }

    void RenderingEngine::RenderPolyMeshZPre(RenderContext* rc, RENDEROBJ currenderobj)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        //2023/12/05
        //4Kモードでは重いシーンで効果があった
        //2Kモードでは遮蔽面積が小さいために　ZPrepassのコストの方が大きくなり遅くなる
        //ZPrepassは4Kモードの場合だけ呼び出すことに.

        if (currenderobj.mqoobj) {
            if (currenderobj.mqoobj->GetDispObj()) {
                if (currenderobj.mqoobj->GetPm3()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderZPrePm3(rc, currenderobj);
                }
                else if (currenderobj.mqoobj->GetPm4()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderZPrePm4(rc, currenderobj);
                }
            }
            if (currenderobj.mqoobj->GetDispLine() && currenderobj.mqoobj->GetExtLine()) {
                //################################
                //GetDispObj()ではなくGetDispLine()
                //################################
                //currenderobj.mqoobj->GetDispLine()->RenderLine(rc, currenderobj);
            }
        }
    }


    void RenderingEngine::RenderPolyMeshShadowMap(RenderContext* rc, RENDEROBJ currenderobj)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        if (currenderobj.mqoobj) {
            if (currenderobj.mqoobj->GetDispObj()) {
                if (currenderobj.mqoobj->GetPm3()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderShadowMapPM3(rc, currenderobj);
                }
                else if (currenderobj.mqoobj->GetPm4()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderShadowMap(rc, currenderobj);
                }
            }
            //if (currenderobj.mqoobj->GetDispLine() && currenderobj.mqoobj->GetExtLine()) {
            //    //################################
            //    //GetDispObj()ではなくGetDispLine()
            //    //################################
            //    currenderobj.mqoobj->GetDispLine()->RenderLine(rc, currenderobj);
            //}
        }
    }

    void RenderingEngine::RenderPolyMeshShadowReciever(RenderContext* rc, RENDEROBJ currenderobj)
    {
        if (!rc) {
            _ASSERT(0);
            return;
        }

        if (currenderobj.mqoobj) {
            if (currenderobj.mqoobj->GetDispObj()) {
                if (currenderobj.mqoobj->GetPm3()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderShadowRecieverPM3(rc, currenderobj);
                }
                else if (currenderobj.mqoobj->GetPm4()) {
                    //CallF(SetShaderConst(curobj, btflag, calcslotflag), return 1);
                    currenderobj.mqoobj->GetDispObj()->RenderShadowReciever(rc, currenderobj);
                }
            }
            if (currenderobj.mqoobj->GetDispLine() && currenderobj.mqoobj->GetExtLine()) {
                //################################
                //GetDispObj()ではなくGetDispLine()
                //################################
                currenderobj.mqoobj->GetDispLine()->RenderLine(rc, currenderobj);
            }
        }
    }


}

