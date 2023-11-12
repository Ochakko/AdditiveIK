#include "stdafx.h"
//#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>
#include <search.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <GetMaterial.h>

#include <crtdbg.h>

#include <mqoobject.h>
#include <polymesh3.h>
//#include <InfScope.h>
#include <InfBone.h>
#include <mqomaterial.h>
#include <mqoface.h>
#include <Bone.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;


#ifndef LINECHARLENG
#define LINECHARLENG 2048
#endif

#define CHKN3PNUM	1000

static int sortfunc_material( void *context, const void *elem1, const void *elem2);
static int sortfunc_order0( void *context, const void *elem1, const void *elem2);


CPolyMesh3::CPolyMesh3()
{
	InitParams();

}

CPolyMesh3::~CPolyMesh3()
{
	DestroyObjs();
}

void CPolyMesh3::InitParams()
{
	m_fbxfileflag = false;

	m_orgpointnum = 0;
	m_orgfacenum = 0;
	m_facenum = 0;

	m_facet = 59.5f;

	m_mqoface = 0;//外部メモリ
	m_pointbuf = 0;//外部メモリ
	m_n3p = 0;

	m_optleng = 0;
	m_optmatnum = 0;
	//m_optv = 0;
	m_dispv = 0;
	m_createoptflag = 0;

	//m_infbone = 0;

	m_matblock = 0;
	m_dispindex = 0;

	ZeroMemory( &chkalpha, sizeof( CHKALPHA ) );
	ZeroMemory( &m_bound, sizeof( MODELBOUND ) );
}

void CPolyMesh3::DestroyObjs()
{
/*
typedef struct tag_n3sm
{
	int smfacenum;
	N3P** ppsmface;//N3Pのポインタの配列
}N3SM;
typedef  struct tag_n3p
{
	PERFACE*	perface;
	PERVERT*	pervert;
	N3SM*		n3sm;
}N3P;//n*3
*/

	DestroySystemDispObj();

	if( m_matblock ){
		free( m_matblock );
		m_matblock = 0;
	}

	//if( m_infbone ){
	//	delete [] m_infbone;
	//	m_infbone = 0;
	//}

	InitParams();
}

void CPolyMesh3::DestroySystemDispObj()
{
	if (m_n3p) {
		delete[] m_n3p;
		m_n3p = 0;
	}

	if (m_dispv) {
		free(m_dispv);
		m_dispv = 0;
	}

	if (m_dispindex) {
		free(m_dispindex);
		m_dispindex = 0;
	}

	if (GetFbxFileFlag()) {
		//###############################################
		//CMQOObject::DestroySystemDispObj()が呼ばれた後
		//###############################################
		m_mqoface = 0;//外部メモリ
		m_pointbuf = 0;//外部メモリ
	}

}



int sortfunc_material( void *context, const void *elem1, const void *elem2)
{
	N3P* n3p1 = (N3P*)elem1;
	N3P* n3p2 = (N3P*)elem2;
	CPolyMesh3* pm3 = (CPolyMesh3*)context;

	int dvno;
	dvno = n3p1->pervert->vno - n3p2->pervert->vno;
	if( dvno == 0 ){
		int dmatno;
		dmatno = n3p1->perface->materialno - n3p2->perface->materialno;
		if( dmatno == 0 ){
			float du;
			du = n3p1->pervert->uv[0].x - n3p2->pervert->uv[0].x;
			if( du == 0.0f ){
				float dv;
				dv = n3p1->pervert->uv[0].y - n3p2->pervert->uv[0].y;
				if( dv == 0.0f ){
					int dvcolflag;
					dvcolflag = n3p1->pervert->vcolflag - n3p2->pervert->vcolflag;
					if( dvcolflag ){
						DWORD dvcol;
						dvcol = n3p1->pervert->vcol - n3p2->pervert->vcol;
						if( dvcol == 0 ){
							return 0;
						}else{
							if( n3p1->pervert->vcol < n3p2->pervert->vcol ){
								return -1;
							}else{
								return 1;
							}
						}
					}else{
						return dvcolflag;
					}
				}else{
					if( dv >= 0.0f ){
						return 1;
					}else{
						return -1;
					}
				}
			}else{
				if( du >= 0.0f ){
					return 1;
				}else{
					return -1;
				}
			}
		}else{
			return dmatno;
		}
	}else{
		return dvno;
	}
}
int sortfunc_order0( void *context, const void *elem1, const void *elem2)
{
	N3P* n3p1 = (N3P*)elem1;
	N3P* n3p2 = (N3P*)elem2;
	CPolyMesh3* pm3 = (CPolyMesh3*)context;

	int dmatno;
	dmatno = n3p1->perface->materialno - n3p2->perface->materialno;
	if( dmatno == 0 ){
		int dfaceno;
		dfaceno = n3p1->perface->faceno - n3p2->perface->faceno;
		if( dfaceno == 0 ){
			int dindexno;
			dindexno = n3p1->pervert->indexno - n3p2->pervert->indexno;
			if( dindexno == 0 ){
				return 0;
			}else{
				return dindexno;
			}
		}else{
			return dfaceno;
		}
	}else{
		return dmatno;
	}
}



