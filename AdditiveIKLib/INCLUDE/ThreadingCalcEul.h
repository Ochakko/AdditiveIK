#ifndef THREADINGCALCEULH
#define THREADINGCALCEULH


#include <ThreadingBase.h>
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <vector>

class CModel;
class CBone;

class CThreadingCalcEul : public CThreadingBase
{
public:
	CThreadingCalcEul();
	virtual ~CThreadingCalcEul();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearBoneList();
	int SetModel(CModel* srcmodel);
	int AddBoneList(CBone* srcbone);
	void CalcBoneEul(CModel* srcmodel, bool limitdegflag, int srcmotid);

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
	CModel* m_model;

	std::vector<CBone*> m_bonevec;

	int motid;
	bool m_limitdegflag;
};


#endif