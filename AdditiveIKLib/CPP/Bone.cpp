#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <GlobalVar.h>

#include <Model.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#define DBGH
#include <dbg.h>

#include <Bone.h>
#include <MQOFace.h>
#include <BtObject.h>

#include <InfScope.h>
#include <MotionPoint.h>
#include <NodeOnLoad.h>

//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <RigidElem.h>
#include <EngName.h>
//#include <BoneProp.h>

#include <ChaScene.h>

//for __nop()
#include <intrin.h>


using namespace std;
using namespace OrgWinGUI;


//制限角度に遊びを設ける
//#define EULLIMITPLAY	5

//大きすぎ？ギザギザの原因？
//#define EULLIMITPLAY	2

//2023/02/12
// 2023/10/18 ChaCalcFunc.cppへ移動
//#define EULLIMITPLAY	1


map<CModel*,int> g_bonecntmap;
/*
extern WCHAR g_basedir[MAX_PATH];
extern int g_boneaxis;
extern bool g_limitdegflag;
extern bool g_wmatDirectSetFlag;
extern bool g_underRetargetFlag;
extern int g_previewFlag;
*/

extern CRITICAL_SECTION g_CritSection_FbxSdk;


//global
void InitCustomRig(CUSTOMRIG* dstcr, CBone* parentbone, int rigno);
int IsValidCustomRig(CModel* srcmodel, CUSTOMRIG srccr, CBone* parentbone);
//void SetCustomRigBone(CUSTOMRIG* dstcr, CBone* childbone);
int IsValidRigElem(CModel* srcmodel, RIGELEM srcrigelem);


static std::vector<CBone*> s_bonepool;//allocate BONEPOOLBLKLEN motoinpoints at onse and pool 



void InitCustomRig(CUSTOMRIG* dstcr, CBone* parentbone, int rigno)
{
	//ZeroMemory(dstcr, sizeof(CUSTOMRIG));
	if (!dstcr) {
		_ASSERT(0);
		return;
	}
	dstcr->Init();

	dstcr->rigboneno = -1;
	int rigelemno;
	for (rigelemno = 0; rigelemno < MAXRIGELEMNUM; rigelemno++) {
		dstcr->rigelem[rigelemno].boneno = -1;
		dstcr->rigelem[rigelemno].rigrigboneno = -1;
		dstcr->rigelem[rigelemno].rigrigno = -1;
		int uvno;
		for (uvno = 0; uvno < 2; uvno++) {
			dstcr->rigelem[rigelemno].transuv[uvno].enable = 1;
		}
	}


	if ((rigno >= 0) && (rigno < MAXRIGNUM)) {
		dstcr->rigno = rigno;
		if (parentbone) {
			swprintf_s(dstcr->rigname, 256, L"%s_Rig%d", parentbone->GetWBoneName(), rigno);
		}
		else {
			swprintf_s(dstcr->rigname, 256, L"Rig%d", rigno);
		}
	}
	else {
		_ASSERT(0);
		dstcr->rigno = 0;
		swprintf_s(dstcr->rigname, 256, L"RigName_00");
	}

	if (parentbone) {
		dstcr->rigboneno = parentbone->GetBoneNo();
		dstcr->elemnum = 1;
		dstcr->rigelem[0].boneno = parentbone->GetBoneNo();
	}
}


int IsValidCustomRig(CModel* srcmodel, CUSTOMRIG srccr, CBone* parentbone)
{
	/*
	typedef struct tag_rigtrans
	{
	int axiskind;
	float applyrate;
	}RIGTRANS;

	typedef struct tag_rigelem
	{
	int boneno;
	RIGTRANS transuv[2];
	}RIGELEM;

	typedef struct tag_customrig
	{
	int useflag;//0 : free, 1 : rental, 2 : valid and in use
	int rigno;//CUSTOMRIGを配列で持つ側のためのCUSTOMRIGのindex
	int rigboneno;
	int elemnum;
	RIGELEM rigelem[4];
	}CUSTOMRIG;
	*/

	if (!srcmodel) {
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。ownerModel NULL");
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		return 0;
	}
	if (!parentbone) {
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。ownerbone NULL");
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		return 0;
	}

	if (parentbone && (srccr.rigboneno != parentbone->GetBoneNo())) {
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。rigboneno : %d", srccr.rigboneno);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}
	if ((srccr.rigno < 0) || (srccr.rigno >= MAXRIGNUM)) {
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。rigno : %d", srccr.rigno);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}
	if ((srccr.elemnum < 1) || (srccr.elemnum > MAXRIGELEMNUM)) {
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。elemnum : %d", srccr.elemnum);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}

	if ((srccr.dispaxis < 0) || (srccr.dispaxis > 2)) {
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。dispaxis : %d", srccr.dispaxis);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}
	if ((srccr.disporder < 0) || (srccr.disporder > RIGPOSINDEXMAX)) {//INDEXMAXは有り
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。disporder : %d", srccr.disporder);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}
	if ((srccr.shapemult < 0) || (srccr.shapemult > RIGMULTINDEXMAX)) {//INDEXMAXは有り
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。shapemult : %d", srccr.shapemult);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}
	if ((srccr.shapekind < 0) || (srccr.shapekind >= RIGSHAPE_MAX)) {//MAXは無し
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。shapekind : %d", srccr.shapekind);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}
	if ((srccr.rigcolor < 0) || (srccr.rigcolor >= RIGCOLOR_MAX)) {//MAXは無し
		WCHAR strerr[256];
		swprintf_s(strerr, 256, L"エラー。rigcolor : %d", srccr.rigcolor);
		::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
		_ASSERT(0);
		return 0;
	}


	int elemno;
	for (elemno = 0; elemno < srccr.elemnum; elemno++) {
		RIGELEM currigelem = srccr.rigelem[elemno];
		int isvalid = IsValidRigElem(srcmodel, currigelem);
		if (isvalid == 0) {
			WCHAR strerr[256];
			swprintf_s(strerr, 256, L"エラー。bonename %s, elem %d", parentbone->GetWBoneName(), elemno);
			::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
			_ASSERT(0);
			return 0;//!!!!!!!!!!!!!
		}
	}

	return 1;
}


int IsValidRigElem(CModel* srcmodel, RIGELEM srcrigelem)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 0;
	}


	if (srcrigelem.rigrigboneno >= 0) {
		CBone* ownerbone = srcmodel->GetBoneByID(srcrigelem.rigrigboneno);
		if (ownerbone) {
			CUSTOMRIG curcr = ownerbone->GetCustomRig(srcrigelem.rigrigno);
			int isvalid = IsValidCustomRig(srcmodel, curcr, ownerbone);
			if (isvalid == 0) {
				WCHAR strerr[256];
				swprintf_s(strerr, 256, L"エラー。ownerbone %s, rigrigno %d", ownerbone->GetWBoneName(), srcrigelem.rigrigno);
				::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
				_ASSERT(0);
				return 0;
			}
		}
		else {
			WCHAR strerr[256];
			swprintf_s(strerr, 256, L"エラー。ownerbone NULL");
			::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
			_ASSERT(0);
			return 0;
		}
	}
	else {
		CBone* chkbone = srcmodel->GetBoneByID(srcrigelem.boneno);
		if (!chkbone) {
			WCHAR strerr[256];
			swprintf_s(strerr, 256, L"エラー。boneno : %d", srcrigelem.boneno);
			::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
			_ASSERT(0);
			return 0;
		}


		if (chkbone->IsNull()) {
			WCHAR strerr[256];
			swprintf_s(strerr, 256, L"エラー。eNullには設定できません。");
			::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
			_ASSERT(0);
			return 0;
		}


		int uvno;
		for (uvno = 0; uvno < 2; uvno++) {
			RIGTRANS currigtrans = srcrigelem.transuv[uvno];
			if ((currigtrans.axiskind < 0) || (currigtrans.axiskind >= RIGAXIS_MAX)) {
				WCHAR strerr[256];
				swprintf_s(strerr, 256, L"エラー。UV %d : axiskind : %d", uvno, currigtrans.axiskind);
				::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
				_ASSERT(0);
				return 0;
			}
			if ((currigtrans.applyrate < -100.0f) || (currigtrans.applyrate > 100.0f)) {
				WCHAR strerr[256];
				swprintf_s(strerr, 256, L"エラー。UV %d : applyrate : %f", uvno, currigtrans.applyrate);
				::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
				_ASSERT(0);
				return 0;
			}
			if ((currigtrans.enable != 0) && (currigtrans.enable != 1)) {
				WCHAR strerr[256];
				swprintf_s(strerr, 256, L"エラー。UV %d : enable : %d", uvno, currigtrans.enable);
				::MessageBox(NULL, strerr, L"入力エラー", MB_OK);
				_ASSERT(0);
				return 0;
			}
		}
	}

	return 1;
}




//class

static CModel* s_coldisp[COL_MAX];//剛体表示用
static int s_coldispgetnum[COL_MAX];//剛体表示用
static CModel* s_refposmark;//RefPosジョイント軌跡表示用
static int s_refposmarkgetnum;//RefPosジョイント軌跡表示用

CBone::CBone( CModel* parmodel )// : m_curmp(), m_axisq()
{
	InitializeCriticalSection(&m_CritSection_AddMP);
	InitializeCriticalSection(&m_CritSection_GetBefNext);
	InitializeCriticalSection(&m_CritSection_GetBefNext2);
	InitParams();
	SetParams(parmodel);
}

CBone::~CBone()
{
	DeleteCriticalSection(&m_CritSection_AddMP);
	DeleteCriticalSection(&m_CritSection_GetBefNext);
	DeleteCriticalSection(&m_CritSection_GetBefNext2);
	DestroyObjs();
}

int CBone::InitParams()
{
	//not use at allocated
	m_useflag = 0;//0: not use, 1: in use
	m_indexofpool = 0;
	m_allocheadflag = 0;//1: head pointer at allocated

	m_dbgcount = 0;
	m_pastedoneflag = false;

	m_matrixindex = -1;//for constant buffer


	m_extendflag = false;

	m_fbxnodeonload = 0;//2022/11/01

	m_curmp[0].InitParams();
	m_curmp[1].InitParams();
	m_updateslot = 0;
	//m_calccurmp.InitParams();
	m_axisq.InitParams();

	m_motionkey.clear();
	m_motionkey[0] = 0;

	//m_addlimitq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	//ChaMatrixIdentity(&m_localS0);
	//m_localR0.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//ChaMatrixIdentity(&m_localT0);
	//ChaMatrixIdentity(&m_firstSRT);
	//ChaMatrixIdentity(&m_firstGlobalSRT);


	m_tmpkinematic = false;
	//m_curmotid = -1;
	m_curmotid = 0;//有効なidは１から
	//m_excludemv = 0;
	//m_mass0 = 0;
	//m_posconstraint = 0;
	//ZeroMemory(m_coldisp, sizeof(CModel*)* COL_MAX);

	ChaMatrixIdentity(&m_tmpsymmat);

	ChaMatrixIdentity(&(m_btmat[0]));
	ChaMatrixIdentity(&(m_btmat[1]));
	//ChaMatrixIdentity(&(m_befbtmat[0]));
	//ChaMatrixIdentity(&(m_befbtmat[1]));
	m_setbtflag = 0;
	m_bteul = ChaVector3(0.0f, 0.0f, 0.0f);

	m_btparentpos = ChaVector3(0.0f, 0.0f, 0.0f);
	m_btchildpos = ChaVector3(0.0f, 0.0f, 0.0f);
	ChaMatrixIdentity(&m_btdiffmat);

	m_initcustomrigflag = 0;
	//InitCustomRig();//<-- after set name

	InitAngleLimit();
	m_upkind = UPVEC_NONE;
	m_motmark.clear();

	m_parmodel = 0;
	m_validflag = 1;
	m_rigidelemname.clear();
	m_btobject.clear();
	m_btkinflag = 1;
	m_btforce = 0;

	m_globalpos.SetIdentity();
	ChaMatrixIdentity( &m_invfirstmat );
	ChaMatrixIdentity( &m_firstmat );
	//ChaMatrixIdentity( &m_startmat2 );
	//ChaMatrixIdentity( &m_axismat_par );
	ChaMatrixIdentity(&m_initmat);
	ChaMatrixIdentity(&m_invinitmat);
	ChaMatrixIdentity(&m_tmpmat);
	//ChaMatrixIdentity(&m_firstaxismatX);
	//ChaMatrixIdentity(&m_firstaxismatZ);

	m_boneno = 0;
	m_topboneflag = 0;
	ZeroMemory( m_bonename, sizeof( char ) * 256 );
	ZeroMemory( m_wbonename, sizeof( WCHAR ) * 256 );
	ZeroMemory( m_engbonename, sizeof( char ) * 256 );

	m_childworld = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_childscreen = ChaVector3( 0.0f, 0.0f, 0.0f );

	m_parent = 0;
	m_child = 0;
	m_brother = 0;

	m_selectflag = 0;

	m_getanimflag = 0;

	m_type = FBXBONE_NONE;

	ChaMatrixIdentity(&m_nodemat);
	ChaMatrixIdentity(&m_nodeanimmat);
	//m_bindmat.SetIdentity();

	m_jointwpos = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_jointfpos = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_oldjointfpos = ChaVector3(0.0f, 0.0f, 0.0f);
	m_defboneposkind  = DEFBONEPOS_NONE;//FbxFile.cpp FbxSetDefaultBonePosReq()でセット　BPの有無など


	ChaMatrixIdentity( &m_laxismat );
	//ChaMatrixIdentity( &m_gaxismatXpar );

	m_remap.clear();
	m_impmap.clear();

	m_tmpq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	m_firstframebonepos = ChaVector3(0.0f, 0.0f, 0.0f);

	m_posefoundflag = false;
	ZeroMemory(m_cachebefmp, sizeof(CMotionPoint*) * (MAXMOTIONNUM + 1));

	m_firstgetflag = 0;//GetCurrentZeroFrameMat用
	ChaMatrixIdentity(&m_firstgetmatrix);//GetCurrentZeroFrameMat用
	ChaMatrixIdentity(&m_invfirstgetmatrix);//GetCurrentZeroFrameMat用

	m_indexedmotionpoint.clear();
	m_initindexedmotionpoint.clear();
	m_dummymp.InitParams();


	m_befupdatetime = -1.0;
	m_skipRenderBoneMark = false;

	m_fbxLclPos = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxRotOff = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxRotPiv = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxPreRot = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxLclRot = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxPostRot = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxSclOff = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxSclPiv = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxLclScl = FbxDouble3(0.0, 0.0, 0.0);
	m_fbxrotationActive = false;
	m_rotationorder = eEulerXYZ;

	m_InheritType = FbxTransform::eInheritRrSs;//0

	//m_localnodemat.SetIdentity();
	//m_localnodeanimmat.SetIdentity();

	m_hasmotioncurve.clear();

	m_ikstopflag = false;
	m_iktargetflag = false;
	m_iktargetpos = ChaVector3(0.0f, 0.0f, 0.0f);

	m_ikrotrec.clear();
	m_ikrotrec_u.clear();
	m_ikrotrec_v.clear();

	return 0;
}

int CBone::InitParamsForReUse(CModel* srcparmodel)
{
	int saveboneno = m_boneno;
	CModel* saveparmodel = m_parmodel;
	int saveindex = GetIndexOfPool();
	int saveallochead = IsAllocHead();

	DestroyObjs();
	InitParams();

	m_parmodel = saveparmodel;
	m_boneno = saveboneno;

	if (m_parmodel == 0) {//!!!!! モデルごと削除されたボーンの再利用
		SetParams(srcparmodel);//m_parmodel, m_boneno
	}


	m_firstcalcrigid = true;

	SetIndexOfPool(saveindex);
	SetIsAllocHead(saveallochead);
	SetUseFlag(1);

	return 0;
}


void CBone::InitColDisp()//static function
{
	ZeroMemory(s_coldisp, sizeof(CModel*) * COL_MAX);
	ZeroMemory(s_coldispgetnum, sizeof(int) * COL_MAX);

	s_refposmark = 0;
	s_refposmarkgetnum = 0;
}
void CBone::DestroyColDisp()//static function
{
	int colindex;
	for (colindex = 0; colindex < COL_MAX; colindex++) {
		CModel* curcol = s_coldisp[colindex];
		if (curcol) {
			delete curcol;
			s_coldisp[colindex] = 0;
		}
	}

	if (s_refposmark) {
		delete s_refposmark;
		s_refposmark = 0;
	}

	InitColDisp();
}
void CBone::ResetColDispInstancingParams()//static function
{
	int colindex;
	for (colindex = 0; colindex < COL_MAX; colindex++) {
		CModel* curcol = s_coldisp[colindex];
		if (curcol) {
			curcol->ResetInstancingParams();
			curcol->ResetDispObjScale();
		}
	}
	ZeroMemory(s_coldispgetnum, sizeof(int) * COL_MAX);
}
void CBone::ResetRefPosMarkInstancingParams()//static function
{
	if (s_refposmark) {
		s_refposmark->ResetInstancingParams();
		s_refposmark->ResetDispObjScale();
	}
	s_refposmarkgetnum = 0;
}

void CBone::RenderColDisp(ChaScene* srcchascene, myRenderer::RenderingEngine* re)//static function
{
	int lightflag = 0;
	ChaVector4 diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, g_rigidmark_alpha);//2024/01/12 alpha
	bool forcewithalpha = true;
	int btflag = 0;
	bool zcmpalways = true;
	bool zenable = true;

	if (s_coldisp[COL_CONE_INDEX] && (s_coldisp[COL_CONE_INDEX]->GetInstancingDrawNum() > 0)) {
		srcchascene->RenderInstancingModel(s_coldisp[COL_CONE_INDEX],
			forcewithalpha, re, lightflag, diffusemult, btflag, 
			zcmpalways, zenable,
			RENDERKIND_INSTANCING_LINE);
	}
	if (s_coldisp[COL_CAPSULE_INDEX] && (s_coldisp[COL_CAPSULE_INDEX]->GetInstancingDrawNum() > 0)) {
		srcchascene->RenderInstancingModel(s_coldisp[COL_CAPSULE_INDEX],
			forcewithalpha, re, lightflag, diffusemult, btflag, 
			zcmpalways, zenable,
			RENDERKIND_INSTANCING_LINE);
	}
	if (s_coldisp[COL_SPHERE_INDEX] && (s_coldisp[COL_SPHERE_INDEX]->GetInstancingDrawNum() > 0)) {
		srcchascene->RenderInstancingModel(s_coldisp[COL_SPHERE_INDEX],
			forcewithalpha, re, lightflag, diffusemult, btflag, 
			zcmpalways, zenable,
			RENDERKIND_INSTANCING_LINE);
	}
	if (s_coldisp[COL_BOX_INDEX] && (s_coldisp[COL_BOX_INDEX]->GetInstancingDrawNum() > 0)) {
		srcchascene->RenderInstancingModel(s_coldisp[COL_BOX_INDEX],
			forcewithalpha, re, lightflag, diffusemult, btflag, 
			zcmpalways, zenable,
			RENDERKIND_INSTANCING_LINE);
	}

}
void CBone::RenderRefPosMark(ChaScene* srcchascene, myRenderer::RenderingEngine* re, ChaVector4 diffusemult)//static function
{
	int lightflag = 0;
	//ChaVector4 diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, g_rigidmark_alpha);//2024/01/12 alpha
	bool forcewithalpha = true;
	int btflag = 0;
	bool zcmpalways = true;
	bool zenable = false;

	if (s_refposmark && (s_refposmark->GetInstancingDrawNum() > 0)) {
		srcchascene->RenderInstancingModel(s_refposmark,
			forcewithalpha, re, lightflag, diffusemult, btflag, 
			zcmpalways, zenable, RENDERKIND_INSTANCING_TRIANGLE);
	}
}



int CBone::SetParams(CModel* parmodel)
{

	m_parmodel = parmodel;
	//_ASSERT(m_parmodel);

	map<CModel*, int>::iterator itrcnt;
	itrcnt = g_bonecntmap.find(m_parmodel);
	if (itrcnt == g_bonecntmap.end()) {
		g_bonecntmap[m_parmodel] = 0;
	}

	int curno = g_bonecntmap[m_parmodel];
	m_boneno = curno;
	g_bonecntmap[m_parmodel] = m_boneno + 1;

	m_firstcalcrigid = true;


	if (!parmodel) {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CBone::InitCustomRig()
{
	int rigno;
	for (rigno = 0; rigno < MAXRIGNUM; rigno++){
		::InitCustomRig(&(m_customrig[rigno]), this, rigno);
	}
	return 0;
}

void CBone::InitAngleLimit()
{


	m_anglelimit.boneaxiskind = BONEAXIS_CURRENT;

	int axiskind;
	for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
		m_anglelimit.limitoff[axiskind] = 0;
		m_anglelimit.via180flag[axiskind] = 0;
		m_anglelimit.lower[axiskind] = -180;
		m_anglelimit.upper[axiskind] = 180;
	}
	SetAngleLimitOff();

}
void CBone::SetAngleLimitOff()
{
	int axiskind;
	for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
		if ((m_anglelimit.lower[axiskind] == -180) && (m_anglelimit.upper[axiskind] == 180)) {
			m_anglelimit.limitoff[axiskind] = 1;
		}
		else {
			m_anglelimit.limitoff[axiskind] = 0;
		}
	}
}


int CBone::DestroyObjs()
{
	ZeroMemory(m_cachebefmp, sizeof(CMotionPoint*) * (MAXMOTIONNUM + 1));


	m_motmark.clear();

	map<int, CMotionPoint*>::iterator itrmp;
	for( itrmp = m_motionkey.begin(); itrmp != m_motionkey.end(); itrmp++ ){
		CMotionPoint* topkey = itrmp->second;
		if( topkey ){
			CMotionPoint* curmp = topkey;
			CMotionPoint* nextmp = 0;
			while( curmp ){
				nextmp = curmp->GetNext();

				//delete curmp;
				CMotionPoint::InvalidateMotionPoint(curmp);

				curmp = nextmp;
			}
		}
	}
	m_motionkey.clear();


	map<string, std::map<CBone*, CRigidElem*>>::iterator itrmap;
	for( itrmap = m_remap.begin(); itrmap != m_remap.end(); itrmap++ ){
		map<CBone*, CRigidElem*>::iterator itrre;
		for( itrre = itrmap->second.begin(); itrre != itrmap->second.end(); itrre++ ){
			CRigidElem* curre = itrre->second;
			if (curre){
				//delete curre;
				CRigidElem::InvalidateRigidElem(curre);
			}
		}
		itrmap->second.clear();
	}
	m_remap.clear();
	m_impmap.clear();

	m_rigidelemname.clear();



	return 0;
}


int CBone::AddChild( CBone* childptr )
{
	if( !m_child ){
		m_child = childptr;
		m_child->m_parent = this;
	}
	else{
		CBone* broptr = m_child;
		if (broptr){
			while (broptr->m_brother){
				broptr = broptr->m_brother;
			}
			broptr->m_brother = childptr;
			broptr->m_brother->m_parent = this;//!!!!!!!!
		}
	}

	return 0;
}


int CBone::UpdateMatrix(bool limitdegflag, int srcmotid, double srcframe, 
	ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat, bool callingbythread, int updateslot)
	//default : callingbythread = false, updateslot = 0
{
	if (!wmat || !vmat || !pmat) {
		_ASSERT(0);
		return 1;
	}


	//2023/08/26
	if (GetParModel() && (GetParModel()->GetInView() == false)) {
		return 0;
	}

	if (updateslot >= 2) {
		m_updateslot = updateslot - 2;
	}
	else {
		m_updateslot = updateslot;
	}

	


	//2023/01/18 注意書修正
	//UpdateMatrixのsrcframeは経過時間計算を考慮した　浮動小数有りの時間が渡される
	//GetMotionPoint, GetWorldMatは intに丸めてからdoubleにして検索する justでtimeが一致しないとMotionPointが返らない
	//一方で　GetBefNextMPには　フレーム間姿勢の補間のために　小数有りの時間を渡す　justが無くても　befとnextを返す

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	int existflag = 0;

	if ((g_previewFlag != 5) || (m_parmodel && (m_parmodel->GetBtCnt() == 0))){
		if (srcframe >= 0.0) {
			ChaMatrix newworldmat;
			ChaMatrixIdentity(&newworldmat);

			//###################################
			//補間のためにroundingframeではない
			//###################################
			CallF(CalcFBXMotion(limitdegflag, srcmotid, srcframe, &(m_curmp[m_updateslot]), &existflag), return 1);
			//newworldmat = m_curmp.GetWorldMat();// **wmat;
			newworldmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, &(m_curmp[m_updateslot]));

			//2023/02/03
			//計算済を取得して補間するだけなので　m_curmp以外にはセットしない
			////2022/12/17
			////オイラー角情報更新
			////if (callingbythread == false) {//worldmatに変更が無いときにthreadで呼ばれる　よってローカル計算可能
			//CMotionPoint* mpptr = GetMotionPoint(srcmotid, roundingframe);
			//if (mpptr) {
			//	ChaMatrix wm = GetWorldMat(srcmotid, roundingframe, mpptr);
			//	ChaVector3 cureul = CalcLocalEulXYZ(-1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
			//	SetLocalEul(srcmotid, roundingframe, cureul, mpptr);
			//	if (g_limitdegflag == true) {
			//		mpptr->SetCalcLimitedWM(2);
			//	}
			//}
			//else {
			//	_ASSERT(0);
			//}


		//2023/02/02
		//modelのworldmatが掛かっていないアニメ姿勢も保存　GetCurrent..., CalcCurrent...用
			m_curmp[m_updateslot].SetAnimMat(newworldmat);


		//modelのworldmatを掛ける
			//skinmeshの変換の際にはシェーダーでg_hmWorldは掛けない　すでにg_hmWorldが掛かっている必要有
			ChaMatrix tmpmat = newworldmat * *wmat; // !!!!!!!!!!!!!!!!!!!!!!!!!!!
			SetWorldMat(limitdegflag, srcmotid, roundingframe, tmpmat, &(m_curmp[m_updateslot]));//roundingframe!!!!

			if (limitdegflag == true) {
				m_curmp[m_updateslot].SetCalcLimitedWM(2);
			}

			ChaVector3 jpos = GetJointFPos();
			ChaVector3TransformCoord(&m_childworld, &jpos, &tmpmat);
			ChaMatrix vpmat = *vmat * *pmat;
			ChaMatrix wvpmat = tmpmat * vpmat;
			ChaVector3TransformCoord(&m_childscreen, &m_childworld, &vpmat);//wmatで変換した位置に対して　vp変換
		}
		else {
			_ASSERT(0);
			m_curmp[m_updateslot].InitParams();
			m_curmp[m_updateslot].SetWorldMat(*wmat);
			m_curmp[m_updateslot].SetFrame(roundingframe);
			SetWorldMat(limitdegflag, srcmotid, roundingframe, *wmat, &(m_curmp[m_updateslot]));//roundingframe!!!!
		}

		//if (updateslot >= 2) {
		//  //Render処理中に書き込むと余計に乱れて難しくなるのでコメントアウト
		// 
		//	//2024/03/12 ダブルバッファ物理の始まりで乱れないように　両方のスロットにセット
		//	int otherslot = (int)(!(m_updateslot != 0));
		//	m_curmp[otherslot].SetWorldMat(m_curmp[m_updateslot].GetWorldMat());
		//	m_curmp[otherslot].SetLimitedWM(m_curmp[m_updateslot].GetLimitedWM());
		//	m_curmp[otherslot].SetAnimMat(m_curmp[m_updateslot].GetAnimMat());
		//	m_curmp[otherslot].SetFrame(m_curmp[m_updateslot].GetFrame());
		//	if (limitdegflag == true) {
		//		m_curmp[otherslot].SetCalcLimitedWM(2);
		//	}
		//}


		if (m_parmodel && (m_parmodel->GetBtCnt() == 0)) {//2022/08/18 add checking m_parmodel
			//bool settobothflag = true;//2023/11/04 ダブルバッファ物理の始まりで乱れないように　両方のスロットにセット
			bool settobothflag = false;//2023/03/12
			SetBtMat(GetWorldMat(limitdegflag, srcmotid, roundingframe, &(m_curmp[m_updateslot])), settobothflag);
		}
	}
	else{
		//RagdollIK時のボーン選択対策
		ChaVector3 jpos = GetJointFPos();

		ChaMatrix wmat2, wvpmat;
		if (GetParent(true)){
			wmat2 = GetParent(true)->GetBtMat();// **wmat;
		}
		else{
			wmat2 = GetBtMat();// **wmat;
		}
		ChaMatrix vpmat = *vmat * *pmat;
		wvpmat = wmat2 * vpmat;


		//ChaVector3TransformCoord(&m_childscreen, &m_childworld, &wvpmat);
		//ChaVector3TransformCoord(&m_childworld, &jpos, &wmat);
		ChaVector3TransformCoord(&m_childworld, &jpos, &wmat2);
		
		//ChaVector3TransformCoord(&m_childworld, &jpos, &(GetBtMat()));
		ChaVector3TransformCoord(&m_childscreen, &m_childworld, &vpmat);
	}

	m_befupdatetime = srcframe;

	return 0;
}

int CBone::CopyLimitedWorldToWorld(int srcmotid, double srcframe)//制限角度有りの姿勢を制限無しの姿勢にコピーする
{
	double roundingframe = RoundingTime(srcframe);

	//2023/04/28 2023/05/23
	if (IsNotSkeleton() && IsNotCamera()) {
		return 0;
	}

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		ChaMatrix limitedwm;
		limitedwm = curmp->GetLimitedWM();

		ChaMatrix newwm;
		newwm.SetIdentity();
		if (GetParent(false)) {
			if (GetParent(false)->IsSkeleton()) {
				ChaMatrix limitedlocal;
				ChaMatrix parentlimitedwm;
				ChaMatrix parentunlimitedwm;
				limitedlocal.SetIdentity();
				parentunlimitedwm.SetIdentity();
				parentlimitedwm.SetIdentity();

				parentlimitedwm = GetParent(false)->GetWorldMat(true, srcmotid, roundingframe, 0);
				parentunlimitedwm = GetParent(false)->GetWorldMat(false, srcmotid, roundingframe, 0);

				limitedlocal = limitedwm * ChaMatrixInv(parentlimitedwm);
				newwm = limitedlocal * parentunlimitedwm;
			}
			else if (GetParent(false)->IsNull() || GetParent(false)->IsCamera()) {
				newwm = limitedwm;
			}
			else {
				_ASSERT(0);
				newwm = limitedwm;
			}
		}
		else {
			newwm = limitedwm;
		}

		//bool limitdegflag = false;
		//bool directsetflag = false;
		////bool directsetflag = true;//2023/02/08 copyなのでdirectset.
		//bool infooutflag = false;
		//int setchildflag = 1;//setchildflagは directsetflag == falseのときしか働かない
		//SetWorldMat(limitdegflag, directsetflag, infooutflag, setchildflag, srcmotid, roundingframe, newwm);

		bool limitdegflag = false;
		UpdateCurrentWM(limitdegflag, srcmotid, roundingframe, newwm);

	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CBone::CopyWorldToLimitedWorld(int srcmotid, double srcframe)//制限角度無しの姿勢を制限有りの姿勢にコピーする
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.CopyWorldToLimitedWorld(this, srcmotid, srcframe);
}


int CBone::ApplyNewLimitsToWM(int srcmotid, double srcframe, ChaMatrix befeditparentmat)
{
	//2023/02/03
	//この関数を実行する前に　limitedworldmatにworldmatをコピーしておく

	ChaMatrix curwm;
	curwm.SetIdentity();

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		
		//2023/02/03 LimitEulにチェックが入っていない場合にも　limitedに対して操作
		bool limitdegflag = true;//!!!!!!!!!!!!!

		curwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);

		bool directsetflag = false;
		int infooutflag = 0;


		//2023/10/24
		// 
		//int setchildflag = 1;//<-- 必須 RootNodeの回転を絞り込めば分かる <--- 2023/10/24コメントアウト
		// 
		//この関数はCBone単体呼び出しでは使わない　RootのBoneから再帰的に全てのボーンに対して呼び出すはず　よってこのフラグを０にして高速化
		//setchildflag = 0で正しく動かすには　１階層ごとにbefeditmatと新しいwmを考慮
		//
		int setchildflag = 0;//!!!!!!!!!!!!!
		ChaMatrix parentmat;
		if (GetParent(false)) {
			CMotionPoint* parentmp = GetParent(false)->GetMotionPoint(srcmotid, roundingframe);
			if (parentmp) {
				parentmat = parentmp->GetLimitedWM();
				curwm = curwm * ChaMatrixInv(befeditparentmat) * parentmat;

				curmp->SetLimitedWM(curwm);//SetWorldMatから呼び出すCalcLocalEulXYZ()のために　新しいparentwmを反映したwmをセットしておく
			}
		}

		int onlycheck = 0;
		bool fromiktarget = false;
		
		SetWorldMat(limitdegflag, directsetflag, infooutflag, setchildflag, srcmotid, roundingframe, curwm, onlycheck, fromiktarget);
		
		//curmp->SetLimitedWM(newwm);
		//curmp->SetLimitedLocalEul(neweul);
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CBone::ClearLimitedWorldMat(int srcmotid, double srcframe0)
{
	int existflag = 0;

	//2023/04/28 2023/05/23
	if (IsNotSkeleton() && IsNotCamera()) {
		return 0;
	}

	//ChaMatrix newworldmat;
	//ChaMatrixIdentity(&newworldmat);

	//制限角度有り
	double srcframe = RoundingTime(srcframe0);
	CMotionPoint* orgbefmp = 0;
	CMotionPoint* orgnextmp = 0;
	GetBefNextMP(srcmotid, srcframe, &orgbefmp, &orgnextmp, &existflag);
	//if ((existflag == 1) && orgbefmp) {
	if (orgbefmp) {
		orgbefmp->SetCalcLimitedWM(0);
	}

	return 0;

}


//int CBone::UpdateMatrixFromEul(int srcmotid, double srcframe, ChaVector3 neweul, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//	int existflag = 0;
//
//	if ((g_previewFlag != 5) || (m_parmodel && (m_parmodel->GetBtCnt() == 0))) {
//
//
//		if (srcframe >= 0.0) {
//			CallF(CalcFBXMotion(srcmotid, srcframe, &m_curmp, &existflag), return 1);
//			//ChaMatrix tmpmat = m_curmp.GetWorldMat();// **wmat;
//
//			ChaMatrix newworldmat;
//			ChaMatrixIdentity(&newworldmat);
//			newworldmat = CalcWorldMatFromEul(0, 1, neweul, srcmotid, srcframe, 0);
//			//m_curmp.SetWorldMat(newworldmat);
//
//			//ChaMatrix tmpmat = m_curmp.GetWorldMat() * *wmat;
//			ChaMatrix tmpmat = newworldmat * *wmat;
//			m_curmp.SetWorldMat(tmpmat);
//
//
//			ChaVector3 jpos = GetJointFPos();
//			//ChaVector3TransformCoord(&m_childworld, &jpos, &m_curmp.GetWorldMat());
//			ChaVector3TransformCoord(&m_childworld, &jpos, &newworldmat);
//
//			//ChaMatrix wvpmat = m_curmp.GetWorldMat() * *vpmat;
//			ChaMatrix wvpmat = newworldmat * *vpmat;
//
//			ChaVector3TransformCoord(&m_childscreen, &m_childworld, vpmat);
//			//ChaVector3TransformCoord(&m_childscreen, &m_childworld, &wvpmat);
//		}
//		else {
//			m_curmp.InitParams();
//			m_curmp.SetWorldMat(*wmat);
//		}
//
//		if (m_parmodel->GetBtCnt() == 0) {
//			SetBtMat(m_curmp.GetWorldMat());
//		}
//
//	}
//	else {
//		//RagdollIK時のボーン選択対策
//		ChaVector3 jpos = GetJointFPos();
//
//		ChaMatrix wmat2, wvpmat;
//		if (m_parent) {
//			wmat2 = m_parent->GetBtMat();// **wmat;
//		}
//		else {
//			wmat2 = GetBtMat();// **wmat;
//		}
//		wvpmat = wmat2 * *vpmat;
//
//
//		//ChaVector3TransformCoord(&m_childscreen, &m_childworld, &wvpmat);
//		//ChaVector3TransformCoord(&m_childworld, &jpos, &wmat);
//		ChaVector3TransformCoord(&m_childworld, &jpos, &wmat2);
//
//		//ChaVector3TransformCoord(&m_childworld, &jpos, &(GetBtMat()));
//		ChaVector3TransformCoord(&m_childscreen, &m_childworld, vpmat);
//	}
//	return 0;
//}

//int CBone::AddMotionPointAll(int srcmotid, double animleng)
//{
//	EnterCriticalSection(&m_CritSection_AddMP);
//
//	if ((srcmotid <= 0) || (srcmotid > (m_motionkey.size() + 1))) {// on add : ひとつ大きくても可 : 他の部分でのチェックは motid > m_motionkey.size()
//		_ASSERT(0);
//		LeaveCriticalSection(&m_CritSection_AddMP);
//		return 1;
//	}
//
//
//	//indexedmotionpointが無ければ作成　内容クリア
//	std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
//	itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
//	if (itrvecmpmap == m_indexedmotionpoint.end()) {
//		std::vector<CMotionPoint*> newvecmp;
//		m_indexedmotionpoint[srcmotid] = newvecmp;//STL 参照されていれば無くならない？？？
//
//		std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap2;
//		itrvecmpmap2 = m_indexedmotionpoint.find(srcmotid);
//		if (itrvecmpmap2 == m_indexedmotionpoint.end()) {
//			_ASSERT(0);
//			LeaveCriticalSection(&m_CritSection_AddMP);
//			return 1;
//		}
//
//		itrvecmpmap = itrvecmpmap2;
//	}
//	(itrvecmpmap->second).clear();//!!!!!!!!!!!!!!!
//	m_initindexedmotionpoint[srcmotid] = false;
//
//
//
//	CMotionPoint* firstmp = 0;
//	if (srcmotid < m_motionkey.size()) {
//		firstmp = m_motionkey[srcmotid - 1];
//		if (firstmp) {
//			//既にモーションポイントが存在するのでリターン
//			LeaveCriticalSection(&m_CritSection_AddMP);
//			return 0;//この場合も正常とする
//		}
//	}
//	
//	firstmp = CMotionPoint::GetNewMP();
//	if (firstmp) {
//		firstmp->SetFrame(0.0);
//		m_motionkey[srcmotid - 1] = firstmp;
//		(itrvecmpmap->second).push_back(firstmp);//indexedmotionpointの設定
//
//		CMotionPoint* newmp = 0;
//		CMotionPoint* pbef = firstmp;
//
//		double srcframe;
//		for (srcframe = 1.0; srcframe < animleng; srcframe+=1.0) {
//			//newmp = new CMotionPoint();
//			newmp = CMotionPoint::GetNewMP();
//			if (!newmp) {
//				_ASSERT(0);
//				LeaveCriticalSection(&m_CritSection_AddMP);
//				return 1;
//			}
//			newmp->SetFrame(srcframe);
//
//			if (pbef) {
//				int result2 = pbef->AddToNext(newmp);
//				if (result2) {
//					LeaveCriticalSection(&m_CritSection_AddMP);
//					return 1;
//				}
//			}
//
//			(itrvecmpmap->second).push_back(newmp);//indexedmotionpointの設定
//		}
//	}
//	else {
//		_ASSERT(0);
//		LeaveCriticalSection(&m_CritSection_AddMP);
//		return 1;
//	}
//
//
//	//int result3 = CreateIndexedMotionPoint(srcmotid, animleng);
//	//if (result3 != 0) {
//	//	_ASSERT(0);
//	//	LeaveCriticalSection(&m_CritSection_AddMP);
//	//	return 1;
//	//}
//
//	m_initindexedmotionpoint[srcmotid] = true;
//
//
//	LeaveCriticalSection(&m_CritSection_AddMP);
//
//	return 0;
//}

CMotionPoint* CBone::AddMotionPoint(int srcmotid, double srcframe, int* existptr)
{

	EnterCriticalSection(&m_CritSection_AddMP);

	if (!existptr) {
		_ASSERT(0);
		LeaveCriticalSection(&m_CritSection_AddMP);
		return 0;
	}

	//2023/04/28 2023/05/23 2023/06/29
	if (IsNotSkeleton() && IsNotCamera() && IsNotNull()) {
		if (existptr) {
			*existptr = 0;
		}
		LeaveCriticalSection(&m_CritSection_AddMP);
		return 0;
	}


	if ((srcmotid <= 0) || (srcmotid > (m_motionkey.size() + 1))) {// on add : ひとつ大きくても可 : 他の部分でのチェックは motid > m_motionkey.size()
		_ASSERT(0);
		LeaveCriticalSection(&m_CritSection_AddMP);
		return 0;
	}


	CMotionPoint* newmp = 0;
	CMotionPoint* pbef = 0;
	CMotionPoint* pnext = 0;
	bool onaddmotion = true;
	int result = GetBefNextMP(srcmotid, srcframe, &pbef, &pnext, existptr, onaddmotion);
	if (result != 0) {
		LeaveCriticalSection(&m_CritSection_AddMP);
		return 0;
	}

	if (*existptr){
		pbef->SetFrame(srcframe);
		newmp = pbef;
	}
	else{
		//newmp = new CMotionPoint();
		newmp = CMotionPoint::GetNewMP();
		if (!newmp){
			_ASSERT(0);
			LeaveCriticalSection(&m_CritSection_AddMP);
			return 0;
		}
		newmp->SetFrame(srcframe);

		if (pbef){
			int result2 = pbef->AddToNext(newmp);
			if (result2) {
				LeaveCriticalSection(&m_CritSection_AddMP);
				return 0;
			}
		}
		else{
			m_motionkey[srcmotid - 1] = newmp;
			if (pnext){
				newmp->SetNext(pnext);
			}
		}


		//Comment out 2022/10/30 push_back対応が難しいので　AddMotionPoint, 長さが変わるInitMp処理時に　呼び出し側でCreateIndexedMotionPointを呼ぶ
		//GetMotionPointなどは　indexをチェックして　エントリーが無い場合には　チェインを辿る
		//std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
		//itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
		//if (itrvecmpmap != m_indexedmotionpoint.end()) {
		//	//(itrvecmpmap->second).clear();
		//	int frameindex = (int)(srcframe + 0.0001);
		//	(itrvecmpmap->second)[frameindex] = newmp;//indexedmotionpointはモーションポイントの実体管理用ではなくインデックス用、作成と破棄はチェインで行うので上書きしても良い。
		//}
	}



	LeaveCriticalSection(&m_CritSection_AddMP);

	return newmp;
}

//void CBone::ResizeIndexedMotionPointReq(int srcmotid, double animleng)
//{
//	ResizeIndexedMotionPoint(srcmotid, animleng);
//
//	if (GetChild()) {
//		GetChild()->ResizeIndexedMotionPointReq(srcmotid, animleng);
//	}
//	if (GetBrother()) {
//		GetBrother()->ResizeIndexedMotionPointReq(srcmotid, animleng);
//	}
//}


//int CBone::ResizeIndexedMotionPoint(int srcmotid, double animleng)
//{
//	//std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
//	//itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
//	//if (itrvecmpmap != m_indexedmotionpoint.end()) {
//	//	(itrvecmpmap->second).resize((int)(animleng + 0.0001));
//	//}
//	return 0;
//}


int CBone::CalcFBXMotion(bool limitdegflag, 
	int srcmotid, double srcframe, CMotionPoint* dstmpptr, int* existptr)
{
	if (!dstmpptr || !existptr) {
		_ASSERT(0);
		return 1;
	}


	//2023/04/28 2023/05/23
	if (IsNotSkeleton() && IsNotCamera()) {
		if (dstmpptr) {
			ChaMatrix inimat;
			inimat.SetIdentity();
			dstmpptr->SetWorldMat(inimat);
			dstmpptr->SetLimitedWM(inimat);
			dstmpptr->SetFrame(srcframe);
		}
		return 0;
	}



	CMotionPoint* befptr = 0;
	CMotionPoint* nextptr = 0;
	CallF( GetBefNextMP( srcmotid, srcframe, &befptr, &nextptr, existptr ), return 1 );
	CallF( CalcFBXFrame(limitdegflag, srcframe, befptr, nextptr, *existptr, dstmpptr), return 1 );

	return 0;
}

void CBone::ResetMotionCache()
{
	ZeroMemory(m_cachebefmp, sizeof(CMotionPoint*) * (MAXMOTIONNUM + 1));
}

int CBone::GetBefNextMP(int srcmotid, double srcframe, CMotionPoint** ppbef, CMotionPoint** ppnext, int* existptr, bool onaddmotion)//default : onaddmotion = false
{

	if (!ppbef || !ppnext || !existptr) {
		_ASSERT(0);
		return 1;
	}

	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetBefNextMP(this, srcmotid, srcframe, ppbef, ppnext, existptr, onaddmotion);

}


int CBone::CalcFBXFrame(bool limitdegflag, double srcframe, CMotionPoint* befptr, CMotionPoint* nextptr, int existflag, CMotionPoint* dstmpptr)
{

	if (!dstmpptr) {
		_ASSERT(0);
		return 1;
	}

	//GetWorldMat対策(未計算時の取得含む)のため　SetFrameには roundingframeを使用

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28 2023/05/23
	if (IsNotSkeleton() && IsNotCamera()) {
		dstmpptr->InitParams();
		dstmpptr->SetFrame(roundingframe);
		return 0;
	}


	if( existflag == 1 ){
		*dstmpptr = *befptr;
		dstmpptr->SetFrame(roundingframe);
		return 0;
	}else if( !befptr ){
		dstmpptr->InitParams();
		dstmpptr->SetFrame(roundingframe);
		return 0;
	}else if( !nextptr ){
		*dstmpptr = *befptr;
		dstmpptr->SetFrame(roundingframe);
		return 0;
	}else{
		double diffframe = nextptr->GetFrame() - befptr->GetFrame();
		_ASSERT( diffframe != 0.0 );
		double t = ( srcframe - befptr->GetFrame() ) / diffframe;

		ChaMatrix befmat, nextmat;
		befmat = GetWorldMat(limitdegflag, m_curmotid, befptr->GetFrame(), befptr);
		nextmat = GetWorldMat(limitdegflag, m_curmotid, nextptr->GetFrame(), nextptr);

		ChaMatrix tmpmat = befmat + (nextmat - befmat) * (float)t;

		SetWorldMat(limitdegflag, m_curmotid, roundingframe, tmpmat, dstmpptr);
		dstmpptr->SetFrame(roundingframe);

		//dstmpptr->SetPrev(befptr);
		//dstmpptr->SetNext(nextptr);

		//ChaMatrix newmat;
		//ChaMatrix orgparmat;
		//ChaMatrix newparmat;
		//if (GetParent()) {
		//	CMotionPoint* parentmp;
		//	newparmat = GetParent()->GetCurMp().GetWorldMat();
		//	parentmp = GetParent()->GetMotionPoint(m_curmotid, (double)((int)srcframe));
		//	orgparmat = parentmp->GetWorldMat();

		//	ChaVector3 orgpos;
		//	ChaVector3TransformCoord(&orgpos, &(GetJointFPos()), &orgparmat);
		//	ChaVector3 newpos;
		//	ChaVector3TransformCoord(&newpos, &(GetJointFPos()), &newparmat);
		//	ChaVector3 diffpos;
		//	diffpos = orgpos - newpos;
		//	//AddBoneTraReq()
		//	
		//}
		//else {
		//	newmat = tmpmat;
		//	//dstmpptr->SetWorldMat( tmpmat );
		//	dstmpptr->SetWorldMat(newmat);
		//	dstmpptr->SetFrame(srcframe);
		//	dstmpptr->SetPrev(befptr);
		//	dstmpptr->SetNext(nextptr);
		//}

		return 0;
	}
}

int CBone::DeleteMotion( int srcmotid )
{
	//#######################################################
	//削除によりm_motionkeyに空きが出来るがmotidは変わらない
	//#######################################################

	map<int, CMotionPoint*>::iterator itrmp;
	itrmp = m_motionkey.find( srcmotid - 1 );//2021/08/26
	if( itrmp != m_motionkey.end() ){
		CMotionPoint* topkey = itrmp->second;
		if( topkey ){
			CMotionPoint* curmp = topkey;
			CMotionPoint* nextmp = 0;
			while( curmp ){
				nextmp = curmp->GetNext();

				//delete curmp;
				CMotionPoint::InvalidateMotionPoint(curmp);

				curmp = nextmp;
			}
		}
	}

	//m_motionkey.erase( itrmp );
	m_motionkey[srcmotid - 1] = 0;////2021/08/26 eraseするとアクセスするためのインデックスがsrcmotid - 1ではなくなる



	std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
	itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
	if (itrvecmpmap != m_indexedmotionpoint.end()) {
		(itrvecmpmap->second).clear();
	}

	m_initindexedmotionpoint[srcmotid] = false;


	return 0;
}



int CBone::DeleteMPOutOfRange( int motid, double srcleng )
{

	//2023/04/28 2023/05/23
	if (IsNotSkeleton() && IsNotCamera() && IsNotNull()) {
		return 0;
	}


	if ((motid <= 0) || (motid > m_motionkey.size())) {
		_ASSERT(0);
		return 1;
	}

	CMotionPoint* curmp = m_motionkey[motid - 1];

	while( curmp ){
		CMotionPoint* nextmp = curmp->GetNext();

		if( curmp->GetFrame() > srcleng ){
			curmp->LeaveFromChain( motid, this );


			//delete curmp;
			CMotionPoint::InvalidateMotionPoint(curmp);

		}
		curmp = nextmp;
	}

	std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
	itrvecmpmap = m_indexedmotionpoint.find(motid);
	if (itrvecmpmap != m_indexedmotionpoint.end()) {
		//(itrvecmpmap->second).clear();

		size_t delframeno;
		size_t roundingleng;
		roundingleng = (size_t)(srcleng + 0.0001);
		for (delframeno = roundingleng; delframeno < (itrvecmpmap->second).size(); delframeno++) {
			(itrvecmpmap->second)[delframeno] = 0;
		}
		(itrvecmpmap->second).resize(roundingleng);
	}


	return 0;
}


int CBone::SetName( const char* srcname )
{
	strcpy_s( m_bonename, 256, srcname );
	TermJointRepeats(m_bonename);

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_bonename, 256, m_wbonename, 256 );

	if (m_initcustomrigflag == 0){
		InitCustomRig();
		m_initcustomrigflag = 1;
	}

	return 0;
}

int CBone::CalcAxisMatZ( ChaVector3* curpos, ChaVector3* childpos )
{
	if( *curpos == *childpos ){
		ChaMatrixIdentity( &m_laxismat );
		m_axisq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		return 0;
	}

	ChaVector3 startpos, endpos, upvec;

	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 vecx1, vecy1, vecz1;

	startpos = *curpos;
	endpos = *childpos;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize( &bonevec, &bonevec );

	//m_axisq.RotationArc(vecz0, bonevec);
	//m_laxismat = m_axisq.MakeRotMatX();

	if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
	}else{
		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;
	}

	vecz1 = bonevec;
		
	ChaVector3Cross( &vecx1, (const ChaVector3*)&upvec, (const ChaVector3*)&vecz1 );
	ChaVector3Normalize( &vecx1, &vecx1 );

	ChaVector3Cross( &vecy1, (const ChaVector3*)&vecz1, (const ChaVector3*)&vecx1 );
	ChaVector3Normalize( &vecy1, &vecy1 );


	ChaMatrixIdentity( &m_laxismat );
	m_laxismat.SetRow(0, vecx1);
	m_laxismat.SetRow(1, vecy1);
	m_laxismat.SetRow(2, vecz1);

	m_axisq.RotationMatrix(m_laxismat);

	return 0;
}



float CBone::CalcAxisMatX_Manipulator(bool limitdegflag, int srcboneaxis, int bindflag, CBone* childbone, ChaMatrix* dstmat, int setstartflag)
{
	//############################################################################################
	//2022/11/04
	//マニピュレータの姿勢計算関数
	//以前のCalcAxisMatXを改造
	//CalcAxisMatXは　ボーンの位置だけからマニピュレータを計算していた
	//しかし　位置だけから計算すると　IK操作時のマニピュレータのIK平面がブレてしまう
	//これを解決するには　ボーンの姿勢行列を加工して　マニピュレータ行列を求めることが有効だった
	//ボーンの姿勢と　マニピュレータ行列の関係式が決まっていれば　IK中でもIK平面がブレない
	//############################################################################################


	//#########################################################################################################################
	//2023/01/11
	//AXISKIND_CURRENTについて
	//この関数は GetParent()->CalcAxisMatX_Manipulator( , childbone, ,,)のように呼び出される
	//つまり選択中のジョイントの親としての関数が呼び出されている
	//AdditiveIKにおいては　回転用のマニピュレータの操作は　IK　つまり　子供ジョイントドラッグで親ジョイントを回転する
	//マニピュレータは　子供ジョイント位置に表示するが　その軸の向きは　Parentの姿勢を反映したものとなる
	//よってAXISKIND_CURRENTの場合　親からみた　currentのNodeMatにcurrentのGetLimitedWorldMatを掛けたものが基準となる
	//#########################################################################################################################

	//bool ishipsjoint = IsHipsBone();


	ChaVector3 aftbonepos;
	ChaVector3 aftchildpos;
	ChaVector3 aftparentpos;

	if (!dstmat) {
		_ASSERT(0);
		return 0.0f;
	}
	if (!childbone) {
		_ASSERT(0);
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}

	//bool ishipsjoint = childbone->IsHipsBone();//2024/01/03

	if (!GetParModel()) {
		_ASSERT(0);
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}

	int curmotid;
	curmotid = GetParModel()->GetCurrentMotion();
	if (curmotid <= 0) {
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}
	double curframe;
	curframe = GetParModel()->GetCurrentMotionFrame();
	if (curframe < 0.0) {
		_ASSERT(0);
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}


	////2023/04/28
	if (IsNotSkeleton()) {
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;//!!!!!!!!!!!!!!!!!!!! return !!!!!!!!!!!!!!!!!!!
	}

	ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);

	ChaMatrix tmpzerofm = GetNodeMat() * GetCurrentZeroFrameMat(limitdegflag, 1);
	ChaMatrix tmplimwm = GetNodeMat() * GetCurrentWorldMat(false);
	ChaMatrix tmpbtmat = GetNodeMat() * GetBtMat();
	ChaMatrix tmpchildzerofm = childbone->GetNodeMat() * childbone->GetCurrentZeroFrameMat(limitdegflag, 1);
	ChaMatrix tmpchildlimwm = childbone->GetNodeMat() * childbone->GetCurrentWorldMat(false);
	ChaMatrix tmpchildbtmat = childbone->GetNodeMat() * childbone->GetBtMat();

	////ChaMatrix tmpchildbtmat;
	//ChaMatrix tmpparentzerofm;
	//ChaMatrix tmpparentlimwm;
	//ChaMatrix tmpparentbtmat;
	//if (GetParent(false)) {//<--- 2024/01/13 parentに対して呼び出すので実質gpar
	//	if (GetParent(false)->IsSkeleton()) {
	//		tmpparentzerofm = GetParent(false)->GetNodeMat() * GetParent(false)->GetCurrentZeroFrameMat(limitdegflag, 1);
	//		tmpparentlimwm = GetParent(false)->GetNodeMat() * GetParent(false)->GetCurrentWorldMat(false);
	//		tmpparentbtmat = GetParent(false)->GetNodeMat() * GetParent(false)->GetBtMat();
	//	}
	//	else if (GetParent(false)->IsNull()) {
	//		tmpparentzerofm = GetParent(false)->GetTransformMat(0.0, false);
	//		tmpparentlimwm = GetParent(false)->GetTransformMat(curframe, false);
	//		tmpparentbtmat = GetParent(false)->GetTransformMat(curframe, false);
	//	}
	//	else {
	//		tmpparentzerofm.SetIdentity();
	//		tmpparentlimwm.SetIdentity();
	//		tmpparentbtmat.SetIdentity();
	//	}
	//}
	//else {
	//	tmpparentzerofm.SetIdentity();
	//	tmpparentlimwm.SetIdentity();
	//	tmpparentbtmat.SetIdentity();
	//}


	ChaMatrix convmat;
	convmat.SetIdentity();

	if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
		//ChaVector3TransformCoord(&aftparentpos, &zeropos, &tmpparentlimwm);
		ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmplimwm);
		ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildlimwm);
	}
	else {
		//ChaVector3TransformCoord(&aftparentpos, &zeropos, &tmpparentbtmat);
		ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmpbtmat);
		ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildbtmat);
	}

	//if (ishipsjoint == false) {

		//hipsjointではない場合

		if ((srcboneaxis == BONEAXIS_CURRENT) || (srcboneaxis == BONEAXIS_BINDPOSE)) {
			////current bone axis
			//if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
			//	convmat = tmplimwm;
			//}
			//else {
			//	convmat = tmpbtmat;
			//}
			//current bone axis
			if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
				convmat = tmpchildlimwm;//2024/01/03
			}
			else {
				convmat = tmpchildbtmat;//2024/01/03
			}

		}
		else if (srcboneaxis == BONEAXIS_PARENT) {
			////parent bone axis
			//if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
			//	convmat = tmpparentlimwm;
			//}
			//else {
			//	convmat = tmpparentbtmat;
			//}
			//parent bone axis
			if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
				convmat = tmplimwm;//2024/01/03
			}
			else {
				convmat = tmpbtmat;//2024/01/03
			}
		}
		else if (srcboneaxis == BONEAXIS_GLOBAL) {
			//global axis
			convmat.SetIdentity();
		}
		else {
			convmat = tmplimwm;
		}
	//}
	//else {
	//	//2023/01/14
	//	//hipsjointの場合には　IK回転として　Global回転するしかない(移動成分も回転する)ので　マニピュレータもそれに合わせる
	//	//global axis
	//	convmat.SetIdentity();
	//}



	//カレント変換したボーン軸
	ChaVector3 vecforleng;
	vecforleng = aftchildpos - aftbonepos;
	float retleng = (float)ChaVector3LengthDbl(&vecforleng);



	//if ((aftbonepos == aftchildpos) || (srcboneaxis == BONEAXIS_GLOBAL) || (ishipsjoint == true)) {
	if ((aftbonepos == aftchildpos) || (srcboneaxis == BONEAXIS_GLOBAL)) {
		//ボーンの長さが０のとき　Identity回転　ボーン軸の種類がグローバルの場合　Identity回転
		dstmat->SetIdentity();
		//#########################################################
		//位置は　ボーンの親の位置　つまりカレントジョイントの位置
		//#########################################################
		dstmat->SetTranslation(aftbonepos);

		return retleng;
	}

	ChaVector3 startpos, endpos;

	if ((srcboneaxis == BONEAXIS_CURRENT) || (srcboneaxis == BONEAXIS_BINDPOSE)) {
		//current bone axis
		startpos = aftbonepos;
		endpos = aftchildpos;
	}
	else if (srcboneaxis == BONEAXIS_PARENT) {
		//parent bone axis
		startpos = aftparentpos;
		endpos = aftbonepos;
	}
	else if (srcboneaxis == BONEAXIS_GLOBAL) {
		//global axis

		_ASSERT(0);//上方でIdentity回転をセットしてリターンしているので　ここは通らない
		dstmat->SetIdentity();
		//#########################################################
		//位置は　ボーンの親の位置　つまりカレントジョイントの位置
		//#########################################################
		dstmat->SetTranslation(aftbonepos);
		return retleng;
	}
	else {
		_ASSERT(0);//想定外
		dstmat->SetIdentity();
		//#########################################################
		//位置は　ボーンの親の位置　つまりカレントジョイントの位置
		//#########################################################
		dstmat->SetTranslation(aftbonepos);
		return retleng;
	}


	//カレント変換したボーン軸
	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize(&bonevec, &bonevec);


	//###########################################################################################
	//convmatのvecxをbonevecにする　それに合わせて３軸が互いに垂直になるようにvecy, veczを求める
	//###########################################################################################
	//#########################################################
	//位置は　ボーンの親の位置　つまりカレントジョイントの位置
	//#########################################################

	if (srcboneaxis != BONEAXIS_BINDPOSE) {//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BINDPOSEの時は　オートフィットしない
		//ボーン軸をX軸に　オートフィット
		*dstmat = CalcAxisMatX(bonevec, aftbonepos, convmat);//ChaVecCalc.cpp
	}
	else {
		//BONEAXIS_BINDPOSEの場合には　ボーン軸をX軸には合わせずに　BindPoseそのままの向きを返す
		*dstmat = convmat;

		//#####################################################################################################
		//位置は　ボーンの(呼び出すときの)親の位置　つまり(呼び出されたインスタンスの)カレントジョイントの位置
		//#####################################################################################################
		dstmat->SetTranslation(aftbonepos);
	}
	

	return retleng;
}

