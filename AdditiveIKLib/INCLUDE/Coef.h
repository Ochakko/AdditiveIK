#ifndef COEFH
#define		COEFH

#include <Windows.h>

//#include <d3dcommon.h>
//#include <dxgi.h>
//#include <d3d10_1.h>
//#include <d3d10.h>
//#include <d3dcompiler.h>
//#include <d3dx10.h>


#include <usercoef.h>

#include <vector>
#include <map>
#include <string>

class CModel;
class CMQOMaterial;

#define ALIGNED		_declspec(align(16))
#define	u_long	unsigned long


//#####################
//ID_RMENU_0を足して使う
//#####################
//Dlgからのメニューオフセットはcoef.hに　100以降はAdditiveIK.cppに
// (90)はCRetargetDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_RETARGETDLG			(90)
// (91)はCLightsDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_LIGHTSDLG			(91)
// (92)はCBlendShapeDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_BLENDSHAPEDLG		(92)
// (93)はCProjLodDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_PROJLODDLG			(93)
// (94)はCBulletDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_BULLETDLG			(94)
// (95)はCDispLimitsDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_DISPLIMITSDLG		(95)
// (96)はCRigidParamsDlgをトリガーとする呼び出し用に確保
#define MENUOFFSET_RIGIDPARAMSDLG		(96)
// (97)はCpInfoDlg2をトリガーとするCopyMotionFunc()呼び出し用に確保
#define MENUOFFSET_CPINFODLG			(97)
// (98)はDollyHistoryDlg2のOnSaveDolly()呼び出し用に確保
#define MENUOFFSET_DOLLYHISTORYDLG		(98)
// (99)はCopyHistoryDlg2のOnSearch()呼び出し用に確保
#define MENUOFFSET_COPYHISTORYDLG		(99)



//lParam
enum {
	RIGIDPARAMSDLG_OPE_CLOSE,
	RIGIDPARAMSDLG_OPE_COLIDLG,
	RIGIDPARAMSDLG_OPE_GCOLIDLG,
	RIGIDPARAMSDLG_OPE_MAX
};
//lParam
enum {
	RETARGETDLG_OPE_SELECTBVH,
	RETARGETDLG_OPE_SELECTBONE,
	RETARGETDLG_OPE_RETARGETGUI,
	RETARGETDLG_OPE_SAVEFILE,
	RETARGETDLG_OPE_LOADFILE,
	RETARGETDLG_OPE_MAX
};


#define HISTORYCOMMENTLEN	32
#define EDIT_BUFLEN_NUM		20
#define EDIT_BUFLEN_MEMO	(HISTORYCOMMENTLEN)


//2023/12/13, 2023/12/14
//for shadowmap
//シャドウマップパラメータは　この後GUIで設定可能にし g_*変数 にする予定
#define SHADOWMAP_SIZE	2048
//#define SHADOWMAP_FOV	60.0f
////2023/12/14 PROJSCALEはアセットのシーン全体の倍率を想定　シャドウライトの高さにも影響
////PROJSCALE : UnityAssetJapaneseCityで1.0f, UnityAssetTheHuntStreet1で7.0f位
//#define SHADOWMAP_PROJSCALE	1.0f
//#define SHADOWMAP_NEAR	(50.0f * SHADOWMAP_PROJSCALE)
//#define SHADOWMAP_FAR	(2000.0f * SHADOWMAP_PROJSCALE)
//#define SHADOWMAP_BIAS	0.0010f
#define SHADOWSLOTNUM	8
#define SKYSLOTNUM	8
#define FOGSLOTNUM	8
#define DOFSLOTNUM	8


#define ZPRE_ZMAX	250000.0f


//バウンダリー簡易クリッピング計算用　大きいほど見切れないが重くなる
#define CHKINVIEW_BACKPOSCOEF	1.0f



