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
struct CSChkInViewInputData
{
    float4 bs;//[0]-[2]:centerpos, [3]:r
    float4 bbmin;//bbox min
    float4 bbmax;//bbox max
    int4 lodno; //[0]:lodnumindex(lodnum - 1), [1]:lodno, [2]-[3]:未使用
    int4 forceresult; //[0]:forceInView, [1]:forceInViewValue, [2]:forceInShadow, [3]:forceInShadowValue
};

struct CSChkInViewOutputData
{
    int4 inview; //[0]:InView, [1]:InShadow, [2]-[3]:未使用
    float4 dist; //[0]:distFromCamera
};


///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
cbuffer ChkInViewCb : register(b0)
{
    int4 mBufferSize; //[0]:bsnum, [1]:forceInView, [2]:forceInShadow
    float4x4 mWorld; //ワールド行列。
    float4 camEye;
    float4 params1; //[0]:BACKPOSCOEF, [1]:cos(g_fovy * 0.85), [2]:g_projfar, [3]:g_projnear
    //float4 lodrate2L;
    //float4 lodrate3L;
    //float4 lodrate4L;
    float4 mlodmindist[4]; //2024/04/25 mlodmindist[lodnum][lodno]
    float4 mlodmaxdist[4]; //2024/04/25 mlodmaxdist[lodnum][lodno]    
    float4 shadowPos;
    float4 shadowparams1; //[0]:cos(shadowfov), [1]:shadowmaxdist(g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno])
    float4 frustumPlanes[6];
    float4 frustumCorners[8];
    float4 shadowPlanes[6];
    float4 shadowCorners[8];
};


// 入力にアクセスするための変数
StructuredBuffer<CSChkInViewInputData> g_inputData : register(t0);
// 出力先にアクセスするための変数
RWStructuredBuffer<CSChkInViewOutputData> g_outputData : register(u0);

int isBoxInFrustum(float4 bbmin, float4 bbmax)
{
    for (int i = 0; i < 6; i++)
    {
        int r = 0;
        r += (dot(frustumPlanes[i], float4(bbmin.x, bbmin.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmax.x, bbmin.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmin.x, bbmax.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmax.x, bbmax.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmin.x, bbmin.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmax.x, bbmin.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmin.x, bbmax.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(frustumPlanes[i], float4(bbmax.x, bbmax.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
                
        if (r == 8)
            return 0;
    }
        
    int r = 0;
    r = 0;
    for (int a = 0; a < 8; a++)
        r += ((frustumCorners[a].x > bbmax.x) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int b = 0; b < 8; b++)
        r += ((frustumCorners[b].x < bbmin.x) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int c = 0; c < 8; c++)
        r += ((frustumCorners[c].y > bbmax.y) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int d = 0; d < 8; d++)
        r += ((frustumCorners[d].y < bbmin.y) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int e = 0; e < 8; e++)
        r += ((frustumCorners[e].z > bbmax.z) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int f = 0; f < 8; f++)
        r += ((frustumCorners[f].z < bbmin.z) ? 1 : 0);
    if (r == 8)
        return 0;

    return 1;
}

int isBoxInShadow(float4 bbmin, float4 bbmax)
{
    for (int i = 0; i < 6; i++)
    {
        int r = 0;
        r += (dot(shadowPlanes[i], float4(bbmin.x, bbmin.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmax.x, bbmin.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmin.x, bbmax.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmax.x, bbmax.y, bbmin.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmin.x, bbmin.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmax.x, bbmin.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmin.x, bbmax.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;
        r += (dot(shadowPlanes[i], float4(bbmax.x, bbmax.y, bbmax.z, 1.0f)) < 0.f) ? 1 : 0;

        if (r == 8)
            return 0;
    }
        
    int r = 0;
    r = 0;
    for (int a = 0; a < 8; a++)
        r += ((shadowCorners[a].x > bbmax.x) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int b = 0; b < 8; b++)
        r += ((shadowCorners[b].x < bbmin.x) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int c = 0; c < 8; c++)
        r += ((shadowCorners[c].y > bbmax.y) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int d = 0; d < 8; d++)
        r += ((shadowCorners[d].y < bbmin.y) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int e = 0; e < 8; e++)
        r += ((shadowCorners[e].z > bbmax.z) ? 1 : 0);
    if (r == 8)
        return 0;
    r = 0;
    for (int f = 0; f < 8; f++)
        r += ((shadowCorners[f].z < bbmin.z) ? 1 : 0);
    if (r == 8)
        return 0;
    
    return 1;
}

[numthreads(4, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    int dataIndex = DTid.x;
    if (dataIndex >= mBufferSize[0])
        return;
     
    float4 tracenter;
    float4 centerpos = float4(g_inputData[dataIndex].bs.xyz, 1.0f);
    tracenter = mul(mWorld, centerpos);
    
//#############
//ChkInView
//#############
    float3 cam2obj = tracenter.xyz - camEye.xyz;
    float dist_cam2obj = length(cam2obj);

    int inFrustum = isBoxInFrustum(g_inputData[dataIndex].bbmin, g_inputData[dataIndex].bbmax);
        
	//############################
	//LODがvisibleかどうかチェック
	//############################
    int srclodnum = g_inputData[dataIndex].lodno[0];//lodnumindex : (lodnum - 1)
    int srclodno = g_inputData[dataIndex].lodno[1];
    float lod_mindist = mlodmindist[srclodnum][srclodno];
    float lod_maxdist = mlodmaxdist[srclodnum][srclodno];
 
    int lodinview = ((dist_cam2obj < lod_maxdist) && (dist_cam2obj >= lod_mindist)) ? 1 : 0;
	//##################
	//LODがvisible && 
	//##################
    float calcinview = ((lodinview == 1) && (inFrustum == 1)) ? 1 : 0;
    float calcdist = dist_cam2obj;


    g_outputData[dataIndex].inview[0] = (g_inputData[dataIndex].forceresult[0] == 0) ? calcinview : g_inputData[dataIndex].forceresult[1];
    g_outputData[dataIndex].dist[0] = (g_inputData[dataIndex].forceresult[0] == 0) ? calcdist : 0.0f;
    
    
//#############
//ChkInShadow
//#############

	//############################################################################
	//シャドウマップがシャドウ射影範囲に入っているかどうかの判定をする
    //視野外の場合にはinshadowも０になるように　視野内判定結果をシャドウ範囲内判定結果に掛ける
	//############################################################################
    int calcinshadow = isBoxInShadow(g_inputData[dataIndex].bbmin, g_inputData[dataIndex].bbmax) * g_outputData[dataIndex].inview[0];
    
    g_outputData[dataIndex].inview[1] = (g_inputData[dataIndex].forceresult[2] == 0) ? calcinshadow : g_inputData[dataIndex].forceresult[3];
    
}
