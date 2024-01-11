#include "stdafx.h"
//#include <stdafx.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <mqoobject.h>

#include <DispObj.h>
#include <polymesh3.h>
#include <polymesh4.h>
#include <GetMaterial.h>

#include <ExtLine.h>

#include <mqoface.h>
#include <mqomaterial.h>
#include <InfBone.h>
#include <Bone.h>
#include <Model.h>

#include <RigidElem.h>

#include <Collision.h>

#include <GlobalVar.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

typedef struct tag_mqotriline
{
	int index[2];
	float leng;
	int sortno;
} MQOTRILINE;


static int SortTriLine( const VOID* arg1, const VOID* arg2 );
static int s_alloccnt = 0;

typedef struct tag_latheelem
{
	float height;
	float dist;
} LATHEELEM;


CMQOObject::CMQOObject() : m_frustum()
{
	InitParams();
	s_alloccnt++;
	m_objectno = s_alloccnt;
}

CMQOObject::~CMQOObject()
{

	DestroySystemDispObj();


	if( m_pm3 ){
		delete m_pm3;
		m_pm3 = 0;
	}
	if( m_pm4 ){
		delete m_pm4;
		m_pm4 = 0;
	}
	if( m_extline ){
		delete m_extline;
		m_extline = 0;
	}

	if (m_dispobj) {
		delete m_dispobj;
		m_dispobj = nullptr;
	}

	if( m_displine ){
		delete m_displine;
		m_displine = 0;
	}


	DestroyShapeObj();


	//########
	//bank管理
	//########
	//map<int, CMQOMaterial*>::iterator itr;
	//for( itr = m_material.begin(); itr != m_material.end(); itr++ ){
	//	CMQOMaterial* delmat = itr->second;
	//	if( delmat ){
	//		delete delmat;
	//	}
	//}
	m_onloadmaterial.clear();

}

void CMQOObject::DestroySystemDispObj()
{
	if (m_pointbuf) {
		free(m_pointbuf);
		m_pointbuf = 0;
	}

	if (m_facebuf) {
		delete[] m_facebuf;
		m_facebuf = 0;
	}

	if (m_colorbuf) {
		free(m_colorbuf);
		m_colorbuf = 0;
	}

	if (m_pointbuf2) {
		free(m_pointbuf2);
		m_pointbuf2 = 0;
	}

	if (m_facebuf2) {
		delete[] m_facebuf2;
		m_facebuf2 = 0;
	}

	if (m_colorbuf2) {
		free(m_colorbuf2);
		m_colorbuf2 = 0;
	}

	if (m_connectface) {
		delete[] m_connectface;
		m_connectface = 0;
	}

	if (m_normal) {
		free(m_normal);
		m_normal = 0;
	}
	if (m_uvbuf) {
		free(m_uvbuf);
		m_uvbuf = 0;
	}
	if (m_uvbuf1) {
		free(m_uvbuf1);
		m_uvbuf1 = 0;
	}

	if (m_mpoint) {
		free(m_mpoint);
		m_mpoint = 0;
	}

}


int CMQOObject::DestroyShapeObj()
{
	map<string,ChaVector3*>::iterator itrshape;
	for( itrshape = m_shapevert.begin(); itrshape != m_shapevert.end(); itrshape++ ){
		ChaVector3* curshape = itrshape->second;
		if( curshape ){
			free( curshape );
		}
	}
	m_shapevert.clear();
	m_shapeweight.clear();

	m_findshape.clear();

	return 0;
}


void CMQOObject::InitParams()
{
	m_objfrom = OBJFROM_MQO;

	m_pnode = 0;
	m_cluster.clear();

	m_lodnum = 1;

	m_normalmappingmode = 0;//0:eByPolygonVertex, 1:eByControlPoint
	m_dbgcount = 0;

	m_dispflag = 1;
	m_objectno = -1;
	ZeroMemory( m_name, 256 );
	ZeroMemory( m_engname, 256 );

	m_patch = 0;
	m_segment = 1;

	m_visible = 15;
	m_locking = 0;

	m_shading	= 1;
	m_facet = 59.5f;
	m_color.x = 1.0f;
	m_color.y = 1.0f;
	m_color.z = 1.0f;
	m_color.w = 1.0f;
	m_color_type = 0;
	m_mirror = 0;
	m_mirror_axis = 1;
	m_issetmirror_dis = 0;
	m_mirror_dis = 0.0f;
	m_lathe = 0;
	m_lathe_axis = 0;
	m_lathe_seg = 3;
	m_vertex = 0;
	//BVertex;
	m_face = 0;

	m_pointbuf = 0;
	m_facebuf = 0;

	m_vertex2 = 0;
	m_face2 = 0;
	m_pointbuf2 = 0;
	m_facebuf2 = 0;

	m_hascolor = 0;
	m_colorbuf = 0;
	m_colorbuf2 = 0;

	m_connectnum = 0;
	m_connectface = 0;

	m_pm3 = 0;
	m_pm4 = 0;
	m_extline = 0;
	ChaMatrixIdentity( &m_multmat );

	m_dispobj = nullptr;
	m_displine = 0;

	m_normalleng = 0;
	m_normal = 0;
	m_uvleng = 0;
	m_uvbuf = 0;
	m_uvbuf1 = 0;

	m_shapevert.clear();
	m_shapeweight.clear();
	m_mpoint = 0;
	m_findshape.clear();

	m_meshmat.SetIdentity();

	m_latermaterial.clear();
	m_onloadmaterial.clear();

	m_getuvnum = 0;

//	next = 0;
}


