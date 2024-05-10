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

#define COLLISIONCPP
#include <Collision.h>

#define DBGH
#include <dbg.h>

#include <ChaVecCalc.h>

int ChkRay( int allowrev, int i1, int i2, int i3, 
	ChaVector3* pointbuf, ChaVector3 startpos, ChaVector3 dir, 
	float justval, int* justptr )
{
	ChaVector3 v1;
	v1 = startpos;

	ChaVector3 v;
	v = dir;

	ChaVector3 e;
	ChaVector3Normalize( &e, &v );

	ChaVector3 point1, point2, point3;
	point1 = *(pointbuf + i1);
	point2 = *(pointbuf + i2);
	point3 = *(pointbuf + i3);


	ChaVector3 s, t;
	s = point2 - point1;
	t = point3 - point1;
	ChaVector3 abc;
	ChaVector3Cross( &abc, (const ChaVector3*)&s, (const ChaVector3*)&t );
	ChaVector3Normalize( &abc, &abc );

	float d;
	d = -ChaVector3Dot( &abc, &point1 );

	float dotface = ChaVector3Dot( &abc, &e );
	if( dotface == 0.0f ){
//		_ASSERT( 0 );
		return 0;
	}
	if( (allowrev == 0) && (dotface < 0.0f) ){
		//���ʂ͓�����Ȃ�
		return 0;
	}

	float k;
	k = -( (ChaVector3Dot( &abc, &v1 ) + d) / ChaVector3Dot( &abc, &e ) );
	if( fabs( k ) <= justval ){
		(*justptr)++;
		return 0;
	}
	if( k < 0.0f ){
		return 0;
	}

	ChaVector3 q;
	q = v1 + e * k;

	ChaVector3 g0, g1, cA, cB, cC;
	
	g1 = point2 - point1;
	g0 = q - point1;
	ChaVector3Cross( &cA, (const ChaVector3*)&g0, (const ChaVector3*)&g1 );
	ChaVector3Normalize( &cA, &cA );

	g1 = point3 - point2;
	g0 = q - point2;
	ChaVector3Cross( &cB, (const ChaVector3*)&g0, (const ChaVector3*)&g1 );
	ChaVector3Normalize( &cB, &cB );

	g1 = point1 - point3;
	g0 = q - point3;
	ChaVector3Cross( &cC, (const ChaVector3*)&g0, (const ChaVector3*)&g1 );
	ChaVector3Normalize( &cC, &cC );


	float dota, dotb, dotc;
	dota = ChaVector3Dot( &abc, &cA );
	dotb = ChaVector3Dot( &abc, &cB );
	dotc = ChaVector3Dot( &abc, &cC );

	int zeroflag;
	zeroflag = (fabs(dota) < 0.05f) && (fabs(dotb) < 0.05f) && (fabs(dotc) < 0.05f);
	if( zeroflag != 0 ){
		//(*justptr)++;
		return 1;
	}

	if( ((dota <= -0.50f) && (dotb <= -0.50f) && (dotc <= -0.50f)) ||
		((dota >= 0.50f) && (dotb >= 0.50f) && (dotc >= 0.50f)) ){
		return 1;
	}else{
		return 0;
	}

}

int ChkRay(int allowrev, int i1, int i2, int i3,
	BINORMALDISPV* pointbuf, ChaVector3 startpos, ChaVector3 dir,
	float justval, int* justptr, ChaVector3* dsthitpos)
{
	if (!pointbuf || !justptr || !dsthitpos) {
		_ASSERT(0);
		return 0;
	}

	*dsthitpos = ChaVector3(0.0f, 0.0f, 0.0f);

	ChaVector3 v1;
	v1 = startpos;

	ChaVector3 v;
	v = dir;

	ChaVector3 e;
	ChaVector3Normalize(&e, &v);

	ChaVector3 point1, point2, point3;
	point1 = ChaVector3((pointbuf + i1)->pos.x, (pointbuf + i1)->pos.y, (pointbuf + i1)->pos.z);
	point2 = ChaVector3((pointbuf + i2)->pos.x, (pointbuf + i2)->pos.y, (pointbuf + i2)->pos.z);
	point3 = ChaVector3((pointbuf + i3)->pos.x, (pointbuf + i3)->pos.y, (pointbuf + i3)->pos.z);

	ChaVector3 s, t;
	s = point2 - point1;
	t = point3 - point1;
	ChaVector3 abc;
	ChaVector3Cross(&abc, (const ChaVector3*)&s, (const ChaVector3*)&t);
	ChaVector3Normalize(&abc, &abc);

	float d;
	d = -ChaVector3Dot(&abc, &point1);

	float dotface = ChaVector3Dot(&abc, &e);
	if (dotface == 0.0f) {
		//		_ASSERT( 0 );
		return 0;
	}
	if ((allowrev == 0) && (dotface < 0.0f)) {
		//���ʂ͓�����Ȃ�
		return 0;
	}

	float k;
	k = -((ChaVector3Dot(&abc, &v1) + d) / ChaVector3Dot(&abc, &e));
	if (fabs(k) <= justval) {
		(*justptr)++;
		return 0;
	}
	if (k < 0.0f) {
		return 0;
	}

	ChaVector3 q;
	q = v1 + e * k;

	*dsthitpos = q;//!!!!!!!!!

	ChaVector3 g0, g1, cA, cB, cC;

	g1 = point2 - point1;
	g0 = q - point1;
	ChaVector3Cross(&cA, (const ChaVector3*)&g0, (const ChaVector3*)&g1);
	ChaVector3Normalize(&cA, &cA);

	g1 = point3 - point2;
	g0 = q - point2;
	ChaVector3Cross(&cB, (const ChaVector3*)&g0, (const ChaVector3*)&g1);
	ChaVector3Normalize(&cB, &cB);

	g1 = point1 - point3;
	g0 = q - point3;
	ChaVector3Cross(&cC, (const ChaVector3*)&g0, (const ChaVector3*)&g1);
	ChaVector3Normalize(&cC, &cC);


	float dota, dotb, dotc;
	dota = ChaVector3Dot(&abc, &cA);
	dotb = ChaVector3Dot(&abc, &cB);
	dotc = ChaVector3Dot(&abc, &cC);

	int zeroflag;
	zeroflag = (fabs(dota) < 0.05f) && (fabs(dotb) < 0.05f) && (fabs(dotc) < 0.05f);
	if (zeroflag != 0) {
		//(*justptr)++;
		return 1;
	}

	if (((dota <= -0.50f) && (dotb <= -0.50f) && (dotc <= -0.50f)) ||
		((dota >= 0.50f) && (dotb >= 0.50f) && (dotc >= 0.50f))) {
		return 1;
	}
	else {
		return 0;
	}

}

