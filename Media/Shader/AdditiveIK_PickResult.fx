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
struct CSInputData_PickResult
{
    int4 result; //0:hitflag, 1:justflag, 2:hitfaceno, 3:未使用0
    int4 dbginfo; //0:return code, 1:index1, 2:index2, 3:index3
    float4 hitpos; //2024/09/15
};

struct CSOutputData_PickResult
{
    int4 result;//0:hitflag, 1:justflag, 2:hitfaceno, 3:未使用0
    int4 dbginfo;//0:return code, 1:index1, 2:index2, 3:index3
    float4 hitpos;//2024/09/15
};

///////////////////////////////////////////
// 定数バッファー
///////////////////////////////////////////
// モデル用の定数バッファー
cbuffer PickCb_PickResult : register(b0)
{
    int4 mBufferSize; //0:vertexnum, 1:facenum, 2:indicesnum, 3:未使用0
};


RWStructuredBuffer<CSInputData_PickResult> g_input_PickResult : register(u0);
// 出力先にアクセスするための変数
RWStructuredBuffer<CSOutputData_PickResult> g_output_PickResult : register(u1);


//出力データの同一インデックスにアクセスするので　スレッド数は１であることが必要
[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    //int faceIndex0 = DTid.x;
    //if (faceIndex0 >= mBufferSize[1])//facenoチェック
    //    return;
    
    int firstflag = 1;
    float mindist = 0.0f;
    int faceIndex;
    for (faceIndex = 0; faceIndex < mBufferSize[1]; faceIndex++)
    {
        if (g_input_PickResult[faceIndex].result[0] == 1)
        {
            float chkdist = g_input_PickResult[faceIndex].hitpos.w;
            if ((firstflag == 1) || (mindist > chkdist))
            {
                mindist = chkdist;
                
                g_output_PickResult[0] = g_input_PickResult[faceIndex];
                firstflag = 0;
            }
        }
    }
}
