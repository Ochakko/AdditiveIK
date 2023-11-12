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
#include <ChaVecCalc.h>


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
#include <algorithm>

#include <crtdbg.h>



extern bool g_wmatDirectSetFlag;//!!!!!!!!!!!!


								//BoneProp

double vecDotVec(ChaVector3* vec1, ChaVector3* vec2)
{
	return ((double)vec1->x * (double)vec2->x + (double)vec1->y * (double)vec2->y + (double)vec1->z * (double)vec2->z);
}

double lengthVec(ChaVector3* vec)
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

double aCos(double dot)
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

int vec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion iniq;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ret = dirq.SetRotationXYZ(&iniq, 0, deg, 0);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	dstvec->x = (float)((double)dirm._11 * (double)srcvec->x + (double)dirm._21 * (double)srcvec->y + (double)dirm._31 * (double)srcvec->z + (double)dirm._41);
	dstvec->y = (float)((double)dirm._12 * (double)srcvec->x + (double)dirm._22 * (double)srcvec->y + (double)dirm._32 * (double)srcvec->z + (double)dirm._42);
	dstvec->z = (float)((double)dirm._13 * (double)srcvec->x + (double)dirm._23 * (double)srcvec->y + (double)dirm._33 * (double)srcvec->z + (double)dirm._43);

	return 0;
}
int vec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion iniq;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ret = dirq.SetRotationXYZ(&iniq, deg, 0, 0);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	dstvec->x = (float)((double)dirm._11 * (double)srcvec->x + (double)dirm._21 * (double)srcvec->y + (double)dirm._31 * (double)srcvec->z + (double)dirm._41);
	dstvec->y = (float)((double)dirm._12 * (double)srcvec->x + (double)dirm._22 * (double)srcvec->y + (double)dirm._32 * (double)srcvec->z + (double)dirm._42);
	dstvec->z = (float)((double)dirm._13 * (double)srcvec->x + (double)dirm._23 * (double)srcvec->y + (double)dirm._33 * (double)srcvec->z + (double)dirm._43);


	return 0;
}
int vec3RotateZ(ChaVector3* dstvec, float deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion iniq;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ret = dirq.SetRotationXYZ(&iniq, 0, 0, deg);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	dstvec->x = (float)((double)dirm._11 * (double)srcvec->x + (double)dirm._21 * (double)srcvec->y + (double)dirm._31 * (double)srcvec->z + (double)dirm._41);
	dstvec->y = (float)((double)dirm._12 * (double)srcvec->x + (double)dirm._22 * (double)srcvec->y + (double)dirm._32 * (double)srcvec->z + (double)dirm._42);
	dstvec->z = (float)((double)dirm._13 * (double)srcvec->x + (double)dirm._23 * (double)srcvec->y + (double)dirm._33 * (double)srcvec->z + (double)dirm._43);

	return 0;
}


int qToEulerAxis(CQuaternion axisQ, CQuaternion* srcq, ChaVector3* Euler)
{

	int ret;
	CQuaternion invaxisQ, EQ;
	ret = axisQ.inv(&invaxisQ);
	_ASSERT(!ret);
	EQ = invaxisQ * *srcq * axisQ;


	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);

	ChaVector3 targetVec, shadowVec;
	ChaVector3 tmpVec;

	EQ.Rotate(&targetVec, axisZVec);
	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
	shadowVec.y = 0.0f;
	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);
	if (lengthVec(&shadowVec) == 0.0) {
		Euler->y = 90.0f;
	}
	else {
		Euler->y = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisXVec) < 0.0) {
		Euler->y = -Euler->y;
	}

	/***
	E3DMultQVec targetQ,0.0,0.0,1.0,targetVec.0,targetVec.1,targetVec.2
	shadowVec.0= vecDotVec(targetVec,axisXVec)
	shadowVec.1= 0.0
	shadowVec.2= vecDotVec(targetVec,axisZVec)
	if( lengthVec(shadowVec) == 0 ){
	eulerY= 90.0
	}else{
	E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerY		//Y軸回転度数
	}
	if( vecDotVec(shadowVec,axisXVec) < 0.0 ){ eulerY= -eulerY }				//右回りに修正
	***/
	vec3RotateY(&tmpVec, -Euler->y, &targetVec);
	shadowVec.x = 0.0f;
	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
	if (lengthVec(&shadowVec) == 0.0) {
		Euler->x = 90.0f;
	}
	else {
		Euler->x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisYVec) > 0.0) {
		Euler->x = -Euler->x;
	}
	/***
	E3DVec3RotateY targetVec.0,targetVec.1,targetVec.2,(-eulerY),shadowVec.0,shadowVec.1,shadowVec.2
	if( lengthVec(shadowVec) == 0 ){
	eulerX= 90.0
	}else{
	E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerX		//X軸回転度数
	}
	if( vecDotVec(shadowVec,axisYVec) > 0.0 ){ eulerX= -eulerX }				//右回りに修正
	***/

	EQ.Rotate(&targetVec, axisYVec);
	vec3RotateY(&tmpVec, -Euler->y, &targetVec);
	targetVec = tmpVec;
	vec3RotateX(&tmpVec, -Euler->x, &targetVec);
	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
	shadowVec.z = 0.0f;
	if (lengthVec(&shadowVec) == 0.0) {
		Euler->z = 90.0f;
	}
	else {
		Euler->z = (float)aCos(vecDotVec(&shadowVec, &axisYVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisXVec) > 0.0) {
		Euler->z = -Euler->z;
	}
	/***
	E3DMultQVec targetQ,0.0,1.0,0.0,targetVec.0,targetVec.1,targetVec.2
	E3DVec3RotateY (targetVec.0+0.0),(targetVec.1+0.0),(targetVec.2+0.0),(-eulerY),targetVec.0,targetVec.1,targetVec.2
	E3DVec3RotateY targetVec.2,targetVec.0,targetVec.1,(-eulerX),shadowVec.2,shadowVec.0,shadowVec.1
	　　----> 引数がZ,X,YになっているためRotateXと同じ！！！

	  if( lengthVec(shadowVec) == 0 ){
	  eulerZ= 90.0
	  }else{
	  E3DACos ( vecDotVec(shadowVec,axisYVec)/ lengthVec(shadowVec) ), eulerZ		//Z軸回転度数
	  }
	  if( vecDotVec(shadowVec,axisXVec) > 0.0 ){ eulerZ= -eulerZ }				//右回りに修正

	  ***/
	return 0;
}



int GetRound(float srcval)
{
	if (srcval > 0.0f) {
		return (int)(srcval + 0.5f);
	}
	else {
		return (int)(srcval - 0.5f);
	}
}

//int modifyEuler(ChaVector3* eulerA, ChaVector3* eulerB)
//{
//
//	//オイラー角Aの値をオイラー角Bの値に近い表示に修正
//	float tmpX1, tmpY1, tmpZ1;
//	float tmpX2, tmpY2, tmpZ2;
//	float s1, s2;
//
//	//予想される角度1
//	tmpX1 = eulerA->x + 360.0f * GetRound((eulerB->x - eulerA->x) / 360.0f);
//	tmpY1 = eulerA->y + 360.0f * GetRound((eulerB->y - eulerA->y) / 360.0f);
//	tmpZ1 = eulerA->z + 360.0f * GetRound((eulerB->z - eulerA->z) / 360.0f);
//
//	//予想される角度2
//	tmpX2 = 180.0f - eulerA->x + 360.0f * GetRound((eulerB->x + eulerA->x - 180.0f) / 360.0f);
//	tmpY2 = eulerA->y + 180.0f + 360.0f * GetRound((eulerB->y - eulerA->y - 180.0f) / 360.0f);
//	tmpZ2 = eulerA->z + 180.0f + 360.0f * GetRound((eulerB->z - eulerA->z - 180.0f) / 360.0f);
//
//	//角度変化の大きさ
//	s1 = (eulerB->x - tmpX1) * (eulerB->x - tmpX1) + (eulerB->y - tmpY1) * (eulerB->y - tmpY1) + (eulerB->z - tmpZ1) * (eulerB->z - tmpZ1);
//	s2 = (eulerB->x - tmpX2) * (eulerB->x - tmpX2) + (eulerB->y - tmpY2) * (eulerB->y - tmpY2) + (eulerB->z - tmpZ2) * (eulerB->z - tmpZ2);
//
//	//変化の少ない方に修正
//	if (s1 < s2) {
//		eulerA->x = tmpX1; eulerA->y = tmpY1; eulerA->z = tmpZ1;
//	}
//	else {
//		eulerA->x = tmpX2; eulerA->y = tmpY2; eulerA->z = tmpZ2;
//	}
//
//	return 0;
//}

int IsInitRot(ChaMatrix srcmat)
{
	int retval = 0;

	float d11, d12, d13;
	float d21, d22, d23;
	float d31, d32, d33;

	d11 = srcmat._11 - 1.0f;
	d12 = srcmat._12 - 0.0f;
	d13 = srcmat._13 - 0.0f;

	d21 = srcmat._21 - 0.0f;
	d22 = srcmat._22 - 1.0f;
	d23 = srcmat._23 - 0.0f;

	d31 = srcmat._31 - 0.0f;
	d32 = srcmat._32 - 0.0f;
	d33 = srcmat._33 - 1.0f;

	float dmin = 0.000001f;

	if ((fabs(d11) <= dmin) && (fabs(d12) <= dmin) && (fabs(d13) <= dmin) &&
		(fabs(d21) <= dmin) && (fabs(d22) <= dmin) && (fabs(d23) <= dmin) &&
		(fabs(d31) <= dmin) && (fabs(d32) <= dmin) && (fabs(d33) <= dmin)) {
		retval = 1;
	}

	return retval;
}


int IsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2)
{

	int retval = 0;
	ChaMatrix diffmat;
	diffmat = srcmat1 - srcmat2;

	float dmin = 0.000001f;

	if ((fabs(diffmat._11) <= dmin) && (fabs(diffmat._12) <= dmin) && (fabs(diffmat._13) <= dmin) && (fabs(diffmat._14) <= dmin) &&
		(fabs(diffmat._21) <= dmin) && (fabs(diffmat._22) <= dmin) && (fabs(diffmat._23) <= dmin) && (fabs(diffmat._24) <= dmin) &&
		(fabs(diffmat._31) <= dmin) && (fabs(diffmat._32) <= dmin) && (fabs(diffmat._33) <= dmin) && (fabs(diffmat._34) <= dmin) &&
		(fabs(diffmat._41) <= dmin) && (fabs(diffmat._42) <= dmin) && (fabs(diffmat._43) <= dmin) && (fabs(diffmat._44) <= dmin)
		) {
		retval = 1;
	}

	return retval;
}

int IsSameEul(ChaVector3 srceul1, ChaVector3 srceul2)
{
	int retval = 0;

	ChaVector3 diffeul;
	diffeul = srceul1 - srceul2;

	//float dmin = 0.000015f;
	float dmin = 0.00010f;//(degree)

	if ((fabs(diffeul.x) <= dmin) && (fabs(diffeul.y) <= dmin) && (fabs(diffeul.z) <= dmin)) {
		retval = 1;
	}

	return retval;
}

