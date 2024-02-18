///////////////////////////////////////////////////
// 定数
///////////////////////////////////////////////////
//static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_DIRECTIONAL_LIGHT = 8; // ディレクションライトの最大本数
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

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos          : SV_POSITION;
    float4 normal       : NORMAL;    
    float2 uv           : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
};

struct SPSInShadowMap
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float2 depth : TEXCOORD1; // ライト空間での座標
};

struct SPSInShadowReciever
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
    
    // ライトビュースクリーン空間での座標を追加
    float4 posInLVP : TEXCOORD2; // ライトビュースクリーン空間でのピクセルの座標
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

cbuffer ModelCbMatrix : register(b1)
{
    uniform int4 lightsnum;
	//lightsnum.x :有効なライトの数(値をセットしてあるライトの数)
	//toonlightdir :トゥーンライトインデックス(有効なライトだけ格納したシェーダ定数内のインデックス)
	//lightsnum.z :シャドウライトインデックス(有効なライトだけ格納したシェーダ定数内のインデックス)
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    float4 eyePos; // カメラの視点
    float4 specPow; // スペキュラの絞り
    //float4 ambientLight; // 環境光
    float4 toonlightdir;
    float4x4 mBoneMat[1000];
};

// step-8 影用のパラメータにアクセスする定数バッファーを定義
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
sampler g_sampler_clamp : register(s4); //2024/02/14
sampler g_sampler_shadow : register(s5);


float4 CalcDiffuseColor(float multiplecoef, float3 meshnormal, float3 lightdir)
{
    float3 normaly0 = normalize(float3(meshnormal.x, 0.0f, meshnormal.z));
    float3 lighty0 = normalize(float3(lightdir.x, 0.0f, lightdir.z));
    float nl;
    nl = dot(normaly0, lighty0);
    float toonh = (nl + 1.0f) * 0.5f * multiplecoef;
    float2 diffuseuv = { 0.5f, toonh };
    float4 diffusecol = g_diffusetex.Sample(g_sampler_clamp, diffuseuv) * materialdisprate.x;
    
    return diffusecol;
}

/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント
/// </summary>
SPSIn VSMainSkinStd(SVSIn vsIn, uniform bool hasSkin)
{
    SPSIn psIn;

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
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);
    //psIn.pos /= psIn.pos.w;
    
    psIn.normal = normalize(mul(finalmat, vsIn.normal));    
    psIn.uv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    
    psIn.diffusemult = diffusemult;
    
    return psIn;
}

SPSInShadowMap VSMainSkinStdShadowMap(SVSIn vsIn, uniform bool hasSkin)
{
    SPSInShadowMap psIn;

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
    float4 worldPos = psIn.pos / psIn.pos.w;
    //float4 worldPos = psIn.pos;
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);
    //psIn.pos /= psIn.pos.w;
    
    // step-9 頂点のライトから見た深度値と、ライトから見た深度値の2乗を計算する
    psIn.depth.x = length(worldPos.xyz - lightPos.xyz) / shadowmaxz.x;
    //float4 posLVP = mul(mLVP, worldPos);
    //psIn.depth.x = posLVP.z / posLVP.w;
    psIn.depth.y = psIn.depth.x * psIn.depth.x;
        
    //psIn.normal = normalize(mul(finalmat, vsIn.normal));
    psIn.uv = vsIn.uv.xy;

    //psIn.diffusemult = diffusemult;
    
    return psIn;
}

SPSInShadowReciever VSMainSkinStdShadowReciever(SVSIn vsIn, uniform bool hasSkin)
{
    SPSInShadowReciever psIn;

    //float4 wPos;
    int bi[4] = { vsIn.bindices.r, vsIn.bindices.g, vsIn.bindices.b, vsIn.bindices.a };
    float bw[4] = { vsIn.bweight.x, vsIn.bweight.y, vsIn.bweight.z, vsIn.bweight.w };
    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        matrix addmat = mBoneMat[bi[i]];
        finalmat += (addmat * bw[i]);
    }
	
    float4 worldPos = mul(finalmat, vsIn.pos);
    worldPos /= worldPos.w;
    psIn.pos = mul(mView, worldPos);    
    psIn.pos = mul(mProj, psIn.pos);

    // ライトビュースクリーン空間の座標を計算する
    psIn.posInLVP = mul(mLVP, worldPos);
    //psIn.posInLVP /= psIn.posInLVP.w;
    //psIn.posInLVP.xy = psIn.pos.xy;
    //psIn.posInLVP.w = psIn.pos.w;    
    // step-12 頂点のライトから見た深度値を計算する
    psIn.posInLVP.z = length(worldPos.xyz - lightPos.xyz) / shadowmaxz.x;
    
    psIn.normal = normalize(mul(finalmat, vsIn.normal));
    psIn.uv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;

    psIn.diffusemult = diffusemult;
   
    return psIn;
}

