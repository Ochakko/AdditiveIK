#include "stdafx.h"
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


#include <polymesh4.h>


#include <mqoobject.h>
#include <InfScope.h>
#include <InfBone.h>
#include <mqomaterial.h>
#include <mqoface.h>
#include <Bone.h>

#include <dispobj.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

static int sortfunc_material( void *context, const void *elem1, const void *elem2);



CPolyMesh4::CPolyMesh4()
{
	InitParams();
}
CPolyMesh4::~CPolyMesh4()
{
	DestroyObjs();
}
void CPolyMesh4::InitParams()
{
	m_normalmappingmode = 0;//0:polygonvertex, 1:controlpoint

	m_materialoffset.clear();

	m_orgpointnum = 0;
	m_orgfacenum = 0;
	m_facenum = 0;

	m_mqoface = 0;//外部メモリ
	m_pointbuf = 0;//外部メモリ
	m_normal = 0;//外部メモリ
	m_uvbuf = 0;//外部メモリ
	
	m_normalleng = 0;
	m_uvleng = 0;

	m_triface = 0;

	m_optleng = 0;

	m_dispv = 0;
	m_createoptflag = 0;

	m_infbone = 0;
	m_pm3inf = 0;

	m_dispindex = 0;
	m_orgindex = 0;
	m_fbxindex = 0;

	ZeroMemory( &chkalpha, sizeof( CHKALPHA ) );
	ZeroMemory( &m_bound, sizeof( MODELBOUND ) );

	m_dirtyflag = 0;
	m_lastvalidvno = 0;


}
void CPolyMesh4::DestroyObjs()
{

	DestroySystemDispObj();

}

void CPolyMesh4::DestroySystemDispObj()
{
	if (m_triface) {
		delete[] m_triface;
		m_triface = 0;
	}

	if (m_dispv) {
		free(m_dispv);
		m_dispv = 0;
	}

	if (m_dispindex) {
		free(m_dispindex);
		m_dispindex = 0;
	}

	if (m_orgindex) {
		free(m_orgindex);
		m_orgindex = 0;
	}
	if (m_fbxindex) {
		free(m_fbxindex);
		m_fbxindex = 0;
	}

	if (m_infbone) {
		delete[] m_infbone;
		m_infbone = 0;
	}

	if (m_pm3inf) {
		free(m_pm3inf);
		m_pm3inf = 0;
	}

	if (m_dirtyflag) {
		free(m_dirtyflag);
		m_dirtyflag = 0;
	}

	//###############################################
	//CMQOObject::DestroySystemDispObj()が呼ばれた後
	//###############################################
	m_mqoface = 0;//外部メモリ
	m_pointbuf = 0;//外部メモリ
	m_normal = 0;//外部メモリ
	m_uvbuf = 0;//外部メモリ

}

int sortfunc_material( void *context, const void *elem1, const void *elem2)
{
	CMQOFace* face1 = (CMQOFace*)elem1;
	CMQOFace* face2 = (CMQOFace*)elem2;
	CPolyMesh4* pm4 = (CPolyMesh4*)context;

	int dvno;
	dvno = face1->GetMaterialNo() - face2->GetMaterialNo();
	if( dvno > 0 ){
		return 1;
	}else if( dvno < 0 ){
		return -1;
	}else{
		return 0;
	}
}