void SetAngleLimitOff(ANGLELIMIT* dstal)
{
	if (dstal) {
		int axiskind;
		for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
			if ((dstal->lower[axiskind] == -180) && (dstal->upper[axiskind] == 180)) {
				dstal->limitoff[axiskind] = 1;
			}
			else {
				dstal->limitoff[axiskind] = 0;
			}
		}
	}
}

void InitAngleLimit(ANGLELIMIT* dstal)
{
	dstal->boneaxiskind = BONEAXIS_CURRENT;

	if (dstal) {
		int axiskind;
		for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
			dstal->limitoff[axiskind] = 0;
			dstal->via180flag[axiskind] = 0;
			dstal->lower[axiskind] = -180;
			dstal->upper[axiskind] = 180;
		}
		SetAngleLimitOff(dstal);
	}
}



bool IsTimeEqual(double srctime1, double srctime2)
{
	double difftime = srctime1 - srctime2;
	if ((difftime >= -0.0001) && (difftime <= 0.0001)) {
		return true;
	}
	else {
		return false;
	}
}

double VecLength(ChaVector3 srcvec)
{
	double tmpval = (double)srcvec.x * (double)srcvec.x + (double)srcvec.y * (double)srcvec.y + (double)srcvec.z * (double)srcvec.z;
	if (tmpval > 0.0) {
		return sqrt(tmpval);
	}
	else {
		return 0.0;
	}
}



void GetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr)
{
	*svecptr = ChaVector3(1.0f, 1.0f, 1.0f);
	ChaMatrixIdentity(rmatptr);
	*tvecptr = ChaVector3(0.0f, 0.0f, 0.0f);

	ChaMatrix tmpmat1 = srcmat;

	tvecptr->x = tmpmat1._41;
	tvecptr->y = tmpmat1._42;
	tvecptr->z = tmpmat1._43;

	tmpmat1._41 = 0.0f;
	tmpmat1._42 = 0.0f;
	tmpmat1._43 = 0.0f;

	ChaVector3 vec1, vec2, vec3;
	vec1.x = tmpmat1._11;
	vec1.y = tmpmat1._12;
	vec1.z = tmpmat1._13;

	vec2.x = tmpmat1._21;
	vec2.y = tmpmat1._22;
	vec2.z = tmpmat1._23;

	vec3.x = tmpmat1._31;
	vec3.y = tmpmat1._32;
	vec3.z = tmpmat1._33;

	double len1, len2, len3;
	len1 = VecLength(vec1);
	len2 = VecLength(vec2);
	len3 = VecLength(vec3);

	if ((fabs(len1) - 1.0) < 0.0001) {
		len1 = 1.0;//誤差で急に大きさが変わるのを防止
	}
	if (fabs(len1) < 0.0001) {
		len1 = 0.0001;//0scale禁止
	}
	if ((fabs(len2) - 1.0) < 0.0001) {
		len2 = 1.0;//誤差で急に大きさが変わるのを防止
	}
	if (fabs(len2) < 0.0001) {
		len2 = 0.0001;//0scale禁止
	}
	if ((fabs(len3) - 1.0) < 0.0001) {
		len3 = 1.0;//誤差で急に大きさが変わるのを防止
	}
	if (fabs(len3) < 0.0001) {
		len3 = 0.0001;//0scale禁止
	}
	svecptr->x = (float)len1;
	svecptr->y = (float)len2;
	svecptr->z = (float)len3;

	if (len1 != 0.0) {
		rmatptr->_11 = (float)((double)tmpmat1._11 / len1);
		rmatptr->_12 = (float)((double)tmpmat1._12 / len1);
		rmatptr->_13 = (float)((double)tmpmat1._13 / len1);
	}
	else {
		rmatptr->_11 = 1.0f;
		rmatptr->_12 = 0.0f;
		rmatptr->_13 = 0.0f;
	}

	if (len2 != 0.0f) {
		rmatptr->_21 = (float)((double)tmpmat1._21 / len2);
		rmatptr->_22 = (float)((double)tmpmat1._22 / len2);
		rmatptr->_23 = (float)((double)tmpmat1._23 / len2);
	}
	else {
		rmatptr->_21 = 0.0f;
		rmatptr->_22 = 1.0f;
		rmatptr->_23 = 0.0f;
	}

	if (len3 != 0.0f) {
		rmatptr->_31 = (float)((double)tmpmat1._31 / len3);
		rmatptr->_32 = (float)((double)tmpmat1._32 / len3);
		rmatptr->_33 = (float)((double)tmpmat1._33 / len3);
	}
	else {
		rmatptr->_31 = 0.0f;
		rmatptr->_32 = 0.0f;
		rmatptr->_33 = 1.0f;
	}


}

void GetSRTMatrix2(ChaMatrix srcmat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr)
{
	if (!smatptr || !rmatptr || !tmatptr) {
		return;
	}

	ChaMatrix smat, rmat, tmat;
	ChaMatrixIdentity(&smat);
	ChaMatrixIdentity(&rmat);
	ChaMatrixIdentity(&tmat);

	ChaVector3 svec = ChaVector3(0.0, 0.0, 0.0);
	ChaVector3 tvec = ChaVector3(0.0, 0.0, 0.0);

	GetSRTMatrix(srcmat, &svec, &rmat, &tvec);

	ChaMatrixScaling(&smat, svec.x, svec.y, svec.z);
	ChaMatrixTranslation(&tmat, tvec.x, tvec.y, tvec.z);

	*smatptr = smat;
	*rmatptr = rmat;
	*tmatptr = tmat;
}

ChaMatrix GetS0RTMatrix(ChaMatrix srcmat) 
{
	//拡大縮小を初期化したRT行列を返す
	ChaMatrix retm;
	ChaVector3 svec, tvec;
	ChaMatrix rmat, tmat;
	GetSRTMatrix(srcmat, &svec, &rmat, &tvec);

	retm = srcmat;

	if (svec.x != 0.0f) {
		retm._11 = (float)((double)retm._11 / (double)svec.x);
		retm._12 = (float)((double)retm._12 / (double)svec.x);
		retm._13 = (float)((double)retm._13 / (double)svec.x);
	}
	else {
		retm._11 = 1.0f;
		retm._12 = 0.0f;
		retm._13 = 0.0f;
	}

	if (svec.y != 0.0f) {
		retm._21 = (float)((double)retm._21 / (double)svec.y);
		retm._22 = (float)((double)retm._22 / (double)svec.y);
		retm._23 = (float)((double)retm._23 / (double)svec.y);
	}
	else {
		retm._21 = 0.0f;
		retm._22 = 1.0f;
		retm._23 = 0.0f;
	}

	if (svec.z != 0.0f) {
		retm._31 = (float)((double)retm._31 / (double)svec.z);
		retm._32 = (float)((double)retm._32 / (double)svec.z);
		retm._33 = (float)((double)retm._33 / (double)svec.z);
	}
	else {
		retm._31 = 0.0f;
		retm._32 = 0.0f;
		retm._33 = 1.0f;
	}

	return retm;
}




ChaMatrix TransZeroMat(ChaMatrix srcmat)
{
	ChaMatrix retmat;
	retmat = srcmat;
	retmat._41 = 0.0f;
	retmat._42 = 0.0f;
	retmat._43 = 0.0f;

	return retmat;
}

ChaMatrix ChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (!srcmat3x3) {
		return retmat;
	}

	btVector3 tmpcol[3];
	int colno;
	for (colno = 0; colno < 3; colno++) {
		tmpcol[colno] = srcmat3x3->getColumn(colno);
		//tmpcol[colno] = worldmat.getRow( colno );
	}

	retmat._11 = tmpcol[0].x();
	retmat._12 = tmpcol[0].y();
	retmat._13 = tmpcol[0].z();

	retmat._21 = tmpcol[1].x();
	retmat._22 = tmpcol[1].y();
	retmat._23 = tmpcol[1].z();

	retmat._31 = tmpcol[2].x();
	retmat._32 = tmpcol[2].y();
	retmat._33 = tmpcol[2].z();

	return retmat;
}

ChaMatrix ChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpivot)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	btVector3 tmpcol[3];
	int colno;
	for (colno = 0; colno < 3; colno++) {
		tmpcol[colno] = srcmat3x3->getColumn(colno);
		//tmpcol[colno] = worldmat.getRow( colno );
	}

	retmat._11 = tmpcol[0].x();
	retmat._12 = tmpcol[0].y();
	retmat._13 = tmpcol[0].z();

	retmat._21 = tmpcol[1].x();
	retmat._22 = tmpcol[1].y();
	retmat._23 = tmpcol[1].z();

	retmat._31 = tmpcol[2].x();
	retmat._32 = tmpcol[2].y();
	retmat._33 = tmpcol[2].z();

	retmat._41 = srcpivot->x();
	retmat._42 = srcpivot->y();
	retmat._43 = srcpivot->z();

	return retmat;
}

ChaMatrix ChaMatrixInv(ChaMatrix srcmat)
{
	ChaMatrix retmat;
	ChaMatrixInverse(&retmat, NULL, &srcmat);
	return retmat;
}



//ChaVector2

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
ChaVector2& ChaVector2::operator/= (float srcw) { if (srcw != 0.0f) { *this = *this / srcw; return *this; } else { this->x = 0.0f; this->y = 0.0f; return *this; } }
ChaVector2 ChaVector2::operator* (double srcw) const { return ChaVector2((float)((double)this->x * srcw), (float)((double)this->y * srcw)); }
ChaVector2& ChaVector2::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaVector2 ChaVector2::operator/ (double srcw) const { if (srcw != 0.0) { return ChaVector2((float)((double)this->x / srcw), (float)((double)this->y / srcw)); } else { return ChaVector2(0.0f, 0.0f); } }
ChaVector2& ChaVector2::operator/= (double srcw) { if (srcw != 0.0) { *this = *this / srcw; return *this; } else { this->x = 0.0f; this->y = 0.0f; return *this; } }
ChaVector2 ChaVector2::operator+ (const ChaVector2 &v) const { return ChaVector2(x + v.x, y + v.y); }
ChaVector2 &ChaVector2::operator+= (const ChaVector2 &v) { *this = *this + v; return *this; }
ChaVector2 ChaVector2::operator- (const ChaVector2 &v) const { return ChaVector2(x - v.x, y - v.y); }
ChaVector2 &ChaVector2::operator-= (const ChaVector2 &v) { *this = *this - v; return *this; }

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

ChaVector3::~ChaVector3()
{

}


