enum {
	AVGF_NONE = 0,
	AVGF_MOVING,				//移動平均
	AVGF_WEIGHTED_MOVING,		//加重移動平均
	AVGF_GAUSSIAN,				//ガウシアン 
	AVGF_MAX
};

static WCHAR stravgf[AVGF_MAX][20] =
{
	L"なし",
	L"移動平均",
	L"加重移動平均",
	L"ガウシアン"
};

static int avgfsize[10] =
{
	3, 5, 7, 9, 11,
	13, 15, 17, 19, 21
};

static  WCHAR stravgfsize[10][5] =
{
	L"3", L"5", L"7", L"9", L"11",
	L"13", L"15", L"17", L"19", L"21"
};
