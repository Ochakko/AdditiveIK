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
    float4 pos : POSITION0;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 biNormal : BINORMAL;        
    float4 uv : TEXCOORD0;
    float4 projpos : POSITIONT;   
    float4 vertexid4 : POSITION1;
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
    float4 FogAndOther : TEXCOORD2; //x:Fog, y:Mono, w:texid
    float4 depth : TEXCOORD3;
};

struct SGSIn
{
    float2 uv           : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
    float4 FogAndOther : TEXCOORD2; //x:Fog, y:Mono
    float4 depth : TEXCOORD4;
    float4 normal       : NORMAL;    
    float4 pos          : POSITION0;
    float4 vertexid : POSITION1;
};
struct SGSOut
{
    float2 uv           : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
    float4 FogAndOther : TEXCOORD2; //x:Fog, y:Mono
    float4 RefPosParams : TEXCOORD3; //x:pointsize, y:texkind, z:refcounter    
    float4 depth : TEXCOORD4;
    float4 normal       : NORMAL;    
    float4 posrw          : POSITION;
    float4 pos          : SV_POSITION;
};
struct SPSInFromGS
{
    float2 uv           : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
    float4 FogAndOther : TEXCOORD2; //x:Fog
    float4 RefPosParams : TEXCOORD3; //x:pointsize, y:texkind, z:refcounter
    float4 depth : TEXCOORD4;
    float4 normal       : NORMAL;    
    float4 posrw          : POSITION;
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
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 diffusemult : TEXCOORD1;
    
    // ライトビュースクリーン空間での座標を追加
    float4 posInLVP : TEXCOORD2; // ライトビュースクリーン空間でのピクセルの座標
    float4 FogAndOther : TEXCOORD3; //x:Fog
    float4 depth : TEXCOORD4;
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


cbuffer cbImmutable
{
    static float3 g_positions[4] =
    {
        float3(-1, 1, 0),
        float3(1, 1, 0),
        float3(-1, -1, 0),
        float3(1, -1, 0)
    };
    
    static float2 g_texcoords[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1)
    };
};


///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
cbuffer ModelCb : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 minvView;
    float4x4 mProj;
    float4x4 mViewProj;
    float4 diffusemult;
    float4 ambient0;//ambient0.wはAlphaTestの閾値
    float4 emission;
    float4 metalcoef;
    float4 materialdisprate;
    float4 shadowmaxz; //x:(1/shadowfar), y:shadowbias
    int4 UVs; //x:UVSet, y:TilingU, z:TilingV, w:distortionFlag   
    int4 Flags1; //x:skyflag, y:groundflag, z:skydofflag, w:VSM
    int4 Flags2; //x:grassflag, y:MonoFlag, z:RefPosDec, w:RefPosSkip
    float4 time1; //2024/04/27 x:DXUTTime, y:refpos_pointsize, z:refposCounter01, w:refpos_power
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
Texture2D<float4> g_emissiveMap : register(t4); // 自己照明マップ
Texture2D<float4> g_shadowMap : register(t5);
Texture2D<float4> g_numMap1 : register(t6);
Texture2D<float4> g_numMap2 : register(t7);
Texture2D<float4> g_numMap3 : register(t8);
Texture2D<float4> g_numMap4 : register(t9);

// サンプラーステート
sampler g_sampler : register(s0);
sampler g_sampler_albedo : register(s1);
sampler g_sampler_normal : register(s2);
sampler g_sampler_metal : register(s3);
sampler g_sampler_clamp : register(s4); //2024/02/14
sampler g_sampler_shadow : register(s5);
sampler g_sampler_num1 : register(s6);
sampler g_sampler_num2 : register(s7);
sampler g_sampler_num3 : register(s8);
sampler g_sampler_num4 : register(s9);


float fracSin11(float x)
{ // 1 in, 1 out
    return frac(1000.0f * sin(x));
}
float fracSin21(float2 xy)
{ // 2 in, 1 out
    return frac(sin(dot(xy, float2(12.9898f, 78.233f))) * 43758.5453123f);
}