ChaVector3 ChaVector3::operator= (ChaVector3 v) { this->x = v.x; this->y = v.y; this->z = v.z; return *this; };
ChaVector3 ChaVector3::operator* (float srcw) const { return ChaVector3((float)((double)this->x * (double)srcw), (float)((double)this->y * (double)srcw), (float)((double)this->z * (double)srcw)); }
ChaVector3 &ChaVector3::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaVector3 ChaVector3::operator/ (float srcw) const { if (srcw != 0.0f) { return ChaVector3((float)((double)this->x / (double)srcw), (float)((double)this->y / (double)srcw), (float)((double)this->z / (double)srcw)); } else { return ChaVector3(0.0f, 0.0f, 0.0f); } }
ChaVector3 &ChaVector3::operator/= (float srcw) { *this = *this / srcw; return *this; }
ChaVector3 ChaVector3::operator* (double srcw) const { return ChaVector3((float)((double)this->x * srcw), (float)((double)this->y * srcw), (float)((double)this->z * srcw)); }
ChaVector3& ChaVector3::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaVector3 ChaVector3::operator/ (double srcw) const { if (srcw != 0.0) { return ChaVector3((float)((double)this->x / srcw), (float)((double)this->y / srcw), (float)((double)this->z / srcw)); } else { return ChaVector3(0.0f, 0.0f, 0.0f); } }
ChaVector3& ChaVector3::operator/= (double srcw) { *this = *this / srcw; return *this; }
ChaVector3 ChaVector3::operator+ (const ChaVector3 &v) const { return ChaVector3(x + v.x, y + v.y, z + v.z); }
ChaVector3 &ChaVector3::operator+= (const ChaVector3 &v) { *this = *this + v; return *this; }
ChaVector3 ChaVector3::operator- (const ChaVector3 &v) const { return ChaVector3(x - v.x, y - v.y, z - v.z); }
ChaVector3 &ChaVector3::operator-= (const ChaVector3 &v) { *this = *this - v; return *this; }

ChaVector3 ChaVector3::operator- () const { return *this * -1.0; }



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

ChaVector4::~ChaVector4()
{

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

ChaVector4 ChaVector4::operator- () const { return *this * -1.0; }


ChaMatrix::ChaMatrix()
{
	_11 = 1.0f;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;

	_21 = 0.0f;
	_22 = 1.0f;
	_23 = 0.0f;
	_24 = 0.0f;

	_31 = 0.0f;
	_32 = 0.0f;
	_33 = 1.0f;
	_34 = 0.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = 0.0f;
	_44 = 1.0f;

}

ChaMatrix::ChaMatrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	_11 = m11;
	_12 = m12;
	_13 = m13;
	_14 = m14;

	_21 = m21;
	_22 = m22;
	_23 = m23;
	_24 = m24;

	_31 = m31;
	_32 = m32;
	_33 = m33;
	_34 = m34;

	_41 = m41;
	_42 = m42;
	_43 = m43;
	_44 = m44;

}

#ifdef CONVD3DX11
ChaMatrix::ChaMatrix(DirectX::XMMATRIX m) {
	_11 = m.r[0].m128_f32[0];
	_12 = m.r[0].m128_f32[1];
	_13 = m.r[0].m128_f32[2];
	_14 = m.r[0].m128_f32[3];

	_21 = m.r[1].m128_f32[0];
	_22 = m.r[1].m128_f32[1];
	_23 = m.r[1].m128_f32[2];
	_24 = m.r[1].m128_f32[3];

	_31 = m.r[2].m128_f32[0];
	_32 = m.r[2].m128_f32[1];
	_33 = m.r[2].m128_f32[2];
	_34 = m.r[2].m128_f32[3];

	_41 = m.r[3].m128_f32[0];
	_42 = m.r[3].m128_f32[1];
	_43 = m.r[3].m128_f32[2];
	_44 = m.r[3].m128_f32[3];

	//_11 = m.r[0].m128_f32[0];
	//_12 = m.r[1].m128_f32[0];
	//_13 = m.r[2].m128_f32[0];
	//_14 = m.r[3].m128_f32[0];

	//_21 = m.r[0].m128_f32[1];
	//_22 = m.r[1].m128_f32[1];
	//_23 = m.r[2].m128_f32[1];
	//_24 = m.r[3].m128_f32[1];

	//_31 = m.r[0].m128_f32[2];
	//_32 = m.r[1].m128_f32[2];
	//_33 = m.r[2].m128_f32[2];
	//_34 = m.r[3].m128_f32[2];

	//_41 = m.r[0].m128_f32[3];
	//_42 = m.r[1].m128_f32[3];
	//_43 = m.r[2].m128_f32[3];
	//_44 = m.r[3].m128_f32[3];

};
#endif


ChaMatrix::~ChaMatrix()
{


}



ChaMatrix ChaMatrix::operator= (ChaMatrix m) { 
	this->_11 = m._11; 
	this->_12 = m._12; 
	this->_13 = m._13;
	this->_14 = m._14;
	
	this->_21 = m._21;
	this->_22 = m._22;
	this->_23 = m._23;
	this->_24 = m._24;

	this->_31 = m._31;
	this->_32 = m._32;
	this->_33 = m._33;
	this->_34 = m._34;

	this->_41 = m._41;
	this->_42 = m._42;
	this->_43 = m._43;
	this->_44 = m._44;

	return *this;
};



ChaMatrix ChaMatrix::operator* (float srcw) const {
	return ChaMatrix((float)((double)this->_11 * (double)srcw), (float)((double)this->_12 * (double)srcw), (float)((double)this->_13 * (double)srcw), (float)((double)this->_14 * (double)srcw),
		(float)((double)this->_21 * (double)srcw), (float)((double)this->_22 * (double)srcw), (float)((double)this->_23 * (double)srcw), (float)((double)this->_24 * (double)srcw),
		(float)((double)this->_31 * (double)srcw), (float)((double)this->_32 * (double)srcw), (float)((double)this->_33 * (double)srcw), (float)((double)this->_34 * (double)srcw),
		(float)((double)this->_41 * (double)srcw), (float)((double)this->_42 * (double)srcw), (float)((double)this->_43 * (double)srcw), (float)((double)this->_44 * (double)srcw));
}
ChaMatrix &ChaMatrix::operator*= (float srcw) { *this = *this * srcw; return *this; }
ChaMatrix ChaMatrix::operator/ (float srcw) const {
	if (srcw != 0.0f) {
		return ChaMatrix((float)((double)this->_11 / (double)srcw), (float)((double)this->_12 / (double)srcw), (float)((double)this->_13 / (double)srcw), (float)((double)this->_14 / (double)srcw),
			(float)((double)this->_21 / (double)srcw), (float)((double)this->_22 / (double)srcw), (float)((double)this->_23 / (double)srcw), (float)((double)this->_24 / (double)srcw),
			(float)((double)this->_31 / (double)srcw), (float)((double)this->_32 / (double)srcw), (float)((double)this->_33 / (double)srcw), (float)((double)this->_34 / (double)srcw),
			(float)((double)this->_41 / (double)srcw), (float)((double)this->_42 / (double)srcw), (float)((double)this->_43 / (double)srcw), (float)((double)this->_44 / (double)srcw));
	}
	else {
		ChaMatrix retmat;
		ChaMatrixIdentity(&retmat);
		return retmat;
	}
}
ChaMatrix &ChaMatrix::operator/= (float srcw) { *this = *this / srcw; return *this; }
ChaMatrix ChaMatrix::operator* (double srcw) const {
	return ChaMatrix((float)((double)this->_11 * srcw), (float)((double)this->_12 * srcw), (float)((double)this->_13 * srcw), (float)((double)this->_14 * srcw),
		(float)((double)this->_21 * srcw), (float)((double)this->_22 * srcw), (float)((double)this->_23 * srcw), (float)((double)this->_24 * srcw),
		(float)((double)this->_31 * srcw), (float)((double)this->_32 * srcw), (float)((double)this->_33 * srcw), (float)((double)this->_34 * srcw),
		(float)((double)this->_41 * srcw), (float)((double)this->_42 * srcw), (float)((double)this->_43 * srcw), (float)((double)this->_44 * srcw));
}
ChaMatrix& ChaMatrix::operator*= (double srcw) { *this = *this * srcw; return *this; }
ChaMatrix ChaMatrix::operator/ (double srcw) const {
	if (srcw != 0.0) {
		return ChaMatrix((float)((double)this->_11 / srcw), (float)((double)this->_12 / srcw), (float)((double)this->_13 / srcw), (float)((double)this->_14 / srcw),
			(float)((double)this->_21 / srcw), (float)((double)this->_22 / srcw), (float)((double)this->_23 / srcw), (float)((double)this->_24 / srcw),
			(float)((double)this->_31 / srcw), (float)((double)this->_32 / srcw), (float)((double)this->_33 / srcw), (float)((double)this->_34 / srcw),
			(float)((double)this->_41 / srcw), (float)((double)this->_42 / srcw), (float)((double)this->_43 / srcw), (float)((double)this->_44 / srcw));
	}
	else {
		ChaMatrix retmat;
		ChaMatrixIdentity(&retmat);
		return retmat;
	}
}
ChaMatrix& ChaMatrix::operator/= (double srcw) { *this = *this / srcw; return *this; }



ChaMatrix ChaMatrix::operator+ (const ChaMatrix &m) const { 
	return ChaMatrix(
		_11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
		_21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
		_31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
		_41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44
		);
}
ChaMatrix &ChaMatrix::operator+= (const ChaMatrix &m) { *this = *this + m; return *this; }

ChaMatrix ChaMatrix::operator- (const ChaMatrix &m) const { 
	return ChaMatrix(
	_11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
	_21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
	_31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
	_41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44
	); }
ChaMatrix &ChaMatrix::operator-= (const ChaMatrix &m) { *this = *this - m; return *this; }


