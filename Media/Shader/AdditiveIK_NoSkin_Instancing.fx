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
    float4 projpos : POSITIONT;    
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
    float4 bendvec : TEXCOORD11;//2024/05/12
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
    float4 FogAndOther : TEXCOORD2; //x:Fog
    float4 depth : TEXCOORD3;
    
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
    float4 ambient0; //ambient0.wはAlphaTestの閾値
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

// ライト用の定数バッファー
cbuffer LightCb : register(b1)
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
    float4 diffusecol = g_diffusetex.Sample(g_sampler_albedo, diffuseuv) * materialdisprate.x;
    
    return diffusecol;
}

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
    
    if (Flags2.x == 1)
    {
        float bendval = ((vsIn.pos.y >= (bbsize.y * 0.5f))) ? (vsIn.pos.y / bbsize.y) : 0.0f;
        //float xgrassshift = sin(time1) * bendval * bendval * bbsize.x * 0.5f;
        float maxshift = min(7.5f, bbsize.x);
        float xgrassshift = sin(time1) * bendval * bendval * maxshift;
        //psIn.pos.x += xgrassshift;
        psIn.pos.xyz += vsIn.bendvec.xyz * xgrassshift * psIn.pos.w;//bendvec : 2024/05/12
    }
    
    float3 distvec = (psIn.pos.xyz / psIn.pos.w) - eyePos.xyz;
    float skyvalue = (Flags1.z == 1) ? 490000.0f : 0.0f; //skydof ? skydofON : skydofOFF
    psIn.depth.xyz = (Flags1.x == 0) ? length(distvec) : skyvalue; // !skymesh ? dist : skyvalue
    psIn.depth.w = 1.0f; //自動的にwで割られても良いように
    psIn.FogAndOther.x = (vFog.w > 0.1f) ? CalcVSFog(psIn.pos) : 0.0f;
        
    psIn.pos = mul(psIn.pos, vpmat);
    psIn.diffusemult = diffusemult * vsIn.material;
    
    psIn.normal = normalize(mul(wmat, vsIn.normal));

    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float) UVs.y;
    psIn.uv.y = orguv.y * (float) UVs.z;

    
    return psIn;
}

//SPSOut2 PSMainNoSkinInstancingNoLight(SPSIn psIn) : SV_Target
float4 PSMainNoSkinInstancingNoLight(SPSIn psIn) : SV_Target
{
    // 普通にテクスチャを
    //return g_texture.Sample(g_sampler, psIn.uv);
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);

    float2 diffuseuv = { 0.5f, 0.5f };
    float4 diffusecol = g_diffusetex.Sample(g_sampler_albedo, diffuseuv) * materialdisprate.x;
    //texcol.w = 1.0f;
    //return texcol;
      
    float4 pscol = emission * materialdisprate.z + albedocol * diffusecol * psIn.diffusemult;
    //pscol.w = albedocol.w * psIn.diffusemult.w * materialdisprate.x;
    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    
    //SPSOut2 psOut;
    //psOut.color_0 = pscol;
    //psOut.color_1 = float4(0.0f, 0.0f, 0.0f, 0.0f);//DOFでボケないように0.0をセット
    //return psOut;
    
    return pscol;
}


////SPSOut2 PSMainNoSkinInstancingGrass(SPSIn psIn) : SV_Target
//float4 PSMainNoSkinInstancingGrass(SPSIn psIn) : SV_Target
//{
//    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);


//    //float4 diffusecol = (lightsnum.y == 1) ? CalcDiffuseColor(1.0f, psIn.normal.xyz, toonlightdir.xyz) : float4(1.0f, 1.0f, 1.0f, 1.0f);
//    float4 diffusecol = CalcDiffuseColor(1.0f, psIn.normal.xyz, toonlightdir.xyz);
//    float4 pscol = emission * materialdisprate.z + albedoColor * diffusecol * psIn.diffusemult;
//    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
//    //pscol.w = ((pscol.w - ambient0.w) > 0.0f) ? pscol.w : 0.0f;
    
//    //SPSOut2 psOut;
//    //psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
//    //psOut.color_1 = psIn.depth;
//    //return psOut;

//    return pscol;
//}

float4 PSMainNoSkinInstancingGrass(SPSIn psIn) : SV_Target
{
    // 普通にテクスチャを
    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);

     
    float3 wPos = psIn.pos.xyz / psIn.pos.w;
    
    float3 totaldiffuse = float3(0, 0, 0);
    float3 totalspecular = float3(0, 0, 0);
    float totalalpha = 0.0f;
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
        totalalpha += diffusecol.w;
    }
    float4 totaldiffuse4 = float4(totaldiffuse, 1.0f);
    totaldiffuse4.w = (lightsnum.x != 0) ? (totalalpha * divlights.x) : 1.0f;
    float4 totalspecular4 = float4(totalspecular, 0.0f) * materialdisprate.y * metalcoef.w; //ライト８個で白飛びしないように応急処置1/8=0.125
    float4 pscol = emission * materialdisprate.z + albedoColor * psIn.diffusemult * totaldiffuse4 + totalspecular4;
    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    //pscol.w = ((pscol.w - ambient0.w) > 0.0f) ? pscol.w : 0.0f;

    
    //SPSOut2 psOut;
    //psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
    //psOut.color_1 = psIn.depth;
    //return psOut;
    
    pscol = CalcPSFog(pscol, psIn.FogAndOther.x);
    
    return pscol;
}



