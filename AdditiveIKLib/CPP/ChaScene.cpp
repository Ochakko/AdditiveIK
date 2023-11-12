﻿#include "stdafx.h"
//#include <stdafx.h>

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

#include <GlobalVar.h>

#include <ChaScene.h>
#include <Model.h>
#include <polymesh3.h>
#include <polymesh4.h>
#include <ExtLine.h>

#include <GetMaterial.h>

#include <mqofile.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#include <Bone.h>
#include <mqoface.h>

#include <InfoWindow.h>

#include <FbxFile.h>

#include <EGPFile.h>


#include <BPWorld.h>
#include <GlutStuff.h>
#include <GLDebugDrawer.h>
#include <btBulletDynamicsCommon.h>

#include <ThreadingMotion2Bt.h>
#include <ThreadingSetBtMotion.h>


#define DBGH
#include <dbg.h>

//#include <DXUT.h>
//#include <DXUTcamera.h>
//#include <DXUTgui.h>
//#include <DXUTsettingsdlg.h>
//#include <SDKmisc.h>

#include <DispObj.h>
#include <MySprite.h>

#include <MotionPoint.h>
//#include <quaternion.h>
//#include <ChaVecCalc.h>//TheadingCalcEul.hでChaCalcFunc.hからChaVecCalc.hはインクルート

#include <Collision.h>
#include <EngName.h>

#include <RigidElem.h>

#include <string>

#include <fbxsdk.h>
#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <fbxsdk/scene/animation/fbxanimevaluator.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"


#include <BopFile.h>
#include <BtObject.h>

#include <collision.h>
#include <EditRange.h>
//#include <BoneProp.h>

#include <ThreadingLoadFbx.h>
#include <ThreadingUpdateMatrix.h>
#include <ThreadingPostIK.h>
#include <ThreadingFKTra.h>
#include <ThreadingCopyW2LW.h>
#include <ThreadingCalcEul.h>
#include <ThreadingRetarget.h>
#include <ThreadingInitMp.h>

#include <NodeOnLoad.h>

#include <OrgWindow.h>


#include <DXUT.h>
#include <io.h>


//for __nop()
#include <intrin.h>


using namespace std;




ChaScene::ChaScene()
{
	InitParams();
}
ChaScene::~ChaScene()
{
	DestroyObjs();
}

void ChaScene::InitParams()
{
	ClearModelIndex();

	m_totalmb.Init();
	m_totalmb.center = ChaVector3(0.0f, 0.0f, 0.0f);
	m_totalmb.max = ChaVector3(5.0f, 5.0f, 5.0f);
	m_totalmb.min = ChaVector3(-5.0f, -5.0f, -5.0f);
	m_totalmb.r = (float)ChaVector3LengthDbl(&m_totalmb.max);

	m_curmodelmenuindex = -1;

	m_totalupdatethreadsnum = 0;
	m_updateslot = 0;

	m_Motion2BtThreads = 0;//モデル数分配列
	m_SetBtMotionThreads = 0;//モデル数分配列
	m_created_Motion2BtThreadsNum = 0;
	m_created_SetBtMotionThreadsNum = 0;


}
void ChaScene::DestroyObjs()
{

	DestroyMotion2BtThreads();
	DestroySetBtMotionThreads();

	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			//FbxScene* pscene = curmodel->GetScene();
			//if (pscene){
			//	pscene->Destroy();
			//}
			//curmodel->DestroyScene();

			delete curmodel;
		}
	}
	m_modelindex.clear();

}