ChaMatrix ChaMatrix::operator* (const ChaMatrix &m) const {
	//*this * m
	double m_11 = (double)m._11;
	double m_12 = (double)m._12;
	double m_13 = (double)m._13;
	double m_14 = (double)m._14;
	double m_21 = (double)m._21;
	double m_22 = (double)m._22;
	double m_23 = (double)m._23;
	double m_24 = (double)m._24;
	double m_31 = (double)m._31;
	double m_32 = (double)m._32;
	double m_33 = (double)m._33;
	double m_34 = (double)m._34;
	double m_41 = (double)m._41;
	double m_42 = (double)m._42;
	double m_43 = (double)m._43;
	double m_44 = (double)m._44;

	ChaMatrix res;
	res._11 = (float)(m_11 * (double)_11 + m_21 * (double)_12 + m_31 * (double)_13 + m_41 * (double)_14);
	res._21 = (float)(m_11 * (double)_21 + m_21 * (double)_22 + m_31 * (double)_23 + m_41 * (double)_24);
	res._31 = (float)(m_11 * (double)_31 + m_21 * (double)_32 + m_31 * (double)_33 + m_41 * (double)_34);
	res._41 = (float)(m_11 * (double)_41 + m_21 * (double)_42 + m_31 * (double)_43 + m_41 * (double)_44);

	res._12 = (float)(m_12 * (double)_11 + m_22 * (double)_12 + m_32 * (double)_13 + m_42 * (double)_14);
	res._22 = (float)(m_12 * (double)_21 + m_22 * (double)_22 + m_32 * (double)_23 + m_42 * (double)_24);
	res._32 = (float)(m_12 * (double)_31 + m_22 * (double)_32 + m_32 * (double)_33 + m_42 * (double)_34);
	res._42 = (float)(m_12 * (double)_41 + m_22 * (double)_42 + m_32 * (double)_43 + m_42 * (double)_44);

	res._13 = (float)(m_13 * (double)_11 + m_23 * (double)_12 + m_33 * (double)_13 + m_43 * (double)_14);
	res._23 = (float)(m_13 * (double)_21 + m_23 * (double)_22 + m_33 * (double)_23 + m_43 * (double)_24);
	res._33 = (float)(m_13 * (double)_31 + m_23 * (double)_32 + m_33 * (double)_33 + m_43 * (double)_34);
	res._43 = (float)(m_13 * (double)_41 + m_23 * (double)_42 + m_33 * (double)_43 + m_43 * (double)_44);

	res._14 = (float)(m_14 * (double)_11 + m_24 * (double)_12 + m_34 * (double)_13 + m_44 * (double)_14);
	res._24 = (float)(m_14 * (double)_21 + m_24 * (double)_22 + m_34 * (double)_23 + m_44 * (double)_24);
	res._34 = (float)(m_14 * (double)_31 + m_24 * (double)_32 + m_34 * (double)_33 + m_44 * (double)_34);
	res._44 = (float)(m_14 * (double)_41 + m_24 * (double)_42 + m_34 * (double)_43 + m_44 * (double)_44);

	//ChaMatrix res;
	//res._11 = m._11 * _11 + m._21 * _12 + m._31 * _13 + m._41 * _14;
	//res._21 = m._11 * _21 + m._21 * _22 + m._31 * _23 + m._41 * _24;
	//res._31 = m._11 * _31 + m._21 * _32 + m._31 * _33 + m._41 * _34;
	//res._41 = m._11 * _41 + m._21 * _42 + m._31 * _43 + m._41 * _44;

	//res._12 = m._12 * _11 + m._22 * _12 + m._32 * _13 + m._42 * _14;
	//res._22 = m._12 * _21 + m._22 * _22 + m._32 * _23 + m._42 * _24;
	//res._32 = m._12 * _31 + m._22 * _32 + m._32 * _33 + m._42 * _34;
	//res._42 = m._12 * _41 + m._22 * _42 + m._32 * _43 + m._42 * _44;

	//res._13 = m._13 * _11 + m._23 * _12 + m._33 * _13 + m._43 * _14;
	//res._23 = m._13 * _21 + m._23 * _22 + m._33 * _23 + m._43 * _24;
	//res._33 = m._13 * _31 + m._23 * _32 + m._33 * _33 + m._43 * _34;
	//res._43 = m._13 * _41 + m._23 * _42 + m._33 * _43 + m._43 * _44;

	//res._14 = m._14 * _11 + m._24 * _12 + m._34 * _13 + m._44 * _14;
	//res._24 = m._14 * _21 + m._24 * _22 + m._34 * _23 + m._44 * _24;
	//res._34 = m._14 * _31 + m._24 * _32 + m._34 * _33 + m._44 * _34;
	//res._44 = m._14 * _41 + m._24 * _42 + m._34 * _43 + m._44 * _44;

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
#ifdef CONVD3DX11
int CQuaternion::SetParams(DirectX::XMFLOAT4 srcxq)
{
	w = srcxq.w;
	x = srcxq.x;
	y = srcxq.y;
	z = srcxq.z;
	return 0;
}
#endif

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
	else if (srcphai < 0.0f) {
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
	double phai2;
	double cos_phai2, sin_phai2;

	phai2 = QuaternionLimitPhai(phai) * 0.5;
	cos_phai2 = cos(phai2);
	sin_phai2 = sin(phai2);

	w = (float)cos_phai2;
	x = (float)((double)srcaxis.x * sin_phai2);
	y = (float)((double)srcaxis.y * sin_phai2);
	z = (float)((double)srcaxis.z * sin_phai2);

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
		return CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}
}