float CBone::CalcAxisMatX_NodeMat(CBone* childbone, ChaMatrix* dstmat)
{
	ChaVector3 aftbonepos;
	ChaVector3 aftchildpos;

	if (!dstmat) {
		_ASSERT(0);
		return 0.0f;
	}
	if (!childbone) {
		_ASSERT(0);
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}


	//2023/05/09 位置はセット可能なので下方のaftbonepos == aftchildposのところと一緒にした
	////2023/04/28
	//if (IsNotSkeleton()) {
	//	ChaMatrix inimat;
	//	ChaMatrixIdentity(&inimat);
	//	*dstmat = inimat;
	//	return 0.0f;
	//}


	ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);

	ChaVector3 tmpfpos = GetJointFPos();
	ChaVector3 tmpchildfpos = childbone->GetJointFPos();
	ChaMatrix convmat;
	convmat.SetIdentity();

	ChaMatrix tmpnodefm = GetNodeMat();
	ChaMatrix tmpchildnodefm = childbone->GetNodeMat();
	ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmpnodefm);
	ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildnodefm);
	convmat = tmpnodefm;


	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (IsNotSkeleton() || (aftbonepos == aftchildpos)) {
		//長さ０ボーン対策
		*dstmat = retmat;
		dstmat->SetTranslation(aftbonepos);
		//_ASSERT(0);
		return 0.0f;
	}


	//カレント変換したボーン軸
	ChaVector3 bonevec;
	bonevec = aftchildpos - aftbonepos;
	ChaVector3Normalize(&bonevec, &bonevec);

	//###########################################################################################
	//convmatのvecxをbonevecにする　それに合わせて３軸が互いに垂直になるようにvecy, veczを求める
	//###########################################################################################
	//#########################################################
	//位置は　ボーンの親の位置　つまりカレントジョイントの位置
	//#########################################################

	*dstmat = CalcAxisMatX(bonevec, aftbonepos, convmat);//ChaVecCalc.cpp

	ChaVector3 diffvec = aftbonepos - aftchildpos;
	float retleng = (float)ChaVector3LengthDbl(&diffvec);

	return retleng;


}

float CBone::CalcAxisMatX_RigidBody(bool limitdegflag, bool dir2xflag, int bindflag, CBone* childbone, ChaMatrix* dstmat, int setstartflag)
{
	//#############################################################################################
	//2022/11/03
	//RigidBody用　basevecは マニピュレータと同じvecx(RigidBody形状はCapsule_dirX.mqo)
	//CalcAxisMatX_Manipulatorと違うところは　g_ikaxis_kindによらず　RigidBody用の計算をするところ
	//#############################################################################################

	//########################################################################################################################
	//2023/01/18
	//dir2xflagを追加
	//bullet physicsのrigidbodyの回転モーションの軸を　通常モーションの軸と合わせるためには　NodeMatをX軸に向けないことが必要
	//dir2xflag == falseのときには　NodeMatをX軸に向けない
	// 
	//カプセル形状の表示のためには　NodeMatをX軸に向ける必要がある　この時には dir2xflagをtrueにしてこの関数を呼ぶ
	//########################################################################################################################

	ChaVector3 aftbonepos;
	ChaVector3 aftchildpos;

	if (!dstmat) {
		_ASSERT(0);
		return 0.0f;
	}
	if (!childbone) {
		_ASSERT(0);
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}
	//2023/04/28
	if (IsNotSkeleton()) {
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		*dstmat = inimat;
		return 0.0f;
	}

	ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);

	ChaVector3 tmpfpos = GetJointFPos();
	ChaVector3 tmpchildfpos = childbone->GetJointFPos();
	ChaMatrix convmat;
	convmat.SetIdentity();

	if (bindflag == 1) {
		//bind pose
		ChaMatrix tmpzerofm = GetNodeMat() * GetCurrentZeroFrameMat(limitdegflag, 1);
		ChaMatrix tmpchildzerofm = childbone->GetNodeMat() * childbone->GetCurrentZeroFrameMat(limitdegflag, 1);
		ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmpzerofm);
		ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildzerofm);

		convmat = tmpzerofm;
	}
	else {
		if ((g_previewFlag != 5) && (g_previewFlag != 4)) {
			ChaMatrix tmpzerofm = GetNodeMat() * GetCurrentZeroFrameMat(limitdegflag, 1);
			ChaMatrix tmplimwm = GetNodeMat() * GetCurrentWorldMat(true);
			ChaMatrix tmpchildzerofm = childbone->GetNodeMat() * childbone->GetCurrentZeroFrameMat(limitdegflag, 1);
			ChaMatrix tmpchildlimwm = childbone->GetNodeMat() * childbone->GetCurrentWorldMat(true);

			if (setstartflag == 1) {
				ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmpzerofm);
				ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildzerofm);

				convmat = tmpzerofm;
			}
			else {
				ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmplimwm);
				ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildlimwm);

				convmat = tmplimwm;
			}
		}
		else {
			ChaMatrix tmpzerofm = GetNodeMat() * GetCurrentZeroFrameMat(limitdegflag, 1);
			ChaMatrix tmpbtmat = GetNodeMat() * GetBtMat();
			ChaMatrix tmpchildzerofm = childbone->GetNodeMat() * childbone->GetCurrentZeroFrameMat(limitdegflag, 1);
			ChaMatrix tmpchildbtmat = childbone->GetNodeMat() * childbone->GetBtMat();
			if (setstartflag == 1) {
				ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmpzerofm);
				ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildzerofm);

				convmat = tmpzerofm;
			}
			else {
				ChaVector3TransformCoord(&aftbonepos, &zeropos, &tmpbtmat);
				ChaVector3TransformCoord(&aftchildpos, &zeropos, &tmpchildbtmat);

				convmat = tmpbtmat;
			}
		}
	}
	
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (aftbonepos == aftchildpos) {

		//長さ０ボーン対策
		*dstmat = retmat;
		dstmat->SetTranslation(aftbonepos);
		//_ASSERT(0);
		return 0.0f;
	}


	//カレント変換したボーン軸
	ChaVector3 bonevec;
	bonevec = aftchildpos - aftbonepos;
	ChaVector3Normalize(&bonevec, &bonevec);

	//###########################################################################################
	//convmatのvecxをbonevecにする　それに合わせて３軸が互いに垂直になるようにvecy, veczを求める
	//###########################################################################################
	//#########################################################
	//位置は　ボーンの親の位置　つまりカレントジョイントの位置
	//#########################################################
	

	//2023/01/18
	if (dir2xflag == true) {
		*dstmat = CalcAxisMatX(bonevec, aftbonepos, convmat);//ChaVecCalc.cpp
	}
	else {
		*dstmat = convmat;
	}
	
	ChaVector3 diffvec = aftbonepos - aftchildpos;
	float retleng = (float)ChaVector3LengthDbl(&diffvec);

	return retleng;
}




/*
int CBone::CalcAxisMatX()
{
	ChaVector3 curpos;
	ChaVector3 childpos;

	if (m_parent){
		ChaVector3TransformCoord(&curpos, &(m_parent->GetJointFPos()), &(m_parent->m_startmat2));
		ChaVector3TransformCoord(&childpos, &(GetJointFPos()), &m_startmat2);

		CalcAxisMatX_aft(curpos, childpos, &m_gaxismatXpar);
	}

	return 0;
}
*/

/*
int CBone::CalcAxisMatX_aft(ChaVector3 curpos, ChaVector3 childpos, ChaMatrix* dstmat)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (curpos == childpos){
		*dstmat = retmat;
		return 0;
	}

	ChaVector3 startpos, endpos, upvec;

	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 vecx1, vecy1, vecz1;

	startpos = curpos;
	endpos = childpos;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize(&bonevec, &bonevec);

	if ((fabs(bonevec.x) >= 0.000001f) || (fabs(bonevec.y) >= 0.000001f)){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
		m_upkind = UPVEC_Z;//vecx1-->X(bone), vecy1-->Y, vecz1-->Z
	}
	else{
		upvec.x = 0.0f;
		upvec.y = 1.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_Y;//vecx1-->X(bone), vecy1-->Z, vecz1-->Y
	}

	vecx1 = bonevec;

	ChaVector3Cross(&vecy1, (const ChaVector3*)&upvec, (const ChaVector3*)&vecx1);

	int illeagalflag = 0;
	float crleng = ChaVector3LengthDbl(&vecy1);
	if (crleng < 0.000001f){
		illeagalflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	ChaVector3Normalize(&vecy1, &vecy1);


	ChaVector3Cross(&vecz1, (const ChaVector3*)&vecx1, (const ChaVector3*)&vecy1);
	ChaVector3Normalize(&vecz1, &vecz1);


	ChaMatrixIdentity(dstmat);
	if (illeagalflag == 0){
		dstmat->_11 = vecx1.x;
		dstmat->_12 = vecx1.y;
		dstmat->_13 = vecx1.z;

		dstmat->_21 = vecy1.x;
		dstmat->_22 = vecy1.y;
		dstmat->_23 = vecy1.z;

		dstmat->_31 = vecz1.x;
		dstmat->_32 = vecz1.y;
		dstmat->_33 = vecz1.z;
	}

	return 0;
}
*/

int CBone::CalcAxisMatZ_aft(ChaVector3 curpos, ChaVector3 childpos, ChaMatrix* dstmat)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (curpos == childpos){
		*dstmat = retmat;
		return 0;
	}

	//2023/04/28
	if (IsNotSkeleton()) {
		*dstmat = retmat;
		return 0;
	}


	ChaVector3 startpos, endpos, upvec;

	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 vecx1, vecy1, vecz1;

	startpos = curpos;
	endpos = childpos;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize(&bonevec, &bonevec);

	if ((fabs(bonevec.x) >= 0.000001f) || (fabs(bonevec.z) >= 0.000001f)){
		upvec.x = 0.0f;
		upvec.y = 1.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_Y;//vecx1-->X, vecy1-->Y, vecz1-->Z(bone)
	}
	else{
		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_X;//vecx1-->Y, vecy1-->X, vecz1-->Z(bone)
	}

	vecz1 = bonevec;

	//ChaVector3Cross(&vecx1, &vecz1, &upvec);
	ChaVector3Cross(&vecx1, (const ChaVector3*)&upvec, (const ChaVector3*)&vecz1);
	ChaVector3Normalize(&vecx1, &vecx1);

	ChaVector3Cross(&vecy1, (const ChaVector3*)&vecz1, (const ChaVector3*)&vecx1);
	ChaVector3Normalize(&vecy1, &vecy1);

	retmat.SetRow(0, vecx1);
	retmat.SetRow(1, vecy1);
	retmat.SetRow(2, vecz1);

	*dstmat = retmat;

	return 0;
}



//int CBone::CalcAxisMatY( CBone* childbone, ChaMatrix* dstmat )
//{
//
//	ChaVector3 curpos;
//	ChaVector3 childpos;
//	ChaVector3 tmpfpos = GetJointFPos();
//	ChaMatrix tmpwm = GetWorldMat(m_curmotid, m_curmp.GetFrame(), &m_curmp);
//	ChaVector3TransformCoord(&curpos, &tmpfpos, &tmpwm);
//	//ChaVector3TransformCoord(&childpos, &(childbone->GetJointFPos()), &(childbone->m_curmp.GetWorldMat()));
//	ChaVector3 tmpchildfpos = childbone->GetJointFPos();
//	ChaVector3TransformCoord(&childpos, &tmpchildfpos, &tmpwm);
//
//	ChaVector3 diff = curpos - childpos;
//	float leng;
//	leng = (float)ChaVector3LengthDbl( &diff );
//
//	if( leng <= 0.00001f ){
//		ChaMatrixIdentity( dstmat );
//		return 0;
//	}
//
//	ChaVector3 startpos, endpos, upvec;
//
//	ChaVector3 vecx0, vecy0, vecz0;
//	ChaVector3 vecx1, vecy1, vecz1;
//
//	startpos = curpos;
//	endpos = childpos;
//
//	vecx0.x = 1.0;
//	vecx0.y = 0.0;
//	vecx0.z = 0.0;
//
//	vecy0.x = 0.0;
//	vecy0.y = 1.0;//!!!!!!!!!!!!!!!!!!
//	vecy0.z = 0.0;
//
//	vecz0.x = 0.0;
//	vecz0.y = 0.0;
//	vecz0.z = 1.0;
//
//	ChaVector3 bonevec;
//	bonevec = endpos - startpos;
//	ChaVector3Normalize( &bonevec, &bonevec );
//
//	if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
//		upvec.x = 0.0f;
//		upvec.y = 0.0f;
//		upvec.z = 1.0f;
//	}else{
//		upvec.x = 1.0f;
//		upvec.y = 0.0f;
//		upvec.z = 0.0f;
//	}
//
//	vecy1 = bonevec;
//		
//	ChaVector3Cross( &vecx1, (const ChaVector3*)&vecy1, (const ChaVector3*)&upvec );
//
//	int illeagalflag = 0;
//	float crleng = (float)ChaVector3LengthDbl( &vecx1 );
//	if( crleng < 0.000001f ){
//		illeagalflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	}
//
//	ChaVector3Normalize( &vecx1, &vecx1 );
//
//	ChaVector3Cross( &vecz1, (const ChaVector3*)&vecx1, (const ChaVector3*)&vecy1 );
//	ChaVector3Normalize( &vecy1, &vecy1 );
//
//	//D3DXQUATERNION tmpxq;
//
//	ChaMatrixIdentity( dstmat );
//	if( illeagalflag == 0 ){
//		dstmat->data[0] = vecx1.x;
//		dstmat->data[1] = vecx1.y;
//		dstmat->data[2] = vecx1.z;
//
//		dstmat->data[4] = vecy1.x;
//		dstmat->data[5] = vecy1.y;
//		dstmat->data[6] = vecy1.z;
//
//		dstmat->data[8] = vecz1.x;
//		dstmat->data[9] = vecz1.y;
//		dstmat->data[10] = vecz1.z;
//	}
//
//	return 0;
//}

