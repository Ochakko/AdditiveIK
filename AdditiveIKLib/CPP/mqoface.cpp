#include "stdafx.h"
//#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <mqoface.h>

typedef struct tag_mqotriline
{
	int index[2];
	float leng;
	int sortno;
} MQOTRILINE;

static int SortTriLine( const VOID* arg1, const VOID* arg2 );
static int s_allocno = 0;

////////////////////////////////


CMQOFace::CMQOFace()
{
	InitParams();
	m_serialno = s_allocno++;

}
CMQOFace::~CMQOFace()
{


}

void CMQOFace::InitParams()
{
	m_pointnum = 0;
	ZeroMemory( m_index, sizeof( int ) * 4 );
	m_materialno = -1;

	m_hasuv = 0;
	ZeroMemory(m_uv, sizeof(ChaVector2) * 4);
	ZeroMemory(m_uv1, sizeof(ChaVector2) * 4);

	m_dirtyflag = 0;


	int i;
	for( i = 0; i < 4; i++ ){
		m_col[i] = 0xFFFFFFFF;
	}


	m_bonetype = MIKOBONE_NONE;
	m_parentindex = -1;
	m_childindex = -1;
	m_hindex = -1;

	m_parent = 0;
	m_child = 0;
	m_brother = 0;

	ZeroMemory( m_bonename, sizeof( char ) * 256 );

	m_vcolsetflag = 0;

	m_shapeno = -1;
}

int CMQOFace::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
	}


	return 0;
}

int CMQOFace::GetI64( __int64* dsti64, char* srcchar, int pos, int srcleng, int* stepnum )
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

		//*dstuint = (unsigned int)atol( tempchar );
		//*dstlong = atol( tempchar );
		*dsti64 = _atoi64( tempchar );

		*stepnum = endpos - pos;

//DbgOut( L"check !!! : mqoface : GetUInt : tempchar %s, dsti64 %d\r\n", tempchar, *dsti64 );


	}else{
		_ASSERT( 0 );
	}

	return 0;
}


int CMQOFace::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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
	}


	return 0;
}

