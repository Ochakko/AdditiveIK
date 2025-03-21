#include "stdafx.h"
#include <windows.h>
#include <math.h>


//#ifdef CONVD3DX11
//#include <d3dcommon.h>
//#include <dxgi.h>
//#include <d3d10_1.h>
//#include <d3d10.h>
//#include <d3dcompiler.h>
//#include <d3dx10.h>
//#endif

#define CHACALCCPP
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <GlobalVar.h>
#include <Model.h>
#include <Bone.h>
#include <MotionPoint.h>
#include <EditRange.h>
#include <RIgidElem.h>
#include <Collision.h>

#include "../Examples/CommonInterfaces/CommonExampleInterface.h"
#include "../Examples/CommonInterfaces/CommonGUIHelperInterface.h"
#include "../BTSRC/BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "../BTSRC/BulletCollision/CollisionShapes/btCollisionShape.h"
#include "../BTSRC/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"


#include "../BTSRC/LinearMath/btTransform.h"
#include "../BTSRC/LinearMath/btHashMap.h"
//#include "../Examples/MultiThreadedDemo/CommonRigidBodyMTBase.h"
//#include "../Examples/CommonInterfaces/CommonMultiBodyBase.h"

#include "../Examples/CommonInterfaces/CommonRigidBodyBase.h"
#include "../Examples/MultiThreadedDemo/CommonRigidBodyMTBase.h"


#include "../BTSRC/btBulletDynamicsCommon.h"
#include "../BTSRC/LinearMath/btIDebugDraw.h"

#include <math.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

#include <crtdbg.h>

using namespace std;


//extern bool g_wmatDirectSetFlag;//!!!!!!!!!!!!
//extern bool g_underIKRot;
extern bool g_x180flag;
extern CRITICAL_SECTION g_CritSection_FbxSdk;


//########################################################################################################
// 	ここから　//https://lxjk.github.io/2020/02/07/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained-JP.html
//########################################################################################################
#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

// for row major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A1 |
//                                              | A2  A3 |
// 2x2 row major Matrix multiply A*B
__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
{
	return
		_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 0, 3, 0, 3)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}
// 2x2 row major Matrix adjugate multiply (A#)*B
__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3, 3, 0, 0), vec2),
			_mm_mul_ps(VecSwizzle(vec1, 1, 1, 2, 2), VecSwizzle(vec2, 2, 3, 0, 1)));

}
// 2x2 row major Matrix multiply adjugate A*(B#)
__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 3, 0, 3, 0)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}
//########################################################################################################
// 	ここまで　//https://lxjk.github.io/2020/02/07/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained-JP.html
//########################################################################################################


//###########################################################################################################
// ここから　https://www.isus.jp/games/using-simd-technologies-on-intel-architecture-to-speed-up-game-code/
//###########################################################################################################
#define SHUFFLE_PARAM(x, y, z, w) ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))
//同じ要素の値を同時に計算するため、4 つの同じ単精度浮動小数点値を 1 つの __m128 データに設定します。

#define _mm_madd_ps(a, b, c) _mm_add_ps(_mm_mul_ps((a), (b)), (c))


#define _mm_replicate_x_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(0, 0, 0, 0))

#define _mm_replicate_y_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(1, 1, 1, 1))

#define _mm_replicate_z_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(2, 2, 2, 2))

#define _mm_replicate_w_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(3, 3, 3, 3))
//###########################################################################################################
// ここまで　https://www.isus.jp/games/using-simd-technologies-on-intel-architecture-to-speed-up-game-code/
//###########################################################################################################


								//BoneProp

double vecDotVec(ChaVector3* vec1, ChaVector3* vec2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfvecDotVec(vec1, vec2);
}

double lengthVec(ChaVector3* vec)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccflengthVec(vec);
}

double aCos(double dot)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfaCos(dot);
}

int vec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfvec3RotateY(dstvec, deg, srcvec);
}
int vec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfvec3RotateX(dstvec, deg, srcvec);
}
int vec3RotateZ(ChaVector3* dstvec, float deg, ChaVector3* srcvec)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfvec3RotateZ(dstvec, deg, srcvec);
}

int IsInitRot(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIsInitRot(srcmat);
}

int IsInitMat(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIsInitMat(srcmat);
}

int IsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIsSameMat(srcmat1, srcmat2);
}

int IsSameEul(ChaVector3 srceul1, ChaVector3 srceul2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIsSameEul(srceul1, srceul2);
}

bool IsJustEqualTime(double srctime1, double srctime2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIsJustEqualTime(srctime1, srctime2);
}

double RoundingTime(double srctime)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfRoundingTime(srctime);
}
int IntTime(double srctime)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIntTime(srctime);
}

bool IsEqualRoundingTime(double srctime1, double srctime2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfIsEqualRoundingTime(srctime1, srctime2);
}

int Float2Int(float srcfloat)
{
	return (int)(srcfloat + 0.0001f);
}

double VecLength(ChaVector3 srcvec)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfVecLength(srcvec);
}



void GetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfGetSRTMatrix(srcmat, svecptr, rmatptr, tvecptr);
}

void GetSRTMatrix2(ChaMatrix srcmat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfGetSRTMatrix2(srcmat, smatptr, rmatptr, tmatptr);
}

void GetSRTandTraAnim(ChaMatrix srcmat, ChaMatrix srcnodemat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr, ChaMatrix* tanimmatptr)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfGetSRTandTraAnim(srcmat, srcnodemat, smatptr, rmatptr, tmatptr, tanimmatptr);
}

ChaMatrix ChaMatrixFromSRT(bool sflag, bool tflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixFromSRT(sflag, tflag, srcnodemat, srcsmat, srcrmat, srctmat);
}

ChaMatrix ChaMatrixKeepScale(ChaMatrix srcmat, ChaVector3 srcsvec)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixKeepScale(srcmat, srcsvec);
}

ChaMatrix ChaMatrixFromSRTraAnim(bool sflag, bool tanimflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctanimmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixFromSRTraAnim(sflag, tanimflag, srcnodemat, srcsmat, srcrmat, srctanimmat);
}

ChaMatrix GetS0RTMatrix(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfGetS0RTMatrix(srcmat);
}




ChaMatrix TransZeroMat(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfTransZeroMat(srcmat);
}

ChaMatrix ChaMatrixFromFbxAMatrix(FbxAMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixFromFbxAMatrix(srcmat);
}

ChaMatrix ChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixFromBtMat3x3(srcmat3x3);
}

ChaMatrix ChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpivot)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixFromBtTransform(srcmat3x3, srcpivot);
}

ChaMatrix ChaMatrixInv(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixInv(srcmat);
}

//##############
//ChaVectorDbl2
//##############

ChaVectorDbl2::ChaVectorDbl2()
{
	x = 0.0;
	y = 0.0;
}

ChaVectorDbl2::ChaVectorDbl2(double srcx, double srcy)
{
	x = srcx;
	y = srcy;
}

ChaVectorDbl2::~ChaVectorDbl2()
{

}


ChaVectorDbl2 ChaVectorDbl2::operator= (ChaVectorDbl2 v) { this->x = v.x; this->y = v.y;; return *this; };
ChaVectorDbl2 ChaVectorDbl2::operator* (double srcw) const { return ChaVectorDbl2(this->x * srcw, this->y * srcw); }
ChaVectorDbl2& ChaVectorDbl2::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaVectorDbl2 ChaVectorDbl2::operator/ (double srcw) const { if (srcw != 0.0f) { return ChaVectorDbl2(this->x / srcw, this->y / srcw); } else { return ChaVectorDbl2(0.0, 0.0); } }
ChaVectorDbl2& ChaVectorDbl2::operator/= (double srcw) { if (srcw != 0.0f) { *this = *this / srcw; return *this; } else { this->x = 0.0f; this->y = 0.0f; return *this; } }
ChaVectorDbl2 ChaVectorDbl2::operator+ (const ChaVectorDbl2& v) const { return ChaVectorDbl2(x + v.x, y + v.y); }
ChaVectorDbl2& ChaVectorDbl2::operator+= (const ChaVectorDbl2& v) { *this = *this + v; return *this; }
ChaVectorDbl2 ChaVectorDbl2::operator- (const ChaVectorDbl2& v) const { return ChaVectorDbl2(x - v.x, y - v.y); }
ChaVectorDbl2& ChaVectorDbl2::operator-= (const ChaVectorDbl2& v) { *this = *this - v; return *this; }
ChaVectorDbl2 ChaVectorDbl2::operator* (const ChaVectorDbl2& v) const { return ChaVectorDbl2(this->x * v.x, this->y * v.y); }

ChaVectorDbl2 ChaVectorDbl2::operator- () const { return *this * -1.0; }


//############
//ChaVector2
//############

ChaVector2::ChaVector2()
{
	x = 0.0f;
	y = 0.0f;
}

ChaVector2::ChaVector2(float srcx, float srcy)
{
	x = srcx;
	y = srcy;
}

ChaVector2::~ChaVector2()
{

}


ChaVector2 ChaVector2::operator= (ChaVector2 v) { this->x = v.x; this->y = v.y;; return *this; };
ChaVector2 ChaVector2::operator* (float srcw) const { return ChaVector2((float)((double)this->x * (double)srcw), (float)((double)this->y * (double)srcw)); }
ChaVector2 &ChaVector2::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaVector2 ChaVector2::operator/ (float srcw) const { if (srcw != 0.0f) { return ChaVector2((float)((double)this->x / (double)srcw), (float)((double)this->y / (double)srcw)); } else { return ChaVector2(0.0f, 0.0f); } }
ChaVector2 &ChaVector2::operator/= (float srcw) { if (srcw != 0.0f) { *this = *this / srcw; return *this; } else { this->x = 0.0f; this->y = 0.0f; return *this; } }
ChaVector2 ChaVector2::operator* (double srcw) const { return ChaVector2((float)((double)this->x * srcw), (float)((double)this->y * srcw)); }
ChaVector2 &ChaVector2::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaVector2 ChaVector2::operator/ (double srcw) const { if (srcw != 0.0) { return ChaVector2((float)((double)this->x / srcw), (float)((double)this->y / srcw)); } else { return ChaVector2(0.0f, 0.0f); } }
ChaVector2 &ChaVector2::operator/= (double srcw) { if (srcw != 0.0) { *this = *this / srcw; return *this; } else { this->x = 0.0f; this->y = 0.0f; return *this; } }
ChaVector2 ChaVector2::operator+ (const ChaVector2 &v) const { return ChaVector2(x + v.x, y + v.y); }
ChaVector2 &ChaVector2::operator+= (const ChaVector2 &v) { *this = *this + v; return *this; }
ChaVector2 ChaVector2::operator- (const ChaVector2 &v) const { return ChaVector2(x - v.x, y - v.y); }
ChaVector2 &ChaVector2::operator-= (const ChaVector2 &v) { *this = *this - v; return *this; }
ChaVector2 ChaVector2::operator* (const ChaVector2 &v) const { return ChaVector2((float)((double)this->x * (double)v.x), (float)((double)this->y * (double)v.y)); }

ChaVector2 ChaVector2::operator+ (const ChaVectorDbl2& v) const { return ChaVector2((float)((double)x + v.x), (float)((double)y + v.y)); }
ChaVector2 ChaVector2::operator* (const ChaVectorDbl2 &v) const { return ChaVector2((float)((double)this->x * v.x), (float)((double)this->y * v.y)); }

ChaVector2 ChaVector2::operator- () const { return *this * -1.0; }



ChaVector3::ChaVector3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

ChaVector3::ChaVector3(float srcx, float srcy, float srcz)
{
	x = srcx;
	y = srcy;
	z = srcz;
}

ChaVector3::ChaVector3(DirectX::XMVECTOR v)
{
	x = v.m128_f32[0];
	y = v.m128_f32[1];
	z = v.m128_f32[2];
}
ChaVector3::ChaVector3(FbxDouble3 src)
{
	x = (float)src[0];
	y = (float)src[1];
	z = (float)src[2];
}

ChaVector3::ChaVector3(FbxVector4 src, bool divbyw)
{
	if (divbyw == false) {
		x = (float)src[0];
		y = (float)src[1];
		z = (float)src[2];
	}
	else {
		if ((src[3] != 0.0) && (src[3] != 1.0)) {
			x = (float)(src[0] / src[3]);
			y = (float)(src[1] / src[3]);
			z = (float)(src[2] / src[3]);
		}
		else {
			x = (float)src[0];
			y = (float)src[1];
			z = (float)src[2];
		}
	}
}

void ChaVector3::SetParams(FbxVector4 src, bool divbyw)
{
	if (divbyw == false) {
		x = (float)src[0];
		y = (float)src[1];
		z = (float)src[2];
	}
	else {
		if ((src[3] != 0.0) && (src[3] != 1.0)) {
			x = (float)(src[0] / src[3]);
			y = (float)(src[1] / src[3]);
			z = (float)(src[2] / src[3]);
		}
		else {
			x = (float)src[0];
			y = (float)src[1];
			z = (float)src[2];
		}
	}
}



void ChaVector3::SetZeroVec3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}


ChaVector3::~ChaVector3()
{

}

void ChaVector3::Normalize()
{
	ChaVector3 resultvec3;
	resultvec3.SetZeroVec3();
	ChaVector3Normalize(&resultvec3, this);
	*this = resultvec3;
}

void ChaVector3::Clamp(float srcmin, float srcmax)
{
	float tmpx = fmax(srcmin, fmin(srcmax, x));
	float tmpy = fmax(srcmin, fmin(srcmax, y));
	float tmpz = fmax(srcmin, fmin(srcmax, z));
	x = tmpx;
	y = tmpy;
	z = tmpz;
}

FbxDouble3 ChaVector3::ConvRotOrder2XYZ(EFbxRotationOrder rotorder)
{
	//src:*this, dst:return value

	CQuaternion rotq;
	rotq.SetRotation(rotorder, 0, *this);

	ChaVector3 eulxyz;
	eulxyz.SetParams(0.0f, 0.0f, 0.0f);
	//ChaVector3 befeul.SetParams(0.0f, 0.0f, 0.0f);
	BEFEUL befeul;
	befeul.Init();

	int isfirstbone = 0;
	int isendbone = 0;
	int notmodify180flag = 1;
	bool underikrot = false;
	bool underretarget = false;
	rotq.Q2EulXYZusingQ(underikrot, underretarget, 0, befeul, &eulxyz, isfirstbone, isendbone, notmodify180flag);
	//rotq.Q2EulXYZusingMat(0, 0, befeul, &eulxyz, isfirstbone, isendbone, notmodify180flag);

	return FbxDouble3(eulxyz.x, eulxyz.y, eulxyz.z);
}



ChaVector3 ChaVector3::operator= (ChaVector3 v) { this->x = v.x; this->y = v.y; this->z = v.z; return *this; };
ChaVector3 ChaVector3::operator* (float srcw) const { return ChaVector3((float)((double)this->x * (double)srcw), (float)((double)this->y * (double)srcw), (float)((double)this->z * (double)srcw)); }
ChaVector3& ChaVector3::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaVector3 ChaVector3::operator/ (float srcw) const { if (srcw != 0.0f) { return ChaVector3((float)((double)this->x / (double)srcw), (float)((double)this->y / (double)srcw), (float)((double)this->z / (double)srcw)); } else { return ChaVector3(0.0f, 0.0f, 0.0f); } }
ChaVector3& ChaVector3::operator/= (float srcw) { *this = *this / srcw; return *this; }
ChaVector3 ChaVector3::operator* (double srcw) const { return ChaVector3((float)((double)this->x * srcw), (float)((double)this->y * srcw), (float)((double)this->z * srcw)); }
ChaVector3& ChaVector3::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaVector3 ChaVector3::operator/ (double srcw) const { if (srcw != 0.0) { return ChaVector3((float)((double)this->x / srcw), (float)((double)this->y / srcw), (float)((double)this->z / srcw)); } else { return ChaVector3(0.0f, 0.0f, 0.0f); } }
ChaVector3& ChaVector3::operator/= (double srcw) { *this = *this / srcw; return *this; }
ChaVector3 ChaVector3::operator+ (const ChaVector3& v) const { return ChaVector3(x + v.x, y + v.y, z + v.z); }
ChaVector3& ChaVector3::operator+= (const ChaVector3& v) { *this = *this + v; return *this; }
ChaVector3 ChaVector3::operator- (const ChaVector3& v) const { return ChaVector3(x - v.x, y - v.y, z - v.z); }
ChaVector3& ChaVector3::operator-= (const ChaVector3& v) { *this = *this - v; return *this; }

ChaVector3 ChaVector3::operator- () const { return *this * -1.0; }


ChaMatrix ChaVector3::MakeTraMat()
{
	ChaMatrix retmat;
	retmat.SetIdentity();
	retmat.data[MATI_41] = x;
	retmat.data[MATI_42] = y;
	retmat.data[MATI_43] = z;
	return retmat;
}
ChaMatrix ChaVector3::MakeInvTraMat()
{
	ChaMatrix retmat;
	retmat.SetIdentity();
	retmat.data[MATI_41] = -x;
	retmat.data[MATI_42] = -y;
	retmat.data[MATI_43] = -z;
	return retmat;
}
ChaMatrix ChaVector3::MakeXYZRotMat(CQuaternion* srcaxisq)
{
	ChaMatrix retmat;
	retmat.SetIdentity();

	CQuaternion rotq;
	rotq.SetRotationXYZ(srcaxisq, *this);

	retmat = rotq.MakeRotMatX();
	return retmat;
}
ChaMatrix ChaVector3::MakeScaleMat()
{
	ChaMatrix retmat;
	retmat.SetIdentity();

	retmat.data[MATI_11] = x;
	retmat.data[MATI_22] = y;
	retmat.data[MATI_33] = z;
	return retmat;
}


//int ChaVector3::GetRowNo(int srcrowno)
//{
//	//テスト実装
//	//カメラ行列の列が row(0)==-z, row(1)==y, row(2)==x の場合があったので　テスト対応
//
//
//	float thval = 0.0001f;
//	if ((fabs(x) <= thval) && (fabs(y) <= thval) && (fabs(z) > thval)){
//		if (z >= 0.0f) {
//			return 3;//z axis
//		}
//		else {
//			return -3;//-z axis
//		}
//	}
//	else if ((fabs(x) > thval) && (fabs(y) <= thval) && (fabs(z) <= thval)) {
//		if (x >= 0.0f) {
//			return 1;//x axis
//		}
//		else {
//			return -1;//x axis
//		}
//	}
//	else if ((fabs(x) <= thval) && (fabs(y) > thval) && (fabs(z) <= thval)) {
//		if (y >= 0.0f) {
//			return 2;//y axis
//		}
//		else {
//			return -2;//y axis
//		}
//	}
//	else {
//		return (srcrowno + 1);
//	}
//}

ChaTexRGBA::ChaTexRGBA() {
	InitParams();
};
ChaTexRGBA::ChaTexRGBA(unsigned char srcR, unsigned char srcG, unsigned char srcB, unsigned char srcA) {
	R = srcR;
	G = srcG;
	B = srcB;
	A = srcA;
	Clamp();
};
ChaTexRGBA::ChaTexRGBA(float srcR, float srcG, float srcB, float srcA) {
	R = (unsigned char)(srcR * 255.0f);
	G = (unsigned char)(srcG * 255.0f);
	B = (unsigned char)(srcB * 255.0f);
	A = (unsigned char)(srcA * 255.0f);
	Clamp();
};
ChaTexRGBA::ChaTexRGBA(ChaVector4 src) {
	R = (unsigned char)(src.x * 255.0f);
	G = (unsigned char)(src.y * 255.0f);
	B = (unsigned char)(src.z * 255.0f);
	A = (unsigned char)(src.w * 255.0f);
	Clamp();
};
ChaTexRGBA::~ChaTexRGBA() {
};

void ChaTexRGBA::InitParams() {
	R = 0;
	G = 0;
	B = 0;
	A = 255;
};

void ChaTexRGBA::FromHSV(ChaVector4 srchsv) {
	ChaVector4 calcrgba = srchsv.HSV2RGB();
	this->SetParams(calcrgba);
};
void ChaTexRGBA::FromRGBA(ChaVector4 srcrgba) {
	this->SetParams(srcrgba);
};

ChaTexRGBA ChaTexRGBA::operator= (ChaTexRGBA v) { this->R = v.R; this->G = v.G; this->B = v.B; this->A = v.A; return *this; };
ChaTexRGBA ChaTexRGBA::operator* (float srcw) const {
	ChaTexRGBA result;
	result.R = (unsigned char)((float)this->R * srcw + 0.0001f);
	result.G = (unsigned char)((float)this->G * srcw + 0.0001f);
	result.B = (unsigned char)((float)this->B * srcw + 0.0001f);
	result.A = (unsigned char)((float)this->A * srcw + 0.0001f);
	result.Clamp();
	return result;
}
ChaTexRGBA& ChaTexRGBA::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaTexRGBA ChaTexRGBA::operator/ (float srcw) const { 
	if (srcw != 0.0f) { 
		ChaTexRGBA result;
		result.R = (unsigned char)((float)this->R / srcw + 0.0001f);
		result.G = (unsigned char)((float)this->G / srcw + 0.0001f);
		result.B = (unsigned char)((float)this->B / srcw + 0.0001f);
		result.A = (unsigned char)((float)this->A / srcw + 0.0001f);
		result.Clamp();
		return result;
	} 
	else { 
		return ChaTexRGBA(0.0f, 0.0f, 0.0f, 0.0f); 
	} 
}
ChaTexRGBA& ChaTexRGBA::operator/= (float srcw) { *this = *this / srcw; return *this; }
ChaTexRGBA ChaTexRGBA::operator* (double srcw) const { 
	ChaTexRGBA result;
	result.R = (unsigned char)((double)this->R * srcw + 0.0001);
	result.G = (unsigned char)((double)this->G * srcw + 0.0001);
	result.B = (unsigned char)((double)this->B * srcw + 0.0001);
	result.A = (unsigned char)((double)this->A * srcw + 0.0001);
	result.Clamp();
	return result;
}
ChaTexRGBA& ChaTexRGBA::operator*= (double srcw) { 
	*this = *this * srcw; return *this; 
}
ChaTexRGBA ChaTexRGBA::operator/ (double srcw) const { 
	if (srcw != 0.0f) {
		ChaTexRGBA result;
		result.R = (unsigned char)((double)this->R / srcw + 0.0001);
		result.G = (unsigned char)((double)this->G / srcw + 0.0001);
		result.B = (unsigned char)((double)this->B / srcw + 0.0001);
		result.A = (unsigned char)((double)this->A / srcw + 0.0001);
		result.Clamp();
		return result;
	}
	else {
		return ChaTexRGBA(0.0f, 0.0f, 0.0f, 0.0f);
	}
}
ChaTexRGBA& ChaTexRGBA::operator/= (double srcw) { *this = *this / srcw; return *this; }
ChaTexRGBA ChaTexRGBA::operator+ (const ChaTexRGBA& v) const { 
	ChaTexRGBA result;
	result.R = R + v.R;
	result.G = G + v.B;
	result.B = B + v.B;
	result.A = A + v.A;
	result.Clamp();
	return result;
}
ChaTexRGBA& ChaTexRGBA::operator+= (const ChaTexRGBA& v) { *this = *this + v; return *this; }
ChaTexRGBA ChaTexRGBA::operator- (const ChaTexRGBA& v) const { 
	ChaTexRGBA result;
	result.R = R - v.R;
	result.G = G - v.B;
	result.B = B - v.B;
	result.A = A - v.A;
	result.Clamp();
	return result;
}
ChaTexRGBA& ChaTexRGBA::operator-= (const ChaTexRGBA& v) { *this = *this - v; return *this; }
ChaTexRGBA ChaTexRGBA::operator* (const ChaTexRGBA& v) const {
	ChaVector4 param1;
	param1.x = (float)((double)this->R / 255.0);
	param1.y = (float)((double)this->G / 255.0);
	param1.z = (float)((double)this->B / 255.0);
	param1.w = (float)((double)this->A / 255.0);

	ChaVector4 param2;
	param2.x = (float)((double)v.R / 255.0);
	param2.y = (float)((double)v.G / 255.0);
	param1.z = (float)((double)v.B / 255.0);
	param1.w = (float)((double)v.A / 255.0);

	ChaVector4 calc1;
	calc1 = param1 * param2;

	ChaTexRGBA result;
	result.SetParams(calc1.x, calc1.y, calc1.z, calc1.w);
	result.Clamp();

	return result;
}
ChaTexRGBA& ChaTexRGBA::operator*= (const ChaTexRGBA& v) { *this = *this * v; return *this; }





ChaVector4::ChaVector4()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

ChaVector4::ChaVector4(float srcx, float srcy, float srcz, float srcw)
{
	x = srcx;
	y = srcy;
	z = srcz;
	w = srcw;
}

ChaVector4::ChaVector4(ChaVector3 srcvec3, float srcw)
{
	x = srcvec3.x;
	y = srcvec3.y;
	z = srcvec3.z;
	w = srcw;
}

ChaVector4::~ChaVector4()
{

}

void ChaVector4::Normalize()
{
	if (w >= 0.0000010f) {
		*this = *this / w;
	}
	ChaVector3 xyz;
	xyz.SetParams(x, y, z);
	ChaVector3Normalize(&xyz, &xyz);
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	w = w;
}

