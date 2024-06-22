#include "stdafx.h"
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

#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
#include "../../MiniEngine/TResourceBank.h"


//#include <DXUT.h>
#include <io.h>


//for __nop()
#include <intrin.h>


using namespace std;


//extern
//extern TResourceBank<CMQOMaterial> g_materialbank;


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
	//m_totalmb.center.SetParams(0.0f, 0.0f, 0.0f);
	//m_totalmb.max.SetParams(5.0f, 5.0f, 5.0f);
	//m_totalmb.min.SetParams(-5.0f, -5.0f, -5.0f);
	//m_totalmb.r = (float)ChaVector3LengthDbl(&m_totalmb.max);

	m_curmodelmenuindex = -1;

	m_totalupdatethreadsnum = 0;
	m_updateslot = 0;

	m_Motion2BtThreads = 0;//モデル数分配列
	m_SetBtMotionThreads = 0;//モデル数分配列
	m_created_Motion2BtThreadsNum = 0;
	m_created_SetBtMotionThreadsNum = 0;

	ClearRenderObjs();
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

void ChaScene::SetUpdateSlot()
{
	m_updateslot = (int)(!(m_updateslot != 0));

	if (!m_modelindex.empty()) {
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* curmodel = m_modelindex[modelindex].modelptr;
			if (curmodel) {
				curmodel->SetUpdateSlotReq(curmodel->GetTopBone(false), m_updateslot);
				curmodel->ResetBtMovableReq(curmodel->GetTopBone(false));
			}
		}
	}
}

void ChaScene::ResetCSFirstDispatchFlag()
{
	if (!m_modelindex.empty()) {
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* curmodel = m_modelindex[modelindex].modelptr;
			if (curmodel) {
				curmodel->SetCSFirstDispatchFlag(true);
			}
		}
	}
}



int ChaScene::UpdateMatrixModels(bool limitdegflag, ChaMatrix* vmat, ChaMatrix* pmat, double srcframe, int loopstartflag)
{
	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}

	if (!m_modelindex.empty()) {

		//m_totalupdatethreadsnum = 0;

		bool needwaitflag = false;
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* curmodel = m_modelindex[modelindex].modelptr;
			if (curmodel && (curmodel->GetGrassFlag() == false)) {

				int curmotid = curmodel->GetCurrentMotID();
				if (curmotid <= 0) {
					int dbgflag1 = 1;
				}

				if (curmodel->ExistCurrentMotion()) {
					curmodel->SetMotionFrame(srcframe);//refposの場合にも必要
				}

				if (curmodel->GetRefPosFlag() == false) {//2024/02/06
					ChaMatrix wmat = curmodel->GetWorldMat();
					curmodel->UpdateMatrix(limitdegflag, &wmat, vmat, pmat, needwaitflag, 0);// , updateslot);
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
		//if (g_previewFlag == 0) {
		//	WaitUpdateThreads();
		//}


	}

	return 0;
}

int ChaScene::UpdateMatrixOneModel(CModel* srcmodel, bool limitdegflag, 
	ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat, 
	double srcframe, int refposindex)
{
	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}
	if (!srcmodel) {
		return 0;
	}

	//m_updateslot = (int)(!(m_updateslot != 0));
	////m_totalupdatethreadsnum = 0;

	bool needwaitflag = true;//!!!!!!!!!!!!
	if (srcmodel->ExistCurrentMotion()) {
		srcmodel->SetMotionFrame(srcframe);
		srcmodel->UpdateMatrix(limitdegflag, wmat, vmat, pmat, needwaitflag, refposindex);// , m_updateslot);
	}
	else {
		//モーションが無い場合にもChkInViewを呼ぶためにUpdateMatrix呼び出しは必要
		srcmodel->UpdateMatrix(limitdegflag, wmat, vmat, pmat, needwaitflag, refposindex);//, m_updateslot);
	}

	return 0;
}