//2023/10/19 計算に使うスレッドの数　CPUコアの数に応じて調整
// 
//#define POSTIK_THREADSNUM	8
//#define CALCEUL_THREADSNUM	8
#define POSTIK_THREADSNUM		4
#define CALCEUL_THREADSNUM		4
#define POSTFKTRA_THREADSNUM	4
#define COPYW2LW_THREADSNUM		4
#define RETARGET_THREADSNUM		4
#define INITMP_THREADSNUM		4
//#define POSTIK_THREADSNUM	6
//#define CALCEUL_THREADSNUM	6
//#define POSTIK_THREADSNUM	12
//#define CALCEUL_THREADSNUM	12



#define EDITRANGEHISTORYNUM	10000

#define COLIGROUPNUM	10


//2023/11/14
//#define MAXBONENUM	2048

//2023/11/30 MAXBONENUMはシェーダ定数の配列長にも使っている(数値直書き)ので変更時にはシェーダも変更
#define MAXBONENUM	1000

//2023/12/08 ShaderTypeWndで使用
#define MAXMATERIALNUM	1000


#define PATH_LENG	2048
#define MAXMOTIONNUM	100
#define MAXMODELNUM		100
#define MAXRENUM		100

#define MAXDISPGROUPNUM	20

//lightnumの最大数はシェーダー(Media/Shader/Ochakko.fx)の変更も必要とするので注意
#define LIGHTNUMMAX		8

#define LIGHTSLOTNUM	8


//配列長が256　名前は255文字まで
#define JOINTNAMELENG	256	

#define RIGPOSINDEXMAX	30
//#define RIGMULTINDEXMAX	12	
//#define RIGMULTINDEXMAX	24
#define RIGMULTINDEXMAX	256
#define GRASSINDEXMAX	(RIGMULTINDEXMAX)


//2024/02/10 大きいfbx(TheHuntCity)+VRoidを読み込んでもメモリ不足にならない程度にREFPOSMAXNUMを小さくした
//#define REFPOSMAXNUM	10
#define REFPOSMAXNUM	4


// * 100だとOpenFile()関数のスタックサイズが大きすぎて警告が出るので * 32に変更
//#define MULTIPATH	(MAX_PATH * 100)
#define MULTIPATH	(MAX_PATH * 32)

#define COL_CONE_INDEX		0
#define COL_CAPSULE_INDEX	1
#define COL_SPHERE_INDEX	2
#define COL_BOX_INDEX		3
#define COL_MAX				4


#define CGP_GROUND	1
#define CGP_CHARA	2
#define COLOF_G		CGP_CHARA
#define COLOF_CHARA_0	(CGP_GROUND | CGP_CHARA)
#define COLOF_CHARA_1	CGP_CHARA

#define UNDOMAX 999

//boneごとのRIG配列長
#define MAXRIGNUM	10
#define MAXRIGELEMNUM	5



enum //dbg.cpp OutputToInfoWnd(INFOCOLOR_INFO, )用 
{
	INFOCOLOR_INFO,
	INFOCOLOR_WARNING,
	INFOCOLOR_ERROR,
	INFOCOLOR_MAX
};


enum 
{
	SKNUMBUTTON_0,
	SKNUMBUTTON_1,
	SKNUMBUTTON_2,
	SKNUMBUTTON_3,
	SKNUMBUTTON_4,
	SKNUMBUTTON_5,
	SKNUMBUTTON_6,
	SKNUMBUTTON_7,
	SKNUMBUTTON_8,
	SKNUMBUTTON_9,

	SKNUMBUTTON_PERIOD,
	SKNUMBUTTON_SIGNE,
	SKNUMBUTTON_CP1,
	SKNUMBUTTON_CP2,
	SKNUMBUTTON_CP3,
	SKNUMBUTTON_PS1,
	SKNUMBUTTON_PS2,
	SKNUMBUTTON_PS3,

	SKNUMBUTTON_CLEAR,
	SKNUMBUTTON_BACKSPACE,
	SKNUMBUTTON_CLOSE,
	SKNUMBUTTON_MAX
};