int CBone::CalcRigidElemParams(bool setinstancescale, CBone* childbone, int setstartflag )
{
	
	ChaMatrix retmat;
	retmat.SetIdentity();

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	//剛体の形状(m_coldisp)を複数の子供で使いまわしている。使用するたびにこの関数で大きさをセットしている。
	if (!childbone) {
		_ASSERT(0);
		return 0;
	}
	if (childbone->IsNotSkeleton()) {
		return 0;
	}
	//if (childbone->GetParent(false) != this) {
	//	return 0;
	//}

	CRigidElem* curre = GetRigidElem(childbone);
	if( !curre ){
		//_ASSERT( 0 );
		return 0;
	}

	//_ASSERT( colptr );
	_ASSERT( childbone );

	CModel* curcoldisp = s_coldisp[curre->GetColtype()];
	_ASSERT( curcoldisp );


	//ChaMatrix bmmat;
	//ChaMatrixIdentity(&bmmat);
	//CalcAxisMatZ( &aftbonepos, &aftchildpos );
	//CalcAxisMatY( childbone, &bmmat );			
	//float diffleng = CalcAxisMatX(0, childbone, &bmmat, 1);

	//ChaMatrix bindcapsulemat;
	//ChaMatrixIdentity(&bindcapsulemat);
	//bool dir2xflag = false;
	//float diffleng = CalcAxisMatX_RigidBody(dir2xflag, 1, childbone, &bindcapsulemat, 1);

	ChaVector3 jointpos, childjointpos, diffvec;
	jointpos = GetJointFPos();
	childjointpos = childbone->GetJointFPos();
	diffvec = jointpos - childjointpos;
	float diffleng = (float)ChaVector3LengthDbl(&diffvec);


	float cylileng = curre->GetCylileng();
	float sphr = curre->GetSphr();
	float boxz = curre->GetBoxz();

	//if ((setstartflag != 0) || (m_firstcalcrigid == true)){
	if (curre->GetColtype() == COL_CAPSULE_INDEX) {
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			if (strcmp(curobj->GetName(), "cylinder") == 0) {
				CallF(curobj->ScaleBtCapsule(setinstancescale, curre, diffleng, 0, &cylileng), return 1);
			}
			else if (strcmp(curobj->GetName(), "sph_ue") == 0) {
				CallF(curobj->ScaleBtCapsule(setinstancescale, curre, diffleng, 1, &sphr), return 1);
			}
			else {
				CallF(curobj->ScaleBtCapsule(setinstancescale, curre, diffleng, 2, 0), return 1);
			}
		}
	}
	else if (curre->GetColtype() == COL_CONE_INDEX) {
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			CallF(curobj->ScaleBtCone(setinstancescale, curre, diffleng, &cylileng, &sphr), return 1);
		}
	}
	else if (curre->GetColtype() == COL_SPHERE_INDEX) {
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			CallF(curobj->ScaleBtSphere(setinstancescale, curre, diffleng, &cylileng, &sphr), return 1);
		}
	}
	else if (curre->GetColtype() == COL_BOX_INDEX) {
		map<int, CMQOObject*>::iterator itrobj;
		for (itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++) {
			CMQOObject* curobj = itrobj->second;
			_ASSERT(curobj);
			CallF(curobj->ScaleBtBox(setinstancescale, curre, diffleng, &cylileng, &sphr, &boxz), return 1);
#ifndef NDEBUG
			DbgOut(L"bonecpp : calcrigidelemparams : BOX : cylileng %f, sphr %f, boxz %f\r\n", cylileng, sphr, boxz);
#endif
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}
	//}

	//bmmat._41 = ( aftbonepos.x + aftchildpos.x ) * 0.5f;
	//bmmat._42 = ( aftbonepos.y + aftchildpos.y ) * 0.5f;
	//bmmat._43 = ( aftbonepos.z + aftchildpos.z ) * 0.5f;

	//bmmat._41 = aftbonepos.x;
	//bmmat._42 = aftbonepos.y;
	//bmmat._43 = aftbonepos.z;

	//curre->SetBindcapsulemat(bindcapsulemat);
	//bmmat = curre->GetEndbone()->CalcManipulatorPostureMatrix(0, 1, 1);

	//curre->SetCapsulemat( bmmat );
	curre->SetCylileng(cylileng);
	curre->SetSphr(sphr);
	curre->SetBoxz(boxz);

	if( setstartflag != 0 ){
		//bmmat = curre->GetCapsulemat(1);
		//curre->SetFirstcapsulemat( bmmat );
		//curre->SetFirstWorldmat(childbone->GetCurrentZeroFrameMat(0));
		//curre->SetFirstWorldmat(GetCurMp().GetWorldMat());

		//if (setstartflag == 2){
		//	childbone->SetNodeMat(bmmat);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//}

		bool calcslotflag = true;
		childbone->SetBtMat(childbone->GetCurMp(calcslotflag).GetWorldMat());//!!!!!!!!!!!!!btmatの初期値
	}


	m_firstcalcrigid = false;

	return 0;
}

//void CBone::SetStartMat2Req()
//{
//	//SetStartMat2(m_curmp.GetWorldMat());
//	SetStartMat2(GetCurrentZeroFrameMat(0));
//	SetBtMat(GetWorldMat(m_curmotid, m_curmp.GetFrame(), &m_curmp));//!!!!!!!!!!!!!btmatの初期値
//
//	if (m_child){
//		m_child->SetStartMat2Req();
//	}
//	if (m_brother){
//		m_brother->SetStartMat2Req();
//	}
//}

/*
int CBone::CalcAxisMat( int firstflag, float delta )
{
	if( firstflag == 1 ){
		SetStartMat2Req();
		
		CalcAxisMatX();
	}

	if( delta != 0.0f ){
		CQuaternion multq;
		ChaVector3 gparentpos, gchildpos, gbonevec;
		if (m_parent){
			ChaVector3TransformCoord(&gparentpos, &(m_parent->GetJointFPos()), &(m_parent->m_startmat2));
			ChaVector3TransformCoord(&gchildpos, &(GetJointFPos()), &m_startmat2);
			gbonevec = gchildpos - gparentpos;
			ChaVector3Normalize(&gbonevec, &gbonevec);
		}
		else{
			_ASSERT(0);
			return 0;
		}


		multq.SetAxisAndRot( gbonevec, delta );
		ChaMatrix multmat = multq.MakeRotMatX();
		m_gaxismatXpar = m_gaxismatXpar * multmat;
	}

	ChaMatrix invpar;
	ChaMatrixInverse( &invpar, NULL, &m_gaxismatXpar );

	CalcLocalAxisMat( m_startmat2, m_gaxismatXpar, m_gaxismatYpar );

	return 0;
}
*/
//int CBone::CalcLocalAxisMat( ChaMatrix motmat, ChaMatrix axismatpar, ChaMatrix gaxisy )
//{
//	ChaMatrix startpar0 = axismatpar;
//	startpar0.data[MATI_41] = 0.0f;
//	startpar0.data[MATI_42] = 0.0f;
//	startpar0.data[MATI_43] = 0.0f;
//
//	ChaMatrix starty = gaxisy;
//	starty.data[MATI_41] = 0.0f;
//	starty.data[MATI_42] = 0.0f;
//	starty.data[MATI_43] = 0.0f;
//
//	ChaMatrix motmat0 = motmat;
//	motmat0.data[MATI_41] = 0.0f;
//	motmat0.data[MATI_42] = 0.0f;
//	motmat0.data[MATI_43] = 0.0f;
//
//	ChaMatrix invmotmat;
//	ChaMatrixInverse( &invmotmat, NULL, &motmat0 );
//
//	m_axismat_par = startpar0 * invmotmat;
//
//	return 0;
//}

int CBone::CreateRigidElem( CBone* parentbone, int reflag, std::string rename, int impflag, std::string impname )
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	if (!parentbone){
		return 0;
	}

	if (parentbone->IsNotSkeleton() && !IsHipsBone()) {
		return 0;
	}
	//childがhipsの場合は parentはeNullも可



	if (reflag){
		//CRigidElem* newre = new CRigidElem();
		CRigidElem* newre = CRigidElem::GetNewRigidElem();
		if (!newre){
			_ASSERT(0);
			return 1;
		}
		newre->SetBone(parentbone);
		newre->SetEndbone(this);
		SetGroupNoByName(newre, this);
		parentbone->SetRigidElemOfMap(rename, this, newre);
	}

//////////////
	if( impflag ){
		map<string, map<CBone*, ChaVector3>>::iterator findimpmap;
		findimpmap = parentbone->FindImpMap(impname);
		if (findimpmap != parentbone->GetImpMapEnd()){
			map<CBone*, ChaVector3>::iterator itrimp;
			itrimp = findimpmap->second.find(this);
			if (itrimp != findimpmap->second.end()){
				return 0;
			}
			findimpmap->second[this] = ChaVector3(0.0f, 0.0f, 0.0f);
		}else{
			map<CBone*, ChaVector3> curmap;

			curmap[this] = ChaVector3( 0.0f, 0.0f, 0.0f );
			parentbone->m_impmap[impname] = curmap;
		}
	}
	return 0;
}

int CBone::SetGroupNoByName( CRigidElem* curre, CBone* childbone )
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	char* groupmark = strstr( childbone->m_bonename, "_G_" );
	if( groupmark ){
		char* numstart = groupmark + 3;
		char* numend = strstr( numstart, "_" );
		if( numend ){
			int numleng = (int)(numend - numstart);
			if( (numleng > 0) && (numleng <= 2) ){
				char strnum[5];
				ZeroMemory( strnum, sizeof( char ) * 5 );
				strncpy_s( strnum, 5, numstart, numleng );
				int gno = (int)atoi( strnum );
				if( (gno >= 1) && (gno <= COLIGROUPNUM) ){
					curre->SetGroupid( gno ); 
				}
			}
		}
	}
	int cmpbt = strncmp( childbone->m_bonename, "BT_", 3 );
	if( cmpbt == 0 ){
		m_btforce = 1;
	}

	return 0;
}

int CBone::SetCurrentRigidElem( std::string curname )
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	m_rigidelemname = curname;

	/*
	m_rigidelem.clear();

	if( !m_child ){
		return 0;
	}

	map<string, map<CBone*, CRigidElem*>>::iterator itrmap;
	itrmap = m_remap.find( curname );
	if( itrmap == m_remap.end() ){
		_ASSERT( 0 );
		::MessageBoxA(NULL, "CBone SetCurrentRigidElem : map not found", "error", MB_OK);
		return 1;
	}

	m_rigidelem = itrmap->second;
	*/


	return 0;
}


CMotionPoint* CBone::AddBoneTraReq(bool limitdegflag, CMotionPoint* parmp, int srcmotid, double srcframe, 
	ChaVector3 srctra, ChaMatrix befparentwm, ChaMatrix newparentwm)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.AddBoneTraReq(this, limitdegflag, parmp, srcmotid, srcframe, srctra, befparentwm, newparentwm);
}


CMotionPoint* CBone::AddBoneScaleReq(bool limitdegflag, CMotionPoint* parmp, int srcmotid, double srcframe, ChaVector3 srcscale, ChaMatrix befparentwm, ChaMatrix newparentwm)
{


	double roundingframe = RoundingTime(srcframe);


	//2023/04/28
	if (IsNotSkeleton()) {
		_ASSERT(0);
		return 0;
	}


	int existflag = 0;
	//CMotionPoint* curmp = AddMotionPoint(srcmotid, srcframe, &existflag);
	//if (!curmp || !existflag) {
	CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 0;
	}

	ChaMatrix currentbefwm;
	ChaMatrix currentnewwm;
	currentbefwm.SetIdentity();
	currentnewwm.SetIdentity();
	currentbefwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);


	bool infooutflag = false;


	//curmp->SetBefWorldMat( curmp->GetWorldMat() );
	if (parmp) {
		//ChaMatrix invbefpar;
		//ChaMatrix tmpparbefwm = parmp->GetBefWorldMat();//!!!!!!! 2022/12/23 引数にするべき
		//ChaMatrixInverse(&invbefpar, NULL, &tmpparbefwm);
		//ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * parmp->GetWorldMat();
		ChaMatrix tmpmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp) * ChaMatrixInv(befparentwm) * newparentwm;
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		SetWorldMat(limitdegflag, directsetflag, infooutflag, 0, srcmotid, roundingframe, tmpmat, onlycheck, fromiktarget);

		currentnewwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	}
	else {
		ChaVector3 curpos = GetJointFPos();
		ChaMatrix beftramat;
		beftramat.SetIdentity();//2023/02/12
		ChaMatrixTranslation(&beftramat, -curpos.x, -curpos.y, -curpos.z);
		ChaMatrix afttramat;
		afttramat.SetIdentity();//2023/02/12
		ChaMatrixTranslation(&afttramat, curpos.x, curpos.y, curpos.z);
		ChaMatrix scalemat;
		scalemat.SetIdentity();//2023/02/12
		ChaMatrixScaling(&scalemat, srcscale.x, srcscale.y, srcscale.z);
		ChaMatrix tramat;
		tramat = beftramat * scalemat * afttramat;
		//ChaMatrix tmpmat = curmp->GetWorldMat() * tramat;
		ChaMatrix tmpmat = tramat * GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		SetWorldMat(limitdegflag, directsetflag, infooutflag, 0, srcmotid, roundingframe, tmpmat, onlycheck, fromiktarget);

		currentnewwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

		////ChaVector3 cureul;
		////cureul = CalcLocalEulXYZ(-1, srcmotid, srcframe, BEFEUL_BEFFRAME);
		//ChaMatrix parentmat;
		//if (GetParent()) {
		//	CMotionPoint* parentmp;
		//	parentmp = GetParent()->GetMotionPoint(srcmotid, srcframe);
		//	if (parentmp) {
		//		parentmat = parentmp->GetWorldMat();
		//	}
		//	else {
		//		ChaMatrixIdentity(&parentmat);
		//	}
		//}
		//else {
		//	ChaMatrixIdentity(&parentmat);
		//}

		//ChaMatrix curlocalmat;
		//curlocalmat = curmp->GetWorldMat() * ChaMatrixInv(parentmat);
		//ChaMatrix cursmat, currmat, curtmat;
		//GetSRTMatrix2(curlocalmat, &cursmat, &currmat, &curtmat);

		//ChaVector3 cursvec, curtvec;
		//GetSRTMatrix(curlocalmat, &cursvec, &currmat, &curtvec);

		//ChaVector3 curtraanim;
		//curtraanim = CalcLocalTraAnim(srcmotid, srcframe);
		//ChaMatrix curtanimmat;
		//ChaMatrixIdentity(&curtanimmat);
		//ChaMatrixTranslation(&curtanimmat, curtraanim.x, curtraanim.y, curtraanim.z);

		////CQuaternion rotq;
		////rotq.SetRotationXYZ(0, cureul);
		////ChaVector3 curtra;
		////curtra = CalcLocalTraAnim(srcmotid, srcframe);
		////ChaMatrix curtramat;
		////ChaMatrixIdentity(&curtramat);
		////ChaMatrixTranslation(&curtramat, curtra.x, curtra.y, curtra.z);


		//ChaVector3 curpos = GetJointFPos();
		//ChaMatrix beftramat;
		//ChaMatrixIdentity(&beftramat);
		//ChaMatrixTranslation(&beftramat, -curpos.x, -curpos.y, -curpos.z);
		//ChaMatrix afttramat;
		//ChaMatrixIdentity(&afttramat);
		//ChaMatrixTranslation(&afttramat, curpos.x, curpos.y, curpos.z);
		//ChaMatrix scalemat;
		//ChaMatrixIdentity(&scalemat);
		//ChaMatrixScaling(&scalemat, srcscale.x, srcscale.y, srcscale.z);
		////ChaMatrix tramat;
		////tramat = beftramat * scalemat * afttramat;
		//////ChaMatrix tmpmat = curmp->GetWorldMat() * tramat;
		////ChaMatrix tmpmat = tramat * curmp->GetWorldMat();
		////g_wmatDirectSetFlag = true;
		////SetWorldMat(infooutflag, 0, srcmotid, srcframe, tmpmat);
		////g_wmatDirectSetFlag = false;

		//ChaMatrix newmat;
		//newmat = beftramat * scalemat * cursmat * currmat * afttramat * curtanimmat * parentmat;

		////g_wmatDirectSetFlag = true;
		////SetWorldMat(infooutflag, 0, srcmotid, srcframe, newmat);
		////g_wmatDirectSetFlag = false;

		//curmp->SetWorldMat(newmat);


	}

	curmp->SetAbsMat(GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));

	if (GetChild(false)) {
		GetChild(false)->AddBoneScaleReq(limitdegflag, curmp, srcmotid, roundingframe, srcscale, currentbefwm, currentnewwm);
	}
	if (GetBrother(false) && parmp) {
		GetBrother(false)->AddBoneScaleReq(limitdegflag, parmp, srcmotid, roundingframe, srcscale, befparentwm, newparentwm);
	}

	return curmp;
}


void CBone::PasteRotReq(bool limitdegflag, int srcmotid, double srcframe, double dstframe)
{
	//src : srcmp srcparmp
	//dst : curmp parmp

	double roundingframe = RoundingTime(srcframe);
	CMotionPoint* curmp = 0;

	if (IsSkeleton()) {

		int existflag0 = 0;
		//CMotionPoint* srcmp = AddMotionPoint( srcmotid, srcframe, &existflag0 );
		//if( !existflag0 || !srcmp ){
		CMotionPoint* srcmp = GetMotionPoint(srcmotid, roundingframe);
		if (!srcmp) {
			_ASSERT(0);
			return;
		}

		int existflag = 0;
		//CMotionPoint* curmp = AddMotionPoint( srcmotid, dstframe, &existflag );
		//if( !existflag || !curmp ){
		curmp = GetMotionPoint(srcmotid, roundingframe);
		if (!curmp) {
			_ASSERT(0);
			return;
		}

		//curmp->SetBefWorldMat( curmp->GetWorldMat() );
		curmp->SetWorldMat(GetWorldMat(limitdegflag, srcmotid, roundingframe, srcmp));
		curmp->SetAbsMat(srcmp->GetAbsMat());


		//オイラー角初期化
		ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
		int paraxsiflag = 1;
		//int isfirstbone = 0;
		//cureul = CalcLocalEulXYZ(-1, srcmotid, srcframe, BEFEUL_ZERO);
		cureul = CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
		SetLocalEul(limitdegflag, srcmotid, roundingframe, cureul, 0);
	}

	if (GetChild(false)) {
		GetChild(false)->PasteRotReq(limitdegflag, srcmotid, roundingframe, dstframe);
	}
	if (GetBrother(false)) {
		GetBrother(false)->PasteRotReq(limitdegflag, srcmotid, roundingframe, dstframe);
	}
	//return curmp;
}

ChaMatrix CBone::CalcNewLocalRotMatFromQofIK(bool limitdegflag, int srcmotid, double srcframe, CQuaternion qForRot, ChaMatrix* dstsmat, ChaMatrix* dstrmat, ChaMatrix* dsttanimmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.CalcNewLocalRotMatFromQofIK(this, limitdegflag, srcmotid, srcframe,
		qForRot, dstsmat, dstrmat, dsttanimmat);
}

ChaMatrix CBone::CalcNewLocalTAnimMatFromSRTraAnim(ChaMatrix srcnewlocalrotmat, 
	ChaMatrix srcsmat, ChaMatrix srcrmat, ChaMatrix srctanimmat, ChaVector3 zeroframetanim)
{
	//############# 引数traanimはローカル姿勢 ###########



	//2023/02/08
	//この関数は　RotAndTraBoneQReqからしか呼ばれていない
	//リターゲット時には　RotBoneQReqを呼ぶので　この関数は呼ばれない
	//グローバル変数による機能選択は避ける　必要な場合は引数にする


	ChaMatrix newtanimmatrotated;
	newtanimmatrotated.SetIdentity();

	//2023/04/28
	if (IsNotSkeleton()) {
		return newtanimmatrotated;
	}

	//if (g_underRetargetFlag == false) {

		//traanimをローカル回転する　ただしリターゲット時以外 (IK時などに回転する)
		//newtanimvec = traanim + ChaMatrixTraVec(srctanimmat);//現在のtanimに引数で移動分を指定する場合
		ChaVector3 difftanimvec;
		difftanimvec = ChaMatrixTraVec(srctanimmat) - zeroframetanim;//現在のtanimと0frameのtanimの差分
		//difftanimvec = ChaMatrixTraVec(srctanimmat);


		CQuaternion nodeq;
		CQuaternion curq;
		CQuaternion newlocalrotq;
		CQuaternion additionallocalrotq;
		ChaMatrix additionallocalrotmat;
		nodeq.RotationMatrix(GetNodeMat());
		additionallocalrotmat.SetIdentity();
		curq.RotationMatrix(srcrmat);
		newlocalrotq.RotationMatrix(srcnewlocalrotmat);

		additionallocalrotq = curq.inverse() * newlocalrotq;//!!!!!!!
		//additionallocalrotq = nodeq * (curq.inverse() * newlocalrotq) * nodeq.inverse();//!!!!!!!
		//additionallocalrotq = nodeq.inverse() * (curq.inverse() * newlocalrotq) * nodeq;//!!!!!!!
		
		additionallocalrotmat = additionallocalrotq.MakeRotMatX();

		ChaVector3 diffanimvecrotated;
		ChaVector3TransformCoord(&diffanimvecrotated, &difftanimvec, &additionallocalrotmat);//vector回転のために　回転行列で変換
		newtanimmatrotated.SetIdentity();
		newtanimmatrotated.SetTranslation(diffanimvecrotated + zeroframetanim);
		//newtanimmatrotated.SetTranslation(diffanimvecrotated);
	//}
	//else {
	//	//リターゲット時には　traanimをそのままセットするので　traanimの回転はしない
	//	newtanimmatrotated.SetIdentity();
	//	newtanimmatrotated.SetTranslation(ChaMatrixTraVec(srctanimmat));
	//}

	return newtanimmatrotated;
}

void CBone::UpdateCurrentWM(bool limitdegflag, int srcmotid, double srcframe,
	ChaMatrix newwm)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.UpdateCurrentWM(this, limitdegflag, srcmotid, srcframe, newwm);
}



void CBone::UpdateParentWMReq(bool limitdegflag, bool setbroflag, int srcmotid, double srcframe, 
	ChaMatrix oldparentwm, ChaMatrix newparentwm)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.UpdateParentWMReq(this, limitdegflag, setbroflag, srcmotid, srcframe, oldparentwm, newparentwm);
}




//##########################################################
//CBone::RotBoneQReq()
//引数rotqはグローバル回転　引数traanimはローカル移動アニメ
//##########################################################
CMotionPoint* CBone::RotBoneQReq(bool limitdegflag, bool infooutflag, 
	CBone* parentbone, int srcmotid, double srcframe, 
	CQuaternion rotq, ChaMatrix srcbefparentwm, ChaMatrix srcnewparentwm,
	CBone* bvhbone, ChaVector3 traanim)// , int setmatflag, ChaMatrix* psetmat, bool onretarget)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.RotBoneQReq(this, limitdegflag, infooutflag,
		parentbone, srcmotid, srcframe,
		rotq, srcbefparentwm, srcnewparentwm,
		bvhbone, traanim);
}


int CBone::SaveSRT(bool limitdegflag, int srcmotid, double srcframe)
{
	double curframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, curframe);
	if (curmp) {
		ChaMatrix curwm, parentwm, localmat;
		curwm = GetWorldMat(limitdegflag, srcmotid, curframe, curmp);
		if (GetParent(false)) {
			parentwm = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, curframe, 0);
			localmat = curwm * ChaMatrixInv(parentwm);
		}
		else {
			parentwm.SetIdentity();
			localmat = curwm;
		}

		ChaMatrix smat, rmat, tmat, tanimmat;
		smat.SetIdentity();
		rmat.SetIdentity();
		tmat.SetIdentity();
		tanimmat.SetIdentity();
		GetSRTandTraAnim(localmat, GetNodeMat(), &smat, &rmat, &tmat, &tanimmat);
		curmp->SetSaveSRTandTraAnim(smat, rmat, tmat, tanimmat);
	}
	return 0;
}



//###############################
//2023/03/04 ismovableをリターン
//###############################
int CBone::RotAndTraBoneQReq(bool limitdegflag, int* onlycheckptr,
	double srcstartframe, bool infooutflag, CBone* parentbone, int srcmotid, double srcframe,
	CQuaternion qForRot, CQuaternion qForHipsRot, bool fromiktarget)
{
	//######################################
	//IK用.　RetargetはRotBoneQReq()を使用
	//hipsはtanimを qForHipsRot で回転する
	//hips以外はtanimを qForRot　で回転する
	//######################################

	//###################################################################
	//onlycheckptr != NULLの場合には
	//SetWorldMatをonlycheckで呼び出して　回転可能かどうかだけを調べる
	//初回呼び出し時のCBoneに対してだけチェックをして　直ちにリターンする
	//###################################################################


	ChaCalcFunc chacalcfunc;
	return chacalcfunc.RotAndTraBoneQReq(this, limitdegflag, onlycheckptr,
		srcstartframe, infooutflag, parentbone, srcmotid, srcframe,
		qForRot, qForHipsRot, fromiktarget);
}


//CMotionPoint* CBone::RotBoneQCurrentReq(bool infooutflag, CBone* parbone, int srcmotid, double srcframe, CQuaternion rotq, CBone* bvhbone, ChaVector3 traanim, int setmatflag, ChaMatrix* psetmat)
//{
//	int existflag = 0;
//	//CMotionPoint* curmp = AddMotionPoint(srcmotid, srcframe, &existflag);
//	//if (!existflag || !curmp) {
//	//	_ASSERT(0);
//	//	return 0;
//	//}
//	CMotionPoint* curmp = &m_curmp;
//
//	ChaMatrix smat, rmat, tmat;
//	GetSRTMatrix2(curmp->GetWorldMat(), &smat, &rmat, &tmat);
//
//	if (parbone) {
//		//再帰から呼び出し
//		//ChaMatrix befparmat = parmp->GetBefWorldMat();
//		CMotionPoint* befparmp = parbone->GetMotionPoint(srcmotid, (double)((int)(srcframe + 0.1)));
//		if (befparmp) {
//			ChaMatrix befparmat = befparmp->GetWorldMat();
//			ChaMatrix newparmat = parbone->GetCurMp().GetWorldMat();
//			//if ((g_underRetargetFlag == true) || (IsSameMat(befparmat, newparmat) == 0)){
//			ChaMatrix invbefpar;
//			ChaMatrixInverse(&invbefpar, NULL, &befparmat);
//			ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * newparmat;
//
//			//g_wmatDirectSetFlag = true;
//			//SetWorldMat(infooutflag, 0, srcmotid, srcframe, tmpmat);
//			//g_wmatDirectSetFlag = false;
//			curmp->SetWorldMat(tmpmat);
//		}
//		//}
//	//else{
//		//parmatに変化がないときは変更しない。
//	//	curmp->SetBefWorldMat( curmp->GetWorldMat() );
//	//}
//		//if (bvhbone) {
//		//	if (m_child) {
//		//		bvhbone->SetTmpMat(curmp->GetWorldMat());
//		//	}
//		//	else {
//		//		bvhbone->SetTmpMat(newparmat);
//		//	}
//		//}
//	}
//	else {
//		//初回呼び出し
//		ChaMatrix tramat;
//		ChaMatrixIdentity(&tramat);
//		ChaMatrixTranslation(&tramat, traanim.x, traanim.y, traanim.z);
//
//		if (m_child) {
//			if (setmatflag == 0) {
//				ChaVector3 rotcenter;// = m_childworld;
//				ChaVector3TransformCoord(&rotcenter, &(GetJointFPos()), &(curmp->GetWorldMat()));
//				ChaMatrix befrot, aftrot;
//				ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//				ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//				ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
//
//				ChaMatrix tmpmat0 = curmp->GetWorldMat() * rotmat;// *tramat;
//				ChaVector3 tmppos;
//				ChaVector3TransformCoord(&tmppos, &(GetJointFPos()), &tmpmat0);
//				ChaVector3 diffvec;
//				diffvec = rotcenter - tmppos;
//				ChaMatrix tmptramat;
//				ChaMatrixIdentity(&tmptramat);
//				ChaMatrixTranslation(&tmptramat, diffvec.x, diffvec.y, diffvec.z);
//				ChaMatrix tmpmat;
//				tmpmat = tmpmat0 * tmptramat * tramat;
//
//				////directflagまたはunderRetargetFlagがないときはtramat成分は無視され、SetWorldMatFromEul中でbone::CalcLocalTraAnimの値が適用される。
//				//SetWorldMat(infooutflag, 0, srcmotid, srcframe, tmpmat);
//				curmp->SetWorldMat(tmpmat);
//
//				
//				//if (bvhbone) {
//				//	bvhbone->SetTmpMat(tmpmat);
//				//}
//			}
//			else {
//				ChaMatrix tmpmat = *psetmat;
//				//g_wmatDirectSetFlag = true;
//				//SetWorldMat(infooutflag, 0, srcmotid, srcframe, tmpmat);
//				//g_wmatDirectSetFlag = false;
//				curmp->SetWorldMat(tmpmat);
//
//				//if (bvhbone) {
//				//	bvhbone->SetTmpMat(tmpmat);
//				//}
//			}
//		}
//		else {
//			ChaVector3 rotcenter;// = m_childworld;
//			ChaVector3TransformCoord(&rotcenter, &(GetJointFPos()), &(curmp->GetWorldMat()));
//			ChaMatrix befrot, aftrot;
//			ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
//			ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
//			ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
//			//ChaMatrix tmpmat = curmp->GetWorldMat() * rotmat * tramat;
//			//ChaMatrix tmpmat = GetS0RTMatrix(curmp->GetWorldMat()) * rotmat * tramat;
//
//			ChaMatrix tmpmat0 = curmp->GetWorldMat() * rotmat;// *tramat;
//			ChaVector3 tmppos;
//			ChaVector3TransformCoord(&tmppos, &(GetJointFPos()), &tmpmat0);
//			ChaVector3 diffvec;
//			diffvec = rotcenter - tmppos;
//			ChaMatrix tmptramat;
//			ChaMatrixIdentity(&tmptramat);
//			ChaMatrixTranslation(&tmptramat, diffvec.x, diffvec.y, diffvec.z);
//			ChaMatrix tmpmat;
//			tmpmat = tmpmat0 * tmptramat * tramat;
//
//
//			//g_wmatDirectSetFlag = true;//!!!!!!!!
//			//SetWorldMat(infooutflag, 0, srcmotid, srcframe, tmpmat);
//			//g_wmatDirectSetFlag = false;//!!!!!!!
//
//			curmp->SetWorldMat(tmpmat);
//
//			//if (bvhbone) {
//			//	bvhbone->SetTmpMat(tmpmat);
//			//}
//
//
//		}
//	}
//
//
//	curmp->SetAbsMat(curmp->GetWorldMat());
//
//	if (m_child && curmp) {
//		m_child->RotBoneQCurrentReq(infooutflag, this, srcmotid, srcframe, rotq);
//	}
//	if (m_brother && parbone) {
//		m_brother->RotBoneQCurrentReq(infooutflag, parbone, srcmotid, srcframe, rotq);
//	}
//	return curmp;
//}





CMotionPoint* CBone::RotBoneQOne(bool limitdegflag, CBone* srcparentbone, CMotionPoint* parmp, int srcmotid, double srcframe, ChaMatrix srcmat)
{
	if (!srcparentbone) {
		_ASSERT(0);
		return 0;
	}


	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	int existflag = 0;
	//CMotionPoint* curmp = AddMotionPoint(srcmotid, srcframe, &existflag);
	//if (!existflag || !curmp){
	CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 0;
	}

	bool infooutflag = false;

	if (parmp){
		//parentの行列をセット !!!!!!!!!
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		SetWorldMat(limitdegflag, directsetflag, infooutflag, 0, srcmotid, roundingframe, 
			srcparentbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, parmp),
			onlycheck, fromiktarget);
	} else{
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		SetWorldMat(limitdegflag, directsetflag, infooutflag, 0, 
			srcmotid, roundingframe, srcmat,
			onlycheck, fromiktarget);
	}

	curmp->SetAbsMat(GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));

	return curmp;
}


CMotionPoint* CBone::SetAbsMatReq(bool limitdegflag, int broflag, 
	int srcmotid, double srcframe, double firstframe )
{
	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	int existflag = 0;
	//CMotionPoint* curmp = AddMotionPoint( srcmotid, srcframe, &existflag );
	//if( !existflag || !curmp ){
	CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT( 0 );
		return 0;
	}

	int existflag2 = 0;
	//CMotionPoint* firstmp = AddMotionPoint( srcmotid, firstframe, &existflag2 );
	//if( !existflag2 || !firstmp ){
	CMotionPoint* firstmp = GetMotionPoint(srcmotid, roundingframe);
	if (!firstmp) {
		_ASSERT( 0 );
		return 0;
	}

	bool infooutflag = false;
	bool directsetflag = true;
	int onlycheck = 0;
	bool fromiktarget = false;
	SetWorldMat(limitdegflag, directsetflag, infooutflag, 0, 
		srcmotid, roundingframe, firstmp->GetAbsMat(),
		onlycheck, fromiktarget);

	if(GetChild(false)){
		GetChild(false)->SetAbsMatReq(limitdegflag, 1, srcmotid, roundingframe, firstframe);
	}
	if(GetBrother(false) && broflag){
		GetBrother(false)->SetAbsMatReq(limitdegflag, 1, srcmotid, roundingframe, firstframe);
	}
	return curmp;
}



int CBone::DestroyMotionKey( int srcmotid )
{
	if ((srcmotid <= 0) || (srcmotid > m_motionkey.size())) {
		_ASSERT(0);
		return 1;
	}

	CMotionPoint* curmp = m_motionkey[srcmotid - 1];
	while( curmp ){
		CMotionPoint* nextmp = curmp->GetNext();
		
		//delete curmp;
		CMotionPoint::InvalidateMotionPoint(curmp);


		curmp = nextmp;
	}

	m_motionkey[srcmotid - 1] = NULL;



	std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
	itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
	if (itrvecmpmap != m_indexedmotionpoint.end()) {
		(itrvecmpmap->second).clear();
	}

	m_initindexedmotionpoint[srcmotid] = false;


	return 0;
}


int CBone::AddBoneMarkIfNot( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double curframe, int flag )
{
	if ((motid <= 0) || (motid > m_motionkey.size())) {
		_ASSERT(0);
		return 1;
	}

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	map<double, int> curmark;
	map<int, map<double, int>>::iterator itrcur;
	itrcur = m_motmark.find( motid - 1 );//2021/08/26
	if( itrcur == m_motmark.end() ){
		curmark.clear();
	}else{
		curmark = itrcur->second;
	}
	map<double, int>::iterator itrmark;
	itrmark = curmark.find( curframe );
	if( itrmark == curmark.end() ){
		curmark[ curframe ] = flag;
		m_motmark[motid - 1] = curmark;
	}

	return 0;
}

int CBone::DelBoneMarkRange( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double startframe, double endframe )
{
	if ((motid <= 0) || (motid > m_motionkey.size())) {
		_ASSERT(0);
		return 1;
	}

	map<int, map<double, int>>::iterator itrcur;
	itrcur = m_motmark.find( motid - 1 );//2021/08/26
	if( itrcur == m_motmark.end() ){
		return 0;
	}

	map<double, int> curmark;
	curmark = itrcur->second;

	double frame;
	for( frame = (startframe + 1.0); frame <= (endframe - 1.0); frame += 1.0 ){
		map<double, int>::iterator itrfind;
		itrfind = curmark.find( frame );
		if( itrfind != curmark.end() ){
			curmark.erase( itrfind );
		}
	}

	m_motmark[motid - 1] = curmark;

	return 0;
}

int CBone::AddBoneMotMark( int motid, OWP_Timeline* owpTimeline, int curlineno, double startframe, double endframe, int flag )
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	if( startframe != endframe ){
		AddBoneMarkIfNot( motid, owpTimeline, curlineno, startframe, flag );
		AddBoneMarkIfNot( motid, owpTimeline, curlineno, endframe, flag );
		DelBoneMarkRange( motid, owpTimeline, curlineno, startframe, endframe );
	}else{
		AddBoneMarkIfNot( motid, owpTimeline, curlineno, startframe, flag );
	}

	return 0;
}

int CBone::CalcLocalInfo(bool limitdegflag, int motid, double frameno, CMotionPoint* pdstmp )
{
	if (!pdstmp) {
		_ASSERT(0);
		return 1;
	}

	double roundingframe = RoundingTime(frameno);

	//2023/04/28
	if (IsNotSkeleton()) {
		CMotionPoint inimp;
		*pdstmp = inimp;
		return 0;
	}


	CQuaternion eulq;
	CMotionPoint* curmp = 0;
	ChaMatrix localmat;
	curmp = GetMotionPoint(motid, roundingframe);
	localmat.SetIdentity();

	if (curmp) {
		ChaMatrix curwm;
		curwm = GetWorldMat(limitdegflag, motid, roundingframe, curmp);

		if (GetParent(false)) {
			ChaMatrix parentwm, eulmat;
			//parentがeNullの場合はある
			if (GetParent(false)->IsSkeleton() || 
				GetParent(false)->IsNull() || GetParent(false)->IsCamera()) {
				parentwm = GetParent(false)->GetWorldMat(limitdegflag, motid, roundingframe, 0);
				eulq = ChaMatrix2Q(ChaMatrixInv(parentwm)) * ChaMatrix2Q(curwm);
				localmat = curwm * ChaMatrixInv(parentwm);
			}
			else {
				eulq = ChaMatrix2Q(curwm);
				localmat = curwm;
			}
		}
		else {
			eulq = ChaMatrix2Q(curwm);
			localmat = curwm;
		}
	}
	else {
		//_ASSERT(0);
		eulq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		localmat.SetIdentity();
	}

	ChaVector3 svec = ChaVector3(1.0f, 1.0f, 1.0f);
	ChaMatrix rmat;
	rmat.SetIdentity();
	ChaVector3 tvec = ChaVector3(0.0f ,0.0f, 0.0f);
	GetSRTMatrix(localmat, &svec, &rmat, &tvec);

	CMotionPoint setmp;
	setmp.CalcQandTra(eulq.MakeRotMatX(), this);
	setmp.SetLocalScale(svec);//2024/01/31
	*pdstmp = setmp;



	//CMotionPoint* pcurmp = 0;
	////CMotionPoint* pparmp = 0;
	//pcurmp = GetMotionPoint(motid, roundingframe);//current mp
	//if(GetParent(false)){
	//	if( pcurmp ){
	//		CMotionPoint setmp;
	//		ChaMatrix invpar = ChaMatrixInv(GetParent(false)->GetWorldMat(limitdegflag, motid, roundingframe, 0));
	//		ChaMatrix localmat = GetWorldMat(limitdegflag, motid, roundingframe, pcurmp) * invpar;
	//		//pcurmp->CalcQandTra(localmat, this);
	//		setmp.CalcQandTra(localmat, this);

	//		int inirotcur, inirotpar;
	//		inirotcur = IsInitRot(GetWorldMat(limitdegflag, motid, roundingframe, pcurmp));
	//		inirotpar = IsInitRot(GetParent(false)->GetWorldMat(limitdegflag, motid, roundingframe, 0));
	//		if (inirotcur && inirotpar){
	//			CQuaternion iniq;
	//			iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//			//pcurmp->SetQ(iniq);
	//			setmp.SetQ(iniq);
	//		}

	//		*pdstmp = setmp;
	//	}else{
	//		CMotionPoint inimp;
	//		*pdstmp = inimp;

	//		//_ASSERT( 0 );
	//		return 0;
	//	}
	//}else{
	//	if( pcurmp ){
	//		CMotionPoint setmp;
	//		ChaMatrix localmat = GetWorldMat(limitdegflag, motid, roundingframe, pcurmp);
	//		setmp.CalcQandTra( localmat, this );

	//		int inirotcur;
	//		inirotcur = IsInitRot(GetWorldMat(limitdegflag, motid, roundingframe, pcurmp));
	//		if (inirotcur ){
	//			CQuaternion iniq;
	//			iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//			setmp.SetQ(iniq);
	//		}

	//		*pdstmp = setmp;

	//	}else{
	//		CMotionPoint inimp;
	//		*pdstmp = inimp;

	//		//_ASSERT( 0 );
	//		return 0;
	//	}
	//}

	return 0;
}

