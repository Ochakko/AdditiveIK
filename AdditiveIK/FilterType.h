enum {
	AVGF_NONE = 0,
	AVGF_MOVING,				//�ړ�����
	AVGF_WEIGHTED_MOVING,		//���d�ړ�����
	AVGF_GAUSSIAN,				//�K�E�V�A�� 
	AVGF_MAX
};

static WCHAR stravgf[AVGF_MAX][20] =
{
	L"�Ȃ�",
	L"�ړ�����",
	L"���d�ړ�����",
	L"�K�E�V�A��"
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