enum
{
	SKALNUM_Q,
	SKALNUM_W,
	SKALNUM_E,
	SKALNUM_R,
	SKALNUM_T,
	SKALNUM_Y,
	SKALNUM_U,
	SKALNUM_I,
	SKALNUM_O,
	SKALNUM_P,
	SKALNUM_A,
	SKALNUM_S,
	SKALNUM_D,
	SKALNUM_F,
	SKALNUM_G,
	SKALNUM_H,
	SKALNUM_J,
	SKALNUM_K,
	SKALNUM_L,
	SKALNUM_Z,
	SKALNUM_X,
	SKALNUM_C,
	SKALNUM_V,
	SKALNUM_B,
	SKALNUM_N,
	SKALNUM_M,

	SKALNUM_0,
	SKALNUM_1,
	SKALNUM_2,
	SKALNUM_3,
	SKALNUM_4,
	SKALNUM_5,
	SKALNUM_6,
	SKALNUM_7,
	SKALNUM_8,
	SKALNUM_9,

	SKALNUM_PERIOD,
	SKALNUM_SIGNE,
	SKALNUM_CP1,
	SKALNUM_CP2,
	SKALNUM_CP3,
	SKALNUM_PS1,
	SKALNUM_PS2,
	SKALNUM_PS3,

	SKALNUM_LS,
	SKALNUM_CLEAR,
	SKALNUM_BACKSPACE,
	SKALNUM_CLOSE,
	SKALNUM_MAX
};


enum
{
	EDITTARGET_BONE,
	EDITTARGET_CAMERA,
	EDITTARGET_MORPH,
	EDITTARGET_MAX
};

enum
{
	CHKINVIEW_LOD0,//LODが無い場合にもLOD0をセット
	CHKINVIEW_LOD1,
	CHKINVIEW_LOD2,
	CHKINVIEW_LOD3,
	CHKINVIEW_LODMAX
};


enum
{
	//CBone::m_defboneposkind

	DEFBONEPOS_NONE,
	DEFBONEPOS_FROMBP,
	DEFBONEPOS_FROMCLUSTER,
	DEFBONEPOS_FROMCALC,
	DEFBONEPOS_MAX
};

enum
{
	//g_cameraInheritMode

	CAMERA_INHERIT_ALL,
	CAMERA_INHERIT_CANCEL_NULL1,
	CAMERA_INHERIT_CANCEL_NULL2,
	CAMERA_INHERIT_MAX
};

enum
{
	//CQuaternion::Q2EulXYZusingMat()

	ROTORDER_XYZ,
	ROTORDER_YZX,
	ROTORDER_ZXY,
	ROTORDER_XZY,
	ROTORDER_YXZ,
	ROTORDER_ZYX,
	ROTORDER_MAX
};

enum
{
	BGCOL_BLACK,
	BGCOL_WHITE,
	BGCOL_BLUE,
	BGCOL_GREEN,
	BGCOL_RED,
	BGCOL_GRAY,
	BGCOL_MAX
};


enum
{
	INITMP_ROTTRA,
	INITMP_ROT,
	INITMP_TRA,
	INITMP_SCALE,
	INITMP_MAX
};

enum
{
	//for bit mask operation
	SYMROOTBONE_SAMEORG = 0,
	SYMROOTBONE_SYMDIR = 1,
	SYMROOTBONE_SYMPOS = 2
};


enum tag_befeulkind
{
	BEFEUL_ZERO,
	BEFEUL_BEFFRAME,
	BEFEUL_DIRECT
};


enum tag_axiskind
{
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,
	AXIS_MAX = 3
};

enum
{
	RIGAXIS_CURRENT_X,
	RIGAXIS_CURRENT_Y,
	RIGAXIS_CURRENT_Z,

	RIGAXIS_PARENT_X,
	RIGAXIS_PARENT_Y,
	RIGAXIS_PARENT_Z,

	RIGAXIS_GLOBAL_X,
	RIGAXIS_GLOBAL_Y,
	RIGAXIS_GLOBAL_Z,

	RIGAXIS_NODE_X,
	RIGAXIS_NODE_Y,
	RIGAXIS_NODE_Z,

	RIGAXIS_MAX
};


//Bone : m_upkind
enum {
	UPVEC_NONE,
	UPVEC_X,
	UPVEC_Y,
	UPVEC_Z,
	UPVEC_MAX
};

