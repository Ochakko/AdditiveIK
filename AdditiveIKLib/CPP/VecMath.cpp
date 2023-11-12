#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <VecMath.h>



float ChaVector3Dot( ChaVector3* vec1, ChaVector3* vec2 )
{
	float retval;
	retval = vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
	return retval;
}
int ChaVector3Cross( ChaVector3* outvec, ChaVector3* vec1, ChaVector3* vec2 )
{
	ChaVector3 v1, v2;
	v1 = *vec1;
	v2 = *vec2;

	outvec->x = v1.y * v2.z - v1.z * v2.y;
	outvec->y = v1.z * v2.x - v1.x * v2.z;
	outvec->z = v1.x * v2.y - v1.y * v2.x;
	return 0;
}

float ChaVector3Length( ChaVector3* v )
{
	float leng;
	float mag;
	mag = v->x * v->x + v->y * v->y + v->z * v->z;
	if( mag != 0.0f ){
		leng = sqrtf( mag );
	}else{
		leng = 0.0f;
	}
	return leng;
}

int ChaVector3Normalize( ChaVector3* outv, ChaVector3* inv )
{
	ChaVector3 tmpv = *inv;
	float leng = ChaVector3Length( &tmpv );
	if( leng != 0.0f ){
		*outv = tmpv / leng;
	}else{
		*outv = ChaVector3( 0.0f, 0.0f, 0.0f );
	}
	return 0;
}
