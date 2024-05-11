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
    float4 projpos : POSITIONT;    
    float4 bweight : BLENDWEIGHT;
    int4 bindices : BLENDINDICES;
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION; // スクリーン空間でのピクセルの座標
    float4 normal : NORMAL; // 法線
    float4 tangent : TANGENT;
    float4 biNormal : BINORMAL;
    float2 uv : TEXCOORD0; // uv座標
    float4 worldPos : TEXCOORD1; // ワールド空間でのピクセルの座標
    float4 diffusemult : TEXCOORD2;
    float4 FogAndOther : TEXCOORD3; //x:Fog
    float4 depth : TEXCOORD4;
};

struct SPSInShadowMap
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 depth : TEXCOORD1; // ライト空間での座標
    //float4 zpredepth : TEXCOORD2;
};


struct SPSInShadowReciever
{
    float4 pos : SV_POSITION; // スクリーン空間でのピクセルの座標
    float4 normal : NORMAL; // 法線
    float4 tangent : TANGENT;
    float4 biNormal : BINORMAL;
    float2 uv : TEXCOORD0; // uv座標
    float4 worldPos : TEXCOORD1; // ワールド空間でのピクセルの座標
    float4 diffusemult : TEXCOORD2;
  
    // ライトビュースクリーン空間での座標を追加
    float4 posInLVP : TEXCOORD3; // ライトビュースクリーン空間でのピクセルの座標        
    float4 FogAndOther : TEXCOORD4; //x:Fog
    float4 depth : TEXCOORD5;
};

