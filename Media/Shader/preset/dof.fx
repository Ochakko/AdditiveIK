 /*!
 * @brief 被写界深度
 */

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
    float4 params;//z:TextureWidth, w:TextureHeight
    float4 dofparams; //x:dofstart, y:dofend (深度値dofparams.xからボケが始まり、深度値dofparams.yで最大のボケ具合になる。)
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

/*!
 * @brief 頂点シェーダー
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

//step-11 ボケ画像と深度テクスチャにアクセスするための変数を追加。
Texture2D<float4> bokeTexture : register(t0);  // ボケ画像
Texture2D<float4> prezdepthTexture : register(t1); // 深度テクスチャ

sampler Sampler : register(s0);

/////////////////////////////////////////////////////////
// ボケ画像書き込み用。
/////////////////////////////////////////////////////////

float4 PSMain(PSInput In) : SV_Target0
{
    // step-12 ボケ画像描き込み用のピクセルシェーダーを実装。
    // カメラ空間での深度値をサンプリング。
    float4 prezdepth = prezdepthTexture.Sample(Sampler, In.uv);

    // カメラ空間での深度値がdofparams.x以下ならピクセルキル 
    //      -> ボケ画像を描きこまない。
    //clip(prezdepth.z - dofparams.x);
    //float testth = 1.0f / 2000.0f;
    //float testth = 0.5f;
    //float testth = 500.0f / 2000.0f;
    clip(prezdepth.z - dofparams.x);

  
    // ボケ画像をサンプリング。
    float4 boke = bokeTexture.Sample(Sampler, In.uv);

    // 深度値から不透明度を計算する。
    // 深度値dofparams.xからボケが始まり、深度値dofparams.yで最大のボケ具合になる。
    //  -> つまり、深度値dofparams.yで不透明度が1になる。
    boke.a = min(1.0f, max(0.0f, ((prezdepth.z - dofparams.x) / (dofparams.y - dofparams.x))));
    //boke.a = min(1.0f, max(0.0f, ((prezdepth.z - testth) / testth)));

    
    // ボケ画像を出力。
    return boke;
}