ChaVector4 ChaVector4::operator= (ChaVector4 v) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = v.w; return *this; };
ChaVector4 ChaVector4::operator* (float srcw) const { return ChaVector4((float)((double)this->x * (double)srcw), (float)((double)this->y * (double)srcw), (float)((double)this->z * (double)srcw), (float)((double)this->w * (double)srcw)); }
ChaVector4 &ChaVector4::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaVector4 ChaVector4::operator/ (float srcw) const { if (srcw != 0.0f) { return ChaVector4((float)((double)this->x / (double)srcw), (float)((double)this->y / (double)srcw), (float)((double)this->z / (double)srcw), (float)((double)this->w / (double)srcw)); } else { return ChaVector4(0.0f, 0.0f, 0.0f, 0.0f); } }
ChaVector4 &ChaVector4::operator/= (float srcw) { *this = *this / srcw; return *this; }
ChaVector4 ChaVector4::operator* (double srcw) const { return ChaVector4((float)((double)this->x * srcw), (float)((double)this->y * srcw), (float)((double)this->z * srcw), (float)((double)this->w * srcw)); }
ChaVector4& ChaVector4::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaVector4 ChaVector4::operator/ (double srcw) const { if (srcw != 0.0) { return ChaVector4((float)((double)this->x / srcw), (float)((double)this->y / srcw), (float)((double)this->z / srcw), (float)((double)this->w / srcw)); } else { return ChaVector4(0.0f, 0.0f, 0.0f, 0.0f); } }
ChaVector4& ChaVector4::operator/= (double srcw) { *this = *this / srcw; return *this; }
ChaVector4 ChaVector4::operator+ (const ChaVector4 &v) const { return ChaVector4(x + v.x, y + v.y, z + v.z, w + v.w); }
ChaVector4 &ChaVector4::operator+= (const ChaVector4 &v) { *this = *this + v; return *this; }
ChaVector4 ChaVector4::operator- (const ChaVector4 &v) const { return ChaVector4(x - v.x, y - v.y, z - v.z, w - v.w); }
ChaVector4 &ChaVector4::operator-= (const ChaVector4 &v) { *this = *this - v; return *this; }
ChaVector4 ChaVector4::operator* (const ChaVector4& v) const { return ChaVector4(x * v.x, y * v.y, z * v.z, w * v.w); }
ChaVector4& ChaVector4::operator*= (const ChaVector4& v) { *this = *this * v; return *this; }



ChaVector4 ChaVector4::operator- () const { return *this * -1.0; }

void ChaVector4::Clamp(float srcmin, float srcmax)
{
	float tmpx = fmax(srcmin, fmin(srcmax, x));
	float tmpy = fmax(srcmin, fmin(srcmax, y));
	float tmpz = fmax(srcmin, fmin(srcmax, z));
	float tmpw = fmax(srcmin, fmin(srcmax, w));
	x = tmpx;
	y = tmpy;
	z = tmpz;
}

void ChaVector4::ClampHSV()
{
	x = fmin(360.0f, x);
	x = fmax(0.0f, x);

	y = fmin(1.0f, y);
	y = fmax(0.0f, y);
	
	z = fmin(1.0f, z);
	z = fmax(0.0f, z);

	w = fmin(1.0f, w);
	w = fmax(0.0f, w);
}

int ChaVector4::HSV_Add(ChaVector4 srcadd)
{
	ChaVector4 temphsv = RGB2HSV();

	temphsv.x += srcadd.x;
	if (temphsv.x > 360.0f) {
		if (fabs(temphsv.x) < (360.0f + 1e-4)) {
			temphsv.x = 360.0f;//誤差
		}
		else {
			temphsv.x -= 360.0f;
		}
	}
	if (temphsv.x < 0.0f) {
		if (fabs(temphsv.x) < 1e-4) {
			temphsv.x = 0.0f;//誤差
		}
		else {
			temphsv.x += 360.0f;
		}
	}
	temphsv.x = fmin(360.0f, temphsv.x);
	temphsv.x = fmax(0.0f, temphsv.x);

	temphsv.y += srcadd.y;
	temphsv.y = fmin(1.0f, temphsv.y);
	temphsv.y = fmax(0.0f, temphsv.y);

	temphsv.z += srcadd.z;
	temphsv.z = fmin(1.0f, temphsv.z);
	temphsv.z = fmax(0.0f, temphsv.z);

	temphsv.w += srcadd.w;
	temphsv.w = fmin(1.0f, temphsv.w);
	temphsv.w = fmax(0.0f, temphsv.w);

	ChaVector4 resultrgb = temphsv.HSV2RGB();
	*this = resultrgb;

	return 0;


}

int ChaVector4::HSV_AddH(float addh)
{
	//this:RGB[0,1], ret:RGB[0,1], temphsv:h[0,360] s[0,1] v[0,1]

	if (fabs(addh) <= 1e-4) {
		return 0;//!!!!!!!!!!!!!!
	}


	ChaVector4 temphsv = RGB2HSV();

	temphsv.x += addh;
	if (temphsv.x > 360.0f) {
		if (fabs(temphsv.x) < (360.0f + 1e-4)) {
			temphsv.x = 360.0f;//誤差
		}
		else {
			temphsv.x -= 360.0f;
		}
	}
	if (temphsv.x < 0.0f) {
		if (fabs(temphsv.x) < 1e-4) {
			temphsv.x = 0.0f;//誤差
		}
		else {
			temphsv.x += 360.0f;
		}
	}
	temphsv.x = fmin(360.0f, temphsv.x);
	temphsv.x = fmax(0.0f, temphsv.x);

	ChaVector4 resultrgb = temphsv.HSV2RGB();
	*this = resultrgb;

	return 0;

}

int ChaVector4::HSV_AddS(float adds)
{
	//this:RGB[0,1], ret:RGB[0,1], temphsv:h[0,360] s[0,1] v[0,1]

	if (fabs(adds) <= 1e-4) {
		return 0;//!!!!!!!!!!!!!!
	}


	ChaVector4 temphsv = RGB2HSV();
	temphsv.y += adds;
	temphsv.y = fmin(1.0f, temphsv.y);
	temphsv.y = fmax(0.0f, temphsv.y);

	ChaVector4 resultrgb = temphsv.HSV2RGB();
	*this = resultrgb;

	return 0;
}
int ChaVector4::HSV_AddV(float addv)
{
	//this:RGB[0,1], ret:RGB[0,1], temphsv:h[0,360] s[0,1] v[0,1]

	if (fabs(addv) <= 1e-4) {
		return 0;//!!!!!!!!!!!!!!
	}

	ChaVector4 temphsv = RGB2HSV();
	temphsv.z += addv;
	temphsv.z = fmin(1.0f, temphsv.z);
	temphsv.z = fmax(0.0f, temphsv.z);

	ChaVector4 resultrgb = temphsv.HSV2RGB();
	*this = resultrgb;

	return 0;
}
ChaVector4 ChaVector4::RGB2HSV()
{
	//this:RGB[0,1], ret:HSV h[0,360] s[0,1] v[0,1]
	ChaVector4 rethsv;

	double _max, _min, _diff;
	_max = fmax(x, fmax(y, z));
	_min = fmin(x, fmin(y, z));
	_diff = _max - _min;

	if (_max <= 1e-4) {
		rethsv.SetParams(0.0f, 0.0f, 0.0f, this->w);
	}
	else {
		double out_h, out_s, out_v;
		out_h = 0.0;
		if (fabs(_diff) <= 1e-4) {
			out_h = 0.0;
		}
		else {
			if (_max == x) {
				out_h = 60.0 * (y - z) / _diff;
			}
			else if (_max == y) {
				out_h = (60.0 * (z - x) / _diff) + 120.0;
			}
			else if (_max == z) {
				out_h = (60.0 * (x - y) / _diff) + 240.0;
			}
		}

		if (out_h < 0.0) {
			out_h += 360.0;
		}
		out_v = _max;
		out_s = _diff / _max;

		rethsv.SetParams((float)out_h, (float)out_s, (float)out_v, this->w);
	}

	return rethsv;
}
ChaVector4 ChaVector4::HSV2RGB()
{
	//this:HSV h[0,360] s[0,1] v[0,1], ret:RGB[0,1]
	ChaVector4 retrgb;
	double out_r, out_g, out_b;

	double f, p, q, t;
	int Hi;

	if (y == 0.0) {
		out_r = out_g = out_b = z;
	}
	else {
		Hi = (int)(x / 60.0) % 6;
		if (Hi < 0) {
			Hi *= -1;
		}

		f = x / 60.0 - (double)Hi;
		p = z * (1.0 - y);
		q = z * (1.0 - f * y);
		t = z * (1.0 - (1.0 - f) * y);

		if (Hi == 0) {
			out_r = z;
			out_g = t;
			out_b = p;
		}
		else if (Hi == 1) {
			out_r = q;
			out_g = z;
			out_b = p;
		}
		else if (Hi == 2) {
			out_r = p;
			out_g = z;
			out_b = t;
		}
		else if (Hi == 3) {
			out_r = p;
			out_g = q;
			out_b = z;
		}
		else if (Hi == 4) {
			out_r = t;
			out_g = p;
			out_b = z;
		}
		else {
			out_r = z;
			out_g = p;
			out_b = q;
		}
	}
	retrgb.SetParams((float)out_r, (float)out_g, (float)out_b, this->w);
	return retrgb;
}

void ChaVector4::HSV_Lerp(ChaVector4 befhsv, ChaVector4 afthsv, float t)
{
	ChaVector4 diffhsv = afthsv - befhsv;
	ChaVector4 stephsv = diffhsv * t;
	ChaVector4 calchsv = befhsv + stephsv;
	calchsv.ClampHSV();
	*this = calchsv;
}


ChaPlane::ChaPlane()
{
	a = 0.0f;
	b = 0.0f;
	c = 1.0f;
	d = 0.0f;
}

ChaPlane::ChaPlane(float srca, float srcb, float srcc, float srcd)
{
	a = srca;
	b = srcb;
	c = srcc;
	c = srcd;
}

ChaPlane::~ChaPlane()
{
}

ChaPlane ChaPlane::operator= (ChaPlane v) { this->a = v.a; this->b = v.b; this->c = v.c; this->d = v.d; return *this; };

int ChaPlane::FromPoints(ChaVector3 point1, ChaVector3 point2, ChaVector3 point3)
{
	ChaVector3 n;
	float d;

	//ChaVector3 v21 = point1 - point2;
	//ChaVector3 v31 = point1 - point3;
	//ChaVector3Cross(&n, &v21, &v31);

	//ChaVector3 v21 = point1 - point2;
	//ChaVector3 v32 = point3 - point2;
	//ChaVector3Cross(&n, &v21, &v32);
	
	
	ChaVector3 v12 = point2 - point1;
	ChaVector3 v13 = point3 - point1;
	ChaVector3Cross(&n, &v12, &v13);
	ChaVector3Normalize(&n, &n);

	d = ChaVector3Dot(&n, &point1) * -1.0f;

	a = n.x;
	b = n.y;
	c = n.z;

	//Result.x = N.x;
	//Result.y = N.y;
	//Result.z = N.z;
	//Result.w = -D.w;

	return 0;
}

int ChaPlane::GetFootOnPlane(ChaVector3 srcpos, ChaVector3* dstpos)
{
	if (!dstpos) {
		_ASSERT(0);
		return 1;
	}

	double t0, tmpt1, t1, t;
	t0 = a * srcpos.x + b * srcpos.y + c * srcpos.z + d;
	tmpt1 = a * a + b * b + c * c;
	if (tmpt1 != 0.0f) {
		t1 = sqrt(tmpt1);
	}
	else {
		t1 = 0.0f;
	}
	
	if (t1 != 0.0f) {
		t = -(t0 / t1);
	}
	else {
		t = 0.0f;
	}

	dstpos->x = (float)((double)srcpos.x + t * (double)a);
	dstpos->y = (float)((double)srcpos.y + t * (double)b);
	dstpos->z = (float)((double)srcpos.z + t * (double)c);

	return 0;
}


ChaMatrix::ChaMatrix()
{
	data[MATI_11] = 1.0f;
	data[MATI_12] = 0.0f;
	data[MATI_13] = 0.0f;
	data[MATI_14] = 0.0f;

	data[MATI_21] = 0.0f;
	data[MATI_22] = 1.0f;
	data[MATI_23] = 0.0f;
	data[MATI_24] = 0.0f;

	data[MATI_31] = 0.0f;
	data[MATI_32] = 0.0f;
	data[MATI_33] = 1.0f;
	data[MATI_34] = 0.0f;

	data[MATI_41] = 0.0f;
	data[MATI_42] = 0.0f;
	data[MATI_43] = 0.0f;
	data[MATI_44] = 1.0f;

	//_11 = 1.0f;
	//_12 = 0.0f;
	//_13 = 0.0f;
	//data[MATI_14] = 0.0f;

	//data[MATI_21] = 0.0f;
	//data[MATI_22] = 1.0f;
	//data[MATI_23] = 0.0f;
	//data[MATI_24] = 0.0f;

	//data[MATI_31] = 0.0f;
	//data[MATI_32] = 0.0f;
	//data[MATI_33] = 1.0f;
	//data[MATI_34] = 0.0f;

	//data[MATI_41] = 0.0f;
	//data[MATI_42] = 0.0f;
	//data[MATI_43] = 0.0f;
	//data[MATI_44] = 1.0f;

}

ChaMatrix::ChaMatrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	data[MATI_11] = m11;
	data[MATI_12] = m12;
	data[MATI_13] = m13;
	data[MATI_14] = m14;

	data[MATI_21] = m21;
	data[MATI_22] = m22;
	data[MATI_23] = m23;
	data[MATI_24] = m24;

	data[MATI_31] = m31;
	data[MATI_32] = m32;
	data[MATI_33] = m33;
	data[MATI_34] = m34;

	data[MATI_41] = m41;
	data[MATI_42] = m42;
	data[MATI_43] = m43;
	data[MATI_44] = m44;

}

void ChaMatrix::SetIdentity()
{
	ChaMatrixIdentity(this);
}

float* ChaMatrix::GetDataPtr()
{
	return (float*)(&data[MATI_11]);
}



FbxAMatrix ChaMatrix::FBXAMATRIX()
{
	FbxAMatrix retmat;
	retmat.SetIdentity();
	retmat.SetRow(0, FbxVector4(data[MATI_11], data[MATI_12], data[MATI_13], data[MATI_14]));
	retmat.SetRow(1, FbxVector4(data[MATI_21], data[MATI_22], data[MATI_23], data[MATI_24]));
	retmat.SetRow(2, FbxVector4(data[MATI_31], data[MATI_32], data[MATI_33], data[MATI_34]));
	retmat.SetRow(3, FbxVector4(data[MATI_41], data[MATI_42], data[MATI_43], data[MATI_44]));
	return retmat;
}

void ChaMatrix::SetRow(int rowindex, ChaVector3 srcrow)
{
	//行
	switch (rowindex) {
	case 0:
		data[MATI_11] = srcrow.x;
		data[MATI_12] = srcrow.y;
		data[MATI_13] = srcrow.z;
		break;
	case 1:
		data[MATI_21] = srcrow.x;
		data[MATI_22] = srcrow.y;
		data[MATI_23] = srcrow.z;
		break;
	case 2:
		data[MATI_31] = srcrow.x;
		data[MATI_32] = srcrow.y;
		data[MATI_33] = srcrow.z;
		break;
	case 3:
		data[MATI_41] = srcrow.x;
		data[MATI_42] = srcrow.y;
		data[MATI_43] = srcrow.z;
		break;
	default:
		_ASSERT(0);
		break;
	}
}
void ChaMatrix::SetCol(int colindex, ChaVector3 srccol)
{
	//列
	switch (colindex) {
	case 0:
		data[MATI_11] = srccol.x;
		data[MATI_21] = srccol.y;
		data[MATI_31] = srccol.z;
		break;
	case 1:
		data[MATI_12] = srccol.x;
		data[MATI_22] = srccol.y;
		data[MATI_32] = srccol.z;
		break;
	case 2:
		data[MATI_13] = srccol.x;
		data[MATI_23] = srccol.y;
		data[MATI_33] = srccol.z;
		break;
	case 3:
		data[MATI_14] = srccol.x;
		data[MATI_24] = srccol.y;
		data[MATI_34] = srccol.z;
		break;
	default:
		_ASSERT(0);
		break;
	}

}

ChaVector3 ChaMatrix::GetRow(int rowindex)
{
	//行
	ChaVector3 retrow;
	switch (rowindex) {
	case 0:
		retrow.SetParams(data[MATI_11], data[MATI_12], data[MATI_13]);
		break;
	case 1:
		retrow.SetParams(data[MATI_21], data[MATI_22], data[MATI_23]);
		break;
	case 2:
		retrow.SetParams(data[MATI_31], data[MATI_32], data[MATI_33]);
		break;
	case 3:
		retrow.SetParams(data[MATI_41], data[MATI_42], data[MATI_43]);
		break;
	default:
		_ASSERT(0);
		retrow.SetParams(data[MATI_11], data[MATI_12], data[MATI_13]);
		break;
	}

	return retrow;
}
ChaVector3 ChaMatrix::GetCol(int colindex)
{
	//列
	ChaVector3 retcol;
	switch (colindex) {
	case 0:
		retcol.SetParams(data[MATI_11], data[MATI_21], data[MATI_31]);
		break;
	case 1:
		retcol.SetParams(data[MATI_12], data[MATI_22], data[MATI_32]);
		break;
	case 2:
		retcol.SetParams(data[MATI_13], data[MATI_23], data[MATI_33]);
		break;
	case 3:
		retcol.SetParams(data[MATI_14], data[MATI_24], data[MATI_34]);
		break;
	default:
		_ASSERT(0);
		retcol.SetParams(data[MATI_11], data[MATI_12], data[MATI_13]);
		break;
	}

	return retcol;
}
ChaVector3 ChaMatrix::GetTranslation()
{
	//GetRow(3)
	return GetRow(3);
}

void ChaMatrix::SetTranslation(ChaVector3 srctra)
{
	//初期化しない

	data[MATI_41] = srctra.x;
	data[MATI_42] = srctra.y;
	data[MATI_43] = srctra.z;
}
void ChaMatrix::SetTranslationZero()
{
	//初期化しない

	SetTranslation(ChaVector3(0.0f, 0.0f, 0.0f));
}
void ChaMatrix::SetForVectorTransform()
{
	//初期化しない

	SetTranslationZero();
	data[MATI_44] = 0.0f;
}
void ChaMatrix::AddTranslation(ChaVector3 srctra)
{
	//初期化しない

	data[MATI_41] += srctra.x;
	data[MATI_42] += srctra.y;
	data[MATI_43] += srctra.z;
}

void ChaMatrix::SetRotation(EFbxRotationOrder rotorder, CQuaternion* axisq, ChaVector3 srceul)
{
	//初期化しない
	CQuaternion rotq;
	rotq.SetRotation(rotorder, axisq, srceul);
	ChaMatrix rotmat;
	rotmat = rotq.MakeRotMatX();

	SetBasis(rotmat);
}

void ChaMatrix::SetXYZRotation(CQuaternion* srcaxisq, ChaVector3 srceul)
{
	//初期化しない
	ChaMatrix rotmat = srceul.MakeXYZRotMat(srcaxisq);
	SetBasis(rotmat);
}
void ChaMatrix::SetXYZRotation(CQuaternion* srcaxisq, CQuaternion srcq)
{
	//初期化しない

	CQuaternion EQ;
	if (srcaxisq) {
		EQ = srcaxisq->inverse() * srcq * *srcaxisq;
	}
	else {
		EQ = srcq;
	}

	ChaMatrix rotmat = srcq.MakeRotMatX();
	SetBasis(rotmat);

}
void ChaMatrix::SetScale(ChaVector3 srcscale)
{
	//初期化しない

	data[MATI_11] = srcscale.x;
	data[MATI_22] = srcscale.y;
	data[MATI_33] = srcscale.z;

}
void ChaMatrix::SetBasis(ChaMatrix srcmat)
{
	//初期化しない　copy3x3
	data[MATI_11] = srcmat.data[MATI_11];
	data[MATI_12] = srcmat.data[MATI_12];
	data[MATI_13] = srcmat.data[MATI_13];

	data[MATI_21] = srcmat.data[MATI_21];
	data[MATI_22] = srcmat.data[MATI_22];
	data[MATI_23] = srcmat.data[MATI_23];

	data[MATI_31] = srcmat.data[MATI_31];
	data[MATI_32] = srcmat.data[MATI_32];
	data[MATI_33] = srcmat.data[MATI_33];
}

//#ifdef CONVD3DX11
ChaMatrix::ChaMatrix(Matrix m)
{
	data[MATI_11] = m._11;
	data[MATI_12] = m._12;
	data[MATI_13] = m._13;
	data[MATI_14] = m._14;

	data[MATI_21] = m._21;
	data[MATI_22] = m._22;
	data[MATI_23] = m._23;
	data[MATI_24] = m._24;

	data[MATI_31] = m._31;
	data[MATI_32] = m._32;
	data[MATI_33] = m._33;
	data[MATI_34] = m._34;

	data[MATI_41] = m._41;
	data[MATI_42] = m._42;
	data[MATI_43] = m._43;
	data[MATI_44] = m._44;
}

ChaMatrix::ChaMatrix(DirectX::XMMATRIX m)
{
	data[MATI_11] = m.r[0].m128_f32[0];
	data[MATI_12] = m.r[0].m128_f32[1];
	data[MATI_13] = m.r[0].m128_f32[2];
	data[MATI_14] = m.r[0].m128_f32[3];

	data[MATI_21] = m.r[1].m128_f32[0];
	data[MATI_22] = m.r[1].m128_f32[1];
	data[MATI_23] = m.r[1].m128_f32[2];
	data[MATI_24] = m.r[1].m128_f32[3];

	data[MATI_31] = m.r[2].m128_f32[0];
	data[MATI_32] = m.r[2].m128_f32[1];
	data[MATI_33] = m.r[2].m128_f32[2];
	data[MATI_34] = m.r[2].m128_f32[3];

	data[MATI_41] = m.r[3].m128_f32[0];
	data[MATI_42] = m.r[3].m128_f32[1];
	data[MATI_43] = m.r[3].m128_f32[2];
	data[MATI_44] = m.r[3].m128_f32[3];

	//_11 = m.r[0].m128_f32[0];
	//_12 = m.r[1].m128_f32[0];
	//_13 = m.r[2].m128_f32[0];
	//data[MATI_14] = m.r[3].m128_f32[0];

	//data[MATI_21] = m.r[0].m128_f32[1];
	//data[MATI_22] = m.r[1].m128_f32[1];
	//data[MATI_23] = m.r[2].m128_f32[1];
	//data[MATI_24] = m.r[3].m128_f32[1];

	//data[MATI_31] = m.r[0].m128_f32[2];
	//data[MATI_32] = m.r[1].m128_f32[2];
	//data[MATI_33] = m.r[2].m128_f32[2];
	//data[MATI_34] = m.r[3].m128_f32[2];

	//data[MATI_41] = m.r[0].m128_f32[3];
	//data[MATI_42] = m.r[1].m128_f32[3];
	//data[MATI_43] = m.r[2].m128_f32[3];
	//data[MATI_44] = m.r[3].m128_f32[3];

};
//#endif


ChaMatrix::~ChaMatrix()
{


}

CQuaternion ChaMatrix::GetRotQ()
{
	CQuaternion retq;
	retq.RotationMatrix(*this);
	return retq;
}

int ChaMatrix::MakeLookAt(ChaVector3 srcpos, ChaVector3 srctarget, ChaVector3 srcupvec)
{
	SetIdentity();
	Matrix tmpresult;
	Vector3 tmppos, tmptarget, tmpup;
	tmpresult.SetIdentity();
	tmppos.Set(srcpos.x, srcpos.y, srcpos.z);
	tmptarget.Set(srctarget.x, srctarget.y, srctarget.z);
	tmpup.Set(srcupvec.x, srcupvec.y, srcupvec.z);

	tmpresult.MakeLookAt(tmppos, tmptarget, tmpup);
	
	this->SetParams(tmpresult);
	return 0;
}



ChaMatrix ChaMatrix::operator= (ChaMatrix m) { 

	//this->data[MATI_11] = m.data[MATI_11]; 
	//this->data[MATI_12] = m.data[MATI_12]; 
	//this->data[MATI_13] = m.data[MATI_13];
	//this->data[MATI_14] = m.data[MATI_14];
	//
	//this->data[MATI_21] = m.data[MATI_21];
	//this->data[MATI_22] = m.data[MATI_22];
	//this->data[MATI_23] = m.data[MATI_23];
	//this->data[MATI_24] = m.data[MATI_24];

	//this->data[MATI_31] = m.data[MATI_31];
	//this->data[MATI_32] = m.data[MATI_32];
	//this->data[MATI_33] = m.data[MATI_33];
	//this->data[MATI_34] = m.data[MATI_34];

	//this->data[MATI_41] = m.data[MATI_41];
	//this->data[MATI_42] = m.data[MATI_42];
	//this->data[MATI_43] = m.data[MATI_43];
	//this->data[MATI_44] = m.data[MATI_44];

	this->mVec[0] = m.mVec[0];
	this->mVec[1] = m.mVec[1];
	this->mVec[2] = m.mVec[2];
	this->mVec[3] = m.mVec[3];

	return *this;
};



