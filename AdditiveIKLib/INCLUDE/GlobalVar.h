#ifndef GLOBALVARH
#define GLOBALVARH

#include <Windows.h>

#include "../../MiniEngine/MiniEngine.h"


//#ifdef CONVD3DX11
//// Direct3D11 includes
//#include <d3dcommon.h>
//#include <dxgi.h>
//#include <d3d11_1.h>
//#include <d3d11_2.h>
//#include <d3dcompiler.h>
//#ifdef USE_DIRECT3D11_3
//#include <d3d11_3.h>
//#endif
//#ifdef USE_DIRECT3D11_4
//#include <d3d11_4.h>
//#endif
//// DirectXMath includes
//#include <DirectXMath.h>
//#include <DirectXColors.h>
//// WIC includes
//#include <wincodec.h>
//// XInput includes
//#include <xinput.h>
//// HRESULT translation for Direct3D and other APIs
////#include <dxerr.h>
//#endif


////#undef DEFINE_GUID
////#include "INITGUID.h"
//#include "e:\PG\AdditiveIK_git\AdditiveIK\Effects11\Inc\d3dx11effect.h"
//
////#include "..\..\Effects11\pchfx.h"



//class ID3D11DepthStencilState;
//class ID3D11ShaderResourceView;
//class ID3DX11EffectTechnique;
//class ID3DX11EffectMatrixVariable;
//class ID3DX11EffectVectorVariable;
//class ID3DX11EffectScalarVariable;
//class ID3DX11EffectShaderResourceVariable;



//#include <d3dx10.h>
#include <ChaVecCalc.h>
#include <TexBank.h>


class CInfoWindow;

#ifdef ADDITIVEIKLIBGLOBALVAR

WCHAR g_brushname[MAX_PATH] = { 0 };
int g_fogindex = 0;
CFogParams g_fogparams[FOGSLOTNUM];
int g_dofindex = 0;
ChaVector4 g_dofparams[DOFSLOTNUM];
bool g_skydofflag[DOFSLOTNUM];
bool g_skydispflag = true;
int g_skyindex = 0;

bool g_blendshapeAddtiveMode = false;
double g_blendshapedist = 1.0f;

bool g_refposflag = false;

bool g_pickmeshflag = false;
int g_pickorder = 1;
double g_pickdistrate = 0.5;

bool g_hdrpbloom = true;
bool g_alphablending = true;
bool g_freefps = true;
bool g_zalways = false;

bool g_pasteScale = true;
bool g_pasteRotation = true;
bool g_pasteTranslation = true;

int g_jumpgravity = 65;

int g_uvset = 0;
float g_bonemark_bright = 1.0f;
float g_rigidmark_alpha = 0.75f;
float g_rigmark_alpha = 0.75f;

float g_lodrate2L[CHKINVIEW_LODMAX];
float g_lodrate3L[CHKINVIEW_LODMAX];
float g_lodrate4L[CHKINVIEW_LODMAX];


float g_thdeg = 181.0f;
float g_thdeg_endjoint = 159.0f;
float g_thRoundX = 179.0f;
float g_thRoundY = 179.0f;
float g_thRoundZ = 179.0f;


bool g_enableshadow = true;
bool g_VSMflag = true;
bool g_blurShadow = true;
int g_shadowmap_slotno = 0;
float g_shadowmap_fov[SHADOWSLOTNUM];
float g_shadowmap_projscale[SHADOWSLOTNUM];
float g_shadowmap_near[SHADOWSLOTNUM];
float g_shadowmap_far[SHADOWSLOTNUM];
float g_shadowmap_color[SHADOWSLOTNUM];
float g_shadowmap_bias[SHADOWSLOTNUM];
float g_shadowmap_plusup[SHADOWSLOTNUM];
float g_shadowmap_distscale[SHADOWSLOTNUM];
int g_shadowmap_lightdir[SHADOWSLOTNUM];

bool g_zpreflag = false;
bool g_zcmpalways = false;
bool g_rotatetanim = false;
bool g_tpose = true;
bool g_preciseOnPreviewToo = false;
bool g_x180flag = false;
//bool g_edgesmp = false;