//int CBone::CalcCurrentLocalInfo(CMotionPoint* pdstmp)
//{
//	if (!pdstmp) {
//		_ASSERT(0);
//		return 1;
//	}
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		CMotionPoint inimp;
//		*pdstmp = inimp;
//		return 0;
//	}
//
//
//	CMotionPoint curmp;
//	CMotionPoint parmp;
//	
//	curmp = GetCurMp();
//
//	//2023/02/02
//	//m_curmp, GetCurMp()のworldmatには　例外的にモデルのworldmatが掛かっている
//	//アニメーションの計算には　GetWorldMatではなく GetAnimMatを使う
//
//	if (GetParent(false)) {
//		if (GetParent(false)->IsSkeleton()) {
//			parmp = GetParent(false)->GetCurMp();
//			CMotionPoint setmp;
//			ChaMatrix invpar = ChaMatrixInv(parmp.GetAnimMat());
//			ChaMatrix localmat = curmp.GetAnimMat() * invpar;
//			setmp.CalcQandTra(localmat, this);
//
//			int inirotcur, inirotpar;
//			inirotcur = IsInitRot(curmp.GetAnimMat());
//			inirotpar = IsInitRot(parmp.GetAnimMat());
//			if (inirotcur && inirotpar) {
//				CQuaternion iniq;
//				iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//				//pcurmp->SetQ(iniq);
//				setmp.SetQ(iniq);
//			}
//
//			setmp.SetFrame(m_curmp.GetFrame());
//			*pdstmp = setmp;
//		}
//		else if (GetParent(false)->IsNull()) {
//			CMotionPoint setmp;
//			ChaMatrix invpar = ChaMatrixInv(GetParent(false)->GetENullMatrix());//!!!!!!!!
//			ChaMatrix localmat = curmp.GetAnimMat() * invpar; //<---------- InvENullはNodeMatに掛けるのでは？
//			setmp.CalcQandTra(localmat, this);
//
//			int inirotcur, inirotpar;
//			inirotcur = IsInitRot(curmp.GetAnimMat());
//			inirotpar = IsInitRot(GetParent(false)->GetENullMatrix());//!!!!!!!!
//			if (inirotcur && inirotpar) {
//				CQuaternion iniq;
//				iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//				//pcurmp->SetQ(iniq);
//				setmp.SetQ(iniq);
//			}
//
//			setmp.SetFrame(m_curmp.GetFrame());
//			*pdstmp = setmp;
//		}
//		else {
//			CMotionPoint setmp;
//			ChaMatrix localmat = curmp.GetAnimMat();
//			setmp.CalcQandTra(localmat, this);
//
//			setmp.SetFrame(m_curmp.GetFrame());
//			*pdstmp = setmp;
//		}
//	}
//	else {
//		CMotionPoint setmp;
//		ChaMatrix localmat = curmp.GetAnimMat();//GetParent() == NULLのときのローカル
//		setmp.CalcQandTra(localmat, this);
//
//		int inirotcur;
//		inirotcur = IsInitRot(localmat);
//		if (inirotcur) {
//			CQuaternion iniq;
//			iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//			setmp.SetQ(iniq);
//		}
//
//		setmp.SetFrame(m_curmp.GetFrame());
//		*pdstmp = setmp;
//	}
//
//	return 0;
//}

int CBone::CalcBtLocalInfo(CMotionPoint* pdstmp)
{
	if (!pdstmp) {
		_ASSERT(0);
		return 1;
	}

	//2023/04/28
	if (IsNotSkeleton()) {
		CMotionPoint inimp;
		*pdstmp = inimp;
		return 0;
	}

	if (GetParent(false) && GetParent(false)->IsSkeleton()) {
		CMotionPoint setmp;
		ChaMatrix parentbtmat;
		ChaMatrix currentbtmat;
		ChaMatrix localbtmat;
		parentbtmat = GetParent(false)->GetBtMat();
		currentbtmat = GetBtMat();
		localbtmat = currentbtmat * ChaMatrixInv(parentbtmat);

		setmp.CalcQandTra(localbtmat, this);

		int inirotcur, inirotpar;
		inirotcur = IsInitRot(currentbtmat);
		inirotpar = IsInitRot(parentbtmat);
		if (inirotcur && inirotpar) {
			CQuaternion iniq;
			iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			//pcurmp->SetQ(iniq);
			setmp.SetQ(iniq);
		}

		*pdstmp = setmp;
	}
	else {
		CMotionPoint setmp;
		ChaMatrix localbtmat;
		localbtmat = GetBtMat();

		setmp.CalcQandTra(localbtmat, this);

		int inirotcur;
		inirotcur = IsInitRot(localbtmat);
		if (inirotcur) {
			CQuaternion iniq;
			iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			setmp.SetQ(iniq);
		}

		*pdstmp = setmp;

	}

	return 0;
}



//int CBone::CalcInitLocalInfo(int motid, double frameno, CMotionPoint* pdstmp)
//{
//	double roundingframe = (double)((int)(frameno + 0.0001));
//
//	CMotionPoint* pcurmp = 0;
//	CMotionPoint* pparmp = 0;
//	pcurmp = GetMotionPoint(motid, roundingframe);
//	if (GetParent()){
//		if (pcurmp){
//			pparmp = GetParent()->GetMotionPoint(motid, roundingframe);
//			if (pparmp){
//				ChaMatrix invpar = ChaMatrixInv(GetParent()->GetWorldMat(motid, roundingframe, pparmp));
//				ChaMatrix invinitmat = GetInvInitMat();
//				ChaMatrix localmat = invinitmat * GetWorldMat(motid, roundingframe, pcurmp) * invpar;//world == init * local * parだからlocalを計算するには、invinit * world * invpar。
//				pcurmp->CalcQandTra(localmat, this);
//			}
//			else{
//				_ASSERT(0);
//				return 0;
//			}
//		}
//		else{
//			_ASSERT(0);
//			return 0;
//		}
//	}
//	else{
//		if (pcurmp){
//			ChaMatrix invinitmat = GetInvInitMat();
//			ChaMatrix localmat = invinitmat * GetWorldMat(motid, roundingframe, pcurmp);
//			pcurmp->CalcQandTra(localmat, this);
//		}
//		else{
//			_ASSERT(0);
//			return 0;
//		}
//	}
//
//	*pdstmp = *pcurmp;
//
//	return 0;
//}


int CBone::GetBoneNum()
{
	int retnum = 0;

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}

	if(!GetChild(false)){
		return 0;
	}else{
		retnum++;
	}

	CBone* cbro = GetChild(false)->GetBrother(false);
	while (cbro) {
		retnum++;
		cbro = cbro->GetBrother(false);
	}

	return retnum;
}

int CBone::CalcFirstFrameBonePos(ChaMatrix srcmat)
{
	ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);
	ChaVector3TransformCoord(&m_firstframebonepos, &zeropos, &srcmat);


	//ChaVector3 jpos = GetJointFPos();
	//ChaVector3TransformCoord(&m_firstframebonepos, &jpos, &srcmat);
	
	
	//ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);//2022/07/29
	//ChaVector3TransformCoord(&m_firstframebonepos, &zeropos, &(GetFirstMat()));


	//if ((m_firstframebonepos.x == 0.0f) && (m_firstframebonepos.y == 0.0f) && (m_firstframebonepos.z == 0.0f)){
	//	_ASSERT(0);
	//}
	return 0;
}

/*
void CBone::CalcFirstAxisMatX()
{
	ChaVector3 curpos;
	ChaVector3 childpos;

	if (m_parent){
		CalcAxisMatX_aft(m_parent->GetJointFPos(), GetJointFPos(), &m_firstaxismatX);
	}

}
*/
//void CBone::CalcFirstAxisMatZ()
//{
//	ChaVector3 curpos;
//	ChaVector3 childpos;
//
//	if (m_parent){
//		CalcAxisMatZ_aft(m_parent->GetJointFPos(), GetJointFPos(), &m_firstaxismatZ);
//	}
//
//}

int CBone::CalcBoneDepth()
{
	int retdepth = 0;
	CBone* curbone = this;
	if (curbone){
		while (curbone->GetParent(false)){
			retdepth++;
			curbone = curbone->GetParent(false);
		}
	}
	return retdepth;
}

ChaVector3 CBone::GetJointFPos()
{
	if (m_parmodel) {
		if (m_parmodel->GetOldAxisFlagAtLoading() == 0) {
			return m_jointfpos;
		}
		else {
			return m_oldjointfpos;
		}
	}
	else {
		return ChaVector3(0.0f, 0.0f, 0.0f);
	}

}
void CBone::SetJointFPos(ChaVector3 srcpos)
{ 
	m_jointfpos = srcpos; 
}
void CBone::SetOldJointFPos(ChaVector3 srcpos){
	m_oldjointfpos = srcpos;
}

BEFEUL CBone::GetBefEul(bool limitdegflag, int srcmotid, double srcframe)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetBefEul(this, limitdegflag, srcmotid, srcframe);
}

//ChaVector3 CBone::GetUnlimitedBefEul(int srcmotid, double srcframe)
//{
//	double roundingframe = RoundingTime(srcframe);
//
//	ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//
//	if (IsNotSkeleton()) {
//		return befeul;
//	}
//
//
//	//1つ前のフレームのEULはすでに計算されていると仮定する。
//	double befframe;
//	befframe = roundingframe - 1.0;
//	if (roundingframe <= 1.01) {
//		//befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//		CMotionPoint* curmp;
//		curmp = GetMotionPoint(srcmotid, roundingframe);
//		if (curmp) {
//			befeul = curmp->GetLocalEul();//unlimited !!!
//		}
//	}
//	else {
//		CMotionPoint* befmp;
//		befmp = GetMotionPoint(srcmotid, befframe);
//		if (befmp) {
//			befeul = befmp->GetLocalEul();//unlimited !!!
//		}
//	}
//
//	//if (g_underIKRot == true) {
//	//	if (roundingframe <= 1.01) {
//	//		befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//	//	}
//	//}
//
//	return befeul;
//}

int CBone::GetNotModify180Flag(int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);

	//2023/02/04
	//ModifyEuler360()の内容を変えたので　全フレームmodifyする
	//int notmodify180flag = 0;

	if (IsNotSkeleton() && IsNotCamera()) {
		return 1;
	}


	//2023/01/14
	//rootjointを２回転する場合など　180度補正は必要(１フレームにつき165度までの変化しか出来ない制限は必要)
	//しかし　bvh2fbxなど　１フレームにアニメが付いているデータでうまくいくようにするために　0フレームと１フレームは除外
	int notmodify180flag = 1;
	//if (g_underIKRot == false) {
	if (GetParModel() && (GetParModel()->GetUnderIKRot() == false)) {
		if (roundingframe <= 1.01) {
			//0フレームと１フレームは　180度ずれチェックをしない
			notmodify180flag = 1;
		}
		else {
			notmodify180flag = 0;
		}
	}
	else {
		//2023/01/26
		//IKRot中は　０フレームも１フレームも　180度チェックをする
		notmodify180flag = 0;
	}




	////2023/02/03
	////CalcLocalEulXYZに渡すbefeulを変更した
	////カレントフレームが０フレームと１フレームのときには
	////befeulをカレントフレームのオイラー角として modifyeul180チェックをすることにした
	//int notmodify180flag = 0;

	//2023/02/03_2
	//１フレームでX:-180, Z:-180で後ろを向いているモーションがある
	//そのようなモーションで　180度を取り消してしまうと　体が反対を向く
	//よって　notmodify180flagは1にしてみる
	//notmodify180flagを1にすると
	//LimitEulオン時に　１フレームだけオイラー角が360度違うことがある副作用があるが　対策は後で
	//int notmodify180flag = 1;


	return notmodify180flag;
}


ChaVector3 CBone::CalcLocalEulXYZ(bool limitdegflag, int axiskind,
	int srcmotid, double srcframe, tag_befeulkind befeulkind, ChaVector3* directbefeul)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.CalcLocalEulXYZ(this, limitdegflag, axiskind, srcmotid, srcframe, befeulkind, directbefeul);
}

//ChaVector3 CBone::CalcLocalLimitedEulXYZ(int srcmotid, double srcframe)
//{
//	//スレッド呼び出しのUpdateMatrixの後処理用
//	// 
//	//制限角度の制限を受ける姿勢のオイラー角を計算
//	//計算済のlimitedwmからオイラー角を計算
//
//	double roundingframe = RoundingTime(srcframe);
//
//	ChaVector3 reteul = ChaVector3(0.0f, 0.0f, 0.0f);
//	//reteul = GetLimitedLocalEul(srcmotid, srcframe);
//	//reteul = CalcLocalEulAndSetLimitedEul(srcmotid, roundingframe);
//
//	CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
//	if (curmp) {
//		ChaMatrix limitedlocal, curlimitedwm;
//		curlimitedwm = curmp->GetLimitedWM();
//		if (GetParent()) {
//			CMotionPoint* parentmp = GetParent()->GetMotionPoint(srcmotid, roundingframe);
//			if (parentmp) {
//				ChaMatrix parentlimitedwm;
//				parentlimitedwm = parentmp->GetLimitedWM();
//				limitedlocal = curlimitedwm * ChaMatrixInv(parentlimitedwm);
//			}
//			else {
//				limitedlocal = curlimitedwm;
//			}
//		}
//		else {
//			limitedlocal = curlimitedwm;
//		}
//
//		CQuaternion eulq;
//		eulq.RotationMatrix(limitedlocal);
//		CQuaternion axisq;
//		axisq.RotationMatrix(GetNodeMat());
//
//		ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//		double befframe;
//		befframe = roundingframe - 1.0;
//		if (befframe >= -0.0001) {
//			CMotionPoint* befmp;
//			befmp = GetMotionPoint(srcmotid, roundingframe);
//			if (befmp) {
//				befeul = befmp->GetLimitedLocalEul();
//			}
//		}
//
//		int isfirstbone = 0;
//		int isendbone = 0;
//		int notmodify180flag = 1;
//		if (g_underIKRot == false) {
//			if (roundingframe <= 1.01) {
//				//0フレームと１フレームは　180度ずれチェックをしない
//				notmodify180flag = 1;
//			}
//			else {
//				notmodify180flag = 0;
//			}
//		}
//		else {
//			//2023/01/26
//			//IKRot中は　０フレームも１フレームも　180度チェックをする
//			notmodify180flag = 0;
//			if (roundingframe <= 1.01) {
//				befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//			}
//		}
//		eulq.Q2EulXYZusingQ(&axisq, befeul, &reteul, isfirstbone, isendbone, notmodify180flag);
//
//		return reteul;
//	}
//	else {
//		return reteul;
//	}
//
//}


//ChaVector3 CBone::CalcLocalUnlimitedEulXYZ(int srcmotid, double srcframe)
//{
//	//制限角度の制限を受けない姿勢のオイラー角を計算
//	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
//	double roundingframe = RoundingTime(srcframe);
//
//	CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
//	if (curmp) {
//		cureul = curmp->GetLocalEul();//unlimited !!!
//	}
//	else {
//		_ASSERT(0);
//		return cureul;
//	}
//
//
//	//CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
//	//if (curmp) {
//	//	ChaMatrix localmat;
//	//	ChaMatrix curwm, parentwm;
//	//	curwm = curmp->GetWorldMat();//unlimited !!!
//	//	if (GetParent()) {
//	//		CMotionPoint* parentmp = GetParent()->GetMotionPoint(srcmotid, roundingframe);
//	//		if (parentmp) {
//	//			parentwm = parentmp->GetWorldMat();//unlimited !!!
//	//			localmat = curwm * ChaMatrixInv(parentwm);
//	//		}
//	//		else {
//	//			localmat = curwm;
//	//		}
//	//	}
//	//	else {
//	//		localmat = curwm;
//	//	}
//	//	CQuaternion eulq;
//	//	eulq.RotationMatrix(localmat);
//
//	//	CQuaternion axisq;
//	//	axisq.RotationMatrix(GetNodeMat());
//
//	//	//1つ前のフレームのEULはすでに計算されていると仮定する。
//	//	ChaVector3 befeul = GetUnlimitedBefEul(srcmotid, roundingframe);
//	//	int notmodify180flag = GetNotModify180Flag(srcmotid, roundingframe);
//
//	//	int isfirstbone = 0;
//	//	int isendbone = 0;
//	//	eulq.Q2EulXYZusingQ(&axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//	//	//eulq.Q2EulXYZusingMat(ROTORDER_XYZ, &axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//
//	//	return cureul;
//
//	//}
//	//else {
//	//	_ASSERT(0);
//	//	return cureul;
//	//}
//
//
//	//CMotionPoint* curmp;
//	//curmp = GetMotionPoint(srcmotid, roundingframe);
//	//if (curmp) {
//	//	ChaVector3 oldeul = curmp->GetLocalEul();
//	//	if (IsSameEul(oldeul, cureul) == 0) {
//	//		return cureul;
//	//	}
//	//	else {
//	//		return oldeul;
//	//	}
//	//}
//	//else {
//	//	return cureul;
//	//}
//}


ChaMatrix CBone::CalcLocalRotMatFromEul(ChaVector3 srceul, int srcmotid, double srcframe)
{
	CQuaternion noderot;
	//CQuaternion invnoderot;
	noderot.RotationMatrix(GetNodeMat());
	//noderot.inv(&invnoderot);
	//CQuaternion parentnoderot;
	//CQuaternion invparentnoderot;
	//if (GetParent()) {
	//	parentnoderot.RotationMatrix(GetParent()->GetNodeMat());
	//	parentnoderot.inv(&invparentnoderot);
	//}
	//else {
	//	parentnoderot.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//	invparentnoderot.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//}


	//const WCHAR* bonename = GetWBoneName();
	//bool ishipsjoint = false;
	//if ((wcscmp(bonename, L"Hips_Joint") == 0) || (wcscmp(bonename, L"hips_Joint") == 0) ||
	//	(wcscmp(bonename, L"Hips") == 0) || (wcscmp(bonename, L"hips") == 0)) {
	//	ishipsjoint = true;
	//}
	//else {
	//	ishipsjoint = false;
	//}

	CQuaternion newrot;
	//newrot0.SetRotationXYZ(0, srceul);//GetNodeMat() * (curwm * ChaMatrixInv(parentwm)) * ChaMatrixInv(GetParent()->GetNodeMat()) の　GLOBAL軸オイラー角
	////########### CalcLocalEulXYZ()におけるeulmatは式１################################################################
	////eulmat = GetNodeMat() * (curwm * ChaMatrixInv(parentwm)) * ChaMatrixInv(GetParent()->GetNodeMat());//式１　//CalcLocalEulXYZ()
	//// 式２で　curwm * invparentwm　の回転に修正する
	////#################################################################################################################
	////newrotmat = invnoderot.MakeRotMatX() * newrot.MakeRotMatX() * parentnoderot.MakeRotMatX();//式２　//curwm * invparentwmの回転
	//CQuaternion newrot;
	//newrot = parentnoderot * newrot0 * invnoderot;


	//(curwm * ChaMatrixInv(parentwm)　の　NodeMat軸のオイラー角 2022/12/20 こっちの方がMayaに近い
	//その後　NodeMatに０フレームアニメを含めないように計算して　Mayaのオイラー角と一致
	newrot.SetRotationXYZ(&noderot, srceul);


	ChaMatrix retlocalrotmat;
	retlocalrotmat = newrot.MakeRotMatX();

	return retlocalrotmat;

}


//ChaVector3 CBone::CalcCurrentLocalEulXYZ(int axiskind, tag_befeulkind befeulkind, ChaVector3* directbefeul)
//{
//	//axiskind : BONEAXIS_*  or  -1(CBone::m_anglelimit.boneaxiskind)
//
//	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
//	ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//
//	const WCHAR* bonename = GetWBoneName();
//	if (wcscmp(bonename, L"RootNode") == 0) {
//		return cureul;//!!!!!!!!!!!!!!!!!!!!!!!!
//	}
//
//	int curmotid = 0;
//	double curframe = 0;
//	if (m_parmodel) {
//		MOTINFO* curmi = m_parmodel->GetCurMotInfo();
//		if (curmi) {
//			curmotid = curmi->motid;
//			curframe = RoundingTime(curmi->curframe);
//		}
//	}
//	else {
//		_ASSERT(0);
//		return cureul;
//	}
//
//
//	if (befeulkind == BEFEUL_BEFFRAME) {
//		//1つ前のフレームのEULはすでに計算されていると仮定する。
//		double befframe;
//		befframe = curframe - 1.0;
//		if (befframe >= -0.0001) {
//			CMotionPoint* befmp;
//			//befmp = GetCurMp();
//			//befeul = befmp.GetLocalEul();
//			befmp = GetMotionPoint(curmotid, befframe);
//			if (befmp) {
//				if (g_limitdegflag == 0) {
//					befeul = befmp->GetLocalEul();
//				}
//				else {
//					befeul = befmp->GetLimitedLocalEul();
//				}
//			}
//		}
//	}
//	else if ((befeulkind == BEFEUL_DIRECT) && directbefeul) {
//		befeul = *directbefeul;
//	}
//
//	int isfirstbone = 0;
//	int isendbone = 0;
//
//	if (GetParent()) {
//		isfirstbone = 0;
//	}
//	else {
//		isfirstbone = 1;
//	}
//
//	if (GetChild()) {
//		if (GetChild()->GetChild()) {
//			isendbone = 0;
//		}
//		else {
//			isendbone = 1;
//		}
//	}
//	else {
//		isendbone = 1;
//	}
//
//	//2023/01/14
//	//rootjointを２回転する場合など　180度補正は必要(１フレームにつき165度までの変化しか出来ない制限は必要)
//	//しかし　bvh2fbxなど　１フレームにアニメが付いているデータでうまくいくようにするために　0フレームと１フレームは除外
//	int notmodify180flag = 1;
//	if (g_underIKRot == false) {
//		if (curframe <= 1.01) {
//			//0フレームと１フレームは　180度ずれチェックをしない
//			notmodify180flag = 1;
//		}
//		else {
//			notmodify180flag = 0;
//		}
//	}
//	else {
//		//2023/01/26
//		//IKRot中は　０フレームも１フレームも　180度チェックをする
//		notmodify180flag = 0;
//		if (curframe <= 1.01) {
//			befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//		}
//	}
//
//	CQuaternion eulq;
//	if (GetParent()) {
//		isfirstbone = 0;
//
//		ChaMatrix curwm, parentwm, eulmat;
//		curwm = GetCurMp().GetWorldMat();
//		parentwm = GetParent()->GetCurMp().GetWorldMat();
//		//eulq = ChaMatrix2Q(ChaMatrixInv(GetParent()->GetNodeMat())) * ChaMatrix2Q(ChaMatrixInv(parentwm)) * ChaMatrix2Q(curwm) * ChaMatrix2Q(GetNodeMat());//2022/12/14 mesh付きのfbxでOK
//		eulq = ChaMatrix2Q(ChaMatrixInv(parentwm)) * ChaMatrix2Q(curwm);
//	}
//	else {
//		isfirstbone = 1;
//
//		ChaMatrix curwm, eulmat;
//		curwm = GetCurMp().GetWorldMat();
//		//eulq = ChaMatrix2Q(curwm) * ChaMatrix2Q(GetNodeMat());
//		eulq = ChaMatrix2Q(curwm);
//	}
//	//eulq.Q2EulXYZusingMat(ROTORDER_XYZ, 0, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//	//eulq.Q2EulXYZusingQ(0, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//
//	CQuaternion axisq;
//	axisq.RotationMatrix(GetNodeMat());
//	eulq.Q2EulXYZusingQ(&axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//	//eulq.Q2EulXYZusingMat(ROTORDER_XYZ, &axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//
//
//	CMotionPoint* curmp;
//	curmp = GetMotionPoint(curmotid, curframe);
//	if (curmp) {
//		ChaVector3 oldeul = curmp->GetLocalEul();
//		if (IsSameEul(oldeul, cureul) == 0) {
//			return cureul;
//		}
//		else {
//			return oldeul;
//		}
//	}
//	else {
//		return cureul;
//	}
//
//
//	return cureul;
//}

//ChaMatrix CBone::CalcCurrentLocalRotMatFromEul(ChaVector3 srceul)
//{
//	CQuaternion noderot;
//	CQuaternion invnoderot;
//	noderot.RotationMatrix(GetNodeMat());
//	noderot.inv(&invnoderot);
//	CQuaternion parentnoderot;
//	CQuaternion invparentnoderot;
//	if (GetParent()) {
//		parentnoderot.RotationMatrix(GetParent()->GetNodeMat());
//		parentnoderot.inv(&invparentnoderot);
//	}
//	else {
//		parentnoderot.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		invparentnoderot.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//	}
//
//
//	const WCHAR* bonename = GetWBoneName();
//	bool ishipsjoint = false;
//	if ((wcscmp(bonename, L"Hips_Joint") == 0) || (wcscmp(bonename, L"hips_Joint") == 0) ||
//		(wcscmp(bonename, L"Hips") == 0) || (wcscmp(bonename, L"hips") == 0)) {
//		ishipsjoint = true;
//	}
//	else {
//		ishipsjoint = false;
//	}
//
//
//	//CQuaternion newrot0;
//	//newrot0.SetRotationXYZ(0, srceul);//GetNodeMat() * (curwm * ChaMatrixInv(parentwm)) * ChaMatrixInv(GetParent()->GetNodeMat()) の　GLOBAL軸オイラー角
//	////########### CalcLocalEulXYZ()におけるeulmatは式１################################################################
//	////eulmat = GetNodeMat() * (curwm * ChaMatrixInv(parentwm)) * ChaMatrixInv(GetParent()->GetNodeMat());//式１　//CalcLocalEulXYZ()
//	//// 式２で　curwm * invparentwm　の回転に修正する
//	////#################################################################################################################
//	////newrotmat = invnoderot.MakeRotMatX() * newrot.MakeRotMatX() * parentnoderot.MakeRotMatX();//式２　//curwm * invparentwmの回転
//	//CQuaternion newrot;
//	//newrot = parentnoderot * newrot0 * invnoderot;
//
//
//	//2022/12/26 CalcLocalRotMatFromEulと合わせる
//	CQuaternion newrot;
//	newrot.SetRotationXYZ(&noderot, srceul);
//
//
//	ChaMatrix retlocalrotmat;
//	retlocalrotmat = newrot.MakeRotMatX();
//
//	return retlocalrotmat;
//
//}


//ChaVector3 CBone::CalcBtLocalEulXYZ(int axiskind, tag_befeulkind befeulkind, ChaVector3* directbefeul)
//{
//	//axiskind : BONEAXIS_*  or  -1(CBone::m_anglelimit.boneaxiskind)
//
//	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
//	ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//
//	const WCHAR* bonename = GetWBoneName();
//	if (wcscmp(bonename, L"RootNode") == 0) {
//		return cureul;//!!!!!!!!!!!!!!!!!!!!!!!!
//	}
//
//	//if (befeulkind == BEFEUL_BEFFRAME) {
//	//	//1つ前のフレームのEULはすでに計算されていると仮定する。
//	//	double befframe;
//	//	befframe = srcframe - 1.0;
//	//	if (befframe >= -0.0001) {
//	//		CMotionPoint* befmp;
//	//		//befmp = GetCurMp();
//	//		//befeul = befmp.GetLocalEul();
//	//		befmp = GetMotionPoint(srcmotid, befframe);
//	//		if (befmp) {
//	//			befeul = befmp->GetLocalEul();
//	//		}
//	//	}
//	//}
//	//else if ((befeulkind == BEFEUL_DIRECT) && directbefeul) {
//	//	befeul = *directbefeul;
//	//}
//
//	CMotionPoint tmpmp;
//	//CalcLocalInfo(srcmotid, srcframe, &tmpmp);//local!!!
//	CalcBtLocalInfo(&tmpmp);//local!!!
//
//	ChaMatrix axismat;
//	CQuaternion axisq;
//	//int multworld = 0;//local!!!
//	//axismat = CalcManipulatorMatrix(1, 0, multworld, srcmotid, srcframe);
//	//axisq.RotationMatrix(axismat);
//
//	int isfirstbone = 0;
//	int isendbone = 0;
//
//	if (GetParent()) {
//		CRigidElem* curre = GetParent()->GetRigidElem(this);
//		if (curre) {
//			axismat = curre->GetBindcapsulemat();
//		}
//		else {
//			//_ASSERT(0);
//			ChaMatrixIdentity(&axismat);
//		}
//		//axismat = GetParent()->GetCurMp().GetWorldMat();
//
//		axisq.RotationMatrix(axismat);
//
//		isfirstbone = 0;
//	}
//	else {
//		ChaMatrixIdentity(&axismat);
//		axisq.SetParams(1.0, 0.0, 0.0, 0.0);
//
//		isfirstbone = 1;
//	}
//
//	if (GetChild()) {
//		if (GetChild()->GetChild()) {
//			isendbone = 0;
//		}
//		else {
//			isendbone = 1;
//		}
//	}
//	else {
//		isendbone = 1;
//	}
//
//	//int notmodifyflag;
//	//if ((srcframe == 0.0) || (srcframe == 1.0)) {
//	//	notmodifyflag = 1;
//	//}
//	//else {
//	//	notmodifyflag = 0;
//	//}
//
//
//	int notmodify180flag = 1;//!!!! 165度以上のIK編集のために　180度チェックはしない
//
//
//	if (axiskind == -1) {
//		if (m_anglelimit.boneaxiskind != BONEAXIS_GLOBAL) {
//			tmpmp.GetQ().Q2EulXYZ(&axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//		}
//		else {
//			tmpmp.GetQ().Q2EulXYZ(0, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//		}
//	}
//	else if (axiskind != BONEAXIS_GLOBAL) {
//		tmpmp.GetQ().Q2EulXYZ(&axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//	}
//	else {
//		tmpmp.GetQ().Q2EulXYZ(0, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
//	}
//
//	//CMotionPoint* curmp;
//	//curmp = GetMotionPoint(srcmotid, srcframe);
//	//if (curmp) {
//	//	ChaVector3 oldeul = curmp->GetLocalEul();
//	//	if (IsSameEul(oldeul, cureul) == 0) {
//	//		return cureul;
//	//	}
//	//	else {
//	//		return oldeul;
//	//	}
//	//}
//	//else {
//	//	return cureul;
//	//}
//
//	return cureul;
//}



int CBone::SetWorldMatFromEul(bool limitdegflag, int inittraflag, int setchildflag, ChaMatrix befwm, ChaVector3 srceul, int srcmotid, double srcframe, int initscaleflag)//initscaleflag = 1 : default
{
	//anglelimitをした後のオイラー角が渡される。anglelimitはCBone::SetWorldMatで処理する。


	//2022/11/23 CommentOut なぜこのif文があったのか？ 不具合によりエンドジョイントにモーションポイントが無かったから？
	//if (!m_child){
	//	return 0;
	//}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		//_ASSERT(0);
		return 1;
	}

	//ChaMatrix befwm = curmp->GetBefWorldMat();

	ChaMatrix newworldmat = CalcWorldMatFromEul(limitdegflag, inittraflag, setchildflag, srceul, srcmotid, roundingframe, initscaleflag);


	//CMotionPoint* curmp;
	//curmp = GetMotionPoint(srcmotid, roundingframe);
	if (curmp){
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		SetWorldMat(limitdegflag, srcmotid, roundingframe, newworldmat, curmp);
		SetLocalEul(limitdegflag, srcmotid, roundingframe, srceul, curmp);
		if (limitdegflag == true) {
			curmp->SetCalcLimitedWM(2);
		}

		if (setchildflag == 1){
			if (GetChild(false)){
				bool setbroflag = true;
				GetChild(false)->UpdateParentWMReq(limitdegflag, setbroflag,
					srcmotid, roundingframe, befwm, newworldmat);
			}
		}
	}
	else{
		_ASSERT(0);
	}

	return 0;
}

//int CBone::SetBtWorldMatFromEul(int setchildflag, ChaVector3 srceul)//initscaleflag = 1 : default
//{
//	//anglelimitをした後のオイラー角が渡される。anglelimitはCBone::SetWorldMatで処理する。
//	if (!m_child) {
//		return 0;
//	}
//
//	ChaMatrix axismat;
//	CQuaternion axisq;
//	//int multworld = 0;//local!!!
//	//axismat = CalcManipulatorMatrix(1, 0, multworld, srcmotid, srcframe);
//	//axisq.RotationMatrix(axismat);
//	if (GetParent()) {
//		CRigidElem* curre = GetParent()->GetRigidElem(this);
//		if (curre) {
//			axismat = curre->GetBindcapsulemat();
//		}
//		else {
//			//_ASSERT(0);
//			ChaMatrixIdentity(&axismat);
//		}
//		axisq.RotationMatrix(axismat);
//	}
//	else {
//		ChaMatrixIdentity(&axismat);
//		axisq.SetParams(1.0, 0.0, 0.0, 0.0);
//	}
//
//
//	CQuaternion newrot;
//	if (m_anglelimit.boneaxiskind != BONEAXIS_GLOBAL) {
//		newrot.SetRotationXYZ(&axisq, srceul);
//	}
//	else {
//		newrot.SetRotationXYZ(0, srceul);
//	}
//
//
//	//QuaternionInOrder(srcmotid, srcframe, &newrot);
//
//
//	ChaMatrix curlocalmat;
//	if (GetParent()) {
//		curlocalmat = GetBtMat() * ChaMatrixInv(GetParent()->GetBtMat());
//	}
//	else {
//		curlocalmat = GetBtMat();
//	}
//	
//	ChaVector3 curtrapos;
//	ChaVector3 tmpfpos = GetJointFPos();
//	ChaVector3TransformCoord(&curtrapos, &tmpfpos, &curlocalmat);
//
//	ChaMatrix cursmat, currmat, curtmat;
//	GetSRTMatrix2(curlocalmat, &cursmat, &currmat, &curtmat);
//
//
//	ChaMatrix newlocalmat, newrotmat, befrotmat, aftrotmat;
//	newrotmat = newrot.MakeRotMatX();
//	ChaMatrixTranslation(&befrotmat, -tmpfpos.x, -tmpfpos.y, -tmpfpos.z);
//	ChaMatrixTranslation(&aftrotmat, tmpfpos.x, tmpfpos.y, tmpfpos.z);
//	//newlocalmat = befrotmat * newrotmat * aftrotmat;
//
//	//if (initscaleflag == 0) {
//	//	newlocalmat = befrotmat * cursmat * newrotmat * aftrotmat;
//	//}
//	//else {
//		newlocalmat = befrotmat * newrotmat * aftrotmat;
//	//}
//	//if (inittraflag == 0) {
//	//	//ChaVector3 tmppos;
//	//	//ChaVector3TransformCoord(&tmppos, &(GetJointFPos()), &newlocalmat);
//	//	//ChaVector3 diffvec;
//	//	//diffvec = curtrapos - tmppos;
//	//	//ChaMatrix tmptramat;
//	//	//ChaMatrixIdentity(&tmptramat);
//	//	//ChaMatrixTranslation(&tmptramat, diffvec.x, diffvec.y, diffvec.z);
//
//	//	ChaMatrix tmptramat;
//	//	ChaMatrixIdentity(&tmptramat);
//	//	ChaMatrixTranslation(&tmptramat, orgtraanim.x, orgtraanim.y, orgtraanim.z);
//
//	//	newlocalmat = newlocalmat * tmptramat;
//	//}
//
//	//if (inittraflag == 0){
//	//	ChaVector3 traanim = CalcLocalTraAnim(srcmotid, srcframe);
//	//	ChaMatrix tramat;
//	//	ChaMatrixIdentity(&tramat);
//	//	ChaMatrixTranslation(&tramat, traanim.x, traanim.y, traanim.z);
//	//	newlocalmat = newlocalmat * tramat;
//	//}
//
//	ChaMatrix newmat;
//	if (m_parent) {
//		newmat = newlocalmat * GetParent()->GetBtMat();
//	}
//	else {
//		newmat = newlocalmat;
//	}
//
//	////curmp->SetBefWorldMat(curmp->GetWorldMat());
//	//curmp->SetWorldMat(newmat);
//	//curmp->SetLocalEul(srceul);
//
//	//if (setchildflag == 1) {
//	//	if (m_child) {
//	//		bool infooutflag = false;
//	//		CQuaternion dummyq;
//	//		m_child->RotBoneQReq(infooutflag, this, srcmotid, srcframe, dummyq);
//	//	}
//	//}
//
//	return 0;
//}

