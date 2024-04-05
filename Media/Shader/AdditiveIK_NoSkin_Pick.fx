///////////////////////////////////////////////////
// 定数
///////////////////////////////////////////////////
//static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_DIRECTIONAL_LIGHT = 8; // ディレクションライトの本数
static const float PI = 3.1415926f; // π
//static const float POW = 15.0;
//static const float POW = 0.2f;
static const float POW = 5.0f;


static float JUSTVAL = 0.0001f;

///////////////////////////////////////////
// 構造体
///////////////////////////////////////////
// 頂点シェーダーへの入力
struct CSIndices_PickNoBone
{
    int4 index;
};
struct CSInputData_PickNoBone
{
    float4 pos;
};

struct CSOutputData_PickNoBone
{
    int4 result;//0:hitflag, 1:justflag, 2:hitfaceno, 3:未使用0
    int4 dbginfo;//0:return code, 1:index1, 2:index2, 3:index3
};

///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
cbuffer PickCb_PickNoBone : register(b0)
{
    int4 mBufferSize; //0:vertexnum, 1:facenum, 2:indicesnum, 3:未使用0
    float4 mStartglobal; //start point of PickRay
    float4 mDirglobal; //direction of PickRay
    int4 mFlags; //0:excludeinvface, 1-3:未使用0
};


// 入力にアクセスするための変数
StructuredBuffer<CSIndices_PickNoBone> g_inputIndices_PickNoBone : register(t0);
// 入力にアクセスするための変数
RWStructuredBuffer<CSInputData_PickNoBone> g_inputVertex_PickNoBone : register(u0);
// 出力先にアクセスするための変数
RWStructuredBuffer<CSOutputData_PickNoBone> g_output_PickNoBone : register(u1);//要素数1


[numthreads(4, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    int faceIndex = DTid.x;
    if (faceIndex >= mBufferSize[1])//facenoチェック
        return;
    
    //当りが既にみつかっていたばあいにはすぐにリターン
    if ((g_output_PickNoBone[0].result[0] != 0) || (g_output_PickNoBone[0].result[1] != 0))
        return;
    

    //########################################################################################
    //当りがみつからずにリターンする場合にはresultには値をセットしない　並列実行で当りマークが消されないように
    //########################################################################################
    
    
    //g_output_PickNoBone[0].dbginfo[0] = 999;
        
    int index1 = g_inputIndices_PickNoBone[faceIndex].index[0];
    int index2 = g_inputIndices_PickNoBone[faceIndex].index[1];
    int index3 = g_inputIndices_PickNoBone[faceIndex].index[2];
    
    //g_output_PickNoBone[0].dbginfo[1] = index1;
    //g_output_PickNoBone[0].dbginfo[2] = index2;
    //g_output_PickNoBone[0].dbginfo[3] = index3;
    
    
    float3 point1 = g_inputVertex_PickNoBone[index1].pos.xyz;
    float3 point2 = g_inputVertex_PickNoBone[index2].pos.xyz;
    float3 point3 = g_inputVertex_PickNoBone[index3].pos.xyz;
    
    float3 v1 = mStartglobal.xyz;
    float3 v = mDirglobal.xyz;

    float3 ev; //e
    ev = normalize(v);

    float3 s, t;
    s = point2 - point1;
    t = point3 - point1;
    float3 abc;
    abc = cross(s, t);
    abc = normalize(abc);

    float d;
    d = -dot(abc, point1);

    float dotface = dot(abc, ev);
    if (dotface == 0.0f)
    {
        //g_output_PickNoBone[0].dbginfo[0] = 1;
        return;
    }
    if ((mFlags[0] == 0) && (dotface < 0.0f))
    {
		//裏面は当たらない
        //g_output_PickNoBone[0].dbginfo[0] = 2;
        return;
    }

    float k;
    k = -((dot(abc, v1) + d) / dot(abc, ev));
    if (abs(k) <= JUSTVAL)
    {
        g_output_PickNoBone[0].result[1] = 1; //justval !!!
        g_output_PickNoBone[0].result[2] = faceIndex;        
        //g_output_PickNoBone[0].dbginfo[0] = 3;
        return;
    }
    if (k < 0.0f)
    {
        return;
    }
    
    float3 q;
    q = v1 + ev * k;

    float3 g0, g1, cA, cB, cC;

    g1 = point2 - point1;
    g0 = q - point1;
    cA = cross(g0, g1);
    cA = normalize(cA);

    g1 = point3 - point2;
    g0 = q - point2;
    cB = cross(g0, g1);
    cB = normalize(cB);

    g1 = point1 - point3;
    g0 = q - point3;
    cC = cross(g0, g1);
    cC = normalize(cC);

    float dota, dotb, dotc;
    dota = dot(abc, cA);
    dotb = dot(abc, cB);
    dotc = dot(abc, cC);

    if ((abs(dota) < 0.05f) && (abs(dotb) < 0.05f) && (abs(dotc) < 0.05f))//zero
    {
        //(*justptr)++;
        g_output_PickNoBone[0].result[0] = 1;
        g_output_PickNoBone[0].result[1] = 1;
        g_output_PickNoBone[0].result[2] = faceIndex;
        //g_output_PickNoBone[0].dbginfo[0] = 4;
        return;
    }

    if (((dota <= -0.50f) && (dotb <= -0.50f) && (dotc <= -0.50f)) ||
		((dota >= 0.50f) && (dotb >= 0.50f) && (dotc >= 0.50f))
        )
    {
        g_output_PickNoBone[0].result[0] = 1;
        g_output_PickNoBone[0].result[1] = 0;
        g_output_PickNoBone[0].result[2] = faceIndex;
        //g_output_PickNoBone[0].dbginfo[0] = 5;
        return;
    }
    else
    {
        //g_output_PickNoBone[0].dbginfo[0] = 6;
        return;
    }
}
