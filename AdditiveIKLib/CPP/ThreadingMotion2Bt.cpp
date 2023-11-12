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

#include <ThreadingMotion2Bt.h>
#include <GlobalVar.h>

using namespace std;

extern int g_previewFlag;


CThreadingMotion2Bt::CThreadingMotion2Bt()
{
	InitParams();
}
int CThreadingMotion2Bt::InitParams()
{
	CThreadingBase::InitParams();

	m_model = 0;

	limitdegflag = 0;
	nextframe = 0.0;
	matVP.SetIdentity();
	loopstartflag = 0;
	updateslot = 0;

	return 0;
}
CThreadingMotion2Bt::~CThreadingMotion2Bt()
{
	DestroyObjs();
}

void CThreadingMotion2Bt::DestroyObjs()
{
	CThreadingBase::DestroyObjs();
}

int CThreadingMotion2Bt::ThreadFunc()
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
		if (m_model && m_model->GetUnderMotion2Bt()) {
		
			//###########################
			// 高回転モード　: High rpm
			//###########################

			if (InterlockedAdd(&m_start_state, 0) == 1) {//計算開始命令をキャッチ
				if (InterlockedAdd(&m_exit_state, 0) != 1) {//スレッドが終了していない場合

					//if (m_model && (m_model->GetInView() == true)) {
					if (m_model) {
						//EnterCriticalSection(&m_CritSection);//再入防止 呼び出し側で処理終了を待つので不要
						if ((m_model->GetBtCnt() != 0) && (loopstartflag == 1)) {
							m_model->ZeroBtCnt();
						}
						else {
							if (m_model->GetCurMotInfo()) {
								m_model->Motion2Bt(limitdegflag, nextframe, &matVP, updateslot);
							}
							m_model->PlusPlusBtCnt();
						}
					}
					
					InterlockedExchange(&m_start_state, 0L);
					//LeaveCriticalSection(&m_CritSection);
				}
				else {
					//スレッド終了フラグが立っていた場合
					InterlockedExchange(&m_start_state, 0L);
				}

				if (m_model) {
					m_model->SetUnderMotion2Bt(false);//!!!!!!!!
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
					//if (m_model && (m_model->GetInView() == true)) {
					if (m_model) {
						EnterCriticalSection(&m_CritSection);
						if ((m_model->GetBtCnt() != 0) && (loopstartflag == 1)) {
							m_model->ZeroBtCnt();
						}
						else {
							if (m_model->GetCurMotInfo()) {
								m_model->Motion2Bt(limitdegflag, nextframe, &matVP, updateslot);
							}
							m_model->PlusPlusBtCnt();
						}

						InterlockedExchange(&m_start_state, 0L);
						LeaveCriticalSection(&m_CritSection);
					}
					else {
						InterlockedExchange(&m_start_state, 0L);
					}

					if (m_model) {
						m_model->SetUnderMotion2Bt(false);//!!!!!!!!
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


int CThreadingMotion2Bt::SetModel(CModel* srcmodel)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	m_model = srcmodel;
	return 0;
}

int CThreadingMotion2Bt::Motion2Bt(bool srclimitdegflag, double srcnextframe, ChaMatrix* srcpmVP, 
	int srcloopstartflag, int srcupdateslot)
{
	if (!m_model || !srcpmVP) {
		return 1;
	}
	//if (m_model && (m_model->GetInView() == false)) {
	//	return;
	//}

	m_model->SetUnderMotion2Bt(true);//!!!!!!!!


	//if (!m_framenovec.empty()) {
		EnterCriticalSection(&m_CritSection);

		limitdegflag = srclimitdegflag;
		nextframe = srcnextframe;
		matVP = *srcpmVP;
		loopstartflag = srcloopstartflag;
		updateslot = srcupdateslot;

		LeaveCriticalSection(&m_CritSection);
		InterlockedExchange(&m_start_state, 1L);
		SetEvent(m_hEvent);
	//}
	//else {
	//	InterlockedExchange(&m_start_state, 0L);
	//}

	return 0;
}