//int CBone::CalcWorldMatAfterThread(int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat)
//{
//	//############################################################
//	//g_limitdegflag == trueのときに並列化計算後の処理として呼ばれる
//	//############################################################
//
//	//ChaVector3 orgeul, neweul;
//	//GetTempLocalEul(&orgeul, &neweul);
//	//ChaMatrix newworldmat = CalcWorldMatFromEul(0, 1, neweul, orgeul, srcmotid, (double)((int)(srcframe + 0.1)), 0);
//
//	ChaMatrix limworldmat;
//	ChaMatrixIdentity(&limworldmat);
//
//	int callingstate = 2;//!!!!!!!!!!!!!!!!!!!
//
//	CMotionPoint* pbefmp = 0;
//
//	GetCalclatedLimitedWM(srcmotid, srcframe, &limworldmat, &pbefmp, callingstate);//srcframeのbefaftによる補間もしている
//
//	ChaMatrix tmpmat = limworldmat * *wmat;
//	m_curmp.SetWorldMat(tmpmat);//2021/12/21
//
//	//CMotionPoint* pmp = GetMotionPoint(srcmotid, (double)((int)(srcframe + 0.1)));
//	//if (pmp) {
//	//	pmp->SetLimitedWM(newworldmat);
//	//	pmp->SetCalcLimitedWM(1);
//	//}
// 
//	ChaVector3 jpos = GetJointFPos();
//	ChaMatrix tmpwm = m_curmp.GetWorldMat();
//	ChaVector3TransformCoord(&m_childworld, &jpos, &tmpwm);
//	//ChaVector3TransformCoord(&m_childworld, &jpos, &newworldmat);
//
//	ChaMatrix wvpmat = m_curmp.GetWorldMat() * *vpmat;
//	//ChaMatrix wvpmat = newworldmat * *vpmat;
//
//	ChaVector3TransformCoord(&m_childscreen, &m_childworld, vpmat);
//	//ChaVector3TransformCoord(&m_childscreen, &m_childworld, &wvpmat);
//
//	return 0;
//}


ChaMatrix CBone::CalcWorldMatFromEul(bool limitdegflag, int inittraflag, int setchildflag, ChaVector3 srceul, int srcmotid, double srcframe, int initscaleflag)//initscaleflag = 1 : default
{
	double roundingframe = RoundingTime(srcframe);

	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	//2023/04/28
	if (IsNotSkeleton()) {
		return retmat;
	}


	//anglelimitをした後のオイラー角が渡される。anglelimitはCBone::SetWorldMatで処理する。
	//if (!m_child) {
	//	return retmat;
	//}

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		//_ASSERT(0);
		return retmat;
	}


	ChaMatrix curwm;
	curwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);
	//if (g_limitdegflag == false) {
	//	curwm = curmp->GetWorldMat();
	//}
	//else {
	//	curwm = curmp->GetLimitedWM();
	//}

	ChaMatrix oldlocalmat;
	oldlocalmat.SetIdentity();
	if (GetParent(false)) {
		ChaMatrix parmat;
		parmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		oldlocalmat = curwm * ChaMatrixInv(parmat);
	}
	else {
		oldlocalmat = curwm;
	}
	ChaMatrix cursmat, currmat, curtmat, curtanimmat;
	GetSRTandTraAnim(oldlocalmat, GetNodeMat(), &cursmat, &currmat, &curtmat, &curtanimmat);


	ChaMatrix newlocalrotmat = CalcLocalRotMatFromEul(srceul, srcmotid, roundingframe);
	ChaMatrix newlocalmat;
	bool sflag = (initscaleflag == 0);
	bool tanimflag = (inittraflag == 0);
	newlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, GetNodeMat(), &cursmat, &newlocalrotmat, &curtanimmat);


	ChaMatrix newmat;
	if (GetParent(false)) {
		ChaMatrix limitedparmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		newmat = newlocalmat * limitedparmat;
	}
	else {
		newmat = newlocalmat;
	}

	retmat = newmat;

	return retmat;
}



int CBone::SetWorldMatFromEulAndScaleAndTra(bool limitdegflag, int inittraflag, int setchildflag, 
	ChaMatrix befwm, ChaVector3 srceul, ChaVector3 srcscale, ChaVector3 srctra, int srcmotid, double srcframe)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.SetWorldMatFromEulAndScaleAndTra(this, limitdegflag, inittraflag, setchildflag,
		befwm, srceul, srcscale, srctra, srcmotid, srcframe);
}




int CBone::SetWorldMatFromQAndTra(bool limitdegflag, int setchildflag, 
	ChaMatrix befwm, CQuaternion axisq, CQuaternion srcq, ChaVector3 srctra, int srcmotid, double srcframe)
{
	if (!GetChild(false)){
		return 0;
	}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CQuaternion invaxisq;
	axisq.inv(&invaxisq);
	CQuaternion newrot = invaxisq * srcq * axisq;
	ChaMatrix newlocalrotmat;
	newlocalrotmat = newrot.MakeRotMatX();
	ChaMatrix newtramat;
	ChaMatrixIdentity(&newtramat);
	ChaMatrixTranslation(&newtramat, srctra.x, srctra.y, srctra.z);

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 1;
	}

	//ChaMatrix befwm = curmp->GetBefWorldMat();
	ChaMatrix curwm;
	curwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);

	ChaMatrix parmat;
	ChaMatrix parnodemat;
	ChaMatrixIdentity(&parmat);
	ChaMatrixIdentity(&parnodemat);
	if (GetParent(false)) {
		parmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		parnodemat = GetParent(false)->GetNodeMat();
	}

	ChaMatrix cursmat, currmat, curtmat, curtanimmat;
	GetSRTandTraAnim((curwm * ChaMatrixInv(parmat)), GetNodeMat(), &cursmat, &currmat, &curtmat, &curtanimmat);

	ChaMatrix newlocalmat;
	bool sflag = false;
	bool tanimflag = true;
	newlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, GetNodeMat(), 0, &newlocalrotmat, &newtramat);

	ChaMatrix newmat;
	if (GetParent(false)) {
		ChaMatrix limitedparmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		newmat = newlocalmat * limitedparmat;
	}
	else {
		newmat = newlocalmat;
	}


	if (curmp){
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		SetWorldMat(limitdegflag, srcmotid, roundingframe, newmat, curmp);
		//ChaVector3 neweul = CalcLocalEulXYZ(-1, srcmotid, roundingframe, BEFEUL_ZERO);
		ChaVector3 neweul = CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
		SetLocalEul(limitdegflag, srcmotid, roundingframe, neweul, curmp);
		if (limitdegflag == true) {
			curmp->SetCalcLimitedWM(2);
		}


		if (setchildflag == 1) {
			if (GetChild(false)) {
				bool setbroflag = true;
				GetChild(false)->UpdateParentWMReq(limitdegflag, setbroflag,
					srcmotid, roundingframe, befwm, newmat);
			}
		}
	}
	else{
		_ASSERT(0);
	}

	return 0;
}


int CBone::SetWorldMatFromQAndScaleAndTra(bool limitdegflag, int setchildflag,
	ChaMatrix befwm, CQuaternion axisq, 
	CQuaternion srcq, ChaVector3 srcscale, ChaVector3 srctra, int srcmotid, double srcframe)
{
	if (!GetChild(false)) {
		return 0;
	}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CQuaternion invaxisq;
	axisq.inv(&invaxisq);
	CQuaternion newrot = invaxisq * srcq * axisq;
	ChaMatrix newlocalrotmat;
	newlocalrotmat = newrot.MakeRotMatX();
	ChaMatrix newtramat;
	ChaMatrixIdentity(&newtramat);
	ChaMatrixTranslation(&newtramat, srctra.x, srctra.y, srctra.z);
	ChaMatrix newscmat;
	newscmat.SetIdentity();
	newscmat.SetScale(srcscale);

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 1;
	}

	//ChaMatrix befwm = curmp->GetBefWorldMat();
	ChaMatrix curwm;
	curwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);

	ChaMatrix parmat;
	ChaMatrix parnodemat;
	ChaMatrixIdentity(&parmat);
	ChaMatrixIdentity(&parnodemat);
	if (GetParent(false)) {
		parmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		parnodemat = GetParent(false)->GetNodeMat();
	}

	ChaMatrix cursmat, currmat, curtmat, curtanimmat;
	GetSRTandTraAnim((curwm * ChaMatrixInv(parmat)), GetNodeMat(), &cursmat, &currmat, &curtmat, &curtanimmat);

	ChaMatrix newlocalmat;
	bool sflag = true;//!!!!!!
	bool tanimflag = true;
	newlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, GetNodeMat(), &newscmat, &newlocalrotmat, &newtramat);

	ChaMatrix newmat;
	if (GetParent(false)) {
		ChaMatrix limitedparmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		newmat = newlocalmat * limitedparmat;
	}
	else {
		newmat = newlocalmat;
	}


	if (curmp) {
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		SetWorldMat(limitdegflag, srcmotid, roundingframe, newmat, curmp);
		//ChaVector3 neweul = CalcLocalEulXYZ(-1, srcmotid, roundingframe, BEFEUL_ZERO);
		ChaVector3 neweul = CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
		SetLocalEul(limitdegflag, srcmotid, roundingframe, neweul, curmp);
		if (limitdegflag == true) {
			curmp->SetCalcLimitedWM(2);
		}


		if (setchildflag == 1) {
			if (GetChild(false)) {
				bool setbroflag = true;
				GetChild(false)->UpdateParentWMReq(limitdegflag, setbroflag,
					srcmotid, roundingframe, befwm, newmat);
			}
		}
	}
	else {
		_ASSERT(0);
	}

	return 0;
}



int CBone::SetWorldMatFromEulAndTra(bool limitdegflag, int setchildflag, 
	ChaMatrix befwm, ChaVector3 srceul, ChaVector3 srctra, int srcmotid, double srcframe)
{
	//anglelimitをした後のオイラー角が渡される。anglelimitはCBone::SetWorldMatで処理する。
	


	//2022/11/23 CommentOut なぜこのif文があったのか？ 不具合によりエンドジョイントにモーションポイントが無かったから？
	//if (!m_child){
	//	return 0;
	//}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	ChaMatrix newtramat;
	ChaMatrixIdentity(&newtramat);
	ChaMatrixTranslation(&newtramat, srctra.x, srctra.y, srctra.z);//TraAnimをそのまま

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 1;
	}

	//ChaMatrix befwm = curmp->GetBefWorldMat();
	ChaMatrix curwm;
	curwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);


	ChaMatrix parmat;
	ChaMatrix parnodemat;
	ChaMatrixIdentity(&parmat);
	ChaMatrixIdentity(&parnodemat);
	if (GetParent(false)) {
		parmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		parnodemat = GetParent(false)->GetNodeMat();
	}

	ChaMatrix newlocalrotmat = CalcLocalRotMatFromEul(srceul, srcmotid, roundingframe);

	ChaMatrix cursmat, currmat, curtmat, curtanimmat;
	GetSRTandTraAnim((curwm * ChaMatrixInv(parmat)), GetNodeMat(), &cursmat, &currmat, &curtmat, &curtanimmat);

	ChaMatrix newlocalmat;
	bool sflag = false;
	bool tanimflag = true;
	newlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, GetNodeMat(), 0, &newlocalrotmat, &newtramat);


	ChaMatrix newmat;
	if (GetParent(false)) {
		ChaMatrix limitedparmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		newmat = newlocalmat * limitedparmat;
	}
	else {
		newmat = newlocalmat;
	}

	//CMotionPoint* curmp;
	//curmp = GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		SetWorldMat(limitdegflag, srcmotid, roundingframe, newmat, curmp);
		SetLocalEul(limitdegflag, srcmotid, roundingframe, srceul, curmp);
		if (limitdegflag == true) {
			curmp->SetCalcLimitedWM(2);
		}


		if (setchildflag == 1) {
			if (GetChild(false)) {
				bool setbroflag = true;
				GetChild(false)->UpdateParentWMReq(limitdegflag, setbroflag,
					srcmotid, roundingframe, befwm, newmat);
			}
		}
	}
	else {
		_ASSERT(0);
	}

	return 0;
}


int CBone::SetLocalEul(bool limitdegflag, int srcmotid, double srcframe, ChaVector3 srceul, CMotionPoint* srcmp)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.SetLocalEul(this, limitdegflag, srcmotid, srcframe, srceul, srcmp);
}

ChaVector3 CBone::GetLocalEul(bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* srcmp)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetLocalEul(this, limitdegflag, srcmotid, srcframe, srcmp);
}

int CBone::SetWorldMat(bool limitdegflag, bool directsetflag, 
	bool infooutflag, int setchildflag, 
	int srcmotid, double srcframe, ChaMatrix srcmat, int onlycheck, bool fromiktarget)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.SetWorldMat(this, limitdegflag, directsetflag,
		infooutflag, setchildflag,
		srcmotid, srcframe, srcmat, onlycheck, fromiktarget);
}

int CBone::ChkMovableEul(ChaVector3 srceul)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ChkMovableEul(this, srceul);
}


//float CBone::LimitAngle(enum tag_axiskind srckind, float srcval)
//{
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		return 0.0f;
//	}
//
//
//
//	SetAngleLimitOff(&m_anglelimit);
//	if (m_anglelimit.limitoff[srckind] == 1){
//		return srcval;
//	}
//	else{
//		float newval = srcval;
//
//		float cmpvalupper, cmpvallower;
//		cmpvalupper = srcval;
//		cmpvallower = srcval;
//
//		if (abs(m_anglelimit.upper[srckind] - m_anglelimit.lower[srckind]) > EULLIMITPLAY) {
//			//リミット付近でもIKが動くためには遊びの部分が必要
//
//			if (cmpvalupper > m_anglelimit.upper[srckind]) {
//				newval = min(cmpvalupper, (float)(m_anglelimit.upper[srckind] - EULLIMITPLAY));
//			}
//			if (cmpvallower < m_anglelimit.lower[srckind]) {
//				newval = max(cmpvallower, (float)(m_anglelimit.lower[srckind] + EULLIMITPLAY));
//			}
//		}
//		else {
//			//lowerとupperの間がEULLIMITPLAYより小さいとき
//			
//			if (cmpvalupper > m_anglelimit.upper[srckind]) {
//				newval = (float)(m_anglelimit.upper[srckind]);
//			}
//			if (cmpvallower < m_anglelimit.lower[srckind]) {
//				newval = (float)(m_anglelimit.lower[srckind]);
//			}
//		}
//
//		return newval;
//	}
//}


ChaVector3 CBone::LimitEul(ChaVector3 srceul)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.LimitEul(this, srceul);

}

ANGLELIMIT CBone::GetAngleLimit(bool limitdegflag, int getchkflag)
{
	SetAngleLimitOff();

	//2023/04/28
	if (IsNotSkeleton()) {
		m_anglelimit.chkeul[AXIS_X] = 0.0f;
		m_anglelimit.chkeul[AXIS_Y] = 0.0f;
		m_anglelimit.chkeul[AXIS_Z] = 0.0f;
		return m_anglelimit;
	}


	if (getchkflag == 1) {

		//int existflag = 0;
		//CalcFBXMotion(curmotid, curframe, &m_curmp, &existflag);

		//ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
		//ChaVector3 neweul = ChaVector3(0.0f, 0.0f, 0.0f);
		////cureul = CalcLocalEulXYZ(m_anglelimit.boneaxiskind, curmotid, curframe, BEFEUL_BEFFRAME);
		//cureul = CalcCurrentLocalEulXYZ(m_anglelimit.boneaxiskind, curmotid, curframe, BEFEUL_BEFFRAME);

		if (m_parmodel) {
			MOTINFO* curmi = m_parmodel->GetCurMotInfo();
			if (curmi) {
				int curmotid = curmi->motid;
				int curframe = IntTime(curmi->curframe);

				ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
				ChaVector3 neweul = ChaVector3(0.0f, 0.0f, 0.0f);
				if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
					////cureul = CalcCurrentLocalEulXYZ(-1, BEFEUL_BEFFRAME);
					//cureul = CalcLocalEulXYZ(-1, m_curmotid, curframe, BEFEUL_BEFFRAME, 0);
					//if (g_limitdegflag) {
					//	neweul = LimitEul(cureul);
					//}
					//else {
					//	neweul = cureul;
					//}
					
					neweul = GetLocalEul(limitdegflag, m_curmotid, curframe, 0);

				}
				else {
					neweul = GetBtEul();
				}
				m_anglelimit.chkeul[AXIS_X] = neweul.x;
				m_anglelimit.chkeul[AXIS_Y] = neweul.y;
				m_anglelimit.chkeul[AXIS_Z] = neweul.z;
			}
			else {
				_ASSERT(0);
			}
		}
		else {
			_ASSERT(0);
		}


		//m_anglelimit.chkeul[AXIS_X] = neweul.x;
		//m_anglelimit.chkeul[AXIS_Y] = neweul.y;
		//m_anglelimit.chkeul[AXIS_Z] = neweul.z;
	}

	return m_anglelimit;
};

int CBone::SwapAngleLimitUpperLowerIfRev()
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	int axiskind;
	for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
		if (m_anglelimit.lower[axiskind] > m_anglelimit.upper[axiskind]) {
			//_ASSERT(0);
			//swap
			int tmpval = m_anglelimit.lower[axiskind];
			m_anglelimit.lower[axiskind] = m_anglelimit.upper[axiskind];
			m_anglelimit.upper[axiskind] = tmpval;
		}
	}

	return 0;
}

void CBone::SetAngleLimit(bool limitdegflag, ANGLELIMIT srclimit)
{
	m_anglelimit = srclimit;

	//2023/04/28
	if (IsNotSkeleton()) {
		return;
	}


	//int axiskind;
	//for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++){
		//if (m_anglelimit.lower[axiskind] < -180){
		//	m_anglelimit.lower[axiskind] = -180;
		//}
		//else if (m_anglelimit.lower[axiskind] > 180){
		//	m_anglelimit.lower[axiskind] = 180;
		//}

		//if (m_anglelimit.upper[axiskind] < -180){
		//	m_anglelimit.upper[axiskind] = -180;
		//}
		//else if (m_anglelimit.upper[axiskind] > 180){
		//	m_anglelimit.upper[axiskind] = 180;
		//}
	//}

	SwapAngleLimitUpperLowerIfRev();
	SetAngleLimitOff();

	ChaVector3 neweul;
	neweul.x = m_anglelimit.chkeul[AXIS_X];
	neweul.y = m_anglelimit.chkeul[AXIS_Y];
	neweul.z = m_anglelimit.chkeul[AXIS_Z];

	ChaVector3 limiteul = neweul;
	//if (g_limitdegflag) {
	//	limiteul = LimitEul(neweul);
	//}
	//else {
	//	limiteul = neweul;
	//}

	if (GetParModel()) {
		MOTINFO* curmi = GetParModel()->GetCurMotInfo();
		if (curmi) {
			int curmotid;
			double curframe;
			curmotid = curmi->motid;
			curframe = curmi->curframe;

			limiteul = GetLocalEul(limitdegflag, curmotid, curframe, 0);
		}
	}

	m_anglelimit.chkeul[AXIS_X] = limiteul.x;
	m_anglelimit.chkeul[AXIS_Y] = limiteul.y;
	m_anglelimit.chkeul[AXIS_Z] = limiteul.z;

	//chkチェックスライダーは操作用ではなくリファレンス用にする。元データを上書きしないため。
	//if ((srcmotid >= 1) && (srcframe >= 0.0)) {
	//	//int inittraflag = 0;
	//	//int setchildflag = 1;
	//	//int initscaleflag = 0;
	//	//SetWorldMatFromEul(inittraflag, setchildflag, neweul, srcmotid, srcframe, initscaleflag);
	//	ChaMatrix newworldmat;
	//	ChaMatrixIdentity(&newworldmat);
	//	//if (g_limitdegflag == true) {
	//	//制限角度有り
	//	ChaVector3 orgeul = CalcLocalEulXYZ(-1, srcmotid, (double)((int)(srcframe + 0.1)), BEFEUL_BEFFRAME);
	//	ChaVector3 neweul = LimitEul(orgeul);
	//	SetLocalEul(srcmotid, (double)((int)(srcframe + 0.1)), neweul);//!!!!!!!!!!!!
	//	newworldmat = CalcWorldMatFromEul(0, 1, neweul, orgeul, srcmotid, (double)((int)(srcframe + 0.1)), 0);
	//	//}
	//	//else {
	//	//	//制限角度無し
	//	//	CallF(CalcFBXMotion(srcmotid, srcframe, &m_curmp, &existflag), return 1);
	//	//	newworldmat = m_curmp.GetWorldMat();// **wmat;
	//	//}
	//	//ChaMatrix tmpmat = newworldmat * *wmat;
	//	//m_curmp.SetWorldMat(newworldmat);//underchecking
	//	m_curmp.SetWorldMat(newworldmat);
	//}

};


int CBone::GetFreeCustomRigNo()
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return -1;
	}


	int rigno;
	int findrigno = -1;
	for (rigno = 0; rigno < MAXRIGNUM; rigno++){
		if (m_customrig[rigno].useflag == 0){//0 : free mark
			findrigno = rigno;
			break;
		}
	}
	
	if (findrigno == -1){
		//freeがなかった場合、rentalから
		for (rigno = 0; rigno < MAXRIGNUM; rigno++){
			if (m_customrig[rigno].useflag == 1){//1 : rental mark
				findrigno = rigno;
				break;
			}
		}
	}

	return findrigno;
}
CUSTOMRIG CBone::GetFreeCustomRig()
{
	int freerigno = GetFreeCustomRigNo();
	if ((freerigno >= 0) && (freerigno < MAXRIGNUM)){
		m_customrig[freerigno].useflag = 1;//1 : rental mark
		//_ASSERT(0);
		return m_customrig[freerigno];
	}
	else{
		_ASSERT(0);
		CUSTOMRIG dummycr;
		::InitCustomRig(&dummycr, 0, 0);
		return dummycr;
	}
}

int CBone::InvalidateCustomRig(int rigno)
{
	if ((rigno >= 0) && (rigno < MAXRIGNUM)) {
		::InitCustomRig(&m_customrig[rigno], this, rigno);
		m_customrig[rigno].useflag = 0;
	}

	return 0;
}


CUSTOMRIG CBone::GetCustomRig(int rigno)
{
	if ((rigno >= 0) && (rigno < MAXRIGNUM)){
		return m_customrig[rigno];
	}
	else{
		_ASSERT(0);
		CUSTOMRIG dummycr;
		::InitCustomRig(&dummycr, 0, 0);
		return dummycr;
	}
}
void CBone::SetCustomRig(CUSTOMRIG srccr)
{
	int isvalid = IsValidCustomRig(m_parmodel, srccr, this);
	if (isvalid == 1){
		m_customrig[srccr.rigno] = srccr;
		m_customrig[srccr.rigno].useflag = 2;//2 : valid mark
	}
}

ChaMatrix CBone::CalcSymXMat(bool limitdegflag, int srcmotid, double srcframe)
{
	return CalcSymXMat2(limitdegflag, srcmotid, srcframe, SYMROOTBONE_SYMDIR | SYMROOTBONE_SYMPOS);
}

ChaMatrix CBone::CalcSymXMat2(bool limitdegflag, int srcmotid, double srcframe, int symrootmode)
{
	/*
	enum
	{
		//for bit mask operation
		SYMROOTBONE_SAMEORG = 0,
		SYMROOTBONE_SYMDIR = 1,
		SYMROOTBONE_SYMPOS = 2
	};
	*/

	double roundingframe = RoundingTime(srcframe);

	ChaMatrix directsetmat;
	ChaMatrixIdentity(&directsetmat);
	//ChaVector3 symscale = ChaVector3(1.0f, 1.0f, 1.0f);
	//symscale = CalcLocalScaleAnim(srcmotid, roundingframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return directsetmat;
	}



	int rotcenterflag1 = 1;
	if (GetParent(false) && GetParent(false)->IsSkeleton()){
		directsetmat = CalcLocalSymScaleRotMat(limitdegflag, rotcenterflag1, srcmotid, roundingframe);
	}
	else{
		//root bone
		if (symrootmode == SYMROOTBONE_SAMEORG){
			directsetmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			return directsetmat;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		else if(symrootmode & SYMROOTBONE_SYMDIR){
			directsetmat = CalcLocalSymScaleRotMat(limitdegflag, rotcenterflag1, srcmotid, roundingframe);
		}
		else{
			directsetmat = CalcLocalScaleRotMat(limitdegflag, rotcenterflag1, srcmotid, roundingframe);
		}
	}

////tra anim
	//ChaVector3 curanimtra = CalcLocalTraAnim(srcmotid, roundingframe);
	ChaVector3 curanimtra = CalcLocalSymTraAnim(limitdegflag, srcmotid, roundingframe);//traanimもsym対応

	if (GetParent(false) && GetParent(false)->IsSkeleton()) {
		//inv x signe
		directsetmat.AddTranslation(ChaVector3(-curanimtra.x, curanimtra.y, curanimtra.z));
	}
	else{
		//root bone
		if (symrootmode & SYMROOTBONE_SYMPOS){
			//inv x signe
			directsetmat.AddTranslation(ChaVector3(-curanimtra.x, curanimtra.y, curanimtra.z));
		}
		else{
			//same signe
			directsetmat.AddTranslation(curanimtra);
		}
	}


	//directsetmat._11 *= symscale.x;
	//directsetmat._12 *= symscale.x;
	//directsetmat._13 *= symscale.x;
	//directsetmat._21 *= symscale.y;
	//directsetmat._22 *= symscale.y;
	//directsetmat._23 *= symscale.y;
	//directsetmat._31 *= symscale.z;
	//directsetmat._32 *= symscale.z;
	//directsetmat._33 *= symscale.z;

	return directsetmat;
}


ChaMatrix CBone::GetWorldMat(bool limitdegflag, 
	int srcmotid, double srcframe, CMotionPoint* srcmp, ChaVector3* dsteul)//default : dsteul = 0
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetWorldMat(this, limitdegflag, srcmotid, srcframe, srcmp, dsteul);
}

ChaMatrix CBone::GetCurrentWorldMat(bool multmodelwm)
{
	//CMotionPoint curmp;
	//curmp = GetCurMp();
	//return curmp.GetWorldMat();

	bool currentlimitdegflag = g_limitdegflag;

	//2023/04/28
	if (IsNotSkeleton()) {
		ChaMatrix inimat;
		inimat.SetIdentity();
		return inimat;
	}

	MOTINFO* curmi = 0;
	if (GetParModel()) {
		curmi = GetParModel()->GetCurMotInfo();
		if (curmi) {
			int curmotid = curmi->motid;
			double curframe = curmi->curframe;

			ChaMatrix newworldmat;
			ChaMatrixIdentity(&newworldmat);
			//###################################
			//補間のためにroundingframeではない
			//###################################
			CMotionPoint tmpmp;
			int existflag = 0;
			CalcFBXMotion(currentlimitdegflag, curmotid, curframe, &tmpmp, &existflag);
			newworldmat = GetWorldMat(currentlimitdegflag, curmotid, curframe, &tmpmp);
			if (multmodelwm == true) {
				newworldmat = newworldmat * GetParModel()->GetWorldMat();
			}
			return newworldmat;
		}
		else {
			//_ASSERT(0);
			if (multmodelwm == true) {
				return GetParModel()->GetWorldMat();
			}
			else {
				ChaMatrix initmat;
				initmat.SetIdentity();
				return initmat;
			}
		}
	}
	else {
		_ASSERT(0);
		ChaMatrix initmat;
		initmat.SetIdentity();
		return initmat;
	}
}

int CBone::SetWorldMat(bool limitdegflag, 
	int srcmotid, double srcframe, ChaMatrix srcmat, CMotionPoint* srcmp)//default : srcmp = 0
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.SetWorldMat(this, limitdegflag,
		srcmotid, srcframe, srcmat, srcmp);
}

CMotionPoint* CBone::GetMotionPoint(int srcmotid, double srcframe, bool onaddmotion) 
{
	//存在するときだけ返す。
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetMotionPoint(this, srcmotid, srcframe, onaddmotion);
}



ChaMatrix CBone::CalcLocalScaleRotMat(bool limitdegflag, int rotcenterflag, int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		ChaMatrix inimat;
		inimat.SetIdentity();
		return inimat;
	}

	ChaMatrix curmat;
	curmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

	ChaMatrix parmat, invparmat;
	ChaMatrixIdentity(&parmat);
	ChaMatrixIdentity(&invparmat);
	if (GetParent(false)) {
		parmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		invparmat = ChaMatrixInv(parmat);
	}
	ChaMatrix localmat;
	localmat = curmat * invparmat;

	ChaMatrix localsmat, localrmat, localtmat;
	GetSRTMatrix2(localmat, &localsmat, &localrmat, &localtmat);

	//CMotionPoint curlocalmp;
	//CalcLocalInfo(srcmotid, roundingframe, &curlocalmp);
	ChaMatrix currotmat;
	//currotmat = curlocalmp.GetQ().MakeRotMatX();

	//currotmat._41 = 0.0f;
	//currotmat._42 = 0.0f;
	//currotmat._43 = 0.0f;

	if (rotcenterflag == 1){
		ChaMatrix befrotmat, aftrotmat;
		ChaMatrixIdentity(&befrotmat);
		ChaMatrixTranslation(&befrotmat, -GetJointFPos().x, -GetJointFPos().y, -GetJointFPos().z);
		ChaMatrixIdentity(&aftrotmat);
		ChaMatrixTranslation(&aftrotmat, GetJointFPos().x, GetJointFPos().y, GetJointFPos().z);
		currotmat = befrotmat * localsmat * localrmat * aftrotmat;// smat有り　！！！！
	}
	else {
		_ASSERT(0);
		currotmat = localsmat * localrmat;
	}

	return currotmat;
}


ChaMatrix CBone::CalcLocalSymScaleRotMat(bool limitdegflag, int rotcenterflag, int srcmotid, double srcframe)
{
	ChaMatrix retmat;
	retmat.SetIdentity();

	//2023/04/28
	if (IsNotSkeleton()) {
		return retmat;
	}

	int symboneno = 0;
	int existflag = 0;
	m_parmodel->GetSymBoneNo(GetBoneNo(), &symboneno, &existflag);
	if (symboneno >= 0){
		CBone* symbone = m_parmodel->GetBoneByID(symboneno);
		_ASSERT(symbone);
		if (symbone){
			//if (symbone == this){
			//	WCHAR dbgmes[1024];
			//	swprintf_s(dbgmes, 1024, L"CalcLocalSymRotMat : frame %lf : samebone : this[%s]--sym[%s]", srcframe, GetWBoneName(), symbone->GetWBoneName());
			//	::MessageBox(NULL, dbgmes, L"check", MB_OK);
			//}

			CMotionPoint symlocalmp;
			symbone->CalcLocalInfo(limitdegflag, srcmotid, srcframe, &symlocalmp);
			retmat = symlocalmp.GetQ().CalcSymX2();

			ChaVector3 symscale = ChaVector3(1.0f, 1.0f, 1.0f);
			symscale = symbone->CalcLocalScaleAnim(limitdegflag, srcmotid, srcframe);
			ChaMatrix symscalemat;
			ChaMatrixIdentity(&symscalemat);
			ChaMatrixScaling(&symscalemat, symscale.x, symscale.y, symscale.z);

			retmat.SetTranslationZero();

			if (rotcenterflag == 1){
				ChaMatrix befrotmat, aftrotmat;
				ChaMatrixIdentity(&befrotmat);
				ChaMatrixTranslation(&befrotmat, -GetJointFPos().x, -GetJointFPos().y, -GetJointFPos().z);
				ChaMatrixIdentity(&aftrotmat);
				ChaMatrixTranslation(&aftrotmat, GetJointFPos().x, GetJointFPos().y, GetJointFPos().z);
				retmat = befrotmat * symscalemat * retmat * aftrotmat;
			}
			else {
				retmat = symscalemat * retmat;
			}
		}
		else{
			retmat = CalcLocalScaleRotMat(limitdegflag, rotcenterflag, srcmotid, srcframe);

			//CalcLocalRotMatはrotcenter対応、scale有り!!!!!!!!!!!!!!
			

			//ChaVector3 symscale = ChaVector3(1.0f, 1.0f, 1.0f);
			//symscale = CalcLocalScaleAnim(srcmotid, srcframe);
			//ChaMatrix symscalemat;
			//ChaMatrixIdentity(&symscalemat);
			//ChaMatrixScaling(&symscalemat, symscale.x, symscale.y, symscale.z);

			//if (rotcenterflag == 1) {
			//	ChaMatrix befrotmat, aftrotmat;
			//	ChaMatrixIdentity(&befrotmat);
			//	ChaMatrixTranslation(&befrotmat, -GetJointFPos().x, -GetJointFPos().y, -GetJointFPos().z);
			//	ChaMatrixIdentity(&aftrotmat);
			//	ChaMatrixTranslation(&aftrotmat, GetJointFPos().x, GetJointFPos().y, GetJointFPos().z);
			//	retmat = befrotmat * symscalemat * retmat * aftrotmat;
			//}
			//else {
			//	retmat = symscalemat * retmat;
			//}

			_ASSERT(0);
		}
	}
	else{
		retmat = CalcLocalScaleRotMat(limitdegflag, rotcenterflag, srcmotid, srcframe);

		//CalcLocalRotMatはrotcenter対応、scale有り!!!!!!!!!!!!!!


		//ChaVector3 symscale = ChaVector3(1.0f, 1.0f, 1.0f);
		//symscale = CalcLocalScaleAnim(srcmotid, srcframe);
		//ChaMatrix symscalemat;
		//ChaMatrixIdentity(&symscalemat);
		//ChaMatrixScaling(&symscalemat, symscale.x, symscale.y, symscale.z);

		//if (rotcenterflag == 1) {
		//	ChaMatrix befrotmat, aftrotmat;
		//	ChaMatrixIdentity(&befrotmat);
		//	ChaMatrixTranslation(&befrotmat, -GetJointFPos().x, -GetJointFPos().y, -GetJointFPos().z);
		//	ChaMatrixIdentity(&aftrotmat);
		//	ChaMatrixTranslation(&aftrotmat, GetJointFPos().x, GetJointFPos().y, GetJointFPos().z);
		//	retmat = befrotmat * symscalemat * retmat * aftrotmat;
		//}
		//else {
		//	retmat = symscalemat * retmat;
		//}

		_ASSERT(0);
	}

	return retmat;
}

ChaVector3 CBone::CalcLocalSymScaleVec(bool limitdegflag, int srcmotid, double srcframe)
{
	ChaVector3 retscale = ChaVector3(1.0f, 1.0f, 1.0f);

	//2023/04/28
	if (IsNotSkeleton()) {
		return retscale;
	}



	int symboneno = 0;
	int existflag = 0;
	m_parmodel->GetSymBoneNo(GetBoneNo(), &symboneno, &existflag);
	if (symboneno >= 0) {
		CBone* symbone = m_parmodel->GetBoneByID(symboneno);
		_ASSERT(symbone);
		if (symbone) {
			//if (symbone == this){
			//	WCHAR dbgmes[1024];
			//	swprintf_s(dbgmes, 1024, L"CalcLocalSymRotMat : frame %lf : samebone : this[%s]--sym[%s]", srcframe, GetWBoneName(), symbone->GetWBoneName());
			//	::MessageBox(NULL, dbgmes, L"check", MB_OK);
			//}
			retscale = symbone->CalcLocalScaleAnim(limitdegflag, srcmotid, srcframe);
		}
		else {
			retscale = CalcLocalScaleAnim(limitdegflag, srcmotid, srcframe);
			_ASSERT(0);
		}
	}
	else {
		retscale = CalcLocalScaleAnim(limitdegflag, srcmotid, srcframe);
		_ASSERT(0);
	}

	return retscale;
}



ChaVector3 CBone::CalcLocalTraAnim(bool limitdegflag, int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return ChaVector3(0.0f, 0.0f, 0.0f);
	}


	ChaMatrix curmat;
	curmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

	ChaMatrix localmat;
	localmat.SetIdentity();
	if (GetParent(false)){
		ChaMatrix parmat;
		parmat = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		localmat = curmat * ChaMatrixInv(parmat);
	}
	else {
		localmat = curmat;
	}

	ChaMatrix smat, rmat, tmat;
	GetSRTMatrix2(localmat, &smat, &rmat, &tmat);

	ChaVector3 jointpos;
	ChaMatrix beftramat, afttramat;
	ChaMatrix localSRmat;
	ChaMatrixIdentity(&beftramat);
	ChaMatrixIdentity(&afttramat);
	ChaMatrixIdentity(&localSRmat);
	jointpos = GetJointFPos();
	ChaMatrixTranslation(&beftramat, -jointpos.x, -jointpos.y, -jointpos.z);
	ChaMatrixTranslation(&afttramat, jointpos.x, jointpos.y, jointpos.z);
	localSRmat = beftramat * smat * rmat * afttramat;

	ChaVector3 srpos, localpos;
	ChaVector3TransformCoord(&srpos, &jointpos, &localSRmat);
	ChaVector3TransformCoord(&localpos, &jointpos, &localmat);

	ChaVector3 rettraanim;
	rettraanim = localpos - srpos;
	return rettraanim;

}

