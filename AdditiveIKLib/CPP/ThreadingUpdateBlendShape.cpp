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

#define DBGH
#include <dbg.h>

#include <string>

//#include <fbxsdk.h>
//#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
//#include <fbxsdk/scene/animation/fbxanimevaluator.h>
//
//#include "btBulletDynamicsCommon.h"
//#include "LinearMath/btIDebugDraw.h"

#include <io.h>

//for __nop()
#include <intrin.h>

#include <ThreadingUpdateBlendShape.h>
#include <Model.h>
#include <GlobalVar.h>

using namespace std;



CThreadingUpdateBlendShape::CThreadingUpdateBlendShape()
{
	InitParams();
}

int CThreadingUpdateBlendShape::InitParams()
{
	CThreadingBase::InitParams();

	m_model = nullptr;
	m_motid = 0;
	m_frame = 1.0;

	return 0;
}

CThreadingUpdateBlendShape::~CThreadingUpdateBlendShape()
{
	DestroyObjs();
}

void CThreadingUpdateBlendShape::DestroyObjs()
{
	CThreadingBase::DestroyObjs();
}

int CThreadingUpdateBlendShape::ThreadFunc()
{
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}

	while (InterlockedAdd(&m_exit_state, 0) != 1) {

		////if ((g_HighRpmMode == true) && 
		////	((g_previewFlag != 0) || (g_underIKRot == true))) {//プレビュー中　または　IK回転中　だけ
		//if (m_model && (g_HighRpmMode == true) &&
		//	((g_previewFlag != 0) || (m_model->GetUnderIKRot()))) {//プレビュー中　または　IK回転中　だけ

		if (m_model && (g_HighRpmMode == true)) {

			//###########################
			// 高回転モード　: High rpm
			//###########################

			if (InterlockedAdd(&m_start_state, 0) == 1) {//計算開始命令をキャッチ
				if (InterlockedAdd(&m_exit_state, 0) != 1) {//スレッドが終了していない場合

					if (m_model && (m_motid > 0) && (m_frame >= 0.0)) {
						//EnterCriticalSection(&m_CritSection);//再入防止 呼び出し側で処理終了を待つので不要
						m_model->UpdateBlendShape(m_motid, m_frame);// , updateslot);
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
					if (m_model && (m_motid > 0) && (m_frame >= 0.0)) {
						EnterCriticalSection(&m_CritSection);
						if (m_model && (m_motid > 0) && (m_frame >= 0.0)) {
							//EnterCriticalSection(&m_CritSection);//再入防止 呼び出し側で処理終了を待つので不要
							m_model->UpdateBlendShape(m_motid, m_frame);// , updateslot);
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



void CThreadingUpdateBlendShape::UpdateBlendShape(CModel* srcmodel, int srcmotid, double srcframe)
{

	CThreadingUpdateBlendShape* curload = this;//for debug

	//実行終了を待つ
	while (InterlockedAdd(&m_start_state, 0) != 0) {
		timeBeginPeriod(1);
		SleepEx(0, TRUE);
		timeEndPeriod(1);
	}

	//####################################################################
	//## g_limitdegflag == true　の場合にはローカルの計算だけ並列化
	//####################################################################

	if ((srcmodel != nullptr) && (srcmotid > 0) && (srcframe >= 0.0)) {
		EnterCriticalSection(&m_CritSection);
		m_model = srcmodel;
		m_motid = srcmotid;
		m_frame = srcframe;
		LeaveCriticalSection(&m_CritSection);
		InterlockedExchange(&m_start_state, 1L);
		SetEvent(m_hEvent);
	}
	else {
		InterlockedExchange(&m_start_state, 0L);
	}
}



