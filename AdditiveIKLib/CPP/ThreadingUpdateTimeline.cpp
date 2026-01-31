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

#include <fbxsdk.h>
#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <fbxsdk/scene/animation/fbxanimevaluator.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

#include <io.h>

//for __nop()
#include <intrin.h>

#include <ThreadingUpdateTimeline.h>
#include <GlobalVar.h>

using namespace std;

extern int g_previewFlag;

CThreadingUpdateTimeline::CThreadingUpdateTimeline()
{
	InitParams();
}
int CThreadingUpdateTimeline::InitParams()
{
	CThreadingBase::InitParams();

	m_mbuttonflag = 1;
	m_newframe = 0.0;
	m_timelinecursor = 0;

	return 0;
}
CThreadingUpdateTimeline::~CThreadingUpdateTimeline()
{
	DestroyObjs();
}

void CThreadingUpdateTimeline::DestroyObjs()
{
	CThreadingBase::DestroyObjs();

	m_mbuttonflag = 1;
	m_newframe = 0.0;
	m_timelinecursor = 0;
}

int CThreadingUpdateTimeline::ThreadFunc()
{
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}

	while (InterlockedAdd(&m_exit_state, 0) != 1) {

		//if ((g_HighRpmMode == true) && 
		//	((g_previewFlag != 0) || (g_underIKRot == true))) {//プレビュー中　または　IK回転中　だけ
		if ((g_HighRpmMode == true) &&
			(g_previewFlag != 0)) {//プレビュー中　だけ


			//###########################
			// 高回転モード　: High rpm
			//###########################

			if (InterlockedAdd(&m_start_state, 0) == 1) {
				if (InterlockedAdd(&m_exit_state, 0) != 1) {//終了していない場合
					EnterCriticalSection(&m_CritSection);
					if (m_timelinecursor) {
						(m_timelinecursor)(m_mbuttonflag, m_newframe);
					}
					InterlockedExchange(&m_start_state, 0L);
					LeaveCriticalSection(&m_CritSection);
				}
				else {
					InterlockedExchange(&m_start_state, 0L);
				}
			}
			else {
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
			switch (dwWaitResult)
			{
				// Event object was signaled
			case WAIT_OBJECT_0:
			{
				EnterCriticalSection(&m_CritSection);
				if (m_timelinecursor) {
					(m_timelinecursor)(m_mbuttonflag, m_newframe);
				}
				InterlockedExchange(&m_start_state, 0L);
				LeaveCriticalSection(&m_CritSection);

			}
			break;

			// An error occurred
			default:
				//printf("Wait error (%d)\n", GetLastError());
				//return 0;
				break;
			}
		}
	}

	if (m_hExitEvent != NULL) {
		SetEvent(m_hExitEvent);
	}

	return 0;
}

void CThreadingUpdateTimeline::UpdateTimeline(int (*srctimelinecursor)(int mbuttonflag, double newframe), int srcmbuttonflag, double srcnewframe)
{

	//終了を待たずに呼ぶようなクリティカルな計算ではないことを前提に、実行中の場合は何もしないでリターンする
	if (InterlockedAdd(&m_start_state, 0) == 1) {
		return;
	}

	////実行終了を待つ
	//while (InterlockedAdd(&m_start_state, 0) != 0) {
	//	timeBeginPeriod(1);
	//	SleepEx(0, TRUE);
	//	timeEndPeriod(1);
	//}


	EnterCriticalSection(&m_CritSection);
	m_mbuttonflag = srcmbuttonflag;
	m_newframe = srcnewframe;
	m_timelinecursor = srctimelinecursor;
	LeaveCriticalSection(&m_CritSection);
	InterlockedExchange(&m_start_state, 1L);
	SetEvent(m_hEvent);
}