float g_AmbientFactorAtLoading = 1.0f;
float g_DiffuseFactorAtLoading = 1.0f;
float g_SpecularFactorAtLoading = 1.0f;
float g_EmissiveFactorAtLoading = 1.0f;
float g_AmbientFactorAtSaving = 1.0f;
float g_DiffuseFactorAtSaving = 1.0f;
float g_SpecularFactorAtSaving = 1.0f;
float g_EmissiveFactorAtSaving = 1.0f;

bool g_VSync = false;
int g_fpskind = 0;
bool g_HighRpmMode = true;
int g_UpdateMatrixThreads = 2;
int g_lightflag = 1;

int g_usephysik = 0;
int g_ClearColorIndex = 0;
float g_ClearColor[BGCOL_MAX][4] = {
	{0.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{0.0f, 0.5f, 0.25f, 1.0f},
	{1.0f, 0.5f, 0.5f, 1.0f}
};

bool g_bakelimiteulonsave = false;

double g_befeuldiffmax = 60.0;
int g_dsmousewait = 0;
HBITMAP g_mouseherebmp = 0;
COLORREF g_tranbmp = 0;
float g_mouseherealpha;
int g_iklevel = 1;
CInfoWindow* g_infownd = 0;
int g_endappflag = 0;
//ID3D11DepthStencilState *g_pDSStateZCmp = 0;
//ID3D11DepthStencilState *g_pDSStateZCmpAlways = 0;
//ID3D11ShaderResourceView* g_presview = 0;

bool g_underloading = false;
int g_underselectingframe = 0;
//bool g_underIKRot = false;
//bool g_underIKRotApplyFrame = false;
bool g_fpsforce30 = false;
bool g_underWriteFbx = false;
//bool g_underCalcEul = false;
//bool g_underPostFKTra = false;
//bool g_underInitMp = false;
//bool g_underCopyW2LW = false;
bool g_changeUpdateThreadsNum = false;

int	g_numthread = 3;

//double g_btcalccnt = 3.0;
//double g_btcalccnt = 1.0;//2023/01/18
//double g_btcalccnt = 2.0;//2023/01/21 : Main.cpp InitApp()内でセットし直しているので注意


//2023/11/04 物理ダブルバッファ化で表示速度が速くなったので　2だった値を1に変更
//Main.cpp InitApp()内でセットし直しているので注意
double g_btcalccnt = 1.0;


int g_dbgloadcnt = 0;
double g_calcfps = 60.0;

bool g_selecttolastFlag = false;
bool g_underselecttolast = false;
bool g_undereditrange = false;

//bool g_limitdegflag = true;
bool g_limitdegflag = false;


//2024/04/17 limitrateは構造体ANGLELIMITのメンバにしてCBone::m_anglelimitでボーンごとに管理することにした
//int g_limitrate = 15;
//int g_limitrate = 85;//2024/04/15 limitrateが実質FreeRateになっていたので修正　新しいlimitrate = (100 - 古いlimitrate)
//double g_physicalLimitScale = 1.0;//全てのCBoneのlimitrateに掛けるスケール
//int g_physicalMovableRate = 25;//2024/05/02 LimitEulオンで物理シミュをする場合の　制限角度内における動作変化率％
double g_physicalLimitScale = 0.0;//全てのCBoneのlimitrateに掛けるスケール limitを0に設定しても制限角度を越えるとフラグが立って速度設定が変わる
int g_physicalMovableRate = 100;//2024/05/02 LimitEulオンで物理シミュをする場合の　制限角度内における動作変化率％
double g_physicalVeloScale = 0.20;//2024/05/04 LimitEulオンで物理シミュする場合の　剛体速度スケール

//bool g_wmatDirectSetFlag = false;
//bool g_underRetargetFlag = false;

float g_impscale = 1.0f;
float g_initmass = 1.0f;

ChaVector3 g_camEye = ChaVector3(0.0f, 0.0f, 0.0f);
ChaVector3 g_camtargetpos = ChaVector3(0.0f, 0.0f, 0.0f);
ChaVector3 g_befcamEye = ChaVector3(0.0f, 0.0f, 0.0f);
ChaVector3 g_befcamtargetpos = ChaVector3(0.0f, 0.0f, 0.0f);
ChaVector3 g_cameraupdir = ChaVector3(0.0f, 1.0f, 0.0f);
float g_camdist = 50.0f;
float g_initcamdist = 50.0f;
float g_fovy = (float)(PI / 4.0);
float g_projnear = 0.01f;
float g_projfar = g_initcamdist * 100.0f;


float g_l_kval[3] = { 1.0f, powf(10.0f, 2.61f), 2000.0f };//
float g_a_kval[3] = { 0.1f, powf(10.0f, 0.3f), 70.0f };
//float g_a_kval[3] = { 0.0f, 0.5f, 1.0f };//2023/01/18
float g_initcuslk = 1e2;
//float g_initcuslk = 2000.0f;
//float g_initcuslk = 100.0f;
//float g_initcusak = 70.0f;
//float g_initcusak = 0.5f;
float g_initcusak = 0.10f;
float g_akscale = 1.0f;//2024/04/23 全ての剛体の回転バネ定数に対するスケール

//float g_l_dmp = 0.75f;
//float g_a_dmp = 0.50f;

float g_l_dmp = 0.50f;
float g_a_dmp = 0.50f;

//int g_shadertype = -1;//マテリアル毎に設定することに
int g_previewFlag = 0;			// プレビューフラグ
int g_cameraanimmode = 0;//0: OFF, 1:ON, 2:ON and RootMotionOption ON
int g_cameraInheritMode = CAMERA_INHERIT_ALL;

int g_applyendflag = 0;
int g_slerpoffflag = 0;
int g_absikflag = 0;
int g_bonemarkflag = 1;
int g_rigidmarkflag = 1;
int g_pseudolocalflag = 1;
//int g_wallscrapingikflag = 1;
int g_wallscrapingikflag = 0;
int g_prepcntonphysik = 30;
//int g_boneaxis = 1;//parent
int g_boneaxis = 0;//current 2022/10/31
int g_brushrepeats = 1;
int g_brushmirrorUflag = 0;
int g_brushmirrorVflag = 0;
int g_ifmirrorVDiv2flag = 0;

CTexBank*	g_texbank = 0;

float g_tmpmqomult = 1.0f;
WCHAR g_tmpmqopath[MULTIPATH] = { 0L };
float g_tmpbvhfilter = 100.0f;

//ID3D11BlendState* g_blendState = 0;
//
//
//ID3DX11Effect* g_pEffect = 0;
//
//ID3DX11EffectTechnique* g_hRenderBoneL0 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL1 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL2 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL3 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL4 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL5 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL6 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL7 = 0;
//ID3DX11EffectTechnique* g_hRenderBoneL8 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL0 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL1 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL2 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL3 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL4 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL5 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL6 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL7 = 0;
//ID3DX11EffectTechnique* g_hRenderNoBoneL8 = 0;
//ID3DX11EffectTechnique* g_hRenderLine = 0;
//ID3DX11EffectTechnique* g_hRenderSprite = 0;
//
//ID3DX11EffectMatrixVariable* g_hm4x4Mat = 0;
//ID3DX11EffectMatrixVariable* g_hmWorld = 0;
//ID3DX11EffectMatrixVariable* g_hmVP = 0;
//
//ID3DX11EffectVectorVariable* g_hEyePos = 0;
//ID3DX11EffectScalarVariable* g_hnNumLight = 0;
//ID3DX11EffectVectorVariable* g_hLightDir = 0;
//ID3DX11EffectVectorVariable* g_hLightDiffuse = 0;
//ID3DX11EffectVectorVariable* g_hLightAmbient = 0;
//ID3DX11EffectVectorVariable* g_hSpriteOffset = 0;
//ID3DX11EffectVectorVariable* g_hSpriteScale = 0;
//ID3DX11EffectVectorVariable* g_hPm3Scale = 0;
//ID3DX11EffectVectorVariable* g_hPm3Offset = 0;
//
//
//ID3DX11EffectVectorVariable* g_hdiffuse = 0;
//ID3DX11EffectVectorVariable* g_hambient = 0;
//ID3DX11EffectVectorVariable* g_hspecular = 0;
//ID3DX11EffectScalarVariable* g_hpower = 0;
//ID3DX11EffectVectorVariable* g_hemissive = 0;
//ID3DX11EffectShaderResourceVariable* g_hMeshTexture = 0;

BYTE g_keybuf[256];
BYTE g_savekeybuf[256];

WCHAR g_basedir[MAX_PATH] = { 0 };

double						g_dspeed = 1.0;
//double						g_dspeed = 0.0;


float g_ikfirst = 1.0f;
float g_ikrate = 1.0f;
double g_applyrate = 50.0;
int g_refposstep = 10;
int g_refalpha = 50;
//float g_physicsmvrate = 1.0f;
////float g_physicsmvrate = 0.1f;
//float g_physicsmvrate = 0.5f;
float g_physicsmvrate = 0.3f;

float g_fLightScale;
int g_nNumActiveLights;
//int                         g_nActiveLight;
int g_lightSlot = 0;
ChaVector3 g_lightDir[LIGHTSLOTNUM][LIGHTNUMMAX];
ChaVector3 g_lightDiffuse[LIGHTSLOTNUM][LIGHTNUMMAX];
bool g_lightEnable[LIGHTSLOTNUM][LIGHTNUMMAX];
bool g_lightDirWithView[LIGHTSLOTNUM][LIGHTNUMMAX];
float g_lightScale[LIGHTSLOTNUM][LIGHTNUMMAX];
int g_lightNo[LIGHTNUMMAX];

int g_motionbrush_method = 0;
double g_motionbrush_startframe = 0.0;
double g_motionbrush_endframe = 0.0;
double g_motionbrush_applyframe = 0.0;
double g_motionbrush_numframe = 0.0;
int g_motionbrush_frameleng = 0;
float* g_motionbrush_value = 0;
double g_playingstart = 1.0;
double g_playingend = 1.0;



#else
extern WCHAR g_brushname[MAX_PATH];
extern int g_fogindex;
extern CFogParams g_fogparams[FOGSLOTNUM];
extern int g_dofindex;
extern ChaVector4 g_dofparams[DOFSLOTNUM];
extern bool g_skydofflag[DOFSLOTNUM];
extern bool g_skydispflag;
extern int g_skyindex;

extern bool g_blendshapeAddtiveMode;
extern double g_blendshapedist;

extern bool g_refposflag;

extern bool g_pickmeshflag;
extern int g_pickorder;
extern double g_pickdistrate;

extern bool g_hdrpbloom;
extern bool g_alphablending;
extern bool g_freefps;
extern bool g_zalways;

extern bool g_pasteScale;
extern bool g_pasteRotation;
extern bool g_pasteTranslation;

extern int g_jumpgravity;

extern int g_uvset;
extern float g_bonemark_bright;
extern float g_rigidmark_alpha;
extern float g_rigmark_alpha;

extern float g_lodrate2L[CHKINVIEW_LODMAX];
extern float g_lodrate3L[CHKINVIEW_LODMAX];
extern float g_lodrate4L[CHKINVIEW_LODMAX];


extern float g_thdeg;
extern float g_thdeg_endjoint;
extern float g_thRoundX;
extern float g_thRoundY;
extern float g_thRoundZ;


extern bool g_enableshadow;
extern bool g_VSMflag;
extern bool g_blurShadow;
extern int g_shadowmap_slotno;
extern float g_shadowmap_fov[SHADOWSLOTNUM];
extern float g_shadowmap_projscale[SHADOWSLOTNUM];
extern float g_shadowmap_near[SHADOWSLOTNUM];
extern float g_shadowmap_far[SHADOWSLOTNUM];
extern float g_shadowmap_color[SHADOWSLOTNUM];
extern float g_shadowmap_bias[SHADOWSLOTNUM];
extern float g_shadowmap_plusup[SHADOWSLOTNUM];
extern float g_shadowmap_distscale[SHADOWSLOTNUM];
extern int g_shadowmap_lightdir[SHADOWSLOTNUM];

extern bool g_zpreflag;
extern bool g_zcmpalways;
extern bool g_rotatetanim;
extern bool g_tpose;
extern bool g_preciseOnPreviewToo;
extern bool g_x180flag;
//extern bool g_edgesmp;

extern float g_AmbientFactorAtLoading;
extern float g_DiffuseFactorAtLoading;
extern float g_SpecularFactorAtLoading;
extern float g_EmissiveFactorAtLoading;
extern float g_AmbientFactorAtSaving;
extern float g_DiffuseFactorAtSaving;
extern float g_SpecularFactorAtSaving;
extern float g_EmissiveFactorAtSaving;

extern bool g_VSync;
extern int g_fpskind;
extern bool g_HighRpmMode;
extern int g_UpdateMatrixThreads;
extern int g_lightflag;

extern int g_usephysik;
extern int g_ClearColorIndex;
extern float g_ClearColor[BGCOL_MAX][4];

extern bool g_bakelimiteulonsave;


extern double g_befeuldiffmax;
extern int g_dsmousewait;
extern HBITMAP g_mouseherebmp;
extern COLORREF g_tranbmp;
extern float g_mouseherealpha;
extern int g_iklevel;
extern CInfoWindow* g_infownd;
extern int g_endappflag;
//extern ID3D11DepthStencilState *g_pDSStateZCmp;
//extern ID3D11DepthStencilState *g_pDSStateZCmpAlways;
//extern ID3D11ShaderResourceView* g_presview;

extern bool g_underloading;
extern int g_underselectingframe;
//extern bool g_underIKRot;
//extern bool g_underIKRotApplyFrame;
extern bool g_fpsforce30;
extern bool g_underWriteFbx;
//extern bool g_underCalcEul;
//extern bool g_underPostFKTra;
//extern bool g_underInitMp;
//extern bool g_underCopyW2LW;
extern bool g_changeUpdateThreadsNum;

extern int	g_numthread;
extern double g_btcalccnt;
extern int g_dbgloadcnt;
extern double g_calcfps;

extern bool g_selecttolastFlag;
extern bool g_underselecttolast;
extern bool g_undereditrange;

extern bool g_limitdegflag;


//2024/04/17 limitrateは構造体ANGLELIMITのメンバにしてCBone::m_anglelimitでボーンごとに管理することにした
//extern int g_limitrate;
extern double g_physicalLimitScale;//全てのCBoneのlimitrateに掛けるスケール
extern int g_physicalMovableRate;//2024/05/02 LimitEulオンで物理シミュをする場合の　制限角度内における動作変化率％
extern double g_physicalVeloScale;//2024/05/04 LimitEulオンで物理シミュする場合の　剛体速度スケール


//extern bool g_wmatDirectSetFlag;
//extern bool g_underRetargetFlag;

extern float g_impscale;
extern float g_initmass;

extern ChaVector3 g_camEye;
extern ChaVector3 g_camtargetpos;
extern ChaVector3 g_befcamEye;
extern ChaVector3 g_befcamtargetpos;
extern ChaVector3 g_cameraupdir;
extern float g_camdist;
extern float g_initcamdist;
extern float g_fovy;
extern float g_projnear;
extern float g_projfar;


extern float g_l_kval[3];//
extern float g_a_kval[3];//
extern float g_initcuslk;
//float g_initcuslk = 2000.0f;
//float g_initcuslk = 100.0f;
extern float g_initcusak;
extern float g_akscale;//全ての剛体の回転バネ定数に対するスケール

//float g_l_dmp = 0.75f;
//float g_a_dmp = 0.50f;

extern float g_l_dmp;
extern float g_a_dmp;

//extern int g_shadertype;////マテリアル毎に設定することに
extern int g_previewFlag;			// プレビューフラグ
extern int g_cameraanimmode;//0: OFF, 1:ON, 2:ON and RootMotionOption ON
extern int g_cameraInheritMode;

extern int g_applyendflag;
extern int g_slerpoffflag;
extern int g_absikflag;
extern int g_bonemarkflag;
extern int g_rigidmarkflag;
extern int g_pseudolocalflag;
extern int g_wallscrapingikflag;
extern int g_prepcntonphysik;
extern int g_boneaxis;
extern int g_brushrepeats;
extern int g_brushmirrorUflag;
extern int g_brushmirrorVflag;
extern int g_ifmirrorVDiv2flag;


extern CTexBank*	g_texbank;

extern float g_tmpmqomult;
extern WCHAR g_tmpmqopath[MULTIPATH];
extern float g_tmpbvhfilter;

//extern ID3D11BlendState* g_blendState;
//
//extern ID3DX11Effect* g_pEffect;
//
//extern ID3DX11EffectTechnique* g_hRenderBoneL0;
//extern ID3DX11EffectTechnique* g_hRenderBoneL1;
//extern ID3DX11EffectTechnique* g_hRenderBoneL2;
//extern ID3DX11EffectTechnique* g_hRenderBoneL3;
//extern ID3DX11EffectTechnique* g_hRenderBoneL4;
//extern ID3DX11EffectTechnique* g_hRenderBoneL5;
//extern ID3DX11EffectTechnique* g_hRenderBoneL6;
//extern ID3DX11EffectTechnique* g_hRenderBoneL7;
//extern ID3DX11EffectTechnique* g_hRenderBoneL8;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL0;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL1;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL2;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL3;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL4;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL5;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL6;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL7;
//extern ID3DX11EffectTechnique* g_hRenderNoBoneL8;
//extern ID3DX11EffectTechnique* g_hRenderLine;
//extern ID3DX11EffectTechnique* g_hRenderSprite;
//
//extern ID3DX11EffectMatrixVariable* g_hm4x4Mat;
//extern ID3DX11EffectMatrixVariable* g_hmWorld;
//extern ID3DX11EffectMatrixVariable* g_hmVP;
//
//extern ID3DX11EffectVectorVariable* g_hEyePos;
//extern ID3DX11EffectScalarVariable* g_hnNumLight;
//extern ID3DX11EffectVectorVariable* g_hLightDir;
//extern ID3DX11EffectVectorVariable* g_hLightDiffuse;
//extern ID3DX11EffectVectorVariable* g_hLightAmbient;
//extern ID3DX11EffectVectorVariable* g_hSpriteOffset;
//extern ID3DX11EffectVectorVariable* g_hSpriteScale;
//extern ID3DX11EffectVectorVariable* g_hPm3Scale;
//extern ID3DX11EffectVectorVariable* g_hPm3Offset;
//
//
//extern ID3DX11EffectVectorVariable* g_hdiffuse;
//extern ID3DX11EffectVectorVariable* g_hambient;
//extern ID3DX11EffectVectorVariable* g_hspecular;
//extern ID3DX11EffectScalarVariable* g_hpower;
//extern ID3DX11EffectVectorVariable* g_hemissive;
//extern ID3DX11EffectShaderResourceVariable* g_hMeshTexture;

extern BYTE g_keybuf[256];
extern BYTE g_savekeybuf[256];

extern WCHAR g_basedir[MAX_PATH];

extern double						g_dspeed;
//double						g_dspeed = 0.0;


extern float g_ikfirst;
extern float g_ikrate;
extern double g_applyrate;
extern int g_refposstep;
extern int g_refalpha;
extern float g_physicsmvrate;

extern float                       g_fLightScale;
extern int                         g_nNumActiveLights;
//extern int                         g_nActiveLight;
extern int g_lightSlot;
extern ChaVector3 g_lightDir[LIGHTSLOTNUM][LIGHTNUMMAX];
extern ChaVector3 g_lightDiffuse[LIGHTSLOTNUM][LIGHTNUMMAX];
extern bool g_lightEnable[LIGHTSLOTNUM][LIGHTNUMMAX];
extern bool g_lightDirWithView[LIGHTSLOTNUM][LIGHTNUMMAX];
extern float g_lightScale[LIGHTSLOTNUM][LIGHTNUMMAX];
extern int g_lightNo[LIGHTNUMMAX];

extern int g_motionbrush_method;
extern double g_motionbrush_startframe;
extern double g_motionbrush_endframe;
extern double g_motionbrush_applyframe;
extern double g_motionbrush_numframe;
extern int g_motionbrush_frameleng;
extern float* g_motionbrush_value;

extern double g_playingstart;
extern double g_playingend;

#endif

#endif

