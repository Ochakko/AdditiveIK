///////////////////////////////////////////////////
// 定数
///////////////////////////////////////////////////
//static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_DIRECTIONAL_LIGHT = 2; // ディレクションライトの本数
static const float PI = 3.1415926f; // π
//static const float POW = 15.0;
//static const float POW = 0.2f;
static const float POW = 5.0f;

///////////////////////////////////////////
// 構造体
///////////////////////////////////////////
// 頂点シェーダーへの入力
struct SVSInWithoutBone
{
    float4 pos      : POSITION;
    float4 normal   : NORMAL;
    float4 tangent : TANGENT;
    float4 biNormal : BINORMAL; 
    float2 uv       : TEXCOORD0;
};

struct SVSInExtLine
{
    float4 pos : POSITION;
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos          : SV_POSITION;
    float4 normal       : NORMAL;
    float2 uv           : TEXCOORD0;
    float4 posInProj    : TEXCOORD1;
    float4 diffusemult : TEXCOORD2;
};

struct SPSInExtLine
{
    float4 pos : SV_POSITION;
    float4 diffusemult : COLOR0;
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
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    float4 eyePos; // カメラの視点
    float4 specPow; // スペキュラの絞り
    float4 ambientLight; // 環境光
};

///////////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////////
// モデルテクスチャ
Texture2D<float4> g_diffusetex : register(t0);
Texture2D<float4> g_albedo : register(t1); // アルベドマップ
Texture2D<float4> g_normalMap : register(t2); // 法線マップ
Texture2D<float4> g_metallicSmoothMap : register(t3); // メタリックスムースマップ。rにメタリック、aにスムース
// サンプラーステート
sampler g_sampler : register(s0);

/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント
/// </summary>
SPSIn VSMainNoSkinStd(SVSInWithoutBone vsIn, uniform bool hasSkin)
{
    SPSIn psIn;

    psIn.pos = mul(mWorld, vsIn.pos);   // モデルの頂点をワールド座標系に変換
    psIn.pos = mul(mView, psIn.pos);    // ワールド座標系からカメラ座標系に変換
    psIn.pos = mul(mProj, psIn.pos);    // カメラ座標系からスクリーン座標系に変換
    psIn.uv = vsIn.uv;

    //step-4 頂点の正規化スクリーン座標系の座標をピクセルシェーダーに渡す
    psIn.posInProj = psIn.pos;
    psIn.posInProj.xy /= psIn.posInProj.w;
    
    psIn.diffusemult = diffusemult;
    
    psIn.normal = normalize(mul(mWorld, vsIn.normal));
    
    
    return psIn;
}

SPSInExtLine VSMainExtLine(SVSInExtLine vsIn, uniform bool hasSkin)
{
    SPSInExtLine psIn;

    psIn.pos = mul(mWorld, vsIn.pos); // モデルの頂点をワールド座標系に変換
    psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
    psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換

    psIn.diffusemult = diffusemult;
    
    return psIn;
}



/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>
float4 PSMainNoSkinStd(SPSIn psIn) : SV_Target0
{
    // 普通にテクスチャを
    //return g_texture.Sample(g_sampler, psIn.uv);
    float4 albedocol = g_albedo.Sample(g_sampler, psIn.uv);
    float2 diffuseuv = { 0.5f, 0.5f };
    float4 diffusecol = g_diffusetex.Sample(g_sampler, diffuseuv);
    //texcol.w = 1.0f;
    //return texcol;
     
    float3 wPos = psIn.pos.xyz / psIn.pos.w;
    
    float3 totaldiffuse = float3(0, 0, 0);
    float3 totalspecular = float3(0, 0, 0);
    float calcpower = POW * 0.05f;//!!!!!!!!!!!
    float3 lig = 0;
    //for (int ligNo = 0; ligNo < NUM_DIRECTIONAL_LIGHT; ligNo++)
    for (int ligNo = 0; ligNo < 1; ligNo++)//!!!!!!!!!!!!!!!!!!!!!
    {
        float nl;
        float3 h;
        float nh;
        float4 tmplight;
		
        nl = dot(psIn.normal.xyz, -directionalLight[ligNo].direction.xyz);
        h = normalize((-directionalLight[ligNo].direction.xyz + eyePos.xyz - wPos) * 0.5f);
        nh = dot(psIn.normal.xyz, h);

        totaldiffuse += directionalLight[ligNo].color.xyz * max(0, dot(psIn.normal.xyz, -directionalLight[ligNo].direction.xyz));
        totalspecular += ((nl) < 0) || ((nh) < 0) ? 0 : ((nh) * calcpower);
    }
    float4 totaldiffuse4 = float4(totaldiffuse, 1.0f);
    float4 totalspecular4 = float4(totalspecular, 0.0f);
    float4 pscol = albedocol * diffusecol * psIn.diffusemult * totaldiffuse4 + totalspecular4;
    return pscol;
}

float4 PSMainNoSkinNoLight(SPSIn psIn) : SV_Target0
{
    // 普通にテクスチャを
    //return g_texture.Sample(g_sampler, psIn.uv);
    float4 albedocol = g_albedo.Sample(g_sampler, psIn.uv);
    float2 diffuseuv = { 0.5f, 0.5f };
    float4 diffusecol = g_diffusetex.Sample(g_sampler, diffuseuv);
    //texcol.w = 1.0f;
    //return texcol;
      
    float4 pscol = albedocol * diffusecol * psIn.diffusemult;
    return pscol;
}

float4 PSMainExtLine(SPSInExtLine psIn) : SV_Target0
{
    float4 pscol = psIn.diffusemult;
    //float4 pscol = { 1.0f, 1.0f, 1.0f, 1.0f };
    return pscol;
}