int ChaScene::UpdateMatrixModels(bool limitdegflag, ChaMatrix* vpmat, double srcframe)
{
	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}

	if (!m_modelindex.empty()) {

		m_updateslot = (int)(!(m_updateslot != 0));
		//m_totalupdatethreadsnum = 0;

		bool needwaitflag = false;
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* curmodel = m_modelindex[modelindex].modelptr;
			if (curmodel) {
				if (curmodel->GetCurMotInfo()) {
					curmodel->SetMotionFrame(srcframe);

					ChaMatrix wmat = curmodel->GetWorldMat();
					curmodel->UpdateMatrix(limitdegflag, &wmat, vpmat, needwaitflag, m_updateslot);
				}
				else {
					//モーションが無い場合にもChkInViewを呼ぶためにUpdateMatrix呼び出しは必要
					ChaMatrix tmpwm = curmodel->GetWorldMat();
					curmodel->UpdateMatrix(limitdegflag, &tmpwm, vpmat, needwaitflag, m_updateslot);
				}

				//2023/11/03
				//ここ(UpdateMatrixModels)でtotalupdatethreadsnumをセットすると
				//UpdateMatrixModelsとRenderModelsの間で　OnDelModelを呼んだ場合にスレッド数が合わずに無限ループする
				//よって　スレッド総数はWaitUpdateThreadsで調べてセットすることにした
				//m_totalupdatethreadsnum += curmodel->GetThreadingUpdateMatrixNum();
			}
		}

		//#########################################################################################
		//2023/11/01
		//姿勢データをダブルバッファ化した
		// m_updateslotで計算中と描画用を識別
		// 
		//ここで姿勢計算スレッドを Not Wait
		//待たずにそのまま　Render関数を呼び　Render関数の終わりでアップデートスレッドを待って同期
		//Renderはm_updateslotとは違う側の計算済のデータを参照する
		//#########################################################################################



		//2023/11/09 1.2.0.30 RC1
		//プレビュー中以外に　タイムラインのフレームを移動する場合に　前フレーム以前の表示が気になるので
		//プレビュー中以外のときには同期する　Render()においてはcalcslotflag=trueで描画
		if (g_previewFlag == 0) {
			WaitUpdateThreads();
		}


	}

	return 0;
}

int ChaScene::RenderModels(ID3D11DeviceContext* pd3dImmediateContext, int lightflag, ChaVector4 diffusemult, int btflag)
{

	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}


	//2023/11/09
	//マウスホイールで　ロングタイムラインのフレームを移動する際に
	//前フレーム以前のゴーストがみえないように calcslotflag = trueを SetShaderConst()に渡す
	bool calcslotflag;
	if (g_previewFlag == 0) {
		calcslotflag = true;
	}
	else {
		calcslotflag = false;
	}


	//####################################################################################
	//2023/10/31
	//全モデルを横断して　DispGourpのグループ番号順に描画
	// モデルの描画順序を操作しなくても　グループ番号設定で描画順を制御可能
	// 
	// 描画順の例：　
	//	noalpha 
	//		model1のgroup1-->model2のgorup1-->model1のgroup2-->model2のgroup2--> ...
	// -->withalpha
	//		model1のgroup1-->model2のgorup1-->model1のgroup2-->model2のgroup2--> ...
	//####################################################################################


	if (!m_modelindex.empty()) {
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		int renderindex;


		//int renderslot = (int)(!(m_updateslot != 0));

		for (renderindex = 0; renderindex < 2; renderindex++) {

			bool withalpha;
			if (renderindex == 0) {
				withalpha = false;
			}
			else {
				withalpha = true;
			}

			int groupindex;
			for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {

				for (modelindex = 0; modelindex < modelnum; modelindex++) {

					CModel* curmodel = m_modelindex[modelindex].modelptr;
					if (curmodel && curmodel->GetModelDisp() && curmodel->GetInView()) {

						ChaVector4 materialdisprate = curmodel->GetMaterialDispRate();

						if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {

							int elemnum = curmodel->GetDispGroupSize(groupindex);
							int elemno;
							for (elemno = 0; elemno < elemnum; elemno++) {

								CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);

								if (curobj && curobj->GetVisible()) {

									//if (curobj->GetDispLine()) {
									//	int dbgflag1 = 1;
									//}


									bool found_noalpha = false;
									bool found_alpha = false;
									int result = curobj->IncludeTransparent(diffusemult.w, &found_noalpha, &found_alpha);//2023/09/24
									if (result == 1) {
										_ASSERT(0);
										return 1;
									}
									else if (result == 2) {
										continue;
									}

									if ((withalpha == false) && (found_noalpha == false)) {
										//不透明描画時　１つも不透明がなければ　レンダースキップ
										continue;
									}
									if ((withalpha == true) && (found_alpha == false)) {
										//半透明描画時　１つも半透明がなければ　レンダースキップ
										continue;
									}

									if (curobj->GetDispObj()) {
										if (curobj->GetPm3()) {
											CallF(curmodel->SetShaderConst(curobj, btflag, calcslotflag), return 1);
											CallF(curobj->GetDispObj()->RenderNormalPM3(withalpha, pd3dImmediateContext, lightflag, diffusemult, materialdisprate, curobj), return 1);
										}
										else if (curobj->GetPm4()) {
											CallF(curmodel->SetShaderConst(curobj, btflag, calcslotflag), return 1);
											CallF(curobj->GetDispObj()->RenderNormal(withalpha, pd3dImmediateContext, lightflag, diffusemult, materialdisprate, curobj), return 1);
										}
										else {
											_ASSERT(0);
										}
									}
									else if (curobj->GetDispLine()) {
										CallF(curobj->GetDispLine()->RenderLine(withalpha, pd3dImmediateContext, diffusemult, materialdisprate), return 1);
									}
								}
							}
						}
					}
				}
			}
		}


		//#########################################################################################################################
		//2023/11/01
		//姿勢データをダブルバッファ化した
		// m_updateslotで計算中と描画用を識別
		// 
		//UpdateMatrixModels()関数はスレッド終了を待たずに　Render関数を呼び　Render関数の終わりでアップデートスレッドを待って同期
		//Renderはm_updateslotとは違う側の計算済のデータを参照する
		//#########################################################################################################################

		//Render中は同時進行し　Render後に待つことで　ダブルバッファ同期!!!!!!!!!!!!!!!!
		if ((g_previewFlag != 0) && (g_previewFlag != 4)) {
			WaitUpdateThreads();
		}
		else if (g_previewFlag == 4) {
			WaitSetBtMotionFinished();//!!!!!!!!!!!!!!!!
		}
		else {
			//
			//g_previewFlag == 0
			// 
			// 
			//2023/11/09 1.2.0.30 RC1
			//プレビュー中以外に　タイムラインのフレームを移動する場合に　前フレーム以前の表示が気になるので
			//プレビュー中以外のときには同期する(UpdaetMatrixModelsの終わりで終了待機)　Render()においてはcalcslotflag=trueで描画
		}

		
	}

	return 0;
}

