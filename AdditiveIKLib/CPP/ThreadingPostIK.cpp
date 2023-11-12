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

#include <Model.h>
#include <Bone.h>
#include <MotionPoint.h>
#include <ChaVecCalc.h>

#define DBGH
#include <dbg.h>

#include <string>

#include <fbxsdk.h>
#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <fbxsdk/scene/animation/fbxanimevaluator.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

#include <io.h>

//for __nop()
#include <intrin.h>

#include <ThreadingPostIK.h>
#include <GlobalVar.h>

using namespace std;

extern int g_previewFlag;


CThreadingPostIK::CThreadingPostIK()
{
	InitParams();
}
int CThreadingPostIK::InitParams()
{
	CThreadingBase::InitParams();

	m_model = 0;

	limitdegflag = 0;
	erptr = 0;
	keyno = 0;
	rotbone = 0;
	parentbone = 0;
	motid = 0;
	startframe = 0.0;
	applyframe = 0.0;
	rotq0.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	keynum1flag = false;
	postflag = true;
	fromiktarget = false;

	ClearFrameList();

	return 0;
}
CThreadingPostIK::~CThreadingPostIK()
{
	DestroyObjs();
}

void CThreadingPostIK::DestroyObjs()
{
	CThreadingBase::DestroyObjs();
	ClearFrameList();
}

int CThreadingPostIK::ThreadFunc()
{
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}

	while (InterlockedAdd(&m_exit_state, 0) != 1) {

		//if ((g_HighRpmMode == true) && 
		//	((g_previewFlag != 0) || (g_underIKRot == true))) {//プレビュー中　または　IK回転中　だけ



		//if ((g_underIKRot == true) && (g_underIKRotApplyFrame == false)) {
		if (m_model && m_model->GetUnderIKRot() && (m_model->GetUnderIKRotApplyFrame() == false)) {
		//if (m_model && m_model->GetUnderPostIK()) {
		
			//###########################
			// 高回転モード　: High rpm
			//###########################

			if (InterlockedAdd(&m_start_state, 0) == 1) {//計算開始命令をキャッチ
				if (InterlockedAdd(&m_exit_state, 0) != 1) {//スレッドが終了していない場合

					if (m_model && (m_model->GetInView() == true)) {
						//EnterCriticalSection(&m_CritSection);//再入防止 呼び出し側で処理終了を待つので不要
						if (!m_framenovec.empty()) {
							int framenum = (int)m_framenovec.size();
							int frameindex;
							for (frameindex = 0; frameindex < framenum; frameindex++) {
								ChaCalcFunc chacalcfunc;
								double curframe = m_framenovec[frameindex];
								if (curframe != applyframe) {
									int dummykeyno;
									if (curframe == 0.0) {
										dummykeyno = 0;
									}
									else {
										dummykeyno = 1;
									}

									chacalcfunc.IKRotateOneFrame(m_model, limitdegflag, erptr,
										dummykeyno,
										rotbone, parentbone,
										motid, curframe, startframe, applyframe,
										rotq0, keynum1flag, postflag, fromiktarget);
								}
							}
						}
					}
					
					InterlockedExchange(&m_start_state, 0L);
					//LeaveCriticalSection(&m_CritSection);
				}
				else {
					//スレッド終了フラグが立っていた場合
					InterlockedExchange(&m_start_state, 0L);
				}
			}
			else {

				//計算は終了していて計算命令待ち状態

				//__nop();
				//Sleep(0);
				timeBeginPeriod(1);
				SleepEx(0, TRUE);
				//SleepEx(1, TRUE);
				timeEndPeriod(1);
			}

		}
		else {

			//############################
			// eco モード
			//############################

			DWORD dwWaitResult = WaitForSingleObject(m_hEvent, INFINITE);
			ResetEvent(m_hEvent);
			if (InterlockedAdd(&m_exit_state, 0) != 1) {//スレッドが終了していない場合
				switch (dwWaitResult)
				{
					// Event object was signaled
				case WAIT_OBJECT_0:
				{
					if (m_model && (m_model->GetInView() == true)) {
						EnterCriticalSection(&m_CritSection);
						if (!m_framenovec.empty()) {
							int framenum = (int)m_framenovec.size();
							int frameindex;
							for (frameindex = 0; frameindex < framenum; frameindex++) {
								ChaCalcFunc chacalcfunc;
								double curframe = m_framenovec[frameindex];

								if (curframe != applyframe) {
									int dummykeyno;
									if (curframe == 0.0) {
										dummykeyno = 0;
									}
									else {
										dummykeyno = 1;
									}

									chacalcfunc.IKRotateOneFrame(m_model, limitdegflag, erptr,
										dummykeyno,
										rotbone, parentbone,
										motid, curframe, startframe, applyframe,
										rotq0, keynum1flag, postflag, fromiktarget);
								}
							}
						}
						InterlockedExchange(&m_start_state, 0L);
						LeaveCriticalSection(&m_CritSection);
					}
					else {
						InterlockedExchange(&m_start_state, 0L);
					}
				}
				break;

				// An error occurred
				default:
					//printf("Wait error (%d)\n", GetLastError());
					//return 0;
					break;
				}
			}
			else {
				//スレッド終了フラグが立っていた場合
				InterlockedExchange(&m_start_state, 0L);
			}
		}
	}

	if (m_hExitEvent != NULL) {
		SetEvent(m_hExitEvent);
	}

	return 0;
}


int CThreadingPostIK::ClearFrameList()
{
	m_framenovec.clear();

	return 0;
}
int CThreadingPostIK::SetModel(CModel* srcmodel)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	m_model = srcmodel;
	return 0;
}
int CThreadingPostIK::AddFramenoList(double srcframeno)
{
	if (srcframeno < 0.0) {
		_ASSERT(0);
		return 1;
	}

	m_framenovec.push_back(srcframeno);
	return 0;
}

void CThreadingPostIK::IKRotateOneFrame(CModel* srcmodel, int srclimitdegflag, CEditRange* srcerptr,
	int srckeyno, CBone* srcrotbone, CBone* srcparentbone,
	int srcmotid, double srcstartframe, double srcapplyframe,
	CQuaternion srcrotq0, bool srckeynum1flag, bool srcpostflag, bool srcfromiktarget)
{

	if (!m_model) {
		return;
	}
	if (m_model && (m_model->GetInView() == false)) {
		return;
	}

	if (!m_framenovec.empty()) {
		EnterCriticalSection(&m_CritSection);

		limitdegflag = srclimitdegflag;
		erptr = srcerptr;
		keyno = srckeyno;
		rotbone = srcrotbone;
		parentbone = srcparentbone;
		motid = srcmotid;
		startframe = srcstartframe;
		applyframe = srcapplyframe;
		rotq0 = srcrotq0;
		keynum1flag = srckeynum1flag;
		postflag = srcpostflag;
		fromiktarget = srcfromiktarget;

		LeaveCriticalSection(&m_CritSection);
		InterlockedExchange(&m_start_state, 1L);
		SetEvent(m_hEvent);
	}
	else {
		InterlockedExchange(&m_start_state, 0L);
	}


}



