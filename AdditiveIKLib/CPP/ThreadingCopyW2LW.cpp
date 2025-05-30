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
#include <ChaCalcFunc.h>

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

#include <ThreadingCopyW2LW.h>
#include <GlobalVar.h>

using namespace std;

extern int g_previewFlag;


CThreadingCopyW2LW::CThreadingCopyW2LW()
{
	InitParams();
}
int CThreadingCopyW2LW::InitParams()
{
	CThreadingBase::InitParams();

	m_model = 0;

	pbone = 0;
	motid = 0;
	startframe = 1.0;
	endframe = 1.0;

	ClearFrameList();

	return 0;
}
CThreadingCopyW2LW::~CThreadingCopyW2LW()
{
	DestroyObjs();
}

void CThreadingCopyW2LW::DestroyObjs()
{
	CThreadingBase::DestroyObjs();
	ClearFrameList();
}

int CThreadingCopyW2LW::ThreadFunc()
{
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}

	while (InterlockedAdd(&m_exit_state, 0) != 1) {

		//if ((g_HighRpmMode == true) && 
		//	((g_previewFlag != 0) || (g_underIKRot == true))) {//プレビュー中　または　IK回転中　だけ


		//if (g_underCopyW2LW) {
		if (m_model && m_model->GetUnderCopyW2LW()) {

			//###########################
			// 高回転モード　: High rpm
			//###########################

			if (InterlockedAdd(&m_start_state, 0) == 1) {//計算開始命令をキャッチ
				if (InterlockedAdd(&m_exit_state, 0) != 1) {//スレッドが終了していない場合

					if (m_model && (m_model->GetInView(0) == true)) {
						//EnterCriticalSection(&m_CritSection);//再入防止 呼び出し側で処理終了を待つので不要
						if (!m_framenovec.empty()) {
							int framenum = (int)m_framenovec.size();
							int frameindex;
							for (frameindex = 0; frameindex < framenum; frameindex++) {
								ChaCalcFunc chacalcfunc;

								double curframe = RoundingTime(m_framenovec[frameindex]);;
								chacalcfunc.CopyWorldToLimitedWorldOne(m_model, pbone, motid, curframe);
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
					if (m_model && (m_model->GetInView(0) == true)) {
						EnterCriticalSection(&m_CritSection);
						if (!m_framenovec.empty()) {
							int framenum = (int)m_framenovec.size();
							int frameindex;
							for (frameindex = 0; frameindex < framenum; frameindex++) {
								ChaCalcFunc chacalcfunc;

								double curframe = RoundingTime(m_framenovec[frameindex]);;
								chacalcfunc.CopyWorldToLimitedWorldOne(m_model, pbone, motid, curframe);
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


int CThreadingCopyW2LW::ClearFrameList()
{
	m_framenovec.clear();

	return 0;
}
int CThreadingCopyW2LW::SetModel(CModel* srcmodel)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	m_model = srcmodel;
	return 0;
}
int CThreadingCopyW2LW::AddFramenoList(double srcframeno)
{
	if (srcframeno < 0.0) {
		_ASSERT(0);
		return 1;
	}

	m_framenovec.push_back(srcframeno);
	return 0;
}

void CThreadingCopyW2LW::CopyWorldToLimitedWorld(CBone* srcbone, int srcmotid, double srcstartframe, double srcendframe)
{

	if (!m_model) {
		return;
	}
	if (m_model && (m_model->GetInView(0) == false)) {
		return;
	}

	if (!m_framenovec.empty()) {
		EnterCriticalSection(&m_CritSection);

		pbone = srcbone;
		motid = srcmotid;
		startframe = srcstartframe;
		endframe = srcendframe;

		LeaveCriticalSection(&m_CritSection);
		InterlockedExchange(&m_start_state, 1L);
		SetEvent(m_hEvent);
	}
	else {
		InterlockedExchange(&m_start_state, 0L);
	}


}



