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
struct CSInputData
{
    float4 pos;
    float4 bweight;
    int4 bindices;
};

struct CSOutputData
{
    float4 pos;
    float4 bweight;
    int4 bindices;
};

///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
cbuffer ModelCb : register(b0)
{
    int4 mVertexNum;
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
    float4x4 mBoneMat[1000];
};


// 入力にアクセスするための変数
StructuredBuffer<CSInputData> g_inputData : register(t0);
// 出力先にアクセスするための変数
RWStructuredBuffer<CSOutputData> g_outputData : register(u0);


//[numthreads(128, 1, 1)]
//[numthreads(1, 1, 1)]
//[numthreads(4, 1, 1)]
//[numthreads(8, 1, 1)]
[numthreads(16, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    int dataIndex = DTid.x;
    if (dataIndex >= mVertexNum[0])
        return;
    
    matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        finalmat += (mBoneMat[g_inputData[dataIndex].bindices[i]] * g_inputData[dataIndex].bweight[i]);
    }
        
    g_outputData[dataIndex].pos = mul(finalmat, g_inputData[dataIndex].pos);    
    //g_outputData[dataIndex].projpos = mul(mView, g_outputData[dataIndex].pos);
    //g_outputData[dataIndex].projpos = mul(mProj, g_outputData[dataIndex].projpos);
    
    //g_outputData[dataIndex].bweight = g_inputData[dataIndex].bweight;
    //g_outputData[dataIndex].bindices = g_inputData[dataIndex].bindices;    
}