int CPolyMesh3::CreatePM3(bool fbxfileflag, int pointnum, int facenum, float facet, ChaVector3* pointptr, CMQOFace* faceptr, 
	map<int,CMQOMaterial*>& srcmat, ChaMatrix multmat)
{
	m_orgpointnum = pointnum;
	m_orgfacenum = facenum;
	m_mqoface = faceptr;
	m_pointbuf = pointptr;
	m_facet = facet;
	m_fbxfileflag = fbxfileflag;

	CallF( CreateN3PFromMQOFace( 0, &m_facenum ), return 1 );

	if( m_facenum <= 0 ){
		return 0;
	}
	
	int n3pnum = m_facenum * 3;

	m_n3p = new N3P[n3pnum];
	if( !m_n3p ){
		_ASSERT( 0 );
		return 1;
	}
	int n3pno;
	for (n3pno = 0; n3pno < n3pnum; n3pno++) {
		N3P* initn3p = m_n3p + n3pno;
		if (initn3p) {
			initn3p->InitParams();
		}
	}

	int n3;
	for( n3 = 0; n3 < n3pnum; n3++ ){
		N3P* curn3p = m_n3p + n3;

		PERFACE* perface = (PERFACE*)malloc( sizeof( PERFACE ) );
		if( !perface ){
			_ASSERT( 0 );
			//2021/09/28
			curn3p->perface = NULL;
			//curn3p->pervert = 0;
			//curn3p->n3sm = 0;
			return 1;
		}
		ZeroMemory( perface, sizeof( PERFACE ) );

		PERVERT* pervert = (PERVERT*)malloc( sizeof( PERVERT ) );
		if( !pervert ){
			_ASSERT( 0 );
			//2021/09/28
			//curn3p->perface = 0;
			curn3p->pervert = NULL;
			//curn3p->n3sm = 0;
			return 1;
		}
		ZeroMemory( pervert, sizeof( PERVERT ) );

		N3SM* n3sm = new N3SM();
		if( !n3sm ){
			_ASSERT( 0 );
			//2021/09/28
			//curn3p->perface = 0;
			//curn3p->pervert = 0;
			curn3p->n3sm = NULL;
			return 1;
		}
		ZeroMemory( n3sm, sizeof( N3SM ) );

		curn3p->perface = perface;
		curn3p->pervert = pervert;
		curn3p->n3sm = n3sm;
	}

	int tmpnum = 0;
	CallF( CreateN3PFromMQOFace( m_n3p, &tmpnum ), return 1 );
	if( tmpnum != m_facenum ){
		_ASSERT( 0 );
		return 1;
	}

//////////
	CallF( CalcOrgNormal(), return 1 );
	qsort_s( m_n3p, m_facenum * 3, sizeof( N3P ), sortfunc_material, (void*)this );
	CallF( SetSMFace(), return 1 );
	qsort_s( m_n3p, m_facenum * 3, sizeof( N3P ), sortfunc_order0, (void*)this );

///////////
	CallF( SetOptV( 0, &m_optleng, &m_optmatnum, srcmat ), return 1 );
	if( (m_optleng <= 0) || (m_optmatnum <= 0) ){
		_ASSERT( 0 );
		return 0;
	}
	m_dispv = (PM3DISPV*)malloc( sizeof( PM3DISPV ) * m_optleng );
	if( !m_dispv ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_dispv, sizeof( PM3DISPV ) * m_optleng );
	m_matblock = (MATERIALBLOCK*)malloc( sizeof( MATERIALBLOCK ) * m_optmatnum );
	if( !m_matblock ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_matblock, sizeof( MATERIALBLOCK ) * m_optmatnum );

	m_dispindex = (int*)malloc( sizeof( int ) * m_facenum * 3 );
	if( !m_dispindex ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_dispindex, sizeof( int ) * m_facenum * 3 );

	int tmpleng, tmpmatnum;
	CallF( SetOptV( m_dispv, &tmpleng, &tmpmatnum, srcmat ), return 1 );
	if( (tmpleng != m_optleng) || (tmpmatnum != m_optmatnum) ){
		_ASSERT( 0 );
		return 1;
	}

	CallF( CalcBound(), return 1 );


	//m_infbone = new CInfBone[ m_orgpointnum ];
	//if( !m_infbone ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	m_createoptflag = 1;

	return 0;
}