ChaVector3 CBone::CalcLocalSymTraAnim(bool limitdegflag, int srcmotid, double srcframe)
{

	ChaVector3 rettra = ChaVector3(0.0f, 0.0f, 0.0);//scaleに設定されていてもrotcenterの位置になる

	//2023/04/28
	if (IsNotSkeleton()) {
		return rettra;
	}


	int symboneno = 0;
	int existflag = 0;
	m_parmodel->GetSymBoneNo(GetBoneNo(), &symboneno, &existflag);
	if (symboneno >= 0) {
		CBone* symbone = m_parmodel->GetBoneByID(symboneno);
		_ASSERT(symbone);
		if (symbone) {
			//if (symbone == this){
			//	WCHAR dbgmes[1024];
			//	swprintf_s(dbgmes, 1024, L"CalcLocalSymRotMat : frame %lf : samebone : this[%s]--sym[%s]", srcframe, GetWBoneName(), symbone->GetWBoneName());
			//	::MessageBox(NULL, dbgmes, L"check", MB_OK);
			//}
			rettra = symbone->CalcLocalTraAnim(limitdegflag, srcmotid, srcframe);
			//rettra.x *= -1.0f;//この関数の外で必要に応じて-Xするので　ここではしない
		}
		else {
			rettra = CalcLocalScaleAnim(limitdegflag, srcmotid, srcframe);
			_ASSERT(0);
		}
	}
	else {
		rettra = CalcLocalScaleAnim(limitdegflag, srcmotid, srcframe);
		_ASSERT(0);
	}

	return rettra;

}

ChaVector3 CBone::CalcFbxScaleAnim(bool limitdegflag, int srcmotid, double srcframe)
{
	//############################
	//fbx書き出し専用
	//############################

	//############################################################################
	// 2022/09/12 
	// fromnobindpose : bindpose無しのfbx書き出し：NodeMatを掛けた姿勢を書き出す。
	//############################################################################

	//############################################################################
	// 2022/10/31 
	// NodeMatを掛けた姿勢を書き出す。
	//############################################################################

	double roundingframe = RoundingTime(srcframe);

	ChaVector3 svec, tvec;
	ChaMatrix rmat;
	ChaVector3 iniscale = ChaVector3(1.0f, 1.0f, 1.0f);


	//ChaMatrix wmanim = GetNodeMat() * GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	//
	//ChaMatrix parentfbxwm;
	//parentfbxwm.SetIdentity();
	//if (GetParent(false)) {

	//	//parentがeNullの場合はある
	//	if (GetParent(false)->IsSkeleton()) {
	//		ChaMatrix parentwmanim = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	//		parentfbxwm = GetParent(false)->GetNodeMat() * parentwmanim;
	//	}
	//	else {
	//		parentfbxwm = GetParent(false)->GetNodeMat();
	//	}
	//}
	//else {
	//	parentfbxwm.SetIdentity();
	//}

	//ChaMatrix localfbxmat = wmanim * ChaMatrixInv(parentfbxwm);

	if (IsSkeleton()) {
		ChaMatrix localfbxmat = CalcFbxLocalMatrix(limitdegflag, srcmotid, srcframe);
		GetSRTMatrix(localfbxmat, &svec, &rmat, &tvec);
	}
	else if(IsNull() || IsCamera()){
		if (GetFbxNodeOnLoad()) {
			FbxTime fbxtime;
			fbxtime.SetSecondDouble(roundingframe / 30.0);
			FbxVector4 fbxcamerascl = GetFbxNodeOnLoad()->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot, true, true);
			svec = ChaVector3(fbxcamerascl);
		}
		else {
			svec = ChaVector3(1.0f, 1.0f, 1.0f);
		}
	}
	else {
		svec = ChaVector3(1.0f, 1.0f, 1.0f);
	}

	return svec;
}



ChaVector3 CBone::CalcLocalScaleAnim(bool limitdegflag, int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return ChaVector3(1.0f, 1.0f, 1.0f);
	}


	ChaVector3 svec, tvec;
	ChaMatrix rmat;
	ChaVector3 iniscale = ChaVector3(1.0f, 1.0f, 1.0f);

	CMotionPoint* pcurmp = 0;
	//CMotionPoint* pparmp = 0;
	pcurmp = GetMotionPoint(srcmotid, roundingframe);
	if (GetParent(false)) {
		if (pcurmp) {
			CMotionPoint setmp;
			ChaMatrix invpar = ChaMatrixInv(GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0));
			ChaMatrix localmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, pcurmp) * invpar;

			GetSRTMatrix(localmat, &svec, &rmat, &tvec);
			return svec;
		}
		else {
			return iniscale;
		}
	}
	else {
		if (pcurmp) {
			CMotionPoint setmp;
			ChaMatrix localmat = GetWorldMat(limitdegflag, srcmotid, roundingframe, pcurmp);

			GetSRTMatrix(localmat, &svec, &rmat, &tvec);
			return svec;

		}
		else {
			return iniscale;
		}
	}
}



int CBone::PasteMotionPoint(bool limitdegflag, int srcmotid, double srcframe, CMotionPoint srcmp)
{
	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CMotionPoint* newmp = 0;
	newmp = GetMotionPoint(srcmotid, roundingframe);
	if (newmp){
		ChaMatrix befrotmat, aftrotmat;
		befrotmat.SetIdentity();
		aftrotmat.SetIdentity();
		befrotmat.SetTranslation(-GetJointFPos());
		aftrotmat.SetTranslation(GetJointFPos());


		ChaMatrix orgwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, newmp);
		ChaMatrix orglocalmat;
		if (GetParent(false)) {
			ChaMatrix parentwm;
			parentwm = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			orglocalmat = orgwm * ChaMatrixInv(parentwm);
		}
		else {
			orglocalmat = orgwm;
		}
		ChaMatrix orgLocalS, orgLocalR, orgLocalT, orgLocalTAnim;
		GetSRTandTraAnim(orglocalmat, GetNodeMat(), &orgLocalS, &orgLocalR, &orgLocalT, &orgLocalTAnim);


		ChaMatrix copylocalmat = srcmp.GetWorldMat();//localがセットされている
		ChaMatrix copyLocalS, copyLocalR, copyLocalT, copyLocalTAnim;
		GetSRTandTraAnim(copylocalmat, GetNodeMat(), &copyLocalS, &copyLocalR, &copyLocalT, &copyLocalTAnim);



		ChaMatrix setLocalS, setLocalR, setLocalTAnim;
		setLocalS.SetIdentity();
		setLocalR.SetIdentity();
		setLocalTAnim.SetIdentity();

		//2024/03/11 S, R, T毎にペースト可能に
		if (g_pasteScale) {
			setLocalS = copyLocalS;
		}
		else {
			setLocalS = orgLocalS;
		}
		if (g_pasteRotation) {
			setLocalR = copyLocalR;
		}
		else {
			setLocalR = orgLocalR;
		}
		if (g_pasteTranslation) {
			setLocalTAnim = copyLocalTAnim;
		}
		else {
			setLocalTAnim = orgLocalTAnim;
		}
		ChaMatrix setlocalmat;
		setlocalmat = befrotmat * setLocalS * setLocalR * aftrotmat * setLocalTAnim;


		ChaMatrix setmat;
		setmat.SetIdentity();
		if (GetParent(false)){
			ChaMatrix parentwm;
			parentwm = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			setmat = setlocalmat * parentwm;//copy情報はローカルなのでグロバールにする
		}
		else {
			setmat = setlocalmat;
		}

		//bool directsetflag = false;
		//bool infooutflag = false;
		//int setchildflag = 1;//setchildflagは directsetflag == falseのときしか働かない
		//SetWorldMat(limitdegflag, directsetflag, infooutflag, setchildflag, srcmotid, roundingframe, setmat);

		UpdateCurrentWM(limitdegflag, srcmotid, roundingframe, setmat);

	}

	return 0;
}

ChaVector3 CBone::CalcFBXEulXYZ(bool limitdegflag, int srcmotid, double srcframe)
{

	//############################
	// fbx書き出し専用
	//############################

	//############################################################################
	// 2022/09/12 
	// fromnobindpose : bindpose無しのfbx書き出し：NodeMatを掛けた姿勢を書き出す。
	//############################################################################

	//################################################
	//必要ノイズ付与機能付き　: FBX書き出し時のみ使用
	//################################################

	//############################################################################
	// 2022/10/31 
	// NodeMatを掛けた姿勢を書き出す。
	//############################################################################

	//#####################################################################
	// 2023/02/16
	// オイラー角がMayaと同じなのだから　そのまま書き出す方が合っている
	// ジョイントの向きが設定されていても正しく書き出せる
	//#####################################################################

	double roundingframe = RoundingTime(srcframe);

	ChaVector3 cureul;
	cureul = CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);

	return cureul;
}
//ChaVector3 CBone::CalcFBXEulZXY(int srcmotid, double srcframe, ChaVector3* befeulptr)
//{
//	CMotionPoint tmpmp;
//	CalcLocalInfo(srcmotid, srcframe, &tmpmp);
//	int isfirstbone;
//	if (GetParent()){
//		isfirstbone = 0;
//	}
//	else{
//		isfirstbone = 1;
//	}
//
//	ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
//	if (befeulptr){
//		befeul = *befeulptr;
//	}
//
//	//tmpmp.GetQ().CalcFBXEul(0, befeul, &cureul, isfirstbone);
//	tmpmp.GetQ().CalcFBXEulZXY(0, befeul, &cureul, isfirstbone);
//
//	return cureul;
//
//}
ChaVector3 CBone::CalcFBXTra(bool limitdegflag, int srcmotid, double srcframe)
{
	//############################
	// fbx書き出し専用
	//############################

	//############################################################################
	// 2022/09/12 
	// fromnobindpose : bindpose無しのfbx書き出し：NodeMatを掛けた姿勢を書き出す。
	//############################################################################

	//############################################################################
	// 2022/10/31 
	// NodeMatを掛けた姿勢を書き出す。
	//############################################################################


	double roundingframe = RoundingTime(srcframe);

	if (IsSkeleton()) {

		//#################
		//Skeletonの場合
		//#################

		ChaMatrix localfbxmat = CalcFbxLocalMatrix(limitdegflag, srcmotid, srcframe);
		ChaVector3 svec, tvec;
		ChaMatrix rmat;
		GetSRTMatrix(localfbxmat, &svec, &rmat, &tvec);
		return tvec;
	}
	else if(IsCamera() || IsNull()){

		//########################
		//カメラまたはeNull の場合
		//########################

		//if (GetParModel() && GetParModel()->IsCameraLoaded() && GetFbxNodeOnLoad()) {
		if (GetParModel() && GetFbxNodeOnLoad()) {
			FbxTime fbxtime;
			fbxtime.SetSecondDouble(roundingframe / 30.0);
			FbxVector4 fbxtra = GetFbxNodeOnLoad()->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot, true, true);
			ChaVector3 tra = ChaVector3(fbxtra, false);

			return tra;

		}
		else {
			_ASSERT(0);
			return ChaVector3(0.0f, 0.0f, 0.0f);
		}

	}
	else {
		return ChaVector3(0.0f, 0.0f, 0.0f);
	}


	////ver. 1.2.0.20
	////Hipsよりも上の階層にスケールを設定したトランスフォームノードがある場合のテスト結果により
	////FbxTraはFbxScaleで割る
	//ChaVector3 rettvec = tvec;
	//if (svec.x != 0.0f) {
	//	rettvec.x /= svec.x;
	//}
	//if (svec.y != 0.0f) {
	//	rettvec.y /= svec.y;
	//}
	//if (svec.z != 0.0f) {
	//	rettvec.z /= svec.z;
	//}
	//return rettvec;


	//ChaVector3 fbxtra = ChaVector3(localfbxmat.data[MATI_41], localfbxmat.data[MATI_42], localfbxmat.data[MATI_43]);
	//return fbxtra;

}

int CBone::QuaternionInOrder(bool limitdegflag, int srcmotid, double srcframe, CQuaternion* srcdstq)
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CQuaternion beflocalq;
	CMotionPoint befmp;
	double befframe = srcframe - 1.0;
	if (befframe >= 0.0001){
		CalcLocalInfo(limitdegflag, srcmotid, befframe, &befmp);
		beflocalq = befmp.GetQ();
	}

	beflocalq.InOrder(srcdstq);

	return 0;

}

/*
int CBone::CalcNewBtMat(CRigidElem* srcre, CBone* childbone, ChaMatrix* dstmat, ChaVector3* dstpos)
{
	ChaMatrixIdentity(dstmat);
	*dstpos = ChaVector3(0.0f, 0.0f, 0.0f);

	if (!childbone || !dstmat || !dstpos){
		return 1;
	}

	ChaVector3 jointfpos;
	ChaMatrix firstworld;
	ChaMatrix invfirstworld;
	ChaMatrix curworld;
	ChaMatrix befworld;
	ChaMatrix invbefworld;
	ChaMatrix diffworld;
	ChaVector3 rigidcenter;
	ChaMatrix multmat;
	ChaMatrix tramat;


	firstworld = GetStartMat2();
	ChaMatrixInverse(&invfirstworld, NULL, &firstworld);


	//current
	if (GetBtKinFlag() == 1){
		diffworld = invfirstworld * GetCurMp().GetWorldMat();
		tramat = GetCurMp().GetWorldMat();
	}
	else{
		//シミュ結果をそのまま。アニメーションは考慮しなくてよい。
		if (GetCurMp().GetBtFlag() == 0){
			diffworld = invfirstworld * GetCurMp().GetWorldMat();
			tramat = GetCurMp().GetWorldMat();
		}
		else{
			//ここでのBtMatは一回前の姿勢。
			diffworld = invfirstworld * GetCurMp().GetBtMat();
			tramat = GetCurMp().GetBtMat();
		}
	}
	jointfpos = GetJointFPos();
	ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);

	//child
	jointfpos = childbone->GetJointFPos();
	ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);



	multmat = srcre->GetFirstcapsulemat() * diffworld;
	//rigidcenter = (m_btparentpos + m_btchildpos) * 0.5f;
	rigidcenter = m_btparentpos;


	*dstmat = multmat;
	*dstpos = rigidcenter;

	return 0;
}
*/



//#################################################
//2023/01/18
//通常モーションと物理剛体の軸合わせをしたので
//回転情報は通常モーションと物理とで共通となった
//#################################################
int CBone::CalcNewBtMat(CModel* srcmodel, CBone* childbone, ChaMatrix* dstmat, ChaVector3* dstpos)
{
	//srcframe : 時間補間有り

	ChaMatrixIdentity(dstmat);
	*dstpos = ChaVector3(0.0f, 0.0f, 0.0f);

	if (!childbone || !dstmat || !dstpos){
		return 1;
	}

	//2023/04/28
	if (IsNotSkeleton()) {
		dstmat->SetIdentity();
		*dstpos = ChaVector3(0.0f, 0.0f, 0.0f);
		return 0;
	}



	ChaVector3 jointfpos;
	ChaMatrix curworld;
	ChaMatrix befworld;
	ChaMatrix invbefworld;
	ChaVector3 rigidcenter;
	ChaMatrix rotmat;
	ChaMatrix tramat, childtramat;

	ChaMatrix befbtmat;
	befbtmat = GetBtMat();

	//if (GetBtFlag() == 0){
		//再帰処理中のまだ未セットの状態の場合
		//befbtmat = GetBtMat();
	//}
	//else{
	//	//再帰処理中のすでにセットした状態の場合
	//	befbtmat = GetBefBtMat();
	//}


	curworld = GetCurrentWorldMat(true);


	//current
	if (GetBtKinFlag() == 1){
		//tramat = GetCurMp().GetWorldMat();
		tramat = curworld;
		rotmat = ChaMatrixRot(tramat);

		jointfpos = GetJointFPos();
		ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);
		jointfpos = childbone->GetJointFPos();
		ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);

	}
	else{
		//シミュ結果をそのまま。アニメーションは考慮しなくてよい。
		if (srcmodel->GetBtCnt() == 0){
			//tramat = GetCurMp().GetWorldMat();
			tramat = curworld;
			rotmat = ChaMatrixRot(tramat);

			jointfpos = GetJointFPos();
			ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);
			jointfpos = childbone->GetJointFPos();
			ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);
		}
		else{
			//親方向に　Kinematicのボーンを探す
			CBone* kinematicbone = 0;
			CBone* findbone = GetParent(false);
			while (findbone) {
				if (findbone->IsSkeleton() && (findbone->GetBtKinFlag() != 0)) {
					kinematicbone = findbone;
					break;
				}
				findbone = findbone->GetParent(false);
			}

			//2023/01/28
			//純粋な物理計算においては　Kinematic部分だけ手動で移動すれば良いのだが
			//計算が乱れやすく　大げさになり易いので
			//Kinematicとそうではない境目のKinematicの　全フレームからの移動分を　子供ジョイントに波及させる
			//この処理を加えることにより　ジャンプして着地した時の　乱れ方が　大きくなり過ぎないようになった
			if (kinematicbone) {
				ChaMatrix befparentwm, curparentwm;
				befparentwm = kinematicbone->GetBtMat();//実質一回前の　BtMat
				curparentwm = kinematicbone->GetCurrentWorldMat(true);//カレントのKinematic姿勢

				jointfpos = kinematicbone->GetJointFPos();
				ChaVector3 befparentpos, curparentpos;
				ChaVector3TransformCoord(&befparentpos, &jointfpos, &befparentwm);
				ChaVector3TransformCoord(&curparentpos, &jointfpos, &curparentwm);
				ChaVector3 movevec;
				movevec = curparentpos - befparentpos;//前回から今回への　位置移動分
				ChaMatrix movemat;
				movemat.SetIdentity();
				movemat.SetTranslation(movevec);

				tramat = befbtmat * movemat;
			}
			else {
				tramat = befbtmat;
			}

				
			rotmat = ChaMatrixRot(tramat);				

			jointfpos = GetJointFPos();
			ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);
			jointfpos = childbone->GetJointFPos();
			ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);
		}
	}

	rigidcenter = (m_btparentpos + m_btchildpos) * 0.5f;


	*dstmat = rotmat;
	*dstpos = rigidcenter;

	return 0;
}


//###################################################################################
//2023/01/18よりも前のバージョン
//通常モーションと物理剛体の軸が異なる場合の　diffを使ったやり方
//軸合わせをしたので　回転情報をそのままセットする方法に変えた　よってコメントアウト
//###################################################################################
//int CBone::CalcNewBtMat(CModel* srcmodel, CRigidElem* srcre, CBone* childbone, ChaMatrix* dstmat, ChaVector3* dstpos)
//{
//	ChaMatrixIdentity(dstmat);
//	*dstpos = ChaVector3(0.0f, 0.0f, 0.0f);
//
//	if (!childbone || !dstmat || !dstpos) {
//		return 1;
//	}
//
//	ChaVector3 jointfpos;
//	ChaMatrix firstmat;
//	ChaMatrix invfirstmat;
//	ChaMatrix curworld;
//	ChaMatrix befworld;
//	ChaMatrix invbefworld;
//	ChaMatrix diffworld;
//	ChaVector3 rigidcenter;
//	ChaMatrix multmat;
//	ChaMatrix tramat;
//
//
//	//firstmat = GetFirstMat();
//	firstmat = GetCurrentZeroFrameMat(0);
//	ChaMatrixInverse(&invfirstmat, NULL, &firstmat);
//
//	ChaMatrix befbtmat;
//	if (GetBtFlag() == 0) {
//		//再帰処理中のまだ未セットの状態の場合
//		befbtmat = GetBtMat();
//	}
//	else {
//		//再帰処理中のすでにセットした状態の場合
//		befbtmat = GetBefBtMat();
//	}
//
//	//current
//	if (GetBtKinFlag() == 1) {
//		diffworld = invfirstmat * GetCurMp().GetWorldMat();
//		tramat = GetCurMp().GetWorldMat();
//
//		jointfpos = GetJointFPos();
//		ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);
//		jointfpos = childbone->GetJointFPos();
//		ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);
//
//	}
//	else {
//		//シミュ結果をそのまま。アニメーションは考慮しなくてよい。
//		if (srcmodel->GetBtCnt() == 0) {
//			diffworld = invfirstmat * GetCurMp().GetWorldMat();
//			tramat = GetCurMp().GetWorldMat();
//
//			jointfpos = GetJointFPos();
//			ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);
//			jointfpos = childbone->GetJointFPos();
//			ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);
//		}
//		else {
//			if (GetParent() && (GetParent()->GetBtKinFlag() == 1)) {
//				//ここでのBtMatは一回前の姿勢。
//
//				//BtMatにアニメーションの移動成分のみを掛けたものを新しい姿勢行列として子供ジョイント位置を計算してシミュレーションに使用する。
//				curworld = GetCurMp().GetWorldMat();
//				//befworld = GetCurMp().GetBefWorldMat();
//				befworld = GetCurrentZeroFrameMat(0);
//
//				ChaVector3 befparentpos, curparentpos;
//				jointfpos = GetJointFPos();
//				ChaVector3TransformCoord(&befparentpos, &jointfpos, &befworld);
//				ChaVector3TransformCoord(&curparentpos, &jointfpos, &curworld);
//				ChaVector3 diffmv = curparentpos - befparentpos;
//
//				ChaMatrix diffmvmat;
//				ChaMatrixIdentity(&diffmvmat);
//				ChaMatrixTranslation(&diffmvmat, diffmv.x, diffmv.y, diffmv.z);
//
//				ChaMatrixInverse(&invbefworld, NULL, &befworld);
//				ChaMatrix newtramat = befbtmat * diffmvmat;
//
//				diffworld = invfirstmat * newtramat;
//
//				m_btparentpos = curparentpos;
//				jointfpos = childbone->GetJointFPos();
//				//ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &befbtmat);
//				ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &newtramat);
//			}
//			else {
//				diffworld = invfirstmat * befbtmat;
//				tramat = befbtmat;
//
//				jointfpos = GetJointFPos();
//				ChaVector3TransformCoord(&m_btparentpos, &jointfpos, &tramat);
//				jointfpos = childbone->GetJointFPos();
//				ChaVector3TransformCoord(&m_btchildpos, &jointfpos, &tramat);
//			}
//		}
//	}
//
//	//multmat = srcre->GetBindcapsulemat() * diffworld;
//	CBtObject* curbto = GetBtObject(childbone);
//	if (curbto) {
//		multmat = curbto->GetFirstTransformMatX() * diffworld;
//	}
//	else {
//		multmat = GetCurrentZeroFrameMat(0) * diffworld;
//		_ASSERT(0);
//	}
//	rigidcenter = (m_btparentpos + m_btchildpos) * 0.5f;
//
//	*dstmat = multmat;
//	*dstpos = rigidcenter;
//
//	return 0;
//}

ChaVector3 CBone::GetChildWorld(){
	if (g_previewFlag != 5){
		//ChaVector3TransformCoord(&m_childworld, &m_jointfpos, &m_curmp.GetWorldMat());
		ChaMatrix tmplimwm = GetCurrentWorldMat(true);
		ChaVector3TransformCoord(&m_childworld, &m_jointfpos, &tmplimwm);
	}
	else{
		ChaMatrix wmat;
		if (GetParent(false) && GetParent(false)->IsSkeleton()){
			wmat = GetParent(false)->GetBtMat();
		}
		else{
			wmat = GetBtMat();
		}

		//ChaVector3TransformCoord(&m_childworld, &m_jointfpos, &(GetBtMat()));
		ChaVector3TransformCoord(&m_childworld, &m_jointfpos, &wmat);
	}
	return m_childworld;
};

int CBone::LoadCapsuleShape(ID3D12Device* pdev)
{

	WCHAR wfilename[MAX_PATH];
	WCHAR mpath[MAX_PATH];

	wcscpy_s(mpath, MAX_PATH, g_basedir);
	WCHAR* lasten = 0;
	WCHAR* last2en = 0;
	lasten = wcsrchr(mpath, TEXT('\\'));
	if (lasten){
		*lasten = 0L;
		last2en = wcsrchr(mpath, TEXT('\\'));
		if (last2en){
			*last2en = 0L;
			wcscat_s(mpath, MAX_PATH, L"\\Media\\MameMedia");
		}
	}

	if (!s_coldisp[COL_CONE_INDEX]) {
		s_coldisp[COL_CONE_INDEX] = new CModel();
		if (!s_coldisp[COL_CONE_INDEX]) {
			_ASSERT(0);
			return 1;
		}
		s_coldisp[COL_CONE_INDEX]->SetInstancingNum(RIGMULTINDEXMAX);
		swprintf_s(wfilename, MAX_PATH, L"%s\\%s", mpath, L"cone_dirX.mqo");
		CallF(s_coldisp[COL_CONE_INDEX]->LoadMQO(pdev, wfilename, 0, 1.0f, 0), return 1);
		//CallF(m_coldisp[COL_CONE_INDEX]->MakeDispObj(), return 1);
	}

	if (!s_coldisp[COL_CAPSULE_INDEX]) {
		s_coldisp[COL_CAPSULE_INDEX] = new CModel();
		if (!s_coldisp[COL_CAPSULE_INDEX]) {
			_ASSERT(0);
			return 1;
		}
		s_coldisp[COL_CAPSULE_INDEX]->SetInstancingNum(RIGMULTINDEXMAX);
		swprintf_s(wfilename, MAX_PATH, L"%s\\%s", mpath, L"capsule_dirX.mqo");
		CallF(s_coldisp[COL_CAPSULE_INDEX]->LoadMQO(pdev, wfilename, 0, 1.0f, 0), return 1);
		//CallF(m_coldisp[COL_CAPSULE_INDEX]->MakeDispObj(), return 1);
	}

	if (!s_coldisp[COL_SPHERE_INDEX]) {
		s_coldisp[COL_SPHERE_INDEX] = new CModel();
		if (!s_coldisp[COL_SPHERE_INDEX]) {
			_ASSERT(0);
			return 1;
		}
		s_coldisp[COL_SPHERE_INDEX]->SetInstancingNum(RIGMULTINDEXMAX);
		swprintf_s(wfilename, MAX_PATH, L"%s\\%s", mpath, L"sphere_dirX.mqo");
		CallF(s_coldisp[COL_SPHERE_INDEX]->LoadMQO(pdev, wfilename, 0, 1.0f, 0), return 1);
		//CallF(m_coldisp[COL_SPHERE_INDEX]->MakeDispObj(), return 1);
	}

	if (!s_coldisp[COL_BOX_INDEX]) {
		s_coldisp[COL_BOX_INDEX] = new CModel();
		if (!s_coldisp[COL_BOX_INDEX]) {
			_ASSERT(0);
			return 1;
		}
		s_coldisp[COL_BOX_INDEX]->SetInstancingNum(RIGMULTINDEXMAX);
		swprintf_s(wfilename, MAX_PATH, L"%s\\%s", mpath, L"box.mqo");
		CallF(s_coldisp[COL_BOX_INDEX]->LoadMQO(pdev, wfilename, 0, 1.0f, 0), return 1);
		//CallF(m_coldisp[COL_BOX_INDEX]->MakeDispObj(), return 1);
	}


	//2024/02/08 for RefPos
	if (!s_refposmark) {
		s_refposmark = new CModel();
		if (!s_refposmark) {
			_ASSERT(0);
			return 1;
		}
		s_refposmark->SetInstancingNum(RIGMULTINDEXMAX);
		swprintf_s(wfilename, MAX_PATH, L"%s\\%s", mpath, L"Trajectory_dirX.mqo");
		CallF(s_refposmark->LoadMQO(pdev, wfilename, 0, 1.0f, 0), return 1);
		//CallF(m_coldisp[COL_REFPOS_INDEX]->MakeDispObj(), return 1);
	}


	return 0;
}

CModel* CBone::GetColDisp(CBone* childbone, int srcindex)
{
	if ((srcindex < COL_CONE_INDEX) || (srcindex > COL_MAX)) {
		return 0;
	}
	if (!childbone) {
		return 0;
	}

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	CRigidElem* curre = GetRigidElem(childbone);
	if (!curre) {
		_ASSERT(0);
		return 0;
	}

	//_ASSERT(colptr);
	_ASSERT(childbone);

	CModel* retcoldisp = s_coldisp[srcindex];
	_ASSERT(retcoldisp);

	return retcoldisp;


}
CModel* CBone::GetRefPosMark()
{
	if (IsNotSkeleton()) {
		return 0;
	}

	return s_refposmark;
}


CModel* CBone::GetCurColDispInstancing(CBone* childbone, int* pinstanceno)
{
	if (!childbone || !pinstanceno) {
		return 0;
	}

	*pinstanceno = -1;

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}
	if (!GetParModel()) {
		return 0;
	}

	CRigidElem* curre = GetRigidElem(childbone);
	if (!curre){
		_ASSERT(0);
		return 0;
	}

	//_ASSERT(colptr);
	_ASSERT(childbone);


	int instanceno = s_coldispgetnum[curre->GetColtype()];
	*pinstanceno = instanceno;

	CModel* curcoldisp = s_coldisp[curre->GetColtype()];
	_ASSERT(curcoldisp);
	s_coldispgetnum[curre->GetColtype()] = instanceno + 1;

	return curcoldisp;
}
CModel* CBone::GetRefPosMarkInstancing(int* pinstanceno)
{
	if (!pinstanceno) {
		_ASSERT(0);
		return 0;
	}

	*pinstanceno = -1;

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}
	if (!GetParModel()) {
		return 0;
	}

	//CRigidElem* curre = GetRigidElem(childbone);
	//if (!curre) {
	//	_ASSERT(0);
	//	return 0;
	//}
	////_ASSERT(colptr);
	//_ASSERT(childbone);

	int instanceno = s_refposmarkgetnum;
	*pinstanceno = instanceno;

	_ASSERT(s_refposmark);

	s_refposmarkgetnum = instanceno + 1;

	return s_refposmark;
}


void CBone::SetRigidElemOfMap(std::string srcstr, CBone* srcbone, CRigidElem* srcre)
{
	
	//2023/04/28
	if (IsNotSkeleton()) {
		return;
	}


	std::map<std::string, std::map<CBone*, CRigidElem*>>::iterator itrremap;
	itrremap = m_remap.find(srcstr);
	if (itrremap != m_remap.end()){

		//itrremap->second[srcbone] = srcre;

		std::map<CBone*, CRigidElem*>::iterator itrsetmap;
		itrsetmap = itrremap->second.find(srcbone);
		if (itrsetmap != itrremap->second.end()){
			CRigidElem* delre = itrsetmap->second;
			if (delre){
				//delete itrsetmap->second;
				CRigidElem::InvalidateRigidElem(delre);
				itrsetmap->second = 0;
			}
			itrsetmap->second = srcre;
		}
		else{
			itrremap->second[srcbone] = srcre;
		}
	}
	else{
		std::map<CBone*, CRigidElem*> newmap;
		newmap[srcbone] = srcre;
		m_remap[srcstr] = newmap;
	}
	
	//((m_remap[ srcstr ])[ srcbone ]) = srcre;
}

int CBone::SetCurrentMotion(int srcmotid, double animleng)
{
	SetCurMotID(srcmotid);
	//ResetMotionCache();

	//int result;
	//result = CreateIndexedMotionPoint(srcmotid, animleng);
	//_ASSERT(result == 0);

	return 0;
}

//current motionのframe 0のworldmat
//ChaMatrix CBone::GetCurrentZeroFrameMatFunc(bool limitdegflag, int updateflag, int inverseflag)
//{
//	//ZeroFrameの編集前と編集後のポーズのdiffをとる必要がある場合に対応する
//	//updateflagが1の場合に最新情報。0の場合に前回の取得情報と同じものを返す。
//
//	//static int s_firstgetflag = 0;
//	//static ChaMatrix s_firstgetmatrix;
//	//static ChaMatrix s_invfirstgetmatrix;
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		ChaMatrix inimat;
//		ChaMatrixIdentity(&inimat);
//		return inimat;
//	}
//
//
//	if ((m_curmotid <= 0) || (m_curmotid > m_motionkey.size())) {
//		//_ASSERT(0);
//		ChaMatrix inimat;
//		ChaMatrixIdentity(&inimat);
//		return inimat;
//	}
//
//	if (m_curmotid >= 1) {//idは１から
//		//CMotionPoint* pcur = m_motionkey[m_curmotid - 1];//idは１から
//		CMotionPoint* pcur = m_motionkey[m_curmotid - 1];//idは１から !!!!!!!!!!!!!!
//		if (pcur) {
//			if ((updateflag == 1) || (m_firstgetflag == 0)) {
//				m_firstgetflag = 1;
//				m_firstgetmatrix = GetWorldMat(limitdegflag, m_curmotid, 0.0, pcur);
//				m_invfirstgetmatrix = ChaMatrixInv(m_firstgetmatrix);
//			}
//
//			if (inverseflag == 0) {
//				return m_firstgetmatrix;
//			}
//			else {
//				return m_invfirstgetmatrix;
//			}
//		}
//		else {
//			ChaMatrix inimat;
//			ChaMatrixIdentity(&inimat);
//			return inimat;
//		}
//	}
//	else {
//		ChaMatrix inimat;
//		ChaMatrixIdentity(&inimat);
//		return inimat;
//	}
//
//}


ChaMatrix CBone::GetCurrentZeroFrameMat(bool limitdegflag, int updateflag)
{
	//ZeroFrameの編集前と編集後のポーズのdiffをとる必要がある場合に対応する
	//updateflagが1の場合に最新情報。0の場合に前回の取得情報と同じものを返す。

	ChaCalcFunc chacalcfunc;
	return chacalcfunc.GetCurrentZeroFrameMat(this, limitdegflag, updateflag);

}

ChaMatrix CBone::GetCurrentZeroFrameInvMat(bool limitdegflag, int updateflag)
{
	//ZeroFrameの編集前と編集後のポーズのdiffをとる必要がある場合に対応する
	//updateflagが1の場合に最新情報。0の場合に前回の取得情報と同じものを返す。

	ChaCalcFunc chacalcfunc;
	chacalcfunc.GetCurrentZeroFrameMat(this, limitdegflag, updateflag);

	return m_invfirstgetmatrix;
}


//static func
CBone* CBone::GetNewBone(CModel* parmodel)
{
	//目的としてはメモリの使いまわしではなく、メモリを連続させることでキャッシュヒットの可能性を増すことである

	//parmodelごとの使いまわししか出来ない
	//モデルの削除と作成を繰り返すとメモリが増え続ける
	//しかし必要モデルをあらかじめ作成して、表示非表示を切り替えて（削除作成を繰り返さずに）やりくりすれば良い

	static int s_befheadno = -1;
	static int s_befelemno = -1;

	if (!parmodel) {
		_ASSERT(0);
	}


	int curpoollen;
	curpoollen = (int)s_bonepool.size();

	//if ((s_befheadno != (s_bonepool.size() - 1)) || (s_befelemno != (BONEPOOLBLKLEN - 1))) {//前回リリースしたポインタが最後尾ではない場合

	//前回リリースしたポインタの次のメンバーをチェックして未使用だったらリリース
		int chkheadno;
		chkheadno = s_befheadno;
		int chkelemno;
		chkelemno = s_befelemno + 1;
		//if ((chkheadno >= 0) && (chkheadno >= curpoollen) && (chkelemno >= BONEPOOLBLKLEN)) {
		if ((chkheadno >= 0) && (chkheadno < (curpoollen - 1)) && (chkelemno >= BONEPOOLBLKLEN)) {//2021/08/21
			chkelemno = 0;
			chkheadno++;
		}
		if ((chkheadno >= 0) && (chkheadno < curpoollen) && (chkelemno >= 0) && (chkelemno < BONEPOOLBLKLEN)) {
			CBone* curbonehead = s_bonepool[chkheadno];
			if (curbonehead) {
				CBone* chkbone;
				chkbone = curbonehead + chkelemno;
				if (chkbone && (chkbone->GetParModel() == parmodel)) {//parmodelが同じ必要有。
					if (chkbone->GetUseFlag() == 0) {
						chkbone->InitParamsForReUse(parmodel);//

						s_befheadno = chkheadno;
						s_befelemno = chkelemno;

						return chkbone;
					}
				}
				else if (chkbone && (chkbone->GetParModel() == 0)) {
					chkbone->InitParamsForReUse(parmodel);//

					s_befheadno = chkheadno;
					s_befelemno = chkelemno;

					return chkbone;
				}
			}
		}

		//if ((chkheadno >= 0) && (chkheadno < curpoollen)) {
			//プールを先頭から検索して未使用がみつかればそれをリリース
		int boneno;
		for (boneno = 0; boneno < curpoollen; boneno++) {
			CBone* curbonehead = s_bonepool[boneno];
			if (curbonehead) {
				int elemno;
				for (elemno = 0; elemno < BONEPOOLBLKLEN; elemno++) {
					CBone* curbone;
					curbone = curbonehead + elemno;
					if (curbone && (curbone->GetParModel() == parmodel)) {//parmodelが同じ必要有。
						if (curbone->GetUseFlag() == 0) {
							curbone->InitParamsForReUse(parmodel);

							s_befheadno = boneno;
							s_befelemno = elemno;

							return curbone;
						}
					}
					else if (curbone && (curbone->GetParModel() == 0)) {
						if (curbone->GetUseFlag() == 0) {
							curbone->InitParamsForReUse(parmodel);

							s_befheadno = boneno;
							s_befelemno = elemno;

							return curbone;
						}
					}
				}
			}
		}
		//}
	//}


	//未使用boneがpoolに無かった場合、アロケートしてアロケートした先頭のポインタをリリース
	CBone* allocbone;
	allocbone = new CBone[BONEPOOLBLKLEN];
	if (!allocbone) {
		_ASSERT(0);

		s_befheadno = -1;
		s_befelemno = -1;

		return 0;
	}
	int allocno;
	for (allocno = 0; allocno < BONEPOOLBLKLEN; allocno++) {
		CBone* curallocbone = allocbone + allocno;
		if (curallocbone) {
			//int indexofpool = curpoollen + allocno;
			int indexofpool = curpoollen;//pool[indexofpool] 2021/08/19
			curallocbone->InitParams();
			curallocbone->SetParams(parmodel);//!!!!!作成時にはparmodel以外にボーン番号なども決定
			curallocbone->SetUseFlag(0);
			curallocbone->SetIndexOfPool(indexofpool);

			if (allocno == 0) {
				curallocbone->SetIsAllocHead(1);
			}
			else {
				curallocbone->SetIsAllocHead(0);
			}
		}
		else {
			_ASSERT(0);

			s_befheadno = -1;
			s_befelemno = -1;

			return 0;
		}
	}
	s_bonepool.push_back(allocbone);//allocate block(アロケート時の先頭ポインタ)を格納

	allocbone->SetUseFlag(1);


	s_befheadno = (int)s_bonepool.size() - 1;
	if (s_befheadno < 0) {
		s_befheadno = 0;
	}
	s_befelemno = 0;

	return allocbone;
}