/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>

float4 PSMainSkinStd(SPSIn psIn) : SV_Target0
{
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    //float4 diffusecol = CalcDiffuseColor(psIn.normal, toonlightdir);
     
    float3 wPos = psIn.pos.xyz / psIn.pos.w;
    
    float3 totaldiffuse = float3(0, 0, 0);
    float3 totalspecular = float3(0, 0, 0);
    float calcpower = POW * 0.05f; //!!!!!!!!!!!
    float3 lig = 0;
    for (int ligNo = 0; ligNo < lightsnum.x; ligNo++)
    {
        float nl;
        float3 h;
        float nh;
        float4 tmplight;
		
        nl = dot(psIn.normal.xyz, directionalLight[ligNo].direction.xyz);
        h = normalize((directionalLight[ligNo].direction.xyz + eyePos.xyz - wPos) * 0.5f);
        nh = dot(psIn.normal.xyz, h);
    
        float multiplecoef = materialdisprate.x;
        float4 diffusecol = CalcDiffuseColor(multiplecoef, psIn.normal.xyz, directionalLight[ligNo].direction.xyz);
        totaldiffuse += directionalLight[ligNo].color.xyz * diffusecol.xyz;
        totalspecular += ((nl) < 0) || ((nh) < 0) ? 0 : ((nh) * calcpower);
    }
    float4 totaldiffuse4 = float4(totaldiffuse, materialdisprate.x);
    float4 totalspecular4 = float4(totalspecular, 0.0f) * materialdisprate.y * 0.125f;//ライト８個で白飛びしないように応急処置1/8=0.125
    float4 pscol = emission * materialdisprate.z + albedocol * psIn.diffusemult * totaldiffuse4 + totalspecular4;
    return pscol;
}

float4 PSMainSkinStdShadowMap(SPSInShadowMap psIn) : SV_Target0
{
    return float4(psIn.depth.x, psIn.depth.y, 0.0f, 1.0f);
}


float4 PSMainSkinStdShadowReciever(SPSInShadowReciever psIn) : SV_Target0
{
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    //float4 diffusecol = CalcDiffuseColor(psIn.normal.xyz, toonlightdir.xyz);
     
    float3 wPos = psIn.pos.xyz / psIn.pos.w;
    
    float3 totaldiffuse = float3(0, 0, 0);
    float3 totalspecular = float3(0, 0, 0);
    float calcpower = POW * 0.05f; //!!!!!!!!!!!
    float3 lig = 0;
    for (int ligNo = 0; ligNo < lightsnum.x; ligNo++)
    {
        float nl;
        float3 h;
        float nh;
        float4 tmplight;
		
        nl = dot(psIn.normal.xyz, directionalLight[ligNo].direction.xyz);
        h = normalize((directionalLight[ligNo].direction.xyz + eyePos.xyz - wPos) * 0.5f);
        nh = dot(psIn.normal.xyz, h);
    
        float multiplecoef = materialdisprate.x;
        float4 diffusecol = CalcDiffuseColor(multiplecoef, psIn.normal.xyz, directionalLight[ligNo].direction.xyz);
        totaldiffuse += directionalLight[ligNo].color.xyz * diffusecol.xyz;
        totalspecular += ((nl) < 0) || ((nh) < 0) ? 0 : ((nh) * calcpower);
    }
    float4 totaldiffuse4 = float4(totaldiffuse, materialdisprate.x);
    float4 totalspecular4 = float4(totalspecular, 0.0f) * materialdisprate.y * 0.125f; //ライト８個で白飛びしないように応急処置1/8=0.125
    float4 pscol = emission * materialdisprate.z + albedocol * psIn.diffusemult * totaldiffuse4 + totalspecular4;

/////////
    // ライトビュースクリーン空間からUV空間に座標変換
    float2 shadowMapUV = psIn.posInLVP.xy / psIn.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    // ライトビュースクリーン空間でのZ値を計算する
    float zInLVP = psIn.posInLVP.z;
    float2 shadowValue = g_shadowMap.Sample(g_sampler_shadow, shadowMapUV).xy;
    pscol.xyz *= ((shadowMapUV.x > 0.0f) && (shadowMapUV.x < 1.0f) && (shadowMapUV.y > 0.0f) && (shadowMapUV.y < 1.0f) && ((zInLVP - shadowmaxz.y) > shadowValue.r) && (zInLVP <= 1.0f)) ? shadowmaxz.z : 1.0f;

    
    //if ((shadowMapUV.x > 0.0f) && (shadowMapUV.x < 1.0f)
    //    && (shadowMapUV.y > 0.0f) && (shadowMapUV.y < 1.0f))
    //{
    //    // step-13 シャドウレシーバーに影を落とす
    //    // シャドウマップから値をサンプリング
    //    float2 shadowValue = g_shadowMap.Sample(g_sampler, shadowMapUV).xy;

    //    // まずこのピクセルが遮蔽されているか調べる。これは通常のデプスシャドウと同じ
    //    if ((zInLVP > shadowValue.r) && (zInLVP <= 1.0f))
    //    {
    //        // 遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める
    //        float depth_sq = shadowValue.x * shadowValue.x;

    //        // このグループの分散具合を求める
    //        // 分散が大きいほど、varianceの数値は大きくなる
    //        float variance = min(max(shadowValue.y - depth_sq, 0.0001f), 1.0f);

    //        // このピクセルのライトから見た深度値とシャドウマップの平均の深度値の差を求める
    //        float md = zInLVP - shadowValue.x;

    //        // 光が届く確率を求める
    //        float lit_factor = variance / (variance + md * md);

    //        // シャドウカラーを求める
    //        float3 shadowColor = pscol.xyz * 0.5f;

    //        // 光が当たる確率を使って通常カラーとシャドウカラーを線形補完
    //        pscol.xyz = lerp(shadowColor, pscol.xyz, lit_factor);
    //    }
    //}

    return pscol;
}



