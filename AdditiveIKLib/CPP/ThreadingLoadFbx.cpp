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

#include <ThreadingLoadFbx.h>


using namespace std;



CThreadingLoadFbx::CThreadingLoadFbx()
{
	InitParams();
}

int CThreadingLoadFbx::InitParams()
{
	CThreadingBase::InitParams();

	ClearBoneList();
	m_pscene = 0;
	m_model = 0;
	m_animno = 0;
	m_motid = 0;
	m_animleng = 1.0;

	return 0;
}

CThreadingLoadFbx::~CThreadingLoadFbx()
{
	DestroyObjs();
}

void CThreadingLoadFbx::DestroyObjs()
{
	CThreadingBase::DestroyObjs();
	ClearBoneList();
}

int CThreadingLoadFbx::ThreadFunc()
{

	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}


	CThreadingLoadFbx* curload = this;//for debug

	while (InterlockedAdd(&m_exit_state, 0) != 1) {

		//if (g_HighRpmMode == true) {

			//###########################
			// 高回転モード　: High rpm
			//###########################

		if (InterlockedAdd(&m_start_state, 0) == 1) {
			if (InterlockedAdd(&m_exit_state, 0) != 1) {//終了していない場合

				//EnterCriticalSection(&m_CritSection);//呼び出し側で処理終了を待つので不要
				if (m_model) {
					if ((m_bonenum > 0) || (m_bonenum <= MAXLOADFBXANIMBONE)) {
						//CBone* firstbone = m_bonelist[0];
						//bool callingbythread = true;
						//firstbone->GetFBXAnim(bvhflag, m_bonelist, m_nodelist, m_bonenum, m_animno, m_motid, m_animleng, callingbythread);
						
						bool callingbythread = true; 
						int boneno;
						for (boneno = 0; boneno < m_bonenum; boneno++) {
							CBone* curbone = m_bonelist[boneno];
							FbxNode* curnode = m_nodelist[boneno];
							if (curbone && curnode) {
								curbone->GetFBXAnim(curnode, m_animno, m_motid, m_animleng, callingbythread);
							}
							else {
								int dbgflag1 = 1;
							}
						}
					}
				}
				InterlockedExchange(&m_start_state, 0L);
				//LeaveCriticalSection(&m_CritSection);
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


		//}
		//else {

		//	//############################
		//	// eco モード
		//	//############################

		//	DWORD dwWaitResult = WaitForSingleObject(m_hEvent, INFINITE);
		//	ResetEvent(m_hEvent);
		//	switch (dwWaitResult)
		//	{
		//		// Event object was signaled
		//	case WAIT_OBJECT_0:
		//	{
		//		EnterCriticalSection(&m_CritSection);
		//		if (m_model) {
		//			if ((m_bonenum > 0) || (m_bonenum <= MAXLOADFBXANIMBONE)) {
		//				int bonecount;
		//				for (bonecount = 0; bonecount < m_bonenum; bonecount++) {
		//					CBone* curbone = m_bonelist[bonecount];
		//					FbxNode* curnode = m_nodelist[bonecount];
		//					if (curbone && curnode) {
		//						bool callingbythread = true;
		//						m_model->GetFbxAnim(m_animno, curnode, m_motid, m_animleng, callingbythread);
		//					}
		//				}
		//			}
		//		}
		//		InterlockedExchange(&m_start_state, 0L);
		//		LeaveCriticalSection(&m_CritSection);

		//	}
		//	break;

		//	// An error occurred
		//	default:
		//		//printf("Wait error (%d)\n", GetLastError());
		//		//return 0;
		//		break;
		//	}
		//}
	}

	if (m_hExitEvent != NULL) {
		SetEvent(m_hExitEvent);
	}

	return 0;
}


int CThreadingLoadFbx::ClearBoneList()
{
	m_bonenum = 0;
	ZeroMemory(m_bonelist, sizeof(CBone*) * MAXLOADFBXANIMBONE);
	ZeroMemory(m_nodelist, sizeof(FbxNode*) * MAXLOADFBXANIMBONE);

	return 0;
}
int CThreadingLoadFbx::SetBoneList(int srcindex, FbxNode* srcnode, CBone* srcbone)
{
	if ((srcindex < 0) || (srcindex >= MAXLOADFBXANIMBONE)) {
		_ASSERT(0);
		return -1;
	}

	if (srcindex != m_bonenum) {
		_ASSERT(0);
		return -1;
	}

	m_bonelist[srcindex] = srcbone;
	m_nodelist[srcindex] = srcnode;

	m_bonenum++;

	return m_bonenum;
}

void CThreadingLoadFbx::LoadFbxAnim(int srcanimno, int srcmotid, double srcanimleng)
{

	CThreadingLoadFbx* curload = this;//for debug

	//実行終了を待つ
	while (InterlockedAdd(&m_start_state, 0) != 0) {
		timeBeginPeriod(1);
		SleepEx(0, TRUE);
		timeEndPeriod(1);
	}

	//####################################################################
	//## g_limitdegflag == true　の場合にはローカルの計算だけ並列化
	//####################################################################

	if ((m_bonenum > 0) && (GetScene()) && (GetModel())) {
		EnterCriticalSection(&m_CritSection);
		m_animno = srcanimno;
		m_motid = srcmotid;
		m_animleng = srcanimleng;
		LeaveCriticalSection(&m_CritSection);
		InterlockedExchange(&m_start_state, 1L);
		SetEvent(m_hEvent);
	}
	else {
		InterlockedExchange(&m_start_state, 0L);
	}


}



