///////////////////////////////////////////
// 構造体
///////////////////////////////////////////
// 頂点シェーダーへの入力
struct SVSIn
{
    float4 pos      : POSITION;
    float4 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos          : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float4 posInProj    : TEXCOORD1;
};

///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
cbuffer ModelCb : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

float4x4 fixView = { 
        {-1.0f, 0.0f, 0.0f, 0.0f }, 
        { 0.0f, 1.0f, 0.0f, 0.0f}, 
        { 0.0f, 0.0f, -1.0f, 0.0f}, 
        { 0.0f, -81.5f, 322.0f, 1.0f} };
float4x4 fixProj = {
    { 1.14842f, 0.0f, 0.0f, 0.0f},
    { 0.0f, 1.73205f, 0.0f, 0.0f},
    { 0.0f, 0.0f, 1.0f, 1.0f},
    { 0.0f, 0.0f, -1.0922f, 0.0f}    
};


///////////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////////
// モデルテクスチャ
Texture2D<float4> g_texture : register(t0);

// step-3 深度テクスチャにアクセスするための変数を追加
//Texture2D<float4> g_depthTexture : register(t10);

///////////////////////////////////////////
// サンプラーステート
///////////////////////////////////////////
sampler g_sampler : register(s0);

/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント
/// </summary>
SPSIn VSMain(SVSIn vsIn, uniform bool hasSkin)
{
    SPSIn psIn;

    psIn.pos = mul(mWorld, vsIn.pos);   // モデルの頂点をワールド座標系に変換
    psIn.pos = mul(mView, psIn.pos);    // ワールド座標系からカメラ座標系に変換
    psIn.pos = mul(mProj, psIn.pos);    // カメラ座標系からスクリーン座標系に変換
    //psIn.pos = mul(mWorld, vsIn.pos); // モデルの頂点をワールド座標系に変換
    //psIn.pos = mul(fixView, vsIn.pos); // ワールド座標系からカメラ座標系に変換
    //psIn.pos = mul(fixProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
    psIn.uv = vsIn.uv;

    //step-4 頂点の正規化スクリーン座標系の座標をピクセルシェーダーに渡す
    psIn.posInProj = psIn.pos;
    psIn.posInProj.xy /= psIn.posInProj.w;

    return psIn;
}

/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
    // 普通にテクスチャを
    //return g_texture.Sample(g_sampler, psIn.uv);
    float4 texcol = g_texture.Sample(g_sampler, psIn.uv);
    texcol.w = 1.0f;
    return texcol;
      
    //return float4(psIn.uv, 1, 1);
    
    
    //float4 testcol = {0.5f, 0.5f, 0.5f, 1.0f};
    //float4 testcol = { 1.0f, 1.0f, 1.0f, 1.0f };
    //return testcol;

}