int CPolyMesh3::MultVert( ChaMatrix multmat )
{
	int pno;
	for( pno = 0; pno < m_orgpointnum; pno++ ){
		ChaVector3 tmpv;
		ChaVector3TransformCoord( &tmpv, m_pointbuf + pno, &multmat );
		*( m_pointbuf + pno ) = tmpv;
	}
	return 0;
}

int CPolyMesh3::CreateN3PFromMQOFace( N3P* n3pptr, int* numptr )
{
	int orgfno;
	int setno = 0;

	for( orgfno = 0; orgfno < m_orgfacenum; orgfno++ ){
		CMQOFace* curmqof = m_mqoface + orgfno;
		N3P* curn3p;
		if( n3pptr ){
			curn3p = n3pptr + setno * 3;
		}else{
			curn3p = 0;
		}

		if( curmqof->GetPointNum() == 3 ){
			if( curn3p ){
				int indexno;
				//int seti[3] = {0, 2, 1};
				int seti[3] = { 0, 1, 2 };
				for( indexno = 0; indexno < 3; indexno++ ){
					(curn3p + indexno)->perface->orgfaceno = orgfno;
					(curn3p + indexno)->perface->faceno = setno;
					(curn3p + indexno)->perface->materialno = curmqof->GetMaterialNo();

					(curn3p + indexno)->pervert->indexno = indexno;
					(curn3p + indexno)->pervert->vno = curmqof->GetIndex( seti[indexno] );
					(curn3p + indexno)->pervert->uvnum = 1;
					(curn3p + indexno)->pervert->uv[0] = curmqof->GetUV( seti[indexno] );
					(curn3p + indexno)->pervert->vcolflag = curmqof->GetVcolSetFlag();
					(curn3p + indexno)->pervert->vcol = (DWORD)( curmqof->GetCol( seti[indexno] ) & 0xFFFFFFFF );
				}			
			}
			setno++;
		}else if( curmqof->GetPointNum() == 4 ){
			if( curn3p ){
				int indexno;
				//int seti[3] = {0, 2, 1};
				int seti[3] = { 0, 1, 2 };
				for( indexno = 0; indexno < 3; indexno++ ){
					(curn3p + indexno)->perface->orgfaceno = orgfno;
					(curn3p + indexno)->perface->faceno = setno;
					(curn3p + indexno)->perface->materialno = curmqof->GetMaterialNo();

					(curn3p + indexno)->pervert->indexno = indexno;
					(curn3p + indexno)->pervert->vno = curmqof->GetIndex( seti[indexno] );
					(curn3p + indexno)->pervert->uvnum = 1;
					(curn3p + indexno)->pervert->uv[0] = curmqof->GetUV( seti[indexno] );
					(curn3p + indexno)->pervert->vcolflag = curmqof->GetVcolSetFlag();
					(curn3p + indexno)->pervert->vcol = (DWORD)( curmqof->GetCol( seti[indexno] ) & 0xFFFFFFFF );
				}			
	
				//int seci[3] = { 0, 3, 2 };
				int seci[3] = { 0, 2, 3 };
				for( indexno = 3; indexno < 6; indexno++ ){
					(curn3p + indexno)->perface->orgfaceno = orgfno;
					(curn3p + indexno)->perface->faceno = setno + 1;
					(curn3p + indexno)->perface->materialno = curmqof->GetMaterialNo();

					(curn3p + indexno)->pervert->indexno = indexno - 3;
					(curn3p + indexno)->pervert->vno = curmqof->GetIndex( seci[indexno - 3] );
					(curn3p + indexno)->pervert->uvnum = 1;
					(curn3p + indexno)->pervert->uv[0] = curmqof->GetUV( seci[indexno - 3] );
					(curn3p + indexno)->pervert->vcolflag = curmqof->GetVcolSetFlag();
					(curn3p + indexno)->pervert->vcol = (DWORD)( curmqof->GetCol( seci[indexno - 3] ) & 0xFFFFFFFF );
				}			
			}
			setno += 2;
		}
	}

	*numptr = setno;

	return 0;
}

