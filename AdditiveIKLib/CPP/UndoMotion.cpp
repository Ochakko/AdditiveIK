#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <UndoMotion.h>

#include <Model.h>
#include <Bone.h>

#define DBGH
#include <dbg.h>

#include <MotionPoint.h>
//#include <quaternion.h>
//#include <VecMath.h>
#include <ChaVecCalc.h>
#include <EditRange.h>


#include <string>


using namespace std;

//#include <MorphKey.h>


extern LONG g_bvh2fbxbatchflag;
//extern LONG g_motioncachebatchflag;
extern LONG g_retargetbatchflag;



CUndoMotion::CUndoMotion()
{
	InitParams();
}
CUndoMotion::~CUndoMotion()
{
	DestroyObjs();
}
int CUndoMotion::InitParams()
{
	m_validflag = 0;
	ZeroMemory( &m_savemotinfo, sizeof( MOTINFO ) );
	m_savemotinfo.motid = -1;

	m_bone2mp.clear();
	m_base2mk.clear();
	m_bone2limit.clear();

	m_curboneno = -1;
	m_curbaseno = -1;

	m_bonemotmark.clear();

	m_keynum = 1;
	m_startframe = 1.0;
	m_endframe = 1.0;
	m_applyrate = 50.0;

	m_brushstate.Init();

	return 0;
}
int CUndoMotion::ClearData()
{
	DestroyObjs();
	InitParams();

	return 0;
}

int CUndoMotion::DestroyObjs()
{
	map<CBone*, CMotionPoint*>::iterator itrb2mp;
	for( itrb2mp = m_bone2mp.begin(); itrb2mp != m_bone2mp.end(); itrb2mp++ ){
		CMotionPoint* mpptr = itrb2mp->second;
		CMotionPoint* nextmp = 0;
		while( mpptr ){
			nextmp = mpptr->GetNext();
			//delete mpptr;
			CMotionPoint::InvalidateMotionPoint(mpptr);
			mpptr = nextmp;
		}
	}
	m_bone2mp.clear();


	m_bone2limit.clear();

/***
	map<CMQOObject*, CMorphKey*>::iterator itrb2mk;
	for( itrb2mk = m_base2mk.begin(); itrb2mk != m_base2mk.end(); itrb2mk++ ){
		CMorphKey* mkptr = itrb2mk->second;
		CMorphKey* nextmk = 0;
		while( mkptr ){
			nextmk = mkptr->m_next;
			delete mkptr;
			mkptr = nextmk;
		}
	}
	m_base2mk.clear();
***/

	m_bonemotmark.clear();

	return 0;
}