//for bvhelem
enum {
	CHANEL_XPOS,
	CHANEL_YPOS,
	CHANEL_ZPOS,
	CHANEL_ZROT,
	CHANEL_XROT,
	CHANEL_YROT,
	CHANEL_MAX
};


enum {
	FB_NORMAL,
	FB_BUNKI_PAR,
	FB_BUNKI_CHIL,
	FB_ROOT,
	FB_ENDJOINT,
	FB_MAX
};

enum {
	INVAL_ONLYDEFAULT,
	INVAL_ALL,
	INVAL_MAX
};

enum {
	OBJFROM_MQO,
	OBJFROM_FBX,
	OBJFROM_MAX
};


//BoneTriangle
enum {
	BT_PARENT,
	BT_CHILD,
	BT_3RD,
	BT_MAX
};

enum {
	MKIND_ROT,
	MKIND_MV,
	MKIND_SC,
	MKIND_MAX
};

enum {
	FUGOU_ZERO,
	FUGOU_PLUS,
	FUGOU_MINUS
};

enum {
	BONEAXIS_CURRENT = 0,
	BONEAXIS_PARENT = 1,
	BONEAXIS_GLOBAL = 2,
	BONEAXIS_BINDPOSE = 3,
	BONEAXIS_MAX
};



typedef struct tag_anglelimit
{
	int limitoff[AXIS_MAX];
	int via180flag[AXIS_MAX];// if flag is 1, movable range is "lower --> -180(+180) --> upper"

	int boneaxiskind;//BONEAXIS_CURRENT or BONEAXIS_PARENT or BONEAXIS_GLOBAL

	//from -180 to 180 : the case lower -180 and upper 180, limit is off.
	int lower[AXIS_MAX];
	int upper[AXIS_MAX];

	bool applyeul[AXIS_MAX];
	float chkeul[AXIS_MAX];

	int limitrate;//2024/04/17 PhysicalAngleLimit

	void Init()
	{
		ZeroMemory(limitoff, sizeof(int) * AXIS_MAX);
		ZeroMemory(via180flag, sizeof(int) * AXIS_MAX);
		boneaxiskind = BONEAXIS_CURRENT;
		int axisno;
		for (axisno = 0; axisno < AXIS_MAX; axisno++) {
			lower[axisno] = -180;
			upper[axisno] = 180;
			applyeul[axisno] = false;
			chkeul[axisno] = 0.0f;
		}
		limitrate = 85;
	};

	tag_anglelimit() {
		Init();
	};
}ANGLELIMIT;

typedef struct tag_rigtrans
{
	int axiskind;
	float applyrate;// from -100.0f to 100.0f
	int enable;//enable 1, disable 0

	void Init() {
		axiskind = 0;
		applyrate = 0.0f;
		enable = 0;
	};
	tag_rigtrans() {
		Init();
	};
}RIGTRANS;

typedef struct tag_rigelem
{
	int rigrigboneno;
	int rigrigno;
	int boneno;
	RIGTRANS transuv[2];

	void Init() {
		rigrigboneno = -1;
		rigrigno = -1;
		boneno = -1;
		transuv[0].Init();
		transuv[1].Init();
	};

	tag_rigelem() {
		Init();
	};
}RIGELEM;

enum {
	RIGSHAPE_SPHERE,
	RIGSHAPE_RINGX,
	RIGSHAPE_RINGY,
	RIGSHAPE_RINGZ,
	RIGSHAPE_MAX
};

enum {
	RIGCOLOR_RED,
	RIGCOLOR_GREEN,
	RIGCOLOR_BLUE,
	RIGCOLOR_MAX
};