int ChaScene::WaitUpdateThreads()
{
	//待つ必要有　コメントアウト
	//if (g_changeUpdateThreadsNum) {
	//	//アップデート用スレッド数を変更中
	//	return 0;
	//}


	int modelnum = (int)m_modelindex.size();

	m_totalupdatethreadsnum = 0;
	if (!m_modelindex.empty()) {
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* curmodel = m_modelindex[modelindex].modelptr;
			if (curmodel) {
				m_totalupdatethreadsnum += curmodel->GetThreadingUpdateMatrixNum();
			}
		}
	}

	int donethreadingnum = 0;
	bool yetflag = true;
	while (donethreadingnum < m_totalupdatethreadsnum) {

		donethreadingnum = 0;

		int modelindex2;
		for (modelindex2 = 0; modelindex2 < modelnum; modelindex2++) {
			CModel* curmodel = m_modelindex[modelindex2].modelptr;
			if (curmodel && (curmodel->GetNoBoneFlag() == false)) {
				if (curmodel->GetThreadingUpdateMatrix() != NULL) {

					int finishedcount = 0;
					int updatecount;
					for (updatecount = 0; updatecount < curmodel->GetThreadingUpdateMatrixNum(); updatecount++) {
						CThreadingUpdateMatrix* curupdate = curmodel->GetThreadingUpdateMatrix() + updatecount;
						if (curupdate->IsFinished()) {
							donethreadingnum++;
						}
					}
				}
				else {
					_ASSERT(0);
				}
			}
		}
	}

	return 0;
}


CModel* ChaScene::GetTheLastCameraModel()
{
	CModel* retcameramodel = 0;
	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			if ((g_endappflag == 0) && curmodel->IsCameraLoaded()) {
				retcameramodel = curmodel;//後から読み込んだモデルの中で　カメラがあれば　それを選ぶ
			}
		}
	}

	return retcameramodel;

}

int ChaScene::CalcTotalModelBound()
{

	m_totalmb.Init();

	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			MODELBOUND mb;
			curmodel->GetModelBound(&mb);
			if (mb.r != 0.0f) {
				AddModelBound(&m_totalmb, &mb);
			}
		}
	}

	return 0;
}

