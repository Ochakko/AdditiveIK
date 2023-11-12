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

#include <InfScope.h>

#include <Model.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#define DBGH
#include <dbg.h>

#include <Bone.h>
#include <MQOFace.h>

//#include <VecMath.h>
#include <ChaCalc.h>

#include <Collision.h>

static int s_alloccnt = 0;

CInfScope::CInfScope()
{
	InitParams();
	s_alloccnt++;
	m_serialno = s_alloccnt;
}

CInfScope::~CInfScope()
{
	DestroyObjs();
}

int CInfScope::InitParams()
{
	m_serialno = 0;
	m_materialno = 0;
	m_validflag = 0;

	m_facenum = 0;
	m_ppface = 0;

	m_vnum = 0;
	m_pvert = 0;

	m_applyboneno = -1;
	m_targetobj = 0;

	m_minv = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_maxv = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_center = ChaVector3( 0.0f, 0.0f, 0.0f );

	m_settminmax = 0;
	m_tminv = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_tmaxv = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_tcenter = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_tmaxdist = 1.0f;

	m_tmpappbone = 0;

	return 0;
}
int CInfScope::DestroyObjs()
{
	if( m_ppface ){
		free( m_ppface );
		m_ppface = 0;
	}
	m_facenum = 0;

	InitParams();

	return 0;
}

int CInfScope::CalcCenter()
{
	if( (m_facenum <= 0) || !m_ppface || (m_vnum <= 0) || !m_pvert ){
		return 0;
	}

	ChaVector3 minv( 0.0f, 0.0f, 0.0f );
	ChaVector3 maxv( 0.0f, 0.0f, 0.0f );

	int firstflag = 1;

	int fno;
	for( fno = 0; fno < m_facenum; fno++ ){
		CMQOFace* curface = *( m_ppface + fno );
		int fi;
		for( fi = 0; fi < curface->m_pointnum; fi++ ){
			ChaVector3* chkv = m_pvert + curface->m_index[ fi ];

			if( firstflag == 1 ){
				minv = *chkv;
				maxv = *chkv;
				firstflag = 0;
			}else{
				if( minv.x > chkv->x ){
					minv.x = chkv->x;
				}
				if( minv.y > chkv->y ){
					minv.y = chkv->y;
				}
				if( minv.z > chkv->z ){
					minv.z = chkv->z;
				}

				if( maxv.x < chkv->x ){
					maxv.x = chkv->x;
				}
				if( maxv.y < chkv->y ){
					maxv.y = chkv->y;
				}
				if( maxv.z < chkv->z ){
					maxv.z = chkv->z;
				}
			}			
		}
	}

	m_minv = minv;
	m_maxv = maxv;
	m_center = ( minv + maxv ) * 0.5f;

	return 0;
}

int CInfScope::CheckInCnt( ChaVector3* vptr, int* incntptr, int* justptr )
{
	*incntptr = 0;
	*justptr = 0;

	ChaVector3 dir = ChaVector3( 0.987f, 1.765f, -1.567f );
	int allowrev = 1;

	int fno;
	for( fno = 0; fno < m_facenum; fno++ ){
		CMQOFace* curface = *( m_ppface + fno );
		if( curface->m_pointnum == 3 ){
			(*incntptr) += ChkRay( allowrev, curface->m_index[0], curface->m_index[1], curface->m_index[2], m_pvert, *vptr, dir, 0.001f, justptr );
		}else if( curface->m_pointnum == 4 ){
			(*incntptr) += ChkRay( allowrev, curface->m_index[0], curface->m_index[1], curface->m_index[2], m_pvert, *vptr, dir, 0.001f, justptr );
			(*incntptr) += ChkRay( allowrev, curface->m_index[0], curface->m_index[2], curface->m_index[3], m_pvert, *vptr, dir, 0.001f, justptr );
		}
		if( (*justptr) > 0 ){
			break;
		}
	}

	return 0;
}