ChaMatrix CQuaternion::MakeRotMatX()
{
	normalize();

	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	//転置
	double _11 = 1.0 - 2.0 * ((double)y * (double)y + (double)z * (double)z);
	double _21 = 2.0 * ((double)x * (double)y - (double)z * (double)w);
	double _31 = 2.0 * ((double)z * (double)x + (double)w * (double)y);

	double _12 = 2.0 * ((double)x * (double)y + (double)z * (double)w);
	double _22 = 1.0 - 2.0 * ((double)z * (double)z + (double)x * (double)x);
	double _32 = 2.0 * ((double)y * (double)z - (double)w * (double)x);

	double _13 = 2.0 * ((double)z * (double)x - (double)w * (double)y);
	double _23 = 2.0 * ((double)y * (double)z + (double)x * (double)w);
	double _33 = 1.0 - 2.0 * ((double)y * (double)y + (double)x * (double)x);

	retmat._11 = (float)_11;
	retmat._21 = (float)_21;
	retmat._31 = (float)_31;

	retmat._12 = (float)_12;
	retmat._22 = (float)_22;
	retmat._32 = (float)_32;

	retmat._13 = (float)_13;
	retmat._23 = (float)_23;
	retmat._33 = (float)_33;


	/*
	retmat._11 = 1.0 - 2.0 * (y * y + z * z);
	retmat._12 = 2.0 * (x * y - z * w);
	retmat._13 = 2.0 * (z * x + w * y);

	retmat._21 = 2.0 * (x * y + z * w);
	retmat._22 = 1.0 - 2.0 * (z * z + x * x);
	retmat._23 = 2.0 * (y * z - w * x);

	retmat._31 = 2.0 * (z * x - w * y);
	retmat._32 = 2.0 * (y * z + x * w);
	retmat._33 = 1.0 - 2.0 * (y * y + x * x);
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

	m[0][0] = rmat._11;
	m[0][1] = rmat._12;
	m[0][2] = rmat._13;
	m[0][3] = rmat._14;
	m[1][0] = rmat._21;
	m[1][1] = rmat._22;
	m[1][2] = rmat._23;
	m[1][3] = rmat._24;
	m[2][0] = rmat._31;
	m[2][1] = rmat._32;
	m[2][2] = rmat._33;
	m[2][3] = rmat._34;
	m[3][0] = rmat._41;
	m[3][1] = rmat._42;
	m[3][2] = rmat._43;
	m[3][3] = rmat._44;

	int i, maxi;
	FLOAT maxdiag;
	double S, trace;

	trace = (double)m[0][0] + (double)m[1][1] + (double)m[2][2] + 1.0;
	if (trace > 0.0f)
	{
		tmpq.x = (float)(((double)m[1][2] - (double)m[2][1]) / (2.0 * sqrt(trace)));
		tmpq.y = (float)(((double)m[2][0] - (double)m[0][2]) / (2.0 * sqrt(trace)));
		tmpq.z = (float)(((double)m[0][1] - (double)m[1][0]) / (2.0 * sqrt(trace)));
		tmpq.w = (float)(sqrt(trace) / 2.0);
		*this = tmpq;
		return;
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
			tmpq = CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
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
			tmpq = CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
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
			tmpq = CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
		}
		break;
	}
	*this = tmpq;


	//ChaMatrix tmpmat;
	//tmpmat = srcmat;
	//tmpmat._41 = 0.0f;
	//tmpmat._42 = 0.0f;
	//tmpmat._43 = 0.0f;

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
	retrad = acos(dot);

	return retrad;
}

int CQuaternion::Slerp2(CQuaternion endq, double t, CQuaternion* dstq)
{
	dstq->SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	if (t == 1.0) {
		*dstq = endq;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	else if (t == 0.0) {
		*dstq = *this;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	double kaku;
	kaku = this->CalcRad(endq);

	if (kaku > (PI * 0.5)) {
		//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
		endq = -endq;
		kaku = this->CalcRad(endq);
		_ASSERT(kaku <= (PI * 0.5));
	}

	// sin( kaku ) == 0.0 付近を調整。
	//180度にはならないので（ならないようにするので）０度のみケア
	int kaku0flag = 0;
	if ((kaku <= 1e-4) && (kaku >= -1e-4)) {
		kaku0flag = 1;
	}

	CQuaternion tmpq;
	tmpq.SetParams(w, x, y, z);

	double alpha, beta;
	if (kaku0flag == 0) {
		alpha = sin(kaku * (1.0 - t)) / sin(kaku);
		beta = sin(kaku * t) / sin(kaku);

		dstq->x = (float)((double)tmpq.x * alpha + (double)endq.x * beta);
		dstq->y = (float)((double)tmpq.y * alpha + (double)endq.y * beta);
		dstq->z = (float)((double)tmpq.z * alpha + (double)endq.z * beta);
		dstq->w = (float)((double)tmpq.w * alpha + (double)endq.w * beta);
		//		retq = tmpq * alpha + endq * beta;

	}
	else {
		*dstq = tmpq;
	}
	return 0;
}

CQuaternion CQuaternion::Slerp(CQuaternion endq, int framenum, int frameno)
{
	CQuaternion retq;
	retq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	double kaku;
	kaku = this->CalcRad(endq);

	if (kaku > (PI * 0.5)) {
		//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
		endq = -endq;
		kaku = this->CalcRad(endq);
		_ASSERT(kaku <= (PI * 0.5));
	}

	// sin( kaku ) == 0.0 付近を調整。
	//180度にはならないので（ならないようにするので）０度のみケア
	int kaku0flag = 0;
	if ((kaku <= 1e-4) && (kaku >= -1e-4)) {
		kaku0flag = 1;
		//DbgOut( "Quaternion : Slerp : kaku0flag 1 : dot %f, kaku %f\n", dot, kaku );
	}


	double t = (double)frameno / (double)framenum;
	double alpha, beta;
	if (kaku0flag == 0) {
		alpha = sin(kaku * (1.0 - t)) / sin(kaku);
		beta = sin(kaku * t) / sin(kaku);

		retq = *this * alpha + endq * beta;
	}
	else {
		retq = *this;
		//DbgOut( "MotionInfo : FillUpMotionPoint 2: frame %d, %f, %f, %f, %f\n",
		//	   frameno, startq.w, startq.x, startq.y, startq.z );
	}

	return retq;
}

#ifdef CONVD3DX11
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
#endif

int CQuaternion::inv(CQuaternion* dstq)
{

	dstq->w = w;
	dstq->x = -x;
	dstq->y = -y;
	dstq->z = -z;

	*dstq = dstq->normalize();

	return 0;
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
	ChaMatrix mat;

	mat = MakeRotMatX();

	ChaVector3TransformCoord(dstvec, &srcvec, &mat);

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
	tmpx = (double)dirm._11 * (double)tmpsrcvec.x + (double)dirm._21 * (double)tmpsrcvec.y + (double)dirm._31 * (double)tmpsrcvec.z + (double)dirm._41;
	tmpy = (double)dirm._12 * (double)tmpsrcvec.x + (double)dirm._22 * (double)tmpsrcvec.y + (double)dirm._32 * (double)tmpsrcvec.z + (double)dirm._42;
	tmpz = (double)dirm._13 * (double)tmpsrcvec.x + (double)dirm._23 * (double)tmpsrcvec.y + (double)dirm._33 * (double)tmpsrcvec.z + (double)dirm._43;

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

	tmpx = (double)dirm._11 * (double)tmpsrcvec.x + (double)dirm._21 * (double)tmpsrcvec.y + (double)dirm._31 * (double)tmpsrcvec.z + (double)dirm._41;
	tmpy = (double)dirm._12 * (double)tmpsrcvec.x + (double)dirm._22 * (double)tmpsrcvec.y + (double)dirm._32 * (double)tmpsrcvec.z + (double)dirm._42;
	tmpz = (double)dirm._13 * (double)tmpsrcvec.x + (double)dirm._23 * (double)tmpsrcvec.y + (double)dirm._33 * (double)tmpsrcvec.z + (double)dirm._43;

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

	tmpx = (double)dirm._11 * (double)tmpsrcvec.x + (double)dirm._21 * (double)tmpsrcvec.y + (double)dirm._31 * (double)tmpsrcvec.z + (double)dirm._41;
	tmpy = (double)dirm._12 * (double)tmpsrcvec.x + (double)dirm._22 * (double)tmpsrcvec.y + (double)dirm._32 * (double)tmpsrcvec.z + (double)dirm._42;
	tmpz = (double)dirm._13 * (double)tmpsrcvec.x + (double)dirm._23 * (double)tmpsrcvec.y + (double)dirm._33 * (double)tmpsrcvec.z + (double)dirm._43;

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


int CQuaternion::Q2EulXYZ(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int isfirstbone, int isendbone, int notmodifyflag)
{

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

	ChaVector3 Euler;


	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);

	ChaVector3 targetVec, shadowVec;
	ChaVector3 tmpVec;

	EQ.Rotate(&targetVec, axisXVec);
	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
	shadowVec.y = (float)vecDotVec(&targetVec, &axisYVec);
	shadowVec.z = 0.0f;
	if (lengthVec(&shadowVec) == 0.0f) {
		Euler.z = 90.0f;
	}
	else {
		Euler.z = (float)aCos(vecDotVec(&shadowVec, &axisXVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisYVec) < 0.0) {
		Euler.z = -Euler.z;
	}
	//if (vecDotVec(&shadowVec, &axisYVec) > 0.0f) {
	//	Euler.z = -Euler.z;
	//}

	vec3RotateZ(&tmpVec, -Euler.z, &targetVec);
	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
	shadowVec.y = 0.0f;
	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
	if (lengthVec(&shadowVec) == 0.0f) {
		Euler.y = 90.0f;
	}
	else {
		Euler.y = (float)aCos(vecDotVec(&shadowVec, &axisXVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisZVec) > 0.0) {
		Euler.y = -Euler.y;
	}
	//if (vecDotVec(&shadowVec, &axisZVec) < 0.0f) {
	//	Euler.y = -Euler.y;
	//}


	EQ.Rotate(&targetVec, axisZVec);
	vec3RotateZ(&tmpVec, -Euler.z, &targetVec);
	targetVec = tmpVec;
	vec3RotateY(&tmpVec, -Euler.y, &targetVec);
	shadowVec.x = 0.0f;
	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
	if (lengthVec(&shadowVec) == 0.0) {
		Euler.x = 90.0f;
	}
	else {
		Euler.x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
	}
	if (vecDotVec(&shadowVec, &axisYVec) > 0.0) {
		Euler.x = -Euler.x;
	}
	//if (vecDotVec(&shadowVec, &axisYVec) < 0.0f) {
	//	Euler.x = -Euler.x;
	//}

	//ModifyEuler(&Euler, &befeul);
	ModifyEulerXYZ(&Euler, &befeul, isfirstbone, isendbone, notmodifyflag);
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

	////オイラー角Aの値をオイラー角Bの値に近い表示に修正
	//double tmpX1, tmpY1, tmpZ1;
	//double tmpX2, tmpY2, tmpZ2;
	//double s1, s2;
	//tmpX1 = eulerA->x;
	//tmpY1 = eulerA->y;
	//tmpZ1 = eulerA->z;
	////予想される角度1
	//tmpX2 = eulerA->x + 360.0 * GetRound((eulerB->x - eulerA->x) / 360.0);
	//tmpY2 = eulerA->y + 360.0 * GetRound((eulerB->y - eulerA->y) / 360.0);
	//tmpZ2 = eulerA->z + 360.0 * GetRound((eulerB->z - eulerA->z) / 360.0);
	////角度変化の大きさ
	//s1 = (eulerB->x - tmpX1) * (eulerB->x - tmpX1) + (eulerB->y - tmpY1) * (eulerB->y - tmpY1) + (eulerB->z - tmpZ1) * (eulerB->z - tmpZ1);
	//s2 = (eulerB->x - tmpX2) * (eulerB->x - tmpX2) + (eulerB->y - tmpY2) * (eulerB->y - tmpY2) + (eulerB->z - tmpZ2) * (eulerB->z - tmpZ2);
	//if (s1 <= s2) {
	//	eulerA->x = (float)tmpX1; eulerA->y = (float)tmpY1; eulerA->z = (float)tmpZ1;
	//}
	//else {
	//	eulerA->x = (float)tmpX2; eulerA->y = (float)tmpY2; eulerA->z = (float)tmpZ2;
	//}


		//オイラー角Aの値をオイラー角Bの値に近い表示に修正
	double tmpX0, tmpY0, tmpZ0;
	double tmpX1, tmpY1, tmpZ1;
	double tmpX2, tmpY2, tmpZ2;
	double tmpX3, tmpY3, tmpZ3;
	double tmpX4, tmpY4, tmpZ4;
	double tmpX5, tmpY5, tmpZ5;
	double tmpX6, tmpY6, tmpZ6;
	double s0, s1, s2 , s3, s4;
	//double mins;


	tmpX0 = eulerA->x;
	tmpY0 = eulerA->y;
	tmpZ0 = eulerA->z;

	//予想される角度1
	tmpX1 = (double)eulerA->x + 360.0 * GetRound((float)(((double)eulerB->x - (double)eulerA->x) / 360.0));
	tmpY1 = (double)eulerA->y + 360.0 * GetRound((float)(((double)eulerB->y - (double)eulerA->y) / 360.0));
	tmpZ1 = (double)eulerA->z + 360.0 * GetRound((float)(((double)eulerB->z - (double)eulerA->z) / 360.0));

	//予想される角度2
	//クォータニオンは１８０°で一回転する。
	//横軸が２シータ、縦軸がsin2シータ、cos2シータのグラフにおいて、newシータ　=　180 + oldシータの値は等しい。
	//tmp2の角度はクォータニオンにおいて等しい姿勢を取るオイラー角である。
	
	//この場合、３つの軸のうち２つだけの軸の角度の符号(ここではY軸)が反転する。
	//！！！！　１つではなく２つの軸が反転する。　！！！！
	
	//X, Y
	if (abs(eulerB->x - (tmpX0 - 180.0)) <= (abs(eulerB->x - (tmpX0 + 180.0)))) {
		tmpX2 = tmpX0 - 180.0;
	}
	else {
		tmpX2 = tmpX0 + 180.0;
	}
	if (abs(eulerB->y - (tmpY0 - 180.0)) <= (abs(eulerB->y - (tmpY0 + 180.0)))) {
		tmpY2 = tmpY0 - 180.0;
	}
	else {
		tmpY2 = tmpY0 + 180.0;
	}
	tmpZ2 = tmpZ0;


	//Y, Z
	tmpX3 = tmpX0;
	if (abs(eulerB->y - (tmpY0 - 180.0)) <= (abs(eulerB->y - (tmpY0 + 180.0)))) {
		tmpY3 = tmpY0 - 180.0;
	}
	else {
		tmpY3 = tmpY0 + 180.0;
	}
	if (abs(eulerB->z - (tmpZ0 - 180.0)) <= (abs(eulerB->z - (tmpZ0 + 180.0)))) {
		tmpZ3 = tmpZ0 - 180.0;
	}
	else {
		tmpZ3 = tmpZ0 + 180.0;
	}


	//Z, X
	if (abs(eulerB->x - (tmpX0 - 180.0)) <= (abs(eulerB->x - (tmpX0 + 180.0)))) {
		tmpX4 = tmpX0 - 180.0;
	}
	else {
		tmpX4 = tmpX0 + 180.0;
	}
	tmpY4 = tmpY0;
	if (abs(eulerB->z - (tmpZ0 - 180.0)) <= (abs(eulerB->z - (tmpZ0 + 180.0)))) {
		tmpZ4 = tmpZ0 - 180.0;
	}
	else {
		tmpZ4 = tmpZ0 + 180.0;
	}



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
		bool operator<(const tag_chkeul& right) const {
			return s == right.s ? index < right.index : s < right.s;
		}
	}CHKEUL;

	std::vector<struct tag_chkeul> vecchkeul;
	CHKEUL tmpchkeul;
	tmpchkeul.s = s0;
	tmpchkeul.index = 0;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s1;
	tmpchkeul.index = 1;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s2;
	tmpchkeul.index = 2;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s3;
	tmpchkeul.index = 3;
	vecchkeul.push_back(tmpchkeul);
	tmpchkeul.s = s4;
	tmpchkeul.index = 4;
	vecchkeul.push_back(tmpchkeul);

	std::sort(vecchkeul.begin(), vecchkeul.end());
	
	CHKEUL mineul = vecchkeul[0];


	if ((notmodifyflag == 0) && (isfirstbone == 0) && (isendbone == 0) && (eulerB->x != 0.0f) && (eulerB->y != 0.0f) && (eulerB->z != 0.0f)) {
	//if((isfirstbone == 0) && (notmodifyflag == 0)){
	//if ((eulerB->x != 0.0f) || (eulerB->y != 0.0f) || (eulerB->z != 0.0f)) {
		switch (mineul.index)
		{
		case 0:
			eulerA->x = (float)tmpX0; eulerA->y = (float)tmpY0; eulerA->z = (float)tmpZ0;
			break;
		case 1:
			eulerA->x = (float)tmpX1; eulerA->y = (float)tmpY1; eulerA->z = (float)tmpZ1;
			break;
		case 2:
			eulerA->x = (float)tmpX2; eulerA->y = (float)tmpY2; eulerA->z = (float)tmpZ2;
			break;
		case 3:
			eulerA->x = (float)tmpX3; eulerA->y = (float)tmpY3; eulerA->z = (float)tmpZ3;
			break;
		case 4:
			eulerA->x = (float)tmpX4; eulerA->y = (float)tmpY4; eulerA->z = (float)tmpZ4;
			break;
		default:
			_ASSERT(0);
			eulerA->x = (float)tmpX0; eulerA->y = (float)tmpY0; eulerA->z = (float)tmpZ0;
			break;
		}
	}
	else {
		if ((notmodifyflag == 1) || (isendbone == 0)) {
			if (s0 <= s1) {
				eulerA->x = (float)tmpX0; eulerA->y = (float)tmpY0; eulerA->z = (float)tmpZ0;
			}
			else {
				eulerA->x = (float)tmpX1; eulerA->y = (float)tmpY1; eulerA->z = (float)tmpZ1;
			}
		}
		else {
			//a case that isendbone == 1

			//########################
			//意味は分からない、だが直る。
			//########################
			//モデル側endboneに於いてモーションキャプチャーデータのtoe(つま先)だけ１８０度異常に回転している場合がある。
			//１軸だけ１８０度回転することは座標系上あり得ないが、以下のようにすると間違ったオイラー角の修正が出来るようだ。

			//upvec間違い直し？

			float plusZ, befplusZ;
			befplusZ = eulerB->z;
			plusZ = tmpZ0;
			if (befplusZ < 0.0f) {
				befplusZ += 360.0f;
			}
			if (plusZ < 0.0f) {
				plusZ += 360.0f;
			}

			if (!((eulerB->x == 0.0f) && (eulerB->y == 0.0f) && (eulerB->z == 0.0f)) && (abs(befplusZ - plusZ) >= 89.0)) {

				//Zだけおかしい場合の修正.あり得ないけど有り得る
				float tmpX5, tmpY5, tmpZ5;
				float s5;
				tmpX5 = tmpX0;
				tmpY5 = tmpY0;
				tmpZ5 = tmpZ0 + 180.0f;
				if (tmpZ5 > 180.0f) {
					tmpZ5 -= 360.0f;
				}
				s5 = ((double)eulerB->x - tmpX5) * ((double)eulerB->x - tmpX5) + ((double)eulerB->y - tmpY5) * ((double)eulerB->y - tmpY5) + ((double)eulerB->z - tmpZ5) * ((double)eulerB->z - tmpZ5);
				if (s0 <= s5) {
					eulerA->x = (float)tmpX0; eulerA->y = (float)tmpY0; eulerA->z = (float)tmpZ0;
				}
				else {
					eulerA->x = (float)tmpX5; eulerA->y = (float)tmpY5; eulerA->z = (float)tmpZ5;
				}
			}

		}
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
	if (srcval > 0.0f) {
		return (int)(srcval + 0.5);
	}
	else {
		return (int)(srcval - 0.5);
	}
}
//
int CQuaternion::CalcFBXEulXYZ(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int isfirstbone, int isendbone, int notmodifyflag)
{

	int noise[4] = { 0, 1, 0, -1 };
	static int dbgcnt = 0;

	ChaVector3 tmpeul(0.0f, 0.0f, 0.0f);
	if (IsInit() == 0) {
		//Q2Eul(axisq, befeul, &tmpeul);
		Q2EulXYZ(axisq, befeul, &tmpeul, isfirstbone, isendbone, notmodifyflag);
		//Q2EulYXZ(axisq, befeul, &tmpeul);
		//Q2EulZXY(axisq, befeul, &tmpeul);
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
	CQuaternion retq;
	retq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	if (!eulmat) {
		return retq;
	}

	btVector3 tmpcol[3];
	int colno;
	for (colno = 0; colno < 3; colno++) {
		tmpcol[colno] = eulmat->getColumn(colno);
	}

	ChaMatrix xmat;
	ChaMatrixIdentity(&xmat);

	xmat._11 = tmpcol[0].x();
	xmat._12 = tmpcol[0].y();
	xmat._13 = tmpcol[0].z();

	xmat._21 = tmpcol[1].x();
	xmat._22 = tmpcol[1].y();
	xmat._23 = tmpcol[1].z();

	xmat._31 = tmpcol[2].x();
	xmat._32 = tmpcol[2].y();
	xmat._33 = tmpcol[2].z();

	retq.RotationMatrix(xmat);
	return retq;

}



void ChaMatrixIdentity(ChaMatrix* pdst)
{
	if (!pdst){
		return;
	}
	pdst->_11 = 1.0f;
	pdst->_12 = 0.0f;
	pdst->_13 = 0.0f;
	pdst->_14 = 0.0f;

	pdst->_21 = 0.0f;
	pdst->_22 = 1.0f;
	pdst->_23 = 0.0f;
	pdst->_24 = 0.0f;

	pdst->_31 = 0.0f;
	pdst->_32 = 0.0f;
	pdst->_33 = 1.0f;
	pdst->_34 = 0.0f;

	pdst->_41 = 0.0f;
	pdst->_42 = 0.0f;
	pdst->_43 = 0.0f;
	pdst->_44 = 1.0f;
}

void ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc)
{
	if (!pdst || !psrc){
		return;
	}

	ChaMatrix res;
	double detA;
	double a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44;
	double b11, b12, b13, b14, b21, b22, b23, b24, b31, b32, b33, b34, b41, b42, b43, b44;

	a11 = psrc->_11;
	a12 = psrc->_12;
	a13 = psrc->_13;
	a14 = psrc->_14;

	a21 = psrc->_21;
	a22 = psrc->_22;
	a23 = psrc->_23;
	a24 = psrc->_24;

	a31 = psrc->_31;
	a32 = psrc->_32;
	a33 = psrc->_33;
	a34 = psrc->_34;

	a41 = psrc->_41;
	a42 = psrc->_42;
	a43 = psrc->_43;
	a44 = psrc->_44;

	detA = (a11 * a22 * a33 * a44) + (a11 * a23 * a34 * a42) + (a11 * a24 * a32 * a43)
		+ (a12 * a21 * a34 * a43) + (a12 * a23 * a31 * a44) + (a12 * a24 * a33 * a41)
		+ (a13 * a21 * a32 * a44) + (a13 * a22 * a34 * a41) + (a13 * a24 * a31 * a42)
		+ (a14 * a21 * a33 * a42) + (a14 * a22 * a31 * a43) + (a14 * a23 * a32 * a41)
		- (a11 * a22 * a34 * a43) - (a11 * a23 * a32 * a44) - (a11 * a24 * a33 * a42)
		- (a12 * a21 * a33 * a44) - (a12 * a23 * a34 * a41) - (a12 * a24 * a31 * a43)
		- (a13 * a21 * a34 * a42) - (a13 * a22 * a31 * a44) - (a13 * a24 * a32 * a41)
		- (a14 * a21 * a32 * a43) - (a14 * a22 * a33 * a41) - (a14 * a23 * a31 * a42);

	if (pdet){
		*pdet = (float)detA;
	}


	if (detA == 0.0){
		*pdst = *psrc;
		return;//!!!!!!!!!!!!!!!!!!!!!!!!!!
	}




	b11 = (a22 * a33 * a44) + (a23 * a34 * a42) + (a24 * a32 * a43) - (a22 * a34 * a43) - (a23 * a32 * a44) - (a24 * a33 * a42);
	b12 = (a12 * a34 * a43) + (a13 * a32 * a44) + (a14 * a33 * a42) - (a12 * a33 * a44) - (a13 * a34 * a42) - (a14 * a32 * a43);
	b13 = (a12 * a23 * a44) + (a13 * a24 * a42) + (a14 * a22 * a43) - (a12 * a24 * a43) - (a13 * a22 * a44) - (a14 * a23 * a42);
	b14 = (a12 * a24 * a33) + (a13 * a22 * a34) + (a14 * a23 * a32) - (a12 * a23 * a34) - (a13 * a24 * a32) - (a14 * a22 * a33);

	b21 = (a21 * a34 * a43) + (a23 * a31 * a44) + (a24 * a33 * a41) - (a21 * a33 * a44) - (a23 * a34 * a41) - (a24 * a31 * a43);
	b22 = (a11 * a33 * a44) + (a13 * a34 * a41) + (a14 * a31 * a43) - (a11 * a34 * a43) - (a13 * a31 * a44) - (a14 * a33 * a41);
	b23 = (a11 * a24 * a43) + (a13 * a21 * a44) + (a14 * a23 * a41) - (a11 * a23 * a44) - (a13 * a24 * a41) - (a14 * a21 * a43);
	b24 = (a11 * a23 * a34) + (a13 * a24 * a31) + (a14 * a21 * a33) - (a11 * a24 * a33) - (a13 * a21 * a34) - (a14 * a23 * a31);

	b31 = (a21 * a32 * a44) + (a22 * a34 * a41) + (a24 * a31 * a42) - (a21 * a34 * a42) - (a22 * a31 * a44) - (a24 * a32 * a41);
	b32 = (a11 * a34 * a42) + (a12 * a31 * a44) + (a14 * a32 * a41) - (a11 * a32 * a44) - (a12 * a34 * a41) - (a14 * a31 * a42);
	b33 = (a11 * a22 * a44) + (a12 * a24 * a41) + (a14 * a21 * a42) - (a11 * a24 * a42) - (a12 * a21 * a44) - (a14 * a22 * a41);
	b34 = (a11 * a24 * a32) + (a12 * a21 * a34) + (a14 * a22 * a31) - (a11 * a22 * a34) - (a12 * a24 * a31) - (a14 * a21 * a32);

	b41 = (a21 * a33 * a42) + (a22 * a31 * a43) + (a23 * a32 * a41) - (a21 * a32 * a43) - (a22 * a33 * a41) - (a23 * a31 * a42);
	b42 = (a11 * a32 * a43) + (a12 * a33 * a41) + (a13 * a31 * a42) - (a11 * a33 * a42) - (a12 * a31 * a43) - (a13 * a32 * a41);
	b43 = (a11 * a23 * a42) + (a12 * a21 * a43) + (a13 * a22 * a41) - (a11 * a22 * a43) - (a12 * a23 * a41) - (a13 * a21 * a42);
	b44 = (a11 * a22 * a33) + (a12 * a23 * a31) + (a13 * a21 * a32) - (a11 * a23 * a32) - (a12 * a21 * a33) - (a13 * a22 * a31);

	res._11 = (float)(b11 / detA);
	res._12 = (float)(b12 / detA);
	res._13 = (float)(b13 / detA);
	res._14 = (float)(b14 / detA);

	res._21 = (float)(b21 / detA);
	res._22 = (float)(b22 / detA);
	res._23 = (float)(b23 / detA);
	res._24 = (float)(b24 / detA);

	res._31 = (float)(b31 / detA);
	res._32 = (float)(b32 / detA);
	res._33 = (float)(b33 / detA);
	res._34 = (float)(b34 / detA);

	res._41 = (float)(b41 / detA);
	res._42 = (float)(b42 / detA);
	res._43 = (float)(b43 / detA);
	res._44 = (float)(b44 / detA);

	*pdst = res;
}

void ChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz)
{
	if (!pdst){
		return;
	}

	pdst->_41 = srcx;
	pdst->_42 = srcy;
	pdst->_43 = srcz;
}

void ChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc)
{
	float m[4][4];

	m[0][0] = psrc->_11;
	m[0][1] = psrc->_12;
	m[0][2] = psrc->_13;
	m[0][3] = psrc->_14;
	m[1][0] = psrc->_21;
	m[1][1] = psrc->_22;
	m[1][2] = psrc->_23;
	m[1][3] = psrc->_24;
	m[2][0] = psrc->_31;
	m[2][1] = psrc->_32;
	m[2][2] = psrc->_33;
	m[2][3] = psrc->_34;
	m[3][0] = psrc->_41;
	m[3][1] = psrc->_42;
	m[3][2] = psrc->_43;
	m[3][3] = psrc->_44;

	pdst->_11 = m[0][0];
	pdst->_12 = m[1][0];
	pdst->_13 = m[2][0];
	pdst->_14 = m[3][0];

	pdst->_21 = m[0][1];
	pdst->_22 = m[1][1];
	pdst->_23 = m[2][1];
	pdst->_24 = m[3][1];

	pdst->_31 = m[0][2];
	pdst->_32 = m[1][2];
	pdst->_33 = m[2][2];
	pdst->_34 = m[3][2];

	pdst->_41 = m[0][3];
	pdst->_42 = m[1][3];
	pdst->_43 = m[2][3];
	pdst->_44 = m[3][3];

}

double ChaVector3LengthDbl(ChaVector3* v)
{
	if (!v) {
		return 0.0f;
	}

	double leng;
	double mag;
	mag = (double)v->x * (double)v->x + (double)v->y * (double)v->y + (double)v->z * (double)v->z;
	if (mag != 0.0) {
		leng = sqrt(mag);
	}
	else {
		leng = 0.0;
	}
	return leng;

}

double ChaVector3DotDbl(const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	if (!psrc1 || !psrc2) {
		return 0.0f;
	}

	double retval = (double)psrc1->x * (double)psrc2->x + (double)psrc1->y * (double)psrc2->y + (double)psrc1->z * (double)psrc2->z;

	return retval;
}


//double ChaVector3LengthDbl(ChaVector3* v)
//{
//	if (!v){
//		return 0.0f;
//	}
//
//	double leng;
//	double mag;
//	mag = (double)v->x * (double)v->x + (double)v->y * (double)v->y + (double)v->z * (double)v->z;
//	if (mag != 0.0f){
//		leng = (float)sqrt(mag);
//	}
//	else{
//		leng = 0.0;
//	}
//	return leng;
//
//}

void ChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc){
	if (!pdst || !psrc){
		return;
	}

	ChaVector3 src = *psrc;

	double mag = (double)src.x * (double)src.x + (double)src.y * (double)src.y + (double)src.z * (double)src.z;
	if (mag != 0.0){
		double divval = ::sqrt(mag);
		if (divval != 0.0) {
			double tmpx = src.x / divval;
			double tmpy = src.y / divval;
			double tmpz = src.z / divval;
			pdst->x = (float)tmpx;
			pdst->y = (float)tmpy;
			pdst->z = (float)tmpz;
		}
		else {
			pdst->x = src.x;
			pdst->y = src.y;
			pdst->z = src.z;
		}
	}
	else{
		//*pdst = ChaVector3(0.0f, 0.0f, 0.0f);
		pdst->x = src.x;
		pdst->y = src.y;
		pdst->z = src.z;
	}
}

float ChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	if (!psrc1 || !psrc2){
		return 0.0f;
	}

	return (psrc1->x * psrc2->x + psrc1->y * psrc2->y + psrc1->z * psrc2->z);

}

void ChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	if (!pdst || !psrc1 || !psrc2){
		return;
	}

	//ChaVector3 v1, v2;
	//v1 = *psrc1;
	//v2 = *psrc2;

	//pdst->x = v1.y * v2.z - v1.z * v2.y;
	//pdst->y = v1.z * v2.x - v1.x * v2.z;
	//pdst->z = v1.x * v2.y - v1.y * v2.x;
	pdst->x = (float)((double)psrc1->y * (double)psrc2->z - (double)psrc1->z * (double)psrc2->y);
	pdst->y = (float)((double)psrc1->z * (double)psrc2->x - (double)psrc1->x * (double)psrc2->z);
	pdst->z = (float)((double)psrc1->x * (double)psrc2->y - (double)psrc1->y * (double)psrc2->x);
}


void ChaVector3TransformCoord(ChaVector3* dstvec, ChaVector3* srcvec, ChaMatrix* srcmat)
{
	if (!dstvec || !srcvec || !srcmat){
		return;
	}

	double tmpx, tmpy, tmpz, tmpw;
	tmpx = (double)srcmat->_11 * (double)srcvec->x + (double)srcmat->_21 * (double)srcvec->y + (double)srcmat->_31 * (double)srcvec->z + (double)srcmat->_41;
	tmpy = (double)srcmat->_12 * (double)srcvec->x + (double)srcmat->_22 * (double)srcvec->y + (double)srcmat->_32 * (double)srcvec->z + (double)srcmat->_42;
	tmpz = (double)srcmat->_13 * (double)srcvec->x + (double)srcmat->_23 * (double)srcvec->y + (double)srcmat->_33 * (double)srcvec->z + (double)srcmat->_43;
	tmpw = (double)srcmat->_14 * (double)srcvec->x + (double)srcmat->_24 * (double)srcvec->y + (double)srcmat->_34 * (double)srcvec->z + (double)srcmat->_44;

	if (tmpw != 0.0){
		dstvec->x = (float)(tmpx / tmpw);
		dstvec->y = (float)(tmpy / tmpw);
		dstvec->z = (float)(tmpz / tmpw);
	}
	else{
		//dstvec->x = 0.0f;
		//dstvec->y = 0.0f;
		//dstvec->z = 0.0f;
		*dstvec = *srcvec;
	}
}

/*
D3DXVECTOR3 *WINAPI D3DXVec3TransformNormal(D3DXVECTOR3 *pOut,
CONST D3DXVECTOR3 *pV,
CONST D3DXMATRIX *pM
);
パラメータ

pOut
[in, out] 演算結果である D3DXVECTOR3 構造体へのポインタ。
pV
[in] 処理の基になる D3DXVECTOR3 構造体へのポインタ。
pM
[in] 処理の基になる D3DXMATRIX 構造体へのポインタ。
戻り値

トランスフォームされたベクトルの D3DXVECTOR3 構造体へのポインタ。

注意

この関数は、ベクトル pV のベクトル法線(x, y, z, 0) を行列 pM でトランスフォームする。
*/
ChaVector3* ChaVector3TransformNormal(ChaVector3 *dstvec, const ChaVector3* srcvec, const ChaMatrix* srcmat)
{
	if (!dstvec || !srcvec || !srcmat) {
		return NULL;
	}

	double tmpx, tmpy, tmpz, tmpw;
	tmpx = (double)srcmat->_11 * (double)srcvec->x + (double)srcmat->_21 * (double)srcvec->y + (double)srcmat->_31 * (double)srcvec->z;// +(double)srcmat->_41;
	tmpy = (double)srcmat->_12 * (double)srcvec->x + (double)srcmat->_22 * (double)srcvec->y + (double)srcmat->_32 * (double)srcvec->z;// + (double)srcmat->_42;
	tmpz = (double)srcmat->_13 * (double)srcvec->x + (double)srcmat->_23 * (double)srcvec->y + (double)srcmat->_33 * (double)srcvec->z;// + (double)srcmat->_43;
	tmpw = (double)srcmat->_14 * (double)srcvec->x + (double)srcmat->_24 * (double)srcvec->y + (double)srcmat->_34 * (double)srcvec->z;// + (double)srcmat->_44;

	if (tmpw != 0.0) {
		dstvec->x = (float)(tmpx / tmpw);
		dstvec->y = (float)(tmpy / tmpw);
		dstvec->z = (float)(tmpz / tmpw);
	}
	else {
		//dstvec->x = 0.0f;
		//dstvec->y = 0.0f;
		//dstvec->z = 0.0f;
		*dstvec = *srcvec;
	}

	return dstvec;
}



double ChaVector3LengthSq(ChaVector3* psrc)
{
	if (!psrc) {
		return 0.0f;
	}

	double mag = (double)psrc->x * (double)psrc->x + (double)psrc->y * (double)psrc->y + (double)psrc->z * (double)psrc->z;
	return mag;

}







void ChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad)
{
	if (!pdst){
		return;
	}
	CQuaternion q;
	q.SetAxisAndRot(*srcaxis, srcrad);

	*pdst = q.MakeRotMatX();

}

void ChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz)
{
	if (!pdst){
		return;
	}

	pdst->_11 = srcx;
	pdst->_22 = srcy;
	pdst->_33 = srcz;
}

void ChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec)
{
/*
zaxis = normal(Eye - At)
xaxis = normal(cross(Up, zaxis))
yaxis = cross(zaxis, xaxis)

xaxis.x           yaxis.x           zaxis.x          0
xaxis.y           yaxis.y           zaxis.y          0
xaxis.z           yaxis.z           zaxis.z          0
-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
*/
	ChaVector3 zaxis;
	ChaVector3 yaxis;
	ChaVector3 xaxis;

	zaxis = *camEye - *camtar;
	ChaVector3Normalize(&zaxis, &zaxis);

	ChaVector3Cross(&xaxis, (const ChaVector3*)camUpVec, (const ChaVector3*)&zaxis);
	ChaVector3Normalize(&xaxis, &xaxis);

	ChaVector3Cross(&yaxis, (const ChaVector3*)&zaxis, (const ChaVector3*)&xaxis);
	ChaVector3Normalize(&yaxis, &yaxis);


	float trax, tray, traz;
	trax = -ChaVector3Dot(&xaxis, camEye);
	tray = -ChaVector3Dot(&yaxis, camEye);
	traz = -ChaVector3Dot(&zaxis, camEye);


	dstviewmat->_11 = xaxis.x;
	dstviewmat->_21 = xaxis.y;
	dstviewmat->_31 = xaxis.z;
	dstviewmat->_41 = trax;

	dstviewmat->_12 = yaxis.x;
	dstviewmat->_22 = yaxis.y;
	dstviewmat->_32 = yaxis.z;
	dstviewmat->_42 = tray;

	dstviewmat->_13 = zaxis.x;
	dstviewmat->_23 = zaxis.y;
	dstviewmat->_33 = zaxis.z;
	dstviewmat->_43 = traz;

	dstviewmat->_14 = 0.0f;
	dstviewmat->_24 = 0.0f;
	dstviewmat->_34 = 0.0f;
	dstviewmat->_44 = 1.0f;


}

