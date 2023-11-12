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

#define DBGH
#include <dbg.h>

#include <string>

#include <process.h>
#include "ThreadingBase.h"


using namespace std;


CThreadingBase::CThreadingBase()
{
	InitParams();

	//ClearBoneList();
	//m_pscene = 0;
	//m_model = 0;
	//m_animno = 0;
	//m_motid = 0;
	//m_animleng = 1.0;
}

int CThreadingBase::InitParams()
{
	m_exit_state = 0;
	m_start_state = 0;
	m_hthread = INVALID_HANDLE_VALUE;
	m_hEvent = NULL;
	m_hExitEvent = NULL;
	InitializeCriticalSection(&m_CritSection);

	return 0;
}
CThreadingBase::~CThreadingBase()
{
	DestroyObjs();
}

void CThreadingBase::DestroyObjs()
{
	InterlockedExchange(&m_exit_state, 1L);
	if (m_hEvent != NULL) {
		SetEvent(m_hEvent);
	}
	InterlockedExchange(&m_start_state, 1L);//計算しないと関数を終了できないこともある

	//Sleep(10);
	if (m_hExitEvent != NULL) {
		WaitForSingleObject(m_hExitEvent, INFINITE);
	}

	DeleteCriticalSection(&m_CritSection);
	//ClearBoneList();
	if (m_hEvent != NULL) {
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	if (m_hExitEvent != NULL) {
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	if (m_hthread && (m_hthread != INVALID_HANDLE_VALUE)) {
		CloseHandle(m_hthread);
		m_hthread = INVALID_HANDLE_VALUE;
	}
}
int CThreadingBase::CreateThread(DWORD affinitymask)
{
	if (m_hthread != INVALID_HANDLE_VALUE) {
		//already created error
		_ASSERT(0);
		return 1;
	}


	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hEvent == NULL) {
		_ASSERT(0);
		return 1;
	}

	unsigned int threadaddr1 = 0;
	m_hthread = (HANDLE)_beginthreadex(
		NULL, 0, &ThreadFuncCaller,
		(void*)this,
		0, &threadaddr1);


	//2023/08/27
	ULONG_PTR  processaffinity = 0;
	ULONG_PTR  systemaffinity = 0;
	GetProcessAffinityMask(GetModuleHandle(NULL), &processaffinity, &systemaffinity);
	if (processaffinity & affinitymask) {
		SetThreadAffinityMask(m_hthread, affinitymask);
	}


	////WiatForしない場合には先に閉じてもOK
	//if (m_hthread && (m_hthread != INVALID_HANDLE_VALUE)) {
	//	CloseHandle(m_hthread);
	//}

	return 0;

}

unsigned __stdcall CThreadingBase::ThreadFuncCaller(LPVOID lpThreadParam)
{
	if (!lpThreadParam) {
		_ASSERT(0);
		return 1;
	}

	CThreadingBase* curcontext = (CThreadingBase*)lpThreadParam;
	if (curcontext) {
		curcontext->ThreadFunc();
	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;
}
int CThreadingBase::ThreadFunc()
{

	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExitEvent == NULL) {
		_ASSERT(0);
		return 1;
	}


	CThreadingBase* curload = this;//for debug

	//while (InterlockedAdd(&m_exit_state, 0) != 1) {

	//	//if (g_HighRpmMode == true) {

	//		//###########################
	//		// 高回転モード　: High rpm
	//		//###########################

	//	if (InterlockedAdd(&m_start_state, 0) == 1) {
	//		if (InterlockedAdd(&m_exit_state, 0) != 1) {//終了していない場合
	//			EnterCriticalSection(&m_CritSection);
	//			if (m_model) {
	//				if ((m_bonenum >= 0) || (m_bonenum <= MAXLOADFBXANIMBONE)) {
	//					CBone* firstbone = m_bonelist[0];
	//					bool callingbythread = true;
	//					firstbone->GetFBXAnim(m_bonelist, m_nodelist, m_bonenum, m_animno, m_motid, m_animleng, callingbythread);

	//					//int bonecount;
	//					//for (bonecount = 0; bonecount < m_bonenum; bonecount++) {
	//					//	CBone* curbone = m_bonelist[bonecount];
	//					//	FbxUInt64 curnodeindex = m_nodelist[bonecount];
	//					//	if (curbone) {
	//					//		bool callingbythread = true;
	//					//		CModel* curmodel = GetModel();
	//					//		if (curmodel) {
	//					//			curbone->GetFBXAnim(GetScene(), m_animno, curnodeindex, m_motid, m_animleng, callingbythread);
	//					//		}
	//					//
	//					//		//Sleep(0);
	//					//	}
	//					//}
	//				}
	//			}
	//			InterlockedExchange(&m_start_state, 0L);
	//			LeaveCriticalSection(&m_CritSection);
	//		}
	//		else {
	//			InterlockedExchange(&m_start_state, 0L);
	//		}
	//	}
	//	else {
	//		//__nop();
	//		Sleep(0);
	//	}


	//	//}
	//	//else {

	//	//	//############################
	//	//	// eco モード
	//	//	//############################

	//	//	DWORD dwWaitResult = WaitForSingleObject(m_hEvent, INFINITE);
	//	//	ResetEvent(m_hEvent);
	//	//	switch (dwWaitResult)
	//	//	{
	//	//		// Event object was signaled
	//	//	case WAIT_OBJECT_0:
	//	//	{
	//	//		EnterCriticalSection(&m_CritSection);
	//	//		if (m_model) {
	//	//			if ((m_bonenum >= 0) || (m_bonenum <= MAXLOADFBXANIMBONE)) {
	//	//				int bonecount;
	//	//				for (bonecount = 0; bonecount < m_bonenum; bonecount++) {
	//	//					CBone* curbone = m_bonelist[bonecount];
	//	//					FbxNode* curnode = m_nodelist[bonecount];
	//	//					if (curbone && curnode) {
	//	//						bool callingbythread = true;
	//	//						m_model->GetFbxAnim(m_animno, curnode, m_motid, m_animleng, callingbythread);
	//	//					}
	//	//				}
	//	//			}
	//	//		}
	//	//		InterlockedExchange(&m_start_state, 0L);
	//	//		LeaveCriticalSection(&m_CritSection);

	//	//	}
	//	//	break;

	//	//	// An error occurred
	//	//	default:
	//	//		//printf("Wait error (%d)\n", GetLastError());
	//	//		//return 0;
	//	//		break;
	//	//	}
	//	//}
	//}

	if (m_hExitEvent != NULL) {
		SetEvent(m_hExitEvent);
	}

	return 0;
}


//int CThreadingBase::ClearBoneList()
//{
//	m_bonenum = 0;
//	ZeroMemory(m_bonelist, sizeof(CBone*) * MAXLOADFBXANIMBONE);
//	ZeroMemory(m_nodelist, sizeof(FbxNode*) * MAXLOADFBXANIMBONE);
//
//	return 0;
//}
//int CThreadingBase::SetBoneList(int srcindex, FbxNode* srcnode, CBone* srcbone)
//{
//	if ((srcindex < 0) || (srcindex >= MAXLOADFBXANIMBONE)) {
//		_ASSERT(0);
//		return -1;
//	}
//
//	if (srcindex != m_bonenum) {
//		_ASSERT(0);
//		return -1;
//	}
//
//	m_bonelist[srcindex] = srcbone;
//	m_nodelist[srcindex] = srcnode;
//
//	m_bonenum++;
//
//	return m_bonenum;
//}

bool CThreadingBase::IsFinished()
{
	if (InterlockedAdd(&m_start_state, 0) == 1) {
		return false;
	}
	else {
		return true;
	}
}

//void CThreadingBase::LoadFbxAnim(int srcanimno, int srcmotid, double srcanimleng)
//{
//
//	CThreadingBase* curload = this;//for debug
//
//
//	//####################################################################
//	//## g_limitdegflag == true　の場合にはローカルの計算だけ並列化
//	//####################################################################
//
//	if ((m_bonenum > 0) && (GetScene()) && (GetModel())) {
//		EnterCriticalSection(&m_CritSection);
//		m_animno = srcanimno;
//		m_motid = srcmotid;
//		m_animleng = srcanimleng;
//		LeaveCriticalSection(&m_CritSection);
//		InterlockedExchange(&m_start_state, 1L);
//		SetEvent(m_hEvent);
//	}
//	else {
//		InterlockedExchange(&m_start_state, 0L);
//	}
//
//
//}
//

