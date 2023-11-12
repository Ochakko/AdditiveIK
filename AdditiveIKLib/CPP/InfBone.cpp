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

#include <InfBone.h>

#include <polymesh3.h>

#include <GetMaterial.h>

#include <mqoface.h>

#define DBGH
#include <dbg.h>

#include <InfScope.h>

using namespace std;


CInfBone::CInfBone()
{
	InitParams();
}

CInfBone::~CInfBone()
{
	DestroyObjs();
}

int CInfBone::InitParams()
{
	m_infdata.clear();
/***
	m_infnum = 0;

	int infno;
	for( infno = 0; infno < INFNUMMAX; infno++ ){
		INFELEM* curie = m_infelem + infno;

		curie->boneno = -1;
		curie->kind = CALCMODE_NONE;
		curie->userrate = 1.0f;
		curie->orginf = 0.0f;
		curie->dispinf = 0.0f;
	}
***/
	return 0;
}

int CInfBone::DestroyObjs()
{
	map<CMQOObject*, INFDATA*>::iterator itrinf;
	for( itrinf = m_infdata.begin(); itrinf != m_infdata.end(); itrinf++ ){
		INFDATA* curinf;
		curinf = itrinf->second;
		if( curinf ){
			free( curinf );
		}
	}
	m_infdata.clear();

	return 0;
}

int CInfBone::InitElem( INFDATA* infptr )
{
	infptr->m_infnum = 0;
	int ieno;
	for( ieno = 0; ieno < INFNUMMAX; ieno++ ){
		INFELEM* curie = infptr->m_infelem + ieno;

		curie->boneno = -1;
		curie->kind = CALCMODE_NONE;
		curie->userrate = 1.0f;
		curie->orginf = 0.0f;
		curie->dispinf = 0.0f;
	}

	return 0;
}

int CInfBone::ExistBone( CMQOObject* srcobj, int srcclusterno )
{
	INFDATA* pinf = GetInfData(srcobj);
	if (pinf){
		int ieno;
		for (ieno = 0; ieno < pinf->m_infnum; ieno++){
			if (pinf->m_infelem[ieno].boneno == srcclusterno){
				return ieno;
			}
		}
	}
	else{
		return -1;
	}

	return -1;
}

int CInfBone::AddInfElem( CMQOObject* srcobj, INFELEM srcie )
{
	if( srcie.isadditive == 1 ){
		srcie.dispinf = srcie.orginf;
	}

	INFDATA* curinf = GetInfData(srcobj);
	if( !curinf ){
		curinf = (INFDATA*)malloc( sizeof( INFDATA ) );
		if (curinf) {
			InitElem(curinf);
			curinf->m_infnum = 1;
			curinf->m_infelem[0] = srcie;
			m_infdata[srcobj] = curinf;
			return 0;//
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	int findieno = -1;
	int ieno;
	for( ieno = 0; ieno < curinf->m_infnum; ieno++ ){
		if( curinf->m_infelem[ ieno ].boneno == srcie.boneno ){
			findieno = ieno;
			break;
		}
	}

	if( findieno >= 0 ){
		
		curinf->m_infelem[ findieno ] = srcie;

	}else{
		if( srcie.isadditive == 1 ){
			curinf->m_infelem[ 0 ] = srcie;//Å‰‚Ì—v‘f‚Éã‘‚«IIadditive‚ÍRigidSkin‚ÌŽž‚ÉŽg‚í‚ê‚éBÅŒã‚É‘«‚³‚ê‚é‚à‚Ì‚Ì‰e‹¿‚ð‚P‚É‚·‚éB
			curinf->m_infnum = 1;
		}else{
			if( curinf->m_infnum >= INFNUMMAX ){
				int minindex = 0;
				float minweight = curinf->m_infelem[0].dispinf;
				int ieno2;
				for( ieno2 = 1; ieno2 < curinf->m_infnum; ieno2++ ){
					if( curinf->m_infelem[ieno2].dispinf < minweight ){
						minindex = ieno2;
						minweight = curinf->m_infelem[ieno2].dispinf;
					}
				}

				if( minweight < srcie.dispinf ){
					curinf->m_infelem[ minindex ] = srcie;
				}
			}else{
				curinf->m_infelem[ curinf->m_infnum ] = srcie;
				(curinf->m_infnum)++;
			}
		}
	}

	return 0;
}
int CInfBone::NormalizeInf( CMQOObject* srcobj )
{
	INFDATA* curinf = GetInfData(srcobj);
	if( !curinf ){
		return 0;
	}

	if( curinf->m_infnum <= 0 ){
//		_ASSERT( 0 );
		return 0;
	}

	if( curinf->m_infnum == 1 ){
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if( curinf->m_infelem[ 0 ].isadditive != 1 ){
			curinf->m_infelem[ 0 ].orginf = 1.0f;
			curinf->m_infelem[ 0 ].dispinf = 1.0f;
		}else{
			curinf->m_infelem[ 0 ].dispinf = curinf->m_infelem[ 0 ].orginf;
		}
		return 0;
	}


	float leng = 0.0f;
	int ieno;
	for( ieno = 0; ieno < curinf->m_infnum; ieno++ ){
		leng += curinf->m_infelem[ ieno ].orginf;
	}

	if( leng != 0.0f ){
		for( ieno = 0; ieno < curinf->m_infnum; ieno++ ){
			if( curinf->m_infelem[ ieno ].isadditive != 1 ){
				curinf->m_infelem[ ieno ].dispinf = curinf->m_infelem[ ieno ].orginf / leng;
			}else{
				curinf->m_infelem[ ieno ].dispinf = curinf->m_infelem[ ieno ].orginf;
			}
		}
	}

	return 0;
}


INFDATA* CInfBone::GetInfData(CMQOObject* srcobj)
{
	if ((int)m_infdata.size() <= 0){
		return 0;
	}

	map<CMQOObject*, INFDATA*>::iterator itrinf;
	itrinf = m_infdata.find(srcobj);
	if (itrinf == m_infdata.end()){
		return 0;
	}
	else{
		return itrinf->second;
	}
}