ChaMatrix ChaMatrix::operator* (float srcw) const {
	ChaMatrix retmat;

	//__m128 multcoef = _mm_setr_ps(srcw, srcw, srcw, srcw);
	//retmat.mVec[0] = _mm_mul_ps(multcoef, mVec[0]);
	//retmat.mVec[1] = _mm_mul_ps(multcoef, mVec[1]);
	//retmat.mVec[2] = _mm_mul_ps(multcoef, mVec[2]);
	//retmat.mVec[3] = _mm_mul_ps(multcoef, mVec[3]);
	//return retmat;

	return ChaMatrix((float)((double)this->data[MATI_11] * (double)srcw), (float)((double)this->data[MATI_12] * (double)srcw), (float)((double)this->data[MATI_13] * (double)srcw), (float)((double)this->data[MATI_14] * (double)srcw),
		(float)((double)this->data[MATI_21] * (double)srcw), (float)((double)this->data[MATI_22] * (double)srcw), (float)((double)this->data[MATI_23] * (double)srcw), (float)((double)this->data[MATI_24] * (double)srcw),
		(float)((double)this->data[MATI_31] * (double)srcw), (float)((double)this->data[MATI_32] * (double)srcw), (float)((double)this->data[MATI_33] * (double)srcw), (float)((double)this->data[MATI_34] * (double)srcw),
		(float)((double)this->data[MATI_41] * (double)srcw), (float)((double)this->data[MATI_42] * (double)srcw), (float)((double)this->data[MATI_43] * (double)srcw), (float)((double)this->data[MATI_44] * (double)srcw));
}
ChaMatrix &ChaMatrix::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaMatrix ChaMatrix::operator/ (float srcw) const {
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	////if (fabs(srcw) >= 1e-5) {
	//if (fabs(srcw) >= FLT_MIN) {//2022/11/23
	//	const __m128 inverseOne = _mm_setr_ps(1.f, 1.f, 1.f, 1.f);
	//	__m128 divcoef = _mm_setr_ps(srcw, srcw, srcw, srcw);
	//	__m128 multcoef = _mm_div_ps(inverseOne, divcoef);
	//	retmat.mVec[0] = _mm_mul_ps(multcoef, mVec[0]);
	//	retmat.mVec[1] = _mm_mul_ps(multcoef, mVec[1]);
	//	retmat.mVec[2] = _mm_mul_ps(multcoef, mVec[2]);
	//	retmat.mVec[3] = _mm_mul_ps(multcoef, mVec[3]);
	//	return retmat;
	//}
	//else {
	//	ChaMatrixIdentity(&retmat);
	//	return retmat;
	//}

	if (srcw != 0.0f) {
		return ChaMatrix((float)((double)this->data[MATI_11] / (double)srcw), (float)((double)this->data[MATI_12] / (double)srcw), (float)((double)this->data[MATI_13] / (double)srcw), (float)((double)this->data[MATI_14] / (double)srcw),
			(float)((double)this->data[MATI_21] / (double)srcw), (float)((double)this->data[MATI_22] / (double)srcw), (float)((double)this->data[MATI_23] / (double)srcw), (float)((double)this->data[MATI_24] / (double)srcw),
			(float)((double)this->data[MATI_31] / (double)srcw), (float)((double)this->data[MATI_32] / (double)srcw), (float)((double)this->data[MATI_33] / (double)srcw), (float)((double)this->data[MATI_34] / (double)srcw),
			(float)((double)this->data[MATI_41] / (double)srcw), (float)((double)this->data[MATI_42] / (double)srcw), (float)((double)this->data[MATI_43] / (double)srcw), (float)((double)this->data[MATI_44] / (double)srcw));
	}
	else {
		ChaMatrixIdentity(&retmat);
		return retmat;
	}
}
ChaMatrix &ChaMatrix::operator/= (float srcw) { *this = *this / srcw; return *this; }
ChaMatrix ChaMatrix::operator* (double srcw) const {
	return ChaMatrix((float)((double)this->data[MATI_11] * srcw), (float)((double)this->data[MATI_12] * srcw), (float)((double)this->data[MATI_13] * srcw), (float)((double)this->data[MATI_14] * srcw),
		(float)((double)this->data[MATI_21] * srcw), (float)((double)this->data[MATI_22] * srcw), (float)((double)this->data[MATI_23] * srcw), (float)((double)this->data[MATI_24] * srcw),
		(float)((double)this->data[MATI_31] * srcw), (float)((double)this->data[MATI_32] * srcw), (float)((double)this->data[MATI_33] * srcw), (float)((double)this->data[MATI_34] * srcw),
		(float)((double)this->data[MATI_41] * srcw), (float)((double)this->data[MATI_42] * srcw), (float)((double)this->data[MATI_43] * srcw), (float)((double)this->data[MATI_44] * srcw));
}
ChaMatrix& ChaMatrix::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaMatrix ChaMatrix::operator/ (double srcw) const {
	if (srcw != 0.0) {
		return ChaMatrix((float)((double)this->data[MATI_11] / srcw), (float)((double)this->data[MATI_12] / srcw), (float)((double)this->data[MATI_13] / srcw), (float)((double)this->data[MATI_14] / srcw),
			(float)((double)this->data[MATI_21] / srcw), (float)((double)this->data[MATI_22] / srcw), (float)((double)this->data[MATI_23] / srcw), (float)((double)this->data[MATI_24] / srcw),
			(float)((double)this->data[MATI_31] / srcw), (float)((double)this->data[MATI_32] / srcw), (float)((double)this->data[MATI_33] / srcw), (float)((double)this->data[MATI_34] / srcw),
			(float)((double)this->data[MATI_41] / srcw), (float)((double)this->data[MATI_42] / srcw), (float)((double)this->data[MATI_43] / srcw), (float)((double)this->data[MATI_44] / srcw));
	}
	else {
		ChaMatrix retmat;
		ChaMatrixIdentity(&retmat);
		return retmat;
	}
}
ChaMatrix& ChaMatrix::operator/= (double srcw) { *this = *this / srcw; return *this; }



ChaMatrix ChaMatrix::operator+ (const ChaMatrix &m) const {
	//ChaMatrix retmat;
	//retmat.mVec[0] = _mm_add_ps(m.mVec[0], mVec[0]);
	//retmat.mVec[1] = _mm_add_ps(m.mVec[1], mVec[1]);
	//retmat.mVec[2] = _mm_add_ps(m.mVec[2], mVec[2]);
	//retmat.mVec[3] = _mm_add_ps(m.mVec[3], mVec[3]);
	//return retmat;

	return ChaMatrix(
		data[MATI_11] + m.data[MATI_11], data[MATI_12] + m.data[MATI_12], data[MATI_13] + m.data[MATI_13], data[MATI_14] + m.data[MATI_14],
		data[MATI_21] + m.data[MATI_21], data[MATI_22] + m.data[MATI_22], data[MATI_23] + m.data[MATI_23], data[MATI_24] + m.data[MATI_24],
		data[MATI_31] + m.data[MATI_31], data[MATI_32] + m.data[MATI_32], data[MATI_33] + m.data[MATI_33], data[MATI_34] + m.data[MATI_34],
		data[MATI_41] + m.data[MATI_41], data[MATI_42] + m.data[MATI_42], data[MATI_43] + m.data[MATI_43], data[MATI_44] + m.data[MATI_44]
		);
}
ChaMatrix &ChaMatrix::operator+= (const ChaMatrix &m) { *this = *this + m; return *this; }

ChaMatrix ChaMatrix::operator- (const ChaMatrix &m) const { 
	//ChaMatrix retmat;
	//retmat.mVec[0] = _mm_sub_ps(mVec[0], m.mVec[0]);
	//retmat.mVec[1] = _mm_sub_ps(mVec[1], m.mVec[1]);
	//retmat.mVec[2] = _mm_sub_ps(mVec[2], m.mVec[2]);
	//retmat.mVec[3] = _mm_sub_ps(mVec[3], m.mVec[3]);
	//return retmat;

	return ChaMatrix(
	data[MATI_11] - m.data[MATI_11], data[MATI_12] - m.data[MATI_12], data[MATI_13] - m.data[MATI_13], data[MATI_14] - m.data[MATI_14],
	data[MATI_21] - m.data[MATI_21], data[MATI_22] - m.data[MATI_22], data[MATI_23] - m.data[MATI_23], data[MATI_24] - m.data[MATI_24],
	data[MATI_31] - m.data[MATI_31], data[MATI_32] - m.data[MATI_32], data[MATI_33] - m.data[MATI_33], data[MATI_34] - m.data[MATI_34],
	data[MATI_41] - m.data[MATI_41], data[MATI_42] - m.data[MATI_42], data[MATI_43] - m.data[MATI_43], data[MATI_44] - m.data[MATI_44]
	); 
}
ChaMatrix &ChaMatrix::operator-= (const ChaMatrix &m) { *this = *this - m; return *this; }

ChaMatrix ChaMatrix::operator* (const ChaMatrix& m) const {

	//ChaMatrix res;
	//__m128  xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	//
	////xmm4 = _mm_loadu_ps(&m.data[MATI_11]);
	////xmm5 = _mm_loadu_ps(&m.data[MATI_21]);
	////xmm6 = _mm_loadu_ps(&m.data[MATI_31]);
	////xmm7 = _mm_loadu_ps(&m.data[MATI_41]);
	//xmm4 = m.mVec[0];
	//xmm5 = m.mVec[1];
	//xmm6 = m.mVec[2];
	//xmm7 = m.mVec[3];

	//// column0
	//xmm0 = _mm_load1_ps(&data[MATI_11]);
	//xmm1 = _mm_load1_ps(&data[MATI_12]);
	//xmm2 = _mm_load1_ps(&data[MATI_13]);
	//xmm3 = _mm_load1_ps(&data[MATI_14]);
	//
	////xmm0 = _mm_mul_ps(xmm0, xmm4);
	////xmm1 = _mm_mul_ps(xmm1, xmm5);
	////xmm2 = _mm_mul_ps(xmm2, xmm6);
	////xmm3 = _mm_mul_ps(xmm3, xmm7);
	////
	////xmm0 = _mm_add_ps(xmm0, xmm1);
	////xmm2 = _mm_add_ps(xmm2, xmm3);
	////xmm0 = _mm_add_ps(xmm0, xmm2);
	////_mm_storeu_ps(&res.data[MATI_11], xmm0);

	//xmm0 = _mm_mul_ps(xmm0, xmm4);
	//xmm1 = _mm_madd_ps(xmm1, xmm5, xmm0);
	//xmm2 = _mm_madd_ps(xmm2, xmm6, xmm1);
	//xmm3 = _mm_madd_ps(xmm3, xmm7, xmm2);

	//res.mVec[0] = xmm3;

	//
	//// column1
	//xmm0 = _mm_load1_ps(&data[MATI_21]);
	//xmm1 = _mm_load1_ps(&data[MATI_22]);
	//xmm2 = _mm_load1_ps(&data[MATI_23]);
	//xmm3 = _mm_load1_ps(&data[MATI_24]);
	//
	////xmm0 = _mm_mul_ps(xmm0, xmm4);
	////xmm1 = _mm_mul_ps(xmm1, xmm5);
	////xmm2 = _mm_mul_ps(xmm2, xmm6);
	////xmm3 = _mm_mul_ps(xmm3, xmm7);
	////
	////xmm0 = _mm_add_ps(xmm0, xmm1);
	////xmm2 = _mm_add_ps(xmm2, xmm3);
	////xmm0 = _mm_add_ps(xmm0, xmm2);
	////
	////_mm_storeu_ps(&res.data[MATI_21], xmm0);

	//xmm0 = _mm_mul_ps(xmm0, xmm4);
	//xmm1 = _mm_madd_ps(xmm1, xmm5, xmm0);
	//xmm2 = _mm_madd_ps(xmm2, xmm6, xmm1);
	//xmm3 = _mm_madd_ps(xmm3, xmm7, xmm2);

	//res.mVec[1] = xmm3;


	//// column2
	//xmm0 = _mm_load1_ps(&data[MATI_31]);
	//xmm1 = _mm_load1_ps(&data[MATI_32]);
	//xmm2 = _mm_load1_ps(&data[MATI_33]);
	//xmm3 = _mm_load1_ps(&data[MATI_34]);
	//
	////xmm0 = _mm_mul_ps(xmm0, xmm4);
	////xmm1 = _mm_mul_ps(xmm1, xmm5);
	////xmm2 = _mm_mul_ps(xmm2, xmm6);
	////xmm3 = _mm_mul_ps(xmm3, xmm7);
	////
	////xmm0 = _mm_add_ps(xmm0, xmm1);
	////xmm2 = _mm_add_ps(xmm2, xmm3);
	////xmm0 = _mm_add_ps(xmm0, xmm2);
	////
	////_mm_storeu_ps(&res.data[MATI_31], xmm0);

	//xmm0 = _mm_mul_ps(xmm0, xmm4);
	//xmm1 = _mm_madd_ps(xmm1, xmm5, xmm0);
	//xmm2 = _mm_madd_ps(xmm2, xmm6, xmm1);
	//xmm3 = _mm_madd_ps(xmm3, xmm7, xmm2);

	//res.mVec[2] = xmm3;



	//// column3
	//xmm0 = _mm_load1_ps(&data[MATI_41]);
	//xmm1 = _mm_load1_ps(&data[MATI_42]);
	//xmm2 = _mm_load1_ps(&data[MATI_43]);
	//xmm3 = _mm_load1_ps(&data[MATI_44]);
	//
	////xmm0 = _mm_mul_ps(xmm0, xmm4);
	////xmm1 = _mm_mul_ps(xmm1, xmm5);
	////xmm2 = _mm_mul_ps(xmm2, xmm6);
	////xmm3 = _mm_mul_ps(xmm3, xmm7);
	////
	////xmm0 = _mm_add_ps(xmm0, xmm1);
	////xmm2 = _mm_add_ps(xmm2, xmm3);
	////xmm0 = _mm_add_ps(xmm0, xmm2);
	////
	////_mm_storeu_ps(&res.data[MATI_41], xmm0);

	//xmm0 = _mm_mul_ps(xmm0, xmm4);
	//xmm1 = _mm_madd_ps(xmm1, xmm5, xmm0);
	//xmm2 = _mm_madd_ps(xmm2, xmm6, xmm1);
	//xmm3 = _mm_madd_ps(xmm3, xmm7, xmm2);

	//res.mVec[3] = xmm3;


	//*this * m
	double m_11 = (double)m.data[MATI_11];
	double m_12 = (double)m.data[MATI_12];
	double m_13 = (double)m.data[MATI_13];
	double m_14 = (double)m.data[MATI_14];
	double m_21 = (double)m.data[MATI_21];
	double m_22 = (double)m.data[MATI_22];
	double m_23 = (double)m.data[MATI_23];
	double m_24 = (double)m.data[MATI_24];
	double m_31 = (double)m.data[MATI_31];
	double m_32 = (double)m.data[MATI_32];
	double m_33 = (double)m.data[MATI_33];
	double m_34 = (double)m.data[MATI_34];
	double m_41 = (double)m.data[MATI_41];
	double m_42 = (double)m.data[MATI_42];
	double m_43 = (double)m.data[MATI_43];
	double m_44 = (double)m.data[MATI_44];

	ChaMatrix res;
	res.data[MATI_11] = (float)(m_11 * (double)data[MATI_11] + m_21 * (double)data[MATI_12] + m_31 * (double)data[MATI_13] + m_41 * (double)data[MATI_14]);
	res.data[MATI_21] = (float)(m_11 * (double)data[MATI_21] + m_21 * (double)data[MATI_22] + m_31 * (double)data[MATI_23] + m_41 * (double)data[MATI_24]);
	res.data[MATI_31] = (float)(m_11 * (double)data[MATI_31] + m_21 * (double)data[MATI_32] + m_31 * (double)data[MATI_33] + m_41 * (double)data[MATI_34]);
	res.data[MATI_41] = (float)(m_11 * (double)data[MATI_41] + m_21 * (double)data[MATI_42] + m_31 * (double)data[MATI_43] + m_41 * (double)data[MATI_44]);

	res.data[MATI_12] = (float)(m_12 * (double)data[MATI_11] + m_22 * (double)data[MATI_12] + m_32 * (double)data[MATI_13] + m_42 * (double)data[MATI_14]);
	res.data[MATI_22] = (float)(m_12 * (double)data[MATI_21] + m_22 * (double)data[MATI_22] + m_32 * (double)data[MATI_23] + m_42 * (double)data[MATI_24]);
	res.data[MATI_32] = (float)(m_12 * (double)data[MATI_31] + m_22 * (double)data[MATI_32] + m_32 * (double)data[MATI_33] + m_42 * (double)data[MATI_34]);
	res.data[MATI_42] = (float)(m_12 * (double)data[MATI_41] + m_22 * (double)data[MATI_42] + m_32 * (double)data[MATI_43] + m_42 * (double)data[MATI_44]);

	res.data[MATI_13] = (float)(m_13 * (double)data[MATI_11] + m_23 * (double)data[MATI_12] + m_33 * (double)data[MATI_13] + m_43 * (double)data[MATI_14]);
	res.data[MATI_23] = (float)(m_13 * (double)data[MATI_21] + m_23 * (double)data[MATI_22] + m_33 * (double)data[MATI_23] + m_43 * (double)data[MATI_24]);
	res.data[MATI_33] = (float)(m_13 * (double)data[MATI_31] + m_23 * (double)data[MATI_32] + m_33 * (double)data[MATI_33] + m_43 * (double)data[MATI_34]);
	res.data[MATI_43] = (float)(m_13 * (double)data[MATI_41] + m_23 * (double)data[MATI_42] + m_33 * (double)data[MATI_43] + m_43 * (double)data[MATI_44]);

	res.data[MATI_14] = (float)(m_14 * (double)data[MATI_11] + m_24 * (double)data[MATI_12] + m_34 * (double)data[MATI_13] + m_44 * (double)data[MATI_14]);
	res.data[MATI_24] = (float)(m_14 * (double)data[MATI_21] + m_24 * (double)data[MATI_22] + m_34 * (double)data[MATI_23] + m_44 * (double)data[MATI_24]);
	res.data[MATI_34] = (float)(m_14 * (double)data[MATI_31] + m_24 * (double)data[MATI_32] + m_34 * (double)data[MATI_33] + m_44 * (double)data[MATI_34]);
	res.data[MATI_44] = (float)(m_14 * (double)data[MATI_41] + m_24 * (double)data[MATI_42] + m_34 * (double)data[MATI_43] + m_44 * (double)data[MATI_44]);

	return res;

}


ChaMatrix &ChaMatrix::operator*= (const ChaMatrix &m) { *this = *this * m; return *this; }
ChaMatrix ChaMatrix::operator- () const { return *this * -1.0; }


CQuaternion::CQuaternion()
{
	InitParams();
}

CQuaternion::CQuaternion(float srcw, float srcx, float srcy, float srcz)
{
	w = srcw;
	x = srcx;
	y = srcy;
	z = srcz;
}

void CQuaternion::InitParams()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}


CQuaternion::~CQuaternion()
{

}

int CQuaternion::SetParams(float srcw, float srcx, float srcy, float srcz)
{
	w = srcw;
	x = srcx;
	y = srcy;
	z = srcz;
	return 0;
}
//#ifdef CONVD3DX11
int CQuaternion::SetParams(DirectX::XMFLOAT4 srcxq)
{
	w = srcxq.w;
	x = srcxq.x;
	y = srcxq.y;
	z = srcxq.z;
	return 0;
}
//#endif

float CQuaternion::QuaternionLimitPhai(float srcphai) 
{
	//#######################################################
	//srcphaiを-180度から180度に直してからクォータニオンの設定をする
	//#######################################################

	float retphai = 0.0f;
	float tmpphai = srcphai;
	int dbgcnt = 0;

	if (srcphai > 0.0f) {
		//retphai = srcphai - (float)((int)(srcphai / (2.0f * (float)PAI))) * (2.0f * (float)PAI);
		//retphai = srcphai - (float)(((int)(srcphai / (float)PAI)) + 1) * (float)PAI;

		//for example : 190 --> -170
		while ((tmpphai >= -(float)PAI) && (tmpphai <= (float)PAI)) {
			tmpphai -= (2.0 * (float)PAI);
			dbgcnt++;
			if (dbgcnt >= 100) {
				tmpphai = srcphai;
				break;
			}
		}

		return tmpphai;
	}
	else if (srcphai < 0.0f) {
		//retphai = srcphai + (float)(((int)(srcphai / (float)PAI)) + 1) * (float)PAI;

		//for example : -190 --> 170

		while ((tmpphai >= -(float)PAI) && (tmpphai <= (float)PAI)) {
			tmpphai += (2.0 * (float)PAI);
			dbgcnt++;
			if (dbgcnt >= 100) {
				tmpphai = srcphai;
				break;
			}
		}

		return tmpphai;
	}
	else {
		return 0.0f;
	}
}

double CQuaternion::QuaternionLimitPhai(double srcphai)
{
	double retphai = 0.0;

	if (srcphai > 0.0) {
		retphai = srcphai - (double)((int)(srcphai / (2.0 * PAI))) * (2.0 * PAI);
		return retphai;
	}
	else if (srcphai < 0.0) {
		retphai = srcphai + (double)((int)(srcphai / (2.0 * PAI))) * (2.0 * PAI);
		return retphai;
	}
	else {
		return 0.0;
	}
}

int CQuaternion::SetAxisAndRot(ChaVector3 srcaxis, float phai)
{
	double dblphai = phai;
	SetAxisAndRot(srcaxis, dblphai);

	//float phai2;
	//float cos_phai2, sin_phai2;

	//phai2 = QuaternionLimitPhai(phai) * 0.5f;
	//cos_phai2 = cosf(phai2);
	//sin_phai2 = sinf(phai2);

	//w = cos_phai2;
	//x = srcaxis.x * sin_phai2;
	//y = srcaxis.y * sin_phai2;
	//z = srcaxis.z * sin_phai2;

	return 0;
}
int CQuaternion::SetAxisAndRot(ChaVector3 srcaxis, double phai)
{
	srcaxis.Normalize();

	double phai2;
	double cos_phai2, sin_phai2;

	phai2 = QuaternionLimitPhai(phai) * 0.5;
	cos_phai2 = cos(phai2);
	sin_phai2 = sin(phai2);

	w = (float)cos_phai2;
	x = (float)((double)srcaxis.x * sin_phai2);
	y = (float)((double)srcaxis.y * sin_phai2);
	z = (float)((double)srcaxis.z * sin_phai2);

	normalize();

	return 0;
}

//int CQuaternion::GetAxisAndRot( ChaVector3* axisvecptr, float* frad )
//{
//	D3DXQUATERNION tempq;
//
//	tempq.x = x;
//	tempq.y = y;
//	tempq.z = z;
//	tempq.w = w;
//
//	D3DXQuaternionToAxisAngle( &tempq, &(axisvecptr->D3DX()), frad );
//	
//	ChaVector3Normalize( axisvecptr, axisvecptr );
//
//	return 0;
//}


int CQuaternion::SetRotation(EFbxRotationOrder rotorder, CQuaternion* srcaxisq, ChaVector3 srceul)
{
	CQuaternion axisQ, invaxisQ;
	if (srcaxisq) {
		axisQ = *srcaxisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}

	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;

	double phaix, phaiy, phaiz;
	if (fabs(srceul.x) >= 1e-3) {
		phaix = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
		cosx = (float)cos(phaix * 0.5);
		sinx = (float)sin(phaix * 0.5);
		qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	}
	else {
		phaix = 0.0;
		cosx = 1.0f;
		sinx = 0.0f;
		qx.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}
	if (fabs(srceul.y) >= 1e-3) {
		phaiy = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
		cosy = (float)cos(phaiy * 0.5);
		siny = (float)sin(phaiy * 0.5);
		qy.SetParams(cosy, 0.0f, siny, 0.0f);
	}
	else {
		phaiy = 0.0;
		cosy = 1.0f;
		siny = 0.0f;
		qy.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}
	if (fabs(srceul.z) >= 1e-3) {
		phaiz = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
		cosz = (float)cos(phaiz * 0.5);
		sinz = (float)sin(phaiz * 0.5);
		qz.SetParams(cosz, 0.0f, 0.0f, sinz);
	}
	else {
		phaiz = 0.0;
		cosz = 1.0f;
		sinz = 0.0f;
		qz.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}
	//switch (rotorder) {
	//case eEulerXYZ:
	//	phaix = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	break;
	//case eEulerXZY:
	//	phaix = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	break;
	//case eEulerYZX:
	//	phaix = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	break;
	//case eEulerYXZ:
	//	phaix = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	break;
	//case eEulerZXY:
	//	phaix = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	break;
	//case eEulerZYX:
	//	phaix = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	break;
	//case eSphericXYZ:
	//	phaix = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	break;
	//default:
	//	phaix = QuaternionLimitPhai((double)srceul.x * DEG2PAI);
	//	phaiy = QuaternionLimitPhai((double)srceul.y * DEG2PAI);
	//	phaiz = QuaternionLimitPhai((double)srceul.z * DEG2PAI);
	//	break;
	//}



	switch (rotorder) {
	case eEulerXYZ:
		q = axisQ * qz * qy * qx * invaxisQ;
		break;
	case eEulerXZY:
		q = axisQ * qy * qz * qx * invaxisQ;
		break;
	case eEulerYZX:
		q = axisQ * qx * qz * qy * invaxisQ;
		break;
	case eEulerYXZ:
		q = axisQ * qz * qx * qy * invaxisQ;
		break;
	case eEulerZXY:
		q = axisQ * qy * qx * qz * invaxisQ;
		break;
	case eEulerZYX:
		q = axisQ * qx * qy * qz * invaxisQ;
		break;
	case eSphericXYZ:
		q = axisQ * qz * qy * qx * invaxisQ;
		break;
	default:
		q = axisQ * qz * qy * qx * invaxisQ;
		break;
	}

	*this = q;

	return 0;
}