int CUndoMotion::SaveUndoMotion(bool LimitDegCheckBoxFlag, bool limitdegflag, CModel* pmodel, int curboneno, int curbaseno,
	CEditRange* srcer, double srcapplyrate, BRUSHSTATE srcbrushstate, bool allframeflag)
{
	if (!pmodel) {
		return 2;
	}

	MOTINFO* curmi = pmodel->GetCurMotInfo();
	if( !curmi ){
		return 2;
	}

	if( pmodel->GetCurMotInfo()->motid < 0 ){
		return 2;
	}
	//if( pmodel->GetBoneListSize()<= 0 ){
	if (pmodel->GetBoneForMotionSize() <= 0) {
		return 2;
	}

	if (!srcer) {
		return 2;
	}


	m_brushstate = srcbrushstate;


	//if (g_bvh2fbxbatchflag || g_motioncachebatchflag || g_retargetbatchflag) {
	//if ((InterlockedAdd(&g_bvh2fbxbatchflag, 0) != 0) && (InterlockedAdd(&g_motioncachebatchflag, 0) != 0) && (InterlockedAdd(&g_retargetbatchflag, 0) != 0)) {
	if ((InterlockedAdd(&g_bvh2fbxbatchflag, 0) != 0) && (InterlockedAdd(&g_retargetbatchflag, 0) != 0)) {
		return 2;
	}


	//ClearData();

	int curmotid = curmi->motid;

	if (LimitDegCheckBoxFlag == false) {//2023/10/27 1.2.0.27 RC5 : LimitDegCheckBoxFlag == true時　つまり　LimitEulボタンのオンオフ時はモーションの保存をスキップ

		map<int, CBone*>::iterator itrbone;
		for (itrbone = pmodel->GetBoneListBegin(); itrbone != pmodel->GetBoneListEnd(); itrbone++) {
			CBone* curbone = itrbone->second;
			//_ASSERT( curbone );
			if (curbone && (curbone->IsSkeleton())) {

				//####################
				// ANGLELIMIT of bone
				//####################

				int getchkflag = 1;
				m_bone2limit[curbone] = curbone->GetAngleLimit(limitdegflag, getchkflag);

				//###################
				// first MotionPoint
				//###################
				double roundingstartframe, roundingendframe;
				if (allframeflag == true) {
					roundingstartframe = 1.0;
					roundingendframe = RoundingTime(curmi->frameleng) - 1.0;
				}
				else {
					roundingstartframe = RoundingTime(srcer->GetStartFrame());
					roundingendframe = RoundingTime(srcer->GetEndFrame());
				}


				CMotionPoint* firstsrcmp = curbone->GetMotionPoint(curmotid, roundingstartframe);
				//CMotionPoint* firstsrcmp = curbone->GetMotionPoint(curmotid, 0.0);
				if (!firstsrcmp) {
					//_ASSERT(0);
					return 2;
				}
				CMotionPoint* firstundomp = 0;
				map<CBone*, CMotionPoint*>::iterator itrbone2mp;
				itrbone2mp = m_bone2mp.find(curbone);
				if (itrbone2mp != m_bone2mp.end()) {
					firstundomp = itrbone2mp->second;
				}
				if (!firstundomp) {
					firstundomp = CMotionPoint::GetNewMP();
					if (!firstundomp) {
						_ASSERT(0);
						SetValidFlag(0);
						return 1;
					}
					m_bone2mp[curbone] = firstundomp;
				}
				firstundomp->CopyMP(firstsrcmp);
				firstundomp->SetUndoValidFlag(1);

				//######################
				// followed MotionPoint
				//######################

				CMotionPoint* befundomp = firstundomp;

				double currenttime;
				CMotionPoint* srcmp = firstsrcmp;
				CMotionPoint* undomp = firstundomp;


				for (currenttime = (roundingstartframe + 1.0); currenttime <= roundingendframe; currenttime += 1.0) {
					//for (currenttime = 1.0; currenttime < (pmodel->GetCurMotInfo()->frameleng - 1.0); currenttime += 1.0) {
					srcmp = curbone->GetMotionPoint(curmotid, (double)((int)(currenttime + 0.1)));
					undomp = 0;
					if (srcmp && befundomp) {
						undomp = befundomp->GetNext();
						if (!undomp) {
							undomp = CMotionPoint::GetNewMP();
							if (!undomp) {
								_ASSERT(0);
								SetValidFlag(0);
								return 1;
							}
							befundomp->AddToNext(undomp);
						}
						undomp->CopyMP(srcmp);
						undomp->SetUndoValidFlag(1);
					}
					else {
						_ASSERT(0);
						SetValidFlag(0);
						return 1;
					}

					befundomp = undomp;
				}

				if (undomp) {
					undomp = undomp->GetNext();
					while (undomp) {
						undomp->SetUndoValidFlag(0);
						undomp = undomp->GetNext();
					}
				}

				map<double, int> tmpmap;
				curbone->GetMotMarkOfMap2(curmotid, tmpmap);
				if ((int)tmpmap.size() > 0) {
					(m_bonemotmark[curbone]).clear();
					m_bonemotmark[curbone] = tmpmap;
				}
				else {
					(m_bonemotmark[curbone]).clear();
				}
			}
		}
	}
	else {

		//##########################################################################################################################
		//2023/10/27 1.2.0.27 RC5 : LimitDegCheckBoxFlag == true時　つまり　LimitEulボタンのオンオフ時はモーションの保存をスキップ
		// 
		//undomp->SetValidFlag(0)をする
		//
		//##########################################################################################################################
		map<int, CBone*>::iterator itrbone;
		for (itrbone = pmodel->GetBoneListBegin(); itrbone != pmodel->GetBoneListEnd(); itrbone++) {
			CBone* curbone = itrbone->second;
			//_ASSERT( curbone );
			if (curbone && (curbone->IsSkeleton())) {

				//####################
				// ANGLELIMIT of bone
				//####################
				int getchkflag = 1;
				m_bone2limit[curbone] = curbone->GetAngleLimit(limitdegflag, getchkflag);

				CMotionPoint* firstundomp = 0;
				map<CBone*, CMotionPoint*>::iterator itrbone2mp;
				itrbone2mp = m_bone2mp.find(curbone);
				if (itrbone2mp != m_bone2mp.end()) {
					firstundomp = itrbone2mp->second;
				}
				if (firstundomp) {
					CMotionPoint* undomp = firstundomp;
					while (undomp) {
						undomp->SetUndoValidFlag(0);
						undomp = undomp->GetNext();
					}
				}
			}
		}
	}
/***
	map<int, CMQOObject*>::iterator itrbase;
	for( itrbase = pmodel->m_mbaseobject.begin(); itrbase != pmodel->m_mbaseobject.end(); itrbase++ ){
		CMQOObject* curbase = itrbase->second;
		_ASSERT( curbase );
		CMorphKey* firstmk = curbase->m_morphkey[ curmotid ];
		if( firstmk ){
			CMorphKey* undofirstmk = new CMorphKey( curbase );
			if( !undofirstmk ){
				_ASSERT( 0 );
				return 1;
			}

			undofirstmk->CopyMotion( firstmk );

			CMorphKey* curmk = firstmk->m_next;
			CMorphKey* befundomk = undofirstmk;
			while( curmk ){
				CMorphKey* newundomk = new CMorphKey( curbase );
				if( !newundomk ){
					_ASSERT( 0 );
					return 1;
				}
				newundomk->CopyMotion( curmk );

				befundomk->AddToNext( newundomk );

				befundomk = newundomk;
				curmk = curmk->m_next;
			}
			m_base2mk[ curbase ] = undofirstmk;
		}
	}
***/
	::MoveMemory( &m_savemotinfo, pmodel->GetCurMotInfo(), sizeof( MOTINFO ) );

	m_curboneno = curboneno;
	m_curbaseno = curbaseno;

	if (srcer) {
		double tmpapplyframe;
		srcer->GetRange(&m_keynum, &m_startframe, &m_endframe, &tmpapplyframe);
		m_applyrate = srcapplyrate;
	}
	else {
		m_keynum = 1;
		m_startframe = 1.0;
		m_endframe = 1.0;
		m_applyrate = 50.0;
	}

	m_validflag = 1;

	return 0;
}
int CUndoMotion::RollBackMotion(bool limitdegflag, CModel* pmodel, int* curboneno, int* curbaseno, double* dststartframe, double* dstendframe, double* dstapplyrate, BRUSHSTATE* dstbrushstate)
{
	if( m_validflag != 1 ){
		_ASSERT( 0 );
		return 2;
	}
	if (!pmodel) {
		_ASSERT(0);
		return 2;
	}
	if (!curboneno) {
		_ASSERT(0);
		return 2;
	}
	if (!curbaseno) {
		_ASSERT(0);
		return 2;
	}
	if (!dststartframe)
	{
		_ASSERT(0);
		return 2;
	}
	if (!dstendframe) {
		_ASSERT(0);
		return 2;
	}
	if (!dstapplyrate) {
		_ASSERT(0);
		return 2;
	}
	if (!dstbrushstate) {
		_ASSERT(0);
		return 2;
	}

	*dstbrushstate = m_brushstate;

	int setmotid = m_savemotinfo.motid;
	MOTINFO* chkmotinfo = pmodel->GetMotInfo( setmotid );
	if( !chkmotinfo ){
		_ASSERT( 0 );
		SetValidFlag(0);//!!!!!!!!!!!!!!!
		return 1;
	}

	//::MoveMemory(chkmotinfo, &m_savemotinfo, sizeof(MOTINFO));
	//pmodel->SetCurMotInfo(chkmotinfo);
	//pmodel->SetCurrentMotion(setmotid);

	/*
/////// destroy
	map<int, CBone*>::iterator itrbone;
	for( itrbone = pmodel->GetBoneListBegin(); itrbone != pmodel->GetBoneListEnd(); itrbone++ ){
		CBone* curbone = itrbone->second;
		_ASSERT( curbone );
		if (curbone){
			curbone->DestroyMotionKey(setmotid);
			curbone->ClearMotMarkOfMap2(setmotid);
		}
	}
	*/

///////// set
	map<int, CBone*>::iterator itrbone;
	for (itrbone = pmodel->GetBoneListBegin(); itrbone != pmodel->GetBoneListEnd(); itrbone++){
		CBone* curbone = itrbone->second;
		_ASSERT( curbone );

		if (curbone && (curbone->IsSkeleton())) {

			//######################
			//2023/02/04
			//ANGLELIMIT of CBone
			//######################

			map<CBone*, ANGLELIMIT>::iterator itrbone2limit;
			itrbone2limit = m_bone2limit.find(curbone);
			if (itrbone2limit != m_bone2limit.end()) {
				curbone->SetAngleLimit(limitdegflag, itrbone2limit->second);
			}

			//#######################
			//CMotionPoint of CBone
			//#######################

			CMotionPoint* srcmp = m_bone2mp[curbone];
			CMotionPoint* befdstmp = 0;
			while (srcmp && (srcmp->GetUndoValidFlag() == 1)) {
				double srcframe = srcmp->GetFrame();
				CMotionPoint* dstmp = curbone->GetMotionPoint(setmotid, (double)((int)(srcframe + 0.1)));

				//モーションが長くなる場合
				if (!dstmp) {
					dstmp = CMotionPoint::GetNewMP();
					if (!dstmp) {
						_ASSERT(0);
						SetValidFlag(0);
						return 1;
					}
					if (befdstmp) {
						befdstmp->AddToNext(dstmp);
					}
				}

				dstmp->CopyMP(srcmp);
				srcmp = srcmp->GetNext();
				befdstmp = dstmp;
			}
			curbone->SetMotMarkOfMap2(setmotid, m_bonemotmark[curbone]);
		}

		//if (curbone){
		//	CMotionPoint* srcmp = m_bone2mp[curbone];
		//	CMotionPoint* dstmp = curbone->GetMotionKey(setmotid);
		//	while (srcmp && (srcmp->GetUndoValidFlag() == 1) && dstmp){
		//		dstmp->CopyMP(srcmp);

		//		srcmp = srcmp->GetNext();
		//		dstmp = dstmp->GetNext();
		//	}

		//	//curbone->SetMotionKey(setmotid, undofirstmp);
		//	curbone->SetMotMarkOfMap2(setmotid, m_bonemotmark[curbone]);
		//}
	}


	//モーションが短くなった場合に対応
	double oldleng = chkmotinfo->frameleng;
	double newleng = m_savemotinfo.frameleng;
	if (oldleng > newleng) {
		map<int, CBone*>::iterator itrbone2;
		for (itrbone2 = pmodel->GetBoneListBegin(); itrbone2 != pmodel->GetBoneListEnd(); itrbone2++) {
			CBone* curbone = itrbone2->second;
			if (curbone && (curbone->IsSkeleton())) {
				curbone->DeleteMPOutOfRange(setmotid, newleng - 1.0);
			}
		}
	}


	MoveMemory( chkmotinfo, &m_savemotinfo, sizeof( MOTINFO ) );
	pmodel->SetCurMotInfo( chkmotinfo );


	*curboneno = m_curboneno;
	*curbaseno = m_curbaseno;


	*dststartframe = m_startframe;
	*dstendframe = m_endframe;
	*dstapplyrate = m_applyrate;

	return 0;
}

