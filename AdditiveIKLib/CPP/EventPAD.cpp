#include "stdafx.h" //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <EventPad.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CEventPad::CEventPad()
{
	InitParams();
}
CEventPad::~CEventPad()
{
	DestroyObjs();
}

int CEventPad::InitParams()
{
	ZeroMemory( m_epad, sizeof( EPAD ) * MOA_PADNUM );
	m_padnum = 0;
	m_savemotid = 0;
	m_saveframeno = 0;

	return 0;
}
int CEventPad::DestroyObjs()
{
	return 0;
}
int CEventPad::AddEPad( EPAD srcek )
{
	if( m_padnum >= MOA_PADNUM ){
		DbgOut( L"ek : AddEpad : padnum overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	EPAD* dstek = m_epad + m_padnum;
	*dstek = srcek;
	dstek->validflag = 1;

	m_padnum++;

	return 0;
}
int CEventPad::DelEPadByIndex( int srcindex )
{
	if( srcindex < 0 ){
		ZeroMemory( m_epad, sizeof( EPAD ) * MOA_PADNUM );
		m_padnum = 0;
	}else{
		m_epad[srcindex].validflag = 0;

		EPAD saveek[MOA_PADNUM];
		ZeroMemory( saveek, sizeof( EPAD ) * MOA_PADNUM );

		int setno = 0;
		int kno;
		for( kno = 0; kno < m_padnum; kno++ ){
			if( kno != srcindex ){
				saveek[setno] = m_epad[kno];
				setno++;
			}
		}
		if( setno != (m_padnum - 1) ){
			DbgOut( L"ek : DelEPadByIndex : setno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ZeroMemory( m_epad, sizeof( EPAD ) * MOA_PADNUM );
		MoveMemory( m_epad, saveek, sizeof( EPAD ) * (m_padnum - 1) );

		m_padnum--;
	}

	return 0;
}

int CEventPad::GetEventNo( int srcpad, int srccnt )
{
	int findevent = 0;

	int kindex;
	for( kindex = 0; kindex < m_padnum; kindex++ ){
		if( (m_epad[kindex].validflag == 1) && (m_epad[kindex].pad == srcpad) ){
			if( m_epad[kindex].singleevent == 0 ){
				if( srccnt != 0 ){
					findevent = m_epad[kindex].eventno;
					break;
				}
			}else{
				if( srccnt == 1 ){
					findevent = m_epad[kindex].eventno;
					break;
				}
			}
		}
	}
	return findevent;
}

int CEventPad::CheckSamePad( int srcpad, int* indexptr )
{
	*indexptr = -1;
	int isfind = 0;

	int kindex;
	for( kindex = 0; kindex < m_padnum; kindex++ ){
		if( (m_epad[kindex].validflag == 1) && (m_epad[kindex].pad == srcpad) ){
			isfind = 1;
			*indexptr = kindex;
			break;
		}
	}
	return isfind;
}

