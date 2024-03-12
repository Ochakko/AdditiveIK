#ifndef THREADINGMOTION2BTH
#define THREADINGMOTION2BTH


#include <ThreadingBase.h>
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <vector>

class CModel;
class CBone;

class CThreadingMotion2Bt : public CThreadingBase
{
public:
	CThreadingMotion2Bt();
	virtual ~CThreadingMotion2Bt();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int SetModel(CModel* srcmodel);
	int Motion2Bt(bool srclimitdegflag, double srcnextframe, 
		ChaMatrix* srcpmView, ChaMatrix* srcpmProj, int srcloopstartflag);//, int srcupdateslot);

private:
	//static unsigned __stdcall ThreadFuncCaller(LPVOID lpThreadParam);
	virtual int ThreadFunc();


private:
	////CRITICAL_SECTION m_CritSection_UpdateMatrix;
	////HANDLE m_hEvent; //手動リセットイベント
	////HANDLE m_hExitEvent; //手動リセットイベント

	////HANDLE m_hthread;
	////LONG m_exit_state;
	////LONG m_start_state;
	CModel* m_model;

	int limitdegflag;
	double nextframe;
	ChaMatrix matView;
	ChaMatrix matProj;
	int loopstartflag;
	//int updateslot;
};


#endif