int CPolyMesh3::CalcOrgNormal()
{
	int fno;
	for( fno = 0; fno < m_facenum; fno++ ){
		int n3p = fno * 3;
		N3P* curn3p = m_n3p + n3p;

		ChaVector3 vpos[3];
		int indexno;
		for( indexno = 0; indexno < 3; indexno++ ){
			int vertno = (curn3p + indexno)->pervert->vno;
			vpos[indexno] = *( m_pointbuf + vertno );
		}

		ChaVector3 nvec;
		CalcNormal( &nvec, vpos, vpos + 2, vpos + 1 );

		for( indexno = 0; indexno < 3; indexno++ ){
			(curn3p + indexno)->perface->facenormal = nvec;
			(curn3p + indexno)->pervert->smnormal = nvec;
		}
	}

	return 0;
}


int CPolyMesh3::CalcNormal( ChaVector3* newn, ChaVector3* curp, ChaVector3* aftp1, ChaVector3* aftp2 )
{
	ChaVector3 vec1, vec2, crossvec;

	vec1 = *aftp1 - *curp;
	vec2 = *aftp2 - *curp;

	if( vec1 != vec2 ){
		Vec3Cross( &crossvec, &vec1, &vec2 );
	}else{
		crossvec = *curp;
	}

	Vec3Normalize( newn, &crossvec );

	return 0;
}

int CPolyMesh3::Vec3Cross( ChaVector3* pOut, ChaVector3* pV1, ChaVector3* pV2 )
{
	//ChaVector3 v;
	
	double x1, y1, z1, x2, y2, z2;
	x1 = pV1->x; y1 = pV1->y; z1 = pV1->z;
	x2 = pV2->x; y2 = pV2->y; z2 = pV2->z;

	pOut->x = (float)(y1 * z2 - z1 * y2);
	pOut->y = (float)(z1 * x2 - x1 * z2);
	pOut->z = (float)(x1 * y2 - y1 * x2);

	return 0;
}

int CPolyMesh3::Vec3Normalize( ChaVector3* retvec, ChaVector3* srcvec )
{
	double mag;
	double srcx, srcy, srcz;
	srcx = srcvec->x; srcy = srcvec->y; srcz = srcvec->z;

	mag = srcx * srcx + srcy * srcy + srcz * srcz;
	double leng;
	leng = sqrt( mag );

	if( leng > 0.0 ){
		double divleng;
		divleng = 1.0f / leng;
		retvec->x = (float)(srcx * divleng);
		retvec->y = (float)(srcy * divleng);
		retvec->z = (float)(srcz * divleng);
	}else{
		retvec->x = 0.0f;
		retvec->y = 0.0f;
		retvec->z = 0.0f;
	}

	return 0;
}