typedef struct tag_customrig
{
	int useflag;//0 : free, 1 : rental, 2 : valid and in use
	int rigno;//CUSTOMRIGを配列で持つ側のためのCUSTOMRIGのindex
	int rigboneno;
	int elemnum;
	WCHAR rigname[256];
	RIGELEM rigelem[MAXRIGELEMNUM];
	int dispaxis;//操作用オブジェクト表示場所（軸） ver1.0.0.19から
	int disporder;//操作用オブジェクト表示場所（番目） ver1.0.0.19から
	bool posinverse;//操作用オブジェクト位置マイナス位置フラグ　ver1.0.0.19から
	int shapemult;//操作用オブジェクト形状表示倍率インデックス　ver1.2.0.14 RC2から
	int shapekind;//RIGSHAPE_SPHERE, RIGSHAPE_RINGX, RIGSHAPE_RINGY, RIGSHAPE_RINGZ　ver1.2.0.14 RC3から
	int rigcolor;//RIGCOLOR_RED, RIGCOLOR_GREEN, RIGCOLOR_BLUE　ver1.2.0.14 RC3から

	void Init() {
		useflag = 0;
		rigno = -1;
		rigboneno = -1;
		elemnum = 0;
		ZeroMemory(rigname, sizeof(WCHAR) * 256);
		int elemno;
		for (elemno = 0; elemno < MAXRIGELEMNUM; elemno++) {
			rigelem[elemno].Init();
		}
		dispaxis = 0;
		disporder = 0;
		posinverse = false;
		shapemult = 0;
		shapekind = RIGSHAPE_SPHERE;
		rigcolor = RIGCOLOR_RED;
	};

	tag_customrig() {
		Init();
	};
}CUSTOMRIG;