struct SPSOut0
{
    float4 color_0 : SV_Target0;
};
struct SPSOut1
{
    float4 color_1 : SV_Target1;
};
struct SPSOut2
{
    float4 color_0 : SV_Target0;
    float4 color_1 : SV_Target1;
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
    float4 shadowmaxz; //x:(1/shadowfar), y:shadowbias
    int4 UVs; //x:UVSet, y:TilingU, z:TilingV, w:distortionFlag   
    int4 Flags1; //x:skyflag, y:groundflag, z:skydofflag, w:VSM
    int4 Flags2; //x:grassflag  
    float4 time1; //2024/04/27
    float4 bbsize; //2024/05/11 size of bourndary        
    int4 distortiontype; //[0]:riverorsea(0:river,1:sea), [1]:maptype(0:rg,1:rb,2:gb)
    float4 distortionscale; //x:distortionscale, y:riverflowrate
    float4 distortioncenter; //xy:seacenter, zw:riverdir    
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
	//lightsnum.x : 有効なライトの数(値をセットしてあるライトの数)
    //lightsnum.y : lightflag
    //lightsnum.z : 未使用
    //lightsnum.w : normalY0flag
    float4 divlights; //x:(1/lightsnum)    
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    float4 eyePos; // カメラの視点
    float4 specPow; // スペキュラの絞り
    //float4 ambientLight; // 環境光
    float4 toonlightdir;
    float4 vFog; //distfog{x:1/(far - near), y:1/far, z:rate}, hieghtfog{x:0.0, y:1/maxheight, z:rate}
    float4 vFogColor;
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


float CalcVSFog(float4 worldpos)
{
    worldpos /= worldpos.w;
    float4 fogpos = worldpos - eyePos;
    float fogy = worldpos.y * vFog.y;
    float fog = (vFog.w < 1.1f) ? (vFog.z * length(fogpos.xyz) * vFog.x) : (vFog.z - vFog.z * fogy * fogy);
    return fog;
}
float4 CalcPSFog(float4 pscol, float fog)
{
    float3 fogcolor = vFogColor.xyz;
    float fograte = max(0.0f, min(1.0f, fog));
    float3 outcolor = lerp(pscol.xyz, fogcolor, fograte);
    return float4(outcolor, pscol.w);
}




float4 CalcDiffuseColor(float multiplecoef, float3 meshnormal, float3 lightdir)
{
    float3 normaly0 = (lightsnum.w == 1) ? normalize(float3(meshnormal.x, 0.0f, meshnormal.z)) : meshnormal;
    float3 lighty0 = (lightsnum.w == 1) ? normalize(float3(lightdir.x, 0.0f, lightdir.z)) : lightdir;
    float nl;
    nl = dot(normaly0, lighty0);
    float toonh = (nl + 1.0f) * 0.5f * multiplecoef;
    float2 diffuseuv = { 0.5f, toonh };
    float4 diffusecol = g_diffusetex.Sample(g_sampler_clamp, diffuseuv) * materialdisprate.x;
    
    return diffusecol;
}


//float3 GetNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
//{
//    float3 binSpaceNormal = g_normalMap.SampleLevel(g_sampler, uv, 0.0f).xyz;
//    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;
//    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;
//    return newNormal;
//}

//// ベックマン分布を計算する
//float Beckmann(float m, float t)
//{
//    float t2 = t * t;
//    float t4 = t * t * t * t;
//    float m2 = m * m;
//    float D = 1.0f / (4.0f * m2 * t4);
//    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
//    return D;
//}

//// フレネルを計算。Schlick近似を使用
//float SpcFresnel(float f0, float u)
//{
//    // from Schlick
//    return f0 + (1 - f0) * pow(1 - u, POW);
//}

///// <summary>
///// Cook-Torranceモデルの鏡面反射を計算
///// </summary>
///// <param name="L">光源に向かうベクトル</param>
///// <param name="V">視点に向かうベクトル</param>
///// <param name="N">法線ベクトル</param>
///// <param name="metallic">金属度</param>
//float CookTorranceSpecular(float3 L, float3 V, float3 N, float metallic)
//{
//    float microfacet = 0.76f;

//    // 金属度を垂直入射の時のフレネル反射率として扱う
//    // 金属度が高いほどフレネル反射は大きくなる
//    float f0 = metallic;

//    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
//    float3 H = normalize(L + V);

//    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
//    float NdotH = saturate(dot(N, H));
//    float VdotH = saturate(dot(V, H));
//    float NdotL = saturate(dot(N, L));
//    float NdotV = saturate(dot(N, V));

//    // D項をベックマン分布を用いて計算する
//    float D = Beckmann(microfacet, NdotH);

//    // F項をSchlick近似を用いて計算する
//    float F = SpcFresnel(f0, VdotH);

//    // G項を求める
//    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

//    // m項を求める
//    float m = PI * NdotV * NdotH;

//    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
//    return max(F * D * G / m, 0.0);
//}

///// <summary>
///// フレネル反射を考慮した拡散反射を計算
///// </summary>
///// <remark>
///// この関数はフレネル反射を考慮した拡散反射率を計算します
///// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
///// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
///// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります
/////
///// </remark>
///// <param name="N">法線</param>
///// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
///// <param name="V">視線に向かうベクトル。</param>
///// <param name="roughness">粗さ。0～1の範囲。</param>
//float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
//{
//    // step-1 ディズニーベースのフレネル反射による拡散反射を真面目に実装する。
//    // 光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
//    float3 H = normalize(L + V);

//    // 粗さは0.5で固定。
//    float roughness = 0.5f;

//    float energyBias = lerp(0.0f, 0.5f, roughness);
//    float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

//    // 光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める
//    float dotLH = saturate(dot(L, H));

//    // 光源に向かうベクトルとハーフベクトル、
//    // 光が平行に入射したときの拡散反射量を求めている
//    float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

//    // 法線と光源に向かうベクトルwを利用して拡散反射率を求める
//    float dotNL = saturate(dot(N, L));
//    float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, POW));

//    // 法線と視点に向かうベクトルを利用して拡散反射率を求める
//    float dotNV = saturate(dot(N, V));
//    float FV = (1 + (Fd90 - 1) * pow(1 - dotNV, POW));

//    // 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
//    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
//    return (FL * FV * energyFactor);
//}


//#############
//7-2
//#############
float3 GetNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv1)
{
    //float3 normalmapNormal = g_normalMap.Sample(g_sampler_normal, uv1).xyz;
    //float3 normalmapNormal = g_normalMap.SampleLevel(g_sampler_normal, uv1, 0.0f).xyz; //2024/02/18 JCityでテストこっちが合っている
    //float3 normalmapNormal = g_normalMap.SampleLevel(g_sampler_clamp, uv1, 0.0f).xyz;//2024/02/14
    float3 normalmapNormal = g_normalMap.Sample(g_sampler_albedo, uv1).xyz;//2024/03/08 複数アセットで確認　UVについてはalbedo,normal,metalをセットで切り替えるように
    
    float3 binSpaceNormal = (normalmapNormal * 2.0f) - 1.0f;
   
    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;
    float normalleng = length(normalmapNormal);
    float3 retNormal = (normalleng >= 0.5f) ? newNormal : normal;
    return retNormal;
}

