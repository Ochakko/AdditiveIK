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

CTexBank::CTexBank( ID3D12Device* pdev )
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
	m_texmap.clear();
	return 0;
}
int CTexBank::DestroyObjs()
{
	//Invalidate( INVAL_ALL );
	//
	map<int,CTexElem*>::iterator itr;
	for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
		CTexElem* delte = itr->second;
		if( delte ){
			delete delte;
		}
	}
	//for (auto& it : m_texmap) {
	//	it.second.reset();
	//}
	m_texmap.clear();

	return 0;
}

CTexElem* CTexBank::ExistTex( const WCHAR* srcpath, const WCHAR* srcname, int srctransparent )
{
	auto finditr = m_texmap.end();

	for (auto& it : m_texmap) {
		//CTexElem* curelem = it.second.get();
		CTexElem* curelem = it.second;
		if (curelem) {
			int cmpname, cmppath;
			cmpname = wcscmp(srcname, curelem->GetName());
			if (cmpname == 0) {
				cmppath = wcscmp(srcpath, curelem->GetPath());
				if (cmppath == 0) {
					if (srctransparent == curelem->GetTransparent()) {
						return curelem;
					}
				}
			}
		}
	}
	return nullptr;
}

//int CTexBank::AddTex( WCHAR* srcpath, WCHAR* srcname, int srctransparent, int srcpool, D3DXCOLOR* srccol, int* dstid )
int CTexBank::AddTex(const WCHAR* srcpath, const WCHAR* srcname, int srctransparent, int srcpool, int* dstid)
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
	//CallF( newelem->CreateTexData( m_pdev, pRenderContext), return 1 );


	//if (wcsstr(srcname, L"_14.png")) {
	//	int dbgflag1 = 1;
	//}



	int result1 = newelem->CreateTexData(m_pdev);
	if (result1 == 0) {
		newelem->SetValidFlag(true);
	}
	else {
		newelem->SetValidFlag(false);
	}

	if (m_texmap.find(newelem->GetID()) == m_texmap.end()) {
		//m_texmap.insert(
		//	std::pair<int, TexElemPtr>(newelem->GetID(), newelem)
		//);
		m_texmap[newelem->GetID()] = newelem;
	}
	else {
		_ASSERT(0);
	}
	//m_texmap[newelemptr->GetID()] = newelemptr;

	*dstid = newelem->GetID();


	return 0;
}

//int CTexBank::Invalidate( int invalmode )
//{
//	for (auto& it : m_texmap) {
//		CTexElem* curelem = it.second.get();
//		if (curelem) {
//			if (curelem && (invalmode == INVAL_ALL) || ((invalmode == INVAL_ONLYDEFAULT) && (curelem->GetPool() == 0))) {
//				curelem->InvalidateTexData();
//			}
//		}
//	}
//
//
//	//map<int,CTexElem*>::iterator itr;
//	//for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
//	//	CTexElem* delelem = itr->second;
//	//	if( delelem && (invalmode == INVAL_ALL) || ((invalmode == INVAL_ONLYDEFAULT) && (delelem->GetPool() == 0)) ){
//	//		delelem->InvalidateTexData();
//	//	}
//	//}
//
//	return 0;
//}
//int CTexBank::Restore(RenderContext* pRenderContext)
//{
//
//	for (auto& it : m_texmap) {
//		CTexElem* telem = it.second.get();
//		if (telem) {
//			if (telem && telem->IsValid()) {
//				int result1 = telem->CreateTexData(m_pdev);
//				if (result1 != 0) {
//					telem->SetValidFlag(false);
//				}
//			}
//		}
//	}
//
//	//map<int,CTexElem*>::iterator itr;
//	//for( itr = m_texmap.begin(); itr != m_texmap.end(); itr++ ){
//	//	CTexElem* telem = itr->second;
//	//	if( telem && telem->IsValid()){
//	//		int result1 = telem->CreateTexData(m_pdev);
//	//		if (result1 != 0) {
//	//			telem->SetValidFlag(false);
//	//		}
//	//	}
//	//}
//
//	return 0;
//}

//int CTexBank::DestroyTex( int srcid )
//{
//	map<int,CTexElem*>::iterator finditr;
//	finditr = m_texmap.find( srcid );
//	if( finditr == m_texmap.end() ){
//		return 0;
//	}
//
//	if( finditr->second ){
//		delete finditr->second;
//	}
//	m_texmap.erase( finditr );
//
//	return 0;
//}
//