float CalcVSFog(float4 worldpos)
{
    worldpos /= worldpos.w;
    float4 fogpos = worldpos - eyePos;
    float fogy = (worldpos.y - vFog.y) * vFog.x;
    float fog = (vFog.w < 1.1f) ? (vFog.z * (length(fogpos.xyz) - vFog.y) * vFog.x) : (vFog.z - vFog.z * fogy * fogy);
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
//float4 CalcDiffuseColorNum(int number, float multiplecoef, float3 meshnormal, float3 lightdir)
//{
//    float3 normaly0 = (lightsnum.w == 1) ? normalize(float3(meshnormal.x, 0.0f, meshnormal.z)) : meshnormal;
//    float3 lighty0 = (lightsnum.w == 1) ? normalize(float3(lightdir.x, 0.0f, lightdir.z)) : lightdir;
//    float nl;
//    nl = dot(normaly0, lighty0);
//    float toonh = (nl + 1.0f) * 0.5f * multiplecoef;
//    float2 diffuseuv = { 0.5f, toonh };
//    float4 diffusecol;
//    switch (number)
//    {
//        case 0:
//            diffusecol = g_numMap1.Sample(g_sampler_num1, diffuseuv) * materialdisprate.x;
//            break;
//        case 1:
//            diffusecol = g_numMap2.Sample(g_sampler_num2, diffuseuv) * materialdisprate.x;
//            break;
//        case 2:
//            diffusecol = g_numMap3.Sample(g_sampler_num3, diffuseuv) * materialdisprate.x;
//            break;
//        case 3:
//            diffusecol = g_numMap4.Sample(g_sampler_num4, diffuseuv) * materialdisprate.x;
//            break;
//        default:
//            diffusecol = g_numMap1.Sample(g_sampler_num1, diffuseuv) * materialdisprate.x;
//            break;
//    }
//    return diffusecol;
//}


float4 GetEmissiveMap(float2 uv1)
{
    float4 emimap = g_emissiveMap.Sample(g_sampler_albedo, uv1); //UVについてはalbedo,normal,metalをセットで切り替えるように
    emimap.w = 0.0f;//diffuseに足し算して使用するのでwは0にしておく
    return emimap;
}

/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント
/// </summary>
SPSIn VSMainSkinStd(SVSIn vsIn, uniform bool hasSkin)
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
    psIn.pos = mul(mViewProj, psIn.pos);
    //psIn.pos /= psIn.pos.w;
    
    psIn.normal = normalize(mul(finalmat, vsIn.normal));    
    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float)UVs.y;
    psIn.uv.y = orguv.y * (float)UVs.z;
    
    psIn.diffusemult = diffusemult;
    
    return psIn;
}

SGSIn VSMainSkinStdForGS(SVSIn vsIn, uniform bool hasSkin)
{
    SGSIn psIn;

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
    psIn.FogAndOther.y = 0.0f;
    psIn.FogAndOther.z = time1.y;
    psIn.FogAndOther.w = time1.z;
        
    //psIn.pos = mul(mView, psIn.pos);
    //psIn.pos = mul(mProj, psIn.pos);
    ////psIn.pos /= psIn.pos.w;
    
    psIn.normal = normalize(mul(finalmat, vsIn.normal));    
    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float)UVs.y;
    psIn.uv.y = orguv.y * (float)UVs.z;
    
    psIn.diffusemult = diffusemult;
    psIn.vertexid = vsIn.vertexid4;
    
    return psIn;
}


SPSInShadowMap VSMainSkinStdShadowMap(SVSIn vsIn, uniform bool hasSkin)
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
    psIn.pos = mul(mViewProj, worldPos);
    //psIn.pos /= psIn.pos.w;
    
    // step-9 頂点のライトから見た深度値と、ライトから見た深度値の2乗を計算する
    psIn.depth.x = min(1.0f, (length(worldPos.xyz - lightPos.xyz) * shadowmaxz.x));
    //float4 posLVP = mul(mLVP, worldPos);
    //psIn.depth.x = posLVP.z / posLVP.w;
    psIn.depth.y = psIn.depth.x * psIn.depth.x;
        
    //psIn.normal = normalize(mul(finalmat, vsIn.normal));
    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float) UVs.y;
    psIn.uv.y = orguv.y * (float) UVs.z;

    //psIn.diffusemult = diffusemult;
    
    return psIn;
}

SPSInShadowReciever VSMainSkinStdShadowReciever(SVSIn vsIn, uniform bool hasSkin)
{
    SPSInShadowReciever psIn;

    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        finalmat += (mBoneMat[vsIn.bindices[i]] * vsIn.bweight[i]);
    }
	
    float4 worldPos = mul(finalmat, vsIn.pos);
    
    float3 distvec = (worldPos.xyz / worldPos.w) - eyePos.xyz;
    float skyvalue = (Flags1.z == 1) ? 490000.0f : 0.0f; //skydof ? skydofON : skydofOFF
    psIn.depth.xyz = (Flags1.x == 0) ? length(distvec) : skyvalue; // !skymesh ? dist : skyvalue
    psIn.depth.w = 1.0f; //自動的にwで割られても良いように
    
    psIn.FogAndOther.x = (vFog.w > 0.1f) ? CalcVSFog(worldPos) : 0.0f;
    worldPos /= worldPos.w;
    psIn.pos = mul(mViewProj, worldPos);    

    // ライトビュースクリーン空間の座標を計算する
    psIn.posInLVP = mul(mLVP, worldPos);
    //psIn.posInLVP /= psIn.posInLVP.w;
    //psIn.posInLVP.xy = psIn.pos.xy;
    //psIn.posInLVP.w = psIn.pos.w;    
    // step-12 頂点のライトから見た深度値を計算する
    psIn.posInLVP.z = min(1.0f, length(worldPos.xyz - lightPos.xyz) * shadowmaxz.x);
    
    psIn.normal = normalize(mul(finalmat, vsIn.normal));
    float2 orguv = (UVs.x == 0) ? vsIn.uv.xy : vsIn.uv.zw;
    psIn.uv.x = orguv.x * (float)UVs.y;
    psIn.uv.y = orguv.y * (float)UVs.z;

    psIn.diffusemult = diffusemult;
   
    return psIn;
}