int CMQOFace::SetParams( char* srcchar, int srcleng )
{

	int pos, stepnum;
	char* find;

//pointnum
	CallF( GetInt( &m_pointnum, srcchar, 0, srcleng, &stepnum ), return 1 );

	if( (m_pointnum < 0) || (m_pointnum > 4) ){
		DbgOut( L"MQOFace : SetParams : pointnum error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}

//index
	char patv[] = "V(";
	size_t patvleng = strlen(patv);
	find = strstr( srcchar, patv );
	if( !find ){
		DbgOut( L"MQOFace : SetParams : find patv error !!!\r\n" );
		_ASSERT( 0 );
		return 1;		
	}
	pos = (int)( find + patvleng - srcchar );

	int i;
	for( i = 0; i < m_pointnum; i++ ){
		CallF( GetInt( m_index + i, srcchar, pos, srcleng, &stepnum ), return 1 );
		pos += stepnum;
		if( (i != m_pointnum - 1) && (pos >= srcleng) ){
			DbgOut( L"MQOFace : SetParams : pos error !!!\r\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
//materialno
	char patm[] = "M(";
	size_t patmleng = strlen( patm );
	find = strstr( srcchar, patm );
	if( find ){	
		pos = (int)( find + patmleng - srcchar );

		CallF( GetInt( &m_materialno, srcchar, pos, srcleng, &stepnum ), return 1 );
	}
//uv
	char patuv[] = "UV(";
	size_t patuvleng = strlen( patuv );
	find = strstr( srcchar, patuv );
	if( find ){
		m_hasuv = 1;//!!!!!!!!!

		pos = (int)( find + patuvleng - srcchar );
		
		for( i = 0; i < m_pointnum; i++ ){
			CallF( GetFloat( &(m_uv[i].x), srcchar, pos, srcleng, &stepnum ), return 1 );

			pos += stepnum;
			if( pos >= srcleng ){
				DbgOut( L"MQOFace : SetParams : pos error !!!\r\n" );
				_ASSERT( 0 );
				return 1;
			}

			CallF( GetFloat( &(m_uv[i].y), srcchar, pos, srcleng, &stepnum ), return 1 );

			pos += stepnum;
			if( (i != m_pointnum - 1) && (pos >= srcleng) ){
				DbgOut( L"MQOFace : SetParams : pos error !!!\r\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}

//col
	char patcol[] = "COL(";
	size_t patcolleng = strlen( patcol );
	find = strstr( srcchar, patcol );

	if( find ){
		pos = (int)( find + patcolleng - srcchar );

		int i2;
		for( i2 = 0; i2 < m_pointnum; i2++ ){
			CallF( GetI64( m_col + i2, srcchar, pos, srcleng, &stepnum ), return 1 );

			pos += stepnum;
			if( (i2 != m_pointnum - 1) && (pos >= srcleng) ){
				DbgOut( L"MQOFace : SetParams : pos error !!!\r\n" );
				_ASSERT( 0 );
				return 1;
			}

//DbgOut( L"check !!! mqoface : %d, %d\r\n", i2, col[i2] );


		}
		m_vcolsetflag = 1;

	}else{
		m_vcolsetflag = 0;
	}

	return 0;
}


int CMQOFace::Dump()
{
	DbgOut( L"\t\t%d V(", m_pointnum );
	int i;
	for( i = 0; i < m_pointnum; i++ ){
		DbgOut( L" %d ", m_index[i] );
	}
	DbgOut( L")" );

	if( m_materialno >= 0 ){
		DbgOut( L" M(%d) ", m_materialno );
	}

	if( m_hasuv == 1 ){
		DbgOut( L"UV( " );

		for( i = 0; i < m_pointnum; i++ ){
			DbgOut( L"%f %f ", m_uv[i].x, m_uv[i].y );
		}

		DbgOut( L")" );
	}

	DbgOut( L"\r\n" );

	return 0;
}

int CMQOFace::CheckSameLine( CMQOFace* chkface, int* findflag )
{
	int pno;
	for( pno = 0; pno < m_pointnum; pno++ ){
		int v0, v1;
		v0 = m_index[pno];
		if( pno != m_pointnum - 1 )
			v1 = m_index[pno + 1];
		else
			v1 = m_index[0];

		int chknum = chkface->m_pointnum;
		int chkno;
		for( chkno = 0; chkno < chknum; chkno++ ){
			int chkv0, chkv1;
			chkv0 = chkface->m_index[chkno];
			if( chkno != chknum - 1 )
				chkv1 = chkface->m_index[chkno + 1];
			else
				chkv1 = chkface->m_index[0];

			if( ((v0 == chkv0) && (v1 == chkv1))
			|| ((v0 == chkv1) && (v1 == chkv0)) ){
				*(findflag + pno) = 1;
				break;
			}
		}
	}

	return 0;
}

int CMQOFace::SetInvFace( CMQOFace* srcface, int offset )
{
	m_materialno = srcface->m_materialno;
	m_pointnum = srcface->m_pointnum;
	int i;
	for( i = 0; i < m_pointnum; i++ ){
		m_index[i] = srcface->m_index[ m_pointnum - 1 - i ] + offset;
	}
	m_hasuv = srcface->m_hasuv;
	for( i = 0; i < m_pointnum; i++ ){
		m_uv[i] = srcface->m_uv[ m_pointnum - 1 - i ];
		m_uv1[i] = srcface->m_uv1[m_pointnum - 1 - i];
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

//int CMQOFace::SetMikoBoneName( char* srcname, int lrflag )
//{
//	int leng1;
//
//	char LRpat[20] = "[]";
//	char Lpat[20] = "[L]";
//	char Rpat[20] = "[R]";
//
//	if( lrflag == 0 ){
//		leng1 = (int)strlen( srcname );
//		if( leng1 >= 256 ){
//			DbgOut( L"mqoface : SetMikoBoneName : name leng too long error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//
//		strcpy_s( m_bonename, 256, srcname );
//
//	}else if( lrflag == 1 ){
//		leng1 = (int)strlen( srcname );
//		
//		if( (leng1 - 2 + 3) >= 256 ){
//			DbgOut( L"mqoface : SetMikoBoneName : name leng too long error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//
//		strncpy_s( m_bonename, 256, srcname, ((size_t)leng1 - 2) );
//		m_bonename[leng1 - 2] = 0;
//		strcat_s( m_bonename, 256, Lpat );
//
//
//	}else if( lrflag == 2 ){
//		leng1 = (int)strlen( srcname );
//		
//		if( (leng1 - 2 + 3) >= 256 ){
//			DbgOut( L"mqoface : SetMikoBoneName : name leng too long error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//
//		strncpy_s( m_bonename, 256, srcname, ((size_t)leng1 - 2) );
//		m_bonename[leng1 - 2] = 0;
//		strcat_s( m_bonename, 256, Rpat );
//
//	}else{
//		_ASSERT( 0 );
//
//		leng1 = (int)strlen( srcname );
//		if( leng1 >= 256 ){
//			DbgOut( L"mqoface : SetMikoBoneName : name leng too long error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//
//		strcpy_s( m_bonename, 256, srcname );
//	}
//
//
//	return 0;
//}

int CMQOFace::CheckLRFlag( ChaVector3* pointptr, int* lrflagptr )
{
	if( (m_bonetype != MIKOBONE_NORMAL) && (m_bonetype != MIKOBONE_FLOAT) ){
		DbgOut( L"mqoface : CheckLRFlag : mikobonetype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_parentindex < 0) || (m_childindex < 0) ){
		DbgOut( L"mqoface : CheckLRFlag : index not set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ChaVector3* parp;
	ChaVector3* chilp;
	float centerx;

	parp = pointptr + m_parentindex;
	chilp = pointptr + m_childindex;

	centerx = ( parp->x + chilp->x ) * 0.5f;

	if( centerx == 0.0f ){
		*lrflagptr = 0;
	}else if( centerx > 0.0f ){
		*lrflagptr = 1;
	}else{
		*lrflagptr = 2;
	}

	return 0;
}