int CQuaternion::SetRotationXYZ(CQuaternion* axisq, ChaVector3 srcdeg)
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。

	CQuaternion axisQ, invaxisQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}


	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;


	double phaix, phaiy, phaiz;
	phaix = QuaternionLimitPhai((double)srcdeg.x * DEG2PAI);
	phaiy = QuaternionLimitPhai((double)srcdeg.y * DEG2PAI);
	phaiz = QuaternionLimitPhai((double)srcdeg.z * DEG2PAI);

	cosx = (float)cos(phaix * 0.5);
	sinx = (float)sin(phaix * 0.5);
	cosy = (float)cos(phaiy * 0.5);
	siny = (float)sin(phaiy * 0.5);
	cosz = (float)cos(phaiz * 0.5);
	sinz = (float)sin(phaiz * 0.5);

	qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	qy.SetParams(cosy, 0.0f, siny, 0.0f);
	qz.SetParams(cosz, 0.0f, 0.0f, sinz);

	//q = axisQ * qy * qx * qz * invaxisQ;
	q = axisQ * qz * qy * qx * invaxisQ;


	*this = q;

	return 0;
}

int CQuaternion::SetRotationRadXYZ(CQuaternion* axisq, ChaVector3 srcrad)
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。

	CQuaternion axisQ, invaxisQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}


	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;


	double phaix, phaiy, phaiz;
	phaix = QuaternionLimitPhai((double)srcrad.x);
	phaiy = QuaternionLimitPhai((double)srcrad.y);
	phaiz = QuaternionLimitPhai((double)srcrad.z);

	cosx = (float)cos(phaix * 0.5);
	sinx = (float)sin(phaix * 0.5);
	cosy = (float)cos(phaiy * 0.5);
	siny = (float)sin(phaiy * 0.5);
	cosz = (float)cos(phaiz * 0.5);
	sinz = (float)sin(phaiz * 0.5);

	qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	qy.SetParams(cosy, 0.0f, siny, 0.0f);
	qz.SetParams(cosz, 0.0f, 0.0f, sinz);

	//q = axisQ * qy * qx * qz * invaxisQ;
	q = axisQ * qz * qy * qx * invaxisQ;


	*this = q;

	return 0;
}


int CQuaternion::SetRotationXYZ(CQuaternion* axisq, double degx, double degy, double degz)
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。
	CQuaternion axisQ, invaxisQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}

	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;

	double phaix, phaiy, phaiz;
	phaix = QuaternionLimitPhai(degx * DEG2PAI);
	phaiy = QuaternionLimitPhai(degy * DEG2PAI);
	phaiz = QuaternionLimitPhai(degz * DEG2PAI);

	cosx = (float)cos(phaix * 0.5);
	sinx = (float)sin(phaix * 0.5);
	cosy = (float)cos(phaiy * 0.5);
	siny = (float)sin(phaiy * 0.5);
	cosz = (float)cos(phaiz * 0.5);
	sinz = (float)sin(phaiz * 0.5);

	qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	qy.SetParams(cosy, 0.0f, siny, 0.0f);
	qz.SetParams(cosz, 0.0f, 0.0f, sinz);

	//q = axisQ * qy * qx * qz * invaxisQ;
	q = axisQ * qz * qy * qx * invaxisQ;

	*this = q;

	return 0;
}

int CQuaternion::SetRotationRadXYZ(CQuaternion* axisq, double radx, double rady, double radz)
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。
	CQuaternion axisQ, invaxisQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}

	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;

	double phaix, phaiy, phaiz;
	phaix = QuaternionLimitPhai(radx);
	phaiy = QuaternionLimitPhai(rady);
	phaiz = QuaternionLimitPhai(radz);

	cosx = (float)cos(phaix * 0.5);
	sinx = (float)sin(phaix * 0.5);
	cosy = (float)cos(phaiy * 0.5);
	siny = (float)sin(phaiy * 0.5);
	cosz = (float)cos(phaiz * 0.5);
	sinz = (float)sin(phaiz * 0.5);

	qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	qy.SetParams(cosy, 0.0f, siny, 0.0f);
	qz.SetParams(cosz, 0.0f, 0.0f, sinz);

	//q = axisQ * qy * qx * qz * invaxisQ;
	q = axisQ * qz * qy * qx * invaxisQ;

	*this = q;

	return 0;
}


int CQuaternion::SetRotationZXY(CQuaternion* axisq, ChaVector3 srcdeg)
{
	// Z軸、X軸、Y軸の順番で、回転する、クォータニオンをセットする。

	CQuaternion axisQ, invaxisQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}


	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;

	double phaix, phaiy, phaiz;
	phaix = QuaternionLimitPhai((double)srcdeg.x * DEG2PAI);
	phaiy = QuaternionLimitPhai((double)srcdeg.y * DEG2PAI);
	phaiz = QuaternionLimitPhai((double)srcdeg.z * DEG2PAI);

	cosx = (float)cos(phaix * 0.5);
	sinx = (float)sin(phaix * 0.5);
	cosy = (float)cos(phaiy * 0.5);
	siny = (float)sin(phaiy * 0.5);
	cosz = (float)cos(phaiz * 0.5);
	sinz = (float)sin(phaiz * 0.5);


	qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	qy.SetParams(cosy, 0.0f, siny, 0.0f);
	qz.SetParams(cosz, 0.0f, 0.0f, sinz);

	q = axisQ * qy * qx * qz * invaxisQ;


	*this = q;

	return 0;
}

int CQuaternion::SetRotationZXY(CQuaternion* axisq, double degx, double degy, double degz)
{
	// Z軸、X軸、Y軸の順番で、回転する、クォータニオンをセットする。
	CQuaternion axisQ, invaxisQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}

	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;


	double phaix, phaiy, phaiz;
	phaix = QuaternionLimitPhai(degx * DEG2PAI);
	phaiy = QuaternionLimitPhai(degy * DEG2PAI);
	phaiz = QuaternionLimitPhai(degz * DEG2PAI);

	cosx = (float)cos(phaix * 0.5);
	sinx = (float)sin(phaix * 0.5);
	cosy = (float)cos(phaiy * 0.5);
	siny = (float)sin(phaiy * 0.5);
	cosz = (float)cos(phaiz * 0.5);
	sinz = (float)sin(phaiz * 0.5);

	qx.SetParams(cosx, sinx, 0.0f, 0.0f);
	qy.SetParams(cosy, 0.0f, siny, 0.0f);
	qz.SetParams(cosz, 0.0f, 0.0f, sinz);

	q = axisQ * qy * qx * qz * invaxisQ;

	*this = q;

	return 0;
}


CQuaternion CQuaternion::operator= (CQuaternion q) { this->x = q.x; this->y = q.y; this->z = q.z; this->w = q.w; return *this; };
CQuaternion CQuaternion::operator* (float srcw) const { return CQuaternion((float)((double)this->w * (double)srcw), (float)((double)this->x * (double)srcw), (float)((double)this->y * (double)srcw), (float)((double)this->z * (double)srcw)); }
CQuaternion &CQuaternion::operator*= (float srcw) { *this = *this * srcw; return *this; }
CQuaternion CQuaternion::operator/ (float srcw) const { if (srcw != 0.0f) { return CQuaternion((float)((double)this->w / (double)srcw), (float)((double)this->x / (double)srcw), (float)((double)this->y / (double)srcw), (float)((double)this->z / (double)srcw)); } else { return CQuaternion(1.0f, 0.0f, 0.0f, 0.0f); } }
CQuaternion &CQuaternion::operator/= (float srcw) { *this = *this / srcw; return *this; }
CQuaternion CQuaternion::operator* (double srcw) const { return CQuaternion((float)((double)this->w * srcw), (float)((double)this->x * srcw), (float)((double)this->y * srcw), (float)((double)this->z * srcw)); }
CQuaternion& CQuaternion::operator*= (double srcw) { *this = *this * srcw; return *this; }
CQuaternion CQuaternion::operator/ (double srcw) const { if (srcw != 0.0) { return CQuaternion((float)((double)this->w / srcw), (float)((double)this->x / srcw), (float)((double)this->y / srcw), (float)((double)this->z / srcw)); } else { return CQuaternion(1.0f, 0.0f, 0.0f, 0.0f); } }
CQuaternion& CQuaternion::operator/= (double srcw) { *this = *this / srcw; return *this; }
CQuaternion CQuaternion::operator+ (const CQuaternion &q) const { return CQuaternion(w + q.w, x + q.x, y + q.y, z + q.z); }
CQuaternion &CQuaternion::operator+= (const CQuaternion &q) { *this = *this + q; return *this; }
CQuaternion CQuaternion::operator- (const CQuaternion &q) const { return CQuaternion(w - q.w, x - q.x, y - q.y, z - q.z); }
CQuaternion &CQuaternion::operator-= (const CQuaternion &q) { *this = *this - q; return *this; }
CQuaternion CQuaternion::operator* (const CQuaternion &q) const {
	//return CQuaternion(
	//	w * q.w - x * q.x - y * q.y - z * q.z,
	//	w * q.x + q.w * x + y * q.z - z * q.y,
	//	w * q.y + q.w * y + z * q.x - x * q.z,
	//	w * q.z + q.w * z + x * q.y - y * q.x ).normalize();
	double tmpx, tmpy, tmpz, tmpw;
	tmpw = (double)w * (double)q.w - (double)x * (double)q.x - (double)y * (double)q.y - (double)z * (double)q.z;
	tmpx = (double)w * (double)q.x + (double)q.w * (double)x + (double)y * (double)q.z - (double)z * (double)q.y;
	tmpy = (double)w * (double)q.y + (double)q.w * (double)y + (double)z * (double)q.x - (double)x * (double)q.z;
	tmpz = (double)w * (double)q.z + (double)q.w * (double)z + (double)x * (double)q.y - (double)y * (double)q.x;
	CQuaternion retq;
	retq.x = (float)tmpx;
	retq.y = (float)tmpy;
	retq.z = (float)tmpz;
	retq.w = (float)tmpw;
	return retq.normalize();
}
CQuaternion &CQuaternion::operator*= (const CQuaternion &q) { *this = *this * q; return *this; }
CQuaternion CQuaternion::operator- () const { return *this * -1.0; }
CQuaternion CQuaternion::normalize() {
	//float mag = w*w+x*x+y*y+z*z;
	//if( mag != 0.0f )
	//	return (*this)*(1.0f/(float)::sqrt(mag));
	//else
	//	return CQuaternion( 1.0f, 0.0f, 0.0f, 0.0f );

	double mag = (double)w * (double)w + (double)x * (double)x + (double)y * (double)y + (double)z * (double)z;
	if (mag != 0.0) {
		double divval = ::sqrt(mag);
		double tmpx;
		double tmpy;
		double tmpz;
		double tmpw;
		if (divval != 0.0) {
			tmpx = (double)x / divval;
			tmpy = (double)y / divval;
			tmpz = (double)z / divval;
			tmpw = (double)w / divval;
		}
		else {
			tmpx = 0.0;
			tmpy = 0.0;
			tmpz = 0.0;
			tmpw = 1.0;
		}
		this->x = (float)tmpx;
		this->y = (float)tmpy;
		this->z = (float)tmpz;
		this->w = (float)tmpw;
		return *this;
	}
	else {
		SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		return *this;
	}
}


ChaMatrix CQuaternion::MakeRotMatX()
{
	normalize();

	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	float data[16];
	ZeroMemory(data, sizeof(float) * 16);

	//転置
	data[MATI_11] = (float)(1.0 - 2.0 * ((double)y * (double)y + (double)z * (double)z));
	data[MATI_21] = (float)(2.0 * ((double)x * (double)y - (double)z * (double)w));
	data[MATI_31] = (float)(2.0 * ((double)z * (double)x + (double)w * (double)y));

	data[MATI_12] = (float)(2.0 * ((double)x * (double)y + (double)z * (double)w));
	data[MATI_22] = (float)(1.0 - 2.0 * ((double)z * (double)z + (double)x * (double)x));
	data[MATI_32] = (float)(2.0 * ((double)y * (double)z - (double)w * (double)x));

	data[MATI_13] = (float)(2.0 * ((double)z * (double)x - (double)w * (double)y));
	data[MATI_23] = (float)(2.0 * ((double)y * (double)z + (double)x * (double)w));
	data[MATI_33] = (float)(1.0 - 2.0 * ((double)y * (double)y + (double)x * (double)x));

	retmat.data[MATI_11] = (float)data[MATI_11];
	retmat.data[MATI_21] = (float)data[MATI_21];
	retmat.data[MATI_31] = (float)data[MATI_31];

	retmat.data[MATI_12] = (float)data[MATI_12];
	retmat.data[MATI_22] = (float)data[MATI_22];
	retmat.data[MATI_32] = (float)data[MATI_32];

	retmat.data[MATI_13] = (float)data[MATI_13];
	retmat.data[MATI_23] = (float)data[MATI_23];
	retmat.data[MATI_33] = (float)data[MATI_33];


	/*
	retmat.data[MATI_11] = 1.0 - 2.0 * (y * y + z * z);
	retmat._12 = 2.0 * (x * y - z * w);
	retmat._13 = 2.0 * (z * x + w * y);

	retmat.data[MATI_21] = 2.0 * (x * y + z * w);
	retmat.data[MATI_22] = 1.0 - 2.0 * (z * z + x * x);
	retmat.data[MATI_23] = 2.0 * (y * z - w * x);

	retmat.data[MATI_31] = 2.0 * (z * x - w * y);
	retmat.data[MATI_32] = 2.0 * (y * z + x * w);
	retmat.data[MATI_33] = 1.0 - 2.0 * (y * y + x * x);
	*/

	/*
	D3DXQUATERNION qx;
	qx.x = x;
	qx.y = y;
	qx.z = z;
	qx.w = w;
	ChaMatrixRotationQuaternion(&retmat, &qx);
	*/
	return retmat;
}


