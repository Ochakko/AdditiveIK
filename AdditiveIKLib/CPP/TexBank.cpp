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


#include <TexBank.h>
#include <TexElem.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;


extern WCHAR g_basedir[ MAX_PATH ];

CTexBank::CTexBank( ID3D11Device* pdev )
{
	InitParams();
	m_pdev = pdev;
}

CTexBank::~CTexBank()
{
	DestroyObjs();
}

int CTexBank::InitParams()
{
	m_pdev = 0;
	return 0;
}
int CTexBank::DestroyObjs()
{
	Invalidate( INVAL_ALL );

	map<int,CTexElem*>::iterator itr;
	for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
		CTexElem* delte = itr->second;
		if( delte ){
			delete delte;
		}
	}

	m_texmap.clear();

	return 0;
}

CTexElem* CTexBank::ExistTex( const WCHAR* srcpath, const WCHAR* srcname, int srctransparent )
{
	map<int,CTexElem*>::iterator finditr = m_texmap.end();

	map<int,CTexElem*>::iterator itr;
	for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
		CTexElem* curelem = itr->second;
		if( curelem ){
			int cmpname, cmppath;
			cmpname = wcscmp( srcname, curelem->GetName() );
			if( cmpname == 0 ){
				cmppath = wcscmp( srcpath, curelem->GetPath() );
				if( cmppath == 0 ){
					if( srctransparent == curelem->GetTransparent() ){
						return curelem;
					}
				}
			}
		}
	}

	return 0;
}

//int CTexBank::AddTex( WCHAR* srcpath, WCHAR* srcname, int srctransparent, int srcpool, D3DXCOLOR* srccol, int* dstid )
int CTexBank::AddTex(ID3D11DeviceContext* pd3dImmediateContext,const WCHAR* srcpath, const WCHAR* srcname, int srctransparent, int srcpool, int* dstid)
{
	*dstid = -1;

	const WCHAR* lasten = wcsrchr(srcname, TEXT('\\'));
	const WCHAR* ptex = 0;
	if (!lasten) {
		lasten = wcsrchr(srcname, TEXT('/'));
	}
	if (lasten) {
		ptex = (lasten + 1);
	}
	else {
		ptex = srcname;
	}


	CTexElem* sameelem = 0;
	//sameelem = ExistTex( srcpath, srcname, srctransparent );
	sameelem = ExistTex(srcpath, ptex, srctransparent);
	if( sameelem ){
		*dstid = sameelem->GetID();
		return 2;
	}


	CTexElem* newelem;
	newelem = new CTexElem();
	if( !newelem ){
		_ASSERT( 0 );
		return 1;
	}



	//newelem->SetName( srcname );
	newelem->SetName(ptex);//2023/08/29
	newelem->SetPath( srcpath );
	newelem->SetTransparent( srctransparent );
	newelem->SetPool( srcpool );
	//if (srccol) {
	//	newelem->SetTransCol(*srccol);
	//}
	//CallF( newelem->CreateTexData( m_pdev, pd3dImmediateContext), return 1 );


	//if (wcsstr(srcname, L"_14.png")) {
	//	int dbgflag1 = 1;
	//}



	int result1 = newelem->CreateTexData(m_pdev, pd3dImmediateContext);
	if (result1 == 0) {
		newelem->SetValidFlag(true);
	}
	else {
		newelem->SetValidFlag(false);
	}
	m_texmap[newelem->GetID()] = newelem;
	*dstid = newelem->GetID();


	return 0;
}

int CTexBank::Invalidate( int invalmode )
{
	map<int,CTexElem*>::iterator itr;
	for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
		CTexElem* delelem = itr->second;
		if( delelem && (invalmode == INVAL_ALL) || ((invalmode == INVAL_ONLYDEFAULT) && (delelem->GetPool() == 0)) ){
			delelem->InvalidateTexData();
		}
	}

	return 0;
}
int CTexBank::Restore(ID3D11DeviceContext* pd3dImmediateContext)
{
	map<int,CTexElem*>::iterator itr;
	for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
		CTexElem* telem = itr->second;
		if( telem && telem->IsValid()){
			int result1 = telem->CreateTexData( m_pdev, pd3dImmediateContext);
			if (result1 != 0) {
				telem->SetValidFlag(false);
			}
		}
	}

	return 0;
}
int CTexBank::DestroyTex( int srcid )
{
	map<int,CTexElem*>::iterator finditr;
	finditr = m_texmap.find( srcid );
	if( finditr == m_texmap.end() ){
		return 0;
	}

	if( finditr->second ){
		delete finditr->second;
	}
	m_texmap.erase( finditr );

	return 0;
}