//static func
void CBone::InvalidateBone(CBone* srcbone)
{
	if (!srcbone) {
		_ASSERT(0);
		return;
	}

	int saveindex = srcbone->GetIndexOfPool();
	int saveallochead = srcbone->IsAllocHead();
	CModel* saveparmodel = srcbone->GetParModel();

	srcbone->DestroyObjs();

	srcbone->InitParams();
	srcbone->SetUseFlag(0);
	srcbone->SetIsAllocHead(saveallochead);
	srcbone->SetIndexOfPool(saveindex);
	srcbone->m_parmodel = saveparmodel;
}

//static func
void CBone::InitBones()
{
	s_bonepool.clear();
}

//static func
void CBone::DestroyBones() 
{
	int boneallocnum = (int)s_bonepool.size();
	int boneno;
	for (boneno = 0; boneno < boneallocnum; boneno++) {
		CBone* delbone;
		delbone = s_bonepool[boneno];
		//if (delbone && (delbone->IsAllocHead() == 1)) {
		if (delbone) {
			delete[] delbone;
		}
	}
	s_bonepool.clear();
}

void CBone::OnDelModel(CModel* srcparmodel)
{
	//if ((chkheadno >= 0) && (chkheadno < curpoollen)) {
		//プールを先頭から検索して未使用がみつかればそのparmodelを０にする

	int curpoollen;
	curpoollen = (int)s_bonepool.size();

	int boneno;
	for (boneno = 0; boneno < curpoollen; boneno++) {
		CBone* curbonehead = s_bonepool[boneno];
		if (curbonehead) {
			int elemno;
			for (elemno = 0; elemno < BONEPOOLBLKLEN; elemno++) {
				CBone* curbone;
				curbone = curbonehead + elemno;
				if (curbone && (curbone->GetParModel() == srcparmodel)) {//parmodelが同じ必要有。
					//if (curbone && (curbone->GetUseFlag() == 0)) {//srcparmodelに関して再利用を防ぐ
						curbone->m_parmodel = 0;
						curbone->SetUseFlag(0);
					//}
				}
			}
		}
	}
	//}

	map<CModel*, int>::iterator itrbonecnt;
	itrbonecnt = g_bonecntmap.find(srcparmodel);
	if (itrbonecnt != g_bonecntmap.end()) {
		g_bonecntmap.erase(itrbonecnt);//エントリー削除
	}

}


//ChaMatrix CBone::CalcParentGlobalMat(int srcmotid, double srcframe)
//{
//	ChaMatrix retmat;
//	ChaMatrixIdentity(&retmat);
//
//	if (!GetParent(false)) {
//		return retmat;
//	}
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		return retmat;
//	}
//
//
//	CalcParentGlobalMatReq(&retmat, GetParent(), srcmotid, srcframe);
//
//
//	return retmat;
//}


//void CBone::CalcParentGlobalMatReq(ChaMatrix* dstmat, CBone* srcbone, int srcmotid, double srcframe)
//{
//	if (!srcbone || !dstmat) {
//		return;
//	}
//
//	double roundingframe = RoundingTime(srcframe);
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		dstmat->SetIdentity();
//		return;
//	}
//
//
//	CMotionPoint* curmp;
//	bool onaddmotion = true;
//	curmp = srcbone->GetMotionPoint(srcmotid, roundingframe, onaddmotion);
//	if (curmp) {
//		ChaMatrix localmat = curmp->GetLocalMat();
//		*dstmat = *dstmat * localmat;//childmat * currentmat   (currentmat * parentmat)
//	}
//
//	if (srcbone->GetParent()) {
//		CalcParentGlobalMatReq(dstmat, srcbone->GetParent(), srcmotid, roundingframe);
//	}
//
//}


////SRT形式
//ChaMatrix CBone::CalcParentGlobalSRT(int srcmotid, double srcframe)
//{
//	ChaMatrix retmat;
//	ChaMatrixIdentity(&retmat);
//
//	if (!GetParent()) {
//		return retmat;
//	}
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		return retmat;
//	}
//
//	CalcParentGlobalSRTReq(&retmat, GetParent(), srcmotid, srcframe);
//
//
//	return retmat;
//}
//
////SRT形式
//void CBone::CalcParentGlobalSRTReq(ChaMatrix* dstmat, CBone* srcbone, int srcmotid, double srcframe)
//{
//	if (!srcbone || !dstmat) {
//		return;
//	}
//
//	double roundingframe = RoundingTime(srcframe);
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		dstmat->SetIdentity();
//	}
//
//
//	CMotionPoint* mpptr = GetMotionPoint(srcmotid, roundingframe);
//	if (!mpptr) {
//		return;
//	}
//
//	ChaMatrix curSRT = mpptr->GetSRT();
//	*dstmat = *dstmat * curSRT;//childmat * currentmat   (currentmat * parentmat)
//
//	if (srcbone->GetParent()) {
//		CalcParentGlobalSRTReq(dstmat, srcbone->GetParent(), srcmotid, roundingframe);
//	}
//
//}

////SRT形式
//ChaMatrix CBone::CalcFirstParentGlobalSRT()
//{
//	ChaMatrix retmat;
//	ChaMatrixIdentity(&retmat);
//
//	if (!GetParent()) {
//		return retmat;
//	}
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		return retmat;
//	}
//
//
//	CalcFirstParentGlobalSRTReq(&retmat, GetParent());
//
//
//	return retmat;
//}
//
////SRT形式
//void CBone::CalcFirstParentGlobalSRTReq(ChaMatrix* dstmat, CBone* srcbone)
//{
//	if (!srcbone || !dstmat) {
//		return;
//	}
//
//	//2023/04/28
//	if (IsNotSkeleton()) {
//		dstmat->SetIdentity();
//	}
//
//
//	ChaMatrix firstSRT = srcbone->GetFirstSRT();
//	*dstmat = *dstmat * firstSRT;//childmat * currentmat   (currentmat * parentmat)
//
//	if (srcbone->GetParent()) {
//		CalcFirstParentGlobalSRTReq(dstmat, srcbone->GetParent());
//	}
//
//}

//int CBone::SetLimitedLocalEul(int srcmotid, double srcframe, ChaVector3 srceul)
//{
//	double roundingframe = RoundingTime(srcframe);
//
//	CMotionPoint* curmp;
//	curmp = GetMotionPoint(srcmotid, roundingframe);
//	if (curmp) {
//		curmp->SetLimitedLocalEul(srceul);
//	}
//	else {
//		_ASSERT(0);
//	}
//	return 0;
//}

ChaVector3 CBone::GetLimitedLocalEul(int srcmotid, double srcframe)
{
	ChaVector3 reteul = ChaVector3(0.0f, 0.0f, 0.0f);

	//2023/04/28
	if (IsNotSkeleton()) {
		return reteul;
	}


	double roundingframe = RoundingTime(srcframe);

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		reteul = curmp->GetLimitedLocalEul();
		return reteul;
	}
	else {
		_ASSERT(0);
		return reteul;
	}
}

ChaVector3 CBone::GetUnlimitedLocalEul(int srcmotid, double srcframe)
{
	ChaVector3 reteul = ChaVector3(0.0f, 0.0f, 0.0f);

	//2023/04/28
	if (IsNotSkeleton()) {
		return reteul;
	}


	double roundingframe = RoundingTime(srcframe);

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		reteul = curmp->GetLocalEul();
		return reteul;
	}
	else {
		_ASSERT(0);
		return reteul;
	}
}

//ChaVector3 CBone::CalcLocalEulAndSetLimitedEul(int srcmotid, double srcframe)
//{
//	double roundingframe = RoundingTime(srcframe);
//
//	ChaVector3 orgeul = ChaVector3(0.0f, 0.0f, 0.0f);
//	ChaVector3 neweul = ChaVector3(0.0f, 0.0f, 0.0f);
//
//	orgeul = CalcLocalEulXYZ(-1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
//	int ismovable;
//	if ((g_limitdegflag == true) && (GetBtForce() == 0)) {//2023/01/28 物理シミュは　自前では制限しない
//		ismovable = ChkMovableEul(orgeul);
//	}
//	else {
//		ismovable = 1;
//	}
//	if (ismovable == 1) {
//		
//		//可動判定
//		
//		neweul = orgeul;
//	}
//	else {
//		
//		//不動判定
//
//		if (g_wallscrapingikflag == 1) {
//			//可動な軸の数値だけ動かす場合
//			neweul = LimitEul(orgeul, orgeul);//壁すりIK用
//		}
//		else {
//			//どの軸の数値も動かさない場合
//				//角度制限ありの場合の場合しか　ここを通らない
//
//
//			//2023/01/29 制限付けの場合分け
//
//			ChaVector3 befeul = GetLocalEul(srcmotid, 0.0, 0);
//
//			if ((roundingframe >= (0.0 - 0.0001)) && (roundingframe < (0.0 + 0.0001))) {
//				//0frame
//					//0frameはいじらない
//				neweul = GetLocalEul(srcmotid, 0.0, 0);
//			}
//			else if ((roundingframe >= (1.0 - 0.0001)) && (roundingframe < (1.0 + 0.0001))) {
//				//1frame
//				neweul = LimitEul(GetLocalEul(srcmotid, 1.0, 0), GetLocalEul(srcmotid, 1.0, 0));//１フレーム姿勢の壁すり
//			}
//			else {
//				double befframe;
//				befframe = roundingframe - 1.0;
//				//befeul = GetLimitedLocalEul(srcmotid, befframe);//2023/01/28　１フレーム前は計算されていると仮定
//				befeul = LimitEul(GetLocalEul(srcmotid, befframe, 0), GetLocalEul(srcmotid, befframe, 0));
//
//				if (g_underIKRot == false) {
//					neweul = LimitEul(befeul, GetLocalEul(srcmotid, befframe, 0));//壁すりIK用
//				}
//				else {
//					//IK中でg_wallscrapingikflag == 0の場合には　動かさない
//					//可動姿勢を探す
//					neweul = LimitEul(GetLocalEul(srcmotid, 1.0, 0), GetLocalEul(srcmotid, 1.0, 0));
//					while (befframe >= (2.0 - 0.0001)) {
//						befeul = GetLocalEul(srcmotid, befframe, 0);
//						if (ChkMovableEul(befeul) == 1) {
//							neweul = befeul;
//							break;
//						}
//						befframe -= 1.0;
//					}
//				}
//			}
//		}
//	}
//
//	SetLocalEul(srcmotid, roundingframe, neweul, 0);//!!!!!!!!!!!!
//	
//
//	return neweul;
//}

//ChaMatrix CBone::CalcLimitedWorldMat(int srcmotid, double srcframe, ChaVector3* dstneweul)
//{
//
//	double roundingframe = RoundingTime(srcframe);
//	ChaMatrix retmat;
//	retmat.SetIdentity();
//	ChaVector3 neweul = ChaVector3(0.0f, 0.0f, 0.0f);
//
//	if (!dstneweul) {
//		_ASSERT(0);
//		return retmat;
//	}
//
//	CMotionPoint* curmp;
//	curmp = GetMotionPoint(srcmotid, roundingframe);
//	if (!curmp) {
//		_ASSERT(0);
//		*dstneweul = neweul;
//		return retmat;
//	}
//
//
//
//	if (GetBtForce() == 0) {
//		neweul = CalcLocalEulAndSetLimitedEul(srcmotid, roundingframe);
//		retmat = CalcWorldMatFromEul(0, 1, neweul, srcmotid, roundingframe, 0);
//	}
//	else {
//		//2023/01/28
//		//btシミュボーンの場合
//		//物理のシミュにおける制限角度は　クランプではなく　行きすぎたら戻るやり方
//		//物理シミュをRecord機能でベイクしたモーションに対して　クランプすると　動きがカクカクになる
//		//よって　物理シミュボーンに対しては　自前では角度を制限しないことにする
//
//		//2023/01/29
//		//ただし　kinematicの親が角度制限を受けるので　それを反映しなくてはならない
//		ChaMatrix curwm;
//		curwm = curmp->GetWorldMat();//unlimited!!!!!
//		ChaMatrix curparentwm, curlimitedparentwm;
//		if (GetParent()) {
//			CMotionPoint* parentmp = GetParent()->GetMotionPoint(srcmotid, roundingframe);
//			if (parentmp) {
//				curparentwm = parentmp->GetWorldMat();
//				curlimitedparentwm = parentmp->GetLimitedWM();
//			}
//			else {
//				curparentwm.SetIdentity();
//				curlimitedparentwm.SetIdentity();
//			}
//		}
//		else {
//			curparentwm.SetIdentity();
//			curlimitedparentwm.SetIdentity();
//		}
//
//		ChaMatrix curlocalmat = curwm * ChaMatrixInv(curparentwm);
//		retmat = curlocalmat * curlimitedparentwm;
//		CQuaternion eulq;
//		eulq.RotationMatrix(curlocalmat);
//
//		ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//		double befframe;
//		befframe = roundingframe - 1.0;
//		if (befframe >= -0.0001) {
//			CMotionPoint* befmp;
//			befmp = GetMotionPoint(srcmotid, roundingframe);
//			if (befmp) {
//				befeul = befmp->GetLocalEul();//unlimited!!!!!
//			}
//		}
//
//		CQuaternion axisq;
//		axisq.RotationMatrix(GetNodeMat());
//		int isfirstbone = 0;
//		int isendbone = 0;
//		int notmodify180flag = 1;
//		if (g_underIKRot == false) {
//			if (roundingframe <= 1.01) {
//				//0フレームと１フレームは　180度ずれチェックをしない
//				notmodify180flag = 1;
//			}
//			else {
//				notmodify180flag = 0;
//			}
//		}
//		else {
//			//2023/01/26
//			//IKRot中は　０フレームも１フレームも　180度チェックをする
//			notmodify180flag = 0;
//			if (roundingframe <= 1.01) {
//				befeul = ChaVector3(0.0f, 0.0f, 0.0f);
//			}
//		}
//		eulq.Q2EulXYZusingQ(&axisq, befeul, &neweul, isfirstbone, isendbone, notmodify180flag);
//
//	}
//
//	*dstneweul = neweul;
//	return retmat;
//
//}

//ChaMatrix CBone::GetLimitedWorldMat(int srcmotid, double srcframe, ChaVector3* dstneweul, int callingstate)//default : dstneweul = 0, default : callingstate = 0
//{
//
//	//###########################
//	//時間に関する姿勢の補間無し
//	//###########################
//
//
//	//callingstate : 0->fullcalc, 1->bythread only current calc, 2->after thread, use result by threading calc, 3->get calclated parents wm
//
//
//	//###################################################################################################################
//	//この関数はオイラーグラフ用またはキー位置のモーションポイントに対しての処理用なので、時間に対する補間処理は必要ない
//	//###################################################################################################################
//
//	double roundingframe = RoundingTime(srcframe);
//
//	ChaMatrix retmat;
//	ChaMatrixIdentity(&retmat);
//	if ((srcmotid <= 0) || (srcmotid > m_motionkey.size())) {
//		return retmat;
//	}
//
//	if (g_limitdegflag == true) {
//		//制限角度有り
//
//		ChaVector3 neweul;
//
//		CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
//		if (curmp) {
//			if (curmp->GetCalcLimitedWM() == 2) {
//				//計算済の場合 物理では無い場合
//				//GetCalcLimitedWM() : 前処理済１，後処理済２
//				retmat = curmp->GetLimitedWM();
//				if (dstneweul) {
//					//*dstneweul = curmp->GetLocalEul();
//					*dstneweul = curmp->GetLimitedLocalEul();//2023/01/29 limited !!!
//				}
//			}
//			else {
//				//計算済で無い場合
//				if (callingstate == 1) {
//					//前処理が出来てない(ボーン構造順に呼び出していないのでLocal計算が出来ない)ので何もしない。
//					//callingstate = 2での呼び出しに任せる。
//
//					//前処理をするかもしれないが　現状：curmp->SetCalcLimitedWM(1);だけする。
//					curmp->SetCalcLimitedWM(1);//前処理済のフラグ。後処理済は２
//				}
//				else {
//					//未計算の場合
//					retmat = CalcLimitedWorldMat(srcmotid, roundingframe, &neweul);
//
//					curmp->SetLimitedWM(retmat);
//					curmp->SetLimitedLocalEul(neweul);//2023/01/28
//					curmp->SetCalcLimitedWM(2);//処理済
//
//					if (dstneweul) {
//						*dstneweul = neweul;
//					}
//				}
//
//				//else if (callingstate == 3) {
//				//	//2022/08/22
//				//	//物理時　計算済のparent(この関数内においてはcurrent)のwmを参照　不具合エフェクト：指の先が遅れて動く
//				//	if (GetChild()) {
//				//		retmat = GetBtMat();
//				//	}
//				//	else {
//				//		//endjoint対策　この対策をしない場合、指の先が遅れて動く
//				//		if (GetParent()) {
//				//			retmat = GetParent()->GetBtMat();
//				//		}
//				//		else {
//				//			retmat = GetBtMat();
//				//		}
//				//	}
//				//	
//				//}
//				//else {
//				//	ChaMatrixIdentity(&retmat);
//				//}
//			}
//		}
//	}
//	else {
//		//制限角度無し
//		//CMotionPoint calcmp;
//		//int existflag = 0;
//		//CallF(CalcFBXMotion(srcmotid, roundingframe, &calcmp, &existflag), return retmat);//コメントにあるように時間に関する補間は必要ないのでコメントアウト2023/01/14
//		//retmat = calcmp.GetWorldMat();// **wmat;
//
//		CMotionPoint* curmp = GetMotionPoint(srcmotid, roundingframe);
//		if (curmp) {
//			retmat = curmp->GetWorldMat();
//		}
//		else {
//			retmat.SetIdentity();
//		}
//
//		if (dstneweul) {
//			ChaVector3 cureul = CalcLocalEulXYZ(-1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
//			*dstneweul = cureul;
//		}
//	}
//
//
//	return retmat;
//}

//ChaMatrix CBone::GetCurrentLimitedWorldMat()
//{
//	ChaMatrix retmat;
//	ChaMatrixIdentity(&retmat);
//
//	if (!GetParModel()) {
//		return retmat;
//	}
//
//	int srcmotid;
//	double srcframe;
//	MOTINFO* curmi;
//	curmi = GetParModel()->GetCurMotInfo();
//	if (!curmi) {
//		return retmat;
//	}
//	srcmotid = curmi->motid;
//	srcframe = curmi->curframe;
//
//	if ((srcmotid <= 0) || (srcmotid > m_motionkey.size())) {
//		return retmat;
//	}
//
//	if (g_limitdegflag == true) {
//		//制限角度有り
//
////####################################################################
////滑らかにするために、後でsrcframeと+1で２セット計算して補間計算するかもしれない
////####################################################################
//		//ChaVector3 neweul;
//		//neweul = CalcLocalEulAndSetLimitedEul(srcmotid, (double)((int)(srcframe + 0.1)));
//		//retmat = CalcWorldMatFromEul(0, 1, neweul, srcmotid, (double)((int)(srcframe + 0.1)), 0);
//
////#####################################
////滑らかにするために　フレーム補間
////#####################################
//		GetCalclatedLimitedWM(srcmotid, srcframe, &retmat);
//
//	}
//	else {
//		//制限角度無し
//		CMotionPoint calcmp;
//		int existflag = 0;
//		CallF(CalcFBXMotion(srcmotid, srcframe, &calcmp, &existflag), return retmat);
//		retmat = calcmp.GetWorldMat();// **wmat;
//	}
//
//
//	return retmat;
//}


ChaVector3 CBone::GetWorldPos(bool limitdegflag, int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);


	ChaVector3 retpos = ChaVector3(0.0f, 0.0f, 0.0f);
	if ((srcmotid <= 0) || (srcmotid > m_motionkey.size())) {
		return retpos;
	}

	//2023/04/28
	if (IsNotSkeleton()) {
		return retpos;
	}


	ChaVector3 jointpos;
	jointpos = GetJointFPos();
	ChaMatrix newworldmat = GetWorldMat(limitdegflag, srcmotid, srcframe, 0);


	////2022/01/07 マニピュレータをキャラクター位置に出すため
	if (GetParModel()) {
		ChaMatrix wm = GetParModel()->GetWorldMat();//(親ボーンではなく)modelのworld
		newworldmat = newworldmat * wm;
	}

	ChaVector3TransformCoord(&retpos, &jointpos, &newworldmat);

	return retpos;
}


void CBone::SetCurMotID(int srcmotid)
{
	m_curmotid = srcmotid;
};


int CBone::CreateIndexedMotionPoint(int srcmotid, double animleng)
{
	//###############################################
	//2022/11/01 AddMotionPointAll内で行うように変更
	//###############################################

	//2023/04/28
	if (IsNotSkeleton() && IsNotCamera() && IsNotNull()) {//2023/05/23  2023/06/29
		return 0;
	}


	if ((srcmotid <= 0) || (srcmotid > m_motionkey.size())) {
		//_ASSERT(0);
		return 1;
	}
	if (animleng < 1.0) {
		_ASSERT(0);
		return 1;
	}



	std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap;
	itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
	if (itrvecmpmap == m_indexedmotionpoint.end()) {
		std::vector<CMotionPoint*> newvecmp;
		m_indexedmotionpoint[srcmotid] = newvecmp;//STL 参照されていれば無くならない？？？

		std::map<int, vector<CMotionPoint*>>::iterator itrvecmpmap2;
		itrvecmpmap2 = m_indexedmotionpoint.find(srcmotid);
		if (itrvecmpmap2 == m_indexedmotionpoint.end()) {
			_ASSERT(0);
			return 1;
		}

		itrvecmpmap = itrvecmpmap2;
	}

	(itrvecmpmap->second).clear();//!!!!!!!!!!!!!!!
	

	CMotionPoint* curmp = m_motionkey[srcmotid - 1];
	if (curmp) {
		double mpframe;// = curmp->GetFrame();

		double frameno;
		for (frameno = 0.0; frameno < animleng; frameno += 1.0) {
			if (curmp) {
				mpframe = curmp->GetFrame();

				if ((mpframe >= 0.0) && (mpframe < animleng) &&
					IsJustEqualTime(mpframe, frameno)) {
					(itrvecmpmap->second).push_back(curmp);
				}
				else {
					//for safety
					(itrvecmpmap->second).push_back(&m_dummymp);
				}
				curmp = curmp->GetNext();
			}
			else {
				(itrvecmpmap->second).push_back(&m_dummymp);
			}
		}
	}
	else {
		return 0;
	}


	m_initindexedmotionpoint[srcmotid] = true;

	return 0;


}

int CBone::ResetAngleLimit(int srcval)
{
	int newlimit;
	//newlimit = min(180, srcval);
	//newlimit = max(0, newlimit);
	newlimit = srcval;


	m_anglelimit.lower[0] = -newlimit;
	m_anglelimit.lower[1] = -newlimit;
	m_anglelimit.lower[2] = -newlimit;

	m_anglelimit.upper[0] = newlimit;
	m_anglelimit.upper[1] = newlimit;
	m_anglelimit.upper[2] = newlimit;

	return 0;
}

int CBone::AngleLimitReplace180to170()
{

	//+180, -180だけ170, -170に変換

	if (m_anglelimit.lower[0] == -180) {
		m_anglelimit.lower[0] = -170;
	}
	if (m_anglelimit.lower[1] == -180) {
		m_anglelimit.lower[1] = -170;
	}
	if (m_anglelimit.lower[2] == -180) {
		m_anglelimit.lower[2] = -170;
	}


	if (m_anglelimit.upper[0] == 180) {
		m_anglelimit.upper[0] = 170;
	}
	if (m_anglelimit.upper[1] == 180) {
		m_anglelimit.upper[1] = 170;
	}
	if (m_anglelimit.upper[2] == 180) {
		m_anglelimit.upper[2] = 170;
	}

	return 0;
}

int CBone::AdditiveCurrentToAngleLimit()
{
	//#########################################################################################################################
	//2022/12/17
	//カレントフレームだけの計算だと　0リセットの影響や制限の影響のあるbefeulを参照するので　オーバー180の計算がうまくいかない
	//よって全フレームのオイラー角を計算して　最大値と最小値を求めて　それを制限としてセットする
	//#########################################################################################################################

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	if (m_parmodel) {
		MOTINFO* curmi = m_parmodel->GetCurMotInfo();
		if (curmi) {
			int curmotid = curmi->motid;
			double frameleng = curmi->frameleng;
			double curframe;

			ChaVector3 calceul;
			float cureul[3];
			float maxeul[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };//必ず更新されるようにMIN(-MAX)
			float mineul[3] = { FLT_MAX, FLT_MAX, FLT_MAX };//必ず更新されるようにMAX
			for (curframe = 1.0; curframe < frameleng; curframe += 1.0) {
				//calceul = CalcLocalEulXYZ(-1, curmotid, curframe, BEFEUL_BEFFRAME, 0);

				//2023/01/28
				//currentもparentも　制限角度無しで計算する必要有
				//calceul = GetUnlimitedLocalEul(curmotid, curframe);
				bool limitdegflag = false;
				calceul = GetLocalEul(limitdegflag, curmotid, curframe, 0);
				//calceul = CalcLocalEulXYZ(limitdegflag, -1, curmotid, curframe, BEFEUL_BEFFRAME);

				cureul[0] = calceul.x;
				cureul[1] = calceul.y;
				cureul[2] = calceul.z;

				int axiskind;
				for (axiskind = 0; axiskind < 3; axiskind++) {
					if (cureul[axiskind] > maxeul[axiskind]) {
						maxeul[axiskind] = cureul[axiskind];
					}
					if (cureul[axiskind] < mineul[axiskind]) {
						mineul[axiskind] = cureul[axiskind];
					}
				}
			}

			int axiskind2;
			for (axiskind2 = 0; axiskind2 < 3; axiskind2++) {
				//m_anglelimit.upper[axiskind2] = (int)(maxeul[axiskind2] + 0.0001f);
				//m_anglelimit.lower[axiskind2] = (int)(mineul[axiskind2] + 0.0001f);

				//2023/01/28
				//実角度ギリギリをintに丸めて制限をかけると　可動部分が制限に引っ掛かることがあったので対応
				float tempmax, tempmin;
				tempmax = maxeul[axiskind2] + 2.0f;//正か負かによらずmaxに足すのが正しい
				tempmin = mineul[axiskind2] - 2.0f;//正か負かによらずminから引くのが正しい
				//
				//丸めには正負が関係する
				if (tempmax > 0.0f) {
					m_anglelimit.upper[axiskind2] = (int)(tempmax + 0.0001f);
				}
				else {
					m_anglelimit.upper[axiskind2] = (int)(tempmax - 0.0001f);
				}
				if (tempmin > 0.0f) {
					m_anglelimit.lower[axiskind2] = (int)(tempmin + 0.0001f);
				}
				else {
					m_anglelimit.lower[axiskind2] = (int)(tempmin - 0.0001f);
				}
			}

		}
	}


	//if (m_parmodel) {
	//	MOTINFO* curmi = m_parmodel->GetCurMotInfo();
	//	if (curmi) {
	//		int curmotid = curmi->motid;
	//		int curframe = curmi->curframe;
	//		ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
	//		cureul = CalcLocalEulXYZ(-1, m_curmotid, curframe, BEFEUL_BEFFRAME, 0);
	//		AdditiveToAngleLimit(cureul);
	//	}
	//	else {
	//		_ASSERT(0);
	//	}
	//}
	//else {
	//	_ASSERT(0);
	//}


	return 0;
}

int CBone::AdditiveAllMotionsToAngleLimit()
{

	//2023/04/28
	if (IsNotSkeleton()) {
		return 0;
	}


	if (m_parmodel) {

		float maxeul[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };//必ず更新されるようにMIN(-MAX)
		float mineul[3] = { FLT_MAX, FLT_MAX, FLT_MAX };//必ず更新されるようにMAX

		int motionnum = m_parmodel->GetMotInfoSize();
		if (motionnum >= 1) {
			std::map<int, MOTINFO*>::iterator itrmi;
			for (itrmi = m_parmodel->GetMotInfoBegin(); itrmi != m_parmodel->GetMotInfoEnd(); itrmi++) {
				MOTINFO* curmi = itrmi->second;
				if (curmi) {
					int curmotid = curmi->motid;
					double frameleng = curmi->frameleng;
					double curframe;

					ChaVector3 calceul;
					float cureul[3];

					for (curframe = 1.0; curframe < frameleng; curframe += 1.0) {
						//calceul = CalcLocalEulXYZ(-1, curmotid, curframe, BEFEUL_BEFFRAME, 0);

						//2023/01/28
						//currentもparentも　制限角度無しで計算する必要有
						//calceul = GetUnlimitedLocalEul(curmotid, curframe);
						bool limitdegflag = false;
						calceul = GetLocalEul(limitdegflag, curmotid, curframe, 0);
						//calceul = CalcLocalEulXYZ(limitdegflag, -1, curmotid, curframe, BEFEUL_BEFFRAME);


						cureul[0] = calceul.x;
						cureul[1] = calceul.y;
						cureul[2] = calceul.z;

						int axiskind;
						for (axiskind = 0; axiskind < 3; axiskind++) {
							if (cureul[axiskind] > maxeul[axiskind]) {
								maxeul[axiskind] = cureul[axiskind];
							}
							if (cureul[axiskind] < mineul[axiskind]) {
								mineul[axiskind] = cureul[axiskind];
							}
						}
					}

				}
			}

			int axiskind2;
			for (axiskind2 = 0; axiskind2 < 3; axiskind2++) {
				//m_anglelimit.upper[axiskind2] = (int)(maxeul[axiskind2] + 0.0001f);
				//m_anglelimit.lower[axiskind2] = (int)(mineul[axiskind2] + 0.0001f);

				//2023/01/28
				//実角度ギリギリをintに丸めて制限をかけると　可動部分が制限に引っ掛かることがあったので対応
				float tempmax, tempmin;
				tempmax = maxeul[axiskind2] + 2.0f;//正か負かによらずmaxに足すのが正しい
				tempmin = mineul[axiskind2] - 2.0f;//正か負かによらずminから引くのが正しい
				//
				//丸めには正負が関係する
				if (tempmax > 0.0f) {
					m_anglelimit.upper[axiskind2] = (int)(tempmax + 0.0001f);
				}
				else {
					m_anglelimit.upper[axiskind2] = (int)(tempmax - 0.0001f);
				}
				if (tempmin > 0.0f) {
					m_anglelimit.lower[axiskind2] = (int)(tempmin + 0.0001f);
				}
				else {
					m_anglelimit.lower[axiskind2] = (int)(tempmin - 0.0001f);
				}
			}
		}
	}


	//if (m_parmodel) {
	//	MOTINFO* curmi = m_parmodel->GetCurMotInfo();
	//	if (curmi) {
	//		int curmotid = curmi->motid;
	//		int curframe = curmi->curframe;
	//		ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
	//		cureul = CalcLocalEulXYZ(-1, m_curmotid, curframe, BEFEUL_BEFFRAME, 0);
	//		AdditiveToAngleLimit(cureul);
	//	}
	//	else {
	//		_ASSERT(0);
	//	}
	//}
	//else {
	//	_ASSERT(0);
	//}


	return 0;
}


//int CBone::AdditiveToAngleLimit(ChaVector3 cureul)
//{
//
//	if ( m_anglelimit.lower[AXIS_X] > (int)cureul.x) {
//		m_anglelimit.lower[AXIS_X] = (int)cureul.x;
//	}
//	if (m_anglelimit.lower[AXIS_Y] > (int)cureul.y) {
//		m_anglelimit.lower[AXIS_Y] = (int)cureul.y;
//	}
//	if (m_anglelimit.lower[AXIS_Z] > (int)cureul.z) {
//		m_anglelimit.lower[AXIS_Z] = (int)cureul.z;
//	}
//
//	if ((int)cureul.x > m_anglelimit.upper[AXIS_X]) {
//		m_anglelimit.upper[AXIS_X] = (int)cureul.x;
//	}
//	if ((int)cureul.y > m_anglelimit.upper[AXIS_Y]) {
//		m_anglelimit.upper[AXIS_Y] = (int)cureul.y;
//	}
//	if ((int)cureul.z > m_anglelimit.upper[AXIS_Z]) {
//		m_anglelimit.upper[AXIS_Z] = (int)cureul.z;
//	}
//
//
//	//#################################################################################################
//	//2022/12/06
//	//floatの角度にintの制限をかける際　差分が遊びより小さい場合に　モーションがぶるぶるすることがある
//	//遊びよりも大きな値分　絶対値の内側に折り込む
//	//#################################################################################################
//	//if ( (m_anglelimit.lower[0] - (int)cureul.x) > EULLIMITPLAY) {
//	//	m_anglelimit.lower[0] = cureul.x + EULLIMITPLAY;
//	//}
//	//if ((m_anglelimit.lower[1] - (int)cureul.y) > EULLIMITPLAY) {
//	//	m_anglelimit.lower[1] = cureul.y + EULLIMITPLAY;
//	//}
//	//if ((m_anglelimit.lower[2] - (int)cureul.z) > EULLIMITPLAY) {
//	//	m_anglelimit.lower[2] = cureul.z + EULLIMITPLAY;
//	//}
//	//if (((int)cureul.x - m_anglelimit.upper[0]) > EULLIMITPLAY) {
//	//	m_anglelimit.upper[0] = cureul.x - EULLIMITPLAY;
//	//}
//	//if (((int)cureul.y - m_anglelimit.upper[1]) > EULLIMITPLAY) {
//	//	m_anglelimit.upper[1] = cureul.y - EULLIMITPLAY;
//	//}
//	//if (((int)cureul.z - m_anglelimit.upper[2]) > EULLIMITPLAY) {
//	//	m_anglelimit.upper[2] = cureul.z - EULLIMITPLAY;
//	//}
//
//
//	SwapAngleLimitUpperLowerIfRev();
//
//
//
//	//#######################################################################################
//	//2022/12/06 Comment out
//	//floatの角度にintの制限をかける際　差分が遊びより小さい場合に　モーションがぶるぶるする 
//	//#######################################################################################
//	//if ((int)cureul.x < m_anglelimit.lower[0]) {
//	//	m_anglelimit.lower[0] = cureul.x;
//	//}
//	//if ((int)cureul.y < m_anglelimit.lower[1]) {
//	//	m_anglelimit.lower[1] = cureul.y;
//	//}
//	//if ((int)cureul.z < m_anglelimit.lower[2]) {
//	//	m_anglelimit.lower[2] = cureul.z;
//	//}
//
//	//if ((int)cureul.x > m_anglelimit.upper[0]) {
//	//	m_anglelimit.upper[0] = cureul.x;
//	//}
//	//if ((int)cureul.y > m_anglelimit.upper[1]) {
//	//	m_anglelimit.upper[1] = cureul.y;
//	//}
//	//if ((int)cureul.z > m_anglelimit.upper[2]) {
//	//	m_anglelimit.upper[2] = cureul.z;
//	//}
//
//	return 0;
//}