int ChkRay(int allowrev,
	BINORMALDISPV p0, BINORMALDISPV p1, BINORMALDISPV p2,
	ChaVector3 startpos, ChaVector3 dir,
	float justval, int* justptr)
{
	ChaVector3 v1;
	v1 = startpos;

	ChaVector3 v;
	v = dir;

	ChaVector3 e;
	ChaVector3Normalize(&e, &v);

	ChaVector3 point1, point2, point3;
	point1 = ChaVector3(p0.pos.x, p0.pos.y, p0.pos.z);
	point2 = ChaVector3(p1.pos.x, p1.pos.y, p1.pos.z);
	point3 = ChaVector3(p2.pos.x, p2.pos.y, p2.pos.z);

	ChaVector3 s, t;
	s = point2 - point1;
	t = point3 - point1;
	ChaVector3 abc;
	ChaVector3Cross(&abc, (const ChaVector3*)&s, (const ChaVector3*)&t);
	ChaVector3Normalize(&abc, &abc);

	float d;
	d = -ChaVector3Dot(&abc, &point1);

	float dotface = ChaVector3Dot(&abc, &e);
	if (dotface == 0.0f) {
		//		_ASSERT( 0 );
		return 0;
	}
	if ((allowrev == 0) && (dotface < 0.0f)) {
		//���ʂ͓�����Ȃ�
		return 0;
	}

	float k;
	k = -((ChaVector3Dot(&abc, &v1) + d) / ChaVector3Dot(&abc, &e));
	if (fabs(k) <= justval) {
		(*justptr)++;
		return 0;
	}
	if (k < 0.0f) {
		return 0;
	}

	ChaVector3 q;
	q = v1 + e * k;

	ChaVector3 g0, g1, cA, cB, cC;

	g1 = point2 - point1;
	g0 = q - point1;
	ChaVector3Cross(&cA, (const ChaVector3*)&g0, (const ChaVector3*)&g1);
	ChaVector3Normalize(&cA, &cA);

	g1 = point3 - point2;
	g0 = q - point2;
	ChaVector3Cross(&cB, (const ChaVector3*)&g0, (const ChaVector3*)&g1);
	ChaVector3Normalize(&cB, &cB);

	g1 = point1 - point3;
	g0 = q - point3;
	ChaVector3Cross(&cC, (const ChaVector3*)&g0, (const ChaVector3*)&g1);
	ChaVector3Normalize(&cC, &cC);


	float dota, dotb, dotc;
	dota = ChaVector3Dot(&abc, &cA);
	dotb = ChaVector3Dot(&abc, &cB);
	dotc = ChaVector3Dot(&abc, &cC);

	int zeroflag;
	zeroflag = (fabs(dota) < 0.05f) && (fabs(dotb) < 0.05f) && (fabs(dotc) < 0.05f);
	if (zeroflag != 0) {
		//(*justptr)++;
		return 1;
	}

	if (((dota <= -0.50f) && (dotb <= -0.50f) && (dotc <= -0.50f)) ||
		((dota >= 0.50f) && (dotb >= 0.50f) && (dotc >= 0.50f))) {
		return 1;
	}
	else {
		return 0;
	}
}




int CalcShadowToPlane( ChaVector3 srcpos, ChaVector3 planedir, ChaVector3 planepos, ChaVector3* shadowptr )
{
	//�ʂƃ��C�Ƃ̌�_(shadow)�����߂�B
	//s-->start, e-->end, b-->point on plane, n-->plane dir

	if (!shadowptr){
		_ASSERT(0);
		return 1;
	}

	ChaVector3 sb, se, n;
	sb = planepos - (srcpos - planedir);
	se = (srcpos + planedir * 100.0f) - (srcpos - planedir);
	n = planedir;

	double t;
	t = ChaVector3DotDbl( &sb, &n ) / ChaVector3DotDbl( &se, &n );

	shadowptr->x = (float)((double)(srcpos.x - planedir.x) * (1.0 - t) + ((double)srcpos.x + (double)planedir.x * 100.0) * t);
	shadowptr->y = (float)((double)(srcpos.y - planedir.y) * (1.0 - t) + ((double)srcpos.y + (double)planedir.y * 100.0) * t);
	shadowptr->z = (float)((double)(srcpos.z - planedir.z) * (1.0 - t) + ((double)srcpos.z + (double)planedir.z * 100.0) * t);
	//*shadowptr = srcpos * (1.0f - t) + (srcpos + planedir * 100.0f) * t;

	return 0;
}