//
// GS for rendering point sprite particles.  Takes a point and turns 
// it into 2 triangles.
//
[maxvertexcount(4)]
void GSParticleDraw(point SGSIn input[1], inout TriangleStream<SGSOut> SpriteStream)
{
    int posrnd = (int) (fracSin21(input[0].pos.xy) * 60.0f);
    int santime2 = (int) (200 / (time1.w * time1.w));
    int timeint2 = (int) (input[0].FogAndOther.w + posrnd) % santime2;// % 200;
    
    int timeint = (int) (input[0].FogAndOther.w + posrnd) % 64;
    float addbase = (float) timeint / 60.0f; // * 0.5f;
    float sizescale = (!Flags2.w && (input[0].vertexid.x % Flags2.z) == 0) ? (addbase * addbase * addbase * addbase * addbase * addbase * input[0].FogAndOther.z) : 0.01f;

    float shiftx = input[0].normal.x * fracSin11(input[0].pos.x) * pow((float) timeint2, time1.w) * input[0].FogAndOther.z;
    float shifty = input[0].normal.y * fracSin11(input[0].pos.y) * pow((float) timeint2, time1.w) * input[0].FogAndOther.z;
    float shiftz = input[0].normal.z * fracSin11(input[0].pos.z) * pow((float) timeint2, time1.w) * input[0].FogAndOther.z;

    float params_x = sizescale;
    float params_y = fracSin21(input[0].pos.xy) * 4.0f;
    float params_z = input[0].FogAndOther.w;
    float params_w = fracSin11(input[0].FogAndOther.w);

    
    // Emit two new triangles.
    for (int i = 0; i < 4; i++)
    {
        SGSOut output;

        //RefPosParams x:pointsize, y:texkind, z:refcounter
        output.RefPosParams.x = params_x;
        output.RefPosParams.y = params_y;
        output.RefPosParams.z = params_z;
        output.RefPosParams.w = params_w;

        float3 position = g_positions[i] * params_x; // x refpos_pointsize
        position = mul((float3x3) minvView, position) + input[0].pos.xyz + float3(shiftx, shifty, shiftz);
      
        output.pos = float4(position, 1.0);
        output.pos = mul(mViewProj, output.pos);
        output.posrw = output.pos;
        
        output.uv = g_texcoords[i]; //input[0].uv;
        output.diffusemult = input[0].diffusemult;
        output.FogAndOther = input[0].FogAndOther;
        output.depth = input[0].depth;
        output.normal = input[0].normal;

        SpriteStream.Append(output);
    }

    SpriteStream.RestartStrip();
}




/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>

SPSOut2 PSMainSkinStd(SPSIn psIn) : SV_Target
{
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
    float4 totalspecular4 = float4(totalspecular, 0.0f) * materialdisprate.y * metalcoef.w;//ライト８個で白飛びしないように応急処置1/8=0.125
    float4 emimap = GetEmissiveMap(psIn.uv);
    float4 pscol = emimap * emission * materialdisprate.z + albedoColor * psIn.diffusemult * totaldiffuse4 + totalspecular4;
    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    //pscol.w = ((pscol.w - ambient0.w) > 0.0f) ? pscol.w : 0.0f;

    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}