int CBone::GetFBXAnim(FbxNode* pNode, int animno, int motid, double animleng, bool callingbythread)
{
	if (!pNode) {
		_ASSERT(0);
		return 1;
	}

	//char nodename[256] = { 0 };
	//strcpy_s(nodename, 256, pNode->GetName());
	//if ((strstr(nodename, "camera") != 0) || (strstr(nodename, "Camera") != 0)) {
	//	if (GetParModel() && GetParModel()->IsCameraMotion(motid)) {
	//		int dbgflag = 1;
	//	}
	//}


	if (GetGetAnimFlag() == 0) {
		SetGetAnimFlag(1);
	}
	if (!GetParModel()) {
		_ASSERT(0);
		return 1;
	}

	//2023/04/28 2023/05/23
	//if (IsNotSkeleton() && IsNotCamera() && IsNotNull()) {
	if (IsNotSkeleton() && IsNotCamera()) {
		return 0;
	}


	//if (IsNull()) {
	//	char motionname[256] = { 0 };
	//	GetParModel()->GetMotionName(motid, 256, motionname);
	//	if (strcmp(GetBoneName(), motionname) != 0) {
	//		return 0;
	//	}
	//}


	FbxTime fbxtime;
	fbxtime.SetSecondDouble(0.0);
	FbxTime difftime;
	difftime.SetSecondDouble(1.0 / 30);
	double framecnt;
	//for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
	//for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {//関数呼び出し時にanimleng - 1している


	FbxAMatrix correctscalemat;
	correctscalemat.SetIdentity();
	FbxAMatrix currentmat;
	currentmat.SetIdentity();
	FbxAMatrix parentmat;
	parentmat.SetIdentity();
	//const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eDestinationPivot);
	//const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eDestinationPivot);
	//const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eDestinationPivot);
	//const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot, true, true);
	//const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
	//const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot, true, true);
	//FbxAMatrix lSRT = pNode->EvaluateLocalTransform(fbxtime, FbxNode::eSourcePivot, true, true);
	//FbxAMatrix lGlobalSRT = pNode->EvaluateGlobalTransform(fbxtime, FbxNode::eSourcePivot, true, true);

	if (pNode) {
		//for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
		for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {//2022/10/21 : 最終フレームにモーションポイントが無い問題対応

			FbxAMatrix lGlobalSRT;

			//#####  2022/11/01  ################################################################################################
			//サブスレッド１つだけで計算することにした(CriticalSection回数が多すぎて遅くなる)ので　CriticalSectionコメントアウト
			//スレッド数(LOADFBXANIMTHREAD)を１以外にする場合には　CriticalSection必須
			//###################################################################################################################
			//EnterCriticalSection(&(GetParModel()->m_CritSection_Node));//#######################
			lGlobalSRT = pNode->EvaluateGlobalTransform(fbxtime, FbxNode::eSourcePivot, true, true);
			//LeaveCriticalSection(&(GetParModel()->m_CritSection_Node));//#######################

			ChaMatrix chaGlobalSRT;
			chaGlobalSRT = ChaMatrixFromFbxAMatrix(lGlobalSRT);

			////##############
			////Add MotionPoint
			////##############
			ChaMatrix localmat;
			ChaMatrixIdentity(&localmat);
			ChaMatrix globalmat;
			ChaMatrixIdentity(&globalmat);

			CMotionPoint* curmp = 0;
			int existflag = 0;
			//curmp = curbone->AddMotionPoint(motid, framecnt, &existflag);
			curmp = GetMotionPoint(motid, framecnt);
			if (!curmp) {
				//_ASSERT(0);
				//return 1;
				curmp = AddMotionPoint(motid, framecnt, &existflag);
				if (!curmp) {
					_ASSERT(0);
					return 1;
				}
			}

			//###############
			//calc globalmat
			//###############


			//FbxAnimLayer* panimlayer = GetParModel()->GetCurrentAnimLayer();
			//if (panimlayer) {
			//	const char* strChannel;
			//	strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			//	FbxAnimCurve* lCurve;
			//	bool createflag = false;
			//	lCurve = pNode->LclTranslation.GetCurve(panimlayer, strChannel, createflag);
			//	//if ((IsNull()) && (!lCurve)) {
			//	if (IsNull()) {//Curveの有無に関係なくNullの場合
			//		if (GetChild()) {
			//			//###################################################################
			//			//transform nodeのnull
			//			//NodeMatに姿勢はセットされているが　アニメーションとしてはIdentity
			//			//###################################################################
			//			globalmat.SetIdentity();
			//		}
			//		else {
			//			//######################
			//			//endjointのnull
			//			//######################
			//			globalmat = (ChaMatrixInv(GetNodeMat()) * chaGlobalSRT);
			//		}
			//		
			//	}
			//	else if (lCurve) {
			//		globalmat = (ChaMatrixInv(GetNodeMat()) * chaGlobalSRT);
			//	}
			//	else {
			//		//カーブが無い場合
			//		if (GetParModel()->GetMqoObjectSize() >= 1) {
			//			//mesh(pointbuf * meshmat)位置そのまま
			//			//UnityでAssetをfbx出力する際に
			//			//SkinMesh + Animationを選んだのに
			//			//アニメーションコントローラを対応付けないで出力した場合を
			//			//カーブの有無(無)とメッシュの有無(有)で検出
			//			//その場合
			//			//そのままでは　モーションが合わず傾いたりするので　Identityで初期化する
			//			
			//			globalmat.SetIdentity();			
			//		}
			//		else {
			//			//カーブを持たないモーションだけのfbxは　ここを通る
			//			globalmat = (ChaMatrixInv(GetNodeMat()) * chaGlobalSRT);
			//		}
			//	}
			//}
			//else {
			//	globalmat.SetIdentity();
			//}
			//
			
			//2023/05/07
			//eNullにアニメーションは無いので　上方で(eSkeleton || eCamera)以外はリターンしている
			//いろいろ直した結果　lCurveが0の場合にも　同じ数式でOKに
			globalmat = (ChaMatrixInv(GetNodeMat()) * chaGlobalSRT);

#ifndef NDEBUG
			//for debug
			if ((animno == 0) && (framecnt == 0.0)) {
				char strdbg[1024] = { 0 };
				WCHAR wstrdbg[1024] = { 0L };
				sprintf_s(strdbg, 1024, "AnimMat firstanim firstframe : (%s)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n",
					GetBoneName(),
					globalmat.data[MATI_11], globalmat.data[MATI_12], globalmat.data[MATI_13], globalmat.data[MATI_14],
					globalmat.data[MATI_21], globalmat.data[MATI_22], globalmat.data[MATI_23], globalmat.data[MATI_24],
					globalmat.data[MATI_31], globalmat.data[MATI_32], globalmat.data[MATI_33], globalmat.data[MATI_34],
					globalmat.data[MATI_41], globalmat.data[MATI_42], globalmat.data[MATI_43], globalmat.data[MATI_44]
				);
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strdbg, 1024, wstrdbg, 1024);
				DbgOut(wstrdbg);
			}
#endif

			curmp->SetWorldMat(globalmat);//anglelimit無し
			curmp->SetLimitedWM(globalmat);//初期値はそのまま

			//##############################################################################
			//SetCalcLimitedWM(2)は　CModel::PostLoadFbxで　LimitedLocalEulをセットしてから
			//##############################################################################


			//##########
			//FirstMot
			//##########
			if ((animno == 0) && (framecnt == 0.0)) {
				//SetFirstMat(globalmat);
				SetFirstMat(chaGlobalSRT);
			}


			fbxtime = fbxtime + difftime;
		}
	}

	//Sleep(0);

	//#####################################################################################################
	//念のために　ジョイントの向きを強制リセットしていたころの　ソースをコメントアウトして残す　2022/10/31
	//#####################################################################################################
	//if ((bvhflag == 0) &&
	//	GetParModel() && GetParModel()->GetHasBindPose()) {
	//
	//	//for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
	//	for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {//2022/10/21 : 最終フレームにモーションポイントが無い問題対応
	//
	//		for (bonecount = 0; bonecount < srcbonenum; bonecount++) {
	//			CBone* curbone = *(bonelist + bonecount);
	//			FbxNode* pNode = *(nodelist + bonecount);
	//			if (curbone && pNode) {
	//				FbxAMatrix lGlobalSRT;
	//
	//				EnterCriticalSection(&(GetParModel()->m_CritSection_Node));//#######################
	//				const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot);
	//				const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot);
	//				const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot);
	//				LeaveCriticalSection(&(GetParModel()->m_CritSection_Node));//#######################
	//
	//				ChaVector3 chatra = ChaVector3((float)lT2[0], (float)lT2[1], (float)lT2[2]);
	//				ChaVector3 chaeul = ChaVector3((float)lR2[0], (float)lR2[1], (float)lR2[2]);
	//				ChaVector3 chascale = ChaVector3((float)lS2[0], (float)lS2[1], (float)lS2[2]);
	//
	//				//####################
	//				//calc joint position
	//				//####################
	//				ChaVector3 jointpos;
	//				jointpos = curbone->GetJointFPos();
	//				ChaVector3 parentjointpos;
	//				if (curbone->GetParent()) {
	//					parentjointpos = curbone->GetParent()->GetJointFPos();
	//				}
	//				else {
	//					parentjointpos = ChaVector3(0.0f, 0.0f, 0.0f);
	//				}
	//
	//				//##############
	//				//calc rotation
	//				//##############
	//				CQuaternion chaq;
	//				chaq.SetRotationXYZ(0, chaeul);
	//				ChaMatrix charotmat;
	//				charotmat = chaq.MakeRotMatX();
	//
	//				ChaMatrix befrotmat, aftrotmat;
	//				ChaMatrixTranslation(&befrotmat, -jointpos.x, -jointpos.y, -jointpos.z);
	//				ChaMatrixTranslation(&aftrotmat, jointpos.x, jointpos.y, jointpos.z);
	//
	//				//#################
	//				//calc translation
	//				//#################
	//				ChaMatrix chatramat;
	//				ChaMatrixIdentity(&chatramat);
	//				ChaMatrixTranslation(&chatramat, chatra.x - jointpos.x + parentjointpos.x, chatra.y - jointpos.y + parentjointpos.y, chatra.z - jointpos.z + parentjointpos.z);
	//
	//				//##############
	//				//calc scalling
	//				//##############
	//				ChaMatrix chascalemat;
	//				ChaMatrixScaling(&chascalemat, chascale.x, chascale.y, chascale.z);
	//
	//				//Set Local frame0
	//				if (framecnt == 0.0) {
	//					curbone->SetLocalR0(chaq);
	//					curbone->SetLocalT0(chatramat);
	//					curbone->SetLocalS0(chascalemat);
	//					//curbone->SetFirstSRT(chaSRT);
	//				}
	//
	//
	//				//##############
	//				//calc localmat
	//				//##############
	//				ChaMatrix localmat;
	//				ChaMatrixIdentity(&localmat);
	//				ChaMatrix globalmat;
	//				ChaMatrixIdentity(&globalmat);
	//
	//				CMotionPoint* curmp = 0;
	//				int existflag = 0;
	//				curmp = curbone->AddMotionPoint(motid, framecnt, &existflag);
	//				if (!curmp) {
	//					_ASSERT(0);
	//					return 1;
	//				}
	//
	//				localmat = befrotmat * chascalemat * charotmat * aftrotmat * chatramat;
	//
	//				//#############
	//				//set localmat
	//				//#############
	//				curmp->SetLocalMat(localmat);//anglelimit無し
	//
	//			}
	//		}
	//		fbxtime = fbxtime + difftime;
	//	}
	//}


	return 0;
}


//int CBone::GetFBXAnim(int bvhflag, CBone** bonelist, FbxNode** nodelist, int srcbonenum, int animno, int motid, double animleng, bool callingbythread)
//{
//
//	//if (curbone && !curbone->GetGetAnimFlag()) {
//	//	curbone->SetGetAnimFlag(1);
//	int bonecount;
//	for (bonecount = 0; bonecount < srcbonenum; bonecount++) {
//		CBone* curbone = *(bonelist + bonecount);
//		if (curbone && !curbone->GetGetAnimFlag()) {
//			curbone->SetGetAnimFlag(1);
//		}
//	}
//
//
//	FbxTime fbxtime;
//	fbxtime.SetSecondDouble(0.0);
//	FbxTime difftime;
//	difftime.SetSecondDouble(1.0 / 30);
//	double framecnt;
//	//for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
//	//for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {//関数呼び出し時にanimleng - 1している
//
//
//	FbxAMatrix correctscalemat;
//	correctscalemat.SetIdentity();
//	FbxAMatrix currentmat;
//	currentmat.SetIdentity();
//	FbxAMatrix parentmat;
//	parentmat.SetIdentity();
//	//const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eDestinationPivot);
//	//const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eDestinationPivot);
//	//const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eDestinationPivot);
//	//const FbxVector4 lT2 = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot, true, true);
//	//const FbxVector4 lR2 = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
//	//const FbxVector4 lS2 = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot, true, true);
//	//FbxAMatrix lSRT = pNode->EvaluateLocalTransform(fbxtime, FbxNode::eSourcePivot, true, true);
//	//FbxAMatrix lGlobalSRT = pNode->EvaluateGlobalTransform(fbxtime, FbxNode::eSourcePivot, true, true);
//
//
//	//2022/11/01 boneloopをtimeloopより外側にして高速化
//	for (bonecount = 0; bonecount < srcbonenum; bonecount++) {
//		CBone* curbone = *(bonelist + bonecount);
//		FbxNode* pNode = *(nodelist + bonecount);
//
//		fbxtime.SetSecondDouble(0.0);
//
//		if (curbone && pNode) {
//			//for (framecnt = 0.0; framecnt < (animleng - 1); framecnt += 1.0) {
//			for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {//2022/10/21 : 最終フレームにモーションポイントが無い問題対応
//
//				FbxAMatrix lGlobalSRT;
//
//				EnterCriticalSection(&(GetParModel()->m_CritSection_Node));//#######################
//				lGlobalSRT = pNode->EvaluateGlobalTransform(fbxtime, FbxNode::eSourcePivot);
//				LeaveCriticalSection(&(GetParModel()->m_CritSection_Node));//#######################
//
//				ChaMatrix chaGlobalSRT;
//				chaGlobalSRT = ChaMatrixFromFbxAMatrix(lGlobalSRT);
//
//				////##############
//				////Add MotionPoint
//				////##############
//				ChaMatrix localmat;
//				ChaMatrixIdentity(&localmat);
//				ChaMatrix globalmat;
//				ChaMatrixIdentity(&globalmat);
//
//				CMotionPoint* curmp = 0;
//				int existflag = 0;
//				//curmp = curbone->AddMotionPoint(motid, framecnt, &existflag);
//				curmp = curbone->GetMotionPoint(motid, framecnt);
//				if (!curmp) {
//					//_ASSERT(0);
//					//return 1;
//					curmp = curbone->AddMotionPoint(motid, framecnt, &existflag);
//					if (!curmp) {
//						_ASSERT(0);
//						return 1;
//					}
//				}
//
//				//###############
//				//calc globalmat
//				//###############
//				globalmat = (ChaMatrixInv(curbone->GetNodeMat()) * chaGlobalSRT);
//				//globalmat = (ChaMatrixInv(curbone->GetNodeMat()) * chaGlobalSRT);
//				curmp->SetWorldMat(globalmat);//anglelimit無し
//
//				//##########
//				//FirstMot
//				//##########
//				if ((animno == 0) && (framecnt == 0.0)) {
//					curbone->SetFirstMat(globalmat);
//				}
//
//
//				fbxtime = fbxtime + difftime;
//			}
//		}
//	}
//
//	Sleep(0);
//
//	return 0;
//}


int CBone::InitMP(bool limitdegflag, int srcmotid, double srcframe)
{
	//###########################################################
	//InitMP 初期姿勢。リターゲットの初期姿勢に関わる。 
	//最初のモーション(firstmotid)の worldmat(firstanim)で初期化
	//###########################################################

	ChaCalcFunc chacalcfunc;
	return chacalcfunc.InitMP(this, limitdegflag, srcmotid, srcframe);
}


bool CBone::IsHipsBone()
{
	if (strlen(GetBoneName()) <= 0) {
		return false;
	}

	const char strpat[20] = "Hips";
	const char* hipsptr = strstr(GetBoneName(), strpat);//strstr
	if (hipsptr) {
		return true;
	}
	else {
		const char strpat2[20] = "Hip";
		if (strcmp(GetBoneName(), strpat2) == 0) {//strcmp
			return true;
		}
		else {
			const char strpat3[20] = "Hip_Joint";
			if (strcmp(GetBoneName(), strpat3) == 0) {//strcmp
				return true;
			}
		}
	}

	return false;
}


//int CBone::SwapCurrentMotionPoint()
//{
//	//m_curmp.CopyMP(&m_calccurmp);
//	return 0;
//}

void CBone::SaveFbxNodePosture(FbxNode* pNode)
{
	//#############################################
	//CModel::CalcMeshMatReq()と内容を同期すること
	//#############################################


	if (pNode) {

		//#########################################################################################
		//2023/06/28
		//Enullノードの子供のCancel2Modeで正常に再生可能なカメラアニメ保存読み込みで検証したところ
		//Lcl*.Get()を保存するとアニメが変質したが　EvaluateLocal*を保存すると変質しなかった
		//#########################################################################################
		FbxTime fbxtime;
		fbxtime.SetSecondDouble(0.0);
		m_fbxLclPos = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot, true, true);//FbxFile.cpp CopyNodePosture()と合わせる
		m_fbxLclRot = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
		m_fbxLclScl = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot, true, true);
		//m_fbxLclPos = pNode->LclTranslation.Get();//2023/05/17
		//m_fbxLclRot = pNode->LclRotation.Get();//2023/05/17
		//m_fbxLclScl = pNode->LclScaling.Get();//2023/05/17


		m_fbxRotOff = pNode->GetRotationOffset(FbxNode::eSourcePivot);
		m_fbxRotPiv = pNode->GetRotationPivot(FbxNode::eSourcePivot);
		m_fbxPreRot = pNode->GetPreRotation(FbxNode::eSourcePivot);
		m_fbxPostRot = pNode->GetPostRotation(FbxNode::eSourcePivot);
		m_fbxSclOff = pNode->GetScalingOffset(FbxNode::eSourcePivot);
		m_fbxSclPiv = pNode->GetScalingPivot(FbxNode::eSourcePivot);
		m_fbxrotationActive = pNode->GetRotationActive();


		pNode->GetRotationOrder(FbxNode::eSourcePivot, m_rotationorder);


		m_InheritType = pNode->InheritType.Get();//2023/06/03

	}
}

//#####################################################################################################################
//現状　CalcNodePostureReq()と変わらず
//#####################################################################################################################
//void CBone::CalcEnullMatReq(double srctime, ChaMatrix* plocalnodemat, ChaMatrix* plocalnodeanimmat)
//{
//	if (!plocalnodemat || !plocalnodeanimmat) {
//		_ASSERT(0);
//		return;
//	}
//	if (!GetFbxNodeOnLoad()) {
//		return;
//	}
//
//	//double enulltime = 0.0;
//	ChaMatrix localnodemat, localnodeanimmat;
//	localnodemat.SetIdentity();
//	localnodeanimmat.SetIdentity();
//	//CalcLocalNodePosture(GetFbxNodeOnLoad(), enulltime, &localnodemat, &localnodeanimmat);
//	CalcLocalNodePosture(GetFbxNodeOnLoad(), srctime, &localnodemat, &localnodeanimmat);//lclrot入りかどうかはGetRotationActive()で判断
//
//	*plocalnodemat = *plocalnodemat * localnodemat;
//	*plocalnodeanimmat = *plocalnodeanimmat * localnodeanimmat;
//
//
//	//parent方向へ計算
//	if (GetParent(false)) {
//		GetParent(false)->CalcEnullMatReq(srctime, plocalnodemat, plocalnodeanimmat);
//	}
//
//}


int CBone::CalcLocalNodePosture(bool bindposeflag, FbxNode* pNode, double srcframe, ChaMatrix* plocalnodemat, ChaMatrix* plocalnodeanimmat)
{

	//#############################################
	//CModel::CalcMeshMatReq()と内容を同期すること
	//#############################################


	if (!plocalnodemat || !plocalnodeanimmat) {
		_ASSERT(0);
		return 0;
	}

	if (!pNode) {
		pNode = m_fbxnodeonload;
	}
	if (!pNode) {
		_ASSERT(0);
		return 0;
	}

	FbxTime fbxtime;
	//fbxtime.SetSecondDouble((double)((int)(srcframe + 0.0001)) / 30.0);
	//2024/01/31 NotRoundingTime
	fbxtime.SetSecondDouble(srcframe / 30.0);


	//2023/07/04
	bool fbxRotationActive = pNode->GetRotationActive();//lclrot入りかどうかはGetRotationActive()で判断


	//#########################################################################################
	//2023/06/28
	//Enullノードの子供のCancel2Modeで正常に再生可能なカメラアニメ保存読み込みで検証したところ
	//Lcl*.Get()を保存するとアニメが変質したが　EvaluateLocal*を保存すると変質しなかった
	//#########################################################################################
	FbxDouble3 fbxLclPos;
	FbxDouble3 fbxLclRot;
	FbxDouble3 fbxLclScl;
	//if (srcframe == 0.0) {
	//	fbxLclPos = pNode->LclTranslation.Get();
	//	fbxLclRot = pNode->LclRotation.Get();
	//	fbxLclScl = pNode->LclScaling.Get();
	//}
	//else {
		fbxLclPos = pNode->EvaluateLocalTranslation(fbxtime, FbxNode::eSourcePivot, true, true);
		fbxLclRot = pNode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
		fbxLclScl = pNode->EvaluateLocalScaling(fbxtime, FbxNode::eSourcePivot, true, true);
	//}



	EFbxRotationOrder rotationorder;
	pNode->GetRotationOrder(FbxNode::eSourcePivot, rotationorder);


	ChaMatrix fbxT, fbxRoff, fbxRp, fbxRpinv, fbxSoff, fbxSp, fbxS, fbxSpinv;
	fbxT.SetIdentity();
	fbxRoff.SetIdentity();
	fbxRp.SetIdentity();
	fbxRpinv.SetIdentity();
	fbxSoff.SetIdentity();
	fbxSp.SetIdentity();
	fbxS.SetIdentity();
	fbxSpinv.SetIdentity();

	fbxT.SetTranslation(ChaVector3(fbxLclPos));//##### at fbxtime
	fbxRoff.SetTranslation(ChaVector3(m_fbxRotOff));
	fbxRp.SetTranslation(ChaVector3(m_fbxRotPiv));


	//######################################################################################################################
	//2023/06/23 use quaternion !!!!
	// 
	// FbxAMatrixの掛け算の順番表記　Rpre * R * Rpost　はミスかと思ったら合っていた
	// rotationorderについても　実際にはXYZなのではと試していたが　GetRotationOrderの結果の通りで合っていた
	// 
	// つまり　ChaMatrixでの掛け算順は　Rpost * R * Rpreとなり　クォータニオンでの掛け算順は　preQ * lclQ * postQとなる
	// TheHuntのCity1シーンのCamera_1, Camera_2のモーションで検証
	// 
	//######################################################################################################################
	CQuaternion rotQ1, rotQ2, preQ, lclQ, postQ;
	//preQ.SetRotation(rotationorder, 0, ChaVector3(m_fbxPreRot));
	//lclQ.SetRotation(rotationorder, 0, ChaVector3(fbxLclRot));//##### at fbxtime
	//postQ.SetRotation(rotationorder, 0, ChaVector3(m_fbxPostRot));


	//##############################################################################################################################
	//2023/06/28
	//Mesh用の変換行列計算(CModel::CalcMeshMatReq())時には　prerot, postrotlclrotはXYZ順　lclrotはrotationorder順でうまくいくようだ
	//		ただし　カメラの子供のメッシュについては　うまくいっていない(parentがeCameraの場合の計算に対応していない)
	//
	//TheHunt Street1 Camera_1の　HUDの位置向き　壁の位置向き　　　TheHunt City1 Camera_1の子供のArmsのなどで検証
	// CBone::CalcLocalNodePosture(), CFbxFile::CopyNodePosture()もそれに合わせる
	// 読み書き読み書き読みで変わらないことを確認
	//##############################################################################################################################
	preQ.SetRotationXYZ(0, ChaVector3(m_fbxPreRot));//
	lclQ.SetRotation(rotationorder, 0, ChaVector3(fbxLclRot));//##### at fbxtime
	postQ.SetRotationXYZ(0, ChaVector3(m_fbxPostRot));//


	rotQ1 = preQ * lclQ * postQ;
	rotQ2 = preQ * postQ;



	fbxRpinv = ChaMatrixInv(fbxRp);
	fbxSoff.SetTranslation(ChaVector3(m_fbxSclOff));
	fbxSp.SetTranslation(ChaVector3(m_fbxSclPiv));
	fbxS.SetScale(ChaVector3(fbxLclScl));//##### at fbxtime
	fbxSpinv = ChaMatrixInv(fbxSp);

	//##################################################################################################################
	// FbxAMatrix Transform = T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp * S * Sp-1
	// 
	// //2023/05/16
	// ただし　FbxAMatrixの＊演算子(左に掛けていく)は　ChaMatrixの*演算子(右に掛けていく)と逆順掛け算　(行列成分は同じ)
	//##################################################################################################################

	ChaMatrix localnodemat, localnodeanimmat;
	//localnodeanimmat = fbxSpinv * fbxS * fbxSp * fbxSoff * fbxRpinv * fbxRpre * fbxR * fbxRpost * fbxRp * fbxRoff * fbxT;//2023/05/17
	localnodeanimmat = fbxSpinv * fbxS * fbxSp * fbxSoff * fbxRpinv * rotQ1.MakeRotMatX() * fbxRp * fbxRoff * fbxT;//2023/06/23
	//localnodeanimmat = fbxT * fbxRoff * fbxRp * rotQ1.MakeRotMatX() * fbxRpinv * fbxSoff * fbxS * fbxSpinv;


	//0フレームアニメ無し : fbxR無し
	//localnodemat = fbxSpinv * fbxS * fbxSp * fbxSoff * fbxRpinv * fbxRpre * fbxRpost * fbxRp * fbxRoff * fbxT;//2023/05/17
	localnodemat = fbxSpinv * fbxS * fbxSp * fbxSoff * fbxRpinv * rotQ2.MakeRotMatX() * fbxRp * fbxRoff * fbxT;
	//localnodemat = fbxT * fbxRoff * fbxRp * rotQ2.MakeRotMatX() * fbxRpinv * fbxSoff * fbxS * fbxSpinv;


	//2023/07/04
	//lclrot入りかどうかは　原則としてGetRotationActive()依存　ただしSkeletonの場合はlclrot無し
		//読み書き読み書き読みテスト
		//Rokoko womandance BP無し 0frameAnim在り, left90 BP無し 0frameAnim在り
		//bvh121
		//Spring1
		//TheHunt City1 Camera1 Camera8
		//TheHunt Street1 Camera1 Camera2 Caemra3
		//Spring1にbvh121とRokoko BP在り無しをリターゲットテスト

	if ((bindposeflag == true) && IsSkeleton()) {
		//2023/07/04
		//	Rokoko womandanceとbvh121について　読み書き読み書き読みテストで　lclrot無しにしないとうまくいかなかった
		//  TheHuntCity1 Camera1のCameraの子供のスキンメッシュには　BPがあるが　lclrot無しにしないと読み書き読み書き読みで形が崩れた
		//  よってBPの有無に関わらず　Skeletonの場合にはlclrot無しとした

		//2023/07/06
		//skeletonの場合だけlclrot無しにするのはbindpose計算時だけ. カレントのポーズ計算時には通常通りGetRotationActiveに依存する
		*plocalnodemat = localnodemat;
	}
	else if (fbxRotationActive) {
		*plocalnodemat = localnodeanimmat;
	}
	else {
		*plocalnodemat = localnodemat;
	}
	

	*plocalnodeanimmat = localnodeanimmat;//明示的にlclrot入りの姿勢を使う場合用


	return 0;
}

void CBone::CalcNodePostureReq(bool bindposeflag, FbxNode* pNode, double srcframe, ChaMatrix* plocalnodemat, ChaMatrix* plocalnodeanimmat)
{
	if (!pNode || !plocalnodemat || !plocalnodeanimmat) {
		_ASSERT(0);
		return;
	}

	ChaMatrix localnodemat, localnodeanimmat;
	localnodemat.SetIdentity();
	localnodeanimmat.SetIdentity();
	CalcLocalNodePosture(bindposeflag, pNode, srcframe, &localnodemat, &localnodeanimmat);

	//親方向へ計算
	ChaMatrix tmpmat1, tmpmat2;
	tmpmat1 = *plocalnodemat * localnodemat;
	tmpmat2 = *plocalnodeanimmat * localnodeanimmat;

	*plocalnodemat = tmpmat1;
	*plocalnodeanimmat = tmpmat2;

	//親方向へ計算
	if (GetParent(false)) {
		GetParent(false)->CalcNodePostureReq(bindposeflag, pNode->GetParent(), srcframe, plocalnodemat, plocalnodeanimmat);
	}
}



////2023/02/16
////fbxの初期姿勢のジョイントの向きを書き出すために追加
//void CBone::RestoreFbxNodePosture(FbxNode* pNode)
//{
//
//	if (pNode) {
//		ChaVector3 roteul, preroteul, postroteul;
//		FbxDouble3 roteulxyz, preroteulxyz, postroteulxyz;
//
//		roteul = ChaVector3((float)m_fbxLclRot[0], (float)m_fbxLclRot[1], (float)m_fbxLclRot[2]);
//		preroteul = ChaVector3((float)m_fbxPreRot[0], (float)m_fbxPreRot[1], (float)m_fbxPreRot[2]);
//		postroteul = ChaVector3((float)m_fbxPostRot[0], (float)m_fbxPostRot[1], (float)m_fbxPostRot[2]);
//		roteulxyz = roteul.ConvRotOrder2XYZ(m_rotationorder);
//		preroteulxyz = preroteul.ConvRotOrder2XYZ(m_rotationorder);
//		postroteulxyz = postroteul.ConvRotOrder2XYZ(m_rotationorder);
//
//
//		pNode->SetRotationOrder(FbxNode::eSourcePivot, eEulerXYZ);//書き出しはXYZ
//
//
//		pNode->LclTranslation.Set(m_fbxLclPos);
//		//pNode->LclRotation.Set(m_fbxLclRot);
//		pNode->LclRotation.Set(roteulxyz);//書き出しはXYZ
//		pNode->LclScaling.Set(m_fbxLclScl);
//
//		pNode->SetRotationOffset(FbxNode::eSourcePivot, m_fbxRotOff);
//
//		pNode->SetRotationPivot(FbxNode::eSourcePivot, m_fbxRotPiv);
//
//		//pNode->SetPreRotation(FbxNode::eSourcePivot, m_fbxPreRot);
//		pNode->SetPreRotation(FbxNode::eSourcePivot, preroteulxyz);//書き出しはXYZ
//
//		//pNode->SetPostRotation(FbxNode::eSourcePivot, m_fbxPostRot);
//		pNode->SetPostRotation(FbxNode::eSourcePivot, postroteulxyz);//書き出しはXYZ
//
//		pNode->SetScalingOffset(FbxNode::eSourcePivot, m_fbxSclOff);
//
//		pNode->SetScalingPivot(FbxNode::eSourcePivot, m_fbxSclPiv);
//
//		pNode->SetRotationActive(m_fbxrotationActive);
//
//		pNode->InheritType.Set(m_InheritType);//2023/06/03
//	}
//}


void CBone::SetIKTargetFlag(bool srcflag)
{
	//2023/04/28
	if (IsNotSkeleton()) {
		return;
	}


	m_iktargetflag = srcflag;

	if (srcflag == true) {
		if (GetParModel()) {
			MOTINFO* curmi = GetParModel()->GetCurMotInfo();
			if (curmi) {
				CMotionPoint curmp = GetCurMp();

				//2023/03/24 model座標系：modelのworldmatを打ち消す
				ChaMatrix curwm = GetWorldMat(g_limitdegflag, curmi->motid, curmi->curframe, &curmp) * ChaMatrixInv(GetParModel()->GetWorldMat());
				
				ChaVector3 jointpos0, jointpos1;
				jointpos0 = GetJointFPos();
				ChaVector3TransformCoord(&jointpos1, &jointpos0, &curwm);
				SetIKTargetPos(jointpos1);
			}
			else {
				SetIKTargetPos(ChaVector3(0.0f, 0.0f, 0.0f));
			}
		}
		else {
			SetIKTargetPos(ChaVector3(0.0f, 0.0f, 0.0f));
		}
	}
	else {
		SetIKTargetPos(ChaVector3(0.0f, 0.0f, 0.0f));
	}

}
bool CBone::GetIKTargetFlag()
{
	return m_iktargetflag;
}

CBone* CBone::GetParent(bool excludenullflag)
{
	CBone* findbone = 0;
	if (m_parent) {
		m_parent->GetParentReq(excludenullflag, &findbone);
	}
	return findbone;
};

CBone* CBone::GetChild(bool excludenullflag)
{ 
	CBone* findbone = 0;
	bool firstfindbroflag = true;
	if (m_child) {
		m_child->GetChildReq(excludenullflag, firstfindbroflag, &findbone);
	}
	return findbone;
};

CBone* CBone::GetBrother(bool excludenullflag)
{ 
	CBone* findbone = 0;
	if (m_brother) {
		m_brother->GetBrotherReq(excludenullflag, &findbone);
	}
	return findbone;
};
CBone* CBone::GetSister(bool excludenullflag) 
{
	CBone* parbone = GetParent(excludenullflag);
	if (parbone) {
		CBone* firstbrobone = parbone->GetChild(excludenullflag);
		CBone* nextbone = firstbrobone;
		while (nextbone) {
			if (nextbone) {
				if (nextbone->GetBrother(excludenullflag) == this) {
					return nextbone;//!!!!!!!!!!!!
				}
				nextbone = nextbone->GetBrother(excludenullflag);
			}
			else {
				return 0;
			}
		}
		return 0;
	}
	else {
		return 0;
	}
	return 0;
};



void CBone::GetParentReq(bool excludenullflag, CBone** ppfindbone)
{
	if (!(*ppfindbone)) {
		if (excludenullflag == true) {
			if (GetType() == FBXBONE_SKELETON) {
				*ppfindbone = this;
				return;
			}
		}
		else {
			*ppfindbone = this;
			return;
		}

		if (!(*ppfindbone)) {
			if (m_parent) {
				m_parent->GetParentReq(excludenullflag, ppfindbone);
			}
			else {
				return;
			}
		}
	}
}
void CBone::GetChildReq(bool excludenullflag, bool findbroflag, CBone** ppfindbone)
{
	if (ppfindbone && !(*ppfindbone)) {
		if (excludenullflag == true) {
			if (GetType() == FBXBONE_SKELETON) {
				*ppfindbone = this;
				return;
			}
		}
		else {
			*ppfindbone = this;
			return;
		}


		if (findbroflag && !(*ppfindbone)) {
			if (m_brother) {
				m_brother->GetChildReq(excludenullflag, findbroflag, ppfindbone);
			}
		}
		if (!(*ppfindbone)) {
			if (m_child) {
				bool findbroflag3 = true;
				m_child->GetChildReq(excludenullflag, findbroflag3, ppfindbone);
			}
		}
	}
}
void CBone::GetBrotherReq(bool excludenullflag, CBone** ppfindbone)
{
	if (ppfindbone && !(*ppfindbone)) {
		if (excludenullflag == true) {
			if (GetType() == FBXBONE_SKELETON) {
				*ppfindbone = this;
				return;
			}
		}
		else {
			*ppfindbone = this;
			return;
		}

		if (!(*ppfindbone)) {
			if (m_brother) {
				m_brother->GetBrotherReq(excludenullflag, ppfindbone);
			}
		}
	}
}

ChaMatrix CBone::GetTransformMat(double srctime, bool forceanimflag)
{
	ChaMatrix retmat;
	retmat.SetIdentity();
	ChaMatrix retanimmat;
	retanimmat.SetIdentity();

	if (GetFbxNodeOnLoad()) {
		bool bindposeflag = false;//!!!!!!!!!!!!! カレントポーズ計算
		CalcNodePostureReq(bindposeflag, GetFbxNodeOnLoad(), srctime, &retmat, &retanimmat);
	}
	else {
		_ASSERT(0);
	}
	
	if (forceanimflag == false) {
		return retmat;//FbxNode::GetRotationActive()の値に依存してlclrotを含む
	}
	else {
		return retanimmat;//強制的にlclrotを含む
	}
}

//##############################
//GetTransformMat()[上記]が吸収
//##############################
//ChaMatrix CBone::GetENullMatrix(double srctime)
//{
//	//ChaMatrix retmat;
//	//retmat.SetIdentity();
//	//ChaMatrix tmpmat;
//	//tmpmat.SetIdentity();
//
//	//if (!GetParModel()) {
//	//	_ASSERT(0);
//	//	return retmat;
//	//}
//
//	//CalcEnullMatReq(srctime, &retmat, &tmpmat);
//	//return retmat;
//
//
//	FbxNode* eNullNode = GetFbxNodeOnLoad();
//	if (eNullNode) {
//		FbxTime time0;
//		time0.SetSecondDouble(srctime / 30.0);
//		FbxAMatrix lGlobalSRT = eNullNode->EvaluateGlobalTransform(time0, FbxNode::eSourcePivot, true, true);
//		ChaMatrix retmat = ChaMatrixFromFbxAMatrix(lGlobalSRT);
//		return retmat;
//	}
//	else {
//		_ASSERT(0);
//		ChaMatrix retmat;
//		retmat.SetIdentity();
//		return retmat;
//	}
//}

ChaMatrix CBone::CalcFbxLocalMatrix(bool limitdegflag, int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);

	ChaMatrix localfbxmat;
	localfbxmat.SetIdentity();


	if (!GetParModel()) {
		_ASSERT(0);
		localfbxmat.SetIdentity();
		return localfbxmat;
	}


	ChaMatrix wmanim = GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	//ChaMatrix fbxwm;
	//fbxwm = GetNodeMat() * wmanim;//eNULL自体のアニメーション書き出しは　しないことに

	ChaMatrix parentfbxwm;
	parentfbxwm.SetIdentity();

	CBone* parentbone = GetParent(false);
	if (parentbone) {
		//書き出し中に parentが eNullの場合はある
		if (parentbone->IsSkeleton()) {
			ChaMatrix parentwmanim = parentbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			parentfbxwm = parentbone->GetNodeMat() * parentwmanim;
			localfbxmat = GetNodeMat() * wmanim * ChaMatrixInv(parentfbxwm);
		}
		else if (parentbone->IsNull()) {
			//2023/06/29 eNullもアニメーション可能にしたので
			//GetENullMatrixを修正してCalcEnullMatReqで計算するようにしたところが2023/06/26から変わったところ
			//SetWorldMat()時には　回転計算用のローカル行列取得時に　parenetがeNullの場合関してもGetWorldMat[invNode * CalcENullMat]を使用
			//Fbx回転計算時には　CalcLocalEulXYZ()内にて　parentがeNullの場合　invNode * CalcENullMatを使用
			//parentfbxwm = parentbone->GetENullMatrix(roundingframe);

			//parentfbxwm = parentbone->GetTransformMat(roundingframe, true);

			parentfbxwm = parentbone->GetTransformMat(0.0, true);
			localfbxmat = GetNodeMat() * wmanim * ChaMatrixInv(parentfbxwm);
		}
		else if (parentbone->IsCamera()) {
			//bool multInvNodeMat = false;
			//parentfbxwm = GetParModel()->GetCameraTransformMat(srcmotid, roundingframe, g_cameraInheritMode, multInvNodeMat);


			//parentfbxwm = parentbone->GetTransformMat(srcframe, true);

			parentfbxwm = parentbone->GetTransformMat(0.0, true);//2023/07/05 Cameraの子供のスキンメッシュの形が　読み書き読み書き読みテストで形崩れしないように
			localfbxmat = GetNodeMat() * wmanim * ChaMatrixInv(parentfbxwm);
		}
		else {
			parentfbxwm.SetIdentity();
			localfbxmat = GetNodeMat() * wmanim;
		}
	}
	else {
		parentfbxwm.SetIdentity();
		localfbxmat = GetNodeMat() * wmanim;
	}
	
	return localfbxmat;

}

bool CBone::HasCameraParent()
{
	bool hascameraparent = false;

	CBone* chkparent = GetParent(false);
	while (chkparent) {
		if (chkparent->IsCamera()) {
			hascameraparent = true;
			break;
		}

		chkparent = chkparent->GetParent(false);
	}

	return hascameraparent;
}

