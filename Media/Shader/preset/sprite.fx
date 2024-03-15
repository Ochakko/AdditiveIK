cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
    float4 params;
    float4 dofparams;    
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

Texture2D<float4> colorTexture : register(t0); // カラーテクスチャ
//Texture2D<float4> mainRenderTargetTexture : register(t0); // メインレンダリングターゲットのテクスチャ
sampler Sampler : register(s0);

PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

float4 PSMain(PSInput In) : SV_Target0
{
    float4 color = colorTexture.Sample(Sampler, In.uv);
    //float4 color = mainRenderTargetTexture.Sample(Sampler, In.uv);
    //float4 color = { 0.0f, 0.0f, 1.0f, 1.0f };
    
    // step-3 ピクセルシェーダーから出力するαを変更する

    return color;
}