/*
D3DXMATRIX *D3DXMatrixOrthoOffCenterRH(          D3DXMATRIX *pOut,
FLOAT l,
FLOAT r,
FLOAT t,
FLOAT b,
FLOAT zn,
FLOAT zf
);

2/(r-l)      0            0           0
0            2/(t-b)      0           0
0            0            1/(zn-zf)   0
(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1
*/

ChaMatrix* ChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf) 
{
	if (!pOut) {
		return NULL;
	}

	if ((r == l) || (t == b) || (zn == zf)) {
		ChaMatrixIdentity(pOut);
		return pOut;
	}


	pOut->_11 = (float)(2.0 / ((double)r - (double)l));
	pOut->_12 = 0.0f;
	pOut->_13 = 0.0f;
	pOut->_14 = 0.0f;

	pOut->_21 = 0.0f;
	pOut->_22 = (float)(2.0 / ((double)t - (double)b));
	pOut->_23 = 0.0f;
	pOut->_24 = 0.0f;

	pOut->_31 = 0.0f;
	pOut->_32 = 0.0f;
	pOut->_33 = (float)(1.0 / ((double)zn - (double)zf));
	pOut->_34 = 0.0f;

	pOut->_41 = (float)(((double)l + (double)r) / ((double)l - (double)r));
	pOut->_42 = (float)(((double)t + (double)b) / ((double)b - (double)t));
	pOut->_43 = (float)((double)zn / ((double)zn - (double)zf));
	pOut->_44 = 1.0f;

	return pOut;
}


