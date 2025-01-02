#ifndef USERCOEFH
#define USERCOEFH



//#ifndef COEFH
//#include <d3dcommon.h>
//#include <dxgi.h>
//#include <d3d10_1.h>
//#include <d3d10.h>
//#include <d3dcompiler.h>
//#include <d3dx10.h>
//#endif


//#################################################
//2022/12/16 PI for double 電卓アプリ最大桁まで
//#################################################
#define PI          3.1415926535897932384626433832795
#define	PAI			3.1415926535897932384626433832795
#define	PAIDIV2		1.5707963267948966192313216916398
#define	PAIDIV4		0.78539816339744830961566084581988
#define	PAI2DEG		57.295779513082320876798154814105
#define	DEG2PAI		0.01745329251994329576923690768489
#define D2R			0.01745329251994329576923690768489


#define	DEGXZINITVAL	0.0f
#define DEGYINITVAL		0.0f
#define	EYEYINITVAL	750.0f
#define	CAMDISTINITVAL	2900.0f

//#define MAXBONENUM	81
//#define MAXCLUSTERNUM	70
//#define MAXCLUSTERNUM	70
#define MAXCLUSTERNUM	200



enum {
	INDX,
	INDY,
	INDZ,
	INDMAX
};

typedef struct tag_vec3f
{
	float x;
	float y;
	float z;

	tag_vec3f() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};
} VEC3F;


typedef struct tag_dvec3
{
	double x;
	double y;
	double z;

	tag_dvec3() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	};
} DVEC3;


enum {
	SKEL_TOPOFJOINT,
	SKEL_TORSO,
	SKEL_LEFT_HIP,
	SKEL_LEFT_KNEE,
	SKEL_LEFT_FOOT,
	SKEL_RIGHT_HIP,
	SKEL_RIGHT_KNEE,
	SKEL_RIGHT_FOOT,
	SKEL_NECK,
	SKEL_HEAD,
	SKEL_LEFT_SHOULDER,
	SKEL_LEFT_ELBOW,
	SKEL_LEFT_HAND,
	SKEL_RIGHT_SHOULDER,
	SKEL_RIGHT_ELBOW,
	SKEL_RIGHT_HAND,
	SKEL_MAX
};


static char strskel[ SKEL_MAX ][30] = {
	"TOPOFJOINT",
	"TORSO",
	"LEFT_HIP",
	"LEFT_KNEE",
	"LEFT_FOOT",
	"RIGHT_HIP",
	"RIGHT_KNEE",
	"RIGHT_FOOT",
	"NECK",
	"HEAD",
	"LEFT_SHOULDER",
	"LEFT_ELBOW",
	"LEFT_HAND",
	"RIGHT_SHOULDER",
	"RIGHT_ELBOW",
	"RIGHT_HAND"
};

static char strconvskel[ SKEL_MAX ][30] = {
	"TOP_OF_JOINT",
	"SKEL_TORSO",
	"SKEL_LEFT_HIP",
	"SKEL_LEFT_KNEE",
	"SKEL_LEFT_FOOT",
	"SKEL_RIGHT_HIP",
	"SKEL_RIGHT_KNEE",
	"SKEL_RIGHT_FOOT",
	"SKEL_NECK",
	"SKEL_HEAD",
	"SKEL_LEFT_SHOULDER",
	"SKEL_LEFT_ELBOW",
	"SKEL_LEFT_HAND",
	"SKEL_RIGHT_SHOULDER",
	"SKEL_RIGHT_ELBOW",
	"SKEL_RIGHT_HAND"
};

typedef struct tag_tpos
{// -1から+1の値
	float ftop;
	float fleft;
	float fbottom;
	float fright;

	tag_tpos() {
		ftop = 0.0f;
		fleft = 0.0f;
		fbottom = 0.0f;
		fright = 0.0f;
	};
}TPOS;

enum {
	CAPMODE_ALL,
	CAPMODE_ONE,
	CAPMODE_MAX
};
enum {
	POSMODE_TOPOFJOINT,
	POSMODE_GLOBAL,
	POSMODE_ZERO,
	POSMODE_MAX
};



typedef struct tag_tselem
{
	int skelno;
	char jointname[256];
	int jointno;
	int twistflag;

	tag_tselem() {
		skelno = 0;
		ZeroMemory(jointname, sizeof(char) * 256);
		jointno = 0;
		twistflag = 0;
	};
}TSELEM;


enum {
	ROTAXIS_X,
	ROTAXIS_Y,
	ROTAXIS_Z,
	ROTAXIS_MAX
};

typedef struct tag_color4uc
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;

	tag_color4uc() {
		a = 255;
		r = 0;
		g = 0;
		b = 0;
	};
} COLOR4UC;

typedef struct tag_color3uc
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	tag_color3uc() {
		r = 0;
		g = 0;
		b = 0;
	};
} COLOR3UC;


typedef struct tag_color4f
{
	float r;
	float g;
	float b;
	float a;

	tag_color4f() {
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		a = 1.0f;
	};
} COLOR4F;

typedef struct RDBColor3f
{
public:
	float r;
	float g;
	float b;

	RDBColor3f() {
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
	};
} RDBColor3f;


typedef struct tag_moainfo
{
	char name[256];
	int id;

	void Init() {
		ZeroMemory(name, sizeof(char) * 256);
		id = 0;
	};
	tag_moainfo() {
		Init();
	};
} MOAINFO;

typedef struct tag_moatrunkinfo
{
	int idling;
	int ev0idle;
	int comid;
	int notcomnum;
	int* notcomid;
	int branchnum;

	void Init() {
		idling = 0;
		ev0idle = 0;
		comid = 0;
		notcomnum = 0;
		notcomid = nullptr;
		branchnum = 0;
	};
	tag_moatrunkinfo() {
		Init();
	};
} MOATRUNKINFO;

typedef struct tag_moabranchinfo
{
	int motid;
	int eventno;
	int frameno1;
	int frameno2;
	int notfu;

	void Init() {
		motid = 0;
		eventno = 0;
		frameno1 = 0;
		frameno2 = 0;
		notfu = 0;
	};
	tag_moabranchinfo() {
		Init();
	};
} MOABRANCHINFO;


enum tag_calcmode
{
	CALCMODE_NONE,//未設定
	CALCMODE_NOSKIN0,//スキニングなし
	CALCMODE_ONESKIN0,//距離と内積から計算
	CALCMODE_ONESKIN1,//距離から計算
	CALCMODE_DIRECT0,//値の直接指定
	CALCMODE_SYM,//対称設定
	CALCMODE_MAX
};

enum {
	BONETYPE_RDB2,
	BONETYPE_MIKO,
	BONETYPE_MAX
};

enum {
	SYMAXIS_NONE,
	SYMAXIS_X,
	SYMAXIS_Y,
	SYMAXIS_Z,
	SYMAXIS_MAX
};


#endif