int CPolyMesh4::CreatePM4(int normalmappingmode, int pointnum, int facenum, int normalleng, int uvleng, ChaVector3* pointptr, ChaVector3* nptr, ChaVector2* uvptr, CMQOFace* faceptr, map<int,CMQOMaterial*>& srcmat )
{
	m_orgpointnum = pointnum;
	m_orgfacenum = facenum;
	m_mqoface = faceptr;
	m_pointbuf = pointptr;
	m_normal = nptr;
	m_uvbuf = uvptr;
	m_normalleng = normalleng;
	m_uvleng = uvleng;
	m_normalmappingmode = normalmappingmode;

	CallF( SetTriFace( 0, &m_facenum ), return 1 );
	if( m_facenum <= 0 ){
		return 0;
	}

	m_triface = new CMQOFace[ m_facenum ];//三角形だけのCMQOFace
	_ASSERT( m_triface );
	int chknum = 0;
	CallF( SetTriFace( m_triface, &chknum ), return 1 );

	if( m_facenum != chknum ){
		_ASSERT( 0 );
		return 1;
	}

	//qsort_s( m_triface, m_facenum, sizeof( CMQOFace ), sortfunc_material, (void*)this );
//////////
	int optmatnum = 0;
	CallF( SetOptV( 0, &m_optleng, &optmatnum, srcmat ), return 1 );
	if( (m_optleng <= 0) ){
		_ASSERT( 0 );
		return 0;
	}
	m_dispv = (PM3DISPV*)malloc( sizeof( PM3DISPV ) * m_optleng );
	if( !m_dispv ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_dispv, sizeof( PM3DISPV ) * m_optleng );

	m_dispindex = (int*)malloc( sizeof( int ) * m_facenum * 3 );
	if( !m_dispindex ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_dispindex, sizeof( int ) * m_facenum * 3 );

	m_orgindex = (int*)malloc( sizeof( int ) * m_facenum * 3 );
	if( !m_orgindex ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_orgindex, sizeof( int ) * m_facenum * 3 );

	m_fbxindex = (int*)malloc(sizeof(int) * m_facenum * 3);
	if (!m_fbxindex){
		_ASSERT(0);
		return 1;
	}
	ZeroMemory(m_fbxindex, sizeof(int) * m_facenum * 3);

	m_dirtyflag = (int*)malloc(sizeof(int) * m_orgpointnum);
	if (!m_dirtyflag){
		_ASSERT(0);
		return 1;
	}
	ZeroMemory(m_dirtyflag, sizeof(int) * m_orgpointnum);


	m_materialoffset.clear();


	int tmpleng, tmpmatnum;
	SetOptV( m_dispv, &tmpleng, &tmpmatnum, srcmat );
//	CallF( SetOptV( m_dispv, m_pm3inf, &tmpleng, &tmpmatnum, srcmat ), return 1 );
	if( (tmpleng != m_optleng) ){
		_ASSERT( 0 );
		return 1;
	}

	CallF( CalcBound(), return 1 );


	m_infbone = new CInfBone[ m_orgpointnum ];
	if( !m_infbone ){
		_ASSERT( 0 );
		return 1;
	}
	int opno;
	for( opno = 0; opno < m_orgpointnum; opno++ ){
		(m_infbone + opno)->InitParams();
	}
	m_pm3inf = (PM3INF*)malloc( sizeof( PM3INF ) * m_optleng );
	if( !m_pm3inf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_pm3inf, sizeof( PM3INF ) * m_optleng );


	SetLastValidVno();


	m_createoptflag = 1;

	return 0;
}
	
int CPolyMesh4::SetTriFace( CMQOFace* faceptr, int* numptr )
{
	int fno;
	int setno = 0;
	for( fno = 0; fno < m_orgfacenum; fno++ ){
		CMQOFace* srcface = m_mqoface + fno;
		if( srcface->GetPointNum() == 3 ){
			if( faceptr ){
				CMQOFace* dstface = faceptr + setno;
				dstface->SetBoneType( MIKOBONE_NONE );
				dstface->SetCol( 0, srcface->GetCol( 0 ) );
				dstface->SetCol( 1, srcface->GetCol( 1 ) );
				dstface->SetCol( 2, srcface->GetCol( 2 ) );
				dstface->SetCol( 3, 0 );
				dstface->SetFaceNo( setno );
				dstface->SetHasUV( srcface->GetHasUV() );
				dstface->SetIndex( 0, srcface->GetIndex( 0 ) );
				dstface->SetIndex( 1, srcface->GetIndex( 1 ) );
				dstface->SetIndex( 2, srcface->GetIndex( 2 ) );
				dstface->SetIndex( 3, 0 );
				dstface->SetMaterialNo( srcface->GetMaterialNo() );
				dstface->SetPointNum( 3 );
				dstface->SetUV( 0, srcface->GetUV( 0 ) );
				dstface->SetUV( 1, srcface->GetUV( 1 ) );
				dstface->SetUV( 2, srcface->GetUV( 2 ) );
				dstface->SetUV( 3, ChaVector2( 0.0f, 0.0f ) );
				dstface->SetVcolSetFlag( srcface->GetVcolSetFlag() );
			}

			setno++;
		//}else if( srcface->GetPointNum() >= 4 ){
		}
		else if (srcface->GetPointNum() != 3) {
			::MessageBoxA(NULL, "triangle only!! Convert to triangles and retry.", "Data Type Error!!", MB_OK);
			_ASSERT( 0 );
			return 1;
		}
	}
	*numptr = setno;

	return 0;
}


