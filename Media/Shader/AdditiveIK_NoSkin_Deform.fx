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
struct CSInputData
{
    float4 pos;
};

struct CSOutputData
{
    float4 pos;
};


///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
// モデル用の定数バッファー
cbuffer ModelCb : register(b0)
{
    int4 mVertexNum;
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};



// 入力にアクセスするための変数
StructuredBuffer<CSInputData> g_inputData : register(t0);
// 出力先にアクセスするための変数
RWStructuredBuffer<CSOutputData> g_outputData : register(u0);


//[numthreads(128, 1, 1)]
//[numthreads(1, 1, 1)]
[numthreads(4, 1, 1)]
//[numthreads(8, 1, 1)]
//[numthreads(16, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    int dataIndex = DTid.x;
    if (dataIndex >= mVertexNum[0])
        return;
  
    g_outputData[dataIndex].pos = mul(mWorld, g_inputData[dataIndex].pos); // モデルの頂点をワールド座標系に変換    
   
    //g_outputData[dataIndex].projpos = mul(mView, g_outputData[dataIndex].pos);
    //g_outputData[dataIndex].projpos = mul(mProj, g_outputData[dataIndex].projpos);    
}

