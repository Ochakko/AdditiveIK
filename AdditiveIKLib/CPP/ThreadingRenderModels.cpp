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
#include <ChaScene.h>

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

#include <ThreadingRenderModels.h>
#include <GlobalVar.h>

using namespace std;

extern int g_previewFlag;


CThreadingRenderModels::CThreadingRenderModels()
{
	InitParams();
}
int CThreadingRenderModels::InitParams()
{
	CThreadingBase::InitParams();

	m_chascene = nullptr;
	m_Engine = nullptr;
	m_rc = nullptr;
	//m_lightflag = 1;
	//m_diffusemult.SetParams(1.0f, 1.0f, 1.0f, 1.0f);
	//m_btflag = 0;

	return 0;
}
CThreadingRenderModels::~CThreadingRenderModels()
{
	DestroyObjs();
}

void CThreadingRenderModels::DestroyObjs()
{
	CThreadingBase::DestroyObjs();
	InitParams();
}

int CThreadingRenderModels::ThreadFunc()
{
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}

	while (InterlockedAdd(&m_exit_state, 0) != 1) {

		//if ((g_HighRpmMode == true) && 
		//	((g_previewFlag != 0) || (g_underIKRot == true))) {//プレビュー中　または　IK回転中　だけ
		if (g_HighRpmMode == true) {

			//###########################
			// 高回転モード　: High rpm
			//###########################

			if (InterlockedAdd(&m_start_state, 0) == 1) {//計算開始命令をキャッチ
				if (InterlockedAdd(&m_exit_state, 0) != 1) {//スレッドが終了していない場合

					if ((m_chascene != nullptr) && (m_Engine != nullptr) && (m_rc != nullptr)) {
						//EnterCriticalSection(&m_CritSection);//再入防止 呼び出し側で処理終了を待つので不要

						//auto& renderContext = g_graphicsEngine->GetRenderContext();
						m_Engine->Execute(m_rc, m_chascene);
						g_engine->EndFrame(m_chascene);
						m_chascene->ClearRenderObjs();//CopyCSDeform()よりも後で呼ぶ
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
					if ((m_chascene != nullptr) && (m_Engine != nullptr) && (m_rc != nullptr)) {
						EnterCriticalSection(&m_CritSection);

						//auto& renderContext = g_graphicsEngine->GetRenderContext();
						m_Engine->Execute(m_rc, m_chascene);
						g_engine->EndFrame(m_chascene);
						m_chascene->ClearRenderObjs();//CopyCSDeform()よりも後で呼ぶ

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


void CThreadingRenderModels::RenderModels(ChaScene* pchascene, 
	myRenderer::RenderingEngine* Engine, RenderContext* rc)
{

	if (!pchascene || !Engine || !rc) {
		InterlockedExchange(&m_start_state, 0L);
		return;
	}

	EnterCriticalSection(&m_CritSection);
	m_chascene = pchascene;
	m_Engine = Engine;
	m_rc = rc;
	//m_lightflag = lightflag;
	//m_diffusemult = diffusemult;
	//m_btflag = btflag;
	LeaveCriticalSection(&m_CritSection);
	InterlockedExchange(&m_start_state, 1L);
	SetEvent(m_hEvent);

}