// ベックマン分布を計算する
float Beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}

// フレネルを計算。Schlick近似を使用
float SpcFresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1 - f0) * pow(1 - u, POW);
}

/// <summary>
/// Cook-Torranceモデルの鏡面反射を計算
/// </summary>
/// <param name="L">光源に向かうベクトル</param>
/// <param name="V">視点に向かうベクトル</param>
/// <param name="N">法線ベクトル</param>
/// <param name="metallic">金属度</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float metallic)
{
    float microfacet = 0.76f;

    // 金属度を垂直入射の時のフレネル反射率として扱う
    // 金属度が高いほどフレネル反射は大きくなる
    float f0 = metallic;

    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);

    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

    // D項をベックマン分布を用いて計算する
    float D = Beckmann(microfacet, NdotH);

    // F項をSchlick近似を用いて計算する
    float F = SpcFresnel(f0, VdotH);

    // G項を求める
    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

    // m項を求める
    float m = PI * NdotV * NdotH;

    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}

/// <summary>
/// フレネル反射を考慮した拡散反射を計算
/// </summary>
/// <remark>
/// この関数はフレネル反射を考慮した拡散反射率を計算します
/// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
/// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
/// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります
///
/// </remark>
/// <param name="N">法線</param>
/// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
/// <param name="V">視線に向かうベクトル。</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // step-4 フレネル反射を考慮した拡散反射光を求める

    // 法線と光源に向かうベクトルがどれだけ似ているかを内積で求める
    float dotNL = saturate(dot(N, L));

    // 法線と視線に向かうベクトルがどれだけ似ているかを内積で求める
    float dotNV = saturate(dot(N, V));

    // 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
    return (dotNL * dotNV);
}



/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント
/// </summary>
SPSIn VSMainSkinPBR(SVSIn vsIn, uniform bool hasSkin)
{
    SPSIn psIn;

    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        finalmat += (mBoneMat[vsIn.bindices[i]] * vsIn.bweight[i]);
    }
	
    psIn.pos = mul(finalmat, vsIn.pos);
    
    float3 distvec = (psIn.pos.xyz / psIn.pos.w) - eyePos.xyz;
    float skyvalue = (Flags1.z == 1) ? 490000.0f : 0.0f; //skydof ? skydofON : skydofOFF
    psIn.depth.xyz = (Flags1.x == 0) ? length(distvec) : skyvalue; // !skymesh ? dist : skyvalue
    psIn.depth.w = 1.0f; //自動的にwで割られても良いように
    
    psIn.FogAndOther.x = (vFog.w > 0.1f) ? CalcVSFog(psIn.pos) : 0.0f;
    psIn.worldPos = psIn.pos;
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);

    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float)UVs.y;
    psIn.uv.y = orguv.y * (float)UVs.z;

    psIn.diffusemult = diffusemult;

    psIn.normal = normalize(mul(finalmat, vsIn.normal));
    psIn.tangent = normalize(mul(finalmat, vsIn.tangent));
    psIn.biNormal = normalize(mul(finalmat, vsIn.biNormal));
    
    return psIn;
}