int ChaScene::AddModelBound(MODELBOUND* mb, MODELBOUND* addmb)
{
	ChaVector3 newmin = mb->min;
	ChaVector3 newmax = mb->max;

	if (newmin.x > addmb->min.x) {
		newmin.x = addmb->min.x;
	}
	if (newmin.y > addmb->min.y) {
		newmin.y = addmb->min.y;
	}
	if (newmin.z > addmb->min.z) {
		newmin.z = addmb->min.z;
	}

	if (newmax.x < addmb->max.x) {
		newmax.x = addmb->max.x;
	}
	if (newmax.y < addmb->max.y) {
		newmax.y = addmb->max.y;
	}
	if (newmax.z < addmb->max.z) {
		newmax.z = addmb->max.z;
	}

	mb->center = (newmin + newmax) * 0.5f;
	mb->min = newmin;
	mb->max = newmax;

	ChaVector3 diff;
	diff = mb->center - newmin;
	mb->r = (float)ChaVector3LengthDbl(&diff);

	return 0;
}

const WCHAR* ChaScene::GetModelFileName(int srcmodelindex)
{
	if (m_modelindex.empty()) {
		return 0;
	}
	else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
		if (m_modelindex[srcmodelindex].modelptr) {
			return m_modelindex[srcmodelindex].modelptr->GetFileName();
		}
		else {
			return 0;
		}
	}
	else {
		_ASSERT(0);
		return 0;
	}
}


int ChaScene::DelModel(int srcmodelindex)
{
	if (m_modelindex.empty()) {
		return 0;
	}
	else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {

		CModel* delmodel;
		delmodel = GetModel(srcmodelindex);
		if (delmodel) {

			std::vector<MODELELEM> tmpvec;
			std::vector<MODELELEM>::iterator itrmodel;
			for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
				MODELELEM chkelem = *itrmodel;
				CModel* chkmodel = chkelem.modelptr;
				if (chkmodel != delmodel) {
					tmpvec.push_back(chkelem);
				}
			}
			m_modelindex.clear();
			m_modelindex = tmpvec;


			//map<CModel*, int>::iterator itrbonecnt;
			//itrbonecnt = g_bonecntmap.find(delmodel);
			//if (itrbonecnt != g_bonecntmap.end()){
			//	g_bonecntmap.erase(itrbonecnt);
			//}

			delmodel->WaitUpdateMatrixFinished();//2022/08/18

			CBone::OnDelModel(delmodel);

			//FbxScene* pscene = delmodel->GetScene();
			//if (pscene){
			//	pscene->Destroy();
			//}
			delete delmodel;
		}

		//int mdlno;
		//std::vector<MODELELEM>::iterator itrmodel = m_modelindex.begin();
		//for (mdlno = 0; mdlno < srcmodelindex; mdlno++) {
		//	itrmodel++;
		//}
		//m_modelindex.erase(itrmodel);
		////s_modelindex.pop_back();


		CreateMotion2BtThreads();
		CreateSetBtMotionThreads();


	}
	else {
		_ASSERT(0);
		return 0;
	}


	return 0;
}

int ChaScene::DelAllModel()
{
	int mdlnum = GetModelNum();
	if (mdlnum <= 0) {
		return 0;
	}

	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* delmodel = itrmodel->modelptr;
		if (delmodel) {
			delmodel->WaitUpdateMatrixFinished();//2022/08/18
			CBone::OnDelModel(delmodel);

			FbxScene* pscene = delmodel->GetScene();
			//if (pscene){
			//	pscene->Destroy();
			//}
			delete delmodel;
		}
		itrmodel->tlarray.clear();
		itrmodel->boneno2lineno.clear();
		itrmodel->lineno2boneno.clear();
	}


	m_modelindex.clear();

	DestroyMotion2BtThreads();
	DestroySetBtMotionThreads();

	return 0;
}

int ChaScene::StopBt()
{
	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			curmodel->BulletSimulationStop();
			SetKinematicToHand(curmodel, false);
		}
	}
	return 0;
}

int ChaScene::StartBt()
{
	vector<MODELELEM>::iterator itrmodel4;
	for (itrmodel4 = m_modelindex.begin(); itrmodel4 != m_modelindex.end(); itrmodel4++) {
		CModel* pmodel4 = itrmodel4->modelptr;
		if (pmodel4) {
			pmodel4->BulletSimulationStop();
			SetKinematicToHand(pmodel4, false);

			pmodel4->BulletSimulationStart();
		}
	}

	return 0;
}