int CPolyMesh3::SetSMFace()
{
//n3psortを頭からチェックし、まったく同じものが無ければcreateflagに１を足す。
//n3psortでvnoが等しいものの法線をチェックしスムージングするものが見つかったらppsmfaceに追加する。
	N3P* samen3p[ CHKN3PNUM ];
	int samevnum = 0;
	int n3pdirty[ CHKN3PNUM ];
	ZeroMemory( n3pdirty, sizeof( int ) * CHKN3PNUM );


	float basedot;
	basedot = (float)cos( m_facet * DEG2PAI );


	samen3p[0] = m_n3p;
	samevnum = 1;

	int n3p;
	N3P* curn3p = 0;
	N3P* befn3p = m_n3p;
	for( n3p = 1; n3p < (m_facenum * 3); n3p++ ){
		curn3p = m_n3p + n3p;

		if( curn3p->pervert->vno == befn3p->pervert->vno ){
			int cmpmat = sortfunc_material( (void*)this, (void*)befn3p, (void*)curn3p );
			if( cmpmat != 0 ){
				curn3p->pervert->createflag |= 1;
			}
			if( samevnum < CHKN3PNUM ){
				samen3p[samevnum] = curn3p;
				samevnum++;
			}
		}else{
			curn3p->pervert->createflag |= 1;



			if( samevnum >= 2 ){
				int n1, n2;
				N3P* n3p1;
				N3P* n3p2;
				for( n1 = 0; n1 < samevnum; n1++ ){
					n3p1 = samen3p[n1];
					for( n2 = 0; n2 < samevnum; n2++ ){
						n3p2 = samen3p[n2];
						if( n1 == n2 ){
							continue;
						}
						float chkdot = ChaVector3Dot( &(n3p1->perface->facenormal), &(n3p2->perface->facenormal) );
						if( chkdot >= basedot ){
							CallF( AddSmFace( n3p1, n3p2 ), return 1 );
						}
					}
				}
			}

//n3psortのvnoが等しいものの*ppsmfaceをチェックし、自分(vno)が始めて出現したときまたは出現しなかったとき(N3P)createflagに２を足す。
			ZeroMemory( n3pdirty, sizeof( int ) * CHKN3PNUM );
			int i1;
			for( i1 = 0; i1 < samevnum; i1++ ){
				N3P* curn3p2 = samen3p[i1];
				if( i1 == 0 ){
					curn3p2->pervert->createflag |= 2;
				}

				int smno;
				for( smno = 0; smno < curn3p2->n3sm->smfacenum; smno++ ){
					N3P* smn3p = (N3P*)(*(curn3p2->n3sm->ppsmface + smno));
					if( curn3p2->perface->faceno != smn3p->perface->faceno ){
						curn3p2->pervert->smnormal += smn3p->perface->facenormal;
					}
					int chkn3;
					for( chkn3 = 0; chkn3 < samevnum; chkn3++ ){
						N3P* chkn3p = samen3p[chkn3];
						if( smn3p == chkn3p ){
							n3pdirty[chkn3] = 1;
						}
					}
				}
				ChaVector3Normalize( &(curn3p2->pervert->smnormal), &(curn3p2->pervert->smnormal) );
			}

			int i2;
			for( i2 = 0; i2 < samevnum; i2++ ){
				if( n3pdirty[i2] == 0 ){
					samen3p[i2]->pervert->createflag |= 2;
				}
			}

			samen3p[0] = curn3p;
			samevnum = 1;
		}


		befn3p = curn3p;
	}

	return 0;
}


int CPolyMesh3::AddSmFace( N3P* n3p1, N3P* n3p2 )
{
	int faceno1, faceno2;
	faceno1 = n3p1->perface->faceno;
	faceno2 = n3p2->perface->faceno;

	int chki;
	int findflag = 0;
	for( chki = 0; chki < n3p1->n3sm->smfacenum; chki++ ){
		if( faceno2 == ((N3P*)*(n3p1->n3sm->ppsmface + chki))->perface->faceno ){
			findflag = 1;
		}
	}
	if( findflag == 0 ){
		int curnum = n3p1->n3sm->smfacenum;
		N3P** newsmface = 0;
		newsmface = (N3P**)realloc( n3p1->n3sm->ppsmface, sizeof(N3P*) * ((size_t)curnum + 1) );
		if( !newsmface ){
			_ASSERT( 0 );
			return 1;
		}
		else {
			n3p1->n3sm->ppsmface = newsmface;
		}
		*( n3p1->n3sm->ppsmface + curnum ) = (N3P*)n3p2;
		n3p1->n3sm->smfacenum = curnum + 1;
	}

//////////
	findflag = 0;
	for( chki = 0; chki < n3p2->n3sm->smfacenum; chki++ ){
		if( faceno1 == ((N3P*)*(n3p2->n3sm->ppsmface + chki))->perface->faceno ){
			findflag = 1;
		}
	}
	if( findflag == 0 ){
		int curnum = n3p2->n3sm->smfacenum;
		N3P** newsmface = 0;
		newsmface = (N3P**)realloc( n3p2->n3sm->ppsmface, sizeof(N3P*) * ((size_t)curnum + 1) );
		if( !newsmface ){
			_ASSERT( 0 );
			return 1;
		}
		else {
			n3p2->n3sm->ppsmface = newsmface;
		}
		*( n3p2->n3sm->ppsmface + curnum ) = (N3P*)n3p1;
		n3p2->n3sm->smfacenum = curnum + 1;
	}

	return 0;
}