void CQuaternion::RotationMatrix(ChaMatrix srcmat)
{
	//転置前バージョン

	//CQuaternionは gpar * par * curの順で掛ける系
	//ChaMatrixは cur * par * gparの順で掛ける系
	//CQuaternionの時に転置してChaMatrixのときにそのままで計算が合う。

	CQuaternion tmpq;

	//スケールに関して正規化した回転からQuaternionを求める。そのためにSRTに分解する。
	ChaVector3 svec, tvec;
	ChaMatrix rmat;
	GetSRTMatrix(srcmat, &svec, &rmat, &tvec);

	float m[4][4];

	m[0][0] = rmat.data[MATI_11];
	m[0][1] = rmat.data[MATI_12];
	m[0][2] = rmat.data[MATI_13];
	m[0][3] = rmat.data[MATI_14];
	m[1][0] = rmat.data[MATI_21];
	m[1][1] = rmat.data[MATI_22];
	m[1][2] = rmat.data[MATI_23];
	m[1][3] = rmat.data[MATI_24];
	m[2][0] = rmat.data[MATI_31];
	m[2][1] = rmat.data[MATI_32];
	m[2][2] = rmat.data[MATI_33];
	m[2][3] = rmat.data[MATI_34];
	m[3][0] = rmat.data[MATI_41];
	m[3][1] = rmat.data[MATI_42];
	m[3][2] = rmat.data[MATI_43];
	m[3][3] = rmat.data[MATI_44];

	int i, maxi;
	FLOAT maxdiag;
	double S, trace;

	trace = (double)m[0][0] + (double)m[1][1] + (double)m[2][2] + 1.0;
	if (trace > 0.0)
	{
		tmpq.x = (float)(((double)m[1][2] - (double)m[2][1]) / (2.0 * sqrt(trace)));
		tmpq.y = (float)(((double)m[2][0] - (double)m[0][2]) / (2.0 * sqrt(trace)));
		tmpq.z = (float)(((double)m[0][1] - (double)m[1][0]) / (2.0 * sqrt(trace)));
		tmpq.w = (float)(sqrt(trace) / 2.0);
		tmpq.normalize();//2025/02/24
		*this = tmpq;
		return;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	maxi = 0;
	maxdiag = m[0][0];
	for (i = 1; i<3; i++)
	{
		if (m[i][i] > maxdiag)
		{
			maxi = i;
			maxdiag = m[i][i];
		}
	}
	switch (maxi)
	{
	case 0:
		S = 2.0 * sqrt(1.0 + (double)m[0][0] - (double)m[1][1] - (double)m[2][2]);
		if (S != 0.0) {
			tmpq.x = (float)(0.25 * S);
			tmpq.y = (float)(((double)m[0][1] + (double)m[1][0]) / S);
			tmpq.z = (float)(((double)m[0][2] + (double)m[2][0]) / S);
			tmpq.w = (float)(((double)m[1][2] - (double)m[2][1]) / S);
		}
		else {
			tmpq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		}
		break;
	case 1:
		S = 2.0 * sqrt(1.0 + (double)m[1][1] - (double)m[0][0] - (double)m[2][2]);
		if (S != 0.0) {
			tmpq.x = (float)(((double)m[0][1] + (double)m[1][0]) / S);
			tmpq.y = (float)(0.25 * S);
			tmpq.z = (float)(((double)m[1][2] + (double)m[2][1]) / S);
			tmpq.w = (float)(((double)m[2][0] - (double)m[0][2]) / S);
		}
		else {
			tmpq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		}
		break;
	case 2:
		S = 2.0 * sqrt(1.0 + (double)m[2][2] - (double)m[0][0] - (double)m[1][1]);
		if (S != 0.0) {
			tmpq.x = (float)(((double)m[0][2] + (double)m[2][0]) / S);
			tmpq.y = (float)(((double)m[1][2] + (double)m[2][1]) / S);
			tmpq.z = (float)(0.25 * S);
			tmpq.w = (float)(((double)m[0][1] - (double)m[1][0]) / S);
		}
		else {
			tmpq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		}
		break;
	}
	tmpq.normalize();//2025/02/24
	*this = tmpq;


	//ChaMatrix tmpmat;
	//tmpmat = srcmat;
	//tmpmat.data[MATI_41] = 0.0f;
	//tmpmat.data[MATI_42] = 0.0f;
	//tmpmat.data[MATI_43] = 0.0f;

	//D3DXQUATERNION qx;
	//D3DXQuaternionRotationMatrix(&qx, &tmpmat);
	//SetParams(qx);
}

/*
void CQuaternion::RotationMatrix(ChaMatrix srcmat)
{
//転置後バージョン

CQuaternion tmpq;

//スケールに関して正規化した回転からQuaternionを求める。そのためにSRTに分解する。
ChaVector3 svec, tvec;
ChaMatrix rmat;
GetSRTMatrix(srcmat, &svec, &rmat, &tvec);

int i, maxi;
FLOAT maxdiag, S, trace;

trace = rmat.m[0][0] + rmat.m[1][1] + rmat.m[2][2] + 1.0f;
if (trace > 0.0f)
{
tmpq.x = (rmat.m[2][1] - rmat.m[1][2]) / (2.0f * sqrt(trace));
tmpq.y = (rmat.m[0][2] - rmat.m[2][0]) / (2.0f * sqrt(trace));
tmpq.z = (rmat.m[1][0] - rmat.m[0][1]) / (2.0f * sqrt(trace));
tmpq.w = sqrt(trace) / 2.0f;
*this = tmpq;
return;
}
maxi = 0;
maxdiag = rmat.m[0][0];
for (i = 1; i<3; i++)
{
if (rmat.m[i][i] > maxdiag)
{
maxi = i;
maxdiag = rmat.m[i][i];
}
}
switch (maxi)
{
case 0:
S = 2.0f * sqrt(1.0f + rmat.m[0][0] - rmat.m[1][1] - rmat.m[2][2]);
tmpq.x = 0.25f * S;
tmpq.y = (rmat.m[1][0] + rmat.m[0][1]) / S;
tmpq.z = (rmat.m[2][0] + rmat.m[0][2]) / S;
tmpq.w = (rmat.m[2][1] - rmat.m[1][2]) / S;
break;
case 1:
S = 2.0f * sqrt(1.0f + rmat.m[1][1] - rmat.m[0][0] - rmat.m[2][2]);
tmpq.x = (rmat.m[1][0] + rmat.m[0][1]) / S;
tmpq.y = 0.25f * S;
tmpq.z = (rmat.m[2][1] + rmat.m[1][2]) / S;
tmpq.w = (rmat.m[0][2] - rmat.m[2][0]) / S;
break;
case 2:
S = 2.0f * sqrt(1.0f + rmat.m[2][2] - rmat.m[0][0] - rmat.m[1][1]);
tmpq.x = (rmat.m[2][0] + rmat.m[0][2]) / S;
tmpq.y = (rmat.m[2][1] + rmat.m[1][2]) / S;
tmpq.z = 0.25f * S;
tmpq.w = (rmat.m[1][0] - rmat.m[0][1]) / S;
break;
}
*this = tmpq;

}
*/

//float CQuaternion::DotProduct(CQuaternion srcq)
//{
//	float dot;
//	dot = w * srcq.w +
//		x * srcq.x +
//		y * srcq.y +
//		z * srcq.z;
//	return dot;
//}
double CQuaternion::DotProduct(CQuaternion srcq)
{
	double dot;
	dot = (double)w * (double)srcq.w +
		(double)x * (double)srcq.x +
		(double)y * (double)srcq.y +
		(double)z * (double)srcq.z;
	return dot;
}



double CQuaternion::CalcRad(CQuaternion srcq)
{
	double dot;
	double retrad;
	dot = this->DotProduct(srcq);

	//!!!!!!!!!!　注意　!!!!!!!!!!!!
	//!!!! dot が１より微妙に大きい値のとき、kakuには、無効な値(-1.#IN00)が入ってしまう。
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (dot > 1.0)
		dot = 1.0;
	if (dot < -1.0)
		dot = -1.0;
	
	if (fabs(dot) <= 1e-4) {//2025/02/24
		retrad = 0.0;
	}
	else {
		retrad = acos(dot);
	}
	return retrad;
}

int CQuaternion::Slerp2(CQuaternion endq, double t, CQuaternion* dstq)
{
	dstq->SetParams(1.0f, 0.0f, 0.0f, 0.0f);


	normalize();//2025/02/24
	endq.normalize();//2025/02/24

	//if (t == 1.0) {
	//	*dstq = endq;
	//	return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	//}
	//else if (t == 0.0) {
	//	*dstq = *this;
	//	return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	//}
	if (fabs(t - 1.0) <= 1e-4) {
		*dstq = endq;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	else if (fabs(t) <= 1e-4) {
		*dstq = *this;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	double kaku;
	kaku = this->CalcRad(endq);

	//if (kaku > (PI * 0.5)) {
	//	//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
	//	endq = -endq;
	//	kaku = this->CalcRad(endq);
	//	//_ASSERT(kaku <= (PI * 0.5));
	//}

	// sin( kaku ) == 0.0 付近を調整。
	//180度にはならないので（ならないようにするので）０度のみケア
	int kaku0flag = 0;
	//if ((kaku <= 1e-4) && (kaku >= -1e-4)) {
	//	kaku0flag = 1;
	//}

	double sinkaku = sin(kaku);
	if (sinkaku < -1.0) {
		sinkaku = -1.0;
	}
	else if (sinkaku > 1.0) {
		sinkaku = 1.0;
	}

	if (fabs(sinkaku) <= 1e-4) {
		kaku0flag = 1;
	}

	CQuaternion tmpq;
	tmpq.SetParams(w, x, y, z);

	double alpha, beta;
	if (kaku0flag == 0) {
		alpha = sin(kaku * (1.0 - t)) / sinkaku;
		beta = sin(kaku * t) / sinkaku;

		dstq->x = (float)((double)tmpq.x * alpha + (double)endq.x * beta);
		dstq->y = (float)((double)tmpq.y * alpha + (double)endq.y * beta);
		dstq->z = (float)((double)tmpq.z * alpha + (double)endq.z * beta);
		dstq->w = (float)((double)tmpq.w * alpha + (double)endq.w * beta);
		//		retq = tmpq * alpha + endq * beta;

		dstq->normalize();//2025/02/24
	}
	else {
		*dstq = *this;
	}
	return 0;
}

CQuaternion CQuaternion::Slerp(CQuaternion endq, int framenum, int frameno)
{

	CQuaternion retq;
	retq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	if (framenum == 0) {
		_ASSERT(0);
		return retq;
	}
	double t = (double)frameno / (double)framenum;
	Slerp2(endq, t, &retq);

	return retq;

	//double kaku;
	//kaku = this->CalcRad(endq);
	//if (kaku > (PI * 0.5)) {
	//	//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
	//	endq = -endq;
	//	kaku = this->CalcRad(endq);
	//	_ASSERT(kaku <= (PI * 0.5));
	//}
	//// sin( kaku ) == 0.0 付近を調整。
	////180度にはならないので（ならないようにするので）０度のみケア
	//int kaku0flag = 0;
	//if ((kaku <= 1e-4) && (kaku >= -1e-4)) {
	//	kaku0flag = 1;
	//	//DbgOut( "Quaternion : Slerp : kaku0flag 1 : dot %f, kaku %f\n", dot, kaku );
	//}
	//double t = (double)frameno / (double)framenum;
	//double alpha, beta;
	//if (kaku0flag == 0) {
	//	alpha = sin(kaku * (1.0 - t)) / sin(kaku);
	//	beta = sin(kaku * t) / sin(kaku);
	//	retq = *this * alpha + endq * beta;
	//}
	//else {
	//	retq = *this;
	//	//DbgOut( "MotionInfo : FillUpMotionPoint 2: frame %d, %f, %f, %f, %f\n",
	//	//	   frameno, startq.w, startq.x, startq.y, startq.z );
	//}
	//return retq;
}

//#ifdef CONVD3DX11
//int CQuaternion::Squad(CQuaternion q0, CQuaternion q1, CQuaternion q2, CQuaternion q3, float t)
//{
//
//	CQuaternion iq0, iq1, iq2, iq3;
//	D3DXQUATERNION qx0, qx1, qx2, qx3;
//	D3DXQUATERNION ax, bx, cx, resx;
//
//	q0.inv(&iq0);
//	q1.inv(&iq1);
//	q2.inv(&iq2);
//	q3.inv(&iq3);
//
//	Q2X(&qx0, iq0);
//	Q2X(&qx1, iq1);
//	Q2X(&qx2, iq2);
//	Q2X(&qx3, iq3);
//
//	D3DXQuaternionSquadSetup(&ax, &bx, &cx, &qx0, &qx1, &qx2, &qx3);
//	D3DXQuaternionSquad(&resx, &qx1, &ax, &bx, &cx, t);
//
//	D3DXQUATERNION iresx;
//	D3DXQuaternionInverse(&iresx, &resx);
//
//	this->x = iresx.x;
//	this->y = iresx.y;
//	this->z = iresx.z;
//	this->w = iresx.w;
//
//
//	return 0;
//}
int CQuaternion::Q2X(DirectX::XMFLOAT4* dstx)
{
	dstx->x = x;
	dstx->y = y;
	dstx->z = z;
	dstx->w = w;

	return 0;
}

int CQuaternion::Q2X(DirectX::XMFLOAT4* dstx, CQuaternion srcq)
{
	dstx->x = srcq.x;
	dstx->y = srcq.y;
	dstx->z = srcq.z;
	dstx->w = srcq.w;

	return 0;
}
//#endif

int CQuaternion::inv(CQuaternion* dstq)
{
	if (dstq) {
		if (IsInit() == 0) {//2022/12/29
			dstq->w = w;
			dstq->x = -x;
			dstq->y = -y;
			dstq->z = -z;

			*dstq = dstq->normalize();
		}
		else {
			dstq->SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

CQuaternion CQuaternion::inverse()
{
	CQuaternion retq;
	this->inv(&retq);
	return retq;
}

int CQuaternion::RotationArc(ChaVector3 srcvec0, ChaVector3 srcvec1)
{
	//srcvec0, srcvec1は、normalizeされているとする。

	ChaVector3 c;
	ChaVector3Cross(&c, (const ChaVector3*)&srcvec0, (const ChaVector3*)&srcvec1);
	double d;
	d = ChaVector3DotDbl(&srcvec0, &srcvec1);
	double mags = (1.0 + d) * 2.0;
	double s;
	if (mags != 0.0) {
		s = sqrt(mags);
		if (s != 0.0) {
			x = (float)((double)c.x / s);
			y = (float)((double)c.y / s);
			z = (float)((double)c.z / s);
			w = (float)(s / 2.0);
		}
		else {
			x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
		}
	}
	else {
		x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
	}
	
	return 0;
}


int CQuaternion::Rotate(ChaVector3* dstvec, ChaVector3 srcvec)
{
	if (!dstvec) {
		_ASSERT(0);
		return 1;
	}

	ChaVector3 tmpsrcvec = srcvec;


	ChaMatrix mat;
	mat.SetIdentity();
	mat = MakeRotMatX();
	mat.SetTranslation(ChaVector3(0.0f, 0.0f, 0.0f));//2023/06/20
	mat.data[MATI_44] = 0.0f;//2023/06/20

	ChaVector3TransformCoord(dstvec, &tmpsrcvec, &mat);

	return 0;
}

//int CQuaternion::QuaternionToAxisAngle( ChaVector3* dstaxis, float* dstrad )
//{
//	D3DXQUATERNION xq;
//
//	int ret;
//	ret = Q2X( &xq );
//	_ASSERT( !ret );
//
//	D3DXQuaternionToAxisAngle( &xq, &(dstaxis->D3DX()), dstrad );
//
//	return 0;
//}

int CQuaternion::transpose(CQuaternion* dstq)
{
	ChaMatrix matx;

	matx = MakeRotMatX();

	ChaMatrix tmatx;
	ChaMatrixTranspose(&tmatx, &matx);

	dstq->RotationMatrix(tmatx);

	return 0;
}

ChaMatrix CQuaternion::CalcSymX2()
{
	CQuaternion tmpq;
	tmpq = *this;
	tmpq.x *= -1.0;
	//tmpq.y *= -1.0f;
	//tmpq.z *= -1.0f;
	tmpq.w *= -1.0;

	return tmpq.MakeRotMatX();
}


int CQuaternion::CalcSym(CQuaternion* dstq)
{
	CQuaternion tmpq;
	tmpq = *this;
	tmpq.x *= -1.0;
	//tmpq.y *= -1.0f;
	//tmpq.z *= -1.0f;
	tmpq.w *= -1.0;

	*dstq = tmpq;

	return 0;
}

double CQuaternion::vecDotVec(ChaVector3* vec1, ChaVector3* vec2)
{
	double tmpval = (double)vec1->x * (double)vec2->x + (double)vec1->y * (double)vec2->y + (double)vec1->z * (double)vec2->z;
	return tmpval;
}

double CQuaternion::lengthVec(ChaVector3* vec)
{
	double mag;
	double leng;

	mag = (double)vec->x * (double)vec->x + (double)vec->y * (double)vec->y + (double)vec->z * (double)vec->z;
	if (mag == 0.0) {
		leng = 0.0;
	}
	else {
		leng = sqrt(mag);
	}
	return leng;
}

double CQuaternion::aCos(double dot)
{
	if (dot > 1.0)
		dot = 1.0;
	else if (dot < -1.0)
		dot = -1.0;

	double rad;
	rad = acos(dot);

	double degree;
	degree = (rad * PAI2DEG);

	return degree;
}

int CQuaternion::vec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ChaVector3 tmpsrcvec = *srcvec;


	ret = dirq.SetRotationXYZ(0, 0, deg, 0);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	double tmpx, tmpy, tmpz;
	tmpx = (double)dirm.data[MATI_11] * (double)tmpsrcvec.x + (double)dirm.data[MATI_21] * (double)tmpsrcvec.y + (double)dirm.data[MATI_31] * (double)tmpsrcvec.z + (double)dirm.data[MATI_41];
	tmpy = (double)dirm.data[MATI_12] * (double)tmpsrcvec.x + (double)dirm.data[MATI_22] * (double)tmpsrcvec.y + (double)dirm.data[MATI_32] * (double)tmpsrcvec.z + (double)dirm.data[MATI_42];
	tmpz = (double)dirm.data[MATI_13] * (double)tmpsrcvec.x + (double)dirm.data[MATI_23] * (double)tmpsrcvec.y + (double)dirm.data[MATI_33] * (double)tmpsrcvec.z + (double)dirm.data[MATI_43];

	dstvec->x = (float)tmpx;
	dstvec->y = (float)tmpy;
	dstvec->z = (float)tmpz;

	return 0;
}
int CQuaternion::vec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ChaVector3 tmpsrcvec = *srcvec;

	ret = dirq.SetRotationXYZ(0, deg, 0, 0);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	double tmpx, tmpy, tmpz;

	tmpx = (double)dirm.data[MATI_11] * (double)tmpsrcvec.x + (double)dirm.data[MATI_21] * (double)tmpsrcvec.y + (double)dirm.data[MATI_31] * (double)tmpsrcvec.z + (double)dirm.data[MATI_41];
	tmpy = (double)dirm.data[MATI_12] * (double)tmpsrcvec.x + (double)dirm.data[MATI_22] * (double)tmpsrcvec.y + (double)dirm.data[MATI_32] * (double)tmpsrcvec.z + (double)dirm.data[MATI_42];
	tmpz = (double)dirm.data[MATI_13] * (double)tmpsrcvec.x + (double)dirm.data[MATI_23] * (double)tmpsrcvec.y + (double)dirm.data[MATI_33] * (double)tmpsrcvec.z + (double)dirm.data[MATI_43];

	dstvec->x = (float)tmpx;
	dstvec->y = (float)tmpy;
	dstvec->z = (float)tmpz;


	return 0;
}
int CQuaternion::vec3RotateZ(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ChaVector3 tmpsrcvec = *srcvec;

	ret = dirq.SetRotationXYZ(0, 0, 0, deg);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	double tmpx, tmpy, tmpz;

	tmpx = (double)dirm.data[MATI_11] * (double)tmpsrcvec.x + (double)dirm.data[MATI_21] * (double)tmpsrcvec.y + (double)dirm.data[MATI_31] * (double)tmpsrcvec.z + (double)dirm.data[MATI_41];
	tmpy = (double)dirm.data[MATI_12] * (double)tmpsrcvec.x + (double)dirm.data[MATI_22] * (double)tmpsrcvec.y + (double)dirm.data[MATI_32] * (double)tmpsrcvec.z + (double)dirm.data[MATI_42];
	tmpz = (double)dirm.data[MATI_13] * (double)tmpsrcvec.x + (double)dirm.data[MATI_23] * (double)tmpsrcvec.y + (double)dirm.data[MATI_33] * (double)tmpsrcvec.z + (double)dirm.data[MATI_43];

	dstvec->x = (float)tmpx;
	dstvec->y = (float)tmpy;
	dstvec->z = (float)tmpz;

	return 0;
}

//int CQuaternion::Q2EulBt(ChaVector3* reteul)
//{
//	ChaVector3 Euler;
//
//
//	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
//	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
//	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);
//
//	ChaVector3 targetVec, shadowVec;
//	ChaVector3 tmpVec;
//
//	Rotate(&targetVec, axisZVec);
//	shadowVec.x = 0.0f;
//	shadowVec.y = (float)vecDotVec(&targetVec, &axisYVec);
//	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.x = 90.0f;
//	}
//	else {
//		Euler.x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) > 0.0) {
//		Euler.x = -Euler.x;
//	}
//
//	vec3RotateX(&tmpVec, -Euler.x, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = 0.0f;
//	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.y = 90.0f;
//	}
//	else {
//		Euler.y = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	//if( vecDotVec( &shadowVec, &axisXVec ) < 0.0f ){
//	if (vecDotVec(&shadowVec, &axisXVec) > 0.0) {
//		Euler.y = -Euler.y;
//	}
//
//
//	Rotate(&targetVec, axisYVec);
//	vec3RotateY(&tmpVec, -Euler.y, &targetVec);
//	targetVec = tmpVec;
//	vec3RotateX(&tmpVec, -Euler.x, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = 0.0f;
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.z = 90.0f;
//	}
//	else {
//		Euler.z = (float)aCos(vecDotVec(&shadowVec, &axisYVec) / lengthVec(&shadowVec));
//	}
//	//if( vecDotVec( &shadowVec, &axisXVec ) > 0.0f ){
//	if (vecDotVec(&shadowVec, &axisXVec) < 0.0) {
//		Euler.z = -Euler.z;
//	}
//
//
//	*reteul = Euler;
//
//	return 0;
//}
//
//int CQuaternion::Q2EulZXY(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul)
//{
//	return Q2Eul(axisq, befeul, reteul);
//}
//
//int CQuaternion::Q2EulYXZ(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul)
//{
//
//	CQuaternion axisQ, invaxisQ, EQ;
//	if (axisq) {
//		axisQ = *axisq;
//		axisQ.inv(&invaxisQ);
//		EQ = invaxisQ * *this * axisQ;
//	}
//	else {
//		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		EQ = *this;
//	}
//
//	ChaVector3 Euler;
//
//
//	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
//	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
//	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);
//
//	ChaVector3 targetVec, shadowVec;
//	ChaVector3 tmpVec;
//
//	EQ.Rotate(&targetVec, axisYVec);
//	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
//	shadowVec.y = (float)vecDotVec(&targetVec, &axisYVec);
//	shadowVec.z = 0.0f;
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.z = 90.0f;
//	}
//	else {
//		Euler.z = (float)aCos(vecDotVec(&shadowVec, &axisYVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) > 0.0) {
//		Euler.z = -Euler.z;
//	}
//
//	vec3RotateZ(&tmpVec, -Euler.z, &targetVec);
//	shadowVec.x = 0.0f;
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.x = 90.0f;
//	}
//	else {
//		Euler.x = (float)aCos(vecDotVec(&shadowVec, &axisYVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisZVec) < 0.0) {
//		Euler.x = -Euler.x;
//	}
//
//
//	EQ.Rotate(&targetVec, axisZVec);
//	vec3RotateZ(&tmpVec, -Euler.z, &targetVec);
//	targetVec = tmpVec;
//	vec3RotateX(&tmpVec, -Euler.x, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = 0.0f;
//	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.y = 90.0f;
//	}
//	else {
//		Euler.y = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) < 0.0) {
//		Euler.y = -Euler.y;
//	}
//
//	ModifyEuler(&Euler, &befeul);
//	*reteul = Euler;
//
//	return 0;
//}

int CQuaternion::Q2EulXYZusingMat(int rotorder, CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int notmodify180flag)
{
	const double DIVVALMIN = 1e-4;

	CQuaternion axisQ, invaxisQ, EQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
		EQ = invaxisQ * *this * axisQ;
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		EQ = *this;
	}

	ChaCalcFunc chacalcfunc;//2023/10/17
	ChaMatrix rotmat;
	rotmat = EQ.MakeRotMatX();

	double m00 = rotmat.data[MATI_11];
	double m01 = rotmat.data[MATI_12];
	double m02 = rotmat.data[MATI_13];

	double m10 = rotmat.data[MATI_21];
	double m11 = rotmat.data[MATI_22];
	double m12 = rotmat.data[MATI_23];

	double m20 = rotmat.data[MATI_31];
	double m21 = rotmat.data[MATI_32];
	double m22 = rotmat.data[MATI_33];

	//rotorder
	//#0:xyz
	//#1:yzx
	//#2:zxy
	//#3:xzy
	//#4:yxz
	//#5:zyx

	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	double alpha, beta, gamma;
	double cosbeta;

	if (rotorder == ROTORDER_XYZ) {
		//:#xyz
		gamma = atan2(m01, m00);
		beta = asin(-m02);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(m12 / cosbeta);
			if (m22 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(-m10, m11);
			beta = asin(-m02);
			alpha = 0.0;
		}
		x = alpha;
		y = beta;
		z = gamma;
	}
	else if(rotorder == ROTORDER_YZX) {
		//:#yzx
		gamma = atan2(m12, m11);
		beta = asin(-m10);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(m20 / cosbeta);
			if (m00 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(-m21, m22);
			beta = asin(-m10);
			alpha = 0.0;
		}
		x = gamma;
		y = alpha;
		z = beta;
	}
	else if (rotorder == ROTORDER_ZXY) {
		//#zxy
		gamma = atan2(m20, m22);
		beta = asin(-m21);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(m01 / cosbeta);
			if (m11 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(-m02, m00);
			beta = asin(-m21);
			alpha = 0.0;
		}
		x = beta;
		y = gamma;
		z = alpha;
	}
	else if (rotorder == ROTORDER_XZY) {
		//#xzy
		gamma = atan2(-m02, m00);
		beta = asin(m01);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(-m21 / cosbeta);
			if (m11 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(m20, m22);
			beta = asin(m01);
			alpha = 0.0;
		}
		x = alpha;
		y = gamma;
		z = beta;
	}
	else if (rotorder == ROTORDER_YXZ) {
		//#yxz
		gamma = atan2(-m10, m11);
		beta = asin(m12);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(-m02 / cosbeta);
			if (m22 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(m01, m00);
			beta = asin(m12);
			alpha = 0.0;
		}
		x = beta;
		y = alpha;
		z = gamma;
	}
	else if (rotorder == ROTORDER_ZYX) {
		//#zyx
		gamma = atan2(-m21, m22);
		beta = asin(m20);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(-m10 / cosbeta);
			if (m00 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(m12, m11);
			beta = asin(m20);
			alpha = 0.0;
		}
		x = gamma;
		y = beta;
		z = alpha;
	}
	else {
		//print('Q2Euler : unknown rotateOrder : calc using default order');
		_ASSERT(0);
		//:#xyz
		gamma = atan2(m01, m00);
		beta = asin(-m02);
		cosbeta = cos(beta);
		if (fabs(cosbeta) > DIVVALMIN) {
			alpha = asin(m12 / cosbeta);
			if (m22 < 0.0) {
				alpha = PAI - alpha;
			}
		}
		else {
			gamma = atan2(-m10, m11);
			beta = asin(-m02);
			alpha = 0.0;
		}
		x = alpha;
		y = beta;
		z = gamma;
	}

	const double PI2 = PAI * 2.0;

	if (x > PAI) {
		x -= PI2;
	}
	else if (x < -PAI) {
		x += PI2;
	}

	if (y > PAI) {
		y -= PI2;
	}
	else if (y < -PAI) {
		y += PI2;
	}
	
	if (z > PAI) {
		z -= PI2;
	}
	else if (z < -PAI) {
		z += PI2;
	}

	ChaVector3 Euler;
	Euler.x = (float)(x * 180.0 / PAI);
	Euler.y = (float)(y * 180.0 / PAI);
	Euler.z = (float)(z * 180.0 / PAI);
	//chacalcfunc.ModifyEuler360(&Euler, &befeul, notmodify180flag, 91.0f, 180.0f, 180.0f);
	chacalcfunc.ModifyEuler360(&Euler, &befeul, notmodify180flag, 179.0f, 179.0f, 179.0f);//2023/11/07


	*reteul = Euler;
	return 0;
}


int CQuaternion::Q2EulXYZusingQ(bool srcunderIKRot, bool srcunderRetarget, 
	CQuaternion* axisq, BEFEUL befeul, ChaVector3* reteul, int isfirstbone, int isendbone, int notmodify180flag)
{

	//2022/12/16 ZEROVECLEN
	const double ZEROVECLEN = 1e-6;
	//const double ZEROVECLEN = 1e-4;

	CQuaternion axisQ, invaxisQ, EQ;
	if (axisq) {
		axisQ = *axisq;
		axisQ.inv(&invaxisQ);
		EQ = invaxisQ * *this * axisQ;
	}
	else {
		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		EQ = *this;
	}
	
	ChaCalcFunc chacalcfunc;//2023/10/17

	ChaVector3 Euler;

	CQuaternion iniq;

	CQuaternion QinvZ;
	CQuaternion EQinvZ;
	ChaVector3 EinvZ;
	
	CQuaternion QinvY;
	CQuaternion EQinvYZ;
	ChaVector3 EinvY;

	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);

	ChaVector3 targetVec, shadowVec;
	ChaVector3 tmpVec;
	double shadowLeng;
	
	float tmpX0, tmpY0, tmpZ0;
	float tmpX1, tmpY1, tmpZ1;

	//const float thdeg = 165.0f;
	//const float thdeg = 90.0f;//2023/10/11

	float thdeg;
	if (isendbone == 0) {

		//if ((g_underRetargetFlag == true) ||
		//	//(GetParModel() && GetParModel()->GetLoadedFlag() == false) ||
		//	(g_underWriteFbx == true)) {
		//	thdeg = 135.0f;
		//}
		//else {
		//	thdeg = 91.0f;
		//}

		//thdeg = 91.0f;
		//thdeg = 135.0f;
		//thdeg = 115.0f;
		//thdeg = 125.0f;
		//thdeg = 165.0f;


		//if (g_underIKRot == true) {
		//	//thdeg = 91.0f;
		//	//thdeg = 98.0f;
		//	thdeg = 90.0f;
		//}
		//else {
		//	thdeg = 140.0f;
		//}

		//thdeg = 140.0f;
		//thdeg = 90.0f;
		

		//thdeg = 91.0f;//1.2.0.27, 1.2.0.28


		//2023/11/06
		//他の部分が直ったから？　直感的にも適当な値でうまくいった
		//bvh144_1, bvh144_2をリターゲットして　読み書き読み書きテストOK
		//thdeg = 181.0f;//1.2.0.29
		thdeg = g_thdeg;//2024/02/20
	}
	else {
		//thdeg = 180.0f;
		//thdeg = 360.0f;
		//thdeg = 91.0f;
		
		//thdeg = 165.0f;//1.2.0.27, 1.2.0.28, 1.2.0.29


		//2023/11/07
		//bvh144_1の足のつま先について　試行錯誤　
		//一番ましなところで妥協
		// 
		//thdeg = 181.0f;//最後から大分前から裏
		//thdeg = 155.0f;//最後の部分が裏
		//thdeg = 145.0f;//最後から少し前から裏
		//thdeg = 179.0f;//最後から大分前から裏
		//thdeg = 175.0f;//最後から大分前から裏
		//thdeg = 160.0f;//最後から大分前から裏
		//thdeg = 134.0f;//最後から大分前から裏
		//thdeg = 158.0f;//最後の部分が裏
		//thdeg = 143.0f;//最後から大分前から裏


		//2023/11/08
		//最後の部分が裏
		//プログラムとしてはこれ以上は難しい
		//最後の部分は使う側で角度制限の設定で対応する他にない
		//つま先のXのmin:-30, max:30(実際にはそんなに動かないが)で　かなり緩和
		//thdeg = 159.0f;//1.2.0.30
		thdeg = g_thdeg_endjoint;//2024/02/20
	}

	//2023/10/16
	//GetRound()では　180度以上のずれを１回転で補正していた
	//chacalcfunc.GetRoundThreshold()では　１回転よりどれだけ小さい角度で一回転とみなすか(使う側で足すのは３６０度単位なので姿勢は変わらない)を指定する(軸ごとに)
	//float throundX = 91.0f;
	//float throundY = 180.0f;
	//float throundZ = 180.0f;
	//float throundX = 91.0f;//1.2.0.27
	//float throundX = 181.0f;//1.2.0.28
	//float throundY = 181.0f;//1.2.0.28
	//float throundZ = 181.0f;//1.2.0.28
	//float throundX = 179.0f;//2023/11/06　bvh144_1も直った　オイラー角の３６０degree段差も直った
	//float throundY = 179.0f;//2023/11/06　bvh144_1も直った　オイラー角の３６０degree段差も直った
	//float throundZ = 179.0f;//2023/11/06　bvh144_1も直った　オイラー角の３６０degree段差も直った


	ChaVector3 validbefeul;//2023/10/14
	//if (g_underIKRotApplyFrame == true) {//2023/10/14
	//if (g_underIKRot == true) {//2023/10/16 IK処理のframe単位のマルチスレッド化の準備としてcurrentframeで計算　後処理でbefframeで計算
	//if ((g_underIKRot == true) || (g_underRetargetFlag == true)) {//2023/10/23 リターゲットもフレーム単位のマルチスレッド化したので　currentframeulを使う
	if (srcunderIKRot || srcunderRetarget) {
			validbefeul = befeul.currentframeeul;
	}
	else {
		validbefeul = befeul.befframeeul;
	}


	//#######################################################################################################
	//マルチスレッドのリターゲット時(befeul = 0.0(InitMp)時)には　modify180をしない　後処理でmodify180をする
	//#######################################################################################################
	//if (g_underRetargetFlag == true) {//2023/10/23
	if (srcunderRetarget) {
		notmodify180flag = 1;
	}


	EQ.Rotate(&targetVec, axisXVec);
	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
	shadowVec.y = (float)vecDotVec(&targetVec, &axisYVec);
	shadowVec.z = 0.0f;
	shadowLeng = lengthVec(&shadowVec);
	if (shadowLeng <= ZEROVECLEN) {
		Euler.z = 90.0f;
	}
	else {
		shadowVec = shadowVec / shadowLeng;//normalize
		Euler.z = (float)aCos(vecDotVec(&shadowVec, &axisXVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisYVec) < 0.0) {
		Euler.z = -Euler.z;
	}
	//if (vecDotVec(&shadowVec, &axisYVec) > 0.0f) {
	//	Euler.z = -Euler.z;
	//}

	{
		if (Euler.z >= 0.0f) {
			tmpZ0 = Euler.z + 360.0f * chacalcfunc.GetRoundThreshold((validbefeul.z - Euler.z) / 360.0f, g_thRoundZ);//オーバー１８０度
		}
		else {
			tmpZ0 = Euler.z - 360.0f * chacalcfunc.GetRoundThreshold((Euler.z - validbefeul.z) / 360.0f, g_thRoundZ);//オーバー１８０度
		}
		tmpZ1 = tmpZ0;
		//if (g_underIKRot == false) {//<--コメントアウトをはずすと　bvh144のリターゲットの太ももが変になる
		if (notmodify180flag == 0) {
			//180度(thdeg : 165度以上)の変化は　軸反転しないような表現に補正
			if ((tmpZ0 - validbefeul.z) >= thdeg) {
				tmpZ1 = tmpZ0 - 180.0f;
			}
			if ((validbefeul.z - tmpZ0) >= thdeg) {
				tmpZ1 = tmpZ0 + 180.0f;
			}
		}
		else {
			//tmpZ0そのまま
		}
		//}
		Euler.z = tmpZ1;
	}



	EinvZ.SetParams(0.0f, 0.0f, -Euler.z);
	QinvZ.SetRotationXYZ(&iniq, EinvZ);
	EQinvZ = QinvZ * EQ;
	EQinvZ.Rotate(&targetVec, axisXVec);
	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
	shadowVec.y = 0.0f;
	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);

	//vec3RotateZ(&tmpVec, -Euler.z, &targetVec);
	//shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
	//shadowVec.y = 0.0f;
	//shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);

	shadowLeng = lengthVec(&shadowVec);
	if (shadowLeng <= ZEROVECLEN) {
		Euler.y = 90.0f;
	}
	else {
		shadowVec = shadowVec / shadowLeng;//normalize
		Euler.y = (float)aCos(vecDotVec(&shadowVec, &axisXVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisZVec) > 0.0) {
		Euler.y = -Euler.y;
	}
	//if (vecDotVec(&shadowVec, &axisZVec) < 0.0f) {
	//	Euler.y = -Euler.y;
	//}

	{

		if (Euler.y >= 0.0f) {
			tmpY0 = Euler.y + 360.0f * chacalcfunc.GetRoundThreshold((validbefeul.y - Euler.y) / 360.0f, g_thRoundY);//オーバー１８０度
		}
		else {
			tmpY0 = Euler.y - 360.0f * chacalcfunc.GetRoundThreshold((Euler.y - validbefeul.y) / 360.0f, g_thRoundY);//オーバー１８０度
		}
		tmpY1 = tmpY0;
		//if (g_underIKRot == false) {//<--コメントアウトをはずすと　bvh144のリターゲットの太ももが変になる
		if (notmodify180flag == 0) {
			//180度(thdeg : 165度以上)の変化は　軸反転しないような表現に補正
			if ((tmpY0 - validbefeul.y) >= thdeg) {
				tmpY1 = tmpY0 - 180.0f;
			}
			if ((validbefeul.y - tmpY0) >= thdeg) {
				tmpY1 = tmpY0 + 180.0f;
			}
		}
		else {
			//tmpY0そのまま
		}
		//}
		Euler.y = tmpY1;
	}

	EinvY.SetParams(0.0f, -Euler.y, 0.0f);
	QinvY.SetRotationXYZ(&iniq, EinvY);
	EQinvYZ = QinvY * QinvZ * EQ;
	EQinvYZ.Rotate(&targetVec, axisZVec);
	shadowVec.x = 0.0f;
	shadowVec.y = (float)vecDotVec(&targetVec, &axisYVec);
	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);

	//EQ.Rotate(&targetVec, axisZVec);
	//vec3RotateZ(&tmpVec, -Euler.z, &targetVec);
	//targetVec = tmpVec;
	//vec3RotateY(&tmpVec, -Euler.y, &targetVec);
	//shadowVec.x = 0.0f;
	//shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
	//shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);

	shadowLeng = lengthVec(&shadowVec);
	if (shadowLeng <= ZEROVECLEN) {
		Euler.x = 90.0f;
	}
	else {
		shadowVec = shadowVec / shadowLeng;//normalize
		Euler.x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisYVec) > 0.0) {
		Euler.x = -Euler.x;
	}
	//if (vecDotVec(&shadowVec, &axisYVec) < 0.0f) {
	//	Euler.x = -Euler.x;
	//}


	{
		if (Euler.x >= 0.0f) {
			tmpX0 = Euler.x + 360.0f * chacalcfunc.GetRoundThreshold((validbefeul.x - Euler.x) / 360.0f, g_thRoundX);//オーバー１８０度
		}
		else {
			tmpX0 = Euler.x - 360.0f * chacalcfunc.GetRoundThreshold((Euler.x - validbefeul.x) / 360.0f, g_thRoundX);//オーバー１８０度
		}
	
		//2023/02/15
		//X軸の角度を180度補正しても　後続の軸が無いので　他の軸の計算に反映出来ない
		//しかし　補正を取り除いてしまうと　リターゲット結果がおかしいことがあるbvh121
		//取り除くと　IK中に　キャラクターが逆立ちしなくなる
		//##############
		//2023/02/23
		//いろいろテストした結果
		// 
		//回転が全てリセットされているところから　IKする場合には　
		//X軸に関して１８０度モディファイをした方が　結果が良い（ひっくり返らない）
		//
		//元のモーションがある上に　IK編集する場合には
		//X軸に関して１８０度モディファイをしない方が　結果が良い
		//
		//自動化が難しいので　ユーザ指定のオプション化
		//DispAndLimitsプレートメニューに　x180チェックボックス追加
		//x180にチェックを入れると　X軸に関しても１８０度モディファイを行う
		tmpX1 = tmpX0;
		
		//if (g_underRetargetFlag == false) {
		//if ((g_underIKRot == false) || (g_x180flag == true)) {
		//if ((srcunderRetarget == false) || (g_x180flag == true)) {


		//2023/11/04 fbxの読み書きの際に　ModifyXは不要(Hunt+bvh144_2の読み書きでテスト)
		//x180のGUIスイッチをオンにした時のみModifyする
		if (g_x180flag == true) {
		
			//if((g_underRetargetFlag == true) || (g_x180flag == true)) {
			//if ((notmodify180flag == 0) && (isendbone != 0)) {
			if (notmodify180flag == 0) {
				//180度(thdeg : 165度以上)の変化は　軸反転しないような表現に補正
				if ((tmpX0 - validbefeul.x) >= thdeg) {
					tmpX1 = tmpX0 - 180.0f;
				}
				if ((validbefeul.x - tmpX0) >= thdeg) {
					tmpX1 = tmpX0 + 180.0f;
				}
			}
			else {
				//tmpX0そのまま
			}
		}
		Euler.x = tmpX1;
	}



	//###################################################################################################################################
	//2023/01/12
	//Rokokoのfbx読み込みテストと　rootjointをIKで２回転するテストをして　オイラーグラフを観察
	//ModifyEulerは　後処理としてではなく　XYZそれぞれの角度を求める際に　その都度補正する必要があった
	//(そのようにしないと　Y軸を２回転する間に　Y軸が９０度の範囲しか動かずに　XとZが１８０度ずつ変化する階段状のグラフになってしまった)
	//よって　後処理としてのModifyEuler*はコメントアウトして　各角度を求める部分で処理した
	//###################################################################################################################################
	//ModifyEuler(&Euler, &befeul);
	//ModifyEulerXYZ(&Euler, &befeul, isfirstbone, isendbone, notmodifyflag);//10027 CommentOut. 処理が重いわりにたまにしか役に立たないので。しばらくコメントアウト。
	//ModifyEuler360(&Euler, &befeul, notmodify180flag);
	//ModifyEuler360(&Euler, &befeul, 0);



	//###########################################################################################
	//2023/02/04
	//当たり前のことだが　XYZEul(180, 0, 180)とXYZEul(0, 0, 0)は違う姿勢
	//360度のプラスマイナスは有りだが　180度のプラスマイナスは　違う姿勢にすること
	//ノイズ対策として+-180度は有り得るが
	//同じ姿勢の別表現としての+-180度は　XYZEul(0, 180, 0)をXYZEul(180, 0, 180)にする以外に思いつかない
	//360のプラスマイナスに戻して　後処理として補正を行う
	//###########################################################################################

	//###########################################################################################################
	//2023/02/07
	//２軸が１８０度変化してY軸が０から９０度までしか動かないように変化しながら
	//体が１回転するというのは　IKRotの際に起こったし　３６０度のプラスマイナスだけだと必ずそうなった
	//１８０度のプラスマイナスは必要と認識し直し
	//以前のようにXYZそれぞれを求めながら１８０度チェック
	// 
	//2023/02/04の変更は　LimitEul時にグラフが１８０度の変化を交互に繰り返す症状が出たから
	//2023/02/07にテストしたところ　それは１８０度プラスマイナス補正が原因の不具合ではなかったようだ
	//
	//2023/02/08 am00:53
	//GetBefEul()の内容の問題だった　unlimitedのオイラーをbefeulにすることで解決したようだ
	//notmodify180flag関連をロールバックしてテスト　問題が出たテストデータ(bvh121)でざっとテスト　今のところOK
	//  　もう寝ます
	//###########################################################################################################

	//if (g_underIKRot == false) {
	//	ModifyEuler360(&Euler, &befeul, notmodify180flag);
	//}
	


	//if (g_underIKRot == true) {
	//	ChaModifyEuler360(&Euler, &(befeul.befframeeul), notmodify180flag, 180.0f, 180.0f, 180.0f);
	//}



	*reteul = Euler;

	return 0;
}



//int CQuaternion::Q2Eul(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul)
//{
//
//	CQuaternion axisQ, invaxisQ, EQ;
//	if (axisq) {
//		axisQ = *axisq;
//		axisQ.inv(&invaxisQ);
//		EQ = invaxisQ * *this * axisQ;
//	}
//	else {
//		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		EQ = *this;
//	}
//
//	ChaVector3 Euler;
//
//
//	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
//	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
//	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);
//
//	ChaVector3 targetVec, shadowVec;
//	ChaVector3 tmpVec;
//
//	EQ.Rotate(&targetVec, axisZVec);
//	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
//	shadowVec.y = 0.0f;
//	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.y = 90.0f;
//	}
//	else {
//		Euler.y = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) < 0.0) {
//		Euler.y = -Euler.y;
//	}
//
//	vec3RotateY(&tmpVec, -Euler.y, &targetVec);
//	shadowVec.x = 0.0f;
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.x = 90.0f;
//	}
//	else {
//		Euler.x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisYVec) > 0.0) {
//		Euler.x = -Euler.x;
//	}
//
//
//	EQ.Rotate(&targetVec, axisYVec);
//	vec3RotateY(&tmpVec, -Euler.y, &targetVec);
//	targetVec = tmpVec;
//	vec3RotateX(&tmpVec, -Euler.x, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = 0.0f;
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.z = 90.0f;
//	}
//	else {
//		Euler.z = (float)aCos(vecDotVec(&shadowVec, &axisYVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) > 0.0) {
//		Euler.z = -Euler.z;
//	}
//
//	ModifyEuler(&Euler, &befeul);
//	*reteul = Euler;
//
//	return 0;
//}
//
//int CQuaternion::Q2EulZYX(int needmodifyflag, CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul)
//{
//	CQuaternion axisQ, invaxisQ, EQ;
//	if (axisq) {
//		axisQ = *axisq;
//		axisQ.inv(&invaxisQ);
//		EQ = invaxisQ * *this * axisQ;
//	}
//	else {
//		axisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		invaxisQ.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
//		EQ = *this;
//	}
//
//	ChaVector3 Euler;
//
//
//	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
//	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
//	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);
//
//	ChaVector3 targetVec, shadowVec;
//	ChaVector3 tmpVec;
//
//	EQ.Rotate(&targetVec, axisZVec);
//	shadowVec.x = 0.0f;
//	shadowVec.y = (float)vecDotVec(&targetVec, &axisYVec);
//	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.x = 90.0f;
//	}
//	else {
//		Euler.x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / (double)lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisYVec) > 0.0) {
//		Euler.x = -Euler.x;
//	}
//
//	vec3RotateX(&tmpVec, -Euler.x, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = 0;
//	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.y = 90.0f;
//	}
//	else {
//		Euler.y = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / (double)lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) < 0.0) {
//		Euler.y = -Euler.y;
//	}
//
//
//	EQ.Rotate(&targetVec, axisXVec);
//	vec3RotateX(&tmpVec, -Euler.x, &targetVec);
//	targetVec = tmpVec;
//	vec3RotateY(&tmpVec, -Euler.y, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = 0.0f;
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler.z = 90.0f;
//	}
//	else {
//		Euler.z = (float)aCos(vecDotVec(&shadowVec, &axisXVec) / (double)lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisYVec) < 0.0) {
//		Euler.z = -Euler.z;
//	}
//
//	if (needmodifyflag == 1) {
//		ModifyEuler(&Euler, &befeul);
//	}
//	*reteul = Euler;
//
//	return 0;
//}
//
//
//
//int CQuaternion::ModifyEuler(ChaVector3* eulerA, ChaVector3* eulerB)
//{
//
//	//オイラー角Aの値をオイラー角Bの値に近い表示に修正
//	double tmpX1, tmpY1, tmpZ1;
//	double tmpX2, tmpY2, tmpZ2;
//	double s1, s2;
//
//	//予想される角度1
//	tmpX1 = eulerA->x + 360.0 * GetRound((float)(((double)eulerB->x - (double)eulerA->x) / 360.0));
//	tmpY1 = eulerA->y + 360.0 * GetRound((float)(((double)eulerB->y - (double)eulerA->y) / 360.0));
//	tmpZ1 = eulerA->z + 360.0 * GetRound((float)(((double)eulerB->z - (double)eulerA->z) / 360.0));
//
//	//予想される角度2
//	//クォータニオンは１８０°で一回転する。
//	//横軸が２シータ、縦軸がsin2シータ、cos2シータのグラフにおいて、newシータ　=　180 + oldシータの値は等しい。
//	//tmp2の角度はクォータニオンにおいて等しい姿勢を取るオイラー角である。
//	//この場合、３つの軸のうち１つだけの軸の角度の符号(ここではX軸)が反転する。
//	//ということだと思う。テストすると合っている。
//	tmpX2 = 180.0 - eulerA->x + 360.0 * GetRound((float)(((double)eulerB->x + (double)eulerA->x - 180.0) / 360.0));
//	tmpY2 = eulerA->y + 180.0 + 360.0 * GetRound((float)(((double)eulerB->y - (double)eulerA->y - 180.0) / 360.0));
//	tmpZ2 = eulerA->z + 180.0 + 360.0 * GetRound((float)(((double)eulerB->z - (double)eulerA->z - 180.0) / 360.0));
//
//
//	//角度変化の大きさ
//	s1 = ((double)eulerB->x - tmpX1) * ((double)eulerB->x - tmpX1) + ((double)eulerB->y - tmpY1) * ((double)eulerB->y - tmpY1) + ((double)eulerB->z - tmpZ1) * ((double)eulerB->z - tmpZ1);
//	s2 = ((double)eulerB->x - tmpX2) * ((double)eulerB->x - tmpX2) + ((double)eulerB->y - tmpY2) * ((double)eulerB->y - tmpY2) + ((double)eulerB->z - tmpZ2) * ((double)eulerB->z - tmpZ2);
//
//	//変化の少ない方に修正
//	if (s1 < s2) {
//		eulerA->x = (float)tmpX1; eulerA->y = (float)tmpY1; eulerA->z = (float)tmpZ1;
//	}
//	else {
//		eulerA->x = (float)tmpX2; eulerA->y = (float)tmpY2; eulerA->z = (float)tmpZ2;
//	}
//
//	return 0;
//}




BOOL IsValidNewEul(ChaVector3 srcneweul, ChaVector3 srcbefeul)
{
	//とりあえず
	return TRUE;
}



int CQuaternion::ModifyEulerXYZ(ChaVector3* eulerA, ChaVector3* eulerB, int isfirstbone, int isendbone, int notmodifyflag)
{

	//オイラー角Aの値をオイラー角Bの値に近い表示に修正
	double tmpX0, tmpY0, tmpZ0;
	double tmpX1, tmpY1, tmpZ1;
	double tmpX2, tmpY2, tmpZ2;
	double tmpX3, tmpY3, tmpZ3;
	double tmpX4, tmpY4, tmpZ4;
	//double tmpX5, tmpY5, tmpZ5;
	double s0, s1, s2, s3, s4;// , s5;
	//double newX, newY, newZ;

	tmpX0 = eulerA->x;
	tmpY0 = eulerA->y;
	tmpZ0 = eulerA->z;

	//予想される角度1
	tmpX1 = (double)eulerA->x + 360.0 * GetRound((float)(((double)eulerB->x - (double)eulerA->x) / 360.0));
	tmpY1 = (double)eulerA->y + 360.0 * GetRound((float)(((double)eulerB->y - (double)eulerA->y) / 360.0));
	tmpZ1 = (double)eulerA->z + 360.0 * GetRound((float)(((double)eulerB->z - (double)eulerA->z) / 360.0));


	//##########################################################################################
	// ModifyEuler いままでの試行錯誤のまとめ
	//####################################
	// 
	// 
	// 	notmodifyflag == 1//!!!! bvh-->fbx書き出し時にはmodifyeulerで裏返りチェックをするが、それ以外の時は２重に処理しないように裏返りチェックをしない
	// 
	// 
	// 
	// 試行錯誤時の症状と数学を思い出し、結局次のようになった.
	// ノイズが乗っているときにも今までで一番きれいなオイラーグラフになった.つま先に関しても改善.
	// 
	//座標系合わせ。軸の向きが座標系に合うように１８０度回転チェック。座標系を合わせるにはbefeulに近づければ良い.
	// 
	// 
	// クォータニオンからオイラー角を計算するとき、ボーン軸に関して１８０度ねじれるようなクオータニオンのオイラー角と同じオイラー角になる.
	// 例えばZが１８０度回転した時、XとYは-X, -Yになる.(180 - X)では無かった.
	// 裏返ったオイラー角の方がbefeulに近い場合を検出してオイラー角を裏返返す処理をする.
	// 
	// 
	//##########################################################################################


	//X + 180のとき
	tmpX2 = tmpX0 + 180.0;
	if (tmpX2 > 180.0) {
		tmpX2 -= 360.0;
	}
	tmpY2 = -tmpY0;
	tmpZ2 = -tmpZ0;
	//tmpY2 = 180.0 - tmpY0;
	//tmpZ2 = 180.0 - tmpZ0;

	//Y + 180のとき
	tmpY3 = tmpY0 + 180.0;
	if (tmpY3 > 180.0) {
		tmpY3 -= 360.0;
	}
	tmpX3 = -tmpX0;
	tmpZ3 = -tmpZ0;
	//tmpX3 = 180.0 - tmpX0;
	//tmpZ3 = 180.0 - tmpZ0;

	//Z + 180のとき
	tmpZ4 = tmpZ0 + 180.0;
	if (tmpZ4 > 180.0) {
		tmpZ4 -= 360.0;
	}
	tmpX4 = -tmpX0;
	tmpY4 = -tmpY0;
	//tmpX4 = 180.0 - tmpX0;
	//tmpY4 = 180.0 - tmpY0;



	//角度変化の大きさ
	s0 = ((double)eulerB->x - tmpX0) * ((double)eulerB->x - tmpX0) + ((double)eulerB->y - tmpY0) * ((double)eulerB->y - tmpY0) + ((double)eulerB->z - tmpZ0) * ((double)eulerB->z - tmpZ0);
	s1 = ((double)eulerB->x - tmpX1) * ((double)eulerB->x - tmpX1) + ((double)eulerB->y - tmpY1) * ((double)eulerB->y - tmpY1) + ((double)eulerB->z - tmpZ1) * ((double)eulerB->z - tmpZ1);
	s2 = ((double)eulerB->x - tmpX2) * ((double)eulerB->x - tmpX2) + ((double)eulerB->y - tmpY2) * ((double)eulerB->y - tmpY2) + ((double)eulerB->z - tmpZ2) * ((double)eulerB->z - tmpZ2);
	s3 = ((double)eulerB->x - tmpX3) * ((double)eulerB->x - tmpX3) + ((double)eulerB->y - tmpY3) * ((double)eulerB->y - tmpY3) + ((double)eulerB->z - tmpZ3) * ((double)eulerB->z - tmpZ3);
	s4 = ((double)eulerB->x - tmpX4) * ((double)eulerB->x - tmpX4) + ((double)eulerB->y - tmpY4) * ((double)eulerB->y - tmpY4) + ((double)eulerB->z - tmpZ4) * ((double)eulerB->z - tmpZ4);

	typedef struct tag_chkeul
	{
		double s;
		int index;
		double X, Y, Z;
		bool operator<(const tag_chkeul& right) const {
			return s == right.s ? index < right.index : s < right.s;
		}
	}CHKEUL;

	std::vector<struct tag_chkeul> vecchkeul;
	CHKEUL tmpchkeul;
	tmpchkeul.s = s0;
	tmpchkeul.index = 0;
	tmpchkeul.X = tmpX0;
	tmpchkeul.Y = tmpY0;
	tmpchkeul.Z = tmpZ0;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s1;
	tmpchkeul.index = 1;
	tmpchkeul.X = tmpX1;
	tmpchkeul.Y = tmpY1;
	tmpchkeul.Z = tmpZ1;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s2;
	tmpchkeul.index = 2;
	tmpchkeul.X = tmpX2;
	tmpchkeul.Y = tmpY2;
	tmpchkeul.Z = tmpZ2;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s3;
	tmpchkeul.index = 3;
	tmpchkeul.X = tmpX3;
	tmpchkeul.Y = tmpY3;
	tmpchkeul.Z = tmpZ3;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s4;
	tmpchkeul.index = 4;
	tmpchkeul.X = tmpX4;
	tmpchkeul.Y = tmpY4;
	tmpchkeul.Z = tmpZ4;
	vecchkeul.push_back(tmpchkeul);

	std::sort(vecchkeul.begin(), vecchkeul.end());

	CHKEUL mineul = vecchkeul[0];

	if ((notmodifyflag == 0) && (isfirstbone == 0)) {

		eulerA->x = (float)mineul.X; eulerA->y = (float)mineul.Y; eulerA->z = (float)mineul.Z;

	}
	else {
		//if (s0 <= s1) {
			eulerA->x = (float)tmpX0; eulerA->y = (float)tmpY0; eulerA->z = (float)tmpZ0;
		//}
		//else {
		//	eulerA->x = (float)tmpX1; eulerA->y = (float)tmpY1; eulerA->z = (float)tmpZ1;
		//}
	}

	return 0;
}

/*
int CQuaternion::ModifyEuler( ChaVector3* eulerA, ChaVector3* eulerB )
{

//オイラー角Aの値をオイラー角Bの値に近い表示に修正
float tmpX1, tmpY1, tmpZ1;
float tmpX2, tmpY2, tmpZ2;
float s1, s2;

//予想される角度1
tmpX1 = eulerA->x + 360.0f * GetRound( (eulerB->x - eulerA->x) / 360.0f );
tmpY1 = eulerA->y + 360.0f * GetRound( (eulerB->y - eulerA->y) / 360.0f );
tmpZ1 = eulerA->z + 360.0f * GetRound( (eulerB->z - eulerA->z) / 360.0f );

//予想される角度2
tmpX2 = 180.0f - eulerA->x + 360.0f * GetRound( (eulerB->x + eulerA->x - 180.0f) / 360.0f );
tmpY2 = eulerA->y + 180.0f + 360.0f * GetRound( (eulerB->y - eulerA->y - 180.0f) / 360.0f );
tmpZ2 = eulerA->z + 180.0f + 360.0f * GetRound( (eulerB->z - eulerA->z - 180.0f) / 360.0f );

//角度変化の大きさ
s1 = (eulerB->x - tmpX1) * (eulerB->x - tmpX1) + (eulerB->y - tmpY1) * (eulerB->y - tmpY1) + (eulerB->z - tmpZ1) * (eulerB->z - tmpZ1);
s2 = (eulerB->x - tmpX2) * (eulerB->x - tmpX2) + (eulerB->y - tmpY2) * (eulerB->y - tmpY2) + (eulerB->z - tmpZ2) * (eulerB->z - tmpZ2);

//変化の少ない方に修正
if( s1 < s2 ){
eulerA->x = tmpX1; eulerA->y = tmpY1; eulerA->z = tmpZ1;
}else{
eulerA->x = tmpX2; eulerA->y = tmpY2; eulerA->z = tmpZ2;
}

return 0;
}
*/
int CQuaternion::GetRound(float srcval)
{
	//2023/02/04
	//-180度と+180度が入れ替わらないように0.5より少し小さくする

	if (srcval > 0.0f) {
		return (int)(srcval + 0.490);
	}
	else {
		return (int)(srcval - 0.490);
	}

	//if (srcval > 0.0f) {
	//	return (int)(srcval + 0.5);
	//}
	//else {
	//	return (int)(srcval - 0.5);
	//}

	//if (srcval > 0.0f) {
	//	return (int)(srcval + 0.0001f);
	//}
	//else {
	//	return (int)(srcval - 0.0001f);
	//}

}



//
int CQuaternion::CalcFBXEulXYZ(CQuaternion* axisq, BEFEUL befeul, ChaVector3* reteul, int isfirstbone, int isendbone, int notmodify180flag)
{
	int noise[4] = { 0, 1, 0, -1 };
	static int dbgcnt = 0;

	ChaVector3 tmpeul(0.0f, 0.0f, 0.0f);
	if (axisq || (IsInit() == 0)) {
		bool underikrot = false;
		bool underretarget = false;
		Q2EulXYZusingQ(underikrot, underretarget, axisq, befeul, &tmpeul, isfirstbone, isendbone, notmodify180flag);
		//Q2EulXYZusingMat(ROTORDER_XYZ, axisq, befeul, &tmpeul, isfirstbone, isendbone, notmodify180flag);
	}
	else {
		//FBX書き出しの際にアニメーションに「ある程度の大きさの変化」がないとキーが省略されてしまう。
		//ノイズを乗せることでアニメーションがなくてもキーが省略されないようになる。
		//ノイズを乗せるのは１つのボーンで良い。
		if (isfirstbone == 1) {
			tmpeul.x = (float)(noise[dbgcnt % 4]) / 100.0f;//!!!!!!!!
		}
		else {
			tmpeul.x = 0.0f;
		}
		tmpeul.y = 0.0f;
		tmpeul.z = 0.0f;
	}
	*reteul = tmpeul;

	dbgcnt++;

	return 0;
}

//int CQuaternion::CalcFBXEulZXY(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int isfirstbone)
//{
//
//	int noise[4] = { 0, 1, 0, -1 };
//	static int dbgcnt = 0;
//
//	ChaVector3 tmpeul(0.0f, 0.0f, 0.0f);
//	if (IsInit() == 0) {
//		//Q2Eul(axisq, befeul, &tmpeul);
//		//Q2EulXYZ(axisq, befeul, &tmpeul);
//		//Q2EulYXZ(axisq, befeul, &tmpeul);
//		Q2EulZXY(axisq, befeul, &tmpeul);
//	}
//	else {
//		//FBX書き出しの際にアニメーションに「ある程度の大きさの変化」がないとキーが省略されてしまう。
//		//ノイズを乗せることでアニメーションがなくてもキーが省略されないようになる。
//		//ノイズを乗せるのは１つのボーンで良い。
//		if (isfirstbone == 1) {
//			tmpeul.x = (float)(noise[dbgcnt % 4]) / 100.0f;//!!!!!!!!
//		}
//		else {
//			tmpeul.x = 0.0f;
//		}
//		tmpeul.y = 0.0f;
//		tmpeul.z = 0.0f;
//	}
//	*reteul = tmpeul;
//
//	dbgcnt++;
//
//	return 0;
//}

int CQuaternion::IsInit()
{
	float dx, dy, dz, dw;
	dx = x - 0.0f;
	dy = y - 0.0f;
	dz = z - 0.0f;
	dw = w - 1.0f;

	if ((fabs(dx) <= 0.000001f) && (fabs(dy) <= 0.000001f) && (fabs(dz) <= 0.000001f) && (fabs(dw) <= 0.000001f)) {
		return 1;
	}
	else {
		return 0;
	}
}

int CQuaternion::InOrder(CQuaternion* srcdstq)
{
	double kaku;
	kaku = CalcRad(*srcdstq);
	if (kaku > (PI * 0.5)) {
		//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
		CQuaternion tmpq = -*srcdstq;
		*srcdstq = tmpq;
	}

	return 0;
}


void CQuaternion::MakeFromD3DXMat(ChaMatrix eulmat)
{
	this->RotationMatrix(eulmat);
}



//global

CQuaternion QMakeFromBtMat3x3(btMatrix3x3* eulmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfQMakeFromBtMat3x3(eulmat);
}



void ChaMatrixIdentity(ChaMatrix* pdst)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixIdentity(pdst);
}