void ChaScene::SetKinematicToHand(CModel* srcmodel, bool srcflag)
{
	if (!srcmodel) {
		return;
	}
	if (!srcmodel->GetTopBone()) {
		return;
	}

	SetKinematicToHandReq(srcmodel, srcmodel->GetTopBone(false), srcflag);
}


void ChaScene::SetKinematicToHandReq(CModel* srcmodel, CBone* srcbone, bool srcflag)
{
	if (!srcmodel) {
		return;
	}
	if (!srcbone) {
		return;
	}

	const char* pbonename = (const char*)srcbone->GetBoneName();
	const char* phandpat1 = strstr(pbonename, "Elbow_branch");
	const char* phandpat2 = strstr(pbonename, "Hand");

	if (srcbone->IsSkeleton()) {
		if ((phandpat1 || phandpat2) && (srcbone->IsSkeleton())) {
			srcmodel->SetKinematicTmpLower(srcbone, srcflag);
		}
	}

	if (srcbone->GetChild(false)) {
		SetKinematicToHandReq(srcmodel, srcbone->GetChild(false), srcflag);
	}
	if (srcbone->GetBrother(false)) {
		SetKinematicToHandReq(srcmodel, srcbone->GetBrother(false), srcflag);
	}

}

int ChaScene::SetMotionSpeed(int srcmodelindex, double srcspeed)
{
	if ((srcmodelindex >= 0) && (srcmodelindex < GetModelNum())) {
		m_modelindex[srcmodelindex].modelptr->SetMotionSpeed(srcspeed);
	}
	else if (srcmodelindex == -1) {
		size_t modelno;
		size_t modelnum = GetModelNum();
		for (modelno = 0; modelno < modelnum; modelno++) {
			m_modelindex[modelno].modelptr->SetTmpMotSpeed((float)srcspeed);//!!!!!!
			m_modelindex[modelno].modelptr->SetMotionSpeed(srcspeed);
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}


int ChaScene::SetMotionFrame(int srcmodelindex, double srcframe)
{
	if ((srcmodelindex >= 0) && (srcmodelindex < GetModelNum())) {
		m_modelindex[srcmodelindex].modelptr->SetMotionFrame(srcframe);
	}
	else if (srcmodelindex == -1) {
		size_t modelno;
		size_t modelnum = GetModelNum();
		for (modelno = 0; modelno < modelnum; modelno++) {
			if (m_modelindex[modelno].modelptr->GetCurMotInfo()) {
				m_modelindex[modelno].modelptr->SetMotionFrame(srcframe);
			}
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int ChaScene::SetENullTime(int srcmodelindex, double srcframe)
{
	if ((srcmodelindex >= 0) && (srcmodelindex < GetModelNum())) {
		m_modelindex[srcmodelindex].modelptr->SetENullTime(srcframe);
	}
	else if (srcmodelindex == -1) {
		size_t modelno;
		size_t modelnum = GetModelNum();
		for (modelno = 0; modelno < modelnum; modelno++) {
			m_modelindex[modelno].modelptr->SetENullTime(srcframe);
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int ChaScene::Motion2Bt(bool limitdegflag, double nextframe, ChaMatrix* pmVP, int loopstartflag)
{
	if (!pmVP) {
		_ASSERT(0);
		return 1;
	}

	if (!m_modelindex.empty()) {

		if (m_Motion2BtThreads) {
			int updatecount;
			for (updatecount = 0; updatecount < m_created_Motion2BtThreadsNum; updatecount++) {
				CThreadingMotion2Bt* curupdate = m_Motion2BtThreads + updatecount;
				curupdate->Motion2Bt(limitdegflag, nextframe, pmVP, loopstartflag, m_updateslot);
			}
			WaitMotion2BtFinished();//次に順番に計算することがあるので　待機が必要
		}



		//m_updateslot = (int)(!(m_updateslot != 0));
		////m_totalupdatethreadsnum = 0;
		//
		//bool needwaitflag = false;
		//int modelnum = (int)m_modelindex.size();
		//int modelindex;
		//for (modelindex = 0; modelindex < modelnum; modelindex++) {
		//	CModel* curmodel = m_modelindex[modelindex].modelptr;
		//	if (curmodel) {
		//		if ((curmodel->GetBtCnt() != 0) && (loopstartflag == 1)) {
		//			curmodel->ZeroBtCnt();
		//		}
		//		else {
		//			if (curmodel->GetCurMotInfo()) {
		//				curmodel->Motion2Bt(limitdegflag, nextframe, pmVP, m_updateslot);
		//			}
		//			curmodel->PlusPlusBtCnt();
		//		}
		//	}
		//}
	}




	return 0;
}


int ChaScene::SetBtMotion(bool limitdegflag, double nextframe, ChaMatrix* pmVP, CModel* smodel, double srcreccnt)
{
	if (!pmVP) {
		_ASSERT(0);
		return 1;
	}


	if (!m_modelindex.empty()) {

		if (m_SetBtMotionThreads) {
			int updatecount;
			for (updatecount = 0; updatecount < m_created_SetBtMotionThreadsNum; updatecount++) {
				CThreadingSetBtMotion* curupdate = m_SetBtMotionThreads + updatecount;
				curupdate->SetBtMotion(limitdegflag, nextframe, pmVP, smodel, srcreccnt, m_updateslot);
			}
			//WaitSetBtMotionFinished();//レンダー中に計算し　レンダー後に待機するので　コメントアウト
		}



		//bool needwaitflag = false;
		//int modelnum = (int)m_modelindex.size();
		//int modelindex;
		//for (modelindex = 0; modelindex < modelnum; modelindex++) {
		//	CModel* curmodel = m_modelindex[modelindex].modelptr;
		//	if (curmodel) {
		//		if (curmodel->GetBtCnt() != 0) {
		//			if (curmodel->GetCurMotInfo()) {

		//				curmodel->SetBtMotionOnBt(limitdegflag, nextframe, pmVP, m_updateslot);

		//				////60 x 60 frames limit : 60 sec limit
		//				//if ((curmodel == s_model) && (s_model->GetBtCnt() > 0) && (s_reccnt < MAXPHYSIKRECCNT)) {
		//				//	s_rectime = (double)((int)s_reccnt);
		//				//	s_model->PhysIKRec(g_limitdegflag, s_rectime);
		//				//	s_reccnt++;
		//				//}
		//			}
		//			else {
		//				//モーションが無い場合にもChkInViewを呼ぶためにUpdateMatrix呼び出しは必要
		//				ChaMatrix tmpwm = curmodel->GetWorldMat();
		//				curmodel->UpdateMatrix(limitdegflag, &tmpwm, pmVP, m_updateslot);
		//			}
		//		}
		//	}
		//}
	}

	return 0;
}


int ChaScene::UpdateBtFunc(bool limitdegflag, double nextframe, ChaMatrix* pmVP, int loopstartflag,
	CModel* smodel, bool recstopflag, BPWorld* bpWorld, double srcreccnt,
	int (*srcStopBtRec)())
{
	if (!pmVP || !smodel || !bpWorld || !srcStopBtRec) {
		_ASSERT(0);
		return 1;
	}

	m_updateslot = (int)(!(m_updateslot != 0));

	Motion2Bt(limitdegflag, nextframe, pmVP, loopstartflag);//MultiThreading per CModel. Wait threads on return.

	if (smodel && (recstopflag == true)) {
		if (srcStopBtRec) {
			(srcStopBtRec)();
		}
	}
	else {
		bpWorld->clientMoveAndDisplay();


		//int modelcount2;
		//for (modelcount2 = 0; modelcount2 < modelnum; modelcount2++) {
		//	CModel* curmodel = s_chascene->GetModel(modelcount2);
		//	if (curmodel && (curmodel->GetBtCnt() != 0)) {
		//		if (curmodel->GetCurMotInfo()) {
		//			//curmodel->SetBtMotion(curmodel->GetBoneByID(s_curboneno), 0, *pnextframe, &curmodel->GetWorldMat(), &s_matVP);
		//			ChaMatrix tmpwm = curmodel->GetWorldMat();
		//			curmodel->SetBtMotion(g_limitdegflag, 0, 0, nextframe, &tmpwm, &s_matVP);//第一引数は物理IK用

		//			//60 x 60 frames limit : 60 sec limit
		//			if ((curmodel == s_model) && (s_model->GetBtCnt() > 0) && (s_reccnt < MAXPHYSIKRECCNT)) {
		//				s_rectime = (double)((int)s_reccnt);
		//				s_model->PhysIKRec(g_limitdegflag, s_rectime);
		//				s_reccnt++;
		//			}
		//		}
		//		else {
		//			//モーションが無い場合にもChkInViewを呼ぶためにUpdateMatrix呼び出しは必要
		//			ChaMatrix tmpwm = curmodel->GetWorldMat();
		//			curmodel->UpdateMatrix(g_limitdegflag, &tmpwm, &s_matVP);
		//		}
		//	}
		//	//s_tum.SetBtMotion(OnFramePreviewBtAftFunc, s_modelindex, *pnextframe);
		//}

		////60 x 60 frames limit : 60 sec limit
		//if ((smodel->GetBtCnt() > 0) && (s_reccnt < MAXPHYSIKRECCNT)) {
		//	s_rectime = (double)((int)s_reccnt);
		//	s_model->PhysIKRec(g_limitdegflag, s_rectime);
		//	s_reccnt++;
		//}

		SetBtMotion(limitdegflag, nextframe, pmVP, smodel, srcreccnt);//MultiThreading per CModel, Not Wait threads on return.

	}

	return 0;
}



int ChaScene::CreateMotion2BtThreads()
{

	DestroyMotion2BtThreads();
	//Sleep(100);

	int ThreadsNum = (int)m_modelindex.size();

	CThreadingMotion2Bt* newthreads;
	newthreads = new CThreadingMotion2Bt[ThreadsNum];
	if (!newthreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < ThreadsNum; createno++) {
		CThreadingMotion2Bt* curupdate = newthreads + createno;
		curupdate->CreateThread((DWORD)(1 << createno));

		CModel* srcmodel = m_modelindex[createno].modelptr;
		curupdate->SetModel(srcmodel);
	}

	m_Motion2BtThreads = newthreads;
	m_created_Motion2BtThreadsNum = ThreadsNum;

	return 0;
}


int ChaScene::DestroyMotion2BtThreads()
{
	WaitMotion2BtFinished();

	if (m_Motion2BtThreads) {
		delete[] m_Motion2BtThreads;
	}
	m_Motion2BtThreads = 0;
	m_created_Motion2BtThreadsNum = 0;

	return 0;
}

void ChaScene::WaitMotion2BtFinished()
{
	if (m_Motion2BtThreads != NULL) {

		//int ThreadsNum = (int)m_modelindex.size();
		int ThreadsNum = m_created_Motion2BtThreadsNum;

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < ThreadsNum; updatecount++) {
				CThreadingMotion2Bt* curupdate = m_Motion2BtThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == ThreadsNum) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}

int ChaScene::CreateSetBtMotionThreads()
{

	DestroySetBtMotionThreads();
	//Sleep(100);

	int ThreadsNum = (int)m_modelindex.size();

	CThreadingSetBtMotion* newthreads;
	newthreads = new CThreadingSetBtMotion[ThreadsNum];
	if (!newthreads) {
		_ASSERT(0);
		return 1;
	}
	int createno;
	for (createno = 0; createno < ThreadsNum; createno++) {
		CThreadingSetBtMotion* curupdate = newthreads + createno;
		curupdate->CreateThread((DWORD)(1 << createno));

		CModel* srcmodel = m_modelindex[createno].modelptr;
		curupdate->SetModel(srcmodel);
	}

	m_SetBtMotionThreads = newthreads;
	m_created_SetBtMotionThreadsNum = ThreadsNum;

	return 0;
}

int ChaScene::DestroySetBtMotionThreads()
{
	WaitSetBtMotionFinished();

	if (m_SetBtMotionThreads) {
		delete[] m_SetBtMotionThreads;
	}
	m_SetBtMotionThreads = 0;
	m_created_SetBtMotionThreadsNum = 0;

	return 0;
}

void ChaScene::WaitSetBtMotionFinished()
{
	if (m_SetBtMotionThreads != NULL) {

		//int ThreadsNum = (int)m_modelindex.size();
		int ThreadsNum = m_created_SetBtMotionThreadsNum;

		bool yetflag = true;
		while (yetflag == true) {
			int finishedcount = 0;
			int updatecount;
			for (updatecount = 0; updatecount < ThreadsNum; updatecount++) {
				CThreadingSetBtMotion* curupdate = m_SetBtMotionThreads + updatecount;
				if (curupdate->IsFinished()) {
					finishedcount++;
				}
			}

			if (finishedcount == ThreadsNum) {
				yetflag = false;
				return;
			}
			else {
				//__nop();
				//__nop();
				//__nop();
				//__nop();
			}
		}

	}

}