int CPolyMesh3::SetOptV( PM3DISPV* dispv, int* pleng, int* matnum, map<int,CMQOMaterial*>& srcmat )
{
	*pleng = 0;

	int n3;
	int materialcnt = 0;
	int befmaterialno = -2;
	int curmaterialno = 0;
	int startmatflag = 0;
	int setno = 0;
	int curfaceno = 0;
	int curtrino = 0;
	int beffaceno = 0;
	N3P* curn3p = 0;
	N3P* befn3p = 0;
	for( n3 = 0; n3 < (m_facenum * 3); n3++ ){
		curn3p = m_n3p + n3;
		curmaterialno = curn3p->perface->materialno;
		curfaceno = n3 / 3;
		curtrino = n3 - curfaceno * 3;

		if( befmaterialno != curmaterialno ){
			if( dispv ){
				(m_matblock + materialcnt)->materialno = curmaterialno;
				CMQOMaterial* setmqomat = GetMaterialFromNo( srcmat, curmaterialno );
				if( !setmqomat ){
					int defmatno = (int)srcmat.size() - 1;
					_ASSERT( defmatno >= 0 );
					setmqomat = GetMaterialFromNo( srcmat, defmatno );
					_ASSERT( setmqomat );
				}
				(m_matblock + materialcnt)->mqomat = setmqomat;
				(m_matblock + materialcnt)->startface = curfaceno;
				(m_matblock + materialcnt)->endface = curfaceno;
			}
			materialcnt++;
			startmatflag = 1;
		}else{
			if( dispv && befn3p ){
				(m_matblock + materialcnt - 1)->endface = curfaceno;
			}
			startmatflag = 0;
		}

//		if( curn3p->pervert->createflag != 0 ){

			if( dispv ){
				PM3DISPV* curv = dispv + setno;
				curv->pos.x = (m_pointbuf + curn3p->pervert->vno)->x;
				curv->pos.y = (m_pointbuf + curn3p->pervert->vno)->y;
				curv->pos.z = (m_pointbuf + curn3p->pervert->vno)->z;
				curv->pos.w = 1.0f;
				
				//curv->normal = curn3p->pervert->smnormal;
				if (m_fbxfileflag == true) {
					curv->normal = -curn3p->pervert->smnormal;//2023/07/29 反転
				}
				else {
					curv->normal = curn3p->pervert->smnormal;
				}
				
				curv->uv = curn3p->pervert->uv[0];

				/***
				curoptv->materialno = curn3p->perface->materialno;
				curoptv->normal = curn3p->pervert->smnormal;
				curoptv->orgfaceno = curn3p->perface->orgfaceno;
				curoptv->orgvno = curn3p->pervert->vno;
				curoptv->pos = *(m_pointbuf + curn3p->pervert->vno);
				curoptv->uv[0] = curn3p->pervert->uv[0];
				curoptv->uv[1] = curn3p->pervert->uv[1];
				curoptv->uvnum = curn3p->pervert->uvnum;
				curoptv->vcolflag = curn3p->pervert->vcolflag;
				curoptv->vcol = curn3p->pervert->vcol;
				***/


				//*( m_dispindex + setno ) = setno;

				if (m_fbxfileflag == true) {
					//2023/07/29 反対周り
					if (curtrino == 0) {
						*(m_dispindex + setno) = curfaceno * 3 + 2;
					}
					else if (curtrino == 1) {
						*(m_dispindex + setno) = curfaceno * 3 + 1;
					}
					else if (curtrino == 2) {
						*(m_dispindex + setno) = curfaceno * 3 + 0;
					}
					else {
						_ASSERT(0);
						*(m_dispindex + setno) = setno;
					}
				}
				else {
					*(m_dispindex + setno) = setno;
				}
			}

			setno++;
//		}


		befn3p = curn3p;
		befmaterialno = curn3p->perface->materialno;
		beffaceno = curfaceno;
	}

	*pleng = setno;
	*matnum = materialcnt;

	return 0;
}

int CPolyMesh3::ChkAlphaNum( map<int,CMQOMaterial*>& srcmat )
{
	chkalpha.alphanum = 0;
	chkalpha.notalphanum = 0;

	int n3;
	for( n3 = 0; n3 < (m_facenum * 3); n3 += 3 ){//3つおきにチェック
		N3P* curn3p = m_n3p + n3;
		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( srcmat, curn3p->perface->materialno );
		if( !curmat ){
			continue;
		}

		if( (curmat->GetCol().w != 1.0f) || (curmat->GetTransparent() != 0) ){
			chkalpha.alphanum++;
		}else{
			chkalpha.notalphanum++;
		}
	}

	return 0;
}

