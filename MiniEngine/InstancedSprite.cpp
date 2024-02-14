#include "stdafx.h"
#include "InstancedSprite.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <GlobalVar.h>

    namespace {
        struct SSimpleVertex {
            Vector4 pos;
            Vector2 tex;
        };
    }
    const Vector2	InstancedSprite::DEFAULT_PIVOT = { 0.5f, 0.5f };
    InstancedSprite::~InstancedSprite()
    {
        DestroyObjs();
    }
    void InstancedSprite::DestroyObjs()
    {
        m_indexBuffer.DestroyObjs();
        m_vertexBuffer.DestroyObjs();
        m_possizeBuffer.DestroyObjs();

        m_constantBufferGPU.DestroyObjs();
        m_userExpandConstantBufferGPU.DestroyObjs();

        m_pipelineState.DestroyObjs();
        m_descriptorHeap.DestroyObjs();

        m_rootSignature.DestroyObjs();
    }

    void InstancedSprite::InitTextures(const SpriteInitData& initData)
    {
        //スプライトで使用するテクスチャを準備する。

        //2023/11/20 WICテクスチャ対応
        //if (initData.m_wicFilePath[0] != nullptr) {
        //    //ddsファイルのパスが指定されてるのなら、ddsファイルからテクスチャを作成する。
        //    int texNo = 0;
        //    while (initData.m_wicFilePath[texNo] && texNo < MAX_TEXTURE) {
        //        wchar_t wwicFilePath[1024];
        //        mbstowcs(wwicFilePath, initData.m_wicFilePath[texNo], 1023);
        //        m_textures[texNo].InitFromWICFile(wwicFilePath);
        //        texNo++;
        //    }
        //    m_numTexture = texNo;
        //}
        //else if (initData.m_ddsFilePath[0] != nullptr) {
        //    //ddsファイルのパスが指定されてるのなら、ddsファイルからテクスチャを作成する。
        //    int texNo = 0;
        //    while (initData.m_ddsFilePath[texNo] && texNo < MAX_TEXTURE) {
        //        wchar_t wddsFilePath[1024];
        //        mbstowcs(wddsFilePath, initData.m_ddsFilePath[texNo], 1023);
        //        m_textures[texNo].InitFromDDSFile(wddsFilePath);
        //        texNo++;
        //    }
        //    m_numTexture = texNo;
        //}
        //else 
        if (initData.m_textures[0] != nullptr) {
            //外部テクスチャを指定されている。
            int texNo = 0;
            //while (initData.m_textures[texNo] != nullptr) {
                m_textureExternal = initData.m_textures[0];
                texNo++;
            //}
            //m_numTexture = texNo;
        }
        else {
            //テクスチャが指定されてない。
            MessageBoxA(nullptr, "initData.m_ddsFilePathかm_texturesのどちらかに使用するテクスチャの情報を設定してください。", "エラー", MB_OK);
            std::abort();
        }
    }
    void InstancedSprite::InitShader(const SpriteInitData& initData)
    {
        if (initData.m_fxFilePath == nullptr) {
            MessageBoxA(nullptr, "fxファイルが指定されていません。", "エラー", MB_OK);
            std::abort();
        }
       
        //シェーダーをロードする。
        m_vs.LoadVS(initData.m_fxFilePath, initData.m_vsEntryPointFunc);
        m_ps.LoadPS(initData.m_fxFilePath, initData.m_psEntryPoinFunc);
    }
    void InstancedSprite::InitDescriptorHeap(const SpriteInitData& initData)
    {
        if (m_textureExternal!= nullptr) {
            //外部のテクスチャが指定されている。
            //for (int texNo = 0; texNo < m_numTexture; texNo++) {
                m_descriptorHeap.RegistShaderResource(0, *m_textureExternal);
            //}
        }
        else {
            _ASSERT(0);
            return;

            //for (int texNo = 0; texNo < m_numTexture; texNo++) {
            //    m_descriptorHeap.RegistShaderResource(texNo, m_textures[texNo]);
            //}
        }
        if (initData.m_expandShaderResoruceView != nullptr) {
            //拡張シェーダーリソースビュー。
            m_descriptorHeap.RegistShaderResource(
                EXPAND_SRV_REG__START_NO,
                *initData.m_expandShaderResoruceView
            );
        }
        m_descriptorHeap.RegistConstantBuffer(0, m_constantBufferGPU);
        if (m_userExpandConstantBufferCPU != nullptr) {
            //ユーザー拡張の定数バッファはb1に関連付けする。
            m_descriptorHeap.RegistConstantBuffer(1, m_userExpandConstantBufferGPU);
        }
        m_descriptorHeap.Commit();
    }
    void InstancedSprite::InitVertexBufferAndIndexBuffer(const SpriteInitData& initData)
    {

        float halfW, halfH;
        halfW = 1.0f;
        halfH = 1.0f;

        //頂点バッファのソースデータ。
        SSimpleVertex vertices[] =
        {
            {
                Vector4(-halfW, -halfH, 0.0f, 1.0f),
                Vector2(0.0f, 1.0f),
            },
            {
                Vector4(halfW, -halfH, 0.0f, 1.0f),
                Vector2(1.0f, 1.0f),
            },
            {
                Vector4(-halfW, halfH, 0.0f, 1.0f),
                Vector2(0.0f, 0.0f)
            },
            {
                Vector4(halfW, halfH, 0.0f, 1.0f),
                Vector2(1.0f, 0.0f)
            }

        };
        //unsigned short indices[] = { 0,1,2,3 };
        //unsigned short indices[] = { 0, 2, 1, 3 };//右手座標系　RH
        unsigned int indices[] = { 0, 2, 1, 3 };//右手座標系　RH

        m_vertexBuffer.Init(sizeof(vertices), sizeof(vertices[0]));
        m_vertexBuffer.Copy(vertices);

        m_indexBuffer.Init(sizeof(indices), sizeof(indices[0]));
        m_indexBuffer.Copy(indices);



        int instano;
        for (instano = 0; instano < INSTANCEDSPMAX; instano++) {
            m_possize[instano].Init();
        }
        UINT possizestride = sizeof(SPPOSSIZECOL);
        UINT possizebuffsize = possizestride * INSTANCEDSPMAX;
        m_possizeBuffer.Init(possizebuffsize, possizestride);
        m_possizeBuffer.Copy(m_possize);

    }


    // Matches CommonStates::AlphaBlend
    const D3D12_BLEND_DESC s_DefaultBlendDesc =
    {
        FALSE, // AlphaToCoverageEnable
        FALSE, // IndependentBlendEnable
        { {
            TRUE, // BlendEnable
            FALSE, // LogicOpEnable
            D3D12_BLEND_ONE, // SrcBlend
            D3D12_BLEND_INV_SRC_ALPHA, // DestBlend
            D3D12_BLEND_OP_ADD, // BlendOp
            D3D12_BLEND_ONE, // SrcBlendAlpha
            D3D12_BLEND_INV_SRC_ALPHA, // DestBlendAlpha
            D3D12_BLEND_OP_ADD, // BlendOpAlpha
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL
        } }
    };

    // Same to CommonStates::CullCounterClockwise
    const D3D12_RASTERIZER_DESC s_DefaultRasterizerDesc =
    {
        D3D12_FILL_MODE_SOLID,

        D3D12_CULL_MODE_NONE,
        //D3D12_CULL_MODE_BACK,


        FALSE, // FrontCounterClockwise
        //TRUE,

        D3D12_DEFAULT_DEPTH_BIAS,
        D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,

        //TRUE, // DepthClipEnable
        FALSE, // DepthClipEnable
        
        TRUE, // MultisampleEnable
        FALSE, // AntialiasedLineEnable
        0, // ForcedSampleCount
        D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };

    // Same as CommonStates::DepthNone
    const D3D12_DEPTH_STENCIL_DESC s_DefaultDepthStencilDesc =
    {
        //FALSE, // DepthEnable
        TRUE,

        //D3D12_DEPTH_WRITE_MASK_ZERO,
        D3D12_DEPTH_WRITE_MASK_ALL,
        
        //D3D12_COMPARISON_FUNC_LESS_EQUAL, // DepthFunc
        D3D12_COMPARISON_FUNC_ALWAYS, // DepthFunc

        FALSE, // StencilEnable
        D3D12_DEFAULT_STENCIL_READ_MASK,
        D3D12_DEFAULT_STENCIL_WRITE_MASK,
        {
            D3D12_STENCIL_OP_KEEP, // StencilFailOp
            D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
            D3D12_STENCIL_OP_KEEP, // StencilPassOp
            D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
        }, // FrontFace
        {
            D3D12_STENCIL_OP_KEEP, // StencilFailOp
            D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
            D3D12_STENCIL_OP_KEEP, // StencilPassOp
            D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
        } // BackFace
    };




    void InstancedSprite::InitPipelineState(const SpriteInitData& initData)
    {
        // 頂点レイアウトを定義する。
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

            { "POSITION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 
            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
            { "POSITION", 2, DXGI_FORMAT_R32G32_FLOAT, 1, 
            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
            { "POSITION", 3, DXGI_FORMAT_R32G32_FLOAT, 1,
            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        };

        //パイプラインステートを作成。
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vs.GetCompiledBlob());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_ps.GetCompiledBlob());
        
        //psoDesc.RasterizerState = s_DefaultRasterizerDesc;

        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        //psoDesc.BlendState = s_DefaultBlendDesc;


        psoDesc.BlendState.IndependentBlendEnable = TRUE;
        if (initData.m_alphaBlendMode == AlphaBlendMode_Trans) {
            //半透明合成のブレンドステートを作成する。
            psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        }
        else if (initData.m_alphaBlendMode == AlphaBlendMode_Add) {
            //加算合成。
            psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        }

        psoDesc.DepthStencilState = s_DefaultDepthStencilDesc;
        //psoDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        //psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        //psoDesc.DepthStencilState.DepthEnable = FALSE;
        //psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        //psoDesc.DepthStencilState.StencilEnable = FALSE;
        //psoDesc.SampleMask = UINT_MAX;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        //psoDesc.DepthStencilState.DepthEnable = FALSE;//!!!
        psoDesc.DepthStencilState.DepthEnable = TRUE;//!!!
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        //psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;


        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        for (auto& format : initData.m_colorBufferFormat) {
            if (format == DXGI_FORMAT_UNKNOWN) {
                break;
            }
            psoDesc.RTVFormats[psoDesc.NumRenderTargets] = format;
            psoDesc.NumRenderTargets++;
        }
        
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
    
        m_pipelineState.Init(psoDesc);
    }
    void InstancedSprite::InitConstantBuffer(const SpriteInitData& initData)
    {
        //定数バッファの初期化。
        m_constantBufferGPU.Init(sizeof(m_constantBufferCPU), nullptr);
        //ユーザー拡張の定数バッファが指定されている。
        if (initData.m_expandConstantBuffer != nullptr){
            m_userExpandConstantBufferCPU = initData.m_expandConstantBuffer;
            m_userExpandConstantBufferGPU.Init(
                initData.m_expandConstantBufferSize, 
                initData.m_expandConstantBuffer
            );
        }
    }
    void InstancedSprite::Init(const SpriteInitData& initData)
    {
        m_size.x = static_cast<float>(initData.m_width);
        m_size.y = static_cast<float>(initData.m_height);

        //テクスチャを初期化。
        InitTextures(initData);
        //頂点バッファとインデックスバッファを初期化。
        InitVertexBufferAndIndexBuffer(initData);
        //定数バッファを初期化。
        InitConstantBuffer(initData);
        
        //ルートシグネチャの初期化。
        m_rootSignature.Init(
            initData.m_samplerFilter,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        //シェーダーを初期化。
        InitShader(initData);
        //パイプラインステートの初期化。
        InitPipelineState(initData);
        //ディスクリプタヒープを初期化。
        InitDescriptorHeap(initData);

        //2023/11/21
        //fileから読み込んだ場合のために　Textureからサイズを取得
        //if (m_textures[0].IsValid()) {
        //    m_size.x = static_cast<float>(m_textures[0].GetWidth());
        //    m_size.y = static_cast<float>(m_textures[0].GetHeight());
        //}
        //else 
        if (m_textureExternal != nullptr) {
            m_size.x = static_cast<float>(m_textureExternal->GetWidth());
            m_size.y = static_cast<float>(m_textureExternal->GetHeight());
        }
        else {
            _ASSERT(0);
        }

    }
    //void InstancedSprite::Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot)
    //{
    //    //ピボットを考慮に入れた平行移動行列を作成。
    //    //ピボットは真ん中が0.0, 0.0、左上が-1.0f, -1.0、右下が1.0、1.0になるようにする。
    //    Vector2 localPivot = pivot;
    //    localPivot.x -= 0.5f;
    //    localPivot.y -= 0.5f;
    //    localPivot.x *= -2.0f;
    //    localPivot.y *= -2.0f;
    //    //画像のハーフサイズを求める。
    //    Vector2 halfSize = m_size;
    //    halfSize.x *= 0.5f;
    //    halfSize.y *= 0.5f;
    //    Matrix mPivotTrans;

    //    mPivotTrans.MakeTranslation(
    //        { halfSize.x * localPivot.x, halfSize.y * localPivot.y, 0.0f }
    //    );
    //    Matrix mTrans, mRot, mScale;
    //    mTrans.MakeTranslation(pos);
    //    mRot.MakeRotationFromQuaternion(rot);
    //    mScale.MakeScaling(scale);
    //    m_world = mPivotTrans * mScale;
    //    m_world = m_world * mRot;
    //    m_world = m_world * mTrans;
    //}


    //void InstancedSprite::UpdateScreen(ChaVector3 srcpos, ChaVector2 srcdispsize)
    //{
    //    float dispsizex, dispsizey;
    //    dispsizey = srcdispsize.y;
    //    dispsizex = srcdispsize.x;// *m_size.x / m_size.y;

    //    //D3D12_VIEWPORT viewport = renderContext.GetViewport();
    //    UINT screenW, screenH;
    //    screenW = g_graphicsEngine->GetFrameBufferWidth();
    //    screenH = g_graphicsEngine->GetFrameBufferHeight();


    //    m_world.SetIdentity();
    //    m_world._11 = dispsizex / (float)screenW;
    //    m_world._22 = dispsizey / (float)screenH;
    //    m_world._33 = 0.0f;
    //    m_world._41 = (srcpos.x / (float)screenW * 2.0f - 1.0f);
    //    m_world._42 = -(srcpos.y / (float)screenH * 2.0f - 1.0f);
    //    m_world._43 = srcpos.z;

    //}

    void InstancedSprite::UpdateScreen(int instanceno, 
        ChaVector3 srcpos, ChaVector2 srcdispsize, ChaVector4 srccolmult)
    {
        if ((instanceno >= 0) && (instanceno < INSTANCEDSPMAX)) {
            m_possize[instanceno].pos = ChaVector4(srcpos, 1.0f);
            m_possize[instanceno].size = srcdispsize;
            m_possize[instanceno].colmult = srccolmult;

            UINT screenW, screenH;
            screenW = g_graphicsEngine->GetFrameBufferWidth();
            screenH = g_graphicsEngine->GetFrameBufferHeight();
            m_possize[instanceno].WH = ChaVector2((float)screenW, (float)screenH);

            if (instanceno >= m_instancenum) {
                m_instancenum = instanceno + 1;//!!!!!!!!!!!
            }
        }
        else {
            _ASSERT(0);
            return;
        }
    }

    void InstancedSprite::DrawScreen(RenderContext* renderContext)
    {
        if (!g_graphicsEngine || !renderContext) {
            _ASSERT(0);
            return;
        }
        if (m_instancenum <= 0) {
            return;
        }

        UINT possizestride = sizeof(SPPOSSIZECOL);
        UINT possizebuffsize = possizestride * m_instancenum;
        m_possizeBuffer.Copy(m_possize);



        UINT screenW, screenH;
        screenW = g_graphicsEngine->GetFrameBufferWidth();
        screenH = g_graphicsEngine->GetFrameBufferHeight();

        m_constantBufferCPU.mvp = m_world;//!!!!!!!!!!!!!!!!
        m_constantBufferCPU.mulColor.x = 1.0f;
        m_constantBufferCPU.mulColor.y = 1.0f;
        m_constantBufferCPU.mulColor.z = 1.0f;
        m_constantBufferCPU.mulColor.w = 1.0f;
        m_constantBufferCPU.screenParam.x = 0.0f;//2023/11/21
        m_constantBufferCPU.screenParam.y = 1.0f;//2023/11/21
        m_constantBufferCPU.screenParam.z = (float)screenW;//2023/11/21
        m_constantBufferCPU.screenParam.w = (float)screenH;//2023/11/21

        

        //定数バッファを更新。
        //renderContext.UpdateConstantBuffer(m_constantBufferGPU, &m_constantBufferCPU);
        m_constantBufferGPU.CopyToVRAM(&m_constantBufferCPU);
        if (m_userExpandConstantBufferCPU != nullptr) {
            //renderContext.UpdateConstantBuffer(m_userExpandConstantBufferGPU, m_userExpandConstantBufferCPU);
            m_userExpandConstantBufferGPU.CopyToVRAM(m_userExpandConstantBufferCPU);
        }
        //ルートシグネチャを設定。
        renderContext->SetRootSignature(m_rootSignature);
        //パイプラインステートを設定。
        renderContext->SetPipelineState(m_pipelineState);
        //頂点バッファを設定。
        renderContext->SetVertexBuffer(m_vertexBuffer);
        renderContext->SetVertexBuffer(1, m_possizeBuffer);
        


        //インデックスバッファを設定。
        renderContext->SetIndexBuffer(m_indexBuffer);
        //プリミティブトポロジーを設定する。
        renderContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        //ディスクリプタヒープを設定する。
        renderContext->SetDescriptorHeap(m_descriptorHeap);
        //描画
        renderContext->DrawIndexedInstanced(m_indexBuffer.GetCount(), m_instancenum);
        //.DrawIndexedInstanced(m_indexBuffer.GetCount(), numinstanced);


        m_instancenum = 0;//ここで初期化しないと　前回の表示が残ったりする

    }


    //void InstancedSprite::Draw(RenderContext& renderContext)
    //{
    //    //現在のビューポートから平行投影行列を計算する。
    //    D3D12_VIEWPORT viewport = renderContext.GetViewport();
    //    //todo カメラ行列は定数に使用。どうせ変えないし・・・。
    //    Matrix viewMatrix = g_camera2D->GetViewMatrix(true);
    //    Matrix projMatrix;
    //    projMatrix.MakeOrthoProjectionMatrix(viewport.Width, viewport.Height, 0.1f, 1.0f);

    //    m_constantBufferCPU.mvp = m_world * viewMatrix * projMatrix;
    //    m_constantBufferCPU.mulColor.x = 1.0f;
    //    m_constantBufferCPU.mulColor.y = 1.0f;
    //    m_constantBufferCPU.mulColor.z = 1.0f;
    //    m_constantBufferCPU.mulColor.w = 1.0f;
    //    m_constantBufferCPU.screenParam.x = g_camera3D->GetNear();
    //    m_constantBufferCPU.screenParam.y = g_camera3D->GetFar();
    //    //m_constantBufferCPU.screenParam.z = s_mainwidth;
    //    //m_constantBufferCPU.screenParam.w = s_mainheight;
    //    //m_constantBufferCPU.screenParam.z = 736;//2023/11/18 tmp set
    //    //m_constantBufferCPU.screenParam.w = 488;//2023/11/18 tmp set
    //    m_constantBufferCPU.screenParam.z = viewport.Width;//2023/11/18 tmp set
    //    m_constantBufferCPU.screenParam.w = viewport.Height;//2023/11/18 tmp set


    //    //定数バッファを更新。
    //    //renderContext.UpdateConstantBuffer(m_constantBufferGPU, &m_constantBufferCPU);
    //    m_constantBufferGPU.CopyToVRAM(&m_constantBufferCPU);
    //    if (m_userExpandConstantBufferCPU != nullptr) {
    //        //renderContext.UpdateConstantBuffer(m_userExpandConstantBufferGPU, m_userExpandConstantBufferCPU);
    //        m_userExpandConstantBufferGPU.CopyToVRAM(m_userExpandConstantBufferCPU);
    //    }
    //    //ルートシグネチャを設定。
    //    renderContext.SetRootSignature(m_rootSignature);
    //    //パイプラインステートを設定。
    //    renderContext.SetPipelineState(m_pipelineState);
    //    //頂点バッファを設定。
    //    renderContext.SetVertexBuffer(m_vertexBuffer);
    //    //インデックスバッファを設定。
    //    renderContext.SetIndexBuffer(m_indexBuffer);
    //    //プリミティブトポロジーを設定する。
    //    renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //    //ディスクリプタヒープを設定する。
    //    renderContext.SetDescriptorHeap(m_descriptorHeap);
    //    //描画
    //    renderContext.DrawIndexed(m_indexBuffer.GetCount());
    //}