int CMQOObject::SetParams( char* srcchar, int srcleng )
{
	char pat[15][20] = 
	{
		"Object",
		"patch",
		"segment",
		"shading",
		"facet",
		"color_type",
		"color",
		"mirror_axis",
		"mirror_dis",
		"mirror",
		"lathe_axis",
		"lathe_seg",
		"lathe",

		"visible",
		"locking"
	};

	int patno;
	size_t patleng;
	size_t pos = 0;
	int cmp;

	int stepnum;
	for( patno = 0; patno < 15; patno++ ){
		while( (pos < srcleng) && ( (*(srcchar + pos) == ' ') || (*(srcchar + pos) == '\t') ) ){
			pos++;
		}
		
		patleng = strlen( pat[patno] );
		cmp = strncmp( pat[patno], srcchar + pos, patleng );
		if( cmp == 0 ){

			pos += patleng;//!!!
			if (pos >= INT_MAX) {
				DbgOut(L"MQOObject : SetParams : pos overflow error !!!");
				_ASSERT(0);
				return 1;
			}

			switch( patno ){
			case 0:
				CallF( GetName( m_name, 256, srcchar, (int)pos, srcleng ), return 1 );
				break;
			case 1:
				CallF( GetInt( &m_patch, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 2:
				CallF( GetInt( &m_segment, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 3:
				CallF( GetInt( &m_shading, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 4:
				CallF( GetFloat( &m_facet, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 5:
				CallF( GetInt( &m_color_type, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 6:
				CallF( GetFloat( &m_color.x, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				pos += stepnum;
				if( pos >= srcleng ){
					DbgOut( L"MQOObject : SetParams : GetFloat : pos error !!!" );
					_ASSERT( 0 );
					return 1;
				}


				CallF( GetFloat( &m_color.y, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				pos += stepnum;
				if( pos >= srcleng ){
					DbgOut( L"MQOObject : SetParams : GetFloat : pos error !!!" );
					_ASSERT( 0 );
					return 1;
				}

				CallF( GetFloat( &m_color.z, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 7:
				CallF( GetInt( &m_mirror_axis, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 8:
				CallF( GetFloat( &m_mirror_dis, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				m_issetmirror_dis = 1;//!!!!
				break;
			case 9:
				CallF( GetInt( &m_mirror, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 10:
				CallF( GetInt( &m_lathe_axis, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 11:
				CallF( GetInt( &m_lathe_seg, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 12:
				CallF( GetInt( &m_lathe, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 13:
				CallF( GetInt( &m_visible, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			case 14:
				CallF( GetInt( &m_locking, srcchar, (int)pos, srcleng, &stepnum ), return 1 );
				break;
			default:
				_ASSERT( 0 );
				break;
			}

			break;
		}
	}
	


	return 0;

}

int CMQOObject::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, ((size_t)endpos - startpos) );
		tempchar[endpos - startpos] = 0;

		*dstint = atoi( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}
int CMQOObject::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') && (*(srcchar + startpos) != '.') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) || (*(srcchar + endpos) == '.') )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, ((size_t)endpos - startpos) );
		tempchar[endpos - startpos] = 0;

		*dstfloat = (float)atof( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}

int CMQOObject::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) && 
		( ( *(srcchar + startpos) == ' ' ) || ( *(srcchar + startpos) == '\t' ) || ( *(srcchar + startpos) == '\"' ) ) 
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( ( *(srcchar + endpos) != ' ' ) && ( *(srcchar + endpos) != '\t' ) && (*(srcchar + endpos) != '\r') && (*(srcchar + endpos) != '\r\n') && (*(srcchar + endpos) != '\"') )
	){
		endpos++;
	}

	if( (endpos - startpos < dstleng) && (endpos - startpos > 0) ){
		strncpy_s( dstchar, dstleng, srcchar + startpos, ((size_t)endpos - startpos) );
		*(dstchar + endpos - startpos) = 0;

	}else{
		_ASSERT( 0 );
	}


	return 0;
}

int CMQOObject::SetVertex( int* vertnum, char* srcchar, int srcleng )
{
	//vertex, または、　BVertexを含む文字列を受け取る。
	if( m_pointbuf ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	char headerchar1[] = "vertex";
	size_t headerleng1 = strlen( headerchar1 );
	char* find1;
	find1 = strstr( srcchar, headerchar1 );


	char headerchar2[] = "Vector";
	size_t headerleng2 = strlen( headerchar2 );
	char* find2;
	find2 = strstr( srcchar, headerchar2 );

	int pos = 0;
	if( find1 != NULL ){
		pos = (int)(find1 + headerleng1 - srcchar);
	}else if( find2 != NULL ){
		pos = (int)(find2 + headerleng2 - srcchar);
	}else{
		_ASSERT(0);
		return 1;
	}
	int stepnum;
	ret = GetInt( &m_vertex, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	*vertnum = m_vertex;
	DbgOut( L"MQOObject : SetVertex : vertex %d\r\n", m_vertex );


	m_pointbuf = (ChaVector3*)malloc( sizeof( ChaVector3 ) * m_vertex );
	if( !m_pointbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_pointbuf, sizeof( ChaVector3 ) * m_vertex );


	return 0;
}
int CMQOObject::SetPointBuf( unsigned char* srcptr, int srcnum )
{
	if( !m_pointbuf || (srcnum > m_vertex) ){
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( m_pointbuf, srcptr, srcnum * sizeof( VEC3F ) );

	return 0;

}


int CMQOObject::SetPointBuf( int vertno, char* srcchar, int srcleng )
{
	if( !m_pointbuf || (vertno >= m_vertex) ){
		_ASSERT( 0 );
		return 1;
	}


	ChaVector3* dstvec;
	dstvec = m_pointbuf + vertno;
	int pos, stepnum;
	pos = 0;

	CallF( GetFloat( &dstvec->x, srcchar, pos, srcleng, &stepnum ), return 1 );
	pos += stepnum;
	if( pos >= srcleng ){
		DbgOut( L"MQOObject : SePointBuf : GetFloat : pos error !!!" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( GetFloat( &dstvec->y, srcchar, pos, srcleng, &stepnum ), return 1 );
	pos += stepnum;
	if( pos >= srcleng ){
		DbgOut( L"MQOObject : SePointBuf : GetFloat : pos error !!!" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( GetFloat( &dstvec->z, srcchar, pos, srcleng, &stepnum ), return 1 );

	return 0;
}

int CMQOObject::SetFace( int* facenum, char* srcchar, int srcleng )
{
	if( m_facebuf ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	char headerchar[] = "face";
	size_t headerleng = strlen( headerchar );

	char* find;
	find = strstr( srcchar, headerchar );
	if( find == NULL ){
		_ASSERT( 0 );
		return 1;
	}

	int pos;
	pos = (int)(find + headerleng - srcchar);
	int stepnum;
	ret = GetInt( &m_face, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	*facenum = m_face;
	DbgOut( L"MQOObject : SetFace : face %d\r\n", m_face );

	
	m_facebuf = new CMQOFace[ m_face ];
	if( !m_facebuf ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CMQOObject::SetFaceBuf( int faceno, char* srcchar, int srcleng, int materialoffset )
{
	if( !m_facebuf || (faceno >= m_face) ){
		_ASSERT( 0 );
		return 1;
	}


	CMQOFace* dstface;
	dstface = m_facebuf + faceno;

	_ASSERT( dstface );

	CallF( dstface->SetParams( srcchar, srcleng ), return 1 );
	int newmatno = dstface->GetMaterialNo() + materialoffset;
	dstface->SetMaterialNo( newmatno );


	return 0;
}

int CMQOObject::Dump()
{
	WCHAR wname[256] = {0};
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_name, 256, wname, 256 );


	DbgOut( L"Ojbect : %s\r\n", wname );
	DbgOut( L"\tpatch %d\r\n", m_patch );
	DbgOut( L"\tsegment %d\r\n", m_segment );

	DbgOut( L"\tvisible %d\r\n", m_visible );
	DbgOut( L"\tlocking %d\r\n", m_locking );

	DbgOut( L"\tshading %d\r\n", m_shading );
	DbgOut( L"\tfacet %f\r\n", m_facet );
	DbgOut( L"\tcolor %f %f %f\r\n", m_color.x, m_color.y, m_color.z );
	DbgOut( L"\tcolor_type %d\r\n", m_color_type );
	DbgOut( L"\tmirror %d\r\n", m_mirror );
	DbgOut( L"\tmirror_axis %d\r\n", m_mirror_axis );
	DbgOut( L"\tmirror_dis %f\r\n", m_mirror_dis );
	DbgOut( L"\tlathe %d\r\n", m_lathe );
	DbgOut( L"\tlathe_axis %d\r\n", m_lathe_axis );
	DbgOut( L"\tlathe_seg %d\r\n", m_lathe_seg );

	DbgOut( L"\tvertex %d\r\n", m_vertex );
	int vertno;
	ChaVector3* curvec = m_pointbuf;
	for( vertno = 0; vertno < m_vertex; vertno++ ){
		DbgOut( L"\t\t%f %f %f\r\n", curvec->x, curvec->y, curvec->z );
		curvec++;
	}

	if( m_hascolor && m_colorbuf ){
		DbgOut( L"\tcolor\r\n" );
		for( vertno = 0; vertno < m_vertex; vertno++ ){
			DbgOut( L"\t\t%d : %f %f %f\r\n", vertno, m_colorbuf[vertno].x, m_colorbuf[vertno].y, m_colorbuf[vertno].z );
		}
	}

	int ret;
	DbgOut( L"\tface %d\r\n", m_face );
	int faceno;
	CMQOFace* curface = m_facebuf;
	for( faceno = 0; faceno < m_face; faceno++ ){
		ret = curface->Dump();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		curface++;
	}

	return 0;
}

int CMQOObject::CreateColor()
{
	if( m_vertex <= 0 ){
		DbgOut( L"MQOObject : CreateColor : vertex error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_colorbuf = (ChaVector4*)malloc( sizeof( ChaVector4 ) * m_vertex );
	if( !m_colorbuf ){
		DbgOut( L"MQOObject : CreateColor : colorbuf alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( m_colorbuf, sizeof( ChaVector4 ) * m_vertex );

	m_hascolor = 1;

	return 0;
}
int CMQOObject::SetColor( char* srcchar, int srcleng )
{
	int pos, stepnum;

	int pointno;
	
	pos = 0;
	CallF( GetInt( &pointno, srcchar, pos, srcleng, &stepnum ), return 1 );

	if( (pointno < 0) || (pointno >= m_vertex) ){
		DbgOut( L"MQOObject : SetColor : pointno error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}


	pos += stepnum;
	DWORD col;
	CallF( GetInt( (int*)&col, srcchar, pos, srcleng, &stepnum ), return 1 );

	DWORD dwr, dwg, dwb;
	dwr = col & 0x000000FF;
	dwg = (col & 0x0000FF00) >> 8;
	dwb = (col & 0x00FF0000) >> 16;

	ChaVector4* dstcol = m_colorbuf + pointno;
	dstcol->x = (float)dwr / 255.0f;
	dstcol->y = (float)dwg / 255.0f;
	dstcol->z = (float)dwb / 255.0f;
	dstcol->w = 1.0f;
	

	return 0;
}

int CMQOObject::MakePolymesh3(bool fbxfileflag, ID3D12Device* pdev, CModel* pmodel)
{
	if( !m_pointbuf || !m_facebuf )
		return 0;

	if( m_pm3 ){
		delete m_pm3;
		m_pm3 = 0;
	}
	if (m_dispobj) {
		delete m_dispobj;
		m_dispobj = nullptr;
	}



	
	if( m_lathe != 0 ){
		CallF( MakeLatheBuf(), return 1 );
	}

	if( m_mirror != 0 ){
		CallF( MakeMirrorBuf(), return 1 );
	}
	

////////


	// 面と頂点の数を取得
	int face_count;
	int vert_count;
	ChaVector3* pointptr;
	CMQOFace* faceptr;
	ChaVector4* colorptr;

	if( m_face2 > 0 ){
		face_count = m_face2;
		faceptr = m_facebuf2;
	}else{
		face_count = m_face;
		faceptr = m_facebuf;
	}
	_ASSERT(faceptr);
	if (!faceptr){
		return 1;
	}

	if( m_vertex2 > 0 ){
		vert_count = m_vertex2;
		pointptr = m_pointbuf2;
	}else{
		vert_count = m_vertex;
		pointptr = m_pointbuf;
	}
	_ASSERT(pointptr);
	if (!pointptr){
		return 1;
	}

	if( m_colorbuf2 ){
		colorptr = m_colorbuf2;
	}else{
		colorptr = m_colorbuf;
	}
	//_ASSERT(colorptr);
	//if (!colorptr){
	//	return 1;
	//}

	m_pm3 = new CPolyMesh3();
	if( !m_pm3 ){
		_ASSERT( 0 );
		return 1;
	}

	if (fbxfileflag == false) {
		//mqofile
		//materialはCModelから引数で受け取ったsrcmaterial
		CallF(m_pm3->CreatePM3(fbxfileflag, vert_count, face_count, m_facet, 
			pointptr, faceptr, pmodel, m_multmat, GetUVNum()), return 1);
	}
	else {
		
		//fbxfile
		//uvはCMQOObjectのm_uvbuf

		if (m_uvbuf) {
			int datano1 = 0;
			int faceno1;
			int trino1;
			for (faceno1 = 0; faceno1 < face_count; faceno1++) {
				CMQOFace* curface = faceptr + faceno1;
				if (curface) {

					curface->SetHasUV(1);

					for (trino1 = 0; trino1 < 3; trino1++) {
						if ((datano1 >= vert_count) || (datano1 >= m_uvleng)) {
							_ASSERT(0);
							break;
						}
						ChaVector2* srcuv = m_uvbuf + faceno1 * 3 + trino1;
						curface->SetUV(trino1, *srcuv);

						if (m_uvbuf1) {
							ChaVector2* srcuv1 = m_uvbuf1 + faceno1 * 3 + trino1;
							curface->SetUV1(trino1, *srcuv1);
						}
					}
				}
			}
		}

		//2023/12/01
		//マテリアル情報のために　pmodelを渡す
		CallF(m_pm3->CreatePM3(fbxfileflag, vert_count, face_count, m_facet, 
			pointptr, faceptr, pmodel, m_multmat, GetUVNum()), return 1);
	}
	
	return 0;
}

int CMQOObject::MakePolymesh4(ID3D12Device* pdev, CModel* pmodel)
{
	if (!pmodel) {
		_ASSERT(0);
		return 1;
	}
	if(!m_pointbuf || !m_facebuf)
		return 0;

	if( m_pm4 ){
		delete m_pm4;
		m_pm4 = 0;
	}
	if (m_dispobj) {
		delete m_dispobj;
		m_dispobj = nullptr;
	}

	m_pm4 = new CPolyMesh4();
	if( !m_pm4 ){
		_ASSERT( 0 );
		return 1;
	}

	CallF( m_pm4->CreatePM4(m_normalmappingmode, m_vertex, m_face, m_normalleng, m_uvleng, m_pointbuf, m_normal, m_uvbuf, m_uvbuf1, m_facebuf, pmodel, GetUVNum()), return 1 );
	
	return 0;
}


int CMQOObject::MakeExtLine(CModel* srcmodel)
{
	if( m_extline ){
		delete m_extline;
		m_extline = 0;
	}

	if( m_displine ){
		delete m_displine;
		m_displine = 0;
	}

	if( !srcmodel || !m_pointbuf || !m_facebuf )
		return 0;



	m_extline = new CExtLine();
	if( !m_extline ){
		_ASSERT( 0 );
		return 1;
	}

	CallF( m_extline->CreateExtLine( srcmodel, m_vertex, m_face, m_pointbuf, m_facebuf, m_color ), return 1 );

	if( m_extline->GetLineNum() <= 0 ){
		delete m_extline;
		m_extline = 0;
	}

	return 0;
}

int CMQOObject::MakeDispObj( ID3D12Device* pdev, int hasbone )
{
	if (m_dispobj) {
		delete m_dispobj;
		m_dispobj = nullptr;
	}
	if( m_displine ){
		delete m_displine;
		m_displine = 0;
	}

	if( !m_pointbuf || !m_facebuf )
		return 0;

	if( (m_vertex <= 0) || (m_face <= 0) ){
		return 0;
	}


	if( m_pm3 && m_pm3->GetCreateOptFlag() ){
		
		m_dispobj = new CDispObj();
		if (!m_dispobj) {
			_ASSERT(0);
			return 1;
		}
		CallF(m_dispobj->CreateDispObj(pdev, m_pm3, hasbone, GetUVNum()), return 1);

		m_pm3->CalcBound();

		if (m_pm3->GetFbxFileFlag()) {//mqoから読み込んだ場合はPickRay()で使うのでDestroyしない
			DestroySystemDispObj();
		}
		m_pm3->DestroySystemDispObj();



	}else if( m_pm4 && m_pm4->GetCreateOptFlag() ){
		if( hasbone ){
			CallF(m_pm4->SetPm3Inf(this), return 1);
		}

		m_dispobj = new CDispObj();
		if (!m_dispobj) {
			_ASSERT(0);
			return 1;
		}
		CallF(m_dispobj->CreateDispObj(pdev, m_pm4, hasbone, GetUVNum()), return 1);


		m_pm4->CalcBound();

		DestroySystemDispObj();
		m_pm4->DestroySystemDispObj();

	}
	else if(m_extline){
		m_displine = new CDispObj();
		if( !m_displine ){
			_ASSERT( 0 );
			return 1;
		}
		CallF( m_displine->CreateDispObj(pdev, m_extline), return 1 );

		m_extline->CalcBound();

		DestroySystemDispObj();
	}

	return 0;
}

int CMQOObject::HasPolygon()
{
	int face2 = 0;
	int face3 = 0;
	int face4 = 0;

	int faceno;

	for( faceno = 0; faceno < m_face; faceno++ ){
		CMQOFace* curface;
		curface = m_facebuf + faceno;
		switch( curface->GetPointNum() ){
			case 2:
				face2++;
				break;
			case 3:
				face3++;
				break;
			case 4:
				face4++;
			default:
				break;
		}
	}
	
	if( m_lathe != 0 ){
		return (face2 + face3 + face4);
	}else{
		return (face3 + face4);
	}
}

int CMQOObject::HasLine()
{
	int face2 = 0;

	int faceno;

	for( faceno = 0; faceno < m_face; faceno++ ){
		CMQOFace* curface;
		curface = m_facebuf + faceno;
		switch( curface->GetPointNum() ){
			case 2:
				face2++;
				break;
			case 3:
			case 4:
			default:
				break;
		}
	}
	
	return face2;
}



int CMQOObject::MakeLatheBuf()
{
	if ((m_face > 0) && !m_facebuf) {
		_ASSERT(0);
		return 1;
	}


	int linenum = 0;
	int faceno;
	CMQOFace* curface;
	for( faceno = 0; faceno < m_face; faceno++ ){
		curface = m_facebuf + faceno;
		if( curface->GetPointNum() == 2 ){
			linenum++;
		}
	}

	if( linenum <= 0 ) //!!!!
		return 0;


	int* lineno2faceno;
	lineno2faceno = (int*)malloc( sizeof( int ) * linenum );
	if( !lineno2faceno ){
		DbgOut( L"MQOObject : MakeLatheBuf : lineno2faceno alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( lineno2faceno, sizeof( int ) * linenum );

	int lineno = 0;
	for( faceno = 0; faceno < m_face; faceno++ ){
		curface = m_facebuf + faceno;
		if( curface->GetPointNum() == 2 ){
			_ASSERT( lineno < linenum );
			*(lineno2faceno + lineno) = faceno; 
			lineno++;
		}
	}

	if( m_pointbuf2 ){
		_ASSERT( 0 );
		return 1;
	}
	m_vertex2 = m_lathe_seg * 2 * linenum;
	m_pointbuf2 = (ChaVector3*)malloc( sizeof( ChaVector3 ) * m_vertex2 );
	if( !m_pointbuf2 ){
		DbgOut( L"MQOObject : MakeLatheBuf : pointbuf2 alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;			
	}
	ZeroMemory( m_pointbuf2, sizeof( ChaVector3 ) * m_vertex2 );

	if( m_colorbuf ){
		if( m_colorbuf2 ){
			_ASSERT( 0 );
			return 1;
		}
		m_colorbuf2 = (ChaVector4*)malloc( sizeof( ChaVector4 ) * m_vertex2 );
		if( !m_colorbuf2 ){
			DbgOut( L"MQOObject : MakeLatheBuf : colorbuf2 alloc error !!!\r\n" );
			_ASSERT( 0 );
			return 1;			
		}
		//ZeroMemory( colorbuf2, sizeof( ARGBF ) * vertex2 );
		MoveMemory( m_colorbuf2, m_colorbuf, sizeof( ChaVector4 ) * m_vertex );
	}

	if( m_facebuf2 ){
		_ASSERT( 0 );
		return 1;
	}
	m_face2 = m_lathe_seg * linenum;
	m_facebuf2 = new CMQOFace[ m_face2 ];
	if( !m_facebuf2 ){
		DbgOut( L"MQOObject : MakeLatheBuf : facebuf2 alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;			
	}
	

	LATHEELEM elem[2];
	float mag, rad;
	int segno;
	rad = (float)(2.0 * PI / (double)m_lathe_seg);

	for( lineno = 0; lineno < linenum; lineno++ ){
		faceno = *(lineno2faceno + lineno);
		int v0, v1;
		v0 = (m_facebuf + faceno)->GetIndex( 0 );
		v1 = (m_facebuf + faceno)->GetIndex( 1 );
		ChaVector3* src0 = m_pointbuf + v0;
		ChaVector3* src1 = m_pointbuf + v1;
		switch( m_lathe_axis ){
		case 0://X
			elem[0].height = src0->x;
			mag = src0->y * src0->y + src0->z * src0->z;
			if( mag > 0.0f ){
				elem[0].dist = (float)sqrt( mag );
			}else{
				elem[0].dist = 0.0f;
			}

			elem[1].height = src1->x;
			mag = src1->y * src1->y + src1->z * src1->z;
			if( mag > 0.0f ){
				elem[1].dist = (float)sqrt( mag );
			}else{
				elem[1].dist = 0.0f;
			}

			for( segno = 0; segno < m_lathe_seg; segno++ ){
				ChaVector3* dst0 = m_pointbuf2 + ((size_t)lineno * 2 * m_lathe_seg + segno);
				ChaVector3* dst1 = dst0 + m_lathe_seg;

				dst0->x = elem[0].height;
				dst0->y = elem[0].dist * (float)cos( rad * segno );
				dst0->z = elem[0].dist * (float)sin( rad * segno );

				dst1->x = elem[1].height;
				dst1->y = elem[1].dist * (float)cos( rad * segno );
				dst1->z = elem[1].dist * (float)sin( rad * segno );

				if( m_colorbuf2 ){
					ChaVector4* dstcol0 = m_colorbuf2 + ((size_t)lineno * 2 * m_lathe_seg + segno);
					ChaVector4* dstcol1 = dstcol0 + m_lathe_seg;
					if (dstcol0 && m_colorbuf) {
						*dstcol0 = *(m_colorbuf + v0);
					}
					else {
						_ASSERT(0);
					}
					if (dstcol1 && m_colorbuf) {
						*dstcol1 = *(m_colorbuf + v1);
					}
					else {
						_ASSERT(0);
					}
				}
			}

			break;
		case 1://Y
			elem[0].height = src0->y;
			mag = src0->x * src0->x + src0->z * src0->z;
			if( mag > 0.0f ){
				elem[0].dist = (float)sqrt( mag );
			}else{
				elem[0].dist = 0.0f;
			}

			elem[1].height = src1->y;
			mag = src1->x * src1->x + src1->z * src1->z;
			if( mag > 0.0f ){
				elem[1].dist = (float)sqrt( mag );
			}else{
				elem[1].dist = 0.0f;
			}

			for( segno = 0; segno < m_lathe_seg; segno++ ){
				ChaVector3* dst0 = m_pointbuf2 + ((size_t)lineno * 2 * m_lathe_seg + segno);
				ChaVector3* dst1 = dst0 + m_lathe_seg;

				dst0->x = elem[0].dist * (float)cos( rad * segno );
				dst0->y = elem[0].height;
				dst0->z = elem[0].dist * (float)sin( rad * segno );

				dst1->x = elem[1].dist * (float)cos( rad * segno );
				dst1->y = elem[1].height;
				dst1->z = elem[1].dist * (float)sin( rad * segno );

				if( m_colorbuf2 ){
					ChaVector4* dstcol0 = m_colorbuf2 + ((size_t)lineno * 2 * m_lathe_seg + segno);
					ChaVector4* dstcol1 = dstcol0 + m_lathe_seg;
					if (dstcol0 && m_colorbuf) {
						*dstcol0 = *(m_colorbuf + v0);
					}
					else {
						_ASSERT(0);
					}
					if (dstcol1 && m_colorbuf) {
						*dstcol1 = *(m_colorbuf + v1);
					}
					else {
						_ASSERT(0);
					}
				}
			}
			break;
		case 2://Z
			elem[0].height = src0->z;
			mag = src0->x * src0->x + src0->y * src0->y;
			if( mag > 0.0f ){
				elem[0].dist = (float)sqrt( mag );
			}else{
				elem[0].dist = 0.0f;
			}

			elem[1].height = src1->z;
			mag = src1->x * src1->x + src1->y * src1->y;
			if( mag > 0.0f ){
				elem[1].dist = (float)sqrt( mag );
			}else{
				elem[1].dist = 0.0f;
			}

			for( segno = 0; segno < m_lathe_seg; segno++ ){
				ChaVector3* dst0 = m_pointbuf2 + ((size_t)lineno * 2 * m_lathe_seg + segno);
				ChaVector3* dst1 = dst0 + m_lathe_seg;

				dst0->x = elem[0].dist * (float)cos( rad * segno );
				dst0->y = elem[0].dist * (float)sin( rad * segno );
				dst0->z = elem[0].height;

				dst1->x = elem[1].dist * (float)cos( rad * segno );
				dst1->y = elem[1].dist * (float)sin( rad * segno );
				dst1->z = elem[1].height;

				if( m_colorbuf2 ){
					ChaVector4* dstcol0 = m_colorbuf2 + ((size_t)lineno * 2 * m_lathe_seg + segno);
					ChaVector4* dstcol1 = dstcol0 + m_lathe_seg;

					if (dstcol0 && m_colorbuf) {
						*dstcol0 = *(m_colorbuf + v0);
					}
					if (dstcol1 && m_colorbuf) {
						*dstcol1 = *(m_colorbuf + v1);
					}
					else {
						_ASSERT(0);
					}
				}
			}
			break;
		default:
			_ASSERT( 0 );
			break;
		}
	}


	for( lineno = 0; lineno < linenum; lineno++ ){ 
		for( segno = 0; segno < m_lathe_seg; segno++ ){
			CMQOFace* dstface = m_facebuf2 + ((size_t)m_lathe_seg * lineno + segno);
			dstface->SetPointNum( 4 );

			if( segno != m_lathe_seg - 1 ){
				dstface->SetIndex( 0, ((size_t)lineno * 2 * m_lathe_seg + segno) );
				dstface->SetIndex( 1, ((size_t)lineno * 2 * m_lathe_seg + m_lathe_seg + segno) );
				dstface->SetIndex( 2, ((size_t)lineno * 2 * m_lathe_seg + m_lathe_seg + 1 + segno) );
				dstface->SetIndex( 3, ((size_t)lineno * 2 * m_lathe_seg + 1 + segno));
			}else{
				dstface->SetIndex( 0, ((size_t)lineno * 2 * m_lathe_seg + segno));
				dstface->SetIndex( 1, ((size_t)lineno * 2 * m_lathe_seg + m_lathe_seg + segno));
				dstface->SetIndex( 2, ((size_t)lineno * 2 * m_lathe_seg + m_lathe_seg));
				dstface->SetIndex( 3, ((size_t)lineno * 2 * m_lathe_seg));
			}
			dstface->SetHasUV( 0 );

		}
	}


	free( lineno2faceno );

	return 0;
}

int CMQOObject::MakeMirrorBuf()
{


	if( m_mirror == 2 ){
		CallF( FindConnectFace( 0 ), return 1 );//connectnumのセット
		if( m_connectnum > 0 ){
			_ASSERT( m_connectface == 0 );
			m_connectface = new CMQOFace[ m_connectnum ];
			if( !m_connectface ){
				DbgOut( L"MQOObject : MakeMirrorBuf : connectface alloc error !!!\r\n" );
				_ASSERT( 0 );
				return 1;
			}

			CallF( FindConnectFace( 1 ), return 1 );//connectfaceのセット
		}

	}else{
		m_connectnum = 0;
		m_connectface = 0;
	}
	

	int doconnect = 0;
	int axis;
	if( axis = (m_mirror_axis & 0x01) ){
		CallF( MakeMirrorPointAndFace( axis, doconnect ), return 1 );
		doconnect = 1;
	}

	if( axis = (m_mirror_axis & 0x02) ){
		CallF( MakeMirrorPointAndFace( axis, doconnect ), return 1 );
		doconnect = 1;
	}

	
	if( axis = (m_mirror_axis & 0x04) ){
		CallF( MakeMirrorPointAndFace( axis, doconnect ), return 1 );
		doconnect = 1;
	}



	return 0;
}

int CMQOObject::MakeMirrorPointAndFace( int axis, int doconnect )
{

	int befvertex, befface;

	if( m_vertex2 > 0 )
		befvertex = m_vertex2;
	else
		befvertex = m_vertex;
	
	if( m_face2 > 0 )
		befface = m_face2;
	else
		befface = m_face;

	m_vertex2 = befvertex * 2;
	
	if( doconnect == 0 )
		m_face2 = befface * 2 + m_connectnum;
	else
		m_face2 = befface * 2;

	if( m_pointbuf2 ){
		ChaVector3* newpb2 = 0;
		newpb2 = (ChaVector3*)realloc( m_pointbuf2, sizeof( ChaVector3 ) * m_vertex2 );
		if( !newpb2 ){
			DbgOut( L"MQOObject : MakeMirrorPointAndFace : pointbuf2 realloc error !!!\r\n" );
			_ASSERT( 0 );
			return 1;
		}
		else {
			m_pointbuf2 = newpb2;
		}
	}else{
		m_pointbuf2 = (ChaVector3*)malloc( sizeof( ChaVector3 ) * m_vertex2 );
		if( !m_pointbuf2 ){
			DbgOut( L"MQOObject : MakeMirrorPointAndFace : pointbuf2 alloc error !!!\r\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( m_pointbuf2, m_pointbuf, sizeof( ChaVector3 ) * befvertex );
	}

	if( m_colorbuf ){
		if( m_colorbuf2 ){
			ChaVector4* newcb2 = 0;
			newcb2 = (ChaVector4*)realloc( m_colorbuf2, sizeof( ChaVector4 ) * m_vertex2 );
			if( !newcb2 ){
				DbgOut( L"MQOObject : MakeMirrorPointAndFace : colorbuf2 realloc error !!!\r\n" );
				_ASSERT( 0 );
				return 1;
			}
			else {
				m_colorbuf2 = newcb2;
			}
		}else{
			m_colorbuf2 = (ChaVector4*)malloc( sizeof( ChaVector4 ) * m_vertex2 );
			if( !m_colorbuf2 ){
				DbgOut( L"MQOObject : MakeMirrorPointAndFace : colorbuf2 alloc error !!!\r\n" );
				_ASSERT( 0 );
				return 1;
			}
			MoveMemory( m_colorbuf2, m_colorbuf, sizeof( ChaVector4 ) * befvertex );
		}
	}

	if( m_facebuf2 ){
		CMQOFace* newface;
		newface = new CMQOFace[ m_face2 ];
		if( !newface ){
			DbgOut( L"MQOObject : MakeMirrorPointAndFace : newface alloc error !!!\r\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( newface, m_facebuf2, sizeof( CMQOFace ) * befface );
		delete [] m_facebuf2;
		m_facebuf2 = newface;

	}else{
		m_facebuf2 = new CMQOFace[ m_face2 ];
		if( !m_facebuf2 ){
			DbgOut( L"MQOObject : MakeMirrorPointAndFace : facebuf2 alloc error !!!\r\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( m_facebuf2, m_facebuf, sizeof( CMQOFace ) * befface );
	}


	int vertno;
	ChaVector3* srcvec;
	ChaVector3* dstvec;
	switch( axis ){
	case 1://X
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srcvec = m_pointbuf2 + vertno;
			dstvec = m_pointbuf2 + vertno + befvertex;
			
			dstvec->x = -srcvec->x;
			dstvec->y = srcvec->y;
			dstvec->z = srcvec->z;
		}
		break;
	case 2://Y
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srcvec = m_pointbuf2 + vertno;
			dstvec = m_pointbuf2 + vertno + befvertex;
			
			dstvec->x = srcvec->x;
			dstvec->y = -srcvec->y;
			dstvec->z = srcvec->z;
		}
		break;
	case 4://Z
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srcvec = m_pointbuf2 + vertno;
			dstvec = m_pointbuf2 + vertno + befvertex;
			
			dstvec->x = srcvec->x;
			dstvec->y = srcvec->y;
			dstvec->z = -srcvec->z;
		}

		break;
	default:
		_ASSERT( 0 );
		break;
	}

	if( m_colorbuf2 ){
		ChaVector4* srccol;
		ChaVector4* dstcol;
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srccol = m_colorbuf2 + vertno;
			dstcol = m_colorbuf2 + vertno + befvertex;
			
			*dstcol = *srccol;
		}
	}


	int faceno;
	CMQOFace* srcface;
	CMQOFace* dstface;
	for( faceno = 0; faceno < befface; faceno++ ){
		srcface = m_facebuf2 + faceno;
		dstface = m_facebuf2 + faceno + befface;

		CallF( dstface->SetInvFace( srcface, befvertex ), return 1 );
	}


	if( (doconnect == 0) && m_connectface ){
		int srcno = 0;
		for( faceno = befface * 2; faceno < m_face2; faceno++ ){
			srcface = m_connectface + srcno;
			dstface = m_facebuf2 + faceno;
			
			dstface->SetPointNum( 4 );
			dstface->SetMaterialNo( srcface->GetMaterialNo() );
			
			dstface->SetIndex( 0, srcface->GetIndex( 0 ) );//反対向きに描画するようにセット。
			dstface->SetIndex( 1, srcface->GetIndex( 0 ) + befvertex );
			dstface->SetIndex( 2, srcface->GetIndex( 1 ) + befvertex );
			dstface->SetIndex( 3, srcface->GetIndex( 1 ) );

			dstface->SetHasUV( srcface->GetHasUV() );

			dstface->SetUV( 0, srcface->GetUV( 0 ) );
			dstface->SetUV( 1, srcface->GetUV( 0 ) );
			dstface->SetUV( 2, srcface->GetUV( 1 ) );
			dstface->SetUV( 3, srcface->GetUV( 1 ) );

			dstface->SetCol( 0, srcface->GetCol( 0 ) );
			dstface->SetCol( 1, srcface->GetCol( 0 ) );
			dstface->SetCol( 2, srcface->GetCol( 1 ) );
			dstface->SetCol( 3, srcface->GetCol( 1 ) );

			srcno++;
		}
	}


	return 0;
}


int CMQOObject::FindConnectFace( int issetface )
{
	if ((m_face > 0) && !m_facebuf) {
		_ASSERT(0);
		return 1;
	}



	int conno = 0;
	int faceno, chkno;
	int pointnum;
	int findflag[4];
	int findall;
	CMQOFace* curface;
	CMQOFace* chkface;
	for( faceno = 0; faceno < m_face; faceno++ ){
		curface = m_facebuf + faceno;
		pointnum = curface->GetPointNum();
		if( pointnum <= 2 )
			continue;

		ZeroMemory( findflag, sizeof( int ) * 4 );

		findall = 0;
		for( chkno = 0; chkno < m_face; chkno++ ){
			if( chkno == faceno )
				continue;
			chkface = m_facebuf + chkno;

			if( chkface->GetPointNum() <= 2 )
				continue;

			CallF( curface->CheckSameLine( chkface, findflag ), return 1 );

			int flagsum = 0;
			int sumno;
			for( sumno = 0; sumno < pointnum; sumno++ ){
				flagsum += findflag[sumno];
			}
			if( flagsum == pointnum ){
				findall = 1;
				break;
			}
		}

		if( findall == 0 ){
			int i;
			for( i = 0; i < pointnum; i++ ){
				if( findflag[i] == 0 ){//共有していない辺
					int distok = 1;
					distok = CheckMirrorDis( m_pointbuf, curface, i, pointnum );
					if( distok ){
						if( issetface ){
							(m_connectface + conno)->SetPointNum( 2 );
							(m_connectface + conno)->SetMaterialNo( curface->GetMaterialNo() );
							(m_connectface + conno)->SetHasUV( curface->GetHasUV() );

							(m_connectface + conno)->SetIndex( 0, curface->GetIndex( i ) );
							(m_connectface + conno)->SetUV( 0, curface->GetUV( i ) );
							if( i != (pointnum - 1) ){
								(m_connectface + conno)->SetIndex( 1, curface->GetIndex( i+1 ) );
								(m_connectface + conno)->SetUV( 1, curface->GetUV( i+1 ) );
							}else{
								(m_connectface + conno)->SetIndex( 1, curface->GetIndex( 0 ) );
								(m_connectface + conno)->SetUV( 1, curface->GetUV( 0 ) );
							}

						}
						conno++;
					}
				}
			}

		}

	}

	if( issetface == 0 ){
		m_connectnum = conno;
	}


	return 0;
}


int CMQOObject::CheckMirrorDis( ChaVector3* pbuf, CMQOFace* fbuf, int lno, int pnum )
{
	if( m_issetmirror_dis == 0 ){
		return 1;
	}else{
		int axisx, axisy, axisz;
		axisx = m_mirror_axis & 0x01;
		axisy = m_mirror_axis & 0x02;
		axisz = m_mirror_axis & 0x04;

		int i0, i1;
		i0 = lno;
		if( lno == pnum - 1 ){
			i1 = 0;
		}else{
			i1 = lno + 1;
		}
		ChaVector3* v0;
		ChaVector3* v1;
		v0 = pbuf + fbuf->GetIndex( i0 );
		v1 = pbuf + fbuf->GetIndex( i1 );

		float distx0, distx1;
		int chkx = 1;
		if( axisx ){
			distx0 = v0->x * 2.0f;
			distx1 = v1->x * 2.0f;

			if( (distx0 > m_mirror_dis) || (distx1 > m_mirror_dis) ){
				chkx = 0;
			}
		}


		float disty0, disty1;
		int chky = 1;
		if( axisy ){
			disty0 = v0->y * 2.0f;
			disty1 = v1->y * 2.0f;

			if( (disty0 > m_mirror_dis) || (disty1 > m_mirror_dis) ){
				chky = 0;
			}
		}


		float distz0, distz1;
		int chkz = 1;
		if( axisz ){
			distz0 = v0->z * 2.0f;
			distz1 = v1->z * 2.0f;

			if( (distz0 > m_mirror_dis) || (distz1 > m_mirror_dis) ){
				chkz = 0;
			}
		}


		if( (chkx == 1) && (chky == 1) && (chkz == 1) ){
			return 1;
		}else{
			return 0;
		}

	}

}

int CMQOObject::MultMat( ChaMatrix multmat )
{
	m_multmat = multmat;
	return 0;

}

int CMQOObject::MultVertex()
{
	int pno;
	for( pno = 0; pno < m_vertex; pno++ ){
		ChaVector3* curp = m_pointbuf + pno;
		ChaVector3 srcv = *curp;

		ChaVector3TransformCoord( curp, &srcv, &m_multmat );

	}
	return 0;
}

int CMQOObject::Multiple( float multiple )
{
	ChaMatrixIdentity( &m_multmat );
	m_multmat.SetScale(ChaVector3(multiple, multiple, -multiple));

	return 0;
}
int CMQOObject::InitFaceDirtyFlag()
{
	int fno;
	CMQOFace* curface;
	for( fno = 0; fno < m_face; fno++ ){
		curface = m_facebuf + fno;

		curface->SetDirtyFlag( 0 );
	}

	return 0;
}

int SortTriLine( const VOID* arg1, const VOID* arg2 )
{
    MQOTRILINE* p1 = (MQOTRILINE*)arg1;
    MQOTRILINE* p2 = (MQOTRILINE*)arg2;
    
    if( p1->leng < p2->leng )
        return -1;
	else if( p1->leng == p2->leng )
		return 0;
	else
		return 1;
}


int CMQOObject::CheckFaceSameChildIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface )
{
	*ppfindface = 0;

	int fno;
	CMQOFace* curface;

	for( fno = 0; fno < m_face; fno++ ){
		curface = m_facebuf + fno;

		if( (curface->GetBoneType() == MIKOBONE_NORMAL) || (curface->GetBoneType() == MIKOBONE_FLOAT) ){
			if( (curface->GetChildIndex() == chkno) && (curface != srcface) ){
				*ppfindface = curface;
				break;
			}
		}
	}

	return 0;
}

int CMQOObject::CheckMaterialSameName( int srcmatno, map<int, CMQOMaterial*> &srcmaterial, int* nameflag )
{
	int fno;
	CMQOFace* curface;

	int samecnt = 0;
	int lrcnt = 0;

	char* nameptr;
	size_t leng;
	int cmp;

	for( fno = 0; fno < m_face; fno++ ){
		curface = m_facebuf + fno;

		if( curface->GetBoneType() == MIKOBONE_NORMAL ){
			if( curface->GetMaterialNo() == srcmatno ){
				samecnt++;

				CMQOMaterial* curmat = GetMaterialFromNo( srcmaterial, srcmatno );
				if( !curmat ){
					DbgOut( L"mqoobject : CheckMaterkalSameName : material %d, name NULL error !!!\r\n", srcmatno );
					_ASSERT( 0 );
					return 1;
				}

				nameptr = (char*)curmat->GetName();
				if (nameptr) {
					*(nameptr + 256 - 1) = 0;
					leng = strlen(nameptr);
					if ((leng > 2) && (leng < 256)) {
						cmp = strcmp(nameptr + leng - 2, "[]");
						if (cmp == 0) {
							lrcnt++;
						}
					}
				}
				else {
					_ASSERT(0);
				}
			}
		}
	}

	if( samecnt == 1 ){
		*nameflag = 0;
	}else if( (samecnt == 2) && (lrcnt == 2) ){
		*nameflag = 1;
	}else if( samecnt >= 2 ){
		*nameflag = 2;
	}else{
		*nameflag = 3;
	}

	return 0;
}


int CMQOObject::IsSameFaceIndex( CMQOFace* face1, CMQOFace* face2 )
{

	if( face1->GetPointNum() != face2->GetPointNum() )
		return 0;


	int pointnum;
	pointnum = face1->GetPointNum();

	int samecnt = 0;
	int i, j;
	int index1, index2;
	for( i = 0; i < pointnum; i++ ){
		index1 = face1->GetIndex( i );

		for( j = 0; j < pointnum; j++ ){
			index2 = face2->GetIndex( j );

			if( index1 == index2 ){
				samecnt++;
			}
		}
	}

	if( samecnt >= pointnum )
		return 1;
	else
		return 0;

}

int CMQOObject::GetMaterialNoInUse( int* noptr, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	if( m_face <= 0 ){
		return 0;
	}

	int findnum = 0;
	int findid[256];
	ZeroMemory( findid, sizeof( int ) * 256 );

	int fno;
	for( fno = 0; fno < m_face; fno++ ){
		CMQOFace* curface = m_facebuf + fno;
		int curmatno = curface->GetMaterialNo();
		if( curmatno >= 0 ){
			int sameflag = 0;
			int chki;
			for( chki = 0; chki < findnum; chki++ ){
				if( findid[chki] == curmatno ){
					sameflag = 1;
					break;
				}
			}

			if( sameflag == 0 ){
				if( findnum >= 256 ){
					DbgOut( L"mqoobj : GetMaterialNoInUse : materialnum too many error !!!\r\n" );
					_ASSERT( 0 );
					return 1;
				}
				findid[findnum] = curmatno;
				findnum++;
			}
		}
	}

	if( noptr ){
		if( arrayleng < findnum ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( noptr, findid, sizeof( int ) * findnum );
	}

	*getnumptr = findnum;

	return 0;
}

int CMQOObject::GetFaceInMaterial( int matno, CMQOFace** ppface, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;
	if( m_face < 0 ){
		return 0;
	}

	int findnum = 0;

	int fno;
	for( fno = 0; fno < m_face; fno++ ){
		CMQOFace* curface = m_facebuf + fno;
		int curmatno = curface->GetMaterialNo();

		if( curmatno == matno ){
			if( ppface ){
				if( arrayleng < findnum ){
					_ASSERT( 0 );
					return 1;
				}
				*( ppface + findnum ) = curface;
			}
			findnum++;
		}
	}

	*getnumptr = findnum;

	return 0;
}

int CMQOObject::CollisionLocal_Ray(ChaVector3 startlocal, ChaVector3 dirlocal, 
	bool excludeinvface)
{
	int face_count;
	int vert_count;
	ChaVector3* pointptr;
	CMQOFace* faceptr;

	if( m_face2 > 0 ){
		face_count = m_face2;
		faceptr = m_facebuf2;
	}else{
		face_count = m_face;
		faceptr = m_facebuf;
	}
	_ASSERT(faceptr);
	if (!faceptr){
		return 0;
	}

	if( m_vertex2 > 0 ){
		vert_count = m_vertex2;
		pointptr = m_pointbuf2;
	}else{
		vert_count = m_vertex;
		pointptr = m_pointbuf;
	}
	_ASSERT(pointptr);
	if (!pointptr){
		return 0;
	}

	int allowrev;
	if (excludeinvface) {
		allowrev = 1;
	}
	else {
		allowrev = 0;
	}

	int fno;
	int hitflag;
	int justflag;
	float justval = 0.01f;
	for( fno = 0; fno < face_count; fno++ ){
		hitflag = 0;
		justflag = 0;
		CMQOFace* curface = faceptr + fno;
		if( curface->GetPointNum() == 3 ){
			hitflag = ChkRay( allowrev, curface->GetIndex( 0 ), curface->GetIndex( 1 ), curface->GetIndex( 2 ), 
				pointptr, startlocal, dirlocal, justval, &justflag );
			if( hitflag || justflag ){
				return 1;
			}
		}else if( curface->GetPointNum() == 4 ){
			hitflag = ChkRay( allowrev, curface->GetIndex( 0 ), curface->GetIndex( 1 ), curface->GetIndex( 2 ), 
				pointptr, startlocal, dirlocal, justval, &justflag );
			if( hitflag || justflag ){
				return 1;
			}
			hitflag = ChkRay( allowrev, curface->GetIndex( 0 ), curface->GetIndex( 2 ), curface->GetIndex( 3 ), 
				pointptr, startlocal, dirlocal, justval, &justflag );
			if( hitflag || justflag ){
				return 1;
			}
		}
	}
	
	return 0;
}



int CMQOObject::AddInfBone( int srcboneno, int srcvno, float srcweight, int isadditive )
{
	CInfBone* ibptr = 0;
	//if (m_pm3) {
	//	//何もしない
	//}
	//if( m_pm3 && m_pm3->GetInfBone() ){
	//	if( (srcvno < 0) || (srcvno >= m_pm3->GetOrgPointNum()) ){
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//	ibptr = m_pm3->GetInfBone() + srcvno;
	//}
	//else 
	if( m_pm4 && m_pm4->GetInfBone() ){
		if( (srcvno < 0) || (srcvno >= m_pm4->GetOrgPointNum()) ){
			_ASSERT( 0 );
			return 1;
		}
		ibptr = m_pm4->GetInfBone() + srcvno;
	}else{
		if (GetName() && (strstr(GetName(), "ND_") == 0)) {//*ND_*の名前は　影響度用のダミーオブジェクト
			_ASSERT(0);
		}
	}
	if( ibptr ){
		INFELEM ie;
		ie.boneno = srcboneno;
		ie.dispinf = srcweight;
		ie.kind = CALCMODE_ONESKIN0;
		ie.orginf = srcweight;
		ie.userrate = 1.0f;
		ie.isadditive = isadditive;

		ibptr->AddInfElem( this, ie );
	}

	return 0;
}

int CMQOObject::NormalizeInfBone()
{
	//if( m_pm3 && m_pm3->GetInfBone() ){
	//	_ASSERT( 0 );
	if(m_pm3){
		//何もしない
	}else if( m_pm4 && m_pm4->GetInfBone() ){
		int vno;
		for( vno = 0; vno < m_pm4->GetOrgPointNum(); vno++ ){
			(m_pm4->GetInfBone() + vno)->NormalizeInf( this );
		}
	}

	return 0;
}

int CMQOObject::UpdateMorphBuffer()
{
	if( !m_pm4 || !m_mpoint || !m_dispobj) {
		_ASSERT( 0 );
		return 1;
	}

	if( m_findshape.empty() ){
		_ASSERT( 0 );
		return 0;
	}

	MoveMemory( m_mpoint, m_pointbuf, sizeof( ChaVector3 ) * m_vertex );

	ChaVector3 diffpoint;
	map<string,float>::iterator itrweight;
	for( itrweight = m_shapeweight.begin(); itrweight != m_shapeweight.end(); itrweight++ ){
		string curname = itrweight->first;

//WCHAR wcurname[256]={0L};
//WCHAR wobjname[256]={0L};
//ZeroMemory( wcurname, sizeof( WCHAR ) * 256 );
//MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)curname.c_str(), 256, wcurname, 256 );
//ZeroMemory( wobjname, sizeof( WCHAR ) * 256 );
//MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)m_name, 256, wobjname, 256 );
//DbgOut( L"updatemorph : objname %s, targetname %s, weight %f\r\n",
//	   wobjname, wcurname, itrweight->second );

		ChaVector3* curshape = m_shapevert[ curname ];
		if( curshape ){
			float curweight = itrweight->second * 0.01f;
			if( curweight != 0.0f ){
				int vno;
				for( vno = 0; vno < m_vertex; vno++ ){
					ChaVector3 targetv = *( curshape + vno );
					ChaVector3 orgv = *(m_pointbuf + vno);
					diffpoint = (targetv - orgv) * curweight;
					*(m_mpoint + vno) += diffpoint;
				}
			}
		}else{
			_ASSERT( 0 );
		}
	}

	CallF( m_pm4->UpdateMorphBuffer( m_mpoint ), return 1 );
	CallF(m_dispobj->CopyDispV(m_pm4), return 1);

	return 0;
}

int CMQOObject::ScaleBtBox(bool setinstancescale, CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr, float* boxzptr )
{
	float sccyli, scsph, scboxz;

	sccyli = boneleng / 200.0f;
	if( sccyli != 0.0f ){
		scsph = reptr->GetSphrate() * sccyli;
		scboxz = reptr->GetBoxzrate() * sccyli;
	}else{
		scsph = 0.0f;
		scboxz = 0.0f;
	}

	*cyliptr = sccyli * 100.0f;
	*sphptr = scsph * 100.0f;
	*boxzptr = scboxz * 100.0f;


	ChaVector3 scale;
	ChaVector3 tra;
	//scale = ChaVector3( scsph, sccyli, scboxz );
	scale = ChaVector3(sccyli, scsph, scboxz);
	tra = ChaVector3( 0.0f, 0.0f, 0.0f );

	if (setinstancescale) {//インスタンシングのデータ順に関わるので必要な時だけ
		MultScale(scale, tra);
	}


	return 0;
}


int CMQOObject::ScaleBtCone(bool setinstancescale, CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr )
{
	float sccyli, scsph;

	sccyli = boneleng / 200.0f;
	if( sccyli != 0.0f ){
		scsph = reptr->GetSphrate() * sccyli;
	}else{
		scsph = 0.0f;
	}

	*cyliptr = sccyli * 200.0f;
	*sphptr = scsph * 100.0f;


	ChaVector3 scale;
	ChaVector3 tra;
	//scale = ChaVector3( scsph, sccyli, scsph );
	scale = ChaVector3(sccyli, scsph, scsph); 
	tra = ChaVector3(0.0f, 0.0f, 0.0f);


	if (setinstancescale) {//インスタンシングのデータ順に関わるので必要な時だけ
		MultScale(scale, tra);
	}


	return 0;
}
int CMQOObject::ScaleBtSphere(bool setinstancescale, CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr )
{
	float sccyli;

	sccyli = boneleng / 200.0f;

	*cyliptr = sccyli * 100.0f;
	*sphptr = sccyli * 100.0f;


	ChaVector3 scale;
	ChaVector3 tra;
	scale = ChaVector3( sccyli, sccyli, sccyli );
	tra = ChaVector3( 0.0f, 0.0f, 0.0f );

	if (setinstancescale) {//インスタンシングのデータ順に関わるので必要な時だけ
		MultScale(scale, tra);
	}

	return 0;
}

int CMQOObject::ScaleBtCapsule(bool setinstancescale, CRigidElem* reptr, float boneleng, int srctype, float* lengptr )
{
	float sccyli, scsph;

	sccyli = boneleng / ( ( 1.0f + reptr->GetSphrate() ) * 200.0f );
	if( sccyli != 0.0f ){
		scsph = reptr->GetSphrate() * sccyli;
	}else{
		scsph = 0.0f;
//		_ASSERT( 0 );
	}

	/*
	ChaVector3 scale;
	ChaVector3 tra;
	if( srctype == 0 ){
		//scale = ChaVector3( scsph, scsph, sccyli );
		scale = ChaVector3( scsph, sccyli, scsph );
		tra = ChaVector3( 0.0f, 0.0f, 0.0f );
		if( lengptr ){
			*lengptr = sccyli * 200.0f;
		}
	}else if( srctype == 1 ){
		scale = ChaVector3( scsph, scsph, scsph );
		tra = ChaVector3( 0.0f, 100.0f * ( sccyli - scsph ), 0.0f );
		if( lengptr ){
			*lengptr = scsph * 100.0f;
		}
	}else{
		scale = ChaVector3( scsph, scsph, scsph );
		tra = ChaVector3( 0.0f, 100.0f * ( scsph - sccyli ), 0.0f );
		if( lengptr ){
			*lengptr = scsph * 100.0f;
		}
	}
	MultScale( scale, tra );
	*/

	ChaVector3 scale;
	ChaVector3 tra;
	if (srctype == 0){
		//cylinder
		//scale = ChaVector3(scsph, sccyli, scsph);
		//tra = ChaVector3(0.0f, 100.0f * (scsph - sccyli), 0.0f);
		scale = ChaVector3(sccyli, scsph, scsph);
		tra = ChaVector3(100.0f * (scsph - sccyli), 0.0f, 0.0f);
		if (lengptr){
			*lengptr = sccyli * 200.0f;
		}
	}
	else if (srctype == 1){
		//upper sphere
		//scale = ChaVector3(scsph, scsph, scsph);
		//tra = ChaVector3(0.0f, -300.0f * scsph + 100.0f * (scsph + 2.0f * sccyli), 0.0f);
		scale = ChaVector3(scsph, scsph, scsph);
		tra = ChaVector3(-300.0f * scsph + 100.0f * (scsph + 2.0f * sccyli), 0.0f, 0.0f);
		if (lengptr){
			*lengptr = scsph * 100.0f;
		}
	}
	else{
		//lower sphere
		scale = ChaVector3(scsph, scsph, scsph);
		tra = ChaVector3(0.0f, 0.0f, 0.0f);
		if (lengptr){
			*lengptr = scsph * 100.0f;
		}
	}

	if (setinstancescale) {//インスタンシングのデータ順に関わるので必要な時だけ
		MultScale(scale, tra);
	}

	return 0;
}


int CMQOObject::MultScale( ChaVector3 srcscale, ChaVector3 srctra )
{
	//インスタンシングのデータ順に関わるので必要な時だけ

	if (m_dispobj) {
		m_dispobj->SetScale(srcscale, srctra);
	}




	//if( m_pm3 ){
	//	m_pm3->MultScale( srcscale, srctra );
	//}
	//if( m_pm4 ){
	//	_ASSERT( 0 );
	//}

	//if( m_dispobj ){
	//	CallF( m_dispobj->CopyDispV( m_pm3 ), return 1 );
	//}

	return 0;
}


int CMQOObject::InitShapeWeight()
{
	m_shapeweight.clear();
	return 0;
}

int CMQOObject::SetShapeWeight( char* nameptr, float srcweight )
{
	m_shapeweight[ nameptr ] = srcweight;
	return 0;
}

int CMQOObject::AddShapeName( char* nameptr )
{
	if( !m_mpoint ){
		m_mpoint = (ChaVector3*)malloc( sizeof( ChaVector3 ) * m_vertex );
		if( !m_mpoint ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_mpoint, sizeof( ChaVector3 ) * m_vertex );
	}

	map<string,int>::iterator itrfind;
	itrfind = m_findshape.find( nameptr );
	if( itrfind == m_findshape.end() ){
		m_findshape[ nameptr ] = 1;

		ChaVector3* newshape = (ChaVector3*)malloc( sizeof( ChaVector3 ) * m_vertex );
		if( !newshape ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( newshape, sizeof( ChaVector3 ) * m_vertex );

		m_shapevert[ nameptr ] = newshape;

	}
	return 0;
}

int CMQOObject::ExistShape( char* nameptr )
{
	int findflag = 0;

	map<string,int>::iterator itrfind;
	itrfind = m_findshape.find( nameptr );
	if( itrfind != m_findshape.end() ){
		return 1;
	}else{
		return 0;
	}
}

int CMQOObject::SetShapeVert( char* nameptr, int vno, ChaVector3 srcv )
{
	ChaVector3* curshape = m_shapevert[ nameptr ];
	if( curshape ){
		if( (vno < 0) || (vno >= m_vertex) ){
			_ASSERT( 0 );
			return 1;
		}
		*(curshape + vno) = srcv;
	}else{
		_ASSERT( 0 );
	}

	return 0;
}

int CMQOObject::ChkInView(ChaMatrix matWorld, ChaMatrix matVP)
{
	MODELBOUND srcmb;
	srcmb.Init();
	ChaMatrix chkmatworld;
	chkmatworld.SetIdentity();


	//2023/10/07
	//現在はmqoに対してはこの関数は呼んでいないが
	//m_pm3とm_extlineが両方存在するデータ在り　(ground2.mqo)
	//最初に　GetDispObj()とGetDispLine()とで場合分けする


	if (GetDispObj()) {
		if (m_pm3) {
			srcmb = m_pm3->GetBound();
			chkmatworld = matWorld;
		}
		else if (m_pm4) {
			srcmb = m_pm4->GetBound();

			//##################################################################
			//2023/08/27
			//hipsが大きく移動すると　視野外になる不具合への対応
			//hipsのworldmatを考慮する
			// 
			//GetHipsBone()はclisterの最初のボーンを親方向へさかのぼって探す
			//##################################################################

			CBone* hipsbone = GetHipsBone();
			if (hipsbone && hipsbone->GetParModel()) {
				int currentmotid = hipsbone->GetParModel()->GetCurrentMotID();
				if (currentmotid > 0) {
					double roundingframe = (double)((int)hipsbone->GetParModel()->GetCurrentFrame());
					ChaMatrix hipsworld = hipsbone->GetWorldMat(g_limitdegflag, currentmotid, roundingframe, 0);
					chkmatworld = hipsworld * matWorld;
				}
				else {
					chkmatworld = matWorld;
				}
			}
			else {
				chkmatworld = matWorld;
			}
		}
		else {
			return 2;
		}
	}
	else if (GetDispLine()) {
		if (m_extline) {
			//2023/10/07
			//extlineは　視野内とする
			m_frustum.SetVisible(true);

			m_frustum.SetInShadow(false);
			return 0;//!!!!!!
		}
		else {
			return 2;
		}
	}
	else {
		_ASSERT(0);
		return 2;
	}


	int lodno = -1;
	if (strstr(GetName(), "LOD0") != 0) {
		lodno = CHKINVIEW_LOD0;
	}
	else if (strstr(GetName(), "LOD1") != 0) {
		lodno = CHKINVIEW_LOD1;
	}
	else if (strstr(GetName(), "LOD2") != 0) {
		lodno = CHKINVIEW_LOD2;
	}

	m_frustum.ChkInView(GetLODNum(), lodno, srcmb, chkmatworld);

	return 0;
}

bool CMQOObject::GetVisible()
{
	bool userflag = GetDispFlag();
	if (userflag == false) {
		return false;//ユーザーが非表示と指定している場合　非表示
	}
	else {
		return m_frustum.GetVisible();//ChkInView()による視錐体判定結果
	}
}
bool CMQOObject::GetInShadow()
{
	bool userflag = GetDispFlag();
	if (userflag == false) {
		return false;//ユーザーが非表示と指定している場合　非表示
	}
	else {
		return m_frustum.GetInShadow();//ChkInView()による視錐体判定結果
	}
}

void CMQOObject::SetInView(bool srcflag) {
	m_frustum.SetVisible(srcflag);
}
void CMQOObject::SetInShadow(bool srcflag) {
	m_frustum.SetInShadow(srcflag);
}

CBone* CMQOObject::GetHipsBone()
{
	//################################################
	//clusterの最初のボーンを親方向へさかのぼって探す
	//################################################

	CBone* hipsbone = 0;

	CBone* firstbone = 0;
	int clusternum = (int)GetClusterSize();
	if (clusternum > 0) {
		firstbone = GetCluster(0);
	}
	else {
		firstbone = 0;
	}

	CBone* chkbone = firstbone;
	while (chkbone) {
		if (chkbone->IsHipsBone()) {
			hipsbone = chkbone;
			break;
		}
		chkbone = chkbone->GetParent(false);
	}

	return hipsbone;
}

int CMQOObject::IncludeTransparent(float multalpha, bool* pfound_noalpha, bool* pfound_alpha)
{
	if (!pfound_noalpha || !pfound_alpha) {
		_ASSERT(0);
		return 1;
	}


	//2023/10/07
	//ExtLine(ground2.mqo)の場合GetPm3()がNULLではない場合があるのでif構成は下記のようになる


	int retresult = 0;

	if (GetDispObj()) {
		if (GetPm3()) {
			retresult = GetPm3()->IncludeTransparent(this, multalpha, pfound_noalpha, pfound_alpha);
		}
		else if (GetPm4()) {
			bool found_alpha = false;
			bool found_noalpha = false;
			int laternum = GetLaterMaterialNum();


			//int materialnum = m_pm4->GetDispMaterialNum();
			//int materialcnt;
			//for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {
			//	CMQOMaterial* curmaterial = nullptr;
			//	int curoffset = 0;
			//	int curtrinum = 0;
			//	int result0 = m_pm4->GetDispMaterial(materialcnt, &curmaterial, &curoffset, &curtrinum);
			//	if ((result0 == 0) && (curmaterial != nullptr) && (curtrinum > 0)) {
			int materialnum = GetOnLoadMaterialSize();
			int matindex;
			for(matindex = 0; matindex < materialnum; matindex++){
				CMQOMaterial* curmaterial = nullptr;
				curmaterial = GetOnLoadMaterialByIndex(matindex);
				if (curmaterial) {

					if (curmaterial->GetTransparent() != 0) {//2023/09/24 VRoidの裾(すそ)透過対策
						found_alpha = true;
					}
					else {
						if ((curmaterial->GetDif4F().w * multalpha) <= 0.99999f) {
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
							LATERMATERIAL chklatermat = GetLaterMaterial(laterno);
							if (chklatermat.pmaterial && (chklatermat.pmaterial == curmaterial)) {
								found_alpha = true;
							}
						}
					}

					if (found_noalpha && found_alpha) {
						break;
					}
				}
			}
			//std::map<int, CMQOMaterial*>::iterator itrmaterial;
			//for (itrmaterial = GetMaterialBegin(); itrmaterial != GetMaterialEnd(); itrmaterial++) {
			//	CMQOMaterial* curmaterial = itrmaterial->second;
			//	if (curmaterial) {

			//		////for debug
			//		//if (curmaterial->GetTex() &&
			//		//	((strstr(curmaterial->GetTex(), "_13.png") != 0) || (strstr(curmaterial->GetTex(), "_15.png") != 0))) {
			//		//	int dbgflag1 = 1;
			//		//}

			//		if (curmaterial->GetTransparent() != 0) {//2023/09/24 VRoidの裾(すそ)透過対策
			//			found_alpha = true;
			//		}
			//		else {
			//			if ((curmaterial->GetDif4F().w * multalpha) <= 0.99999f) {
			//				found_alpha = true;
			//			}
			//			else {
			//				found_noalpha = true;
			//			}
			//		}

			//		//latermaterialチェック
			//		if ((found_alpha == false) && (laternum > 0)) {
			//			int laterno;
			//			for (laterno = 0; laterno < laternum; laterno++) {
			//				LATERMATERIAL chklatermat = GetLaterMaterial(laterno);
			//				if (chklatermat.pmaterial && (chklatermat.pmaterial == curmaterial)) {
			//					found_alpha = true;
			//				}
			//			}
			//		}

			//		if (found_noalpha && found_alpha) {
			//			break;
			//		}
			//	}
			//}
			*pfound_noalpha = found_noalpha;
			*pfound_alpha = found_alpha;
			retresult = 0;
		}
		else {
			*pfound_noalpha = false;
			*pfound_alpha = false;
			retresult = 2;
		}
	}
	else if (GetDispLine()) {
		if (GetExtLine()) {
			bool found_alpha = false;
			bool found_noalpha = false;

			if ((GetExtLine()->GetColor().w * multalpha) > 0.99999f) {
				found_noalpha = true;
			}
			else {
				found_alpha = true;
			}

			*pfound_noalpha = found_noalpha;
			*pfound_alpha = found_alpha;

			retresult = 0;
		}
		else {
			*pfound_noalpha = false;
			*pfound_alpha = false;
			retresult = 2;
		}
	}
	else {
		*pfound_noalpha = false;
		*pfound_alpha = false;
		retresult = 2;
	}

	return retresult;
}


int CMQOObject::MakeLaterMaterial(vector<string> latername)
{

	m_latermaterial.clear();



	if (m_pm4) {

		//##############################################################
		//laternameに格納されている順番で格納するようにループして調べる
		//##############################################################

		int materialnum = m_pm4->GetDispMaterialNum();
		if (materialnum > 0) {
			int latermatnum = (int)latername.size();
			int laterindex; 
			for (laterindex = 0; laterindex < latermatnum; laterindex++) {

				int materialcnt2;
				for (materialcnt2 = 0; materialcnt2 < materialnum; materialcnt2++) {
					CMQOMaterial* curmat = NULL;
					int curoffset = 0;
					int curtrinum = 0;
					int result0 = m_pm4->GetDispMaterial(materialcnt2, &curmat, &curoffset, &curtrinum);
					if ((result0 == 0) && (curmat != NULL) && (curtrinum > 0)) {
						if (curmat->GetTex() && (strlen(curmat->GetTex()) > 0)) {
							if (strcmp(curmat->GetTex(), latername[laterindex].c_str()) == 0) {
								LATERMATERIAL latermaterial;
								latermaterial.Init();
								latermaterial.pmaterial = curmat;
								latermaterial.offset = curoffset;
								latermaterial.trinum = curtrinum;
								m_latermaterial.push_back(latermaterial);
							}
						}
					}
				}
			}
		}
	}
	else if (m_pm3) {

		//##############################################################
		//laternameに格納されている順番で格納するようにループして調べる
		//##############################################################

		int latermatnum = (int)latername.size();
		int laterindex;
		for (laterindex = 0; laterindex < latermatnum; laterindex++) {

			int blno2;
			for (blno2 = 0; blno2 < m_pm3->GetOptMatNum(); blno2++) {
				MATERIALBLOCK* currb = m_pm3->GetMatBlock() + blno2;

				int curnumprim;
				curnumprim = currb->endface - currb->startface + 1;
				int curoffset;
				curoffset = currb->startface * 3;

				CMQOMaterial* curmat;
				curmat = currb->mqomat;
				if (curmat && curmat->GetTex() && (strlen(curmat->GetTex()) > 0)) {
					if (strcmp(curmat->GetTex(), latername[laterindex].c_str()) == 0) {
						LATERMATERIAL latermaterial;
						latermaterial.Init();
						latermaterial.pmaterial = curmat;
						latermaterial.offset = curoffset;
						latermaterial.trinum = curnumprim;
						m_latermaterial.push_back(latermaterial);
					}
				}
			}
		}
	}
	else {
		return 0;
	}

	return 0;
}

bool CMQOObject::ExistInLaterMaterial(CMQOMaterial* srcmat)
{
	int latermatnum = GetLaterMaterialNum();
	int laterindex;
	for (laterindex = 0; laterindex < latermatnum; laterindex++) {
		LATERMATERIAL chkmat = GetLaterMaterial(laterindex);
		if (srcmat && chkmat.pmaterial && (chkmat.pmaterial == srcmat)) {
			return true;//found !!!
		}
	}

	return false;
}


CMQOMaterial* CMQOObject::GetOnLoadMaterialByMaterialNo(int srcno) {
	int materialnum = GetOnLoadMaterialSize();
	int matindex;
	for (matindex = 0; matindex < materialnum; matindex++) {
		CMQOMaterial* curmqomat = nullptr;
		curmqomat = m_onloadmaterial[matindex];
		if (curmqomat && (curmqomat->GetMaterialNo() == srcno)) {
			return curmqomat;
		}
	}
	return nullptr;
};
CMQOMaterial* CMQOObject::GetOnLoadMaterialByIndex(int srcindex) {
	if ((srcindex >= 0) && (srcindex < GetOnLoadMaterialSize())) {
		return m_onloadmaterial[srcindex];
	}
	else {
		return nullptr;
	}
};
CMQOMaterial* CMQOObject::GetOnLoadMaterialByName(const char* srcname) {

	CMQOMaterial* retmqomat = nullptr;

	if (srcname && srcname[0]) {
		int materialnum = GetOnLoadMaterialSize();
		int matindex;
		for (matindex = 0; matindex < materialnum; matindex++) {
			CMQOMaterial* curmqomat = nullptr;
			curmqomat = m_onloadmaterial[matindex];
			if (curmqomat && (strcmp(curmqomat->GetName(), srcname) == 0)) {
				retmqomat = curmqomat;
				break;
			}
		}
	}
	else {
		retmqomat = nullptr;
	}
	return retmqomat;
};