int CPolyMesh3::CalcBound()
{
/***
typedef struct tag_modelbaund
{
	ChaVector3 min;
	ChaVector3 max;
	ChaVector3 center;
	float		r;
}MODELBAUND;
***/

	m_bound.min = ChaVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_bound.max = ChaVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bound.center = ChaVector3(0.0f, 0.0f, 0.0f);
	m_bound.r = 1.0f;


	if( (m_orgpointnum == 0) || (m_facenum == 0) ){
		m_bound.min = ChaVector3(0.0f, 0.0f, 0.0f);
		m_bound.max = ChaVector3(0.0f, 0.0f, 0.0f);
		m_bound.center = ChaVector3(0.0f, 0.0f, 0.0f);
		m_bound.r = 1.0f;
		return 0;
	}


	//m_bound.min = *m_pointbuf;
	//m_bound.max = *m_pointbuf;

	int vno;
	for( vno = 0; vno < m_orgpointnum; vno++ ){
		ChaVector3 curv = *( m_pointbuf + vno );

		if( m_bound.min.x > curv.x ){
			m_bound.min.x = curv.x;
		}
		if( m_bound.min.y > curv.y ){
			m_bound.min.y = curv.y;
		}
		if( m_bound.min.z > curv.z ){
			m_bound.min.z = curv.z;
		}

		if( m_bound.max.x < curv.x ){
			m_bound.max.x = curv.x;
		}
		if( m_bound.max.y < curv.y ){
			m_bound.max.y = curv.y;
		}
		if( m_bound.max.z < curv.z ){
			m_bound.max.z = curv.z;
		}
	}

	m_bound.center = ( m_bound.min + m_bound.max ) * 0.5f;

	ChaVector3 diff;
	diff = m_bound.center - m_bound.min;
	m_bound.r = (float)ChaVector3LengthDbl(&diff);

	return 0;
}

int CPolyMesh3::MultScale( ChaVector3 srcscale, ChaVector3 srctra )
{
	int vno;
	for( vno = 0; vno < m_optleng; vno++ ){
		N3P* curn3p = m_n3p + vno;
		int orgvno = curn3p->pervert->vno;
		ChaVector3* srcv = m_pointbuf + orgvno;
		PM3DISPV* curv = m_dispv + vno;
		curv->pos = ChaVector4( srcv->x * srcscale.x + srctra.x, srcv->y * srcscale.y + srctra.y, srcv->z * srcscale.z + srctra.z, 1.0f );
	}

	return 0;
}

int CPolyMesh3::IncludeTransparent(CMQOObject* srcobj, float alphamult, bool* pfound_noalpha, bool* pfound_alpha)
{
	if (!pfound_noalpha || !pfound_alpha || !srcobj) {
		_ASSERT(0);
		return 1;
	}


	bool found_noalpha = false;
	bool found_alpha = false;
	int laternum = srcobj->GetLaterMaterialNum();


	int blno;
	for (blno = 0; blno < GetOptMatNum(); blno++) {
		MATERIALBLOCK* currb = GetMatBlock() + blno;
		CMQOMaterial* curmat;
		curmat = currb->mqomat;
		if (!curmat) {
			continue;
		}
		if (curmat->GetTransparent() != 0) {//2023/09/24 VRoidの裾(すそ)透過対策
			found_alpha = true;
		}
		else {
			if ((curmat->GetDif4F().w * alphamult) <= 0.99999f) {
				found_alpha = true;
			}
			else {
				found_noalpha = true;
			}
		}

		//latermaterialチェック
		if ((found_alpha == false) && (laternum > 0)) {
			int laterno;
			for (laterno = 0; laterno < laternum; laterno++) {
				LATERMATERIAL chklatermat = srcobj->GetLaterMaterial(laterno);
				if (chklatermat.pmaterial && (chklatermat.pmaterial == curmat)) {
					found_alpha = true;
				}
			}
		}

		if (found_noalpha && found_alpha) {
			break;
		}
	}

	*pfound_noalpha = found_noalpha;
	*pfound_alpha = found_alpha;

	return 0;
}