SPSInShadowMap VSMainSkinPBRShadowMap(SVSIn vsIn, uniform bool hasSkin)
{
    SPSInShadowMap psIn;

    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        finalmat += (mBoneMat[vsIn.bindices[i]] * vsIn.bweight[i]);
    }
	
    psIn.pos = mul(finalmat, vsIn.pos);
    float4 worldPos = psIn.pos / psIn.pos.w;
    
    //float3 distvec = worldPos.xyz - eyePos.xyz;
    //psIn.zpredepth.xyz = (Flags1.x == 0) ? length(distvec) : 490000.0f;
    //psIn.zpredepth.w = 1.0f; //自動的にwで割られても良いように
    
    //float4 worldPos = psIn.pos;
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);
    //psIn.uv = vsIn.uv.xy;

    // step-9 頂点のライトから見た深度値と、ライトから見た深度値の2乗を計算する
    psIn.depth.x = min(1.0f, (length(worldPos.xyz - lightPos.xyz) * shadowmaxz.x));
    //float4 posLVP = mul(mLVP, worldPos);
    //psIn.depth.x = posLVP.z / posLVP.w;    
    psIn.depth.y = psIn.depth.x * psIn.depth.x;
        
    
    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float) UVs.y;
    psIn.uv.y = orguv.y * (float) UVs.z;
    
    return psIn;
}



SPSInShadowReciever VSMainSkinPBRShadowReciever(SVSIn vsIn, uniform bool hasSkin)
{
    SPSInShadowReciever psIn;

    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        finalmat += (mBoneMat[vsIn.bindices[i]] * vsIn.bweight[i]);
    }
	
    psIn.pos = mul(finalmat, vsIn.pos);
    psIn.worldPos = psIn.pos / psIn.pos.w;
    
    float3 distvec = psIn.pos.xyz - eyePos.xyz;
    float skyvalue = (Flags1.z == 1) ? 490000.0f : 0.0f; //skydof ? skydofON : skydofOFF
    psIn.depth.xyz = (Flags1.x == 0) ? length(distvec) : skyvalue; // !skymesh ? dist : skyvalue
    psIn.depth.w = 1.0f; //自動的にwで割られても良いように
    
    psIn.FogAndOther.x = (vFog.w > 0.1f) ? CalcVSFog(psIn.pos) : 0.0f;
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);

    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float)UVs.y;
    psIn.uv.y = orguv.y * (float)UVs.z;

    psIn.diffusemult = diffusemult;
    
    // ライトビュースクリーン空間の座標を計算する
    psIn.posInLVP = mul(mLVP, psIn.worldPos);
    //psIn.posInLVP /= psIn.posInLVP.w;
    //psIn.posInLVP.xy = psIn.pos.xy;
    //psIn.posInLVP.w = psIn.pos.w;    
    // step-12 頂点のライトから見た深度値を計算する
    psIn.posInLVP.z = min(1.0f, (length(psIn.worldPos.xyz - lightPos.xyz) * shadowmaxz.x));
        
    psIn.normal = normalize(mul(finalmat, vsIn.normal));
    psIn.tangent = normalize(mul(finalmat, vsIn.tangent));
    psIn.biNormal = normalize(mul(finalmat, vsIn.biNormal));
    
    return psIn;
}