float4 PSMainSkinNoLight(SPSIn psIn) : SV_Target0
{
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    float4 diffusecol = CalcDiffuseColor(1.0f, psIn.normal.xyz, toonlightdir.xyz);
    float4 pscol = emission * materialdisprate.z + albedocol * diffusecol * psIn.diffusemult;
    return pscol;
}

float4 PSMainSkinNoLightShadowReciever(SPSInShadowReciever psIn) : SV_Target0
{
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    float4 diffusecol = CalcDiffuseColor(1.0f, psIn.normal.xyz, toonlightdir.xyz);      
    float4 pscol = emission * materialdisprate.z + albedocol * diffusecol * psIn.diffusemult;
//////////
    // ライトビュースクリーン空間からUV空間に座標変換
    float2 shadowMapUV = psIn.posInLVP.xy / psIn.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    // ライトビュースクリーン空間でのZ値を計算する
    float zInLVP = psIn.posInLVP.z;
    float2 shadowValue = g_shadowMap.Sample(g_sampler_shadow, shadowMapUV).xy;
    pscol.xyz *= ((shadowMapUV.x > 0.0f) && (shadowMapUV.x < 1.0f) && (shadowMapUV.y > 0.0f) && (shadowMapUV.y < 1.0f) && ((zInLVP - shadowmaxz.y) > shadowValue.r) && (zInLVP <= 1.0f)) ? shadowmaxz.z : 1.0f;

    //if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
    //    && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    //{
    //    // step-13 シャドウレシーバーに影を落とす
    //    // シャドウマップから値をサンプリング
    //    float2 shadowValue = g_shadowMap.Sample(g_sampler, shadowMapUV).xy;

    //    // まずこのピクセルが遮蔽されているか調べる。これは通常のデプスシャドウと同じ
    //    if (zInLVP > shadowValue.r && zInLVP <= 1.0f)
    //    {
    //        //// 遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める
    //        //float depth_sq = shadowValue.x * shadowValue.x;

    //        //// このグループの分散具合を求める
    //        //// 分散が大きいほど、varianceの数値は大きくなる
    //        //float variance = min(max(shadowValue.y - depth_sq, 0.0001f), 1.0f);

    //        //// このピクセルのライトから見た深度値とシャドウマップの平均の深度値の差を求める
    //        //float md = zInLVP - shadowValue.x;

    //        //// 光が届く確率を求める
    //        //float lit_factor = variance / (variance + md * md);

    //        //// シャドウカラーを求める
    //        //float3 shadowColor = pscol.xyz * 0.5f;

    //        //// 光が当たる確率を使って通常カラーとシャドウカラーを線形補完
    //        //pscol.xyz = lerp(shadowColor, pscol.xyz, lit_factor);
            
    //        pscol.xyz = float3(0.0f, 0.0f, 0.0f);

    //    }
    //}
    
    
    
    
    return pscol;
}