int CPolyMesh4::SetOptV( PM3DISPV* dispv, int* pleng, int* matnum, map<int,CMQOMaterial*>& srcmat )
{
	*pleng = 0;
	*matnum = 0;

	int fno;
	int setno = 0;
	int curfaceno = 0;
	int beffaceno = 0;
	CMQOFace* curface = 0;
	CMQOFace* befface = 0;

	CMQOMaterial* befmaterial = 0;
	CMQOMaterial* curmaterial = 0;
	
	for( fno = 0; fno < m_facenum; fno++ ){
		curface = m_triface + fno;
		curfaceno = fno;

		if( dispv ){
			int vi[3] = {0, 2, 1};
			int fbx2020NormalVi[3] = { 0, 1, 2 };


			int materialindex = curface->GetMaterialNo();
			curmaterial = GetMaterialFromNo(srcmat, curface->GetMaterialNo());

			if ((fno == 0) || (curmaterial != befmaterial)) {
				m_materialoffset[setno * 3] = curmaterial;
			}

			int vcnt;
			for( vcnt = 0; vcnt < 3; vcnt++ ){
				PM3DISPV* curv = dispv + (setno * 3 + vcnt);//!!!!!!!!!!!!!!!!!!!!
				int vno = (m_triface + setno)->GetIndex( vi[vcnt] );
				// 0 2 1
				_ASSERT( (vno >= 0) && (vno < m_orgpointnum) );
				curv->pos.x = (m_pointbuf + vno)->x;
				curv->pos.y = (m_pointbuf + vno)->y;
				curv->pos.z = (m_pointbuf + vno)->z;
				curv->pos.w = 1.0f;

				*(m_dirtyflag + vno) = 1;//!!!!!!!!!!

				*(m_orgindex + setno * 3 + vi[vcnt]) = vno;
				//*(m_fbxindex + setno * 3 + vi[vcnt]) = vno;

				if( m_normal ){

					if (m_normalleng == (m_facenum * 3)) {
						if (m_normalmappingmode == 0) {
							curv->normal = *(m_normal + (setno * 3 + vi[vcnt]));//!!!!!! eByPolygonVertex : save with MameBake3D
						}
						else {
							curv->normal = *(m_normal + (setno * 3 + vcnt));//!!!!!!!!!!! eByControlPoint : save with Maya FBX
						}
					}
					else if( m_normalleng == m_orgpointnum ){
						curv->normal = *(m_normal + vno);
					}else{
						_ASSERT( 0 );
					}
				}else{
					curv->normal = ChaVector3( 0.0f, 0.0f, 1.0f );
					_ASSERT( 0 );
				}

				if( m_uvbuf ){
					if (m_uvleng == (m_facenum * 3)) {
						//0 2 1 if文優先
						curv->uv = *(m_uvbuf + setno * 3 + vi[vcnt]);
					}
					else if (m_uvleng >= m_orgpointnum) {//m_orgpointnumのときとm_orgpointnum * 2のとき
						curv->uv = *(m_uvbuf + vno);
					}
					else {
						_ASSERT(0);
					}



					//if (m_uvleng == (m_facenum * 3)){
					//	//0 2 1 if文優先
					//	curv->uv = *(m_uvbuf + setno * 3 + vi[vcnt]);
					//}
					//else if(m_uvleng >= m_orgpointnum){//m_orgpointnumのときとm_orgpointnum * 2のとき
					//	curv->uv = *(m_uvbuf + vno);
					//}
					//else{
					//	_ASSERT(0);
					//}

					//curv->uv.y = 1.0f - curv->uv.y;//表示用 CModel::GetFBXMeshでしている

				}else{
					curv->uv = ChaVector2( 0.0f, 0.0f );
				}

				if (m_dispindex) {
					*(m_dispindex + setno * 3 + vcnt) = setno * 3 + vcnt;
				}
			}


			befmaterial = curmaterial;

		}
		setno ++;

		befface = curface;
		beffaceno = curfaceno;
	}

	*pleng = setno * 3;
	*matnum = 1;

	return 0;
}


