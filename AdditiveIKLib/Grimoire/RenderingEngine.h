#pragma once

#include "MyRenderer.h"
//#include "ShadowMapRender.h"
#include "PostEffect.h"
#include <ChaVecCalc.h>
#include <mqoobject.h>


//class CMQOObject;
class CDispObj;
class Sprite;
class InstancedSprite;
class CFpsSprite;
class CUndoSprite;

namespace myRenderer
{
    typedef struct tag_renderobj
    {
        CModel* pmodel;
        CMQOObject* mqoobj;
        int shadertype;//MQOSHADER_PBR, MQOSHADER_STD, MQOSHADER_TOONの中から選択　-1指定でAutoSelect
        int renderkind;//CDispObj::Render*()内でセット。RENDERKIND_NORMAL, _SHADOWMAP, _SHADOERECIEVER, ZPREPSSから選択
        Matrix mWorld;
        bool withalpha;
        bool forcewithalpha;//rendergroup >= 2の場合強制半透明
        int lightflag;
        ChaVector4 diffusemult;
        ChaVector4 materialdisprate;
        bool calcslotflag;
        int btflag;
        bool zcmpalways;
        bool zenable;
        int refposindex;


        bool operator< (const tag_renderobj& right) const {//sort用
            if (!mqoobj) {
                return false;
            }
            double srcdist = mqoobj->GetDistFromCamera();
            if (!right.mqoobj) {
                return true;
            }
            double cmpdist = right.mqoobj->GetDistFromCamera();

            double diffdist = srcdist - cmpdist;
            if (diffdist < 0) {
                return true;
            }
            else if (diffdist > 0) {
                return false;
            }
            else {
                return (mqoobj->GetObjectNo() < right.mqoobj->GetObjectNo());
            }
        };


        void Init()
        {
            pmodel = nullptr;
            mqoobj = nullptr;
            shadertype = -1;
            renderkind = -1;
            mWorld.SetIdentity();
            withalpha = false;
            forcewithalpha = false;
            //lightflag = 1;
            lightflag = -1;//2024/03/07
            diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
            materialdisprate = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
            calcslotflag = false;
            btflag = 0;
            zcmpalways = false;
            zenable = true;
            refposindex = 0;
        };

        tag_renderobj()
        {
            Init();
        };
    }RENDEROBJ;

    typedef struct tag_rendersprite {
        Sprite* psprite;
        InstancedSprite* pinstancedsprite;
        CFpsSprite* pfpssprite;
        CUndoSprite* pundosprite;
        int userint1;
        int userint2;
        void Init() {
            psprite = 0;
            pinstancedsprite = 0;
            pfpssprite = 0;
            pundosprite = 0;
            userint1 = 0;
            userint2 = 0;
        };
        tag_rendersprite()
        {
            Init();
        };
    }RENDERSPRITE;

    typedef struct tag_renderfont {
        Font* pfont;
        WCHAR strfont[512];
        Vector2 disppos;//-0.5から0.5
        Vector4 color;
        float rotation;
        float scale;
        Vector2 pivot;

        void Init() {
            pfont = 0;
            ZeroMemory(strfont, sizeof(WCHAR) * 512);
            disppos = Vector2(0.0f, 0.0f);
            color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            rotation = 0.0f;
            scale = 1.0f;
            pivot = Vector2(0.0f, 0.0f);
        };
        tag_renderfont()
        {
            Init();
        };
    }RENDERFONT;


    // レンダリングエンジン
    class RenderingEngine
    {
    public:
        // ディレクションライト
        struct DirectionalLight
        {
            Vector3 direction;  // ライトの方向
            int castShadow;     // 影をキャストする？
            Vector4 color;      // ライトのカラー
        };

        // ライト構造体
        struct Light
        {
            std::array<DirectionalLight, NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT> directionalLight; // ディレクションライト
            Vector3 eyePos;         // カメラの位置
            float specPow;          // スペキュラの絞り
            Vector3 ambinetLight;   // 環境光
        };

        // メインレンダリングターゲットのスナップショット
        enum class EnMainRTSnapshot
        {
            enDrawnOpacity,     // 不透明オブジェクトの描画完了時点
            enNum,              // スナップショットの数
        };

        // レンダリングパス
        enum class EnRenderingPass
        {
            enRenderToShadowMap,    // シャドウマップへの描画パス
            enZPrepass,             // ZPrepass
            enRenderToGBuffer,      // G-Bufferへの描画パス
            enForwardRender,        // フォワードレンダリングの描画パス
        };


        RenderingEngine();//2023/11/23
        ~RenderingEngine();



        /// <summary>
        /// レンダリングパイプラインを初期化
        /// </summary>
        void Init();
        
        void DestroyObjs();

