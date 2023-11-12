#ifndef THREADINGRETARGETH
#define THREADINGRETARGETH


#include <ThreadingBase.h>
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <vector>
#include <map>

class CModel;
class CBone;

class CThreadingRetarget : public CThreadingBase
{
public:
	CThreadingRetarget();
	virtual ~CThreadingRetarget();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearFrameList();
	int SetModel(CModel* srcmodel);

	int AddFramenoList(double srcframeno);
	void RetargetReqOne(int srclimitdegflag, CModel* srcmodel, CModel* srcbvhmodel, CBone* srcmodelbone,
		CBone* srcbvhtopbone, float srchrate, std::map<CBone*, CBone*>& srcsconvbonemap);


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

	//void RetargetReqOne(int limitdegflag, CModel* srcmodel, CModel* srcbvhmodel, CBone* modelbone,
	//	double srcframe, CBone* bvhtopbone, float hrate, std::map<CBone*, CBone*>& sconvbonemap);

	int limitdegflag;
	CModel* bvhmodel;
	CBone* modelbone;
	CBone* bvhtopbone;
	float hrate;
	std::map<CBone*, CBone*> sconvbonemap;


	std::vector<double> m_framenovec;
};


#endif