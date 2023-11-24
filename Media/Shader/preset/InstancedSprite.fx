cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
    float4 params;
};

struct VSInput
{
    float4 pos : POSITION0;
    float2 uv  : TEXCOORD0;
    float4 disppos : POSITION1;
    float2 dispsize : POSITION2;
    float2 wh : POSITION3;
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
    //psIn.pos = mul(mvp, In.pos);
    //psIn.uv = In.uv;
    
    //    m_world._11 = dispsizex / (float)screenW;
    //    m_world._22 = dispsizey / (float)screenH;
    //    m_world._33 = 0.0f;
    //    m_world._41 = (srcpos.x / (float)screenW * 2.0f - 1.0f);
    //    m_world._42 = -(srcpos.y / (float)screenH * 2.0f - 1.0f);
    //    m_world._43 = srcpos.z;
    matrix wmat;
    wmat[0][0] = In.dispsize.x / In.wh.x;
    wmat[0][1] = 0.0f;
    wmat[0][2] = 0.0f;
    wmat[0][3] = 0.0f;
    wmat[1][0] = 0.0f;
    wmat[1][1] = In.dispsize.y / In.wh.y;
    wmat[1][2] = 0.0f;
    wmat[1][3] = 0.0f;
    wmat[2][0] = 0.0f;
    wmat[2][1] = 0.0f;
    wmat[2][2] = 0.0f;
    wmat[2][3] = 0.0f;
    wmat[3][0] = In.disppos.x / In.wh.x * 2.0f - 1.0f;
    wmat[3][1] = -(In.disppos.y / In.wh.y * 2.0f - 1.0f);
    wmat[3][2] = In.disppos.z;
    wmat[3][3] = 1.0f;
 
    
    //psIn.pos = mul(wmat, In.pos);
    psIn.pos = mul(In.pos, wmat);//転置　引数順に注意
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