bool ChaScene::PickPolyMesh(int pickkind, 
	UIPICKINFO* tmppickinfo,
	CModel** pickmodel, CMQOObject** pickmqoobj, CMQOMaterial** pickmaterial, ChaVector3* pickhitpos)
{
	if (!tmppickinfo || !pickmodel || !pickmqoobj || !pickmaterial || !pickhitpos) {
		_ASSERT(0);
		return false;
	}

	*pickmodel = nullptr;
	*pickmqoobj = nullptr;
	*pickmaterial = nullptr;
	pickhitpos->SetParams(0.0f, 0.0f, 0.0f);

	vector<myRenderer::RENDEROBJ> pickvec;

	if (!m_modelindex.empty()) {
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		int groupindex;
		for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
			for (modelindex = 0; modelindex < modelnum; modelindex++) {

				CModel* curmodel = m_modelindex[modelindex].modelptr;
				if (curmodel && curmodel->GetModelDisp() && curmodel->GetInView(0)) {
					if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {
						int elemnum = curmodel->GetDispGroupSize(groupindex);
						int elemno;
						for (elemno = 0; elemno < elemnum; elemno++) {

							CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);
							if (curobj && curobj->GetDispObj() && (curobj->GetPm3() || curobj->GetPm4()) && 
								curobj->GetVisible(0)) {

								myRenderer::RENDEROBJ pickobj;
								pickobj.Init();
								pickobj.pmodel = curmodel;
								pickobj.mqoobj = curobj;

								pickvec.push_back(pickobj);
							}
						}
					}
				}
			}
		}
	}

	//##########################################################
	//カメラ距離でソートしてから　pickする
	//##########################################################
	if (!pickvec.empty()) {
		int foundorder = 1;
		if (!pickvec.empty()) {
			std::sort(pickvec.begin(), pickvec.end());//カメラ距離でソート

			int pickvecsize = (int)pickvec.size();
			int pickindex;
			CMQOObject* befmqoobj = nullptr;
			CMQOMaterial* befmqomat = nullptr;
			for (pickindex = 0; pickindex < pickvecsize; pickindex++) {
				myRenderer::RENDEROBJ pickobj = pickvec[pickindex];
				CModel* curmodel = pickobj.pmodel;
				CMQOObject* curobj = pickobj.mqoobj;
				ChaVector3 curhitpos;
				curhitpos.SetParams(0.0f, 0.0f, 0.0f);

				if (pickobj.GetPickOpeFlag(g_projfar, g_pickdistrate) && curmodel && curobj) {
					UIPICKINFO pickinfo = *tmppickinfo;
					int hitfaceindex = -1;
					int colli = 0;
					if (curobj->GetPm3()) {
						//CPU版
						colli = curmodel->CollisionPolyMesh3_Mouse(&pickinfo, curobj, &hitfaceindex, &curhitpos);
					}
					else if (curobj->GetPm4()) {
						//GPU版
						if (curobj->GetDispObj()) {
							//座標変換　ボーン変形　即時実行のコンピュータシェーダ
							curobj->GetDispObj()->ComputeDeform(pickobj);
						}
						colli = curmodel->CollisionPolyMesh_Mouse(&pickinfo, curobj, &hitfaceindex, &curhitpos);
					}
					else {
						colli = 0;
					}

					if ((colli != 0) && (hitfaceindex >= 0)) {
					
						CMQOObject* chkmqoobj = curobj;
						CMQOMaterial* chkmqomat = curobj->GetMaterialByFaceIndex(hitfaceindex);
					
						//g_pickorderは数字キーを押して設定
						//カメラから何番目(数字キーの数字番目、0は10番目)に近いオブジェクトかを意味する
						if (g_pickorder > foundorder) {
							if (pickkind == NUMKEYPICK_MQOOBJECT) {
								if (chkmqoobj == befmqoobj) {
									continue;//前回と同じものを見つけた場合はfoundorderを変えずにcontinue.
								}
							}
							else if (pickkind == NUMKEYPICK_MQOMATERIAL) {
								if (chkmqomat == befmqomat) {
									continue;//前回と同じものを見つけた場合はfoundorderを変えずにcontinue.
								}
							}
							else {
								_ASSERT(0);
							}
					
							befmqoobj = chkmqoobj;
							befmqomat = chkmqomat;
					
							foundorder++;//前回と結果が異なり、かつorderが指定より小さい場合　foundorderを増やしてcontinue.
							continue;
						}
						else {
							//g_pickorder番目のオブジェクトを返す
					
							if (pickkind == NUMKEYPICK_MQOOBJECT) {
								if (chkmqoobj != befmqoobj) {
									*pickmodel = curmodel;
									*pickmqoobj = chkmqoobj;
									*pickmaterial = chkmqomat;
									*tmppickinfo = pickinfo;
									*pickhitpos = curhitpos;
									return true;
								}
							}
							else if (pickkind == NUMKEYPICK_MQOMATERIAL) {
								if (chkmqomat != befmqomat) {
									*pickmodel = curmodel;
									*pickmqoobj = chkmqoobj;
									*pickmaterial = chkmqomat;
									*tmppickinfo = pickinfo;
									*pickhitpos = curhitpos;
									return true;
								}
							}
							else {
								_ASSERT(0);
							}
						}
					}
				}
			}
		}
	}

	return false;

}