SPSOut2 PSMainSkinStdFromGS(SPSInFromGS psIn) : SV_Target
{
    //RefPosParams x:pointsize, y:texkind, z:refcounter

    // 普通にテクスチャを
    //float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    float4 albedoColor; // = g_numMap2.Sample(g_sampler_num2, psIn.uv);
    albedoColor = (psIn.RefPosParams.y >= 2.9f) ? g_numMap4.Sample(g_sampler_num4, psIn.uv) : ((psIn.RefPosParams.y >= 1.9f) ? g_numMap3.Sample(g_sampler_num3, psIn.uv) : ((psIn.RefPosParams.y >= 0.9f) ? g_numMap2.Sample(g_sampler_num2, psIn.uv) : g_numMap1.Sample(g_sampler_num1, psIn.uv)));
    albedoColor *= psIn.diffusemult;
     
    float3 wPos = psIn.posrw.xyz / psIn.posrw.w;
    
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
    float4 totalspecular4 = float4(totalspecular, 0.0f) * materialdisprate.y * metalcoef.w;//ライト８個で白飛びしないように応急処置1/8=0.125
    float4 emimap = GetEmissiveMap(psIn.uv);
    float4 pscol = emimap * emission * materialdisprate.z + albedoColor * psIn.diffusemult * totaldiffuse4 + totalspecular4;
    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    //pscol.w = ((pscol.w - ambient0.w) > 0.0f) ? pscol.w : 0.0f;

    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}


SPSOut0 PSMainSkinStdShadowMap(SPSInShadowMap psIn) : SV_Target0
{
    float4 albedocol = g_albedo.Sample(g_sampler_albedo, psIn.uv);
    clip(albedocol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない

    SPSOut0 psOut;
    psOut.color_0 = float4(psIn.depth.x, psIn.depth.y, 0.0f, 1.0f);
    return psOut;
}


SPSOut2 PSMainSkinStdShadowReciever(SPSInShadowReciever psIn) : SV_Target
{
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
    float4 emimap = GetEmissiveMap(psIn.uv);
    float4 pscol = emimap * emission * materialdisprate.z + albedoColor * psIn.diffusemult * totaldiffuse4 + totalspecular4;

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
            pscol.xyz *= (((zInLVP - shadowmaxz.y) > shadowValue.r) && (zInLVP <= 1.0f)) ? shadowmaxz.z : 1.0f;
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
                float3 shadowColor = pscol.xyz * shadowmaxz.z;

            // 光が当たる確率を使って通常カラーとシャドウカラーを線形補完
                pscol.xyz = lerp(shadowColor, pscol.xyz, lit_factor);
            }
        }
    }

    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    
    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}



SPSOut2 PSMainSkinNoLight(SPSIn psIn) : SV_Target
{
    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);


    float4 diffusecol = (lightsnum.y == 1) ? CalcDiffuseColor(1.0f, psIn.normal.xyz, toonlightdir.xyz) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 emimap = GetEmissiveMap(psIn.uv);
    float4 pscol = emimap * emission * materialdisprate.z + albedoColor * diffusecol * psIn.diffusemult;
    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない
    //pscol.w = ((pscol.w - ambient0.w) > 0.0f) ? pscol.w : 0.0f;
  
    
    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}

SPSOut2 PSMainSkinNoLightShadowReciever(SPSInShadowReciever psIn) : SV_Target
{
    float4 albedoColor = g_albedo.Sample(g_sampler_albedo, psIn.uv);


    float4 diffusecol = CalcDiffuseColor(1.0f, psIn.normal.xyz, toonlightdir.xyz);
    float4 emimap = GetEmissiveMap(psIn.uv);
    float4 pscol = emimap * emission * materialdisprate.z + albedoColor * diffusecol * psIn.diffusemult;
//////////
    // ライトビュースクリーン空間からUV空間に座標変換
    float2 shadowMapUV = psIn.posInLVP.xy / psIn.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    // ライトビュースクリーン空間でのZ値を計算する
    float zInLVP = psIn.posInLVP.z;
    float2 shadowValue = g_shadowMap.Sample(g_sampler_shadow, shadowMapUV).xy;

    if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
        && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {
        if (Flags1.w == 0)
        {
            pscol.xyz *= (((zInLVP - shadowmaxz.y) > shadowValue.r) && (zInLVP <= 1.0f)) ? shadowmaxz.z : 1.0f;
        }
        else
        {
    //###########
    //2024/04/10
    //Variance Shadow Maps　分散シャドウマップ
    //###########                        
        // まずこのピクセルが遮蔽されているか調べる。これは通常のデプスシャドウと同じ
            if ((zInLVP - shadowmaxz.y) > shadowValue.r && zInLVP <= 1.0f)
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
                float3 shadowColor = pscol.xyz * shadowmaxz.z;

            // 光が当たる確率を使って通常カラーとシャドウカラーを線形補完
                pscol.xyz = lerp(shadowColor, pscol.xyz, lit_factor);
            }
        }
    }
    
    
    clip(pscol.w - ambient0.w); //2024/03/22 アルファテスト　ambient.wより小さいアルファは書き込まない

    SPSOut2 psOut;
    psOut.color_0 = CalcPSFog(pscol, psIn.FogAndOther.x);
    psOut.color_1 = psIn.depth;
    return psOut;
}