CQuaternion ChaMatrix2Q(ChaMatrix srcmat)//ChaMatrixを受け取って　CQuaternionを返す
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrix2Q(srcmat);
}

ChaMatrix ChaMatrixRot(ChaMatrix srcmat)//回転成分だけの行列にする
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRot(srcmat);
}

ChaMatrix ChaMatrixScale(ChaMatrix srcmat)//スケール成分だけの行列にする
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixScale(srcmat);
}

ChaMatrix ChaMatrixTra(ChaMatrix srcmat)//移動成分だけの行列にする
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixTra(srcmat);
}

ChaVector3 ChaMatrixScaleVec(ChaMatrix srcmat)//スケール成分のベクトルを取得
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixScaleVec(srcmat);

}
ChaVector3 ChaMatrixRotVec(ChaMatrix srcmat, int notmodify180flag)//回転成分のベクトルを取得
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRotVec(srcmat, notmodify180flag);
}
ChaVector3 ChaMatrixTraVec(ChaMatrix srcmat)//移動成分のベクトルを取得
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixTraVec(srcmat);
}

void ChaMatrixNormalizeRot(ChaMatrix* pdst)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixNormalizeRot(pdst);
}

void ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixInverse(pdst, pdet, psrc);
}


void ChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixTranslation(pdst, srcx, srcy, srcz);
}

void ChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixTranspose(pdst, psrc);
}

double ChaVector3LengthDbl(ChaVector3* v)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaVector3LengthDbl(v);
}

double ChaVector3DotDbl(const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaVector3DotDbl(psrc1, psrc2);
}

void ChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc)
{

	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaVector3Normalize(pdst, psrc);
}

float ChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaVector3Dot(psrc1, psrc2);
}

void ChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaVector3Cross(pdst, psrc1, psrc2);
}


void ChaVector3TransformCoord(ChaVector3* dstvec, ChaVector3* srcvec, ChaMatrix* srcmat)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaVector3TransformCoord(dstvec, srcvec, srcmat);
}

ChaVector3* ChaVector3TransformNormal(ChaVector3 *dstvec, const ChaVector3* srcvec, const ChaMatrix* srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaVector3TransformNormal(dstvec, srcvec, srcmat);
}



double ChaVector3LengthSq(ChaVector3* psrc)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaVector3LengthSq(psrc);
}

void ChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixRotationAxis(pdst, srcaxis, srcrad);
}

void ChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixScaling(pdst, srcx, srcy, srcz);
}

void ChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfChaMatrixLookAtRH(dstviewmat, camEye, camtar, camUpVec);
}

ChaMatrix* ChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf) 
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixOrthoOffCenterRH(pOut, l, r, t, b, zn, zf);
}

ChaMatrix* ChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixPerspectiveFovRH(pOut, fovY, Aspect, zn, zf);
}



const ChaMatrix* ChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRotationQuaternion(dstmat, srcq);
}

ChaMatrix* ChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRotationYawPitchRoll(pOut, srcyaw, srcpitch, srcroll);
}

ChaMatrix* ChaMatrixRotationX(ChaMatrix* pOut, float srcrad)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRotationX(pOut, srcrad);
}

ChaMatrix* ChaMatrixRotationY(ChaMatrix* pOut, float srcrad)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRotationY(pOut, srcrad);
}

ChaMatrix* ChaMatrixRotationZ(ChaMatrix* pOut, float srcrad)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixRotationZ(pOut, srcrad);
}

void CQuaternionIdentity(CQuaternion* dstq)
{
	ChaCalcFunc chacalcfunc;
	chacalcfunc.ccfCQuaternionIdentity(dstq);
}

CQuaternion CQuaternionInv(CQuaternion srcq)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfCQuaternionInv(srcq);
}


ChaMatrix MakeRotMatFromChaMatrix(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfMakeRotMatFromChaMatrix(srcmat);
}

ChaMatrix ChaMatrixTranspose(ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfChaMatrixTranspose(srcmat);
}


ChaMatrix CalcAxisMatX(ChaVector3 vecx, ChaVector3 srcpos, ChaMatrix srcmat)
{
	ChaCalcFunc chacalcfunc;
	return chacalcfunc.ccfCalcAxisMatX(vecx, srcpos, srcmat);
}


COLORREF ChaVector3::ColorRef()
{
	int r255, g255, b255;
	r255 = (int)(x * 255.0f + 0.0001f);
	r255 = min(255, r255);
	r255 = max(0, r255);

	g255 = (int)(y * 255.0f + 0.0001f);
	g255 = min(255, g255);
	g255 = max(0, g255);

	b255 = (int)(z * 255.0f + 0.0001f);
	b255 = min(255, b255);
	b255 = max(0, b255);

	COLORREF retcol;
	retcol = RGB(r255, g255, b255);

	return retcol;
}


int CalcTangentAndBinormal(int srcuvnum, BINORMALDISPV* vert_0, BINORMALDISPV* vert_1, BINORMALDISPV* vert_2)
{
	if (!vert_0 || !vert_1 || !vert_2) {
		_ASSERT(0);
		return 1;
	}


	//頂点スムースは気にしない。
	Vector3 cp0[3];
	Vector3 cp1[3];
	Vector3 cp2[3];
	//Vector3 cp0[] = {
	//	{ vert_0->pos.x, vert_0->uv[1].x, vert_0->uv[1].y},
	//	{ vert_0->pos.y, vert_0->uv[1].x, vert_0->uv[1].y},
	//	{ vert_0->pos.z, vert_0->uv[1].x, vert_0->uv[1].y}
	//};
	//Vector3 cp1[] = {
	//	{ vert_1->pos.x, vert_1->uv[1].x, vert_1->uv[1].y},
	//	{ vert_1->pos.y, vert_1->uv[1].x, vert_1->uv[1].y},
	//	{ vert_1->pos.z, vert_1->uv[1].x, vert_1->uv[1].y}
	//};
	//Vector3 cp2[] = {
	//	{ vert_2->pos.x, vert_2->uv[1].x, vert_2->uv[1].y},
	//	{ vert_2->pos.y, vert_2->uv[1].x, vert_2->uv[1].y},
	//	{ vert_2->pos.z, vert_2->uv[1].x, vert_2->uv[1].y}
	//};
	//Vector3 cp0[] = {
	//	{ vert_0->pos.x, vert_0->uv[0].x, vert_0->uv[0].y},
	//	{ vert_0->pos.y, vert_0->uv[0].x, vert_0->uv[0].y},
	//	{ vert_0->pos.z, vert_0->uv[0].x, vert_0->uv[0].y}
	//};
	//Vector3 cp1[] = {
	//	{ vert_1->pos.x, vert_1->uv[0].x, vert_1->uv[0].y},
	//	{ vert_1->pos.y, vert_1->uv[0].x, vert_1->uv[0].y},
	//	{ vert_1->pos.z, vert_1->uv[0].x, vert_1->uv[0].y}
	//};
	//Vector3 cp2[] = {
	//	{ vert_2->pos.x, vert_2->uv[0].x, vert_2->uv[0].y},
	//	{ vert_2->pos.y, vert_2->uv[0].x, vert_2->uv[0].y},
	//	{ vert_2->pos.z, vert_2->uv[0].x, vert_2->uv[0].y}
	//};

	//if (srcuvnum >= 2) {
	//	cp0[0] = Vector3(vert_0->pos.x, vert_0->uv[1].x, vert_0->uv[1].y);
	//	cp0[1] = Vector3(vert_0->pos.y, vert_0->uv[1].x, vert_0->uv[1].y);
	//	cp0[2] = Vector3(vert_0->pos.z, vert_0->uv[1].x, vert_0->uv[1].y);

	//	cp1[0] = Vector3(vert_1->pos.x, vert_1->uv[1].x, vert_1->uv[1].y);
	//	cp1[1] = Vector3(vert_1->pos.y, vert_1->uv[1].x, vert_1->uv[1].y);
	//	cp1[2] = Vector3(vert_1->pos.z, vert_1->uv[1].x, vert_1->uv[1].y);

	//	cp2[0] = Vector3(vert_2->pos.x, vert_2->uv[1].x, vert_2->uv[1].y);
	//	cp2[1] = Vector3(vert_2->pos.y, vert_2->uv[1].x, vert_2->uv[1].y);
	//	cp2[2] = Vector3(vert_2->pos.z, vert_2->uv[1].x, vert_2->uv[1].y);
	//}
	//else {
		cp0[0].Set(vert_0->pos.x, vert_0->uv[0].x, vert_0->uv[0].y);
		cp0[1].Set(vert_0->pos.y, vert_0->uv[0].x, vert_0->uv[0].y);
		cp0[2].Set(vert_0->pos.z, vert_0->uv[0].x, vert_0->uv[0].y);

		cp1[0].Set(vert_1->pos.x, vert_1->uv[0].x, vert_1->uv[0].y);
		cp1[1].Set(vert_1->pos.y, vert_1->uv[0].x, vert_1->uv[0].y);
		cp1[2].Set(vert_1->pos.z, vert_1->uv[0].x, vert_1->uv[0].y);

		cp2[0].Set(vert_2->pos.x, vert_2->uv[0].x, vert_2->uv[0].y);
		cp2[1].Set(vert_2->pos.y, vert_2->uv[0].x, vert_2->uv[0].y);
		cp2[2].Set(vert_2->pos.z, vert_2->uv[0].x, vert_2->uv[0].y);
	//}



	// 平面パラメータからUV軸座標算出する。
	Vector3 tangent, binormal;
	for (int i = 0; i < 3; ++i) {
		auto V1 = cp1[i] - cp0[i];
		auto V2 = cp2[i] - cp1[i];
		////auto V1 = cp0[i] - cp1[i];
		////auto V2 = cp1[i] - cp2[i];
		//auto V1 = cp2[i] - cp0[i];
		//auto V2 = cp1[i] - cp2[i];
		auto ABC = Cross(V1, V2);
		//auto ABC = Cross(V2, V1);

		if (ABC.x == 0.0f) {
			tangent.v[i] = 0.0f;
			binormal.v[i] = 0.0f;
		}
		else {
			//tangent.v[i] = -ABC.y / ABC.x;
			//binormal.v[i] = -ABC.z / ABC.x;
			tangent.v[i] = -ABC.y / ABC.x;
			binormal.v[i] = -ABC.z / ABC.x;
		}
	}

	//tangent.Normalize();
	//binormal.Normalize();

	vert_0->tangent += ChaVector4(ChaVector3(tangent), 0.0f);
	vert_1->tangent += ChaVector4(ChaVector3(tangent), 0.0f);
	vert_2->tangent += ChaVector4(ChaVector3(tangent), 0.0f);

	vert_0->binormal += ChaVector4(ChaVector3(binormal), 0.0f);
	vert_1->binormal += ChaVector4(ChaVector3(binormal), 0.0f);
	vert_2->binormal += ChaVector4(ChaVector3(binormal), 0.0f);

	//vert_0->tangent.Normalize();
	//vert_1->tangent.Normalize();
	//vert_2->tangent.Normalize();
	//vert_0->binormal.Normalize();
	//vert_1->binormal.Normalize();
	//vert_2->binormal.Normalize();

	return 0;
}


size_t AlignmentSize(size_t size, size_t alignment)
{
	return ((size + alignment) & ~alignment);
}


//#ifdef CONVD3DX11
DirectX::XMFLOAT2 ChaVector2::D3DX()
{
	DirectX::XMFLOAT2 retv;
	retv.x = x;
	retv.y = y;
	return retv;
}

DirectX::XMFLOAT3 ChaVector3::D3DX()
{
	DirectX::XMFLOAT3 retv;
	retv.x = x;
	retv.y = y;
	retv.z = z;
	return retv;
}
DirectX::XMVECTOR ChaVector3::XMVECTOR(float w)
{
	DirectX::XMVECTOR retv;
	retv.m128_f32[0] = x;
	retv.m128_f32[1] = y;
	retv.m128_f32[2] = z;
	retv.m128_f32[3] = w;
	return retv;
}


DirectX::XMFLOAT4 ChaVector4::D3DX()
{
	DirectX::XMFLOAT4 retv;
	retv.x = x;
	retv.y = y;
	retv.z = z;
	retv.w = w;
	return retv;
}


ChaMatrix ChaMatrix::operator= (DirectX::XMMATRIX m) {
	this->data[MATI_11] = m.r[0].m128_f32[0];
	this->data[MATI_12] = m.r[0].m128_f32[1];
	this->data[MATI_13] = m.r[0].m128_f32[2];
	this->data[MATI_14] = m.r[0].m128_f32[3];

	this->data[MATI_21] = m.r[1].m128_f32[0];
	this->data[MATI_22] = m.r[1].m128_f32[1];
	this->data[MATI_23] = m.r[1].m128_f32[2];
	this->data[MATI_24] = m.r[1].m128_f32[3];

	this->data[MATI_31] = m.r[2].m128_f32[0];
	this->data[MATI_32] = m.r[2].m128_f32[1];
	this->data[MATI_33] = m.r[2].m128_f32[2];
	this->data[MATI_34] = m.r[2].m128_f32[3];

	this->data[MATI_41] = m.r[3].m128_f32[0];
	this->data[MATI_42] = m.r[3].m128_f32[1];
	this->data[MATI_43] = m.r[3].m128_f32[2];
	this->data[MATI_44] = m.r[3].m128_f32[3];

	//this->_11 = m.r[0].m128_f32[0];
	//this->_12 = m.r[1].m128_f32[0];
	//this->_13 = m.r[2].m128_f32[0];
	//this->data[MATI_14] = m.r[3].m128_f32[0];

	//this->data[MATI_21] = m.r[0].m128_f32[1];
	//this->data[MATI_22] = m.r[1].m128_f32[1];
	//this->data[MATI_23] = m.r[2].m128_f32[1];
	//this->data[MATI_24] = m.r[3].m128_f32[1];

	//this->data[MATI_31] = m.r[0].m128_f32[2];
	//this->data[MATI_32] = m.r[1].m128_f32[2];
	//this->data[MATI_33] = m.r[2].m128_f32[2];
	//this->data[MATI_34] = m.r[3].m128_f32[2];

	//this->data[MATI_41] = m.r[0].m128_f32[3];
	//this->data[MATI_42] = m.r[1].m128_f32[3];
	//this->data[MATI_43] = m.r[2].m128_f32[3];
	//this->data[MATI_44] = m.r[3].m128_f32[3];


	return *this;
};

Matrix ChaMatrix::TKMatrix()
{
	Matrix retmat;
	retmat._11 = this->data[MATI_11];
	retmat._12 = this->data[MATI_12];
	retmat._13 = this->data[MATI_13];
	retmat._14 = this->data[MATI_14];

	retmat._21 = this->data[MATI_21];
	retmat._22 = this->data[MATI_22];
	retmat._23 = this->data[MATI_23];
	retmat._24 = this->data[MATI_24];

	retmat._31 = this->data[MATI_31];
	retmat._32 = this->data[MATI_32];
	retmat._33 = this->data[MATI_33];
	retmat._34 = this->data[MATI_34];

	retmat._41 = this->data[MATI_41];
	retmat._42 = this->data[MATI_42];
	retmat._43 = this->data[MATI_43];
	retmat._44 = this->data[MATI_44];

	return retmat;
}


DirectX::XMMATRIX ChaMatrix::D3DX()
{
	DirectX::XMMATRIX retm;
	retm.r[0].m128_f32[0] = data[MATI_11];
	retm.r[0].m128_f32[1] = data[MATI_12];
	retm.r[0].m128_f32[2] = data[MATI_13];
	retm.r[0].m128_f32[3] = data[MATI_14];

	retm.r[1].m128_f32[0] = data[MATI_21];
	retm.r[1].m128_f32[1] = data[MATI_22];
	retm.r[1].m128_f32[2] = data[MATI_23];
	retm.r[1].m128_f32[3] = data[MATI_24];

	retm.r[2].m128_f32[0] = data[MATI_31];
	retm.r[2].m128_f32[1] = data[MATI_32];
	retm.r[2].m128_f32[2] = data[MATI_33];
	retm.r[2].m128_f32[3] = data[MATI_34];

	retm.r[3].m128_f32[0] = data[MATI_41];
	retm.r[3].m128_f32[1] = data[MATI_42];
	retm.r[3].m128_f32[2] = data[MATI_43];
	retm.r[3].m128_f32[3] = data[MATI_44];

	//retm.r[0].m128_f32[0] = _11;
	//retm.r[1].m128_f32[0] = data[MATI_12];
	//retm.r[2].m128_f32[0] = _13;
	//retm.r[3].m128_f32[0] = data[MATI_14];

	//retm.r[0].m128_f32[1] = data[MATI_21];
	//retm.r[2].m128_f32[1] = data[MATI_22];
	//retm.r[3].m128_f32[1] = data[MATI_23];
	//retm.r[4].m128_f32[1] = data[MATI_24];

	//retm.r[0].m128_f32[2] = data[MATI_31];
	//retm.r[1].m128_f32[2] = data[MATI_32];
	//retm.r[2].m128_f32[2] = data[MATI_33];
	//retm.r[3].m128_f32[2] = data[MATI_34];

	//retm.r[0].m128_f32[3] = data[MATI_41];
	//retm.r[1].m128_f32[3] = data[MATI_42];
	//retm.r[2].m128_f32[3] = data[MATI_43];
	//retm.r[3].m128_f32[3] = data[MATI_44];

	return retm;
}
//#endif


N3P::N3P() {
	InitParams();
}
N3P::~N3P()
{
	if (perface) {
		free(perface);
		perface = 0;
	}
	if (pervert) {
		free(pervert);
		pervert = 0;
	}
	if (n3sm) {
		delete n3sm;
		n3sm = 0;
	}
}
void N3P::InitParams()
{
	perface = 0;
	pervert = 0;
	n3sm = 0;
}

N3SM::N3SM()
{
	InitParams();
}
N3SM::~N3SM()
{
	if (ppsmface) {
		free(ppsmface);
		ppsmface = 0;
	}
	smfacenum = 0;
}
void N3SM::InitParams()
{
	smfacenum = 0;
	ppsmface = 0;
}



ChaFrustumInfo::ChaFrustumInfo()
{
	InitParams();
}
ChaFrustumInfo::~ChaFrustumInfo()
{
}