typedef struct tag_footrigelem
{
	bool enablefootrig;
	CModel* groundmodel;
	CBone* leftfootbone;
	CBone* rightfootbone;
	CUSTOMRIG leftrig;
	CUSTOMRIG rightrig;
	int leftdir;
	int rightdir;
	float leftoffset;
	float rightoffset;
	float hdiffmax;
	float rigstep;
	int maxcalccount;//2024/09/08
	bool gpucollision;//2024/09/15
	float hopyperstep;//2024/09/16 hop Y per step
	float wmblend;//2024/10/06 blend rate of ModelWorldMat

	void Init() {
		enablefootrig = false;
		groundmodel = nullptr;
		leftfootbone = nullptr;
		rightfootbone = nullptr;
		leftrig.Init();
		rightrig.Init();
		leftdir = 0;
		rightdir = 0;
		leftoffset = 0.0f;
		rightoffset = 0.0f;
		hdiffmax = 100.0f;//2024/09/21 40-->100
		rigstep = 1.0f;//2024/09/21 0.25-->1.0
		maxcalccount = 15;//2024/09/21 50-->15
		gpucollision = false;
		hopyperstep = 100.0f;
		wmblend = 0.0f;
	};

	tag_footrigelem()
	{
		Init();
	};

	bool IsEnable()
	{
		if (enablefootrig) {
			if (leftfootbone && (leftrig.useflag == 2) &&
				rightfootbone && (rightrig.useflag == 2) &&
				groundmodel &&
				(maxcalccount > 0) && (maxcalccount <= 100)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	};

}FOOTRIGELEM;


typedef struct tag_hinfo
{
	float minh;
	float maxh;
	float height;

	tag_hinfo() {
		minh = FLT_MAX;//有効値で置き換わるようにMAX
		maxh = -FLT_MAX;//有効値で置き換わるようにMIN(-MAX)
		height = 0.0f;
	};
}HINFO;

typedef struct tag_reinfo
{
	char filename[MAX_PATH];
	float btgscale;
	void Init() {
		ZeroMemory(filename, sizeof(char) * MAX_PATH);
		btgscale = 1.0f;
	};
	tag_reinfo() {
		Init();
	};
}REINFO;

typedef struct tag_boneinfluence
{
	DWORD Bone;
    DWORD numInfluences;
    DWORD *vertices;
    FLOAT *weights;

	tag_boneinfluence() {
		Bone = 0;
		numInfluences = 0;
		vertices = 0;
		weights = 0;
	};
}BONEINFLUENCE;


typedef struct tag_tlelem
{
	int menuindex;
	int motionid;

	tag_tlelem() {
		menuindex = 0;
		motionid = 0;
	};
}TLELEM;

typedef struct tag_modelelem
{
	CModel* modelptr;
	std::vector<TLELEM> tlarray;
	int motmenuindex;
	std::map<int, int> lineno2boneno;
	std::map<int, int> boneno2lineno;

	void Init()
	{
		modelptr = 0;
		tlarray.clear();
		motmenuindex = 0;
		lineno2boneno.clear();
		boneno2lineno.clear();
	};

	tag_modelelem() {
		Init();
	};
}MODELELEM;


typedef struct tag_xmliobuf
{
	char* buf;
	int bufleng;
	int pos;
	int isend;

	tag_xmliobuf() {
		buf = 0;
		bufleng = 0;
		pos = 0;
		isend = 0;
	};
}XMLIOBUF;

enum {
	XMLIO_LOAD,
	XMLIO_WRITE,
	XMLIO_MAX
};

#define XMLIOLINELEN	1024


//enum {
//	PICK_NONE,
//	PICK_CENTER,
//	PICK_X,
//	PICK_Y,
//	PICK_Z,
//	PICK_CAMMOVE,
//	PICK_CAMROT,
//	PICK_CAMDIST,
//	PICK_SPA_X,
//	PICK_SPA_Y,
//	PICK_SPA_Z,
//	PICK_MAX
//};
enum {
	PICK_NONE,
	PICK_CENTER,
	PICK_X,
	PICK_Y,
	PICK_Z,
	PICK_CAMROT,
	PICK_CAMMOVE,
	PICK_CAMDIST,
	PICK_SPA_X,
	PICK_SPA_Y,
	PICK_SPA_Z,
	PICK_UNDO,
	PICK_REDO,
	PICK_MAX
};


//enum {
//	SPR_CAM_I = 0,
//	SPR_CAM_KAI = 1,
//	SPR_CAM_KAKU = 2,
//	SPR_CAM_MAX
//};
enum {
	SPR_CAM_KAI = 0,
	SPR_CAM_I = 1,
	SPR_CAM_KAKU = 2,
	SPR_CAM_MAX
};


typedef struct tag_motinfo
{
	char motname[256];
	WCHAR wfilename[MAX_PATH];
	char engmotname[256];
	int motid;
	double frameleng;
	double curframe;
	double befframe;
	double speed;
	int loopflag;
	int fbxanimno;//fbxファイルの中で何番目のモーションとして読み込んだか　0から始まる番号
	bool cameramotion;

	void Init() {
		ZeroMemory(motname, sizeof(char) * 256);
		ZeroMemory(wfilename, sizeof(WCHAR) * MAX_PATH);
		ZeroMemory(engmotname, sizeof(char) * 256);
		motid = 0;
		frameleng = 0.0;
		curframe = 0.0;
		befframe = 0.0;
		speed = 1.0;
		loopflag = 0;
		fbxanimno = -1;
		cameramotion = false;
	};

	tag_motinfo() {
		Init();
	};
}MOTINFO;


#define INFSCOPEMAX	10
#define INFNUMMAX	4

typedef struct tag_infelem
{
	int boneno;
	int kind;//CALCMODE_*
	float userrate;//
	float orginf;//CALCMODE_*で計算した値。
	float dispinf;//　orginf[] * userrate[]、normalizeflagが１のときは、正規化する。
	int isadditive;//影響度の正規化無し。

	void Init() {
		boneno = 0;
		kind = 0;
		userrate = 0.0f;
		orginf = 0.0f;
		dispinf = 0.0f;
		isadditive = 0;
	};

	tag_infelem() {
		Init();
	};
}INFELEM;


typedef struct rgbdat
{
	unsigned char b;
	unsigned char g;
	unsigned char r;

	rgbdat() {
		b = 0;
		g = 0;
		r = 0;
	};
} RGBDAT;

typedef struct rgb3f
{
	float r;
	float g;
	float b;

	rgb3f() {
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
	};
} RGB3F;

typedef struct tag_argbfdat
{
	float a;
	float r;
	float g;
	float b;

	tag_argbfdat() {
		a = 1.0f;
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
	};
} ARGBF;

// material mode
enum {
	MMODE_VERTEX,
	MMODE_FACE,
	MMODE_MAX
};

typedef struct tag_infdata
{
	int m_infnum;
	INFELEM m_infelem[INFNUMMAX];

	tag_infdata() {
		m_infnum = 0;
		int infno;
		for (infno = 0; infno < INFNUMMAX; infno++) {
			m_infelem[infno].Init();
		}
	};
} INFDATA;

typedef struct tag_chkalpha
{
	int alphanum;
	int notalphanum;

	tag_chkalpha() {
		alphanum = 0;
		notalphanum = 0;
	};
} CHKALPHA;


typedef struct tag_mqobuf
{
	HANDLE hfile;
	unsigned char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;

	tag_mqobuf() {
		hfile = INVALID_HANDLE_VALUE;
		buf = 0;
		bufleng = 0;
		pos = 0;
		isend = 0;
	};
} MQOBUF;


enum
{
	MATERIAL_DIFFUSE,
	MATERIAL_SPECULAR,
	MATERIAL_AMBIENT,
	MATERIAL_EMISSIVE,
	MATERIAL_POWER,
	MATERIAL_MAX
};


typedef struct tag_materialblock
{
	int materialno;
	int startface;
	int endface;
	CMQOMaterial* mqomat;

	tag_materialblock() {
		materialno = 0;
		startface = 0;
		endface = 0;
		mqomat = 0;
	};
} MATERIALBLOCK;

typedef struct tag_dirtymat
{
	int materialno;
	int* dirtyflag;

	tag_dirtymat() {
		materialno = 0;
		dirtyflag = 0;
	}
} DIRTYMAT;

typedef struct tag_vcoldata
{
	int vertno;
	__int64 vcol;

	tag_vcoldata() {
		vertno = 0;
		vcol = (__int64)0;
	}
} VCOLDATA;

enum {
	SYMMTYPE_NONE,
	SYMMTYPE_L,
	SYMMTYPE_R,
	SYMMTYPE_M,
	SYMMTYPE_MAX
};


typedef struct tag_infelemheader
{
	int infnum;
	int normalizeflag;
	int symaxis;
	float symdist;

	tag_infelemheader() {
		infnum = 0;
		normalizeflag = 0;
		symaxis = 0;
		symdist = 0.0f;
	};
}INFELEMHEADER;

enum {
	MIKOBONE_NONE,
	MIKOBONE_NORMAL,
	MIKOBONE_FLOAT,
	MIKOBONE_NULL,
	MIKOBONE_ILLEAGAL,
	MIKOBONE_MAX
};

enum {
	FBXBONE_NONE,
	FBXBONE_SKELETON,
	FBXBONE_NULL,
	FBXBONE_ROOTNODE,
	FBXBONE_CAMERA,//2023/05/23
	FBXBONE_OTHER,
	FBXBONE_MAX
};

enum {
	MIKODEF_NONE,
	MIKODEF_SDEF,
	MIKODEF_BDEF,
	MIKODEF_NODEF,
	MIKODEF_MAX
};

enum {
	MIKOBLEND_SKINNING,
	MIKOBLEND_MIX,
	MIKOBLEND_MAX
};

enum {
	PARSMODE_PARS,
	PARSMODE_ORTHO,
	PARSMODE_MAX
};

typedef struct tag_tvertex {
	float pos[4]; 
	float tex[2];

	tag_tvertex() {
		ZeroMemory(pos, sizeof(float) * 4);
		ZeroMemory(tex, sizeof(float) * 2);
	};
} TVERTEX;

typedef struct tag_tlvertex {
	float pos[4];
	float diffuse[4];
	float tex[2];

	tag_tlvertex() {
		ZeroMemory(pos, sizeof(float) * 4);
		ZeroMemory(diffuse, sizeof(float) * 4);
		ZeroMemory(tex, sizeof(float) * 2);
	};
} TLVERTEX;



typedef struct tag_pm3inf
{
	float weight[4];
	int boneindex[4];

	tag_pm3inf() {
		ZeroMemory(weight, sizeof(float) * 4);
		ZeroMemory(boneindex, sizeof(int) * 4);
	};
}PM3INF;

typedef struct tag_brushstate
{
	int motionbrush_method;
	int wallscrapingikflag;
	bool limitdegflag;
	int brushmirrorUflag;
	int brushmirrorVflag;
	int ifmirrorVDiv2flag;
	int brushrepeats;

	void Init() {
		motionbrush_method = 0;
		wallscrapingikflag = 0;
		limitdegflag = false;
		brushmirrorUflag = 0;
		brushmirrorVflag = 0;
		ifmirrorVDiv2flag = 0;
		brushrepeats = 1;
	};

	tag_brushstate()
	{
		Init();
	};
}BRUSHSTATE;


// error code
// d3dapp.h から移動。

#ifndef D3DAPPERR_NODIRECT3D
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c

#endif

typedef struct tag_errormes {
	int errorcode;
	DWORD type;
	char* mesptr;

	tag_errormes() {
		errorcode = 0;
		type = 0;
		mesptr = 0;
	};
} ERRORMES;

//interpolation
enum {
	INTERPOLATION_SLERP,
	INTERPOLATION_SQUAD,
	INTERPOLATION_MAX
};

//user defined window message

#define WM_USER_DISPLAY		WM_USER


////////////////////////////////
#define BITMASK_0	0x00000001
#define BITMASK_1	0x00000002
#define BITMASK_2	0x00000004
#define BITMASK_3	0x00000008

#define BITMASK_4	0x00000010
#define BITMASK_5	0x00000020
#define BITMASK_6	0x00000040
#define BITMASK_7	0x00000080

#define BITMASK_8	0x00000100
#define BITMASK_9	0x00000200
#define BITMASK_10	0x00000400
#define BITMASK_11	0x00000800

#define BITMASK_12	0x00001000
#define BITMASK_13	0x00002000
#define BITMASK_14	0x00004000
#define BITMASK_15	0x00008000

#define BITMASK_16	0x00010000
#define BITMASK_17	0x00020000
#define BITMASK_18	0x00040000
#define BITMASK_19	0x00080000

#define BITMASK_20	0x00100000
#define BITMASK_21	0x00200000
#define BITMASK_22	0x00400000
#define BITMASK_23	0x00800000

#define BITMASK_24	0x01000000
#define BITMASK_25	0x02000000
#define BITMASK_26	0x04000000
#define BITMASK_27	0x08000000

#define BITMASK_28	0x10000000
#define BITMASK_29	0x20000000
#define BITMASK_30	0x40000000
#define BITMASK_31	0x80000000

////////////////////////////////
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)
#define D3DFVF_TLVERTEX2 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)

#define D3DFVF_VERTEX	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

#define D3DFVF_LVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)


//		mottype : motno の制御方法。
enum _motiontype {
	MOTIONTYPENONE,

	MOTION_STOP, // motionno 固定。
	MOTION_CLAMP, // motionno が　maxに達したら、そのままそこで固定。
	MOTION_ROUND, //		最初に戻る
	MOTION_INV,	//			逆方向に進む。
	MOTION_JUMP,
	//MOTION_BIV, // 落ち着いたら、バイブレーションも追加。 

	MOTIONTYPEMAX
};



////////////////////


#ifndef COEFHSTRING

#else

/***
	char strskel[ SKEL_MAX ][30] = {
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

	char strconvskel[ SKEL_MAX ][30] = {
		"00TOP_OF_JOINT",
		"01SKEL_TORSO",
		"02SKEL_LEFT_HIP",
		"03SKEL_LEFT_KNEE",
		"04SKEL_LEFT_FOOT",
		"05SKEL_RIGHT_HIP",
		"06SKEL_RIGHT_KNEE",
		"07SKEL_RIGHT_FOOT",
		"08SKEL_NECK",
		"09SKEL_HEAD",
		"10SKEL_LEFT_SHOULDER",
		"11SKEL_LEFT_ELBOW",
		"12SKEL_LEFT_HAND",
		"13SKEL_RIGHT_SHOULDER",
		"14SKEL_RIGHT_ELBOW",
		"15SKEL_RIGHT_HAND"
	};
***/

#endif

#endif