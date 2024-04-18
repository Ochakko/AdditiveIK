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

#define DBGH
#include <dbg.h>

#include <EditRange.h>

#include <GlobalVar.h>

using namespace std;



//double CEditRange::s_applyrate = 100.0;


CEditRange::CEditRange()
{
	InitParams();
}
CEditRange::~CEditRange()
{
	DestroyObjs();
}

int CEditRange::InitParams()
{
	m_setflag = 0;
	m_setcnt = 0;
	m_ki.clear();
	m_keynum = 0;
	m_startframe = 0.0;
	m_endframe = 0.0;
	m_applyframe = 0.0;
	return 0;
}
int CEditRange::DestroyObjs()
{
	m_ki.clear();
	return 0;
}

int CEditRange::Clear()
{
	InitParams();
	return 0;
}

int CEditRange::SetRangeOne(double srcframe)
{
	m_ki.clear();

	m_startframe = (double)((int)(srcframe + 0.1));
	m_endframe = (double)((int)(srcframe + 0.1));
	return 0;
}


int CEditRange::SetRange( std::list<KeyInfo> srcki, double srcframe )
{
	double minframe = 1e10;
	double maxframe = -1e10;

	m_ki.clear();
	m_ki = srcki;

	int num = 0;
	list<KeyInfo>::iterator itrki;
	for( itrki = m_ki.begin(); itrki != m_ki.end(); itrki++ ){
		if( itrki->lineIndex == 1 ){
			double curframe = itrki->time;
			if( minframe > curframe ){
				minframe = curframe;
			}
			if( maxframe < curframe ){
				maxframe = curframe;
			}
			num++;
		}
	}

	m_keynum = num;
	if( m_keynum >= 1 ){
		m_startframe = (double)((int)(minframe + 0.1));
		m_endframe = (double)((int)(maxframe + 0.1));
	}else{
		m_startframe = (double)((int)(srcframe + 0.1));
		m_endframe = (double)((int)(srcframe + 0.1));
	}

	return 0;
}
int CEditRange::GetRange( int* numptr, double* startptr, double* endptr )
{
	*numptr = m_keynum;
	*startptr = m_startframe;
	*endptr = m_endframe;
	return 0;
}

int CEditRange::GetRange( int* numptr, double* startptr, double* endptr, double* applyptr )
{
	*numptr = m_keynum;
	*startptr = m_startframe;
	*endptr = m_endframe;

	//double offset = 0;
	double offset = 0.49;
	//if ((s_applyrate != 0.0) && (m_startframe != m_endframe)){
	//	offset = 1.0;
	//}

	if (g_applyrate == 0.0) {
		m_applyframe = m_startframe;
	}
	else if (g_applyrate == 100.0) {
		m_applyframe = m_endframe;
	}
	else {
		m_applyframe = (double)((int)(m_startframe + (m_endframe - m_startframe) * (g_applyrate / 100.0)));
	}
	
	*applyptr = m_applyframe;

	return 0;
}

CEditRange CEditRange::operator= (CEditRange srcrange)
{
	//m_setflagとm_setcntはここではコピーしない。

	m_ki = srcrange.m_ki;
	m_keynum = srcrange.m_keynum;
	m_startframe = srcrange.m_startframe;
	m_endframe = srcrange.m_endframe;
	m_applyframe = srcrange.m_applyframe;

	//static double s_applyrate

	return *this;
}