int CPolyMesh4::ChkAlphaNum( map<int,CMQOMaterial*>& srcmat )
{

	chkalpha.alphanum = 0;
	chkalpha.notalphanum = 0;

	int fno;
	for( fno = 0; fno < m_facenum; fno++ ){//3つおきにチェック
		CMQOFace* curface = m_triface + fno;
		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( srcmat, curface->GetMaterialNo() );
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
int CPolyMesh4::CalcBound()
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
	m_bound.r = (float)ChaVector3LengthDbl( &diff );

	return 0;
}

int CPolyMesh4::DumpInfBone( CMQOObject* srcobj, map<int,CBone*>& srcbonelist )
{
	DbgOut( L"check!!! DumpInfBone %d\r\n", srcobj->GetObjectNo() );

	int vno;
	for( vno = 0; vno < m_orgpointnum; vno++ ){
		CInfBone* curib = m_infbone + vno;
		INFDATA* curid = curib->GetInfData(srcobj);
		_ASSERT( curid );
		DbgOut( L"vno %d, infnum %d\r\n", vno, curid->m_infnum );
		int infno;
		for( infno = 0; infno < curid->m_infnum; infno++ ){
			INFELEM* curie = curid->m_infelem + infno;
			int boneno = curie->boneno;
			float dispinf = curie->dispinf;

			CBone* curbone = srcbonelist[ boneno ];
			_ASSERT( curbone );
			if (curbone){
				DbgOut(L"\tinfno %d, bonename %s, dispinf %f\r\n",
					infno, curbone->GetWBoneName(), dispinf);
			}
		}
	}
	DbgOut( L"checkend!!! DumpInfBone\r\n\r\n" );

	return 0;
}

int CPolyMesh4::SetPm3InfNoSkin( ID3D11Device* pdev, CMQOObject* srcobj, int clusterno, map<int,CBone*>& srcbonelist )
{
	if (!m_pm3inf || !m_triface) {
		_ASSERT(0);
		return 1;
	}
	ZeroMemory( m_pm3inf, sizeof( PM3INF ) * m_optleng );

	int fno;
	int setno = 0;
	for( fno = 0; fno < m_facenum; fno++ ){
		if( m_pm3inf ){
			int vi[3] = {0, 2, 1};
			int vcnt;
			for( vcnt = 0; vcnt < 3; vcnt++ ){
				PM3INF* curinf = m_pm3inf + ((size_t)setno * 3 + vcnt);
				int vno = (m_triface + setno)->GetIndex( vi[vcnt] );
				_ASSERT( (vno >= 0) && (vno < m_orgpointnum) );
				curinf->boneindex[0] = clusterno;
				curinf->weight[0] = 1.0f;
				curinf->boneindex[1] = 0;
				curinf->weight[1] = 0.0f;
				curinf->boneindex[2] = 0;
				curinf->weight[2] = 0.0f;
				curinf->boneindex[3] = 0;
				curinf->weight[3] = 0.0f;
			}
		}
		setno ++;
	}

	if( (setno * 3) != m_optleng ){
		_ASSERT( 0 );
		return 1;
	}

	CallF( srcobj->GetDispObj()->CreateDispObj( pdev, this, 1 ), return 1 );

	return 0;
}

int CPolyMesh4::SetPm3Inf(CMQOObject* srcobj)
{
	if (!m_pm3inf || !m_triface) {
		_ASSERT(0);
		return 1;
	}
	ZeroMemory( m_pm3inf, sizeof( PM3INF ) * m_optleng );

	int fno;
	int setno = 0;
	for( fno = 0; fno < m_facenum; fno++ ){
		if( m_pm3inf ){
			int vi[3] = {0, 2, 1};
			int vcnt;
			for( vcnt = 0; vcnt < 3; vcnt++ ){
				PM3INF* curinf = m_pm3inf + ((size_t)setno * 3 + vcnt);
				int vno = (m_triface + setno)->GetIndex( vi[vcnt] );
				_ASSERT( (vno >= 0) && (vno < m_orgpointnum) );
				INFDATA* curinfdata = (m_infbone + vno)->GetInfData(srcobj);
				if( curinfdata ){
					int ieno;
					for( ieno = 0; ieno < curinfdata->m_infnum; ieno++ ){
						curinf->boneindex[ieno] = curinfdata->m_infelem[ieno].boneno;
						curinf->weight[ieno] = curinfdata->m_infelem[ieno].dispinf;
					}
				}else{
//					_ASSERT( 0 );
					curinf->boneindex[0] = 0;
					curinf->weight[0] = 1.0f;
				}

			}
		}

		setno ++;
	}

	if( (setno * 3) != m_optleng ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CPolyMesh4::UpdateMorphBuffer( ChaVector3* mpoint )
{
	if( !m_dispv ){
		_ASSERT( 0 );
		return 1;
	}

	int vno;
	for( vno = 0; vno < (m_facenum * 3); vno++ ){
		int curindex = *(m_orgindex + vno);
		PM3DISPV* curv = m_dispv + vno;

		curv->pos = ChaVector4( (mpoint + curindex)->x, (mpoint + curindex)->y, (mpoint + curindex)->z, 1.0f );
	}

	return 0;
}

ChaVector3 CPolyMesh4::GetNormalByControlPointNo(int vno)
{
	if (m_normalleng == (m_facenum * 3)){
		int findindex = -1;
		int chki;
		for (chki = 0; chki < (m_facenum * 3); chki++){
			int curvno = *(m_orgindex + chki);
			if (vno == curvno){
				findindex = chki;
				break;
			}
		}
		if (findindex >= 0){
			return *(m_normal + findindex);
		}
		else{
			_ASSERT(0);
			return ChaVector3(0.0f, 0.0f, 0.0f);
		}
	}
	else if(m_normalleng >= m_orgpointnum){
		return *(m_normal + vno);
	}
	else{
		_ASSERT(0);
		return ChaVector3(0.0f, 0.0f, 0.0f);
	}
}
ChaVector2 CPolyMesh4::GetUVByControlPointNo(int vno)
{
	/*
	if (m_uvleng == (m_facenum * 3)){
		curv->uv = *(m_uvbuf + setno * 3 + vi[vcnt]);
	}
	else if (m_uvleng >= m_orgpointnum){//m_orgpointnumのときとm_orgpointnum * 2のとき
		curv->uv = *(m_uvbuf + vno);
	}
	*/

	/*
	if (m_uvleng == (m_facenum * 3)){
		int findindex = -1;
		int chki;
		for (chki = 0; chki < (m_facenum * 3); chki++){
			int curvno = *(m_orgindex + chki);
			if (vno == curvno){
				findindex = chki;
				break;
			}
		}
		if (findindex >= 0){
			return *(m_uvbuf + findindex);
		}
		else{
			_ASSERT(0);
			return ChaVector2(0.0f, 0.0f);
		}
	}
	else if (m_uvleng >= m_orgpointnum){
		return *(m_uvbuf + vno);
	}
	else{
		_ASSERT(0);
		return ChaVector2(0.0f, 0.0f);
	}
	*/
	if (m_uvleng == (m_facenum * 3)){
		int findindex = -1;
		int chki;
		for (chki = 0; chki < (m_facenum * 3); chki++){
			int curvno = *(m_orgindex + chki);
			if (vno == curvno){
				findindex = chki;
				break;
			}
		}
		if (findindex >= 0){
			return *(m_uvbuf + findindex);
		}
		else{
			_ASSERT(0);
			return ChaVector2(0.0f, 0.0f);
		}
	}
	else if (m_uvleng >= m_orgpointnum){
		return *(m_uvbuf + vno);
	}
	else{
		_ASSERT(0);
		return ChaVector2(0.0f, 0.0f);
	}

	//m_dispv = (PM3DISPV*)malloc(sizeof(PM3DISPV) * m_optleng);


}

int CPolyMesh4::SetLastValidVno()
{
	int vno;
	m_lastvalidvno = -1;

	for (vno = 0; vno < m_orgpointnum; vno++){
		if (*(m_dirtyflag + vno) == 1){
			m_lastvalidvno = vno;
		}
	}

	return 0;
}