        /// <summary>
        /// シャドウマップへの描画パスにモデルを追加
        /// </summary>
        /// <param name="ligNo">シャドウマップを生成するライトの番号</param>
        /// <param name="model0">近景用のシャドウマップに描画するモデル</param>
        /// <param name="model1">中景用のシャドウマップ1に描画するモデル</param>
        /// <param name="model2">遠景用のシャドウマップ2に描画するモデル</param>
        ////void Add3DModelToRenderToShadowMap(
        ////    int ligNo,
        ////    Model& model0,
        ////    Model& model1,
        ////    Model& model2
        ////)
        ////{
        ////    m_shadowMapRenders[ligNo].Add3DModel(model0, model1, model2);
        ////}
        //void Add3DModelToRenderToShadowMap(RENDEROBJ renderobj)
        //{
        //    m_shadowmapModels.push_back(renderobj);
        //};



        /// <summary>
        /// ZPrepassの描画パスにモデルを追加
        /// </summary>
        /// <param name="model"></param>
        ////void Add3DModelToZPrepass(Model& model)
        ////{
        ////    m_zprepassModels.push_back(&model);
        ////}
        //void Add3DModelToZPrepass(RENDEROBJ renderobj)
        //{
        //    m_zprepassModels.push_back(renderobj);
        //}

        /// <summary>
        /// GBufferの描画パスにモデルを追加
        /// </summary>
        /// <param name="model"></param>
        //void Add3DModelToRenderGBufferPass(Model& model)
        //{
        //    m_renderToGBufferModels.push_back(&model);
        //}
        //void Add3DModelToRenderGBufferPass(RENDEROBJ renderobj)
        //{
        //    m_renderToGBufferModels.push_back(renderobj);
        //}

        /// <summary>
        /// フォワードレンダリングの描画パスにモデルを追加
        /// </summary>
        /// <param name="model"></param>
        //void Add3DModelToForwardRenderPass(Model& model)
        //{
        //    m_forwardRenderModels.push_back(&model);
        //}
        void Add3DModelToForwardRenderPass(std::vector<RENDEROBJ>& rendervec)
        {
            //m_forwardRenderModels.push_back(renderobj);

            size_t srcsize = m_forwardRenderModels.size();
            size_t addsize = rendervec.size();
            size_t newsize = srcsize + addsize;

            if ((newsize > 0) && (newsize > m_forwardModelsReserveSize)) {
                m_forwardRenderModels.reserve(newsize);
                m_forwardModelsReserveSize = newsize;
            }

            ////std::copy(rendervec.begin(), rendervec.end(), std::back_inserter(m_forwardRenderModels));

            //size_t addno;
            //for (addno = 0; addno < addsize; addno++) {
            //    m_forwardRenderModels[srcsize + addno] = rendervec[addno];
            //}

            size_t addno;
            for (addno = 0; addno < addsize; addno++) {
                m_forwardRenderModels.push_back(rendervec[addno]);
            }

        }
        void Add3DModelToInstancingRenderPass(RENDEROBJ renderobj)
        {
            m_instancingRenderModels.push_back(renderobj);
        }

        void AddSpriteToForwardRenderPass(RENDERSPRITE rendersprite)
        {
            m_forwardRenderSprites.push_back(rendersprite);
        }
        void AddFontToForwardRenderPass(RENDERFONT renderfont)
        {
            m_forwardRenderFont.push_back(renderfont);
        }

        /// <summary>
        /// ZPrepassで作成された深度テクスチャを取得
        /// </summary>
        /// <returns></returns>
        Texture* GetZPrepassDepthTexture()
        {
            return m_zprepassRenderTarget.GetRenderTargetTexture();
        }

        /// <summary>
        /// GBufferのアルベドテクスチャを取得
        /// </summary>
        /// <returns></returns>
        //Texture* GetGBufferAlbedoTexture()
        //{
        //    return m_gBuffer[enGBufferAlbedo].GetRenderTargetTexture();
        //}

        /// <summary>
        /// 不透明オブジェクトの描画完了時のメインレンダリングターゲットのスナップショットを取得
        /// </summary>
        /// <returns></returns>
        //Texture* GetMainRenderTargetSnapshotDrawnOpacity()
        //{
        //    return m_mainRTSnapshots[(int)EnMainRTSnapshot::enDrawnOpacity].GetRenderTargetTexture();
        //}

        /// <summary>
        /// レンダリングパイプラインを実行
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト。</param>
        void Execute(RenderContext* rc);

        /// <summary>
        /// ディレクションライトのパラメータを設定
        /// </summary>
        /// <param name="lightNo"></param>
        /// <param name="direction"></param>
        /// <param name="color"></param>
        void SetDirectionLight(int lightNo, Vector3 direction, Vector3 color)
        {
            m_deferredLightingCB.m_light.directionalLight[lightNo].direction = direction;
            m_deferredLightingCB.m_light.directionalLight[lightNo].color = color;
        }

    private:
        /// <summary>
        /// G-Bufferを初期化
        /// </summary>
        //void InitGBuffer();

        /// <summary>
        /// ディファードライティングの初期化
        /// </summary>
        void InitDeferredLighting();

        /// <summary>
        /// シャドウマップに描画
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        void RenderToShadowMap(RenderContext* rc);