bool ChaScene::GetResultOfPickRay(int pickkind,
	CModel** pickmodel, CMQOObject** pickmqoobj, CMQOMaterial** pickmaterial, ChaVector3* pickhitpos)
{
	if (!pickmodel || !pickmqoobj || !pickmaterial || !pickhitpos) {
		_ASSERT(0);
		return false;
	}

	vector<myRenderer::RENDEROBJ> pickvec;

	if (!m_modelindex.empty()) {
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		int groupindex;
		for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
			for (modelindex = 0; modelindex < modelnum; modelindex++) {

				CModel* curmodel = m_modelindex[modelindex].modelptr;
				if (curmodel && curmodel->GetModelDisp() && curmodel->GetInView(0)) {
					if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {
						int elemnum = curmodel->GetDispGroupSize(groupindex);
						int elemno;
						for (elemno = 0; elemno < elemnum; elemno++) {

							CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);
							if (curobj && curobj->GetDispObj() && (curobj->GetPm3() || curobj->GetPm4()) &&
								curobj->GetVisible(0)) {

								myRenderer::RENDEROBJ pickobj;
								pickobj.Init();
								pickobj.pmodel = curmodel;
								pickobj.mqoobj = curobj;

								pickvec.push_back(pickobj);
							}
						}
					}
				}
			}
		}
	}

	//##########################################################
	//カメラ距離でソートしてから　pickする
	//##########################################################
	if (!pickvec.empty()) {
		int foundorder = 1;
		if (!pickvec.empty()) {
			std::sort(pickvec.begin(), pickvec.end());//カメラ距離でソート

			int pickvecsize = (int)pickvec.size();
			int pickindex;
			CMQOObject* befmqoobj = nullptr;
			CMQOMaterial* befmqomat = nullptr;
			for (pickindex = 0; pickindex < pickvecsize; pickindex++) {
				myRenderer::RENDEROBJ pickobj = pickvec[pickindex];
				CModel* curmodel = pickobj.pmodel;
				CMQOObject* curobj = pickobj.mqoobj;
				ChaVector3 curhitpos;
				curhitpos.SetParams(0.0f, 0.0f, 0.0f);
				if (curmodel && curobj) {
					int hitfaceindex = -1;
					int colli = 0;
					if (curobj->GetPm3() || curobj->GetPm4()) {
						colli = curobj->GetResultOfPickRay(&hitfaceindex, &curhitpos);
					}
					else {
						colli = 0;
					}
					if ((colli != 0) && (hitfaceindex >= 0)) {

						CMQOObject* chkmqoobj = curobj;
						CMQOMaterial* chkmqomat = curobj->GetMaterialByFaceIndex(hitfaceindex);

						//g_pickorderは数字キーを押して設定
						//カメラから何番目(数字キーの数字番目、0は10番目)に近いオブジェクトかを意味する
						if (g_pickorder > foundorder) {
							if (pickkind == NUMKEYPICK_MQOOBJECT) {
								if (chkmqoobj == befmqoobj) {
									continue;//前回と同じものを見つけた場合はfoundorderを変えずにcontinue.
								}
							}
							else if (pickkind == NUMKEYPICK_MQOMATERIAL) {
								if (chkmqomat == befmqomat) {
									continue;//前回と同じものを見つけた場合はfoundorderを変えずにcontinue.
								}
							}
							else {
								_ASSERT(0);
							}

							befmqoobj = chkmqoobj;
							befmqomat = chkmqomat;

							foundorder++;//前回と結果が異なり、かつorderが指定より小さい場合　foundorderを増やしてcontinue.
							continue;
						}
						else {
							//g_pickorder番目のオブジェクトを返す

							if (pickkind == NUMKEYPICK_MQOOBJECT) {
								if (chkmqoobj != befmqoobj) {
									*pickmodel = curmodel;
									*pickmqoobj = chkmqoobj;
									*pickmaterial = chkmqomat;
									//*tmppickinfo = pickinfo;
									*pickhitpos = curhitpos;
									return true;
								}
							}
							else if (pickkind == NUMKEYPICK_MQOMATERIAL) {
								if (chkmqomat != befmqomat) {
									*pickmodel = curmodel;
									*pickmqoobj = chkmqoobj;
									*pickmaterial = chkmqomat;
									//*tmppickinfo = pickinfo;
									*pickhitpos = curhitpos;
									return true;
								}
							}
							else {
								_ASSERT(0);
							}
						}
					}
				}
			}
		}
	}


	return false;
}


