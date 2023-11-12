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
#include <crtdbg.h>

#include <ExtLine.h>
#include <mqoface.h>


CExtLine::CExtLine()
{
	InitParams();
}
CExtLine::~CExtLine()
{
	DestroyObjs();
}

int CExtLine::InitParams()
{
	m_linenum = 0;
	m_linev = 0;
	m_color = ChaVector4( 1.0f, 1.0f, 1.0f, 1.0f );

	m_pointnum = 0;
	m_facenum = 0;

	m_pointptr = 0;
	m_faceptr = 0;

	return 0;
}
int CExtLine::DestroyObjs()
{
	if( m_linev ){
		free( m_linev );
		m_linev = 0;
	}
	m_linenum = 0;

	m_pointnum = 0;
	m_facenum = 0;

	m_pointptr = 0;
	m_faceptr = 0;

	return 0;
}


int CExtLine::CreateExtLine( int pointnum, int facenum, ChaVector3* pointptr, CMQOFace* faceptr, ChaVector4 srccol )
{
	m_pointptr = pointptr;
	m_faceptr = faceptr;
	m_color = srccol;
	m_pointnum = pointnum;
	m_facenum = facenum;

	if (!m_pointptr || !m_faceptr){
		_ASSERT(0);
		return 1;
	}

	int lnum = 0;
	CallF( CreateBuffer( 0, 0, &lnum ), return 1 );

	if( lnum <= 0 ){
		m_linenum = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!
	}

	m_linev = (EXTLINEV*)malloc( sizeof( EXTLINEV ) * lnum * 2 );
	if( !m_linev ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_linev, sizeof( EXTLINEV ) * lnum * 2 );

	int chknum = 0;
	CallF( CreateBuffer( m_linev, lnum, &chknum ), return 1 );

	if( lnum != chknum ){
		_ASSERT( 0 );
		return 1;
	}

	m_linenum = lnum;

	return 0;
}

int CExtLine::CreateBuffer( EXTLINEV* lineptr, int arrayleng, int* setnum )
{
	*setnum = 0;

	int fno;
	for( fno = 0; fno < m_facenum; fno++ ){
		CMQOFace* curface = m_faceptr + fno;
		if( curface->GetPointNum() == 2 ){
			if( lineptr ){
				if( *setnum >= arrayleng ){
					DbgOut( L"extline : CreateBuffer : arrayleng error !!!\r\n" );
					_ASSERT( 0 );
					return 1;
				}
				(lineptr + ((size_t)(*setnum) * 2))->pos.x = ( m_pointptr + curface->GetIndex( 0 ) )->x;
				(lineptr + ((size_t)(*setnum) * 2))->pos.y = ( m_pointptr + curface->GetIndex( 0 ) )->y;
				(lineptr + ((size_t)(*setnum) * 2))->pos.z = ( m_pointptr + curface->GetIndex( 0 ) )->z;
				(lineptr + ((size_t)(*setnum) * 2))->pos.w = 1.0f;

				(lineptr + ((size_t)(*setnum) * 2 + 1))->pos.x = ( m_pointptr + curface->GetIndex( 1 ) )->x;
				(lineptr + ((size_t)(*setnum) * 2 + 1))->pos.y = ( m_pointptr + curface->GetIndex( 1 ) )->y;
				(lineptr + ((size_t)(*setnum) * 2 + 1))->pos.z = ( m_pointptr + curface->GetIndex( 1 ) )->z;
				(lineptr + ((size_t)(*setnum) * 2 + 1))->pos.w = 1.0f;
			}

			(*setnum)++;
		}
	}

	return 0;
}

int CExtLine::CalcBound()
{
	if( m_linenum <= 0 ){
		m_bound.min = ChaVector3( 0.0f, 0.0f, 0.0f );
		m_bound.max = ChaVector3( 0.0f, 0.0f, 0.0f );
		m_bound.center = ChaVector3( 0.0f, 0.0f, 0.0f );
		m_bound.r = 0.0f;
		return 0;
	}


	m_bound.min = *m_pointptr;
	m_bound.max = *m_pointptr;

	int vno;
	for( vno = 1; vno < m_pointnum; vno++ ){
		ChaVector3 curv = *( m_pointptr + vno );

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

