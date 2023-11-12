#ifndef THREADINGUPDATETIMELINEH
#define THREADINGUPDATETIMELINEH


#include <ThreadingBase.h>
#include <ChaVecCalc.h>


class CThreadingUpdateTimeline : public CThreadingBase
{
public:
	CThreadingUpdateTimeline();
	virtual ~CThreadingUpdateTimeline();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	void UpdateTimeline(int (*srctimelinecursor)(int mbuttonflag, double newframe), int srcmbuttonflag, double srcnewframe);
	//bool IsFinished();

private:
	//static unsigned __stdcall ThreadFuncCaller(LPVOID lpThreadParam);
	virtual int ThreadFunc();


private:
	//CRITICAL_SECTION m_CritSection_UpdateMatrix;
	//HANDLE m_hEvent; //手動リセットイベント
	//HANDLE m_hExitEvent; //手動リセットイベント

	//HANDLE m_hthread;
	//LONG m_exit_state;
	//LONG m_start_state;

	int m_mbuttonflag;
	double m_newframe;
	int (*m_timelinecursor)(int srcmbuttonflag, double srcnewframe);

};


#endif