int ChaScene::RenderModels(myRenderer::RenderingEngine* renderingEngine, int lightflag, ChaVector4 diffusemult, int btflag)
{
	if (!renderingEngine) {
		_ASSERT(0);
		return 1;
	}

	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}

	bool calcslotflag;
	calcslotflag = false;


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

	vector<myRenderer::RENDEROBJ> rendervec;


	if (!m_modelindex.empty()) {
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		int renderindex;

		int resetmodelno;
		for (resetmodelno = 0; resetmodelno < modelnum; resetmodelno++) {
			CModel* resetmodel = m_modelindex[resetmodelno].modelptr;
			if (resetmodel) {
				resetmodel->ResetUpdateFl4x4Flag();
			}
		}



		//int renderslot = (int)(!(m_updateslot != 0));

		for (renderindex = 0; renderindex < 2; renderindex++) {

			bool withalpha = false;
			bool forcewithalpha = false;
			if (renderindex == 0) {
				withalpha = false;
			}
			else {
				withalpha = true;
			}

			int groupindex;
			for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {

				int rendervecReservedSize = 64;
				int rendervecSize = 0;
				rendervec.clear();//!!!!!!!!!!!
				rendervec.reserve(rendervecReservedSize);

				if (groupindex >= 1) {
					forcewithalpha = true;
				}
				else {
					forcewithalpha = false;
				}
				if (forcewithalpha && (renderindex == 0)) {
					continue;
				}

				for (modelindex = 0; modelindex < modelnum; modelindex++) {

					CModel* curmodel = m_modelindex[modelindex].modelptr;
					if (curmodel && (curmodel->GetRefPosFlag() == false) && 
						curmodel->GetModelDisp() && curmodel->GetInView(0) && (curmodel->GetGrassFlag() == false)) {//Grassは別レンダー
					//if (curmodel && curmodel->GetModelDisp()) {

						ChaVector4 materialdisprate = curmodel->GetMaterialDispRate();

						if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {

							int elemnum = curmodel->GetDispGroupSize(groupindex);

							rendervecSize += elemnum;
							if (rendervecSize > rendervecReservedSize) {
								rendervecReservedSize *= 2;
								rendervec.reserve(rendervecReservedSize);
							}

							int elemno;
							for (elemno = 0; elemno < elemnum; elemno++) {

								CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);

 								if (curobj && !curobj->IsND() && 
									(curobj->GetDispObj() || curobj->GetDispLine()) && curobj->GetVisible(0)) {
								//if (curobj) {

								
									if (curobj->GetDispLine()) {
										int dbgflag1 = 1;
									}

									if (forcewithalpha) {
										//groupno >= 2以降は強制半透明
									}
									else {
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
									}
									

									//m_renderingEngine->Add3DModelToZPrepass(curobj);
									//m_renderingEngine->Add3DModelToRenderGBufferPass(curobj);
									myRenderer::RENDEROBJ renderobj;
									renderobj.Init();
									renderobj.pmodel = curmodel;
									renderobj.mqoobj = curobj;
									//renderobj.shadertype = g_shadertype;//!!!!!!!!!!! シェーダの選択はDispParamsプレートメニューを押すと出る右ペインウインドウにて
									renderobj.shadertype = -2;//-2:materialの設定に従う印
									renderobj.withalpha = withalpha;
									renderobj.forcewithalpha = forcewithalpha;
									renderobj.lightflag = lightflag;
									renderobj.diffusemult = diffusemult;
									renderobj.materialdisprate = materialdisprate;
									renderobj.mWorld = curmodel->GetWorldMat().TKMatrix();
									renderobj.calcslotflag = calcslotflag;
									renderobj.btflag = btflag;
									if (curmodel->GetSkyFlag()) {
										renderobj.skyflag = true;
									}
									else {
										renderobj.skyflag = false;
									}

									//renderingEngine->Add3DModelToZPrepass(renderobj);
									
									if (g_enableshadow) {
										if ((curmodel->GetInShadow(0)) && (curobj->GetInShadow(0)) &&
											//(withalpha == false) && (forcewithalpha == false)) {
											(curobj->GetCancelShadow() == false)) {
											//renderingEngine->Add3DModelToRenderToShadowMap(renderobj);
											renderobj.renderkind = RENDERKIND_SHADOWMAP;											
										}
										else {
											//renderingEngine->Add3DModelToForwardRenderPass(renderobj);
											renderobj.renderkind = RENDERKIND_NORMAL;
										}
									}
									else {
										//renderingEngine->Add3DModelToForwardRenderPass(renderobj);
										renderobj.renderkind = RENDERKIND_NORMAL;
									}

									rendervec.push_back(renderobj);
								}
							}
						}
					}
				}

				//#################################################################
				//2024/03/30 不透明と半透明と混在で　遠くから描画することで色がブレンドする
				//#################################################################
				if (!rendervec.empty()) {
					std::sort(rendervec.begin(), rendervec.end());//カメラ距離でソート 近い順に並ぶ
					std::reverse(rendervec.begin(), rendervec.end());//半透明は遠くから順番に描画 遠い順に並ぶ
					Add3DModelToForwardRenderPass(rendervec);
				}


			}

			RenderRefPos(renderingEngine, (renderindex == 0));//2024/03/24
		}

		//#########################################################################################################################
		//2023/11/01
		//姿勢データをダブルバッファ化した
		// m_updateslotで計算中と描画用を識別
		// 
		//UpdateMatrixModels()関数はスレッド終了を待たずに　Render関数を呼び　Render関数の終わりでアップデートスレッドを待って同期
		//Renderはm_updateslotとは違う側の計算済のデータを参照する
		//#########################################################################################################################


		//##########################################################
		//2023/11/29
		//GraphicsEngine::EndRender()内のm_swapChain->Present()の前で
		//WaitForUpdateMatrixModels()を呼ぶことにした
		//##########################################################
		////Render中は同時進行し　Render後に待つことで　ダブルバッファ同期!!!!!!!!!!!!!!!!
		//if ((g_previewFlag != 0) && (g_previewFlag != 4)) {
		//	WaitUpdateThreads();
		//}
		//else if (g_previewFlag == 4) {
		//	WaitSetBtMotionFinished();//!!!!!!!!!!!!!!!!
		//}
		//else {
		//	//
		//	//g_previewFlag == 0
		//	// 
		//	// 
		//	//2023/11/09 1.2.0.30 RC1
		//	//プレビュー中以外に　タイムラインのフレームを移動する場合に　前フレーム以前の表示が気になるので
		//	//プレビュー中以外のときには同期する(UpdaetMatrixModelsの終わりで終了待機)　Render()においてはcalcslotflag=trueで描画
		//}
	}

	return 0;
}

void ChaScene::WaitForUpdateMatrixModels()
{
	//Render中は同時進行し　Render後に待つことで　ダブルバッファ同期!!!!!!!!!!!!!!!!

	//##########################################################
	//2023/11/29
	//GraphicsEngine::EndRender()内のm_swapChain->Present()の前で
	//WaitForUpdateMatrixModels()を呼ぶことにした
	//##########################################################

	//if ((g_previewFlag != 0) && (g_previewFlag != 4)) {
	if (g_previewFlag != 4) {//2024/03/12
		WaitUpdateThreads();
	}
	//else if (g_previewFlag == 4) {
	else {//2024/03/12
		WaitSetBtMotionFinished();//!!!!!!!!!!!!!!!!
	}
	//else {
	//	//
	//	//g_previewFlag == 0
	//	// 
	//	// 
	//	//2023/11/09 1.2.0.30 RC1
	//	//プレビュー中以外に　タイムラインのフレームを移動する場合に　前フレーム以前の表示が気になるので
	//	//プレビュー中以外のときには同期する(UpdaetMatrixModelsの終わりで終了待機)　Render()においてはcalcslotflag=trueで描画
	//}

}

int ChaScene::RenderOneModel(CModel* srcmodel, bool forcewithalpha,
	myRenderer::RenderingEngine* renderingEngine, 
	int lightflag, ChaVector4 diffusemult, int btflag, 
	bool zcmpalways, bool zenable,
	int refposindex)