//int ChaFrustumInfo::UpdateFrustum(ChaMatrix matVP)
//{
//	m_matVP = matVP;
//	ChaMatrix matInvViewProj = ChaMatrixInv(matVP);
//
//	//m_vecFrustum[0].SetParams(-1.0f, -1.0f, 0.0f); // xyz
//	//m_vecFrustum[1].SetParams(1.0f, -1.0f, 0.0f); // Xyz
//	//m_vecFrustum[2].SetParams(-1.0f, 1.0f, 0.0f); // xYz
//	//m_vecFrustum[3].SetParams(1.0f, 1.0f, 0.0f); // XYz
//	//m_vecFrustum[4].SetParams(-1.0f, -1.0f, 1.0f); // xyZ
//	//m_vecFrustum[5].SetParams(1.0f, -1.0f, 1.0f); // XyZ
//	//m_vecFrustum[6].SetParams(-1.0f, 1.0f, 1.0f); // xYZ
//	//m_vecFrustum[7].SetParams(1.0f, 1.0f, 1.0f); // XYZ
//
//	//float minx = -1.0f;
//	//float maxx = 1.0f;
//	//float miny = -1.0f;
//	//float maxy = 1.0f;
//	float minx = -1.0f;
//	float maxx = 1.0f;
//	float miny = -1.0f;
//	float maxy = 1.0f;
//	//float minz = 0.25f;
//	//float minz = 0.50f;
//	//float minz = 0.50f;
//
//	//##################################################################################################################
//	//パースがきついほど　視錐の面に角度がついて　内積での判定がうまくいかない
//	//適正なminz, maxzを探したが　アセットのプロジェクションに依存するので　固定値では無理
//	//Frustumでの判定は正射影用？
//	//今回は　Frustum判定をやめて　２次元的にfovyの内積比較で判定することにした
//	// 
//	//2023/08/26_3 数学の本をみてみた　視錐体の処理はカメラ座標系でするようだ　ワールド座標系でやっていたから失敗した？
//	//既に　fovyでの軽い処理で済ませることにしたので　カメラ座標系での処理は　また必要があったらということで
//	//###################################################################################################################
//	float minz = 0.9990f;
//	float maxz = 0.9997f;
//	//float maxz = 0.70f;
//	m_vecFrustum[0].SetParams(minx, miny, minz); // xyz
//	m_vecFrustum[1].SetParams(maxx, miny, minz); // Xyz
//	m_vecFrustum[2].SetParams(minx, maxy, minz); // xYz
//	m_vecFrustum[3].SetParams(maxx, maxy, minz); // XYz
//	m_vecFrustum[4].SetParams(minx, miny, maxz); // xyZ
//	m_vecFrustum[5].SetParams(maxx, miny, maxz); // XyZ
//	m_vecFrustum[6].SetParams(minx, maxy, maxz); // xYZ
//	m_vecFrustum[7].SetParams(maxx, maxy, maxz); // XYZ
//
//
//
//	for (INT i = 0; i < 8; i++) {
//		ChaVector3TransformCoord(&(m_vecTraFrustum[i]), &(m_vecFrustum[i]), &matInvViewProj);
//	}
//
//	//m_planeFrustum[0].FromPoints(m_vecTraFrustum[0], m_vecTraFrustum[1], m_vecTraFrustum[2]); // Near
//	//m_planeFrustum[1].FromPoints(m_vecTraFrustum[6], m_vecTraFrustum[7], m_vecTraFrustum[5]); // Far
//	//m_planeFrustum[2].FromPoints(m_vecTraFrustum[2], m_vecTraFrustum[6], m_vecTraFrustum[4]); // Left
//	//m_planeFrustum[3].FromPoints(m_vecTraFrustum[7], m_vecTraFrustum[3], m_vecTraFrustum[5]); // Right
//	//m_planeFrustum[4].FromPoints(m_vecTraFrustum[2], m_vecTraFrustum[3], m_vecTraFrustum[6]); // Top
//	//m_planeFrustum[5].FromPoints(m_vecTraFrustum[1], m_vecTraFrustum[0], m_vecTraFrustum[4]); // Bottom
//
//	//m_planeFrustum[0].FromPoints(m_vecTraFrustum[2], m_vecTraFrustum[1], m_vecTraFrustum[0]); // Near
//	//m_planeFrustum[1].FromPoints(m_vecTraFrustum[5], m_vecTraFrustum[7], m_vecTraFrustum[6]); // Far
//	//m_planeFrustum[2].FromPoints(m_vecTraFrustum[4], m_vecTraFrustum[6], m_vecTraFrustum[2]); // Left
//	//m_planeFrustum[3].FromPoints(m_vecTraFrustum[5], m_vecTraFrustum[3], m_vecTraFrustum[7]); // Right
//	//m_planeFrustum[4].FromPoints(m_vecTraFrustum[6], m_vecTraFrustum[3], m_vecTraFrustum[2]); // Top
//	//m_planeFrustum[5].FromPoints(m_vecTraFrustum[4], m_vecTraFrustum[0], m_vecTraFrustum[1]); // Bottom
//
//	m_planeFrustum[0].FromPoints(m_vecTraFrustum[0], m_vecTraFrustum[2], m_vecTraFrustum[1]); // Near
//	m_planeFrustum[1].FromPoints(m_vecTraFrustum[5], m_vecTraFrustum[7], m_vecTraFrustum[6]); // Far !!!
//	m_planeFrustum[2].FromPoints(m_vecTraFrustum[4], m_vecTraFrustum[6], m_vecTraFrustum[0]); // Left
//	m_planeFrustum[3].FromPoints(m_vecTraFrustum[1], m_vecTraFrustum[3], m_vecTraFrustum[5]); // Right
//	m_planeFrustum[4].FromPoints(m_vecTraFrustum[2], m_vecTraFrustum[6], m_vecTraFrustum[3]); // Top !!!
//	m_planeFrustum[5].FromPoints(m_vecTraFrustum[1], m_vecTraFrustum[5], m_vecTraFrustum[4]); // Bottom
//
//	//m_planeFrustum[0].FromPoints(m_vecTraFrustum[0], m_vecTraFrustum[2], m_vecTraFrustum[1]); // Near
//	//m_planeFrustum[1].FromPoints(m_vecTraFrustum[6], m_vecTraFrustum[7], m_vecTraFrustum[5]); // Far !!!
//	//m_planeFrustum[2].FromPoints(m_vecTraFrustum[4], m_vecTraFrustum[6], m_vecTraFrustum[0]); // Left
//	//m_planeFrustum[3].FromPoints(m_vecTraFrustum[1], m_vecTraFrustum[3], m_vecTraFrustum[5]); // Right
//	//m_planeFrustum[4].FromPoints(m_vecTraFrustum[3], m_vecTraFrustum[6], m_vecTraFrustum[2]); // Top !!!
//	//m_planeFrustum[5].FromPoints(m_vecTraFrustum[1], m_vecTraFrustum[5], m_vecTraFrustum[4]); // Bottom
//
//	return 0;
//}

//int ChaFrustumInfo::ChkInView(MODELBOUND srcmb, ChaMatrix matWorld)
//{
//	ChaVector3 tracenter;
//	ChaVector3TransformCoord(&tracenter, &(srcmb.center), &matWorld);
//	//tracenter = srcmb.center;
//
//	int planeno;
//	//int visiblecnt = 0;
//	int pluscount = 0;
//	int minuscount = 0;
//	int zerocount = 0;
//	for (planeno = 0; planeno < 6; planeno++) {
//
//		//float distance = m_planeFrustum[planeno].a * tracenter.x +
//		//	m_planeFrustum[planeno].b * tracenter.y +
//		//	m_planeFrustum[planeno].c * tracenter.z +
//		//	m_planeFrustum[planeno].d;
//		//if (distance > 0.0f) {
//		//	pluscount++;
//		//}
//		//else if (distance < 0.0f) {
//		//	minuscount++;
//		//}
//		//else {
//		//	zerocount++;
//		//}
//
//		//int result = GetFootOnPlane(planeno, tracenter);//m_footpos[planeno]に計算値をセット
//		//if (result) {
//		//	_ASSERT(0);
//		//	return 1;
//		//}
//
//		//ChaVector3 foot2center = tracenter - m_footpos[planeno];
//		//ChaVector3Normalize(&foot2center, &foot2center);
//		////ChaVector3 center2foot = m_footpos[planeno] - tracenter;
//		////ChaVector3Normalize(&center2foot, &center2foot);
//		//
//		ChaVector3 n;
//		n.x = m_planeFrustum[planeno].a;
//		n.y = m_planeFrustum[planeno].b;
//		n.z = m_planeFrustum[planeno].c;
//
//		float dot;
//		dot = ChaVector3Dot(&n, &foot2center);
//
//
//
//
//
//		////float dot2;
//		////dot2 = ChaVector3Dot(&n, &center2foot);
//
//		////距離チェック
//		//float tmpa0, a0, tmpa1, a1;
//		//tmpa0 = n.x * tracenter.x + n.y * tracenter.y + n.z * tracenter.z + m_planeFrustum[planeno].d;
//		//if (tmpa0 != 0.0f) {
//		//	a0 = (float)fabs(tmpa0);
//		//}
//		//else {
//		//	a0 = 0.0f;
//		//}
//		//
//		//tmpa1 = n.x * n.x + n.y * n.y + n.z * n.z;
//		//if (tmpa1 != 0.0f) {
//		//	a1 = sqrtf(tmpa1);
//		//}
//		//else {
//		//	a1 = 0.0f;
//		//}
//		//
//		//float dist;
//		//if (a1 != 0.0f) {
//		//	dist = a0 / a1;
//		//}
//		//else {
//		//	dist = 0.0f;
//		//}
//
//		////if ((dot < 0.0f) && (dist > (srcmb.r * srcmb.r))) {
//		////if ((dot < 0.0f) && (dist > srcmb.r)) {
//		////if (dist > srcmb.r) {
//		////	//視野外
//		////	break;//!!!!!!!!!
//		////}
//		////else {
//		////	visiblecnt++;
//		////}
//
//		if (dot > 0.0f) {
//			pluscount++;
//		}
//		else if (dot < 0.0f) {
//			minuscount++;
//		}
//		else {
//			zerocount++;
//		}
//
//
//	}
//	//if (visiblecnt == 6) {
//	//	SetVisible(true);
//	//}
//	//else {
//	//	SetVisible(false);
//	//}
//
//	if ((pluscount == 0) || (minuscount == 0)) {
//		SetVisible(true);
//	}
//	else {
//		SetVisible(false);
//	}
//
//
//	return 0;
//}


//int ChaFrustumInfo::ChkInView(MODELBOUND srcmb, ChaMatrix matWorld)
//{
//	ChaVector3 tracenter;
//	ChaVector3TransformCoord(&tracenter, &(srcmb.center), &matWorld);
//	//tracenter = srcmb.center;
//
//	int chk1, chk2, chk3, chk4, chk5, chk6;
//
//	//near
//	{
//		ChaVector3 n;
//		n.x = m_planeFrustum[0].a;
//		n.y = m_planeFrustum[0].b;
//		n.z = m_planeFrustum[0].c;
//
//		ChaVector3 vec1, vec2, vec3, vec4;
//		float dot1, dot2, dot3, dot4;
//		
//		vec1 = tracenter - m_vecTraFrustum[0];
//		ChaVector3Normalize(&vec1, &vec1);
//		dot1 = ChaVector3Dot(&n, &vec1);
//
//		vec2 = tracenter - m_vecTraFrustum[1];
//		ChaVector3Normalize(&vec2, &vec2);
//		dot2 = ChaVector3Dot(&n, &vec2);
//
//		vec3 = tracenter - m_vecTraFrustum[2];
//		ChaVector3Normalize(&vec3, &vec3);
//		dot3 = ChaVector3Dot(&n, &vec3);
//
//		vec4 = tracenter - m_vecTraFrustum[3];
//		ChaVector3Normalize(&vec4, &vec4);
//		dot4 = ChaVector3Dot(&n, &vec4);
//
//		if ((dot1 >= 0.0f) && (dot2 >= 0.0f) && (dot3 >= 0.0f) && (dot4 >= 0.0f)) {
//			chk1 = 1;
//		}
//		else if ((dot1 <= 0.0f) && (dot2 <= 0.0f) && (dot3 <= 0.0f) && (dot4 <= 0.0f)) {
//			chk1 = -1;
//		}
//		else {
//			chk1 = 0;
//		}
//	}
//
//	//far
//	{
//		ChaVector3 n;
//		//n.x = m_planeFrustum[1].a;
//		//n.y = m_planeFrustum[1].b;
//		//n.z = m_planeFrustum[1].c;
//		n.x = m_planeFrustum[0].a;
//		n.y = m_planeFrustum[0].b;
//		n.z = m_planeFrustum[0].c;
//
//		ChaVector3 vec1, vec2, vec3, vec4;
//		float dot1, dot2, dot3, dot4;
//
//		vec1 = tracenter - m_vecTraFrustum[4];
//		ChaVector3Normalize(&vec1, &vec1);
//		dot1 = ChaVector3Dot(&n, &vec1);
//
//		vec2 = tracenter - m_vecTraFrustum[5];
//		ChaVector3Normalize(&vec2, &vec2);
//		dot2 = ChaVector3Dot(&n, &vec2);
//
//		vec3 = tracenter - m_vecTraFrustum[6];
//		ChaVector3Normalize(&vec3, &vec3);
//		dot3 = ChaVector3Dot(&n, &vec3);
//
//		vec4 = tracenter - m_vecTraFrustum[7];
//		ChaVector3Normalize(&vec4, &vec4);
//		dot4 = ChaVector3Dot(&n, &vec4);
//
//		if ((dot1 >= 0.0f) && (dot2 >= 0.0f) && (dot3 >= 0.0f) && (dot4 >= 0.0f)) {
//			chk2 = 1;
//		}
//		else if ((dot1 <= 0.0f) && (dot2 <= 0.0f) && (dot3 <= 0.0f) && (dot4 <= 0.0f)) {
//			chk2 = -1;
//		}
//		else {
//			chk2 = 0;
//		}
//	}
//
//	//left
//	{
//		ChaVector3 n;
//		n.x = m_planeFrustum[2].a;
//		n.y = m_planeFrustum[2].b;
//		n.z = m_planeFrustum[2].c;
//
//		ChaVector3 vec1, vec2, vec3, vec4;
//		float dot1, dot2, dot3, dot4;
//
//		vec1 = tracenter - m_vecTraFrustum[0];
//		ChaVector3Normalize(&vec1, &vec1);
//		dot1 = ChaVector3Dot(&n, &vec1);
//
//		vec2 = tracenter - m_vecTraFrustum[2];
//		ChaVector3Normalize(&vec2, &vec2);
//		dot2 = ChaVector3Dot(&n, &vec2);
//
//		vec3 = tracenter - m_vecTraFrustum[4];
//		ChaVector3Normalize(&vec3, &vec3);
//		dot3 = ChaVector3Dot(&n, &vec3);
//
//		vec4 = tracenter - m_vecTraFrustum[6];
//		ChaVector3Normalize(&vec4, &vec4);
//		dot4 = ChaVector3Dot(&n, &vec4);
//
//		if ((dot1 >= 0.0f) && (dot2 >= 0.0f) && (dot3 >= 0.0f) && (dot4 >= 0.0f)) {
//			chk3 = 1;
//		}
//		else if ((dot1 <= 0.0f) && (dot2 <= 0.0f) && (dot3 <= 0.0f) && (dot4 <= 0.0f)) {
//			chk3 = -1;
//		}
//		else {
//			chk3 = 0;
//		}
//	}
//
//	//right
//	{
//		ChaVector3 n;
//		//n.x = m_planeFrustum[3].a;
//		//n.y = m_planeFrustum[3].b;
//		//n.z = m_planeFrustum[3].c;
//		n.x = m_planeFrustum[2].a;
//		n.y = m_planeFrustum[2].b;
//		n.z = m_planeFrustum[2].c;
//
//		ChaVector3 vec1, vec2, vec3, vec4;
//		float dot1, dot2, dot3, dot4;
//
//		vec1 = tracenter - m_vecTraFrustum[1];
//		ChaVector3Normalize(&vec1, &vec1);
//		dot1 = ChaVector3Dot(&n, &vec1);
//
//		vec2 = tracenter - m_vecTraFrustum[3];
//		ChaVector3Normalize(&vec2, &vec2);
//		dot2 = ChaVector3Dot(&n, &vec2);
//
//		vec3 = tracenter - m_vecTraFrustum[5];
//		ChaVector3Normalize(&vec3, &vec3);
//		dot3 = ChaVector3Dot(&n, &vec3);
//
//		vec4 = tracenter - m_vecTraFrustum[7];
//		ChaVector3Normalize(&vec4, &vec4);
//		dot4 = ChaVector3Dot(&n, &vec4);
//
//		if ((dot1 >= 0.0f) && (dot2 >= 0.0f) && (dot3 >= 0.0f) && (dot4 >= 0.0f)) {
//			chk4 = 1;
//		}
//		else if ((dot1 <= 0.0f) && (dot2 <= 0.0f) && (dot3 <= 0.0f) && (dot4 <= 0.0f)) {
//			chk4 = -1;
//		}
//		else {
//			chk4 = 0;
//		}
//	}
//
//	//top
//	{
//		ChaVector3 n;
//		n.x = m_planeFrustum[4].a;
//		n.y = m_planeFrustum[4].b;
//		n.z = m_planeFrustum[4].c;
//
//		ChaVector3 vec1, vec2, vec3, vec4;
//		float dot1, dot2, dot3, dot4;
//
//		vec1 = tracenter - m_vecTraFrustum[2];
//		ChaVector3Normalize(&vec1, &vec1);
//		dot1 = ChaVector3Dot(&n, &vec1);
//
//		vec2 = tracenter - m_vecTraFrustum[3];
//		ChaVector3Normalize(&vec2, &vec2);
//		dot2 = ChaVector3Dot(&n, &vec2);
//
//		vec3 = tracenter - m_vecTraFrustum[6];
//		ChaVector3Normalize(&vec3, &vec3);
//		dot3 = ChaVector3Dot(&n, &vec3);
//
//		vec4 = tracenter - m_vecTraFrustum[7];
//		ChaVector3Normalize(&vec4, &vec4);
//		dot4 = ChaVector3Dot(&n, &vec4);
//
//		if ((dot1 >= 0.0f) && (dot2 >= 0.0f) && (dot3 >= 0.0f) && (dot4 >= 0.0f)) {
//			chk5 = 1;
//		}
//		else if ((dot1 <= 0.0f) && (dot2 <= 0.0f) && (dot3 <= 0.0f) && (dot4 <= 0.0f)) {
//			chk5 = -1;
//		}
//		else {
//			chk5 = 0;
//		}
//	}
//
//	//bottom
//	{
//		ChaVector3 n;
//		//n.x = m_planeFrustum[5].a;
//		//n.y = m_planeFrustum[5].b;
//		//n.z = m_planeFrustum[5].c;
//		n.x = m_planeFrustum[4].a;
//		n.y = m_planeFrustum[4].b;
//		n.z = m_planeFrustum[4].c;
//
//		ChaVector3 vec1, vec2, vec3, vec4;
//		float dot1, dot2, dot3, dot4;
//
//		vec1 = tracenter - m_vecTraFrustum[0];
//		ChaVector3Normalize(&vec1, &vec1);
//		dot1 = ChaVector3Dot(&n, &vec1);
//
//		vec2 = tracenter - m_vecTraFrustum[1];
//		ChaVector3Normalize(&vec2, &vec2);
//		dot2 = ChaVector3Dot(&n, &vec2);
//
//		vec3 = tracenter - m_vecTraFrustum[4];
//		ChaVector3Normalize(&vec3, &vec3);
//		dot3 = ChaVector3Dot(&n, &vec3);
//
//		vec4 = tracenter - m_vecTraFrustum[5];
//		ChaVector3Normalize(&vec4, &vec4);
//		dot4 = ChaVector3Dot(&n, &vec4);
//
//		if ((dot1 >= 0.0f) && (dot2 >= 0.0f) && (dot3 >= 0.0f) && (dot4 >= 0.0f)) {
//			chk6 = 1;
//		}
//		else if ((dot1 <= 0.0f) && (dot2 <= 0.0f) && (dot3 <= 0.0f) && (dot4 <= 0.0f)) {
//			chk6 = -1;
//		}
//		else {
//			chk6 = 0;
//		}
//	}
//
//	//if ((pluscount == 0) || (minuscount == 0)) {
//	//	SetVisible(true);
//	//}
//	//else {
//	//	SetVisible(false);
//	//}
//
//	//if (((chk1 * chk2) == -1) && ((chk3 * chk4) == -1) && ((chk5 * chk6) == -1)) {
//	//if (((chk3 * chk4) == -1) && ((chk5 * chk6) == -1)) {
//	if ((chk3 * chk4) == -1) {
//		SetVisible(true);
//	}
//	else {
//		SetVisible(false);
//	}
//
//
//
//	return 0;
//}


int ChaFrustumInfo::ChkInView(int srclodnum, int srclodno, MODELBOUND srcmb, ChaMatrix matWorld)
{
	//#######################################################
	//AdditiveIK 1.0.0.14にて
	//コンピュートシェーダによる視錐体判定に移行したのでコメントアウト
	//#######################################################


	////###################################
	////視錐体より　軽くて　まあまあの精度
	////###################################

	//ChaVector3 tracenter;
	//ChaVector3TransformCoord(&tracenter, &(srcmb.center), &matWorld);
	////tracenter = srcmb.center;

	//{
	//	ChaVector3 camdir = g_camtargetpos - g_camEye;
	//	ChaVector3Normalize(&camdir, &camdir);


	//	ChaVector3 cam2obj = tracenter - g_camEye;
	//	float dist_cam2obj = (float)ChaVector3LengthDbl(&cam2obj);

	//	ChaVector3 backpos = g_camEye - camdir * srcmb.r * CHKINVIEW_BACKPOSCOEF;//!!!!!!!!!!!
	//	ChaVector3 back2obj = tracenter - backpos;
	//	ChaVector3Normalize(&back2obj, &back2obj);
	//	float dot = ChaVector3Dot(&camdir, &back2obj);

	//	//float dotclip = (float)cos(g_fovy);
	//	float dotclip = (float)cos(g_fovy * 0.85f);//2023/12/30 backposの分　狭くて済むはず



	//	//############################
	//	//LODがvisibleかどうかチェック
	//	//############################
	//	float lod_mindist = 0.0f;
	//	float lod_maxdist = g_projfar;
	//	switch (srclodno) {
	//	case -1:
	//		//LOD無し
	//		lod_mindist = 0.0f;
	//		lod_maxdist = g_projfar;
	//		break;
	//	case CHKINVIEW_LOD0:
	//		lod_mindist = 0.0f;
	//		if (srclodnum == 3) {//3段階LOD
	//			lod_maxdist = g_projfar * g_lodrate3L[srclodno];
	//		}
	//		else {//2段階LOD
	//			lod_maxdist = g_projfar * g_lodrate2L[srclodno];
	//		}
	//		break;
	//	case CHKINVIEW_LOD1:
	//	case CHKINVIEW_LOD2:
	//		if (srclodnum == 3) {//3段階LOD
	//			lod_mindist = g_projfar * g_lodrate3L[srclodno - 1];
	//			lod_maxdist = g_projfar * g_lodrate3L[srclodno];
	//		}
	//		else {//2段階LOD
	//			lod_mindist = g_projfar * g_lodrate2L[srclodno - 1];
	//			lod_maxdist = g_projfar * g_lodrate2L[srclodno];
	//		}
	//		break;
	//	default:
	//		_ASSERT(0);
	//		//LOD無し
	//		lod_mindist = g_projnear;
	//		lod_maxdist = g_projfar;
	//		break;
	//	}
	//	bool lodinview;
	//	if (srclodno >= 0) {
	//		if ((dist_cam2obj < lod_maxdist) && (dist_cam2obj >= lod_mindist)) {
	//			lodinview = true;
	//		}
	//		else {
	//			lodinview = false;
	//		}
	//	}
	//	else {
	//		lodinview = true;
	//	}


	//	//##################
	//	//LODがvisible && 
	//	//##################
	//	if (lodinview && (dot >= dotclip) &&
	//		(dist_cam2obj < (g_projfar + srcmb.r))) {
	//		SetVisible(true);
	//	}
	//	else {
	//		SetVisible(false);
	//	}
	//	SetDistFromCamera(dist_cam2obj);//2024/01/19

	//}

	//if (GetVisible()) {
	//	//#####################################################################
	//	//視野内の場合には　シャドウマップがシャドウ射影範囲に入っているかどうかの判定をする
	//	//#####################################################################

	//	if ((g_shadowmap_slotno < 0) || (g_shadowmap_slotno >= SHADOWSLOTNUM)) {
	//		_ASSERT(0);
	//		g_shadowmap_slotno = 0;
	//	}

	//	ChaVector3 lightpos;
	//	ChaVector3 lighttarget;
	//	if (g_cameraShadow) {
	//		lightpos.SetParams(g_cameraShadow->GetPosition());
	//		lighttarget.SetParams(g_cameraShadow->GetTarget());
	//	}
	//	else {
	//		lightpos = g_camEye;
	//		lighttarget = g_camtargetpos;
	//	}

	//	ChaVector3 camdir = lighttarget - lightpos;
	//	ChaVector3Normalize(&camdir, &camdir);

	//	ChaVector3 cam2obj = tracenter - lightpos;
	//	float dist_cam2obj = (float)ChaVector3LengthDbl(&cam2obj);

	//	ChaVector3 backpos = lightpos - camdir * srcmb.r * CHKINVIEW_BACKPOSCOEF;//!!!!!!!!!!!
	//	ChaVector3 back2obj = tracenter - backpos;
	//	ChaVector3Normalize(&back2obj, &back2obj);
	//	float dot = ChaVector3Dot(&camdir, &back2obj);

	//	float dotclip = (float)cos(g_shadowmap_fov[g_shadowmap_slotno]);

	//	float mindist = 0.0f;//g_shadowmap_near[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno];
	//	float maxdist = g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno];

	//	if ((dot >= dotclip) && 
	//		(dist_cam2obj < (maxdist + srcmb.r)) && (dist_cam2obj >= mindist)) {
	//		SetInShadow(true);
	//	}
	//	else {
	//		SetInShadow(false);
	//	}
	//}
	//else {
	//	//#####################
	//	//視野外は　シャドウ領域外
	//	//#####################

	//	SetInShadow(false);
	//}


	return 0;
}


void ChaFrustumInfo::InitParams()
{
	int pointno;
	for (pointno = 0; pointno < 8; pointno++) {
		m_vecFrustum[pointno].SetZeroVec3();
		m_vecTraFrustum[pointno].SetZeroVec3();
	}
	int planeno;
	for (planeno = 0; planeno < 6; planeno++) {
		m_planeFrustum[planeno].SetParams(0.0f, 0.0f, -1.0f, 0.0f);
		m_footpos[planeno].SetZeroVec3();
	}

	m_matVP.SetIdentity();

	SetVisible(true);
	SetInShadow(false);

	m_distfromcamera = 0.0f;
}

//################
//ChaPlaneに移動
//################
//int ChaFrustumInfo::GetFootOnPlane(int srcplaneindex, ChaVector3 srcpos)
//{
//	if ((srcplaneindex < 0) || (srcplaneindex >= 6)) {
//		_ASSERT(0);
//		return 1;
//	}
//
//	ChaPlane curplane = m_planeFrustum[srcplaneindex];
//
//	double t0, tmpt1, t1, t;
//	t0 = curplane.a * srcpos.x + curplane.b * srcpos.y + curplane.c * srcpos.z + curplane.d;
//	tmpt1 = curplane.a * curplane.a + curplane.b * curplane.b + curplane.c * curplane.c;
//	if (tmpt1 != 0.0f) {
//		t1 = sqrt(tmpt1);
//	}
//	else {
//		t1 = 0.0f;
//	}
//	
//	if (t1 != 0.0f) {
//		t = -(t0 / t1);
//	}
//	else {
//		t = 0.0f;
//	}
//
//	m_footpos[srcplaneindex].x = (float)((double)srcpos.x + t * (double)curplane.a);
//	m_footpos[srcplaneindex].y = (float)((double)srcpos.y + t * (double)curplane.b);
//	m_footpos[srcplaneindex].z = (float)((double)srcpos.z + t * (double)curplane.c);
//
//	return 0;
//}




