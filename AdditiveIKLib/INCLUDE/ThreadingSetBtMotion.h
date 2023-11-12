#ifndef THREADINGSETBTMOTIONH
#define THREADINGSETBTMOTIONH


#include <ThreadingBase.h>
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <vector>


class CModel;
class CBone;

class CThreadingSetBtMotion : public CThreadingBase
{
public:
	CThreadingSetBtMotion();
	virtual ~CThreadingSetBtMotion();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int SetModel(CModel* srcmodel);
	int SetBtMotion(bool srclimitdegflag, double srcnextframe, ChaMatrix* srcpmVP, CModel* srcsmodel, double srcreccnt, int srcupdateslot);

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
	ChaMatrix matVP;
	CModel* smodel;
	double reccnt;
	int updateslot;

};


#endif