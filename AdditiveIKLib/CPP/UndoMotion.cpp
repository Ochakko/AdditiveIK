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

#include <ChaScene.h>

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

	m_undoselect.Init();

	ZeroMemory( &m_savemotinfo, sizeof( MOTINFO ) );
	m_savemotinfo.motid = -1;
	ZeroMemory(&m_savecameramotinfo, sizeof(MOTINFO));
	m_savecameramotinfo.motid = -1;

	m_bone2mp.clear();
	m_base2mk.clear();
	m_bone2limit.clear();

	m_selectedboneno = 0;
	m_curbaseno = -1;

	m_bonemotmark.clear();

	m_keynum = 1;

	m_brushstate.Init();
	m_undocamera.Init();
	m_undomotid.Init();


	m_blendshapeelem.Init();
	m_blendshapeweight.clear();


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
	m_blendshapeweight.clear();

	return 0;
}


int CUndoMotion::SaveUndoMotion(UNDOSELECT srcundoselect,
	bool LimitDegCheckBoxFlag, bool limitdegflag, CModel* pmodel, 
	int selectedboneno, int curbaseno,
	int srcedittarget,//アプリケーションのedittargetモード
	bool undocameraflag,//カメラアニメのUndoとして呼び出す場合にtrue
	CEditRange* srcer, double srcapplyrate,
	BRUSHSTATE srcbrushstate, UNDOCAMERA srcundocamera, 
	CBlendShapeElem srcblendshapeelem,
	bool allframeflag)
{

	SetValidFlag(0);

	//##############################################
	//pmodelは　カメラのUndo時には　cameramodelが渡される
	//##############################################
	if (!pmodel) {
		return 2;
	}


	//2024/06/25
	m_undoselect = srcundoselect;
	m_blendshapeelem = srcblendshapeelem;

	CModel* pcameramodel = nullptr;

	m_undomotid.Init();
	m_undomotid.bonemotid = pmodel->GetCurrentMotID();
	m_undomotid.curmotid = m_undomotid.bonemotid;

	if (!undocameraflag) {

		pcameramodel = srcundocamera.cameramodel;
		if (pcameramodel) {//2024/06/27
			m_undomotid.cameramotid = pcameramodel->GetCameraMotionId();
		}

		if (m_undomotid.bonemotid <= 0) {
			return 2;
		}
		MOTINFO chkmi = pmodel->GetMotInfo(m_undomotid.bonemotid);
		if (chkmi.motid <= 0) {
			return 2;
		}
	}
	else {
		pcameramodel = pmodel;
		m_undomotid.cameramotid = pcameramodel->GetCameraMotionId();

		int chkcameramotion = m_undomotid.cameramotid;
		if (!pcameramodel->IsCameraMotion(chkcameramotion)) {
			return 2;
		}
	}


	//if( opemodel->GetBoneListSize()<= 0 ){
	if (pmodel->GetBoneForMotionSize() <= 0) {
		return 2;
	}

	if (!srcer) {
		return 2;
	}


	m_brushstate = srcbrushstate;
	m_undocamera = srcundocamera;
	m_edittarget = srcedittarget;

	//if (g_bvh2fbxbatchflag || g_motioncachebatchflag || g_retargetbatchflag) {
	//if ((InterlockedAdd(&g_bvh2fbxbatchflag, 0) != 0) && (InterlockedAdd(&g_motioncachebatchflag, 0) != 0) && (InterlockedAdd(&g_retargetbatchflag, 0) != 0)) {
	if ((InterlockedAdd(&g_bvh2fbxbatchflag, 0) != 0) && (InterlockedAdd(&g_retargetbatchflag, 0) != 0)) {
		return 2;
	}


	if (m_undoselect.undokind == UNDOKIND_EDITMOTION) {

		//ClearData();

		int curmotid;
		//if (srcedittarget != EDITTARGET_CAMERA) {
		if (!undocameraflag) {
			curmotid = m_undomotid.bonemotid;
		}
		else {
			curmotid = m_undomotid.cameramotid;
		}
		//m_undomotid.curmotid = curmotid;

		bool cameraanimflag = pmodel->IsCameraMotion(curmotid);


		if (LimitDegCheckBoxFlag == false) {//2023/10/27 1.2.0.27 RC5 : LimitDegCheckBoxFlag == true時　つまり　LimitEulボタンのオンオフ時はモーションの保存をスキップ

			map<int, CBone*>::iterator itrbone;
			for (itrbone = pmodel->GetBoneListBegin(); itrbone != pmodel->GetBoneListEnd(); itrbone++) {
				CBone* curbone = itrbone->second;
				//_ASSERT( curbone );

				bool opeflag = false;
				//if (!cameraanimflag && curbone && (curbone->IsSkeleton() || (curbone->IsNull() && !curbone->IsNullAndChildIsCamera()))) {
				if (!cameraanimflag && curbone && curbone->IsSkeleton()) {
					opeflag = true;
				}
				else if (cameraanimflag && curbone && (curbone->IsCamera() || curbone->IsNullAndChildIsCamera())) {
					opeflag = true;
				}
				else {
					opeflag = false;
					continue;//!!!!!!!!!!!!!!!!
				}

				//if (curbone && (curbone->IsSkeleton() || curbone->IsNullAndChildIsCamera() || curbone->IsCamera())) {
				if (opeflag) {

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
						roundingendframe = RoundingTime(pmodel->GetCurrentMaxFrame());
					}
					else {
						roundingstartframe = RoundingTime(srcer->GetStartFrame());
						roundingendframe = RoundingTime(srcer->GetEndFrame());
					}


					CMotionPoint* firstsrcmp = curbone->GetMotionPoint(curmotid, roundingstartframe);
					//CMotionPoint* firstsrcmp = curbone->GetMotionPoint(curmotid, 0.0);
					//if (!firstsrcmp) {//2024/06/26 １つのfbxに複数のカメラノードと複数のカメラアニメがある場合　このif文を普通に通る　エラーではない
					//	//_ASSERT(0);
					//	return 2;
					//}

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
					if (firstsrcmp) {
						firstundomp->CopyMP(firstsrcmp);
						firstundomp->SetUndoValidFlag(1);
					}
					else {
						firstundomp->SetUndoValidFlag(0);
					}

					//######################
					// followed MotionPoint
					//######################

					CMotionPoint* befundomp = firstundomp;

					double currenttime;
					CMotionPoint* srcmp = firstsrcmp;
					CMotionPoint* undomp = firstundomp;

					if (firstsrcmp) {
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
								//_ASSERT(0);
								//SetValidFlag(0);
								return 1;
							}

							befundomp = undomp;
						}
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
				if (curbone && (curbone->IsSkeleton() || curbone->IsNullAndChildIsCamera() || curbone->IsCamera())) {

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
	}

	MOTINFO curbonemi = pmodel->GetCurMotInfo();
	::MoveMemory(&m_savemotinfo, &curbonemi, sizeof(MOTINFO));

	if (pcameramodel) {
		MOTINFO curcamerami = pcameramodel->GetCurCameraMotInfo();
		::MoveMemory(&m_savecameramotinfo, &curcamerami, sizeof(MOTINFO));//2024/06/24
	}
	else {
		m_savecameramotinfo.Init();
	}


	m_selectedboneno = selectedboneno;
	m_curbaseno = curbaseno;

	if (srcer) {
		double tmpapplyframe;
		srcer->GetRange(&m_keynum, &(m_undomotid.startframe), &(m_undomotid.endframe), &tmpapplyframe);
		m_undomotid.applyrate = srcapplyrate;
	}
	else {
		m_keynum = 1;
		m_undomotid.startframe = 1.0;
		m_undomotid.endframe = 1.0;
		m_undomotid.applyrate = 50.0;
	}



	m_validflag = 1;//!!!!!!!!!!!!!!

	return 0;
}
int CUndoMotion::RollBackMotion(ChaScene* pchascene, 
	bool undocameraflag,//カメラアニメのUndoとして呼び出す場合にtrue
	bool undoblendshapeflag,//blendshapeアニメのUndoとして呼び出す場合にtrue
	bool limitdegflag, CModel* pmodel,
	int* edittarget, int* pselectedboneno, int* curbaseno,
	UNDOSELECT* dstundoselect,
	BRUSHSTATE* dstbrushstate, UNDOCAMERA* dstundocamera, UNDOMOTID* dstundomotid,
	CBlendShapeElem* dstblendshapeelem)
{
	if( m_validflag != 1 ){
		//_ASSERT( 0 );//選択中のモデルにカメラアニメだけあってボーンモーションが無い場合など　普通にここを通る　エラーではない
		return 2;
	}
	if (!pchascene) {
		_ASSERT(0);
		return 2;
	}
	if (!pmodel) {//カメラのUndo時にもpmodelはs_modelが渡される
		_ASSERT(0);
		return 2;
	}
	if (!pselectedboneno) {
		_ASSERT(0);
		return 2;
	}
	if (!curbaseno) {
		_ASSERT(0);
		return 2;
	}
	if (!dstbrushstate) {
		_ASSERT(0);
		return 2;
	}
	if (!dstundocamera) {
		_ASSERT(0);
		return 2;
	}
	if (!edittarget) {
		_ASSERT(0);
		return 2;
	}
	if (!dstundoselect) {
		_ASSERT(0);
		return 2;
	}
	if (!dstblendshapeelem) {
		_ASSERT(0);
		return 2;
	}

	if (undoblendshapeflag && !GetBlendShapeFlag()) {
		return 2;
	}


	CModel* opemodel;//2024/06/24
	if (undoblendshapeflag) {
		opemodel = m_blendshapeelem.model;
		int chkmodelindex = pchascene->FindModelIndex(opemodel);
		if (chkmodelindex < 0) {
			//モデルが削除されていた場合
			return 2;
		}
	}
	else if (undocameraflag) {
		opemodel = m_undocamera.cameramodel;
		int chkcameramotion = m_undomotid.cameramotid;
		if (!pchascene->IsCameraMotion(opemodel, chkcameramotion)) {//カメラモデルとカメラアニメが削除されていないことを確認
			return 2;
		}
	}
	else {
		opemodel = pmodel;
		int chkmodelindex = pchascene->FindModelIndex(opemodel);
		if (chkmodelindex < 0) {
			//モデルが削除されていた場合
			return 2;
		}
	}
	if (!opemodel) {
		return 2;
	}


	*dstundoselect = m_undoselect;
	*dstbrushstate = m_brushstate;
	*dstundocamera = m_undocamera;
	*dstundomotid = m_undomotid;
	*edittarget = m_edittarget;
	*dstblendshapeelem = m_blendshapeelem;


	if (m_undoselect.undokind == UNDOKIND_EDITMOTION) {

		int setmotid;
		if (!undocameraflag) {
			setmotid = m_savemotinfo.motid;
		}
		else {
			setmotid = m_undomotid.cameramotid;//2024/06/24
		}

		MOTINFO chkmotinfo = opemodel->GetMotInfo(setmotid);
		if (chkmotinfo.motid <= 0) {
			//_ASSERT( 0 );
			SetValidFlag(0);//!!!!!!!!!!!!!!!
			return 1;
		}

		bool cameraanimflag = opemodel->IsCameraMotion(setmotid);



		//::MoveMemory(chkmotinfo, &m_savemotinfo, sizeof(MOTINFO));
		//opemodel->SetCurMotInfo(chkmotinfo);
		//opemodel->SetCurrentMotion(setmotid);

		/*
	/////// destroy
		map<int, CBone*>::iterator itrbone;
		for( itrbone = opemodel->GetBoneListBegin(); itrbone != opemodel->GetBoneListEnd(); itrbone++ ){
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
		for (itrbone = opemodel->GetBoneListBegin(); itrbone != opemodel->GetBoneListEnd(); itrbone++) {
			CBone* curbone = itrbone->second;
			_ASSERT(curbone);

			//if (curbone && (curbone->IsSkeleton() || curbone->IsNullAndChildIsCamera() || curbone->IsCamera())) {
			bool opeflag = false;
			//if (!cameraanimflag && curbone && (curbone->IsSkeleton() || (curbone->IsNull() && !curbone->IsNullAndChildIsCamera()))) {
			if (!cameraanimflag && curbone && curbone->IsSkeleton()) {
				opeflag = true;
			}
			else if (cameraanimflag && curbone && (curbone->IsCamera() || curbone->IsNullAndChildIsCamera())) {
				opeflag = true;
			}
			else {
				opeflag = false;
				continue;//!!!!!!!!!!!!!!!!
			}

			if (opeflag) {


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
		double oldleng = chkmotinfo.frameleng;
		double newleng;
		if (!undocameraflag) {
			newleng = m_savemotinfo.frameleng;
		}
		else {
			newleng = m_savecameramotinfo.frameleng;
		}

		if (oldleng > newleng) {
			map<int, CBone*>::iterator itrbone2;
			for (itrbone2 = opemodel->GetBoneListBegin(); itrbone2 != opemodel->GetBoneListEnd(); itrbone2++) {
				CBone* curbone = itrbone2->second;
				if (curbone && (curbone->IsSkeleton() || curbone->IsNullAndChildIsCamera() || curbone->IsCamera())) {
					curbone->DeleteMPOutOfRange(setmotid, newleng - 1.0);
				}
			}
		}


		//MoveMemory( chkmotinfo, &m_savemotinfo, sizeof( MOTINFO ) );
		//opemodel->SetCurMotInfo( chkmotinfo );
		if (!undocameraflag) {
			opemodel->SetMotInfo(setmotid, m_savemotinfo);
		}
		else {
			opemodel->SetMotInfo(setmotid, m_savecameramotinfo);
		}

		//if (m_edittarget != EDITTARGET_CAMERA) {
		//	opemodel->SetCurrentMotion(setmotid);
		//}
		//else {
		//	opemodel->SetCameraMotionId(setmotid);
		//}

	}
	else if (m_undoselect.undokind == UNDOKIND_EDITBLENDSHAPE) {

		if (!m_blendshapeelem.validflag || !m_blendshapeelem.model || !m_blendshapeelem.mqoobj ||
			(m_blendshapeelem.channelindex < 0)) {
			return 2;
		}
		int modelindex = pchascene->FindModelIndex(m_blendshapeelem.model);//modelが削除されていないことを確認
		if (modelindex < 0) {
			return 2;
		}

		int savedmotid = m_undomotid.bonemotid;
		if (savedmotid <= 0) {
			return 2;
		}

		int curmotleng = (int)m_blendshapeelem.model->GetCurrentMotLeng();
		if (curmotleng <= 0) {
			return 2;
		}

		int savedmotleng = (int)m_blendshapeweight.size();
		if (savedmotleng <= 0) {
			return 2;
		}

		int errorname = 0;
		string shapename = m_blendshapeelem.mqoobj->GetShapeName(m_blendshapeelem.channelindex, &errorname);
		if (errorname != 0) {
			_ASSERT(0);
			return 1;
		}
		char tempshapename[256] = { 0 };
		strcpy_s(tempshapename, 256, shapename.c_str());
		int result1 = m_blendshapeelem.mqoobj->AddShapeAnim(tempshapename, savedmotid, savedmotleng);//モーション長を設定してウェイトをクリアする
		if (result1 != 0) {
			_ASSERT(0);
			return 1;
		}

		int curframe;
		for (curframe = 0; curframe < savedmotleng; curframe++) {
			float savedweight = m_blendshapeweight[curframe];

			int result = m_blendshapeelem.mqoobj->SetShapeAnimWeight(m_blendshapeelem.channelindex,
				savedmotid, curframe, savedweight);
			if (result != 0) {
				_ASSERT(0);
				return 1;
			}
		}

		//ロールバック後に変更が反映されるようにWeightBefを初期化
		int result2 = m_blendshapeelem.mqoobj->SetShapeWeightBef(m_blendshapeelem.channelindex, -1.0f);
		if (result2 != 0) {
			_ASSERT(0);
			return 1;
		}
	}

	*pselectedboneno = m_selectedboneno;
	*curbaseno = m_curbaseno;

	return 0;
}

int CUndoMotion::SaveBlendShapeMotion(bool limitdegflag, int selectedboneno, int curbaseno,
	int srcedittarget, CEditRange* srcer, double srcapplyrate,
	BRUSHSTATE srcbrushstate, UNDOCAMERA srcundocamera,
	CBlendShapeElem srcblendshapeelem)
{
	SetValidFlag(0);
	m_undoselect.undokind = UNDOKIND_EDITBLENDSHAPE;
	m_blendshapeweight.clear();

	if (!srcer) {
		return 2;
	}
	MOTINFO curbonemi = srcblendshapeelem.model->GetCurMotInfo();
	::MoveMemory(&m_savemotinfo, &curbonemi, sizeof(MOTINFO));

	m_undomotid.Init();
	m_undomotid.bonemotid = srcblendshapeelem.model->GetCurrentMotID();
	m_undomotid.curmotid = m_undomotid.bonemotid;
	if (m_undomotid.bonemotid <= 0) {
		return 2;
	}
	if (srcblendshapeelem.model->GetBoneForMotionSize() <= 0) {
		return 2;
	}
	MOTINFO chkmi = srcblendshapeelem.model->GetMotInfo(m_undomotid.bonemotid);
	if (chkmi.motid <= 0) {
		return 2;
	}
	CModel* pcameramodel = nullptr;
	pcameramodel = srcundocamera.cameramodel;
	if (pcameramodel) {//2024/06/27
		m_undomotid.cameramotid = pcameramodel->GetCameraMotionId();
	}
	m_brushstate = srcbrushstate;
	m_undocamera = srcundocamera;
	m_edittarget = srcedittarget;



	m_blendshapeelem = srcblendshapeelem;
	m_blendshapeweight.clear();
	if (!srcblendshapeelem.validflag || !srcblendshapeelem.model || !srcblendshapeelem.mqoobj || 
		(srcblendshapeelem.channelindex < 0)) {
		return 1;
	}
	int motleng = (int)srcblendshapeelem.model->GetCurrentMotLeng();
	if (motleng <= 0) {
		return 1;
	}
	int curframe;
	for (curframe = 0; curframe < motleng; curframe++) {
		float curweight = srcblendshapeelem.mqoobj->GetShapeAnimWeight(m_undomotid.bonemotid, curframe, srcblendshapeelem.channelindex);
		m_blendshapeweight.push_back(curweight);
	}



	m_selectedboneno = selectedboneno;
	m_curbaseno = curbaseno;

	if (srcer) {
		double tmpapplyframe;
		srcer->GetRange(&m_keynum, &(m_undomotid.startframe), &(m_undomotid.endframe), &tmpapplyframe);
		m_undomotid.applyrate = srcapplyrate;
	}
	else {
		m_keynum = 1;
		m_undomotid.startframe = 1.0;
		m_undomotid.endframe = 1.0;
		m_undomotid.applyrate = 50.0;
	}

	SetValidFlag(1);
	return 0;
}