//default:refposindex = 0
{
	if (!renderingEngine) {
		_ASSERT(0);
		return 1;
	}


	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}

	if (!srcmodel) {
		return 0;
	}
	CModel* curmodel = srcmodel;


	bool calcslotflag;
	calcslotflag = false;


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

	//vector<myRenderer::RENDEROBJ> rendervec;

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

		if ((forcewithalpha == true) && (renderindex == 0)) {
			continue;
		}


		int groupindex;
		for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {

			//rendervec.clear();//!!!!!!!!!!!!!!!

			if (curmodel && curmodel->GetModelDisp() && curmodel->GetInView(0)) {
				//if (curmodel && curmodel->GetModelDisp()) {

				ChaVector4 materialdisprate = curmodel->GetMaterialDispRate();

				if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {

					int elemnum = curmodel->GetDispGroupSize(groupindex);
					int elemno;
					for (elemno = 0; elemno < elemnum; elemno++) {

						CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);

						if (curobj && !curobj->IsND() && 
							(curobj->GetDispObj() || curobj->GetDispLine()) && curobj->GetVisible(0)) {
							//if (curobj) {


							if (curobj->GetDispLine()) {
								int dbgflag1 = 1;
							}


							if (forcewithalpha == true && renderindex == 1) {
								//強制的に半透明として描画
							}
							else {
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
							}

							//m_renderingEngine->Add3DModelToZPrepass(curobj);
							//m_renderingEngine->Add3DModelToRenderGBufferPass(curobj);
							myRenderer::RENDEROBJ renderobj;
							renderobj.Init();
							renderobj.pmodel = curmodel;
							renderobj.mqoobj = curobj;
							if (curmodel->GetSkyFlag() == false) {
								renderobj.shadertype = MQOSHADER_TOON;//!!!!!!!!!!! マニピュレータと地面はNOLIGHTで表示
							}
							else {
								//2024/03/07
								renderobj.shadertype = -2;//!!!!!!!!!!! skyのシェーダタイプについては、マテリアルの指定に従う
							}
							renderobj.withalpha = withalpha;
							renderobj.forcewithalpha = forcewithalpha;
							renderobj.lightflag = lightflag;
							renderobj.diffusemult = diffusemult;
							renderobj.materialdisprate = materialdisprate;
							renderobj.mWorld = curmodel->GetWorldMat().TKMatrix();
							renderobj.calcslotflag = calcslotflag;
							renderobj.btflag = btflag;
							renderobj.zcmpalways = zcmpalways;
							renderobj.zenable = zenable;//2024/02/08
							//renderingEngine->Add3DModelToForwardRenderPass(renderobj);

							renderobj.renderkind = RENDERKIND_NORMAL;
							renderobj.refposindex = refposindex;//2024/02/06
							if (curmodel->GetSkyFlag()) {
								renderobj.skyflag = true;
							}
							else {
								renderobj.skyflag = false;
							}


							m_forwardRenderModels.push_back(renderobj);
						}
					}
				}
			}

			//if (!rendervec.empty()) {
			//	//std::sort(rendervec.begin(), rendervec.end());//カメラ距離でソート
			////	//if (groupindex != 0) {
			//	//	std::reverse(rendervec.begin(), rendervec.end());//半透明は遠くから順番に描画
			//	//}
			//	Add3DModelToForwardRenderPass(rendervec);
			//}
		}
	}

	return 0;
}

int ChaScene::RenderInstancingModel(CModel* srcmodel, bool forcewithalpha,
	int lightflag, ChaVector4 diffusemult, int btflag, 
	bool zcmpalways, bool zenable,
	int renderkind)
{
	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}

	if (!srcmodel) {
		return 0;
	}
	CModel* curmodel = srcmodel;


	bool calcslotflag;
	calcslotflag = false;


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

		if ((forcewithalpha == true) && (renderindex == 0)) {
			continue;
		}


		int groupindex;
		for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {
			//if (curmodel && curmodel->GetModelDisp() && curmodel->GetInView()) {
			if (curmodel && curmodel->GetModelDisp()) {//剛体マーク、ジョイントマークは視野内判定無し

				ChaVector4 materialdisprate = curmodel->GetMaterialDispRate();

				if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {

					int elemnum = curmodel->GetDispGroupSize(groupindex);
					int elemno;
					for (elemno = 0; elemno < elemnum; elemno++) {

						CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);

						if (curobj && !curobj->IsND() && 
							(curobj->GetDispObj() || curobj->GetDispLine()) && curobj->GetVisible(0)) {
							//if (curobj) {


							if (curobj->GetDispLine()) {
								int dbgflag1 = 1;
							}


							if (forcewithalpha == true && renderindex == 1) {
								//強制的に半透明として描画
							}
							else {
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
							}

							//m_renderingEngine->Add3DModelToZPrepass(curobj);
							//m_renderingEngine->Add3DModelToRenderGBufferPass(curobj);
							myRenderer::RENDEROBJ renderobj;
							renderobj.Init();
							renderobj.renderkind = renderkind;//2024/02/08 INSTANCINGのTRIANGLEとLINEを選択可能
							renderobj.pmodel = curmodel;
							renderobj.mqoobj = curobj;
							renderobj.shadertype = MQOSHADER_TOON;//!!!!!!!!!!! マニピュレータと地面はNOLIGHTで表示
							renderobj.withalpha = withalpha;
							renderobj.forcewithalpha = forcewithalpha;
							renderobj.lightflag = lightflag;
							renderobj.diffusemult = diffusemult;
							renderobj.materialdisprate = materialdisprate;
							renderobj.mWorld = curmodel->GetWorldMat().TKMatrix();
							renderobj.calcslotflag = calcslotflag;
							renderobj.btflag = btflag;
							renderobj.zcmpalways = zcmpalways;
							renderobj.zenable = zenable;//2024/02/08
							//Add3DModelToInstancingRenderPass(renderobj);
							m_instancingRenderModels.push_back(renderobj);
						}
					}
				}
			}
		}
	}

	return 0;
}