        /// <summary>
        /// ZPrepass
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        void ZPrepass(RenderContext* rc);

        /// <summary>
        /// G-Bufferへの描画
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト。</param>
        //void RenderToGBuffer(RenderContext* rc);

        /// <summary>
        /// ディファードライティング
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        void DeferredLighting(RenderContext* rc);

        /// <summary>
        /// メインレンダリングターゲットの内容をフレームバッファにコピーする
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        void CopyMainRenderTargetToFrameBuffer(RenderContext* rc);

        /// <summary>
        /// フォワードレンダリング
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        void ForwardRendering(RenderContext* rc);

        /// <summary>
        /// メインレンダリングターゲットを初期化
        /// </summary>
        void InitMainRenderTarget();

        /// <summary>
        /// メインレンダリングターゲットののスナップショットを取るためのレンダリングターゲットを初期化
        /// </summary>
        //void InitMainRTSnapshotRenderTarget();

        /// <summary>
        /// メインレンダリングターゲットのカラーバッファの内容を
        /// フレームバッファにコピーするためのスプライトを初期化する
        /// </summary>
        void InitCopyMainRenderTargetToFrameBufferSprite();

        /// <summary>
        /// ZPrepass用のレンダリングターゲットを初期化
        /// </summary>
        void InitZPrepassRenderTarget();

        /// <summary>
        /// メインレンダリングターゲットのスナップショットを撮影
        /// </summary>
        //void SnapshotMainRenderTarget(RenderContext* rc, EnMainRTSnapshot enSnapshot);

        /// <summary>
        /// シャドウマップへの描画処理を初期化
        /// </summary>
        void InitShadowMapRender();

        void RenderPolyMesh(RenderContext* rc, RENDEROBJ currenderobj);
        void RenderPolyMeshZPre(RenderContext* rc, RENDEROBJ currenderobj);
        void RenderPolyMeshShadowMap(RenderContext* rc, RENDEROBJ currenderobj);
        void RenderPolyMeshShadowReciever(RenderContext* rc, RENDEROBJ currenderobj);
        void RenderPolyMeshInstancing(RenderContext* rc, RENDEROBJ currenderobj);

    private:
        //GBufferの定義
        enum EnGBuffer
        {
            enGBufferAlbedo,        // アルベド
            enGBufferNormal,        // 法線
            enGBufferWorldPos,      // ワールド座標
            enGBufferMetalSmooth,   // 金属度と滑らかさ。xに金属度、wに滑らかさが記録されている。
            enGBUfferShadowParam,   // 影パラメータ
            enGBufferNum,           // G-Bufferの数
        };

        // ディファードライティング用の定数バッファ
        struct SDeferredLightingCB
        {
            Light m_light;      // ライト
            float pad;          // パディング
            Matrix mlvp[NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
        };

        bool m_initflag;

        //shadow::ShadowMapRender m_shadowMapRenders[NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT];	//シャドウマップへの描画処理
        RenderTarget m_shadowMapRenderTarget;//2023/12/10
        //GaussianBlur m_shadowBlur;//2023/12/10
        SDeferredLightingCB m_deferredLightingCB;   // ディファードライティング用の定数バッファ
        Sprite m_copyMainRtToFrameBufferSprite;     // メインレンダリングターゲットをフレームバッファにコピーするためのスプライト
        Sprite m_diferredLightingSprite;            // ディファードライティングを行うためのスプライト
        RenderTarget m_zprepassRenderTarget;        // ZPrepass描画用のレンダリングターゲット
        RenderTarget m_mainRenderTarget;            // メインレンダリングターゲット
        //RenderTarget m_mainRTSnapshots[(int)EnMainRTSnapshot::enNum];   // メインレンダリングターゲットのスナップショット
        //RenderTarget m_gBuffer[enGBufferNum];                           // G-Buffer
        PostEffect m_postEffect;                                        // ポストエフェクト

        ////std::vector< Model* > m_zprepassModels;                         // ZPrepassの描画パスで描画されるモデルのリスト
        ////std::vector< Model* > m_renderToGBufferModels;                  // Gバッファへの描画パスで描画するモデルのリスト
        ////std::vector< Model* > m_forwardRenderModels;                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
        //std::vector<RENDEROBJ> m_zprepassModels;                         // ZPrepassの描画パスで描画されるモデルのリスト
        //std::vector<RENDEROBJ> m_shadowmapModels;
        std::vector<RENDEROBJ> m_renderToGBufferModels;                  // Gバッファへの描画パスで描画するモデルのリスト
        std::vector<RENDEROBJ> m_forwardRenderModels;                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
        size_t m_forwardModelsReserveSize;
        std::vector<RENDEROBJ> m_instancingRenderModels;                    // インスタンシングレンダリングの描画パスで描画されるモデルのリスト
        std::vector<RENDERSPRITE> m_forwardRenderSprites;
        std::vector<RENDERFONT> m_forwardRenderFont;
    };
}