/*
D3DXMATRIX *D3DXMatrixPerspectiveFovRH(          D3DXMATRIX *pOut,
FLOAT fovY,
FLOAT Aspect,
FLOAT zn,
FLOAT zf
);

w       0       0                0
0       h       0                0
0       0       zf/(zn-zf)      -1
0       0       zn*zf/(zn-zf)    0
where:
h is the view space height. It is calculated from
h = cot(fovY/2);

w is the view space width. It is calculated from
w = h / Aspect.
*/

ChaMatrix* ChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf)
{
	if (!pOut) {
		return NULL;
	}
	if (zn == zf) {
		ChaMatrixIdentity(pOut);
		return pOut;
	}


	double h, w;
	double c, s;
	c = cos((double)fovY / 2.0);
	s = sin((double)fovY / 2.0);
	if ((s != 0.0) && (Aspect != 0.0f)) {
		h = c / s;
		w = h / (double)Aspect;

		pOut->_11 = (float)w;
		pOut->_12 = 0.0f;
		pOut->_13 = 0.0f;
		pOut->_14 = 0.0f;

		pOut->_21 = 0;
		pOut->_22 = (float)h;
		pOut->_23 = 0.0f;
		pOut->_24 = 0.0f;

		pOut->_31 = 0.0f;
		pOut->_32 = 0.0f;
		pOut->_33 = (float)((double)zf / ((double)zn - (double)zf));
		pOut->_34 = -1.0f;

		pOut->_41 = 0.0f;
		pOut->_42 = 0.0f;
		pOut->_43 = (float)((double)zn * (double)zf / ((double)zn - (double)zf));
		pOut->_44 = 0.0f;
	}
	else {
		ChaMatrixIdentity(pOut);
	}

	return pOut;

}



const ChaMatrix* ChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	if (!dstmat || !srcq) {
		return NULL;
	}

	retmat = srcq->MakeRotMatX();
	*dstmat = retmat;
	return (const ChaMatrix*)dstmat;
}

/*
pOut
[in, out] 演算結果である D3DXMATRIX 構造体へのポインタ。
Yaw
[in] y 軸を中心とするヨー(ラジアン単位)。
Pitch
[in] x 軸を中心とするピッチ(ラジアン単位)。
Roll
[in] z 軸を中心とするロール(ラジアン単位)。
戻り値

指定されたヨー･ピッチ･ロールを持つ D3DXMATRIX 構造体へのポインタ。

注意

この関数の戻り値は、pOut パラメータの戻り値と同じである。したがって、D3DXMatrixRotationYawPitchRoll 関数を別の関数の引数として使える。

トランスフォームの順序は、最初にロール、次にピッチ、最後にヨーである。これは、オブジェクトのローカル座標軸を基準として、z 軸の周囲での回転、x 軸の周囲での回転、y 軸の周囲での回転と同じになる。
*/
ChaMatrix* ChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisy = ChaVector3(0.0f, 1.0f, 0.0f);
	ChaVector3 axisx = ChaVector3(1.0f, 0.0f, 0.0f);
	ChaVector3 axisz = ChaVector3(0.0f, 0.0f, 1.0f);

	CQuaternion qyaw, qpitch, qroll;
	qyaw.SetAxisAndRot(axisy, srcyaw);
	qpitch.SetAxisAndRot(axisx, srcpitch);
	qroll.SetAxisAndRot(axisz, srcroll);

	CQuaternion qrot;
	qrot = qyaw * qpitch * qroll;

	*pOut = qrot.MakeRotMatX();
	return pOut;
}

ChaMatrix* ChaMatrixRotationX(ChaMatrix* pOut, float srcrad)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisx = ChaVector3(1.0f, 0.0f, 0.0f);
	CQuaternion qx;
	qx.SetAxisAndRot(axisx, srcrad);

	*pOut = qx.MakeRotMatX();
	return pOut;
}

ChaMatrix* ChaMatrixRotationY(ChaMatrix* pOut, float srcrad)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisy = ChaVector3(0.0f, 1.0f, 0.0f);
	CQuaternion qy;
	qy.SetAxisAndRot(axisy, srcrad);

	*pOut = qy.MakeRotMatX();
	return pOut;
}

ChaMatrix* ChaMatrixRotationZ(ChaMatrix* pOut, float srcrad)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisz = ChaVector3(0.0f, 0.0f, 1.0f);
	CQuaternion qz;
	qz.SetAxisAndRot(axisz, srcrad);

	*pOut = qz.MakeRotMatX();
	return pOut;
}

void CQuaternionIdentity(CQuaternion* dstq)
{
	if (!dstq) {
		return;
	}
	dstq->x = 0.0f;
	dstq->y = 0.0f;
	dstq->z = 0.0f;
	dstq->w = 1.0f;

}

CQuaternion CQuaternionInv(CQuaternion srcq)
{
	CQuaternion invq;
	srcq.inv(&invq);
	return invq;
}


ChaMatrix MakeRotMatFromChaMatrix(ChaMatrix srcmat)
{
	CQuaternion tmpq;
	tmpq.MakeFromD3DXMat(srcmat);
	return tmpq.MakeRotMatX();
}

ChaMatrix ChaMatrixTranspose(ChaMatrix srcmat)
{
	ChaMatrix tmpmat = srcmat;
	ChaMatrix retmat;


	retmat._11 = tmpmat._11;
	retmat._12 = tmpmat._21;
	retmat._13 = tmpmat._31;
	retmat._14 = tmpmat._41;

	retmat._21 = tmpmat._12;
	retmat._22 = tmpmat._22;
	retmat._23 = tmpmat._32;
	retmat._24 = tmpmat._42;

	retmat._31 = tmpmat._13;
	retmat._32 = tmpmat._23;
	retmat._33 = tmpmat._33;
	retmat._34 = tmpmat._43;

	retmat._41 = tmpmat._14;
	retmat._42 = tmpmat._24;
	retmat._43 = tmpmat._34;
	retmat._44 = tmpmat._44;


	return retmat;
}




#ifdef CONVD3DX11
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
	this->_11 = m.r[0].m128_f32[0];
	this->_12 = m.r[0].m128_f32[1];
	this->_13 = m.r[0].m128_f32[2];
	this->_14 = m.r[0].m128_f32[3];

	this->_21 = m.r[1].m128_f32[0];
	this->_22 = m.r[1].m128_f32[1];
	this->_23 = m.r[1].m128_f32[2];
	this->_24 = m.r[1].m128_f32[3];

	this->_31 = m.r[2].m128_f32[0];
	this->_32 = m.r[2].m128_f32[1];
	this->_33 = m.r[2].m128_f32[2];
	this->_34 = m.r[2].m128_f32[3];

	this->_41 = m.r[3].m128_f32[0];
	this->_42 = m.r[3].m128_f32[1];
	this->_43 = m.r[3].m128_f32[2];
	this->_44 = m.r[3].m128_f32[3];

	//this->_11 = m.r[0].m128_f32[0];
	//this->_12 = m.r[1].m128_f32[0];
	//this->_13 = m.r[2].m128_f32[0];
	//this->_14 = m.r[3].m128_f32[0];

	//this->_21 = m.r[0].m128_f32[1];
	//this->_22 = m.r[1].m128_f32[1];
	//this->_23 = m.r[2].m128_f32[1];
	//this->_24 = m.r[3].m128_f32[1];

	//this->_31 = m.r[0].m128_f32[2];
	//this->_32 = m.r[1].m128_f32[2];
	//this->_33 = m.r[2].m128_f32[2];
	//this->_34 = m.r[3].m128_f32[2];

	//this->_41 = m.r[0].m128_f32[3];
	//this->_42 = m.r[1].m128_f32[3];
	//this->_43 = m.r[2].m128_f32[3];
	//this->_44 = m.r[3].m128_f32[3];


	return *this;
};

DirectX::XMMATRIX ChaMatrix::D3DX()
{
	DirectX::XMMATRIX retm;
	retm.r[0].m128_f32[0] = _11;
	retm.r[0].m128_f32[1] = _12;
	retm.r[0].m128_f32[2] = _13;
	retm.r[0].m128_f32[3] = _14;

	retm.r[1].m128_f32[0] = _21;
	retm.r[1].m128_f32[1] = _22;
	retm.r[1].m128_f32[2] = _23;
	retm.r[1].m128_f32[3] = _24;

	retm.r[2].m128_f32[0] = _31;
	retm.r[2].m128_f32[1] = _32;
	retm.r[2].m128_f32[2] = _33;
	retm.r[2].m128_f32[3] = _34;

	retm.r[3].m128_f32[0] = _41;
	retm.r[3].m128_f32[1] = _42;
	retm.r[3].m128_f32[2] = _43;
	retm.r[3].m128_f32[3] = _44;

	//retm.r[0].m128_f32[0] = _11;
	//retm.r[1].m128_f32[0] = _12;
	//retm.r[2].m128_f32[0] = _13;
	//retm.r[3].m128_f32[0] = _14;

	//retm.r[0].m128_f32[1] = _21;
	//retm.r[2].m128_f32[1] = _22;
	//retm.r[3].m128_f32[1] = _23;
	//retm.r[4].m128_f32[1] = _24;

	//retm.r[0].m128_f32[2] = _31;
	//retm.r[1].m128_f32[2] = _32;
	//retm.r[2].m128_f32[2] = _33;
	//retm.r[3].m128_f32[2] = _34;

	//retm.r[0].m128_f32[3] = _41;
	//retm.r[1].m128_f32[3] = _42;
	//retm.r[2].m128_f32[3] = _43;
	//retm.r[3].m128_f32[3] = _44;

	return retm;
}
#endif


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


