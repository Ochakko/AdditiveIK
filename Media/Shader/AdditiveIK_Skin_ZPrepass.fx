///////////////////////////////////////////////////
// 定数
///////////////////////////////////////////////////
//static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_DIRECTIONAL_LIGHT = 8; // ディレクションライトの本数
static const float PI = 3.1415926f; // π
//static const float POW = 15.0;
//static const float POW = 0.2f;
static const float POW = 5.0f;

///////////////////////////////////////////
// 構造体
///////////////////////////////////////////
// 頂点シェーダーへの入力
struct SVSIn
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 biNormal : BINORMAL;
    float4 uv : TEXCOORD0;
    float4 bweight : BLENDWEIGHT;
    int4 bindices : BLENDINDICES;
};

struct SPSInZPrepass
{
    float4 pos : SV_POSITION; //座標。
    float4 depth : POSITION1; //深度値。xにはプロジェクション空間、yにはカメラ空間での正規化されたZ値、zにはカメラ空間でのZ値
    float2 uv : TEXCOORD0;
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
    float4 diffusemult;
    float4 ambient0;//ambient0.wはAlphaTestの閾値
    float4 emission;
    float4 metalcoef;
    float4 materialdisprate;
    float4 shadowmaxz;
    int4 UVs;
};
// ディレクションライト
struct DirectionalLight
{
    float4 direction; // ライトの方向
    float4 color; // ライトの色
};

cbuffer ModelCbMatrix : register(b1)
{
    uniform int4 lightsnum;
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    float4 eyePos; // カメラの視点
    float4 specPow; // スペキュラの絞り
    //float4 ambientLight; // 環境光    
    float4 toonlightdir;
    float4 vFog;
    float4 vFogColor;    
    float4x4 mBoneMat[1000];
};

///////////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////////
// モデルテクスチャ
Texture2D<float4> g_diffusetex : register(t0);
Texture2D<float4> g_albedo : register(t1); // アルベドマップ
Texture2D<float4> g_normalMap : register(t2); // 法線マップ
Texture2D<float4> g_metallicSmoothMap : register(t3); // メタリックスムースマップ。rにメタリック、aにスムース
Texture2D<float4> g_shadowMap : register(t4);
// サンプラーステート
sampler g_sampler : register(s0);
sampler g_sampler_albedo : register(s1);
sampler g_sampler_normal : register(s2);
sampler g_sampler_metal : register(s3);
sampler g_sampler_clamp : register(s4); //2024/02/14
sampler g_sampler_shadow : register(s5);


//########
//シェーダ
//########
SPSInZPrepass VSMainZPrepass(SVSIn vsIn, uniform bool hasSkin)
{
    SPSInZPrepass psIn;

    //float4 wPos;
    int bi[4] = { vsIn.bindices.r, vsIn.bindices.g, vsIn.bindices.b, vsIn.bindices.a };
    float bw[4] = { vsIn.bweight.x, vsIn.bweight.y, vsIn.bweight.z, vsIn.bweight.w };
    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        matrix addmat = mBoneMat[bi[i]];
        finalmat += (addmat * bw[i]);
    }
	psIn.pos = mul(finalmat, vsIn.pos);

    float3 distvec = (psIn.pos.xyz / psIn.pos.w) - eyePos.xyz;
    psIn.depth.z = length(distvec);
    psIn.depth.z = clamp(psIn.depth.z, 0.0f, 250000.0f);
    
    psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
    psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
    psIn.depth.x = psIn.pos.z / psIn.pos.w;
    psIn.depth.y = saturate(psIn.pos.w / 1000.0f);
    psIn.depth.w = 1.0f;
    
    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float) UVs.y;
    psIn.uv.y = orguv.y * (float) UVs.z;
    
    return psIn;
}

float4 PSMainZPrepass(SPSInZPrepass psIn) : SV_Target0
{
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    
    float2 diffuseuv = { 0.5f, 0.5f };//処理を軽くするために簡略計算
    float4 diffusecol = g_diffusetex.Sample(g_sampler_clamp, diffuseuv) * materialdisprate.x;
    
    clip((albedocol.w * diffusecol.w) - ambient0.w); //2024/03/24 アルファテスト　ambient.w * diffuseco.wより小さいアルファは書き込まない
    
    return float4(psIn.depth.x, psIn.depth.y, psIn.depth.z, 1.0f);
}