int ChaScene::ResetRefPos()
{
	m_refpos_opaque.clear();
	m_refpos_transparent.clear();
	return 0;
}

int ChaScene::AddToRefPos(CModel* srcmodel, bool forcewithalpha, myRenderer::RenderingEngine* renderingEngine,
	int lightflag, ChaVector4 diffusemult, int btflag,
	bool zcmpalways, bool zenable,
	int refposindex)
{

	if (!renderingEngine) {
		_ASSERT(0);
		return 1;
	}


	if (g_changeUpdateThreadsNum) {
		//アップデート用スレッド数を変更中
		return 0;
	}

	if (!srcmodel) {
		return 0;
	}
	CModel* curmodel = srcmodel;


	bool calcslotflag;
	calcslotflag = false;

	vector<myRenderer::RENDEROBJ> rendervec;

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

		if ((forcewithalpha == true) && (renderindex == 0)) {
			continue;
		}


		int groupindex;
		for (groupindex = 0; groupindex < MAXDISPGROUPNUM; groupindex++) {

			rendervec.clear();//!!!!!!!!!!!!!!!

			if (curmodel && curmodel->GetModelDisp() && curmodel->GetInView(refposindex)) {
				//if (curmodel && curmodel->GetModelDisp()) {

				ChaVector4 materialdisprate = curmodel->GetMaterialDispRate();

				if (!(curmodel->DispGroupEmpty(groupindex)) && curmodel->GetDispGroupON(groupindex)) {

					int elemnum = curmodel->GetDispGroupSize(groupindex);
					int elemno;
					for (elemno = 0; elemno < elemnum; elemno++) {

						CMQOObject* curobj = curmodel->GetDispGroupMQOObject(groupindex, elemno);

						if (curobj && !curobj->IsND() &&
							(curobj->GetDispObj() || curobj->GetDispLine()) && curobj->GetVisible(refposindex)) {
							//if (curobj) {


							if (curobj->GetDispLine()) {
								int dbgflag1 = 1;
							}


							if (forcewithalpha == true && renderindex == 1) {
								//強制的に半透明として描画
							}
							else {
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
							}

							//m_renderingEngine->Add3DModelToZPrepass(curobj);
							//m_renderingEngine->Add3DModelToRenderGBufferPass(curobj);
							myRenderer::RENDEROBJ renderobj;
							renderobj.Init();
							renderobj.pmodel = curmodel;
							renderobj.mqoobj = curobj;
							if (curmodel->GetSkyFlag() == false) {
								renderobj.shadertype = MQOSHADER_TOON;//!!!!!!!!!!! マニピュレータと地面はNOLIGHTで表示
							}
							else {
								//2024/03/07
								renderobj.shadertype = -2;//!!!!!!!!!!! skyのシェーダタイプについては、マテリアルの指定に従う
							}
							renderobj.withalpha = withalpha;
							renderobj.forcewithalpha = forcewithalpha;
							renderobj.lightflag = lightflag;
							renderobj.diffusemult = diffusemult;
							renderobj.materialdisprate = materialdisprate;
							renderobj.mWorld = curmodel->GetWorldMat().TKMatrix();
							renderobj.calcslotflag = calcslotflag;
							renderobj.btflag = btflag;
							renderobj.zcmpalways = zcmpalways;
							renderobj.zenable = zenable;//2024/02/08
							//renderingEngine->Add3DModelToForwardRenderPass(renderobj);

							renderobj.renderkind = RENDERKIND_NORMAL;
							renderobj.refposindex = refposindex;//2024/02/06
							
							
							//rendervec.push_back(renderobj);
							
							if (renderindex == 0) {
								m_refpos_opaque.push_back(renderobj);
							}
							else {
								m_refpos_transparent.push_back(renderobj);
							}
						}
					}
				}
			}
		}
	}

	return 0;
}
int ChaScene::RenderRefPos(myRenderer::RenderingEngine* renderingEngine, bool opaqueflag)
{
	if (opaqueflag) {
		if (!m_refpos_opaque.empty()) {
			std::sort(m_refpos_opaque.begin(), m_refpos_opaque.end());//カメラ距離でソート
			//不透明は近くから順に描画
			Add3DModelToForwardRenderPass(m_refpos_opaque);
		}
	}
	else {
		if (!m_refpos_transparent.empty()) {
			std::sort(m_refpos_transparent.begin(), m_refpos_transparent.end());//カメラ距離でソート
			std::reverse(m_refpos_transparent.begin(), m_refpos_transparent.end());//半透明は遠くから順番に描画
			Add3DModelToForwardRenderPass(m_refpos_transparent);
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
			mb.Init();
			curmodel->GetModelBound(&mb);//計算する　重い
			if ((mb.r != 0.0f) &&  mb.IsValid()) {
				AddModelBound(&m_totalmb, &mb);
			}
		}
	}

	return 0;
}

