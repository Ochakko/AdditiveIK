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
struct SVSInInstancing
{
    float4 pos      : POSITION;
    float4 normal   : NORMAL;
    float4 tangent : TANGENT;
    float4 biNormal : BINORMAL; 
    float4 uv       : TEXCOORD0;
    float4 wmat0 : TEXCOORD1;
    float4 wmat1 : TEXCOORD2;
    float4 wmat2 : TEXCOORD3;
    float4 wmat3 : TEXCOORD4;
    float4 vpmat0 : TEXCOORD5;
    float4 vpmat1 : TEXCOORD6;
    float4 vpmat2 : TEXCOORD7;
    float4 vpmat3 : TEXCOORD8;
    float4 material : COLOR0;
    float4 scaleinsta : TEXCOORD9;
    float4 offsetinsta : TEXCOORD10;
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos          : SV_POSITION;
    float4 normal       : NORMAL;
    float2 uv           : TEXCOORD0;
    float4 diffusemult  : TEXCOORD1;
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
    float4 ambient;
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

// ライト用の定数バッファー
cbuffer LightCb : register(b1)
{
    uniform int4 lightsnum;
   	//lightsnum.x : 有効なライトの数(値をセットしてあるライトの数)
    //lightsnum.y : lightflag
    //lightsnum.z : 未使用
    //lightsnum.w : normalY0flag 
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    float4 eyePos; // カメラの視点
    float4 specPow; // スペキュラの絞り
    //float4 ambientLight; // 環境光
    float4 toonlightdir;
    float4 vFog;
    float4 vFogColor;    
};

cbuffer ShadowParamCb : register(b2)
{
    float4x4 mLVP; // ライトビュープロジェクション行列
    float4 lightPos; // ライトの座標
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
sampler g_sampler_shadow : register(s4);


/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント
/// </summary>
SPSIn VSMainNoSkinInstancing(SVSInInstancing vsIn, uniform bool hasSkin)
{
    SPSIn psIn;

    float4x4 wmat;
    wmat._11_12_13_14 = vsIn.wmat0;
    wmat._21_22_23_24 = vsIn.wmat1;
    wmat._31_32_33_34 = vsIn.wmat2;
    wmat._41_42_43_44 = vsIn.wmat3;
    float4x4 vpmat;
    vpmat._11_12_13_14 = vsIn.vpmat0;
    vpmat._21_22_23_24 = vsIn.vpmat1;
    vpmat._31_32_33_34 = vsIn.vpmat2;
    vpmat._41_42_43_44 = vsIn.vpmat3;

    float4 scalepos;
    //scalepos.x = (vsIn.pos.x - vsIn.offsetinsta.x) * vsIn.scaleinsta.x + vsIn.offsetinsta.x;
    //scalepos.y = (vsIn.pos.y - vsIn.offsetinsta.y) * vsIn.scaleinsta.y + vsIn.offsetinsta.y;
    //scalepos.z = (vsIn.pos.z - vsIn.offsetinsta.z) * vsIn.scaleinsta.z + vsIn.offsetinsta.z;
    scalepos.xyz = (vsIn.pos.xyz - vsIn.offsetinsta.xyz) * vsIn.scaleinsta.xyz + vsIn.offsetinsta.xyz;
    scalepos.w = 1.0f;
    
    //psIn.pos = mul(wmat, vsIn.pos);
    //psIn.pos = mul(vpmat, psIn.pos);
    //psIn.pos = mul(mWorld, vsIn.pos);//剛体のscalematが入っている
    psIn.pos = mul(scalepos, wmat);
    psIn.pos = mul(psIn.pos, vpmat);
    psIn.diffusemult = diffusemult * vsIn.material;
    
    psIn.normal = normalize(mul(wmat, vsIn.normal));

    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float) UVs.y;
    psIn.uv.y = orguv.y * (float) UVs.z;

    
    return psIn;
}

float4 PSMainNoSkinInstancingNoLight(SPSIn psIn) : SV_Target0
{
    // 普通にテクスチャを
    //return g_texture.Sample(g_sampler, psIn.uv);
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    clip(albedocol.w - 0.0314f); //2024/03/17 アルファテスト　0x08より小さいアルファは書き込まない

    float2 diffuseuv = { 0.5f, 0.5f };
    float4 diffusecol = g_diffusetex.Sample(g_sampler_albedo, diffuseuv) * materialdisprate.x;
    //texcol.w = 1.0f;
    //return texcol;
      
    float4 pscol = emission * materialdisprate.z + albedocol * diffusecol * psIn.diffusemult;
    return pscol;
}