/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>
SPSOut2 PSMainSkinPBR(SPSIn psIn) : SV_Target
{
    // アルベドカラー、スペキュラカラー、金属度、滑らかさをサンプリングする。
    // アルベドカラー（拡散反射光）
    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);

      // 法線を計算
    float3 normal = GetNormal(psIn.normal.xyz, psIn.tangent.xyz, psIn.biNormal.xyz, psIn.uv);


    // スペキュラカラーはアルベドカラーと同じにする。
    float3 specColor = albedoColor.xyz;

    // 金属度
    //float4 metaltexcol = g_metallicSmoothMap.Sample(g_sampler_metal, psIn.uv);
    float4 metaltexcol = g_metallicSmoothMap.Sample(g_sampler_albedo, psIn.uv);//2024/03/08 複数アセットで確認　UVについてはalbedo,normal,metalをセットで切り替えるように
    float metallic = metaltexcol.r * metalcoef.x + metalcoef.z; //2024/02/18
    // 滑らかさ
    float smooth = metaltexcol.a * metalcoef.y; //!!!!smoothcoef

    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(eyePos.xyz - psIn.worldPos.xyz);

    float totalalpha = 0.0f;
    float3 lig = 0;
    for (int ligNo = 0; ligNo < lightsnum.x; ligNo++)    
    {
        // シンプルなディズニーベースの拡散反射を実装する。
        // フレネル反射を考慮した拡散反射を計算
        float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, directionalLight[ligNo].direction.xyz, toEye);

        // 正規化Lambert拡散反射を求める
        //float NdotL = saturate(dot(normal, directionalLight[ligNo].direction.xyz));
        //float3 lambertDiffuse = directionalLight[ligNo].color.xyz * NdotL / PI;
        float multiplecoef = diffuseFromFresnel * materialdisprate.x;
        float4 lambertDiffuse0 = CalcDiffuseColor(multiplecoef, normal.xyz, directionalLight[ligNo].direction.xyz);
        float3 lambertDiffuse1 = lambertDiffuse0.xyz * directionalLight[ligNo].color.xyz;
        
        // 最終的な拡散反射光を計算する
        float3 diffuse = albedoColor.xyz * lambertDiffuse1;

        // Cook-Torranceモデルを利用した鏡面反射率を計算する
        // Cook-Torranceモデルの鏡面反射率を計算する
        float3 spec = CookTorranceSpecular(
            directionalLight[ligNo].direction.xyz, toEye, normal, smooth)
            * directionalLight[ligNo].color.xyz * materialdisprate.y * metalcoef.w;

        // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
        // スペキュラカラーの強さを鏡面反射率として扱う
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
        // 滑らかさが高ければ、拡散反射は弱くなる
        lig += diffuse * (1.0f - smooth) + spec;
        totalalpha += lambertDiffuse0.w;
    }

    // 環境光による底上げ
    //lig += ambientLight.xyz * albedoColor.xyz;
    float diffusew = (lightsnum.x != 0) ? (albedoColor.w * totalalpha * divlights.x) : albedoColor.w;
    float4 finalColor = emission * materialdisprate.z + float4(lig, diffusew) * psIn.diffusemult;
    clip(finalColor.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    //finalColor.w = ((finalColor.w - ambient0.w) > 0.0f) ? finalColor.w : 0.0f;

    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(finalColor, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}


SPSOut0 PSMainSkinPBRShadowMap(SPSInShadowMap psIn) : SV_Target0
{
    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv); // * diffusecol;
    clip(albedoColor.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    
    SPSOut0 psOut;
    psOut.color_0 = float4(psIn.depth.x, psIn.depth.y, 0.0f, 1.0f);
    return psOut;
}


SPSOut2 PSMainSkinPBRShadowReciever(SPSInShadowReciever psIn) : SV_Target
{

    // アルベドカラー、スペキュラカラー、金属度、滑らかさをサンプリングする。
    // アルベドカラー（拡散反射光）
    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);
      // 法線を計算
    float3 normal = GetNormal(psIn.normal.xyz, psIn.tangent.xyz, psIn.biNormal.xyz, psIn.uv);


    // スペキュラカラーはアルベドカラーと同じにする。
    float3 specColor = albedoColor.xyz;

    // 金属度
    //float4 metaltexcol = g_metallicSmoothMap.Sample(g_sampler_metal, psIn.uv);
    float4 metaltexcol = g_metallicSmoothMap.Sample(g_sampler_albedo, psIn.uv);//2024/03/08 複数アセットで確認　UVについてはalbedo,normal,metalをセットで切り替えるように
    float metallic = metaltexcol.r * metalcoef.x + metalcoef.z; //2024/02/18
    // 滑らかさ
    float smooth = metaltexcol.a * metalcoef.y; //!!!!smoothcoef

    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(eyePos.xyz - psIn.worldPos.xyz);

    float totalalpha = 0.0f;
    float3 lig = 0;
    for (int ligNo = 0; ligNo < lightsnum.x; ligNo++)
    {
        // シンプルなディズニーベースの拡散反射を実装する。
        // フレネル反射を考慮した拡散反射を計算
        float diffuseFromFresnel = CalcDiffuseFromFresnel(
            normal, directionalLight[ligNo].direction.xyz, toEye);

        // 正規化Lambert拡散反射を求める
        //float NdotL = saturate(dot(normal, directionalLight[ligNo].direction.xyz));
        //float3 lambertDiffuse = directionalLight[ligNo].color.xyz * NdotL / PI;
        float multiplecoef = diffuseFromFresnel * materialdisprate.x;
        float4 lambertDiffuse0 = CalcDiffuseColor(multiplecoef, normal.xyz, directionalLight[ligNo].direction.xyz);
        float3 lambertDiffuse1 = lambertDiffuse0.xyz * directionalLight[ligNo].color.xyz;
        
        // 最終的な拡散反射光を計算する
        float3 diffuse = albedoColor.xyz * lambertDiffuse1;

        // Cook-Torranceモデルを利用した鏡面反射率を計算する
        // Cook-Torranceモデルの鏡面反射率を計算する
        float3 spec = CookTorranceSpecular(
            directionalLight[ligNo].direction.xyz, toEye, normal, smooth)
            * directionalLight[ligNo].color.xyz * materialdisprate.y * metalcoef.w;

        // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
        // スペキュラカラーの強さを鏡面反射率として扱う
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

        // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
        // 滑らかさが高ければ、拡散反射は弱くなる
        lig += diffuse * (1.0f - smooth) + spec;
        totalalpha += lambertDiffuse0.w;
    }

    // 環境光による底上げ
    //lig += ambientLight.xyz * albedoColor.xyz;

    //float4 finalColor;
    //finalColor.xyz = lig;
    //finalColor.w = albedoColor.w;
    
    float diffusew = (lightsnum.x != 0) ? (albedoColor.w * totalalpha * divlights.x) : albedoColor.w;
    float4 finalColor = emission * materialdisprate.z + float4(lig, diffusew) * psIn.diffusemult;
///////////
/////////
    // ライトビュースクリーン空間からUV空間に座標変換
    float2 shadowMapUV = psIn.posInLVP.xy / psIn.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    // ライトビュースクリーン空間でのZ値を計算する
    float zInLVP = psIn.posInLVP.z;
    float2 shadowValue = g_shadowMap.Sample(g_sampler_shadow, shadowMapUV).xy;

    if ((shadowMapUV.x > 0.0f) && (shadowMapUV.x < 1.0f)
        && (shadowMapUV.y > 0.0f) && (shadowMapUV.y < 1.0f))
    {
        if (Flags1.w == 0)
        {
            finalColor.xyz *= (((zInLVP - shadowmaxz.y) > shadowValue.r) && (zInLVP <= 1.0f)) ? shadowmaxz.z : 1.0f;
        }
        else
        {
    //###########
    //2024/04/10
    //Variance Shadow Maps　分散シャドウマップ
    //###########                        
        // まずこのピクセルが遮蔽されているか調べる。これは通常のデプスシャドウと同じ
            if (((zInLVP - shadowmaxz.y) > shadowValue.r) && (zInLVP <= 1.0f))
            {
            // 遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める
                float depth_sq = shadowValue.x * shadowValue.x;

            // このグループの分散具合を求める
            // 分散が大きいほど、varianceの数値は大きくなる
                float variance = min(max(shadowValue.y - depth_sq, 0.0001f), 1.0f);

            // このピクセルのライトから見た深度値とシャドウマップの平均の深度値の差を求める
                float md = zInLVP - shadowValue.x;

            // 光が届く確率を求める
                float lit_factor = variance / (variance + md * md);

            // シャドウカラーを求める
                float3 shadowColor = finalColor.xyz * shadowmaxz.z;

            // 光が当たる確率を使って通常カラーとシャドウカラーを線形補完
                finalColor.xyz = lerp(shadowColor, finalColor.xyz, lit_factor);
            }
        }
    }

    clip(finalColor.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
        
    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(finalColor, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}