int ChaScene::AddModelBound(MODELBOUND* mb, MODELBOUND* addmb)
{
	if (!mb || !addmb) {
		_ASSERT(0);
		return 1;
	}
	if (addmb->IsValid() == false) {
		return 0;
	}

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

	mb->SetIsValid(true);

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


int ChaScene::DelModel(int srcmodelindex, std::vector<CGrassElem*>& grasselemvec)
{
	if (m_modelindex.empty()) {
		return 0;
	}
	else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {

		CModel* delmodel;
		delmodel = GetModel(srcmodelindex);
		if (delmodel) {

			std::vector<CGrassElem*> newgrasselemvec;
			if (delmodel->GetGrassFlag()) {
				std::vector<CGrassElem*>::iterator itrgrass;
				for (itrgrass = grasselemvec.begin(); itrgrass != grasselemvec.end(); itrgrass++) {
					CGrassElem* curgrass = *itrgrass;
					if (curgrass) {
						if (curgrass->GetGrass() != delmodel) {
							newgrasselemvec.push_back(curgrass);
						}
						else {
							delete curgrass;//2024/05/13
						}
					}
				}
				grasselemvec = newgrasselemvec;//2024/06/22
			}
			//grasselemvec = newgrasselemvec;//2024/06/22 この場所でのセットは間違い delmodel->GetGrassFlag()のif内へ移動


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
			if (m_modelindex[modelno].modelptr->ExistCurrentMotion()) {
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

int ChaScene::Motion2Bt(bool secondcall, bool limitdegflag, bool updatematrixflag, 
	double nextframe, 
	ChaMatrix* pmView, ChaMatrix* pmProj, int loopstartflag)
{
	if (!pmView || !pmProj) {
		_ASSERT(0);
		return 1;
	}

	if (!m_modelindex.empty()) {

		if (updatematrixflag) {//2024/05/02 LimitEulオンのSetBtMotionの後で呼ぶMotion2Bt呼び出しでは　updatematrixflag = false
			//2024/04/06
			// スレッドから呼び出されたCModel::Motion2Bt()からCModel::UpdateMatrixを呼び出していた
			// CModel::UpdateMatrixのChkInViewを複数スレッドから同時に呼び出すことは出来ない
			// CModel::UpdateMatrixのChkInViewをマルチスレッド呼び出しすると即時実行コンピュートシェーダ関連のメモリエラーで落ちる
			//(CBone::UpdateMatrixはコンテクスト限定でマルチスレッド可能)
			//CModel::Motion2Bt()からのCModel::UpdateMatrix呼び出しをやめて、ここでCModel::UpdateMatrixを呼び出して済ませることに
			int modelnum = (int)m_modelindex.size();
			int modelindex;
			for (modelindex = 0; modelindex < modelnum; modelindex++) {
				CModel* curmodel = m_modelindex[modelindex].modelptr;
				if (curmodel && (curmodel->ExistCurrentMotion() == true)) {
					ChaMatrix wmat = curmodel->GetWorldMat();
					bool needwait = true;
					int refposindex = 0;
					curmodel->UpdateMatrix(limitdegflag, &wmat, pmView, pmProj, needwait, refposindex);
				}
			}
		}


		{
			int modelnum = (int)m_modelindex.size();
			int modelindex;
			for (modelindex = 0; modelindex < modelnum; modelindex++) {
				CModel* curmodel = m_modelindex[modelindex].modelptr;
				if (curmodel && (curmodel->ExistCurrentMotion() == true)) {
					curmodel->SetSecondCallOfMotion2Bt(secondcall);
				}
			}
		}

		if (m_Motion2BtThreads) {
			int updatecount;
			for (updatecount = 0; updatecount < m_created_Motion2BtThreadsNum; updatecount++) {
				CThreadingMotion2Bt* curupdate = m_Motion2BtThreads + updatecount;
				curupdate->Motion2Bt(limitdegflag, nextframe, 
					pmView, pmProj, loopstartflag);//, m_updateslot);
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


int ChaScene::SetBtMotion(bool limitdegflag, double nextframe, 
	ChaMatrix* pmView, ChaMatrix* pmProj, CModel* smodel, double srcreccnt)
{
	if (!pmView || !pmProj) {
		_ASSERT(0);
		return 1;
	}


	if (!m_modelindex.empty()) {
		//2024/04/06
		// モーションが無いモデルの場合にはcurupdate->SetBtMotionからCModel::UpdateMatrixだけを呼び出していた
		// CModel::UpdateMatrixのChkInViewを複数スレッドから同時に呼び出すことは出来ない
		// CModel::UpdateMatrixのChkInViewをマルチスレッド呼び出しすると即時実行コンピュートシェーダ関連のメモリエラーで落ちる
		//(CBone::UpdateMatrixはコンテクスト限定でマルチスレッド可能)
		//モーションが無い場合については呼び出し元のシングルスレッドでCModel::UpdateMatrixを呼び出して済ませることに
		int modelnum = (int)m_modelindex.size();
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* curmodel = m_modelindex[modelindex].modelptr;
			if (curmodel && (curmodel->ExistCurrentMotion() == false)) {
				ChaMatrix wmat = curmodel->GetWorldMat();
				bool needwait = true;
				int refposindex = 0;
				curmodel->UpdateMatrix(limitdegflag, &wmat, pmView, pmProj, needwait, refposindex);
			}
		}

		if (m_SetBtMotionThreads) {
			int updatecount;
			for (updatecount = 0; updatecount < m_created_SetBtMotionThreadsNum; updatecount++) {
				CThreadingSetBtMotion* curupdate = m_SetBtMotionThreads + updatecount;
				curupdate->SetBtMotion(limitdegflag, nextframe, pmView, pmProj, smodel, srcreccnt);//, m_updateslot);
			}
			//WaitSetBtMotionFinished();//レンダー中に計算し　レンダー後に待機するので　コメントアウト

			if (limitdegflag) {
				WaitSetBtMotionFinished();//2024/05/02 LimitEulオンの時には　この処理の後にも処理をするので同期する必要がある
			}

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


int ChaScene::UpdateBtFunc(bool limitdegflag, double nextframe, 
	ChaMatrix* pmView, ChaMatrix* pmProj, int loopstartflag,
	CModel* smodel, bool recstopflag, BPWorld* bpWorld, double srcreccnt,
	int (*srcStopBtRec)())
{
	if (!pmView || !pmProj || !smodel || !bpWorld || !srcStopBtRec) {
		_ASSERT(0);
		return 1;
	}

	bool secondcall = false;
	bool updatematrixflag = true;
	Motion2Bt(secondcall, limitdegflag, updatematrixflag, 
		nextframe, pmView, pmProj, loopstartflag);//MultiThreading per CModel. Wait threads on return.

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

		SetBtMotion(limitdegflag, nextframe, pmView, pmProj, smodel, srcreccnt);//MultiThreading per CModel, Not Wait threads on return.


		if (limitdegflag) {
			bool secondcall2 = true;
			bool updatematrixflag2 = false;
			Motion2Bt(secondcall2, limitdegflag, updatematrixflag2,
				nextframe, pmView, pmProj, loopstartflag);//MultiThreading per CModel. Wait threads on return.
		}

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


int ChaScene::SetBoneMatrixForShader(int btflag, bool calcslotflag)
{
	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			curmodel->SetShaderConst(btflag, calcslotflag);
		}
	}
	return 0;
}


int ChaScene::CopyCSDeform()
{
	int rendernum = GetForwardRenderObjNum();
	int renderindex;
	for (renderindex = 0; renderindex < rendernum; renderindex++) {
		myRenderer::RENDEROBJ currenderobj = GetForwardRenderObj(renderindex);
		if (currenderobj.pmodel && currenderobj.mqoobj) {

			if (currenderobj.mqoobj->GetDispObj()) {
				if (currenderobj.mqoobj->GetPm3()) {
					currenderobj.mqoobj->GetDispObj()->CopyCSDeform();
				}
				else if (currenderobj.mqoobj->GetPm4()) {
					currenderobj.mqoobj->GetDispObj()->CopyCSDeform();
				}
			}
			else if (currenderobj.mqoobj->GetDispLine() && currenderobj.mqoobj->GetExtLine()) {
				//################################
				//GetDispObj()ではなくGetDispLine()
				//################################
			}
		}
	}
	return 0;
}

int ChaScene::InitializeBoneAxisKind()
{
	bool existvroid = false;
	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			existvroid = curmodel->GetVRoidJointName();
			if (existvroid) {
				break;
			}
		}
	}


	if (existvroid) {
		//2024/04/22 ボーンの軸としてBONEAXIS_BINDPOSEを指定することにより
		//オイラーグラフの軸とIK回転の軸とボーンの軸が一致する
		//VRoidの場合にはバインドポーズとしてボーン軸が設定されているのでBONEAXIS_BINDPOSEの方が使いやすい
		//(Rigの軸指定の種類のNODE*とも一致するようになる)
		g_boneaxis = BONEAXIS_BINDPOSE;
	}
	else {
		//2024/04/22 ボーン軸としてBONEAXIS_CURRENTを指定すると
		//RokDeBone2のようにバインドポーズとしてグローバル軸が設定されている場合に
		//IK操作をボーン軸で行うことが可能
		g_boneaxis = BONEAXIS_CURRENT;
	}

	return 0;
}


int ChaScene::ChangeAngleSpringScale()
{

	//2024/04/23
	//全ての剛体の回転バネ定数に対するスケールをbulletphysicsに反映させるために
	//CreateBtObjectを呼び出す

	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			curmodel->CreateBtObject(g_limitdegflag, 1);
		}
	}

	return 0;
}

int ChaScene::SetRefPosFlag(CModel* srcmodel)
{
	//2024/05/29
	//srcmodelに対してtrue, 他のモデルに対してfalseを設定
	//srcmodel == nulptrの場合には全てのモデルに対してfalseを設定

	vector<MODELELEM>::iterator itrmodel;
	for (itrmodel = m_modelindex.begin(); itrmodel != m_modelindex.end(); itrmodel++) {
		CModel* curmodel = itrmodel->modelptr;
		if (curmodel) {
			if (srcmodel && (curmodel == srcmodel)) {
				curmodel->SetRefPosFlag(true);
			}
			else {
				curmodel->SetRefPosFlag(false);
			}
		}
	}
	return 0;
}

int ChaScene::MotID2SelIndex(int srcmodelindex, int srcmotid)
{
	int modelindexnum = (int)m_modelindex.size();
	if ((srcmodelindex >= 0) && (srcmodelindex < modelindexnum)) {
		MODELELEM modelelem = m_modelindex[srcmodelindex];
		if (modelelem.modelptr) {
			int timelinenum = (int)modelelem.tlarray.size();
			int tlindex;
			for (tlindex = 0; tlindex < timelinenum; tlindex++) {
				TLELEM tlelem = modelelem.tlarray[tlindex];
				if (tlelem.motionid == srcmotid) {
					return tlelem.menuindex;//!!!!!!!!!!!!!!!!
				}
			}
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
	
	return -1;
}